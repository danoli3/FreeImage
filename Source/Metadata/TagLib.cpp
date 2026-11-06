// ==========================================================
// Tag library
//
// Design and implementation by
// - Hervé Drolon <drolon@infonie.fr>
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

// ==========================================================
// Implementation notes : 
// ----------------------
// The tag info tables declared in this file should probably 
// be loaded from an XML file. 
// This would allow internationalization features and also 
// more extensibility. 
// Maybe in a future release ? 
// ==========================================================

#ifdef _MSC_VER 
#pragma warning (disable : 4786) // identifier was truncated to 'number' characters
#endif

#include <stddef.h>

#include "FreeImage.h"
#include "Utilities.h"
#include "FreeImageTag.h"

/**
 HOW-TO : add a new metadata table
 --------------------------------------------------------------------------
 1) add a table identifier in the TagLib class definition (see enum MDMODEL)
 2) add its rows to TagLibData.h (the sentinel is supplied automatically)
 3) keep the rows sorted by tag ID and add the table to getTagTable
 4) provide a conversion in TagLib::getFreeImageModel
*/

namespace {

// These implementation-only descriptors contain no pointers.
struct TagEntry {
	WORD tag;
	WORD fieldnameOffset;
	WORD descriptionOffset;
};

// Character arrays give offsetof a constant layout in C++98. Each model has
// its own pool so the linker can discard unused tables and their strings.
#define FI_TAG_TABLE_BEGIN(table) \
	namespace table##Data { struct Strings { char nullString[1];
#define FI_TAG_TABLE_END(table) \
	}; typedef char TagStringOffsetsFitInWord[(sizeof(Strings) <= 0x10000) ? 1 : -1]; }
#define FI_TAG(id, name) char name_##id[sizeof(name)];
#define FI_TAG_DESC(id, name, description) \
	FI_TAG(id, name) char description_##id[sizeof(description)];
#include "TagLibData.h"
#undef FI_TAG_TABLE_BEGIN
#undef FI_TAG_TABLE_END
#undef FI_TAG
#undef FI_TAG_DESC

#define FI_TAG_TABLE_BEGIN(table) \
	namespace table##Data { static const Strings strings = { "",
#define FI_TAG_TABLE_END(table) }; }
#define FI_TAG(id, name) name,
#define FI_TAG_DESC(id, name, description) name, description,
#include "TagLibData.h"
#undef FI_TAG_TABLE_BEGIN
#undef FI_TAG_TABLE_END
#undef FI_TAG
#undef FI_TAG_DESC

#define FI_TAG_TABLE_BEGIN(table) \
	namespace table##Data { static const TagEntry entries[] = {
