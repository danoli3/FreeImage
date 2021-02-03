/* -*- C++ -*-
 * Copyright 2020 LibRaw LLC (info@libraw.org)
 *

 LibRaw is free software; you can redistribute it and/or modify
 it under the terms of the one of two licenses as you choose:

1. GNU LESSER GENERAL PUBLIC LICENSE version 2.1
   (See file LICENSE.LGPL provided in LibRaw distribution archive for details).

2. COMMON DEVELOPMENT AND DISTRIBUTION LICENSE (CDDL) Version 1.0
   (See file LICENSE.CDDL provided in LibRaw distribution archive for details).

 */

#ifndef DCRAW_DEFS_H
#define DCRAW_DEFS_H

#include <math.h>
#define LIBRAW_LIBRARY_BUILD
#define LIBRAW_IO_REDEFINED
#include "libraw/libraw.h"
#include "libraw/libraw_types.h"
#include "internal/defines.h"
#include "internal/var_defines.h"

#define stmread(buf, maxlen, fp) stread(buf, MIN(maxlen, sizeof(buf)), fp)
#define strbuflen(buf) strnlen(buf, sizeof(buf) - 1)
#define makeIs(idx) (maker_index == idx)
#define strnXcat(buf, string)                                                  \
  strncat(buf, string, LIM(sizeof(buf) - strbuflen(buf) - 1, 0, sizeof(buf)))

// DNG was written by:
#define nonDNG 0
#define CameraDNG 1
#define AdobeDNG 2

// Makernote tag type:
#define is_0x927c 0 /* most cameras */
#define is_0xc634 2 /* Adobe DNG, Sony SR2, Pentax */
#define ilm imgdata.lens.makernotes
#define icWBC imgdata.color.WB_Coeffs
#define icWBCCTC imgdata.color.WBCT_Coeffs
#define imCanon imgdata.makernotes.canon
#define imFuji imgdata.makernotes.fuji
#define imHassy imgdata.makernotes.hasselblad
#define imKodak imgdata.makernotes.kodak
#define imNikon imgdata.makernotes.nikon
#define imOly imgdata.makernotes.olympus
#define imPana imgdata.makernotes.panasonic
#define imPentax imgdata.makernotes.pentax
#define imSamsung imgdata.makernotes.samsung
#define imSony imgdata.makernotes.sony
#define imCommon imgdata.makernotes.common


#define ph1_bits(n) ph1_bithuff(n, 0)
#define ph1_huff(h) ph1_bithuff(*h, h + 1)
#define getbits(n) getbithuff(n, 0)
#define gethuff(h) getbithuff(*h, h + 1)

#if __ANDROID__ // not defined in
    #ifndef swabndk
        #define swabndk
        #include <stdint.h>
#if __ANDROID_API__ < 28
        #include <asm/byteorder.h> // defined in Android SDK >28
        #define ___swab(x)  ({ __u16 __x = (x);   ((__u16)(   (((__u16)(__x) & (__u16)0x00ffU) << 8) | (((__u16)(__x) & (__u16)0xff00U) >> 8) ));  })
        inline void swab(const void *from, void *to, size_t n)
        {
            size_t i;
            if (n < 0)
                return;
            for (i = 0; i < (n/2)*2; i += 2)
                *((uint16_t*)to+i) = ___swab(*((uint16_t*)from+i));
        }
#endif
        #include <unistd.h>
    #endif
    #endif
#endif
