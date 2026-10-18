// FileToByte converter test suite (self-contained; no external framework).
//
// Covers the Convert() contract from source/FTB.h:
//   * round-trip for a size/pattern corpus in all three encodings
//     (generated source -> parsed bytes -> upstream stb_decompress -> compare),
//   * cross-encoding compressed-stream equality and declared array bounds,
//   * ConvertResult::message() / ok() mapping for every ConvertError,
//   * array-name validation ([A-Za-z][A-Za-z0-9_]*), including validation
//   order,
//   * path error mapping (empty/nonexistent/directory) and UTF-8/non-ASCII
//   names,
//   * the kMaxInputSize cap (sparse file, rejected without reading),
//   * thread safety (8 threads x 20 concurrent Convert calls vs golden output).
//
// Run directly (cd tests && FileToByteTests) or through CTest; exit code 0
// means all checks passed.
#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <exception>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#include "FTB.h"
#include "encoding_parsers.h"
#include "stb_decompress.h"

namespace fs = std::filesystem;

namespace {

int g_checks = 0;
int g_failures = 0;

void Check(bool condition, const std::string &what) {
  ++g_checks;
  if (!condition) {
    ++g_failures;
    std::cout << "  [FAIL] " << what << '\n';
  }
}

// Creates a unique temporary directory and removes it on destruction.
class TempDir {
 public:
  TempDir() {
    static std::atomic<std::uint64_t> counter{0};
    const auto now = std::chrono::steady_clock::now().time_since_epoch();
    const auto stamp = static_cast<std::uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(now).count());
    const auto unique = std::to_string(stamp) + "_" +
                        std::to_string(counter.fetch_add(1)) + "_" +
                        std::to_string(std::hash<std::thread::id>{}(
                                           std::this_thread::get_id()) %
                                       1000000u);
    path_ = fs::temp_directory_path() / ("filetobyte_tests_" + unique);
    std::error_code error;
    fs::create_directories(path_, error);
    ok_ = !error && fs::is_directory(path_);
  }

  ~TempDir() {
    std::error_code error;
    fs::remove_all(path_, error);
  }

  TempDir(const TempDir &) = delete;
  TempDir &operator=(const TempDir &) = delete;

  [[nodiscard]] const fs::path &path() const { return path_; }
  [[nodiscard]] bool ok() const { return ok_; }

 private:
  fs::path path_;
  bool ok_ = false;
};

// Interprets UTF-8 bytes as a native path (same conversion Convert() uses).
fs::path PathFromUtf8(std::string_view utf8) {
  const auto *begin = reinterpret_cast<const char8_t *>(utf8.data());
  return {std::u8string(begin, begin + utf8.size())};
}

bool WriteFile(const fs::path &path, const std::vector<unsigned char> &bytes) {
  std::ofstream file(path, std::ios::binary | std::ios::trunc);
  if (!file) return false;
  if (!bytes.empty()) {
    file.write(reinterpret_cast<const char *>(bytes.data()),
               static_cast<std::streamsize>(bytes.size()));
  }
  return file.good();
}

std::uint64_t SplitMix64(std::uint64_t &state) {
  std::uint64_t z = (state += 0x9E3779B97F4A7C15ULL);
  z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
  z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
  return z ^ (z >> 31);
}

std::vector<unsigned char> RandomBytes(std::size_t size, std::uint64_t seed) {
  std::vector<unsigned char> bytes(size);
  std::uint64_t state = seed;
  for (std::size_t i = 0; i < size; ++i)
    bytes[i] = static_cast<unsigned char>(SplitMix64(state) >> 56);
  return bytes;
}

std::vector<unsigned char> LoremBytes(std::size_t size) {
  static const char *kText =
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do "
      "eiusmod tempor incididunt ut labore et dolore magna aliqua. ";
  const std::size_t text_length = std::strlen(kText);
  std::vector<unsigned char> bytes(size);
  for (std::size_t i = 0; i < size; ++i)
    bytes[i] = static_cast<unsigned char>(kText[i % text_length]);
  return bytes;
}

// Recovers the compressed byte stream from generated source text.
bool ParseGenerated(const std::string &source, const std::string &symbol,
                    Encoding encoding, ftbtest::Encoded &encoded) {
  switch (encoding) {
    case Encoding::U8:
      return ftbtest::ParseU8(source, symbol, encoded);
    case Encoding::U32:
      return ftbtest::ParseU32(source, symbol, encoded);
    case Encoding::Base85:
      return ftbtest::ParseBase85(source, symbol, encoded);
  }
  encoded.error = "unknown encoding";
  return false;
}

