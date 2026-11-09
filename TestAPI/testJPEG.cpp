// ==========================================================
// FreeImage 3 Test Script
//
// Design and implementation by
// - Hervé Drolon (drolon@infonie.fr)
//
// This file is part of FreeImage 3
//
// Copyright (C) 2003-2026 The FreeImage Project
//
// COVERED CODE IS PROVIDED UNDER THIS LICENSE ON AN "AS IS" BASIS, WITHOUT WARRANTY
// OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, WITHOUT LIMITATION, WARRANTIES
// THAT THE COVERED CODE IS FREE OF DEFECTS, MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE
// OR NON-INFRINGING. THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE COVERED
// CODE IS WITH YOU. SHOULD ANY COVERED CODE PROVE DEFECTIVE IN ANY RESPECT, YOU (NOT
// THE INITIAL DEVELOPER OR ANY OTHER CONTRIBUTOR) ASSUME THE COST OF ANY NECESSARY
// SERVICING, REPAIR OR CORRECTION. THIS DISCLAIMER OF WARRANTY CONSTITUTES AN ESSENTIAL
// PART OF THIS LICENSE. NO USE OF ANY COVERED CODE IS AUTHORIZED HEREUNDER EXCEPT UNDER
// THIS DISCLAIMER.
//
// Use at your own risk!
// ==========================================================


#include "TestSuite.h"

// Local test functions
// ----------------------------------------------------------

void testJPEGTransform(const char *src_file) {
	BOOL bResult;
	BOOL perfect;

	// perfect transformation
	perfect = TRUE;
	bResult = FreeImage_JPEGTransform(src_file, "test.jpg", FIJPEG_OP_FLIP_H, perfect);
	assert(bResult == FALSE);
	bResult = FreeImage_JPEGTransform(src_file, "test.jpg", FIJPEG_OP_FLIP_V, perfect);
	assert(bResult);
	bResult = FreeImage_JPEGTransform(src_file, "test.jpg", FIJPEG_OP_TRANSPOSE, perfect);
	assert(bResult);
	bResult = FreeImage_JPEGTransform(src_file, "test.jpg", FIJPEG_OP_TRANSVERSE, perfect);
	assert(bResult == FALSE);
	bResult = FreeImage_JPEGTransform(src_file, "test.jpg", FIJPEG_OP_ROTATE_90, perfect);
	assert(bResult);
	bResult = FreeImage_JPEGTransform(src_file, "test.jpg", FIJPEG_OP_ROTATE_180, perfect);
	assert(bResult == FALSE);
	bResult = FreeImage_JPEGTransform(src_file, "test.jpg", FIJPEG_OP_ROTATE_270, perfect);
	assert(bResult == FALSE);

	// non perfect transformation
	perfect = FALSE;
	bResult = FreeImage_JPEGTransform(src_file, "test.jpg", FIJPEG_OP_FLIP_H, perfect);
	assert(bResult);
	bResult = FreeImage_JPEGTransform(src_file, "test.jpg", FIJPEG_OP_FLIP_V, perfect);
	assert(bResult);
	bResult = FreeImage_JPEGTransform(src_file, "test.jpg", FIJPEG_OP_TRANSPOSE, perfect);
	assert(bResult);
	bResult = FreeImage_JPEGTransform(src_file, "test.jpg", FIJPEG_OP_TRANSVERSE, perfect);
	assert(bResult);
	bResult = FreeImage_JPEGTransform(src_file, "test.jpg", FIJPEG_OP_ROTATE_90, perfect);
	assert(bResult);
	bResult = FreeImage_JPEGTransform(src_file, "test.jpg", FIJPEG_OP_ROTATE_180, perfect);
	assert(bResult);
	bResult = FreeImage_JPEGTransform(src_file, "test.jpg", FIJPEG_OP_ROTATE_270, perfect);
	assert(bResult);

}

void testJPEGCrop(const char *src_file) {
	int left, top, right, bottom;
	BOOL bResult;

	// perfect transformation
	left = 50; top = 100; right = 359; bottom = 354;
	bResult = FreeImage_JPEGCrop(src_file, "test.jpg", left, top, right, bottom);
	assert(bResult);

	// non perfect transformation (crop rectangle is adjusted automatically)
	left = 50; top = 100; right = 650; bottom = 500;
	bResult = FreeImage_JPEGCrop(src_file, "test.jpg", left, top, right, bottom);
	assert(bResult);
}

