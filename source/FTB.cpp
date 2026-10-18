/*
 * Modelled on Dear ImGui's binary_to_compressed_c.cpp
 * (deps/VulkanWindow/deps/ImGui/misc/fonts/binary_to_compressed_c.cpp):
 * same stb_compress + U8 / U32 / Base85 emitters, adapted to read via
 * std::ifstream and return the generated source as a std::string for the GUI.
 */
#include "FTB.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <ios>
#include <new>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

using stb_uint = unsigned int;
using stb_uchar = unsigned char;
stb_uint stb_compress(stb_uchar *out, stb_uchar *in, stb_uint len);

namespace {

// --- Validation and error helpers. ---

constexpr bool IsAsciiAlpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

constexpr bool IsAsciiDigit(char c) { return c >= '0' && c <= '9'; }

// Array names are spliced into C identifiers by all emitters, so they must be
// exactly [A-Za-z][A-Za-z0-9_]*. Using one helper keeps the emitters honest.
bool IsValidArrayName(std::string_view name) {
  if (name.empty() || !IsAsciiAlpha(name.front())) return false;
  for (const char c : name) {
    if (!IsAsciiAlpha(c) && !IsAsciiDigit(c) && c != '_') return false;
  }
  return true;
}

ConvertResult MakeError(ConvertError error) {
  ConvertResult result;
  result.error = error;
  return result;
}

// Interprets `utf8` as UTF-8 and builds a native path. std::filesystem converts
// to the wide native string on Windows, so opening the returned path makes the
// CRT use the wide API and non-ASCII file names work.
std::filesystem::path PathFromUtf8(std::string_view utf8) {
  if (utf8.empty()) return {};
  const auto *begin = reinterpret_cast<const char8_t *>(utf8.data());
  return {std::u8string(begin, begin + utf8.size())};
}

// --- Per-encoding emitters. Each appends one declaration for `symbol`,
// covering `compressed_sz` bytes of `compressed` (which is zero-padded out to
// the next 4-byte boundary so the 32-bit reads below stay in bounds). ---

// Bytes as 0x-prefixed unsigned char[] — endianness-independent. Like
// binary_to_compressed_c.cpp's SourceEncoding_U8, but emitted as two-digit hex
// (0x00, 0x0f, 0xff, ...) instead of the reference's decimal. Padded to a fixed
// width and wrapped at a fixed count so the columns line up into a grid.
void emitU8(std::ostringstream &out, std::string_view symbol,
            const std::vector<char> &compressed, std::size_t compressed_sz) {
  out << "static const unsigned int " << symbol
      << "_compressed_size = " << compressed_sz << ";\n";
  out << "static const unsigned char " << symbol << "_compressed_data["
      << compressed_sz << "] = {" << std::hex << std::setfill('0');

  constexpr std::size_t kPerLine = 12;
  for (std::size_t i = 0; i < compressed_sz; ++i) {
    if (i % kPerLine == 0) out << "\n\t";
    out << "0x" << std::setw(2)
        << static_cast<unsigned int>(static_cast<unsigned char>(compressed[i]))
        << ",";
    if (i % kPerLine != kPerLine - 1 && i != compressed_sz - 1) out << ' ';
  }
  out << std::dec << std::setfill(' ') << "\n};\n";
}

// 32-bit words as 0x........ hex — FileToByte's previous output. Endianness
// dependent. Mirrors SourceEncoding_U32.
void emitU32(std::ostringstream &out, std::string_view symbol,
             const std::vector<char> &compressed, std::size_t compressed_sz) {
  out << "static const unsigned int " << symbol
      << "_compressed_size = " << compressed_sz << ";\n";
  out << "static const unsigned int " << symbol << "_compressed_data["
      << ((compressed_sz + 3) / 4) * 4 << "/4] = {";

  std::size_t column = 0;
  for (std::size_t i = 0; i < compressed_sz; i += 4) {
    unsigned int d;
    std::memcpy(&d, compressed.data() + i, sizeof(d));
    if ((column++ % 6) == 0) out << "\n\t";
    out << "0x" << std::hex << std::setw(8) << std::setfill('0') << d << ", "
        << std::dec;
  }
  out << "\n};\n";
}

// One base85 digit (matches binary_to_compressed_c.cpp's Encode85Byte).
char encode85Byte(unsigned int x) {
  x = (x % 85) + 35;
  return static_cast<char>((x >= '\\') ? x + 1 : x);
}

// Base85 string — ~5 source bytes per 4 data bytes. Mirrors
// SourceEncoding_Base85: no size symbol (loaded via the base85 entry point),
// and adjacent '?' are escaped to dodge trigraph interpretation.
void emitBase85(std::ostringstream &out, std::string_view symbol,
                const std::vector<char> &compressed,
                std::size_t compressed_sz) {
  out << "static const char " << symbol << "_compressed_data_base85["
      << ((compressed_sz + 3) / 4) * 5 + 1 << "] =\n\t\"";

  char prev_c = 0;
  for (std::size_t src_i = 0; src_i < compressed_sz; src_i += 4) {
    unsigned int d;
    std::memcpy(&d, compressed.data() + src_i, sizeof(d));
    for (unsigned int n5 = 0; n5 < 5; ++n5, d /= 85) {
      char c = encode85Byte(d);
      if (c == '?' && prev_c == '?') out << '\\';
      out << c;
      prev_c = c;
    }
    if ((src_i % 112) == 112 - 4) out << "\"\n\t\"";
  }
  out << "\";\n";
}

}  // namespace

