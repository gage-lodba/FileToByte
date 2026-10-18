# Third-party notices

FileToByte itself is licensed under the MIT License (see [`LICENSE`](LICENSE)).
It also contains, links against, or embeds the third-party components listed
below. The complete license texts are in [`licenses/`](licenses/).

**`THIRD-PARTY.md` and `licenses/` must accompany every source and binary
distribution of FileToByte.** The MIT and zlib licenses require their
copyright/license text to travel with copies of the software, and Apache-2.0
§4 additionally requires retaining notices and marking modifications; the
dependencies below are statically linked into the release binaries.

## Components

| Component | Version / pinned revision | License | Where used | Modifications |
| --- | --- | --- | --- | --- |
| [VulkanWindow](https://github.com/gage-lodba/VulkanWindow) | `1a7855fa7c3ebdf975d785936f442d3fe8454a16` (short: `1a7855f`), submodule `deps/VulkanWindow` | MIT (Copyright (c) 2026 Jerimiah) | Window / Vulkan / ImGui application scaffold; statically linked into `FileToByte.exe` | None (used as pinned) |
| [Dear ImGui](https://github.com/ocornut/imgui) | `5a76f2adf1b0403b86a45010121fb32a6bff8680` (short: `5a76f2a`); `IMGUI_VERSION "1.92.9 WIP"`, `IMGUI_VERSION_NUM 19281`; submodule of VulkanWindow | MIT (Copyright (c) 2014-2026 Omar Cornut) | GUI library; statically linked. `imgui_draw.cpp` also supplies the `stb_decompress` decoder that expands the embedded fonts at runtime. | None to the vendored copy. `source/FTB.cpp` is adapted from ImGui's `misc/fonts/binary_to_compressed_c.cpp` (MIT); the adaptation is retained under the same MIT license. |
| [VulkanMemoryAllocator](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator) | `b3cbbb43ea3a506dffe10759e205a41c27c35ae2` (short: `b3cbbb4`); submodule of VulkanWindow | MIT (Copyright (c) 2017-2026 Advanced Micro Devices, Inc. All rights reserved.) | Vulkan memory allocation; statically linked | None |
| [GLFW](https://www.glfw.org/) | `43c9fb329116fcd57e94c8edc9ce2c1619a370a8` (short: `43c9fb3`); submodule of VulkanWindow | zlib (Copyright (c) 2002-2006 Marcus Geelnard; Copyright (c) 2006-2019 Camilla Löwy) | Window creation, input and event handling; statically linked | None |
| [stb](https://github.com/nothings/stb) | Unversioned (see note below) | MIT **or** public domain (Sean Barrett) — dual-licensed, "choose whichever you prefer" | `stb_compress` and `stb_adler32` are copied into `source/FTB.cpp` (the project's own converter). `stb_decompress` is embedded in Dear ImGui's `imgui_draw.cpp` and used at runtime. | `stb_compress`/`stb_adler32` were copied into `source/FTB.cpp` and adapted to C++ (static helpers, `constexpr`/`[[maybe_unused]]` cleanup). |
| [JetBrains Mono](https://github.com/JetBrains/JetBrainsMono) | 2.304 (release tag `v2.304`, 2023-01-14), Regular | SIL Open Font License 1.1 (Copyright 2020 The JetBrains Mono Project Authors) | Embedded in [`source/fonts/JetBrainsMono.h`](source/fonts/JetBrainsMono.h); monospaced UI font (`Mono`) used by the result box | Modified: the `.ttf` is additionally compressed with `stb_compress` and embedded as a C array. The decompressed bytes are byte-identical to the upstream `JetBrainsMono-Regular.ttf` v2.304; the font program itself is unmodified. No Reserved Font Name is declared. |
| [Roboto](https://fonts.google.com/specimen/Roboto) | 2.137 (2017), Regular | Apache License 2.0 (Copyright 2011 Google Inc. All Rights Reserved.) | Embedded in [`source/fonts/Roboto.h`](source/fonts/Roboto.h); default UI font | Modified (Apache-2.0 §4(b) notice): the `.ttf` was re-encoded — compressed with `stb_compress` and emitted as a C array — to embed it in the source tree. The decompressed bytes are byte-identical to the original font file; the font program is unmodified. Copyright/license retained — see below. |

### stb note

The copied stb code in `source/FTB.cpp` is the `stb_compress`/`stb_adler32`
implementation from stb.h by Sean Barrett, dual-licensed as MIT or public
domain. The exact upstream revision is not recorded in the source (a known
gap); this file and
[`licenses/stb-LICENSE.txt`](licenses/stb-LICENSE.txt) provide the required
attribution until the stb revision is pinned.

## Embedded fonts

Both fonts are shipped as `stb_compress`-compressed C arrays and decompressed by
Dear ImGui at startup. The embedded data decompresses byte-for-byte to the
upstream `.ttf` files; only the distribution container differs.

| Font | Header | Upstream source | Version | SHA-256 of the original `.ttf` | License |
| --- | --- | --- | --- | --- | --- |
| JetBrains Mono Regular | `source/fonts/JetBrainsMono.h` | `JetBrainsMono-2.304.zip` → `fonts/ttf/JetBrainsMono-Regular.ttf` | 2.304 (2023-01-14) | `A0BF60EF0F83C5ED4D7A75D45838548B1F6873372DFAC88F71804491898D138F` | OFL-1.1 — [`licenses/OFL-1.1.txt`](licenses/OFL-1.1.txt) |
| Roboto Regular | `source/fonts/Roboto.h` | Roboto Regular (Google) | 2.137 (2017) | `4E147AB64B9FDF6D89D01F6B8C3CA0B3CDDC59D608A8E2218F9A2504B5C98E14` | Apache-2.0 — [`licenses/Apache-2.0.txt`](licenses/Apache-2.0.txt) |

Copyright lines, as recorded in each font's `name` table:

- JetBrains Mono: `Copyright 2020 The JetBrains Mono Project Authors (https://github.com/JetBrains/JetBrainsMono)`
- Roboto: `Copyright 2011 Google Inc. All Rights Reserved.`

The headers are generated deterministically with FileToByte's own converter.
They can be regenerated from the upstream `.ttf` at any time:

```cpp
// U32 encoding, as used by the committed headers.
Convert(path_to_ttf, "JetBrainsMono", Encoding::U32);
Convert(path_to_ttf, "Roboto", Encoding::U32);
```

**Consolas removal.** Earlier revisions embedded Microsoft Consolas
(`source/fonts/Consolas.h`). Microsoft's font licensing does not permit
redistribution, so that header was deleted and the monospaced font was replaced
with JetBrains Mono (OFL-1.1), which explicitly permits bundling and
redistribution with software.

## Not bundled

The Vulkan loader/runtime (`vulkan-1.dll`) and the Microsoft C/C++ runtime
(`MSVCP140.dll`, `VCRUNTIME140*.dll`, `api-ms-win-crt-*`) are system
prerequisites supplied by the Vulkan runtime and the VC++ redistributable. They
are not part of this repository or its release artifacts.

## Full license texts

| File | Applies to |
| --- | --- |
| [`licenses/Apache-2.0.txt`](licenses/Apache-2.0.txt) | Roboto |
| [`licenses/OFL-1.1.txt`](licenses/OFL-1.1.txt) | JetBrains Mono (SIL OFL 1.1 + copyright line) |
| [`licenses/MIT.txt`](licenses/MIT.txt) | VulkanWindow, Dear ImGui, VulkanMemoryAllocator (each with its copyright notice) |
| [`licenses/zlib.txt`](licenses/zlib.txt) | GLFW |
| [`licenses/stb-LICENSE.txt`](licenses/stb-LICENSE.txt) | stb |
