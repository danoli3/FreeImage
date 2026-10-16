// ==========================================================
// Display routines
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

#include "FreeImage.h"
#include "Utilities.h"

#include <stdint.h>


/**
@brief Composite a foreground image against a background color or a background image.

The equation for computing a composited sample value is:<br>
output = alpha * foreground + (1-alpha) * background<br>
where alpha and the input and output sample values are expressed as fractions in the range 0 to 1. 
For colour images, the computation is done separately for R, G, and B samples.

@param fg Foreground image
@param useFileBkg If TRUE and a file background is present, use it as the background color
@param appBkColor If not equal to NULL, and useFileBkg is FALSE, use this color as the background color
@param bg If not equal to NULL and useFileBkg is FALSE and appBkColor is NULL, use this as the background image
@return Returns the composite image if successful, returns NULL otherwise
@see FreeImage_IsTransparent, FreeImage_HasBackgroundColor
*/
FIBITMAP * DLL_CALLCONV
FreeImage_Composite(FIBITMAP *fg, BOOL useFileBkg, RGBQUAD *appBkColor, FIBITMAP *bg) {
	if(!FreeImage_HasPixels(fg)) return NULL;

	int width  = FreeImage_GetWidth(fg);
	int height = FreeImage_GetHeight(fg);
	int bpp    = FreeImage_GetBPP(fg);

	if((bpp != 8) && (bpp != 32))
		return NULL;

	if(bg) {
		int bg_width  = FreeImage_GetWidth(bg);
		int bg_height = FreeImage_GetHeight(bg);
		int bg_bpp    = FreeImage_GetBPP(bg);
		if((bg_width != width) || (bg_height != height) || (bg_bpp != 24))
			return NULL;
	}

	int bytespp = (bpp == 8) ? 1 : 4;

	
	int x, y, c;
	BYTE alpha = 0, not_alpha;
	BYTE index;
	RGBQUAD fgc;	// foreground color
	RGBQUAD bkc;	// background color

	memset(&fgc, 0, sizeof(RGBQUAD));
	memset(&bkc, 0, sizeof(RGBQUAD));

	// allocate the composite image
	FIBITMAP *composite = FreeImage_Allocate(width, height, 24, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);
	if(!composite) return NULL;

	// get the palette
	RGBQUAD *pal = FreeImage_GetPalette(fg);

	// retrieve the alpha table from the foreground image
	BOOL bIsTransparent = FreeImage_IsTransparent(fg);
	BYTE *trns = FreeImage_GetTransparencyTable(fg);

	// retrieve the background color from the foreground image
	BOOL bHasBkColor = FALSE;

	if(useFileBkg && FreeImage_HasBackgroundColor(fg)) {
		FreeImage_GetBackgroundColor(fg, &bkc);
		bHasBkColor = TRUE;
	} else {
		// no file background color
		// use application background color ?
		if(appBkColor) {
			memcpy(&bkc, appBkColor, sizeof(RGBQUAD));
			bHasBkColor = TRUE;
		}
		// use background image ?
		else if(bg) {
			bHasBkColor = FALSE;
		}
	}

	for(y = 0; y < height; y++) {
		// foreground
		BYTE *fg_bits = FreeImage_GetScanLine(fg, y);
		// background
		BYTE *bg_bits = FreeImage_GetScanLine(bg, y);
		// composite image
		BYTE *cp_bits = FreeImage_GetScanLine(composite, y);

		for(x = 0; x < width; x++) {

			// foreground color + alpha

			if(bpp == 8) {
				// get the foreground color
				index = fg_bits[0];
				memcpy(&fgc, &pal[index], sizeof(RGBQUAD));
				// get the alpha
				if(bIsTransparent) {
					alpha = trns[index];
				} else {
					alpha = 255;
				}
			}
			else if(bpp == 32) {
				// get the foreground color
				fgc.rgbBlue  = fg_bits[FI_RGBA_BLUE];
				fgc.rgbGreen = fg_bits[FI_RGBA_GREEN];
				fgc.rgbRed   = fg_bits[FI_RGBA_RED];
				// get the alpha
				alpha = fg_bits[FI_RGBA_ALPHA];
			}

			// background color

			if(!bHasBkColor) {
				if(bg) {
					// get the background color from the background image
					bkc.rgbBlue  = bg_bits[FI_RGBA_BLUE];
					bkc.rgbGreen = bg_bits[FI_RGBA_GREEN];
					bkc.rgbRed   = bg_bits[FI_RGBA_RED];
				}
				else {
					// use a checkerboard pattern
					c = (((y & 0x8) == 0) ^ ((x & 0x8) == 0)) * 192;
					c = c ? c : 255;
					bkc.rgbBlue  = (BYTE)c;
					bkc.rgbGreen = (BYTE)c;
					bkc.rgbRed   = (BYTE)c;
				}
			}

			// composition

			if(alpha == 0) {
				// output = background
				cp_bits[FI_RGBA_BLUE] = bkc.rgbBlue;
				cp_bits[FI_RGBA_GREEN] = bkc.rgbGreen;
				cp_bits[FI_RGBA_RED] = bkc.rgbRed;
			}
			else if(alpha == 255) {
				// output = foreground
				cp_bits[FI_RGBA_BLUE] = fgc.rgbBlue;
				cp_bits[FI_RGBA_GREEN] = fgc.rgbGreen;
				cp_bits[FI_RGBA_RED] = fgc.rgbRed;
			}
			else {
				// output = alpha * foreground + (1-alpha) * background
				not_alpha = (BYTE)~alpha;
				cp_bits[FI_RGBA_BLUE] = (BYTE)((alpha * (WORD)fgc.rgbBlue  + not_alpha * (WORD)bkc.rgbBlue) >> 8);
				cp_bits[FI_RGBA_GREEN] = (BYTE)((alpha * (WORD)fgc.rgbGreen + not_alpha * (WORD)bkc.rgbGreen) >> 8);
				cp_bits[FI_RGBA_RED] = (BYTE)((alpha * (WORD)fgc.rgbRed   + not_alpha * (WORD)bkc.rgbRed) >> 8);
			}

			fg_bits += bytespp;
			bg_bits += 3;
			cp_bits += 3;
		}
	}

	// copy metadata from src to dst
	FreeImage_CloneMetadata(composite, fg);
	
	return composite;	
}

