#include "FreeImage.h"

#include <cstdio>
#include <vector>

typedef std::vector<BYTE> Bytes;

static void word(Bytes &file, unsigned value) {
	file.push_back(static_cast<BYTE>(value >> 8));
	file.push_back(static_cast<BYTE>(value));
}

static void dword(Bytes &file, unsigned value) {
	word(file, value >> 16);
	word(file, value);
}

static bool rejected(Bytes &file) {
	FIMEMORY *memory = FreeImage_OpenMemory(file.data(), static_cast<DWORD>(file.size()));
	if(!memory) {
		return false;
	}
	FIBITMAP *image = FreeImage_LoadFromMemory(FIF_PICT, memory, 0);
	FreeImage_CloseMemory(memory);
	if(image) {
		FreeImage_Unload(image);
		return false;
	}
	return true;
}

int main() {
	FreeImage_Initialise();

	Bytes header_eof(512, 0);
	bool ok = rejected(header_eof);

	Bytes quicktime_eof(512, 0);
	word(quicktime_eof, 0);
	quicktime_eof.insert(quicktime_eof.end(), 8, 0);
	quicktime_eof.insert(quicktime_eof.end(), {0x11, 0x02, 0xff, 0x00});
	word(quicktime_eof, 0x8200);
	dword(quicktime_eof, 4);
	ok = rejected(quicktime_eof) && ok;

	FreeImage_DeInitialise();
	if(!ok) {
		std::fprintf(stderr, "PICT truncation test failed\n");
		return 1;
	}
	return 0;
}
