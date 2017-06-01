/***************************************************************************
                          smooth1dnans.hpp -- nan version of smooth 1d
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
#ifdef INCLUDE_SMOOTH_1D_NAN
//initiate mean of Width first values:
DDouble mean = 0;
DDouble z;
DDouble n = 0;
//initial mean. may be Nan
for (SizeT i = 0; i < (2 * w + 1); ++i) {
 DDouble v = data[i];
 if (isfinite(v)) {
  n += 1.0;
  z = 1. / n;
  mean = (1. - z) * mean + z * v;
 }
}
#if defined(USE_EDGE)  
//start: use mean1, n1
DDouble mean1 = mean;
DDouble n1 = n;
for (SizeT i = 0; i < w; ++i) {
 if (n1 > 0) res[w - i] = mean1;
 DDouble v = data[2 * w - i];
 if (isfinite(v)) {
  mean1 *= n1;
  mean1 -= v;
  n1 -= 1.0;
  mean1 /= n1;
 }

 if (n1 <= 0) mean1 = 0;

#if defined (EDGE_WRAP)  
 v = data[dimx - i - 1];
 if (isfinite(v)) {
#elif defined (EDGE_TRUNCATE)  
 v = data[0];
 if (isfinite(v)) {
#elif defined (EDGE_MIRROR)  
 v = data[i];
 if (isfinite(v)) {
#elif defined (EDGE_ZERO)
 v = 0;
 {
#endif
  mean1 *= n1;
  if (n1 < 2 * w + 1) n1 += 1.0;
  mean1 += v;
  mean1 /= n1;
 }
}
if (n1 > 0) res[0] = mean1;

#endif

//middle: use mean & n
for (SizeT i = w, ivm = 0, ivp = 2 * w + 1; i < dimx - w - 1; ++i, ++ivm, ++ivp) {
 if (n > 0) res[i] = mean; //otherwise contains already data[i] since res is initialized to data.
 DDouble v = data[ivm];
 if (isfinite(v)) {
  mean *= n;
  mean -= v;
  n -= 1.0;
  mean /= n;
 }

 if (n <= 0) mean = 0;

 v = data[ivp];
 if (isfinite(v)) {
  mean *= n;
  if (n < 2 * w + 1) n += 1.0;
  mean += v;
  mean /= n;
 }
}
if (n > 0) res[dimx - 1 - w] = mean;

#ifdef USE_EDGE  
//end: use mean2 & n as we go in the same direction...
DDouble mean2 = mean;

for (SizeT i = dimx - 1 - w; i < dimx - 1; ++i) {
 if (n > 0) res[i] = mean2;
 DDouble v = data[i - w];
 if (isfinite(v)) {
  mean2 *= n;
  mean2 -= v;
  n -= 1.0;
  mean2 /= n;
 }

 if (n <= 0) mean2 = 0;

#if defined (EDGE_WRAP)  
 v = data[i - dimx + w + 1];
 if (isfinite(v)) {
#elif defined (EDGE_TRUNCATE)  
 v = data[dimx - 1];
 if (isfinite(v)) {
#elif defined (EDGE_MIRROR)  
 v = data[2 * dimx - i - w - 2];
 if (isfinite(v)) {
#elif defined (EDGE_ZERO)
 v = 0;
 {
#endif
  mean2 *= n;
  if (n < 2 * w + 1) n += 1.0;
  mean2 += v;
  mean2 /= n;
 }
}
if (n > 0) res[dimx - 1] = mean2;
#endif

#endif