void testJPEGTransformCombined(const char *src_file, const char *dst_file) {
	BOOL bResult;
	BOOL perfect;
	int left, top, right, bottom;

	// perfect transformation required
	perfect = TRUE;

	// (optionnal) simulate the transform and get the new rectangle coordinates (adjusted to the iMCU size)
	// then apply the transform

	left = 50; top = 100; right = 359; bottom = 354;
	bResult = FreeImage_JPEGTransformCombined(src_file, NULL, FIJPEG_OP_FLIP_H, &left, &top, &right, &bottom, perfect);
	assert(bResult == FALSE);
	bResult = FreeImage_JPEGTransformCombined(src_file, dst_file, FIJPEG_OP_FLIP_H, &left, &top, &right, &bottom, perfect);
	assert(bResult == FALSE);

	left = 50; top = 100; right = 359; bottom = 354;
	bResult = FreeImage_JPEGTransformCombined(src_file, NULL, FIJPEG_OP_FLIP_V, &left, &top, &right, &bottom, perfect);
	assert(bResult == TRUE);
	bResult = FreeImage_JPEGTransformCombined(src_file, dst_file, FIJPEG_OP_FLIP_V, &left, &top, &right, &bottom, perfect);
	assert(bResult == TRUE);

	left = 50; top = 100; right = 359; bottom = 354;
	bResult = FreeImage_JPEGTransformCombined(src_file, NULL, FIJPEG_OP_TRANSPOSE, &left, &top, &right, &bottom, perfect);
	assert(bResult == TRUE);
	bResult = FreeImage_JPEGTransformCombined(src_file, dst_file, FIJPEG_OP_TRANSPOSE, &left, &top, &right, &bottom, perfect);
	assert(bResult == TRUE);

	left = 50; top = 100; right = 359; bottom = 354;
	bResult = FreeImage_JPEGTransformCombined(src_file, NULL, FIJPEG_OP_TRANSVERSE, &left, &top, &right, &bottom, perfect);
	assert(bResult == FALSE);
	bResult = FreeImage_JPEGTransformCombined(src_file, dst_file, FIJPEG_OP_TRANSVERSE, &left, &top, &right, &bottom, perfect);
	assert(bResult == FALSE);

	left = 50; top = 100; right = 359; bottom = 354;
	bResult = FreeImage_JPEGTransformCombined(src_file, NULL, FIJPEG_OP_ROTATE_90, &left, &top, &right, &bottom, perfect);
	assert(bResult == TRUE);
	bResult = FreeImage_JPEGTransformCombined(src_file, dst_file, FIJPEG_OP_ROTATE_90, &left, &top, &right, &bottom, perfect);
	assert(bResult == TRUE);

	left = 50; top = 100; right = 359; bottom = 354;
	bResult = FreeImage_JPEGTransformCombined(src_file, NULL, FIJPEG_OP_ROTATE_180, &left, &top, &right, &bottom, perfect);
	assert(bResult == FALSE);
	bResult = FreeImage_JPEGTransformCombined(src_file, dst_file, FIJPEG_OP_ROTATE_180, &left, &top, &right, &bottom, perfect);
	assert(bResult == FALSE);

	left = 50; top = 100; right = 359; bottom = 354;
	bResult = FreeImage_JPEGTransformCombined(src_file, NULL, FIJPEG_OP_ROTATE_270, &left, &top, &right, &bottom, perfect);
	assert(bResult == FALSE);
	bResult = FreeImage_JPEGTransformCombined(src_file, dst_file, FIJPEG_OP_ROTATE_270, &left, &top, &right, &bottom, perfect);
	assert(bResult == FALSE);

	// perfect transformation NOT required
	perfect = FALSE;

	// (optionnal) simulate the transform and get the new rectangle coordinates (adjusted to the iMCU size)
	// then apply the transform

	left = 50; top = 100; right = 650; bottom = 500;
	bResult = FreeImage_JPEGTransformCombined(src_file, NULL, FIJPEG_OP_FLIP_H, &left, &top, &right, &bottom, perfect);
	assert(bResult == TRUE);
	bResult = FreeImage_JPEGTransformCombined(src_file, dst_file, FIJPEG_OP_FLIP_H, &left, &top, &right, &bottom, perfect);
	assert(bResult == TRUE);

	left = 50; top = 100; right = 650; bottom = 500;
	bResult = FreeImage_JPEGTransformCombined(src_file, NULL, FIJPEG_OP_FLIP_V, &left, &top, &right, &bottom, perfect);
	assert(bResult == TRUE);
	bResult = FreeImage_JPEGTransformCombined(src_file, dst_file, FIJPEG_OP_FLIP_V, &left, &top, &right, &bottom, perfect);
	assert(bResult == TRUE);

	left = 50; top = 100; right = 650; bottom = 500;
	bResult = FreeImage_JPEGTransformCombined(src_file, NULL, FIJPEG_OP_TRANSPOSE, &left, &top, &right, &bottom, perfect);
	assert(bResult == TRUE);
	bResult = FreeImage_JPEGTransformCombined(src_file, dst_file, FIJPEG_OP_TRANSPOSE, &left, &top, &right, &bottom, perfect);
	assert(bResult == TRUE);

	left = 50; top = 100; right = 650; bottom = 500;
	bResult = FreeImage_JPEGTransformCombined(src_file, NULL, FIJPEG_OP_TRANSVERSE, &left, &top, &right, &bottom, perfect);
	assert(bResult == TRUE);
	bResult = FreeImage_JPEGTransformCombined(src_file, dst_file, FIJPEG_OP_TRANSVERSE, &left, &top, &right, &bottom, perfect);
	assert(bResult == TRUE);

	left = 50; top = 100; right = 650; bottom = 500;
	bResult = FreeImage_JPEGTransformCombined(src_file, NULL, FIJPEG_OP_ROTATE_90, &left, &top, &right, &bottom, perfect);
	assert(bResult == TRUE);
	bResult = FreeImage_JPEGTransformCombined(src_file, dst_file, FIJPEG_OP_ROTATE_90, &left, &top, &right, &bottom, perfect);
	assert(bResult == TRUE);

	left = 50; top = 100; right = 650; bottom = 500;
	bResult = FreeImage_JPEGTransformCombined(src_file, NULL, FIJPEG_OP_ROTATE_180, &left, &top, &right, &bottom, perfect);
	assert(bResult == TRUE);
	bResult = FreeImage_JPEGTransformCombined(src_file, dst_file, FIJPEG_OP_ROTATE_180, &left, &top, &right, &bottom, perfect);
	assert(bResult == TRUE);

	left = 50; top = 100; right = 650; bottom = 500;
	bResult = FreeImage_JPEGTransformCombined(src_file, NULL, FIJPEG_OP_ROTATE_270, &left, &top, &right, &bottom, perfect);
	assert(bResult == TRUE);
	bResult = FreeImage_JPEGTransformCombined(src_file, dst_file, FIJPEG_OP_ROTATE_270, &left, &top, &right, &bottom, perfect);
	assert(bResult == TRUE);
}

