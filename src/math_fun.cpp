/***************************************************************************
                          math_fun.cpp  -  mathematical GDL library function
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
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

#include <complex>
#include <cmath>

#include <gsl/gsl_sf.h>
#include <gsl/gsl_sf_laguerre.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_cdf.h>

#include "datatypes.hpp"
#include "envt.hpp"
#include "math_utl.hpp"
#include "math_fun.hpp"

//#define GDL_DEBUG
#undef GDL_DEBUG

namespace lib {

  using namespace std;

  template< typename T>
  BaseGDL* sin_fun_template( BaseGDL* p0)
  {
    T* p0C = static_cast<T*>( p0);
    T* res = new T( p0C->Dim(), BaseGDL::NOZERO);
    SizeT nEl = p0->N_Elements();
    for( SizeT i=0; i<nEl; ++i)
      {
	(*res)[ i] = sin((*p0C)[ i]); 
      }
    return res;
  }

  BaseGDL* sin_fun( EnvT* e)
  {
    e->NParam( 1);//, "SIN");

    BaseGDL* p0 = e->GetParDefined( 0);//, "SIN");

    SizeT nEl = p0->N_Elements();
    if( nEl == 0)
      throw GDLException( e->CallingNode(), 
			  "SIN: Variable is undefined: "+e->GetParString(0));
    
    if( p0->Type() == COMPLEX)
	return sin_fun_template< DComplexGDL>( p0);
    else if( p0->Type() == COMPLEXDBL)
	return sin_fun_template< DComplexDblGDL>( p0);
    else if( p0->Type() == DOUBLE)
	return sin_fun_template< DDoubleGDL>( p0);
    else if( p0->Type() == FLOAT)
	return sin_fun_template< DFloatGDL>( p0);
    else 
      {
	DFloatGDL* res = static_cast<DFloatGDL*>
	  (p0->Convert2( FLOAT, BaseGDL::COPY));
	for( SizeT i=0; i<nEl; ++i)
	  {
	    (*res)[ i] = sin((*res)[ i]); 
	  }
	return res;
      }
  }

  template< typename T>
  BaseGDL* cos_fun_template( BaseGDL* p0)
  {
    T* p0C = static_cast<T*>( p0);
    T* res = new T( p0C->Dim(), BaseGDL::NOZERO);
    SizeT nEl = p0->N_Elements();
    for( SizeT i=0; i<nEl; ++i)
      {
	(*res)[ i] = cos((*p0C)[ i]); 
      }
    return res;
  }

  BaseGDL* cos_fun( EnvT* e)
  {
    SizeT nParam=e->NParam();

    if( nParam == 0)
      throw GDLException( e->CallingNode(), 
			  "COS: Incorrect number of arguments.");

    BaseGDL* p0 = e->GetParDefined( 0);//, "COS");

    SizeT nEl = p0->N_Elements();
    if( nEl == 0)
      throw GDLException( e->CallingNode(), 
			  "COS: Variable is undefined: "+e->GetParString(0));
    
    if( p0->Type() == COMPLEX)
	return cos_fun_template< DComplexGDL>( p0);
    else if( p0->Type() == COMPLEXDBL)
	return cos_fun_template< DComplexDblGDL>( p0);
    else if( p0->Type() == DOUBLE)
	return cos_fun_template< DDoubleGDL>( p0);
    else if( p0->Type() == FLOAT)
	return cos_fun_template< DFloatGDL>( p0);
    else 
      {
	DFloatGDL* res = static_cast<DFloatGDL*>
	  (p0->Convert2( FLOAT, BaseGDL::COPY));
	for( SizeT i=0; i<nEl; ++i)
	  {
	    (*res)[ i] = cos((*res)[ i]); 
	  }
	return res;
      }
  }

  template< typename T>
  BaseGDL* tan_fun_template( BaseGDL* p0)
  {
    T* p0C = static_cast<T*>( p0);
    T* res = new T( p0C->Dim(), BaseGDL::NOZERO);
    SizeT nEl = p0->N_Elements();
    for( SizeT i=0; i<nEl; ++i)
      {
	(*res)[ i] = tan((*p0C)[ i]); 
      }
    return res;
  }

  BaseGDL* tan_fun( EnvT* e)
  {
    SizeT nParam=e->NParam();

    if( nParam == 0)
      throw GDLException( e->CallingNode(), 
			  "TAN: Incorrect number of arguments.");

    BaseGDL* p0 = e->GetParDefined( 0);//, "TAN");

    SizeT nEl = p0->N_Elements();
    if( nEl == 0)
      throw GDLException( e->CallingNode(), 
			  "TAN: Variable is undefined: "+e->GetParString(0));
    
    if( p0->Type() == COMPLEX)
	return tan_fun_template< DComplexGDL>( p0);
    else if( p0->Type() == COMPLEXDBL)
	return tan_fun_template< DComplexDblGDL>( p0);
    else if( p0->Type() == DOUBLE)
	return tan_fun_template< DDoubleGDL>( p0);
    else if( p0->Type() == FLOAT)
	return tan_fun_template< DFloatGDL>( p0);
    else 
      {
	DFloatGDL* res = static_cast<DFloatGDL*>
	  (p0->Convert2( FLOAT, BaseGDL::COPY));
	for( SizeT i=0; i<nEl; ++i)
	  {
	    (*res)[ i] = tan((*res)[ i]); 
	  }
	return res;
      }
  }

  template< typename T>
  BaseGDL* sinh_fun_template( BaseGDL* p0)
  {
    T* p0C = static_cast<T*>( p0);
    T* res = new T( p0C->Dim(), BaseGDL::NOZERO);
    SizeT nEl = p0->N_Elements();
    for( SizeT i=0; i<nEl; ++i)
      {
	(*res)[ i] = sinh((*p0C)[ i]); 
      }
    return res;
  }

  BaseGDL* sinh_fun( EnvT* e)
  {
    SizeT nParam=e->NParam();

    if( nParam == 0)
      throw GDLException( e->CallingNode(), 
			  "SINH: Incorrect number of arguments.");

    BaseGDL* p0 = e->GetParDefined( 0);//, "SINH");

    SizeT nEl = p0->N_Elements();
    if( nEl == 0)
      throw GDLException( e->CallingNode(), 
			  "SINH: Variable is undefined: "+e->GetParString(0));
    
    if( p0->Type() == COMPLEX)
	return sinh_fun_template< DComplexGDL>( p0);
    else if( p0->Type() == COMPLEXDBL)
	return sinh_fun_template< DComplexDblGDL>( p0);
    else if( p0->Type() == DOUBLE)
	return sinh_fun_template< DDoubleGDL>( p0);
    else if( p0->Type() == FLOAT)
	return sinh_fun_template< DFloatGDL>( p0);
    else 
      {
	DFloatGDL* res = static_cast<DFloatGDL*>
	  (p0->Convert2( FLOAT, BaseGDL::COPY));
	for( SizeT i=0; i<nEl; ++i)
	  {
	    (*res)[ i] = sinh((*res)[ i]); 
	  }
	return res;
      }
  }

  template< typename T>
  BaseGDL* cosh_fun_template( BaseGDL* p0)
  {
    T* p0C = static_cast<T*>( p0);
    T* res = new T( p0C->Dim(), BaseGDL::NOZERO);
    SizeT nEl = p0->N_Elements();
    for( SizeT i=0; i<nEl; ++i)
      {
	(*res)[ i] = cosh((*p0C)[ i]); 
      }
    return res;
  }

  BaseGDL* cosh_fun( EnvT* e)
  {
    SizeT nParam=e->NParam();

    if( nParam == 0)
      throw GDLException( e->CallingNode(), 
			  "COSH: Incorrect number of arguments.");

    BaseGDL* p0 = e->GetParDefined( 0);//, "COSH");

    SizeT nEl = p0->N_Elements();
    if( nEl == 0)
      throw GDLException( e->CallingNode(), 
			  "COSH: Variable is undefined: "+e->GetParString(0));
    
    if( p0->Type() == COMPLEX)
	return cosh_fun_template< DComplexGDL>( p0);
    else if( p0->Type() == COMPLEXDBL)
	return cosh_fun_template< DComplexDblGDL>( p0);
    else if( p0->Type() == DOUBLE)
	return cosh_fun_template< DDoubleGDL>( p0);
    else if( p0->Type() == FLOAT)
	return cosh_fun_template< DFloatGDL>( p0);
    else 
      {
	DFloatGDL* res = static_cast<DFloatGDL*>
	  (p0->Convert2( FLOAT, BaseGDL::COPY));
	for( SizeT i=0; i<nEl; ++i)
	  {
	    (*res)[ i] = cosh((*res)[ i]); 
	  }
	return res;
      }
  }

  template< typename T>
  BaseGDL* tanh_fun_template( BaseGDL* p0)
  {
    T* p0C = static_cast<T*>( p0);
    T* res = new T( p0C->Dim(), BaseGDL::NOZERO);
    SizeT nEl = p0->N_Elements();
    for( SizeT i=0; i<nEl; ++i)
      {
	(*res)[ i] = tanh((*p0C)[ i]); 
      }
    return res;
  }

  BaseGDL* tanh_fun( EnvT* e)
  {
    SizeT nParam=e->NParam();

    if( nParam == 0)
      throw GDLException( e->CallingNode(), 
			  "TANH: Incorrect number of arguments.");

    BaseGDL* p0 = e->GetParDefined( 0);//, "TANH");

    SizeT nEl = p0->N_Elements();
    if( nEl == 0)
      throw GDLException( e->CallingNode(), 
			  "TANH: Variable is undefined: "+e->GetParString(0));
    
    if( p0->Type() == COMPLEX)
	return tanh_fun_template< DComplexGDL>( p0);
    else if( p0->Type() == COMPLEXDBL)
	return tanh_fun_template< DComplexDblGDL>( p0);
    else if( p0->Type() == DOUBLE)
	return tanh_fun_template< DDoubleGDL>( p0);
    else if( p0->Type() == FLOAT)
	return tanh_fun_template< DFloatGDL>( p0);
    else 
      {
	DFloatGDL* res = static_cast<DFloatGDL*>
	  (p0->Convert2( FLOAT, BaseGDL::COPY));
	for( SizeT i=0; i<nEl; ++i)
	  {
	    (*res)[ i] = tanh((*res)[ i]); 
	  }
	return res;
      }
  }

  BaseGDL* asin_fun( EnvT* e)
  {
    e->NParam( 1);//, "ASIN");

    BaseGDL* p0 = e->GetParDefined( 0);//, "ASIN");

    SizeT nEl = p0->N_Elements();
    if( nEl == 0)
      throw GDLException( e->CallingNode(), 
			  "ASIN: Variable is undefined: "+e->GetParString(0));
    
    if( p0->Type() == COMPLEX || p0->Type() == COMPLEXDBL)
      {
	throw GDLException( e->CallingNode(), 
			    "Operation illegal with complex type.");
      }
    else if( p0->Type() == DOUBLE)
      {
	DDoubleGDL* p0D = static_cast<DDoubleGDL*>( p0);
	DDoubleGDL* res = new DDoubleGDL( p0->Dim(), BaseGDL::NOZERO);
	for( SizeT i=0; i<nEl; ++i)
	  {
	    (*res)[ i] = asin((*p0D)[ i]); 
	  }
	return res;
      }
    else if( p0->Type() == FLOAT)
      {
	DFloatGDL* p0F = static_cast<DFloatGDL*>( p0);
	DFloatGDL* res = new DFloatGDL( p0->Dim(), BaseGDL::NOZERO);
	for( SizeT i=0; i<nEl; ++i)
	  {
	    (*res)[ i] = asin((*p0F)[ i]); 
	  }
	return res;
      }
    else 
      {
	DFloatGDL* res = static_cast<DFloatGDL*>
	  (p0->Convert2( FLOAT, BaseGDL::COPY));
	for( SizeT i=0; i<nEl; ++i)
	  {
	    (*res)[ i] = asin((*res)[ i]); 
	  }
	return res;
      }
  }

  BaseGDL* acos_fun( EnvT* e)
  {
    e->NParam( 1);//, "ACOS");

    BaseGDL* p0 = e->GetParDefined( 0);//, "ACOS");

    SizeT nEl = p0->N_Elements();
    if( nEl == 0)
      throw GDLException( e->CallingNode(), 
			  "ACOS: Variable is undefined: "+e->GetParString(0));
    
    if( p0->Type() == COMPLEX || p0->Type() == COMPLEXDBL)
      {
	throw GDLException( e->CallingNode(), 
			    "Operation illegal with complex type.");
      }
    else if( p0->Type() == DOUBLE)
      {
	DDoubleGDL* p0D = static_cast<DDoubleGDL*>( p0);
	DDoubleGDL* res = new DDoubleGDL( p0->Dim(), BaseGDL::NOZERO);
	for( SizeT i=0; i<nEl; ++i)
	  {
	    (*res)[ i] = acos((*p0D)[ i]); 
	  }
	return res;
      }
    else if( p0->Type() == FLOAT)
      {
	DFloatGDL* p0F = static_cast<DFloatGDL*>( p0);
	DFloatGDL* res = new DFloatGDL( p0->Dim(), BaseGDL::NOZERO);
	for( SizeT i=0; i<nEl; ++i)
	  {
	    (*res)[ i] = acos((*p0F)[ i]); 
	  }
	return res;
      }
    else 
      {
	DFloatGDL* res = static_cast<DFloatGDL*>
	  (p0->Convert2( FLOAT, BaseGDL::COPY));
	for( SizeT i=0; i<nEl; ++i)
	  {
	    (*res)[ i] = acos((*res)[ i]); 
	  }
	return res;
      }
  }

  // atan() for complex
  template< typename C>
  inline C atanC(const C& c)
  {
    const C i(0.0,1.0);
    const C one(1.0,0.0);
    return log( (one + i * c) / (one - i * c)) / (C(2.0,0.0)*i);
  } 

  BaseGDL* atan_fun( EnvT* e)
  {
    SizeT nParam=e->NParam( 1);//, "ATAN");

    BaseGDL* p0 = e->GetParDefined( 0);//, "ATAN");

    SizeT nEl = p0->N_Elements();
    if( nEl == 0)
      throw GDLException( e->CallingNode(), 
			  "ATAN: Variable is undefined: "+e->GetParString(0));
    
    if( nParam == 2)
      {
	BaseGDL* p1 = e->GetPar( 1);
	if( p1 == NULL)
	  throw GDLException( e->CallingNode(), 
			      "ATAN: Variable is undefined: "+e->GetParString(1));
	SizeT nEl1 = p1->N_Elements();
	if( nEl1 == 0)
	  throw GDLException( e->CallingNode(), 
			      "ATAN: Variable is undefined: "+e->GetParString(1));
	
	DType t = (DTypeOrder[ p0->Type()] > DTypeOrder[ p1->Type()])? p0->Type() : p1->Type();
 
	const dimension& dim = (nEl < nEl1)? p0->Dim() : p1->Dim(); 
	
	SizeT nElMin = (nEl < nEl1)? nEl : nEl1;
	
	if( t == COMPLEX)
	  {
	    auto_ptr< DFloatGDL> guard0;
	    auto_ptr< DFloatGDL> guard1;

	    DFloatGDL* p0F = static_cast<DFloatGDL*>(p0->Convert2( FLOAT, BaseGDL::COPY));
	    guard0.reset( p0F);
	    DFloatGDL* p1F = static_cast<DFloatGDL*>(p1->Convert2( FLOAT, BaseGDL::COPY));
	    guard1.reset( p1F);
	      
	    DFloatGDL* res = new DFloatGDL( dim, BaseGDL::NOZERO);
	    for( SizeT i=0; i<nElMin; ++i)
	      {
		(*res)[ i] = atan2((*p0F)[ i], (*p1F)[ i]); 
	      }
	    return res;
	  }
	else if( t == COMPLEXDBL)
	  {
	    auto_ptr< DDoubleGDL> guard0;
	    auto_ptr< DDoubleGDL> guard1;

	    DDoubleGDL* p0F = static_cast<DDoubleGDL*>
	      (p0->Convert2( DOUBLE, BaseGDL::COPY));
	    guard0.reset( p0F);
	    DDoubleGDL* p1F = static_cast<DDoubleGDL*>
	      (p1->Convert2( DOUBLE, BaseGDL::COPY));
	    guard1.reset( p1F);
	      
	    DDoubleGDL* res = new DDoubleGDL( dim, BaseGDL::NOZERO);
	    for( SizeT i=0; i<nElMin; ++i)
	      {
		(*res)[ i] = atan2((*p0F)[ i], (*p1F)[ i]); 
	      }
	    return res;
	  }
	else if( t == DOUBLE)
	  {
	    auto_ptr< DDoubleGDL> guard;
	    
	    DDoubleGDL* p0D;
	    if( p0->Type() != DOUBLE)
	      {
		p0D =  static_cast<DDoubleGDL*>( p0->Convert2( DOUBLE, BaseGDL::COPY));
		guard.reset( p0D);
	      }
	    else
	      {
		p0D =  static_cast<DDoubleGDL*>( p0);
	      }

	    DDoubleGDL* p1D;
	    if( p1->Type() != DOUBLE)
	      {
		p1D =  static_cast<DDoubleGDL*>( p1->Convert2( DOUBLE, BaseGDL::COPY));
		guard.reset( p1D);
	      }
	    else
	      {
		p1D =  static_cast<DDoubleGDL*>( p1);
	      }

	    DDoubleGDL* res = new DDoubleGDL( dim, BaseGDL::NOZERO);
	    for( SizeT i=0; i<nElMin; ++i)
	      {
		(*res)[ i] = atan2((*p0D)[ i],(*p1D)[ i]); 
	      }
	    return res;
	  }
	else if( t == FLOAT)
	  {
	    auto_ptr< DFloatGDL> guard;
	    
	    DFloatGDL* p0F;
	    if( p0->Type() != FLOAT)
	      {
		p0F =  static_cast<DFloatGDL*>( p0->Convert2( FLOAT, BaseGDL::COPY));
		guard.reset( p0F);
	      }
	    else
	      {
		p0F =  static_cast<DFloatGDL*>( p0);
	      }

	    DFloatGDL* p1F;
	    if( p1->Type() != FLOAT)
	      {
		p1F =  static_cast<DFloatGDL*>( p1->Convert2( FLOAT, BaseGDL::COPY));
		guard.reset( p1F);
	      }
	    else
	      {
		p1F =  static_cast<DFloatGDL*>( p1);
	      }

	    DFloatGDL* res = new DFloatGDL( dim, BaseGDL::NOZERO);
	    for( SizeT i=0; i<nElMin; ++i)
	      {
		(*res)[ i] = atan2((*p0F)[ i], (*p1F)[ i]); 
	      }
	    return res;
	  }
	else 
	  {
	    auto_ptr< DFloatGDL> guard0;
	    auto_ptr< DFloatGDL> guard1;

	    DFloatGDL* p0F = static_cast<DFloatGDL*>(p0->Convert2( FLOAT, BaseGDL::COPY));
	    guard0.reset( p0F);
	    DFloatGDL* p1F = static_cast<DFloatGDL*>(p1->Convert2( FLOAT, BaseGDL::COPY));
	    guard1.reset( p1F);
	      
	    DFloatGDL* res = new DFloatGDL( dim, BaseGDL::NOZERO);
	    for( SizeT i=0; i<nElMin; ++i)
	      {
		(*res)[ i] = atan2((*p0F)[ i], (*p1F)[ i]); 
	      }
	    return res;
	  }
      }
    else
      {
	if( p0->Type() == COMPLEX)
	  {
	    DComplexGDL* p0C = static_cast<DComplexGDL*>( p0);
	    DFloatGDL* res = new DFloatGDL( p0C->Dim(), BaseGDL::NOZERO);
	    for( SizeT i=0; i<nEl; ++i)
	      {
		DComplex& C = (*p0C)[ i];
		(*res)[ i] = (C.real() == 0.0)? 
		  ((C.imag() == 0.0)? 0.0 : 1.5707963267948966) : 
		  atan( C.imag()/C.real()); 
	      }
	    return res;
	  }
	else if( p0->Type() == COMPLEXDBL)
	  {
	    DComplexDblGDL* p0C = static_cast<DComplexDblGDL*>( p0);
	    DDoubleGDL* res = new DDoubleGDL( p0C->Dim(), BaseGDL::NOZERO);
	    for( SizeT i=0; i<nEl; ++i)
	      {
		DComplexDbl& C = (*p0C)[ i];
		(*res)[ i] = (C.real() == 0.0)? 
		  ((C.imag() == 0.0)? 0.0 : 1.5707963267948966) : 
		  atan( C.imag()/C.real()); 
	      }
	    return res;
	  }
	else if( p0->Type() == DOUBLE)
	  {
	    DDoubleGDL* p0D = static_cast<DDoubleGDL*>( p0);
	    DDoubleGDL* res = new DDoubleGDL( p0->Dim(), BaseGDL::NOZERO);
	    for( SizeT i=0; i<nEl; ++i)
	      {
		(*res)[ i] = atan((*p0D)[ i]); 
	      }
	    return res;
	  }
	else if( p0->Type() == FLOAT)
	  {
	    DFloatGDL* p0F = static_cast<DFloatGDL*>( p0);
	    DFloatGDL* res = new DFloatGDL( p0->Dim(), BaseGDL::NOZERO);
	    for( SizeT i=0; i<nEl; ++i)
	      {
		(*res)[ i] = atan((*p0F)[ i]); 
	      }
	    return res;
	  }
	else 
	  {
	    DFloatGDL* res = static_cast<DFloatGDL*>
	      (p0->Convert2( FLOAT, BaseGDL::COPY));
	    for( SizeT i=0; i<nEl; ++i)
	      {
		(*res)[ i] = atan((*res)[ i]); 
	      }
	    return res;
	  }
      }
  }

//   template< typename T>
//   BaseGDL* alog_fun_template( BaseGDL* p0)
//   {
//     T* p0C = static_cast<T*>( p0);
//     T* res = new T( p0C->Dim(), BaseGDL::NOZERO);
//     SizeT nEl = p0->N_Elements();
//     for( SizeT i=0; i<nEl; ++i)
//       {
// 	(*res)[ i] = log((*p0C)[ i]); 
//       }
//     return res;
//   }

  BaseGDL* alog_fun( EnvT* e)
  {
    e->NParam( 1);//,"ALOG");

    BaseGDL* p0 = e->GetParDefined( 0);//, "ALOG");

    SizeT nEl = p0->N_Elements();
    if( nEl == 0)
      e->Throw( "Variable is undefined: "+e->GetParString(0));
    
    if( FloatType( p0->Type()) || ComplexType( p0->Type()))
      if( e->StealLocalPar( 0))
	{
	  p0->LogThis();
	  return p0;
	}
      else
	return p0->Log(); //alog_fun_template< DComplexGDL>( p0);
//     else if( p0->Type() == COMPLEXDBL)
//       return p0->Log(); //alog_fun_template< DComplexDblGDL>( p0);
//     else if( p0->Type() == DOUBLE)
//       return p0->Log(); //alog_fun_template< DDoubleGDL>( p0);
//     else if( p0->Type() == FLOAT)
//       return p0->Log(); //alog_fun_template< DFloatGDL>( p0);
    else 
      {
	DFloatGDL* res = static_cast<DFloatGDL*>
	  (p0->Convert2( FLOAT, BaseGDL::COPY));
	res->LogThis();
// 	for( SizeT i=0; i<nEl; ++i)
// 	  {
// 	    (*res)[ i] = log((*res)[ i]); 
// 	  }
	return res;
      }
  }

//   template< typename T>
//   BaseGDL* alog10_fun_template( BaseGDL* p0)
//   {
//     T* p0C = static_cast<T*>( p0);
//     T* res = new T( p0C->Dim(), BaseGDL::NOZERO);
//     SizeT nEl = p0->N_Elements();
//     for( SizeT i=0; i<nEl; ++i)
//       {
// 	(*res)[ i] = log10((*p0C)[ i]); 
//       }
//     return res;
//   }

//   BaseGDL* alog10_fun( EnvT* e)
//   {
//     e->NParam( 1);//, "ALOG10");

//     BaseGDL* p0 = e->GetParDefined( 0);//, "ALOG10");

//     SizeT nEl = p0->N_Elements();
//     if( nEl == 0)
//       throw GDLException( e->CallingNode(), 
// 			  "ALOG10: Variable is undefined: "+e->GetParString(0));
    
//     if( p0->Type() == COMPLEX)
// 	return alog10_fun_template< DComplexGDL>( p0);
//     else if( p0->Type() == COMPLEXDBL)
// 	return alog10_fun_template< DComplexDblGDL>( p0);
//     else if( p0->Type() == DOUBLE)
// 	return alog10_fun_template< DDoubleGDL>( p0);
//     else if( p0->Type() == FLOAT)
// 	return alog10_fun_template< DFloatGDL>( p0);
//     else 
//       {
// 	DFloatGDL* res = static_cast<DFloatGDL*>
// 	  (p0->Convert2( FLOAT, BaseGDL::COPY));
// 	for( SizeT i=0; i<nEl; ++i)
// 	  {
// 	    (*res)[ i] = log10((*res)[ i]); 
// 	  }
// 	return res;
//       }
//   }

  BaseGDL* alog10_fun( EnvT* e)
  {
    e->NParam( 1);

    BaseGDL* p0 = e->GetParDefined( 0);

    SizeT nEl = p0->N_Elements();
    if( nEl == 0)
      e->Throw( "Variable is undefined: "+e->GetParString(0));
    
    if( FloatType( p0->Type()) || ComplexType( p0->Type()))
      if( e->StealLocalPar( 0))
	{
	  p0->Log10This();
	  return p0;
	}
      else
	return p0->Log10();
    else 
      {
	DFloatGDL* res = static_cast<DFloatGDL*>
	  (p0->Convert2( FLOAT, BaseGDL::COPY));
	res->Log10This();
	return res;
      }
  }

  // by joel gales
  template< typename T>
  BaseGDL* sqrt_fun_template( BaseGDL* p0)
  {
    T* p0C = static_cast<T*>( p0);
    T* res = new T( p0C->Dim(), BaseGDL::NOZERO);
    SizeT nEl = p0->N_Elements();
    for( SizeT i=0; i<nEl; ++i)
      {
	(*res)[ i] = sqrt((*p0C)[ i]); 
      }
    return res;
  }

  BaseGDL* sqrt_fun( EnvT* e)
  {
    e->NParam( 1);//, "SQRT");

    BaseGDL* p0 = e->GetParDefined( 0);//, "SQRT");

    SizeT nEl = p0->N_Elements();
    if( nEl == 0)
      throw GDLException( e->CallingNode(), 
			  "SQRT: Variable is undefined: "+e->GetParString(0));
    
    if( p0->Type() == COMPLEX)
	return sqrt_fun_template< DComplexGDL>( p0);
    else if( p0->Type() == COMPLEXDBL)
	return sqrt_fun_template< DComplexDblGDL>( p0);
    else if( p0->Type() == DOUBLE)
	return sqrt_fun_template< DDoubleGDL>( p0);
    else if( p0->Type() == FLOAT)
	return sqrt_fun_template< DFloatGDL>( p0);
    else 
      {
	DFloatGDL* res = static_cast<DFloatGDL*>
	  (p0->Convert2( FLOAT, BaseGDL::COPY));
	for( SizeT i=0; i<nEl; ++i)
	  {
	    (*res)[ i] = sqrt( (*res)[ i]); 
	  }
	return res;
      }
  }


  template< typename T>
  BaseGDL* abs_fun_template( BaseGDL* p0)
  {
    T* p0C = static_cast<T*>( p0);
    T* res = new T( p0C->Dim(), BaseGDL::NOZERO);
    SizeT nEl = p0->N_Elements();
    for( SizeT i=0; i<nEl; ++i)
      {
	(*res)[ i] = abs((*p0C)[ i]); 
      }
    return res;
  }

  BaseGDL* abs_fun( EnvT* e)
  {
    e->NParam( 1);

    BaseGDL* p0 = e->GetParDefined( 0);

    SizeT nEl = p0->N_Elements();
    if( nEl == 0)
      e->Throw( "Variable is undefined: "+e->GetParString(0));
    
    if( p0->Type() == COMPLEX) 
      {
	DComplexGDL* p0C = static_cast<DComplexGDL*>( p0);
	DFloatGDL* res = new DFloatGDL(p0C->Dim(), BaseGDL::NOZERO);
	SizeT nEl = p0->N_Elements();
	for( SizeT i=0; i<nEl; ++i)
	  {
// 	    DComplex& C = (*p0C)[ i];
// 	    float Creal = C.real(), Cimag = C.imag();
// 	    (*res)[ i] = sqrt(Creal*Creal + Cimag*Cimag);
 	    (*res)[ i] = abs( (*p0C)[ i]); //sqrt(Creal*Creal + Cimag*Cimag);
	  }
	return res;
      }
    else if( p0->Type() == COMPLEXDBL)
      {
	DComplexDblGDL* p0C = static_cast<DComplexDblGDL*>( p0);
	DDoubleGDL* res = new DDoubleGDL(p0C->Dim(), BaseGDL::NOZERO);
	SizeT nEl = p0->N_Elements();
	for( SizeT i=0; i<nEl; ++i)
	  {
// 	    DComplexDbl& C = (*p0C)[ i];
// 	    double Creal = C.real(), Cimag = C.imag();
// 	    (*res)[ i] = sqrt(Creal*Creal + Cimag*Cimag);
 	    (*res)[ i] = abs( (*p0C)[ i]); //sqrt(Creal*Creal + Cimag*Cimag);
	  }
	return res;
      }
    else if( p0->Type() == DOUBLE)
	return abs_fun_template< DDoubleGDL>( p0);
    else if( p0->Type() == FLOAT)
	return abs_fun_template< DFloatGDL>( p0);
    else if( p0->Type() == LONG64)
	return abs_fun_template< DLong64GDL>( p0);
    else if( p0->Type() == LONG)
	return abs_fun_template< DLongGDL>( p0);
    else if( p0->Type() == INT)
	return abs_fun_template< DIntGDL>( p0);
    else if( p0->Type() == ULONG64)
      return p0->Dup();
    else if( p0->Type() == ULONG)
      return p0->Dup();
    else if( p0->Type() == UINT)
      return p0->Dup();
    else if( p0->Type() == BYTE)
      return p0->Dup();
    else 
      {
	DFloatGDL* res = static_cast<DFloatGDL*>
	  (p0->Convert2( FLOAT, BaseGDL::COPY));
	for( SizeT i=0; i<nEl; ++i)
	  {
	    (*res)[ i] = abs( (*res)[ i]); 
	  }
	return res;
      }
  }


  template< typename T>
  BaseGDL* round_fun_template( BaseGDL* p0)
  {
    T* p0C = static_cast<T*>( p0);
    DLongGDL* res = new DLongGDL( p0C->Dim(), BaseGDL::NOZERO);
    SizeT nEl = p0->N_Elements();
    for( SizeT i=0; i<nEl; ++i)
      {
	(*res)[ i] = static_cast<DLong>( round((*p0C)[ i])); 
      }
    return res;
  }

  BaseGDL* round_fun( EnvT* e)
  {
    e->NParam( 1);//, "ROUND");

    BaseGDL* p0 = e->GetParDefined( 0);//, "ROUND");

    SizeT nEl = p0->N_Elements();
    if( nEl == 0)
      throw GDLException( e->CallingNode(), 
			  "ROUND: Variable is undefined: "+e->GetParString(0));
    
    if( p0->Type() == COMPLEX)
      {
	DComplexGDL* p0C = static_cast<DComplexGDL*>( p0);
	DLongGDL* res = new DLongGDL(p0C->Dim(), BaseGDL::NOZERO);
	SizeT nEl = p0->N_Elements();
	for( SizeT i=0; i<nEl; ++i)
	  {
	    DComplex& C = (*p0C)[ i];
	    (*res)[ i] = static_cast<DLong>( round(C.real()));
	  }
	return res;
      }
    else if( p0->Type() == COMPLEXDBL)
      {
	DComplexDblGDL* p0C = static_cast<DComplexDblGDL*>( p0);
	DLongGDL* res = new DLongGDL(p0C->Dim(), BaseGDL::NOZERO);
	SizeT nEl = p0->N_Elements();
	for( SizeT i=0; i<nEl; ++i)
	  {
	    DComplexDbl& C = (*p0C)[ i];
	    (*res)[ i] = static_cast<DLong>( round(C.real()));
	  }
	return res;
      }
    else if( p0->Type() == DOUBLE)
	return round_fun_template< DDoubleGDL>( p0);
    else if( p0->Type() == FLOAT)
	return round_fun_template< DFloatGDL>( p0);
    else if( p0->Type() == LONG64)
      return p0->Dup();
    else if( p0->Type() == LONG)
      return p0->Dup();
    else if( p0->Type() == INT)
      return p0->Dup();
    else if( p0->Type() == ULONG64)
      return p0->Dup();
    else if( p0->Type() == ULONG)
      return p0->Dup();
    else if( p0->Type() == UINT)
      return p0->Dup();
    else if( p0->Type() == BYTE)
      return p0->Dup();
    else 
      {
	DFloatGDL* p0F = e->GetParAs<DFloatGDL>( 0);
	DLongGDL* res = new DLongGDL(p0->Dim(), BaseGDL::NOZERO);
	SizeT nEl = p0->N_Elements();
	for( SizeT i=0; i<nEl; ++i)
	  {
	    (*res)[ i] = static_cast<DLong>( round( (*p0F)[ i])); 
	  }
	return res;
      }
  }


  template< typename T>
  BaseGDL* ceil_fun_template( BaseGDL* p0)
  {
    T* p0C = static_cast<T*>( p0);
    DLongGDL* res = new DLongGDL( p0C->Dim(), BaseGDL::NOZERO);
    SizeT nEl = p0->N_Elements();
    for( SizeT i=0; i<nEl; ++i)
      {
	(*res)[ i] = (int) ceil((*p0C)[ i]); 
      }
    return res;
  }

  BaseGDL* ceil_fun( EnvT* e)
  {
    e->NParam( 1);//, "CEIL");

    BaseGDL* p0 = e->GetParDefined( 0);//, "CEIL");

    SizeT nEl = p0->N_Elements();
    if( nEl == 0)
      throw GDLException( e->CallingNode(), 
			  "CEIL: Variable is undefined: "+e->GetParString(0));
    
    if( p0->Type() == COMPLEX)
      {
	DComplexGDL* p0C = static_cast<DComplexGDL*>( p0);
	DLongGDL* res = new DLongGDL(p0C->Dim(), BaseGDL::NOZERO);
	SizeT nEl = p0->N_Elements();
	for( SizeT i=0; i<nEl; ++i)
	  {
	    DComplex& C = (*p0C)[ i];
	    (*res)[ i] = (int) ceil(C.real());
	  }
	return res;
      }
    else if( p0->Type() == COMPLEXDBL)
      {
	DComplexDblGDL* p0C = static_cast<DComplexDblGDL*>( p0);
	DLongGDL* res = new DLongGDL(p0C->Dim(), BaseGDL::NOZERO);
	SizeT nEl = p0->N_Elements();
	for( SizeT i=0; i<nEl; ++i)
	  {
	    DComplexDbl& C = (*p0C)[ i];
	    (*res)[ i] = (int) ceil(C.real());
	  }
	return res;
      }
    else if( p0->Type() == DOUBLE)
	return ceil_fun_template< DDoubleGDL>( p0);
    else if( p0->Type() == FLOAT)
	return ceil_fun_template< DFloatGDL>( p0);
    else if( p0->Type() == LONG64)
      return p0->Dup();
    else if( p0->Type() == LONG)
      return p0->Dup();
    else if( p0->Type() == INT)
      return p0->Dup();
    else if( p0->Type() == ULONG64)
      return p0->Dup();
    else if( p0->Type() == ULONG)
      return p0->Dup();
    else if( p0->Type() == UINT)
      return p0->Dup();
    else if( p0->Type() == BYTE)
      return p0->Dup();
    else 
      {
	DFloatGDL* p0F = e->GetParAs<DFloatGDL>( 0);
	DLongGDL* res = new DLongGDL(p0->Dim(), BaseGDL::NOZERO);
	SizeT nEl = p0->N_Elements();
	for( SizeT i=0; i<nEl; ++i)
	  {
	    (*res)[ i] = (int) ceil((double) (*p0F)[ i]); 
	  }
	return res;
      }
  }

  template< typename T>
  BaseGDL* floor_fun_template( BaseGDL* p0)
  {
    T* p0C = static_cast<T*>( p0);
    DLongGDL* res = new DLongGDL( p0C->Dim(), BaseGDL::NOZERO);
    SizeT nEl = p0->N_Elements();
    for( SizeT i=0; i<nEl; ++i)
      {
	(*res)[ i] = (int) floor((*p0C)[ i]); 
      }
    return res;
  }

  BaseGDL* floor_fun( EnvT* e)
  {
    e->NParam( 1);//, "FLOOR");

    BaseGDL* p0 = e->GetParDefined( 0);//, "FLOOR");

    SizeT nEl = p0->N_Elements();
    if( nEl == 0)
      throw GDLException( e->CallingNode(), 
			  "FLOOR: Variable is undefined: "+e->GetParString(0));
    
    if( p0->Type() == COMPLEX)
      {
	DComplexGDL* p0C = static_cast<DComplexGDL*>( p0);
	DLongGDL* res = new DLongGDL(p0C->Dim(), BaseGDL::NOZERO);
	SizeT nEl = p0->N_Elements();
	for( SizeT i=0; i<nEl; ++i)
	  {
	    DComplex& C = (*p0C)[ i];
	    (*res)[ i] = (int) floor(C.real());
	  }
	return res;
      }
    else if( p0->Type() == COMPLEXDBL)
      {
	DComplexDblGDL* p0C = static_cast<DComplexDblGDL*>( p0);
	DLongGDL* res = new DLongGDL(p0C->Dim(), BaseGDL::NOZERO);
	SizeT nEl = p0->N_Elements();
	for( SizeT i=0; i<nEl; ++i)
	  {
	    DComplexDbl& C = (*p0C)[ i];
	    (*res)[ i] = (int) floor(C.real());
	  }
	return res;
      }
    else if( p0->Type() == DOUBLE)
	return floor_fun_template< DDoubleGDL>( p0);
    else if( p0->Type() == FLOAT)
	return floor_fun_template< DFloatGDL>( p0);
    else if( p0->Type() == LONG64)
      return p0->Dup();
    else if( p0->Type() == LONG)
      return p0->Dup();
    else if( p0->Type() == INT)
      return p0->Dup();
    else if( p0->Type() == ULONG64)
      return p0->Dup();
    else if( p0->Type() == ULONG)
      return p0->Dup();
    else if( p0->Type() == UINT)
      return p0->Dup();
    else if( p0->Type() == BYTE)
      return p0->Dup();
    else 
      {
	DFloatGDL* p0F = e->GetParAs<DFloatGDL>( 0);
	DLongGDL* res = new DLongGDL(p0->Dim(), BaseGDL::NOZERO);
	SizeT nEl = p0->N_Elements();
	for( SizeT i=0; i<nEl; ++i)
	  {
	    (*res)[ i] = (int) floor((double) (*p0F)[ i]); 
	  }
	return res;
      }
  }

  BaseGDL* conj_fun( EnvT* e)
  {
    e->NParam( 1);
    BaseGDL* p0 = e->GetParDefined( 0);
    SizeT nEl = p0->N_Elements();
    //    if( nEl == 0)
    //      e->Throw( "Variable is undefined: "+e->GetParString(0));
    
    if( p0->Type() == COMPLEX)
      {
	DComplexGDL* res = static_cast<DComplexGDL*>(p0->Dup());
	for( SizeT i=0; i<nEl; ++i)
	  {
	    (*res)[i] = DComplex( (*res)[i].real(), -(*res)[i].imag());
	  }
	return res;
      }
    if( p0->Type() == COMPLEXDBL)
      {
	DComplexDblGDL* res = static_cast<DComplexDblGDL*>(p0->Dup());
	for( SizeT i=0; i<nEl; ++i)
	  {
	    (*res)[i] = DComplexDbl( (*res)[i].real(), -(*res)[i].imag());
	  }
	return res;
      }
    if( p0->Type() == DOUBLE || 
	p0->Type() == LONG64 || 
	p0->Type() == ULONG64)
      {
	DComplexDblGDL* res = static_cast<DComplexDblGDL*>
	  (p0->Convert2( COMPLEXDBL, BaseGDL::COPY));
	return res;
      }

    // all other types
    DComplexGDL* res = 
      static_cast<DComplexGDL*>( p0->Convert2( COMPLEX, BaseGDL::COPY));
    return res;
  }

  BaseGDL* imaginary_fun( EnvT* e)
  {
    e->NParam( 1);
    BaseGDL* p0 = e->GetParDefined( 0);
    SizeT nEl = p0->N_Elements();

    //    if( nEl == 0) 
    //      e->Throw( "Variable is undefined: "+e->GetParString(0));
    
    // complex types, return imaginary part
    if( p0->Type() == COMPLEX)
      {
	DComplexGDL* c0 = static_cast<DComplexGDL*>(p0);
	DFloatGDL* res = new DFloatGDL( c0->Dim(), BaseGDL::NOZERO);
	for( SizeT i=0; i<nEl; ++i)
	  {
	    (*res)[i] = (*c0)[i].imag();
	  }
	return res;
      }
    if( p0->Type() == COMPLEXDBL)
      {
	DComplexDblGDL* c0 = static_cast<DComplexDblGDL*>(p0);
	DDoubleGDL* res = new DDoubleGDL( c0->Dim(), BaseGDL::NOZERO);
	for( SizeT i=0; i<nEl; ++i)
	  {
	    (*res)[i] = (*c0)[i].imag();
	  }
	return res;
      }

    // forbidden types
    if( p0->Type() == STRING)
      e->Throw( "String expression not allowed in this context: "+
		e->GetParString(0));
    if( p0->Type() == STRUCT)
      e->Throw( "Struct expression not allowed in this context: "+
		e->GetParString(0));
    if( p0->Type() == PTR)
      e->Throw( "Pointer expression not allowed in this context: "+
		e->GetParString(0));
    if( p0->Type() == OBJECT)
      e->Throw( "Object reference not allowed in this context: "+
		e->GetParString(0));
    
    // all other types (return array of zeros)
    DFloatGDL* res = new DFloatGDL( p0->Dim()); // ZERO
    return res;
  }

  BaseGDL* exp_fun( EnvT* e)
  {
    e->NParam( 1);
    BaseGDL* p0 = e->GetParDefined( 0);

    SizeT nEl = p0->N_Elements();

    DType t = p0->Type();

    if( t == PTR)
      e->Throw( "Pointer not allowed in this context.");
    else if( t == OBJECT)
      e->Throw( "Object references not allowed in this context.");
    else if( t == STRUCT)
      e->Throw( "Struct expression not allowed in this context.");
    else if( t == COMPLEXDBL)
      {
	DComplexDblGDL *c0 = static_cast< DComplexDblGDL*>( p0);
	DComplexDblGDL *res = c0->New( c0->Dim(), BaseGDL::NOZERO);
	for( SizeT i=0; i<nEl; ++i)
	  (*res)[ i] = exp( (*c0)[ i]);
	return res;
      }
    else if( t == COMPLEX)
      {
	DComplexGDL *c0 = static_cast< DComplexGDL*>( p0);
	DComplexGDL *res = c0->New( c0->Dim(), BaseGDL::NOZERO);
	for( SizeT i=0; i<nEl; ++i)
	  (*res)[ i] = exp( (*c0)[ i]);
	return res;
      }
    else if( t == DOUBLE)
      {
	DDoubleGDL *c0 = static_cast< DDoubleGDL*>( p0);
	DDoubleGDL *res = c0->New( c0->Dim(), BaseGDL::NOZERO);
	for( SizeT i=0; i<nEl; ++i)
	  (*res)[ i] = exp( (*c0)[ i]);
	return res;
      }
    else if( t == FLOAT)
      {
	DFloatGDL *c0 = static_cast< DFloatGDL*>( p0);
	DFloatGDL *res = c0->New( c0->Dim(), BaseGDL::NOZERO);
	for( SizeT i=0; i<nEl; ++i)
	  (*res)[ i] = exp( (*c0)[ i]);
	return res;
      }
    else
      {
	DFloatGDL *res = 
	  static_cast< DFloatGDL*>( p0->Convert2( FLOAT, BaseGDL::COPY));
	
	for( SizeT i=0; i<nEl; ++i)
	  (*res)[ i] = exp( (*res)[ i]);
	
	return res;
      }
  }

  // by medericboquien@users.sourceforge.net
   BaseGDL* gauss_pdf(EnvT* e)
  {
    SizeT nParam = e->NParam(1);
    DDoubleGDL* v = static_cast<DDoubleGDL*>(e->GetParDefined(0)->
					     Convert2(DOUBLE,BaseGDL::COPY));
    SizeT nv = v->N_Elements();

    for (int count = 0;count < nv;++count)
      (*v)[count] = gsl_cdf_ugaussian_P((*v)[count]);

    if (e->GetParDefined(0)->Type() == DOUBLE)
      return v;
    else
      return v->Convert2(FLOAT,BaseGDL::CONVERT);
    return new DByteGDL(0);
  }

  // by medericboquien@users.sourceforge.net
   BaseGDL* gauss_cvf(EnvT* e)
  {
    SizeT nParam = e->NParam(1);
    DDoubleGDL* p = static_cast<DDoubleGDL*>(e->GetParDefined(0)->
					     Convert2(DOUBLE,BaseGDL::COPY));
     
    if (p->N_Elements() != 1)
      e->Throw("Parameter must be scalar or one element array: "+
	       e->GetParString(0));
    if ((*p)[0] < 0. || (*p)[0] > 1.)
      e->Throw("Parameter must be in [0,1]: "+e->GetParString(0));

    (*p)[0] = gsl_cdf_ugaussian_Qinv((*p)[0]);

    if (e->GetParDefined(0)->Type() == DOUBLE)
      return p;
    else
      return p->Convert2(FLOAT,BaseGDL::CONVERT);
    return new DByteGDL(0);
  }

  // by medericboquien@users.sourceforge.net
  BaseGDL* t_pdf(EnvT* e)
  {
    SizeT nParam = e->NParam(2);
    DDoubleGDL* v = e->GetParAs<DDoubleGDL>(0);
    DDoubleGDL* df = e->GetParAs<DDoubleGDL>(1);
    DDoubleGDL* res;
    
    SizeT nv = v->N_Elements();
    SizeT ndf = df->N_Elements();

    for (int i=0;i<ndf;++i)
      if ((*df)[i] <= 0.)
        e->Throw("Degrees of freedom must be positive.");

    if (nv == 1 && ndf == 1) {
        res = new DDoubleGDL(dimension(1), BaseGDL::NOZERO);
        (*res)[0] = gsl_cdf_tdist_P((*v)[0],(*df)[0]);
    } else if (nv > 1 && ndf == 1) {
      res = new DDoubleGDL(dimension(nv), BaseGDL::NOZERO);
      for (SizeT count = 0; count < nv; ++count)
        (*res)[count] = gsl_cdf_tdist_P((*v)[count],(*df)[0]);
    } else if (nv == 1 && ndf > 1) {
      res = new DDoubleGDL(dimension(ndf), BaseGDL::NOZERO);
      for (SizeT count = 0; count < ndf; ++count)
        (*res)[count] = gsl_cdf_tdist_P((*v)[0],(*df)[count]);
    } else {
      SizeT nreturn = nv>ndf?ndf:nv;
      res = new DDoubleGDL(dimension(nreturn), BaseGDL::NOZERO);
      for (SizeT count = 0; count < nreturn; ++count)
        (*res)[count] = gsl_cdf_tdist_P((*v)[count],(*df)[count]);
    }
    
    if (e->GetParDefined(0)->Type() != DOUBLE && e->GetParDefined(0)->Type() != DOUBLE)
      return res->Convert2(FLOAT,BaseGDL::CONVERT);
    else
      return res;
    return new DByteGDL(0);
  }

  // by medericboquien@users.sourceforge.net
   BaseGDL* laguerre(EnvT* e)
  {
    SizeT nParam = e->NParam(2);

    DDoubleGDL* xvals = e->GetParAs<DDoubleGDL>(0);
    if(e->GetParDefined(0)->Type() == COMPLEX || e->GetParDefined(0)->Type() == COMPLEXDBL)
      e->Throw("Complex Laguerre not implemented: ");
    
    DIntGDL* nval = e->GetParAs<DIntGDL>(1);
    if (nval->N_Elements() != 1)
      e->Throw("N and K must be scalars.");
    if ((*nval)[0] < 0)
      e->Throw("Argument N must be greater than or equal to zero.");
    
    DDoubleGDL* kval;
    if (nParam>2) {
      kval = e->GetParAs<DDoubleGDL>(2);
      if(kval->N_Elements() != 1)
        e->Throw("N and K must be scalars.");
      if ((*kval)[0] < 0.)
        e->Throw("Argument K must be greater than or equal to zero.");
    } else {
      kval = new DDoubleGDL(0);
      e->Guard(kval);
    }

    DDoubleGDL* res = new DDoubleGDL(xvals->Dim(),BaseGDL::NOZERO);
    DDouble k = (*kval)[0];
    DInt n = (*nval)[0];
    SizeT nEx = xvals->N_Elements();
    SizeT count;
    
    for (count = 0;count<nEx;++count)
      (*res)[count] = gsl_sf_laguerre_n(n,k,(*xvals)[count]);

    static DInt doubleKWIx = e->KeywordIx("DOUBLE");
    static DInt coefKWIx = e->KeywordIx("COEFFICIENTS");   
    
    if(e->KeywordPresent(coefKWIx)) {
      double dcount;
      double gamma_kn1 = gsl_sf_gamma(k+n+1.);
      DDoubleGDL* coefKW = new DDoubleGDL(dimension(n+1) , BaseGDL::NOZERO);

      for(count = 0;count<=n;++count) {
        dcount = static_cast<double>(count);
        (*coefKW)[count] = ((count & 0x0001)?-1.0:1.0)*gamma_kn1/
          (gsl_sf_gamma(n-dcount+1.)*gsl_sf_gamma(k+dcount+1.)*
           gsl_sf_gamma(dcount+1.));
        }
        
      if(e->GetParDefined(0)->Type() != DOUBLE && !e->KeywordSet(doubleKWIx))
        coefKW = static_cast<DDoubleGDL*>(coefKW->
                        Convert2(FLOAT,BaseGDL::CONVERT));
      e->SetKW(coefKWIx, coefKW);
    }

    //convert things back
    if(e->GetParDefined(0)->Type() != DOUBLE && !e->KeywordSet(doubleKWIx))
      return res->Convert2(FLOAT,BaseGDL::CONVERT);
    else
      return res;

  return new DByteGDL(0);
  }


} // namespace
