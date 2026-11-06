Run the metadata regression from the FreeImage source root:

    make -C TestAPI/Metadata check

Run this after changing Source/Metadata/TagLibData.h or TagLib.cpp. CMake builds
with BUILD_TESTS=ON also register this standalone C++98 test as TagLibTest:

    ctest --test-dir build -R TagLibTest --output-on-failure

Assertions remain enabled for this test even in Release builds.

Keep tag IDs sorted and unique within each table. IDs must be single tokens,
normally hexadecimal literals (0x0203, not 0x0200 + 3). FI_TAG has no description;
FI_TAG_DESC adds one. Names and descriptions stay beside their tag ID. Offsets,
string lengths and sentinels are supplied automatically by the compiler.

Each model has its own static string pool, limited to 65,536 bytes by a compile-
time check. Do not bypass that check: a larger pool requires a wider offset type
or splitting the table. Adding a model also requires the model enum, dispatch
and public-model conversion described in TagLib.cpp.

The test checks every source string against its decoded value, each pool's
bounds and terminators, all 16-bit IDs, sorted tables, duplicate-name behavior,
stable returned pointers, unknown models and fallback strings.
