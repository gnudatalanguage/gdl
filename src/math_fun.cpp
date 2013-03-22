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

#include <memory>
#include <complex>
#include <cmath>

#include <gsl/gsl_sf.h>
#include <gsl/gsl_sf_laguerre.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_linalg.h>

#include "objects.hpp"
#include "datatypes.hpp"
#include "envt.hpp"
#include "math_utl.hpp"
#include "math_fun.hpp"

//#define GDL_DEBUG
#undef GDL_DEBUG

#ifdef _MSC_VER
#define round(f) floor(f+0.5)
#endif

namespace lib {

  using namespace std;

  template< typename srcT, typename destT>
  void TransposeFromToGSL(  srcT* src, destT* dest, SizeT srcStride1, SizeT nEl)
  {
    for( SizeT d = 0, ix = 0, srcDim0 = 0; d<nEl; ++d)
      {
	dest[ d] = src[ ix];
	ix += srcStride1;
	if( ix >= nEl) 
	  ix = ++srcDim0;
      }
  }

  template< typename srcT, typename destT>
  void FromToGSL(  srcT* src, destT* dest, SizeT nEl)
  {
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
// #pragma omp for
      for( SizeT d = 0; d<nEl; ++d)
	{
	  dest[ d] = src[ d];
	}
    }
  }

  void svdc( EnvT* e)
  {
    e->NParam( 4);

    static int doubleKWIx = e->KeywordIx( "DOUBLE");
    bool doubleKW = e->KeywordSet( doubleKWIx);

    BaseGDL* A = e->GetParDefined( 0);
    doubleKW = doubleKW || (dynamic_cast< DDoubleGDL*>( A) != NULL) || (dynamic_cast< DComplexDblGDL*>( A) != NULL);

    if( doubleKW)
      {
	A = e->GetParAs< DDoubleGDL>( 0);
      }
    else
      {
	A = e->GetParAs< DFloatGDL>( 0);
      }
    if( A->Rank() != 2)
      e->Throw( "Argument must be a 2-D matrix: "+e->GetParString(0));
    
    e->AssureGlobalPar( 1); // W
    e->AssureGlobalPar( 2); // U
    e->AssureGlobalPar( 3); // V
    
    static int columnKWIx = e->KeywordIx( "COLUMN");
    bool columnKW = e->KeywordSet( columnKWIx);
    static int itmaxKWIx  = e->KeywordIx( "ITMAX");
    DLong itMax = 30;
    e->AssureLongScalarKWIfPresent( itmaxKWIx, itMax);

    DLong n;
    DLong m;
    if( columnKW)
      {
	n = A->Dim( 1);
	m = A->Dim( 0);
      }
    else
      {
	n = A->Dim( 0);
	m = A->Dim( 1);
      }
    if( m < n)
      e->Throw( "SVD of NxM matrix with N>M is not implemented yet.");

    DLong nEl = A->N_Elements();

    if( doubleKW)
      {
	DDoubleGDL* AA = static_cast<DDoubleGDL*>( A);

	gsl_matrix *aGSL = gsl_matrix_alloc( m, n);
	GDLGuard<gsl_matrix> g1( aGSL, gsl_matrix_free);
	if( !columnKW)
	  memcpy(aGSL->data, &(*AA)[0], nEl*sizeof( double));
	else
	  TransposeFromToGSL< DDouble, double>( &(*AA)[0], aGSL->data, AA->Dim( 0), nEl);

	gsl_matrix *vGSL = gsl_matrix_alloc( n, n);
	GDLGuard<gsl_matrix> g2( vGSL, gsl_matrix_free);
	gsl_vector *wGSL = gsl_vector_alloc( n);
	GDLGuard<gsl_vector> g3( wGSL, gsl_vector_free);

	gsl_vector *work = gsl_vector_alloc( n);
	GDLGuard<gsl_vector> g4( work, gsl_vector_free);
	gsl_linalg_SV_decomp( aGSL, vGSL, wGSL, work);
// 	gsl_vector_free( work);

	// aGSL -> uGSL
	gsl_matrix *uGSL = aGSL; // why?

	// U
	DDoubleGDL* U = new DDoubleGDL( AA->Dim(), BaseGDL::NOZERO);
	if( !columnKW)
	  memcpy( &(*U)[0], uGSL->data, nEl*sizeof( double));
	else
	  TransposeFromToGSL< double, DDouble>( uGSL->data, &(*U)[0], U->Dim( 1), nEl);
// 	gsl_matrix_free( uGSL);
	e->SetPar( 2, U);

	// V
	DDoubleGDL* V = new DDoubleGDL( dimension( n, n), BaseGDL::NOZERO);
	if( !columnKW)
	  memcpy( &(*V)[0], vGSL->data, n*n*sizeof( double));
	else
	  TransposeFromToGSL< double, DDouble>( vGSL->data, &(*V)[0], n, n*n);
// 	gsl_matrix_free( vGSL);
	e->SetPar( 3, V);

	// W
	DDoubleGDL* W = new DDoubleGDL( dimension( n), BaseGDL::NOZERO);
	memcpy( &(*W)[0], wGSL->data, n*sizeof( double));
// 	gsl_vector_free( wGSL);
	e->SetPar( 1, W);
      }
    else // float
      {
	DFloatGDL* AA = static_cast<DFloatGDL*>( A);

	gsl_matrix *aGSL = gsl_matrix_alloc( m, n);
	GDLGuard<gsl_matrix> g1( aGSL, gsl_matrix_free);
	if( !columnKW)
	  FromToGSL< DFloat, double>( &(*AA)[0], aGSL->data, nEl);
	else
	  TransposeFromToGSL< DFloat, double>( &(*AA)[0], aGSL->data, AA->Dim( 0), nEl);

	gsl_matrix *vGSL = gsl_matrix_alloc( n, n);
	GDLGuard<gsl_matrix> g2( vGSL, gsl_matrix_free);
	gsl_vector *wGSL = gsl_vector_alloc( n);
	GDLGuard<gsl_vector> g3( wGSL, gsl_vector_free);

	gsl_vector *work = gsl_vector_alloc( n);
	GDLGuard<gsl_vector> g4( work, gsl_vector_free);
	gsl_linalg_SV_decomp( aGSL, vGSL, wGSL, work);
// 	gsl_vector_free( work);

	// aGSL -> uGSL
	gsl_matrix *uGSL = aGSL; // why?

	// U
	DFloatGDL* U = new DFloatGDL( AA->Dim(), BaseGDL::NOZERO);
	if( !columnKW)
	  FromToGSL< double, DFloat>( uGSL->data, &(*U)[0], nEl);
	else
	  TransposeFromToGSL< double, DFloat>( uGSL->data, &(*U)[0], U->Dim( 1), nEl);
// 	gsl_matrix_free( uGSL);
	e->SetPar( 2, U);

	// V
	DFloatGDL* V = new DFloatGDL( dimension( n, n), BaseGDL::NOZERO);
	if( !columnKW)
	  FromToGSL< double, DFloat>( vGSL->data, &(*V)[0], n*n);
	else
	  TransposeFromToGSL< double, DFloat>( vGSL->data, &(*V)[0], n, n*n);
// 	gsl_matrix_free( vGSL);
	e->SetPar( 3, V);

	// W
	DFloatGDL* W = new DFloatGDL( dimension( n), BaseGDL::NOZERO);
	FromToGSL< double, DFloat>( wGSL->data, &(*W)[0], n);
// 	gsl_vector_free( wGSL);
	e->SetPar( 1, W);
      }
  }


  template< typename T>
  BaseGDL* sin_fun_template( BaseGDL* p0)
  {
    T* p0C = static_cast<T*>( p0);
    T* res = new T( p0C->Dim(), BaseGDL::NOZERO);
    SizeT nEl = p0->N_Elements();
// eigen is not faster here
// #ifdef USE_EIGEN
// 
//   Eigen::Map<Eigen::Array<typename T::Ty,Eigen::Dynamic,1> ,Eigen::Aligned> m1(&(*p0C)[0], nEl);
//   Eigen::Map<Eigen::Array<typename T::Ty,Eigen::Dynamic,1> ,Eigen::Aligned> m2(&(*res)[0], nEl);
//   m2 = m1.sin();
//   return res;
// #else
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( OMPInt i=0; i<nEl; ++i)
	{
	  (*res)[ i] = sin((*p0C)[ i]); 
	}
    }
    return res;
