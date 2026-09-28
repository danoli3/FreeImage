//
// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) Contributors to the OpenEXR Project.
//
// This is the OpenEXR library version information.
// ImfVersion.h contains version information about the file format.
#pragma once
#ifndef INCLUDED_OPENEXR_VERSION_H
#    define INCLUDED_OPENEXR_VERSION_H

/* OpenEXRConfig.h defines these first when the C++ headers are on the
   include path (MSVC warning C4005). Keep the same 3.3.14 if this
   header is included on its own. */
#    ifndef OPENEXR_VERSION_MAJOR
#        define OPENEXR_VERSION_MAJOR 3
#    endif
#    ifndef OPENEXR_VERSION_MINOR
#        define OPENEXR_VERSION_MINOR 3
#    endif
#    ifndef OPENEXR_VERSION_PATCH
#        define OPENEXR_VERSION_PATCH 14
#    endif

#endif
