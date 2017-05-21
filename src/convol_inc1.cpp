/***************************************************************************
                          convol_inc1.cpp  -  convol function EDGE_WRAP
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

// to be included from convol.cpp. Special treatment for borders, to be included only in case
// NOTE: All the unreadble #ifdef below are there to avoid IF constructs which results in much faster code when optimization is on.

#ifdef INCLUDE_CONVOL_INC_CPP

#pragma omp parallel num_threads(nchunk) firstprivate(scale,bias) shared(ker,kIxArr,res,aInitIxRef,regArrRef,nchunk,chunksize,aBeg,aEnd,nDim,aBeg0,aStride,ddP,invalidValue,kDim0,kDim0_nDim,nKel,missingValue,aEnd0,dim0,nA,absker,biasker,dim0_1) //default(none)
  {
#pragma omp for
  for (long iloop = 0; iloop < nchunk; ++iloop) {      
  long* aInitIx=aInitIxRef[iloop]; //permits to keep code more or less the same as before.
  bool* regArr=regArrRef[iloop];   //idem.
  for (long ia = iloop*chunksize; (ia < (iloop+1)*chunksize && ia < nA) ; ia += dim0) {
    for (long aSp = 1; aSp < nDim;) {
      if (aInitIx[ aSp] < this->dim[ aSp]) {
        regArr[ aSp] = (aInitIx[aSp] >= aBeg[aSp] && aInitIx[aSp] < aEnd[ aSp]);
        break;
      }
      aInitIx[ aSp] = 0;
      regArr[ aSp] = !aBeg[ aSp];
      ++aInitIx[ ++aSp];
    }
      for (long aInitIx0 = 0; aInitIx0 < dim0; ++aInitIx0) { 
#if (defined(CONVOL_BYTE__) || defined (CONVOL_UINT__) || defined (CONVOL_INT__))
        DLong res_a = 0;
        DLong otfBias = 0;
        DLong curScale = 0;
#else
        Ty    res_a = (*res)[ ia + aInitIx0];
        Ty    otfBias = this->zero;
        Ty    curScale = this->zero;
#endif
#if defined(CONVOL_NAN_INVALID) || defined(CONVOL_INVALID) || defined(CONVOL_NAN)
        SizeT counter = 0;
#endif
        long* kIx = kIxArr;
        for (long k = 0; k < nKel; ++k) {
          long aLonIx = aInitIx0 + kIx[0];
#if defined(CONVOL_EDGE_ZERO)
          bool doit = true;
#endif
          if (aLonIx < 0) {
#if defined (CONVOL_EDGE_WRAP)
            aLonIx += dim0;
#elif defined (CONVOL_EDGE_MIRROR)
            aLonIx = -aLonIx;
#elif defined(CONVOL_EDGE_TRUNCATE)
            aLonIx = 0;
#elif defined(CONVOL_EDGE_ZERO)
            aLonIx = 0;
            doit=false;
#endif
          } else if (aLonIx >= dim0) {
#if defined (CONVOL_EDGE_WRAP)
                aLonIx -= dim0;
#elif defined (CONVOL_EDGE_MIRROR)
                aLonIx = 2*dim0-aLonIx-1;
#elif defined(CONVOL_EDGE_TRUNCATE)
                aLonIx = dim0 - 1;
#elif defined(CONVOL_EDGE_ZERO)
                aLonIx = dim0 - 1;
                doit = false;
#endif
          }
#if defined(CONVOL_EDGE_ZERO)
          if (doit) {
#endif
            for (long rSp = 1; rSp < nDim; ++rSp) {
              long aIx = aInitIx[ rSp] + kIx[ rSp];
              if (aIx < 0) {
#if defined (CONVOL_EDGE_WRAP)
                aIx += this->dim[ rSp];
#elif defined (CONVOL_EDGE_MIRROR)
                aIx = -aIx;
#elif defined(CONVOL_EDGE_TRUNCATE)
                aIx = 0;
#elif defined(CONVOL_EDGE_ZERO)
                aIx = 0;
                doit = false;
#endif 
              } else if (aIx >= this->dim[ rSp]) {
#if defined (CONVOL_EDGE_WRAP)
                aIx -= this->dim[ rSp];
#elif defined (CONVOL_EDGE_MIRROR)
                aIx = 2*this->dim[ rSp]-aIx-1;
#elif defined(CONVOL_EDGE_TRUNCATE)
                aIx = this->dim[ rSp] - 1;
#elif defined(CONVOL_EDGE_ZERO)
                aIx = this->dim[ rSp] - 1;
                doit = false;
#endif
              }
              aLonIx += aIx * aStride[ rSp];
            }
#if defined(CONVOL_EDGE_ZERO)
          }
          if (doit) {
#endif
            Ty ddpHlp = ddP[ aLonIx];
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
              res_a += ddpHlp * ker[ k ];
#if defined (CONVOL_NORMALIZE)
                curScale += absker[ k ];
                otfBias += biasker[ k ];
#endif
            }
#if defined(CONVOL_EDGE_ZERO)
            }
#endif
          kIx += nDim;
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
    ++aInitIx[1];
  } // for ia
      } //for iloop
          } //shared
#endif