/**
Pre-multiplies a 32-bit image's red-, green- and blue channels with it's alpha channel 
for to be used with e.g. the Windows GDI function AlphaBlend(). 
The transformation changes the red-, green- and blue channels according to the following equation:  
channel(x, y) = channel(x, y) * alpha_channel(x, y) / 255  
@param dib Input/Output dib to be premultiplied
@return Returns TRUE on success, FALSE otherwise (e.g. when the bitdepth of the source dib cannot be handled). 
*/
BOOL DLL_CALLCONV 
FreeImage_PreMultiplyWithAlpha(FIBITMAP *dib) {
	if (!FreeImage_HasPixels(dib)) return FALSE;
	
	if ((FreeImage_GetBPP(dib) != 32) || (FreeImage_GetImageType(dib) != FIT_BITMAP)) {
		return FALSE;
	}

	int width = FreeImage_GetWidth(dib);
	int height = FreeImage_GetHeight(dib);

	for(int y = 0; y < height; y++) {
		BYTE *bits = FreeImage_GetScanLine(dib, y);
		for (int x = 0; x < width; x++, bits += 4) {
			const BYTE alpha = bits[FI_RGBA_ALPHA];
			// slightly faster: care for two special cases
			if(alpha == 0x00) {
				// special case for alpha == 0x00
				// color * 0x00 / 0xFF = 0x00
				bits[FI_RGBA_BLUE] = 0x00;
				bits[FI_RGBA_GREEN] = 0x00;
				bits[FI_RGBA_RED] = 0x00;
			} else if(alpha == 0xFF) {
				// nothing to do for alpha == 0xFF
				// color * 0xFF / 0xFF = color
				continue;
			} else {
				bits[FI_RGBA_BLUE] = (BYTE)( (alpha * (WORD)bits[FI_RGBA_BLUE] + 127) / 255 );
				bits[FI_RGBA_GREEN] = (BYTE)( (alpha * (WORD)bits[FI_RGBA_GREEN] + 127) / 255 );
				bits[FI_RGBA_RED] = (BYTE)( (alpha * (WORD)bits[FI_RGBA_RED] + 127) / 255 );
			}
		}
	}
	return TRUE;
}

