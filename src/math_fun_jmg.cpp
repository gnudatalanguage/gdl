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
#include "math_utl.hpp"
#include "math_fun_jmg.hpp"

//#define GDL_DEBUG
#undef GDL_DEBUG

using namespace std;

namespace lib {

  BaseGDL* machar_fun( EnvT* e)
  {
    long int ibeta, it, irnd, ngrd, machep, negep, iexp, minexp, maxexp;
    float  eps, epsneg, xmin, xmax;
    double epsD, epsnegD, xminD, xmaxD;
    
    if( e->KeywordSet( "DOUBLE"))
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
// 	if( p0->Type() == COMPLEX) {
// 	  float* dptr = (float*) &(*p0C)[0];
// 	 } else if( p0->Type() == COMPLEXDBL) {
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

//     if( p0->Type() == COMPLEX) {
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
//     } else if ( p0->Type() == COMPLEXDBL) {
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
//     } else if( p0->Type() == DOUBLE) {
//       DDoubleGDL* p0D = static_cast<DDoubleGDL*>( p0);
//       for( SizeT i=0; i<nEl; ++i)
// 	if (isfinite((*p0D)[ i]) == 0) (*res)[ i] = 0; else (*res)[ i] = 1;
//     } else if( p0->Type() == FLOAT) {
//       DFloatGDL* p0F = static_cast<DFloatGDL*>( p0);
//       for( SizeT i=0; i<nEl; ++i)
// 	if (isfinite((*p0F)[ i]) == 0) (*res)[ i] = 0; else (*res)[ i] = 1;
//     } else {
//       DFloatGDL* p0F = static_cast<DFloatGDL*>
// 	(p0->Convert2( FLOAT, BaseGDL::COPY));
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

   // partial specialization for COMPLEX, DCOMPLEX
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

   BaseGDL* finite_fun( EnvT* e)
   {
     e->NParam( 1);

     BaseGDL* p0     = e->GetParDefined( 0);
     
     static int nanIx = e->KeywordIx( "NAN");
     bool kwNaN      = e->KeywordSet( nanIx);

     static int infinityIx = e->KeywordIx( "INFINITY");
     bool kwInfinity = e->KeywordSet( infinityIx);

     if( kwNaN && kwInfinity)
       e->Throw("Conflicting keywords.");

     switch (p0->Type()) 
       {
       case FLOAT: 
	return finite_template<DFloatGDL, false>(p0, kwNaN, kwInfinity);
       case DOUBLE:
	return finite_template<DDoubleGDL, false>(p0, kwNaN, kwInfinity);
       case COMPLEX:
	return finite_template<DComplexGDL, true>(p0, kwNaN, kwInfinity);
       case COMPLEXDBL:
	return finite_template<DComplexDblGDL, true>(p0, kwNaN, kwInfinity);
       case STRING:
	 DFloatGDL* p0F = 
	   static_cast<DFloatGDL*>(p0->Convert2(FLOAT,BaseGDL::COPY));
	 e->Guard( p0F);
	 return finite_template<DFloatGDL, false>(p0F, kwNaN, kwInfinity);

       case STRUCT:
       case PTR:
       case OBJECT:
	 e->Throw( p0->TypeStr() + " not allowed in this context: " +
		   e->GetParString( 0));

       default: // integer types
	 if( kwNaN || kwInfinity)
	   return new DByteGDL( p0->Dim()); // zero

         DByteGDL* res = new DByteGDL( p0->Dim(), BaseGDL::NOZERO); 
         SizeT nEl = p0->N_Elements();
         for (SizeT i=0; i<nEl; i++)
           (*res)[i] = 1;
         return res;
       }
   }








  BaseGDL* check_math_fun( EnvT* e)
  {
    DLong value=0;
    DLong mask=255;

    if( e->KeywordSet( "MASK"))
      e->AssureLongScalarKWIfPresent( "MASK", mask);	

    if (mask & 16) {
      if (fetestexcept(FE_DIVBYZERO)) {
	value = value | 16;
	if ( e->KeywordSet( "PRINT"))
	  cout << 
	    "% Program caused arithmetic error: Floating divide by 0" << endl;
	if ( !e->KeywordSet( "NOCLEAR")) feclearexcept(FE_DIVBYZERO); 
      }
    }

    if (mask & 32) {
      if (fetestexcept(FE_UNDERFLOW)) {
	value = value | 32;
	if ( e->KeywordSet( "PRINT"))
	  cout << 
	    "% Program caused arithmetic error: Floating underflow" << endl;
	if ( !e->KeywordSet( "NOCLEAR")) feclearexcept(FE_UNDERFLOW); 
      }
    }

    if (mask & 64) {
      if (fetestexcept(FE_OVERFLOW)) {
	value = value | 64;
	if ( e->KeywordSet( "PRINT"))
	  cout << 
	    "% Program caused arithmetic error: Floating overflow" << endl;
	if ( !e->KeywordSet( "NOCLEAR")) feclearexcept(FE_OVERFLOW); 
      }
    }

    if (mask & 128 && value == 0) {
      if (fetestexcept(FE_INVALID)) {
	value = value | 128;
	if ( e->KeywordSet( "PRINT"))
	  cout << 
	    "% Program caused arithmetic error: Floating illegal operand" << endl;
	if ( !e->KeywordSet( "NOCLEAR")) feclearexcept(FE_INVALID); 
      }
    }

    return new DLongGDL( value );
  }


  BaseGDL* radon_fun( EnvT* e)
  {
    BaseGDL* p0 = e->GetParDefined( 0);

    SizeT nEl = p0->N_Elements();
    if( nEl == 0)
      e->Throw( "Variable is undefined: "+e->GetParString(0));

    if (p0->Rank() != 2)
      e->Throw( "RADON: Array must have 2 dimensions: "+e->GetParString(0));

    DFloatGDL* p0F = static_cast<DFloatGDL*>
      (p0->Convert2( FLOAT, BaseGDL::COPY));

    DFloat fpi=4*atan(1.0);

    DFloat dx=1, dy=1;
    DFloat drho = 0.5 * sqrt(dx*dx + dy*dy);

    if( e->KeywordSet( "DX"))
      e->AssureFloatScalarKWIfPresent( "DX", dx);	
    if( e->KeywordSet( "DY"))
      e->AssureFloatScalarKWIfPresent( "DY", dy);
    if( e->KeywordSet( "DRHO"))
      e->AssureFloatScalarKWIfPresent( "DRHO", drho);	

    DLong dims[2];


    if( e->KeywordSet( "BACKPROJECT")) {

      DLong ntheta=p0->Dim(0);
      DLong nrho=p0->Dim(1);
      DFloat dtheta=fpi/ntheta;

      DLong nx=(DLong) floor(2*((drho*nrho/2)/sqrt(dx*dx + dy*dy))+1);
      DLong ny=nx;
      if( e->KeywordSet( "NX"))
	e->AssureLongScalarKWIfPresent( "NX", nx);
      if( e->KeywordSet( "NY"))
	e->AssureLongScalarKWIfPresent( "NY", ny);	

      dims[0] = nx;
      dims[1] = ny;

      dimension dim((SizeT *) dims, 2);

      DFloat xmin = -0.5 * (nx-1);
      DFloat ymin = -0.5 * (ny-1);
      DFloat xmax = +0.5 * (nx-1);
      DFloat ymax = +0.5 * (ny-1);

      if( e->KeywordSet( "XMIN"))
	e->AssureFloatScalarKWIfPresent( "XMIN", xmin);	
      if( e->KeywordSet( "YMIN"))
	e->AssureFloatScalarKWIfPresent( "YMIN", ymin);	


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

      if( e->KeywordSet( "XMIN"))
	e->AssureFloatScalarKWIfPresent( "XMIN", xmin);	
      if( e->KeywordSet( "YMIN"))
	e->AssureFloatScalarKWIfPresent( "YMIN", xmin);	

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

      if( e->KeywordSet( "NTHETA"))
	e->AssureLongScalarKWIfPresent( "NTHETA", dims[0]);
      if( e->KeywordSet( "NRHO"))
	e->AssureLongScalarKWIfPresent( "NRHO", dims[1]);

      static int thetaIx = e->KeywordIx( "THETA"); 
      if( e->KeywordPresent( thetaIx)) {
	DFloatGDL* thetaKW = e->IfDefGetKWAs<DFloatGDL>( thetaIx);
	if (thetaKW != NULL) dims[0] = thetaKW->N_Elements();
      }

      DLong ntheta = dims[0];
      DLong nrho = dims[1];

      dimension dim((SizeT *) dims, 2);

      DFloatGDL* res = new DFloatGDL( dim, BaseGDL::NOZERO);
      for( SizeT i=0; i<ntheta*nrho; ++i) (*res)[i] = 0;

      DFloat theta=0, dtheta=fpi/ntheta, cs, sn;
      DFloat rmin=-0.5*(nrho-1)*drho, rho;
      if( e->KeywordSet( "RMIN"))
	e->AssureFloatScalarKWIfPresent( "RMIN", rmin);	


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
	dimension dim((SizeT *) &nrho, (SizeT) 1);
	*rhoKW = new DFloatGDL(dim, BaseGDL::NOZERO);
	for( SizeT irho=0; irho<nrho; ++irho)
	  (*(DFloatGDL*) *rhoKW)[irho] = rmin + irho*drho;
      }

      // If THETA KW present but variable doesn't exist then write theta array
      if( e->KeywordPresent( thetaIx)) {
	if (e->IfDefGetKWAs<DFloatGDL>( thetaIx) == NULL) {
	  dimension dim((SizeT *) &ntheta, (SizeT) 1);
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

} // namespace

