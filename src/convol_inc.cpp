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
                                 BaseGDL* invalid, bool doInvalid, DDouble edgeVal)
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
                                 BaseGDL* invalid, bool doInvalid, DDouble edgeVal)
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
                                 BaseGDL* invalid, bool doInvalid, DDouble edgeVal)
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
                                 BaseGDL* invalid, bool doInvalid, DDouble edgeVal)
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
                                 BaseGDL* invalid, bool doInvalid, DDouble edgeVal)
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


template<>
BaseGDL* Data_<Sp>::Convol( BaseGDL* kIn, BaseGDL* scaleIn, BaseGDL* biasIn,
			    bool center, bool normalize, int edgeMode,
                            bool doNan, BaseGDL* missing, bool doMissing,
                            BaseGDL* invalid, bool doInvalid, DDouble edgeVal)
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
    if ((GDL_NTHREADS=parallelize( nA))==1) {
      for (SizeT i = 0; i < nA; ++i) {
        if (!gdlValid(ddP[i])) {
          doNan = true;
        }
        if (ddP[i] == invalidValue) {
          doInvalid = true;
        }
      }      
    } else {
    TRACEOMP(__FILE__,__LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < nA; ++i) {
        if (!gdlValid(ddP[i])) {
          doNan = true;
        }
        if (ddP[i] == invalidValue) {
          doInvalid = true;
        }
      }
    }
  }
  else if(doNan)
  {
    doNan = false;
    if ((GDL_NTHREADS=parallelize( nA))==1) {
    for(SizeT i=0; i<nA; ++i)  if (!gdlValid(ddP[i])) {doNan=true;}
    } else {
      TRACEOMP(__FILE__,__LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
    for( OMPInt i=0; i<nA; ++i)  if (!gdlValid(ddP[i])) {doNan=true;}
    }
  }
//same for invalid. a real gain of time if no values are invalid, a small loss if not.
  else if(doInvalid)
  {
    doInvalid=false;
    if ((GDL_NTHREADS=parallelize( nA))==1) {
    for(SizeT i=0; i<nA; ++i)  if (ddP[i] == invalidValue) {doInvalid=true;}
    } else {
#pragma omp parallel for num_threads(GDL_NTHREADS)
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
  GDL_NTHREADS=parallelize( nA, TP_MEMORY_ACCESS);
  chunksize=nA/((GDL_NTHREADS>32)?32:GDL_NTHREADS);
  long n_dim0=chunksize/dim0; chunksize=dim0*n_dim0; //ensures chunksize integer number of dim0.
  if (chunksize>0) {
    nchunk=nA/chunksize;
    if (chunksize*nchunk < nA) ++nchunk;
  } else {nchunk=1; chunksize=nA;}
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

#endif // #ifdef INCLUDE_CONVOL_CPP
