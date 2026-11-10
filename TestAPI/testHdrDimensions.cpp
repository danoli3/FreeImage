#include "FreeImage.h"

#include <cstdio>
#include <cstring>
#include <vector>

static FIBITMAP *load(const char *dimensions, unsigned pixels, int flags = 0) {
	const char prefix[] = "#?RADIANCE\nFORMAT=32-bit_rle_rgbe\n\n";
	std::vector<BYTE> file(prefix, prefix + sizeof(prefix) - 1);
	file.insert(file.end(), dimensions, dimensions + std::strlen(dimensions));
	for(unsigned i = 0; i < pixels; ++i) {
		file.insert(file.end(), {1, 2, 3, 128});
	}
	FIMEMORY *memory = FreeImage_OpenMemory(file.data(), static_cast<DWORD>(file.size()));
	if(!memory) {
		return NULL;
	}
	FIBITMAP *image = FreeImage_LoadFromMemory(FIF_HDR, memory, flags);
	FreeImage_CloseMemory(memory);
	return image;
}

int main() {
	FreeImage_Initialise();

	FIBITMAP *image = load("-Y -2 +X 1\n", 2);
	bool ok = image == NULL;
	if(image) {
		FreeImage_Unload(image);
	}

	image = load("-Y -2 +X 1\n", 2, FIF_LOAD_NOPIXELS);
	ok = ok && image == NULL;
	if(image) {
		FreeImage_Unload(image);
	}

	image = load("-Y 2 +X 1\n", 2);
	ok = ok && image && FreeImage_GetWidth(image) == 1 &&
		FreeImage_GetHeight(image) == 2 && FreeImage_GetImageType(image) == FIT_RGBF;
	if(image) {
		FreeImage_Unload(image);
	}

	FreeImage_DeInitialise();
	if(!ok) {
		std::fprintf(stderr, "HDR dimension validation test failed\n");
		return 1;
	}
	return 0;
}
