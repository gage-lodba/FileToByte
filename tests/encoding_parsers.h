// In-process parsers for the three generated-C-source encodings emitted by
// Convert(): U8 hex bytes, U32 words, and the Base85 string. Tests use them to
// recover the compressed byte stream from the generated source text, then
// decompress it with the upstream stb_decompress (tests/stb_decompress.h).
#pragma once

#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <string>
#include <string_view>
#include <vector>

#include "stb_decompress.h"

namespace ftbtest {

struct Encoded {
  std::size_t declared_size = 0;       // from *_compressed_size (U8/U32)
  std::size_t declared_array_len = 0;  // from the array bound ([N] or [A/4])
  std::vector<unsigned char> bytes;    // recovered compressed bytes
  bool is_base85 = false;
  std::string error;
};

inline void SkipWs(std::string_view text, std::size_t &pos) {
  while (pos < text.size() && (text[pos] == ' ' || text[pos] == '\t' ||
                               text[pos] == '\n' || text[pos] == '\r'))
    ++pos;
}

// Parses the unsigned decimal that follows `anchor` (e.g. "_compressed_size =
// ").
inline bool ParseUIntAfter(std::string_view text, std::string_view anchor,
                           std::size_t &pos, std::size_t &out) {
  const std::size_t found = text.find(anchor, pos);
  if (found == std::string_view::npos) return false;
  std::size_t p = found + anchor.size();
  SkipWs(text, p);
  if (p >= text.size() || text[p] < '0' || text[p] > '9') return false;
  std::size_t value = 0;
  while (p < text.size() && text[p] >= '0' && text[p] <= '9') {
    value = value * 10 + static_cast<std::size_t>(text[p] - '0');
    ++p;
  }
  out = value;
  pos = p;
  return true;
}

// Parses the initializer list of a generated U8 array: {0x00, 0x0f, ...};
inline bool ParseU8(std::string_view src, std::string_view symbol,
                    Encoded &enc) {
  std::size_t pos = 0;
  if (!ParseUIntAfter(src, "_compressed_size = ", pos, enc.declared_size)) {
    enc.error = "U8: _compressed_size not found";
    return false;
  }

  const std::string anchor = std::string(symbol) + "_compressed_data[";
  std::size_t p = src.find(anchor, pos);
  if (p == std::string_view::npos) {
    enc.error = "U8: data array not found";
    return false;
  }
  p += anchor.size();
  const std::size_t close = src.find(']', p);
  if (close == std::string_view::npos) {
    enc.error = "U8: array ']' not found";
    return false;
  }
  {
    std::size_t q = p;
    SkipWs(src, q);
    std::size_t bound = 0;
    while (q < close && src[q] >= '0' && src[q] <= '9')
      bound = bound * 10 + static_cast<std::size_t>(src[q++] - '0');
    enc.declared_array_len = bound;
  }

  const std::size_t lb = src.find('{', close);
  const std::size_t rb =
      lb == std::string_view::npos ? std::string_view::npos : src.find('}', lb);
  if (lb == std::string_view::npos || rb == std::string_view::npos) {
    enc.error = "U8: initializer braces not found";
    return false;
  }
  std::size_t q = lb + 1;
  while (true) {
    SkipWs(src, q);
    if (q >= rb) break;
    if (src[q] == ',') {
      ++q;
      continue;
    }
    if (src[q] != '0' || q + 2 > src.size() || src[q + 1] != 'x') {
      enc.error = "U8: unexpected token at offset " + std::to_string(q);
      return false;
    }
    q += 2;
    unsigned int value = 0;
    int digits = 0;
    while (q < rb && std::isxdigit(static_cast<unsigned char>(src[q]))) {
      const char c = src[q++];
      value = value * 16 + static_cast<unsigned int>(c >= 'a'   ? c - 'a' + 10
                                                     : c >= 'A' ? c - 'A' + 10
                                                                : c - '0');
      ++digits;
    }
    if (digits < 1 || digits > 2) {
      enc.error = "U8: bad hex escape width";
      return false;
    }
    enc.bytes.push_back(static_cast<unsigned char>(value));
  }
  if (enc.bytes.size() != enc.declared_size) {
    enc.error = "U8: emitted " + std::to_string(enc.bytes.size()) +
                " bytes but declared " + std::to_string(enc.declared_size);
    return false;
  }
  if (enc.declared_array_len != enc.declared_size) {
    enc.error = "U8: array bound " + std::to_string(enc.declared_array_len) +
                " != declared size " + std::to_string(enc.declared_size);
    return false;
  }
  return true;
}

// Parses a U32 array (0x%08x words) back into little-endian byte order, which
// is what the generated words mean on the little-endian machine that produced
// them.
inline bool ParseU32(std::string_view src, std::string_view symbol,
                     Encoded &enc) {
  std::size_t pos = 0;
  if (!ParseUIntAfter(src, "_compressed_size = ", pos, enc.declared_size)) {
    enc.error = "U32: _compressed_size not found";
    return false;
  }

  const std::string anchor = std::string(symbol) + "_compressed_data[";
  std::size_t p = src.find(anchor, pos);
  if (p == std::string_view::npos) {
    enc.error = "U32: data array not found";
    return false;
  }
  p += anchor.size();
  const std::size_t close = src.find(']', p);
  if (close == std::string_view::npos) {
    enc.error = "U32: array ']' not found";
    return false;
  }
  // The declared bound is emitted as e.g. "24/4"; evaluate "A/4".
  {
    std::size_t q = p;
    SkipWs(src, q);
    std::size_t a = 0, b = 0;
    while (q < close && src[q] >= '0' && src[q] <= '9')
      a = a * 10 + static_cast<std::size_t>(src[q++] - '0');
    if (q < close && src[q] == '/') {
      ++q;
      while (q < close && src[q] >= '0' && src[q] <= '9')
        b = b * 10 + static_cast<std::size_t>(src[q++] - '0');
      a = (b == 0) ? 0 : a / b;
    }
    enc.declared_array_len = a;
  }

  const std::size_t lb = src.find('{', close);
  const std::size_t rb =
      lb == std::string_view::npos ? std::string_view::npos : src.find('}', lb);
  if (lb == std::string_view::npos || rb == std::string_view::npos) {
    enc.error = "U32: initializer braces not found";
    return false;
  }
  std::vector<unsigned int> words;
  std::size_t q = lb + 1;
  while (true) {
    SkipWs(src, q);
    if (q >= rb) break;
    if (src[q] == ',') {
      ++q;
      continue;
    }
    if (src[q] != '0' || q + 2 > src.size() || src[q + 1] != 'x') {
      enc.error = "U32: unexpected token at offset " + std::to_string(q);
      return false;
    }
    q += 2;
    unsigned int value = 0;
    int digits = 0;
    while (q < rb && std::isxdigit(static_cast<unsigned char>(src[q]))) {
      const char c = src[q++];
      value = (value << 4) + static_cast<unsigned int>(c >= 'a'   ? c - 'a' + 10
                                                       : c >= 'A' ? c - 'A' + 10
                                                                  : c - '0');
      ++digits;
    }
    if (digits != 8) {
      enc.error = "U32: word hex width != 8";
      return false;
    }
    words.push_back(value);
  }
  if (words.size() != enc.declared_array_len) {
    enc.error = "U32: emitted " + std::to_string(words.size()) +
                " words but declared " + std::to_string(enc.declared_array_len);
    return false;
  }
  for (const unsigned int word : words) {
    enc.bytes.push_back(static_cast<unsigned char>(word & 0xff));
    enc.bytes.push_back(static_cast<unsigned char>((word >> 8) & 0xff));
    enc.bytes.push_back(static_cast<unsigned char>((word >> 16) & 0xff));
    enc.bytes.push_back(static_cast<unsigned char>((word >> 24) & 0xff));
  }
  return true;
}

// Parses the Base85 string literal (including "\?" escapes and the
// "\"\n\t\"" line-wrap continuation) and decodes it with ImGui's Decode85.
inline bool ParseBase85(std::string_view src, std::string_view symbol,
                        Encoded &enc) {
  enc.is_base85 = true;
  const std::string anchor = std::string(symbol) + "_compressed_data_base85[";
  std::size_t p = src.find(anchor);
  if (p == std::string_view::npos) {
    enc.error = "B85: array not found";
    return false;
  }
  p += anchor.size();
  const std::size_t close = src.find(']', p);
  if (close == std::string_view::npos) {
    enc.error = "B85: array ']' not found";
    return false;
  }
  {
    std::size_t q = p;
    SkipWs(src, q);
    std::size_t bound = 0;
    while (q < close && src[q] >= '0' && src[q] <= '9')
      bound = bound * 10 + static_cast<std::size_t>(src[q++] - '0');
    enc.declared_array_len = bound;
  }

  std::size_t q = src.find('"', close);
  if (q == std::string_view::npos) {
    enc.error = "B85: opening quote not found";
    return false;
  }
  ++q;
  std::string text;
  bool terminated = false;
  while (q < src.size() && !terminated) {
    const char c = src[q];
    if (c == '\\') {
      // The emitter only ever escapes '?' as "\?". Reject anything else so a
      // real escaping bug cannot be silently normalized away.
      if (q + 1 < src.size() && src[q + 1] == '?') {
        text.push_back('?');
        q += 2;
        continue;
      }
      enc.error = "B85: unsupported escape sequence";
      return false;
    }
    if (c == '"') {
      ++q;
      std::size_t save = q;
      SkipWs(src, q);
      if (q < src.size() && src[q] == '"') {
        ++q;  // start of the next concatenated literal
        continue;
      }
      q = save;
      std::size_t save2 = q;
      SkipWs(src, save2);
      if (save2 < src.size() && src[save2] == ';') {
        terminated = true;
        break;
      }
      enc.error = "B85: expected continuation or terminator after quote";
      return false;
    }
    text.push_back(c);
    ++q;
  }
  if (!terminated) {
    enc.error = "B85: unterminated string literal";
    return false;
  }
  if (text.size() % 5 != 0) {
    enc.error = "B85: char count " + std::to_string(text.size()) +
                " is not a multiple of 5";
    return false;
  }
  if (text.size() + 1 != enc.declared_array_len) {
    enc.error = "B85: strlen " + std::to_string(text.size()) +
                " + NUL != declared array bound " +
                std::to_string(enc.declared_array_len);
    return false;
  }
  if (text.empty()) {
    enc.error = "B85: empty text";
    return false;
  }
  std::vector<unsigned char> z(text.begin(), text.end());
  z.push_back(0);
  enc.bytes.assign(text.size() / 5 * 4, 0);
  Decode85(z.data(), enc.bytes.data());
  return true;
}

}  // namespace ftbtest
