/***************************************************************************
                          smooth2d.hpp -- smooth accelerated 2d
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
#ifdef INCLUDE_SMOOTH_2D
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
   n += 1.0;
   z = 1. / n;
   mean = (1. - z) * mean + z * v;
  }

#if defined(USE_EDGE)  
  //start: use mean1
  DDouble mean1 = mean;
  for (SizeT i = 0; i < w1; ++i) {
   tmp[j + (w1 - i) * dimy] = mean1;
   DDouble v = src[2 * w1 - i + j * dimx];
   mean1 -= z*v;

#if defined (EDGE_WRAP)  
   v = src[dimx - i - 1 + j * dimx];
#elif defined (EDGE_TRUNCATE)  
   v = src[0 + j * dimx];
#elif defined (EDGE_MIRROR)  
   v = src[i + j * dimx];
#elif defined (EDGE_ZERO)
   v = 0;
#endif
   mean1 += z*v;
  }
  tmp[j + 0 * dimy] = mean1;
#else //just transpose
  for (SizeT i = 0; i < w1; ++i) tmp[j + i * dimy] = src[i + j * dimx];
#endif      

  //middle: use mean & n
  for (SizeT i = w1; i < dimx - w1 - 1;) {
   tmp[j + i * dimy] = mean;
   DDouble v = src[i - w1 + j * dimx];
   mean -= z*v;
   v = src[(i++) + w1 + 1 + j * dimx];
   mean += z*v;
  }
  tmp[j + (dimx - w1 - 1) * dimy] = mean;

#if defined(USE_EDGE)  
  //end: use mean

  for (SizeT i = dimx - 1 - w1; i < dimx - 1; ++i) {
   tmp[j + i * dimy] = mean;
   DDouble v = src[i - w1 + j * dimx];
   mean -= z*v;

#if defined (EDGE_WRAP)  
   v = src[i - dimx + w1 + 1 + j * dimx];
#elif defined (EDGE_TRUNCATE)  
   v = src[dimx - 1 + j * dimx];
#elif defined (EDGE_MIRROR)  
   v = src[2 * dimx - i - w1 - 2 + j * dimx];
#elif defined (EDGE_ZERO)
   v = 0;
#endif
   mean += z*v;
  }
  tmp[j + (dimx - 1) * dimy] = mean;
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
   n += 1.0;
   z = 1. / n;
   mean = (1. - z) * mean + z * v;
  }

#if defined(USE_EDGE)  
  //start: use mean1
  DDouble mean1 = mean;
  for (SizeT i = 0; i < w2; ++i) {
   dest[j + (w2 - i) * dimx] = mean1;
   DDouble v = tmp[2 * w2 - i + j * dimy];
   mean1 -= z*v;

#if defined (EDGE_WRAP)  
   v = tmp[dimy - i - 1 + j * dimy];
#elif defined (EDGE_TRUNCATE)  
   v = tmp[0 + j * dimy];
#elif defined (EDGE_MIRROR)  
   v = tmp[i + j * dimy];
#elif defined (EDGE_ZERO)
   v = 0;
#endif
   mean1 += z*v;
  }
  dest[j + 0 * dimx] = mean1;
#else //just transpose
  for (SizeT i = 0; i < w2; ++i) dest[j + i * dimx] = tmp[i + j * dimy];
#endif

  //middle: use mean
  for (SizeT i = w2; i < dimy - w2 - 1;) {
   dest[j + i * dimx] = mean;
   DDouble v = tmp[i - w2 + j * dimy];
   mean -= z*v;
   v = tmp[(i++) + w2 + 1 + j * dimy];
   mean += z*v;
  }
  dest[j + (dimy - w2 - 1) * dimx] = mean;

#if defined(USE_EDGE)  
  //end: use mean
  for (SizeT i = dimy - 1 - w2; i < dimy - 1; ++i) {
   dest[j + i * dimx] = mean;
   DDouble v = tmp[i - w2 + j * dimy];
   mean -= z*v;

#if defined (EDGE_WRAP)  
   v = tmp[i - dimy + w2 + 1 + j * dimy];
#elif defined (EDGE_TRUNCATE)  
   v = tmp[dimy - 1 + j * dimy];
#elif defined (EDGE_MIRROR)  
   v = tmp[2 * dimy - i - w2 - 2 + j * dimy];
#elif defined (EDGE_ZERO)
   v = 0;
#endif
   mean += z*v;
  }
  dest[j + (dimy - 1) * dimx] = mean;
#else
  for (SizeT i = dimy - w2; i < dimy; ++i) dest[j + i * dimx] = tmp[i + j * dimy];
#endif
 }
}
#endif
