/***************************************************************************
                          convol_inc0.cpp  -  convol function 
edgemode = 0 (skip)
                             -------------------
    begin                : Sep 19 2004
    copyright            : (C) 2004 by Marc Schellens
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

// to be included from convol.cpp

#ifdef INCLUDE_CONVOL_INC_CPP
// Parallel acceleration:
// first have static addresses for reference to the acceleratore below:
static long* aInitIxRef[33]; //32 threads +1!
static bool* regArrRef[33];
// Compute nchunk using parallel max number of cpu, but chunksize must be a multiple of dim0 and not zero.
// We limit the number of parallel threads to 33 given the size of the array of pointers above.
// It can be extended at will, but if someone is in a position to augment '33' then it's probably beacuse the problem
// should be treated by another algorithm than this one.
// The whole purpose of this stuff with these static arrays of pointers is to insure that the compiler does not overoptimize
// the code in the conv_inc* includes regarding the accelerator. AND to keep the code in conv_inc* more or less identical to the
// historical version, slow but true and tested (well, a few bugs were cured along with this version). 
  long chunksize=nA;
  long nchunk=1;
  if (nA > 1000) { //no use start parallel threading for small numbers.
    chunksize=nA/((CpuTPOOL_NTHREADS>32)?32:CpuTPOOL_NTHREADS);
    long n_dim0=chunksize/dim0; chunksize=dim0*n_dim0; //ensures chunksize integer number of dim0.
    if (chunksize>0) {
      nchunk=nA/chunksize;
      if (chunksize*nchunk < nA) ++nchunk;
    } else {nchunk=1; chunksize=nA;}
  }
// build a nchunk times copy of the master offset table (accelerator). Each thread will use its own copy, properly initialized.
// GD: could the offset accelerator be made easier? This would certainly simplify the code.
  long  aInitIxT[ MAXRANK+1]; //T for template
  for ( long aSp=0; aSp<=nDim; ++aSp) aInitIxT[ aSp] = 0;
  bool  regArrT[ MAXRANK];
  long  aInitIxPool[nchunk][ MAXRANK+1];
  for ( long aSp=0; aSp<=nDim; ++aSp) for (int iloop=0; iloop< nchunk; ++iloop) aInitIxPool[iloop][ aSp] = 0;
  bool  regArrPool[nchunk][ MAXRANK];
  
   for (long iloop = 0; iloop < nchunk; ++iloop) {
   //memorize current state accelerator for chunk iloop:
   for ( long aSp=0; aSp<=nDim; ++aSp) aInitIxPool[iloop][ aSp] = aInitIxT[ aSp]; 
   for ( long aSp=0; aSp<MAXRANK; ++aSp) regArrPool[iloop][ aSp] = regArrT[ aSp];
   //store addresses in static part
   aInitIxRef[iloop]=aInitIxPool[iloop];
   regArrRef[iloop]=regArrPool[iloop];
   // continue with next table
   for (long ia = iloop*chunksize; (ia < (iloop+1)*chunksize && ia < nA) ; ia += dim0) {
    register bool regular = true;
    for (long aSp = 1; aSp < nDim;) {
      if (aInitIxT[ aSp] < this->dim[ aSp]) {
        regArrT[ aSp] = (aInitIxT[aSp] >= aBeg[aSp] && aInitIxT[aSp] < aEnd[ aSp]);
        if (regular) for (; aSp < nDim; ++aSp) if (!regArrT[ aSp]) {
              regular = false;
              break;
            }
        break;
      }
      aInitIxT[ aSp] = 0;
      regArrT[ aSp] = !aBeg[ aSp];
      if (aBeg[ aSp]) regular = false;
      ++aInitIxT[ ++aSp];
    }
    ++aInitIxT[1];
   }
  }

// for all result elements
#pragma omp parallel num_threads(nchunk) firstprivate(scale,bias) shared(ker,kIxArr,res,aInitIxRef,regArrRef,nchunk,chunksize,aBeg,aEnd,nDim,aBeg0,aStride,ddP,invalidValue,kDim0,kDim0_nDim,nKel,missingValue,aEnd0,dim0,nA) //default(none)
  {
#pragma omp for nowait
  for (long iloop = 0; iloop < nchunk; ++iloop) {
  long* aInitIx=aInitIxRef[iloop]; //permits to keep code more or less the same as before.
  bool* regArr=regArrRef[iloop];   //idem.
  for (long ia = iloop*chunksize; (ia < (iloop+1)*chunksize && ia < nA) ; ia += dim0) {
    register bool regular = true;
    for (long aSp = 1; aSp < nDim;) {
      if (aInitIx[ aSp] < this->dim[ aSp]) {
        regArr[ aSp] = (aInitIx[aSp] >= aBeg[aSp] && aInitIx[aSp] < aEnd[ aSp]);
        if (regular) for (; aSp < nDim; ++aSp) if (!regArr[ aSp]) {
              regular = false;
              break;
            }
        break;
      }
      aInitIx[ aSp] = 0;
      regArr[ aSp] = !aBeg[ aSp];
      if (aBeg[ aSp]) regular = false;
      ++aInitIx[ ++aSp];
    }
  if (regular) {
    for (long aInitIx0 = aBeg0; aInitIx0 < aEnd0; ++aInitIx0) {
#if (defined(CONVOL_BYTE__) || defined (CONVOL_UINT__) || defined (CONVOL_INT__))
        register DLong res_a = 0;
        register DLong otfBias = 0;
        register DLong curScale = 0;
#else
        register Ty res_a = (*res)[ ia + aInitIx0];
        register Ty otfBias = this->zero;
        register Ty curScale = this->zero;
#endif
        register long counter = 0;
        long *kIx = kIxArr;
        for (long k = 0; k < nKel; k += kDim0) {
          register long aLonIx = aInitIx0 + kIx[0];
          for (long rSp = 1; rSp < nDim; ++rSp) aLonIx += (aInitIx[ rSp] + kIx[ rSp]) * aStride[ rSp];

          for (long k0 = 0; k0 < kDim0; ++k0) {
#ifdef CONVOL_CENTER
            register Ty ddpHlp = ddP[ aLonIx + k0];
#else
            register Ty ddpHlp = ddP[ aLonIx - k0];
#endif
#if defined (CONVOL_NAN_INVALID)
            if (ddpHlp != invalidValue && gdlValid(ddpHlp)) {
#elif defined (CONVOL_INVALID)
            if (ddpHlp != invalidValue) {
#elif defined (CONVOL_NAN)
            if (gdlValid(ddpHlp)) {
#else
              {
#endif
              res_a += ddpHlp * ker[ k + k0];
              counter++;
#if defined (CONVOL_NORMALIZE)
                curScale += absker[ k + k0];
                otfBias += biasker[ k + k0];
#endif
            }
          }
          kIx += kDim0_nDim;
        }
#if defined (CONVOL_NORMALIZE)
          scale = curScale;
#if defined(CONVOL_BYTE__) || defined (CONVOL_UINT__)
          CONVERT_MODIFY_BIAS;
#else
          bias = this->zero;
#endif
#endif
        if (counter == 0) res_a = missingValue;
        else {
          res_a /= scale;
          res_a += bias;
        }

#if defined(CONVOL_BYTE__) || defined (CONVOL_UINT__) || defined (CONVOL_INT__)
        CONVERT_CONVOL_TO_ORIG;
#else
        (*res)[ia + aInitIx0] = res_a;
#endif
            }
  }// if( regular) // in dim 1-n
  ++aInitIx[1];
} // for(...)
      } //for iloop
  }
#endif