#define FI_TAG_TABLE_END(table) { 0, 0, 0 } }; }
#define FI_TAG(id, name) { id, offsetof(Strings, name_##id), 0 },
#define FI_TAG_DESC(id, name, description) \
	{ id, offsetof(Strings, name_##id), offsetof(Strings, description_##id) },
#include "TagLibData.h"
#undef FI_TAG_TABLE_BEGIN
#undef FI_TAG_TABLE_END
#undef FI_TAG
#undef FI_TAG_DESC

// This view is returned by value, not stored in a pointer-bearing registry.
struct TagTable {
	const TagEntry* entries;
	size_t count;
	const char* strings;
};

template<size_t N, typename Strings>
TagTable makeTagTable(const TagEntry (&entries)[N], const Strings& strings) {
	const TagTable table = { entries, N - 1, reinterpret_cast<const char*>(&strings) };
	return table;
}

// Table entries are sorted by tag ID. The final sentinel is not part of the lookup.
TagTable getTagTable(TagLib::MDMODEL model) {
	switch(model) {
		case TagLib::EXIF_MAIN:
		case TagLib::EXIF_EXIF:
			return makeTagTable(exif_exif_tag_tableData::entries, exif_exif_tag_tableData::strings);
		case TagLib::EXIF_GPS:
			return makeTagTable(exif_gps_tag_tableData::entries, exif_gps_tag_tableData::strings);
		case TagLib::EXIF_INTEROP:
			return makeTagTable(exif_interop_tag_tableData::entries, exif_interop_tag_tableData::strings);
		case TagLib::EXIF_MAKERNOTE_CANON:
			return makeTagTable(exif_canon_tag_tableData::entries, exif_canon_tag_tableData::strings);
		case TagLib::EXIF_MAKERNOTE_CASIOTYPE1:
			return makeTagTable(exif_casio_type1_tag_tableData::entries, exif_casio_type1_tag_tableData::strings);
		case TagLib::EXIF_MAKERNOTE_CASIOTYPE2:
			return makeTagTable(exif_casio_type2_tag_tableData::entries, exif_casio_type2_tag_tableData::strings);
		case TagLib::EXIF_MAKERNOTE_FUJIFILM:
			return makeTagTable(exif_fujifilm_tag_tableData::entries, exif_fujifilm_tag_tableData::strings);
		case TagLib::EXIF_MAKERNOTE_KYOCERA:
			return makeTagTable(exif_kyocera_tag_tableData::entries, exif_kyocera_tag_tableData::strings);
		case TagLib::EXIF_MAKERNOTE_MINOLTA:
			return makeTagTable(exif_minolta_tag_tableData::entries, exif_minolta_tag_tableData::strings);
		case TagLib::EXIF_MAKERNOTE_NIKONTYPE1:
			return makeTagTable(exif_nikon_type1_tag_tableData::entries, exif_nikon_type1_tag_tableData::strings);
		case TagLib::EXIF_MAKERNOTE_NIKONTYPE2:
			return makeTagTable(exif_nikon_type2_tag_tableData::entries, exif_nikon_type2_tag_tableData::strings);
		case TagLib::EXIF_MAKERNOTE_NIKONTYPE3:
			return makeTagTable(exif_nikon_type3_tag_tableData::entries, exif_nikon_type3_tag_tableData::strings);
		case TagLib::EXIF_MAKERNOTE_OLYMPUSTYPE1:
			return makeTagTable(exif_olympus_type1_tag_tableData::entries, exif_olympus_type1_tag_tableData::strings);
		case TagLib::EXIF_MAKERNOTE_PANASONIC:
			return makeTagTable(exif_panasonic_tag_tableData::entries, exif_panasonic_tag_tableData::strings);
		case TagLib::EXIF_MAKERNOTE_ASAHI:
			return makeTagTable(exif_asahi_tag_tableData::entries, exif_asahi_tag_tableData::strings);
		case TagLib::EXIF_MAKERNOTE_PENTAX:
			return makeTagTable(exif_pentax_tag_tableData::entries, exif_pentax_tag_tableData::strings);
		case TagLib::EXIF_MAKERNOTE_SONY:
			return makeTagTable(exif_sony_tag_tableData::entries, exif_sony_tag_tableData::strings);
		case TagLib::EXIF_MAKERNOTE_SIGMA_SD1:
			return makeTagTable(exif_sigma_sd1_tag_tableData::entries, exif_sigma_sd1_tag_tableData::strings);
		case TagLib::EXIF_MAKERNOTE_SIGMA_FOVEON:
			return makeTagTable(exif_sigma_foveon_tag_tableData::entries, exif_sigma_foveon_tag_tableData::strings);
		case TagLib::EXIF_MAKERNOTE_APPLE_IOS:
			return makeTagTable(exif_apple_ios_tag_tableData::entries, exif_apple_ios_tag_tableData::strings);
		case TagLib::IPTC:
			return makeTagTable(iptc_tag_tableData::entries, iptc_tag_tableData::strings);
		case TagLib::GEOTIFF:
			return makeTagTable(geotiff_tag_tableData::entries, geotiff_tag_tableData::strings);
		case TagLib::ANIMATION:
			return makeTagTable(animation_tag_tableData::entries, animation_tag_tableData::strings);
	}
	const TagTable empty = { NULL, 0, NULL };
	return empty;
}

// Offset zero represents NULL, not an empty string.
const char* getTagString(const TagTable& table, WORD offset) {
	return offset ? table.strings + offset : NULL;
}

const TagEntry* findTag(const TagTable& table, WORD tagID) {
	size_t count = table.count;
	const TagEntry* entries = table.entries;
	while(count != 0) {
		const size_t step = count / 2;
		const TagEntry* entry = entries + step;
		if(entry->tag < tagID) {
			entries = entry + 1;
			count -= step + 1;
		} else if(entry->tag > tagID) {
			count = step;
		} else {
			return entry;
		}
	}
	return NULL;
}

} // namespace



TagLib&
TagLib::instance() {
	static TagLib s;
	return s;
}

const char*
TagLib::getTagFieldName(MDMODEL md_model, WORD tagID, char *defaultKey) {
	return getTagFields(md_model, tagID, defaultKey, NULL);
}

const char*
TagLib::getTagFields(MDMODEL md_model, WORD tagID, char *defaultKey, const char **description) {

	const TagTable table = getTagTable(md_model);
	const TagEntry *info = findTag(table, tagID);
	if(description != NULL) {
		*description = info ? getTagString(table, info->descriptionOffset) : NULL;
	}
	if(NULL == info) {
		if(defaultKey != NULL) {
			sprintf(defaultKey, "Tag 0x%04X", tagID);
			return &defaultKey[0];
		} else {
			return NULL;
		}
	}

	return getTagString(table, info->fieldnameOffset);
}

const char* 
TagLib::getTagDescription(MDMODEL md_model, WORD tagID) {

	const TagTable table = getTagTable(md_model);
	const TagEntry *info = findTag(table, tagID);
	if(info) {
		return getTagString(table, info->descriptionOffset);
	}

	return NULL;
}

int TagLib::getTagID(MDMODEL md_model, const char *key) {
	const TagTable table = getTagTable(md_model);
	for(size_t i = 0; i < table.count; ++i) {
		if(strcmp(getTagString(table, table.entries[i].fieldnameOffset), key) == 0) {
			// Keep the first (lowest tag ID) match when field names repeat.
			return (int)table.entries[i].tag;
		}
	}
	return -1;
}

FREE_IMAGE_MDMODEL 
TagLib::getFreeImageModel(MDMODEL model) {
	switch(model) {
		case EXIF_MAIN:
			return FIMD_EXIF_MAIN;

		case EXIF_EXIF:
			return FIMD_EXIF_EXIF;

		case EXIF_GPS: 
			return FIMD_EXIF_GPS;

		case EXIF_INTEROP:
			return FIMD_EXIF_INTEROP;

		case EXIF_MAKERNOTE_CANON:
		case EXIF_MAKERNOTE_CASIOTYPE1:
		case EXIF_MAKERNOTE_CASIOTYPE2:
		case EXIF_MAKERNOTE_FUJIFILM:
		case EXIF_MAKERNOTE_KYOCERA:
		case EXIF_MAKERNOTE_MINOLTA:
		case EXIF_MAKERNOTE_NIKONTYPE1:
		case EXIF_MAKERNOTE_NIKONTYPE2:
		case EXIF_MAKERNOTE_NIKONTYPE3:
		case EXIF_MAKERNOTE_OLYMPUSTYPE1:
		case EXIF_MAKERNOTE_PANASONIC:
		case EXIF_MAKERNOTE_ASAHI:
		case EXIF_MAKERNOTE_PENTAX:
		case EXIF_MAKERNOTE_SONY:
		case EXIF_MAKERNOTE_SIGMA_SD1:
		case EXIF_MAKERNOTE_SIGMA_FOVEON:
		case EXIF_MAKERNOTE_APPLE_IOS:
			return FIMD_EXIF_MAKERNOTE;

		case IPTC:
			return FIMD_IPTC;

		case GEOTIFF:
			return FIMD_GEOTIFF;

		case ANIMATION:
			return FIMD_ANIMATION;
		default:
			break;
	}

	return FIMD_NODATA;
}

