/***************************************************************************
                          smooth2dnans.hpp -- nan version of smooth accelerated 2d
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
#ifdef INCLUDE_SMOOTH_2D_NAN

SizeT w1 = width[0] / 2;
SizeT w2 = width[1] / 2;
SizeT nEl = dimx*dimy;
SMOOTH_Ty tmp[nEl];
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
#pragma omp for
 for (SizeT j = 0; j < dimy; ++j) {
  DDouble z;
  DDouble n = 0;
  DDouble mean = 0;
  for (SizeT i = j * dimx; i < j * dimx + (2 * w1 + 1);) {
   DDouble v = src[i++];
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
  for (SizeT i = 0; i < w1; ++i) {
   if (n1 > 0) tmp[j + (w1 - i) * dimy] = mean1;
   DDouble v = src[2 * w1 - i + j * dimx];
   if (isfinite(v)) {
    mean1 *= n1;
    mean1 -= v;
    n1 -= 1.0;
    mean1 /= n1;
   }

   if (n1 <= 0) mean1 = 0;

#if defined (EDGE_WRAP)  
   v = src[dimx - i - 1 + j * dimx];
   if (isfinite(v)) {
#elif defined (EDGE_TRUNCATE)  
   v = src[0 + j * dimx];
   if (isfinite(v)) {
#elif defined (EDGE_MIRROR)  
   v = src[i + j * dimx];
   if (isfinite(v)) {
#elif defined (EDGE_ZERO)
   v = 0;
   {
#endif
    mean1 *= n1;
    if (n1 < 2 * w1 + 1) n1 += 1.0;
    mean1 += v;
    mean1 /= n1;
   }
  }
  if (n1 > 0) tmp[j + 0 * dimy] = mean1;
#else //just transpose
  for (SizeT i = 0; i < w1; ++i) tmp[j + i * dimy] = src[i + j * dimx];
#endif      

  //middle: use mean & n
  for (SizeT i = w1; i < dimx - w1 - 1;) {
   tmp[j + i * dimy] = (n > 0) ? mean : src[i + j * dimx];
   DDouble v = src[i - w1 + j * dimx];
   if (isfinite(v)) {
    mean *= n;
    mean -= v;
    n -= 1.0;
    mean /= n;
   }
   if (n <= 0) mean = 0;

   v = src[(i++) + w1 + 1 + j * dimx];
   if (isfinite(v)) {
    mean *= n;
    if (n < 2 * w1 + 1) n += 1.0;
    mean += v;
    mean /= n;
   }
  }
  tmp[j + (dimx - w1 - 1) * dimy] = (n > 0) ? mean : src[ dimx - w1 - 1 + j * dimx];

#if defined(USE_EDGE)  
  //end: use mean2 & n as we go in the same direction...
  DDouble mean2 = mean;

  for (SizeT i = dimx - 1 - w1; i < dimx - 1; ++i) {
   if (n > 0) tmp[j + i * dimy] = mean2;
   DDouble v = src[i - w1 + j * dimx];
   if (isfinite(v)) {
    mean2 *= n;
    mean2 -= v;
    n -= 1.0;
    mean2 /= n;
   }

   if (n <= 0) mean2 = 0;

#if defined (EDGE_WRAP)  
   v = src[i - dimx + w1 + 1 + j * dimx];
   if (isfinite(v)) {
#elif defined (EDGE_TRUNCATE)  
   v = src[dimx - 1 + j * dimx];
   if (isfinite(v)) {
#elif defined (EDGE_MIRROR)  
   v = src[2 * dimx - i - w1 - 2 + j * dimx];
   if (isfinite(v)) {
#elif defined (EDGE_ZERO)
   v = 0;
   {
#endif
    mean2 *= n;
    if (n < 2 * w1 + 1) n += 1.0;
    mean2 += v;
    mean2 /= n;
   }
  }
  if (n > 0) tmp[j + (dimx - 1) * dimy] = mean2;
#else
  for (SizeT i = dimx - w1; i < dimx; ++i) tmp[j + i * dimy] = src[i + j * dimx];
#endif
 }

#pragma omp for
 for (SizeT j = 0; j < dimx; ++j) {
  DDouble z;
  DDouble n = 0;
  DDouble mean = 0;
  for (SizeT i = j * dimy; i < j * dimy + (2 * w2 + 1);) {
   DDouble v = tmp[i++];
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
  for (SizeT i = 0; i < w2; ++i) {
   if (n1 > 0) dest[j + (w2 - i) * dimx] = mean1;
   DDouble v = tmp[2 * w2 - i + j * dimy];
   if (isfinite(v)) {
    mean1 *= n1;
    mean1 -= v;
    n1 -= 1.0;
    mean1 /= n1;
   }

   if (n1 <= 0) mean1 = 0;

#if defined (EDGE_WRAP)  
   v = tmp[dimy - i - 1 + j * dimy];
   if (isfinite(v)) {
#elif defined (EDGE_TRUNCATE)  
   v = tmp[0 + j * dimy];
   if (isfinite(v)) {
#elif defined (EDGE_MIRROR)  
   v = tmp[i + j * dimy];
   if (isfinite(v)) {
#elif defined (EDGE_ZERO)
   v = 0;
   {
#endif
    mean1 *= n1;
    if (n1 < 2 * w2 + 1) n1 += 1.0;
    mean1 += v;
    mean1 /= n1;
   }
  }
  if (n1 > 0) dest[j + 0 * dimx] = mean1;
#else //just transpose
  for (SizeT i = 0; i < w2; ++i) dest[j + i * dimx] = tmp[i + j * dimy];
#endif

  //middle: use mean & n
  for (SizeT i = w2; i < dimy - w2 - 1;) {
   dest[j + i * dimx] = (n > 0) ? mean : tmp[i + j * dimy];
   DDouble v = tmp[i - w2 + j * dimy];
   if (isfinite(v)) {
    mean *= n;
    mean -= v;
    n -= 1.0;
    mean /= n;
   }
   if (n <= 0) mean = 0;

   v = tmp[(i++) + w2 + 1 + j * dimy];
   if (isfinite(v)) {
    mean *= n;
    if (n < 2 * w2 + 1) n += 1.0;
    mean += v;
    mean /= n;
   }
  }
  dest[j + (dimy - w2 - 1) * dimx] = (n > 0) ? mean : tmp[dimy - w2 - 1 + j * dimy];

#if defined(USE_EDGE)  
  //end: use mean2 & n as we go in the same direction...
  DDouble mean2 = mean;

  for (SizeT i = dimy - 1 - w2; i < dimy - 1; ++i) {
   if (n > 0) dest[j + i * dimx] = mean2;
   DDouble v = tmp[i - w2 + j * dimy];
   if (isfinite(v)) {
    mean2 *= n;
    mean2 -= v;
    n -= 1.0;
    mean2 /= n;
   }

   if (n <= 0) mean2 = 0;

#if defined (EDGE_WRAP)  
   v = tmp[i - dimy + w2 + 1 + j * dimy];
   if (isfinite(v)) {
#elif defined (EDGE_TRUNCATE)  
   v = tmp[dimy - 1 + j * dimy];
   if (isfinite(v)) {
#elif defined (EDGE_MIRROR)  
   v = tmp[2 * dimy - i - w2 - 2 + j * dimy];
   if (isfinite(v)) {
#elif defined (EDGE_ZERO)
   v = 0;
   {
#endif
    mean2 *= n;
    if (n < 2 * w2 + 1) n += 1.0;
    mean2 += v;
    mean2 /= n;
   }
  }
  if (n > 0) dest[j + (dimy - 1) * dimx] = mean2;
#else
  for (SizeT i = dimy - w2; i < dimy; ++i) dest[j + i * dimx] = tmp[i + j * dimy];
#endif
 }
}
#endif
