#pragma once

#include <cstddef>
#include <string>
#include <string_view>

// Output encoding for the generated C array, mirroring Dear ImGui's
// binary_to_compressed_c.cpp.
enum class Encoding {
  U8,      // unsigned char[] of 0x-prefixed hex bytes (default;
           // endianness-independent)
  U32,     // unsigned int[] of 0x........ words (endianness-dependent)
  Base85,  // const char[] base85 string (loads via
           // AddFontFromMemoryCompressedBase85TTF)
};

enum class ConvertError {
  None,
  FileOpen,  // could not open (missing, locked, directory, unrepresentable
             // path)
  FileSize,  // could not determine the file size
  FileRead,  // read failed / file changed while reading
  TooLarge,  // file exceeds kMaxInputSize
  InvalidArrayName,  // array_name is not a valid C identifier
  OutOfMemory,       // allocation failed
  Compress,          // compressor reported failure
};

struct ConvertResult {
  std::string source;  // generated C++ source; empty on failure
  ConvertError error = ConvertError::None;

  [[nodiscard]] bool ok() const noexcept { return error == ConvertError::None; }
  // Human-readable, static-lifetime message for `error`. Never nullptr.
  [[nodiscard]] const char *message() const noexcept;
};

// Maximum supported input file size. Larger files are rejected with
// ConvertError::TooLarge so the generated source and buffers stay bounded.
inline constexpr std::size_t kMaxInputSize = 16u * 1024u * 1024u;

// Reads `file_path` (UTF-8, on Windows opened through the wide API), compresses
// it with stb_compress, and returns C++ source declaring the compressed bytes
// as
// `<array_name>_compressed_*` in the requested encoding. Thread-safe: may be
// called concurrently. `array_name` must match [A-Za-z][A-Za-z0-9_]*.
[[nodiscard]] ConvertResult Convert(std::string_view file_path,
                                    std::string_view array_name,
                                    Encoding encoding = Encoding::U8);
