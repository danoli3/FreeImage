// ==========================================================
// FreeImage metadata table regression test
//
// This file is part of FreeImage 3
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


// Compile the metadata implementation alone so table ordering and sentinels
// can be checked without exposing them through the public library API.
#include "../../Source/Metadata/TagLib.cpp"

#include <cassert>
#include <cstdio>

#ifdef NDEBUG
#error This test requires assertions
#endif

int main() {
	TagLib& tags = TagLib::instance();
	unsigned checked = 0;
	for(int value = TagLib::EXIF_MAIN; value <= TagLib::ANIMATION; ++value) {
		const TagLib::MDMODEL model = static_cast<TagLib::MDMODEL>(value);
		size_t count = 0;
		const TagInfo* table = getTagTable(model, count);
		assert(table != NULL);
		// Apple iOS currently registers an empty table.
		assert(count != 0 || model == TagLib::EXIF_MAKERNOTE_APPLE_IOS);
		if(count == 0) assert(tags.getTagID(model, NULL) == -1);
		assert(table[count].fieldname == NULL);
		assert(table[count].description == NULL);
		for(size_t i = 0; i < count; ++i) {
			assert(table[i].fieldname != NULL);
			assert(i == 0 || table[i - 1].tag < table[i].tag);
			assert(tags.getTagInfo(model, table[i].tag) == &table[i]);
			assert(tags.getTagFieldName(model, table[i].tag, NULL) == table[i].fieldname);
			assert(tags.getTagDescription(model, table[i].tag) == table[i].description);
			// Duplicate names must continue to return the lowest numeric ID.
			size_t first = 0;
			while(strcmp(table[first].fieldname, table[i].fieldname) != 0) {
				++first;
			}
			assert(tags.getTagID(model, table[i].fieldname) == table[first].tag);
		}
		// A linear walk is the oracle for every hit, gap and endpoint.
		size_t next = 0;
		for(unsigned id = 0; id <= 0xFFFF; ++id) {
			const TagInfo* expected = NULL;
			if(next < count && table[next].tag == id) {
				expected = &table[next++];
			}
			assert(tags.getTagInfo(model, static_cast<WORD>(id)) == expected);
			++checked;
		}
		assert(next == count);
		assert(tags.getTagID(model, "") == -1);
		assert(tags.getTagID(model, "__not_a_metadata_tag__") == -1);
		FREE_IMAGE_MDMODEL expectedModel = FIMD_EXIF_MAKERNOTE;
		if(model <= TagLib::EXIF_INTEROP) {
			static const FREE_IMAGE_MDMODEL exifModels[] = {
				FIMD_EXIF_MAIN, FIMD_EXIF_EXIF, FIMD_EXIF_GPS, FIMD_EXIF_INTEROP
			};
			expectedModel = exifModels[value - TagLib::EXIF_MAIN];
		} else if(model == TagLib::IPTC) {
			expectedModel = FIMD_IPTC;
		} else if(model == TagLib::GEOTIFF) {
			expectedModel = FIMD_GEOTIFF;
		} else if(model == TagLib::ANIMATION) {
			expectedModel = FIMD_ANIMATION;
		}
		assert(tags.getFreeImageModel(model) == expectedModel);
	}
	const TagLib::MDMODEL unknown = static_cast<TagLib::MDMODEL>(TagLib::ANIMATION + 1);
	assert(tags.getTagInfo(unknown, 0) == NULL);
	assert(tags.getTagID(unknown, NULL) == -1);
	assert(tags.getTagDescription(unknown, 0) == NULL);
	assert(tags.getFreeImageModel(unknown) == FIMD_NODATA);
	char fallback[16] = {0};
	assert(tags.getTagFieldName(unknown, 0x1234, fallback) == fallback);
	assert(strcmp(fallback, "Tag 0x1234") == 0);
	assert(tags.getTagFieldName(unknown, 0x1234, NULL) == NULL);
	printf("PASS %u lookups, sorted tables, reverse names, models and fallbacks\n", checked);
}
