/***************************************************************************
                          smoothPolyDnans.hpp -- nan version of smooth accelerated n D
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
#ifdef INCLUDE_SMOOTH_POLYD_NAN

SMOOTH_Ty* src=srcIn;
SMOOTH_Ty* dest=destIn;
SMOOTH_Ty* store;

SizeT srcDim[MAXRANK];
for (int i = 0; i < rank; ++i) srcDim[i] = datainDim[i];

SizeT nEl=1;
for (int i=0; i< rank; ++i) nEl*=srcDim[i];

DUInt perm[rank]; //turntable transposition index list
for (int i = 0; i < rank; ++i) perm[i] = ((i + 1) % rank); //[1,2,...,0] 

SizeT destStride[ MAXRANK + 1];

SizeT destDim[ MAXRANK]; //use destDim mainly to turn resDim each time
for (int i = 0; i < rank; ++i) destDim[i] = srcDim[i];

// successively apply smooth 1d and write perm-transposed values in res, then exchange res and data for next iteration.
// the trick is to update srcDim and resStride each time.
for (int r = 0; r < rank; ++r) {
 //accelerator: compute destStride from a perm-uted srcDim:
 destStride[0] = 1;
 destStride[1] = srcDim[perm[0]];
 int m = 1;
 for (; m < rank; ++m) destStride[m + 1] = destStride[m] * srcDim[perm[m]];
 for (; m < MAXRANK; ++m) destStride[m + 1] = destStride[rank];

 SizeT dimx = srcDim[0]; //which has been permutated 
 SizeT dimy = nEl / dimx;
 SizeT w = width[r] / 2;
 if (w == 0) {//fast transpose
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
  {
#pragma omp for nowait
   for (SizeT i = 0; i < nEl; ++i) dest[transposed1Index(i, srcDim, destStride, rank)] = src[i];
  }
 } else { //smooth & transpose
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
  {
#pragma omp for nowait

   for (SizeT j = 0; j < dimy; ++j) {
    //initiate mean of Width first values:
    DDouble z;
    DDouble n = 0;
    DDouble mean = 0;
    for (SizeT i = j * dimx; i < j * dimx + (2 * w + 1); ++i) {
     DDouble v = src[i];
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
     dest[transposed1Index(w - i + j * dimx, srcDim, destStride, rank)] = (n1 > 0) ? mean1 : src[w - i + j * dimx];
     DDouble v = src[2 * w - i + j * dimx];
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
      if (n1 < 2 * w + 1) n1 += 1.0;
      mean1 += v;
      mean1 /= n1;
     }
    }
    dest[transposed1Index(0 + j*dimx, srcDim, destStride, rank)] = (n1 > 0) ? mean1 : src[0 + j*dimx];
#else //just transpose
    for (SizeT i = j * dimx; i < j * dimx + w; ++i) dest[transposed1Index(i, srcDim, destStride, rank)] = src[i];
#endif      

    //center
    for (SizeT i = w + j * dimx, ibef = j * dimx, iend = 2 * w + 1 + j * dimx; i < dimx - w - 1 + j * dimx; ++i, ++ibef, ++iend) {
     dest[transposed1Index(i, srcDim, destStride, rank)] = (n > 0) ? mean : src[i];
     DDouble v = src[ibef];
     if (isfinite(v)) {
      mean *= n;
      mean -= v;
      n -= 1.0;
      mean /= n;
     }

     if (n <= 0) mean = 0;

     v = src[iend];
     if (isfinite(v)) {
      mean *= n;
      if (n < 2 * w + 1) n += 1.0;
      mean += v;
      mean /= n;
     }
    }
    dest[transposed1Index(dimx - 1 - w + j*dimx, srcDim, destStride, rank)] = (n > 0) ? mean : src[dimx - 1 - w + j*dimx];

#if defined(USE_EDGE)  
    //end: use mean2 & n as we go in the same direction...
    DDouble mean2 = mean;

    for (SizeT i = dimx - 1 - w; i < dimx - 1; ++i) {
     dest[transposed1Index(i + j*dimx, srcDim, destStride, rank)] = (n > 0) ? mean2 : src[i + j*dimx];
     DDouble v = src[i - w + j * dimx];
     if (isfinite(v)) {
      mean2 *= n;
      mean2 -= v;
      n -= 1.0;
      mean2 /= n;
     }

     if (n <= 0) mean2 = 0;

#if defined (EDGE_WRAP)  
     v = src[i - dimx + w + 1 + j * dimx];
     if (isfinite(v)) {
#elif defined (EDGE_TRUNCATE)  
     v = src[dimx - 1 + j * dimx];
     if (isfinite(v)) {
#elif defined (EDGE_MIRROR)  
     v = src[2 * dimx - i - w - 2 + j * dimx];
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
    dest[transposed1Index(dimx - 1 + j*dimx, srcDim, destStride, rank)] = (n > 0) ? mean2 : src[dimx - 1 + j*dimx];
#else //just transpose
    for (SizeT i = (dimx - w + j * dimx); i < (dimx + j * dimx); ++i) dest[transposed1Index(i, srcDim, destStride, rank)] = src[i];
#endif   
   }
  }
 }
 //pseudo-dim of src is now rotated by 1
 SizeT tempSize[MAXRANK];
 for (int i = 0; i < rank; ++i) tempSize[i] = srcDim[i];
 for (int i = 0; i < rank; ++i) srcDim[i] = tempSize[perm[i]];
 // exchange dest and src
  store=src;
  src=dest;
  dest=store; //now 'src' is the last computed array, whatever srcIn or destIn...
}
//if rank is even, must copy back src to destIn (since only the pointers were exchanged)
if (rank%2 == 0) memcpy(destIn, src, nEl * sizeof (src[0]));
#endif
