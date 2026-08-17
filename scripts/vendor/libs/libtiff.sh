VENDOR_DIR="Source/LibTIFF4"
SOURCE_SUBDIRS="."
ROOT_FILES=""
CMAKE_VAR="FreeImage_LIBTIFF_SRCS"
URL_TEMPLATE="https://gitlab.com/libtiff/libtiff/-/archive/v{version}/libtiff-v{version}.tar.gz"
# Flat layout - merge-copy so LibTIFF4.2017.vcxproj* survive. Official
# repo moved to GitLab; tif_config.h/tiffconf.h are checked-in generated
# headers, not part of upstream - review after bumping.
