/***************************************************************************
                          convol.cpp  -  convol function
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


// optimized version

// to be included from datatypes.cpp (twice)
#ifdef INCLUDE_CONVOL_CPP

#ifndef CONVOL_COMMON__
#define CONVOL_COMMON__

// common used functions

#endif //#define CONVOL_COMMON__



using namespace std;


#define CONVERT_CONVOL_TO_ORIG   if(res_a>CONVOL_TRUNCATE_MIN){if(res_a<CONVOL_TRUNCATE_MAX){(*res)[a]=res_a;}else{(*res)[a]=CONVOL_TRUNCATE_MAX;}}else{(*res)[a]=CONVOL_TRUNCATE_MIN;}
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
  Ty invalidValue = (*static_cast<Data_*>( invalid))[0];
  

  SizeT nA = N_Elements();
  SizeT nK = kernel->N_Elements();

  if(normalize)
    { 
      scale = this->zero;
      for ( SizeT ind=0; ind<nK; ind++ )
      { //abs(kern) needed when normalizing:
        absker[ind]=abs(ker[ind]);
        scale+=absker[ind];
      }
      bias=this->zero;
#if defined(CONVOL_BYTE__)||defined (CONVOL_UINT__)
      DDouble tmp=0; 
      for ( SizeT ind=0; ind<nK; ind++ ) { if(ker[ind]<0) biasker[ind]=absker[ind]; tmp+=biasker[ind];}
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

  // setup kIxArr[ nDim * nK] the offset array
  // this handles center
  long* kIxArr = new long[ nDim * nK];
  ArrayGuard<long> kIxArrGuard( kIxArr); // guard it
  for( SizeT k=0; k<nK; ++k)
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

  long  aInitIx[ MAXRANK+1];
  for( SizeT aSp=0; aSp<=nDim; ++aSp) aInitIx[ aSp] = 0;

  bool  regArr[ MAXRANK];

  long  aBeg[ MAXRANK];
  long  aEnd[ MAXRANK];
  for( SizeT aSp=0; aSp<nDim; ++aSp) 
    {
      SizeT kDim = kernel->Dim( aSp);
      if( kDim == 0) kDim = 1;
      aBeg[ aSp] = (center) ? kDim/2 : kDim-1; // >= 
      regArr[ aSp] = !aBeg[ aSp];
      aEnd[ aSp] = (center) ? this->dim[aSp]-(kDim-1)/2 : this->dim[aSp]; // <
    }

  Ty* ddP = &(*this)[0];
  
//test if array has nans when donan is present (treatment would be shorter if array had no nans)
  if(doNan)
  {
    doNan=false;
#pragma omp parallel if (nA >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nA))
    {
#pragma omp for
    for( OMPInt i=0; i<nA; ++i)  if (!gdlValid(ddP[i])) {doNan=true;}
    }
  }
//same for invalid. a real gain of time if no values are invalid, a small loss if not.
  if(doInvalid)
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
  SizeT dim0_aEnd0 = dim0 - aEnd[0];
  SizeT kDim0      = kernel->Dim( 0);
  SizeT	kDim0_nDim = kDim0 * nDim;


#define INCLUDE_CONVOL_INC_CPP 
          
  if( edgeMode == 0)
    {
    if (!doInvalid && !doNan) {
        //special version to speed up in this case
#include "convol_inc2.cpp"
    } else {
#include "convol_inc0.cpp"
    }
  }
  else if( edgeMode == 1)
    {
#define CONVOL_EDGE_WRAP
#include "convol_inc1.cpp"
#undef CONVOL_EDGE_WRAP
    }
  else if( edgeMode == 2)
    {
#define CONVOL_EDGE_TRUNCATE
#include "convol_inc1.cpp"
#undef CONVOL_EDGE_TRUNCATE
    }
  else if( edgeMode == 3)
    {
#define CONVOL_EDGE_ZERO
#include "convol_inc1.cpp"
#include "basegdl.hpp"
#include "envt.hpp"
#undef CONVOL_EDGE_ZERO
    }
#undef INCLUDE_CONVOL_INC_CPP

  return res;
 }//end of template convol
#undef CONVOL_TRUNCATE_MIN 
#undef CONVOL_TRUNCATE_MAX

#if !defined(CONVOL_BYTE__) && !defined(CONVOL_UINT__) && !defined(CONVOL_INT__) && !defined(CONVOL_ULONG__) && !defined(CONVOL_ULONG64__)

namespace lib {

/*****************************************convol_fun*********************************************************/
  BaseGDL* convol_fun( EnvT* e)
  {
    SizeT nParam=e->NParam( 2); 

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
    if( p1->Rank() != 1)
      {
	SizeT rank = p0->Rank();
	if( rank != p1->Rank())
	  e->Throw( "Incompatible dimensions for Array and Kernel.");

	for( SizeT r=0; r<rank; ++r)
	  if( p0->Dim( r) < p1->Dim( r))
	    e->Throw( "Incompatible dimensions for Array and Kernel.");
      }


    /***************************************Preparing_matrices*************************************************/
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
    int edgeMode = 0; 
    if( edge_wrap)
      edgeMode = 1;
    else if( edge_truncate)
      edgeMode = 2;
    else if( edge_zero)
      edgeMode = 3;
    
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
    bool doMissing = e->KeywordSet(missingIx);
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
    bool doInvalid = e->KeywordSet( invalidIx );
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
    if (!doMissing && (p0->Type()==GDL_FLOAT ||p0->Type()==GDL_COMPLEX))
      missing = SysVar::Values()->GetTag(SysVar::Values()->Desc()->TagIndex("F_NAN"), 0);
    if (!doMissing && (p0->Type()==GDL_DOUBLE ||p0->Type()==GDL_COMPLEXDBL))
      missing = SysVar::Values()->GetTag(SysVar::Values()->Desc()->TagIndex("D_NAN"), 0);
    
    return p0->Convol( p1, scale, bias, center, normalize, edgeMode, doNan, missing, doMissing, invalid,doInvalid);
  } //end of convol_fun


  }//end of namespace

#endif

#endif // #ifdef INCLUDE_CONVOL_CPP
