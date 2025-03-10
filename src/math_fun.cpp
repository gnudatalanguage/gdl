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
#include "envt.hpp"
#include "math_utl.hpp"
#include "math_fun.hpp"
#include "gdlfpexceptions.hpp"

//#define GDL_DEBUG
//#undef GDL_DEBUG

#ifdef _MSC_VER
#define round(f) floor(f+0.5)
#endif

namespace lib {

  using namespace std;

  template< typename srcT, typename destT>
  void TransposeFromToGSL(srcT* src, destT* dest, SizeT srcStride1, SizeT nEl) {
    for (SizeT d = 0, ix = 0, srcDim0 = 0; d < nEl; ++d) {
      dest[ d] = src[ ix];
      ix += srcStride1;
      if (ix >= nEl)
        ix = ++srcDim0;
    }
  }

  template< typename srcT, typename destT>
  void FromToGSL(srcT* src, destT* dest, SizeT nEl) {
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (SizeT d = 0; d < nEl; ++d) dest[ d] = src[ d];
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (SizeT d = 0; d < nEl; ++d) dest[ d] = src[ d];
    }
  }

  void svdc(EnvT* e) {
    e->NParam(4);

    static int doubleKWIx = e->KeywordIx("DOUBLE");
    bool doubleKW = e->KeywordSet(doubleKWIx);

    BaseGDL* A = e->GetParDefined(0);
    doubleKW = doubleKW || (A->Type() == GDL_DOUBLE) || (A->Type() == GDL_COMPLEXDBL);

    if (doubleKW) {
      A = e->GetParAs< DDoubleGDL>(0);
    } else {
      A = e->GetParAs< DFloatGDL>(0);
    }
    if (A->Rank() != 2)
      e->Throw("Argument must be a 2-D matrix: " + e->GetParString(0));

    e->AssureGlobalPar(1); // W
    e->AssureGlobalPar(2); // U
    e->AssureGlobalPar(3); // V

    static int columnKWIx = e->KeywordIx("COLUMN");
    bool columnKW = e->KeywordSet(columnKWIx);
    static int itmaxKWIx = e->KeywordIx("ITMAX");
    DLong itMax = 30;
    e->AssureLongScalarKWIfPresent(itmaxKWIx, itMax);

    DLong n;
    DLong m;
    if (columnKW) {
      n = A->Dim(1);
      m = A->Dim(0);
    } else {
      n = A->Dim(0);
      m = A->Dim(1);
    }
    if (m < n)
      e->Throw("SVD of NxM matrix with N>M is not implemented yet.");

    DLong nEl = A->N_Elements();

    if (doubleKW) {
      DDoubleGDL* AA = static_cast<DDoubleGDL*> (A);

      gsl_matrix *aGSL = gsl_matrix_alloc(m, n);
      GDLGuard<gsl_matrix> g1(aGSL, gsl_matrix_free);
      if (!columnKW)
        memcpy(aGSL->data, &(*AA)[0], nEl * sizeof ( DDouble));
      else
        TransposeFromToGSL< DDouble, double>(&(*AA)[0], aGSL->data, AA->Dim(0), nEl);

      gsl_matrix *vGSL = gsl_matrix_alloc(n, n);
      GDLGuard<gsl_matrix> g2(vGSL, gsl_matrix_free);
      gsl_vector *wGSL = gsl_vector_alloc(n);
      GDLGuard<gsl_vector> g3(wGSL, gsl_vector_free);

      gsl_vector *work = gsl_vector_alloc(n);
      GDLGuard<gsl_vector> g4(work, gsl_vector_free);
      gsl_linalg_SV_decomp(aGSL, vGSL, wGSL, work);
      // 	gsl_vector_free( work);

      // aGSL -> uGSL
      gsl_matrix *uGSL = aGSL; // why?

      // U
      DDoubleGDL* U = new DDoubleGDL(AA->Dim(), BaseGDL::NOZERO);
      if (!columnKW)
        memcpy(&(*U)[0], uGSL->data, nEl * sizeof ( DDouble));
      else
        TransposeFromToGSL< double, DDouble>(uGSL->data, &(*U)[0], U->Dim(1), nEl);
      // 	gsl_matrix_free( uGSL);
      e->SetPar(2, U);

      // V
      DDoubleGDL* V = new DDoubleGDL(dimension(n, n), BaseGDL::NOZERO);
      if (!columnKW)
        memcpy(&(*V)[0], vGSL->data, n * n * sizeof ( DDouble));
      else
        TransposeFromToGSL< double, DDouble>(vGSL->data, &(*V)[0], n, n * n);
      // 	gsl_matrix_free( vGSL);
      e->SetPar(3, V);

      // W
      DDoubleGDL* W = new DDoubleGDL(dimension(n), BaseGDL::NOZERO);
      memcpy(&(*W)[0], wGSL->data, n * sizeof ( DDouble));
      // 	gsl_vector_free( wGSL);
      e->SetPar(1, W);
    } else // float
    {
      DFloatGDL* AA = static_cast<DFloatGDL*> (A);

      gsl_matrix *aGSL = gsl_matrix_alloc(m, n);
      GDLGuard<gsl_matrix> g1(aGSL, gsl_matrix_free);
      if (!columnKW)
        FromToGSL< DFloat, double>(&(*AA)[0], aGSL->data, nEl);
      else
        TransposeFromToGSL< DFloat, double>(&(*AA)[0], aGSL->data, AA->Dim(0), nEl);

      gsl_matrix *vGSL = gsl_matrix_alloc(n, n);
      GDLGuard<gsl_matrix> g2(vGSL, gsl_matrix_free);
      gsl_vector *wGSL = gsl_vector_alloc(n);
      GDLGuard<gsl_vector> g3(wGSL, gsl_vector_free);

      gsl_vector *work = gsl_vector_alloc(n);
      GDLGuard<gsl_vector> g4(work, gsl_vector_free);
      gsl_linalg_SV_decomp(aGSL, vGSL, wGSL, work);
      // 	gsl_vector_free( work);

      // aGSL -> uGSL
      gsl_matrix *uGSL = aGSL; // why?

      // U
      DFloatGDL* U = new DFloatGDL(AA->Dim(), BaseGDL::NOZERO);
      if (!columnKW)
        FromToGSL< double, DFloat>(uGSL->data, &(*U)[0], nEl);
      else
        TransposeFromToGSL< double, DFloat>(uGSL->data, &(*U)[0], U->Dim(1), nEl);
      // 	gsl_matrix_free( uGSL);
      e->SetPar(2, U);

      // V
      DFloatGDL* V = new DFloatGDL(dimension(n, n), BaseGDL::NOZERO);
      if (!columnKW)
        FromToGSL< double, DFloat>(vGSL->data, &(*V)[0], n * n);
      else
        TransposeFromToGSL< double, DFloat>(vGSL->data, &(*V)[0], n, n * n);
      // 	gsl_matrix_free( vGSL);
      e->SetPar(3, V);

      // W
      DFloatGDL* W = new DFloatGDL(dimension(n), BaseGDL::NOZERO);
      FromToGSL< double, DFloat>(wGSL->data, &(*W)[0], n);
      // 	gsl_vector_free( wGSL);
      e->SetPar(1, W);
    }
  }

  ////These functions return LONG or LONG64.
  ////  Note: IDL adds the supplementary optization that they are not called at all if they are applied on integer types, as, e.g., round(integer)==integer.
  //// TBD: This trick must be performed at interpreter level.
  /// O3 compiled code speed is OK wrt IDL in all cases.

  template< typename T> //ONLY USED FOR FLOATS AND DOUBLE
  BaseGDL*
  round_fun_template(BaseGDL* p0, bool isKWSetL64) {
    T* p0C = static_cast<T*> (p0);
    SizeT nEl = p0->N_Elements();
    // L64 keyword support
    if (isKWSetL64) {
      DLong64GDL* res = new DLong64GDL(p0C->Dim(), BaseGDL::NOZERO);
      if (nEl == 1) {
        (*res)[ 0] = round((*p0C)[ 0]);
        return res;
      }
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = round((*p0C)[ i]);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = round((*p0C)[ i]);
      }
      return res;
    } else {
      DLongGDL* res = new DLongGDL(p0C->Dim(), BaseGDL::NOZERO);
      if (nEl == 1) {
        (*res)[ 0] = round((*p0C)[ 0]);
        return res;
      }
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = round((*p0C)[ i]);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = round((*p0C)[ i]);
      }

      return res;
    }
  }

  BaseGDL*
  round_fun(EnvT* e) {
    e->NParam(1); //, "ROUND");
    BaseGDL* p0 = e->GetParDefined(0); //, "ROUND");

    SizeT nEl = p0->N_Elements();

    if (nEl == 0) e->Throw("Variable is undefined: " + e->GetParString(0));
    if (!(NumericType(p0->Type()))) e->Throw(p0->TypeStr() + " expression: not allowed in this context: " + e->GetParString(0));

    static int l64Ix = e->KeywordIx("L64");
    bool forceL64 = e->KeywordSet(l64Ix);
    
    //L64 means it: output IS ALWAYS L64.
    if (forceL64) {
      if (p0->Type() == GDL_COMPLEX) {
        DComplexGDL* p0C = static_cast<DComplexGDL*> (p0);
        SizeT nEl = p0->N_Elements();
        DLong64GDL* res = new DLong64GDL(p0C->Dim(), BaseGDL::NOZERO);
        if (nEl == 1) {
          (*res)[ 0] = round((*p0C)[ 0].real());
          return res;
        }
        if ((GDL_NTHREADS=parallelize( nEl))==1) {
          for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = round((*p0C)[ i].real());
        } else {
          TRACEOMP(__FILE__, __LINE__)
 #pragma omp parallel for num_threads(GDL_NTHREADS)
           for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = round((*p0C)[ i].real());
        }
        return res;
      } else if (p0->Type() == GDL_COMPLEXDBL) {
        DComplexDblGDL* p0C = static_cast<DComplexDblGDL*> (p0);
        SizeT nEl = p0->N_Elements();
        DLong64GDL* res = new DLong64GDL(p0C->Dim(), BaseGDL::NOZERO);
        if (nEl == 1) {
          (*res)[ 0] = round((*p0C)[ 0].real());
          return res;
        }
        if ((GDL_NTHREADS=parallelize( nEl))==1) {
          for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = round((*p0C)[ i].real());
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = round((*p0C)[ i].real());
        }
        return res;
      } else if (p0->Type() == GDL_DOUBLE) return round_fun_template< DDoubleGDL>(p0, true);
      else if (p0->Type() == GDL_FLOAT) return round_fun_template< DFloatGDL>(p0, true);
      else if (p0->Type() == GDL_LONG64) return p0->Dup();
      else return p0->Convert2(GDL_LONG64, BaseGDL::COPY);
    } else {
      if (p0->Type() == GDL_COMPLEX) {
        DComplexGDL* p0C = static_cast<DComplexGDL*> (p0);
        SizeT nEl = p0->N_Elements();
        DLongGDL* res = new DLongGDL(p0C->Dim(), BaseGDL::NOZERO);
        if (nEl == 1) {
          (*res)[ 0] = round((*p0C)[ 0].real());
          return res;
        }
        if ((GDL_NTHREADS=parallelize( nEl))==1) {
          for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = round((*p0C)[ i].real());
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = round((*p0C)[ i].real());
        }
        return res;
      } else if (p0->Type() == GDL_COMPLEXDBL) {
        DComplexDblGDL* p0C = static_cast<DComplexDblGDL*> (p0);
        SizeT nEl = p0->N_Elements();
        DLongGDL* res = new DLongGDL(p0C->Dim(), BaseGDL::NOZERO);
        if (nEl == 1) {
          (*res)[ 0] = round((*p0C)[ 0].real());
          return res;
        }
        if ((GDL_NTHREADS=parallelize( nEl))==1) {
          for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = round((*p0C)[ i].real());
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = round((*p0C)[ i].real());
        }
        return res;
      } else if (p0->Type() == GDL_DOUBLE) return round_fun_template< DDoubleGDL>(p0, false);
      else if (p0->Type() == GDL_FLOAT) return round_fun_template< DFloatGDL>(p0, false);
      else return p0->Dup();
    }
  }

  template< typename T>
  BaseGDL* ceil_fun_template(BaseGDL* p0, bool isKWSetL64) {
    T* p0C = static_cast<T*> (p0);
    SizeT nEl = p0->N_Elements();
    // L64 keyword support
    if (isKWSetL64) {
      DLong64GDL* res = new DLong64GDL(p0C->Dim(), BaseGDL::NOZERO);
      if (nEl == 1) {
        (*res)[ 0] = ceil((*p0C)[ 0]);
        return res;
      }
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = ceil((*p0C)[ i]);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = ceil((*p0C)[ i]);
      }
      return res;
    } else {
      DLongGDL* res = new DLongGDL(p0C->Dim(), BaseGDL::NOZERO);
      if (nEl == 1) {
        (*res)[ 0] = ceil((*p0C)[ 0]);
        return res;
      }
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = ceil((*p0C)[ i]);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = ceil((*p0C)[ i]);
      }

      return res;
    }
  }

  BaseGDL* ceil_fun(EnvT* e) {
    e->NParam(1); //, "CEIL");
    BaseGDL* p0 = e->GetParDefined(0); //, "CEIL");

    SizeT nEl = p0->N_Elements();
    if (nEl == 0) e->Throw("Variable is undefined: " + e->GetParString(0));
    if (!(NumericType(p0->Type()))) e->Throw(p0->TypeStr() + " expression: not allowed in this context: " + e->GetParString(0));

    static int l64Ix = e->KeywordIx("L64");
    bool forceL64 = e->KeywordSet(l64Ix);

    //L64 means it: output IS ALWAYS L64.
    if (forceL64) {
      if (p0->Type() == GDL_COMPLEX) {
        DComplexGDL* p0C = static_cast<DComplexGDL*> (p0);
        SizeT nEl = p0->N_Elements();
        DLong64GDL* res = new DLong64GDL(p0C->Dim(), BaseGDL::NOZERO);
        if (nEl == 1) {
          (*res)[ 0] = ceil((*p0C)[ 0].real());
          return res;
        }
        if ((GDL_NTHREADS=parallelize( nEl))==1) {
          for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = ceil((*p0C)[ i].real());
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = ceil((*p0C)[ i].real());
        }
        return res;
      } else if (p0->Type() == GDL_COMPLEXDBL) {
        DComplexDblGDL* p0C = static_cast<DComplexDblGDL*> (p0);
        SizeT nEl = p0->N_Elements();
        DLong64GDL* res = new DLong64GDL(p0C->Dim(), BaseGDL::NOZERO);
        if (nEl == 1) {
          (*res)[ 0] = ceil((*p0C)[ 0].real());
          return res;
        }
        if ((GDL_NTHREADS=parallelize( nEl))==1) {
          for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = ceil((*p0C)[ i].real());
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = ceil((*p0C)[ i].real());
        }
        return res;
      } else if (p0->Type() == GDL_DOUBLE) return ceil_fun_template< DDoubleGDL>(p0, true);
      else if (p0->Type() == GDL_FLOAT) return ceil_fun_template< DFloatGDL>(p0, true);
      else if (p0->Type() == GDL_LONG64) return p0->Dup();
      else return p0->Convert2(GDL_LONG64, BaseGDL::COPY);
    } else {
      if (p0->Type() == GDL_COMPLEX) {
        DComplexGDL* p0C = static_cast<DComplexGDL*> (p0);
        SizeT nEl = p0->N_Elements();
        DLongGDL* res = new DLongGDL(p0C->Dim(), BaseGDL::NOZERO);
        if (nEl == 1) {
          (*res)[ 0] = ceil((*p0C)[ 0].real());
          return res;
        }
        if ((GDL_NTHREADS=parallelize( nEl))==1) {
          for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = ceil((*p0C)[ i].real());
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = ceil((*p0C)[ i].real());
        }
        return res;
      } else if (p0->Type() == GDL_COMPLEXDBL) {
        DComplexDblGDL* p0C = static_cast<DComplexDblGDL*> (p0);
        SizeT nEl = p0->N_Elements();
        DLongGDL* res = new DLongGDL(p0C->Dim(), BaseGDL::NOZERO);
        if (nEl == 1) {
          (*res)[ 0] = ceil((*p0C)[ 0].real());
          return res;
        }
        if ((GDL_NTHREADS=parallelize( nEl))==1) {
          for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = ceil((*p0C)[ i].real());
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = ceil((*p0C)[ i].real());
        }
        return res;
      } else if (p0->Type() == GDL_DOUBLE) return ceil_fun_template< DDoubleGDL>(p0, false);
      else if (p0->Type() == GDL_FLOAT) return ceil_fun_template< DFloatGDL>(p0, false);
      else return p0->Dup();
    }
  }

  template< typename T>
  BaseGDL* floor_fun_template(BaseGDL* p0, bool isKWSetL64) {
    T* p0C = static_cast<T*> (p0);
    SizeT nEl = p0->N_Elements();
    // L64 keyword support
    if (isKWSetL64) {
      DLong64GDL* res = new DLong64GDL(p0C->Dim(), BaseGDL::NOZERO);
      if (nEl == 1) {
        (*res)[ 0] = floor((*p0C)[ 0]);
        return res;
      }
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = floor((*p0C)[ i]);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = floor((*p0C)[ i]);
      }
      return res;
    } else {
      DLongGDL* res = new DLongGDL(p0C->Dim(), BaseGDL::NOZERO);
      if (nEl == 1) {
        (*res)[ 0] = floor((*p0C)[ 0]);
        return res;
      }
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = floor((*p0C)[ i]);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = floor((*p0C)[ i]);
      }

      return res;
    }
  }

  BaseGDL* floor_fun(EnvT* e) {
    e->NParam(1); //, "floor");
    BaseGDL* p0 = e->GetParDefined(0); //, "floor");

    SizeT nEl = p0->N_Elements();
    if (nEl == 0) e->Throw("Variable is undefined: " + e->GetParString(0));
    if (!(NumericType(p0->Type()))) e->Throw(p0->TypeStr() + " expression: not allowed in this context: " + e->GetParString(0));

    static int l64Ix = e->KeywordIx("L64");
    bool forceL64 = e->KeywordSet(l64Ix);

    //L64 means it: output IS ALWAYS L64.
    if (forceL64) {
      if (p0->Type() == GDL_COMPLEX) {
        DComplexGDL* p0C = static_cast<DComplexGDL*> (p0);
        SizeT nEl = p0->N_Elements();
        DLong64GDL* res = new DLong64GDL(p0C->Dim(), BaseGDL::NOZERO);
        if (nEl == 1) {
          (*res)[ 0] = floor((*p0C)[ 0].real());
          return res;
        }
        if ((GDL_NTHREADS=parallelize( nEl))==1) {
          for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = floor((*p0C)[ i].real());
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = floor((*p0C)[ i].real());
        }
        return res;
      } else if (p0->Type() == GDL_COMPLEXDBL) {
        DComplexDblGDL* p0C = static_cast<DComplexDblGDL*> (p0);
        SizeT nEl = p0->N_Elements();
        DLong64GDL* res = new DLong64GDL(p0C->Dim(), BaseGDL::NOZERO);
        if (nEl == 1) {
          (*res)[ 0] = floor((*p0C)[ 0].real());
          return res;
        }
        if ((GDL_NTHREADS=parallelize( nEl))==1) {
          for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = floor((*p0C)[ i].real());
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = floor((*p0C)[ i].real());
        }
        return res;
      } else if (p0->Type() == GDL_DOUBLE) return floor_fun_template< DDoubleGDL>(p0, true);
      else if (p0->Type() == GDL_FLOAT) return floor_fun_template< DFloatGDL>(p0, true);
      else if (p0->Type() == GDL_LONG64) return p0->Dup();
      else return p0->Convert2(GDL_LONG64, BaseGDL::COPY);
    } else {
      if (p0->Type() == GDL_COMPLEX) {
        DComplexGDL* p0C = static_cast<DComplexGDL*> (p0);
        SizeT nEl = p0->N_Elements();
        DLongGDL* res = new DLongGDL(p0C->Dim(), BaseGDL::NOZERO);
        if (nEl == 1) {
          (*res)[ 0] = floor((*p0C)[ 0].real());
          return res;
        }
        if ((GDL_NTHREADS=parallelize( nEl))==1) {
          for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = floor((*p0C)[ i].real());
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = floor((*p0C)[ i].real());
        }
        return res;
      } else if (p0->Type() == GDL_COMPLEXDBL) {
        DComplexDblGDL* p0C = static_cast<DComplexDblGDL*> (p0);
        SizeT nEl = p0->N_Elements();
        DLongGDL* res = new DLongGDL(p0C->Dim(), BaseGDL::NOZERO);
        if (nEl == 1) {
          (*res)[ 0] = floor((*p0C)[ 0].real());
          return res;
        }
        if ((GDL_NTHREADS=parallelize( nEl))==1) {
          for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = floor((*p0C)[ i].real());
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
            for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = floor((*p0C)[ i].real());
        }
        return res;
      } else if (p0->Type() == GDL_DOUBLE) return floor_fun_template< DDoubleGDL>(p0, false);
      else if (p0->Type() == GDL_FLOAT) return floor_fun_template< DFloatGDL>(p0, false);
      else return p0->Dup();
    }
  }

  // GDL Direct functions (no new environment created because
  // the function has no keywords and only one parameter-->no overheads)
  // RETURNS: float (all 32 bits + strings), double, complex, double complex outputs.

  //SQRT

  template< typename T>
  BaseGDL* sqrt_fun_template(BaseGDL* p0) {
    T* p0C = static_cast<T*> (p0);
    T* res = new T(p0C->Dim(), BaseGDL::NOZERO);
    SizeT nEl = p0->N_Elements();
	GDLStartRegisteringFPExceptions();
	
    if (nEl == 1) {
      (*res)[0] = sqrt((*p0C)[0]);
      return res;
    }
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = sqrt((*p0C)[ i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = sqrt((*p0C)[ i]);
    }

	GDLStopRegisteringFPExceptions();
	
    return res;
  }

  template< typename T>
  BaseGDL* sqrt_fun_template_grab(BaseGDL* p0) {
    T* p0C = static_cast<T*> (p0);
    SizeT nEl = p0->N_Elements();

	GDLStartRegisteringFPExceptions();
	
    if (nEl == 1) {
      (*p0C)[0] = sqrt((*p0C)[0]);
      return p0;
    }
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (SizeT i = 0; i < nEl; ++i) (*p0C)[ i] = sqrt((*p0C)[ i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (SizeT i = 0; i < nEl; ++i) (*p0C)[ i] = sqrt((*p0C)[ i]);
    }

	GDLStopRegisteringFPExceptions();
	
    return p0;
  }

  BaseGDL* sqrt_fun(BaseGDL* p0, bool isReference) {
    SizeT nEl = p0->N_Elements();
    DType p0Type = p0->Type();
    if (p0Type == GDL_UNDEF) throw GDLException("Variable is undefined: !NULL");
    if (p0Type == GDL_COMPLEX)
      if (isReference) return sqrt_fun_template< DComplexGDL>(p0);
      else return sqrt_fun_template_grab< DComplexGDL>(p0);
    else if (p0Type == GDL_COMPLEXDBL)
      if (isReference) return sqrt_fun_template< DComplexDblGDL>(p0);
      else return sqrt_fun_template_grab< DComplexDblGDL>(p0);
    else if (p0Type == GDL_DOUBLE)
      if (isReference) return sqrt_fun_template< DDoubleGDL>(p0);
      else return sqrt_fun_template_grab< DDoubleGDL>(p0);
    else if (p0Type == GDL_FLOAT)
      if (isReference) return sqrt_fun_template< DFloatGDL>(p0);
      else return sqrt_fun_template_grab< DFloatGDL>(p0);
    else {

	  GDLStartRegisteringFPExceptions();
	  
     DFloatGDL* res = static_cast<DFloatGDL*> (p0->Convert2(GDL_FLOAT, BaseGDL::COPY));
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (SizeT i = 0; i < nEl; ++i)(*res)[ i] = sqrt((*res)[ i]);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (SizeT i = 0; i < nEl; ++i)(*res)[ i] = sqrt((*res)[ i]);
	  }

	  GDLStopRegisteringFPExceptions();
	  
      return res;
    }
  }

  //SIN

  template< typename T>
  BaseGDL* sin_fun_template(BaseGDL* p0) {
    T* p0C = static_cast<T*> (p0);
    T* res = new T(p0C->Dim(), BaseGDL::NOZERO);
    SizeT nEl = p0->N_Elements();
    if (nEl == 1) {
      (*res)[0] = sin((*p0C)[0]);
      return res;
    }
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = sin((*p0C)[ i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = sin((*p0C)[ i]);
    }
    return res;
  }

  template< typename T>
  BaseGDL* sin_fun_template_grab(BaseGDL* p0) {
    T* p0C = static_cast<T*> (p0);
    SizeT nEl = p0->N_Elements();
    if (nEl == 1) {
      (*p0C)[0] = sin((*p0C)[0]);
      return p0;
    }
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (SizeT i = 0; i < nEl; ++i) (*p0C)[ i] = sin((*p0C)[ i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (SizeT i = 0; i < nEl; ++i) (*p0C)[ i] = sin((*p0C)[ i]);
    }
    return p0;
  }

  BaseGDL* sin_fun(BaseGDL* p0, bool isReference) {
    SizeT nEl = p0->N_Elements();
    DType p0Type = p0->Type();
    if (p0Type == GDL_UNDEF) throw GDLException("Variable is undefined: !NULL");
    if (p0Type == GDL_COMPLEX)
      if (isReference) return sin_fun_template< DComplexGDL>(p0);
      else return sin_fun_template_grab< DComplexGDL>(p0);
    else if (p0Type == GDL_COMPLEXDBL)
      if (isReference) return sin_fun_template< DComplexDblGDL>(p0);
      else return sin_fun_template_grab< DComplexDblGDL>(p0);
    else if (p0Type == GDL_DOUBLE)
      if (isReference) return sin_fun_template< DDoubleGDL>(p0);
      else return sin_fun_template_grab< DDoubleGDL>(p0);
    else if (p0Type == GDL_FLOAT)
      if (isReference) return sin_fun_template< DFloatGDL>(p0);
      else return sin_fun_template_grab< DFloatGDL>(p0);
    else {
      DFloatGDL* res = static_cast<DFloatGDL*> (p0->Convert2(GDL_FLOAT, BaseGDL::COPY));
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (SizeT i = 0; i < nEl; ++i)(*res)[ i] = sin((*res)[ i]);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (SizeT i = 0; i < nEl; ++i)(*res)[ i] = sin((*res)[ i]);
      }
      return res;
    }
  }

  //COS

  template< typename T>
  BaseGDL* cos_fun_template(BaseGDL* p0) {
    T* p0C = static_cast<T*> (p0);
    T* res = new T(p0C->Dim(), BaseGDL::NOZERO);
    SizeT nEl = p0->N_Elements();
    if (nEl == 1) {
      (*res)[0] = cos((*p0C)[0]);
      return res;
    }
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = cos((*p0C)[ i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = cos((*p0C)[ i]);
    }
    return res;
  }

  template< typename T>
  BaseGDL* cos_fun_template_grab(BaseGDL* p0) {
    T* p0C = static_cast<T*> (p0);
    SizeT nEl = p0->N_Elements();
    if (nEl == 1) {
      (*p0C)[0] = cos((*p0C)[0]);
      return p0;
    }
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (SizeT i = 0; i < nEl; ++i) (*p0C)[ i] = cos((*p0C)[ i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (SizeT i = 0; i < nEl; ++i) (*p0C)[ i] = cos((*p0C)[ i]);
    }
    return p0;
  }

  BaseGDL* cos_fun(BaseGDL* p0, bool isReference) {
    SizeT nEl = p0->N_Elements();
    DType p0Type = p0->Type();
    if (p0Type == GDL_UNDEF) throw GDLException("Variable is undefined: !NULL");
    if (p0Type == GDL_COMPLEX)
      if (isReference) return cos_fun_template< DComplexGDL>(p0);
      else return cos_fun_template_grab< DComplexGDL>(p0);
    else if (p0Type == GDL_COMPLEXDBL)
      if (isReference) return cos_fun_template< DComplexDblGDL>(p0);
      else return cos_fun_template_grab< DComplexDblGDL>(p0);
    else if (p0Type == GDL_DOUBLE)
      if (isReference) return cos_fun_template< DDoubleGDL>(p0);
      else return cos_fun_template_grab< DDoubleGDL>(p0);
    else if (p0Type == GDL_FLOAT)
      if (isReference) return cos_fun_template< DFloatGDL>(p0);
      else return cos_fun_template_grab< DFloatGDL>(p0);
    else {
      DFloatGDL* res = static_cast<DFloatGDL*> (p0->Convert2(GDL_FLOAT, BaseGDL::COPY));
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (SizeT i = 0; i < nEl; ++i)(*res)[ i] = cos((*res)[ i]);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (SizeT i = 0; i < nEl; ++i)(*res)[ i] = cos((*res)[ i]);
      }
      return res;
    }
  }

  //TAN

  template< typename T>
  BaseGDL* tan_fun_template(BaseGDL* p0) {
    T* p0C = static_cast<T*> (p0);
    T* res = new T(p0C->Dim(), BaseGDL::NOZERO);
    SizeT nEl = p0->N_Elements();
    if (nEl == 1) {
      (*res)[0] = tan((*p0C)[0]);
      return res;
    }
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = tan((*p0C)[ i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = tan((*p0C)[ i]);
    }
    return res;
  }

  template< typename T>
  BaseGDL* tan_fun_template_grab(BaseGDL* p0) {
    T* p0C = static_cast<T*> (p0);
    SizeT nEl = p0->N_Elements();
    if (nEl == 1) {
      (*p0C)[0] = tan((*p0C)[0]);
      return p0;
    }
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (SizeT i = 0; i < nEl; ++i) (*p0C)[ i] = tan((*p0C)[ i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (SizeT i = 0; i < nEl; ++i) (*p0C)[ i] = tan((*p0C)[ i]);
    }
    return p0;
  }

  BaseGDL* tan_fun(BaseGDL* p0, bool isReference) {
    SizeT nEl = p0->N_Elements();
    DType p0Type = p0->Type();
    if (p0Type == GDL_UNDEF) throw GDLException("Variable is undefined: !NULL");
    if (p0Type == GDL_COMPLEX)
      if (isReference) return tan_fun_template< DComplexGDL>(p0);
      else return tan_fun_template_grab< DComplexGDL>(p0);
    else if (p0Type == GDL_COMPLEXDBL)
      if (isReference) return tan_fun_template< DComplexDblGDL>(p0);
      else return tan_fun_template_grab< DComplexDblGDL>(p0);
    else if (p0Type == GDL_DOUBLE)
      if (isReference) return tan_fun_template< DDoubleGDL>(p0);
      else return tan_fun_template_grab< DDoubleGDL>(p0);
    else if (p0Type == GDL_FLOAT)
      if (isReference) return tan_fun_template< DFloatGDL>(p0);
      else return tan_fun_template_grab< DFloatGDL>(p0);
    else {
      DFloatGDL* res = static_cast<DFloatGDL*> (p0->Convert2(GDL_FLOAT, BaseGDL::COPY));
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (SizeT i = 0; i < nEl; ++i)(*res)[ i] = tan((*res)[ i]);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (SizeT i = 0; i < nEl; ++i)(*res)[ i] = tan((*res)[ i]);
      }
      return res;
    }
  }

  //SINH

  template< typename T>
  BaseGDL* sinh_fun_template(BaseGDL* p0) {
    T* p0C = static_cast<T*> (p0);
    T* res = new T(p0C->Dim(), BaseGDL::NOZERO);
    SizeT nEl = p0->N_Elements();
    if (nEl == 1) {
      (*res)[0] = sinh((*p0C)[0]);
      return res;
    }
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = sinh((*p0C)[ i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = sinh((*p0C)[ i]);
    }
    return res;
  }

  template< typename T>
  BaseGDL* sinh_fun_template_grab(BaseGDL* p0) {
    T* p0C = static_cast<T*> (p0);
    SizeT nEl = p0->N_Elements();
    if (nEl == 1) {
      (*p0C)[0] = sinh((*p0C)[0]);
      return p0;
    }
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (SizeT i = 0; i < nEl; ++i) (*p0C)[ i] = sinh((*p0C)[ i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (SizeT i = 0; i < nEl; ++i) (*p0C)[ i] = sinh((*p0C)[ i]);
    }
    return p0;
  }

  BaseGDL* sinh_fun(BaseGDL* p0, bool isReference) {
    SizeT nEl = p0->N_Elements();
    DType p0Type = p0->Type();
    if (p0Type == GDL_UNDEF) throw GDLException("Variable is undefined: !NULL");
    if (p0Type == GDL_COMPLEX)
      if (isReference) return sinh_fun_template< DComplexGDL>(p0);
      else return sinh_fun_template_grab< DComplexGDL>(p0);
    else if (p0Type == GDL_COMPLEXDBL)
      if (isReference) return sinh_fun_template< DComplexDblGDL>(p0);
      else return sinh_fun_template_grab< DComplexDblGDL>(p0);
    else if (p0Type == GDL_DOUBLE)
      if (isReference) return sinh_fun_template< DDoubleGDL>(p0);
      else return sinh_fun_template_grab< DDoubleGDL>(p0);
    else if (p0Type == GDL_FLOAT)
      if (isReference) return sinh_fun_template< DFloatGDL>(p0);
      else return sinh_fun_template_grab< DFloatGDL>(p0);
    else {
      DFloatGDL* res = static_cast<DFloatGDL*> (p0->Convert2(GDL_FLOAT, BaseGDL::COPY));
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (SizeT i = 0; i < nEl; ++i)(*res)[ i] = sinh((*res)[ i]);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (SizeT i = 0; i < nEl; ++i)(*res)[ i] = sinh((*res)[ i]);
      }
      return res;
    }
  }

  //COSH

  template< typename T>
  BaseGDL* cosh_fun_template(BaseGDL* p0) {
    T* p0C = static_cast<T*> (p0);
    T* res = new T(p0C->Dim(), BaseGDL::NOZERO);
    SizeT nEl = p0->N_Elements();
    if (nEl == 1) {
      (*res)[0] = cosh((*p0C)[0]);
      return res;
    }
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = cosh((*p0C)[ i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = cosh((*p0C)[ i]);
    }
    return res;
  }

  template< typename T>
  BaseGDL* cosh_fun_template_grab(BaseGDL* p0) {
    T* p0C = static_cast<T*> (p0);
    SizeT nEl = p0->N_Elements();
    if (nEl == 1) {
      (*p0C)[0] = cosh((*p0C)[0]);
      return p0;
    }
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (SizeT i = 0; i < nEl; ++i) (*p0C)[ i] = cosh((*p0C)[ i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (SizeT i = 0; i < nEl; ++i) (*p0C)[ i] = cosh((*p0C)[ i]);
    }
    return p0;
  }

  BaseGDL* cosh_fun(BaseGDL* p0, bool isReference) {
    SizeT nEl = p0->N_Elements();
    DType p0Type = p0->Type();
    if (p0Type == GDL_UNDEF) throw GDLException("Variable is undefined: !NULL");
    if (p0Type == GDL_COMPLEX)
      if (isReference) return cosh_fun_template< DComplexGDL>(p0);
      else return cosh_fun_template_grab< DComplexGDL>(p0);
    else if (p0Type == GDL_COMPLEXDBL)
      if (isReference) return cosh_fun_template< DComplexDblGDL>(p0);
      else return cosh_fun_template_grab< DComplexDblGDL>(p0);
    else if (p0Type == GDL_DOUBLE)
      if (isReference) return cosh_fun_template< DDoubleGDL>(p0);
      else return cosh_fun_template_grab< DDoubleGDL>(p0);
    else if (p0Type == GDL_FLOAT)
      if (isReference) return cosh_fun_template< DFloatGDL>(p0);
      else return cosh_fun_template_grab< DFloatGDL>(p0);
    else {
      DFloatGDL* res = static_cast<DFloatGDL*> (p0->Convert2(GDL_FLOAT, BaseGDL::COPY));
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (SizeT i = 0; i < nEl; ++i)(*res)[ i] = cosh((*res)[ i]);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (SizeT i = 0; i < nEl; ++i)(*res)[ i] = cosh((*res)[ i]);
      }
      return res;
    }
  }

  //TANH

  template< typename T>
  BaseGDL* tanh_fun_template(BaseGDL* p0) {
    T* p0C = static_cast<T*> (p0);
    T* res = new T(p0C->Dim(), BaseGDL::NOZERO);
    SizeT nEl = p0->N_Elements();
    if (nEl == 1) {
      (*res)[0] = tanh((*p0C)[0]);
      return res;
    }
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = tanh((*p0C)[ i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = tanh((*p0C)[ i]);
    }
    return res;
  }

  template< typename T>
  BaseGDL* tanh_fun_template_grab(BaseGDL* p0) {
    T* p0C = static_cast<T*> (p0);
    SizeT nEl = p0->N_Elements();
    if (nEl == 1) {
      (*p0C)[0] = tanh((*p0C)[0]);
      return p0;
    }
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (SizeT i = 0; i < nEl; ++i) (*p0C)[ i] = tanh((*p0C)[ i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (SizeT i = 0; i < nEl; ++i) (*p0C)[ i] = tanh((*p0C)[ i]);
    }
    return p0;
  }

  BaseGDL* tanh_fun(BaseGDL* p0, bool isReference) {
    SizeT nEl = p0->N_Elements();
    DType p0Type = p0->Type();
    if (p0Type == GDL_UNDEF) throw GDLException("Variable is undefined: !NULL");
    if (p0Type == GDL_COMPLEX)
      if (isReference) return tanh_fun_template< DComplexGDL>(p0);
      else return tanh_fun_template_grab< DComplexGDL>(p0);
    else if (p0Type == GDL_COMPLEXDBL)
      if (isReference) return tanh_fun_template< DComplexDblGDL>(p0);
      else return tanh_fun_template_grab< DComplexDblGDL>(p0);
    else if (p0Type == GDL_DOUBLE)
      if (isReference) return tanh_fun_template< DDoubleGDL>(p0);
      else return tanh_fun_template_grab< DDoubleGDL>(p0);
    else if (p0Type == GDL_FLOAT)
      if (isReference) return tanh_fun_template< DFloatGDL>(p0);
      else return tanh_fun_template_grab< DFloatGDL>(p0);
    else {
      DFloatGDL* res = static_cast<DFloatGDL*> (p0->Convert2(GDL_FLOAT, BaseGDL::COPY));
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (SizeT i = 0; i < nEl; ++i)(*res)[ i] = tanh((*res)[ i]);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (SizeT i = 0; i < nEl; ++i)(*res)[ i] = tanh((*res)[ i]);
      }
      return res;
    }
  }

  template< typename T>
  BaseGDL* asin_fun_template(BaseGDL* p0) {
    T* p0C = static_cast<T*> (p0);
    T* res = new T(p0C->Dim(), BaseGDL::NOZERO);
    SizeT nEl = p0->N_Elements();
    if (nEl == 1) {
      (*res)[0] = asin((*p0C)[0]);
      return res;
    }
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = asin((*p0C)[ i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = asin((*p0C)[ i]);
    }
    return res;
  }

  template< typename T>
  BaseGDL* asin_fun_template_grab(BaseGDL* p0) {
    T* p0C = static_cast<T*> (p0);
    SizeT nEl = p0->N_Elements();
    if (nEl == 1) {
      (*p0C)[0] = asin((*p0C)[0]);
      return p0;
    }
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (SizeT i = 0; i < nEl; ++i) (*p0C)[ i] = asin((*p0C)[ i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (SizeT i = 0; i < nEl; ++i) (*p0C)[ i] = asin((*p0C)[ i]);
    }
    return p0;
  }

  BaseGDL* asin_fun(BaseGDL* p0, bool isReference) {
    SizeT nEl = p0->N_Elements();
    DType p0Type = p0->Type();
    if (p0Type == GDL_UNDEF) throw GDLException("Variable is undefined: !NULL");
    if (p0Type == GDL_COMPLEX)
      if (isReference) return asin_fun_template< DComplexGDL>(p0);
      else return asin_fun_template_grab< DComplexGDL>(p0);
    else if (p0Type == GDL_COMPLEXDBL)
      if (isReference) return asin_fun_template< DComplexDblGDL>(p0);
      else return asin_fun_template_grab< DComplexDblGDL>(p0);
    else if (p0Type == GDL_DOUBLE)
      if (isReference) return asin_fun_template< DDoubleGDL>(p0);
      else return asin_fun_template_grab< DDoubleGDL>(p0);
    else if (p0Type == GDL_FLOAT)
      if (isReference) return asin_fun_template< DFloatGDL>(p0);
      else return asin_fun_template_grab< DFloatGDL>(p0);
    else {
      DFloatGDL* res = static_cast<DFloatGDL*> (p0->Convert2(GDL_FLOAT, BaseGDL::COPY));
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (SizeT i = 0; i < nEl; ++i)(*res)[ i] = asin((*res)[ i]);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (SizeT i = 0; i < nEl; ++i)(*res)[ i] = asin((*res)[ i]);
      }
      return res;
    }
  }

  template< typename T>
  BaseGDL* acos_fun_template(BaseGDL* p0) {
    T* p0C = static_cast<T*> (p0);
    T* res = new T(p0C->Dim(), BaseGDL::NOZERO);
    SizeT nEl = p0->N_Elements();
    if (nEl == 1) {
      (*res)[0] = acos((*p0C)[0]);
      return res;
    }
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = acos((*p0C)[ i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = acos((*p0C)[ i]);
    }
    return res;
  }

  template< typename T>
  BaseGDL* acos_fun_template_grab(BaseGDL* p0) {
    T* p0C = static_cast<T*> (p0);
    SizeT nEl = p0->N_Elements();
    if (nEl == 1) {
      (*p0C)[0] = acos((*p0C)[0]);
      return p0;
    }
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (SizeT i = 0; i < nEl; ++i) (*p0C)[ i] = acos((*p0C)[ i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (SizeT i = 0; i < nEl; ++i) (*p0C)[ i] = acos((*p0C)[ i]);
    }
    return p0;
  }

  BaseGDL* acos_fun(BaseGDL* p0, bool isReference) {
    SizeT nEl = p0->N_Elements();
    DType p0Type = p0->Type();
    if (p0Type == GDL_UNDEF) throw GDLException("Variable is undefined: !NULL");
    if (p0Type == GDL_COMPLEX)
      if (isReference) return acos_fun_template< DComplexGDL>(p0);
      else return acos_fun_template_grab< DComplexGDL>(p0);
    else if (p0Type == GDL_COMPLEXDBL)
      if (isReference) return acos_fun_template< DComplexDblGDL>(p0);
      else return acos_fun_template_grab< DComplexDblGDL>(p0);
    else if (p0Type == GDL_DOUBLE)
      if (isReference) return acos_fun_template< DDoubleGDL>(p0);
      else return acos_fun_template_grab< DDoubleGDL>(p0);
    else if (p0Type == GDL_FLOAT)
      if (isReference) return acos_fun_template< DFloatGDL>(p0);
      else return acos_fun_template_grab< DFloatGDL>(p0);
    else {
      DFloatGDL* res = static_cast<DFloatGDL*> (p0->Convert2(GDL_FLOAT, BaseGDL::COPY));
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (SizeT i = 0; i < nEl; ++i)(*res)[ i] = acos((*res)[ i]);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (SizeT i = 0; i < nEl; ++i)(*res)[ i] = acos((*res)[ i]);
      }
      return res;
    }
  }

  //EXP

  template< typename T>
  BaseGDL* exp_fun_template(BaseGDL* p0) {
    T* p0C = static_cast<T*> (p0);
    T* res = new T(p0C->Dim(), BaseGDL::NOZERO);
    SizeT nEl = p0->N_Elements();
    if (nEl == 1) {
      (*res)[0] = exp((*p0C)[0]);
      return res;
    }
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = exp((*p0C)[ i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = exp((*p0C)[ i]);
    }
    return res;
  }

  template< typename T>
  BaseGDL* exp_fun_template_grab(BaseGDL* p0) {
    T* p0C = static_cast<T*> (p0);
    SizeT nEl = p0->N_Elements();
    if (nEl == 1) {
      (*p0C)[0] = exp((*p0C)[0]);
      return p0;
    }
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (SizeT i = 0; i < nEl; ++i) (*p0C)[ i] = exp((*p0C)[ i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (SizeT i = 0; i < nEl; ++i) (*p0C)[ i] = exp((*p0C)[ i]);
    }
    return p0;
  }

  BaseGDL* exp_fun(BaseGDL* p0, bool isReference) {
    SizeT nEl = p0->N_Elements();
    DType p0Type = p0->Type();
    if (p0Type == GDL_UNDEF) throw GDLException("Variable is undefined: !NULL");
    if (p0Type == GDL_COMPLEX)
      if (isReference) return exp_fun_template< DComplexGDL>(p0);
      else return exp_fun_template_grab< DComplexGDL>(p0);
    else if (p0Type == GDL_COMPLEXDBL)
      if (isReference) return exp_fun_template< DComplexDblGDL>(p0);
      else return exp_fun_template_grab< DComplexDblGDL>(p0);
    else if (p0Type == GDL_DOUBLE)
      if (isReference) return exp_fun_template< DDoubleGDL>(p0);
      else return exp_fun_template_grab< DDoubleGDL>(p0);
    else if (p0Type == GDL_FLOAT)
      if (isReference) return exp_fun_template< DFloatGDL>(p0);
      else return exp_fun_template_grab< DFloatGDL>(p0);
    else {
      DFloatGDL* res = static_cast<DFloatGDL*> (p0->Convert2(GDL_FLOAT, BaseGDL::COPY));
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (SizeT i = 0; i < nEl; ++i)(*res)[ i] = exp((*res)[ i]);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (SizeT i = 0; i < nEl; ++i)(*res)[ i] = exp((*res)[ i]);
      }
      return res;
    }
  }
  
  template< typename T>
  BaseGDL* signum_fun_template(BaseGDL* p0) {
    T* p0C = static_cast<T*> (p0);
    T* res = new T(p0C->Dim(), BaseGDL::NOZERO);
    SizeT nEl = p0->N_Elements();
    if (nEl == 1) {
      (*res)[0] = ((*p0C)[0] == 0)?0:signbit((*p0C)[0]);
      return res;
    }
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = ((*p0C)[i] == 0)?0:signbit((*p0C)[ i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = ((*p0C)[i] == 0)?0:signbit((*p0C)[ i]);
    }
    return res;
  }

  template< typename T>
  BaseGDL* signum_fun_template_grab(BaseGDL* p0) {
    T* p0C = static_cast<T*> (p0);
    SizeT nEl = p0->N_Elements();
    if (nEl == 1) {
      (*p0C)[0] = ((*p0C)[0] == 0)?0:signbit((*p0C)[0]);
      return p0;
    }
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (SizeT i = 0; i < nEl; ++i) (*p0C)[ i] = ((*p0C)[i] == 0)?0:signbit((*p0C)[ i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (SizeT i = 0; i < nEl; ++i) (*p0C)[ i] = ((*p0C)[i] == 0)?0:signbit((*p0C)[ i]);
    }
    return p0;
  }

   BaseGDL* signum_fun(BaseGDL* p0, bool isReference) {
    SizeT nEl = p0->N_Elements();
    DType p0Type = p0->Type();
    if (!ConvertableType(p0Type)) throw GDLException("operation illegal with "+p0->TypeStr());
     if (p0Type == GDL_FLOAT)
      if (isReference) return signum_fun_template< DFloatGDL>(p0);
      else return signum_fun_template_grab< DFloatGDL>(p0);
     else if (p0Type == GDL_BYTE)
      if (isReference) return signum_fun_template< DByteGDL>(p0);
      else return signum_fun_template_grab< DByteGDL>(p0);
     else if (p0Type == GDL_INT)
      if (isReference) return signum_fun_template< DIntGDL>(p0);
      else return signum_fun_template_grab< DIntGDL>(p0);
     else if (p0Type == GDL_UINT)
      if (isReference) return signum_fun_template< DUIntGDL>(p0);
      else return signum_fun_template_grab< DUIntGDL>(p0);
     else if (p0Type == GDL_LONG)
      if (isReference) return signum_fun_template< DLongGDL>(p0);
      else return signum_fun_template_grab< DLongGDL>(p0);
     else if (p0Type == GDL_ULONG)
      if (isReference) return signum_fun_template< DULongGDL>(p0);
      else return signum_fun_template_grab< DULongGDL>(p0);
     else if (p0Type == GDL_LONG64)
      if (isReference) return signum_fun_template< DLong64GDL>(p0);
      else return signum_fun_template_grab< DLong64GDL>(p0);
     else if (p0Type == GDL_ULONG64)
      if (isReference) return signum_fun_template< DULong64GDL>(p0);
      else return signum_fun_template_grab< DULong64GDL>(p0);
     else if (p0Type == GDL_STRING) {
         BaseGDL* temp=p0->Convert2(GDL_FLOAT, BaseGDL::COPY);
         Guard<BaseGDL>g;
         g.reset(temp);
         return signum_fun_template< DFloatGDL>(temp); //NOTE: no GRAB
     }
     else throw GDLException("forgotten "+p0->TypeStr()+" type in GDL code for SIGNUM, please report!");
  }
   
   //   BaseGDL* alog_fun( EnvT* e)

  BaseGDL* alog_fun(BaseGDL* p0, bool isReference) {
    if (p0->Type() == GDL_UNDEF) throw GDLException("Variable is undefined: !NULL");

    if (!isReference) //e->StealLocalPar( 0))
    {
      return p0->LogThis();
    }
    return p0->Log();
  }

  //   BaseGDL* alog10_fun( EnvT* e)

  BaseGDL* alog10_fun(BaseGDL* p0, bool isReference) {
    if (p0->Type() == GDL_UNDEF) throw GDLException("Variable is undefined: !NULL");

    if (!isReference) //e->StealLocalPar( 0))
    {
      return p0->Log10This();
    }
    return p0->Log10();
  }

  //   BaseGDL* alog2_fun( EnvT* e)
  //very quick and dirty.
  BaseGDL* alog2_fun(BaseGDL* p0, bool isReference) {
    static const double logue2=log(2.);
    if (p0->Type() == GDL_UNDEF) throw GDLException("Variable is undefined: !NULL");
    BaseGDL* ret;
    if (!isReference) //e->StealLocalPar( 0))
    {
      ret=p0->LogThis();
    } else  ret=p0->Log();
    DFloatGDL* val = static_cast<DFloatGDL*> (ret->Convert2(GDL_FLOAT, BaseGDL::COPY));
    for (SizeT i=0; i< p0->N_Elements(); ++i) (*val)[i] /= logue2;
    return val;
  }

  //Following produce Float or doubles for complex.

  template< typename T>
  BaseGDL* abs_fun_template(BaseGDL* p0) {
    T* p0C = static_cast<T*> (p0);
    T* res = new T(p0C->Dim(), BaseGDL::NOZERO);
    SizeT nEl = p0->N_Elements();
    if (nEl == 1) {
      (*res)[ 0] = abs((*p0C)[ 0]);
      return res;
    }
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = abs((*p0C)[ i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = abs((*p0C)[ i]);
    }
    return res;
  }

  BaseGDL* abs_fun(BaseGDL* p0, bool isReference) {
    if (p0->Type() == GDL_UNDEF) throw GDLException("Variable is undefined: !NULL");
    if (p0->Type() == GDL_COMPLEX) {
      DComplexGDL* p0C = static_cast<DComplexGDL*> (p0);
      DFloatGDL* res = new DFloatGDL(p0C->Dim(), BaseGDL::NOZERO);
      SizeT nEl = p0->N_Elements();
      if (nEl == 1) {
        (*res)[ 0] = abs((*p0C)[ 0]);
        return res;
      }
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = abs((*p0C)[ i]); //sqrt(Creal*Creal + Cimag*Cimag);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = abs((*p0C)[ i]); //sqrt(Creal*Creal + Cimag*Cimag);
      }
      return res;
    } else if (p0->Type() == GDL_COMPLEXDBL) {
      DComplexDblGDL* p0C = static_cast<DComplexDblGDL*> (p0);
      DDoubleGDL* res = new DDoubleGDL(p0C->Dim(), BaseGDL::NOZERO);
      SizeT nEl = p0->N_Elements();
      if (nEl == 1) {
        (*res)[ 0] = abs((*p0C)[ 0]);
        return res;
      }
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = abs((*p0C)[ i]); //sqrt(Creal*Creal + Cimag*Cimag);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = abs((*p0C)[ i]); //sqrt(Creal*Creal + Cimag*Cimag);
      }
      return res;
    } else if (p0->Type() == GDL_DOUBLE)
      return abs_fun_template< DDoubleGDL>(p0);
    else if (p0->Type() == GDL_FLOAT)
      return abs_fun_template< DFloatGDL>(p0);
    else if (p0->Type() == GDL_LONG64)
      return abs_fun_template< DLong64GDL>(p0);
    else if (p0->Type() == GDL_LONG)
      return abs_fun_template< DLongGDL>(p0);
    else if (p0->Type() == GDL_INT)
      return abs_fun_template< DIntGDL>(p0);
    else if (isReference) {
      if (p0->Type() == GDL_ULONG64)
        return p0->Dup();
      else if (p0->Type() == GDL_ULONG)
        return p0->Dup();
      else if (p0->Type() == GDL_UINT)
        return p0->Dup();
      else if (p0->Type() == GDL_BYTE)
        return p0->Dup();
    } else {
      if (p0->Type() == GDL_ULONG64)
        return p0;
      else if (p0->Type() == GDL_ULONG)
        return p0;
      else if (p0->Type() == GDL_UINT)
        return p0;
      else if (p0->Type() == GDL_BYTE)
        return p0;
    }
    DFloatGDL* res = static_cast<DFloatGDL*>
      (p0->Convert2(GDL_FLOAT, BaseGDL::COPY));
    SizeT nEl = p0->N_Elements();
    if (nEl == 1) {
      (*res)[ 0] = abs((*res)[ 0]);
      return res;
    }
    if ((GDL_NTHREADS=parallelize( nEl))==1) {
      for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = abs((*res)[ i]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = abs((*res)[ i]);
    }
    return res;
  }

  //COMPLEX SPECIALS

  BaseGDL* conj_fun(BaseGDL* p0, bool isReference)//( EnvT* e)
  {
    SizeT nEl = p0->N_Elements();

    if (p0->Type() == GDL_UNDEF) throw GDLException("Variable is undefined: !NULL");

    if (p0->Type() == GDL_COMPLEX) {
      DComplexGDL* res;
      if (isReference) res = static_cast<DComplexGDL*> (p0)->NewResult();
      else res = static_cast<DComplexGDL*> (p0);
      DComplexGDL* p0C = static_cast<DComplexGDL*> (p0);
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (SizeT i = 0; i < nEl; ++i) (*res)[i] = std::conj((*p0C)[i]);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (SizeT i = 0; i < nEl; ++i) (*res)[i] = std::conj((*p0C)[i]);
      }
      return res;
    }
    if (p0->Type() == GDL_COMPLEXDBL) {
      DComplexDblGDL* res;
      if (isReference) res = static_cast<DComplexDblGDL*> (p0)->NewResult();
      else res = static_cast<DComplexDblGDL*> (p0);
      DComplexDblGDL* p0C = static_cast<DComplexDblGDL*> (p0);
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (SizeT i = 0; i < nEl; ++i) (*res)[i] = std::conj((*p0C)[i]);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (SizeT i = 0; i < nEl; ++i) (*res)[i] = std::conj((*p0C)[i]);
      }
      return res;
    }
    if (p0->Type() == GDL_DOUBLE ||
      p0->Type() == GDL_LONG64 ||
      p0->Type() == GDL_ULONG64) {
      DComplexDblGDL* res = static_cast<DComplexDblGDL*> (p0->Convert2(GDL_COMPLEXDBL, BaseGDL::COPY));
      return res;
    }

    // all other types
    return static_cast<DComplexGDL*> (p0->Convert2(GDL_COMPLEX, BaseGDL::COPY));
  }
  //returns Double or floats

  BaseGDL* imaginary_fun(BaseGDL* p0, bool isReference)//( EnvT* e)
  {
    SizeT nEl = p0->N_Elements();
    if (p0->Type() == GDL_UNDEF) throw GDLException("Variable is undefined: !NULL");
    // complex types, return imaginary part
    if (p0->Type() == GDL_COMPLEX) {
      DComplexGDL* c0 = static_cast<DComplexGDL*> (p0);
      DFloatGDL* res = new DFloatGDL(c0->Dim(), BaseGDL::NOZERO);
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (SizeT i = 0; i < nEl; ++i) (*res)[i] = imag((*c0)[i]);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (SizeT i = 0; i < nEl; ++i) (*res)[i] = imag((*c0)[i]);
      }
      return res;
    }
    if (p0->Type() == GDL_COMPLEXDBL) {
      DComplexDblGDL* c0 = static_cast<DComplexDblGDL*> (p0);
      DDoubleGDL* res = new DDoubleGDL(c0->Dim(), BaseGDL::NOZERO);
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (SizeT i = 0; i < nEl; ++i) (*res)[i] = imag((*c0)[i]);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (SizeT i = 0; i < nEl; ++i) (*res)[i] = imag((*c0)[i]);
      }
      return res;
    }

    // forbidden types
    DType t = p0->Type();
    if (t == GDL_STRING)
      throw GDLException("String expression not allowed in this context.");
    if (t == GDL_STRUCT)
      throw GDLException("Struct expression not allowed in this context.");
    if (t == GDL_PTR)
      throw GDLException("Pointer expression not allowed in this context.");
    if (t == GDL_OBJ)
      throw GDLException("Object reference not allowed in this context.");

    // all other types (return array of zeros)
    return new DFloatGDL(p0->Dim(), BaseGDL::ZERO); // ZERO
  }

  BaseGDL* real_part_fun(BaseGDL* p0, bool isReference) {
    SizeT nEl = p0->N_Elements();
    if (p0->Type() == GDL_UNDEF) throw GDLException("Variable is undefined: !NULL");
    // complex types, return real part
    if (p0->Type() == GDL_COMPLEX) {
      DComplexGDL* c0 = static_cast<DComplexGDL*> (p0);
      DFloatGDL* res = new DFloatGDL(c0->Dim(), BaseGDL::NOZERO);
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (SizeT i = 0; i < nEl; ++i) (*res)[i] = real((*c0)[i]);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (SizeT i = 0; i < nEl; ++i) (*res)[i] = real((*c0)[i]);
      }
      return res;
    }
    if (p0->Type() == GDL_COMPLEXDBL) {
      DComplexDblGDL* c0 = static_cast<DComplexDblGDL*> (p0);
      DDoubleGDL* res = new DDoubleGDL(c0->Dim(), BaseGDL::NOZERO);
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (SizeT i = 0; i < nEl; ++i) (*res)[i] = real((*c0)[i]);
      } else {
        TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
          for (SizeT i = 0; i < nEl; ++i) (*res)[i] = real((*c0)[i]);
      }
      return res;
    }

    DType t = p0->Type();
    // avoid forbidden types
    if (t == GDL_STRUCT)
      throw GDLException("Struct expression not allowed in this context.");
    if (t == GDL_PTR)
      throw GDLException("Pointer expression not allowed in this context.");
    if (t == GDL_OBJ)
      throw GDLException("Object reference not allowed in this context.");
    // Doubles to double, copy
    if (t == GDL_DOUBLE) {
      if (isReference) return p0->Dup();
      else return p0;
    }
    // Floats to float, copy
    if (t == GDL_FLOAT) {
      if (isReference) return p0->Dup();
      else return p0;
    }
    // all other types to float
    return static_cast<DFloatGDL*> (p0->Convert2(GDL_FLOAT, BaseGDL::COPY));
  }


  //atan() is different as complex is different. Note that atan(complex(0.888,0)) does not give exactly 0 for its imaginary part, as IDL does.

  // atan() for complex
  // .. is now in C++11

  //  template< typename C>
  //  inline C atanC(const C& c)
  //  {
  //    //     double x = c.real();
  //    //     double x2 = x * x;
  //    //     double y = c.imag();
  //    //     return C(0.5 * atan2(2.0*x, 1.0 - x2 - y*y), 0.25 * log( (x2 + (y+1)*(y+1)) / (x2 + (y-1)*(y-1)) ));
  //    const C i(0.0, 1.0);
  //    const C one(1.0, 0.0);
  //    return log((one + i * c) / (one - i * c)) / (C(2.0, 0.0) * i);
  //  }

  template< typename C>
  inline C atanC(const C& c1, const C& c2) {
    const C i(0.0, 1.0);
    //const C one(1.0,0.0);
    //     return -i * log((c2 + i * c1) / (sqrt(pow(c2, 2) + pow(c1, 2))));
    return -i * log((c2 + i * c1) / sqrt((c2 * c2) + (c1 * c1)));
  }

  BaseGDL* atan_fun(EnvT* e) {
    //lots of different guards defined here to insure they do their work only at exit of atan_fun.
    Guard< DDoubleGDL> guardp0D;
    Guard< DDoubleGDL> guardp1D;
    Guard< DFloatGDL> guardp0F;
    Guard< DFloatGDL> guardp1F;
    Guard< DComplexDblGDL> guardp0DC;
    Guard< DComplexDblGDL> guardp1DC;
    Guard< DComplexGDL> guardp0C;
    Guard< DComplexGDL> guardp1C;

    SizeT nParam = e->NParam(1); //, "ATAN");

    BaseGDL* p0 = e->GetParDefined(0); //, "ATAN");

    SizeT nEl = p0->N_Elements();
    if (nEl == 0)
      e->Throw(
      "Variable is undefined: " + e->GetParString(0));

    if (nParam == 2) {
      BaseGDL* p1 = e->GetPar(1);
      if (p1 == NULL)
        e->Throw(
        "Variable is undefined: " + e->GetParString(1));
      SizeT nEl1 = p1->N_Elements();
      if (nEl1 == 0)
        e->Throw(
        "Variable is undefined: " + e->GetParString(1));

      DType t = (DTypeOrder[ p0->Type()] > DTypeOrder[ p1->Type()]) ? p0->Type() : p1->Type();

      // WRT. the previous version, written to insure that zero-dimension values are taken as of size as long as the other argument,
      // we hopefully keep the same behaviour but permit the speedup of parallelism by using local loop variables.
      dimension dim;
      int cas = 0;
      SizeT nElMin;
      if ((p0->Rank() == 0 && p1->Rank() == 0) || (p0->Rank() != 0 && p1->Rank() != 0)) {
        cas = 0;
        dim = (nEl1 > nEl) ? p0->Dim() : p1->Dim();
        nElMin = (nEl1 > nEl) ? nEl : nEl1;
      } else if (p0->Rank() != 0 && p1->Rank() == 0) {
        cas = 1;
        dim = p0->Dim();
        nElMin = nEl;
      } else if (p0->Rank() == 0 && p1->Rank() != 0) {
        cas = 2;
        dim = p1->Dim();
        nElMin = nEl1;
      }

      if (t == GDL_DOUBLE) {
        DDoubleGDL* p0D;
        DDoubleGDL* p1D;
        if (p0->Type() == GDL_DOUBLE) p0D = static_cast<DDoubleGDL*> (p0);
        else {
          p0D = static_cast<DDoubleGDL*> (p0->Convert2(GDL_DOUBLE, BaseGDL::COPY));
          guardp0D.Reset(p0D);
        }
        if (p1->Type() == GDL_DOUBLE) p1D = static_cast<DDoubleGDL*> (p1);
        else {
          p1D = static_cast<DDoubleGDL*> (p1->Convert2(GDL_DOUBLE, BaseGDL::COPY));
          guardp1D.Reset(p1D);
        }
        DDoubleGDL* res = new DDoubleGDL(dim, BaseGDL::NOZERO);
        if (nElMin == 1) {
          (*res)[ 0] = atan2((*p0D)[0], (*p1D)[0]);
          return res;
        }
        switch (cas) {
        case 0:
          if ((GDL_NTHREADS=parallelize( nElMin))==1) {
            for (SizeT i = 0; i < nElMin; ++i) (*res)[i] = atan2((*p0D)[i], (*p1D)[i]);
          } else {
            TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
              for (SizeT i = 0; i < nElMin; ++i) (*res)[i] = atan2((*p0D)[i], (*p1D)[i]);
          }
          return res;
        case 1:
          if ((GDL_NTHREADS=parallelize( nElMin))==1) {
            for (SizeT i = 0; i < nElMin; ++i) (*res)[i] = atan2((*p0D)[i], (*p1D)[0]);
          } else {
            TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
              for (SizeT i = 0; i < nElMin; ++i) (*res)[i] = atan2((*p0D)[i], (*p1D)[0]);
          }
          return res;
        case 2:
          if ((GDL_NTHREADS=parallelize( nElMin))==1) {
            for (SizeT i = 0; i < nElMin; ++i) (*res)[i] = atan2((*p0D)[0], (*p1D)[i]);
          } else {
            TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
              for (SizeT i = 0; i < nElMin; ++i) (*res)[i] = atan2((*p0D)[0], (*p1D)[i]);
          }
          return res;
        }
      } else if (t == GDL_FLOAT) {
        DFloatGDL* p0F;
        DFloatGDL* p1F;
        if (p0->Type() == GDL_FLOAT) p0F = static_cast<DFloatGDL*> (p0);
        else {
          p0F = static_cast<DFloatGDL*> (p0->Convert2(GDL_FLOAT, BaseGDL::COPY));
          guardp0F.Reset(p0F);
        }
        if (p1->Type() == GDL_FLOAT) p1F = static_cast<DFloatGDL*> (p1);
        else {
          p1F = static_cast<DFloatGDL*> (p1->Convert2(GDL_FLOAT, BaseGDL::COPY));
          guardp1F.Reset(p1F);
        }
        DFloatGDL* res = new DFloatGDL(dim, BaseGDL::NOZERO);
        if (nElMin == 1) {
          (*res)[ 0] = atan2((*p0F)[0], (*p1F)[0]);
          return res;
        }
        switch (cas) {
        case 0:
          if ((GDL_NTHREADS=parallelize( nElMin))==1) {
            for (SizeT i = 0; i < nElMin; ++i) (*res)[i] = atan2((*p0F)[i], (*p1F)[i]);
          } else {
            TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
              for (SizeT i = 0; i < nElMin; ++i) (*res)[i] = atan2((*p0F)[i], (*p1F)[i]);
          }
          return res;
        case 1:
          if ((GDL_NTHREADS=parallelize( nElMin))==1) {
            for (SizeT i = 0; i < nElMin; ++i) (*res)[i] = atan2((*p0F)[i], (*p1F)[0]);
          } else {
            TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
              for (SizeT i = 0; i < nElMin; ++i) (*res)[i] = atan2((*p0F)[i], (*p1F)[0]);
          }
          return res;
        case 2:
          if ((GDL_NTHREADS=parallelize( nElMin))==1) {
            for (SizeT i = 0; i < nElMin; ++i) (*res)[i] = atan2((*p0F)[0], (*p1F)[i]);
          } else {
            TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
              for (SizeT i = 0; i < nElMin; ++i) (*res)[i] = atan2((*p0F)[0], (*p1F)[i]);
          }
          return res;
        }
      } else if (t == GDL_COMPLEX) {
        DComplexGDL* p0C;
        DComplexGDL* p1C;
        if (p0->Type() == GDL_COMPLEX) p0C = static_cast<DComplexGDL*> (p0);
        else {
          p0C = static_cast<DComplexGDL*> (p0->Convert2(GDL_COMPLEX, BaseGDL::COPY));
          guardp0C.reset(p0C);
        }
        if (p1->Type() == GDL_COMPLEX) p1C = static_cast<DComplexGDL*> (p1);
        else {
          p1C = static_cast<DComplexGDL*> (p1->Convert2(GDL_COMPLEX, BaseGDL::COPY));
          guardp1C.Reset(p1C);
        }
        DComplexGDL* res = new DComplexGDL(dim, BaseGDL::NOZERO);
        if (nElMin == 1) {
          (*res)[ 0] = atanC((*p0C)[0], (*p1C)[0]);
          return res;
        }
        switch (cas) {
        case 0:
          if ((GDL_NTHREADS=parallelize( nElMin))==1) {
            for (SizeT i = 0; i < nElMin; ++i) (*res)[i] = atanC((*p0C)[i], (*p1C)[i]);
          } else {
            TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
              for (SizeT i = 0; i < nElMin; ++i) (*res)[i] = atanC((*p0C)[i], (*p1C)[i]);
          }
          return res;
        case 1:
          if ((GDL_NTHREADS=parallelize( nElMin))==1) {
            for (SizeT i = 0; i < nElMin; ++i) (*res)[i] = atanC((*p0C)[i], (*p1C)[0]);
          } else {
            TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
              for (SizeT i = 0; i < nElMin; ++i) (*res)[i] = atanC((*p0C)[i], (*p1C)[0]);
          }
          return res;
        case 2:
          if ((GDL_NTHREADS=parallelize( nElMin))==1) {
            for (SizeT i = 0; i < nElMin; ++i) (*res)[i] = atanC((*p0C)[0], (*p1C)[i]);
          } else {
            TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
              for (SizeT i = 0; i < nElMin; ++i) (*res)[i] = atanC((*p0C)[0], (*p1C)[i]);
          }
          return res;
        }
      } else if (t == GDL_COMPLEXDBL) {
        DComplexDblGDL* p0DC;
        DComplexDblGDL* p1DC;
        if (p0->Type() == GDL_COMPLEXDBL) p0DC = static_cast<DComplexDblGDL*> (p0);
        else {
          p0DC = static_cast<DComplexDblGDL*> (p0->Convert2(GDL_COMPLEXDBL, BaseGDL::COPY));
          guardp0DC.Reset(p0DC);
        }
        if (p1->Type() == GDL_COMPLEXDBL) p1DC = static_cast<DComplexDblGDL*> (p1);
        else {
          p1DC = static_cast<DComplexDblGDL*> (p1->Convert2(GDL_COMPLEXDBL, BaseGDL::COPY));
          guardp1DC.Reset(p1DC);
        }
        DComplexDblGDL* res = new DComplexDblGDL(dim, BaseGDL::NOZERO);
        if (nElMin == 1) {
          (*res)[ 0] = atanC((*p0DC)[0], (*p1DC)[0]);
          return res;
        }
        switch (cas) {
        case 0:
          if ((GDL_NTHREADS=parallelize( nElMin))==1) {
            for (SizeT i = 0; i < nElMin; ++i) (*res)[i] = atanC((*p0DC)[i], (*p1DC)[i]);
          } else {
            TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
              for (SizeT i = 0; i < nElMin; ++i) (*res)[i] = atanC((*p0DC)[i], (*p1DC)[i]);
          }
          return res;
        case 1:
          if ((GDL_NTHREADS=parallelize( nElMin))==1) {
            for (SizeT i = 0; i < nElMin; ++i) (*res)[i] = atanC((*p0DC)[i], (*p1DC)[0]);
          } else {
            TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
              for (SizeT i = 0; i < nElMin; ++i) (*res)[i] = atanC((*p0DC)[i], (*p1DC)[0]);
          }
          return res;
        case 2:
          if ((GDL_NTHREADS=parallelize( nElMin))==1) {
            for (SizeT i = 0; i < nElMin; ++i) (*res)[i] = atanC((*p0DC)[0], (*p1DC)[i]);
          } else {
            TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
              for (SizeT i = 0; i < nElMin; ++i) (*res)[i] = atanC((*p0DC)[0], (*p1DC)[i]);
          }
          return res;
        }
      } else {
        DDoubleGDL* p0D = static_cast<DDoubleGDL*> (p0->Convert2(GDL_DOUBLE, BaseGDL::COPY));
        guardp0D.Reset(p0D);
        DDoubleGDL* p1D = static_cast<DDoubleGDL*> (p1->Convert2(GDL_DOUBLE, BaseGDL::COPY));
        guardp1D.Reset(p1D);
        DFloatGDL* res = new DFloatGDL(dim, BaseGDL::NOZERO);
        if (nElMin == 1) {
          (*res)[ 0] = atan2((*p0D)[0], (*p1D)[0]);
          return res;
        }
        switch (cas) {
        case 0:
          if ((GDL_NTHREADS=parallelize( nElMin))==1) {
            for (SizeT i = 0; i < nElMin; ++i) (*res)[i] = atan2((*p0D)[i], (*p1D)[i]);
          } else {
            TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
              for (SizeT i = 0; i < nElMin; ++i) (*res)[i] = atan2((*p0D)[i], (*p1D)[i]);
          }
          return res;
        case 1:
          if ((GDL_NTHREADS=parallelize( nElMin))==1) {
            for (SizeT i = 0; i < nElMin; ++i) (*res)[i] = atan2((*p0D)[i], (*p1D)[0]);
          } else {
            TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
              for (SizeT i = 0; i < nElMin; ++i) (*res)[i] = atan2((*p0D)[i], (*p1D)[0]);
          }
          return res;
        case 2:
          if ((GDL_NTHREADS=parallelize( nElMin))==1) {
            for (SizeT i = 0; i < nElMin; ++i) (*res)[i] = atan2((*p0D)[0], (*p1D)[i]);
          } else {
            TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
              for (SizeT i = 0; i < nElMin; ++i) (*res)[i] = atan2((*p0D)[0], (*p1D)[i]);
          }
          return res;
        }
      }
    } else {
      static int phaseIx = e->KeywordIx("PHASE");
      if (e->KeywordSet(phaseIx) && (p0->Type() == GDL_COMPLEX || p0->Type() == GDL_COMPLEXDBL)) { //special for phase
        if (p0->Type() == GDL_COMPLEX) {
          DComplexGDL* p0C = static_cast<DComplexGDL*> (p0);
          DFloatGDL* res = new DFloatGDL(p0C->Dim(), BaseGDL::NOZERO);
          if (nEl == 1) {
            (*res)[ 0] = atan2(((*p0C)[ 0]).imag(), ((*p0C)[ 0]).real());
            return res;
          }
          if ((GDL_NTHREADS=parallelize( nEl))==1) {
            for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = atan2(((*p0C)[i]).imag(), ((*p0C)[i]).real());
          } else {
            TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
              for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = atan2(((*p0C)[i]).imag(), ((*p0C)[i]).real());
          }
          return res;
        } else if (p0->Type() == GDL_COMPLEXDBL) {
          DComplexDblGDL* p0C = static_cast<DComplexDblGDL*> (p0);
          DDoubleGDL* res = new DDoubleGDL(p0C->Dim(), BaseGDL::NOZERO);
          if (nEl == 1) {
            (*res)[ 0] = atan2(((*p0C)[ 0]).imag(), ((*p0C)[ 0]).real());
            return res;
          }
          if ((GDL_NTHREADS=parallelize( nEl))==1) {
            for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = atan2(((*p0C)[i]).imag(), ((*p0C)[i]).real());
          } else {
            TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
              for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = atan2(((*p0C)[i]).imag(), ((*p0C)[i]).real());
          }
          return res;
        }
      } else {
        if (p0->Type() == GDL_DOUBLE) {
          DDoubleGDL* p0D = static_cast<DDoubleGDL*> (p0);
          DDoubleGDL* res = new DDoubleGDL(p0D->Dim(), BaseGDL::NOZERO);
          if (nEl == 1) {
            (*res)[ 0] = atan((*p0D)[ 0]);
            return res;
          }
          if ((GDL_NTHREADS=parallelize( nEl))==1) {
            for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = atan((*p0D)[i]);
          } else {
            TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
              for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = atan((*p0D)[i]);
          }
          return res;
        } else if (p0->Type() == GDL_FLOAT) {
          DFloatGDL* p0F = static_cast<DFloatGDL*> (p0);
          DFloatGDL* res = new DFloatGDL(p0F->Dim(), BaseGDL::NOZERO);
          if (nEl == 1) {
            (*res)[ 0] = atan((*p0F)[ 0]);
            return res;
          }
          if ((GDL_NTHREADS=parallelize( nEl))==1) {
            for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = atan((*p0F)[i]);
          } else {
            TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
              for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = atan((*p0F)[i]);
          }
          return res;
        } else if (p0->Type() == GDL_COMPLEX) {
          DComplexGDL* p0C = static_cast<DComplexGDL*> (p0);
          DComplexGDL* res = new DComplexGDL(p0C->Dim(), BaseGDL::NOZERO);
          if (nEl == 1) {
            (*res)[ 0] = std::atan((*p0C)[ 0]);
            return res;
          }
          if ((GDL_NTHREADS=parallelize( nEl))==1) {
            for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = std::atan((*p0C)[ i]);
          } else {
            TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
              for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = std::atan((*p0C)[ i]);
          }
          return res;
        } else if (p0->Type() == GDL_COMPLEXDBL) {
          DComplexDblGDL* p0C = static_cast<DComplexDblGDL*> (p0);
          DComplexDblGDL* res = new DComplexDblGDL(p0C->Dim(), BaseGDL::NOZERO);
          if (nEl == 1) {
            (*res)[ 0] = std::atan((*p0C)[ 0]);
            return res;
          }
          if ((GDL_NTHREADS=parallelize( nEl))==1) {
            for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = std::atan((*p0C)[ i]);
          } else {
            TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
              for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = std::atan((*p0C)[ i]);
          }
          return res;
        } else {
          DFloatGDL* res = static_cast<DFloatGDL*> (p0->Convert2(GDL_FLOAT, BaseGDL::COPY)); //use same allocation
          if (nEl == 1) {
            (*res)[ 0] = atan((*res)[ 0]);
            return res;
          }
          if ((GDL_NTHREADS=parallelize( nEl))==1) {
            for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = atan((*res)[i]);
          } else {
            TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
              for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = atan((*res)[i]);
          }
          return res;
        }
      }
    }
    assert(false);
    return NULL; //neverreached end of non-void function
  }

  // by medericboquien@users.sourceforge.net

  BaseGDL* gauss_pdf(EnvT* e) {
    //    SizeT nParam = e->NParam(1);
    DDoubleGDL* v = static_cast<DDoubleGDL*> (e->GetParDefined(0)->
      Convert2(GDL_DOUBLE, BaseGDL::COPY));
    SizeT nv = v->N_Elements();

    for (int count = 0; count < nv; ++count)
      (*v)[count] = gsl_cdf_ugaussian_P((*v)[count]);

    if (e->GetParDefined(0)->Type() == GDL_DOUBLE)
      return v;
    else
      return v->Convert2(GDL_FLOAT, BaseGDL::CONVERT);
    return new DByteGDL(0);
  }

  // by medericboquien@users.sourceforge.net

  BaseGDL* gauss_cvf(EnvT* e) {
    //    SizeT nParam = e->NParam(1);
    DDoubleGDL* p = static_cast<DDoubleGDL*> (e->GetParDefined(0)->
      Convert2(GDL_DOUBLE, BaseGDL::COPY));

    if (p->N_Elements() != 1)
      e->Throw("Parameter must be scalar or one element array: " +
      e->GetParString(0));
    if ((*p)[0] < 0. || (*p)[0] > 1.)
      e->Throw("Parameter must be in [0,1]: " + e->GetParString(0));

    (*p)[0] = gsl_cdf_ugaussian_Qinv((*p)[0]);

    if (e->GetParDefined(0)->Type() == GDL_DOUBLE)
      return p;
    else
      return p->Convert2(GDL_FLOAT, BaseGDL::CONVERT);
    return new DByteGDL(0);
  }

  // by medericboquien@users.sourceforge.net
  // (tested OK by AC on +-NaN +-Inf in 2023)

  BaseGDL* t_pdf(EnvT* e) {
    SizeT nParam = e->NParam(2);

    DType t0 = e->GetParDefined(0)->Type();
    if ((t0 == GDL_COMPLEX) || (t0 == GDL_COMPLEXDBL))
      e->Throw("Complex not implemented. (please report)");
    DType t1 = e->GetParDefined(1)->Type();
    if ((t1 == GDL_COMPLEX) || (t1 == GDL_COMPLEXDBL))
      e->Throw("Complex not implemented. (please report)");
    
    DDoubleGDL* v = e->GetParAs<DDoubleGDL>(0);
    DDoubleGDL* df = e->GetParAs<DDoubleGDL>(1);
    DDoubleGDL* res;

    SizeT nv = v->N_Elements();
    SizeT ndf = df->N_Elements();

    for (int i = 0; i < ndf; ++i)
      if ((*df)[i] <= 0.)
        e->Throw("Degrees of freedom must be positive.");

    // revised by AC 2023-12-26 to enforce Dim() & Rank()
    
    if (v->Rank() == 0) {
      res = new DDoubleGDL(df->Dim(), BaseGDL::NOZERO);
      for (SizeT count = 0; count < ndf; ++count) 
	(*res)[count] = gsl_cdf_tdist_P((*v)[0], (*df)[count]);
    } else if (df->Rank() == 0) {
      res = new DDoubleGDL(v->Dim(), BaseGDL::NOZERO);
      for (SizeT count = 0; count < nv; ++count)
        (*res)[count] = gsl_cdf_tdist_P((*v)[count], (*df)[0]);
    } else {
      SizeT nbp;
      if (nv > ndf) {
	nbp=ndf;
	res = new DDoubleGDL(df->Dim(), BaseGDL::NOZERO);
      } else {
	nbp=nv;
	res = new DDoubleGDL(v->Dim(), BaseGDL::NOZERO);
      }	
      for (SizeT count = 0; count < nbp; ++count)
        (*res)[count] = gsl_cdf_tdist_P((*v)[count], (*df)[count]);
    }

    if (e->GetParDefined(0)->Type() != GDL_DOUBLE &&
	e->GetParDefined(1)->Type() != GDL_DOUBLE)
      return res->Convert2(GDL_FLOAT, BaseGDL::CONVERT);
    else
      return res;
  }

  // by medericboquien@users.sourceforge.net

  BaseGDL* laguerre(EnvT* e) {
    SizeT nParam = e->NParam(2);

    DDoubleGDL* xvals = e->GetParAs<DDoubleGDL>(0);
    if (e->GetParDefined(0)->Type() == GDL_COMPLEX ||
	e->GetParDefined(0)->Type() == GDL_COMPLEXDBL)
      e->Throw("Complex Laguerre not implemented: ");

    DIntGDL* nval = e->GetParAs<DIntGDL>(1);
    if (nval->N_Elements() != 1)
      e->Throw("N and K must be scalars.");
    if ((*nval)[0] < 0)
      e->Throw("Argument N must be greater than or equal to zero.");

    DDoubleGDL* kval;
    Guard<DDoubleGDL> kval_guard;
    if (nParam > 2) {
      kval = e->GetParAs<DDoubleGDL>(2);
      if (kval->N_Elements() != 1)
        e->Throw("N and K must be scalars.");
      if ((*kval)[0] < 0.)
        e->Throw("Argument K must be greater than or equal to zero.");
    } else {
      kval = new DDoubleGDL(0);
      kval_guard.Reset(kval);
    }

    DDoubleGDL* res = new DDoubleGDL(xvals->Dim(), BaseGDL::NOZERO);
    DDouble k = (*kval)[0];
    DInt n = (*nval)[0];
    SizeT nEx = xvals->N_Elements();

    if ((GDL_NTHREADS=parallelize( nEx))==1) {
      for (SizeT count = 0; count < nEx; ++count) (*res)[count] = gsl_sf_laguerre_n(n, k, (*xvals)[count]);
    } else {
      TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (SizeT count = 0; count < nEx; ++count) (*res)[count] = gsl_sf_laguerre_n(n, k, (*xvals)[count]);
    }

    static DInt doubleKWIx = e->KeywordIx("DOUBLE");
    static DInt coefKWIx = e->KeywordIx("COEFFICIENTS");

    if (e->KeywordPresent(coefKWIx)) {
      double gamma_kn1 = gsl_sf_gamma(k + n + 1.);
      DDoubleGDL* coefKW = new DDoubleGDL(dimension(n + 1), BaseGDL::NOZERO);

      //GD: parallelizing this complicated loop cannot be done just like that.
      //I further doubt Laguerre is going to be used on large arrays.
      for (SizeT count = 0; count <= n; ++count) {
        double dcount = static_cast<double> (count);
        (*coefKW)[count] = ((count & 0x0001) ? -1.0 : 1.0) * gamma_kn1 /
          (gsl_sf_gamma(n - dcount + 1.) * gsl_sf_gamma(k + dcount + 1.) *
          gsl_sf_gamma(dcount + 1.));
      }

      if (e->GetParDefined(0)->Type() != GDL_DOUBLE && !e->KeywordSet(doubleKWIx))
        coefKW = static_cast<DDoubleGDL*> (coefKW->
        Convert2(GDL_FLOAT, BaseGDL::CONVERT));
      e->SetKW(coefKWIx, coefKW);
    }

    //convert things back
    if (e->GetParDefined(0)->Type() != GDL_DOUBLE && !e->KeywordSet(doubleKWIx))
      return res->Convert2(GDL_FLOAT, BaseGDL::CONVERT);
    else
      return res;

  }

  // SA: based on equations 5-5 & 5-6 from Snyder (1987) USGS report no 1395 (page 31)
  //     available for download at: http://pubs.er.usgs.gov/djvu/PP/pp_1395.djvu

  template <typename T> inline void ll_arc_distance_helper(
    T c, T Az, T phi1, T l0, T& phi, T& l, bool degrees) {
    // temporary variables
    T pi = 4 * atan((T) 1.),
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

  BaseGDL* ll_arc_distance(EnvT* e) {
    // sanity check (for number of parameters)
    //    SizeT nParam = e->NParam();

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

    static int degreesIx=e->KeywordIx("DEGREES");
    
    BaseGDL* rt = p0->New(dimension(2, BaseGDL::NOZERO));
    if (type == GDL_FLOAT) {
      ll_arc_distance_helper(
        (*static_cast<DFloatGDL*> (p1))[0],
        (*static_cast<DFloatGDL*> (p2))[0],
        (*static_cast<DFloatGDL*> (p0))[1],
        (*static_cast<DFloatGDL*> (p0))[0],
        (*static_cast<DFloatGDL*> (rt))[1],
        (*static_cast<DFloatGDL*> (rt))[0],
        e->KeywordSet(degreesIx) //DEGREES (sole option)
        );
    } else {
      ll_arc_distance_helper(
        (*static_cast<DDoubleGDL*> (p1))[0],
        (*static_cast<DDoubleGDL*> (p2))[0],
        (*static_cast<DDoubleGDL*> (p0))[1],
        (*static_cast<DDoubleGDL*> (p0))[0],
        (*static_cast<DDoubleGDL*> (rt))[1],
        (*static_cast<DDoubleGDL*> (rt))[0],
        e->KeywordSet(degreesIx)
        );
    }

    // handling complex/dcomplex conversion
    return rt->Convert2(
      args_complexdbl ? GDL_COMPLEXDBL : args_complex ? GDL_COMPLEX : type,
      BaseGDL::CONVERT
      );
  }

  BaseGDL* crossp(EnvT* e) {
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
    a->Add(b); // | | a = shift(p0, -1)
    delete b; // | `--mem: del b
    b = p1->CShift(-2)->Convert2(a->Type(), BaseGDL::CONVERT);
    // | .--mem: new b
    a->Mult(b); // | | a = shift(p0, -1) * shift(p1, -2)
    b->Sub(b); // | | b = 0
    c = p0->CShift(1)->Convert2(a->Type(), BaseGDL::CONVERT);
    // | | .--mem: new c
    b->Sub(c); // | | | b = - shift(p0, 1)
    delete c; // | | `--mem: del c
    c = p1->CShift(2)->Convert2(a->Type(), BaseGDL::CONVERT);
    // | | .--mem: new c
    b->Mult(c); // | | | b = - shift(p0, 1) * shift(p1, 2)
    delete c; // | | `--mem: del c
    a->Add(b); // | | a = shift(p0, -1) * shift(p1, -2) - shift(p0, 1) * shift(p1, 2)
    delete b; // | `--mem: del b
    return a; // `--->
  }


  // SA: adapted from the GPL-licensed GNU plotutils (plotutils-2.5/ode/specfun.c)
  // -----------------------------------------------------------------------------

  template <typename T>
  T inverf(T p) /* Inverse Error Function */ {
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
    static T a1 = -.5751703, a2 = -1.896513, a3 = -.05496261,
      b0 = -.113773, b1 = -3.293474, b2 = -2.374996, b3 = -1.187515,
      c0 = -.1146666, c1 = -.1314774, c2 = -.2368201, c3 = .05073975,
      d0 = -44.27977, d1 = 21.98546, d2 = -7.586103,
      e0 = -.05668422, e1 = .3937021, e2 = -.3166501, e3 = .06208963,
      f0 = -6.266786, f1 = 4.666263, f2 = -2.962883,
      g0 = 1.851159e-4, g1 = -.002028152, g2 = -.1498384, g3 = .01078639,
      h0 = .09952975, h1 = .5211733, h2 = -.06888301;

    /* Local variables */
    static T a, b, f, w, x, y, z, sigma, z2, sd, wi, sn;

    x = p;

    /* determine sign of x */
    sigma = (x > 0 ? 1.0 : -1.0);

    /* Note: -1.0 < x < 1.0 */

    z = abs(x);

    /* z between 0.0 and 0.85, approx. f by a
       rational function in z  */

    if (z <= 0.85) {
      z2 = z * z;
      f = z + z * (b0 + a1 * z2 / (b1 + z2 + a2 / (b2 + z2 + a3 / (b3 + z2))));
    } else /* z greater than 0.85 */ {
      a = 1.0 - z;
      b = z;

      /* reduced argument is in (0.85,1.0), obtain the transformed variable */

      w = sqrt(-(T) log(a + a * b));

      if (w >= 4.0)
        /* w greater than 4.0, approx. f by a rational function in 1.0 / w */ {
        wi = 1.0 / w;
        sn = ((g3 * wi + g2) * wi + g1) * wi;
        sd = ((wi + h2) * wi + h1) * wi + h0;
        f = w + w * (g0 + sn / sd);
      } else if (w < 4.0 && w > 2.5)
        /* w between 2.5 and 4.0, approx.  f by a rational function in w */ {
        sn = ((e3 * w + e2) * w + e1) * w;
        sd = ((w + f2) * w + f1) * w + f0;
        f = w + w * (e0 + sn / sd);

        /* w between 1.13222 and 2.5, approx. f by
           a rational function in w */
      } else if (w <= 2.5 && w > 1.13222) {
        sn = ((c3 * w + c2) * w + c1) * w;
        sd = ((w + d2) * w + d1) * w + d0;
        f = w + w * (c0 + sn / sd);
      }
    }
    y = sigma * f;

    return y;
  }
  // -----------------------------------------------------------------------------

  BaseGDL* gdl_erfinv_fun(EnvT* e) {
    BaseGDL* p0 = e->GetNumericParDefined(0);
    SizeT n = p0->N_Elements();
    static int doubleIx = e->KeywordIx("DOUBLE");
    if (e->KeywordSet(doubleIx) || p0->Type() == GDL_DOUBLE) {
      DDoubleGDL *ret = new DDoubleGDL(dimension(n),BaseGDL::NOZERO), *p0d = e->GetParAs<DDoubleGDL>(0);
      while (n != 0) --n, (*ret)[n] = inverf((*p0d)[n]);
      return ret;
    } else {
      DFloatGDL *ret = new DFloatGDL(dimension(n),BaseGDL::NOZERO), *p0f = e->GetParAs<DFloatGDL>(0);
      while (n != 0) --n, (*ret)[n] = inverf((*p0f)[n]);
      return ret;
    }
  }

} // namespace
