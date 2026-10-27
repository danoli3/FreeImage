What is FreeImage ?
-----------------------------------------------------------------------------
FreeImage is an Open Source library project for developers who would like to support image formats including PNG, BMP, JPEG, TIFF, WebP, JPEG 2000, JPEG XR, OpenEXR, camera RAW, and optional HEIF/HEIC, AVIF and JPEG XL as needed by today's multimedia applications.
FreeImage is easy to use, fast, multithreading safe, and cross-platform (works with Windows, Linux and Mac OS X).

### This GitHub fork

Security and bug fixes on top of upstream FreeImage, plus a CMake build that compiles FreeImage and every bundled library from one `CMakeLists.txt`. This repository is the source tree. [GitHub releases](https://github.com/danoli3/FreeImage/releases) provide source archives and Linux packages; see the [packaging guide](packaging/README.md). Build a static library or a DLL with the commands in [Building this fork](#building-this-fork).

This tree is FreeImage **3.19.28**. `FreeImage_GetVersion()` prints `FREEIMAGE_MAJOR_VERSION`, `FREEIMAGE_MINOR_VERSION`, and `FREEIMAGE_RELEASE_SERIAL` from `Source/FreeImage.h`. CMake takes the version from the latest numeric git tag (`git describe`) and falls back to 3.19.28 when the archive has no `.git` history.

The ANSI C API is usable from C, C++, VB, C#, Delphi, Java, and from scripting languages such as Perl, Python, PHP, TCL, Lua, and Ruby.

### Why use FreeImage instead of linking each format library yourself?

Loading a PNG, a GIF and a JPEG from scratch in C++ means three different APIs (`libpng`, `giflib`-or-hand-rolled-LZW, `libjpeg`), three different error-handling conventions, and three sets of build flags to get right across platforms. FreeImage wraps [all of the libraries below](#supported-formats--bundled-libraries) behind one API - one `FreeImage_Load()`/`FreeImage_Save()` pair, one pixel format model (`FIBITMAP`), one metadata API (EXIF/IPTC/XMP) - so format-specific code doesn't leak into the rest of your application. Swapping a PNG for a WebP is a one-line change rather than a new dependency. Camera RAW uses the same `FreeImage_Load()` call. It is included unless the build passes `-DBUILD_LIBRAWLITE=OFF`.

This fork additionally builds the whole stack (FreeImage plus every bundled library) from one `CMakeLists.txt`, so `find_package(FreeImage)` is the only thing a consuming CMake project needs - see [Using compiled binaries](#using-compiled-binaries) below.

## Supported formats & bundled libraries

Formats implemented directly in FreeImage's own plugin code (no external library): BMP, ICO, TARGA/TGA, PCX, DDS, GIF, PSD, PICT, PFM, HDR/Radiance RGBE, RAS/Sun Raster, SGI, XBM, XPM, Amiga IFF/LBM, Kodak PCD, PNM/PBM/PGM/PPM, CUT, and WBMP.

Everything else is backed by a bundled copy of the format's reference library. Turn a format off with its `BUILD_*` option, or link a system copy with `USE_SYSTEM_*` (or `FREEIMAGE_USE_SYSTEM_LIBS`, which turns every `USE_SYSTEM_*` default on). See `CMakeLists.txt`.

The bundled libtiff is fully synced to 4.7.2, including its integer-overflow hardening and decompression-ratio checks. OpenEXR is included when `CMAKE_CXX_STANDARD` is unset or 17 or newer, and left out for an older standard.

| Formats | Library | Bundled version | Default | CMake option |
|---|---|---|---|---|
| PNG | [libpng](http://www.libpng.org/pub/png/libpng.html) (+ [zlib](https://zlib.net/)) | 1.6.59 (zlib 1.3.2) | on | `BUILD_LIBPNG` |
| JPEG | [libjpeg (IJG)](http://ijg.org/) | 10 (25 Jan 2026) | on | `BUILD_LIBJPEG` |
| TIFF | [libtiff](http://www.libtiff.org/) | 4.7.2 | on | `BUILD_LIBTIFF` |
| JPEG 2000 (J2K/JP2) | [OpenJPEG](https://github.com/uclouvain/openjpeg) | 2.5.4 | on | `BUILD_LIBOPENJPEG` |
| OpenEXR (HDR) | [OpenEXR](https://openexr.com/) | 3.5.2 | on for C++17+ | `BUILD_OPENEXR` |
| WebP | [libwebp](https://developers.google.com/speed/webp) | 1.6.0 | on | `BUILD_WEBP` |
| Camera RAW (including CR3 and X3F) | [LibRaw](https://www.libraw.org/) | 0.22.3 | on | `BUILD_LIBRAWLITE` |
| JPEG XR (JXR/WDP/HDP) | [jxrlib](https://github.com/4creators/jxrlib) | unversioned snapshot | on for Windows; optional elsewhere | `BUILD_JXR` |
| HEIF/HEIC/HIF and AVIF | [libheif](https://github.com/strukturag/libheif) | 1.23.7 | off | `BUILD_LIBHEIF` |
| JPEG XL (JXL) | [libjxl](https://github.com/libjxl/libjxl) | 0.12.0 | off | `BUILD_LIBJXL` |

The optional formats use bundled sources when enabled through CMake. HEIF/AVIF
and JPEG XL support reading and writing still images; see their sections below
for pixel types, metadata and encoder limitations.

Supporting libraries are bundled alongside the format libraries:

| Used by | Bundled libraries |
|---|---|
| PNG, TIFF and other compressed formats | zlib 1.3.2 |
| OpenEXR | Imath 3.2.3, libdeflate 1.26, Zstandard 1.5.7, OpenJPH 0.32.0 |
| HEIF/HEIC/HIF | libde265 1.1.3 (HEVC decoding), Kvazaar 2.3.2 (HEVC encoding) |
| AVIF | libaom 3.15.2 (AV1 decoding/encoding; enabled by default with bundled HEIF) |
| JPEG XL | Highway 1.4.0, Brotli 1.2.1, Little CMS 2.19.1 |

See [third-party versions](THIRD_PARTY_VERSIONS.md) for version sources and
[optional codec bundles](Source/ThirdParty/README.md) for archive checksums,
licenses and build requirements.

## Original Source Code Upstream
https://sourceforge.net/projects/freeimage
Original library can be found : https://freeimage.sourceforge.io 

## Status

[![CMake build](https://github.com/danoli3/FreeImage/actions/workflows/cmake.yml/badge.svg)](https://github.com/danoli3/FreeImage/actions/workflows/cmake.yml)

## Building this fork

JPEG-XR is enabled by default only on Windows (`BUILD_JXR` defaults to `WIN32`); pass `-DBUILD_JXR=ON` to include it on other platforms. Camera RAW is included by default (`-DBUILD_LIBRAWLITE=OFF` leaves it out).

### Simply build it

```bash
cmake . -B cmake-build
cmake --build cmake-build # On Linux, add -j$(nproc) for multicore build
```

For JP2/JXR format detection and display-conversion troubleshooting, see the
[codec diagnostic example](Examples/codec-diagnostics/README.md). Detect the
file signature before falling back to its extension: a raw J2K codestream can
have a `.jp2` filename. JPEG 2000 and JPEG XR can decode to native 16-bit or
floating-point image types. Check `FreeImage_GetImageType()` before display
conversion; HDR previews require an explicit tone-mapping policy. The example
provides a 24-bit RGB preview without changing the native decoded pixels.

### Platform-specific examples

**Linux (gcc) / macOS (clang)**
```bash
cmake -S . -B build -DBUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build -- -j$(nproc)   # macOS: -j$(sysctl -n hw.ncpu)
```

**Windows (MSVC, Visual Studio toolchain)**
```bash
cmake -S . -B build -DBUILD_TESTS=ON -DCMAKE_CXX_STANDARD=17
cmake --build build --config Debug
```
See [Generate Visual Studio project files](#generate-visual-studio-project-files) below for opening the solution directly in the IDE.

To select the MSVC C runtime, pass `-DFREEIMAGE_MSVC_RUNTIME=MD` for the
dynamic CRT (`/MDd` in Debug and `/MD` in Release), or
`-DFREEIMAGE_MSVC_RUNTIME=MT` for the static CRT (`/MTd` in Debug and `/MT` in
Release). The default `Default` leaves CMake's runtime selection unchanged;
CMake's default with this project's policy is `/MDd` in Debug and `/MD`
in Release.
This setting is forwarded to bundled HEIF and JPEG XL dependencies, including
AOM and Highway. Prebuilt system libraries are not changed by this option; use
libraries built with the same CRT setting. Static FreeImage consumers should
also use the same CRT.

For example, to build and test a DLL with the static CRT:
```powershell
cmake -S . -B build -DBUILD_TESTS=ON -DFREEIMAGE_STATIC=OFF -DFREEIMAGE_MSVC_RUNTIME=MT
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

MSVC Debug installs include available PDB symbols. Static builds install the
compiler-generated `FreeImage.pdb` beside the `.lib` in `lib/` (the
`Development` component). DLL builds install the linker-generated PDB beside
the DLL in `bin/` (the `Runtime` component). Toolchains or debug formats that
embed symbols in object files do not produce a separate compiler PDB; its
absence does not prevent installation. Release installs omit these PDB files.

**Windows (MinGW, e.g. a toolchain already on `PATH`)**
```bash
cmake -S . -B build -DBUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug -G "MinGW Makefiles"
cmake --build build
```

**Windows (MinGW-w64 via [MSYS2](https://www.msys2.org/), see [#30](https://github.com/danoli3/FreeImage/issues/30))**

From an MSYS2 `MINGW64` shell:
```bash
pacman -S mingw-w64-x86_64-toolchain mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja
cmake -S . -B build -G Ninja -DBUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

**Plain `make` (Linux, no CMake)**
```bash
make -j$(nproc)
```

### Generate Visual Studio project files

```bash
cmake --preset vs2022   # or: cmake --preset vs2026
```
Generates a solution under `build-vs2022/` (or `build-vs2026/`). Open `FreeImage.sln` from there. Both presets are Windows-only and turn `BUILD_TESTS` on. `vs2026` needs CMake 4.2 or newer.

`cmake --preset ninja-msvc` builds with `cl.exe` and Ninja under `build-ninja-msvc/`. Run it from a Developer Command Prompt, or after `vcvarsall.bat`, so `cl.exe` is on `PATH`. See `CMakePresets.json`.

### Build and install Debug and Release configuration

```bash
cmake . -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=install_dir
cmake --build cmake-build-debug --config Debug --target install # Linux: -j$(nproc)

cmake . -B cmake-build-release -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install_dir
cmake --build cmake-build-release --config Release --target install # Linux: -j$(nproc)
```
Now `install_dir` contains the compiled binaries for debug and release, as well as the header file and the CMake Config files.

## Using compiled binaries

First, build and install it like explained above.

Then, in another project add the following CMake code:
```cmake
set(CMAKE_PREFIX_PATH <freeimage_install_location>)
find_package(FreeImage CONFIG REQUIRED)
...
target_link_libraries(<your_target> PRIVATE FreeImage::FreeImage)
```

For `find_package` to work, simply set `CMAKE_PREFIX_PATH` to the directory where the compiled binaries are installed (the `install_dir` folder from above).

## Running tests

```bash
cmake . -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON
cmake --build cmake-build-debug --config Debug # Linux: -j$(nproc)
ctest --test-dir cmake-build-debug -C Debug # Optionally --rerun-failed --output-on-failure
```

> This ctest command only works with CMake 3.20 or higher. For earlier versions, you must `cd` into `cmake-build-debug` and call ctest without `--test-dir cmake-build-debug`.

### Optional HEIF/HEIC/HIF and AVIF

Enable with `-DBUILD_LIBHEIF=ON` to build bundled libheif 1.23.7, libde265,
Kvazaar and libaom. The option defaults to OFF. Source archives are included
and SHA-256 verified; no external codec SDK or network download is needed.
See [bundled codec build instructions](#build-heifavif-and-jpeg-xl-from-bundled-sources)
for compiler requirements and how to omit AOM when only HEVC is needed.

For an external SDK, also select `-DUSE_SYSTEM_LIBHEIF=ON` and install
**libheif 1.12 or newer** with its HEVC/AV1 backends. Ubuntu can use
`sudo apt install libheif-dev pkg-config`; macOS can use
`brew install libheif pkg-config`. Available decoders and encoders depend on
the SDK's backends. The legacy make build registers unsupported format stubs.

Use `FIF_HEIF` for HEIF/HEIC/HIF and `FIF_AVIF` for AVIF, or detect the format
with `FreeImage_GetFileType`. The decoder loads the primary still image as
8-bit RGB or RGBA, including alpha and ICC profiles, and supports
`FIF_LOAD_NOPIXELS`. High-bit-depth sources are converted to 8-bit output.
Raw EXIF is imported/exported and parsed EXIF is available on load; embedded
thumbnails are imported when decodable. A corrupt thumbnail is skipped with
a diagnostic, allowing the primary image to load. Animation and XMP import
are not included.

Saving accepts 24-bit RGB or 32-bit RGBA, quality 1..100 (default 90), and
`HEIF_LOSSLESS` when the selected encoder supports exact RGB/4:4:4 coding.
Bundled Kvazaar supports lossy HEIF output; exact RGB lossless HEIF requires
a suitable system encoder such as x265. Bundled AOM supports lossless AVIF.
Encoder availability is reflected by `FreeImage_FIFSupportsWriting`.

The decoder is adapted from [FreeImageRe's PluginHEIF.cpp](https://github.com/agruzdev/FreeImageRe/blob/b51d3945bfe785ad382a0969f1376aee344af8c6/Source/Plugins/PluginHEIF.cpp)
under its FIPL/GPLv2 licensing. libheif and libde265 are LGPL-3.0-or-later;
other backends have their own licenses. See the bundled license notices.

### Saving files safely

FreeImage_Save and FreeImage_SaveU stage encoder output beside the destination.
Encoding, stream and close errors return FALSE and leave an existing file intact.
Replacement occurs only after successful close. This is an atomic replacement,
not a power-loss durability guarantee. POSIX new files start with private 0600
permissions; existing regular-file permissions are preserved. POSIX special files
and symlink destinations are rejected; hard links are replaced, not updated in place.

### Optional JPEG XL

Configure with `-DBUILD_LIBJXL=ON` (default OFF) to build bundled libjxl 0.12.0, Highway, Brotli and Little CMS. For an external SDK, also set `-DUSE_SYSTEM_LIBJXL=ON` and install libjxl >=0.7 (pkg-config on Unix, or the Windows SDK setup below).
The FIF_JPEGXL plugin reads/writes still images: RGB/RGBA and grayscale,
8/16-bit integer and 32-bit float, ICC profiles, and header-only loading.
Animations are rejected. EXIF/XMP and thumbnails are not imported initially.
Pass JXL_LOSSLESS for exact lossless encoding; otherwise quality flags 1..100
map to distance (100-quality)/10, with 90 as the default.
The classic make build registers an unsupported stub; enable with CMake.

### Windows HEIF/AVIF and JPEG XL SDKs

With `USE_SYSTEM_LIBHEIF=ON` / `USE_SYSTEM_LIBJXL=ON`, the optional codec
switches use installed development libraries. Discovery prefers native CMake packages,
then pkg-config, then SDK headers and import libraries. Visual Studio users
can use vcpkg without installing pkg-config separately:

```powershell
C:/vcpkg/vcpkg.exe install "libheif[aom]:x64-windows" libjxl:x64-windows
cmake -S . -B build -A x64 `
  -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake `
  -DBUILD_LIBHEIF=ON -DBUILD_LIBJXL=ON `
  -DUSE_SYSTEM_LIBHEIF=ON -DUSE_SYSTEM_LIBJXL=ON
cmake --build build --config Release
```

Adjust the vcpkg path to your installation. For CMake GUI, set the toolchain
file before the first Configure (use a fresh build directory when changing
it). AOM enables AVIF; the default HEVC feature supplies HEIF encoding, and
libde265 supplies HEIF decoding. Match the SDK architecture to the generator.
Make the codec DLLs and their backend dependencies available on PATH when
running applications, or deploy them beside the executable. If the SDK builds
codec backends as plugins, set LIBHEIF_PLUGIN_PATH to its plugins/libheif
directory as well.

An existing SDK can instead be selected with CMAKE_PREFIX_PATH. Direct SDK
hints are FreeImage_HEIF_INCLUDE_DIR / FreeImage_JXL_INCLUDE_DIR and matching
FreeImage_<codec>_LIBRARY_RELEASE / _LIBRARY_DEBUG cache entries. For static
codec SDKs, use their exported CMake targets or provide any missing transitive
libraries through FreeImage_<codec>_EXTRA_LIBRARIES in SDK mode. Installed
FreeImage CMake consumers use the same discovery rules.

### Unicode multipage filenames (Windows)

Use `FreeImage_OpenMultiBitmapU` with a `wchar_t` UTF-16 path to create, read,
edit and close multipage TIFF/GIF/ICO files on Windows. Disk caches and spool
files also use Unicode paths. Arguments and ownership match
`FreeImage_OpenMultiBitmap`. Like `FreeImage_LoadU`/`FreeImage_SaveU`, this
entry point returns NULL on non-Windows platforms; use native `char` paths there.

### Build HEIF/AVIF and JPEG XL from bundled sources

```sh
cmake -S . -B build -DBUILD_LIBHEIF=ON -DBUILD_LIBJXL=ON
cmake --build build --config Release --parallel
```

Independent bundled dependencies can build concurrently when the top-level
build uses `--parallel`. Libheif starts after libde265, Kvazaar and (when
enabled) libaom have been installed; JPEG XL similarly waits for its own
prerequisites.

Nested dependency builds automatically use the host's available logical CPU
count, detected with CMake's `ProcessorCount()` (falling back to one job if
unavailable). Set the standard `CMAKE_BUILD_PARALLEL_LEVEL` environment variable
to choose a job count for both the top-level build and nested dependencies:

```sh
CMAKE_BUILD_PARALLEL_LEVEL=8 cmake --build build --config Release
```

On PowerShell, set `$env:CMAKE_BUILD_PARALLEL_LEVEL = 8` before building.
For a separate per-dependency limit, configure with
`-DFREEIMAGE_CODEC_BUILD_JOBS=8`. This cache setting defaults to empty
(automatic); an explicit positive integer takes precedence over the environment.
Clear an existing override with `-DFREEIMAGE_CODEC_BUILD_JOBS=` to restore
automatic detection. The environment is read at build time, so changing it
needs no reconfigure. These settings also work with multi-configuration generators.

The top-level `--parallel` option without a count uses the native build tool's
default; it does not universally mean all CPU threads. A numeric `--parallel N`
on the top-level command does not propagate to nested builds, so use the
environment variable to configure both. Several dependencies can run at once;
these are per-build limits, rather than one shared limit across all processes.

Bundled builds require CMake 3.20 or newer. Both format switches remain OFF by default. When enabled, CMake builds the
checked-in, SHA-256-verified source archives and their required backends;
no network, package manager, external SDK or pkg-config is needed. Sources
are unpacked inside the build directory. HEIF includes libde265/Kvazaar for
HEVC and libaom for AV1 by default; JPEG XL includes Highway, Brotli and
Little CMS. Set `-DFREEIMAGE_BUNDLE_AOM=OFF` to omit libaom and avoid its Perl
build-time requirement. This keeps bundled HEIF decoding/encoding but disables
AVIF support. `FREEIMAGE_BUNDLE_AOM` defaults to ON.
The compiler must support C++20 for bundled libheif and C++17 for bundled
libjxl; these requirements do not change FreeImage's own language standard.
The classic Makefiles still provide unsupported codec stubs.

`USE_SYSTEM_LIBHEIF=ON` and `USE_SYSTEM_LIBJXL=ON` retain external SDK support.
`FREEIMAGE_USE_SYSTEM_LIBS=ON` defaults both switches to ON, like the existing
codec options. Set individual switches explicitly when reusing a build cache.
Bundled dependencies are static, including in shared FreeImage builds.
Installed CMake packages include the private archives needed by static
consumers. See [source versions, licenses and build details](Source/ThirdParty/README.md),
including the LGPL terms for libheif/libde265. Bundled AOM/libde265 use portable
code; JPEG XL retains Highway runtime SIMD. Optional codec build times and
source/archive size are larger than before.

Bundled Kvazaar supports lossy HEIF output; exact RGB lossless HEIF needs
a system libheif encoder supporting RGB/4:4:4 (for example x265).
`HEIF_LOSSLESS` returns FALSE with the bundled HEIF encoder. Lossless AVIF
and JPEG XL remain supported by the bundled backends.
