#!/bin/sh
# CMake build directories (cmake-build*/, cmake-build-debug/, build/,
# build-vs2022/, build-vs2026/, etc. - matches .gitignore's build*/ and
# cmake-build-*/ patterns) and their caches.
find . -maxdepth 1 -type d \( -name 'build*' -o -name 'cmake-build*' \) -exec rm -rf {} +
find . -name 'CMakeCache.txt' -exec rm -f {} ";"
find . -name 'CMakeFiles' -type d -exec rm -rf {} +
find . -name 'cmake_install.cmake' -exec rm -f {} ";"
find . -name 'CTestTestfile.cmake' -exec rm -f {} ";"

find . -name '*.pch' -exec rm -f {} ";"
find . -name '*.ncb' -exec rm -f {} ";"
find . -name '*.opt' -exec rm -f {} ";"
find . -name '*.plg' -exec rm -f {} ";"
find . -name '*.obj' -exec rm -f {} ";"
find . -name '*.dll' -exec rm -f {} ";"
find . -name '*.exe' -exec rm -f {} ";"
find . -name '*.bsc' -exec rm -f {} ";"
find . -name '*.bak' -exec rm -f {} ";"
find . -name '*.pdb' -exec rm -f {} ";"
find . -name '*.sql' -exec rm -f {} ";"
find . -name '*.mdb' -exec rm -f {} ";"
find . -name '*.lib' -exec rm -f {} ";"
find . -name '*.exp' -exec rm -f {} ";"
find . -name '*.ilk' -exec rm -f {} ";"
find . -name '*.idb' -exec rm -f {} ";"
find . -name '*.o' -exec rm -f {} ";"
find . -name '*.o-ppc' -exec rm -f {} ";"
find . -name '*.o-i386' -exec rm -f {} ";"