// Decompresses `compressed` and compares it with `expected`.
bool DecompressMatches(const std::vector<unsigned char> &compressed,
                       const std::vector<unsigned char> &expected,
                       std::string &error) {
  if (compressed.size() < 16) {
    error = "compressed stream shorter than the stb header";
    return false;
  }
  const unsigned int length = ftbtest::stb_decompress_length(compressed.data());
  if (length != expected.size()) {
    error = "embedded length " + std::to_string(length) + " != expected " +
            std::to_string(expected.size());
    return false;
  }
  std::vector<unsigned char> decoded(expected.size() + 1);
  const unsigned int got =
      ftbtest::stb_decompress(decoded.data(), compressed.data(),
                              static_cast<unsigned int>(compressed.size()));
  if (expected.empty()) return got == 0;
  if (got != expected.size()) {
    error = "decompressed " + std::to_string(got) + " bytes, expected " +
            std::to_string(expected.size());
    return false;
  }
  if (std::memcmp(decoded.data(), expected.data(), expected.size()) != 0) {
    error = "decompressed bytes differ from the input file";
    return false;
  }
  return true;
}

// ---------------------------------------------------------------------------
// Tests
// ---------------------------------------------------------------------------

void TestMessageStrings() {
  std::cout << "[test] ConvertResult::message() mapping\n";
  struct MessageCase {
    ConvertError error;
    const char *message;
  };
  const MessageCase cases[] = {
      {.error = ConvertError::None, .message = ""},
      {.error = ConvertError::FileOpen, .message = "File failed to open"},
      {.error = ConvertError::FileSize,
       .message = "Failed to determine file size"},
      {.error = ConvertError::FileRead, .message = "Failed to read file"},
      {.error = ConvertError::TooLarge,
       .message = "File is too large (maximum is 16 MiB)"},
      {.error = ConvertError::InvalidArrayName,
       .message = "Array name must be a C identifier: [A-Za-z][A-Za-z0-9_]*"},
      {.error = ConvertError::OutOfMemory,
       .message = "Not enough memory to convert this file"},
      {.error = ConvertError::Compress, .message = "Compression failed"},
  };
  for (const MessageCase &c : cases) {
    ConvertResult result;
    result.error = c.error;
    Check(result.message() != nullptr, "message() is never nullptr");
    Check(std::strcmp(result.message(), c.message) == 0,
          std::string("message() wording for error ") +
              std::to_string(static_cast<int>(c.error)));
    Check(result.ok() == (c.error == ConvertError::None),
          std::string("ok() agrees with error ") +
              std::to_string(static_cast<int>(c.error)));
    Check(result.source.empty(), "default result has empty source");
  }
  const ConvertResult ok_default;
  Check(ok_default.ok(), "default-constructed result is ok");
  Check(std::strcmp(ok_default.message(), "") == 0,
        "default-constructed result has empty message");
}

void TestArrayNames(const fs::path &payload_path) {
  std::cout << "[test] array-name validation\n";
  const char *invalid_names[] = {
      "",        "1abc", "9",  "a b", "abc ", "abc\t",       "my-name", "a.b",
      "foo;bar", "a$",   "_x", "-x",  "a[b]", "caf\xc3\xa9", "a\nb"};
  for (const char *name : invalid_names) {
    const ConvertResult result = Convert(payload_path.string(), name);
    Check(!result.ok(), std::string("invalid name rejected: \"") + name + "\"");
    Check(
        result.error == ConvertError::InvalidArrayName,
        std::string("invalid name maps to InvalidArrayName: \"") + name + "\"");
    Check(result.source.empty(),
          std::string("invalid name produces no source: \"") + name + "\"");
    Check(std::strcmp(result.message(),
                      "Array name must be a C identifier: "
                      "[A-Za-z][A-Za-z0-9_]*") == 0,
          std::string("invalid name message wording: \"") + name + "\"");
  }

  // Validation order: the array name is checked before the file is opened.
  {
    const ConvertResult result =
        Convert("X:\\definitely_missing_file.bin", "1bad");
    Check(result.error == ConvertError::InvalidArrayName,
          "array name is validated before the path is opened");
  }

  const char *valid_names[] = {
      "a",      "A",         "img",
      "img_1",  "CamelCase", "int",
      "static", "x9_",       "a_very_long_symbol_name_123"};
  for (const char *name : valid_names) {
    const ConvertResult result = Convert(payload_path.string(), name);
    Check(result.ok(), std::string("valid name accepted: \"") + name + "\"");
    if (result.ok()) {
      Check(result.source.find(std::string(name) + "_compressed_data") !=
                std::string::npos,
            std::string("generated source declares symbol: \"") + name + "\"");
    }
  }
}

