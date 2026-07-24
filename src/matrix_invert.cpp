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

//GD July 2026 following #2206 and given that Eigen:: flavour has shown memory leaks, use GSL by default.
#if defined(USE_EIGEN)
    bool Eigen_possible_but_discouraged=TRUE;
#else
    bool Eigen_possible_but_discouraged=FALSE;
#endif
    
    if (e->KeywordSet(EIGENIx) && Eigen_possible_but_discouraged) return invert_eigen_fun(e, hasDouble);
    if (e->KeywordSet(EIGENIx) && !Eigen_possible_but_discouraged) Warning("Eigen Invert not available, GSL used");
    
    return invert_gsl_fun(e, hasDouble);
  }

  BaseGDL* invert_gsl_fun( EnvT* e, bool hasDouble) {

    BaseGDL* p0 = e->GetParDefined(0);
    SizeT nEl = p0->N_Elements();

    // related to "status" param
    // check here, if not done, res would be pending in case of SetPar() throws
    // SetPar() only throws in AssureGlobalPar()
    SizeT nParam = e->NParam(1);
    if (nParam == 2) e->AssureGlobalPar(1);

    long singular = 0;

    int s;
    float f32;
    double f64;
    double det;

    // matrix with ONE element already treated in AC_invert_fun

    // more than one element matrix

    // GSL error handling
    //    SetTemporaryGSLErrorHandlerT lib::setTemporaryGSLErrorHandler( GDLGenericGSLErrorHandler);

    if (ComplexType(p0->Type())) {
      DComplexDblGDL* p0C = static_cast<DComplexDblGDL*>(p0->Convert2( GDL_COMPLEXDBL, BaseGDL::COPY)); //Dup or converted, to be guarded.
	  Guard<DComplexDblGDL> Guard( p0C);
      
      gsl_matrix_complex_view mv = gsl_matrix_complex_view_array((double*)p0C->DataAddr(),p0->Dim(0), p0->Dim(1) );
      gsl_matrix_complex *mat = &mv.matrix;
      gsl_permutation *perm = gsl_permutation_alloc(p0->Dim(0));
      GDLGuard<gsl_permutation> g3(perm, gsl_permutation_free);

      if (hasDouble || p0->Type() == GDL_COMPLEXDBL) { //save allocation of output data
        DComplexDblGDL* res = new DComplexDblGDL(p0->Dim(), BaseGDL::ZERO);
        gsl_matrix_complex_view mvi = gsl_matrix_complex_view_array((double*)res->DataAddr(),p0->Dim(0), p0->Dim(1) );
        gsl_matrix_complex *inverse = &mvi.matrix;
        gsl_linalg_complex_LU_decomp(mat, perm, &s);
        det = gsl_linalg_complex_LU_lndet(mat);
        if (gsl_isinf(det) == 0) {
          gsl_linalg_complex_LU_invert(mat, perm, inverse);
          if (abs(det) * LOG10E < 1e-5) singular = 2;
        } else singular = 1;
        if (nParam == 2) e->SetPar(1, new DLongGDL(singular));
        return res;
      } else {
        float f32_2[2];
        double f64_2[2];
        gsl_matrix_complex *inverse = gsl_matrix_complex_calloc(p0->Dim(0), p0->Dim(1));
        GDLGuard<gsl_matrix_complex> g2(inverse, gsl_matrix_complex_free);
      
        gsl_linalg_complex_LU_decomp(mat, perm, &s);
        det = gsl_linalg_complex_LU_lndet(mat);
        if (gsl_isinf(det) == 0) {
          gsl_linalg_complex_LU_invert(mat, perm, inverse);
          if (abs(det) * LOG10E < 1e-5) singular = 2;
        } else singular = 1;
        if (nParam == 2) e->SetPar(1, new DLongGDL(singular));
        DComplexGDL* res = new DComplexGDL(p0C->Dim(), BaseGDL::NOZERO);
        for (SizeT i = 0; i < nEl; ++i) {
          memcpy(&f64_2[0], &inverse->data[2 * i], szdbl * 2);
          f32_2[0] = (float) f64_2[0];
          f32_2[1] = (float) f64_2[1];
          memcpy(&(*res)[i], &f32_2[0], szflt * 2);
        }
        return res;
      }
    } else {
	  DDoubleGDL* p0D = static_cast<DDoubleGDL*>(p0->Convert2( GDL_DOUBLE, BaseGDL::COPY)); //Dup or converted, to be guarded.
	  Guard<DDoubleGDL> Guard( p0D);

      gsl_matrix_view mv = gsl_matrix_view_array((double*)p0D->DataAddr(),p0->Dim(0), p0->Dim(1) );
      gsl_matrix *mat = &mv.matrix;
      gsl_permutation *perm = gsl_permutation_alloc(p0->Dim(0));
      GDLGuard<gsl_permutation> g3(perm, gsl_permutation_free);

      if (hasDouble || p0->Type() == GDL_DOUBLE) { //save allocation of output data
        DDoubleGDL* res = new DDoubleGDL(p0->Dim(), BaseGDL::ZERO);
        gsl_matrix_view mvi = gsl_matrix_view_array((double*)res->DataAddr(),p0->Dim(0), p0->Dim(1) );
        gsl_matrix *inverse = &mvi.matrix;
        gsl_linalg_LU_decomp(mat, perm, &s);
        det = gsl_linalg_LU_lndet(mat);
        if (gsl_isinf(det) == 0) {
          gsl_linalg_LU_invert(mat, perm, inverse);
          if (abs(det) * LOG10E < 1e-5) singular = 2;
        } else singular = 1;
        if (nParam == 2) e->SetPar(1, new DLongGDL(singular));
        return res;
      } else {
        gsl_matrix *inverse = gsl_matrix_calloc(p0->Dim(0), p0->Dim(1));
        GDLGuard<gsl_matrix> g2(inverse, gsl_matrix_free);
        gsl_linalg_LU_decomp(mat, perm, &s);
        det = gsl_linalg_LU_lndet(mat);
        if (gsl_isinf(det) == 0) {
          gsl_linalg_LU_invert(mat, perm, inverse);
          if (abs(det) * LOG10E < 1e-5) singular = 2;
        } else singular = 1;
        if (nParam == 2) e->SetPar(1, new DLongGDL(singular));
        DFloatGDL* res = new DFloatGDL(p0->Dim(), BaseGDL::NOZERO);
        for (SizeT i = 0; i < nEl; ++i) {
          float f32 = (float) inverse->data[i];
          memcpy(&(*res)[i], &f32, 4);
        }
        return res;       
      }
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