const char *ConvertResult::message() const noexcept {
  switch (error) {
    case ConvertError::None:
      return "";
    case ConvertError::FileOpen:
      return "File failed to open";
    case ConvertError::FileSize:
      return "Failed to determine file size";
    case ConvertError::FileRead:
      return "Failed to read file";
    case ConvertError::TooLarge:
      return "File is too large (maximum is 16 MiB)";
    case ConvertError::InvalidArrayName:
      return "Array name must be a C identifier: [A-Za-z][A-Za-z0-9_]*";
    case ConvertError::OutOfMemory:
      return "Not enough memory to convert this file";
    case ConvertError::Compress:
      return "Compression failed";
  }
  return "";
}

ConvertResult Convert(std::string_view file_path, std::string_view array_name,
                      Encoding encoding) {
  // Validate the array name before touching the filesystem: it is the cheapest
  // check and the caller gets a deterministic error regardless of the path.
  if (!IsValidArrayName(array_name)) {
    return MakeError(ConvertError::InvalidArrayName);
  }

  std::filesystem::path fs_path;
  try {
    fs_path = PathFromUtf8(file_path);
  } catch (...) {
    // Unrepresentable/invalid UTF-8 path bytes.
    return MakeError(ConvertError::FileOpen);
  }

  std::ifstream file(fs_path, std::ios::in | std::ios::binary);
  if (!file.is_open()) return MakeError(ConvertError::FileOpen);

  // POSIX lets a directory be opened as a stream; reject it explicitly so the
  // caller gets FileOpen instead of a confusing read failure.
  std::error_code is_directory_error;
  if (std::filesystem::is_directory(fs_path, is_directory_error)) {
    return MakeError(ConvertError::FileOpen);
  }

  file.seekg(0, std::ios::end);
  const std::streamsize data_sz = file.tellg();
  if (data_sz < 0) return MakeError(ConvertError::FileSize);
  if (std::cmp_greater(data_sz, kMaxInputSize)) {
    return MakeError(ConvertError::TooLarge);
  }
  file.seekg(0, std::ios::beg);
  if (!file) return MakeError(ConvertError::FileRead);

  try {
    // Worst-case compressed size, then zero-pad the tail so the 32-bit reads in
    // the U32/Base85 emitters never read past the compressed data. The size cap
    // above keeps every one of these buffers bounded.
    const auto size = static_cast<std::size_t>(data_sz);
    std::vector<char> data(size + 4);
    if (!file.read(data.data(), data_sz)) {
      return MakeError(ConvertError::FileRead);
    }
    std::memset(data.data() + size, 0, 4);

    const std::size_t maxlen = size + 512 + (size >> 2) + sizeof(int);
    std::vector<char> compressed(maxlen);
    const stb_uint compressed_u32 =
        stb_compress(reinterpret_cast<stb_uchar *>(compressed.data()),
                     reinterpret_cast<stb_uchar *>(data.data()),
                     static_cast<stb_uint>(size));
    if (compressed_u32 == 0) return MakeError(ConvertError::Compress);
    const auto compressed_sz = static_cast<std::size_t>(compressed_u32);
    // The emitters read 4 bytes at a time and rely on zero padding, so keep at
    // least 4 bytes of slack after the stream (and never trust the compressor).
    if (compressed_sz > maxlen || maxlen - compressed_sz < 4) {
      return MakeError(ConvertError::Compress);
    }
    std::memset(compressed.data() + compressed_sz, 0, maxlen - compressed_sz);

    std::ostringstream stream;

    stream << "#pragma once\n\n";
    stream << "/*\n";
    stream << " *\tGenerated by File To Byte\n";
    stream << " *\thttps://github.com/gage-lodba/FileToByte\n";
    stream << " */\n\n";

    switch (encoding) {
      case Encoding::U32:
        emitU32(stream, array_name, compressed, compressed_sz);
        break;
      case Encoding::Base85:
        emitBase85(stream, array_name, compressed, compressed_sz);
        break;
      case Encoding::U8:
        emitU8(stream, array_name, compressed, compressed_sz);
        break;
    }

    return ConvertResult{.source = stream.str(), .error = ConvertError::None};
  } catch (const std::bad_alloc &) {
    return MakeError(ConvertError::OutOfMemory);
  } catch (const std::exception &) {
    // Any other stdlib failure (e.g. a stream exception) means compression or
    // output generation could not complete.
    return MakeError(ConvertError::Compress);
  }
}

