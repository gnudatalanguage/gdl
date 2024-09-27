// -*-C-*-
//
//  Maurice LeBrun
//  IFS, University of Texas at Austin
//  18-Jul-1994
//
//  Copyright (C) 2004-2016  Alan W. Irwin
//  Copyright (C) 2004  Rafael Laboissiere
//  Copyright (C) 2004  Joao Cardoso
//
//  This file is part of PLplot.
//
//  PLplot is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Library General Public License as published
//  by the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  PLplot is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Library General Public License for more details.
//
//  You should have received a copy of the GNU Library General Public License
//  along with PLplot; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

// Configured (by CMake) macros for PLplot that are required for the
// core build and the build of the installed examples (and presumably
// any user applications).  Therefore, the configured plConfig.h
// should be installed.  In contrast, plplot_config.h.in (note,
// plConfig.h #includes plplot_config.h for the core build because
// PLPLOT_HAVE_CONFIG_H is #defined in that case) contains configured macros
// that are only required for the core build.  Therefore, in contrast
// to plConfig.h, plplot_config.h should not be installed.
//
// Maintenance issue: in makes no sense to configure duplicate macros
// for both plplot_config.h and plConfig.h.  Therefore, when adding a macro
// decide which file to put it in depending on whether the result is
// needed for the installed examples build or not.  Furthermore, move
// configured macros from one file to the other as needed depending on
// that criterion, but do not copy them.

#ifndef __PLCONFIG_H__
#define __PLCONFIG_H__

#ifdef PLPLOT_HAVE_CONFIG_H
#  include <plplot_config.h>
#endif

// PLplot version information.
#define PLPLOT_VERSION_MAJOR    5
#define PLPLOT_VERSION_MINOR    15
#define PLPLOT_VERSION_PATCH    0
#define PLPLOT_VERSION          "5.15.0"

// Define if you have c++ accessible stdint.h
#define PL_HAVE_CXX_STDINT_H

// Define if snprintf is available
#ifndef PL_HAVE_SNPRINTF
#define PL_HAVE_SNPRINTF
#endif

// Define if _snprintf is available
#ifndef _PL_HAVE_SNPRINTF
/* #undef _PL_HAVE_SNPRINTF */
#endif

// Define if isfinite is available
#define PL_HAVE_ISFINITE

// Define if finite is available
/* #undef PL_HAVE_FINITE */

// Define if _finite is available
/* #undef PL__HAVE_FINITE */

// Define if isinf is available
#define PL_HAVE_ISINF

// Define if _isinf is available
/* #undef PL__HAVE_ISINF */

// Define if isnan is available
#define PL_HAVE_ISNAN

// Define if _isnan is available
/* #undef PL__HAVE_ISNAN */

// Define to 1 if you have the <stdint.h> header file.
#define PL_HAVE_STDINT_H 1

// Define to 1 if you have the <unistd.h> header file.
#define PL_HAVE_UNISTD_H 1

// Define if nanosleep is available
#define PL_HAVE_NANOSLEEP

// Define if you want PLplot's float type to be double
#define PL_DOUBLE

// Define if C++ compiler accepts using namespace
#define PL_USE_NAMESPACE

// Define if isnan is present in <math.h> but not in <cmath>
// - broken Mac OSX systems
/* #undef PL_BROKEN_ISNAN_CXX */

#endif  // __PLCONFIG_H__
