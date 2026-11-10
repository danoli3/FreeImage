#include "TestSuite.h"
#include <cstring>

// WEBP_SHARPYUV must produce a valid lossy WebP that differs from the default
// conversion on a chroma-edge image.
void testWebPSharpYUV() {
	if (!FreeImage_FIFSupportsWriting(FIF_WEBP)) return;
	FIBITMAP *dib = FreeImage_Allocate(64, 64, 24);
	for (unsigned y = 0; y < 64; y++) {
		BYTE *p = FreeImage_GetScanLine(dib, y);
		for (unsigned x = 0; x < 64; x++, p += 3) {
			bool red = ((x / 3) & 1) != 0;
			p[FI_RGBA_RED] = red ? 255 : 0;
			p[FI_RGBA_GREEN] = 0;
			p[FI_RGBA_BLUE] = red ? 0 : 255;
		}
	}
	FIMEMORY *plain = FreeImage_OpenMemory(), *sharp = FreeImage_OpenMemory();
	assert(FreeImage_SaveToMemory(FIF_WEBP, dib, plain, 80));
	assert(FreeImage_SaveToMemory(FIF_WEBP, dib, sharp, 80 | WEBP_SHARPYUV));
	FreeImage_SeekMemory(sharp, 0, SEEK_SET);
	FIBITMAP *back = FreeImage_LoadFromMemory(FIF_WEBP, sharp, 0);
	assert(back && FreeImage_GetWidth(back) == 64 && FreeImage_GetHeight(back) == 64);
	DWORD a = 0, b = 0;
	BYTE *pa = NULL, *pb = NULL;
	FreeImage_AcquireMemory(plain, &pa, &a);
	FreeImage_AcquireMemory(sharp, &pb, &b);
	assert(a != b || memcmp(pa, pb, a) != 0);
	FreeImage_Unload(back);
	FreeImage_Unload(dib);
	FreeImage_CloseMemory(plain);
	FreeImage_CloseMemory(sharp);
}