void TestPathErrors(const fs::path &temp_dir) {
  std::cout << "[test] path error mapping\n";
  struct PathCase {
    std::string path;
    ConvertError expected;
  };
  const PathCase cases[] = {
      {.path = "", .expected = ConvertError::FileOpen},
      {.path = (temp_dir / "does_not_exist_12345.bin").string(),
       .expected = ConvertError::FileOpen},
      {.path = temp_dir.string(),  // directory
       .expected = ConvertError::FileOpen},
  };
  for (const PathCase &c : cases) {
    const ConvertResult result = Convert(c.path, "img");
    Check(!result.ok(), "path rejected: \"" + c.path + "\"");
    Check(result.error == c.expected, "path error mapping: \"" + c.path + "\"");
    Check(result.source.empty(),
          "path failure produces no source: \"" + c.path + "\"");
  }
  // Bytes that are not valid UTF-8 cannot name an existing file either.
  const ConvertResult bad_utf8 = Convert("\xff\xfe\xfd_missing.bin", "img");
  Check(bad_utf8.error == ConvertError::FileOpen,
        "invalid UTF-8 path maps to FileOpen");
}

void TestRoundTripCorpus(const fs::path &temp_dir) {
  std::cout << "[test] round-trip corpus (all encodings)\n";
  struct Case {
    std::string name;
    std::vector<unsigned char> bytes;
  };
  std::vector<Case> cases;
  const std::size_t sizes[] = {0,   1,   2,     3,     4,     5,     7,
                               8,   11,  12,    13,    31,    32,    33,
                               127, 128, 65535, 65536, 65537, 262144};
  for (const std::size_t size : sizes) {
    cases.emplace_back("size_" + std::to_string(size),
                       RandomBytes(size, 0xABCULL + size));
  }
  cases.emplace_back("random_1MiB", RandomBytes(1024 * 1024, 0xC0FFEEULL));
  cases.emplace_back("zeros_1MiB",
                     std::vector<unsigned char>(1024 * 1024, 0x00));
  cases.emplace_back("text_1MiB", LoremBytes(1024 * 1024));
  {
    std::vector<unsigned char> all_bytes(256);
    for (int i = 0; i < 256; ++i)
      all_bytes[static_cast<std::size_t>(i)] = static_cast<unsigned char>(i);
    cases.emplace_back("all_bytes_256", std::move(all_bytes));
  }

  const Encoding encodings[] = {Encoding::U8, Encoding::U32, Encoding::Base85};
  const char *encoding_names[] = {"U8", "U32", "Base85"};
  const std::string symbol = "img";

  for (const Case &c : cases) {
    const fs::path path = temp_dir / (c.name + ".bin");
    if (!WriteFile(path, c.bytes)) {
      Check(false, "write input file " + c.name);
      continue;
    }

    std::vector<unsigned char> streams[3];
    bool parsed_ok[3] = {false, false, false};
    std::size_t declared_array_len[3] = {0, 0, 0};
    for (int e = 0; e < 3; ++e) {
      const ConvertResult result = Convert(path.string(), symbol, encodings[e]);
      Check(result.ok(), c.name + "/" + encoding_names[e] + ": Convert ok");
      if (!result.ok()) continue;
      Check(result.source.starts_with("#pragma once"),
            c.name + "/" + encoding_names[e] + ": starts with #pragma once");

      ftbtest::Encoded encoded;
      parsed_ok[e] =
          ParseGenerated(result.source, symbol, encodings[e], encoded);
      Check(parsed_ok[e], c.name + "/" + encoding_names[e] +
                              ": parse generated source" +
                              (parsed_ok[e] ? "" : " (" + encoded.error + ")"));
      if (!parsed_ok[e]) continue;
      streams[e] = encoded.bytes;
      declared_array_len[e] = encoded.declared_array_len;

      std::string decompress_error;
      const bool matches =
          DecompressMatches(encoded.bytes, c.bytes, decompress_error);
      Check(matches, c.name + "/" + encoding_names[e] + ": round-trip" +
                         (matches ? "" : " (" + decompress_error + ")"));
    }

    // Cross-encoding equality: the three encodings must carry the same stream
    // (U32/Base85 pad the last 4/5-byte group, so compare the U8 prefix).
    if (parsed_ok[0] && !streams[0].empty()) {
      const std::size_t u8_size = streams[0].size();
      if (parsed_ok[1]) {
        Check(declared_array_len[1] == (u8_size + 3) / 4,
              c.name + ": U32 word count == ceil(n/4)");
        bool equal = streams[1].size() >= u8_size;
        for (std::size_t i = 0; equal && i < u8_size; ++i)
          equal = streams[1][i] == streams[0][i];
        Check(equal, c.name + ": U32 stream equals U8 stream");
      }
      if (parsed_ok[2]) {
        Check(declared_array_len[2] == 5 * ((u8_size + 3) / 4) + 1,
              c.name + ": Base85 declared bound == 5*ceil(n/4)+1");
        bool equal = streams[2].size() >= u8_size;
        for (std::size_t i = 0; equal && i < u8_size; ++i)
          equal = streams[2][i] == streams[0][i];
        Check(equal, c.name + ": Base85 stream equals U8 stream");
      }
    }
  }
}

