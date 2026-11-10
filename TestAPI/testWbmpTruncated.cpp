#include "FreeImage.h"

#include <cstdio>

static FIBITMAP *load(BYTE *data, DWORD size) {
	FIMEMORY *memory = FreeImage_OpenMemory(data, size);
	if(!memory) {
		return NULL;
	}
	FIBITMAP *image = FreeImage_LoadFromMemory(FIF_WBMP, memory, 0);
	FreeImage_CloseMemory(memory);
	return image;
}

int main() {
	FreeImage_Initialise();

	BYTE missing_fixed_header[] = {0x00};
	BYTE missing_extension[] = {0x00, 0x80};
	BYTE valid[] = {0x00, 0x80, 0xa0, 0x20, 0x01, 0x01, 0x80};

	FIBITMAP *image = load(missing_fixed_header, sizeof(missing_fixed_header));
	bool ok = image == NULL;
	if(image) {
		FreeImage_Unload(image);
	}
	image = load(missing_extension, sizeof(missing_extension));
	ok = ok && image == NULL;
	if(image) {
		FreeImage_Unload(image);
	}
	image = load(valid, sizeof(valid));
	ok = ok && image && FreeImage_GetWidth(image) == 1 &&
		FreeImage_GetHeight(image) == 1 && FreeImage_GetBPP(image) == 1;
	if(image) {
		FreeImage_Unload(image);
	}

	FreeImage_DeInitialise();
	if(!ok) {
		std::fprintf(stderr, "WBMP truncation test failed\n");
		return 1;
	}
	return 0;
}
