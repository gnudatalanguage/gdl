//  Copyright (C) 2005-2018 Alan W. Irwin
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
//
//--------------------------------------------------------------------------
//
//  This file contains deprecated routines to provide backwards compatibility
//  for a while.  For each routine the new routine you should be using instead
//  is explicitly commented.
//

#include "plplotP.h"

#ifdef PL_DEPRECATED

// The following functions have been removed from plplot ahead of the 5.9.8
// release. They have long been advertised as deprecated.
//   plParseOpts
//   plHLS_RGB
//   plRGB_HLS
//   plarrows


// The following functions have been marked as obsolete for some time,
// but were formally deprecated as of version 5.9.8 and removed as of 5.14.0.
//   plrgb
//   plrgb1
//   plhls

// The following function was formally deprecated in 5.9.10 and removed as of 5.14.0.
//   plwid

#endif // PL_DEPRECATED