void TestSizeCap(const fs::path &temp_dir) {
  std::cout << "[test] kMaxInputSize cap\n";
  const fs::path oversized = temp_dir / "oversized_sparse.bin";
  {
    std::error_code error;
    const bool created = WriteFile(oversized, {});
    fs::resize_file(oversized, kMaxInputSize + 1, error);
    Check(created && !error, "create sparse file of kMaxInputSize + 1 bytes");
  }
  {
    const ConvertResult result = Convert(oversized.string(), "img");
    Check(!result.ok(), "file above kMaxInputSize is rejected");
    Check(result.error == ConvertError::TooLarge,
          "file above kMaxInputSize maps to TooLarge");
    Check(result.source.empty(), "TooLarge produces no source");
    Check(std::strcmp(result.message(),
                      "File is too large (maximum is 16 MiB)") == 0,
          "TooLarge message wording");
  }

  // A file of exactly kMaxInputSize is allowed and round-trips.
  const fs::path at_cap = temp_dir / "at_cap_sparse.bin";
  {
    std::error_code error;
    const bool created = WriteFile(at_cap, {});
    fs::resize_file(at_cap, kMaxInputSize, error);
    Check(created && !error,
          "create sparse file of exactly kMaxInputSize bytes");
  }
  {
    const ConvertResult result = Convert(at_cap.string(), "img", Encoding::U8);
    Check(result.ok(), "file of exactly kMaxInputSize is accepted");
    if (result.ok()) {
      ftbtest::Encoded encoded;
      const bool parsed = ftbtest::ParseU8(result.source, "img", encoded);
      Check(parsed, "parse at-cap generated source");
      if (parsed) {
        std::vector<unsigned char> expected(kMaxInputSize, 0x00);
        std::string error;
        Check(DecompressMatches(encoded.bytes, expected, error),
              "at-cap round-trip (" + error + ")");
      }
    }
  }
  std::error_code error;
  fs::remove(oversized, error);
  fs::remove(at_cap, error);
}

void TestNonAsciiPaths(const fs::path &temp_dir) {
  std::cout << "[test] UTF-8 / non-ASCII file names\n";
  const std::vector<unsigned char> payload = RandomBytes(4096, 0x5EEDULL);
  const std::string names[] = {
      // テスト_ファイル.bin
      "\xe3\x83\x86\xe3\x82\xb9\xe3\x83\x88_\xe3\x83\x95\xe3\x82\xa1"
      "\xe3\x82\xa4\xe3\x83\xab.bin",
      // café.bin
      "caf\xc3\xa9.bin",
  };
  for (const std::string &name : names) {
    // Build the path as UTF-8 bytes directly: converting a non-ASCII fs::path
    // back to a narrow string is lossy on Windows (ACP conversion).
    const std::string utf8_path = temp_dir.string() + "/" + name;
    const fs::path native_path = PathFromUtf8(utf8_path);
    if (!WriteFile(native_path, payload)) {
      Check(false, "write non-ASCII input file: " + name);
      continue;
    }
    const ConvertResult result = Convert(utf8_path, "img", Encoding::U8);
    Check(result.ok(), "non-ASCII path opens: " + name);
    if (!result.ok()) continue;
    ftbtest::Encoded encoded;
    Check(ftbtest::ParseU8(result.source, "img", encoded),
          "parse non-ASCII-path output: " + name);
    std::string error;
    Check(DecompressMatches(encoded.bytes, payload, error),
          "non-ASCII path round-trip: " + name + " (" + error + ")");
    std::error_code remove_error;
    fs::remove(native_path, remove_error);
  }
}

