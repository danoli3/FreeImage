VENDOR_DIR="Source/LibOpenJPEG"
SOURCE_SUBDIRS="."
ROOT_FILES=""
CMAKE_VAR="FreeImage_LIBOPENJPEG_SRCS"
URL_TEMPLATE="https://github.com/uclouvain/openjpeg/archive/refs/tags/v{version}.tar.gz"
# Flat layout - merge-copy so LibOpenJPEG.2017.vcxproj* and opj_config*.h
# (checked-in generated headers, not upstream) survive. Bundled is 2.0.0;
# upstream has restructured some internal APIs since - check
# PluginJ2K.cpp/PluginJP2.cpp/J2KHelper.cpp compile cleanly, this is a
# large multi-version jump.
