#pragma once
#include <string>

// Output encoding for the generated C array, mirroring Dear ImGui's
// binary_to_compressed_c.cpp.
enum class Encoding {
  U8,      // unsigned char[] of 0x-prefixed hex bytes (default;
           // endianness-independent)
  U32,     // unsigned int[] of 0x........ words (FileToByte's previous output)
  Base85,  // const char[] base85 string (loads via AddFontFromMemoryCompressed
           // Base85TTF; no separate size symbol)
};

// Reads `filename`, stb_compresses it, and returns a C source snippet declaring
// the compressed bytes as `symbol`_compressed_* in the requested `encoding`.
// On failure returns a human-readable error string (shown directly in the UI).
std::string Convert(const std::string &filename, const std::string &symbol,
                    Encoding encoding = Encoding::U8);