void TestThreadSafety(const fs::path &temp_dir) {
  std::cout << "[test] thread safety (8 threads x 20 Convert calls)\n";
  struct Input {
    fs::path path;
    std::vector<unsigned char> bytes;
  };
  std::vector<Input> inputs;
  inputs.emplace_back(temp_dir / "thread_64k.bin",
                      RandomBytes(64 * 1024, 0x11ULL));
  inputs.emplace_back(temp_dir / "thread_256k.bin",
                      RandomBytes(256 * 1024, 0x22ULL));
  inputs.emplace_back(temp_dir / "thread_text.bin", LoremBytes(128 * 1024));
  inputs.emplace_back(temp_dir / "thread_zeros.bin",
                      std::vector<unsigned char>(1024 * 1024, 0));
  for (const Input &input : inputs) {
    if (!WriteFile(input.path, input.bytes)) {
      Check(false, "write thread-test input " + input.path.string());
      return;
    }
  }
  const Encoding encodings[] = {Encoding::U8, Encoding::U32, Encoding::Base85};
  // Sequential golden results, one per (input, encoding).
  struct Golden {
    bool ok = false;
    std::string source;
    ConvertError error = ConvertError::None;
  };
  std::vector<Golden> golden;
  golden.reserve(inputs.size() * 3);
  for (const Input &input : inputs) {
    for (const Encoding encoding : encodings) {
      const ConvertResult result =
          Convert(input.path.string(), "img", encoding);
      golden.emplace_back(result.ok(), result.source, result.error);
    }
  }

  constexpr int kThreads = 8;
  constexpr int kCallsPerThread = 20;
  std::atomic<int> mismatches{0};
  std::atomic<int> exceptions{0};
  std::atomic<int> calls{0};

  auto worker = [&](int thread_index) {
    for (int call = 0; call < kCallsPerThread; ++call) {
      const std::size_t input_index =
          static_cast<std::size_t>(thread_index + call) % inputs.size();
      const std::size_t encoding_index =
          static_cast<std::size_t>(thread_index * 3 + call) % 3;
      const Golden &expected = golden[input_index * 3 + encoding_index];
      try {
        const ConvertResult result = Convert(inputs[input_index].path.string(),
                                             "img", encodings[encoding_index]);
        ++calls;
        if (result.ok() != expected.ok || result.error != expected.error ||
            result.source != expected.source) {
          ++mismatches;
        }
      } catch (...) {
        ++exceptions;
      }
    }
  };

  std::vector<std::thread> threads;
  threads.reserve(kThreads);
  for (int i = 0; i < kThreads; ++i) threads.emplace_back(worker, i);
  for (std::thread &thread : threads) thread.join();

  Check(calls.load() == kThreads * kCallsPerThread,
        "all concurrent Convert calls completed");
  Check(exceptions.load() == 0, "no exceptions thrown by concurrent Convert");
  Check(mismatches.load() == 0,
        "concurrent Convert output matches sequential golden output");
}

}  // namespace

int main() {
  // Flush progress immediately so CI logs survive a crash in a later test.
  std::cout << std::unitbuf;
  std::cout << "FileToByteTests: converter contract tests\n";
  const TempDir temp_dir;
  if (!temp_dir.ok()) {
    std::cerr << "FileToByteTests: cannot create a temporary directory\n";
    return 2;
  }
  std::cout << "  temp dir: " << temp_dir.path().string() << '\n';

  const fs::path payload_path = temp_dir.path() / "payload.bin";
  if (!WriteFile(payload_path, RandomBytes(100, 0x1234ULL))) {
    std::cerr << "FileToByteTests: cannot write payload\n";
    return 2;
  }

  TestMessageStrings();
  TestArrayNames(payload_path);
  TestPathErrors(temp_dir.path());
  TestRoundTripCorpus(temp_dir.path());
  TestSizeCap(temp_dir.path());
  TestNonAsciiPaths(temp_dir.path());
  TestThreadSafety(temp_dir.path());

  std::cout << "FileToByteTests: " << g_checks << " checks, " << g_failures
            << " failures\n";
  return g_failures == 0 ? 0 : 1;
}
