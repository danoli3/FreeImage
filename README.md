What is FreeImage ?
-----------------------------------------------------------------------------
FreeImage is an Open Source library project for developers who would like to support popular graphics image formats like PNG, BMP, JPEG, TIFF and others as needed by today's multimedia applications.
FreeImage is easy to use, fast, multithreading safe, and cross-platform (works with Windows, Linux and Mac OS X).

Thanks to it's ANSI C interface, FreeImage is usable in many languages including C, C++, VB, C#, Delphi, Java and also in common scripting languages such as Perl, Python, PHP, TCL, Lua or Ruby.

The library comes in two versions: a binary DLL distribution that can be linked against any WIN32/WIN64 C/C++ compiler and a source distribution.
Workspace files for Microsoft Visual Studio provided, as well as makefiles for Linux, Mac OS X and other systems.


Original Libary Can be found : https://freeimage.sourceforge.io

## Building this fork

### Simply build it

```bash
cmake . -B cmake-build
cmake --build cmake-build # On Linux, add -j$(nproc) for multicore build
```

### Build and install Debug and Release configuration

```bash
cmake . -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=install_dir
cmake --build cmake-build-debug --config=Debug --target=install # Linux: -j$(nproc)

cmake . -B cmake-build-release -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install_dir
cmake --build cmake-build-release --config=Release --target=install # Linux: -j$(nproc)
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
cmake . -B cmake-build -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTS=ON
cmake --build cmake-build --config=Debug # Linux: -j$(nproc)
ctest --test-dir cmake-build -C Debug
```
