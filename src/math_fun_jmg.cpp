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

#include "datatypes.hpp"
#include "envt.hpp"
#include "dinterpreter.hpp"
#include "initsysvar.hpp"
#include "math_utl.hpp"
#include "math_fun_jmg.hpp"
#include "graphicsdevice.hpp"

//#define GDL_DEBUG
#undef GDL_DEBUG

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
//dirty patch to circumvent icc v.14.0.0 fatal error ("__builtin_signbit not found")
#ifdef __INTEL_COMPILER
#define signbit(d) (d < 0.0)? 1:0
#endif
namespace lib {

  using namespace std;
#ifndef _MSC_VER
  using std::isinf;
  using std::isnan;
#endif

#ifdef PL_HAVE_QHULL
extern "C" {
  //prevent qhull using its own memory tricks. Stay on the safe side.
    #define qh_NOmem 1
    #include <libqhull/qhull_a.h>
}
#endif

  BaseGDL* machar_fun( EnvT* e)
  {
    long int ibeta, it, irnd, ngrd, machep, negep, iexp, minexp, maxexp;
    float  eps, epsneg, xmin, xmax;
    double epsD, epsnegD, xminD, xmaxD;
    
    if( e->KeywordSet(0)) //DOUBLE
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
   template< typename T, bool> struct finite_helper
   {
     inline static BaseGDL* do_it(T* src, bool kwNaN, bool kwInfinity)
     {
       DByteGDL* res = new DByteGDL( src->Dim(), BaseGDL::NOZERO);
       SizeT nEl = src->N_Elements();
       if (kwNaN)
         for ( SizeT i=0; i<nEl; ++i) (*res)[ i] = isnan((*src)[ i]);
       else if (kwInfinity)
         for ( SizeT i=0; i<nEl; ++i) (*res)[ i] = isinf((*src)[ i]);
       else
         for ( SizeT i=0; i<nEl; ++i) (*res)[ i] = isfinite((*src)[ i]);
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
       if (kwNaN)
         for ( SizeT i=0; i<nEl; ++i) 
     	    (*res)[ i] = isnan((*src)[ i].real()) || isnan((*src)[ i].imag());
       else if (kwInfinity)
         for ( SizeT i=0; i<nEl; ++i)
           (*res)[ i] = isinf((*src)[ i].real()) || isinf((*src)[ i].imag());
       else
         for ( SizeT i=0; i<nEl; ++i)
           (*res)[ i] = isfinite((*src)[ i].real()) && 
                        isfinite((*src)[ i].imag());
       return res;
     }
   };

   template< typename T, bool IS_COMPLEX>
   inline BaseGDL* finite_template( BaseGDL* src, bool kwNaN, bool kwInfinity)
   {
     return finite_helper<T, IS_COMPLEX>::
       do_it(static_cast<T*>(src), kwNaN, kwInfinity);
   };

   template< typename T, bool> struct finite_helper_sign
   {
     inline static BaseGDL* do_it(T* src, bool kwNaN, bool kwInfinity, DLong kwSign)
     {

// #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))

       DByteGDL* res = new DByteGDL( src->Dim(), BaseGDL::ZERO); // ::ZERO is not working
       SizeT nEl = src->N_Elements();

       //       for ( SizeT i=0; i<nEl; ++i) (*res)[i]=0;

       if (kwInfinity) {
	 if (kwSign > 0) {
// #pragma omp for
	   for ( SizeT i=0; i<nEl; ++i) {
	     if (isinf((*src)[ i]) && (signbit((*src)[ i]) == 0)) (*res)[i]=1; 
	   }
	 } else {
// #pragma omp for
	   for ( SizeT i=0; i<nEl; ++i) {
	     if (isinf((*src)[ i]) && (signbit((*src)[ i]) != 0)) (*res)[i]=1; 
	   }
	 }
	 return res;	 
       }
       if (kwNaN) {
	 if (kwSign > 0) {
// #pragma omp for
	   for ( SizeT i=0; i<nEl; ++i) {
	     if (isnan((*src)[ i]) && (signbit((*src)[ i]) == 0)) (*res)[i]=1; 
	   }
	 } else {
// #pragma omp for
	   for ( SizeT i=0; i<nEl; ++i) {
	     if (isnan((*src)[ i]) && (signbit((*src)[ i]) != 0)) (*res)[i]=1; 
	   }
	 }
	 return res;
       }
      assert( false);
     }
   };

   // partial specialization for GDL_COMPLEX, DCOMPLEX
   template< typename T> struct finite_helper_sign<T, true>
   {
     inline static BaseGDL* do_it(T* src, bool kwNaN, bool kwInfinity, DLong kwSign)
     {
       DByteGDL* res = new DByteGDL( src->Dim(), BaseGDL::ZERO);
       SizeT nEl = src->N_Elements();
       
       for ( SizeT i=0; i<nEl; ++i)
	 {
	   if      ((kwInfinity && isinf((*src)[ i].real()) || kwNaN && isnan((*src)[ i].real())) && signbit((*src)[ i].real())==0 && kwSign > 0) (*res)[i]=1;
	   else if ((kwInfinity && isinf((*src)[ i].imag()) || kwNaN && isnan((*src)[ i].imag())) && signbit((*src)[ i].imag())==0 && kwSign > 0) (*res)[i]=1;
	   else if ((kwInfinity && isinf((*src)[ i].real()) || kwNaN && isnan((*src)[ i].real())) && signbit((*src)[ i].real())==1 && kwSign < 0) (*res)[i]=1;
	   else if ((kwInfinity && isinf((*src)[ i].imag()) || kwNaN && isnan((*src)[ i].imag())) && signbit((*src)[ i].imag())==1 && kwSign < 0) (*res)[i]=1;	 
	 }
       return res;
     }
   };

   template< typename T, bool IS_COMPLEX>
   inline BaseGDL* finite_template( BaseGDL* src, bool kwNaN, bool kwInfinity, DLong kwSign)
   {
     return finite_helper_sign<T, IS_COMPLEX>::
       do_it(static_cast<T*>(src), kwNaN, kwInfinity, kwSign);
   };

   BaseGDL* finite_fun( EnvT* e)
   {
     e->NParam( 1);

     BaseGDL* p0     = e->GetParDefined( 0);
     Guard<BaseGDL> guard;

     static int nanIx = e->KeywordIx( "NAN");
     bool kwNaN      = e->KeywordSet( nanIx);

     static int infinityIx = e->KeywordIx( "INFINITY");
     bool kwInfinity = e->KeywordSet( infinityIx);

     static int signIx = e->KeywordIx( "SIGN");
     DLong kwSign = 0;
     e->AssureLongScalarKWIfPresent( signIx, kwSign); 

     if( kwNaN && kwInfinity)
       e->Throw("Conflicting keywords.");
     
     if(kwSign==0 || (kwInfinity==0 && kwNaN==0))
       {
	 switch (p0->Type()) 
	   {
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
		 static_cast<DFloatGDL*>(p0->Convert2(GDL_FLOAT,BaseGDL::COPY));
	       guard.Reset( p0F);
	       return finite_template<DFloatGDL, false>(p0F, kwNaN, kwInfinity);
	     }
	   case GDL_STRUCT:
	   case GDL_PTR:
	   case GDL_OBJ:
	     {
	       e->Throw( p0->TypeStr() + " not allowed in this context: " +
			 e->GetParString( 0));
	     }
	   default: // integer types
	     {
	       if( kwNaN || kwInfinity)
		 return new DByteGDL( p0->Dim()); // zero
	       
	   DByteGDL* res = new DByteGDL( p0->Dim(), BaseGDL::NOZERO); 
	   SizeT nEl = p0->N_Elements();
	   for (SizeT i=0; i<nEl; i++)
	     (*res)[i] = 1;
	   return res;
	     }
	   }
       }
     // Sign
     else
       {
	 switch (p0->Type()) 
	   {
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
		 static_cast<DFloatGDL*>(p0->Convert2(GDL_FLOAT,BaseGDL::COPY));
	       guard.Reset( p0F);
	       return finite_template<DFloatGDL, false>(p0F, kwNaN, kwInfinity, kwSign);
	     }
	   case GDL_STRUCT:
	   case GDL_PTR:
	   case GDL_OBJ:
	     {
	       e->Throw( p0->TypeStr() + " not allowed in this context: " +
			 e->GetParString( 0));
	     }
	   default: // integer types
	     {
	       if( kwNaN || kwInfinity)
		 return new DByteGDL( p0->Dim()); // zero
	       
	       DByteGDL* res = new DByteGDL( p0->Dim(), BaseGDL::NOZERO);
	       SizeT nEl = p0->N_Elements();
	       for (SizeT i=0; i<nEl; i++)
	       (*res)[i] = 0;
	       return res;
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
      DFloat xmax = +0.5 * (nx-1);
      DFloat ymax = +0.5 * (ny-1);

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
      if( e->KeywordPresent( rhoIx)) {
	BaseGDL** rhoKW = &e->GetKW( rhoIx);
	delete (*rhoKW);
	dimension dim((DLong *) &nrho, (SizeT) 1);
	*rhoKW = new DFloatGDL(dim, BaseGDL::NOZERO);
	for( SizeT irho=0; irho<nrho; ++irho)
	  (*(DFloatGDL*) *rhoKW)[irho] = rmin + irho*drho;
      }

      // If THETA KW present but variable doesn't exist then write theta array
      if( e->KeywordPresent( thetaIx)) {
	if (e->IfDefGetKWAs<DFloatGDL>( thetaIx) == NULL) {
	  dimension dim((DLong *) &ntheta, (SizeT) 1);
	  BaseGDL** thetaKW = &e->GetKW( thetaIx);
	  delete (*thetaKW);
	  *thetaKW = new DFloatGDL(dim, BaseGDL::NOZERO);
	for( SizeT itheta=0; itheta<ntheta; ++itheta)
	  (*(DFloatGDL*) *thetaKW)[itheta] = itheta*dtheta;
	}
      }	 

      return res;
    }
  }

// see http://www.geom.umn.edu/software/qhull/. Used also with plplot.
#ifdef PL_HAVE_QHULL
  void    qh_errexit(int exitcode, facetT *facet, ridgeT *ridge)
  {
      ThrowGDLException("Qhull error.");
  }
void triangulate ( EnvT* e)
  {
    
    // Template 2016 by Reto Stockli
    // Todo: 
    // 1. Check input for missing values (NAN/FNAN)
    // 2. Return tr array from vertex loop at the end
    // 3. Implement keywords: [, B] [, CONNECTIVITY=variable] [, SPHERE=variable 
    //    [/DEGREES]] [, FVALUE=variable] [, REPEATS=variable] [, TOLERANCE=value]

    char hidden_options[]=" d n v H U Qb QB Qc Qf Qg Qi Qm Qr QR Qv Qx TR E V FC Fi Fo Ft Fp FV Q0 Q1 Q2 Q3 Q4 Q5 Q6 Q7 Q8 Q9 ";

    int DIMENSION = 2;
    int dimqhull = DIMENSION + 1;

    int i,j;

    coordT *points;

    facetT *facetlist;
    facetT *facet;

    setT *vertices;
    vertexT *vertex;
    vertexT **vertexp;

    char flags[25];
    boolT ismalloc;

    int numfacets;
    int numvertices;

    DDoubleGDL *yVal, *xVal;
    int npts;
    SizeT nParam=e->NParam();
    if( nParam < 3)
    {
      e->Throw("Incorrect number of arguments.");
    } else  {
	  e->AssureGlobalPar(2); //since we return values in it?  
	} 
    yVal = e->GetParAs< DDoubleGDL > (0);
    if (yVal->Rank() == 0) e->Throw("Expression must be an array in this context: " + e->GetParString(0));
    npts=yVal->N_Elements();
    xVal = e->GetParAs< DDoubleGDL > (1);
    if (xVal->Rank() == 0) e->Throw("Expression must be an array in this context: " + e->GetParString(1));
    if (xVal->N_Elements()!=npts) e->Throw("X & Y arrays must have same number of points.");

    /* init QHULL */
    sprintf (flags, "qdelaunay i Qt");
    qh_meminit(NULL);
    qh NOerrexit = False;
    qh_init_A(stdin, stdout, stderr, 0, NULL);
    qh_option("delaunay", NULL, NULL);
    qh DELAUNAY= True;     /* 'd'   */
    qh SCALElast= True;    /* 'Qbb' */
    qh KEEPcoplanar= True; /* 'Qc', to keep coplanars in 'p' */
    qh_checkflags(flags, hidden_options);
    qh_initflags(flags);

    /* assign X/Y coordinates to QHULL points structure */
    /* QHULL requires a vector of points including the squared sum of X/Y points */
    ismalloc=True;
    points= (coordT*)qh_malloc((npts)*(dimqhull)*sizeof(coordT));

    for (i=0;i<npts;i++) {
      points[i*3]   = (*yVal)[i]; 
      points[i*3+1] = (*xVal)[i];
      points[i*3+2] = (*yVal)[i] * (*yVal)[i] + (*xVal)[i] * (*xVal)[i];
    }

    /* run QHULL */
    qh_init_B(points, npts, dimqhull, ismalloc);
    qh_qhull();
    qh_check_output();
    qh_prepare_output();

    /* get QHULL output */
    facetlist = qh facet_list;
  
    numvertices = qh hull_dim;
    numfacets = 0;
    FORALLfacet_(facetlist) {
      if (!qh_skipfacet(facet)) numfacets++;
    }

    printf("# facets: %i; # vertices: %i \n",numfacets,numvertices);
    if (numfacets<1) e->Throw("Triangulation failed.");
    if (numvertices!=3) e->Throw("Invalid Number of Facets returned bu QHULL!");
    SizeT d[2];
    d[0]=numvertices;
    d[1]=numfacets;
    DLongGDL* returned_triangles=new DLongGDL(dimension(d,2), BaseGDL::NOZERO);

    {
      SizeT k=0;
      FORALLfacet_(facetlist) {
        if (!qh_skipfacet(facet)) {
          vertices = qh_facet3vertex(facet);
          FOREACHvertex_(vertices)
          (*returned_triangles)[k++]=qh_pointid(vertex->point);
        }
      }
    }
    //pass back to GDL env:
    e->SetPar(2, returned_triangles);
    /* free QHULL memory */
#ifdef qh_NOmem
  qh_freeqhull(qh_ALL);
#else
  qh_freeqhull(!qh_ALL);
#if 0
  int curlong, totlong; /* used !qh_NOmem */
  qh_memfreeshort(&curlong, &totlong);
  if (curlong || totlong)
    cerr<<"qhull internal warning (main): did not free "<<totlong<<" bytes of long memory("<<curlong<<" pieces)"<<endl;
#endif
#endif
  }
  void qhull ( EnvT* e)
  {
    e->Throw("Writing in progress.");
  }

  void grid_input (EnvT* e)
  {
    e->Throw("Writing in progress.");
  }

  BaseGDL* qgrid3_fun ( EnvT* e)
  {
    e->Throw("Writing in progress.");
  }
  BaseGDL* sph_scat_fun ( EnvT* e)
  {
    e->Throw("Writing in progress.");
  }

#endif
  
  template< typename T1, typename T2>
  BaseGDL* poly_2d_fun_template( DLong nCol, DLong nRow, image_t* warped)
  {
    dimension dim(nCol,nRow);  
//     dim.Set(0, nCol);
//     dim.Set(1, nRow);
    T1* res = new T1( dim, BaseGDL::NOZERO);
    for ( SizeT i=0; i<nCol*nRow; ++i) {
      int col = i / nRow;
      int row = i - col*nRow;
      (*res)[row*nCol+col] = (T2) warped->data[i];
    }
    image_del(warped);
    return res;
  }


  template< typename T1, typename T2>
  BaseGDL* poly_2d_shift_template( BaseGDL* p0, DLong nCol, DLong nRow, 
				   int shift_y, int shift_x, DDouble missing)
  {
    dimension dim(nCol,nRow);  
//     dim.Set(0, nCol);
//     dim.Set(1, nRow);
//    cout<<"shifting"<<endl;
    T1* res = new T1( dim, BaseGDL::NOZERO);
    T2 missed=missing;
    
    int lx = (int) p0->Dim(0);
    int ly = (int) p0->Dim(1);
    int lx_out = (int) nCol;
    int ly_out = (int) nRow;

    char *p_out = (char *) res->DataAddr();
    T2 *resAddr = (T2 *) res->DataAddr();
    for ( SizeT i=0; i<nCol*nRow; i++) {
      resAddr[i] = missed;
    }
    char *p_in  = (char *) p0->DataAddr();

    SizeT sz = sizeof(T2);
    for (SizeT j=0 ; j<ly ; j++) {
        for (SizeT i=0 ; i<lx ; i++) {
            if (((i-shift_x)>0) && ((i-shift_x)<lx_out) &&
                ((j-shift_y)>0) && ((j-shift_y)<ly_out)) {
	      memcpy((void *) &p_out[sz*((i-shift_x)+(j-shift_y)*lx_out)],
		     (void *) &p_in[sz*(i+j*lx)],
		     sz);
	    }
	}
    }

    return res;
  }


  BaseGDL* poly_2d_fun( EnvT* e)
  {
    /*
    The poly_2d code uses in large part, "ECLIPSE", the
    ESO C Library for an Image Processing Software Environment
 
    N. Devillard, "The eclipse software", The messenger No 87 - March 1997

    http://www.eso.org/projects/aot/eclipse/
    */

    SizeT nParam=e->NParam();
    if( nParam < 3)
      e->Throw( "Incorrect number of arguments.");

    BaseGDL* p0 = e->GetParDefined( 0);
    if (p0->Rank() != 2)
      e->Throw( "Array must have 2 dimensions: "+e->GetParString(0));

    BaseGDL* p1 = e->GetParDefined( 1);
    BaseGDL* p2 = e->GetParDefined( 2);

    DLong interp=0;
    if (nParam >= 4) e->AssureLongScalarPar( 3, interp);
    if (interp < 0 || interp > 2)
      e->Throw( "Value of Interpolation type is out of allowed range.: "+e->GetParString(0));

    if( nParam == 5)
      e->Throw( "Incorrect number of arguments.");

    DLong nCol = p0->Dim(0);
    DLong nRow = p0->Dim(1);
    if (nParam >= 6) {
      e->AssureLongScalarPar( 4, nCol);
      e->AssureLongScalarPar( 5, nRow);
    }
    static int CUBICIx=e->KeywordIx("CUBIC");
    DDouble cubic=-0.5;
    if( e->KeywordSet( CUBICIx)) {
      e->AssureDoubleScalarKWIfPresent( CUBICIx, cubic);	
      interp = 2;
    }
    
    static int MISSINGIx=e->KeywordIx("MISSING");
    DDouble missing=0.0;
    bool doMissing=( e->KeywordSet( MISSINGIx));
    if(doMissing) {
      e->AssureDoubleScalarKWIfPresent( MISSINGIx, missing);	
    }

    SizeT nEl;

    DDouble sqp1 = sqrt((DDouble) p1->N_Elements());
    DLong lsqp1 = (DLong) sqp1;
    DDouble sqp2 = sqrt((DDouble) p2->N_Elements());
    DLong lsqp2 = (DLong) sqp2;

    if (p1->N_Elements() == 1)
      e->Throw( "Value of Polynomial degree is out of allowed range.");
    if (sqp1 != lsqp1)
      e->Throw( "Value of Polynomial degree is out of allowed range.");

    if (p2->N_Elements() == 1)
      e->Throw( "Coefficient arrays must have (degree+1)^2 elements");
    if (sqp2 != lsqp2)
      e->Throw( "Coefficient arrays must have (degree+1)^2 elements");

    DLong nDegree = lsqp1 - 1;
    DLong nc = (nDegree + 1) * (nDegree + 1);

    DDoubleGDL* P = static_cast<DDoubleGDL*>
      (p1->Convert2( GDL_DOUBLE, BaseGDL::COPY));

    DDoubleGDL* Q = static_cast<DDoubleGDL*>
      (p2->Convert2( GDL_DOUBLE, BaseGDL::COPY));

    char kernel_name[32];
    kernel_name[0] = 0;
    if (interp == 1) strncpy(kernel_name, "linear", 31);
    if (interp == 2) strncpy(kernel_name, "cubic", 31);
    image_t* warped;


    DDouble lineartrans[6];
    poly2d* poly_u ;
    poly2d* poly_v ;

    if ((*P)[3] == 0 && (*Q)[3] == 0) {

      if ((*P)[2] == 1 && (*Q)[1] == 1 &&
	  (*P)[1] == 0 && (*Q)[2] == 0 &&
	  ( ((int) (*P)[0] == (*P)[0]) && ((int) (*Q)[0] == (*Q)[0]))) {
	// Translation 
	if (((int) (*P)[0] == 0) && ((int) (*Q)[0] == 0)) {
	  return p0->Dup();
	} else {
	  if (p0->Type() == GDL_BYTE) {
	    return poly_2d_shift_template< DByteGDL, DByte>( p0, nCol, nRow,  
							     (int) (*Q)[0], 
							     (int) (*P)[0],missing);
	  } else if (p0->Type() == GDL_INT) {
	    return poly_2d_shift_template< DIntGDL, DInt>( p0, nCol, nRow,  
							    (int) (*Q)[0], 
							    (int) (*P)[0],missing);
	  } else if (p0->Type() == GDL_UINT) {
	    return poly_2d_shift_template< DUIntGDL, DUInt>( p0, nCol, nRow,  
							     (int) (*Q)[0], 
							     (int) (*P)[0],missing);
	  } else if (p0->Type() == GDL_LONG) {
	    return poly_2d_shift_template< DLongGDL, DLong>( p0, nCol, nRow,  
							     (int) (*Q)[0], 
							     (int) (*P)[0],missing);
	  } else if (p0->Type() == GDL_ULONG) {
	    return poly_2d_shift_template< DULongGDL, DULong>( p0, nCol, nRow,  
							       (int) (*Q)[0], 
							       (int) (*P)[0],missing);
	  } else if (p0->Type() == GDL_LONG64) {
	    return poly_2d_shift_template< DLong64GDL, DLong64>( p0, nCol, nRow,  
								 (int) (*Q)[0], 
								 (int) (*P)[0],missing);
	  } else if (p0->Type() == GDL_ULONG64) {
	    return poly_2d_shift_template< DULong64GDL, DULong64>( p0, nCol, nRow,  
								   (int) (*Q)[0], 
								   (int) (*P)[0],missing);
	  } else if (p0->Type() == GDL_FLOAT) {
	    return poly_2d_shift_template< DFloatGDL, DFloat>( p0, nCol, nRow,  
							       (int) (*Q)[0], 
							       (int) (*P)[0],missing);
	  } else if (p0->Type() == GDL_DOUBLE) {
	    return poly_2d_shift_template< DDoubleGDL, DDouble>( p0, nCol, nRow,  
								 (int) (*Q)[0], 
								 (int) (*P)[0],missing);
	  }
	}
      } else {
	lineartrans[0] = (*Q)[1];
	lineartrans[1] = (*Q)[2];
	lineartrans[2] = (*Q)[0];
	lineartrans[3] = (*P)[1];
	lineartrans[4] = (*P)[2];
	lineartrans[5] = (*P)[0];

	warped = image_warp(p0->Dim(1), p0->Dim(0), nRow, nCol, p0->Type(), 
			    p0->DataAddr(), kernel_name,
			    lineartrans, NULL, NULL, //poly_v, poly_u,
			    interp, cubic, LINEAR, missing, doMissing);
      }
    } else {
      // Polynomial
      poly_u = (poly2d *) malloc(sizeof(poly2d));
      poly_u->nc = nc ;
      poly_u->px = (DLong *) malloc(nc * sizeof(DLong));
      poly_u->py = (DLong *) malloc(nc * sizeof(DLong));
      poly_u->c  = (DDouble *) malloc(nc * sizeof(DDouble));

      for ( SizeT i=0; i<(nDegree+1)*(nDegree+1); ++i) {
	poly_u->px[i] = i / (nDegree+1);
	poly_u->py[i] = i - (poly_u->px[i] * (nDegree+1));
	poly_u->c[i] = (*P)[poly_u->px[i]+(nDegree+1)*poly_u->py[i]];
      }

      poly_v = (poly2d *) malloc(sizeof(poly2d));
      poly_v->nc = nc ;
      poly_v->px = (DLong *) malloc(nc * sizeof(DLong));
      poly_v->py = (DLong *) malloc(nc * sizeof(DLong));
      poly_v->c  = (DDouble *) malloc(nc * sizeof(DDouble));

      for ( SizeT i=0; i<(nDegree+1)*(nDegree+1); ++i) {
	poly_v->px[i] = i / (nDegree+1);
	poly_v->py[i] = i - (poly_v->px[i] * (nDegree+1));
	poly_v->c[i] = (*Q)[poly_v->px[i]+(nDegree+1)*poly_v->py[i]];
      }

      warped = image_warp(p0->Dim(1), p0->Dim(0), nRow, nCol, p0->Type(), 
			  p0->DataAddr(), kernel_name, 
			  lineartrans, poly_v, poly_u, 
			  interp, cubic, GENERIC, missing, doMissing);

      if (poly_u->px != NULL) free(poly_u->px);
      if (poly_u->py != NULL) free(poly_u->py);
      if (poly_u->c  != NULL) free(poly_u->c);

      free(poly_u);

      if (poly_v->px != NULL) free(poly_v->px);
      if (poly_v->py != NULL) free(poly_v->py);
      if (poly_v->c  != NULL) free(poly_v->c);

      free(poly_v);
    }

    if (p0->Type() == GDL_BYTE) {
      for ( SizeT i=0; i<nCol*nRow; ++i) {
	if (warped->data[i] < 0)   warped->data[i] = 0;
	if (warped->data[i] > 255) warped->data[i] = 255;
      }
      return poly_2d_fun_template< DByteGDL, DByte>( nCol, nRow, warped);
    } else if (p0->Type() == GDL_INT) {
      return poly_2d_fun_template< DIntGDL, DInt>( nCol, nRow, warped);
    } else if (p0->Type() == GDL_UINT) {
      return poly_2d_fun_template< DUIntGDL, DUInt>( nCol, nRow, warped);
    } else if (p0->Type() == GDL_LONG) {
      return poly_2d_fun_template< DLongGDL, DLong>( nCol, nRow, warped);
    } else if (p0->Type() == GDL_ULONG) {
      return poly_2d_fun_template< DULongGDL, DULong>( nCol, nRow, warped);
    } else if (p0->Type() == GDL_LONG64) {
      return poly_2d_fun_template< DLong64GDL, DLong64>( nCol, nRow, warped);
    } else if (p0->Type() == GDL_ULONG64) {
      return poly_2d_fun_template< DULong64GDL, DULong64>( nCol, nRow, warped);
    } else if (p0->Type() == GDL_FLOAT) {
      return poly_2d_fun_template< DFloatGDL, DFloat>( nCol, nRow, warped);
    } else if (p0->Type() == GDL_DOUBLE) {
      return poly_2d_fun_template< DDoubleGDL, DDouble>( nCol, nRow, warped);
    }
    e->Throw("Unhandled type: "+i2s(p0->Type()));
    return NULL;

  }


/*-------------------------------------------------------------------------*/
/**
  @brief        Same as pow(x,y) but for integer values of y only (faster).
  @param        x       A double number.
  @param        p       An integer power.
  @return       x to the power p.

  This is much faster than the math function due to the integer. Some
  compilers make this optimization already, some do not.

  p can be positive, negative or null.
 */
/*--------------------------------------------------------------------------*/
double ipow(double x, int p)
{
        double r, recip ;

        /* Get rid of trivial cases */
        switch (p) {
                case 0:
                return 1.00 ;

                case 1:
                return x ;

                case 2:
                return x*x ;

                case 3:
                return x*x*x ;

                case -1:
                return 1.00 / x ;

                case -2:
                return (1.00 / x) * (1.00 / x) ;
        }
        if (p>0) {
                r = x ;
                while (--p) r *= x ;
        } else {
                r = recip = 1.00 / x ;
                while (++p) r *= recip ;
        }
        return r;
}

/*-------------------------------------------------------------------------*/
/**
  @brief	Compute the value of a poly2d at a given point.
  @param	p	Poly2d object.
  @param	x	x coordinate.
  @param	y	y coordinate.
  @return	The value of the 2d polynomial at (x,y) as a double.

  This computes the value of a poly2d in a single point. To
  compute many values in a row, see poly2d_compute_array().
 */
/*--------------------------------------------------------------------------*/
double poly2d_compute(
		poly2d	*	p,
		double		x,
		double		y)
{
	double	z ;
	int		i ;

	z = 0.00 ;
	for (i=0 ; i<p->nc ; i++) {
		z += p->c[i] * ipow(x, p->px[i]) * ipow(y, p->py[i]) ;
	}
	return z ;
}

/*-------------------------------------------------------------------------*/
/**
  @brief	Cardinal sine.
  @param	x	double value.
  @return	1 double.

  Compute the value of the function sinc(x)=sin(pi*x)/(pi*x) at the
  requested x.
 */
/*--------------------------------------------------------------------------*/
double sinc(double x)
{
    if (fabs(x)<1e-4)
        return (double)1.00 ;
    else
        return ((sin(x * (double)PI_NUMB)) / (x * (double)PI_NUMB)) ;
}



/*-------------------------------------------------------------------------*/
/**
  @brief	Generate an interpolation kernel to use in this module.
  @param	kernel_type		Type of interpolation kernel.
  @return	1 newly allocated array of doubles.

  Provide the name of the kernel you want to generate. Supported kernel
  types are:

  \begin{tabular}{ll}
  NULL			&	default kernel, currently "tanh" \\
  "default"		&	default kernel, currently "tanh" \\
  "tanh"		&	Hyperbolic tangent \\
  "sinc2"		&	Square sinc \\
  "lanczos"		&	Lanczos2 kernel \\
  "hamming"		&	Hamming kernel \\
  "hann"		&	Hann kernel
  \end{tabular}

  The returned array of doubles is ready of use in the various re-sampling
  functions in this module. It must be deallocated using free().
 */
/*--------------------------------------------------------------------------*/
double * generate_interpolation_kernel(char * kernel_type, DDouble cubic)
{
    double  *	tab ;
    int     	i ;
    double  	x ;
    double		alpha ;
    double		inv_norm ;
    int     	samples = KERNEL_SAMPLES ;

	if (kernel_type==NULL) {
	  //		tab = generate_interpolation_kernel("tanh") ;
	} else if (!strcmp(kernel_type, "linear")) {
	  /*
	    Taken from "Image Reconstruction By Piecewise Polynomial Kernels", 
	    Meijering et al
	  */

	  tab = (double *) malloc(samples * sizeof(double)) ;
	  tab[0] = 1.0 ;
	  tab[samples-1] = 0.0 ;
	  for (i=1 ; i<samples ; i++) {
	    x = (double)KERNEL_WIDTH * (double)i/(double)(samples-1) ;
	    if (x < 1)
	      tab[i] = -x + 1;
	    else if (x >= 1)
	      tab[i] = 0;
	  }
	} else if (!strcmp(kernel_type, "cubic")) {
	  /*
	    Taken from "Image Reconstruction By Piecewise Polynomial Kernels", 
	    Meijering et al
	  */

	  tab = (double *) malloc(samples * sizeof(double)) ;
	  tab[0] = 1.0 ;
	  tab[samples-1] = 0.0 ;
	  for (i=1 ; i<samples ; i++) {
	    x = (double)KERNEL_WIDTH * (double)i/(double)(samples-1) ;
	    if (x < 1)
	      tab[i] = (cubic+2)*ipow(x,3) - (cubic+3)*ipow(x,2) + 1;
	    else if (x >= 1 && x < 2)
	      tab[i] = cubic*ipow(x,3) - 
		(5*cubic)*ipow(x,2) + (8*cubic)*x - (4*cubic);
	  }
	} else if (!strcmp(kernel_type, "sinc")) {
	  tab = (double *) malloc(samples * sizeof(double)) ;
	  tab[0] = 1.0 ;
	  tab[samples-1] = 0.0 ;
	  for (i=1 ; i<samples ; i++) {
	    x = (double)KERNEL_WIDTH * (double)i/(double)(samples-1) ;
	    tab[i] = sinc(x) ;
	  }
	} else {
	  //		e_error("unrecognized kernel type [%s]: aborting generation",
	  //			kernel_type) ;
		return NULL ;
	}
    return tab ;
}


/*-------------------------------------------------------------------------*/
/**
  @brief	Warp an image according to a polynomial transformation.
  @param	image_in		Image to warp.
  @param	kernel_type		Interpolation kernel to use.
  @param	poly_u			Polynomial transform in U.
  @param	poly_v			Polynomial transform in V.
  @return	1 newly allocated image.

  Warp an image according to a polynomial transform. Provide two
  polynomials (see poly2d.h for polynomials in this library) Pu and Pv such
  as:

  \begin{verbatim}
  x = poly2d_compute(Pu, u, v)
  y = poly2d_compute(Pv, u, v)
  \end{verbatim}

  Attention! The polynomials define a reverse transform. (u,v) are
  coordinates in the warped image and (x,y) are coordinates in the original
  image. The transform you provide is used to compute from the warped
  image, which pixels contributed in the original image.

  The output image will have strictly the same size as in the input image.
  Beware that for extreme transformations, this might lead to blank images
  as result.

  See the function generate_interpolation_kernel() for possible kernel
  types. If you want to use a default kernel, provide NULL for kernel type.

  The returned image is a newly allocated objet, use image_del() to
  deallocate it.

 */
/*--------------------------------------------------------------------------*/

  image_t * image_warp(
      SizeT lx,
      SizeT ly,
      SizeT lx_out,
      SizeT ly_out,
      DType type,
      void* data,
      char * kernel_type,
      DDouble *param,
      poly2d * poly_u,
      poly2d * poly_v,
      DLong interp,
      DDouble cubic,
      DLong warpType,
      DDouble initvalue,
      bool doMissing) {
    image_t * image_out;
    int i, j, k;
    double cur;
    double neighbors[16];
    double rsc[8],
        sumrs;
    double x, y;
    int px, py;
    int pos;
    int tabx, taby;
    double * kernel = NULL;
    int leaps[16];

    DByte data_b;
    DInt data_i;
    DUInt data_ui;
    DLong data_l;
    DULong data_ul;
    DLong64 data_l64;
    DULong64 data_ul64;
    float data_f;
    double data_d;
    char *ptr = (char *) data;

    /* Generate linear interpolation kernel if necessary */
    if (interp == 1) {
      kernel = generate_interpolation_kernel(kernel_type, (double) 0.0);
      if (kernel == NULL) {
        //        e_error("cannot generate kernel: aborting resampling") ;
        return NULL;
      }
    }

    /* Generate cubic interpolation kernel if necessary */
    if (interp == 2) {
      kernel = generate_interpolation_kernel(kernel_type, cubic);
      if (kernel == NULL) {
        //        e_error("cannot generate kernel: aborting resampling") ;
        return NULL;
      }
    }

    image_out = image_new(lx_out, ly_out, initvalue);

    /* Pre compute leaps for 16 closest neighbors positions */

    leaps[0] = -1 - lx;
    leaps[1] = -lx;
    leaps[2] = 1 - lx;
    leaps[3] = 2 - lx;

    leaps[4] = -1;
    leaps[5] = 0;
    leaps[6] = 1;
    leaps[7] = 2;

    leaps[8] = -1 + lx;
    leaps[9] = lx;
    leaps[10] = 1 + lx;
    leaps[11] = 2 + lx;

    leaps[12] = -1 + 2 * lx;
    leaps[13] = 2 * lx;
    leaps[14] = 1 + 2 * lx;
    leaps[15] = 2 + 2 * lx;

    for (k = 0; k < 16; k++) neighbors[k] = 0;

    /* Double loop on the output image  */
    for (j = 0; j < ly_out; j++) {
      for (i = 0; i < lx_out; i++) {
        /* Compute the original source for this pixel   */

        if (warpType == LINEAR) {
          x = param[0] * (double) i + param[1] * (double) j + param[2];
          y = param[3] * (double) i + param[4] * (double) j + param[5];
        } else {
          x = poly2d_compute(poly_u, (double) i, (double) j);
          y = poly2d_compute(poly_v, (double) i, (double) j);
        }

        /* Which is the closest integer positioned neighbor?    */
        px = (int) x;
        py = (int) y;

        if (doMissing) {
          if ((px < 1) ||
              (px > (lx - 1)) ||
              (py < 1) ||
              (py > (ly - 1))) {
            continue; // already initialised to 'missing' value. No need to put zero here.
          }
        }
        if ((px < 1) || (px > (lx - 1)) || (py < 1) || (py > (ly - 1))) {
          if (px < 1) px = 0;
          if (px > (lx - 1)) px = (lx - 1);
          if (py < 1) py = 0;
          if (py > (ly - 1)) py = (ly - 1);
          pos = px + py * lx;
          int row = (pos) / lx;
          int col = (pos) - row*lx;
          if (type == GDL_BYTE) {
            memcpy(&data_b, &ptr[sizeof (char)*(col * ly + row)],
                sizeof (char));
            neighbors[5] = (double) data_b;
          }
          if (type == GDL_INT) {
            memcpy(&data_i, &ptr[sizeof (DInt)*(col * ly + row)],
                sizeof (DInt));
            neighbors[5] = (double) data_i;
          }
          if (type == GDL_UINT) {
            memcpy(&data_ui, &ptr[sizeof (DUInt)*(col * ly + row)],
                sizeof (DUInt));
            neighbors[5] = (double) data_ui;
          }
          if (type == GDL_LONG) {
            memcpy(&data_l, &ptr[sizeof (DLong)*(col * ly + row)],
                sizeof (DLong));
            neighbors[5] = (double) data_l;
          }
          if (type == GDL_ULONG) {
            memcpy(&data_ul, &ptr[sizeof (DULong)*(col * ly + row)],
                sizeof (DULong));
            neighbors[5] = (double) data_ul;
          }
          if (type == GDL_LONG64) {
            memcpy(&data_l64, &ptr[sizeof (DLong64)*(col * ly + row)],
                sizeof (DLong64));
            neighbors[5] = (double) data_l64;
          }
          if (type == GDL_ULONG64) {
            memcpy(&data_ul64, &ptr[sizeof (DULong64)*(col * ly + row)],
                sizeof (DULong64));
            neighbors[5] = (double) data_ul64;
          }
          if (type == GDL_FLOAT) {
            memcpy(&data_f, &ptr[sizeof (float)*(col * ly + row)],
                sizeof (float));
            neighbors[5] = (double) data_f;
          }
          if (type == GDL_DOUBLE) {
            memcpy(&data_d, &ptr[sizeof (double)*(col * ly + row)],
                sizeof (double));
            neighbors[5] = data_d;
          }
          image_out->data[i + j * lx_out] = (pixelvalue) neighbors[5];
        } else {
          /* Now feed the positions for the closest 16 neighbors  */
          pos = px + py * lx;
          for (k = 0; k < 16; k++) {

            if (interp == 0 && k != 5) continue;

            int row = (pos + leaps[k]) / lx;
            int col = (pos + leaps[k]) - row*lx;
            if (type == GDL_BYTE) {
              memcpy(&data_b, &ptr[sizeof (char)*(col * ly + row)],
                  sizeof (char));
              neighbors[k] = (double) data_b;
            }
            if (type == GDL_INT) {
              memcpy(&data_i, &ptr[sizeof (DInt)*(col * ly + row)],
                  sizeof (DInt));
              neighbors[k] = (double) data_i;
            }
            if (type == GDL_UINT) {
              memcpy(&data_ui, &ptr[sizeof (DUInt)*(col * ly + row)],
                  sizeof (DUInt));
              neighbors[k] = (double) data_ui;
            }
            if (type == GDL_LONG) {
              memcpy(&data_l, &ptr[sizeof (DLong)*(col * ly + row)],
                  sizeof (DLong));
              neighbors[k] = (double) data_l;
            }
            if (type == GDL_ULONG) {
              memcpy(&data_ul, &ptr[sizeof (DULong)*(col * ly + row)],
                  sizeof (DULong));
              neighbors[k] = (double) data_ul;
            }
            if (type == GDL_LONG64) {
              memcpy(&data_l64, &ptr[sizeof (DLong64)*(col * ly + row)],
                  sizeof (DLong64));
              neighbors[k] = (double) data_l64;
            }
            if (type == GDL_ULONG64) {
              memcpy(&data_ul64, &ptr[sizeof (DULong64)*(col * ly + row)],
                  sizeof (DULong64));
              neighbors[k] = (double) data_ul64;
            }
            if (type == GDL_FLOAT) {
              memcpy(&data_f, &ptr[sizeof (float)*(col * ly + row)],
                  sizeof (float));
              neighbors[k] = (double) data_f;
            }
            if (type == GDL_DOUBLE) {
              memcpy(&data_d, &ptr[sizeof (double)*(col * ly + row)],
                  sizeof (double));
              neighbors[k] = data_d;
            }
          }

          if (interp == 0) {
            image_out->data[i + j * lx_out] = (pixelvalue) neighbors[5];
          } else if (interp == 1) {
            /* Which tabulated value index shall we use?    */
            tabx = (int) ((x - (double) px) * (double) (TABSPERPIX));
            taby = (int) ((y - (double) py) * (double) (TABSPERPIX));

            /* Compute resampling coefficients  */
            /* rsc[0..3] in x, rsc[4..7] in y   */

            rsc[0] = kernel[TABSPERPIX + tabx];
            rsc[1] = kernel[tabx];
            rsc[2] = kernel[TABSPERPIX - tabx];
            rsc[4] = kernel[TABSPERPIX + taby];
            rsc[5] = kernel[taby];
            rsc[6] = kernel[TABSPERPIX - taby];

            sumrs = (rsc[0] + rsc[1] + rsc[2]) *
                (rsc[4] + rsc[5] + rsc[6]);

            /* Compute interpolated pixel now   */
            if ((x - (double) px) < 0 && (y - (double) py) < 0) {
              cur = rsc[4] * (rsc[0] * neighbors[0] +
                  rsc[1] * neighbors[1]) +
                  rsc[5] * (rsc[0] * neighbors[4] +
                  rsc[1] * neighbors[5]);
            } else if ((x - (double) px) >= 0 && (y - (double) py) < 0) {
              cur = rsc[4] * (rsc[1] * neighbors[1] +
                  rsc[2] * neighbors[2]) +
                  rsc[5] * (rsc[1] * neighbors[5] +
                  rsc[2] * neighbors[6]);
            } else if ((x - (double) px) < 0 && (y - (double) py) >= 0) {
              cur = rsc[5] * (rsc[0] * neighbors[4] +
                  rsc[1] * neighbors[5]) +
                  rsc[6] * (rsc[0] * neighbors[8] +
                  rsc[1] * neighbors[9]);
            } else if ((x - (double) px) >= 0 && (y - (double) py) >= 0) {
              cur = rsc[5] * (rsc[1] * neighbors[5] +
                  rsc[2] * neighbors[6]) +
                  rsc[6] * (rsc[1] * neighbors[9] +
                  rsc[2] * neighbors[10]);
            }

            /* Affect the value to the output image */
            image_out->data[i + j * lx_out] = (pixelvalue) (cur / sumrs);
            /* done ! */
          } else {
            /* Which tabulated value index shall we use?    */
            tabx = (int) ((x - (double) px) * (double) (TABSPERPIX));
            taby = (int) ((y - (double) py) * (double) (TABSPERPIX));

            /* Compute resampling coefficients  */
            /* rsc[0..3] in x, rsc[4..7] in y   */

            rsc[0] = kernel[TABSPERPIX + tabx];
            rsc[1] = kernel[tabx];
            rsc[2] = kernel[TABSPERPIX - tabx];
            rsc[3] = kernel[2 * TABSPERPIX - tabx];
            rsc[4] = kernel[TABSPERPIX + taby];
            rsc[5] = kernel[taby];
            rsc[6] = kernel[TABSPERPIX - taby];
            rsc[7] = kernel[2 * TABSPERPIX - taby];

            sumrs = (rsc[0] + rsc[1] + rsc[2] + rsc[3]) *
                (rsc[4] + rsc[5] + rsc[6] + rsc[7]);

            /* Compute interpolated pixel now   */
            cur = rsc[4] * (rsc[0] * neighbors[0] +
                rsc[1] * neighbors[1] +
                rsc[2] * neighbors[2] +
                rsc[3] * neighbors[3]) +
                rsc[5] * (rsc[0] * neighbors[4] +
                rsc[1] * neighbors[5] +
                rsc[2] * neighbors[6] +
                rsc[3] * neighbors[7]) +
                rsc[6] * (rsc[0] * neighbors[8] +
                rsc[1] * neighbors[9] +
                rsc[2] * neighbors[10] +
                rsc[3] * neighbors[11]) +
                rsc[7] * (rsc[0] * neighbors[12] +
                rsc[1] * neighbors[13] +
                rsc[2] * neighbors[14] +
                rsc[3] * neighbors[15]);

            /* Affect the value to the output image */
            image_out->data[i + j * lx_out] = (pixelvalue) (cur / sumrs);
            /* done ! */
          }
        }
      }
    }

    if (kernel != NULL) free(kernel);
    return image_out;
  }


/*-------------------------------------------------------------------------*/
/**
  @brief    Allocate an image structure and pixel buffer for an image.
  @param    size_x  Size in x
  @param    size_y  Size in y
  @return   1 newly allocated image.
 
  Allocates both space for the image structure and the pixel buffer. The
  returned pixel buffer is always seen as if it were in memory.
 
  The returned image must be deallocated using image_del().
 */
/*--------------------------------------------------------------------------*/
image_t * image_new(
		int 	size_x, 
		int 	size_y,
                DDouble  initvalue)
{
    image_t    *	image_new ;

    if ((size_x<1) ||
        (size_x>MAX_COLUMN_NUMBER) || 
        (size_y<1) || 
        (size_y>MAX_LINE_NUMBER)) {
      //        e_error("cannot create image with size [%dx%d]", size_x, size_y) ;
        return NULL ;
    }

    image_new = (image_t *) calloc(1, sizeof(image_t)) ;

    image_new->lx = size_x ;
    image_new->ly = size_y ;
    image_new->data = (pixelvalue *) calloc(size_x * size_y, sizeof(pixelvalue));
 
    for (SizeT i=0; i < size_x * size_y; i++) (image_new->data)[i]=initvalue;
    
    return image_new ;
}


/*-------------------------------------------------------------------------*/
/**
  @brief	Free memory associated to an image object.
  @param	d	Image to destroy.
  @return	void

  Frees all memory associated to an image.
 */
/*--------------------------------------------------------------------------*/
void image_del(image_t * d)
{
    if (d == NULL) return ;
	if (d->data != NULL) {
		free(d->data) ;
	}
	free(d) ;
}

// used in rk4jmg_fun below only
//  void executeString( EnvBaseT* caller, istringstream *istr)
//  {
//    // P.S:  I don't know how this works.  Ask Marc.
//
//    RefDNode theAST;
//    GDLLexer lexer(*istr, "",GDLParser::NONE);
//    GDLParser& parser = lexer.Parser();
//    parser.interactive();
//    theAST = parser.getAST();
//    RefDNode trAST;
//    GDLTreeParser treeParser( caller);
//    treeParser.interactive(theAST);
//    trAST = treeParser.getAST();
//    ProgNodeP progAST = ProgNode::NewProgNode( trAST);
//    Guard< ProgNode> progAST_guard( progAST);
//
//	// Marc: necessary for correct FOR loop handling
//	assert( dynamic_cast<EnvUDT*>(caller) != NULL);
//    EnvUDT* env = static_cast<EnvUDT*>(caller);
//    int nForLoopsIn = env->NForLoops();
//    int nForLoops = ProgNode::NumberForLoops( progAST, nForLoopsIn);
//	env->ResizeForLoops( nForLoops);
//
//    RetCode retCode = caller->Interpreter()->execute( progAST);
//  
//  	env->ResizeForLoops( nForLoopsIn);
//}


//  BaseGDL* rk4jmg_fun(EnvT* e)
//  {  
//    // Get current DOY HR MN SEC and form unique tag
//    struct tm *curtime;
//    time_t bintime;
//    char time_str[128];
//    time( &bintime);
//    curtime = gmtime( &bintime);
//    strftime( time_str, 128, "%j%H%M%S", curtime);
//    DString tag = time_str;
//
//    SizeT nParam = e->NParam(5);
//    // Result = RK4( Y, Dydx, X, H, Derivs [, /GDL_DOUBLE] )
//
//    BaseGDL* par = e->GetParDefined( 0);
//
//    // Get # of y-elements
//    SizeT nEy = par->N_Elements();
//    //    cout << "nEy: " << nEy << endl;
//
//    bool doubleFlag = false;
//    if ( par->Type() == GDL_DOUBLE) doubleFlag = true;
//    static int DOUBLEIx = e->KeywordIx("DOUBLE");
//    if ( e->KeywordSet( DOUBLEIx)) doubleFlag = true;
//
//    DString retTypeString;
//    if ( doubleFlag) 
//      retTypeString = "DOUBLE (";
//    else
//      retTypeString = "FLOAT (";
//    //    cout << doubleFlag << endl;
//
//    // Get Name of GDL-code derivative function
//    DString derivName;
//    e->AssureScalarPar<DStringGDL>( 4, derivName); 
//
//
//    // Check if GDL-code derivative function exist
//    vector<DString> fList;
//    bool found = false;
//    for( FunListT::iterator i=funList.begin(); i != funList.end(); i++) {
//      fList.push_back((*i)->ObjectName());
//    }
//    for( SizeT i = 0; i<funList.size(); ++i) {
//	if ( StrUpCase(derivName) == fList[ i]) {
//	  found = true;
//	  break;
//	}
//    }
//    if ( found == false) {
//      return NULL;
//    }
//
//
//    ostringstream ostr;
//    int xI;
//
//    // Get current level of calling stack
//    EnvStackT& callStack = e->Interpreter()->CallStack();
//    DLong curlevnum = callStack.size()-1;
//
//    // Get calling procedure
//    DSubUD* pro = static_cast<DSubUD*>(callStack[curlevnum-1]->GetPro());
//
//    // Get total number of current variables & keywords at calling level
//    SizeT nVar = pro->Size();
//    SizeT nKey = pro->NKey();
//
//    // Get GDL variable names for y, dydx, x, h
//    DString varName[4];
//    DLong nTemp = 0;
//    // For each RK4 function parameter 
//    for( SizeT i = 0; i<4; ++i) {
//      BaseGDL *val = e->GetPar( i);
//      // Loop through all variables
//      found = false;
//      for( SizeT j = 0; j<nVar; ++j) {
//	par = ((EnvT*)(callStack[curlevnum-1]))->GetPar( j-nKey);
//
//	// If match then get and save variable name
//	if ( par == val) {
//	  varName[i] = pro->GetVarName( j);
//	  found = true;
//	  break;
//	}
//      }
//
//      // If variable name not found then make temp variable
//      if ( !found) {
//	if ( i == 0) varName[i] = "TEMP_Y_" + tag;
//	if ( i == 1) varName[i] = "TEMP_DYDX_" + tag;
//	if ( i == 2) varName[i] = "TEMP_X_" + tag;
//	if ( i == 3) varName[i] = "TEMP_H_" + tag;
//
//	SizeT u = pro->AddVar(StrUpCase(varName[i]));
//	SizeT s = callStack[curlevnum-1]->AddEnv();
//
//	BaseGDL*& par = ((EnvT*)(callStack[curlevnum-1]))->GetPar( s-nKey);
//	BaseGDL* res = e->GetPar( i)->Dup();
//	memcpy(&par, &res, 4);
//
//	nTemp++;
//      }
//    }
//
//
//    // Form input variable names
//    DString Y    = varName[0];
//    DString DYDX = varName[1];
//    DString X    = varName[2];
//    DString H    = varName[3];
//
//    DString H_2    = "0.5 * "        + varName[3];
//    DString H_1_6  = "(1.D0 / 6) * " + varName[3];
//
//    // Form output variable names
//    DString varName_K2 = "K2_" + tag;
//    DString varName_K3 = "K3_" + tag;
//    DString varName_K4 = "K4_" + tag;
//    DString varName_Y1 = "Y1_" + tag;
//
//    // Build execution string for: k2 = f(x + h/2, y + h * k1/2)
//    ostr << varName_K2.c_str() << " = " << derivName.c_str() << "(" ;
//    ostr << X.c_str() << " + " << H_2.c_str() << ", ";
//    ostr << Y.c_str() << " + " << H_2.c_str() << " * " << DYDX.c_str();
//    ostr << ") & ";
//
//    // Build execution string for: k3 = f(x + h/2, y + h * k2/2)
//    ostr << varName_K3.c_str() << " = " << derivName.c_str() << "(" ;
//    ostr << X.c_str() << " + " << H_2.c_str() << ", ";
//    ostr << Y.c_str() << " + " << H_2.c_str() << " * " << varName_K2.c_str();
//    ostr << ") & ";
//
//    // Build execution string for: k4 = f(x + h, y + h * k3)
//    ostr << varName_K4.c_str() << " = " << derivName.c_str() << "(" ;
//    ostr << X.c_str() << " + " << H.c_str() << ", ";
//    ostr << Y.c_str() << " + " << H.c_str() << " * " << varName_K3.c_str();
//    ostr << ") & ";
//
//    // Build exec string for: yi+1 = yi + (1/6) * h * [k1 + 2k2 + 2k3 + k4]
//    ostr << varName_Y1.c_str() << " = " << retTypeString.c_str() << Y.c_str();
//    ostr << " + " << H_1_6.c_str() << " * (";
//    ostr << DYDX.c_str() << " + 2 * " << varName_K2.c_str(); 
//    ostr << " + 2 * " << varName_K3.c_str() << " + " << varName_K4.c_str();
//    ostr << ") )";
//
//    DString line = ostr.rdbuf()->str();
//    istringstream istr;
//    istr.str(line+"\n");
//    //    cout << line.c_str() << endl;
//
//    // Execute command string
//    EnvBaseT* caller;
//    caller = e->Caller();
//// ms: commented out to comply with new stack handling
////     e->Interpreter()->CallStack().pop_back();
//
//    executeString( caller, &istr);
//
//    // Retrieve return values for Y_N+1
//    xI = pro->FindVar(StrUpCase( varName_Y1));
//    par = ((EnvT*)(callStack[curlevnum-1]))->GetPar( xI-nKey);
//
//    DFloatGDL*  res_f;
//    DDoubleGDL* res_d;
//
//    if ( doubleFlag) {
//      res_d = new DDoubleGDL(nEy, BaseGDL::NOZERO);
//
//      for( SizeT i = 0; i<nEy; ++i) {
//	(*res_d)[i] = (*(DDoubleGDL*) par)[i];
//	//	cout << "y1 " << i << ": " << (*res_d)[i] << endl;
//      }
//    } else {
//      res_f = new DFloatGDL(nEy, BaseGDL::NOZERO);
//
//      for( SizeT i = 0; i<nEy; ++i) {
//	(*res_f)[i] = (*(DFloatGDL*) par)[i];
//	//	cout << "y1 " << i << ": " << (*res_f)[i] << endl;
//      }
//    }
//
//    // Delete temporary output variables
//    for( SizeT i = 0; i<nTemp; ++i) {
//      pro->DelVar( xI--);
//      callStack[curlevnum-1]->DelEnv();
//    }
//
//    pro->DelVar( xI--);
//    callStack[curlevnum-1]->DelEnv();
//
//    pro->DelVar( xI--);
//    callStack[curlevnum-1]->DelEnv();
//
//    pro->DelVar( xI--);
//    callStack[curlevnum-1]->DelEnv();
//
//    pro->DelVar( xI);
//    callStack[curlevnum-1]->DelEnv();
//
//    if ( doubleFlag) 
//      return res_d;
//    else
//      return res_f;
//  }

} // namespace
