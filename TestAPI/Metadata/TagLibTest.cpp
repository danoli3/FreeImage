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

// Compare every decoded string with its source literal, checking each pool's
// bounds and terminators independently of the decoding helper.
static void checkString(const TagTable& table, size_t poolSize, WORD offset, const char* expected) {
	assert(offset < poolSize);
	if(expected) {
		assert(offset != 0);
		assert(memchr(table.strings + offset, 0, poolSize - offset) != NULL);
		assert(strcmp(getTagString(table, offset), expected) == 0);
	} else {
		assert(offset == 0);
		assert(getTagString(table, offset) == NULL);
	}
}

static void checkTagStrings(const TagTable& table, size_t poolSize, const TagEntry& entry,
	WORD id, const char* name, const char* description) {
	assert(entry.tag == id);
	checkString(table, poolSize, entry.fieldnameOffset, name);
	checkString(table, poolSize, entry.descriptionOffset, description);
}

static void checkStringPools() {
#define FI_TAG_TABLE_BEGIN(table) { \
	const TagTable view = makeTagTable(table##Data::entries, table##Data::strings); \
	const size_t poolSize = sizeof(table##Data::strings); \
	const TagEntry* entry = view.entries;
#define FI_TAG_TABLE_END(table) \
	assert(entry == view.entries + view.count); }
#define FI_TAG(id, name) checkTagStrings(view, poolSize, *entry++, id, name, NULL);
#define FI_TAG_DESC(id, name, description) checkTagStrings(view, poolSize, *entry++, id, name, description);
#include "../../Source/Metadata/TagLibData.h"
#undef FI_TAG_TABLE_BEGIN
#undef FI_TAG_TABLE_END
#undef FI_TAG
#undef FI_TAG_DESC
}

int main() {
	checkStringPools();
	assert(checkTagTablesSorted());
	TagLib& tags = TagLib::instance();
	const char* (TagLib::*fieldName)(TagLib::MDMODEL, WORD, char*) = &TagLib::getTagFieldName;
	unsigned checked = 0;
	for(int value = TagLib::EXIF_MAIN; value <= TagLib::ANIMATION; ++value) {
		const TagLib::MDMODEL model = static_cast<TagLib::MDMODEL>(value);
		const TagTable table = getTagTable(model);
		const size_t count = table.count;
		assert(table.entries != NULL);
		assert(table.strings != NULL);
		// Apple iOS currently registers an empty table.
		assert(count != 0 || model == TagLib::EXIF_MAKERNOTE_APPLE_IOS);
		assert(tags.getTagID(model, NULL) == -1);
		assert(table.entries[count].fieldnameOffset == 0);
		assert(table.entries[count].descriptionOffset == 0);
		assert(getTagString(table, table.entries[count].fieldnameOffset) == NULL);
		assert(getTagString(table, table.entries[count].descriptionOffset) == NULL);
		for(size_t i = 0; i < count; ++i) {
			assert(getTagString(table, table.entries[i].fieldnameOffset) != NULL);
			assert(i == 0 || table.entries[i - 1].tag < table.entries[i].tag);
			assert(findTag(table, table.entries[i].tag) == &table.entries[i]);
			assert(tags.getTagFieldName(model, table.entries[i].tag, NULL) == getTagString(table, table.entries[i].fieldnameOffset));
			assert(tags.getTagDescription(model, table.entries[i].tag) == getTagString(table, table.entries[i].descriptionOffset));
			// Duplicate names must continue to return the lowest numeric ID.
			size_t first = 0;
			while(strcmp(getTagString(table, table.entries[first].fieldnameOffset), getTagString(table, table.entries[i].fieldnameOffset)) != 0) {
				++first;
			}
			assert(tags.getTagID(model, getTagString(table, table.entries[i].fieldnameOffset)) == table.entries[first].tag);
		}
		// A linear walk is the oracle for every hit, gap and endpoint.
		size_t next = 0;
		for(unsigned id = 0; id <= 0xFFFF; ++id) {
			const TagEntry* expected = NULL;
			if(next < count && table.entries[next].tag == id) {
				expected = &table.entries[next++];
			}
			assert(findTag(table, static_cast<WORD>(id)) == expected);
			// The combined lookup must assign both results, including on misses.
			const char *description = "unchanged";
			const char *name = tags.getTagFields(model, static_cast<WORD>(id), NULL, &description);
			assert(name == (expected ? getTagString(table, expected->fieldnameOffset) : NULL));
			assert(name == (tags.*fieldName)(model, static_cast<WORD>(id), NULL));
			assert(description == (expected ? getTagString(table, expected->descriptionOffset) : NULL));
			char fallback[16] = "unchanged";
			description = "unchanged";
			name = tags.getTagFields(model, static_cast<WORD>(id), fallback, &description);
			assert(description == tags.getTagDescription(model, static_cast<WORD>(id)));
			if(expected) {
				assert(name == getTagString(table, expected->fieldnameOffset));
				assert(strcmp(fallback, "unchanged") == 0);
			} else {
				char unknownName[16];
				sprintf(unknownName, "Tag 0x%04X", id);
				assert(name == fallback);
				assert(strcmp(fallback, unknownName) == 0);
			}
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
	assert(findTag(getTagTable(unknown), 0) == NULL);
	assert(tags.getTagID(unknown, NULL) == -1);
	assert(tags.getTagDescription(unknown, 0) == NULL);
	assert(tags.getFreeImageModel(unknown) == FIMD_NODATA);
	char fallback[16] = {0};
	assert(tags.getTagFieldName(unknown, 0x1234, fallback) == fallback);
	assert(strcmp(fallback, "Tag 0x1234") == 0);
	assert(tags.getTagFieldName(unknown, 0x1234, NULL) == NULL);
	const char *description = "unchanged";
	assert(tags.getTagFields(unknown, 0x1234, fallback, &description) == fallback);
	assert(strcmp(fallback, "Tag 0x1234") == 0);
	assert(description == NULL);
	description = "unchanged";
	assert(tags.getTagFields(unknown, 0x1234, NULL, &description) == NULL);
	assert(description == NULL);
	printf("PASS %u lookups, sorted tables, string offsets, reverse names, models and fallbacks\n", checked);
}
