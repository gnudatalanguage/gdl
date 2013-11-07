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


#ifdef CONVOL_BYTE__

template<>
BaseGDL* Data_<SpDByte>::Convol( BaseGDL* kIn, BaseGDL* scaleIn, BaseGDL* bias, 
				 bool center, bool normalize, int edgeMode,
                                 bool doNan, BaseGDL* missing, bool doMissing,
                                 BaseGDL* invalid, bool doInvalid)
{
  Data_<SpDLong>* kernel = static_cast<Data_<SpDLong>*>( kIn);
  DLong scale = (*static_cast<Data_<SpDInt>*>( scaleIn))[0];
  // the result to be returned
  Data_* res = New( dim, BaseGDL::ZERO);
  DInt* ker = static_cast<DInt*>( kernel->DataAddr());
  // DLong* biasd=static_cast<DLong*>( bias);
  Data_<SpDLong>* biast=static_cast<Data_<SpDLong>*>( bias);
  DLong* biasd = static_cast<DLong*>( biast->DataAddr());
  DLong missingValue = *(static_cast<DLong*>( missing->DataAddr()));
  DLong invalidValue = *(static_cast<DLong*>( invalid->DataAddr()));
#else

#ifdef CONVOL_UINT__

template<>
BaseGDL* Data_<SpDUInt>::Convol( BaseGDL* kIn, BaseGDL* scaleIn, BaseGDL* bias, 
				 bool center, bool normalize, int edgeMode,
                                 bool doNan, BaseGDL* missing, bool doMissing,
                                 BaseGDL* invalid, bool doInvalid)
{
  Data_* kernel = static_cast<Data_*>( kIn);
  DLong scale = (*static_cast<Data_<SpDUInt>*>( scaleIn))[0];
  // the result to be returned
  Data_* res = New( dim, BaseGDL::ZERO);
  // DLong* ker = static_cast<DLong*>( kernel->DataAddr());
  Ty* ker = &(*kernel)[0];
  //  DLongGDL* biasd=static_cast<DLong*>( bias);
  Data_* biast=static_cast<Data_*>( bias);
  Ty* biasd = &(*biast)[0];
  Ty missingValue = (*static_cast<Data_*>( missing))[0];
  Ty invalidValue = (*static_cast<Data_*>( invalid))[0];
#else


template<class Sp>
BaseGDL* Data_<Sp>::Convol( BaseGDL* kIn, BaseGDL* scaleIn, BaseGDL* bias,
			    bool center, bool normalize, int edgeMode,
                            bool doNan, BaseGDL* missing, bool doMissing,
                            BaseGDL* invalid, bool doInvalid)
{
  Data_* kernel = static_cast<Data_*>( kIn);
  Ty scale = (*static_cast<Data_*>( scaleIn))[0];
  // the result to be returned
  Data_* res = New( this->dim, BaseGDL::ZERO);
  Ty* ker = &(*kernel)[0];
  Data_* biast=static_cast<Data_*>( bias);
  Ty* biasd = &(*biast)[0];
  Ty missingValue = (*static_cast<Data_*>( missing))[0];
  Ty invalidValue = (*static_cast<Data_*>( invalid))[0];
#endif
#endif
  
  if( scale == this->zero) scale = 1;

  SizeT nA = N_Elements();
  SizeT nK = kernel->N_Elements();

  if(normalize)
    { 

      DDouble tmp=0;
      for ( SizeT ind=0; ind<nK; ind++ )
	tmp+=abs(ker[ind]);
      scale=tmp;

#ifdef CONVOL_BYTE__
	  tmp=0;
	  for ( SizeT ind=0; ind<nK; ind++ )
	    if(ker[ind]<0)
	      tmp+=abs(ker[ind]);
	  biasd[0]=tmp*255/scale;
	  if( biasd[0]<0)
	    biasd[0]=0;
	  else  
	    if( biasd[0]>255) 
	      biasd[0]=255;
#endif

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

  // some loop constants
  SizeT dim0  = this->dim[0];
  SizeT aBeg0 = aBeg[0];
  SizeT aEnd0 = aEnd[0];
  SizeT dim0_1     = dim0 - 1;
  SizeT dim0_aEnd0 = dim0 - aEnd[0];
  SizeT kDim0      = kernel->Dim( 0);
  SizeT	kDim0_nDim = kDim0 * nDim;

#define INCLUDE_CONVOL_INC_CPP 
#define CONVERT_CONVOL_TO_BYTE  if(res_a>0){if(res_a<255){(*res)[a]=res_a;}else{(*res)[a]=255;}}else{(*res)[a]=0;}

  if( edgeMode == 0)
    {
#include "convol_inc0.cpp"
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
#undef CONVOL_EDGE_ZERO
    }
#undef CONVERT_CONVOL_TO_BYTE 
#undef INCLUDE_CONVOL_INC_CPP


  if(biasd[0]!=this->zero)
    {
      for(SizeT indi=0;indi<nA;indi++)
	(*res)[indi]+=biasd[0];
    }

  return res;
 }//end of template convol

#if !defined(CONVOL_BYTE__) && !defined(CONVOL_UINT__)

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
    if( p0->Type() == GDL_BYTE)
      {
	if( p1->Type() != GDL_INT)
	  {
	    p1 = p1->Convert2( GDL_INT, BaseGDL::COPY); 
	    p1Guard.Reset( p1);
	  }
      }
    else if( p0->Type() != p1->Type())
      {
	p1 = p1->Convert2( p0->Type(), BaseGDL::COPY); 
	p1Guard.Reset( p1);
      }

    BaseGDL* scale;
    Guard<BaseGDL> scaleGuard;
    if( nParam > 2)
      {
	scale = e->GetParDefined( 2);
	if( scale->Rank() > 0) 
	  e->Throw( "Expression must be a scalar in this context: "+
		    e->GetParString(2));

	// p1 here handles GDL_BYTE case also
	if( p1->Type() != scale->Type())
	  {
	    scale = scale->Convert2( p1->Type(),BaseGDL::COPY); 
	    scaleGuard.Reset( scale);
	  }
      }
    else
      {
	scale = p1->New( dimension(), BaseGDL::ZERO);
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
    bool statusBias = e->KeywordPresent( biasIx );
    //    DLong bias=0;
    BaseGDL* bias;
    if(statusBias)
      {
	bias=e->GetKW( biasIx);

	if( p0->Type() != bias->Type())
	  {
	    bias = bias->Convert2( p0->Type(), BaseGDL::COPY); 
	  }
      }
    else bias=p1->New( 1,BaseGDL::ZERO);

    /***********************************Parameter_Normalize**********************************/

    static int normalIx = e->KeywordIx( "NORMALIZE");
    bool normalize = e->KeywordPresent( normalIx);
    
    /***********************************Parameter NAN****************************************/

    static int nanIx = e->KeywordIx( "NAN");
    bool doNan = e->KeywordPresent( nanIx);
    
    /***********************************Parameter MISSING************************************/
    static int missingIx = e->KeywordIx("MISSING");
    bool doMissing = e->KeywordPresent( missingIx );
    BaseGDL* missing;
    if (p0->Type() != GDL_BYTE) {
        if (doMissing) {
            missing = e->GetKW(missingIx);
            if (p0->Type() != missing->Type()) {
                missing = missing->Convert2(p0->Type(), BaseGDL::COPY);
            }
        } else missing = p1->New(1, BaseGDL::ZERO);
    } else {
        if (doMissing) { missing = e->GetKW(missingIx);
        } else missing = p1->New(1, BaseGDL::ZERO);
        missing = missing->Convert2(GDL_LONG, BaseGDL::COPY);
    }
   /***********************************Parameter INVALID************************************/
    static int invalidIx = e->KeywordIx("INVALID");
    bool doInvalid = e->KeywordPresent( invalidIx );
    BaseGDL* invalid;
    if (p0->Type() != GDL_BYTE) {
        if (doInvalid) {
            invalid = e->GetKW(invalidIx);
            if (p0->Type() != invalid->Type()) {
                invalid = invalid->Convert2(p0->Type(), BaseGDL::COPY);
            }
        } else invalid = p1->New(1, BaseGDL::ZERO);
    } else {
        if (doInvalid) {
            invalid = e->GetKW(invalidIx);
        } else invalid = p1->New(1, BaseGDL::ZERO);
        invalid = invalid->Convert2(GDL_LONG, BaseGDL::COPY);
    }
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