void testJPEGSameFile(const char *src_file) {
	BOOL bResult;
	BOOL perfect;

	// perfect transformation
	perfect = TRUE;
	bResult = FreeImage_JPEGTransform(src_file, "test.jpg", FIJPEG_OP_ROTATE_90, perfect);
	assert(bResult);
	bResult = FreeImage_JPEGTransform("test.jpg", "test.jpg", FIJPEG_OP_ROTATE_270, perfect);
	assert(bResult);
}

// Main test function
// ----------------------------------------------------------

// Exercise the codec independently of the optional lossless-transform helpers.
static void testJPEGCodec() {
	FIBITMAP *source = FreeImage_Allocate(37, 23, 24);
	assert(source);
	for(unsigned y = 0; y < 23; ++y) {
		BYTE *row = FreeImage_GetScanLine(source, y);
		for(unsigned x = 0; x < 37; ++x) {
			row[3*x + FI_RGBA_RED] = 210;
			row[3*x + FI_RGBA_GREEN] = 100;
			row[3*x + FI_RGBA_BLUE] = 40;
		}
	}
	const int flags[] = {JPEG_QUALITYSUPERB, JPEG_QUALITYSUPERB | JPEG_PROGRESSIVE};
	for(unsigned i = 0; i < sizeof(flags)/sizeof(flags[0]); ++i) {
		FIMEMORY *stream = FreeImage_OpenMemory();
		assert(stream);
		BOOL saved = FreeImage_SaveToMemory(FIF_JPEG, source, stream, flags[i]);
		assert(saved);
		FreeImage_SeekMemory(stream, 0, SEEK_SET);
		FIBITMAP *decoded = FreeImage_LoadFromMemory(FIF_JPEG, stream);
		assert(decoded);
		assert(FreeImage_GetWidth(decoded) == 37);
		assert(FreeImage_GetHeight(decoded) == 23);
		assert(FreeImage_GetBPP(decoded) == 24);
		BYTE *pixel = FreeImage_GetScanLine(decoded, 11) + 3*18;
		assert(abs((int)pixel[FI_RGBA_RED] - 210) <= 3);
		assert(abs((int)pixel[FI_RGBA_GREEN] - 100) <= 3);
		assert(abs((int)pixel[FI_RGBA_BLUE] - 40) <= 3);
		FreeImage_Unload(decoded);
		FreeImage_CloseMemory(stream);
	}
	FreeImage_Unload(source);
}

void testJPEG() {
	testJPEGCodec();
	const char *src_file = "exif.jpg";
#ifdef FREEIMAGE_SYSTEM_LIBJPEG
	BOOL transformed = FreeImage_JPEGTransform(src_file, "test.jpg", FIJPEG_OP_ROTATE_90, TRUE);
	assert(transformed == FALSE);
	printf("Lossless JPEG transforms unavailable with external libjpeg; codec tests passed.\n");
	return;
#endif

	printf("testJPEG (should throw exceptions) ...\n");

	// lossless transform - both perfect/non perfect
	testJPEGTransform(src_file);

	// cropping - both perfect/non perfect
	testJPEGCrop(src_file);

	// lossless transform + cropping - both perfect/non perfect
	testJPEGTransformCombined(src_file, "test.jpg");

	// using the same file for src & dst is allowed
	testJPEGSameFile(src_file);
}
