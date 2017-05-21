/***************************************************************************
                          convol_inc0.cpp  -  convol function 
edgemode = 0 (skip)
                             -------------------
    begin                : Sep 19 2004
    copyright            : (C) 2004 by Marc Schellens, 2017 by G. Duvert
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
// NOTE: All the unreadble #ifdef below are there to avoid IF constructs which results in much faster code when optimization is on.
#ifdef INCLUDE_CONVOL_INC_CPP

// for all result elements
#pragma omp parallel num_threads(nchunk) firstprivate(scale,bias) shared(ker,kIxArr,res,aInitIxRef,regArrRef,nchunk,chunksize,aBeg,aEnd,nDim,aBeg0,aStride,ddP,invalidValue,kDim0,kDim0_nDim,nKel,missingValue,aEnd0,dim0,nA,absker,biasker) //default(none)
  {
#pragma omp for
  for (long iloop = 0; iloop < nchunk; ++iloop) {
  long* aInitIx=aInitIxRef[iloop]; //permits to keep code more or less the same as before.
  bool* regArr=regArrRef[iloop];   //idem.
  for (long ia = iloop*chunksize; (ia < (iloop+1)*chunksize && ia < nA) ; ia += dim0) {
    bool regular = true;
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
        DLong res_a = 0;
        DLong otfBias = 0;
        DLong curScale = 0;
#else
        Ty res_a = (*res)[ ia + aInitIx0];
        Ty otfBias = this->zero;
        Ty curScale = this->zero;
#endif
#if defined(CONVOL_NAN_INVALID) || defined(CONVOL_INVALID) || defined(CONVOL_NAN)       
        SizeT counter = 0;
#endif
        long *kIx = kIxArr;
        for (long k = 0; k < nKel; k += kDim0) {
          long aLonIx = aInitIx0 + kIx[0];
          for (long rSp = 1; rSp < nDim; ++rSp) aLonIx += (aInitIx[ rSp] + kIx[ rSp]) * aStride[ rSp];

          for (long k0 = 0; k0 < kDim0; ++k0) {
#ifdef CONVOL_CENTER
            Ty ddpHlp = ddP[ aLonIx + k0];
#else
            Ty ddpHlp = ddP[ aLonIx - k0];
#endif
#if defined(CONVOL_NAN_INVALID)
            if (ddpHlp != invalidValue && gdlValid(ddpHlp)) {
              counter++;
#elif defined (CONVOL_INVALID)
            if (ddpHlp != invalidValue) {
              counter++;
#elif defined (CONVOL_NAN)
            if (gdlValid(ddpHlp)) {
              counter++;
#else
              {
#endif
              res_a += ddpHlp * ker[ k + k0];
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

#if defined(CONVOL_NAN_INVALID) || defined(CONVOL_INVALID) || defined(CONVOL_NAN)       
          res_a /= scale;
          res_a += bias;
          if (counter == 0) res_a = missingValue;
#else
          res_a /= scale;
          res_a += bias;
#endif

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
