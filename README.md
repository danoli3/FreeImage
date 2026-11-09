What is FreeImage ?
-----------------------------------------------------------------------------
FreeImage is an Open Source library project for developers who would like to support image formats including PNG, BMP, JPEG, TIFF, WebP, JPEG 2000, JPEG XR, OpenEXR, camera RAW, and optional HEIF/HEIC, AVIF and JPEG XL as needed by today's multimedia applications.
FreeImage is easy to use, fast, multithreading safe, and cross-platform (works with Windows, Linux and Mac OS X).

### This GitHub fork

Security and bug fixes on top of upstream FreeImage, plus a CMake build that compiles FreeImage and every bundled library from one `CMakeLists.txt`. This repository is the source tree. [GitHub releases](https://github.com/danoli3/FreeImage/releases) provide source archives and Linux packages; see the [packaging guide](packaging/README.md). Build a static library or a DLL with the commands in [Building this fork](#building-this-fork).

This tree is FreeImage **3.19.31**. `FreeImage_GetVersion()` prints `FREEIMAGE_MAJOR_VERSION`, `FREEIMAGE_MINOR_VERSION`, and `FREEIMAGE_RELEASE_SERIAL` from `Source/FreeImage.h`. CMake takes the version from the latest numeric git tag (`git describe`) and falls back to 3.19.31 when the archive has no `.git` history.

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
| OpenEXR (HDR) | [OpenEXR](https://openexr.com/) | 3.5.3 | on for C++17+ | `BUILD_OPENEXR` |
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
| OpenEXR | Imath 3.2.3, libdeflate 1.26, Zstandard 1.5.7, OpenJPH 0.33.0 |
| HEIF/HEIC/HIF | libde265 1.1.3 (HEVC decoding), Kvazaar 2.3.2 (HEVC encoding) |
| AVIF | libaom 3.15.3 (AV1 decoding/encoding; enabled by default with bundled HEIF) |
| JPEG XL | Highway 1.4.0, Brotli 1.2.1, Little CMS 2.20 |

See [third-party versions](THIRD_PARTY_VERSIONS.md) for version sources and
[optional codec bundles](Source/ThirdParty/README.md) for archive checksums,
licenses and build requirements.

## Original Source Code Upstream
https://sourceforge.net/projects/freeimage
Original library can be found : https://freeimage.sourceforge.io 

The [upstream patch archive](upstream-patches/README.md) lists submitted patches,
what they fix, and fork changes needing submission review. See also the
[SourceForge submission index](SOURCEFORGE_SVN_PATCHES.md).

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

### Using libjpeg-turbo (optional)

The default JPEG backend is bundled IJG libjpeg. To use an installed
[libjpeg-turbo](https://libjpeg-turbo.org/) instead:

```bash
cmake -S . -B build-turbo -DUSE_LIBJPEG_TURBO=ON \
  -DCMAKE_PREFIX_PATH=/path/to/libjpeg-turbo -DBUILD_TESTS=ON \
  -DCMAKE_BUILD_TYPE=Debug
cmake --build build-turbo --config Debug --parallel
ctest --test-dir build-turbo -C Debug --output-on-failure
```

For Homebrew, the prefix is `$(brew --prefix jpeg-turbo)`. If the library is
already on CMake's search path, omit `CMAKE_PREFIX_PATH`. You can also set
`JPEG_INCLUDE_DIR` and `JPEG_LIBRARY` explicitly; select the library that
implements the libjpeg API (`jpeg`), rather than `turbojpeg`.

`USE_LIBJPEG_TURBO` defaults to `OFF`, requires `BUILD_LIBJPEG=ON`, and selects
the external JPEG path even when `USE_SYSTEM_LIBJPEG=OFF`. Configuration fails
if the selected library is not libjpeg-turbo. No dependency is downloaded.
`USE_SYSTEM_LIBJPEG=ON` remains available for either IJG libjpeg or libjpeg-turbo
without requiring a particular implementation.

Normal JPEG loading and saving use the existing FreeImage API. As with other
external JPEG builds, FreeImage's lossless JPEG rotation, flipping and cropping
functions currently return `FALSE`, because the build does not include the
`transupp` helpers. Keep the bundled backend if you need those operations.

### Platform-specific examples

**Linux (gcc) / macOS (clang)**
```bash
cmake -S . -B build -DBUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build -- -j$(nproc)   # macOS: -j$(sysctl -n hw.ncpu)
```

**Windows (MSVC, Visual Studio toolchain)**

Install Visual Studio with **Desktop development with C++**, an x86/x64 MSVC
toolset and a Windows SDK. Add CMake to `PATH`: the presets require CMake
3.21 or newer for VS 2022, or 4.2 or newer for VS 2026. Run the commands below
in PowerShell from the FreeImage source directory. Build Tools supports
command-line builds; opening a solution requires the Visual Studio IDE.

For an x64 Debug build and tests:

```powershell
cmake --preset vs2022 -A x64 -B build-vs2022-x64
cmake --build build-vs2022-x64 --config Debug
ctest --test-dir build-vs2022-x64 -C Debug --output-on-failure
```
See the [Visual Studio and CMake build guide](doc/visual-studio.md) for tool
installation, Win32/ARM64 builds, a combined platform dropdown, DLLs and installation.
See [Generate Visual Studio project files](#generate-visual-studio-project-files) below for opening the solution directly in the IDE.
For unresolved MSVC STL symbols when linking a prebuilt library, see
[Windows linker troubleshooting](#windows-linker-troubleshooting).

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
cmake --preset vs2022 -A x64 -B build-vs2022-dll -DFREEIMAGE_STATIC=OFF -DFREEIMAGE_MSVC_RUNTIME=MT
cmake --build build-vs2022-dll --config Release
ctest --test-dir build-vs2022-dll -C Release --output-on-failure
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

The [complete Visual Studio build guide](doc/visual-studio.md) covers setup,
IDE and command-line builds, output paths, testing and installation.

Choose a preset for your Visual Studio version and how you want to build:

| Build mode | VS 2022 preset | VS 2026 preset | Solution path |
| --- | --- | --- | --- |
| Portable solution with a platform dropdown | `vs2022-all` | `vs2026-all` | `build-vs<year>-all/visualstudio/FreeImage.sln` |
| Single architecture in a fixed build directory | `vs2022` | `vs2026` | `<build-directory>/FreeImage.sln` |

All four presets run on Windows and enable `BUILD_TESTS`. VS 2026 needs CMake
4.2 or newer. Use `--config Debug` or `--config Release` when building;
`CMAKE_BUILD_TYPE` does not select the configuration for a Visual Studio generator.

**Portable solution with Win32, x64, ARM64 and ARM64EC**

```powershell
cmake --preset vs2026-all   # For VS 2022: cmake --preset vs2022-all
```

Open **`build-vs2026-all/visualstudio/FreeImage.sln`** (or
`build-vs2022-all/visualstudio/FreeImage.sln`). Choose **Win32**, **x64**,
**ARM64**, **ARM64EC**, or **All Platforms** in the platform dropdown, select
Debug or Release, then choose **Build > Build Solution**. All Platforms builds
all four architectures. Install the matching ARM64/ARM64EC C++ toolsets before
building those platforms; ARM64EC requires VS 2022 17.3 or newer.

To build all four architectures from the command line:

```powershell
cmake --build build-vs2026-all --config Release --target FreeImageAllPlatforms
```

The projects invoke CMake for the selected architecture, so keep CMake on
`PATH` when building from the IDE. Libraries and DLLs are placed under
`build-vs2026-all/visualstudio/artifacts/<architecture>/<configuration>/`.
The combined frontend supports Visual Studio 2022 and 2026 with bundled
dependencies and native MSVC toolchains.

Choose static libraries, DLLs and optional codecs at configure time. The same
settings apply to every architecture; for example, to generate a DLL solution
with bundled HEIF/AVIF and JPEG XL:

```powershell
cmake --preset vs2026-all -DFREEIMAGE_STATIC=OFF -DBUILD_LIBHEIF=ON -DBUILD_LIBJXL=ON
```

Use `-DFREEIMAGE_STATIC=ON` for a static library. The
`FREEIMAGE_MSVC_RUNTIME` setting described above also applies to every platform.

**Moving or sharing the solution**

The `-all` solution uses relative source and include paths, addressing
[issue #238](https://github.com/danoli3/FreeImage/issues/238). Move or copy the
whole source tree, including `build-vs2026-all/visualstudio/`, while keeping
their relative placement on the same drive. Open the solution at its new
location and build; each machine/location gets fresh architecture caches
under `visualstudio/cache/` automatically.

Ordinary `vs2022`/`vs2026` solutions and CMake caches contain absolute paths.
After moving their source tree, configure a fresh build directory. To share a
relocatable IDE solution, generate it with a `-all` preset.

**Single-architecture solutions**

Use a separate build directory for each architecture:

```powershell
# 32-bit x86
cmake --preset vs2026 -A Win32 -B build-vs2026-win32
cmake --build build-vs2026-win32 --config Release

# 64-bit x86
cmake --preset vs2026 -A x64 -B build-vs2026-x64
cmake --build build-vs2026-x64 --config Release

# Native ARM64
cmake --preset vs2026 -A ARM64 -B build-vs2026-arm64
cmake --build build-vs2026-arm64 --config Release

# ARM64EC
cmake --preset vs2026 -A ARM64EC -B build-vs2026-arm64ec
cmake --build build-vs2026-arm64ec --config Release
```

Open `FreeImage.sln` in the selected build directory. For Visual Studio 2022,
replace `vs2026` with `vs2022` in the preset and directory names. For external
SDK toolchains or system codec libraries, use a separate build directory per
architecture.
Choose a new directory when changing the generator or architecture.

Run tests and install from a single-architecture build directory. For example,
on x64 Windows:

```powershell
ctest --test-dir build-vs2026-x64 -C Release --output-on-failure
cmake --install build-vs2026-x64 --config Release --prefix install-vs2026-x64
```

ARM64 and ARM64EC executables require a compatible Windows ARM64 host. Install
each architecture into a separate prefix; headers go under `include/`,
libraries under `lib/`, and DLLs under `bin/`.

**Ninja with MSVC**

Run `cmake --preset ninja-msvc` from a Developer Command Prompt or Developer
PowerShell, or after `vcvarsall.bat`, with `cl.exe` and Ninja on `PATH`:

```powershell
cmake --preset ninja-msvc
cmake --build build-ninja-msvc
ctest --test-dir build-ninja-msvc --output-on-failure
```

This preset defaults to Debug. Set `-DCMAKE_BUILD_TYPE=Release` when configuring
a Release build with Ninja. See `CMakePresets.json`.

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

<<<<<<< HEAD
## Windows linker troubleshooting

If a prebuilt `FreeImage.lib` or `FreeImageD.lib` fails to link with `LNK2019`
or `LNK2001` for symbols such as `__std_search_1`,
`__std_find_last_trivial_1`, or `__std_find_last_of_trivial_pos_1`, the library
may have been built with newer Microsoft C++ build tools than the application
uses. See [issue #266](https://github.com/danoli3/FreeImage/issues/266).
These are Microsoft STL implementation helpers; both builds saying
"Visual Studio 2022 / v143" does not guarantee matching minor toolset versions.

**Use the same MSVC build-tool version as the prebuilt library, or a newer
compatible version, for the application's final link.** Updating only the
Visual C++ Redistributable does not update the compiler, linker or development
libraries.

1. Save your work and close Visual Studio. Open **Visual Studio Installer**
   and choose **Update** for the Visual Studio or Build Tools installation
   used by the application.
2. Under **Modify**, ensure **Desktop development with C++** and the required
   MSVC tools for your target architecture are installed. For a VS2022/v143
   package, install the latest available **MSVC v143 - VS 2022 C++ x64/x86
   build tools** component (or the corresponding ARM tools). Compare the
   selected toolset version with the package's build log; if that package was
   built with newer tools than your installation supports, use a newer
   supported installation or rebuild FreeImage with your toolset.
3. Reopen the solution. Under **Project > Properties > Configuration
   Properties > General > Platform Toolset**, select the intended installed
   toolset for all relevant configurations and projects. Check for an older
   `VCToolsVersion` pinned in `.vcxproj` or imported `.props` files, and for
   custom library paths that select an older MSVC installation.
4. Open a new **Developer Command Prompt** for that installation if building
   from the command line. Use `where cl`, `where link` and `cl /Bv` to inspect
   the tools selected in that shell; also check the actual project's build
   log. Clean and rebuild the solution. For CMake, configure in a new build
   directory so cached compiler or toolset selections are not reused.

If updating is not possible, rebuild FreeImage and the application with the
same installed toolset, matching architecture, Debug/Release configuration
and runtime-library settings (`/MD`, `/MDd`, `/MT` or `/MTd`). If the toolset
versions already match, enable the linker's `/VERBOSE:LIB` option to check
which C++ libraries are actually selected.

Microsoft documents the [toolset compatibility rules](https://learn.microsoft.com/en-us/cpp/porting/binary-compat-2015-2017#restrictions-on-binary-compatibility)
and [Visual Studio update steps](https://learn.microsoft.com/en-us/visualstudio/install/update-visual-studio).
Libraries built with `/GL` or linked with `/LTCG` require the exact same build
tools, including minor versions, for compilation and the final link.

### Disabling MSVC STL vectorized algorithms

To build FreeImage with `_USE_STD_VECTOR_ALGORITHMS=0`, use the CMake option:

```bat
cmake -S . -B build-no-stl-vector -DFREEIMAGE_MSVC_VECTOR_ALGORITHMS=OFF
cmake --build build-no-stl-vector --config Debug --parallel
```

Or add this before `add_subdirectory()` in a parent `CMakeLists.txt`:

```cmake
set(FREEIMAGE_MSVC_VECTOR_ALGORITHMS OFF CACHE BOOL "Disable MSVC STL vectorized algorithms")
add_subdirectory(path/to/FreeImage)
target_link_libraries(myApp PRIVATE FreeImage)
```

The option defaults to `ON`, leaving the STL's platform defaults unchanged.
When `OFF` in an MSVC-ABI build (including clang-cl), FreeImage defines the
macro for all its C++ sources, including bundled codecs, and propagates it to
C++ targets linking FreeImage through CMake, including the installed
`FreeImage::FreeImage` target. Other toolchains are unaffected.

[Microsoft requires the same macro value across linked translation units](https://learn.microsoft.com/en-us/cpp/standard-library/vectorized-stl-algorithms).
Rebuild the affected libraries and application. Independently built libraries,
`USE_SYSTEM_*` dependencies, and projects linking a raw `.lib` need the same
setting in their own builds. This disables the STL's manual vectorization;
compiler auto-vectorization and codec SIMD remain enabled. It may reduce
algorithm performance and does not make newer-toolset binaries compatible
with older MSVC tools or fix `__std_init_once_link_alternate_names_and_abort`.
The installed static-package toolset compatibility check still applies.

### Building for Visual Studio 2019 / v142

Changing a consuming project's `PlatformToolset` from `v143` to `v142` does
not rebuild its precompiled `.lib` files. The same mismatch can report
`__std_find_trivial_1`, `__std_max_element_4`, or
`__std_init_once_link_alternate_names_and_abort`. In openFrameworks, errors
from both `openframeworksLib_debug.lib` and `FreeImageD.lib` require rebuilding
both libraries and any other newer static dependencies, then the application.
The last symbol is threading support; disabling STL vector algorithms cannot
remove every dependency on a newer runtime.

For a v142 application, build FreeImage and its bundled codecs with v142 in
a fresh build directory. The `vs2019` preset requires Visual Studio 2019;
`vs2022-v142` uses Visual Studio 2022 with the optional **MSVC v142 - VS 2019
C++ x64/x86 build tools** component installed. The IDE hosting the build may
be newer than the compiler selected by the preset. Presets require CMake 3.21+.

```bat
cmake --preset vs2019
cmake --build build-vs2019 --config Debug --parallel
ctest --test-dir build-vs2019 -C Debug --output-on-failure
cmake --install build-vs2019 --config Debug --prefix install-v142-debug
```

With VS2022 as the host, use `cmake --preset vs2022-v142` and the corresponding
`build-vs2022-v142` directory in the remaining commands. Both presets select
x64, v142, C++17, a static FreeImage build and no CMake IPO. Repeat for Release
with a separate installation prefix. Match the application's architecture,
configuration and CRT (`/MDd` for Debug and `/MD` for Release by default).
Do not add `/GL` to distributed static libraries intended for multiple
compatible toolset versions. If using `USE_SYSTEM_*`, those external libraries
also need a compatible toolset; the preset cannot rebuild them.

Installed CMake packages record FreeImage's producer compiler version and
reject an older selected MSVC compiler for a static package during
`find_package(FreeImage CONFIG)`, with rebuild instructions. This check does
not cover raw `.lib` files, external dependencies, custom linker overrides or
clang-cl toolset selection. The Windows v142 CI job builds bundled codecs,
runs the tests, installs the library, and links and runs the installed-package
consumer with both v142 and v143, in Debug and Release.

For binary distributors, make the oldest supported toolset the build baseline,
or publish separate packages labelled with architecture, toolset/compiler
version, configuration and CRT. Keep those packages in distinct directories
and test each package with its oldest supported consumer. An openFrameworks
v142 distribution must ship compatible dependency binaries and rebuild the
core with v142; changing FreeImage alone cannot fix errors from the core.
A v143-only distribution should report that requirement before compiling.
Do not use `/FORCE` to work around missing runtime symbols: it cannot supply
the implementations, and the resulting executable can be invalid. `LNK4075`
about `/INCREMENTAL` is a separate warning caused by that flag combination.
=======
## Apple archive warnings about objects with no symbols

Apple's `libtool` can report `FreeImage.a(...o) has no symbols` when creating
or combining static libraries. Some bundled source files intentionally
compile to empty objects because their platform or optional feature is not
enabled, such as a Windows semaphore implementation on iOS. This diagnostic
alone does not indicate missing FreeImage functions.

The CMake Xcode build suppresses this specific warning on the static
FreeImage target. If another target later repacks `FreeImage.a`, configure
that target too; archive options are not stored in the `.a` file or inherited
by consumers. For another CMake target:

```cmake
if(APPLE AND CMAKE_GENERATOR STREQUAL "Xcode")
    set_property(TARGET MyStaticLibrary APPEND PROPERTY
        STATIC_LIBRARY_OPTIONS -no_warning_for_no_symbols)
endif()
```

For a native Xcode static-library target, set **Other Librarian Flags**
(`OTHER_LIBTOOLFLAGS`) in its build settings or shared `.xcconfig`:

```text
OTHER_LIBTOOLFLAGS = $(inherited) -no_warning_for_no_symbols
```

In openFrameworks, apply this to the core static-library target that combines
FreeImage, Cairo and oF objects. Setting it only on the application target
will not affect a separate nested `xcodebuild` invocation for the core.
This flag belongs to Apple's archive tool, not compiler flags or
`OTHER_LDFLAGS`; do not pass it to `ar` in a Ninja/Makefile build. It suppresses
only empty-object warnings and leaves other archive diagnostics enabled.

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

Set `-DFREEIMAGE_HEIF_EXPORT=OFF` for read-only HEIF/AVIF. The save
functions are not compiled, so `FreeImage_FIFSupportsWriting` returns FALSE
for both formats and saving fails. Bundled builds also skip Kvazaar and build
libaom without its AV1 encoder. With a system SDK only the plugin changes.
`FREEIMAGE_HEIF_EXPORT` defaults to ON. `-DFREEIMAGE_CODEC_EXPORT=OFF` turns
off saving for HEIF/AVIF and JPEG XL together.

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

### Strip auxiliary metadata

Pass `FREEIMAGE_STRIP` together with the usual format flags to discard ICC
profiles, EXIF, XMP, IPTC, comments, custom metadata and embedded thumbnails:

```cpp
FIBITMAP *image = FreeImage_Load(FIF_JPEG, "input.jpg", FREEIMAGE_STRIP);
if (image) {
    FreeImage_Save(FIF_PNG, image, "output.png", FREEIMAGE_STRIP);
    FreeImage_Unload(image);
}
```

The flag also works with memory, handle, 64-bit TIFF and multipage load/save
entry points. Saving leaves the source bitmap and its metadata unchanged.
Call `FreeImage_StripMetadata(image)` to remove metadata from an existing
bitmap in place. Pixel storage, palettes, transparency, resolution and the
CMYK interpretation flag are preserved. Animation timing, looping and disposal
metadata are retained so animated images keep their playback behavior.

Use the named constant: its sign bit avoids the upper-bit field used by JPEG
scaled loading. Combining it with `JPEG_EXIFROTATE` applies EXIF orientation
before discarding the metadata.

Stripping skips optional metadata readers where supported, but codec libraries
still parse container structures needed to decode images. Explicit EXIF
rotation requires parsing orientation metadata. Removing an ICC profile does
not convert pixel colors; color-managed display may therefore change.

Configure with `-DFREEIMAGE_METADATA_STRIP=OFF` (or compile with
`-DFREEIMAGE_METADATA_STRIP=0` outside CMake) to build without stripping.
`FREEIMAGE_STRIP` is then ignored and `FreeImage_StripMetadata()` returns
FALSE; both stay declared, so the ABI does not change.

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
Set `-DFREEIMAGE_JXL_EXPORT=OFF` for read-only JPEG XL, or
`-DFREEIMAGE_CODEC_EXPORT=OFF` for read-only HEIF/AVIF and JPEG XL. The save
function is not compiled, so `FreeImage_FIFSupportsWriting(FIF_JPEGXL)`
returns FALSE and saving fails. libjxl is still built in full, but nothing
references its encoder, so the linker leaves it out of FreeImage.
`FREEIMAGE_JXL_EXPORT` defaults to ON.

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

### Large-file TIFF input

`FreeImage_Load64(FIF_TIFF, filename, flags)` reads a single TIFF image using
64-bit file offsets, including BigTIFF files with directories or pixel strips
beyond 2 GiB or 4 GiB. `FIF_LOAD_NOPIXELS`, Exif metadata and embedded
thumbnails use the same path.

For a custom seekable stream, use `FreeImage_LoadFromHandle64` with a
`FreeImageIO64` callback table. Read, seek and tell callbacks are required;
write may be NULL for read-only input. Read/write return complete element
counts, as `fread`/`fwrite` do, and each transfer must fit in `size_t`. Seek
offsets and tell positions are signed 64-bit values; seek returns zero on
success and tell returns a negative value on failure. The caller owns the
handle and closes it after loading.

These entry points currently support TIFF only and return NULL for other
formats. The existing `FreeImageIO` and external `Plugin` layouts are
unchanged. The new path does not extend saving, multipage operations or the
DWORD-sized memory-stream API. File-position support does not increase the
address space available for decoded pixel buffers.

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

Nested MSVC dependency builds use English compiler and MSBuild messages by
default to avoid garbled localized output when build tools pipe their logs
([issue #236](https://github.com/danoli3/FreeImage/issues/236)). This applies
to both dependency configuration and compilation, including Ninja builds.
The language overrides are confined to those child processes. To retain
localized dependency messages, configure with
`-DFREEIMAGE_CODEC_ENGLISH_MESSAGES=OFF`.

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
