// Decompressor and Base85 decoder used by the FileToByte converter tests.
//
// Extracted verbatim from the pinned upstream Dear ImGui copy in this repo:
//   deps/VulkanWindow/deps/ImGui/imgui_draw.cpp
//     - stb_decompress_length / stb_decompress:  lines 6229-6337
//     - Decode85Byte / Decode85:                 lines 3141-3151
// The stb decompressor is public-domain stb.h code by Sean Barrett
// (https://github.com/nothings/stb); the Decode85 helper is Dear ImGui
// (MIT-licensed, deps/VulkanWindow/deps/ImGui/LICENSE.txt). The only edits are
// mechanical: IM_ASSERT -> assert and a test namespace for header-only use.
#pragma once

#include <cassert>
#include <cstring>

namespace ftbtest {

// ---- stb_decompress (imgui_draw.cpp:6229-6337) ----
inline unsigned int stb_decompress_length(const unsigned char *input) {
  return (input[8] << 24) + (input[9] << 16) + (input[10] << 8) + input[11];
}

inline unsigned char *stb__barrier_out_e;
inline unsigned char *stb__barrier_out_b;
inline const unsigned char *stb__barrier_in_b;
inline unsigned char *stb__dout;

inline void stb__match(const unsigned char *data, unsigned int length) {
  // INVERSE of memmove... write each byte before copying the next...
  assert(stb__dout + length <= stb__barrier_out_e);
  if (stb__dout + length > stb__barrier_out_e) {
    stb__dout += length;
    return;
  }
  if (data < stb__barrier_out_b) {
    stb__dout = stb__barrier_out_e + 1;
    return;
  }
  while (length--) *stb__dout++ = *data++;
}

inline void stb__lit(const unsigned char *data, unsigned int length) {
  assert(stb__dout + length <= stb__barrier_out_e);
  if (stb__dout + length > stb__barrier_out_e) {
    stb__dout += length;
    return;
  }
  if (data < stb__barrier_in_b) {
    stb__dout = stb__barrier_out_e + 1;
    return;
  }
  std::memcpy(stb__dout, data, length);
  stb__dout += length;
}

#define stb__in2(x) ((i[x] << 8) + i[(x) + 1])
#define stb__in3(x) ((i[x] << 16) + stb__in2((x) + 1))
#define stb__in4(x) ((i[x] << 24) + stb__in3((x) + 1))

inline const unsigned char *stb_decompress_token(const unsigned char *i) {
  if (*i >= 0x20) {  // use fewer if's for cases that expand small
    if (*i >= 0x80)
      stb__match(stb__dout - i[1] - 1, i[0] - 0x80 + 1), i += 2;
    else if (*i >= 0x40)
      stb__match(stb__dout - (stb__in2(0) - 0x4000 + 1), i[2] + 1), i += 3;
    else /* *i >= 0x20 */
      stb__lit(i + 1, i[0] - 0x20 + 1), i += 1 + (i[0] - 0x20 + 1);
  } else {  // more ifs for cases that expand large, since overhead is
            // amortized
    if (*i >= 0x18)
      stb__match(stb__dout - (stb__in3(0) - 0x180000 + 1), i[3] + 1), i += 4;
    else if (*i >= 0x10)
      stb__match(stb__dout - (stb__in3(0) - 0x100000 + 1), stb__in2(3) + 1),
          i += 5;
    else if (*i >= 0x08)
      stb__lit(i + 2, stb__in2(0) - 0x0800 + 1),
          i += 2 + (stb__in2(0) - 0x0800 + 1);
    else if (*i == 0x07)
      stb__lit(i + 3, stb__in2(1) + 1), i += 3 + (stb__in2(1) + 1);
    else if (*i == 0x06)
      stb__match(stb__dout - (stb__in3(1) + 1), i[4] + 1), i += 5;
    else if (*i == 0x04)
      stb__match(stb__dout - (stb__in3(1) + 1), stb__in2(4) + 1), i += 6;
  }
  return i;
}

inline unsigned int stb_adler32(unsigned int adler32, unsigned char *buffer,
                                unsigned int buflen) {
  const unsigned long ADLER_MOD = 65521;
  unsigned long s1 = adler32 & 0xffff, s2 = adler32 >> 16;
  unsigned long blocklen = buflen % 5552;

  unsigned long i;
  while (buflen) {
    for (i = 0; i + 7 < blocklen; i += 8) {
      s1 += buffer[0], s2 += s1;
      s1 += buffer[1], s2 += s1;
      s1 += buffer[2], s2 += s1;
      s1 += buffer[3], s2 += s1;
      s1 += buffer[4], s2 += s1;
      s1 += buffer[5], s2 += s1;
      s1 += buffer[6], s2 += s1;
      s1 += buffer[7], s2 += s1;

      buffer += 8;
    }

    for (; i < blocklen; ++i) s1 += *buffer++, s2 += s1;

    s1 %= ADLER_MOD, s2 %= ADLER_MOD;
    buflen -= blocklen;
    blocklen = 5552;
  }
  return (unsigned int)(s2 << 16) + (unsigned int)s1;
}

// Returns the decompressed length on success, 0 on failure. NOTE: a legitimate
// 0-byte payload also returns 0; callers that care about the empty case must
// check stb_decompress_length() themselves.
inline unsigned int stb_decompress(unsigned char *output,
                                   const unsigned char *i,
                                   unsigned int /*length*/) {
  if (stb__in4(0) != 0x57bC0000) return 0;
  if (stb__in4(4) != 0) return 0;  // error! stream is > 4GB
  const unsigned int olen = stb_decompress_length(i);
  stb__barrier_in_b = i;
  stb__barrier_out_e = output + olen;
  stb__barrier_out_b = output;
  i += 16;

  stb__dout = output;
  for (;;) {
    const unsigned char *old_i = i;
    i = stb_decompress_token(i);
    if (i == old_i) {
      if (*i == 0x05 && i[1] == 0xfa) {
        assert(stb__dout == output + olen);
        if (stb__dout != output + olen) return 0;
        if (stb_adler32(1, output, olen) != (unsigned int)stb__in4(2)) return 0;
        return olen;
      } else {
        assert(0); /* NOTREACHED */
        return 0;
      }
    }
    assert(stb__dout <= output + olen);
    if (stb__dout > output + olen) return 0;
  }
}

#undef stb__in2
#undef stb__in3
#undef stb__in4

// ---- Base85 decoder (imgui_draw.cpp:3141-3151) ----
inline unsigned int Decode85Byte(char c) {
  return c >= '\\' ? (unsigned int)(c - 36) : (unsigned int)(c - 35);
}

inline void Decode85(const unsigned char *src, unsigned char *dst) {
  while (*src) {
    unsigned int tmp = Decode85Byte((char)src[0]) +
                       85 * (Decode85Byte((char)src[1]) +
                             85 * (Decode85Byte((char)src[2]) +
                                   85 * (Decode85Byte((char)src[3]) +
                                         85 * Decode85Byte((char)src[4]))));
    dst[0] = ((tmp >> 0) & 0xFF);
    dst[1] = ((tmp >> 8) & 0xFF);
    dst[2] = ((tmp >> 16) & 0xFF);
    dst[3] = ((tmp >> 24) & 0xFF);  // We can't assume little-endianness.
    src += 5;
    dst += 4;
  }
}

}  // namespace ftbtest
