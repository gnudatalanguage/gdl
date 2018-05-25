/***************************************************************************
                          smooth.cpp  -  mathematical GDL library function
                             -------------------
    begin                : 05 September 2014
    copyright            : (C) 2014 by Levan Loria  (with Alain Coulais)
                         : (C) 2017 by Gilles Duvert 
    email                : alaingdl@users.sourceforge.net
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

#include "nullgdl.hpp"
#include "dstructgdl.hpp"
#include "dinterpreter.hpp"
#ifdef _MSC_VER
#define isfinite _finite
#define std__isnan isnan
#else
#define std__isnan std::isnan
#endif

using std::isfinite;

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

//compute index when srcDim of rank rank is tranposed by [1,2,3...,0]. destStride must have been computed externally (accelerator).
inline static SizeT transposed1Index(const SizeT inputindex, const SizeT* srcDim, const SizeT* destStride, const long rank)
{
  SizeT dim[MAXRANK];
  SizeT index=inputindex;
  SizeT sizeleft = index;
  SizeT dim0;
  for (SizeT i = 0; i < rank; ++i) {
    dim0= srcDim[i];
    sizeleft /= dim0;
    dim[i] = index - sizeleft * dim0; //the corresponding index in src data.
    index = sizeleft;
  }
  SizeT ix = 0;
  for (SizeT i = 0; i < rank-1; ++i) ix += dim[i+1] * destStride[i]; ix+=dim[0]*destStride[rank-1]; //note dim has been rotated by 1

  return ix;
}
// include repeatedly smooth_inc for all useful types.

#define SMOOTH_Ty DByte
#define SMOOTH_SP SpDByte
#include "smooth_inc.cpp"
#undef SMOOTH_Ty
#undef SMOOTH_SP

#define SMOOTH_Ty DInt
#define SMOOTH_SP SpDInt
#include "smooth_inc.cpp"
#undef SMOOTH_Ty
#undef SMOOTH_SP

#define SMOOTH_Ty DUInt
#define SMOOTH_SP SpDUInt
#include "smooth_inc.cpp"
#undef SMOOTH_Ty
#undef SMOOTH_SP

#define SMOOTH_Ty DLong
#define SMOOTH_SP SpDLong
#include "smooth_inc.cpp"
#undef SMOOTH_Ty
#undef SMOOTH_SP

#define SMOOTH_Ty DULong
#define SMOOTH_SP SpDULong
#include "smooth_inc.cpp"
#undef SMOOTH_Ty
#undef SMOOTH_SP

#define SMOOTH_Ty DLong64
#define SMOOTH_SP SpDLong64
#include "smooth_inc.cpp"
#undef SMOOTH_Ty
#undef SMOOTH_SP

#define SMOOTH_Ty DULong64
#define SMOOTH_SP SpDULong64
#include "smooth_inc.cpp"
#undef SMOOTH_Ty
#undef SMOOTH_SP

#define SMOOTH_Ty DFloat
#define SMOOTH_SP SpDFloat
#include "smooth_inc.cpp"
#undef SMOOTH_Ty
#undef SMOOTH_SP

#define SMOOTH_Ty DDouble
#define SMOOTH_SP SpDDouble
#include "smooth_inc.cpp"
#undef SMOOTH_Ty
#undef SMOOTH_SP

template<>
BaseGDL* Data_<SpDString>::Smooth( DLong* width, int edgeMode,
                                   bool doNan, BaseGDL* missing)
{
  throw GDLException("String expression not allowed in this context.");
}
template<>
BaseGDL* Data_<SpDObj>::Smooth( DLong* width, int edgeMode,
                                bool doNan, BaseGDL* missing)
{
  throw GDLException("Object expression not allowed in this context.");
}
template<>
BaseGDL* Data_<SpDPtr>::Smooth( DLong* width, int edgeMode,
                                bool doNan, BaseGDL* missing)
{
  throw GDLException("Pointer expression not allowed in this context.");
}
template<>
BaseGDL* Data_<SpDComplexDbl>::Smooth( DLong* width, int edgeMode,
                                bool doNan, BaseGDL* missing)
{
  Ty missingValue = (*static_cast<Data_*>( missing))[0];
  DDoubleGDL* missr=new DDoubleGDL(missingValue.real());
  DDoubleGDL* missi=new DDoubleGDL(missingValue.imag());
  Data_* res = this->Dup();
  DDoubleGDL* re=new DDoubleGDL(this->Dim(), BaseGDL::NOZERO);
  for (SizeT i=0; i< this->N_Elements(); ++i) (*re)[i]=(*this)[i].real();
  BaseGDL* resr=re->Smooth(width, edgeMode, doNan, missr);
  DDoubleGDL* im=new DDoubleGDL(this->Dim(), BaseGDL::NOZERO);
  for (SizeT i=0; i< this->N_Elements(); ++i) (*im)[i]=(*this)[i].imag();
  BaseGDL* resi=im->Smooth(width, edgeMode, doNan, missi);
  DDouble* dresi=(DDouble*)resi->DataAddr();  
  DDouble* dresr=(DDouble*)resr->DataAddr();  
  for (SizeT i=0; i< this->N_Elements(); ++i) (*res)[i]=std::complex<DDouble>(dresr[i], dresr[i]);
  GDLDelete (resr);
  GDLDelete (re);
  GDLDelete (missr);
  GDLDelete (resi);
  GDLDelete (im);
  GDLDelete (missi);
  return res;
}
template<>
BaseGDL* Data_<SpDComplex>::Smooth( DLong* width, int edgeMode,
                                bool doNan, BaseGDL* missing)
{
  Ty missingValue = (*static_cast<Data_*>( missing))[0];
  DFloatGDL* missr=new DFloatGDL(missingValue.real());
  DFloatGDL* missi=new DFloatGDL(missingValue.imag());
  Data_* res = this->Dup();
  DFloatGDL* re=new DFloatGDL(this->Dim(), BaseGDL::NOZERO);
  for (SizeT i=0; i< this->N_Elements(); ++i) (*re)[i]=(*this)[i].real();
  BaseGDL* resr=re->Smooth(width, edgeMode, doNan, missr);
  DFloatGDL* im=new DFloatGDL(this->Dim(), BaseGDL::NOZERO);
  for (SizeT i=0; i< this->N_Elements(); ++i) (*im)[i]=(*this)[i].imag();
  BaseGDL* resi=im->Smooth(width, edgeMode, doNan, missi);
  DFloat* fresi=(DFloat*)resi->DataAddr();  
  DFloat* fresr=(DFloat*)resr->DataAddr();  
  for (SizeT i=0; i< this->N_Elements(); ++i) (*res)[i]=std::complex<DFloat>(fresr[i], fresr[i]);
  GDLDelete (resr);
  GDLDelete (re);
  GDLDelete (missr);
  GDLDelete (resi);
  GDLDelete (im);
  GDLDelete (missi);
  return res;
}

#include "datatypes.hpp"
#include "envt.hpp"

#include "smooth.hpp"

namespace lib {

using namespace std;
using namespace antlr;

BaseGDL* smooth_fun( EnvT* e)
  {
    long nParam=e->NParam( 2); 

    /************************************Checking_parameters************************************************/

    BaseGDL* p0 = e->GetNumericParDefined(0);
    int rank=p0->Rank();
    if (rank == 0)
      e->Throw("Expression must be an array in this context: " +
      e->GetParString(0));

    //smooth width should have proper dimensions, etc and be odd
    DLong width[MAXRANK];
    
    BaseGDL* p1 = e->GetNumericParDefined(1);
    if (p1->Rank() != 0) {
      if (rank != p1->N_Elements())
        e->Throw("Number of Array dimensions does not match number of Widths.");

      DLongGDL* gdlwidth = e->GetParAs<DLongGDL>(1);
      for (long r = 0; r < rank; ++r) {
        width[r] = (*gdlwidth)[r];
        if (((width[r]) % 2) == 0) width[r] += 1;
      }
    } else {
      DLongGDL* gdlwidth = e->GetParAs<DLongGDL>(1);
      for (long r = 0; r < rank; ++r) {
        width[r] = (*gdlwidth)[0];
        if (((width[r]) % 2) == 0) width[r] += 1;
      }
    }
     for (long r = 0; r < rank; ++r) if (p0->Dim(r) < width[r])  e->Throw("Width must be nonnegative and smaller than array dimensions");
    /********************************************Arguments_treatement***********************************/

    // overrides EDGE_NOTHING
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
    
    //Nan can be ignored for non floating point data:
    if (IntType(p0->Type())) doNan=false;
    
    if (!doNan) doMissing=false;
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
      DComplex tmp;
      DFloat tmpNaN;
      tmpNaN=std::numeric_limits<float>::quiet_NaN();
      tmp=complex<float>(tmpNaN,tmpNaN);
      // real(tmp)=std::numeric_limits<double>::quiet_NaN();
      // imag(tmp)=std::numeric_limits<double>::quiet_NaN();
      memcpy((*missing).DataAddr(), &tmp, sizeof(tmp));
    }
    if (!doMissing && p0->Type()==GDL_COMPLEXDBL) {
      DComplexDbl tmp;
      DDouble tmpNaN;
      tmpNaN=std::numeric_limits<double>::quiet_NaN();
      tmp=complex<double>(tmpNaN,tmpNaN);
      // tmp.real()=std::numeric_limits<double>::quiet_NaN();
      // tmp.imag()=std::numeric_limits<double>::quiet_NaN();
      memcpy((*missing).DataAddr(), &tmp, sizeof(tmp));
    }

    return p0->Smooth( width, edgeMode, doNan, missing);
  } //end of smooth_fun

  }//end of namespace
