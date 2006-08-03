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
	 {
	   return finite_template<DFloatGDL, false>(p0, kwNaN, kwInfinity);
	 }
       case DOUBLE:
	 {
	   return finite_template<DDoubleGDL, false>(p0, kwNaN, kwInfinity);
	 }
       case COMPLEX:
	 {
	   return finite_template<DComplexGDL, true>(p0, kwNaN, kwInfinity);
	 }
       case COMPLEXDBL:
	 {
	   return finite_template<DComplexDblGDL, true>(p0, kwNaN, kwInfinity);
	 }
       case STRING:
	 {
	   DFloatGDL* p0F = 
	     static_cast<DFloatGDL*>(p0->Convert2(FLOAT,BaseGDL::COPY));
	   e->Guard( p0F);
	   return finite_template<DFloatGDL, false>(p0F, kwNaN, kwInfinity);
	 }
       case STRUCT:
       case PTR:
       case OBJECT:
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


  template< typename T1, typename T2>
  BaseGDL* poly_2d_fun_template( DLong nCol, DLong nRow, image_t* warped)
  {
    dimension dim;  
    dim.Set(0, nCol);
    dim.Set(1, nRow);
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
				   int shift_y, int shift_x)
  {
    dimension dim;  
    dim.Set(0, nCol);
    dim.Set(1, nRow);
    T1* res = new T1( dim, BaseGDL::NOZERO);

    int lx = (int) p0->Dim(0);
    int ly = (int) p0->Dim(1);
    int lx_out = (int) nCol;
    int ly_out = (int) nRow;

    char *p_out = (char *) res->DataAddr();
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
      e->Throw( "POLY_2d: Incorrect number of arguments.");

    BaseGDL* p0 = e->GetParDefined( 0);
    if (p0->Rank() != 2)
      e->Throw( "POLY_2D: Array must have 2 dimensions: "+e->GetParString(0));

    BaseGDL* p1 = e->GetParDefined( 1);
    BaseGDL* p2 = e->GetParDefined( 2);

    DLong interp=0;
    if (nParam >= 4) e->AssureLongScalarPar( 3, interp);
    if (interp < 0 || interp > 2)
      e->Throw( "POLY_2D: Value of Interpolation type is out of allowed range.: "+e->GetParString(0));

    if( nParam == 5)
      e->Throw( "POLY_2d: Incorrect number of arguments.");

    DLong nCol = p0->Dim(0);
    DLong nRow = p0->Dim(1);
    if (nParam >= 6) {
      e->AssureLongScalarPar( 4, nCol);
      e->AssureLongScalarPar( 5, nRow);
    }

    DDouble cubic=-0.5;
    if( e->KeywordSet( "CUBIC")) {
      e->AssureDoubleScalarKWIfPresent( "CUBIC", cubic);	
      interp = 2;
    }

    DDouble missing=0.0;
    if( e->KeywordSet( "MISSING")) {
      e->AssureDoubleScalarKWIfPresent( "MISSING", missing);	
    }

    SizeT nEl;

    DDouble sqp1 = sqrt((DDouble) p1->N_Elements());
    DLong lsqp1 = (DLong) sqp1;
    DDouble sqp2 = sqrt((DDouble) p2->N_Elements());
    DLong lsqp2 = (DLong) sqp2;

    if (p1->N_Elements() == 1)
      e->Throw( "POLY_2D: Value of Polynomial degree is out of allowed range.");
    if (sqp1 != lsqp1)
      e->Throw( "POLY_2D: Value of Polynomial degree is out of allowed range.");

    if (p2->N_Elements() == 1)
      e->Throw( "POLY_2D: Coefficient arrays must have (degree+1)^2 elements");
    if (sqp2 != lsqp2)
      e->Throw( "POLY_2D: Coefficient arrays must have (degree+1)^2 elements");

    DLong nDegree = lsqp1 - 1;
    DLong nc = (nDegree + 1) * (nDegree + 1);

    DDoubleGDL* P = static_cast<DDoubleGDL*>
      (p1->Convert2( DOUBLE, BaseGDL::COPY));

    DDoubleGDL* Q = static_cast<DDoubleGDL*>
      (p2->Convert2( DOUBLE, BaseGDL::COPY));

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
	  (*P)[1] == 0 && (*Q)[2] == 0) {
	// Translation 
	if (((int) (*P)[0] == 0) && ((int) (*Q)[0] == 0)) {
	  return p0->Dup();
	} else {
	  if (p0->Type() == BYTE) {
	    return poly_2d_shift_template< DByteGDL, DByte>( p0, nCol, nRow,  
							     (int) (*Q)[0], 
							     (int) (*P)[0]);
	  } else if (p0->Type() == INT) {
	    return poly_2d_shift_template< DIntGDL, DInt>( p0, nCol, nRow,  
							    (int) (*Q)[0], 
							    (int) (*P)[0]);
	  } else if (p0->Type() == UINT) {
	    return poly_2d_shift_template< DUIntGDL, DUInt>( p0, nCol, nRow,  
							     (int) (*Q)[0], 
							     (int) (*P)[0]);
	  } else if (p0->Type() == LONG) {
	    return poly_2d_shift_template< DLongGDL, DLong>( p0, nCol, nRow,  
							     (int) (*Q)[0], 
							     (int) (*P)[0]);
	  } else if (p0->Type() == ULONG) {
	    return poly_2d_shift_template< DULongGDL, DULong>( p0, nCol, nRow,  
							       (int) (*Q)[0], 
							       (int) (*P)[0]);
	  } else if (p0->Type() == LONG64) {
	    return poly_2d_shift_template< DLong64GDL, DLong64>( p0, nCol, nRow,  
								 (int) (*Q)[0], 
								 (int) (*P)[0]);
	  } else if (p0->Type() == ULONG64) {
	    return poly_2d_shift_template< DULong64GDL, DULong64>( p0, nCol, nRow,  
								   (int) (*Q)[0], 
								   (int) (*P)[0]);
	  } else if (p0->Type() == FLOAT) {
	    return poly_2d_shift_template< DFloatGDL, DFloat>( p0, nCol, nRow,  
							       (int) (*Q)[0], 
							       (int) (*P)[0]);
	  } else if (p0->Type() == DOUBLE) {
	    return poly_2d_shift_template< DDoubleGDL, DDouble>( p0, nCol, nRow,  
								 (int) (*Q)[0], 
								 (int) (*P)[0]);
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
			    lineartrans, poly_v, poly_u,
			    interp, cubic, LINEAR);
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
			  interp, cubic, GENERIC);

      if (poly_u->px != NULL) free(poly_u->px);
      if (poly_u->py != NULL) free(poly_u->py);
      if (poly_u->c  != NULL) free(poly_u->c);

      free(poly_u);

      if (poly_v->px != NULL) free(poly_v->px);
      if (poly_v->py != NULL) free(poly_v->py);
      if (poly_v->c  != NULL) free(poly_v->c);

      free(poly_v);
    }

    if (p0->Type() == BYTE) {
      for ( SizeT i=0; i<p0->N_Elements(); ++i) {
	if (warped->data[i] < 0)   warped->data[i] = 0;
	if (warped->data[i] > 255) warped->data[i] = 255;
      }
      return poly_2d_fun_template< DByteGDL, DByte>( nCol, nRow, warped);
    } else if (p0->Type() == INT) {
      return poly_2d_fun_template< DIntGDL, DInt>( nCol, nRow, warped);
    } else if (p0->Type() == UINT) {
      return poly_2d_fun_template< DUIntGDL, DUInt>( nCol, nRow, warped);
    } else if (p0->Type() == LONG) {
      return poly_2d_fun_template< DLongGDL, DLong>( nCol, nRow, warped);
    } else if (p0->Type() == ULONG) {
      return poly_2d_fun_template< DULongGDL, DULong>( nCol, nRow, warped);
    } else if (p0->Type() == LONG64) {
      return poly_2d_fun_template< DLong64GDL, DLong64>( nCol, nRow, warped);
    } else if (p0->Type() == ULONG64) {
      return poly_2d_fun_template< DULong64GDL, DULong64>( nCol, nRow, warped);
    } else if (p0->Type() == FLOAT) {
      return poly_2d_fun_template< DFloatGDL, DFloat>( nCol, nRow, warped);
    } else if (p0->Type() == DOUBLE) {
      return poly_2d_fun_template< DDoubleGDL, DDouble>( nCol, nRow, warped);
    }
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
		     SizeT  lx,		     
		     SizeT  ly,		     
		     SizeT  lx_out,		     
		     SizeT  ly_out,		     
		     DType  type,		     
		     void*  data,		     
		     char		*	kernel_type,
		     DDouble *param,
		     poly2d		*	poly_u,
		     poly2d		*	poly_v,
		     DLong interp,
		     DDouble cubic,
		     DLong warpType)
{
    image_t    *	image_out ;
    int         	i, j, k ;
    double       	cur ;
    double       	neighbors[16] ;
    double       	rsc[8],
					sumrs ;
    double       	x, y ;
    int     		px, py ;
    int     		pos ;
    int         	tabx, taby ;
    double      *	kernel=NULL ;
    int		      	leaps[16] ;

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
      kernel = generate_interpolation_kernel(kernel_type, (double) 0.0) ;
      if (kernel == NULL) {
	//        e_error("cannot generate kernel: aborting resampling") ;
        return NULL ;
      }
    }

    /* Generate cubic interpolation kernel if necessary */
    if (interp == 2) {
      kernel = generate_interpolation_kernel(kernel_type, cubic) ;
      if (kernel == NULL) {
	//        e_error("cannot generate kernel: aborting resampling") ;
        return NULL ;
      }
    }

    image_out = image_new(lx_out, ly_out) ;

    /* Pre compute leaps for 16 closest neighbors positions */

    leaps[0] = -1 - lx ;
    leaps[1] =    - lx ;
    leaps[2] =  1 - lx ;
    leaps[3] =  2 - lx ;

    leaps[4] = -1 ;
    leaps[5] =  0 ;
    leaps[6] =  1 ;
    leaps[7] =  2 ;

    leaps[8] = -1 + lx ;
    leaps[9] =      lx ;
    leaps[10]=  1 + lx ;
    leaps[11]=  2 + lx ;

    leaps[12]= -1 + 2*lx ;
    leaps[13]=      2*lx ;
    leaps[14]=  1 + 2*lx ;
    leaps[15]=  2 + 2*lx ;

    for (k=0 ; k<16 ; k++) neighbors[k] = 0;

    /* Double loop on the output image  */
    for (j=0 ; j < ly_out ; j++) {
        for (i=0 ; i< lx_out ; i++) {
            /* Compute the original source for this pixel   */

	  if (warpType == LINEAR) {
	    x = param[0] * (double)i + param[1] * (double)j + param[2]; 
	    y = param[3] * (double)i + param[4] * (double)j + param[5]; 
	  } else {
	    x = poly2d_compute(poly_u, (double)i, (double)j);
	    y = poly2d_compute(poly_v, (double)i, (double)j);
	  }

	  /* Which is the closest integer positioned neighbor?    */
	  px = (int)x ;
	  py = (int)y ;

	  if ((px < 1) ||
	      (px > (lx-1)) ||
	      (py < 1) ||
	      (py > (ly-1)))
	    image_out->data[i+j*lx_out] = (pixelvalue)0.0 ;
	  else {
	    /* Now feed the positions for the closest 16 neighbors  */
	    pos = px + py * lx ;
	    for (k=0 ; k<16 ; k++) {

	      if (interp == 0 && k != 5) continue;

	      int row = (pos+leaps[k]) / lx;
	      int col = (pos+leaps[k]) - row*lx;
	      if (type == BYTE) {
		memcpy(&data_b, &ptr[sizeof(char)*(col*ly+row)], 
		       sizeof(char));
		neighbors[k] = (double) data_b;
	      } 
	      if (type == INT) {
		memcpy(&data_i, &ptr[sizeof(DInt)*(col*ly+row)], 
		       sizeof(DInt));
		neighbors[k] = (double) data_i;
	      } 
	      if (type == UINT) {
		memcpy(&data_ui, &ptr[sizeof(DUInt)*(col*ly+row)], 
		       sizeof(DUInt));
		neighbors[k] = (double) data_ui;
	      } 
	      if (type == LONG) {
		memcpy(&data_l, &ptr[sizeof(DLong)*(col*ly+row)], 
		       sizeof(DLong));
		neighbors[k] = (double) data_l;
	      } 
	      if (type == ULONG) {
		memcpy(&data_ul, &ptr[sizeof(DULong)*(col*ly+row)], 
		       sizeof(DULong));
		neighbors[k] = (double) data_ul;
	      } 
	      if (type == LONG64) {
		memcpy(&data_l64, &ptr[sizeof(DLong64)*(col*ly+row)], 
		       sizeof(DLong64));
		neighbors[k] = (double) data_l64;
	      } 
	      if (type == ULONG64) {
		memcpy(&data_ul64, &ptr[sizeof(DULong64)*(col*ly+row)], 
		       sizeof(DULong64));
		neighbors[k] = (double) data_ul64;
	      } 
	      if (type == FLOAT) {
		memcpy(&data_f, &ptr[sizeof(float)*(col*ly+row)], 
		       sizeof(float));
		neighbors[k] = (double) data_f;
	      } 
	      if (type == DOUBLE) {
		memcpy(&data_d, &ptr[sizeof(double)*(col*ly+row)], 
		       sizeof(double));
		neighbors[k] = data_d;
	      } 
	    }

	    if (interp == 0) {
	      image_out->data[i+j*lx_out] = (pixelvalue) neighbors[5];
	    } else if (interp == 1) {
	      /* Which tabulated value index shall we use?    */
	      tabx = (int)((x - (double)px) * (double)(TABSPERPIX)) ;
	      taby = (int)((y - (double)py) * (double)(TABSPERPIX)) ;
	      
	      /* Compute resampling coefficients  */
	      /* rsc[0..3] in x, rsc[4..7] in y   */
	      
	      rsc[0] = kernel[TABSPERPIX + tabx] ;
	      rsc[1] = kernel[tabx] ;
	      rsc[2] = kernel[TABSPERPIX - tabx] ;
	      rsc[4] = kernel[TABSPERPIX + taby] ;
	      rsc[5] = kernel[taby] ;
	      rsc[6] = kernel[TABSPERPIX - taby] ;
	      
	      sumrs = (rsc[0]+rsc[1]+rsc[2]) *
		(rsc[4]+rsc[5]+rsc[6]) ;
	      
	      /* Compute interpolated pixel now   */
	      if ((x - (double)px) < 0 && (y - (double)py) < 0) {
		cur =   rsc[4] * (  rsc[0]*neighbors[0] +
				    rsc[1]*neighbors[1] ) +
		        rsc[5] * (  rsc[0]*neighbors[4] +
			            rsc[1]*neighbors[5]);
	      } else if ((x - (double)px) >= 0 && (y - (double)py) < 0) {
		cur =   rsc[4] * (  rsc[1]*neighbors[1] +
				    rsc[2]*neighbors[2] ) +
		        rsc[5] * (  rsc[1]*neighbors[5] +
			            rsc[2]*neighbors[6]);
	      } else if ((x - (double)px) < 0 && (y - (double)py) >= 0) {
		cur =   rsc[5] * (  rsc[0]*neighbors[4] +
				    rsc[1]*neighbors[5] ) +
		        rsc[6] * (  rsc[0]*neighbors[8] +
			            rsc[1]*neighbors[9]);
	      } else if ((x - (double)px) >= 0 && (y - (double)py) >= 0) {
		cur =   rsc[5] * (  rsc[1]*neighbors[5] +
				    rsc[2]*neighbors[6] ) +
		        rsc[6] * (  rsc[1]*neighbors[9] +
			            rsc[2]*neighbors[10]);
	      }
	      
	      /* Affect the value to the output image */
	      image_out->data[i+j*lx_out] = (pixelvalue)(cur/sumrs) ;
	      /* done ! */
	    } else {
	      /* Which tabulated value index shall we use?    */
	      tabx = (int)((x - (double)px) * (double)(TABSPERPIX)) ;
	      taby = (int)((y - (double)py) * (double)(TABSPERPIX)) ;
	      
	      /* Compute resampling coefficients  */
	      /* rsc[0..3] in x, rsc[4..7] in y   */
	      
	      rsc[0] = kernel[TABSPERPIX + tabx] ;
	      rsc[1] = kernel[tabx] ;
	      rsc[2] = kernel[TABSPERPIX - tabx] ;
	      rsc[3] = kernel[2 * TABSPERPIX - tabx] ;
	      rsc[4] = kernel[TABSPERPIX + taby] ;
	      rsc[5] = kernel[taby] ;
	      rsc[6] = kernel[TABSPERPIX - taby] ;
	      rsc[7] = kernel[2 * TABSPERPIX - taby] ;
	      
	      sumrs = (rsc[0]+rsc[1]+rsc[2]+rsc[3]) *
		(rsc[4]+rsc[5]+rsc[6]+rsc[7]) ;
	      
	      /* Compute interpolated pixel now   */
	      cur =   rsc[4] * (  rsc[0]*neighbors[0] +
				  rsc[1]*neighbors[1] +
				  rsc[2]*neighbors[2] +
				  rsc[3]*neighbors[3] ) +
		rsc[5] * (  rsc[0]*neighbors[4] +
			    rsc[1]*neighbors[5] +
			    rsc[2]*neighbors[6] +
			    rsc[3]*neighbors[7] ) +
		rsc[6] * (  rsc[0]*neighbors[8] +
			    rsc[1]*neighbors[9] +
			    rsc[2]*neighbors[10] +
			    rsc[3]*neighbors[11] ) +
		rsc[7] * (  rsc[0]*neighbors[12] +
			    rsc[1]*neighbors[13] +
			    rsc[2]*neighbors[14] +
			    rsc[3]*neighbors[15] ) ; 
	      
	      /* Affect the value to the output image */
	      image_out->data[i+j*lx_out] = (pixelvalue)(cur/sumrs) ;
	      /* done ! */
	    }
	  }       
        }
    }

    if (kernel != NULL) free(kernel) ;
    return image_out ;
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
		int 	size_y)
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

} // namespace

/*

	gsl_matrix *mat = gsl_matrix_alloc(4,4);
	gsl_matrix *inverse = gsl_matrix_calloc(4, 4);
	gsl_permutation *perm = gsl_permutation_alloc(4);

	memcpy(mat->data, &(*p0D)[0], nEl*szdbl);

	gsl_linalg_LU_decomp (mat, perm, &s);
	det = gsl_linalg_LU_lndet(mat);
	if (gsl_isinf(det) == 0) {
	  gsl_linalg_LU_invert (mat, perm, inverse);
	}
	else singular = 1;

	memcpy(&(*res)[0], inverse->data, nEl*szdbl);

	gsl_permutation_free(perm);
	gsl_matrix_free(mat);
	gsl_matrix_free(inverse);

*/
