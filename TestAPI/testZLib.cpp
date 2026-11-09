// Exercise the bundled zlib through its public API and FreeImage's wrappers.
#include "FreeImage.h"
#include "../Source/ZLib/zlib.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <vector>

static void check(bool condition, const char *message) {
    if (!condition)
        throw std::runtime_error(message);
}

static void invalidWindowReset() {
    std::vector<Bytef> source(65536);
    for (size_t i = 0; i < source.size(); ++i)
        source[i] = static_cast<Bytef>(i * 37);
    std::vector<Bytef> compressed(compressBound(static_cast<uLong>(source.size())));
    uLongf compressedSize = static_cast<uLongf>(compressed.size());
    check(compress(compressed.data(), &compressedSize, source.data(),
                   static_cast<uLong>(source.size())) == Z_OK, "compress failed");

    z_stream stream = {};
    check(inflateInit2(&stream, 0) == Z_OK, "inflateInit2 failed");
    // CINFO=15 requests a forbidden 2^23-byte window. FCHECK is valid.
    Bytef invalid[] = {0xf8, 0x00};
    invalid[1] = static_cast<Bytef>((31 - (0xf800 % 31)) % 31);
    Bytef outputByte = 0;
    stream.next_in = invalid;
    stream.avail_in = sizeof(invalid);
    stream.next_out = &outputByte;
    stream.avail_out = 1;
    check(inflate(&stream, Z_NO_FLUSH) == Z_DATA_ERROR, "Invalid window was accepted");
    check(inflateReset(&stream) == Z_OK, "inflateReset failed");

    std::vector<Bytef> restored(source.size());
    stream.next_in = compressed.data();
    stream.avail_in = static_cast<uInt>(compressedSize);
    stream.next_out = restored.data();
    // Stop one byte short of the end: zlib only retains the sliding window
    // while the stream is still in progress.
    const size_t produced = restored.size() - 1;
    stream.avail_out = static_cast<uInt>(produced);
    check(inflate(&stream, Z_NO_FLUSH) == Z_OK, "Reset stream did not decode");
    check(std::equal(restored.begin(), restored.begin() + produced, source.begin()),
          "Reset stream corrupted output");

    // Before 1.4, the rejected header's window size survived reset and this
    // copied 64 KiB into the API's documented 32 KiB dictionary buffer.
    std::vector<Bytef> dictionary(32768);
    uInt dictionarySize = 0;
    check(inflateGetDictionary(&stream, dictionary.data(), &dictionarySize) == Z_OK,
          "inflateGetDictionary failed");
    check(dictionarySize == dictionary.size(), "Dictionary exceeds the valid window");
    check(std::memcmp(dictionary.data(), source.data() + produced - dictionarySize,
                      dictionarySize) == 0, "Dictionary contents are incorrect");
    check(inflateEnd(&stream) == Z_OK, "inflateEnd failed");
}

static void freeImageRoundTrips() {
    std::vector<BYTE> source(65536);
    for (size_t i = 0; i < source.size(); ++i)
        source[i] = static_cast<BYTE>(i * 37);
    std::vector<BYTE> compressed(source.size() * 2);
    std::vector<BYTE> restored(source.size());
    const DWORD sourceSize = static_cast<DWORD>(source.size());
    const DWORD capacity = static_cast<DWORD>(compressed.size());
    DWORD size = FreeImage_ZLibCompress(compressed.data(), capacity, source.data(), sourceSize);
    check(size != 0, "FreeImage_ZLibCompress failed");
    check(FreeImage_ZLibUncompress(restored.data(), sourceSize, compressed.data(), size) == sourceSize,
          "FreeImage_ZLibUncompress failed");
    check(restored == source, "Zlib round trip corrupted output");
    size = FreeImage_ZLibGZip(compressed.data(), capacity, source.data(), sourceSize);
    check(size != 0, "FreeImage_ZLibGZip failed");
    std::fill(restored.begin(), restored.end(), 0);
    check(FreeImage_ZLibGUnzip(restored.data(), sourceSize, compressed.data(), size) == sourceSize,
          "FreeImage_ZLibGUnzip failed");
    check(restored == source, "Gzip round trip corrupted output");
}

int main() {
    try {
        check(std::strcmp(zlibVersion(), ZLIB_VERSION) == 0, "Bundled zlib header/library mismatch");
        invalidWindowReset();
        freeImageRoundTrips();
        std::printf("Bundled zlib %s: window-reset regression and compression round trips passed\n",
                    zlibVersion());
        return 0;
    } catch (const std::exception &error) {
        std::fprintf(stderr, "%s\n", error.what());
        return 1;
    }
}
