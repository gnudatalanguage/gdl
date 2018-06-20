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

	 #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	 {
       if (kwNaN){
		#pragma omp for
         for ( SizeT i=0; i<nEl; ++i) (*res)[ i] = isnan((*src)[ i]);
       }
       else if (kwInfinity){
		#pragma omp for
         for ( SizeT i=0; i<nEl; ++i) (*res)[ i] = isinf((*src)[ i]);
       }
       else{
		#pragma omp for
         for ( SizeT i=0; i<nEl; ++i) (*res)[ i] = isfinite((*src)[ i]);
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
	 #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	 {
       if (kwNaN){
		#pragma omp for
         for ( SizeT i=0; i<nEl; ++i) 
     	    (*res)[ i] = isnan((*src)[ i].real()) || isnan((*src)[ i].imag());
	   }
       else if (kwInfinity){
		#pragma omp for
         for ( SizeT i=0; i<nEl; ++i)
           (*res)[ i] = isinf((*src)[ i].real()) || isinf((*src)[ i].imag());
	   }
       else{
		#pragma omp for
         for ( SizeT i=0; i<nEl; ++i)
           (*res)[ i] = isfinite((*src)[ i].real()) && 
                        isfinite((*src)[ i].imag());
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
   template< typename T, bool> struct finite_helper_sign
   {
     inline static BaseGDL* do_it(T* src, bool kwNaN, bool kwInfinity, DLong kwSign)
     {


       DByteGDL* res = new DByteGDL( src->Dim(), BaseGDL::ZERO); // ::ZERO is not working
       SizeT nEl = src->N_Elements();
	if (kwInfinity || kwNaN)
	{
	 #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	 {
		if (kwInfinity) {
			if (kwSign > 0) {
				#pragma omp for
				for ( SizeT i=0; i<nEl; ++i) {
					if (isinf((*src)[ i]) && (signbit((*src)[ i]) == 0)) (*res)[i]=1; 
				}
			} else {
				#pragma omp for
				for ( SizeT i=0; i<nEl; ++i) {
					if (isinf((*src)[ i]) && (signbit((*src)[ i]) != 0)) (*res)[i]=1; 
				}
			}
		}
		if (kwNaN) {
			if (kwSign > 0) {
				#pragma omp for
				for ( SizeT i=0; i<nEl; ++i) {
					if (isnan((*src)[ i]) && (signbit((*src)[ i]) == 0)) (*res)[i]=1; 
				}
			} else {
				#pragma omp for
				for ( SizeT i=0; i<nEl; ++i) {
					if (isnan((*src)[ i]) && (signbit((*src)[ i]) != 0)) (*res)[i]=1; 
				}
			}
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
       
	 #pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
	 {
	  #pragma omp for
       for ( SizeT i=0; i<nEl; ++i)
	  {
	   if      ((kwInfinity && isinf((*src)[ i].real()) || kwNaN && isnan((*src)[ i].real())) && signbit((*src)[ i].real())==0 && kwSign > 0) (*res)[i]=1;
	   else if ((kwInfinity && isinf((*src)[ i].imag()) || kwNaN && isnan((*src)[ i].imag())) && signbit((*src)[ i].imag())==0 && kwSign > 0) (*res)[i]=1;
	   else if ((kwInfinity && isinf((*src)[ i].real()) || kwNaN && isnan((*src)[ i].real())) && signbit((*src)[ i].real())==1 && kwSign < 0) (*res)[i]=1;
	   else if ((kwInfinity && isinf((*src)[ i].imag()) || kwNaN && isnan((*src)[ i].imag())) && signbit((*src)[ i].imag())==1 && kwSign < 0) (*res)[i]=1;	 
	  }
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
 
  template< typename T1, typename T2>
  BaseGDL* warp_linear0(
    SizeT nCols,
    SizeT nRows,
    BaseGDL* data_,
    DDouble *P,
    DDouble *Q,
    DDouble initvalue_,
    bool doMissing) {

    DLong lx = data_->Dim(0);
    DLong ly = data_->Dim(1);

    dimension dim(nCols, nRows);
    T1* res_ = new T1(dim, BaseGDL::NOZERO);
    T2 initvalue = initvalue_;
    DLong nEl = nCols*nRows;

    T2* res = (T2*) res_->DataAddr();
    T2* data = (T2*) data_->DataAddr();
    if (doMissing) {
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
      {
#pragma omp for
        for (DLong i = 0; i < nCols * nRows; ++i) res[i] = initvalue;
      }
    }

    /* Double loop on the output image  */
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for collapse(2)
      for (DLong j = 0; j < nRows; ++j) {
        for (DLong i = 0; i < nCols; ++i) {
          // Compute the original source for this pixel, note order of j and i in P and Q definition of IDL doc.
          DLong px = (DLong) (P[0] + P[1] * (DDouble) j + P[2] * (DDouble) i);
          DLong py = (DLong) (Q[0] + Q[1] * (DDouble) j + Q[2] * (DDouble) i);
          if (doMissing && ((px < 0) || (px > (lx - 1)) || (py < 0) || (py > (ly - 1)))) {
            continue; // already initialised to 'missing' value.
          } else {
            if (px < 0) px = 0;
            if (px > (lx - 1)) px = (lx - 1);
            if (py < 0) py = 0;
            if (py > (ly - 1)) py = (ly - 1);
            res[i + j * nCols] = data[px + py * lx];
          }
        }
      }
    }
    return res_;
  }

  template< typename T1, typename T2>
  BaseGDL* warp_linear2(
    SizeT nCols,
    SizeT nRows,
    BaseGDL* data_,
    DDouble *P,
    DDouble *Q,
    DDouble cubicParameter,
    DDouble missingValue,
    bool doMissing) {

    DLong lx = data_->Dim(0);
    DLong ly = data_->Dim(1);

    dimension dim(nCols, nRows);
    T1* res_ = new T1(dim, BaseGDL::NOZERO);
    T2 initvalue = missingValue;
    DLong nEl = nCols*nRows;

    T2* res = (T2*) res_->DataAddr();
    T2* data = (T2*) data_->DataAddr();

    int leaps[16];

    DDouble * kernel = generate_interpolation_kernel(2, cubicParameter);

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


    if (doMissing) {
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
      {
#pragma omp for
        for (DLong i = 0; i < nCols * nRows; ++i) res[i] = initvalue;
      }
    }

    /* Double loop on the output image  */
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for collapse(2)
      for (DLong j = 0; j < nRows; ++j) {
        for (DLong i = 0; i < nCols; ++i) {
          // Compute the original source for this pixel, note order of j and i in P and Q definition of IDL doc.
          DDouble x = (P[0] + P[1] * (DDouble) j + P[2] * (DDouble) i);
          DLong px = (DLong) x;
          DDouble y = (Q[0] + Q[1] * (DDouble) j + Q[2] * (DDouble) i);
          DLong py = (DLong) y;
          if (doMissing && ((px < 0) || (px > (lx - 1)) || (py < 0) || (py > (ly - 1)))) {
            continue; // already initialised to 'missing' value.
          } else {
            if (px < 1) px = 0;
            if (px > (lx - 1)) px = (lx - 1);
            if (py < 1) py = 0;
            if (py > (ly - 1)) py = (ly - 1);
            if ((px < 1) || (px > (lx - 3)) || (py < 1) || (py > (ly - 3))) res[i + j * nCols] = data[px + py * lx];
            else {
              DDouble cur;
              DDouble neighbors[16];
              DDouble rsc[8], sumrs;

              // Feed the positions for the closest 16 neighbors
              for (SizeT k = 0; k < 16; ++k) neighbors[k] = data[px + py * lx + leaps[k]];

              DDouble dpx = (DDouble) px;
              DDouble dpy = (DDouble) py;
              // Which tabulated value index shall we use?
              DLong tabx = (DLong) ((x - dpx) * (DDouble) (TABSPERPIX));
              DLong taby = (DLong) ((y - dpy) * (DDouble) (TABSPERPIX));

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
              res[i + j * nCols] = (cur / sumrs);
              /* done ! */
            }
          }
        }
      }
    }
    free(kernel);
    return res_;
  }

  template< typename T1, typename T2>
  BaseGDL* warp_linear1(
    SizeT nCols,
    SizeT nRows,
    BaseGDL* data_,
    DDouble *P,
    DDouble *Q,
    DDouble missingValue,
    bool doMissing) {

    DLong lx = data_->Dim(0);
    DLong ly = data_->Dim(1);

    dimension dim(nCols, nRows);
    T1* res_ = new T1(dim, BaseGDL::NOZERO);
    T2 initvalue = missingValue;
    DLong nEl = nCols*nRows;

    T2* res = (T2*) res_->DataAddr();
    T2* data = (T2*) data_->DataAddr();

    int leaps[9];

    DDouble * kernel = generate_interpolation_kernel(1, 0.0);

    /* Pre compute leaps for 9 closest neighbors positions */

    leaps[0] = -1 - lx;
    leaps[1] = -lx;
    leaps[2] = 1 - lx;

    leaps[3] = -1;
    leaps[4] = 0;
    leaps[5] = 1;

    leaps[6] = -1 + lx;
    leaps[7] = lx;
    leaps[8] = 1 + lx;


    if (doMissing) {
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
      {
#pragma omp for
        for (DLong i = 0; i < nCols * nRows; ++i) res[i] = initvalue;
      }
    }

    /* Double loop on the output image  */
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for collapse(2)
      for (DLong j = 0; j < nRows; ++j) {
        for (DLong i = 0; i < nCols; ++i) {
          // Compute the original source for this pixel, note order of j and i in P and Q definition of IDL doc.
          DDouble x = (P[0] + P[1] * (DDouble) j + P[2] * (DDouble) i);
          DLong px = (DLong) x;
          DDouble y = (Q[0] + Q[1] * (DDouble) j + Q[2] * (DDouble) i);
          DLong py = (DLong) y;
          if (doMissing && ((px < 0) || (px > (lx - 1)) || (py < 0) || (py > (ly - 1)))) {
            continue; // already initialised to 'missing' value.
          } else {
            if (px < 1) px = 0;
            if (px > (lx - 1)) px = (lx - 1);
            if (py < 1) py = 0;
            if (py > (ly - 1)) py = (ly - 1);
            if ((px < 1) || (px > (lx - 2)) || (py < 1) || (py > (ly - 2))) res[i + j * nCols] = data[px + py * lx];
            else {
              DDouble cur;
              DDouble neighbors[9];
              DDouble rsc[6], sumrs;

              // Feed the positions for the closest 16 neighbors
              for (SizeT k = 0; k < 9; ++k) neighbors[k] = data[px + py * lx + leaps[k]];

              DDouble dpx = (DDouble) px;
              DDouble dpy = (DDouble) py;
              // Which tabulated value index shall we use?
              DLong tabx = (DLong) ((x - dpx) * (DDouble) (TABSPERPIX));
              DLong taby = (DLong) ((y - dpy) * (DDouble) (TABSPERPIX));

              /* Compute resampling coefficients  */
              /* rsc[0..3] in x, rsc[4..7] in y   */

              rsc[0] = kernel[TABSPERPIX + tabx];
              rsc[1] = kernel[tabx];
              rsc[2] = kernel[TABSPERPIX - tabx];
              rsc[3] = kernel[TABSPERPIX + taby];
              rsc[4] = kernel[taby];
              rsc[5] = kernel[TABSPERPIX - taby];

              sumrs = (rsc[0] + rsc[1] + rsc[2]) *
                (rsc[3] + rsc[4] + rsc[5]);

              /* Compute interpolated pixel now   */
              cur = rsc[3] * (rsc[0] * neighbors[0] + rsc[1] * neighbors[1] + rsc[2] * neighbors[2]) +
                rsc[4] * (rsc[0] * neighbors[3] + rsc[1] * neighbors[4] + rsc[2] * neighbors[5]) +
                rsc[5] * (rsc[0] * neighbors[6] + rsc[1] * neighbors[7] + rsc[2] * neighbors[8]);

              /* Affect the value to the output image */
              res[i + j * nCols] = (cur / sumrs);
              /* done ! */
            }
          }
        }
      }
    }
    free(kernel);
    return res_;
  }

  template< typename T1, typename T2>
  BaseGDL* warp0(
    SizeT nCols,
    SizeT nRows,
    BaseGDL* data_,
    poly2d* poly_u,
    poly2d* poly_v,
    DDouble initvalue_,
    bool doMissing) {

    DLong lx = data_->Dim(0);
    DLong ly = data_->Dim(1);

    dimension dim(nCols, nRows);
    T1* res_ = new T1(dim, BaseGDL::NOZERO);
    T2 initvalue = initvalue_;
    DLong nEl = nCols*nRows;

    T2* res = (T2*) res_->DataAddr();
    T2* data = (T2*) data_->DataAddr();
    if (doMissing) {
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
      {
#pragma omp for
        for (DLong i = 0; i < nCols * nRows; ++i) res[i] = initvalue;
      }
    }

    /* Double loop on the output image  */
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for collapse(2)
      for (DLong j = 0; j < nRows; ++j) {
        for (DLong i = 0; i < nCols; ++i) {
          // Compute the original source for this pixel, note order of j and i.
          DDouble x = poly2d_compute(poly_u, (DDouble) j, (DDouble) i);
          DDouble y = poly2d_compute(poly_v, (DDouble) j, (DDouble) i);
          DLong px = (DLong) x;
          DLong py = (DLong) y;
          if (doMissing && ((px < 0) || (px > (lx - 1)) || (py < 0) || (py > (ly - 1)))) {
            continue; // already initialised to 'missing' value.
          } else {
            if (px < 0) px = 0;
            if (px > (lx - 1)) px = (lx - 1);
            if (py < 0) py = 0;
            if (py > (ly - 1)) py = (ly - 1);
            res[i + j * nCols] = data[px + py * lx];
          }
        }
      }
    }
    free(poly_u->px);
    free(poly_u->py);
    free(poly_u->c);
    free(poly_u);
    free(poly_v->px);
    free(poly_v->py);
    free(poly_v->c);
    free(poly_v);
    return res_;
  }

  template< typename T1, typename T2>
  BaseGDL* warp2(
    SizeT nCols,
    SizeT nRows,
    BaseGDL* data_,
    DDouble cubicParameter,
    poly2d* poly_u,
    poly2d* poly_v,
    DDouble missingValue,
    bool doMissing) {

    DLong lx = data_->Dim(0);
    DLong ly = data_->Dim(1);

    dimension dim(nCols, nRows);
    T1* res_ = new T1(dim, BaseGDL::NOZERO);
    T2 initvalue = missingValue;
    DLong nEl = nCols*nRows;

    T2* res = (T2*) res_->DataAddr();
    T2* data = (T2*) data_->DataAddr();

    int leaps[16];

    DDouble * kernel = generate_interpolation_kernel(2, cubicParameter);

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


    if (doMissing) {
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
      {
#pragma omp for
        for (DLong i = 0; i < nCols * nRows; ++i) res[i] = initvalue;
      }
    }

    /* Double loop on the output image  */
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for collapse(2)
      for (DLong j = 0; j < nRows; ++j) {
        for (DLong i = 0; i < nCols; ++i) {
          // Compute the original source for this pixel, note order of j and i.
          DDouble x = poly2d_compute(poly_u, (DDouble) j, (DDouble) i);
          DDouble y = poly2d_compute(poly_v, (DDouble) j, (DDouble) i);
          DLong px = (DLong) x;
          DLong py = (DLong) y;
          if (doMissing && ((px < 0) || (px > (lx - 1)) || (py < 0) || (py > (ly - 1)))) {
            continue; // already initialised to 'missing' value.
          } else {
            if (px < 1) px = 0;
            if (px > (lx - 1)) px = (lx - 1);
            if (py < 1) py = 0;
            if (py > (ly - 1)) py = (ly - 1);
            if ((px < 1) || (px > (lx - 3)) || (py < 1) || (py > (ly - 3))) res[i + j * nCols] = data[px + py * lx];
            else {
              DDouble cur;
              DDouble neighbors[16];
              DDouble rsc[8], sumrs;

              // Feed the positions for the closest 16 neighbors
              for (SizeT k = 0; k < 16; ++k) neighbors[k] = data[px + py * lx + leaps[k]];

              DDouble dpx = (DDouble) px;
              DDouble dpy = (DDouble) py;
              // Which tabulated value index shall we use?
              DLong tabx = (DLong) ((x - dpx) * (DDouble) (TABSPERPIX));
              DLong taby = (DLong) ((y - dpy) * (DDouble) (TABSPERPIX));

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
              res[i + j * nCols] = (cur / sumrs);
              /* done ! */
            }
          }
        }
      }
    }
    free(kernel);
    free(poly_u->px);
    free(poly_u->py);
    free(poly_u->c);
    free(poly_u);
    free(poly_v->px);
    free(poly_v->py);
    free(poly_v->c);
    free(poly_v);
    return res_;
  }

  template< typename T1, typename T2>
  BaseGDL* warp1(
    SizeT nCols,
    SizeT nRows,
    BaseGDL* data_,
    poly2d* poly_u,
    poly2d* poly_v,
    DDouble missingValue,
    bool doMissing) {

    DLong lx = data_->Dim(0);
    DLong ly = data_->Dim(1);

    dimension dim(nCols, nRows);
    T1* res_ = new T1(dim, BaseGDL::NOZERO);
    T2 initvalue = missingValue;
    DLong nEl = nCols*nRows;

    T2* res = (T2*) res_->DataAddr();
    T2* data = (T2*) data_->DataAddr();

    int leaps[9];

    DDouble * kernel = generate_interpolation_kernel(1, 0.0);

    /* Pre compute leaps for 9 closest neighbors positions */

    leaps[0] = -1 - lx;
    leaps[1] = -lx;
    leaps[2] = 1 - lx;

    leaps[3] = -1;
    leaps[4] = 0;
    leaps[5] = 1;

    leaps[6] = -1 + lx;
    leaps[7] = lx;
    leaps[8] = 1 + lx;


    if (doMissing) {
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
      {
#pragma omp for
        for (DLong i = 0; i < nCols * nRows; ++i) res[i] = initvalue;
      }
    }

    /* Double loop on the output image  */
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
    {
#pragma omp for collapse(2)
      for (DLong j = 0; j < nRows; ++j) {
        for (DLong i = 0; i < nCols; ++i) {
          // Compute the original source for this pixel, note order of j and i.
          DDouble x = poly2d_compute(poly_u, (DDouble) j, (DDouble) i);
          DDouble y = poly2d_compute(poly_v, (DDouble) j, (DDouble) i);
          DLong px = (DLong) x;
          DLong py = (DLong) y;
          if (doMissing && ((px < 0) || (px > (lx - 1)) || (py < 0) || (py > (ly - 1)))) {
            continue; // already initialised to 'missing' value.
          } else {
            if (px < 1) px = 0;
            if (px > (lx - 1)) px = (lx - 1);
            if (py < 1) py = 0;
            if (py > (ly - 1)) py = (ly - 1);
            if ((px < 1) || (px > (lx - 2)) || (py < 1) || (py > (ly - 2))) res[i + j * nCols] = data[px + py * lx];
            else {
              DDouble cur;
              DDouble neighbors[9];
              DDouble rsc[6], sumrs;

              // Feed the positions for the closest 16 neighbors
              for (SizeT k = 0; k < 9; ++k) neighbors[k] = data[px + py * lx + leaps[k]];

              DDouble dpx = (DDouble) px;
              DDouble dpy = (DDouble) py;
              // Which tabulated value index shall we use?
              DLong tabx = (DLong) ((x - dpx) * (DDouble) (TABSPERPIX));
              DLong taby = (DLong) ((y - dpy) * (DDouble) (TABSPERPIX));

              /* Compute resampling coefficients  */
              /* rsc[0..3] in x, rsc[4..7] in y   */

              rsc[0] = kernel[TABSPERPIX + tabx];
              rsc[1] = kernel[tabx];
              rsc[2] = kernel[TABSPERPIX - tabx];
              rsc[3] = kernel[TABSPERPIX + taby];
              rsc[4] = kernel[taby];
              rsc[5] = kernel[TABSPERPIX - taby];

              sumrs = (rsc[0] + rsc[1] + rsc[2]) *
                (rsc[3] + rsc[4] + rsc[5]);

              /* Compute interpolated pixel now   */
              cur = rsc[3] * (rsc[0] * neighbors[0] + rsc[1] * neighbors[1] + rsc[2] * neighbors[2]) +
                rsc[4] * (rsc[0] * neighbors[3] + rsc[1] * neighbors[4] + rsc[2] * neighbors[5]) +
                rsc[5] * (rsc[0] * neighbors[6] + rsc[1] * neighbors[7] + rsc[2] * neighbors[8]);

              /* Affect the value to the output image */
              res[i + j * nCols] = (cur / sumrs);
              /* done ! */
            }
          }
        }
      }
    }
    free(kernel);
    free(poly_u->px);
    free(poly_u->py);
    free(poly_u->c);
    free(poly_u);
    free(poly_v->px);
    free(poly_v->py);
    free(poly_v->c);
    free(poly_v);
    return res_;
  }


  BaseGDL* poly_2d_fun(EnvT* e) {
    /*
    The poly_2d code is inspired by "ECLIPSE", the  ESO C Library for an Image Processing Software Environment
    N. Devillard, "The eclipse software", The messenger No 87 - March 1997 http://www.eso.org/projects/aot/eclipse/
     */

    SizeT nParam = e->NParam();
    if (nParam < 3)
      e->Throw("Incorrect number of arguments.");

    BaseGDL* p0 = e->GetParDefined(0);
    
    if (p0->Type() == GDL_COMPLEX || p0->Type() == GDL_COMPLEXDBL)  e->Throw("Complex values not supported (FIXME)");
    
    if (p0->Rank() != 2)
      e->Throw("Array must have 2 dimensions: " + e->GetParString(0));

    BaseGDL* p1 = e->GetParDefined(1);
    BaseGDL* p2 = e->GetParDefined(2);

    DLong interp = 0;
    if (nParam >= 4) e->AssureLongScalarPar(3, interp);
    if (interp < 0 || interp > 2) e->Throw("Value of Interpolation type is out of allowed range.");

    if (nParam == 5)
      e->Throw("Incorrect number of arguments.");

    DLong nCol = p0->Dim(0);
    DLong nRow = p0->Dim(1);
    if (nParam >= 6) {
      e->AssureLongScalarPar(4, nCol);
      e->AssureLongScalarPar(5, nRow);
      if (nCol <1 || nRow <1) e->Throw("Array dimensions must be greater than 0.");
    }
    static int CUBICIx = e->KeywordIx("CUBIC");
    DDouble cubicParameter = -0.5;
    if (e->KeywordSet(CUBICIx)) {
      e->AssureDoubleScalarKWIfPresent(CUBICIx, cubicParameter);
      interp = 2;
    }

    static int MISSINGIx = e->KeywordIx("MISSING");
    DDouble missing = 0.0;
    bool doMissing = (e->KeywordPresent(MISSINGIx));
    if (doMissing) {
      e->AssureDoubleScalarKWIfPresent(MISSINGIx, missing);
    }
// check P dimension first
    DLong Psize=p1->N_Elements();
    if (Psize < 4) e->Throw("Value of Polynomial degree is out of allowed range.");
    
    DDouble dPdim = sqrt((DDouble) Psize);
    DLong nDegree = (DLong) dPdim -1;
    DLong nc=(nDegree + 1) * (nDegree + 1);
    if ( p2->N_Elements() < nc ) e->Throw("Coefficient arrays must have (degree+1)^2 elements");

    DDoubleGDL* P = static_cast<DDoubleGDL*>
      (p1->Convert2(GDL_DOUBLE, BaseGDL::COPY));

    DDoubleGDL* Q = static_cast<DDoubleGDL*>
      (p2->Convert2(GDL_DOUBLE, BaseGDL::COPY));

    if (nDegree == 1 && (*P)[3] == 0 && (*Q)[3] == 0 ) { //LINEAR CASE
      //return p0 if identity.
      if ((*P)[0] == 0 && (*P)[1] == 0 && (*P)[2] == 1 && (*P)[3] == 0 && (*Q)[0] == 0 && (*Q)[1] == 1 && (*Q)[2] == 0 && (*Q)[3] == 0) {
        return p0->Dup();
      }
      
      if (interp==0) {
         if (p0->Type() == GDL_BYTE) {
          return warp_linear0< DByteGDL, DByte>(nCol, nRow, p0, (DDouble*) P->DataAddr(),(DDouble*) Q->DataAddr(), missing, doMissing);
        } else if (p0->Type() == GDL_INT) {
          return warp_linear0< DIntGDL, DInt>(nCol, nRow, p0, (DDouble*) P->DataAddr(),(DDouble*) Q->DataAddr(), missing, doMissing);
        } else if (p0->Type() == GDL_UINT) {
          return warp_linear0< DUIntGDL, DUInt>(nCol, nRow, p0, (DDouble*) P->DataAddr(),(DDouble*) Q->DataAddr(), missing, doMissing);
        } else if (p0->Type() == GDL_LONG) {
          return warp_linear0< DLongGDL, DLong>(nCol, nRow, p0, (DDouble*) P->DataAddr(),(DDouble*) Q->DataAddr(), missing, doMissing);
        } else if (p0->Type() == GDL_ULONG) {
          return warp_linear0< DULongGDL, DULong>(nCol, nRow, p0, (DDouble*) P->DataAddr(),(DDouble*) Q->DataAddr(), missing, doMissing);
        } else if (p0->Type() == GDL_LONG64) {
          return warp_linear0< DLong64GDL, DLong64>(nCol, nRow, p0, (DDouble*) P->DataAddr(),(DDouble*) Q->DataAddr(), missing, doMissing);
        } else if (p0->Type() == GDL_ULONG64) {
          return warp_linear0< DULong64GDL, DULong64>(nCol, nRow, p0, (DDouble*) P->DataAddr(),(DDouble*) Q->DataAddr(), missing, doMissing);
        } else if (p0->Type() == GDL_FLOAT) {
          return warp_linear0< DFloatGDL, DFloat>(nCol, nRow, p0, (DDouble*) P->DataAddr(),(DDouble*) Q->DataAddr(), missing, doMissing);
        } else if (p0->Type() == GDL_DOUBLE) {
          return warp_linear0< DDoubleGDL, DDouble>(nCol, nRow, p0, (DDouble*) P->DataAddr(),(DDouble*) Q->DataAddr(), missing, doMissing);
        }
      } else if (interp==1) {
         if (p0->Type() == GDL_BYTE) {
          return warp_linear1< DByteGDL, DByte>(nCol, nRow, p0, (DDouble*) P->DataAddr(),(DDouble*) Q->DataAddr(), missing, doMissing);
        } else if (p0->Type() == GDL_INT) {
          return warp_linear1< DIntGDL, DInt>(nCol, nRow, p0, (DDouble*) P->DataAddr(),(DDouble*) Q->DataAddr(), missing, doMissing);
        } else if (p0->Type() == GDL_UINT) {
          return warp_linear1< DUIntGDL, DUInt>(nCol, nRow, p0, (DDouble*) P->DataAddr(),(DDouble*) Q->DataAddr(), missing, doMissing);
        } else if (p0->Type() == GDL_LONG) {
          return warp_linear1< DLongGDL, DLong>(nCol, nRow, p0, (DDouble*) P->DataAddr(),(DDouble*) Q->DataAddr(), missing, doMissing);
        } else if (p0->Type() == GDL_ULONG) {
          return warp_linear1< DULongGDL, DULong>(nCol, nRow, p0, (DDouble*) P->DataAddr(),(DDouble*) Q->DataAddr(), missing, doMissing);
        } else if (p0->Type() == GDL_LONG64) {
          return warp_linear1< DLong64GDL, DLong64>(nCol, nRow, p0, (DDouble*) P->DataAddr(),(DDouble*) Q->DataAddr(), missing, doMissing);
        } else if (p0->Type() == GDL_ULONG64) {
          return warp_linear1< DULong64GDL, DULong64>(nCol, nRow, p0, (DDouble*) P->DataAddr(),(DDouble*) Q->DataAddr(), missing, doMissing);
        } else if (p0->Type() == GDL_FLOAT) {
          return warp_linear1< DFloatGDL, DFloat>(nCol, nRow, p0, (DDouble*) P->DataAddr(),(DDouble*) Q->DataAddr(), missing, doMissing);
        } else if (p0->Type() == GDL_DOUBLE) {
          return warp_linear1< DDoubleGDL, DDouble>(nCol, nRow, p0, (DDouble*) P->DataAddr(),(DDouble*) Q->DataAddr(), missing, doMissing);
        }
      } else if (interp==2) {
         if (p0->Type() == GDL_BYTE) {
          return warp_linear2< DByteGDL, DByte>(nCol, nRow, p0, (DDouble*) P->DataAddr(),(DDouble*) Q->DataAddr(), cubicParameter, missing, doMissing);
        } else if (p0->Type() == GDL_INT) {
          return warp_linear2< DIntGDL, DInt>(nCol, nRow, p0, (DDouble*) P->DataAddr(),(DDouble*) Q->DataAddr(), cubicParameter, missing, doMissing);
        } else if (p0->Type() == GDL_UINT) {
          return warp_linear2< DUIntGDL, DUInt>(nCol, nRow, p0, (DDouble*) P->DataAddr(),(DDouble*) Q->DataAddr(), cubicParameter, missing, doMissing);
        } else if (p0->Type() == GDL_LONG) {
          return warp_linear2< DLongGDL, DLong>(nCol, nRow, p0, (DDouble*) P->DataAddr(),(DDouble*) Q->DataAddr(), cubicParameter, missing, doMissing);
        } else if (p0->Type() == GDL_ULONG) {
          return warp_linear2< DULongGDL, DULong>(nCol, nRow, p0, (DDouble*) P->DataAddr(),(DDouble*) Q->DataAddr(), cubicParameter, missing, doMissing);
        } else if (p0->Type() == GDL_LONG64) {
          return warp_linear2< DLong64GDL, DLong64>(nCol, nRow, p0, (DDouble*) P->DataAddr(),(DDouble*) Q->DataAddr(), cubicParameter, missing, doMissing);
        } else if (p0->Type() == GDL_ULONG64) {
          return warp_linear2< DULong64GDL, DULong64>(nCol, nRow, p0, (DDouble*) P->DataAddr(),(DDouble*) Q->DataAddr(), cubicParameter, missing, doMissing);
        } else if (p0->Type() == GDL_FLOAT) {
          return warp_linear2< DFloatGDL, DFloat>(nCol, nRow, p0, (DDouble*) P->DataAddr(),(DDouble*) Q->DataAddr(), cubicParameter, missing, doMissing);
        } else if (p0->Type() == GDL_DOUBLE) {
          return warp_linear2< DDoubleGDL, DDouble>(nCol, nRow, p0, (DDouble*) P->DataAddr(),(DDouble*) Q->DataAddr(), cubicParameter, missing, doMissing);
        }
      }
    } else {
    
    //NON-LINEAR Polynomial

    poly2d* poly_u;
    poly2d* poly_v;

    poly_u = (poly2d *) malloc(sizeof (poly2d));
    poly_u->nc = nc;
    poly_u->px = (DLong *) malloc(nc * sizeof (DLong));
    poly_u->py = (DLong *) malloc(nc * sizeof (DLong));
    poly_u->c = (DDouble *) malloc(nc * sizeof (DDouble));

    for (SizeT i = 0; i < (nDegree + 1)*(nDegree + 1); ++i) {
      poly_u->px[i] = i / (nDegree + 1);
      poly_u->py[i] = i - (poly_u->px[i] * (nDegree + 1));
      poly_u->c[i] = (*P)[poly_u->px[i]+(nDegree + 1) * poly_u->py[i]];
    }

    poly_v = (poly2d *) malloc(sizeof (poly2d));
    poly_v->nc = nc;
    poly_v->px = (DLong *) malloc(nc * sizeof (DLong));
    poly_v->py = (DLong *) malloc(nc * sizeof (DLong));
    poly_v->c = (DDouble *) malloc(nc * sizeof (DDouble));

    for (SizeT i = 0; i < (nDegree + 1)*(nDegree + 1); ++i) {
      poly_v->px[i] = i / (nDegree + 1);
      poly_v->py[i] = i - (poly_v->px[i] * (nDegree + 1));
      poly_v->c[i] = (*Q)[poly_v->px[i]+(nDegree + 1) * poly_v->py[i]];
    }
    if (interp==0) {
       if (p0->Type() == GDL_BYTE) {
        return warp0< DByteGDL, DByte>(nCol, nRow, p0, poly_u, poly_v, missing, doMissing);
      } else if (p0->Type() == GDL_INT) {
        return warp0< DIntGDL, DInt>(nCol, nRow, p0, poly_u, poly_v, missing, doMissing);
      } else if (p0->Type() == GDL_UINT) {
        return warp0< DUIntGDL, DUInt>(nCol, nRow, p0, poly_u, poly_v, missing, doMissing);
      } else if (p0->Type() == GDL_LONG) {
        return warp0< DLongGDL, DLong>(nCol, nRow, p0, poly_u, poly_v, missing, doMissing);
      } else if (p0->Type() == GDL_ULONG) {
        return warp0< DULongGDL, DULong>(nCol, nRow, p0, poly_u, poly_v, missing, doMissing);
      } else if (p0->Type() == GDL_LONG64) {
        return warp0< DLong64GDL, DLong64>(nCol, nRow, p0, poly_u, poly_v, missing, doMissing);
      } else if (p0->Type() == GDL_ULONG64) {
        return warp0< DULong64GDL, DULong64>(nCol, nRow, p0, poly_u, poly_v, missing, doMissing);
      } else if (p0->Type() == GDL_FLOAT) {
        return warp0< DFloatGDL, DFloat>(nCol, nRow, p0, poly_u, poly_v, missing, doMissing);
      } else if (p0->Type() == GDL_DOUBLE) {
        return warp0< DDoubleGDL, DDouble>(nCol, nRow, p0, poly_u, poly_v, missing, doMissing);
      }
    } else if (interp==1) {
       if (p0->Type() == GDL_BYTE) {
        return warp1< DByteGDL, DByte>(nCol, nRow, p0, poly_u, poly_v, missing, doMissing);
      } else if (p0->Type() == GDL_INT) {
        return warp1< DIntGDL, DInt>(nCol, nRow, p0, poly_u, poly_v, missing, doMissing);
      } else if (p0->Type() == GDL_UINT) {
        return warp1< DUIntGDL, DUInt>(nCol, nRow, p0, poly_u, poly_v, missing, doMissing);
      } else if (p0->Type() == GDL_LONG) {
        return warp1< DLongGDL, DLong>(nCol, nRow, p0, poly_u, poly_v, missing, doMissing);
      } else if (p0->Type() == GDL_ULONG) {
        return warp1< DULongGDL, DULong>(nCol, nRow, p0, poly_u, poly_v, missing, doMissing);
      } else if (p0->Type() == GDL_LONG64) {
        return warp1< DLong64GDL, DLong64>(nCol, nRow, p0, poly_u, poly_v, missing, doMissing);
      } else if (p0->Type() == GDL_ULONG64) {
        return warp1< DULong64GDL, DULong64>(nCol, nRow, p0, poly_u, poly_v, missing, doMissing);
      } else if (p0->Type() == GDL_FLOAT) {
        return warp1< DFloatGDL, DFloat>(nCol, nRow, p0, poly_u, poly_v, missing, doMissing);
      } else if (p0->Type() == GDL_DOUBLE) {
        return warp1< DDoubleGDL, DDouble>(nCol, nRow, p0, poly_u, poly_v, missing, doMissing);
      }
    } else if (interp==2) {
       if (p0->Type() == GDL_BYTE) {
        return warp2< DByteGDL, DByte>(nCol, nRow, p0, cubicParameter, poly_u, poly_v, missing, doMissing);
      } else if (p0->Type() == GDL_INT) {
        return warp2< DIntGDL, DInt>(nCol, nRow, p0, cubicParameter, poly_u, poly_v, missing, doMissing);
      } else if (p0->Type() == GDL_UINT) {
        return warp2< DUIntGDL, DUInt>(nCol, nRow, p0, cubicParameter, poly_u, poly_v, missing, doMissing);
      } else if (p0->Type() == GDL_LONG) {
        return warp2< DLongGDL, DLong>(nCol, nRow, p0, cubicParameter, poly_u, poly_v, missing, doMissing);
      } else if (p0->Type() == GDL_ULONG) {
        return warp2< DULongGDL, DULong>(nCol, nRow, p0, cubicParameter, poly_u, poly_v, missing, doMissing);
      } else if (p0->Type() == GDL_LONG64) {
        return warp2< DLong64GDL, DLong64>(nCol, nRow, p0, cubicParameter, poly_u, poly_v, missing, doMissing);
      } else if (p0->Type() == GDL_ULONG64) {
        return warp2< DULong64GDL, DULong64>(nCol, nRow, p0, cubicParameter, poly_u, poly_v, missing, doMissing);
      } else if (p0->Type() == GDL_FLOAT) {
        return warp2< DFloatGDL, DFloat>(nCol, nRow, p0, cubicParameter, poly_u, poly_v, missing, doMissing);
      } else if (p0->Type() == GDL_DOUBLE) {
        return warp2< DDoubleGDL, DDouble>(nCol, nRow, p0, cubicParameter, poly_u, poly_v, missing, doMissing);
      }
    }

    }
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

///*-------------------------------------------------------------------------*/
///**
//  @brief	Cardinal sine.
//  @param	x	double value.
//  @return	1 double.
//
//  Compute the value of the function sinc(x)=sin(pi*x)/(pi*x) at the
//  requested x.
// */
///*--------------------------------------------------------------------------*/
//double sinc(double x)
//{
//    if (fabs(x)<1e-4)
//        return (double)1.00 ;
//    else
//        return ((sin(x * (double)PI_NUMB)) / (x * (double)PI_NUMB)) ;
//}



/*-------------------------------------------------------------------------*/
/**
  @brief	Generate an interpolation kernel to use in this module.
  @param	kernel_type (integer) 1:linear 2:cubic 3 quintic
  @param	cubic (DDouble) cubic parameter [0..1[ for cubic kernel.
  @return	1 newly allocated array of doubles.

  The returned array of doubles must be deallocated using free().
 */
/*--------------------------------------------------------------------------*/
DDouble * generate_interpolation_kernel(int kernel_type, DDouble cubicParameter)
{
    double  *	tab ;
    int     	i ;
    double  	x ;
    int     	samples = KERNEL_SAMPLES ;
	  /*
	    Taken from "Image Reconstruction By Piecewise Polynomial Kernels", 
	    Meijering et al
	  */

     if (kernel_type == 1) {
	  tab = (double *) calloc(samples , sizeof(double)) ;
	  tab[0] = 1.0 ;
	  for (i=1 ; i<samples ; ++i) {
	    x = (double)KERNEL_WIDTH * (double)i/(double)(samples-1) ;
	    if (x < 1)
	      tab[i] = -x + 1;
	    else if (x >= 1)
	      tab[i] = 0;
	  }
	} else if (kernel_type == 2) { //uses cubic
	  tab = (double *) calloc(samples , sizeof(double)) ;
	  tab[0] = 1.0 ;
	  for (i=1 ; i<samples ; ++i) {
	    x = (double)KERNEL_WIDTH * (double)i/(double)(samples-1) ;
	    if (x < 1)
	      tab[i] = (cubicParameter+2)*ipow(x,3) - (cubicParameter+3)*ipow(x,2) + 1;
	    else if (x < 2)
	      tab[i] = cubicParameter*ipow(x,3) - 
		(5*cubicParameter)*ipow(x,2) + (8*cubicParameter)*x - (4*cubicParameter);
	  }
    } else if (kernel_type == 3) { //quintic
	  tab = (double *) calloc(samples , sizeof(double)) ;
	  tab[0] = 1.0 ;
	  for (i=1 ; i<samples ; ++i) {
	    x = (double)KERNEL_WIDTH * (double)i/(double)(samples-1) ;
	    if (x < 1)
	      tab[i] = (10.*cubicParameter-(21./16.))*ipow(x,5) +
            (-18.*cubicParameter+(45./16))*ipow(x,4)+
            (8.*cubicParameter-(5./2.))*ipow(x,2)+
            1.0;
	    else if (x < 2)
	      tab[i] = (11.*cubicParameter-(5./16.))*ipow(x,5)+
            (-88.*cubicParameter+(45./16.))*ipow(x,4)+
            (270.*cubicParameter-10)*ipow(x,3)+
            (-392.*cubicParameter+(35./2.))*ipow(x,2)+
            (265.*cubicParameter-15.)*x+
            (-66.*cubicParameter+5);
        else if (x < 3)
	      tab[i] = cubicParameter*ipow(x,5) +
            (-14.*cubicParameter)*ipow(x,4) +
            (78.*cubicParameter)*ipow(x,3)  +
            (-216.*cubicParameter)*ipow(x,2)+
            297.*cubicParameter*x +
            (-162.*cubicParameter);
	  }
	} else {
      throw GDLException("Internal GDL error in generate_interpolation_kernel(), please report.");
	  return NULL ;
	}
    return tab ;
}

} // namespace
