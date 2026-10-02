# Vendored codec integration

The issue #137 update imports these upstream tags:

| Library | Upstream tag | Commit | Library sources |
|---|---|---|---|
| libdeflate | `v1.26` | `92e6a0db9fa848d742f9eb286c92afc60f2c3dda` | `common_defs.h`, `libdeflate.h`, `lib/` |
| libtiff | `v4.7.2` | `d01a94be176f5f6a87f7ee1c0b32e65416aa2b4d` | `libtiff/`, plus `port/libport.h` |
| OpenEXR | `v3.5.1` | `490f4cb496ac1db9f675dae7bad689f676a8256f` | `src/lib/{Iex,IlmThread,OpenEXR,OpenEXRCore}` |

OpenEXR's `OpenEXR` library directory maps to `Source/OpenEXR/IlmImf`.
Its installed-style `Imath/` include prefixes are removed because this fork
keeps Imath headers flat under `Source/Imath`. Keep the checked-in portable
configuration headers, update version numbers and internal namespaces together,
and preserve the include-order guards in `OpenEXRCore/openexr_version.h`.
The upstream ARM64EC SIMD guards are now present in 3.5.1.

OpenEXR's auxiliary Zstandard 1.5.7 and OpenJPH 0.32.0 snapshots come from
that same tag's `external/` tree, including their licenses. The portable
OpenJPH sources are embedded directly into FreeImage. `ojph_arch.h` defines
`OJPH_DISABLE_SIMD`, so CMake and legacy make compile the same portable code
without additional target-specific flags. This can be slower for HTJ2K/LJ2K
than an upstream OpenJPH build with SIMD enabled. The C allocation helper uses
its upstream portable aligned-allocation fallback. Zstandard uses its C
implementation with `ZSTD_DISABLE_ASM`; preserve the ARM64EC exclusions in
`common/{compiler.h,cpu.h,portability_macros.h}` so `_M_X64` does not select
x86 intrinsics or CPUID on ARM64EC.

libdeflate 1.26 requires C11 (MSVC 2019 16.8 or later). CMake applies C11
specifically to its sources; GNU make defaults to C11. Upstream now excludes ARM64EC
from x86 detection and uses its portable fallback there.

libtiff retains FreeImage's checked-in `tif_config.h` and `tiffconf.h`;
`tiffvers.h` is substituted from the upstream template. Preserve these local
patches when re-importing:

- `tif_predict.c` excludes ARM64EC from the `_M_X64` SSE guards.
- `tif_win32.c` omits allocation/memory helpers supplied by `PluginTIFF.cpp`.
- `tif_config.h` uses `"zu"` for `size_t` and `PRId64` for this fork's
  `int64_t` signed size type, without a leading `%`.

The full 4.7.2 sync moves cursor fields into `TIFFDirectory`; the G3 loader
uses `tif_dir.td_row`. PixarLog now includes the decompression-ratio hook.

Always reconcile upstream CMake library source lists with both FreeImage
build paths. Include generated tables and their initialization helpers,
but exclude table-generator executables (`b44ExpLogTable.cpp`, `dwaLookups.cpp`).
After an update, run TestAPI and EXR/TIFF/G3 round trips, including EXR's new
compression methods and variable-sample deep ZSTD data, with AddressSanitizer.
Windows, ARM64EC, and Linux GNU make validation belongs in the existing CI matrix.
