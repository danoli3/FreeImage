# Changelog

Release notes for this fork, newest first. Tag `3.19.21` is not a newer release. It points at the same 2024 commit as `3.19.3`.

## 3.19.17 — 24 Sep 2026

CMake and JPEG-XR build fix. No new CVEs.

- JPEG-XR is built on Windows again. The old CMake default never enabled it, and the source list was cut off, so the plugin did not link.
- `JXRMeta.h` no longer needs the removed MSVC `__in` annotation.
- MinGW uses the system `guiddef.h` and `_byteswap_ulong`.
- `add_subdirectory()` gets an include path for `FreeImage.h`.
- A static build publishes `FREEIMAGE_LIB`. A shared Windows build compiles with `FREEIMAGE_EXPORTS`.
- `cmake --install` supports `--component Runtime` and `--component Development`.
- iOS builds use `CMAKE_SYSTEM_NAME=iOS` with the Xcode generator. See `README.iphone`.

## 3.19.16 — 22 Sep 2026

Security release for the bundled decoders.

- **OpenEXR 3.3.13 → 3.3.14.** [GHSA-rmgv-rm47-38g3](https://github.com/AcademySoftwareFoundation/openexr/security/advisories/GHSA-rmgv-rm47-38g3) and [GHSA-5j5m-22wr-mhc6](https://github.com/AcademySoftwareFoundation/openexr/security/advisories/GHSA-5j5m-22wr-mhc6) (High). Unbounded allocation while parsing an `idmanifest`. Upstream had requested CVEs. None were named in the release.
- **LibRaw 0.22.1 → 0.22.2.** Allocation overflows and missing checks in CRX, X3F, Bayer, Olympus 14-bit, floating-point DNG, and thumbnails. No CVE ids from upstream.
- **LibTIFF stays 4.7.1.** PixarLog and OJPEG only take the 4.7.2 integer-overflow hardening. The rest of 4.7.2 was not taken.
- **Imath 3.1.12 → 3.2.3.**
- Libdeflate compress and decompress paths were synced toward 1.26 for an output-overflow fix. The header string is still `1.18`.
- OpenEXR builds on ARM64EC. MSVC was including x86 intrinsic headers there.

## 3.19.15 — 14 Sep 2026

Three old FreeImage loader overflows. All are NVD 8.8 High.

- **CVE-2020-24292.** ICO `LoadStandardIcon()`. Illegal bits-per-pixel or dimensions walked off the heap.
- **CVE-2020-24293.** PSD thumbnail packed-row copy used a separate `WidthBytes` from the allocated scanline.
- **CVE-2020-24295.** PSD `ReadImageLine()` wrote a channel past the DIB on both raw and RLE paths.

## 3.19.14 — 2 Sep 2026

The remaining FreeImage-owned CVEs from issue #35.

- **CVE-2024-9029** and **CVE-2024-28568.** IPTC 1-byte out-of-bounds read.
- **CVE-2024-28570.** Exif MakerNote signature compared past a short tag.
- **CVE-2024-28573** and **CVE-2024-28577.** Exif APP1 read with no length check.
- **CVE-2024-28578** and **CVE-2024-28580.** RAS loop counters and allocation overflow.
- **CVE-2024-28583.** XBM off-by-one stack write.
- **CVE-2024-28566.** TIFF `AssignPixel()` copied past the source strip.
- **CVE-2024-28571.** JPEG `fill_input_buffer()` use-after-free.
- **CVE-2024-28572.** Canon MakerNote tags read as the wrong type.
- XPM short pixel rows and color-name `sprintf` were hardened in the same change. No separate CVE id.

## 3.19.13 — 2 Sep 2026

Speed, a TIFF crash, and the big vendor bump.

- Sequential animated GIF playback is about 8× faster. Issue #18.
- Heap use-after-free when a TIFF has both an EXIF IFD and an ICC profile. Issue #27.
- **LibRaw 0.22.1, libwebp 1.6.0, libpng 1.6.58.**
- **OpenEXR 3.3.13** closes **CVE-2024-28562**, **CVE-2024-28563**, **CVE-2024-28564**, and **CVE-2024-28569**.
- **OpenJPEG 2.5.4** closes **CVE-2024-28574**, **CVE-2024-28575**, and **CVE-2024-28576**.
- libtiff and IJG libjpeg were bumped in the same stack. The current tree is libtiff 4.7.1 and libjpeg 10 (25 Jan 2026).
- OpenEXR is on by default for C++17 and newer. clang-cl, VS 2022/2026 presets, and MSYS2 MINGW64/UCRT64/CLANG64 were added.

## 3.19.12 — 17 Aug 2026

Loader security fixes and zlib 1.3.2.

- **CVE-2020-22524.** PFM integer overflow, undersized allocation.
- **CVE-2020-24294**, **CVE-2024-28565**, and **CVE-2025-65803.** PSD RLE unpacker heap overflow.
- **CVE-2024-28579** and **CVE-2024-28582.** HDR RLE scanline heap overflow.
- **CVE-2024-28584.** JPEG 2000 NULL pointer dereference.
- **CWE-457.** PCX uninitialized read when `planes=3` and `bpp=8`.
- **CWE-787.** PICT `UnpackBits` heap overflow.
- BMP RLE8 no longer rejects valid files. PICT and RAS crash fixes.
- **zlib 1.3.2.** Optional system-library linking via `USE_SYSTEM_*`.

## 3.19.11 — 22 Apr 2026

Three out-of-bounds writes found in academic testing. The release did not assign CVE ids.

- **CWE-787.** Crafted PICT file.
- **CWE-787.** 24-byte CUT file (`width=0x230B`). Crash in `PluginCUT.cpp`.
- **CWE-122.** 21-byte TGA file.
- Hotfixes for the macOS zlib and libpng builds.

## 3.19.10 — 31 Dec 2024

Same vendor set as 3.19.9. **LibTIFF 4.6.1.** C++23 support. `wchar` path fixes. `toHalfFloat` fix. No 32-bit DLL.

## 3.19.9 — 30 Jul 2024

Same notes as 3.19.8. **LibTIFF 4.6.1**, zlib 1.2.13, libpng 1.6.41, libwebp 1.2.1, LibRaw 0.21.1, libjpeg 9d, jxrlib 1.2.

## 3.19.8 — 29 Jul 2024

**LibTIFF 4.5.0 → 4.6.1.** `toHalfFloat` fix. Otherwise the same CMake and vendor set as 3.19.7.

## 3.19.7 — 26 Jul 2024

CMake build, C++23, and the first bundled set: zlib 1.2.13, libpng 1.6.41, LibTIFF 4.5.0, libwebp 1.2.1, LibRaw 0.21.1, libjpeg 9d, jxrlib 1.2. `wchar` path fixes. No 32-bit DLL.

## 3.19.6 — 10 Jul 2024

Unicode `wchar` path fixes. No separate GitHub release text. No CVEs.

## 3.19.5, 3.19.4, 3.19.3, 3.19.2 — 9–10 Jul 2024

The first CMake releases of this fork. All four published the same vendor list: zlib 1.2.13, libpng 1.6.41, LibTIFF 4.5.0, libwebp 1.2.1, LibRaw 0.21.1, libjpeg 9d, jxrlib 1.2. C++23 support. No 32-bit DLL. No CVEs in the release text.