// #endif
    
  }

  BaseGDL* sin_fun( BaseGDL* p0, bool isReference)
  {
    assert( p0 != NULL);
    assert( p0->N_Elements() > 0);

//     e->NParam( 1);//, "SIN");
// 
//     BaseGDL* p0 = e->GetParDefined( 0);//, "SIN");
// 
    SizeT nEl = p0->N_Elements();
//     if( nEl == 0)
//       e->Throw( 
// 	       "Variable is undefined: "+e->GetParString(0));
    
    if( p0->Type() == GDL_COMPLEX)
      return sin_fun_template< DComplexGDL>( p0);
    else if( p0->Type() == GDL_COMPLEXDBL)
      return sin_fun_template< DComplexDblGDL>( p0);
    else if( p0->Type() == GDL_DOUBLE)
      return sin_fun_template< DDoubleGDL>( p0);
    else if( p0->Type() == GDL_FLOAT)
      return sin_fun_template< DFloatGDL>( p0);
    else 
      {
	DFloatGDL* res = static_cast<DFloatGDL*>
	  (p0->Convert2( GDL_FLOAT, BaseGDL::COPY));
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( OMPInt i=0; i<nEl; ++i)
	    {
	      (*res)[ i] = sin((*res)[ i]); 
	    }
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
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( OMPInt i=0; i<nEl; ++i)
	{
	  (*res)[ i] = cos((*p0C)[ i]); 
	}
    }
    return res;
  }

  BaseGDL* cos_fun( BaseGDL* p0, bool isReference)
  {
    assert( p0 != NULL);
    assert( p0->N_Elements() > 0);

//     SizeT nParam=e->NParam();
// 
//     if( nParam == 0)
//       e->Throw( 
// 	       "Incorrect number of arguments.");
// 
//     BaseGDL* p0 = e->GetParDefined( 0);//, "COS");
// 
    SizeT nEl = p0->N_Elements();
//     if( nEl == 0)
//       e->Throw( 
// 	       "Variable is undefined: "+e->GetParString(0));
    
    if( p0->Type() == GDL_COMPLEX)
      return cos_fun_template< DComplexGDL>( p0);
    else if( p0->Type() == GDL_COMPLEXDBL)
      return cos_fun_template< DComplexDblGDL>( p0);
    else if( p0->Type() == GDL_DOUBLE)
      return cos_fun_template< DDoubleGDL>( p0);
    else if( p0->Type() == GDL_FLOAT)
      return cos_fun_template< DFloatGDL>( p0);
    else 
      {
	DFloatGDL* res = static_cast<DFloatGDL*>
	  (p0->Convert2( GDL_FLOAT, BaseGDL::COPY));
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( OMPInt i=0; i<nEl; ++i)
	    {
	      (*res)[ i] = cos((*res)[ i]); 
	    }
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
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( OMPInt i=0; i<nEl; ++i)
	{
	  (*res)[ i] = tan((*p0C)[ i]); 
	}
    }
    return res;
  }

