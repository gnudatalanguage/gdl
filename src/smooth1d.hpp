/***************************************************************************
                          smooth1d.hpp -- accelerated smooth 1d
                             -------------------
    begin                : May 30 2017
    copyright            : (C) 2017 by G. Duvert
    email                : m_schellens@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// to be included from datatypes.cpp
#ifdef INCLUDE_SMOOTH_1D
//initiate mean of Width first values:
DDouble mean = 0;
DDouble z;
DDouble n = 0;
//initial mean. may be Nan
for (SizeT i = 0; i < (2 * w + 1); ++i) {
 DDouble v = data[i];
 n += 1.0;
 z = 1. / n;
 mean = (1. - z) * mean + z * v;
}
#if defined(USE_EDGE)  
//start: use mean1
DDouble mean1 = mean;
for (SizeT i = 0; i < w; ++i) {
 res[w - i] = mean1;
 DDouble v = data[2 * w - i];
 mean1 -= z*v;

#if defined (EDGE_WRAP)  
 v = data[dimx - i - 1];
#elif defined (EDGE_TRUNCATE)  
 v = data[0];
#elif defined (EDGE_MIRROR)  
 v = data[i];
#elif defined (EDGE_ZERO)
 v = 0;
#endif
 mean1 += z*v;
}
res[0] = mean1;

#endif

//middle: use mean & n
for (SizeT i = w, ivm = 0, ivp = 2 * w + 1; i < dimx - w - 1; ++i, ++ivm, ++ivp) {
 res[i] = mean;
 DDouble v = data[ivm];
 mean -= z*v;
 v = data[ivp];
 mean += z*v;
}
res[dimx - 1 - w] = mean;

#ifdef USE_EDGE  
//end: use mean 
for (SizeT i = dimx - 1 - w; i < dimx - 1; ++i) {
 res[i] = mean;
 DDouble v = data[i - w];
 mean -= z*v;
#if defined (EDGE_WRAP)  
 v = data[i - dimx + w + 1];
#elif defined (EDGE_TRUNCATE)  
 v = data[dimx - 1];
#elif defined (EDGE_MIRROR)  
 v = data[2 * dimx - i - w - 2];
#elif defined (EDGE_ZERO)
 v = 0;
#endif
 mean += z*v;
}
res[dimx - 1] = mean;
#endif

#endif