// stb_compress* from stb.h - definition
//
// Vendored from stb.h by Sean Barrett (https://github.com/nothings/stb),
// dual-licensed MIT / public domain. Full notices: THIRD-PARTY.md and
// licenses/stb-LICENSE.txt.
//
// FileToByte's fork of the upstream (single-threaded) compressor threads all
// mutable state through CompressorState so concurrent Convert() calls cannot
// clobber each other. The public stb_compress() signature is unchanged and only
// touches its arguments plus locals.

////////////////////           compressor         ///////////////////////

namespace {

struct CompressorState {
  stb_uchar *out;
  stb_uint running_adler;
};

static stb_uint stb_adler32(stb_uint adler32, stb_uchar *buffer,
                            stb_uint buflen) {
  constexpr uint32_t ADLER_MOD = 65521;
  uint32_t s1 = adler32 & 0xffff;
  uint32_t s2 = adler32 >> 16;

  auto blocklen = buflen % 5552;
  while (buflen) {
    uint32_t i = 0;
    for (; i + 7 < blocklen; i += 8) {
      s1 += buffer[0];
      s2 += s1;
      s1 += buffer[1];
      s2 += s1;
      s1 += buffer[2];
      s2 += s1;
      s1 += buffer[3];
      s2 += s1;
      s1 += buffer[4];
      s2 += s1;
      s1 += buffer[5];
      s2 += s1;
      s1 += buffer[6];
      s2 += s1;
      s1 += buffer[7];
      s2 += s1;

      buffer += 8;
    }

    for (; i < blocklen; ++i) s1 += *buffer++, s2 += s1;

    s1 %= ADLER_MOD;
    s2 %= ADLER_MOD;
    buflen -= static_cast<stb_uint>(blocklen);
    blocklen = 5552;
  }
  return (s2 << 16) + s1;
}

static stb_uint stb_matchlen(stb_uchar *m1, stb_uchar *m2, stb_uint maxlen) {
  for (stb_uint i = 0; i < maxlen; ++i)
    if (m1[i] != m2[i]) return i;
  return maxlen;
}

static void stb_out(CompressorState &state, stb_uint v) {
  *state.out++ = static_cast<stb_uchar>(v);
}

static void stb_out2(CompressorState &state, stb_uint v) {
  stb_out(state, v >> 8);
  stb_out(state, v);
}
static void stb_out3(CompressorState &state, stb_uint v) {
  stb_out(state, v >> 16);
  stb_out(state, v >> 8);
  stb_out(state, v);
}
static void stb_out4(CompressorState &state, stb_uint v) {
  stb_out(state, v >> 24);
  stb_out(state, v >> 16);
  stb_out(state, v >> 8);
  stb_out(state, v);
}

static void outliterals(CompressorState &state, stb_uchar *in, int numlit) {
  while (numlit > 65536) {
    outliterals(state, in, 65536);
    in += 65536;
    numlit -= 65536;
  }

  if (numlit == 0)
    ;
  else if (numlit <= 32)
    stb_out(state, 0x000020 + numlit - 1);
  else if (numlit <= 2048)
    stb_out2(state, 0x000800 + numlit - 1);
  else /*  numlit <= 65536) */
    stb_out3(state, 0x070000 + numlit - 1);

  std::memcpy(state.out, in, numlit);
  state.out += numlit;
}

static constexpr int stb__window = 0x40000;  // 256K

static constexpr bool stb_not_crap(int best, int dist) {
  return ((best > 2 && dist <= 0x00100) || (best > 5 && dist <= 0x04000) ||
          (best > 7 && dist <= 0x80000));
}

static constexpr stb_uint stb__hashsize = 32768;

#define stb__hc(q, h, c) (((h) << 7) + ((h) >> 25) + q[c])
#define stb__hc2(q, h, c, d) (((h) << 14) + ((h) >> 18) + (q[c] << 7) + q[d])
#define stb__hc3(q, c, d, e) ((q[c] << 14) + (q[d] << 7) + q[e])

static int stb_compress_chunk(CompressorState &state, stb_uchar *history,
                              stb_uchar *start, stb_uchar *end, int length,
                              int *pending_literals, stb_uchar **chash,
                              stb_uint mask) {
  (void)history;
  [[maybe_unused]] const auto window = stb__window;
  stb_uint match_max;
  stb_uchar *lit_start = start - *pending_literals;
  stb_uchar *q = start;

#define STB__SCRAMBLE(h) (((h) + ((h) >> 16)) & mask)

  while (q < start + length && q + 12 < end) {
    int m;
    stb_uint h1, h2, h3, h4, h;
    stb_uchar *t;
    int best = 2, dist = 0;

    if (q + 65536 > end)
      match_max = static_cast<stb_uint>(end - q);
    else
      match_max = 65536;

#define stb__nc(b, d) \
  ((d) <= window &&   \
   ((b) > 9 || stb_not_crap(static_cast<int>(b), static_cast<int>(d))))

#define STB__TRY(t, p)                              \
  if (p ? dist != static_cast<int>(q - t) : 1)      \
    if ((m = stb_matchlen(t, q, match_max)) > best) \
      if (stb__nc(m, q - (t))) best = m, dist = static_cast<int>(q - (t))

    h = stb__hc3(q, 0, 1, 2);
    h1 = STB__SCRAMBLE(h);
    t = chash[h1];
    if (t) STB__TRY(t, 0);
    h = stb__hc2(q, h, 3, 4);
    h2 = STB__SCRAMBLE(h);
    h = stb__hc2(q, h, 5, 6);
    t = chash[h2];
    if (t) STB__TRY(t, 1);
    h = stb__hc2(q, h, 7, 8);
    h3 = STB__SCRAMBLE(h);
    h = stb__hc2(q, h, 9, 10);
    t = chash[h3];
    if (t) STB__TRY(t, 1);
    h = stb__hc2(q, h, 11, 12);
    h4 = STB__SCRAMBLE(h);
    t = chash[h4];
    if (t) STB__TRY(t, 1);

    chash[h1] = chash[h2] = chash[h3] = chash[h4] = q;

    if (best > 2) assert(dist > 0);

    if (best < 3) {
      ++q;
    } else if (best > 2 && best <= 0x80 && dist <= 0x100) {
      outliterals(state, lit_start, static_cast<int>(q - lit_start));
      lit_start = (q += best);
      stb_out(state, 0x80 + best - 1);
      stb_out(state, dist - 1);
    } else if (best > 5 && best <= 0x100 && dist <= 0x4000) {
      outliterals(state, lit_start, static_cast<int>(q - lit_start));
      lit_start = (q += best);
      stb_out2(state, 0x4000 + dist - 1);
      stb_out(state, best - 1);
    } else if (best > 7 && best <= 0x100 && dist <= 0x80000) {
      outliterals(state, lit_start, static_cast<int>(q - lit_start));
      lit_start = (q += best);
      stb_out3(state, 0x180000 + dist - 1);
      stb_out(state, best - 1);
    } else if (best > 8 && best <= 0x10000 && dist <= 0x80000) {
      outliterals(state, lit_start, static_cast<int>(q - lit_start));
      lit_start = (q += best);
      stb_out3(state, 0x100000 + dist - 1);
      stb_out2(state, best - 1);
    } else if (best > 9 && dist <= 0x1000000) {
      if (best > 65536) best = 65536;
      outliterals(state, lit_start, static_cast<int>(q - lit_start));
      lit_start = (q += best);
      if (best <= 0x100) {
        stb_out(state, 0x06);
        stb_out3(state, dist - 1);
        stb_out(state, best - 1);
      } else {
        stb_out(state, 0x04);
        stb_out3(state, dist - 1);
        stb_out2(state, best - 1);
      }
    } else {
      ++q;
    }
  }

  if (q - start < length) q = start + length;

  *pending_literals = static_cast<int>(q - lit_start);

  state.running_adler =
      stb_adler32(state.running_adler, start, static_cast<stb_uint>(q - start));
  return static_cast<int>(q - start);
}

static int stb_compress_inner(CompressorState &state, stb_uchar *input,
                              stb_uint length) {
  int literals = 0;

  std::vector<stb_uchar *> chash(stb__hashsize, nullptr);

  stb_out(state, 0x57);
  stb_out(state, 0xbc);
  stb_out2(state, 0);
  stb_out4(state, 0);
  stb_out4(state, length);
  stb_out4(state, stb__window);

  state.running_adler = 1;

  // The chunk call must run for its side effects (it writes the compressed
  // output); only the size check is debug-only, so len is unused once asserts
  // are compiled out in release builds.
  [[maybe_unused]] const auto len =
      stb_compress_chunk(state, input, input, input + length, length, &literals,
                         chash.data(), stb__hashsize - 1);
  assert(std::cmp_equal(len, length));

  outliterals(state, input + length - literals, literals);

  stb_out2(state, 0x05fa);
  stb_out4(state, state.running_adler);

  return 1;
}

}  // namespace

stb_uint stb_compress(stb_uchar *out, stb_uchar *input, stb_uint length) {
  CompressorState state{.out = out, .running_adler = 0};

  stb_compress_inner(state, input, length);

  return static_cast<stb_uint>(state.out - out);
}
