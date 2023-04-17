cmake_minimum_required(VERSION 3.0)

project(FreeImage)

set(FreeImage_SOURCES
	./Source/CacheFile.h ./Source/FreeImage ./Source/FreeImage.h ./Source/FreeImageIO.h ./Source/LibJPEG/cderror.h ./Source/LibJPEG/cdjpeg.h ./Source/LibJPEG/jconfig.h ./Source/LibJPEG/jdct.h ./Source/LibJPEG/jerror.h ./Source/LibJPEG/jinclude.h ./Source/LibJPEG/jpegint.h ./Source/LibJPEG/jpeglib.h ./Source/LibJPEG/jmemsys.h ./Source/LibJPEG/jmorecfg.h ./Source/LibJPEG/transupp.h ./Source/LibJPEG/jversion.h ./Source/LibJXR/image/decode/decode.c ./Source/LibJXR/image/decode/JXRTranscode.c ./Source/LibJXR/image/decode/postprocess.c ./Source/LibJXR/image/decode/segdec.c ./Source/LibJXR/image/decode/strInvTransform.c ./Source/LibJXR/image/decode/strdec.c ./Source/LibJXR/image/decode/strdec_x86.c ./Source/LibJXR/image/decode/strPredQuantDec.c ./Source/LibJXR/image/encode/encode.c ./Source/LibJXR/image/encode/segenc.c ./Source/LibJXR/image/encode/strenc.c ./Source/LibJXR/image/encode/strenc_x86.c ./Source/LibJXR/image/encode/strFwdTransform.c ./Source/LibJXR/image/encode/strPredQuantEnc.c ./Source/LibJXR/jxrgluelib/JXRGlue.c ./Source/LibJXR/jxrgluelib/JXRGlueJxr.c ./Source/LibJXR/jxrgluelib/JXRGluePFC.c ./Source/LibJXR/jxrgluelib/JXRMeta.c ./Source/LibOpenJPEG/bio.c ./Source/LibOpenJPEG/cio.c ./Source/LibOpenJPEG/dwt.c ./Source/LibOpenJPEG/event.c ./Source/LibOpenJPEG/function_list.c ./Source/LibOpenJPEG/image.c ./Source/LibOpenJPEG/invert.c ./Source/LibOpenJPEG/j2k.c ./Source/LibOpenJPEG/jp2.c ./Source/LibOpenJPEG/mct.c ./Source/LibOpenJPEG/mqc.c ./Source/LibOpenJPEG/openjpeg.c ./Source/LibOpenJPEG/opj_clock.c ./Source/LibOpenJPEG/pi.c ./Source/LibOpenJPEG/raw.c ./Source/LibOpenJPEG/t1.c ./Source/LibOpenJPEG/t2.c ./Source/LibOpenJPEG/tcd.c ./Source/LibOpenJPEG/tgt.c ./Source/LibTIFF4/t4.h ./Source/LibTIFF4/tif_config.h ./Source/LibTIFF4/tif_config.vc.h ./Source/LibTIFF4/tif_config.wince.h ./Source/LibTIFF4/tif_dir.h ./Source/LibTIFF4/tif_fax3.h ./Source/LibTIFF4/tif_predict.h ./Source/LibTIFF4/tiff.h ./Source/LibTIFF4/tiffconf.h ./Source/LibTIFF4/tiffconf.vc.h ./Source/LibTIFF4/tiffconf.wince.h ./Source/LibTIFF4/tiffio.h ./Source/LibTIFF4/tiffvers.h ./Source/LibTIFF4/uvcode.h ./Source/LibWebP/src/dec/ ./Source/FreeImage/eLut.h ./Source/FreeImage/J2KHelper.h ./Source/FreeImage/PSDParser.h ./Source/FreeImage/toFloat.h ./Source/FreeImageToolkit/Filters.h ./Source/FreeImageToolkit/Resize.h ./Source/LibJXR/common/include/guiddef.h ./Source/LibJXR/common/include/wmsal.h ./Source/LibJXR/common/include/wmspecstring.h ./Source/LibJXR/common/include/wmspecstrings_adt.h ./Source/LibJXR/common/include/wmspecstrings_strict.h ./Source/LibJXR/common/include/wmspecstrings_undef.h ./Source/LibJXR/image/sys/ansi.h ./Source/LibJXR/image/sys/common.h ./Source/LibJXR/image/sys/perfTimer.h ./Source/LibJXR/image/sys/strcodec.h ./Source/LibJXR/image/sys/strTransform.h ./Source/LibJXR/image/sys/windowsmediaphoto.h ./Source/LibJXR/image/sys/xplatform_image.h ./Source/LibJXR/image/x86/x86.h ./Source/LibPNG/png.h ./Source/LibPNG/pngconf.h ./Source/LibPNG/pngdebug.h ./Source/LibPNG/pnginfo.h ./Source/LibPNG/pnglibconf.h ./Source/LibPNG/pngpriv.h ./Source/LibPNG/pngstruct.h ./Source/MapIntrospector.h ./Source/Metadata/FIRational.h ./Source/Metadata/FreeImageTag.h ./Source/Plugin.h ./Source/Quantizers.h ./Source/ToneMapping.h ./Source/Utilities.h ./Source/ZLib/crc32.h ./Source/ZLib/deflate.h ./Source/ZLib/gzguts.h ./Source/ZLib/inffast.h ./Source/ZLib/inffixed.h ./Source/ZLib/inflate.h ./Source/ZLib/inftrees.h ./Source/ZLib/trees.h ./Source/ZLib/zconf.h ./Source/ZLib/zlib.h ./Source/ZLib/zutil.h ./TestAPI/TestSuite.h 
)

# need include directories for FreeImage and all the sub libs
include_directories(Source)
include_directories(Source/Metadata)
include_directories(Source/FreeImageToolkit)
include_directories(Source/LibJPEG)
include_directories(Source/LibPNG)
include_directories(Source/LibTIFF4)
include_directories(Source/ZLib)
include_directories(Source/LibOpenJPEG)
include_directories(Source/LibWebP)
include_directories(Source/LibJXR)
include_directories(Source/LibJXR/image/sys)
include_directories(Source/LibJXR/common/include)
include_directories(Source/LibJXR/jxrgluelib)

add_definitions(-DOPJ_STATIC -DNO_WINDOWS -DNDEBUG -DNO_LCMS -D__ANSI__ -DDISABLE_PERF_MEASUREMENT -DFREEIMAGE_LIB -DNO_LCMS)
add_library(FreeImage STATIC ${FreeImage_SOURCES})
#install_dep(FreeImage include Source/FreeImage.h)

if (APPLE)
 set_target_properties(FreeImage PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH "NO")

 if(BUILD_PLATFORM_APPLE_IOS)
  set_target_properties(FreeImage PROPERTIES XCODE_ATTRIBUTE_GCC_THUMB_SUPPORT "NO")
 endif()
endif()

set(FreeImage_INCLUDE_DIR "${FreeImage_SOURCE_DIR}/Source" CACHE PATH "" FORCE)
set(FreeImage_LIBRARY_DBG FreeImage CACHE STRING "" FORCE)
set(FreeImage_LIBRARY_REL FreeImage CACHE STRING "" FORCE)
mark_as_advanced(FreeImage_INCLUDE_DIR FreeImage_LIBRARY_DBG FreeImage_LIBRARY_REL)

INSTALL (
    DIRECTORY ${CMAKE_SOURCE_DIR}/Source/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h*")
