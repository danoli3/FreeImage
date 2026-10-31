#ifndef FREEIMAGE_METADATA_STRIP_H
#define FREEIMAGE_METADATA_STRIP_H

#include "../FreeImage.h"

namespace FreeImageInternal {

// Per-operation policy for optional metadata readers, including nested loads.
// Decoder libraries may still parse container fields needed to decode pixels.
bool IsStrippingMetadata();
class MetadataStripScope {
    bool previous_;
    MetadataStripScope(const MetadataStripScope &);
    MetadataStripScope &operator=(const MetadataStripScope &);
public:
    explicit MetadataStripScope(bool strip);
    ~MetadataStripScope();
};

// Owns a separate header/palette/animation model, borrowing the source pixels.
// FreeImage_Unload releases the view without freeing the borrowed pixels.
FIBITMAP *CreateStrippedView(FIBITMAP *source);

}
#endif
