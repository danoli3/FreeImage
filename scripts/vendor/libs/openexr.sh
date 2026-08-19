VENDOR_DIR="Source/OpenEXR"
SOURCE_SUBDIRS=""
ROOT_FILES=""
CMAKE_VAR=""
URL_TEMPLATE=""
# DO NOT use bump-lib.sh for OpenEXR - bundled is 2.2.0 (2017), and
# OpenEXR 3.x restructured the whole tree (Imath split into its own
# separate repo/library, IlmImf/IlmThread reorganized into OpenEXRCore,
# CMake-only build). This needs a manual, from-scratch integration, not
# a subdir merge. Flagged as high priority: the 3.4.14 release notes
# cite 15 CVE fixes over 2.2.0.
