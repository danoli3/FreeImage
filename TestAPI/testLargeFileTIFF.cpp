#include "FreeImage.h"
#include <cstdio>
#include <cstdint>
#include <cstring>

#ifdef _WIN32
#include <io.h>
#define fi_fseeko _fseeki64
#else
#include <sys/types.h>
#define fi_fseeko fseeko
#endif

static void put16(FILE *file, uint16_t value) {
    unsigned char bytes[2] = { (unsigned char)value, (unsigned char)(value >> 8) };
    fwrite(bytes, 1, sizeof(bytes), file);
}

static void put64(FILE *file, uint64_t value) {
    unsigned char bytes[8];
    for (unsigned i = 0; i < 8; ++i) bytes[i] = (unsigned char)(value >> (8 * i));
    fwrite(bytes, 1, sizeof(bytes), file);
}

static void entry(FILE *file, uint16_t tag, uint16_t type, uint64_t count, uint64_t value) {
    put16(file, tag);
    put16(file, type);
    put64(file, count);
    put64(file, value);
}

int main() {
    const char *path = "large-file-tiff-test.tif";
    FILE *file = fopen(path, "wb");
    if (!file) return 1;

    // Minimal 1x1 BigTIFF with its single pixel stored beyond 2 GiB.
    fwrite("II", 1, 2, file);
    put16(file, 43);
    put16(file, 8);
    put16(file, 0);
    put64(file, 16);
    put64(file, 10);
    entry(file, 256, 4, 1, 1);                // ImageWidth
    entry(file, 257, 4, 1, 1);                // ImageLength
    entry(file, 258, 3, 3, 0x000800080008ULL);// BitsPerSample (8,8,8)
    entry(file, 259, 3, 1, 1);                // Compression
    entry(file, 262, 3, 1, 2);                // Photometric RGB
    entry(file, 273, 16, 1, 0x80000010ULL);   // StripOffsets
    entry(file, 277, 3, 1, 3);                // SamplesPerPixel
    entry(file, 278, 4, 1, 1);                // RowsPerStrip
    entry(file, 279, 16, 1, 3);               // StripByteCounts
    entry(file, 284, 3, 1, 1);                // PlanarConfiguration
    put64(file, 0);                           // next IFD
    if (fi_fseeko(file, (int64_t)0x80000010ULL, SEEK_SET) != 0) {
        fclose(file);
        remove(path);
        return 2;
    }
    const unsigned char pixel[3] = { 0x12, 0x34, 0x56 };
    if (fwrite(pixel, 1, sizeof(pixel), file) != sizeof(pixel)) {
        fclose(file);
        remove(path);
        return 3;
    }
    fclose(file);

    FreeImage_Initialise(FALSE);
    FIBITMAP *dib = FreeImage_Load64(FIF_TIFF, path, 0);
    RGBQUAD color = {};
    const bool passed = dib && FreeImage_GetWidth(dib) == 1 && FreeImage_GetHeight(dib) == 1 &&
        FreeImage_GetPixelColor(dib, 0, 0, &color) && color.rgbRed == 0x12 &&
        color.rgbGreen == 0x34 && color.rgbBlue == 0x56;
    if (dib) FreeImage_Unload(dib);
    FreeImage_DeInitialise();
    remove(path);
    return passed ? 0 : 4;
}
