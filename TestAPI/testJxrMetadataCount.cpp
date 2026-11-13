// JPEG XR metadata count regression (GHSA-gwj3-px9g-x6gf).
//
// ReadBinaryData() computed the buffer size as (U32)(count + 2), so a
// CameraMake entry whose file-supplied count was 0xFFFFFFFE or 0xFFFFFFFF
// wrapped to a 0 or 1 byte allocation while the stream was still asked for
// the full count. The memory read callback then copied the remaining input
// into that buffer. Run under AddressSanitizer to see the overflow; the
// checks below also verify that loading fails or succeeds cleanly.
#include "FreeImage.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

static const BYTE kPoc[119] = {
    0x49, 0x49, 0xbc, 0x01, 0x1e, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x50, 0x00, 0x0c, 0xff,
    0x8d, 0x9c, 0x8d, 0x79, 0x03, 0x8d, 0x8d, 0x01, 0x00, 0x53, 0x00, 0x00, 0x80, 0x7f, 0x04, 0x00,
    0x81, 0xbc, 0x01, 0x1e, 0x8d, 0x8d, 0x01, 0x00, 0x53, 0x00, 0x00, 0x00, 0x01, 0x8d, 0x01, 0x00,
    0x02, 0xf0, 0xfe, 0x02, 0x12, 0x00, 0x00, 0x00, 0x0f, 0x01, 0x02, 0x00, 0xff, 0xff, 0xff, 0xff,
    0x50, 0x00, 0x00, 0x00, 0xfe, 0xff, 0x11, 0x03, 0x8d, 0x8d, 0x57, 0x50, 0x20, 0x22, 0x00, 0x10,
    0x7f, 0x80, 0x00, 0x00, 0xf5, 0x00, 0x0a, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41,
    0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41,
    0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41
};

// Offset of the CameraMake (tag 0x010F, type ASCII) entry's 32-bit count.
static const size_t kCountOffset = 60;

static void check(bool ok, const char *message) {
    if (!ok) {
        std::fprintf(stderr, "JXR metadata count test failed: %s\n", message);
        std::exit(1);
    }
}

static void loadWithCount(unsigned count) {
    BYTE data[sizeof(kPoc)];
    std::memcpy(data, kPoc, sizeof(data));
    for (unsigned i = 0; i < 4; ++i)
        data[kCountOffset + i] = static_cast<BYTE>(count >> (8 * i));

    FIMEMORY *memory = FreeImage_OpenMemory(data, sizeof(data));
    check(memory != NULL, "could not open memory stream");
    check(FreeImage_GetFileTypeFromMemory(memory, 0) == FIF_JXR, "input is not detected as JPEG XR");
    FreeImage_SeekMemory(memory, 0, SEEK_SET);
    FIBITMAP *bitmap = FreeImage_LoadFromMemory(FIF_JXR, memory, 0);
    FreeImage_Unload(bitmap);
    FreeImage_CloseMemory(memory);
}

int main() {
    FreeImage_Initialise(FALSE);
    check(std::memcmp(kPoc + kCountOffset - 4, "\x0f\x01\x02\x00", 4) == 0, "count offset is stale");
    check(std::memcmp(kPoc + kCountOffset, "\xff\xff\xff\xff", 4) == 0, "count is not 0xFFFFFFFF");
    loadWithCount(0xFFFFFFFFu);
    loadWithCount(0xFFFFFFFEu);
    loadWithCount(0xFFFFFFFDu);
    loadWithCount(64u);
    FreeImage_DeInitialise();
    std::puts("JXR metadata count tests passed");
    return 0;
}
