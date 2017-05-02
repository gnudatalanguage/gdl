/***************************************************************************
                          convol.cpp  -  convol function
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


// optimized version

// to be included from datatypes.cpp (twice)
#ifdef INCLUDE_CONVOL_CPP

#ifndef CONVOL_COMMON__
#define CONVOL_COMMON__

// common used functions

#endif //#define CONVOL_COMMON__



using namespace std;

#define CONVERT_CONVOL_TO_ORIG   if(res_a>CONVOL_TRUNCATE_MIN){if(res_a<CONVOL_TRUNCATE_MAX){(*res)[ia + aInitIx0]=res_a;}else{(*res)[ia + aInitIx0]=CONVOL_TRUNCATE_MAX;}}else{(*res)[ia + aInitIx0]=CONVOL_TRUNCATE_MIN;}
//modify bias will not be used with *INT* type (documentation).
#define CONVERT_MODIFY_BIAS  bias=(scale==0)?0:otfBias*CONVOL_TRUNCATE_MAX/scale;if(bias<CONVOL_TRUNCATE_MIN){bias=CONVOL_TRUNCATE_MIN;}else{if( bias>CONVOL_TRUNCATE_MAX) bias=CONVOL_TRUNCATE_MAX;}

#if defined (CONVOL_BYTE__)
#define CONVOL_TRUNCATE_MIN 0
#define CONVOL_TRUNCATE_MAX 255

template<>
BaseGDL* Data_<SpDByte>::Convol( BaseGDL* kIn, BaseGDL* scaleIn, BaseGDL* biasIn, 
				 bool center, bool normalize, int edgeMode,
                                 bool doNan, BaseGDL* missing, bool doMissing,
                                 BaseGDL* invalid, bool doInvalid)
{
  Data_<SpDLong>* kernel = static_cast<Data_<SpDLong>*>( kIn);
  Data_<SpDLong>* dabskern = new Data_<SpDLong>( kIn->Dim(), BaseGDL::ZERO);
  Data_<SpDLong>* dbiaskern = new Data_<SpDLong>( kIn->Dim(), BaseGDL::ZERO);
  DLong* absker = static_cast<DLong*>( dabskern->DataAddr());
  DLong* biasker = static_cast<DLong*>( dbiaskern->DataAddr());
  
  DLong scale = (*static_cast<Data_<SpDLong>*>( scaleIn))[0];
  // the result to be returned
  Data_* res = New( dim, BaseGDL::ZERO);
  DLong* ker = static_cast<DLong*>( kernel->DataAddr());
  DLong bias = (*static_cast<Data_<SpDLong>*>( biasIn))[0];
  
#elif defined (CONVOL_UINT__)
#define CONVOL_TRUNCATE_MIN 0
#define CONVOL_TRUNCATE_MAX 65535
template<>
BaseGDL* Data_<SpDUInt>::Convol( BaseGDL* kIn, BaseGDL* scaleIn, BaseGDL* biasIn, 
				 bool center, bool normalize, int edgeMode,
                                 bool doNan, BaseGDL* missing, bool doMissing,
                                 BaseGDL* invalid, bool doInvalid)
{
  Data_<SpDLong>* kernel = static_cast<Data_<SpDLong>*>( kIn);
  Data_<SpDLong>* dabskern = new Data_<SpDLong>( kIn->Dim(), BaseGDL::ZERO);
  Data_<SpDLong>* dbiaskern = new Data_<SpDLong>( kIn->Dim(), BaseGDL::ZERO);
  DLong* absker = static_cast<DLong*>( dabskern->DataAddr());
  DLong* biasker = static_cast<DLong*>( dbiaskern->DataAddr());

  DLong scale = (*static_cast<Data_<SpDLong>*>( scaleIn))[0];
  // the result to be returned
  Data_* res = New( dim, BaseGDL::ZERO);
  DLong* ker = static_cast<DLong*>( kernel->DataAddr());
  DLong bias = (*static_cast<Data_<SpDLong>*>( biasIn))[0];
  
#elif defined (CONVOL_INT__)
#define CONVOL_TRUNCATE_MIN -32768
#define CONVOL_TRUNCATE_MAX 32767
template<>
BaseGDL* Data_<SpDInt>::Convol( BaseGDL* kIn, BaseGDL* scaleIn, BaseGDL* biasIn, 
				 bool center, bool normalize, int edgeMode,
                                 bool doNan, BaseGDL* missing, bool doMissing,
                                 BaseGDL* invalid, bool doInvalid)
{
  Data_<SpDLong>* kernel = static_cast<Data_<SpDLong>*>( kIn);
  Data_<SpDLong>* dabskern = new Data_<SpDLong>( kIn->Dim(), BaseGDL::ZERO);
  Data_<SpDLong>* dbiaskern = new Data_<SpDLong>( kIn->Dim(), BaseGDL::ZERO);
  DLong* absker = static_cast<DLong*>( dabskern->DataAddr());
  DLong* biasker = static_cast<DLong*>( dbiaskern->DataAddr());

  DLong scale = (*static_cast<Data_<SpDLong>*>( scaleIn))[0];
  // the result to be returned
  Data_* res = New( dim, BaseGDL::ZERO);
  DLong* ker = static_cast<DLong*>( kernel->DataAddr());
  DLong bias = (*static_cast<Data_<SpDLong>*>( biasIn))[0];
#elif defined (CONVOL_ULONG__)
#define CONVOL_TRUNCATE_MIN 0
#define CONVOL_TRUNCATE_MAX 1 //not truncated, anything goes.
template<>
BaseGDL* Data_<SpDULong>::Convol( BaseGDL* kIn, BaseGDL* scaleIn, BaseGDL* biasIn, 
				 bool center, bool normalize, int edgeMode,
                                 bool doNan, BaseGDL* missing, bool doMissing,
                                 BaseGDL* invalid, bool doInvalid)
{
  Data_<SpDLong>* kernel = static_cast<Data_<SpDLong>*>( kIn);
  Data_<SpDLong>* dabskern = new Data_<SpDLong>( kIn->Dim(), BaseGDL::ZERO);
  Data_<SpDLong>* dbiaskern = new Data_<SpDLong>( kIn->Dim(), BaseGDL::ZERO);
  DLong* absker = static_cast<DLong*>( dabskern->DataAddr());
  DLong* biasker = static_cast<DLong*>( dbiaskern->DataAddr());

  DLong scale = (*static_cast<Data_<SpDLong>*>( scaleIn))[0];
  // the result to be returned
  Data_* res = New( dim, BaseGDL::ZERO);
  DLong* ker = static_cast<DLong*>( kernel->DataAddr());
  DLong bias = (*static_cast<Data_<SpDLong>*>( biasIn))[0];
#elif defined (CONVOL_ULONG64__)
#define CONVOL_TRUNCATE_MIN 0
#define CONVOL_TRUNCATE_MAX 1 //not truncated, anything goes.
template<>
BaseGDL* Data_<SpDULong64>::Convol( BaseGDL* kIn, BaseGDL* scaleIn, BaseGDL* biasIn, 
				 bool center, bool normalize, int edgeMode,
                                 bool doNan, BaseGDL* missing, bool doMissing,
                                 BaseGDL* invalid, bool doInvalid)
{
  Data_<SpDLong64>* kernel = static_cast<Data_<SpDLong64>*>( kIn);
  Data_<SpDLong64>* dabskern = new Data_<SpDLong64>( kIn->Dim(), BaseGDL::ZERO);
  Data_<SpDLong64>* dbiaskern = new Data_<SpDLong64>( kIn->Dim(), BaseGDL::ZERO);
  DLong64* absker = static_cast<DLong64*>( dabskern->DataAddr());
  DLong64* biasker = static_cast<DLong64*>( dbiaskern->DataAddr());

  DLong64 scale = (*static_cast<Data_<SpDLong64>*>( scaleIn))[0];
  // the result to be returned
  Data_* res = New( dim, BaseGDL::ZERO);
  DLong64* ker = static_cast<DLong64*>( kernel->DataAddr());
  DLong64 bias = (*static_cast<Data_<SpDLong64>*>( biasIn))[0];
#else


template<class Sp>
BaseGDL* Data_<Sp>::Convol( BaseGDL* kIn, BaseGDL* scaleIn, BaseGDL* biasIn,
			    bool center, bool normalize, int edgeMode,
                            bool doNan, BaseGDL* missing, bool doMissing,
                            BaseGDL* invalid, bool doInvalid)
{
  Data_* kernel = static_cast<Data_*>( kIn);
  Data_* dabskern = new Data_( kIn->Dim(), BaseGDL::ZERO);
  Data_* dbiaskern = new Data_( kIn->Dim(), BaseGDL::ZERO);
  Ty* absker = &(*dabskern)[0];
  Ty* biasker = &(*dbiaskern)[0];
  Ty scale = (*static_cast<Data_*>( scaleIn))[0];
  // the result to be returned
  Data_* res = New( this->dim, BaseGDL::ZERO);
  Ty* ker = &(*kernel)[0];
  Ty bias = (*static_cast<Data_*>( biasIn))[0];
#endif

  Ty missingValue = (*static_cast<Data_*>( missing))[0];
  Ty invalidValue = (*static_cast<Data_*> (invalid))[0];


  SizeT nA = N_Elements();
  SizeT nKel = kernel->N_Elements();

  if(normalize)
    { 
      scale = this->zero;
      for ( SizeT ind=0; ind<nKel; ind++ )
      { //abs(kern) needed when normalizing:
        absker[ind]=abs(ker[ind]);
        scale+=absker[ind];
      }
      bias=this->zero;
#if defined(CONVOL_BYTE__)||defined (CONVOL_UINT__)
      DDouble tmp=0; 
      for ( SizeT ind=0; ind<nKel; ind++ ) { if(ker[ind]<0) biasker[ind]=absker[ind]; tmp+=biasker[ind];}
	  bias=tmp*CONVOL_TRUNCATE_MAX/scale;
	  if( bias<CONVOL_TRUNCATE_MIN) bias=CONVOL_TRUNCATE_MIN; else if( bias>CONVOL_TRUNCATE_MAX) bias=CONVOL_TRUNCATE_MAX;
#endif
    }
  else 
  {
      if( scale == this->zero) scale = 1;
  }

  SizeT nDim = this->Rank(); // number of dimension to run over

  SizeT kStride[MAXRANK+1];
  kernel->Dim().Stride( kStride, nDim);

  // setup kIxArr[ nDim * nKel] the offset array
  // this handles center
  long* kIxArr = new long[ nDim * nKel];
  ArrayGuard<long> kIxArrGuard( kIxArr); // guard it
  for( SizeT k=0; k<nKel; ++k)
    {
      kIxArr[ k * nDim + 0] = -(k % kernel->Dim( 0));
      if( center) kIxArr[ k * nDim + 0] = -(kIxArr[ k * nDim + 0] + 
					    kernel->Dim( 0) / 2);
      for( SizeT kSp=1; kSp<nDim; ++kSp)
	{
	  SizeT kDim = kernel->Dim( kSp);
	  if( kDim == 0) kDim = 1;
	  kIxArr[ k * nDim + kSp] = -((k / kStride[kSp]) % kDim);
	  if( center) kIxArr[ k * nDim + kSp] = -(kIxArr[ k * nDim + kSp] + 
						  kDim / 2);
	}
    }

  SizeT  aStride[ MAXRANK + 1];
  this->dim.Stride( aStride, nDim);

  long  aBeg[ MAXRANK];
  long  aEnd[ MAXRANK];
  for( SizeT aSp=0; aSp<nDim; ++aSp) 
    {
      SizeT kDim = kernel->Dim( aSp);
      if( kDim == 0) kDim = 1;
      aBeg[ aSp] = (center) ? kDim/2 : kDim-1; // >= 
      aEnd[ aSp] = (center) ? this->dim[aSp]-(kDim-1)/2 : this->dim[aSp]; // <
    }

  Ty* ddP = &(*this)[0];
 
//test if array has nans when donan is present (treatment would be shorter if array had no nans)
  if(doNan && doInvalid)
  {
    doNan = false;
    doInvalid=false;
#pragma omp parallel if (nA >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nA))
    {
#pragma omp for
    for( OMPInt i=0; i<nA; ++i)  {
        if (!gdlValid(ddP[i])) {doNan=true;}
        if (ddP[i] == invalidValue) {doInvalid=true;}
      }
    }
  }
  else if(doNan)
  {
    doNan = false;
#pragma omp parallel if (nA >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nA))
    {
#pragma omp for
    for( OMPInt i=0; i<nA; ++i)  if (!gdlValid(ddP[i])) {doNan=true;}
    }
  }
//same for invalid. a real gain of time if no values are invalid, a small loss if not.
  else if(doInvalid)
  {
    doInvalid=false;
#pragma omp parallel if (nA >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nA))
    {
#pragma omp for
    for( OMPInt i=0; i<nA; ++i)  if (ddP[i] == invalidValue) {doInvalid=true;}
    }
  }
  // some loop constants
  SizeT dim0  = this->dim[0];
  SizeT aBeg0 = aBeg[0];
  SizeT aEnd0 = aEnd[0];
  SizeT dim0_1     = dim0 - 1;
  SizeT kDim0      = kernel->Dim( 0);
  SizeT	kDim0_nDim = kDim0 * nDim;
  
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
  bool  regArrT[ MAXRANK];for ( long aSp=0; aSp<MAXRANK; ++aSp) regArrT[aSp] = !aBeg[ aSp];
  long  aInitIxPool[nchunk][ MAXRANK+1];
  for ( long aSp=0; aSp<=nDim; ++aSp) for (int iloop=0; iloop< nchunk; ++iloop) aInitIxPool[iloop][ aSp] = 0;
  bool  regArrPool[nchunk][ MAXRANK]; 
  for ( long aSp=0; aSp<MAXRANK; ++aSp) for (int iloop=0; iloop< nchunk; ++iloop) regArrPool[iloop][ aSp] = !aBeg[ aSp];
  
   for (long iloop = 0; iloop < nchunk; ++iloop) {
   //memorize current state accelerator for chunk iloop:
   for ( long aSp=0; aSp<=nDim; ++aSp) aInitIxPool[iloop][ aSp] = aInitIxT[ aSp]; 
   for ( long aSp=0; aSp<MAXRANK; ++aSp) regArrPool[iloop][ aSp] = regArrT[ aSp];
   //store addresses in static part
   aInitIxRef[iloop]=aInitIxPool[iloop];
   regArrRef[iloop]=regArrPool[iloop];
   // continue with next table
   for (long ia = iloop*chunksize; (ia < (iloop+1)*chunksize && ia < nA) ; ia += dim0) {
    for (long aSp = 1; aSp < nDim;) {
      if (aInitIxT[ aSp] < this->dim[ aSp]) {
        regArrT[ aSp] = (aInitIxT[aSp] >= aBeg[aSp] && aInitIxT[aSp] < aEnd[ aSp]);
        break;
      }
      aInitIxT[ aSp] = 0;
      regArrT[ aSp] = !aBeg[ aSp];
      ++aInitIxT[ ++aSp];
    }
    ++aInitIxT[1];
   }
  }
  
#define INCLUDE_CONVOL_INC_CPP  //to make the include files behave.

if (normalize) {
#define CONVOL_NORMALIZE
  if (edgeMode == 0) {
    if (doInvalid && doNan) {
#define CONVOL_NAN_INVALID
      if (center) {
#define CONVOL_CENTER   // /CENTER option
#include "convol_inc0.cpp"
#undef CONVOL_CENTER   // /CENTER option
      } else {
#include "convol_inc0.cpp"
      }
#undef CONVOL_NAN_INVALID
    } else if (doInvalid) {
#define CONVOL_INVALID
      if (center) {
#define CONVOL_CENTER   // /CENTER option
#include "convol_inc0.cpp"
#undef CONVOL_CENTER   // /CENTER option
      } else {
#include "convol_inc0.cpp"
      }
#undef CONVOL_INVALID
    } else if (doNan) {
#define CONVOL_NAN
      if (center) {
#define CONVOL_CENTER   // /CENTER option
#include "convol_inc0.cpp"
#undef CONVOL_CENTER   // /CENTER option
      } else {
#include "convol_inc0.cpp"
      }
#undef CONVOL_NAN
    } else {
      if (center) {
#define CONVOL_CENTER   // /CENTER option
#include "convol_inc0.cpp"
#undef CONVOL_CENTER   // /CENTER option
      } else {
#include "convol_inc0.cpp"
      }
    }
  } 
  else if (edgeMode == 1) {
#define CONVOL_EDGE_WRAP
    if (doInvalid && doNan) {
#define CONVOL_NAN_INVALID
      if (center) {
#define CONVOL_CENTER   // /CENTER option
#include "convol_inc1.cpp"
#undef CONVOL_CENTER   // /CENTER option
      } else {
#include "convol_inc1.cpp"
      }
#undef CONVOL_NAN_INVALID
    } else if (doInvalid) {
#define CONVOL_INVALID
      if (center) {
#define CONVOL_CENTER   // /CENTER option
#include "convol_inc1.cpp"
#undef CONVOL_CENTER   // /CENTER option
      } else {
#include "convol_inc1.cpp"
      }
#undef CONVOL_INVALID
    } else if (doNan) {
#define CONVOL_NAN
      if (center) {
#define CONVOL_CENTER   // /CENTER option
#include "convol_inc1.cpp"
#undef CONVOL_CENTER   // /CENTER option
      } else {
#include "convol_inc1.cpp"
      }
#undef CONVOL_NAN
    } else {
      if (center) {
#define CONVOL_CENTER   // /CENTER option
#include "convol_inc1.cpp"
#undef CONVOL_CENTER   // /CENTER option
      } else {
#include "convol_inc1.cpp"
      }
    }
#undef CONVOL_EDGE_WRAP
  } else if (edgeMode == 2) {
#define CONVOL_EDGE_TRUNCATE
    if (doInvalid && doNan) {
#define CONVOL_NAN_INVALID
      if (center) {
#define CONVOL_CENTER   // /CENTER option
#include "convol_inc1.cpp"
#undef CONVOL_CENTER   // /CENTER option
      } else {
#include "convol_inc1.cpp"
      }
#undef CONVOL_NAN_INVALID
    } else if (doInvalid) {
#define CONVOL_INVALID
      if (center) {
#define CONVOL_CENTER   // /CENTER option
#include "convol_inc1.cpp"
#undef CONVOL_CENTER   // /CENTER option
      } else {
#include "convol_inc1.cpp"
      }
#undef CONVOL_INVALID
    } else if (doNan) {
#define CONVOL_NAN
      if (center) {
#define CONVOL_CENTER   // /CENTER option
#include "convol_inc1.cpp"
#undef CONVOL_CENTER   // /CENTER option
      } else {
#include "convol_inc1.cpp"
      }
#undef CONVOL_NAN
    } else {
      if (center) {
#define CONVOL_CENTER   // /CENTER option
#include "convol_inc1.cpp"
#undef CONVOL_CENTER   // /CENTER option
      } else {
#include "convol_inc1.cpp"
      }
    }
#undef CONVOL_EDGE_TRUNCATE
    } else if (edgeMode == 3) {
#define CONVOL_EDGE_ZERO
      if (doInvalid && doNan) {
#define CONVOL_NAN_INVALID
        if (center) {
#define CONVOL_CENTER   // /CENTER option
#include "convol_inc1.cpp"
#undef CONVOL_CENTER   // /CENTER option
        } else {
#include "convol_inc1.cpp"
        }
#undef CONVOL_NAN_INVALID
      } else if (doInvalid) {
#define CONVOL_INVALID
        if (center) {
#define CONVOL_CENTER   // /CENTER option
#include "convol_inc1.cpp"
#undef CONVOL_CENTER   // /CENTER option
        } else {
#include "convol_inc1.cpp"
        }
#undef CONVOL_INVALID
      } else if (doNan) {
#define CONVOL_NAN
        if (center) {
#define CONVOL_CENTER   // /CENTER option
#include "convol_inc1.cpp"
#undef CONVOL_CENTER   // /CENTER option
        } else {
#include "convol_inc1.cpp"
        }
#undef CONVOL_NAN
      } else {
        if (center) {
#define CONVOL_CENTER   // /CENTER option
#include "convol_inc1.cpp"
#undef CONVOL_CENTER   // /CENTER option
        } else {
#include "convol_inc1.cpp"
        }
      }
#undef CONVOL_EDGE_ZERO
    } else if (edgeMode == 4) {
#define CONVOL_EDGE_MIRROR
    if (doInvalid && doNan) {
#define CONVOL_NAN_INVALID
      if (center) {
#define CONVOL_CENTER   // /CENTER option
#include "convol_inc1.cpp"
#undef CONVOL_CENTER   // /CENTER option
      } else {
#include "convol_inc1.cpp"
      }
#undef CONVOL_NAN_INVALID
    } else if (doInvalid) {
#define CONVOL_INVALID
      if (center) {
#define CONVOL_CENTER   // /CENTER option
#include "convol_inc1.cpp"
#undef CONVOL_CENTER   // /CENTER option
      } else {
#include "convol_inc1.cpp"
      }
#undef CONVOL_INVALID
    } else if (doNan) {
#define CONVOL_NAN
      if (center) {
#define CONVOL_CENTER   // /CENTER option
#include "convol_inc1.cpp"
#undef CONVOL_CENTER   // /CENTER option
      } else {
#include "convol_inc1.cpp"
      }
#undef CONVOL_NAN
    } else {
      if (center) {
#define CONVOL_CENTER   // /CENTER option
#include "convol_inc1.cpp"
#undef CONVOL_CENTER   // /CENTER option
      } else {
#include "convol_inc1.cpp"
      }
    }
#undef CONVOL_EDGE_MIRROR
  }
#undef CONVOL_NORMALIZE
} else {
  if (edgeMode == 0) {
    if (doInvalid && doNan) {
#define CONVOL_NAN_INVALID
      if (center) {
#define CONVOL_CENTER   // /CENTER option
#include "convol_inc0.cpp"
#undef CONVOL_CENTER   // /CENTER option
      } else {
#include "convol_inc0.cpp"
      }
#undef CONVOL_NAN_INVALID
    } else if (doInvalid) {
#define CONVOL_INVALID
      if (center) {
#define CONVOL_CENTER   // /CENTER option
#include "convol_inc0.cpp"
#undef CONVOL_CENTER   // /CENTER option
      } else {
#include "convol_inc0.cpp"
      }
#undef CONVOL_INVALID
    } else if (doNan) {
#define CONVOL_NAN
      if (center) {
#define CONVOL_CENTER   // /CENTER option
#include "convol_inc0.cpp"
#undef CONVOL_CENTER   // /CENTER option
      } else {
#include "convol_inc0.cpp"
      }
#undef CONVOL_NAN
    } else {
      if (center) {
#define CONVOL_CENTER   // /CENTER option
#include "convol_inc0.cpp"
#undef CONVOL_CENTER   // /CENTER option
      } else {
#include "convol_inc0.cpp"
      }
    }
  } 
  else if (edgeMode == 1) {
#define CONVOL_EDGE_WRAP
    if (doInvalid && doNan) {
#define CONVOL_NAN_INVALID
      if (center) {
#define CONVOL_CENTER   // /CENTER option
#include "convol_inc1.cpp"
#undef CONVOL_CENTER   // /CENTER option
      } else {
#include "convol_inc1.cpp"
      }
#undef CONVOL_NAN_INVALID
    } else if (doInvalid) {
#define CONVOL_INVALID
      if (center) {
#define CONVOL_CENTER   // /CENTER option
#include "convol_inc1.cpp"
#undef CONVOL_CENTER   // /CENTER option
      } else {
#include "convol_inc1.cpp"
      }
#undef CONVOL_INVALID
    } else if (doNan) {
#define CONVOL_NAN
      if (center) {
#define CONVOL_CENTER   // /CENTER option
#include "convol_inc1.cpp"
#undef CONVOL_CENTER   // /CENTER option
      } else {
#include "convol_inc1.cpp"
      }
#undef CONVOL_NAN
    } else {
      if (center) {
#define CONVOL_CENTER   // /CENTER option
#include "convol_inc1.cpp"
#undef CONVOL_CENTER   // /CENTER option
      } else {
#include "convol_inc1.cpp"
      }
    }
#undef CONVOL_EDGE_WRAP
  } else if (edgeMode == 2) {
#define CONVOL_EDGE_TRUNCATE
    if (doInvalid && doNan) {
#define CONVOL_NAN_INVALID
      if (center) {
#define CONVOL_CENTER   // /CENTER option
#include "convol_inc1.cpp"
#undef CONVOL_CENTER   // /CENTER option
      } else {
#include "convol_inc1.cpp"
      }
#undef CONVOL_NAN_INVALID
    } else if (doInvalid) {
#define CONVOL_INVALID
      if (center) {
#define CONVOL_CENTER   // /CENTER option
#include "convol_inc1.cpp"
#undef CONVOL_CENTER   // /CENTER option
      } else {
#include "convol_inc1.cpp"
      }
#undef CONVOL_INVALID
    } else if (doNan) {
#define CONVOL_NAN
      if (center) {
#define CONVOL_CENTER   // /CENTER option
#include "convol_inc1.cpp"
#undef CONVOL_CENTER   // /CENTER option
      } else {
#include "convol_inc1.cpp"
      }
#undef CONVOL_NAN
    } else {
      if (center) {
#define CONVOL_CENTER   // /CENTER option
#include "convol_inc1.cpp"
#undef CONVOL_CENTER   // /CENTER option
      } else {
#include "convol_inc1.cpp"
      }
    }
#undef CONVOL_EDGE_TRUNCATE
  } else if (edgeMode == 3) {
#define CONVOL_EDGE_ZERO
    if (doInvalid && doNan) {
#define CONVOL_NAN_INVALID
      if (center) {
#define CONVOL_CENTER   // /CENTER option
#include "convol_inc1.cpp"
#undef CONVOL_CENTER   // /CENTER option
      } else {
#include "convol_inc1.cpp"
      }
#undef CONVOL_NAN_INVALID
    } else if (doInvalid) {
#define CONVOL_INVALID
      if (center) {
#define CONVOL_CENTER   // /CENTER option
#include "convol_inc1.cpp"
#undef CONVOL_CENTER   // /CENTER option
      } else {
#include "convol_inc1.cpp"
      }
#undef CONVOL_INVALID
    } else if (doNan) {
#define CONVOL_NAN
      if (center) {
#define CONVOL_CENTER   // /CENTER option
#include "convol_inc1.cpp"
#undef CONVOL_CENTER   // /CENTER option
      } else {
#include "convol_inc1.cpp"
      }
#undef CONVOL_NAN
    } else {
      if (center) {
#define CONVOL_CENTER   // /CENTER option
#include "convol_inc1.cpp"
#undef CONVOL_CENTER   // /CENTER option
      } else {
#include "convol_inc1.cpp"
      }
    }
#undef CONVOL_EDGE_ZERO
    } else if (edgeMode == 4) {
#define CONVOL_EDGE_MIRROR
    if (doInvalid && doNan) {
#define CONVOL_NAN_INVALID
      if (center) {
#define CONVOL_CENTER   // /CENTER option
#include "convol_inc1.cpp"
#undef CONVOL_CENTER   // /CENTER option
      } else {
#include "convol_inc1.cpp"
      }
#undef CONVOL_NAN_INVALID
    } else if (doInvalid) {
#define CONVOL_INVALID
      if (center) {
#define CONVOL_CENTER   // /CENTER option
#include "convol_inc1.cpp"
#undef CONVOL_CENTER   // /CENTER option
      } else {
#include "convol_inc1.cpp"
      }
#undef CONVOL_INVALID
    } else if (doNan) {
#define CONVOL_NAN
      if (center) {
#define CONVOL_CENTER   // /CENTER option
#include "convol_inc1.cpp"
#undef CONVOL_CENTER   // /CENTER option
      } else {
#include "convol_inc1.cpp"
      }
#undef CONVOL_NAN
    } else {
      if (center) {
#define CONVOL_CENTER   // /CENTER option
#include "convol_inc1.cpp"
#undef CONVOL_CENTER   // /CENTER option
      } else {
#include "convol_inc1.cpp"
      }
    }
#undef CONVOL_EDGE_MIRROR
  }
#undef CONVOL_NORMALIZE
}  

    
#undef INCLUDE_CONVOL_INC_CPP
  return res;
} //end of template convol

#undef CONVOL_TRUNCATE_MIN 
#undef CONVOL_TRUNCATE_MAX

#if !defined(CONVOL_BYTE__) && !defined(CONVOL_UINT__) && !defined(CONVOL_INT__) && !defined(CONVOL_ULONG__) && !defined(CONVOL_ULONG64__)

namespace lib {

/*****************************************convol_fun*********************************************************/
  BaseGDL* convol_fun( EnvT* e)
  {
    long nParam=e->NParam( 2); 

    /************************************Checking_parameters************************************************/

    BaseGDL* p0 = e->GetNumericParDefined( 0);
    if( p0->Rank() == 0) 
      e->Throw( "Expression must be an array in this context: "+
		e->GetParString(0));
    
    BaseGDL* p1 = e->GetNumericParDefined( 1);
    if( p1->Rank() == 0) 
      e->Throw( "Expression must be an array in this context: "+
		e->GetParString(1));
    
    if( p0->N_Elements() < p1->N_Elements())
      e->Throw( "Incompatible dimensions for Array and Kernel.");

    // rank 1 for kernel works always
    if( p1->Rank() != 1) {
      long rank = p0->Rank();
      if (rank != p1->Rank())
        e->Throw("Incompatible dimensions for Array and Kernel.");

      for (long r = 0; r < rank; ++r)
        if (p0->Dim(r) < p1->Dim(r))
          e->Throw("Incompatible dimensions for Array and Kernel.");
    } else { //check however that kernel is not too big...
      if (p0->Dim(0) <  p1->Dim(0)) e->Throw("Incompatible dimensions for Array and Kernel.");
    }
   
    //compute some interesting values about kernel and array dimensions
    int maxposK=0,curdimK,sumofdimsK=0,maxdimK=-1;
    int maxpos=0, curdimprod, maxdimprod=-1;

    for (int i=0; i<p1->Rank(); ++i) {
      curdimK=p1->Dim(i);
      sumofdimsK+=curdimK;
      if (curdimK>maxdimK) {
        maxdimK=curdimK;
        maxposK=i;
      }
    }
    
    // If kernel is not 1-D, test which dimension is larger. Transposing the data and kernel to have this dimension first is faster since
    // it is the kernel sum which is parallelized here.
    // Probably there is a minimum difference in size (magicfactor=1.2 ? 1.5?) at which one would benefit given the added complexity of transposition.
    bool doTranspose=false;
    if (sumofdimsK > maxdimK+(p1->Rank())-1) {
      
      // Now about dimensions.
      // convolution code (in convol_inc*.pro) is quite tricky. Only the inner part of the loop can be safely parallelized.
      // It is a double loop on the first dimension of kernel (kDim0) times the first dimension of the array (dim0 or aEnd0-aBeg0).
      // To benefit from this speedup, we need to have dim0*kDim0 maximum:
      // find largest array or kernel dimension; transpose array, makes for faster convol, will be
      // transposed back at end.

      // find maximum of dim0xkDim0
      for (int i = 0; i < p1->Rank(); ++i) { //0->Rank and p1->Rank same here
        curdimprod = p1->Dim(i)*p0->Dim(i);
        if (curdimprod > maxdimprod) {
          maxdimprod = curdimprod;
          maxpos = i;
        }
      }
      float magicfactor=2.0;
      if ( maxdimprod > magicfactor*p1->Dim(0)*p0->Dim(0) ) doTranspose=true;   
    }
    // array of dims for transpose
    DUInt* perm = new DUInt[p0->Rank()]; //direct
    DUInt* mrep = new DUInt[p0->Rank()]; //reverse
    ArrayGuard<DUInt> perm_guard(perm);
    ArrayGuard<DUInt> mrep_guard(mrep);
    if (doTranspose) {

        DUInt i = 0, j = 0;
        for (i = 0; i < p0->Rank(); ++i) if (i != maxpos) {
            perm[j + 1] = i;
            j++;
          }
        perm[0] = maxpos;
        for (i = 0; i < p0->Rank(); ++i) mrep[i]=i; //populate reverse
        for (i = 0; i < maxpos+1; ++i) mrep[i]=i+1; //this and the following line should give the reverse transpose order.
        mrep[maxpos]=0;
    }
    /***************************************Preparing_matrices*************************************************/
     
    //Computations for REAL and COMPLEX are better made in double precision if we do not want to lose precision
    //Apparently IDL has severe problems regarding this loss of precision.
    // try for example the following, which should give exactly ZERO:
    // C=32*32*0.34564 & a=findgen(100,100)*0.0+1 & b=convol(a,fltarr(32,32)+0.34564) & print,(b-c)[20:80,50],format='(4(F20.12))' 
    //So, we convert p0 to Double precision if necessary and convert back result.
    //Do it here since all other parameters are converted to  p0's type.
    Guard<BaseGDL> p0Guard;
    bool deprecise=false;
    if (p0->Type() == GDL_FLOAT) {
        p0 = p0->Convert2(GDL_DOUBLE, BaseGDL::COPY);
        p0Guard.Reset(p0);
        deprecise=true;
    } else if (p0->Type() == GDL_COMPLEX) {
        p0 = p0->Convert2(GDL_COMPLEXDBL, BaseGDL::COPY);
        p0Guard.Reset(p0);
        deprecise=true;
    }
 
    // convert kernel to array type
    Guard<BaseGDL> p1Guard;
    if (p0->Type() == GDL_BYTE || p0->Type() == GDL_UINT || p0->Type() == GDL_INT) {
      if (p1->Type() != GDL_LONG) {
        p1 = p1->Convert2(GDL_LONG, BaseGDL::COPY);
        p1Guard.Reset(p1);
      }
    } else if (p0->Type() != p1->Type()) {
      p1 = p1->Convert2(p0->Type(), BaseGDL::COPY);
      p1Guard.Reset(p1);
    }
    
    BaseGDL* scale;
    Guard<BaseGDL> scaleGuard;
    if (nParam > 2) {
      scale = e->GetParDefined(2);
      if (scale->Rank() > 0)
        e->Throw("Expression must be a scalar in this context: " +
          e->GetParString(2));

      // p1 here handles GDL_BYTE||GDL_UINT||GDL_INT case also
      if (p1->Type() != scale->Type()) {
        scale = scale->Convert2(p1->Type(), BaseGDL::COPY);
        scaleGuard.Reset(scale);
      }
    } else {
      scale = p1->New(1, BaseGDL::ZERO);
    }
    /********************************************Arguments_treatement***********************************/
    bool center = true;
    static int centerIx = e->KeywordIx( "CENTER");
    if( e->KeywordPresent( centerIx))
      {
	DLong c;
	e->AssureLongScalarKW( centerIx, c);
	center = (c != 0);
      }

    // overrides EDGE_TRUNCATE
    static int edge_wrapIx = e->KeywordIx( "EDGE_WRAP");
    bool edge_wrap = e->KeywordSet( edge_wrapIx);
    static int edge_truncateIx = e->KeywordIx( "EDGE_TRUNCATE");
    bool edge_truncate = e->KeywordSet( edge_truncateIx);
    static int edge_zeroIx = e->KeywordIx( "EDGE_ZERO");
    bool edge_zero = e->KeywordSet( edge_zeroIx);
    static int edge_mirrorIx = e->KeywordIx( "EDGE_MIRROR");
    bool edge_mirror = e->KeywordSet( edge_mirrorIx);
    int edgeMode = 0; 
    if( edge_wrap)
      edgeMode = 1;
    else if( edge_truncate)
      edgeMode = 2;
    else if( edge_zero)
      edgeMode = 3;
    else if(edge_mirror)
      edgeMode = 4;
    
    // p0, p1 and scale have same type
    // p1 has rank of 1 or same rank as p0 with each dimension smaller than p0
    // scale is a scalar

    /***********************************Parameter_BIAS**************************************/
    static int biasIx = e->KeywordIx("BIAS");
    bool statusBias = e->KeywordPresent(biasIx);
    //    DLong bias=0;
    BaseGDL* bias;
    Guard<BaseGDL> biasGuard;
    if (statusBias) {
      bias = e->GetKW(biasIx);

      // p1 here handles GDL_BYTE||GDL_UINT||GDL_INT case also
      if (p1->Type() != bias->Type()) {
        bias = bias->Convert2(p1->Type(), BaseGDL::COPY);
        biasGuard.Reset(bias);
      }
    } else bias = p1->New(1, BaseGDL::ZERO);

    /***********************************Parameter_Normalize**********************************/

    static int normalIx = e->KeywordIx( "NORMALIZE");
    bool normalize = e->KeywordPresent( normalIx);
    
    /***********************************Parameter NAN****************************************/

    static int nanIx = e->KeywordIx( "NAN");
    bool doNan = e->KeywordSet( nanIx);
    
    /***********************************Parameter MISSING************************************/
    static int missingIx = e->KeywordIx("MISSING");
    bool doMissing = e->KeywordPresent(missingIx);
    BaseGDL* missing;
    Guard<BaseGDL> missGuard;
    if (doMissing) {
      missing = e->GetKW(missingIx);
      if (p0->Type() != missing->Type()) {
        missing = missing->Convert2(p0->Type(), BaseGDL::COPY);
        missGuard.Reset(missing);
      }
    } else missing = p0->New(1, BaseGDL::ZERO);
   /***********************************Parameter INVALID************************************/
    static int invalidIx = e->KeywordIx("INVALID");
    bool doInvalid = e->KeywordPresent( invalidIx );
    BaseGDL* invalid;
    Guard<BaseGDL> invalGuard;
    if (doInvalid) {
        invalid = e->GetKW(invalidIx);
        if (p0->Type() != invalid->Type()) {
          invalid = invalid->Convert2(p0->Type(), BaseGDL::COPY);
          invalGuard.Reset(invalid);
        }
    } else invalid = p0->New(1, BaseGDL::ZERO);
    if (!doNan && !doInvalid) doMissing=false;
    if (!doMissing && p0->Type()==GDL_FLOAT)
      missing = SysVar::Values()->GetTag(SysVar::Values()->Desc()->TagIndex("F_NAN"), 0);
    if (!doMissing && p0->Type()==GDL_DOUBLE)
      missing = SysVar::Values()->GetTag(SysVar::Values()->Desc()->TagIndex("D_NAN"), 0);
    //populating a Complex with Nans is not easy as there is no objective method for that.
    if (!doMissing && p0->Type()==GDL_COMPLEX) {
      DComplex tmp;
      tmp.real()=tmp.imag()=std::numeric_limits<float>::quiet_NaN();
      memcpy((*missing).DataAddr(), &tmp, sizeof(tmp));
    }
    if (!doMissing && p0->Type()==GDL_COMPLEXDBL) {
      DComplexDbl tmp;
      tmp.real()=tmp.imag()=std::numeric_limits<double>::quiet_NaN();
      memcpy((*missing).DataAddr(), &tmp, sizeof(tmp));
    }
    BaseGDL* result;
    //handle transpositions
    if (doTranspose) {
      BaseGDL* input;
      Guard<BaseGDL> inputGuard;
      input = p0->Transpose(perm);
      inputGuard.Reset(input);
      BaseGDL* transpP1;
      Guard<BaseGDL> transpP1Guard;
      transpP1=p1->Transpose(perm);
      transpP1Guard.Reset(transpP1);
      result=input->Convol(transpP1, scale, bias, center, normalize, edgeMode, doNan, missing, doMissing, invalid, doInvalid)->Transpose(mrep);
    } else result=p0->Convol( p1, scale, bias, center, normalize, edgeMode, doNan, missing, doMissing, invalid, doInvalid);
    
    if (deprecise) {
      Guard<BaseGDL> resultGuard;
      resultGuard.reset(result);
      if (p0->Type() == GDL_DOUBLE) return result->Convert2(GDL_FLOAT, BaseGDL::COPY);
      else if (p0->Type() == GDL_COMPLEXDBL) return result->Convert2(GDL_COMPLEX, BaseGDL::COPY);
      else return result; //should not happen!
    } else 
      
      return result; 
    
  } //end of convol_fun


  }//end of namespace

#endif

#endif // #ifdef INCLUDE_CONVOL_CPP
