/***************************************************************************
                          matrix_invert.cpp 
                          -------------------
    begin                : May 17 2013
    copyright            : (C) 2004 by Joel Gales
    email                : jomoga@users.sourceforge.net
 ***************************************************************************/

/* history:

At the beginning, this code was only available using the GSL library.
At that time, it was included in the "gsl_fun.hpp/cpp" code

In 2013 we start to use Eigen3 in conjonction with the GSL.
The GSL is mandatory. Eigen3 is not mandatory.

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

//#ifdef HAVE_CONFIG_H
//#include <config.h>
//#endif

// current versions are based on Eigen3
// #if defined(HAVE_LIBGSL) && defined(HAVE_LIBGSLCBLAS)

#include "includefirst.hpp"

#include <map>
#include <cmath>
#include <stdio.h>
#include <iostream>
//#include <fstream>
#include <string>
#include <complex>


#include "datatypes.hpp"
#include "envt.hpp"
#include "basic_fun.hpp"
#include "gsl_fun.hpp"
#include "dinterpreter.hpp"

#include <gsl/gsl_sys.h>
#include <gsl/gsl_linalg.h>

#include "matrix_invert.hpp"
//#include "gsl_errorhandler.hpp"

#define LOG10E 0.434294

#if defined(USE_EIGEN)
#include <Eigen/LU>
#include <Eigen/Eigenvalues>
#include <Eigen/Core>
#endif

namespace lib {

#if defined(USE_EIGEN)
  using namespace Eigen;
#endif

  using namespace std;
  //void SetGDLGenericGSLErrorHandler();

  const int szdbl=sizeof(double);
  const int szflt=sizeof(float);

  BaseGDL* AC_invert_fun( EnvT* e)
  {
    if (e->KeywordSet("GSL") && e->KeywordSet("EIGEN"))
      e->Throw("Conflicting keywords");

    bool Eigen_flag=FALSE;
#if defined(USE_EIGEN)
    Eigen_flag=TRUE;
#endif

    if (e->KeywordSet("EIGEN") && (!Eigen_flag))
      Warning("Eigen Invert not available, GSL used");
    
    if (e->KeywordSet("GSL") ||(!Eigen_flag)) 
      {
	return invert_gsl_fun(e);
      }
    else
      {      
	//	return invert_eigen_fun(e);
	//
	// AC 2014-08-10 : during tests of Chianti Code,
	// we discovered that the GSL code was less sensitive
	// to very high range in matrix ... 
	// If status used, if Eigen fails, we try GSL

	BaseGDL* tmp;
	tmp=invert_eigen_fun(e);	
	SizeT nParam=e->NParam(1);
	if (nParam == 2) 
	  {
	    BaseGDL* p1 = e->GetParDefined(1);
	    DLongGDL* res = static_cast<DLongGDL*>
	      (p1->Convert2(GDL_LONG, BaseGDL::COPY));
	    DLong status;
	    status=(*res)[0];
	    if (status > 0) tmp=invert_gsl_fun(e);
	  }
	return tmp;
      }
  }

  BaseGDL* invert_gsl_fun( EnvT* e)
  {
    SizeT nParam=e->NParam(1);
    int s;
    float f32;
    double f64;
    double det;
    long singular=0;

    //     if( nParam == 0)
    //       e->Throw( "Incorrect number of arguments.");

    BaseGDL* p0 = e->GetParDefined( 0);

    SizeT nEl = p0->N_Elements();

    if( nEl == 0)
      e->Throw( "Variable is undefined: " + e->GetParString(0));
    
    if (p0->Rank() > 2)
      e->Throw( "Input must be a square matrix:" + e->GetParString(0));
    
    if (p0->Rank() > 1) {
      if (p0->Dim(0) != p0->Dim(1))
        e->Throw( "Input must be a square matrix:" + e->GetParString(0));
    }

    // status 
    // check here, if not done, res would be pending in case of SetPar() throws
    // SetPar() only throws in AssureGlobalPar()
    if (nParam == 2) e->AssureGlobalPar( 1);

    // only one element matrix

    if( nEl == 1) {
      if( p0->Type() == GDL_COMPLEXDBL) {
	DComplexDblGDL* res = static_cast<DComplexDblGDL*>
	  (p0->Convert2(GDL_COMPLEXDBL, BaseGDL::COPY));
	double a, b, deno;
	a=real((*res)[0]);
	b=imag((*res)[0]);
	deno=a*a+b*b;
	if (deno == 0.0) {
	  singular=1;
	  (*res)[0]= DComplexDbl(0., 0.);
	} else {
	  (*res)[0]= DComplexDbl(a/deno, -b/deno);
	}
	if (nParam == 2) e->SetPar(1,new DLongGDL( singular)); 
	return res;
      }
      if( p0->Type() == GDL_COMPLEX) {
	DComplexGDL* res = static_cast<DComplexGDL*>
	  (p0->Convert2(GDL_COMPLEX, BaseGDL::COPY));
	float a, b, deno;
	a=real((*res)[0]);
	b=imag((*res)[0]);
	deno=a*a+b*b;
	if (deno == 0.0) {
	  singular=1;
	  (*res)[0]= DComplex(0., 0.);
	} else {
	  (*res)[0]= DComplex(a/deno, -b/deno);
	}
	if (nParam == 2) e->SetPar(1,new DLongGDL( singular)); 
	return res;
      }
      if(( p0->Type() == GDL_DOUBLE) || e->KeywordSet("DOUBLE")) {
	DDoubleGDL* res = static_cast<DDoubleGDL*>
	  (p0->Convert2(GDL_DOUBLE, BaseGDL::COPY));
	if ((*res)[0] == 0.0) {
	  singular=1;
	} else {
	  double unity=1.0 ;
	  (*res)[0]= unity / ((*res)[0]);
	}
	if (nParam == 2) e->SetPar(1,new DLongGDL( singular)); 
	return res;
      }

      // all other cases (including GDL_STRING, Float, Int, ... )
      //      if( p0->Type() == GDL_STRING) {
      DFloatGDL* res = static_cast<DFloatGDL*>
	(p0->Convert2( GDL_FLOAT, BaseGDL::COPY));
      if ((*res)[0] == 0.0) {
	singular=1;
      } else {
	(*res)[0]= 1.0 / ((*res)[0]);
      }
      if (nParam == 2) e->SetPar(1,new DLongGDL( singular)); 
      return res;
    }
    
    // more than one element matrix

    // GSL error handling
    //    SetTemporaryGSLErrorHandlerT lib::setTemporaryGSLErrorHandler( GDLGenericGSLErrorHandler);

    if( p0->Type() == GDL_COMPLEX)
      {
	DComplexGDL* p0C = static_cast<DComplexGDL*>( p0);
	DComplexGDL* res = new DComplexGDL( p0C->Dim(), BaseGDL::NOZERO);
	Guard<DComplexGDL> resGuard( res);

	float f32_2[2];
	double f64_2[2];

	gsl_matrix_complex *mat = 
	  gsl_matrix_complex_alloc(p0->Dim(0), p0->Dim(1));
	GDLGuard<gsl_matrix_complex> g1( mat, gsl_matrix_complex_free);
	gsl_matrix_complex *inverse = 
	  gsl_matrix_complex_calloc(p0->Dim(0), p0->Dim(1));
	GDLGuard<gsl_matrix_complex> g2( inverse, gsl_matrix_complex_free);
	gsl_permutation *perm = gsl_permutation_alloc(p0->Dim(0));
	GDLGuard<gsl_permutation> g3( perm, gsl_permutation_free);

	for( SizeT i=0; i<nEl; ++i) {
	  memcpy(f32_2, &(*p0C)[i], szdbl);
	  f64 = (double) f32_2[0];
	  memcpy(&mat->data[2*i], &f64, szdbl);

	  f64 = (double) f32_2[1];
	  memcpy(&mat->data[2*i+1], &f64, szdbl);
	}
 
	gsl_linalg_complex_LU_decomp (mat, perm, &s);
       	det = gsl_linalg_complex_LU_lndet(mat);
	if (gsl_isinf(det) == 0) {
	  gsl_linalg_complex_LU_invert (mat, perm, inverse);
	  if (abs(det) * LOG10E < 1e-5) singular = 2;
	}
	else singular = 1;

	for( SizeT i=0; i<nEl; ++i) {
	  memcpy(&f64_2[0], &inverse->data[2*i], szdbl*2);
	  f32_2[0] = (float) f64_2[0];
	  f32_2[1] = (float) f64_2[1];

	  memcpy(&(*res)[i], &f32_2[0], szflt*2);
	}

	// 	gsl_permutation_free(perm);
	// 	gsl_matrix_complex_free(mat);
	// 	gsl_matrix_complex_free(inverse);

	if (nParam == 2) e->SetPar(1,new DLongGDL( singular)); 
	resGuard.release();
	return res;
      }
    else if( p0->Type() == GDL_COMPLEXDBL)
      {
	DComplexDblGDL* p0C = static_cast<DComplexDblGDL*>( p0);
	DComplexDblGDL* res = new DComplexDblGDL( p0C->Dim(), BaseGDL::NOZERO);
	Guard<DComplexDblGDL> resGuard( res);

	gsl_matrix_complex *mat = 
	  gsl_matrix_complex_alloc(p0->Dim(0), p0->Dim(1));
	GDLGuard<gsl_matrix_complex> g1( mat, gsl_matrix_complex_free);
	gsl_matrix_complex *inverse = 
	  gsl_matrix_complex_calloc(p0->Dim(0), p0->Dim(1));
	GDLGuard<gsl_matrix_complex> g2( inverse, gsl_matrix_complex_free);
	gsl_permutation *perm = gsl_permutation_alloc(p0->Dim(0));
	GDLGuard<gsl_permutation> g3( perm, gsl_permutation_free);

	memcpy(mat->data, &(*p0C)[0], nEl*szdbl*2);

	gsl_linalg_complex_LU_decomp (mat, perm, &s);
	det = gsl_linalg_complex_LU_lndet(mat);
	if (gsl_isinf(det) == 0) {
	  gsl_linalg_complex_LU_invert (mat, perm, inverse);
	  if (abs(det) * LOG10E < 1e-5) singular = 2;
	}
	else singular = 1;

	memcpy(&(*res)[0], inverse->data, nEl*szdbl*2);

	// 	gsl_permutation_free(perm);
	// 	gsl_matrix_complex_free(mat);
	// 	gsl_matrix_complex_free(inverse);

	if (nParam == 2) e->SetPar(1,new DLongGDL( singular)); 
	resGuard.release();
	return res;
      }
    else if (( p0->Type() == GDL_DOUBLE) ||  e->KeywordSet("DOUBLE"))
      {

	DDoubleGDL* p0D = static_cast<DDoubleGDL*>
	  (p0->Convert2(GDL_DOUBLE,BaseGDL::COPY));


	//	DDoubleGDL* p0D = static_cast<DDoubleGDL*>( p0);
	DDoubleGDL* res = new DDoubleGDL( p0->Dim(), BaseGDL::NOZERO);
	Guard<DDoubleGDL> resGuard( res);

	gsl_matrix *mat = gsl_matrix_alloc(p0->Dim(0), p0->Dim(1));
	GDLGuard<gsl_matrix> g1( mat, gsl_matrix_free);
	gsl_matrix *inverse = gsl_matrix_calloc(p0->Dim(0), p0->Dim(1));
	GDLGuard<gsl_matrix> g2( inverse, gsl_matrix_free);
	gsl_permutation *perm = gsl_permutation_alloc(p0->Dim(0));
	GDLGuard<gsl_permutation> g3( perm, gsl_permutation_free);

	memcpy(mat->data, &(*p0D)[0], nEl*szdbl);

	gsl_linalg_LU_decomp (mat, perm, &s);
	det = gsl_linalg_LU_lndet(mat);
	if (gsl_isinf(det) == 0) {
	  gsl_linalg_LU_invert (mat, perm, inverse);
	  if (abs(det) * LOG10E < 1e-5) singular = 2;
	}
	else singular = 1;

	memcpy(&(*res)[0], inverse->data, nEl*szdbl);

	// 	gsl_permutation_free(perm);
	// 	gsl_matrix_free(mat);
	// 	gsl_matrix_free(inverse);

	if (nParam == 2) e->SetPar(1,new DLongGDL( singular)); 
	resGuard.release();
	return res;
      }
    else if( p0->Type() == GDL_FLOAT ||
	     p0->Type() == GDL_LONG ||
	     p0->Type() == GDL_ULONG ||
	     p0->Type() == GDL_INT ||
	     p0->Type() == GDL_STRING ||
	     p0->Type() == GDL_UINT ||
	     p0->Type() == GDL_BYTE)
      {
	DFloatGDL* p0F = static_cast<DFloatGDL*>( p0);
	DLongGDL* p0L = static_cast<DLongGDL*>( p0);
	DULongGDL* p0UL = static_cast<DULongGDL*>( p0);
	DIntGDL* p0I = static_cast<DIntGDL*>( p0);
	//	DStringGDL* p0S = static_cast<DStringGDL*>( p0);
	DUIntGDL* p0UI = static_cast<DUIntGDL*>( p0);
	DByteGDL* p0B = static_cast<DByteGDL*>( p0);

	//	if (p0->Type() == STRING) {
	DFloatGDL* p0SS = static_cast<DFloatGDL*>
	  (p0->Convert2( GDL_FLOAT, BaseGDL::COPY));
	Guard<DFloatGDL> p0SSGuard( p0SS);
	//}

	DFloatGDL* res = new DFloatGDL( p0->Dim(), BaseGDL::NOZERO);
	Guard<DFloatGDL> resGuard( res);

	gsl_matrix *mat = gsl_matrix_alloc(p0->Dim(0), p0->Dim(1));
	GDLGuard<gsl_matrix> g1( mat, gsl_matrix_free);
	gsl_matrix *inverse = gsl_matrix_calloc(p0->Dim(0), p0->Dim(1));
	GDLGuard<gsl_matrix> g2( inverse, gsl_matrix_free);
	gsl_permutation *perm = gsl_permutation_alloc(p0->Dim(0));
	GDLGuard<gsl_permutation> g3( perm, gsl_permutation_free);

	for( SizeT i=0; i<nEl; ++i) {
	  switch ( p0->Type()) {
	  case GDL_FLOAT: f64 = (double) (*p0F)[i]; break;
	  case GDL_LONG:  f64 = (double) (*p0L)[i]; break;
	  case GDL_ULONG: f64 = (double) (*p0UL)[i]; break;
	  case GDL_INT:   f64 = (double) (*p0I)[i]; break;
	  case GDL_STRING:f64 = (double) (*p0SS)[i]; break;
	  case GDL_UINT:  f64 = (double) (*p0UI)[i]; break;
	  case GDL_BYTE:  f64 = (double) (*p0B)[i]; break;
	  }
	  memcpy(&mat->data[i], &f64, szdbl);
	}

	gsl_linalg_LU_decomp (mat, perm, &s);
	det = gsl_linalg_LU_lndet(mat);
	if (gsl_isinf(det) == 0) {
	  gsl_linalg_LU_invert (mat, perm, inverse);
	  if (abs(det) * LOG10E < 1e-5) singular = 2;
	}
	else singular = 1;

	for( SizeT i=0; i<nEl; ++i) {
	  f32 = (float) inverse->data[i];
	  memcpy(&(*res)[i], &f32, 4);
	}

	// 	gsl_permutation_free(perm);
	// 	gsl_matrix_free(mat);
	// 	gsl_matrix_free(inverse);

	if (nParam == 2) e->SetPar(1,new DLongGDL( singular)); 
	resGuard.release();
	return res;
      }
    else 
      {
	cout << "Should never reach this point ! Please report it !" << endl; 
	DFloatGDL* res = static_cast<DFloatGDL*>
	  (p0->Convert2( GDL_FLOAT, BaseGDL::COPY));

	if (nParam == 2) e->SetPar(1,new DLongGDL( singular)); 
	return res;
      }
  }


#if defined(USE_EIGEN)
  BaseGDL* invert_eigen_fun( EnvT* e)
  {
    //obsolete
    //#if defined _OPENMP 
    //set_num_threads();
    //#endif
    
    BaseGDL* p0 = e->GetParDefined( 0);
    SizeT nParam=e->NParam(1);
    long singular=0;
    SizeT nEl = p0->N_Elements();

    if( nEl == 0)
      e->Throw( "Variable is undefined: " + e->GetParString(0));
    
    if (p0->Rank() > 2)
      e->Throw( "Input must be a square matrix:" + e->GetParString(0));
    
    if (p0->Rank() > 1) {
      if (p0->Dim(0) != p0->Dim(1))
        e->Throw( "Input must be a square matrix:" + e->GetParString(0));
    }

    if (p0->Rank() == 0) 
        e->Throw( "Expression must be an array in this context:" + e->GetParString(0));

    if (nParam == 2) e->AssureGlobalPar( 1);

    // only one element matrix
    if( nEl == 1) {
      if( p0->Type() == GDL_COMPLEXDBL) {
	DComplexDblGDL* res = static_cast<DComplexDblGDL*>
	  (p0->Convert2(GDL_COMPLEXDBL, BaseGDL::COPY));
	double a, b, deno;
	a=real((*res)[0]);
	b=imag((*res)[0]);
	deno=a*a+b*b;
	if (deno == 0.0) {
	  singular=1;
	  (*res)[0]= DComplexDbl(0., 0.);
	} else {
	  (*res)[0]= DComplexDbl(a/deno, -b/deno);
	}
	if (nParam == 2) e->SetPar(1,new DLongGDL( singular)); 
	return res;
      }

      if( p0->Type() == GDL_COMPLEX) {
	DComplexGDL* res = static_cast<DComplexGDL*>
	  (p0->Convert2(GDL_COMPLEX, BaseGDL::COPY));
	float a, b, deno;
	a=real((*res)[0]);
	b=imag((*res)[0]);
	deno=a*a+b*b;
	if (deno == 0.0) {
	  singular=1;
	  (*res)[0]= DComplex(0., 0.);
	} else {
	  (*res)[0]= DComplex(a/deno, -b/deno);
	}
	if (nParam == 2) e->SetPar(1,new DLongGDL( singular)); 
	return res;
      }


      if(( p0->Type() == GDL_DOUBLE) || e->KeywordSet("DOUBLE")) {
	DDoubleGDL* res = static_cast<DDoubleGDL*>
	  (p0->Convert2(GDL_DOUBLE, BaseGDL::COPY));
	if ((*res)[0] == 0.0) {
	  singular=1;
	} else {
	  double unity=1.0 ;
	  (*res)[0]= unity / ((*res)[0]);
	}
	if (nParam == 2) e->SetPar(1,new DLongGDL( singular)); 
	return res;
      }
      // all other cases (including GDL_STRING, Float, Int, ... )
      //      if( p0->Type() == GDL_STRING) {
      DFloatGDL* res = static_cast<DFloatGDL*>
	(p0->Convert2( GDL_FLOAT, BaseGDL::COPY));
      if ((*res)[0] == 0.0) {
	singular=1;
      } else {
	(*res)[0]= 1.0 / ((*res)[0]);
      }
      if (nParam == 2) e->SetPar(1,new DLongGDL( singular)); 
      return res;
   }


    // more than one element matrix

    long NbCol, NbRow;

    if( p0->Type() == GDL_COMPLEX) {
      DComplexGDL* p0C = static_cast<DComplexGDL*>
	(p0->Convert2(GDL_COMPLEX , BaseGDL::COPY));  
      NbCol=p0->Dim(0);
      NbRow=p0->Dim(1);
      MatrixXcf tmp_res (NbCol,NbRow);
      Map<Matrix<complex<float>,Dynamic,Dynamic> > m0(&(*p0C)[0], NbCol,NbRow);

      FullPivLU<MatrixXcf> lu(m0);

      if(lu.isInvertible()) {
	tmp_res=lu.inverse();
	if (abs(lu.determinant().real())* LOG10E < 1e-5) singular = 2;
	}
	else singular=1;

      DComplexGDL* res = new DComplexGDL(p0->Dim(), BaseGDL::NOZERO);
      Map<MatrixXcf>(&(*res)[0], NbCol, NbRow) = tmp_res.cast<complex<float> >();

      if (nParam == 2) e->SetPar(1,new DLongGDL( singular));
      return res;
    }

    else if( p0->Type() == GDL_COMPLEXDBL) {

      DComplexDblGDL* p0C = static_cast<DComplexDblGDL*>
	(p0->Convert2(GDL_COMPLEXDBL , BaseGDL::COPY));
      NbCol=p0->Dim(0);
      NbRow=p0->Dim(1);
      MatrixXcd tmp_res (NbCol,NbRow);
      Map<Matrix<complex<double>,Dynamic,Dynamic> > m0(&(*p0C)[0], NbCol,NbRow);

      FullPivLU<MatrixXcd> lu(m0);

      if(lu.isInvertible()) {
	tmp_res=lu.inverse();
	if (abs(lu.determinant().real())* LOG10E < 1e-5) singular = 2;
     }
      else singular=1;

      DComplexDblGDL* res = new DComplexDblGDL(p0->Dim(), BaseGDL::NOZERO);
      Map<MatrixXcd>(&(*res)[0], NbCol, NbRow) = tmp_res.cast<complex<double> >();

      if (nParam == 2) e->SetPar(1,new DLongGDL( singular));
      return res;
    }

    else if(( p0->Type() == GDL_DOUBLE) || e->KeywordSet("DOUBLE")) {
      DDoubleGDL* p0D = static_cast<DDoubleGDL*>
	(p0->Convert2( GDL_DOUBLE, BaseGDL::COPY));
      NbCol=p0->Dim(0);
      NbRow=p0->Dim(1);
      MatrixXd tmp_res (NbCol,NbRow);
      Map<Matrix<double,Dynamic,Dynamic> > m0(&(*p0D)[0], NbCol,NbRow);

      Eigen::FullPivLU<MatrixXd> lu(m0);
      //      cout << lu.determinant() << endl;
      //cout << lu.isInvertible() << endl;
    
      if(lu.isInvertible()) {
	tmp_res=lu.inverse();
	if (abs(lu.determinant()) * LOG10E < 1e-5) singular = 2;
      }
      else singular=1;

      DDoubleGDL* res = new DDoubleGDL(p0->Dim(), BaseGDL::NOZERO);
      Map<MatrixXd>(&(*res)[0], NbCol, NbRow) = tmp_res.cast<double>();

      if (nParam == 2) e->SetPar(1,new DLongGDL( singular));
      return res;
    }


    // all other cases (including GDL_STRING, Float, Int, ... )
    //      if( p0->Type() == GDL_STRING) {

    else if( p0->Type() == GDL_FLOAT   ||
	     p0->Type() == GDL_LONG    ||
	     p0->Type() == GDL_LONG64  ||
	     p0->Type() == GDL_ULONG64 ||
	     p0->Type() == GDL_ULONG   ||
	     p0->Type() == GDL_INT     ||
	     p0->Type() == GDL_STRING  ||
	     p0->Type() == GDL_UINT    ||
	     p0->Type() == GDL_BYTE)
      {
	DFloatGDL* p0SS = static_cast<DFloatGDL*>
	  (p0->Convert2( GDL_FLOAT, BaseGDL::COPY));
        NbCol=p0->Dim(0);
	NbRow=p0->Dim(1);
	MatrixXf tmp_res(NbCol,NbRow);
	Map<Matrix<float,Dynamic,Dynamic> > m0(&(*p0SS)[0], NbCol,NbRow);
	
	FullPivLU<MatrixXf> lu(m0);
	
	if(lu.isInvertible()) {
	  tmp_res=lu.inverse();
	  if (abs(lu.determinant()) * LOG10E < 1e-5) singular = 2;
	}
	else singular=1;

	DFloatGDL* res =new DFloatGDL(p0->Dim(), BaseGDL::NOZERO);
	Map<MatrixXf>(&(*res)[0], NbCol, NbRow) = tmp_res.cast<float>();

	if (nParam == 2) e->SetPar(1,new DLongGDL( singular));
	return res;
      }
    else 
      {
	cout << "Should never reach this point ! Please report it !" << endl; 
	DFloatGDL* res = static_cast<DFloatGDL*>
	  (p0->Convert2( GDL_FLOAT, BaseGDL::COPY));

	if (nParam == 2) e->SetPar(1,new DLongGDL( singular)); 
	return res;
      }
  }
#else
  BaseGDL* invert_eigen_fun( EnvT* e){
    e->Throw( "sorry, INVERT with Eigen not available. GDL must be compiled with Eigen lib.");
     return NULL;
  }
#endif

}

