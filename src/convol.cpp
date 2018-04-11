/***************************************************************************
                          convol.cpp  -  convol(), defines all datatypes
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
                         :     2018 by Gilles Duvert
    email                : m_schellens@users.sf.net
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "includefirst.hpp"

#if defined(USE_PYTHON) || defined(PYTHON_MODULE)
#include <numpy/arrayobject.h>
#endif

#include "nullgdl.hpp"
#include "dstructgdl.hpp"
#include "dinterpreter.hpp"

template<typename T>
inline bool gdlValid( const T &value )
{
    T max_value = std::numeric_limits<T>::max();
    T min_value = - max_value;
    return ( ( min_value <= value && value <= max_value ) &&  (value == value));
}
inline bool gdlValid( const DComplex &value )
{
    DFloat max_value = std::numeric_limits<DFloat>::max();
    DFloat min_value = - max_value;
    return ( ( min_value <= value.real() && value.real() <= max_value ) &&  (value.real() == value.real()))&&
            ( ( min_value <= value.imag() && value.imag() <= max_value ) && (value.imag() == value.imag()));
}
inline bool gdlValid( const DComplexDbl &value )
{
    DDouble max_value = std::numeric_limits<DDouble>::max();
    DDouble min_value = - max_value;
    return ( ( min_value <= value.real() && value.real() <= max_value ) &&  (value.real() == value.real()))&&
            ( ( min_value <= value.imag() && value.imag() <= max_value ) &&  (value.imag() == value.imag()));
}

template<>
BaseGDL* Data_<SpDString>::Convol( BaseGDL* kIn, BaseGDL* scaleIn, BaseGDL* bias,
 				   bool center, bool normalize, int edgeMode,
                                   bool doNan, BaseGDL* missing, bool doMissing,
                                   BaseGDL* invalid, bool doInvalid)
{
  throw GDLException("String expression not allowed in this context.");
}
template<>
BaseGDL* Data_<SpDObj>::Convol( BaseGDL* kIn, BaseGDL* scaleIn, BaseGDL* bias,
 				bool center, bool normalize, int edgeMode,
                                bool doNan, BaseGDL* missing, bool doMissing,
                                BaseGDL* invalid, bool doInvalid)
{
  throw GDLException("Object expression not allowed in this context.");
}
template<>
BaseGDL* Data_<SpDPtr>::Convol( BaseGDL* kIn, BaseGDL* scaleIn,BaseGDL* bias,
 				bool center, bool normalize, int edgeMode,
                                bool doNan, BaseGDL* missing, bool doMissing,
                                BaseGDL* invalid, bool doInvalid)
{
  throw GDLException("Pointer expression not allowed in this context.");
}

#define INCLUDE_CONVOL_CPP 1
#define CONVOL_BYTE__
#include "convol_inc.cpp"
#undef CONVOL_BYTE__

#define CONVOL_UINT__
#include "convol_inc.cpp"
#undef CONVOL_UINT__

#define CONVOL_INT__
#include "convol_inc.cpp"
#undef CONVOL_INT__

#define CONVOL_ULONG__
#include "convol_inc.cpp"
#undef CONVOL_ULONG__

#define CONVOL_ULONG64__
#include "convol_inc.cpp"
#undef CONVOL_ULONG64__

#define Sp SpDLong64
#define Ty DLong64
#include "convol_inc.cpp"
#undef Sp
#undef Ty

#define Sp SpDLong
#define Ty DLong
#include "convol_inc.cpp"
#undef Sp
#undef Ty

#define Sp SpDFloat
#define Ty DFloat
#include "convol_inc.cpp"
#undef Sp
#undef Ty

#define Sp SpDDouble
#define Ty DDouble
#include "convol_inc.cpp"
#undef Sp
#undef Ty

#define Sp SpDComplex
#define Ty DComplex
#include "convol_inc.cpp"
#undef Sp
#undef Ty

#define Sp SpDComplexDbl
#define Ty DComplexDbl
#include "convol_inc.cpp"
#undef Sp
#undef Ty

#undef INCLUDE_CONVOL_CPP

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
    if (!doMissing && p0->Type()==GDL_FLOAT) {
      DFloat tmp=std::numeric_limits<float>::quiet_NaN();
      memcpy((*missing).DataAddr(), &tmp, sizeof(tmp));
    }
    if (!doMissing && p0->Type()==GDL_DOUBLE){
      DDouble tmp=std::numeric_limits<double>::quiet_NaN();
      memcpy((*missing).DataAddr(), &tmp, sizeof(tmp));
    }
    //populating a Complex with Nans is not easy as there is no objective method for that.
    if (!doMissing && p0->Type()==GDL_COMPLEX) {
      DComplex tmp=std::complex<DFloat>(std::numeric_limits<float>::quiet_NaN(),std::numeric_limits<float>::quiet_NaN());
      memcpy((*missing).DataAddr(), &tmp, sizeof(tmp));
    }
    if (!doMissing && p0->Type()==GDL_COMPLEXDBL) {
      DComplexDbl tmp=std::complex<DDouble>(std::numeric_limits<double>::quiet_NaN(),std::numeric_limits<double>::quiet_NaN());
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