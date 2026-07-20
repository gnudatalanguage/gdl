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
The GSL is mandatory. Eigen3 is not mandatory. */

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


#include "envt.hpp"
#include "basic_fun.hpp"
#include "gsl_fun.hpp"

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

  void matrix_input_check_dims( EnvT* e)
  {
    BaseGDL* p0 = e->GetNumericParDefined( 0);

    //cout << p0->Rank() << endl;

    SizeT nEl = p0->N_Elements();

    if( nEl == 0)
      e->Throw( "Variable is undefined: " + e->GetParString(0));
    
    if (p0->Rank() > 2)
      e->Throw( "Input must be a square matrix: " + e->GetParString(0));
    
    if (p0->Rank() == 2) {
      if (p0->Dim(0) != p0->Dim(1))
        e->Throw( "Input must be a square matrix: " + e->GetParString(0));
    }

    // array singleton OK (e.g. [1]), vector not
    if ((p0->Rank() == 1) && (nEl > 1)) {
        e->Throw( "Input must be a square matrix: " + e->GetParString(0));
    }

    if (p0->Rank() == 0)
      e->Throw( "Expression must be an array in this context: " + e->GetParString(0));
    
  }

  BaseGDL* AC_invert_fun( EnvT* e)
  {
    static int GSLIx=e->KeywordIx("GSL");
    static int EIGENIx=e->KeywordIx("EIGEN");
    if (e->KeywordSet(GSLIx) && e->KeywordSet(EIGENIx))
      e->Throw("Conflicting keywords");
    
    static int DOUBLEIx=e->KeywordIx("DOUBLE");
    bool hasDouble=e->KeywordSet(DOUBLEIx);
    
    matrix_input_check_dims(e);
    
    BaseGDL* p0 = e->GetParDefined( 0);
    SizeT nEl = p0->N_Elements();

    // only one element matrix: no need of eigen or gsl.
    if (nEl == 1) {
      SizeT nParam = e->NParam();
      if (nParam == 2) e->AssureGlobalPar(1);
      DLong singular=0;
      if (p0->Type() == GDL_COMPLEXDBL) {
        DComplexDblGDL* res = static_cast<DComplexDblGDL*>
            (p0->Convert2(GDL_COMPLEXDBL, BaseGDL::COPY));
        double a, b, deno;
        a = real((*res)[0]);
        b = imag((*res)[0]);
        deno = a * a + b*b;
        if (deno == 0.0) {
          singular = 1;
          (*res)[0] = DComplexDbl(0., 0.);
        } else {
          (*res)[0] = DComplexDbl(a / deno, -b / deno);
        }
        if (nParam == 2) e->SetPar(1, new DLongGDL(singular));
        return res;
      }

      if (p0->Type() == GDL_COMPLEX) {
        DComplexGDL* res = static_cast<DComplexGDL*>
            (p0->Convert2(GDL_COMPLEX, BaseGDL::COPY));
        float a, b, deno;
        a = real((*res)[0]);
        b = imag((*res)[0]);
        deno = a * a + b*b;
        if (deno == 0.0) {
          singular = 1;
          (*res)[0] = DComplex(0., 0.);
        } else {
          (*res)[0] = DComplex(a / deno, -b / deno);
        }
        if (nParam == 2) e->SetPar(1, new DLongGDL(singular));
        return res;
      }


      if ((p0->Type() == GDL_DOUBLE) || hasDouble) {
        DDoubleGDL* res = static_cast<DDoubleGDL*>
            (p0->Convert2(GDL_DOUBLE, BaseGDL::COPY));
        if ((*res)[0] == 0.0) {
          singular = 1;
        } else {
          double unity = 1.0;
          (*res)[0] = unity / ((*res)[0]);
        }
        if (nParam == 2) e->SetPar(1, new DLongGDL(singular));
        return res;
      }
      // all other cases (including GDL_STRING, Float, Int, ... )
      //      if( p0->Type() == GDL_STRING) {
      DFloatGDL* res = static_cast<DFloatGDL*>
          (p0->Convert2(GDL_FLOAT, BaseGDL::COPY));
      if ((*res)[0] == 0.0) {
        singular = 1;
      } else {
        (*res)[0] = 1.0 / ((*res)[0]);
      }
      if (nParam == 2) e->SetPar(1, new DLongGDL(singular));
      return res;
    }


#if defined(USE_EIGEN)
    bool Eigen_flag=TRUE;
#else
    bool Eigen_flag=FALSE;
#endif

    if (e->KeywordSet(EIGENIx) && (!Eigen_flag))
      Warning("Eigen Invert not available, GSL used");
    
    if (e->KeywordSet(GSLIx) || (!Eigen_flag))
      {
	return invert_gsl_fun(e, hasDouble);
      }
    else
      {
	// if /Eigen, we want to use Eigen,
	// then we don't check the status and return ...

	if (e->KeywordSet(EIGENIx)) return invert_eigen_fun(e, hasDouble);

	// AC 2014-08-10 : during tests of Chianti Code,
	// we discovered that the GSL code was less sensitive
	// to very high range in matrix ... 
	// If status used, if Eigen fails, we try GSL

	BaseGDL* tmp;
	tmp=invert_eigen_fun(e, hasDouble);	
	SizeT nParam=e->NParam(1);
	if (nParam == 2) 
	  {
	    BaseGDL* p1 = e->GetParDefined(1);
	    DLongGDL* res = static_cast<DLongGDL*>
	      (p1->Convert2(GDL_LONG, BaseGDL::COPY));
	    DLong status;
	    status=(*res)[0];
	    if (status > 0) tmp=invert_gsl_fun(e, hasDouble);
	  }
	return tmp;
      }
  }

  BaseGDL* invert_gsl_fun( EnvT* e, bool hasDouble)
  {

    BaseGDL* p0 = e->GetParDefined( 0);
    SizeT nEl = p0->N_Elements();

    // related to "status" param
    // check here, if not done, res would be pending in case of SetPar() throws
    // SetPar() only throws in AssureGlobalPar()
    SizeT nParam=e->NParam(1);
    if (nParam == 2) e->AssureGlobalPar( 1);

    long singular=0;

    int s;
    float f32;
    double f64;
    double det;

    // matrix with ONE element already treated in AC_invert_fun
    
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

// attention: « void* memcpy(void*, const void*, size_t) » copie un objet du type non trivial « Data_<SpDComplex>::Ty » {aka « struct std::complex<float> »} depuis un tableau de « float » [-Wclass-memaccess]
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
//attention: « void* memcpy(void*, const void*, size_t) » copie un objet du type non trivial « Data_<SpDComplexDbl>::Ty » {aka « struct std::complex<double> »} depuis un tableau de « double » [-Wclass-memaccess]
	memcpy(&(*res)[0], inverse->data, nEl*szdbl*2);

	// 	gsl_permutation_free(perm);
	// 	gsl_matrix_complex_free(mat);
	// 	gsl_matrix_complex_free(inverse);

	if (nParam == 2) e->SetPar(1,new DLongGDL( singular)); 
	resGuard.release();
	return res;
      }
    else if ( p0->Type() == GDL_DOUBLE)
      {
      DDoubleGDL* p0D = static_cast<DDoubleGDL*> (p0); //already double: no expensive copy.
	DDoubleGDL* res = new DDoubleGDL( p0->Dim(), BaseGDL::NOZERO);
	Guard<DDoubleGDL> resGuard( res);

	gsl_matrix *mat = gsl_matrix_alloc(p0->Dim(0), p0->Dim(1)); //we could use p0D data instead of this costly allocation. Just fill correct values in mat struct.
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
	     p0->Type() == GDL_LONG64 ||
	     p0->Type() == GDL_STRING ||
	     p0->Type() == GDL_UINT ||
	     p0->Type() == GDL_ULONG64 ||
	     p0->Type() == GDL_BYTE)
      {
	DFloatGDL* p0F = static_cast<DFloatGDL*>( p0);
	DLongGDL* p0L = static_cast<DLongGDL*>( p0);
	DLong64GDL* p0L64 = static_cast<DLong64GDL*>( p0);
	DULongGDL* p0UL = static_cast<DULongGDL*>( p0);
	DULong64GDL* p0UL64 = static_cast<DULong64GDL*>( p0);
	DIntGDL* p0I = static_cast<DIntGDL*>( p0);
	DUIntGDL* p0UI = static_cast<DUIntGDL*>( p0);
	DByteGDL* p0B = static_cast<DByteGDL*>( p0);
    DFloatGDL* p0SS;
	if (p0->Type() == GDL_STRING) {
	  p0SS = static_cast<DFloatGDL*>(p0->Convert2( GDL_FLOAT, BaseGDL::COPY));
	  Guard<DFloatGDL> p0SSGuard( p0SS);
	}

	DFloatGDL* res = new DFloatGDL( p0->Dim(), BaseGDL::NOZERO);
	Guard<DFloatGDL> resGuard( res);

	gsl_matrix *mat = gsl_matrix_alloc(p0->Dim(0), p0->Dim(1));
	GDLGuard<gsl_matrix> g1( mat, gsl_matrix_free);
	gsl_matrix *inverse = gsl_matrix_calloc(p0->Dim(0), p0->Dim(1));
	GDLGuard<gsl_matrix> g2( inverse, gsl_matrix_free);
	gsl_permutation *perm = gsl_permutation_alloc(p0->Dim(0));
	GDLGuard<gsl_permutation> g3( perm, gsl_permutation_free);

	  switch ( p0->Type()) {
	  case GDL_FLOAT: for( SizeT i=0; i<nEl; ++i) { f64 = (double) (*p0F)[i]; memcpy(&mat->data[i], &f64, szdbl); } break;
	  case GDL_LONG:  for( SizeT i=0; i<nEl; ++i) { f64 = (double) (*p0L)[i]; memcpy(&mat->data[i], &f64, szdbl); } break;
	  case GDL_LONG64:  for( SizeT i=0; i<nEl; ++i) { f64 = (double) (*p0L64)[i]; memcpy(&mat->data[i], &f64, szdbl); } break;
	  case GDL_ULONG: for( SizeT i=0; i<nEl; ++i) { f64 = (double) (*p0UL)[i]; memcpy(&mat->data[i], &f64, szdbl); } break;
	  case GDL_ULONG64: for( SizeT i=0; i<nEl; ++i) { f64 = (double) (*p0UL64)[i]; memcpy(&mat->data[i], &f64, szdbl); } break;
	  case GDL_INT:   for( SizeT i=0; i<nEl; ++i) { f64 = (double) (*p0I)[i]; memcpy(&mat->data[i], &f64, szdbl); } break;
	  case GDL_STRING:for( SizeT i=0; i<nEl; ++i) { f64 = (double) (*p0SS)[i]; memcpy(&mat->data[i], &f64, szdbl); } break;
	  case GDL_UINT:  for( SizeT i=0; i<nEl; ++i) { f64 = (double) (*p0UI)[i]; memcpy(&mat->data[i], &f64, szdbl); } break;
	  case GDL_BYTE:  for( SizeT i=0; i<nEl; ++i) { f64 = (double) (*p0B)[i]; memcpy(&mat->data[i], &f64, szdbl); } break;
        default: break; //not reached. pacifies compilers.
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
  BaseGDL* invert_eigen_fun( EnvT* e, bool hasDouble)
  {
    
    BaseGDL* p0 = e->GetParDefined( 0);
    SizeT nEl = p0->N_Elements();

    // related to "status" param : see comment in "invert_gsl_fun"
    SizeT nParam=e->NParam(1);
    if (nParam == 2) e->AssureGlobalPar( 1);
    long singular = 0;

    // only one element matrix already treated in AC_invert_fun
    // more than one element matrix

    long NbCol, NbRow;

    if( p0->Type() == GDL_COMPLEX) {
      if (hasDouble) {
        DComplexDblGDL* p0DC = static_cast<DComplexDblGDL*> (p0->Convert2(GDL_COMPLEXDBL, BaseGDL::COPY)); //expensive copy, guard.
        Guard<BaseGDL> g(p0DC);
        NbCol = p0->Dim(0);
        NbRow = p0->Dim(1);
        Map<Matrix<complex<double>, Dynamic, Dynamic> > m0(&(*p0DC)[0], NbCol, NbRow);
        Eigen::FullPivLU<MatrixXcd> lu(m0);
        if (!lu.isInvertible()) {
          if (nParam == 2) e->SetPar(1, new DLongGDL(1));
          g.Release(); //need a double complex in return.
          return p0DC; //must return a copy of input
        }
        if (abs(lu.determinant()) * LOG10E < 1e-5) singular = 2;
        else singular = 0;
        DComplexDblGDL* res = new DComplexDblGDL(p0->Dim(), BaseGDL::NOZERO);
        Map<MatrixXcd>(&(*res)[0], NbCol, NbRow) = lu.inverse();
        if (nParam == 2) e->SetPar(1, new DLongGDL(singular));
        return res;
      } else {
        DComplexGDL* p0C = static_cast<DComplexGDL*> (p0); //no expensive copy.
        NbCol = p0->Dim(0);
        NbRow = p0->Dim(1);
        Map<Matrix<complex<float>, Dynamic, Dynamic> > m0(&(*p0C)[0], NbCol, NbRow);
        Eigen::FullPivLU<MatrixXcf> lu(m0);
        if (!lu.isInvertible()) {
          if (nParam == 2) e->SetPar(1, new DLongGDL(1));
          return p0C->Dup(); //must return a copy of input
        }
        if (abs(lu.determinant()) * LOG10E < 1e-5) singular = 2;
        else singular = 0;
        DComplexGDL* res = new DComplexGDL(p0->Dim(), BaseGDL::NOZERO);
        Map<MatrixXcf>(&(*res)[0], NbCol, NbRow) = lu.inverse();
        if (nParam == 2) e->SetPar(1, new DLongGDL(singular));
        return res;
      }
    }
    else if( p0->Type() == GDL_COMPLEXDBL) {
      DComplexDblGDL* p0DC = static_cast<DComplexDblGDL*> (p0); //no expensive copy.
      NbCol = p0->Dim(0);
      NbRow = p0->Dim(1);
      Map<Matrix<complex<double>, Dynamic, Dynamic> > m0(&(*p0DC)[0], NbCol, NbRow);
      Eigen::FullPivLU<MatrixXcd> lu(m0);
      if (!lu.isInvertible()) {
        if (nParam == 2) e->SetPar(1, new DLongGDL(1));
        return p0DC->Dup(); //must return a copy of input
      }
      if (abs(lu.determinant()) * LOG10E < 1e-5) singular = 2;
      else singular = 0;
      DComplexDblGDL* res = new DComplexDblGDL(p0->Dim(), BaseGDL::NOZERO);
      Map<MatrixXcd>(&(*res)[0], NbCol, NbRow) = lu.inverse();
      if (nParam == 2) e->SetPar(1, new DLongGDL(singular));
      return res;
    }
    
    else if (p0->Type() == GDL_DOUBLE) {
      DDoubleGDL* p0D = static_cast<DDoubleGDL*> (p0); //already double: no expensive copy.
      NbCol = p0->Dim(0);
      NbRow = p0->Dim(1);
      Map<Matrix<double, Dynamic, Dynamic> > m0(&(*p0D)[0], NbCol, NbRow);
      Eigen::FullPivLU<MatrixXd> lu(m0);
      if (!lu.isInvertible()) {
        if (nParam == 2) e->SetPar(1, new DLongGDL(1));
        return p0D->Dup();
      }
      if (abs(lu.determinant()) * LOG10E < 1e-5) singular = 2;
      else singular = 0;
      DDoubleGDL* res = new DDoubleGDL(p0->Dim(), BaseGDL::NOZERO);
      Map<MatrixXd>(&(*res)[0], NbCol, NbRow) = lu.inverse();
      if (nParam == 2) e->SetPar(1, new DLongGDL(singular));
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
	     p0->Type() == GDL_BYTE) {
      if (hasDouble) {
        DDoubleGDL* p0D = static_cast<DDoubleGDL*> (p0->Convert2(GDL_DOUBLE, BaseGDL::COPY)); //expensive copy.
        Guard<BaseGDL> g(p0D);
        NbCol = p0->Dim(0);
        NbRow = p0->Dim(1);
        Map<Matrix<double, Dynamic, Dynamic> > m0(&(*p0D)[0], NbCol, NbRow);
        Eigen::FullPivLU<MatrixXd> lu(m0);
        if (!lu.isInvertible()) {
          if (nParam == 2) e->SetPar(1, new DLongGDL(1));
          g.Release(); //need a double in return whatever p0 was.
          return p0D; //return a "double" copy of input
        }
        if (abs(lu.determinant()) * LOG10E < 1e-5) singular = 2;
        else singular = 0;
        DDoubleGDL* res = new DDoubleGDL(p0->Dim(), BaseGDL::NOZERO);
        Map<MatrixXd>(&(*res)[0], NbCol, NbRow) = lu.inverse();
        if (nParam == 2) e->SetPar(1, new DLongGDL(singular));
        return res;
      } else {
        DFloatGDL* p0F; 
        Guard<BaseGDL> g;
        if (p0->Type() == GDL_FLOAT) {
          p0F = static_cast<DFloatGDL*> (p0); //no conversion saves one copy
        } else {
          p0F = static_cast<DFloatGDL*> (p0->Convert2(GDL_FLOAT, BaseGDL::COPY)); //expensive copy.
          g.Reset(p0F); //... that will be removed at end
        }
        NbCol = p0->Dim(0);
        NbRow = p0->Dim(1);
        Map<Matrix<float, Dynamic, Dynamic> > m0(&(*p0F)[0], NbCol, NbRow);
        Eigen::FullPivLU<MatrixXf> lu(m0);
        if (!lu.isInvertible()) {
          if (nParam == 2) e->SetPar(1, new DLongGDL(1));
          if (p0->Type() == GDL_FLOAT) return p0->Dup(); else {g.Release(); return p0F; } //return a "float" copy of input
        }
        if (abs(lu.determinant()) * LOG10E < 1e-5) singular = 2;
        else singular = 0;
        DFloatGDL* res = new DFloatGDL(p0->Dim(), BaseGDL::NOZERO);
        Map<MatrixXf>(&(*res)[0], NbCol, NbRow) = lu.inverse();
        if (nParam == 2) e->SetPar(1, new DLongGDL(singular));
        return res;
      }
    }
    throw;
    }
#else
  BaseGDL* invert_eigen_fun( EnvT* e, bool hasDouble){
    e->Throw( "sorry, INVERT with Eigen not available. GDL must be compiled with Eigen lib.");
     return NULL;
  }
#endif

}