template<>
  BaseGDL* tan_fun_template< DComplexGDL>( BaseGDL* p0)
  {
  typedef DComplexGDL T;
    T* p0C = static_cast<T*>( p0);
    T* res = new T( p0C->Dim(), BaseGDL::NOZERO);
    SizeT nEl = p0->N_Elements();
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( OMPInt i=0; i<nEl; ++i)
	{
	  (*res)[ i] = tan(static_cast<DComplexDbl>((*p0C)[ i]));
	}
    }
    return res;
  }

  BaseGDL* tan_fun( BaseGDL* p0, bool isReference)
  {
    assert( p0 != NULL);
    assert( p0->N_Elements() > 0);

//     SizeT nParam=e->NParam();
// 
//     if( nParam == 0)
//       e->Throw( 
// 	       "Incorrect number of arguments.");
// 
//     BaseGDL* p0 = e->GetParDefined( 0);//, "TAN");
// 
    SizeT nEl = p0->N_Elements();
//     if( nEl == 0)
//       e->Throw( 
// 	       "Variable is undefined: "+e->GetParString(0));
    
    if( p0->Type() == GDL_COMPLEX)
      return tan_fun_template< DComplexGDL>( p0);
    else if( p0->Type() == GDL_COMPLEXDBL)
      return tan_fun_template< DComplexDblGDL>( p0);
    else if( p0->Type() == GDL_DOUBLE)
      return tan_fun_template< DDoubleGDL>( p0);
    else if( p0->Type() == GDL_FLOAT)
      return tan_fun_template< DFloatGDL>( p0);
    else 
      {
	DFloatGDL* res = static_cast<DFloatGDL*>
	  (p0->Convert2( GDL_FLOAT, BaseGDL::COPY));
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( OMPInt i=0; i<nEl; ++i)
	    {
	      (*res)[ i] = tan((*res)[ i]); 
	    }
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
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( OMPInt i=0; i<nEl; ++i)
	{
	  (*res)[ i] = sinh((*p0C)[ i]); 
	}
    }
    return res;
  }

  BaseGDL* sinh_fun( BaseGDL* p0, bool isReference)
  {
    assert( p0 != NULL);
    assert( p0->N_Elements() > 0);

//     SizeT nParam=e->NParam();
// 
//     if( nParam == 0)
//       e->Throw( 
// 	       "Incorrect number of arguments.");
// 
//     BaseGDL* p0 = e->GetParDefined( 0);//, "SINH");
// 
    SizeT nEl = p0->N_Elements();
//     if( nEl == 0)
//       e->Throw( 
// 	       "Variable is undefined: "+e->GetParString(0));
    
    if( p0->Type() == GDL_COMPLEX)
      return sinh_fun_template< DComplexGDL>( p0);
    else if( p0->Type() == GDL_COMPLEXDBL)
      return sinh_fun_template< DComplexDblGDL>( p0);
    else if( p0->Type() == GDL_DOUBLE)
      return sinh_fun_template< DDoubleGDL>( p0);
    else if( p0->Type() == GDL_FLOAT)
      return sinh_fun_template< DFloatGDL>( p0);
    else 
      {
	DFloatGDL* res = static_cast<DFloatGDL*>
	  (p0->Convert2( GDL_FLOAT, BaseGDL::COPY));
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( OMPInt i=0; i<nEl; ++i)
	    {
	      (*res)[ i] = sinh((*res)[ i]); 
	    }
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
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( OMPInt i=0; i<nEl; ++i)
	{
	  (*res)[ i] = cosh((*p0C)[ i]); 
	}
    }
    return res;
  }

  BaseGDL* cosh_fun( BaseGDL* p0, bool isReference)
  {
    assert( p0 != NULL);
    assert( p0->N_Elements() > 0);
    
//     SizeT nParam=e->NParam();
// 
//     if( nParam == 0)
//       e->Throw( 
// 	       "Incorrect number of arguments.");
// 
//     BaseGDL* p0 = e->GetParDefined( 0);//, "COSH");
// 
    SizeT nEl = p0->N_Elements();
//     if( nEl == 0)
//       e->Throw( 
// 	       "Variable is undefined: "+e->GetParString(0));
    
    if( p0->Type() == GDL_COMPLEX)
      return cosh_fun_template< DComplexGDL>( p0);
    else if( p0->Type() == GDL_COMPLEXDBL)
      return cosh_fun_template< DComplexDblGDL>( p0);
    else if( p0->Type() == GDL_DOUBLE)
      return cosh_fun_template< DDoubleGDL>( p0);
    else if( p0->Type() == GDL_FLOAT)
      return cosh_fun_template< DFloatGDL>( p0);
    else 
      {
	DFloatGDL* res = static_cast<DFloatGDL*>
	  (p0->Convert2( GDL_FLOAT, BaseGDL::COPY));
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( OMPInt i=0; i<nEl; ++i)
	    {
	      (*res)[ i] = cosh((*res)[ i]); 
	    }
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
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( OMPInt i=0; i<nEl; ++i)
	{
	  (*res)[ i] = tanh((*p0C)[ i]); 
	}
    }
    return res;
  }

  BaseGDL* tanh_fun( BaseGDL* p0, bool isReference)
  {
    assert( p0 != NULL);
    assert( p0->N_Elements() > 0);

//     SizeT nParam=e->NParam();
// 
//     if( nParam == 0)
//       e->Throw( 
// 	       "Incorrect number of arguments.");
// 
//     BaseGDL* p0 = e->GetParDefined( 0);//, "TANH");
// 
//     if( nEl == 0)
//       e->Throw( 
// 	       "Variable is undefined: "+e->GetParString(0));
    
    if( p0->Type() == GDL_COMPLEX)
      return tanh_fun_template< DComplexGDL>( p0);
    else if( p0->Type() == GDL_COMPLEXDBL)
      return tanh_fun_template< DComplexDblGDL>( p0);
    else if( p0->Type() == GDL_DOUBLE)
      return tanh_fun_template< DDoubleGDL>( p0);
    else if( p0->Type() == GDL_FLOAT)
      return tanh_fun_template< DFloatGDL>( p0);
    else 
      {
	DFloatGDL* res = static_cast<DFloatGDL*>
	  (p0->Convert2( GDL_FLOAT, BaseGDL::COPY));
	SizeT nEl = p0->N_Elements();
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( OMPInt i=0; i<nEl; ++i)
	    {
	      (*res)[ i] = tanh((*res)[ i]); 
	    }
	}
	return res;
      }
  }

  BaseGDL* asin_fun( BaseGDL* p0, bool isReference)
  {
    assert( p0 != NULL);
    assert( p0->N_Elements() > 0);
//     e->NParam( 1);//, "ASIN");
// 
//     BaseGDL* p0 = e->GetParDefined( 0);//, "ASIN");
// 
    SizeT nEl = p0->N_Elements();
//     if( nEl == 0)
//       e->Throw( 
// 	       "Variable is undefined: "+e->GetParString(0));
 
    if( p0->Type() == GDL_COMPLEX || p0->Type() == GDL_COMPLEXDBL)
      {
	throw GDLException( "Operation illegal with complex type.");
      }
    else if( p0->Type() == GDL_DOUBLE)
      {
	DDoubleGDL* p0D = static_cast<DDoubleGDL*>( p0);
	DDoubleGDL* res = new DDoubleGDL( p0->Dim(), BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( OMPInt i=0; i<nEl; ++i)
	    {
	      (*res)[ i] = asin((*p0D)[ i]); 
	    }
	}
	return res;
      }
    else if( p0->Type() == GDL_FLOAT)
      {
	DFloatGDL* p0F = static_cast<DFloatGDL*>( p0);
	DFloatGDL* res = new DFloatGDL( p0->Dim(), BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( OMPInt i=0; i<nEl; ++i)
	    {
	      (*res)[ i] = asin((*p0F)[ i]); 
	    }
	}
	return res;
      }
    else 
      {
	DFloatGDL* res = static_cast<DFloatGDL*>
	  (p0->Convert2( GDL_FLOAT, BaseGDL::COPY));
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( OMPInt i=0; i<nEl; ++i)
	    {
	      (*res)[ i] = asin((*res)[ i]); 
	    }
	}
	return res;
      }
  }

  BaseGDL* acos_fun( BaseGDL* p0, bool isReference)
  {
    assert( p0 != NULL);
    assert( p0->N_Elements() > 0);
//     e->NParam( 1);//, "ACOS");
// 
//     BaseGDL* p0 = e->GetParDefined( 0);//, "ACOS");
// 
    SizeT nEl = p0->N_Elements();
//     if( nEl == 0)
//       e->Throw( 
// 	       "Variable is undefined: "+e->GetParString(0));
    
    if( p0->Type() == GDL_COMPLEX || p0->Type() == GDL_COMPLEXDBL)
      {
	throw GDLException( "Operation illegal with complex type.");
      }
    else if( p0->Type() == GDL_DOUBLE)
      {
	DDoubleGDL* p0D = static_cast<DDoubleGDL*>( p0);
	DDoubleGDL* res = new DDoubleGDL( p0->Dim(), BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( OMPInt i=0; i<nEl; ++i)
	    {
	      (*res)[ i] = acos((*p0D)[ i]); 
	    }
	}
	return res;
      }
    else if( p0->Type() == GDL_FLOAT)
      {
	DFloatGDL* p0F = static_cast<DFloatGDL*>( p0);
	DFloatGDL* res = new DFloatGDL( p0->Dim(), BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( OMPInt i=0; i<nEl; ++i)
	    {
	      (*res)[ i] = acos((*p0F)[ i]); 
	    }
	}	return res;
      }
    else 
      {
	DFloatGDL* res = static_cast<DFloatGDL*>
	  (p0->Convert2( GDL_FLOAT, BaseGDL::COPY));
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( OMPInt i=0; i<nEl; ++i)
	    {
	      (*res)[ i] = acos((*res)[ i]); 
	    }
	}
	return res;
      }
  }

  // atan() for complex
  template< typename C>
  inline C atanC(const C& c)
  {
//     double x = c.real();
//     double x2 = x * x;
//     double y = c.imag();
//     return C(0.5 * atan2(2.0*x, 1.0 - x2 - y*y), 0.25 * log( (x2 + (y+1)*(y+1)) / (x2 + (y-1)*(y-1)) ));
    const C i(0.0,1.0);
    const C one(1.0,0.0);
    return log( (one + i * c) / (one - i * c)) / (C(2.0,0.0)*i);
  } 
  template< typename C>
  inline C atanC(const C& c1, const C& c2)
  {
    const C i(0.0,1.0);
    //const C one(1.0,0.0);
//     return -i * log((c2 + i * c1) / (sqrt(pow(c2, 2) + pow(c1, 2))));
    return -i * log((c2 + i * c1) / sqrt((c2 * c2) + (c1 * c1)));
  }

  BaseGDL* atan_fun( EnvT* e)
  {
    SizeT nParam=e->NParam( 1);//, "ATAN");

    BaseGDL* p0 = e->GetParDefined( 0);//, "ATAN");

    SizeT nEl = p0->N_Elements();
    if( nEl == 0)
      e->Throw( 
	       "Variable is undefined: "+e->GetParString(0));
    
    if( nParam == 2)
      {
	BaseGDL* p1 = e->GetPar( 1);
	if( p1 == NULL)
	  e->Throw( 
		   "Variable is undefined: "+e->GetParString(1));
	SizeT nEl1 = p1->N_Elements();
	if( nEl1 == 0)
	  e->Throw( 
		   "Variable is undefined: "+e->GetParString(1));
	
	DType t = (DTypeOrder[ p0->Type()] > DTypeOrder[ p1->Type()])? p0->Type() : p1->Type();
 
        bool p0dim;
        if      (p0->Rank() == 0 && p1->Rank() != 0)  p0dim = false;
        else if (p0->Rank() != 0 && p1->Rank() == 0)  p0dim = true;
        else if (nEl <= nEl1)                         p0dim = true;
        else                                          p0dim = false;
 
	const dimension& dim = p0dim ? p0->Dim() : p1->Dim(); 

	SizeT nElMin = p0dim ? nEl : nEl1;
        SizeT i, zero = 0, *i0, *i1;
        i0 = p0->Rank() == 0 ? &zero : &i, 
	  i1 = p1->Rank() == 0 ? &zero : &i;

	if( t == GDL_COMPLEX)
	  {
	    Guard< DComplexGDL> guard0;
	    Guard< DComplexGDL> guard1;

	    DComplexGDL* p0F = static_cast<DComplexGDL*>(p0->Convert2( GDL_COMPLEX, BaseGDL::COPY));
	    guard0.Init( p0F);
	    DComplexGDL* p1F = static_cast<DComplexGDL*>(p1->Convert2( GDL_COMPLEX, BaseGDL::COPY));
	    guard1.Init( p1F);
	      
	    DComplexGDL* res = new DComplexGDL( dim, BaseGDL::NOZERO);
	    for (i = 0; i < nElMin; ++i) (*res)[i] = atanC((*p0F)[*i0], (*p1F)[*i1]); 
	    return res;
	  }
	else if( t == GDL_COMPLEXDBL)
	  {
	    Guard< DComplexDblGDL> guard0;
	    Guard< DComplexDblGDL> guard1;

	    DComplexDblGDL* p0F = static_cast<DComplexDblGDL*>(p0->Convert2( GDL_COMPLEXDBL, BaseGDL::COPY));
	    guard0.Init( p0F);
	    DComplexDblGDL* p1F = static_cast<DComplexDblGDL*>(p1->Convert2( GDL_COMPLEXDBL, BaseGDL::COPY));
	    guard1.Init( p1F);
	      
	    DComplexDblGDL* res = new DComplexDblGDL( dim, BaseGDL::NOZERO);
	    for (i = 0; i < nElMin; ++i) (*res)[i] = atanC((*p0F)[*i0], (*p1F)[*i1]); 
	    return res;
	  }
	else if( t == GDL_DOUBLE)
	  {
	    Guard< DDoubleGDL> guard;
	    
	    DDoubleGDL* p0D;
	    if( p0->Type() != GDL_DOUBLE)
	      {
		p0D =  static_cast<DDoubleGDL*>( p0->Convert2( GDL_DOUBLE, BaseGDL::COPY));
		guard.Reset( p0D);
	      }
	    else
	      {
		p0D =  static_cast<DDoubleGDL*>( p0);
	      }

	    DDoubleGDL* p1D;
	    if( p1->Type() != GDL_DOUBLE)
	      {
		p1D =  static_cast<DDoubleGDL*>( p1->Convert2( GDL_DOUBLE, BaseGDL::COPY));
		guard.Reset( p1D);
	      }
	    else
	      {
		p1D =  static_cast<DDoubleGDL*>( p1);
	      }

	    DDoubleGDL* res = new DDoubleGDL( dim, BaseGDL::NOZERO);
	    for (i = 0; i < nElMin; ++i) (*res)[i] = atan2((*p0D)[*i0], (*p1D)[*i1]); 
	    return res;
	  }
	else if( t == GDL_FLOAT)
	  {
	    Guard< DFloatGDL> guard;
	    
	    DFloatGDL* p0F;
	    if( p0->Type() != GDL_FLOAT)
	      {
		p0F =  static_cast<DFloatGDL*>( p0->Convert2( GDL_FLOAT, BaseGDL::COPY));
		guard.Reset( p0F);
	      }
	    else
	      {
		p0F =  static_cast<DFloatGDL*>( p0);
	      }

	    DFloatGDL* p1F;
	    if( p1->Type() != GDL_FLOAT)
	      {
		p1F =  static_cast<DFloatGDL*>( p1->Convert2( GDL_FLOAT, BaseGDL::COPY));
		guard.Reset( p1F);
	      }
	    else
	      {
		p1F =  static_cast<DFloatGDL*>( p1);
	      }

	    DFloatGDL* res = new DFloatGDL( dim, BaseGDL::NOZERO);
	    for (i = 0; i < nElMin; ++i) (*res)[i] = 
					   (float)atan2((double)(*p0F)[*i0], (double)(*p1F)[*i1]); 
	    return res;
	  }
	else 
	  {
	    Guard< DFloatGDL> guard0;
	    Guard< DFloatGDL> guard1;

	    DFloatGDL* p0F = static_cast<DFloatGDL*>(p0->Convert2( GDL_FLOAT, BaseGDL::COPY));
	    guard0.Init( p0F);
	    DFloatGDL* p1F = static_cast<DFloatGDL*>(p1->Convert2( GDL_FLOAT, BaseGDL::COPY));
	    guard1.Init( p1F);
	      
	    DFloatGDL* res = new DFloatGDL( dim, BaseGDL::NOZERO);
	    for (i = 0; i < nElMin; ++i) 
              (*res)[i] = (float)atan2((double)(*p0F)[*i0], (double)(*p1F)[*i1]); 
	    return res;
	  }
      }
    else
      {
        static int phaseIx = e->KeywordIx("PHASE");
        static float half_pi_f = .5 * atan((float(1))); 
        static double half_pi_d = .5 * atan(double(1));

	if( p0->Type() == GDL_COMPLEX && e->KeywordSet(phaseIx))
	  {
	    DComplexGDL* p0C = static_cast<DComplexGDL*>( p0);
	    DFloatGDL* res = new DFloatGDL( p0C->Dim(), BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	    {
#pragma omp for
	      for( OMPInt i=0; i<nEl; ++i)
		{
		  DComplex& C = (*p0C)[ i];
		  (*res)[ i] = (float)atan2((double)C.imag(), (double)C.real());
		}
	    }
	    return res;
	  }
	else if( p0->Type() == GDL_COMPLEX)
	  {
	    DComplexGDL* p0C = static_cast<DComplexGDL*>( p0);
	    DComplexGDL* res = new DComplexGDL( p0->Dim(), BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	    {
#pragma omp for
	      for( OMPInt i=0; i<nEl; ++i) (
			*res)[ i] = atanC((*p0C)[ i]);
	    }
	    return res;
	  }  
	else if( p0->Type() == GDL_COMPLEXDBL && e->KeywordSet(phaseIx))
	  {
	    DComplexDblGDL* p0C = static_cast<DComplexDblGDL*>( p0);
	    DDoubleGDL* res = new DDoubleGDL( p0C->Dim(), BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	    {
#pragma omp for
	       for( OMPInt i=0; i<nEl; ++i)
	      	      {
	      		DComplexDbl& C = (*p0C)[ i];
	      		(*res)[ i] = atan2( C.imag(), C.real());
	      	      }
	    }
	    return res;
	  }
	else if( p0->Type() == GDL_COMPLEXDBL)
	  {
	    DComplexDblGDL* p0C = static_cast<DComplexDblGDL*>( p0);
	    DComplexDblGDL* res = new DComplexDblGDL( p0->Dim(), BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	    {
#pragma omp for
	      for( OMPInt i=0; i<nEl; ++i)
			(*res)[ i] = atanC((*p0C)[ i]);
	    }
	    return res;
	  }  
	else if( p0->Type() == GDL_DOUBLE)
	  {
	    DDoubleGDL* p0D = static_cast<DDoubleGDL*>( p0);
	    DDoubleGDL* res = new DDoubleGDL( p0->Dim(), BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	    {
#pragma omp for
	      for( OMPInt i=0; i<nEl; ++i)
		{
		  (*res)[ i] = atan((*p0D)[ i]); 
		}
	    }
	    return res;
	  }
	else if( p0->Type() == GDL_FLOAT)
	  {
	    DFloatGDL* p0F = static_cast<DFloatGDL*>( p0);
	    DFloatGDL* res = new DFloatGDL( p0->Dim(), BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	    {
#pragma omp for
	      for( OMPInt i=0; i<nEl; ++i)
		{
		  (*res)[ i] = atan((*p0F)[ i]); 
		}
	    }
	    return res;
	  }
	else 
	  {
	    DFloatGDL* res = static_cast<DFloatGDL*>
	      (p0->Convert2( GDL_FLOAT, BaseGDL::COPY));
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	    {
#pragma omp for
	      for( OMPInt i=0; i<nEl; ++i)
		{
		  (*res)[ i] = atan((*res)[ i]); 
		}
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

//   BaseGDL* alog_fun( EnvT* e)
  BaseGDL* alog_fun( BaseGDL* p0, bool isReference)
  {
    assert( p0 != NULL);
    assert( p0->N_Elements() > 0);
    
    if( !isReference) //e->StealLocalPar( 0))
      {
	return p0->LogThis();
      }
    return p0->Log();

//       if( FloatType( p0->Type()) || ComplexType( p0->Type()))
//       if( !isReference) //e->StealLocalPar( 0))
// 	{
// 	  p0->LogThis();
// 	  return p0;
// 	}
//       else
// 	return p0->Log();
//     else 
//       {
// 	DFloatGDL* res = static_cast<DFloatGDL*>
// 	  (p0->Convert2( GDL_FLOAT, BaseGDL::COPY));
// 	res->LogThis();
// 	return res;
//       }
  }


//   BaseGDL* alog10_fun( EnvT* e)
BaseGDL* alog10_fun( BaseGDL* p0, bool isReference)
  {
    assert( p0 != NULL);
    assert( p0->N_Elements() > 0);
    
    if( !isReference) //e->StealLocalPar( 0))
      {
	return p0->Log10This();
      }
    return p0->Log10();

//     if( FloatType( p0->Type()) || ComplexType( p0->Type()))
//       if( !isReference) //e->StealLocalPar( 0))
// 	{
// 	  p0->Log10This();
// 	  return p0;
// 	}
//       else
// 	return p0->Log10();
//     else 
//       {
// 	DFloatGDL* res = static_cast<DFloatGDL*>
// 	  (p0->Convert2( GDL_FLOAT, BaseGDL::COPY));
// 	res->Log10This();
// 	return res;
//       }
  }

  // original by joel gales
  template< typename T>
  BaseGDL* sqrt_fun_template( BaseGDL* p0)
  {
    T* p0C = static_cast<T*>( p0);
    T* res = new T( p0C->Dim(), BaseGDL::NOZERO);
    SizeT nEl = p0->N_Elements();
#ifdef USE_EIGEN

  Eigen::Map<Eigen::Array<typename T::Ty,Eigen::Dynamic,1> ,Eigen::Aligned> mP0C(&(*p0C)[0], nEl);
  Eigen::Map<Eigen::Array<typename T::Ty,Eigen::Dynamic,1> ,Eigen::Aligned> mRes(&(*res)[0], nEl);
  mRes = mP0C.sqrt();
  return res;
#else

TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma intel omp forthis
      for( OMPInt i=0; i<nEl; ++i)
	{
	  (*res)[ i] = sqrt((*p0C)[ i]); 
	}
    }
    return res;
#endif
    
  }

  template< typename T>
  BaseGDL* sqrt_fun_template_grab( BaseGDL* p0)
  {
    T* p0C = static_cast<T*>( p0);
    SizeT nEl = p0->N_Elements();
#ifdef USE_EIGEN

  Eigen::Map<Eigen::Array<typename T::Ty,Eigen::Dynamic,1> ,Eigen::Aligned> mP0C(&(*p0C)[0], nEl);
  mP0C = mP0C.sqrt();
  return p0C;
#else
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( OMPInt i=0; i<nEl; ++i)
	{
	  (*p0C)[ i] = sqrt((*p0C)[ i]); 
	}
    }
    return p0C;
#endif
  }

  BaseGDL* sqrt_fun( BaseGDL* p0, bool isReference)//( EnvT* e)
  {
    assert( p0 != NULL);
    assert( p0->N_Elements() > 0);
    
    DType p0Type = p0->Type();  
    if( isReference)
    {
    if( p0Type == GDL_COMPLEX)
      return sqrt_fun_template< DComplexGDL>( p0);
    else if( p0Type == GDL_COMPLEXDBL)
      return sqrt_fun_template< DComplexDblGDL>( p0);
    else if( p0Type == GDL_DOUBLE)
      return sqrt_fun_template< DDoubleGDL>( p0);
    else if( p0Type == GDL_FLOAT)
      return sqrt_fun_template< DFloatGDL>( p0);
    }
    else
    {
    if( p0Type == GDL_COMPLEX)
      return sqrt_fun_template_grab< DComplexGDL>( p0);
    else if( p0Type == GDL_COMPLEXDBL)
      return sqrt_fun_template_grab< DComplexDblGDL>( p0);
    else if( p0Type == GDL_DOUBLE)
      return sqrt_fun_template_grab< DDoubleGDL>( p0);
    else if( p0Type == GDL_FLOAT)
      return sqrt_fun_template_grab< DFloatGDL>( p0);
    } 
    {
      DFloatGDL* res = static_cast<DFloatGDL*>
	(p0->Convert2( GDL_FLOAT, BaseGDL::COPY));
      SizeT nEl = p0->N_Elements();
#ifdef USE_EIGEN

      Eigen::Map<Eigen::Array<DFloat,Eigen::Dynamic,1> ,Eigen::Aligned> mRes(&(*res)[0], nEl);
      mRes = mRes.sqrt();
      return res;
#else
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
      {
#pragma omp for
	for( OMPInt i=0; i<nEl; ++i)
	  {
	    (*res)[ i] = sqrt( (*res)[ i]); 
	  }
      }
      return res;
#endif  
    }
  }

  template< typename T>
  BaseGDL* abs_fun_template( BaseGDL* p0)
  {
    T* p0C = static_cast<T*>( p0);
    T* res = new T( p0C->Dim(), BaseGDL::NOZERO);
    SizeT nEl = p0->N_Elements();
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( OMPInt i=0; i<nEl; ++i)
	{
	  (*res)[ i] = abs((*p0C)[ i]); 
	}
    }
    return res;
  }

  BaseGDL* abs_fun( BaseGDL* p0, bool isReference)
  {
    assert( p0 != NULL);
    assert( p0->N_Elements() > 0);
//     e->NParam( 1);
// 
//     BaseGDL* p0 = e->GetParDefined( 0);
// 
//     SizeT nEl = p0->N_Elements();
//     if( nEl == 0)
//       e->Throw( "Variable is undefined: "+e->GetParString(0));
    
    if( p0->Type() == GDL_COMPLEX) 
      {
	DComplexGDL* p0C = static_cast<DComplexGDL*>( p0);
	DFloatGDL* res = new DFloatGDL(p0C->Dim(), BaseGDL::NOZERO);
	SizeT nEl = p0->N_Elements();
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( OMPInt i=0; i<nEl; ++i)
	    {
	      // 	    DComplex& C = (*p0C)[ i];
	      // 	    float Creal = C.real(), Cimag = C.imag();
	      // 	    (*res)[ i] = sqrt(Creal*Creal + Cimag*Cimag);
	      (*res)[ i] = abs( (*p0C)[ i]); //sqrt(Creal*Creal + Cimag*Cimag);
	    }
	}
	return res;
      }
    else if( p0->Type() == GDL_COMPLEXDBL)
      {
	DComplexDblGDL* p0C = static_cast<DComplexDblGDL*>( p0);
	DDoubleGDL* res = new DDoubleGDL(p0C->Dim(), BaseGDL::NOZERO);
	SizeT nEl = p0->N_Elements();
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( OMPInt i=0; i<nEl; ++i)
	    {
	      // 	    DComplexDbl& C = (*p0C)[ i];
	      // 	    double Creal = C.real(), Cimag = C.imag();
	      // 	    (*res)[ i] = sqrt(Creal*Creal + Cimag*Cimag);
	      (*res)[ i] = abs( (*p0C)[ i]); //sqrt(Creal*Creal + Cimag*Cimag);
	    }
	}
	return res;
      }
    else if( p0->Type() == GDL_DOUBLE)
      return abs_fun_template< DDoubleGDL>( p0);
    else if( p0->Type() == GDL_FLOAT)
      return abs_fun_template< DFloatGDL>( p0);
    else if( p0->Type() == GDL_LONG64)
      return abs_fun_template< DLong64GDL>( p0);
    else if( p0->Type() == GDL_LONG)
      return abs_fun_template< DLongGDL>( p0);
    else if( p0->Type() == GDL_INT)
      return abs_fun_template< DIntGDL>( p0);
    else if( isReference)
    {
    if( p0->Type() == GDL_ULONG64)
      return p0->Dup();
    else if( p0->Type() == GDL_ULONG)
      return p0->Dup();
    else if( p0->Type() == GDL_UINT)
      return p0->Dup();
    else if( p0->Type() == GDL_BYTE)
      return p0->Dup();
    }
    else
    {
    if( p0->Type() == GDL_ULONG64)
      return p0;
    else if( p0->Type() == GDL_ULONG)
      return p0;
    else if( p0->Type() == GDL_UINT)
      return p0;
    else if( p0->Type() == GDL_BYTE)
      return p0;     
    }
    DFloatGDL* res = static_cast<DFloatGDL*>
      (p0->Convert2( GDL_FLOAT, BaseGDL::COPY));
    SizeT nEl = p0->N_Elements();
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for
      for( OMPInt i=0; i<nEl; ++i)
	{
	  (*res)[ i] = abs( (*res)[ i]); 
	}
    }
    return res;
  }


  template< typename T>
  BaseGDL* round_fun_template( BaseGDL* p0, bool isKWSetL64)
  {
    T* p0C = static_cast<T*>( p0);
    SizeT nEl = p0->N_Elements();

    // L64 keyword support
    if (isKWSetL64) {
      DLong64GDL* res = new DLong64GDL(p0C->Dim(), BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
      {
#pragma omp for
	for( OMPInt i=0; i<nEl; ++i)
	  {
	    (*res)[ i] = static_cast<DLong64>( round((*p0C)[ i])); 
	  }
      }
      return res;
    } else {
      DLongGDL* res = new DLongGDL(p0C->Dim(), BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
      {
#pragma omp for
	for( OMPInt i=0; i<nEl; ++i)
	  {
	    (*res)[ i] = static_cast<DLong>( round((*p0C)[ i])); 
	  }
      }
      return res;
    }
  }

  BaseGDL* round_fun( EnvT* e)
  {
    e->NParam( 1);//, "ROUND");

    BaseGDL* p0 = e->GetParDefined( 0);//, "ROUND");

    SizeT nEl = p0->N_Elements();
    if( nEl == 0)
      e->Throw( 
	       "ROUND: Variable is undefined: "+e->GetParString(0));

    static SizeT l64Ix = e->KeywordIx( "L64");
    bool isKWSetL64 = e->KeywordSet( l64Ix);

    if( p0->Type() == GDL_COMPLEX)
      {
	DComplexGDL* p0C = static_cast<DComplexGDL*>( p0);
	SizeT nEl = p0->N_Elements();

	// L64 keyword support
	if (isKWSetL64) {
	  DLong64GDL* res = new DLong64GDL(p0C->Dim(), BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	  {
#pragma omp for
	    for( OMPInt i=0; i<nEl; ++i)
	      {
		DComplex& C = (*p0C)[ i];
		(*res)[ i] = (DLong64) round(C.real());
	      }
	  }
	  return res;
	} else {
	  DLongGDL* res = new DLongGDL(p0C->Dim(), BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	  {
#pragma omp for
	    for( OMPInt i=0; i<nEl; ++i)
	      {
		DComplex& C = (*p0C)[ i];
		(*res)[ i] = (int) round(C.real());
	      }
	  }
	  return res;
	}
      }
    else if( p0->Type() == GDL_COMPLEXDBL)
      {
	DComplexDblGDL* p0C = static_cast<DComplexDblGDL*>( p0);
	SizeT nEl = p0->N_Elements();

	// L64 keyword support
	if (isKWSetL64) {
	  DLong64GDL* res = new DLong64GDL(p0C->Dim(), BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	  {
#pragma omp for
	    for( OMPInt i=0; i<nEl; ++i)
	      {
		DComplexDbl& C = (*p0C)[ i];
		(*res)[ i] = (DLong64) round(C.real());
	      }
	  }
	  return res;
	} else {
	  DLongGDL* res = new DLongGDL(p0C->Dim(), BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	  {
#pragma omp for
	    for( OMPInt i=0; i<nEl; ++i)
	      {
		DComplexDbl& C = (*p0C)[ i];
		(*res)[ i] = (int) round(C.real());
	      }
	  }
	  return res;
	}
      }
    else if( p0->Type() == GDL_DOUBLE)
      return round_fun_template< DDoubleGDL>( p0, isKWSetL64);
    else if( p0->Type() == GDL_FLOAT)
      return round_fun_template< DFloatGDL>( p0, isKWSetL64);
    else if( p0->Type() == GDL_LONG64)
      return p0->Dup();
    else if( p0->Type() == GDL_LONG)
      return p0->Dup();
    else if( p0->Type() == GDL_INT)
      return p0->Dup();
    else if( p0->Type() == GDL_ULONG64)
      return p0->Dup();
    else if( p0->Type() == GDL_ULONG)
      return p0->Dup();
    else if( p0->Type() == GDL_UINT)
      return p0->Dup();
    else if( p0->Type() == GDL_BYTE)
      return p0->Dup();
    else 
      {
	DFloatGDL* p0F = e->GetParAs<DFloatGDL>( 0);
	DLongGDL* res = new DLongGDL(p0->Dim(), BaseGDL::NOZERO);
	SizeT nEl = p0->N_Elements();
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( OMPInt i=0; i<nEl; ++i)
	    {
	      (*res)[ i] = (int) round((double) (*p0F)[ i]); 
	    }
	}
	return res;
      }
  }

  template< typename T>
  BaseGDL* ceil_fun_template( BaseGDL* p0, bool isKWSetL64)
  {
    T* p0C = static_cast<T*>( p0);
    SizeT nEl = p0->N_Elements();

    // L64 keyword support
    if (isKWSetL64) {
      DLong64GDL* res = new DLong64GDL(p0C->Dim(), BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
      {
#pragma omp for
	for( OMPInt i=0; i<nEl; ++i)
	  {
	    (*res)[ i] = (DLong64) ceil((*p0C)[ i]); 
	  }
      }
      return res;
    } else {
      DLongGDL* res = new DLongGDL(p0C->Dim(), BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
      {
#pragma omp for
	for( OMPInt i=0; i<nEl; ++i)
	  {
	    (*res)[ i] = (int) ceil((*p0C)[ i]); 
	  }
      }
      return res;
    }
  }

  BaseGDL* ceil_fun( EnvT* e)
  {
    e->NParam( 1);//, "CEIL");

    BaseGDL* p0 = e->GetParDefined( 0);//, "CEIL");

    SizeT nEl = p0->N_Elements();
    if( nEl == 0)
      e->Throw( 
	       "Variable is undefined: "+e->GetParString(0));

    bool isKWSetL64 = e->KeywordSet( "L64");

    if( p0->Type() == GDL_COMPLEX)
      {
	DComplexGDL* p0C = static_cast<DComplexGDL*>( p0);
	SizeT nEl = p0->N_Elements();

	// L64 keyword support
	if (isKWSetL64) {
	  DLong64GDL* res = new DLong64GDL(p0C->Dim(), BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	  {
#pragma omp for
	    for( OMPInt i=0; i<nEl; ++i)
	      {
		DComplex& C = (*p0C)[ i];
		(*res)[ i] = (DLong64) ceil(C.real());
	      }
	  }
	  return res;
	} else {
	  DLongGDL* res = new DLongGDL(p0C->Dim(), BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	  {
#pragma omp for
	    for( OMPInt i=0; i<nEl; ++i)
	      {
		DComplex& C = (*p0C)[ i];
		(*res)[ i] = (int) ceil(C.real());
	      }
	  }
	  return res;
	}
      }
    else if( p0->Type() == GDL_COMPLEXDBL)
      {
	DComplexDblGDL* p0C = static_cast<DComplexDblGDL*>( p0);
	SizeT nEl = p0->N_Elements();

	// L64 keyword support
	if (isKWSetL64) {
	  DLong64GDL* res = new DLong64GDL(p0C->Dim(), BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	  {
#pragma omp for
	    for( OMPInt i=0; i<nEl; ++i)
	      {
		DComplexDbl& C = (*p0C)[ i];
		(*res)[ i] = (DLong64) ceil(C.real());
	      }
	  }
	  return res;
	} else {
	  DLongGDL* res = new DLongGDL(p0C->Dim(), BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	  {
#pragma omp for
	    for( OMPInt i=0; i<nEl; ++i)
	      {
		DComplexDbl& C = (*p0C)[ i];
		(*res)[ i] = (int) ceil(C.real());
	      }
	  }
	  return res;
	}
      }
    else if( p0->Type() == GDL_DOUBLE)
      return ceil_fun_template< DDoubleGDL>( p0, isKWSetL64);
    else if( p0->Type() == GDL_FLOAT)
      return ceil_fun_template< DFloatGDL>( p0, isKWSetL64);
    else if( p0->Type() == GDL_LONG64)
      return p0->Dup();
    else if( p0->Type() == GDL_LONG)
      return p0->Dup();
    else if( p0->Type() == GDL_INT)
      return p0->Dup();
    else if( p0->Type() == GDL_ULONG64)
      return p0->Dup();
    else if( p0->Type() == GDL_ULONG)
      return p0->Dup();
    else if( p0->Type() == GDL_UINT)
      return p0->Dup();
    else if( p0->Type() == GDL_BYTE)
      return p0->Dup();
    else 
      {
	DFloatGDL* p0F = e->GetParAs<DFloatGDL>( 0);
	DLongGDL* res = new DLongGDL(p0->Dim(), BaseGDL::NOZERO);
	SizeT nEl = p0->N_Elements();
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( OMPInt i=0; i<nEl; ++i)
	    {
	      (*res)[ i] = (int) ceil((double) (*p0F)[ i]); 
	    }
	}
	return res;
      }
  }

  template< typename T>
  BaseGDL* floor_fun_template( BaseGDL* p0, bool isKWSetL64)
  {
    T* p0C = static_cast<T*>( p0);
    SizeT nEl = p0->N_Elements();

    // L64 keyword support
    if (isKWSetL64) {
      DLong64GDL* res = new DLong64GDL(p0C->Dim(), BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
      {
#pragma omp for
	for( OMPInt i=0; i<nEl; ++i)
	  {
	    (*res)[ i] = (DLong64) floor((*p0C)[ i]); 
	  }
      }
      return res;
    } else {
      DLongGDL* res = new DLongGDL(p0C->Dim(), BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
      {
#pragma omp for
	for( OMPInt i=0; i<nEl; ++i)
	  {
	    (*res)[ i] = (int) floor((*p0C)[ i]); 
	  }
      }
      return res;
    }
  }

  BaseGDL* floor_fun( EnvT* e)
  {
    e->NParam( 1);//, "FLOOR");

    BaseGDL* p0 = e->GetParDefined( 0);//, "FLOOR");

    SizeT nEl = p0->N_Elements();
    if( nEl == 0)
      e->Throw( 
	       "Variable is undefined: "+e->GetParString(0));

    bool isKWSetL64 = e->KeywordSet( "L64");

    if( p0->Type() == GDL_COMPLEX)
      {
	DComplexGDL* p0C = static_cast<DComplexGDL*>( p0);
	SizeT nEl = p0->N_Elements();

	// L64 keyword support
	if (isKWSetL64) {
	  DLong64GDL* res = new DLong64GDL(p0C->Dim(), BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	  {
#pragma omp for
	    for( OMPInt i=0; i<nEl; ++i)
	      {
		DComplex& C = (*p0C)[ i];
		(*res)[ i] = (DLong64) floor(C.real());
	      }
	  }
	  return res;
	} else {
	  DLongGDL* res = new DLongGDL(p0C->Dim(), BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	  {
#pragma omp for
	    for( OMPInt i=0; i<nEl; ++i)
	      {
		DComplex& C = (*p0C)[ i];
		(*res)[ i] = (int) floor(C.real());
	      }
	  }
	  return res;
	}
      }
    else if( p0->Type() == GDL_COMPLEXDBL)
      {
	DComplexDblGDL* p0C = static_cast<DComplexDblGDL*>( p0);
	SizeT nEl = p0->N_Elements();

	// L64 keyword support
	if (isKWSetL64) {
	  DLong64GDL* res = new DLong64GDL(p0C->Dim(), BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	  {
#pragma omp for
	    for( OMPInt i=0; i<nEl; ++i)
	      {
		DComplexDbl& C = (*p0C)[ i];
		(*res)[ i] = (DLong64) floor(C.real());
	      }
	  }
	  return res;
	} else {
	  DLongGDL* res = new DLongGDL(p0C->Dim(), BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	  {
#pragma omp for
	    for( OMPInt i=0; i<nEl; ++i)
	      {
		DComplexDbl& C = (*p0C)[ i];
		(*res)[ i] = (int) floor(C.real());
	      }
	  }
	  return res;
	}
      }
    else if( p0->Type() == GDL_DOUBLE)
      return floor_fun_template< DDoubleGDL>( p0, isKWSetL64);
    else if( p0->Type() == GDL_FLOAT)
      return floor_fun_template< DFloatGDL>( p0, isKWSetL64);
    else if( p0->Type() == GDL_LONG64)
      return p0->Dup();
    else if( p0->Type() == GDL_LONG)
      return p0->Dup();
    else if( p0->Type() == GDL_INT)
      return p0->Dup();
    else if( p0->Type() == GDL_ULONG64)
      return p0->Dup();
    else if( p0->Type() == GDL_ULONG)
      return p0->Dup();
    else if( p0->Type() == GDL_UINT)
      return p0->Dup();
    else if( p0->Type() == GDL_BYTE)
      return p0->Dup();
    else 
      {
	DFloatGDL* p0F = e->GetParAs<DFloatGDL>( 0);
	DLongGDL* res = new DLongGDL(p0->Dim(), BaseGDL::NOZERO);
	SizeT nEl = p0->N_Elements();
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( OMPInt i=0; i<nEl; ++i)
	    {
	      (*res)[ i] = (int) floor((double) (*p0F)[ i]); 
	    }
	}
	return res;
      }
  }

  BaseGDL* conj_fun( BaseGDL* p0, bool isReference)//( EnvT* e)
  {
    assert( p0 != NULL);
    assert( p0->N_Elements() > 0);

//     e->NParam( 1);
//     BaseGDL* p0 = e->GetParDefined( 0);
    SizeT nEl = p0->N_Elements();
    //    if( nEl == 0)
    //      e->Throw( "Variable is undefined: "+e->GetParString(0));
    
    if( p0->Type() == GDL_COMPLEX)
      {
	DComplexGDL* res = static_cast<DComplexGDL*>(p0)->NewResult();// static_cast<DComplexGDL*>(p0->Dup());
	DComplexGDL* p0C = static_cast<DComplexGDL*>(p0);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( OMPInt i=0; i<nEl; ++i)
	    {
	      (*res)[i] = DComplex( (*p0C)[i].real(), -(*p0C)[i].imag());
	    }
	}
	return res;
      }
    if( p0->Type() == GDL_COMPLEXDBL)
      {
	DComplexDblGDL* res = static_cast<DComplexDblGDL*>(p0)->NewResult();//static_cast<DComplexDblGDL*>(p0->Dup());
	DComplexDblGDL* p0C = static_cast<DComplexDblGDL*>(p0);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( OMPInt i=0; i<nEl; ++i)
	    {
	      (*res)[i] = DComplexDbl( (*p0C)[i].real(), -(*p0C)[i].imag());
	    }
	}
	return res;
      }
    if( p0->Type() == GDL_DOUBLE || 
	p0->Type() == GDL_LONG64 || 
	p0->Type() == GDL_ULONG64)
      {
	DComplexDblGDL* res = static_cast<DComplexDblGDL*>
	  (p0->Convert2( GDL_COMPLEXDBL, BaseGDL::COPY));
	return res;
      }

    // all other types
    DComplexGDL* res = 
      static_cast<DComplexGDL*>( p0->Convert2( GDL_COMPLEX, BaseGDL::COPY));
    return res;
  }

  BaseGDL* imaginary_fun( BaseGDL* p0, bool isReference)//( EnvT* e)
  {
    assert( p0 != NULL);
    assert( p0->N_Elements() > 0);

//     e->NParam( 1);
//     BaseGDL* p0 = e->GetParDefined( 0);
    SizeT nEl = p0->N_Elements();

    //    if( nEl == 0) 
    //      e->Throw( "Variable is undefined: "+e->GetParString(0));
    
    // complex types, return imaginary part
    if( p0->Type() == GDL_COMPLEX)
      {
	DComplexGDL* c0 = static_cast<DComplexGDL*>(p0);
	DFloatGDL* res = new DFloatGDL( c0->Dim(), BaseGDL::NOZERO);
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
// #pragma omp for
	  for( SizeT i=0; i<nEl; ++i)
	    {
	      (*res)[i] = (*c0)[i].imag();
	    }
	}
	return res;
      }
    if( p0->Type() == GDL_COMPLEXDBL)
      {
	DComplexDblGDL* c0 = static_cast<DComplexDblGDL*>(p0);
	DDoubleGDL* res = new DDoubleGDL( c0->Dim(), BaseGDL::NOZERO);
// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
// #pragma omp for
	  for( SizeT i=0; i<nEl; ++i)
	    {
	      (*res)[i] = (*c0)[i].imag();
	    }
	}
	return res;
      }

    // forbidden types
    DType t = p0->Type();
    if( t == GDL_STRING)
      throw GDLException( "String expression not allowed in this context.");
    if( t == GDL_STRUCT)
      throw GDLException( "Struct expression not allowed in this context.");
    if( t == GDL_PTR)
      throw GDLException( "Pointer expression not allowed in this context.");
    if( t == GDL_OBJ)
      throw GDLException( "Object reference not allowed in this context.");
    
    // all other types (return array of zeros)
    DFloatGDL* res = new DFloatGDL( p0->Dim()); // ZERO
    return res;
  }

  BaseGDL* exp_fun( BaseGDL* p0, bool isReference)
  {
    assert( p0 != NULL);
    assert( p0->N_Elements() > 0);
	
//     e->NParam( 1);
//     BaseGDL* p0 = e->GetParDefined( 0);

	SizeT nEl = p0->N_Elements();

    DType t = p0->Type();
	if( t == GDL_COMPLEXDBL)
      {
	DComplexDblGDL *c0 = static_cast< DComplexDblGDL*>( p0);
	DComplexDblGDL *res = c0->New( c0->Dim(), BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( OMPInt i=0; i<nEl; ++i)
	    (*res)[ i] = exp( (*c0)[ i]);
	}
	return res;
      }
    else if( t == GDL_COMPLEX)
      {
	DComplexGDL *c0 = static_cast< DComplexGDL*>( p0);
	DComplexGDL *res = c0->New( c0->Dim(), BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( OMPInt i=0; i<nEl; ++i)
	    (*res)[ i] = exp( (*c0)[ i]);
	}
	return res;
      }
    else if( t == GDL_DOUBLE)
      {
	DDoubleGDL *c0 = static_cast< DDoubleGDL*>( p0);
	DDoubleGDL *res = c0->New( c0->Dim(), BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( OMPInt i=0; i<nEl; ++i)
	    (*res)[ i] = exp( (*c0)[ i]);
	}
	return res;
      }
    else if( t == GDL_FLOAT)
      {
	DFloatGDL *c0 = static_cast< DFloatGDL*>( p0);
	DFloatGDL *res = c0->New( c0->Dim(), BaseGDL::NOZERO);
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( OMPInt i=0; i<nEl; ++i)
	    (*res)[ i] = exp( (*c0)[ i]);
	}
	return res;
      }
    else if( t == GDL_PTR)
      throw GDLException( "Pointer not allowed in this context.");
    else if( t == GDL_OBJ)
      throw GDLException( "Object references not allowed in this context.");
    else if( t == GDL_STRUCT)
      throw GDLException( "Struct expression not allowed in this context.");
    else 
      {
	DFloatGDL *res = 
	  static_cast< DFloatGDL*>( p0->Convert2( GDL_FLOAT, BaseGDL::COPY));
	
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	{
#pragma omp for
	  for( OMPInt i=0; i<nEl; ++i)
	    (*res)[ i] = exp( (*res)[ i]);
	}	
	return res;
      }
  }

  // by medericboquien@users.sourceforge.net
  BaseGDL* gauss_pdf(EnvT* e)
  {
    SizeT nParam = e->NParam(1);
    DDoubleGDL* v = static_cast<DDoubleGDL*>(e->GetParDefined(0)->
					     Convert2(GDL_DOUBLE,BaseGDL::COPY));
    SizeT nv = v->N_Elements();

    for (int count = 0;count < nv;++count)
      (*v)[count] = gsl_cdf_ugaussian_P((*v)[count]);

    if (e->GetParDefined(0)->Type() == GDL_DOUBLE)
      return v;
    else
      return v->Convert2(GDL_FLOAT,BaseGDL::CONVERT);
    return new DByteGDL(0);
  }

  // by medericboquien@users.sourceforge.net
  BaseGDL* gauss_cvf(EnvT* e)
  {
    SizeT nParam = e->NParam(1);
    DDoubleGDL* p = static_cast<DDoubleGDL*>(e->GetParDefined(0)->
					     Convert2(GDL_DOUBLE,BaseGDL::COPY));
     
    if (p->N_Elements() != 1)
      e->Throw("Parameter must be scalar or one element array: "+
	       e->GetParString(0));
    if ((*p)[0] < 0. || (*p)[0] > 1.)
      e->Throw("Parameter must be in [0,1]: "+e->GetParString(0));

    (*p)[0] = gsl_cdf_ugaussian_Qinv((*p)[0]);

    if (e->GetParDefined(0)->Type() == GDL_DOUBLE)
      return p;
    else
      return p->Convert2(GDL_FLOAT,BaseGDL::CONVERT);
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
    
    if (e->GetParDefined(0)->Type() != GDL_DOUBLE && e->GetParDefined(0)->Type() != GDL_DOUBLE)
      return res->Convert2(GDL_FLOAT,BaseGDL::CONVERT);
    else
      return res;
    return new DByteGDL(0);
  }

  // by medericboquien@users.sourceforge.net
  BaseGDL* laguerre(EnvT* e)
  {
    SizeT nParam = e->NParam(2);

    DDoubleGDL* xvals = e->GetParAs<DDoubleGDL>(0);
    if(e->GetParDefined(0)->Type() == GDL_COMPLEX || e->GetParDefined(0)->Type() == GDL_COMPLEXDBL)
      e->Throw("Complex Laguerre not implemented: ");
    
    DIntGDL* nval = e->GetParAs<DIntGDL>(1);
    if (nval->N_Elements() != 1)
      e->Throw("N and K must be scalars.");
    if ((*nval)[0] < 0)
      e->Throw("Argument N must be greater than or equal to zero.");
    
    DDoubleGDL* kval;
    Guard<DDoubleGDL> kval_guard;
    if (nParam>2) {
      kval = e->GetParAs<DDoubleGDL>(2);
      if(kval->N_Elements() != 1)
        e->Throw("N and K must be scalars.");
      if ((*kval)[0] < 0.)
        e->Throw("Argument K must be greater than or equal to zero.");
    } else {
      kval = new DDoubleGDL(0);
      kval_guard.Reset(kval);
    }

    DDoubleGDL* res = new DDoubleGDL(xvals->Dim(),BaseGDL::NOZERO);
    DDouble k = (*kval)[0];
    DInt n = (*nval)[0];
    SizeT nEx = xvals->N_Elements();
    OMPInt count;
    
TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (nEx >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEx))
	{
#pragma omp for
    for (count = 0;count<nEx;++count)
      (*res)[count] = gsl_sf_laguerre_n(n,k,(*xvals)[count]);
	}
	
    static DInt doubleKWIx = e->KeywordIx("DOUBLE");
    static DInt coefKWIx = e->KeywordIx("COEFFICIENTS");   
    
    if(e->KeywordPresent(coefKWIx)) {
      double gamma_kn1 = gsl_sf_gamma(k+n+1.);
      DDoubleGDL* coefKW = new DDoubleGDL(dimension(n+1) , BaseGDL::NOZERO);

TRACEOMP( __FILE__, __LINE__)
#pragma omp parallel if (n >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= n))
	{
#pragma omp for
      for(count = 0;count<=n;++count) {
        double dcount = static_cast<double>(count);
        (*coefKW)[count] = ((count & 0x0001)?-1.0:1.0)*gamma_kn1/
          (gsl_sf_gamma(n-dcount+1.)*gsl_sf_gamma(k+dcount+1.)*
           gsl_sf_gamma(dcount+1.));
      }
      }
      if(e->GetParDefined(0)->Type() != GDL_DOUBLE && !e->KeywordSet(doubleKWIx))
        coefKW = static_cast<DDoubleGDL*>(coefKW->
					  Convert2(GDL_FLOAT,BaseGDL::CONVERT));
      e->SetKW(coefKWIx, coefKW);
    }

    //convert things back
    if(e->GetParDefined(0)->Type() != GDL_DOUBLE && !e->KeywordSet(doubleKWIx))
      return res->Convert2(GDL_FLOAT,BaseGDL::CONVERT);
    else
      return res;

  }

  // SA: based on equations 5-5 & 5-6 from Snyder (1987) USGS report no 1395 (page 31)
  //     available for download at: http://pubs.er.usgs.gov/djvu/PP/pp_1395.djvu
  template <typename T> inline void ll_arc_distance_helper(
							   T c, T Az, T phi1, T l0, T& phi, T& l, bool degrees) 
  {
    // temporary variables
    T pi = 4 * atan((T)1.), 
      dtor = degrees ? pi / 180. : 1,
      sin_c = sin(c), 
      cos_c = cos(c), 
      cos_Az = cos(Az * dtor), 
      sin_phi1 = sin(phi1 * dtor),
      cos_phi1 = cos(phi1 * dtor);
    // computing the results
    phi = asin(sin_phi1 * cos_c + cos_phi1 * sin_c * cos_Az) / dtor;
    l = l0 * dtor + atan2(
			  sin_c * sin(Az * dtor), (cos_phi1 * cos_c - sin_phi1 * sin_c * cos_Az)
			  ); 
    // placing the result in (-pi, pi)
    while (l < -pi) l += 2 * pi;
    while (l > pi) l -= 2 * pi;
    // converting to degrees if needed
    l /= dtor;                                      
  }
  BaseGDL* ll_arc_distance(EnvT* e)
  {
    // sanity check (for number of parameters)
    SizeT nParam = e->NParam();

    // 1-st argument : longitude/latitude values pair (in radians unless DEGREE kw. present)
    BaseGDL* p0 = e->GetNumericParDefined(0);

    // 2-nd argument : arc distance (in radians regardless of DEGREE kw. presence)
    BaseGDL* p1 = e->GetNumericParDefined(1);
    if (p1->N_Elements() != 1) 
      e->Throw("second argument is expected to be a scalar or 1-element array");

    // 3-rd argument : azimuth (in radians unless DEGREE kw. present)
    BaseGDL* p2 = e->GetNumericParDefined(2);
    if (p2->N_Elements() != 1) 
      e->Throw("third argument is expected to be a scalar or 1-element array");

    // chosing a type for the return value 
    bool args_complexdbl = 
      (p0->Type() == GDL_COMPLEXDBL || p1->Type() == GDL_COMPLEXDBL || p2->Type() == GDL_COMPLEXDBL);
    bool args_complex = args_complexdbl ? false : 
      (p0->Type() == GDL_COMPLEX || p1->Type() == GDL_COMPLEX || p2->Type() == GDL_COMPLEX);
    DType type = (
		  p0->Type() == GDL_DOUBLE || p1->Type() == GDL_DOUBLE || p2->Type() == GDL_DOUBLE || args_complexdbl
		  ) ? GDL_DOUBLE : GDL_FLOAT;

    // converting datatypes if neccesarry
    if (p0->Type() != type) p0 = p0->Convert2(type, BaseGDL::COPY);
    if (p1->Type() != type) p1 = p1->Convert2(type, BaseGDL::COPY);
    if (p2->Type() != type) p2 = p2->Convert2(type, BaseGDL::COPY); 
    
    // calculating (by calling a helper template function for float/double versions)
    BaseGDL* rt = p0->New(dimension(2, BaseGDL::NOZERO));
    if (type == GDL_FLOAT) 
      {
	ll_arc_distance_helper(
			       (*static_cast<DFloatGDL*>(p1))[0], 
			       (*static_cast<DFloatGDL*>(p2))[0], 
			       (*static_cast<DFloatGDL*>(p0))[1], 
			       (*static_cast<DFloatGDL*>(p0))[0], 
			       (*static_cast<DFloatGDL*>(rt))[1], 
			       (*static_cast<DFloatGDL*>(rt))[0],
			       e->KeywordSet("DEGREES")
			       );
      }
    else
      {
	ll_arc_distance_helper(
			       (*static_cast<DDoubleGDL*>(p1))[0], 
			       (*static_cast<DDoubleGDL*>(p2))[0], 
			       (*static_cast<DDoubleGDL*>(p0))[1], 
			       (*static_cast<DDoubleGDL*>(p0))[0], 
			       (*static_cast<DDoubleGDL*>(rt))[1], 
			       (*static_cast<DDoubleGDL*>(rt))[0],
			       e->KeywordSet("DEGREES")
			       );
      }

    // handling complex/dcomplex conversion
    return rt->Convert2(
			args_complexdbl ? GDL_COMPLEXDBL : args_complex ? GDL_COMPLEX : type,
			BaseGDL::CONVERT
			);
  }

  BaseGDL* crossp(EnvT* e)
  {
    BaseGDL* p0 = e->GetNumericParDefined(0);
    BaseGDL* p1 = e->GetNumericParDefined(1);
    if (p0->N_Elements() != 3 || p1->N_Elements() != 3)
      e->Throw("Both arguments must have 3 elements");

    BaseGDL *a, *b, *c;

    a = (DTypeOrder[p0->Type()] >= DTypeOrder[p1->Type()] ? p0 : p1)->New(dimension(3), BaseGDL::ZERO);
    // a = 0
    // .--mem: new a (with the type and shape of the result)
    b = p0->CShift(-1)->Convert2(a->Type(), BaseGDL::CONVERT);
    // | .--mem: new b
    a->Add(b);            // | | a = shift(p0, -1)
    delete b;             // | `--mem: del b
    b = p1->CShift(-2)->Convert2(a->Type(), BaseGDL::CONVERT);
    // | .--mem: new b
    a->Mult(b);           // | | a = shift(p0, -1) * shift(p1, -2)
    b->Sub(b);            // | | b = 0
    c = p0->CShift(1)->Convert2(a->Type(), BaseGDL::CONVERT);
    // | | .--mem: new c
    b->Sub(c);            // | | | b = - shift(p0, 1)
    delete c;             // | | `--mem: del c
    c = p1->CShift(2)->Convert2(a->Type(), BaseGDL::CONVERT); 
    // | | .--mem: new c
    b->Mult(c);           // | | | b = - shift(p0, 1) * shift(p1, 2)
    delete c;             // | | `--mem: del c
    a->Add(b);            // | | a = shift(p0, -1) * shift(p1, -2) - shift(p0, 1) * shift(p1, 2)
    delete b;             // | `--mem: del b
    return a;             // `--->
  }


  // SA: adapted from the GPL-licensed GNU plotutils (plotutils-2.5/ode/specfun.c)
  // -----------------------------------------------------------------------------
  template <typename T> 
  T inverf (T p)               /* Inverse Error Function */
  {
  /*
   * Source: This routine was derived (using f2c) from the Fortran
   * subroutine MERFI found in ACM Algorithm 602, obtained from netlib.
   *
   * MDNRIS code is copyright 1978 by IMSL, Inc.  Since MERFI has been
   * submitted to netlib, it may be used with the restrictions that it may
   * only be used for noncommercial purposes, and that IMSL be acknowledged
   * as the copyright-holder of the code.
   */

  /* Initialized data */
  static T a1 = -.5751703,   a2 = -1.896513,   a3 = -.05496261,
           b0 = -.113773,    b1 = -3.293474,   b2 = -2.374996,  b3 = -1.187515,
           c0 = -.1146666,   c1 = -.1314774,   c2 = -.2368201,  c3 = .05073975,
           d0 = -44.27977,   d1 = 21.98546,    d2 = -7.586103, 
           e0 = -.05668422,  e1 = .3937021,    e2 = -.3166501,  e3 = .06208963,
           f0 = -6.266786,   f1 = 4.666263,    f2 = -2.962883,
           g0 = 1.851159e-4, g1 = -.002028152, g2 = -.1498384,  g3 = .01078639,
           h0 = .09952975,   h1 = .5211733,    h2 = -.06888301;

    /* Local variables */
    static T a, b, f, w, x, y, z, sigma, z2, sd, wi, sn;

    x = p;

    /* determine sign of x */
    sigma = (x > 0 ? 1.0 : -1.0);

    /* Note: -1.0 < x < 1.0 */

    z = abs(x);

    /* z between 0.0 and 0.85, approx. f by a
       rational function in z  */

    if (z <= 0.85)
    {
      z2 = z * z;
      f = z + z * (b0 + a1 * z2 / (b1 + z2 + a2 / (b2 + z2 + a3 / (b3 + z2))));
    }
    else  /* z greater than 0.85 */
    {
      a = 1.0 - z;
      b = z;

      /* reduced argument is in (0.85,1.0), obtain the transformed variable */

      w = sqrt(-(T)log(a + a * b));

      if (w >= 4.0)
      /* w greater than 4.0, approx. f by a rational function in 1.0 / w */
      {
        wi = 1.0 / w;
        sn = ((g3 * wi + g2) * wi + g1) * wi;
        sd = ((wi + h2) * wi + h1) * wi + h0;
        f = w + w * (g0 + sn / sd);
      }
      else if (w < 4.0 && w > 2.5)
      /* w between 2.5 and 4.0, approx.  f by a rational function in w */
      {
        sn = ((e3 * w + e2) * w + e1) * w;
        sd = ((w + f2) * w + f1) * w + f0;
        f = w + w * (e0 + sn / sd);

        /* w between 1.13222 and 2.5, approx. f by
           a rational function in w */
      }
      else if (w <= 2.5 && w > 1.13222)
      {
        sn = ((c3 * w + c2) * w + c1) * w;
        sd = ((w + d2) * w + d1) * w + d0;
        f = w + w * (c0 + sn / sd);
      }
    }
    y = sigma * f;

    return y;
  } 
  // -----------------------------------------------------------------------------

  BaseGDL* gdl_erfinv_fun(EnvT* e)
  {
    BaseGDL* p0 = e->GetNumericParDefined(0);    
    SizeT n = p0->N_Elements();
    static int doubleIx = e->KeywordIx("DOUBLE");
    if (e->KeywordSet(doubleIx) || p0->Type() == GDL_DOUBLE)
    {
      DDoubleGDL *ret = new DDoubleGDL(dimension(n)), *p0d = e->GetParAs<DDoubleGDL>(0);
      while (n != 0) --n, (*ret)[n] = inverf((*p0d)[n]);
      return ret;
    }
    else
    {
      DFloatGDL *ret = new DFloatGDL(dimension(n)), *p0f = e->GetParAs<DFloatGDL>(0);
      while (n != 0) --n, (*ret)[n] = inverf((*p0f)[n]); 
      return ret;
    }
  }

} // namespace
