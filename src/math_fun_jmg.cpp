/***************************************************************************
                          math_fun_jmg.cpp  -  math GDL library function (JMG)
                             -------------------
    begin                : March 03 2004
    copyright            : (C) 2002 by Joel Gales
    email                : jomoga@users.sourceforge.net
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

#include <iostream>
#include <complex>
#include <cmath>
#include <cfenv>
#include "envt.hpp"
#include "math_utl.hpp"
#include "math_fun_jmg.hpp"
#include "gdl_util.hpp"

//#define GDL_DEBUG
//#undef GDL_DEBUG

#define COMPLEX2 GDL_COMPLEX
#if defined(_MSC_VER) && _MSC_VER < 1800
#define FE_DIVBYZERO
#define FE_UNDERFLOW
#define FE_OVERFLOW
#define FE_INVALID
#define round(f) floor(f+0.5)
#define isnan _isnan
#define isinf !_finite
#define isfinite _finite
#define signbit(d) (d < 0.0)? 1:0
#endif
namespace lib {

  using namespace std;
#ifndef _MSC_VER
  using std::isinf;
  using std::isnan;
#endif

  BaseGDL* machar_fun( EnvT* e)
  {
    long int ibeta, it, irnd, ngrd, machep, negep, iexp, minexp, maxexp;
    float  eps, epsneg, xmin, xmax;
    double epsD, epsnegD, xminD, xmaxD;

    static int doubleIx=e->KeywordIx("DOUBLE");

    if (e->KeywordSet(doubleIx))
      {
	machar_d(&ibeta, &it, &irnd, &ngrd, &machep, 
		 &negep, &iexp, &minexp, &maxexp, 
		 &epsD, &epsnegD, &xminD, &xmaxD );

	DStructGDL*  machar = new DStructGDL( "DMACHAR");

	machar->InitTag("IBETA", DLongGDL(ibeta));
	machar->InitTag("IT", DLongGDL(it));
	machar->InitTag("IRND", DLongGDL(irnd));
	machar->InitTag("NGRD", DLongGDL(ngrd));
	machar->InitTag("MACHEP", DLongGDL(machep));
	machar->InitTag("NEGEP", DLongGDL(negep));
	machar->InitTag("IEXP", DLongGDL(iexp));
	machar->InitTag("MINEXP", DLongGDL(minexp));
	machar->InitTag("MAXEXP", DLongGDL(maxexp));
	machar->InitTag("EPS", DDoubleGDL(epsD));
	machar->InitTag("EPSNEG", DDoubleGDL(epsnegD));
	machar->InitTag("XMIN", DDoubleGDL(xminD));
	machar->InitTag("XMAX", DDoubleGDL(xmaxD));

	return machar;
      }
    else
      {
	machar_s(&ibeta, &it, &irnd, &ngrd, &machep, 
		 &negep, &iexp, &minexp, &maxexp, 
		 &eps, &epsneg, &xmin, &xmax );

	DStructGDL*  machar = new DStructGDL( "MACHAR");

	machar->InitTag("IBETA", DLongGDL(ibeta));
	machar->InitTag("IT", DLongGDL(it));
	machar->InitTag("IRND", DLongGDL(irnd));
	machar->InitTag("NGRD", DLongGDL(ngrd));
	machar->InitTag("MACHEP", DLongGDL(machep));
	machar->InitTag("NEGEP", DLongGDL(negep));
	machar->InitTag("IEXP", DLongGDL(iexp));
	machar->InitTag("MINEXP", DLongGDL(minexp));
	machar->InitTag("MAXEXP", DLongGDL(maxexp));
	machar->InitTag("EPS", DFloatGDL(eps));
	machar->InitTag("EPSNEG", DFloatGDL(epsneg));
	machar->InitTag("XMIN", DFloatGDL(xmin));
	machar->InitTag("XMAX", DFloatGDL(xmax));

	return machar;
      }
  }

// #if 0
//   template< typename T>
//   BaseGDL* finite_fun_template( BaseGDL* p0)
//   {
//     T* p0C = static_cast<T*>( p0);
//     DByteGDL* res = new DByteGDL( p0C->Dim(), BaseGDL::NOZERO);
//     SizeT nEl = p0->N_Elements();
//     for( SizeT i=0; i<nEl; ++i)
//       {
// 	if( p0->Type() == GDL_COMPLEX) {
// 	  float* dptr = (float*) &(*p0C)[0];
// 	 } else if( p0->Type() == GDL_COMPLEXDBL) {
// 	    int a=0;
// 	} else {
// 	  int out = isfinite((*p0C)[ i]); 
// 	  if (out == 0)
// 	    (*res)[ i] = 0;
// 	  else
// 	    (*res)[ i] = 1;
// 	}
//       }
//     return res;
//   }
// #endif

//   BaseGDL* finite_fun( EnvT* e)
//   {
//     e->NParam( 1);//, "FINITE");

//     BaseGDL* p0 = e->GetParDefined( 0);//, "FINITE");

//     SizeT nEl = p0->N_Elements();
//     if( nEl == 0)
//       e->Throw( "Variable is undefined: "+e->GetParString(0));

//     DByteGDL* res = new DByteGDL( p0->Dim(), BaseGDL::NOZERO);

//     if( p0->Type() == GDL_COMPLEX) {
//       DComplexGDL* p0C = static_cast<DComplexGDL*>( p0);
//       for( SizeT i=0; i<nEl; ++i) {
// // 	float* dptr = (float*) &(*p0C)[ i];
// // 	float r_part = *dptr++;
// // 	float i_part = *dptr;
// 	float r_part = (*p0C)[ i].real();
// 	float i_part = (*p0C)[ i].imag();
// 	if (isfinite(r_part) == 0 || isfinite(i_part) == 0) 
// 	  (*res)[ i] = 0; else (*res)[ i] = 1;
//       }
//     } else if ( p0->Type() == GDL_COMPLEXDBL) {
//       DComplexDblGDL* p0C = static_cast<DComplexDblGDL*>( p0);
//       for( SizeT i=0; i<nEl; ++i) {
// //         double* dptr = (double*) &(*p0C)[ i];
// // 	double r_part = *dptr++;
// // 	double i_part = *dptr;
// 	double r_part = (*p0C)[ i].real();
// 	double i_part = (*p0C)[ i].imag();
// 	if (isfinite(r_part) == 0 || isfinite(i_part) == 0) 
// 	  (*res)[ i] = 0; else (*res)[ i] = 1;
//       }
//     } else if( p0->Type() == GDL_DOUBLE) {
//       DDoubleGDL* p0D = static_cast<DDoubleGDL*>( p0);
//       for( SizeT i=0; i<nEl; ++i)
// 	if (isfinite((*p0D)[ i]) == 0) (*res)[ i] = 0; else (*res)[ i] = 1;
//     } else if( p0->Type() == GDL_FLOAT) {
//       DFloatGDL* p0F = static_cast<DFloatGDL*>( p0);
//       for( SizeT i=0; i<nEl; ++i)
// 	if (isfinite((*p0F)[ i]) == 0) (*res)[ i] = 0; else (*res)[ i] = 1;
//     } else {
//       DFloatGDL* p0F = static_cast<DFloatGDL*>
// 	(p0->Convert2( GDL_FLOAT, BaseGDL::COPY));
//       for( SizeT i=0; i<nEl; ++i)
// 	if (isfinite((*p0F)[ i]) == 0) (*res)[ i] = 0; else (*res)[ i] = 1;
//     }
//     return res;
//   }


  // FINITE function 
  // by Pierre Chanial, pchanial@users.sourceforge.net
  // slightly modified by Marc Schellens, m_schellens@users.sourceforge.net

  // general template

  template< typename T, bool> struct finite_helper {

    inline static BaseGDL* do_it(T* src, bool kwNaN, bool kwInfinity) {
      DByteGDL* res = new DByteGDL(src->Dim(), BaseGDL::NOZERO);
      SizeT nEl = src->N_Elements();

      if (kwNaN) {
        if ((GDL_NTHREADS=parallelize( nEl))==1) {
          for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = isnan((*src)[ i]);
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) 
            for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = isnan((*src)[ i]);
        }
      } else if (kwInfinity) {
        if ((GDL_NTHREADS=parallelize( nEl))==1) {
          for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = isinf((*src)[ i]);
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) 
            for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = isinf((*src)[ i]);
        }
      } else {
        if ((GDL_NTHREADS=parallelize( nEl))==1) {
          for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = isfinite((*src)[ i]);
        } else {
          TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) 
            for (SizeT i = 0; i < nEl; ++i) (*res)[ i] = isfinite((*src)[ i]);
        }
      }
      return res;
    }
  };

   // partial specialization for GDL_COMPLEX, DCOMPLEX
   template< typename T> struct finite_helper<T, true>
   {
    inline static BaseGDL* do_it(T* src, bool kwNaN, bool kwInfinity)
    {
       DByteGDL* res = new DByteGDL( src->Dim(), BaseGDL::NOZERO);
       SizeT nEl = src->N_Elements();
       if (kwNaN){
	 if ((GDL_NTHREADS=parallelize( nEl))==1) {
         for ( SizeT i=0; i<nEl; ++i) (*res)[ i] = isnan((*src)[ i].real()) || isnan((*src)[ i].imag());
	 } else {
   TRACEOMP(__FILE__,__LINE__)
#pragma omp parallel  for num_threads(GDL_NTHREADS) 
         for ( SizeT i=0; i<nEl; ++i) (*res)[ i] = isnan((*src)[ i].real()) || isnan((*src)[ i].imag());
	   }
       }
       else if (kwInfinity){
	 if ((GDL_NTHREADS=parallelize( nEl))==1) {
         for ( SizeT i=0; i<nEl; ++i) (*res)[ i] = isinf((*src)[ i].real()) || isinf((*src)[ i].imag());
	 } else {
   TRACEOMP(__FILE__,__LINE__)
#pragma omp parallel  for num_threads(GDL_NTHREADS) 
         for ( SizeT i=0; i<nEl; ++i) (*res)[ i] = isinf((*src)[ i].real()) || isinf((*src)[ i].imag());
	   }
       }
       else{
	 if ((GDL_NTHREADS=parallelize( nEl))==1) {
         for ( SizeT i=0; i<nEl; ++i) (*res)[ i] = isfinite((*src)[ i].real()) && isfinite((*src)[ i].imag());
	 } else {
   TRACEOMP(__FILE__,__LINE__)
#pragma omp parallel  for num_threads(GDL_NTHREADS) 
         for ( SizeT i=0; i<nEl; ++i) (*res)[ i] = isfinite((*src)[ i].real()) && isfinite((*src)[ i].imag());
	   }
       }
     return res;
    }
   };

   template< typename T, bool IS_COMPLEX>
   inline BaseGDL* finite_template( BaseGDL* src, bool kwNaN, bool kwInfinity)
   {
     return finite_helper<T, IS_COMPLEX>::
       do_it(static_cast<T*>(src), kwNaN, kwInfinity);
   };

  //general signed template

  template< typename T, bool> struct finite_helper_sign {

    inline static BaseGDL* do_it(T* src, bool kwNaN, bool kwInfinity, DLong kwSign) {


      DByteGDL* res = new DByteGDL(src->Dim(), BaseGDL::ZERO); // ::ZERO is not working
      SizeT nEl = src->N_Elements();
      if (kwInfinity || kwNaN) {
        {
          if (kwInfinity) {
            if (kwSign > 0) {
              if ((GDL_NTHREADS=parallelize( nEl))==1) {
                for (SizeT i = 0; i < nEl; ++i) {
                  (*res)[i] = (isinf((*src)[ i]) && (signbit((*src)[ i]) == 0));
                }
              } else {
                TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) 
                  for (SizeT i = 0; i < nEl; ++i) {
                  (*res)[i] = (isinf((*src)[ i]) && (signbit((*src)[ i]) == 0));
                }
              }
            } else {
              if ((GDL_NTHREADS=parallelize( nEl))==1) {
                for (SizeT i = 0; i < nEl; ++i) {
                  (*res)[i] = (isinf((*src)[ i]) && (signbit((*src)[ i]) != 0));
                }
              } else {
                TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) 
                  for (SizeT i = 0; i < nEl; ++i) {
                  (*res)[i] = (isinf((*src)[ i]) && (signbit((*src)[ i]) != 0));
                }
              }
            }
          }
          if (kwNaN) {
            if (kwSign > 0) {
              if ((GDL_NTHREADS=parallelize( nEl))==1) {
                for (SizeT i = 0; i < nEl; ++i) {
                  (*res)[i] = (isnan((*src)[ i]) && (signbit((*src)[ i]) == 0));
                }
              } else {
                TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) 
                  for (SizeT i = 0; i < nEl; ++i) {
                  (*res)[i] = (isnan((*src)[ i]) && (signbit((*src)[ i]) == 0));
                }
              }
            } else {
              if ((GDL_NTHREADS=parallelize( nEl))==1) {
                for (SizeT i = 0; i < nEl; ++i) {
                  (*res)[i] = (isnan((*src)[ i]) && (signbit((*src)[ i]) != 0));
                }
              } else {
                TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) 
                  for (SizeT i = 0; i < nEl; ++i) {
                  (*res)[i] = (isnan((*src)[ i]) && (signbit((*src)[ i]) != 0));
                }
              }
            }
          }
        }
        return res;
      }
      assert(false);
      return NULL; //-Wreturn-type
    }
  };

  // partial specialization for GDL_COMPLEX, DCOMPLEX

  template< typename T> struct finite_helper_sign<T, true> {

    inline static BaseGDL* do_it(T* src, bool kwNaN, bool kwInfinity, DLong kwSign) {
      DByteGDL* res = new DByteGDL(src->Dim(), BaseGDL::ZERO);
      SizeT nEl = src->N_Elements();
      if (kwInfinity || kwNaN) {
        {
          if (kwInfinity) {
            if (kwSign > 0) {
              if ((GDL_NTHREADS=parallelize( nEl))==1) {
                for (SizeT i = 0; i < nEl; ++i) {
                  (*res)[i] = ((isinf((*src)[ i].real()) && (!signbit((*src)[ i].real()))) || (isinf((*src)[ i].imag()) && (!signbit((*src)[ i].imag()))));
                }
              } else {
                TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) 
                  for (SizeT i = 0; i < nEl; ++i) {
                  (*res)[i] = ((isinf((*src)[ i].real()) && (!signbit((*src)[ i].real()))) || (isinf((*src)[ i].imag()) && (!signbit((*src)[ i].imag()))));
                }
              }
            } else {
              if ((GDL_NTHREADS=parallelize( nEl))==1) {
                for (SizeT i = 0; i < nEl; ++i) {
                  (*res)[i] = ((isinf((*src)[ i].real()) && (signbit((*src)[ i].real()))) || (isinf((*src)[ i].imag()) && (signbit((*src)[ i].imag()))));
                }
              } else {
                TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) 
                  for (SizeT i = 0; i < nEl; ++i) {
                  (*res)[i] = ((isinf((*src)[ i].real()) && (signbit((*src)[ i].real()))) || (isinf((*src)[ i].imag()) && (signbit((*src)[ i].imag()))));
                }
              }
            }
          }
          if (kwNaN) {
            if (kwSign > 0) {
              if ((GDL_NTHREADS=parallelize( nEl))==1) {
                for (SizeT i = 0; i < nEl; ++i) {
                  (*res)[i] = ((isnan((*src)[ i].real()) && (!signbit((*src)[ i].real()))) || (isnan((*src)[ i].imag()) && (!signbit((*src)[ i].imag()))));
                }
              } else {
                TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) 
                  for (SizeT i = 0; i < nEl; ++i) {
                  (*res)[i] = ((isnan((*src)[ i].real()) && (!signbit((*src)[ i].real()))) || (isnan((*src)[ i].imag()) && (!signbit((*src)[ i].imag()))));
                }
              }
            } else {
              if ((GDL_NTHREADS=parallelize( nEl))==1) {
                for (SizeT i = 0; i < nEl; ++i) {
                  (*res)[i] = ((isnan((*src)[ i].real()) && (signbit((*src)[ i].real()))) || (isnan((*src)[ i].imag()) && (signbit((*src)[ i].imag()))));
                }
              } else {
                TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) 
                  for (SizeT i = 0; i < nEl; ++i) {
                  (*res)[i] = ((isnan((*src)[ i].real()) && (signbit((*src)[ i].real()))) || (isnan((*src)[ i].imag()) && (signbit((*src)[ i].imag()))));
                }
              }
            }
          }
        }
        return res;
      }
      assert(false);
      return NULL; //-Wreturn-type
    }
  };

//   template< typename T> struct finite_helper_sign<T, true>
//   {
//    inline static BaseGDL* do_it(T* src, bool kwNaN, bool kwInfinity, DLong kwSign)
//    {
//       DByteGDL* res = new DByteGDL( src->Dim(), BaseGDL::ZERO);
//       SizeT nEl = src->N_Elements();
//       
//		if ((GDL_NTHREADS=parallelize( nEl))==1) {
//       for ( SizeT i=0; i<nEl; ++i)
//	  {
//	   if      ((kwInfinity && isinf((*src)[ i].real()) || kwNaN && isnan((*src)[ i].real())) && signbit((*src)[ i].real())==0 && kwSign > 0) (*res)[i]=1;
//	   else if ((kwInfinity && isinf((*src)[ i].imag()) || kwNaN && isnan((*src)[ i].imag())) && signbit((*src)[ i].imag())==0 && kwSign > 0) (*res)[i]=1;
//	   else if ((kwInfinity && isinf((*src)[ i].real()) || kwNaN && isnan((*src)[ i].real())) && signbit((*src)[ i].real())!=0 && kwSign < 0) (*res)[i]=1;
//	   else if ((kwInfinity && isinf((*src)[ i].imag()) || kwNaN && isnan((*src)[ i].imag())) && signbit((*src)[ i].imag())!=0 && kwSign < 0) (*res)[i]=1;	 
//	  }
//		} else {
//  TRACEOMP(__FILE__,__LINE__)
// 	   #pragma omp parallel for num_threads(GDL_NTHREADS) 
//       for ( SizeT i=0; i<nEl; ++i)
//	  {
//	   if      ((kwInfinity && isinf((*src)[ i].real()) || kwNaN && isnan((*src)[ i].real())) && signbit((*src)[ i].real())==0 && kwSign > 0) (*res)[i]=1;
//	   else if ((kwInfinity && isinf((*src)[ i].imag()) || kwNaN && isnan((*src)[ i].imag())) && signbit((*src)[ i].imag())==0 && kwSign > 0) (*res)[i]=1;
//	   else if ((kwInfinity && isinf((*src)[ i].real()) || kwNaN && isnan((*src)[ i].real())) && signbit((*src)[ i].real())!=0 && kwSign < 0) (*res)[i]=1;
//	   else if ((kwInfinity && isinf((*src)[ i].imag()) || kwNaN && isnan((*src)[ i].imag())) && signbit((*src)[ i].imag())!=0 && kwSign < 0) (*res)[i]=1;	 
//	  }
//     }
//     return res;
//    } 
//   };

   template< typename T, bool IS_COMPLEX>
   inline BaseGDL* finite_template( BaseGDL* src, bool kwNaN, bool kwInfinity, DLong kwSign)
   {
     return finite_helper_sign<T, IS_COMPLEX>::
       do_it(static_cast<T*>(src), kwNaN, kwInfinity, kwSign);
   };

  BaseGDL* finite_fun(EnvT* e)
  {
    e->NParam(1);

    BaseGDL* p0 = e->GetParDefined(0);
    Guard<BaseGDL> guard;

    static int nanIx = e->KeywordIx("NAN");
    bool kwNaN = e->KeywordSet(nanIx);

    static int infinityIx = e->KeywordIx("INFINITY");
    bool kwInfinity = e->KeywordSet(infinityIx);

    static int signIx = e->KeywordIx("SIGN");
    DLong kwSign = 0;
    e->AssureLongScalarKWIfPresent(signIx, kwSign);

    if (kwNaN && kwInfinity)
      e->Throw("Conflicting keywords.");

    if (kwSign == 0 || (kwInfinity == 0 && kwNaN == 0)) {
      switch (p0->Type()) {
      case GDL_FLOAT:
      {
        return finite_template<DFloatGDL, false>(p0, kwNaN, kwInfinity);
      }
      case GDL_DOUBLE:
      {
        return finite_template<DDoubleGDL, false>(p0, kwNaN, kwInfinity);
      }
      case GDL_COMPLEX:
      {
        return finite_template<DComplexGDL, true>(p0, kwNaN, kwInfinity);
      }
      case GDL_COMPLEXDBL:
      {
        return finite_template<DComplexDblGDL, true>(p0, kwNaN, kwInfinity);
      }
      case GDL_STRING:
      {
        DFloatGDL* p0F =
            static_cast<DFloatGDL*> (p0->Convert2(GDL_FLOAT, BaseGDL::COPY));
        guard.Reset(p0F);
        return finite_template<DFloatGDL, false>(p0F, kwNaN, kwInfinity);
      }
      case GDL_STRUCT:
      case GDL_PTR:
      case GDL_OBJ:
      {
        e->Throw(p0->TypeStr() + " not allowed in this context: " +
            e->GetParString(0));
      }
      default: // integer types
      {
        if (kwNaN || kwInfinity) return new DByteGDL(p0->Dim(), BaseGDL::ZERO); //0
        DByteGDL* one = new DByteGDL(1);
        return one->New(p0->Dim(), BaseGDL::INIT); //1
      }
      }
    }      // Sign
    else {
      switch (p0->Type()) {
      case GDL_FLOAT:
      {
        return finite_template<DFloatGDL, false>(p0, kwNaN, kwInfinity, kwSign);
      }
      case GDL_DOUBLE:
      {
        return finite_template<DDoubleGDL, false>(p0, kwNaN, kwInfinity, kwSign);
      }
      case GDL_COMPLEX:
      {
        return finite_template<DComplexGDL, true>(p0, kwNaN, kwInfinity, kwSign);
      }
      case GDL_COMPLEXDBL:
      {
        return finite_template<DComplexDblGDL, true>(p0, kwNaN, kwInfinity, kwSign);
      }
      case GDL_STRING:
      {
        DFloatGDL* p0F =
            static_cast<DFloatGDL*> (p0->Convert2(GDL_FLOAT, BaseGDL::COPY));
        guard.Reset(p0F);
        return finite_template<DFloatGDL, false>(p0F, kwNaN, kwInfinity, kwSign);
      }
      case GDL_STRUCT:
      case GDL_PTR:
      case GDL_OBJ:
      {
        e->Throw(p0->TypeStr() + " not allowed in this context: " +
            e->GetParString(0));
      }
      default: // integer types
      {
        if (kwNaN || kwInfinity) return new DByteGDL(p0->Dim(), BaseGDL::ZERO); //0
        DByteGDL* one = new DByteGDL(1);
        return one->New(p0->Dim(), BaseGDL::INIT); //1
      }
      }
    }
  }

  // AC 06/10/2009 in Sapporo
  // I found, using POIDEV(), that CHECK_MATH() does accept 2 parameters
  // for compatibilities reasons with OLD IDL and PV-Waves
  // e.g. http://vis.lbl.gov/NERSC/Software/pvwave/docs/pvwavehtmlhelp/waveref/files/chb6.htm
  // 

  BaseGDL* check_math_fun( EnvT* e)
  {

    SizeT nParam=e->NParam();
    DLong flag_print=0, flag_noclear=0, flag_clear=1;

    DLong value=0;
    static DLong cumul_value; // auto init to 0
    DLong mask=255;

    //
    static int printKwIx = e->KeywordIx("PRINT");
    static int noClearKwIx = e->KeywordIx("NOCLEAR");
    static int maskKwIx = e->KeywordIx("MASK");

    flag_print = e->KeywordSet( printKwIx);
    flag_noclear = e->KeywordSet( noClearKwIx);
    //
    // if Params are provides (first: print, second: noclear)
    // they do overwrite the same provided by Keyword
    //
    if (nParam >= 1) {
      e->AssureLongScalarPar( 0, flag_print);
    }    
    if (nParam == 2) {
      e->AssureLongScalarPar( 1, flag_noclear);
    }
    
    if (flag_noclear > 0) flag_clear=0;
    
    const int debug=0;
    if (debug) {
      cout << "Flag Print  : " << flag_print << endl;
      cout << "Flag NoClear: " << flag_noclear << endl;
      cout << "Flag Clear: " << flag_clear << endl;
    }

    if( e->KeywordSet( maskKwIx))
      e->AssureLongScalarKWIfPresent( maskKwIx, mask);	

    if (mask & 16) {
#if defined(_MSC_VER) && _MSC_VER < 1800
      if (_statusfp() & _SW_ZERODIVIDE == _SW_ZERODIVIDE) {
#else
      if (fetestexcept(FE_DIVBYZERO)) {
#endif
	value = value | 16;
	if ( flag_print)
	  cout << 
	    "% Program caused arithmetic error: Floating divide by 0" << endl;
	if ( flag_clear) feclearexcept(FE_DIVBYZERO); 
      }
    }

    if (mask & 32) {
#if defined(_MSC_VER) && _MSC_VER < 1800
		if (_statusfp() & _SW_UNDERFLOW == _SW_UNDERFLOW) {
#else
		if (fetestexcept(FE_UNDERFLOW)) {
#endif
	value = value | 32;
	if ( flag_print)
	  cout << 
	    "% Program caused arithmetic error: Floating underflow" << endl;
	if ( flag_clear) feclearexcept(FE_UNDERFLOW); 
      }
    }

    if (mask & 64) {
#if defined(_MSC_VER) && _MSC_VER < 1800
		if (_statusfp() & _SW_OVERFLOW == _SW_OVERFLOW) {
#else
		if (fetestexcept(FE_OVERFLOW)) {
#endif
	value = value | 64;
	if ( flag_print)
	  cout << 
	    "% Program caused arithmetic error: Floating overflow" << endl;
	if ( flag_clear) feclearexcept(FE_OVERFLOW); 
      }
    }

    
    if (mask & 128) {// && value == 0) {
#if defined(_MSC_VER) && _MSC_VER < 1800
		if (_statusfp() & _SW_INVALID == _SW_INVALID) {
#else
		if (fetestexcept(FE_INVALID)) {
#endif
	value = value | 128;
	if ( flag_print) // avoid double message
	  cout << 
	    "% Program caused arithmetic error: Floating illegal operand" << endl;
	if ( flag_clear) feclearexcept(FE_INVALID); 
      }
    }
    
    if (debug) {
      cout << "      value " <<value<< endl;
      cout << "cumul_value " <<cumul_value<< endl;
    }
    
    if (flag_noclear) {
       if (debug) cout << "noclear == 1" << endl;
      cumul_value |= value;
      value=cumul_value;
    } else {
       if (debug) cout << "noclear == 0" << endl;
      cumul_value=0;
    }
    if (debug) {
      cout << "      value " <<value<< endl;
      cout << "cumul_value " <<cumul_value<< endl;
    }

    return new DLongGDL( value);
  }


  BaseGDL* radon_fun( EnvT* e)
  {
    BaseGDL* p0 = e->GetParDefined( 0);

    SizeT nEl = p0->N_Elements();
    if( nEl == 0)
      e->Throw( "Variable is undefined: "+e->GetParString(0));

    if (p0->Rank() != 2)
      e->Throw( "Array must have 2 dimensions: "+e->GetParString(0));

    DFloatGDL* p0F = static_cast<DFloatGDL*>
      (p0->Convert2( GDL_FLOAT, BaseGDL::COPY));

    DFloat fpi=4*atan(1.0);

    DFloat dx=1, dy=1;
    DFloat drho = 0.5 * sqrt(dx*dx + dy*dy);

    static int DXIx=e->KeywordIx("DX");
    static int DYIx=e->KeywordIx("DY");
    static int DRHOIx=e->KeywordIx("DRHO");
    static int NXIx=e->KeywordIx("NX");
    static int NYIx=e->KeywordIx("NY");
    static int XMINIx=e->KeywordIx("XMIN");
    static int YMINIx=e->KeywordIx("YMIN");
    static int NTHETAIx=e->KeywordIx("NTHETA");
    static int NRHOIx=e->KeywordIx("NRHO");
    static int RMINIx=e->KeywordIx("RMIN");
    static int BACKPROJECTIx=e->KeywordIx("BACKPROJECT");

    if( e->KeywordSet( DXIx))
      e->AssureFloatScalarKWIfPresent( DXIx, dx);	
    if( e->KeywordSet( DYIx))
      e->AssureFloatScalarKWIfPresent( DYIx, dy);
    if( e->KeywordSet( DRHOIx))
      e->AssureFloatScalarKWIfPresent( DRHOIx, drho);	

    DLong dims[2];


    if( e->KeywordSet( BACKPROJECTIx)) {

      DLong ntheta=p0->Dim(0);
      DLong nrho=p0->Dim(1);
      DFloat dtheta=fpi/ntheta;

      DLong nx=(DLong) floor(2*((drho*nrho/2)/sqrt(dx*dx + dy*dy))+1);
      DLong ny=nx;
      if( e->KeywordSet( NXIx))
	e->AssureLongScalarKWIfPresent( NXIx, nx);
      if( e->KeywordSet( NYIx))
	e->AssureLongScalarKWIfPresent( NYIx, ny);	

      dims[0] = nx;
      dims[1] = ny;

      dimension dim((DLong *) dims, 2);

      DFloat xmin = -0.5 * (nx-1);
      DFloat ymin = -0.5 * (ny-1);
//      DFloat xmax = +0.5 * (nx-1);
//      DFloat ymax = +0.5 * (ny-1);

      if( e->KeywordSet( YMINIx))
	e->AssureFloatScalarKWIfPresent( XMINIx, xmin);	
      if( e->KeywordSet( YMINIx))
	e->AssureFloatScalarKWIfPresent( YMINIx, ymin);	


      DFloatGDL* res = new DFloatGDL( dim, BaseGDL::NOZERO);
      for( SizeT i=0; i<nx*ny; ++i) (*res)[i] = 0;

      DFloat rhomin = -0.5 * (nrho-1) * drho;

      for( SizeT n=0; n<ny; ++n) {
	for( SizeT m=0; m<nx; ++m) {

	  DFloat theta=0;
	  for( SizeT itheta=0; itheta<ntheta; ++itheta) {
	    DFloat rho = ((m*dx+xmin)*cos(theta) +
			  (n*dy+ymin)*sin(theta) -
			  rhomin) / drho;
	    DLong indx = (DLong) round(rho);
	    //	    if (m ==63 && (n==0 || n==127))
	    // printf("%d %d\n", itheta,indx);
	    if (indx > 0 && indx < nrho)
	      (*res)[m+n*nx] += dtheta * (*p0F)[itheta+ntheta*indx];
	    theta += dtheta;
	  }
	}
      }
      return res;

    } else {
      // "Forward Radon Transform"

      DFloat maxr2[4], mrho2;

      DFloat xmin = -0.5 * (p0->Dim(0)-1);
      DFloat ymin = -0.5 * (p0->Dim(1)-1);
      DFloat xmax = +0.5 * (p0->Dim(0)-1);
      DFloat ymax = +0.5 * (p0->Dim(1)-1);

      if( e->KeywordSet( XMINIx))
	e->AssureFloatScalarKWIfPresent( XMINIx, xmin);	
      if( e->KeywordSet( YMINIx))
	e->AssureFloatScalarKWIfPresent( YMINIx, xmin);	

      maxr2[0] = xmin*xmin + ymin*ymin;
      maxr2[1] = xmin*xmin + ymax*ymax;
      maxr2[2] = xmax*xmax + ymin*ymin;
      maxr2[3] = xmax*xmax + ymax*ymax;
      mrho2 = maxr2[0];
      if (maxr2[1] > mrho2) mrho2 = maxr2[1];
      if (maxr2[2] > mrho2) mrho2 = maxr2[2];
      if (maxr2[3] > mrho2) mrho2 = maxr2[3];

      dims[0] = (DLong) ceil(fpi * sqrt(0.5*(p0->Dim(0)*p0->Dim(0) + 
					     p0->Dim(1)*p0->Dim(1))));
      dims[1] = 2 * (DLong) ceil(sqrt(mrho2)/drho) + 1;

      if( e->KeywordSet( NTHETAIx))
	e->AssureLongScalarKWIfPresent( NTHETAIx, dims[0]);
      if( e->KeywordSet( NRHOIx))
	e->AssureLongScalarKWIfPresent( NRHOIx, dims[1]);

      static int thetaIx = e->KeywordIx( "THETA"); 
      if( e->KeywordPresent( thetaIx)) {
	DFloatGDL* thetaKW = e->IfDefGetKWAs<DFloatGDL>( thetaIx);
	if (thetaKW != NULL) dims[0] = thetaKW->N_Elements();
      }

      DLong ntheta = dims[0];
      DLong nrho = dims[1];

      dimension dim((DLong *) dims, 2);

      DFloatGDL* res = new DFloatGDL( dim, BaseGDL::NOZERO);
      for( SizeT i=0; i<ntheta*nrho; ++i) (*res)[i] = 0;

      DFloat theta=0, dtheta=fpi/ntheta, cs, sn;
      DFloat rmin=-0.5*(nrho-1)*drho, rho;
      if( e->KeywordSet( RMINIx))
	e->AssureFloatScalarKWIfPresent( RMINIx, rmin);	


      for( SizeT itheta=0; itheta<ntheta; ++itheta) {
	//	printf("theta: %d %f\n", itheta, theta);
	sn = sin(theta);
	cs = cos(theta);

	if (sn > 1/sqrt(2.0)) {
	  DFloat a = -(dx/dy) * (cs/sn);
	  DFloat den = dy * sn;
	  DFloat num1 = xmin*cs + ymin*sn;
	  rho = rmin;
	  for( SizeT irho=0; irho<nrho; ++irho) {
	    DFloat b = (rho - num1) / den;
	    for( SizeT m=0; m<p0->Dim(0); ++m) {
	      DLong indx = (DLong) round(a*m+b);
	      if (indx < 0 || indx > (p0->Dim(1)-1)) continue;
	      (*res)[itheta+irho*ntheta] += (*p0F)[m+(p0->Dim(0))*indx];
	    }
	    rho += drho;
	    (*res)[itheta+irho*ntheta] *= dx/abs(sn);
	  } // irho loop
	} else {
	  DFloat a = -(dy/dx) * (sn/cs);
	  DFloat den = dx * cs;
	  DFloat num1 = xmin*cs + ymin*sn;
	  rho = rmin;
	  for( SizeT irho=0; irho<nrho; ++irho) {
	    DFloat b = (rho - num1) / den;
	    for( SizeT n=0; n<p0->Dim(1); ++n) {
	      DLong indx = (DLong) round(a*n+b);
	      if (indx < 0 || indx > (p0->Dim(0)-1)) continue;
	      //	    printf("indx: %d  n: %d  b: %f  rho: %f\n",indx,n,b,rho);
	      (*res)[itheta+irho*ntheta] += (*p0F)[n*(p0->Dim(0))+indx];
	    }
	    rho += drho;
	    (*res)[itheta+irho*ntheta] *= dy/abs(cs);
	  } // irho loop
	}
	theta += dtheta;
      } // itheta loop


      // Write rho array to KW
      static int rhoIx = e->KeywordIx( "RHO"); 
      if( e->WriteableKeywordPresent( rhoIx)) {
	BaseGDL** rhoKW = &e->GetTheKW( rhoIx);
	delete (*rhoKW);
	dimension dim((DLong *) &nrho, (SizeT) 1);
	*rhoKW = new DFloatGDL(dim, BaseGDL::NOZERO);
	for( SizeT irho=0; irho<nrho; ++irho)
	  (*(DFloatGDL*) *rhoKW)[irho] = rmin + irho*drho;
      }

      // If THETA KW present but variable doesn't exist then write theta array
      if( e->WriteableKeywordPresent( thetaIx)) {
	if (e->IfDefGetKWAs<DFloatGDL>( thetaIx) == NULL) {
	  dimension dim((DLong *) &ntheta, (SizeT) 1);
	  BaseGDL** thetaKW = &e->GetTheKW( thetaIx);
	  delete (*thetaKW);
	  *thetaKW = new DFloatGDL(dim, BaseGDL::NOZERO);
	for( SizeT itheta=0; itheta<ntheta; ++itheta)
	  (*(DFloatGDL*) *thetaKW)[itheta] = itheta*dtheta;
	}
      }	 

      return res;
    }
  }

} // namespace