/**
Draws a source bitmap over a destination bitmap using the source alpha channel.
The top-left corner of the source is placed at (left, top); pixels outside the
destination are clipped. Supported formats are matching 8-bit grayscale,
24-bit RGB, and 32-bit RGBA bitmaps.
*/
BOOL DLL_CALLCONV
FreeImage_DrawBitmap(FIBITMAP *dst, FIBITMAP *src, FREE_IMAGE_ALPHA_OPERATION alpha, int32_t left, int32_t top) {
	if(!FreeImage_HasPixels(dst) || !FreeImage_HasPixels(src) || alpha != FIAO_SrcAlpha) {
		return FALSE;
	}

	if(FreeImage_GetImageType(dst) != FIT_BITMAP || FreeImage_GetImageType(src) != FIT_BITMAP) {
		return FALSE;
	}

	const FREE_IMAGE_COLOR_TYPE src_color = FreeImage_GetColorType2(src, FALSE);
	const FREE_IMAGE_COLOR_TYPE dst_color = FreeImage_GetColorType2(dst, FALSE);
	if(src_color != dst_color || (src_color != FIC_MINISBLACK && src_color != FIC_RGB && src_color != FIC_RGBALPHA)) {
		return FALSE;
	}

	const unsigned src_bpp = FreeImage_GetBPP(src);
	if(src_bpp != FreeImage_GetBPP(dst) || (src_bpp != 8 && src_bpp != 24 && src_bpp != 32)) {
		return FALSE;
	}
	if(src_bpp == 8 && FreeImage_IsTransparent(src)) {
		return FALSE;
	}

	const unsigned src_width = FreeImage_GetWidth(src);
	const unsigned src_height = FreeImage_GetHeight(src);
	const unsigned dst_width = FreeImage_GetWidth(dst);
	const unsigned dst_height = FreeImage_GetHeight(dst);
	if(src_width > 0x7fffffffU || src_height > 0x7fffffffU || dst_width > 0x7fffffffU || dst_height > 0x7fffffffU) {
		return FALSE;
	}

	const int64_t roi_left = left > 0 ? left : 0;
	const int64_t roi_top = top > 0 ? top : 0;
	const int64_t source_right = static_cast<int64_t>(left) + src_width;
	const int64_t source_bottom = static_cast<int64_t>(top) + src_height;
	const int64_t roi_right = source_right < dst_width ? source_right : dst_width;
	const int64_t roi_bottom = source_bottom < dst_height ? source_bottom : dst_height;
	if(roi_left >= roi_right || roi_top >= roi_bottom) {
		return TRUE;
	}

	const unsigned bytes_per_pixel = src_bpp / 8;
	for(int64_t y = roi_top; y < roi_bottom; ++y) {
		const unsigned dst_y = dst_height - 1 - static_cast<unsigned>(y);
		const unsigned src_y = src_height - 1 - static_cast<unsigned>(y - top);
		BYTE *dst_row = FreeImage_GetScanLine(dst, static_cast<int>(dst_y));
		const BYTE *src_row = FreeImage_GetScanLine(src, static_cast<int>(src_y));
		BYTE *dst_pixel = dst_row + static_cast<size_t>(roi_left) * bytes_per_pixel;
		const BYTE *src_pixel = src_row + static_cast<size_t>(roi_left - left) * bytes_per_pixel;

		for(int64_t x = roi_left; x < roi_right; ++x, dst_pixel += bytes_per_pixel, src_pixel += bytes_per_pixel) {
			const unsigned src_alpha = src_bpp == 32 ? src_pixel[FI_RGBA_ALPHA] : 255;
			if(src_alpha == 255) {
				if(src_bpp == 8) {
					dst_pixel[0] = src_pixel[0];
				} else {
					dst_pixel[FI_RGBA_BLUE] = src_pixel[FI_RGBA_BLUE];
					dst_pixel[FI_RGBA_GREEN] = src_pixel[FI_RGBA_GREEN];
					dst_pixel[FI_RGBA_RED] = src_pixel[FI_RGBA_RED];
				}
			} else if(src_alpha != 0) {
				const unsigned dst_alpha = 255 - src_alpha;
				if(src_bpp == 8) {
					dst_pixel[0] = static_cast<BYTE>((src_alpha * src_pixel[0] + dst_alpha * dst_pixel[0]) / 255);
				} else {
					for(unsigned channel = FI_RGBA_BLUE; channel <= FI_RGBA_RED; ++channel) {
						dst_pixel[channel] = static_cast<BYTE>((src_alpha * src_pixel[channel] + dst_alpha * dst_pixel[channel]) / 255);
					}
				}
			}
		}
	}

	return TRUE;
}

