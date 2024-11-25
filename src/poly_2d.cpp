/***************************************************************************
                          poly_2d.cpp  -  2D polynomial interpolation
                             -------------------
    begin                : March 03 2004
    copyright            : (C) 2002 by Joel Gales 2024 by Gilles Duvert
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

#include "poly_2d.hpp"
#include "includefirst.hpp"

#define TABSPERPIX      (1000)
#define KERNEL_WIDTH    (2.0)
#define KERNEL_SAMPLES  (1+(int)(TABSPERPIX * KERNEL_WIDTH))
#define DEFAULT_CUBIC_PARAMETER 0

namespace lib {

  // fast function to return a^n

  OMPInt ipowI(OMPInt a, OMPInt n) {

	// Stores final answer
	OMPInt ans = 1;

	while (n > 0) {

	  OMPInt last_bit = (n & 1);

	  // Check if current LSB
	  // is set
	  if (last_bit) {
		ans = ans * a;
	  }

	  a = a * a;

	  // Right shift
	  n = n >> 1;
	}

	return ans;
  }
  // version for floats
  DFloat ipowF(DFloat a, OMPInt n) {

	// Stores final answer
	DFloat ans = 1;

	while (n > 0) {

	  OMPInt last_bit = (n & 1);

	  // Check if current LSB
	  // is set
	  if (last_bit) {
		ans = ans * a;
	  }

	  a = a * a;

	  // Right shift
	  n = n >> 1;
	}

	return ans;
  }


  DFloat * poly2d_compute_init_x(poly2d * p,	SizeT n) {
	DFloat * res= (DFloat*) malloc(p->nc*n*sizeof(DFloat));
	for (auto i=0, s=0; i< n; ++i) for (DLong k = 0; k < p->nc; k++) res[s++]=ipowI(i, p->px[k]);
	return res;
  }
  DFloat* poly2d_compute_init_y(poly2d * p,	SizeT n) {
	DFloat * res= (DFloat*) malloc(p->nc*n*sizeof(DFloat));
	for (auto i=0,s=0; i< n; ++i) for (DLong k = 0; k < p->nc; k++) res[s++]=ipowI(i, p->py[k]);
	return res;
  }

 // cubic kernels are much faster using a precoputed table of kernel values digitzed on 1/1000 th of a pixel.
  static DFloat*  cubicKernel;
  static bool cubicKernelInitialized=false;

DFloat cubicInterpolate (DFloat p[4], DFloat x, DFloat * precomputed_kernel) {
		  // Which tabulated value index shall we use?
		  DLong tabx = (DLong) (x * (DFloat) (TABSPERPIX));
          DFloat rsc[4];
		  rsc[0] = precomputed_kernel[TABSPERPIX + tabx];
		  rsc[1] = precomputed_kernel[tabx];
		  rsc[2] = precomputed_kernel[TABSPERPIX - tabx];
		  rsc[3] = precomputed_kernel[2 * TABSPERPIX - tabx];

		  DFloat sumrs = rsc[0] + rsc[1] + rsc[2] + rsc[3];

		  DFloat val = 	rsc[0] * p[0] +	rsc[1] * p[1] +	rsc[2] * p[2] +	rsc[3] * p[3];
		  val /= sumrs;
	return val;
}

DFloat bicubicInterpolate (DFloat p[4][4], DFloat x, DFloat y, DFloat * precomputed_kernel) {
	DFloat arr[4];
	arr[0] = cubicInterpolate(p[0], x,precomputed_kernel);
	arr[1] = cubicInterpolate(p[1], x,precomputed_kernel);
	arr[2] = cubicInterpolate(p[2], x,precomputed_kernel);
	arr[3] = cubicInterpolate(p[3], x,precomputed_kernel);
	return cubicInterpolate(arr, y,precomputed_kernel);
}

/*-------------------------------------------------------------------------*/
/**
  @brief	Generate an interpolation kernel to use in this module.
  @param	kernel_type (integer) 1:linear 2:cubic 3 quintic
  @param	cubic (DDouble) cubic parameter [0..1[ for cubic kernel.
  @return	1 newly allocated array of DFloats.

  The returned array of DFloats must be deallocated using free().
 */
/*--------------------------------------------------------------------------*/
DFloat * generate_interpolation_kernel(/* int kernel_type, */ DFloat cubicParameter)
{
    DFloat  *	tab ;
    int     	i ;
    DFloat  	x ;
    int     	samples = KERNEL_SAMPLES ;
	  /*
	    Taken from "Image Reconstruction By Piecewise Polynomial Kernels", 
	    Meijering et al (original contribution: Joel Gales)
	  */

	// non-used code commented out by GD.
//     if (kernel_type == 1) {
//	  tab = (DFloat *) calloc(samples , sizeof(DFloat)) ;
//	  tab[0] = 1.0 ;
//	  for (i=1 ; i<samples ; ++i) {
//	    x = (DFloat)KERNEL_WIDTH * (DFloat)i/(DFloat)(samples-1) ;
//	    if (x < 1)
//	      tab[i] = -x + 1;
//	    else if (x >= 1)
//	      tab[i] = 0;
//	  }
//	} else if (kernel_type == 2) { //uses cubic
	  tab = (DFloat *) calloc(samples , sizeof(DFloat)) ;
	  tab[0] = 1.0 ;
	  for (i=1 ; i<samples ; ++i) {
	    x = (DFloat)KERNEL_WIDTH * (DFloat)i/(DFloat)(samples-1) ;
	    if (x < 1)
	      tab[i] = (cubicParameter+2)*ipowF(x,3) - (cubicParameter+3)*ipowF(x,2) + 1;
	    else if (x < 2)
	      tab[i] = cubicParameter*ipowF(x,3) - 
		(5*cubicParameter)*ipowF(x,2) + (8*cubicParameter)*x - (4*cubicParameter);
	  }
//    } else if (kernel_type == 3) { //quintic
//	  tab = (DFloat *) calloc(samples , sizeof(DFloat)) ;
//	  tab[0] = 1.0 ;
//	  for (i=1 ; i<samples ; ++i) {
//	    x = (DFloat)KERNEL_WIDTH * (DFloat)i/(DFloat)(samples-1) ;
//	    if (x < 1)
//	      tab[i] = (10.*cubicParameter-(21./16.))*ipowF(x,5) +
//            (-18.*cubicParameter+(45./16))*ipowF(x,4)+
//            (8.*cubicParameter-(5./2.))*ipowF(x,2)+
//            1.0;
//	    else if (x < 2)
//	      tab[i] = (11.*cubicParameter-(5./16.))*ipowF(x,5)+
//            (-88.*cubicParameter+(45./16.))*ipowF(x,4)+
//            (270.*cubicParameter-10)*ipowF(x,3)+
//            (-392.*cubicParameter+(35./2.))*ipowF(x,2)+
//            (265.*cubicParameter-15.)*x+
//            (-66.*cubicParameter+5);
//        else if (x < 3)
//	      tab[i] = cubicParameter*ipowF(x,5) +
//            (-14.*cubicParameter)*ipowF(x,4) +
//            (78.*cubicParameter)*ipowF(x,3)  +
//            (-216.*cubicParameter)*ipowF(x,2)+
//            297.*cubicParameter*x +
//            (-162.*cubicParameter);
//	  }
//	} else {
//      throw GDLException("Internal GDL error in generate_interpolation_kernel(), please report.");
//	  return NULL ;
//	}
    return tab ;
}

template< typename T1, typename T2>
  BaseGDL* warp_linear0(
    const SizeT nCols,
    const SizeT nRows,
    BaseGDL* data_,
    DFloat * const P,
    DFloat * const Q,
    DFloat const initvalue_,
	const bool doMissing) {
//	std::cerr<<"warp_linear0\n";
	SizeT lx = data_->Dim(0);
	SizeT ly = data_->Dim(1);

	dimension dim(nCols, nRows);
	T1* res_ = new T1(dim, BaseGDL::NOZERO);
	T2 initvalue = initvalue_;
	SizeT nEl = nCols*nRows;

	T2* res = (T2*) res_->DataAddr();
	T2* const data = (T2* const) data_->DataAddr();
	if (doMissing) {
	  if ((GDL_NTHREADS = parallelize(nEl)) == 1) {
		for (OMPInt i = 0; i < nCols * nRows; ++i) res[i] = initvalue;
	  } else {
		TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
		  for (OMPInt i = 0; i < nCols * nRows; ++i) res[i] = initvalue;
	  }
	}
	//these are accelerators - not using them increase exec time by 2 or 3.
	DFloat fllx=lx;
	DFloat flly=ly;
	DFloat fl0x=0;
	DFloat fl0y=0;
	DFloat p0 = P[0];
	DFloat q0 = Q[0];
	DFloat p1 = P[1];
	DFloat q1 = Q[1];
	DFloat p2 = P[2];
	DFloat q2 = Q[2];
	DFloat p3 = P[3];
	DFloat q3 = Q[3];
	DFloat p1j, p3j, q1j, q3j;
	if (doMissing) {
	  // do Missing values. The algorithm IS NOT THE SAME as for other case: test the following:
	  // a=bindgen(3,3) & P=[0.9,-0,1,0] & Q=[0.9,1,0,0] & B = POLY_2D(A, P, Q, MISS=-1) & print,b
	  // versus a=bindgen(3,3) & P=[0.9,-0,1,0] & Q=[0.9,1,0,0] & B = POLY_2D(A, P, Q) & print,b
	  /* Double loop on the output image  */
	  if ((GDL_NTHREADS = parallelize(nEl)) == 1) {
		for (OMPInt j = 0; j < nRows; ++j) {
		  p1j = p0 + p1 * j;
		  p3j = p2 + p3 * j;
		  q1j = q0 + q1 * j;
		  q3j = q2 + q3 * j;
		  for (OMPInt i = 0; i < nCols; ++i) {
			// Compute the original source for this pixel, note order of j and i in P and Q definition of IDL doc.
			// note 'continue' is on DFloat values 
			DFloat x = p1j + p3j * i;
			if (x < fl0x) continue;
			if (x >= fllx) continue;
			DFloat y = q1j + q3j * i;
			if (y < fl0y) continue;
			if (y >= flly) continue;
			SizeT px=x;
			SizeT py=y;
			res[i + j * nCols] = data[px + py * lx];
		  }
		}
	  } else {
		TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) private(p1j,p3j,q1j,q3j)
		  for (OMPInt j = 0; j < nRows; ++j) {
		  p1j = p0 + p1 * j;
		  p3j = p2 + p3 * j;
		  q1j = q0 + q1 * j;
		  q3j = q2 + q3 * j;
		  for (OMPInt i = 0; i < nCols; ++i) {
			// Compute the original source for this pixel, note order of j and i in P and Q definition of IDL doc.
		    DFloat x = p1j + p3j * i;
			if (x < fl0x) continue;
			if (x >= fllx) continue;
			DFloat y = q1j + q3j * i;
			if (y < fl0y) continue;
			if (y >= flly) continue;
			SizeT px=x;
			SizeT py=y;
			res[i + j * nCols] = data[px + py * lx];
		  }
		}
	  }
	  
	} else {
	fllx-=1; //restrict range by 1 for following interger pixel computation to work. 
	flly-=1; //restrict range by 1 for following interger pixel computation to work. 
	  SizeT llx = fllx;
	  SizeT lly = flly;
	  SizeT l0x = fl0x;
	  SizeT l0y = fl0y;
	/* Double loop on the output image  */
	if ((GDL_NTHREADS = parallelize(nEl)) == 1) {
	  for (OMPInt j = 0; j < nRows; ++j) {
		  p1j = p0 + p1 * j;
		  p3j = p2 + p3 * j;
		  q1j = q0 + q1 * j;
		  q3j = q2 + q3 * j;
		for (OMPInt i = 0; i < nCols; ++i) {
		  // Compute the original source for this pixel, note order of j and i in P and Q definition of IDL doc.
		  DFloat x = p1j + p3j * i;
		  SizeT px=x;
		  if (x < fl0x) px = l0x; else if (x > fllx) px = llx;
		  DFloat y = q1j + q3j * i;
		  SizeT py=y;
		  if (y < fl0y) py = l0y; else if (y > flly) py = lly;
		  res[i + j * nCols] = data[px + py * lx];
		}
	  }
	} else {
	  TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) private(p1j,p3j,q1j,q3j)
	  for (OMPInt j = 0; j < nRows; ++j) {
		  p1j = p0 + p1 * j;
		  p3j = p2 + p3 * j;
		  q1j = q0 + q1 * j;
		  q3j = q2 + q3 * j;
		for (OMPInt i = 0; i < nCols; ++i) {
		  // Compute the original source for this pixel, note order of j and i in P and Q definition of IDL doc.
		  DFloat x = p1j + p3j * i;
		  SizeT px=x;
		  if (x < fl0x) px = l0x; else if (x > fllx) px = fllx;
	      DFloat y = q1j + q3j * i;
		  SizeT py=y;
		  if (y < fl0y) py = l0y; else if (y > flly) py = flly;
		  res[i + j * nCols] = data[px + py * lx];
		}
	  }
	}
  }
	return res_;
  }

  template< typename T1, typename T2>
  BaseGDL* warp0(
    const SizeT nCols,
    const SizeT nRows,
    BaseGDL* const data_,
    poly2d* const poly_u,
    poly2d* const poly_v,
    const DFloat initvalue_,
    bool doMissing) {
//	std::cerr<<"warp0\n";
	const SizeT lx = data_->Dim(0);
	const SizeT ly = data_->Dim(1);

	dimension dim(nCols, nRows);
	T1* res_ = new T1(dim, BaseGDL::NOZERO);
	T2 initvalue = initvalue_;
	const SizeT nEl = nCols*nRows;

	T2* res = (T2*) res_->DataAddr();
	T2* const data = (T2* const) data_->DataAddr();
	if (doMissing) {
	if ((GDL_NTHREADS = parallelize(nEl)) == 1) {
		for (OMPInt i = 0; i < nCols * nRows; ++i) res[i] = initvalue;
	  } else {
		TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
		  for (OMPInt i = 0; i < nCols * nRows; ++i) res[i] = initvalue;
	  }
	}
	DFloat fllx=lx-1;  //-1 for neighbor
	DFloat flly=ly-1;
	DFloat fl0x=0;
	DFloat fl0y=0;
	//these are accelerators - not using them increase exec time by 2 or 3.
	DLong nc=poly_u->nc;
	DFloat * const xcoefu=poly2d_compute_init_x(poly_u,lx);
	DFloat * const ycoefu=poly2d_compute_init_y(poly_u,lx);
	DFloat * const xcoefv=poly2d_compute_init_x(poly_v,ly);
	DFloat * const ycoefv=poly2d_compute_init_y(poly_v,ly);
	/* Double loop on the output image  */
	if (doMissing) {
	if ((GDL_NTHREADS = parallelize(nEl)) == 1) {
	  for (OMPInt j = 0; j < nRows; ++j) {
		for (OMPInt i = 0; i < nCols; ++i) {
          DFloat x = poly_u->c[0]; for (auto k=1; k< nc; ++k) x+=poly_u->c[k]*xcoefu[j*nc+k]*ycoefu[i*nc+k];
		  if (x < fl0x) continue;
		  if (x >= fllx) continue; // already initialised to 'missing' value.
          DFloat y = poly_v->c[0]; for (auto k=1; k< nc; ++k) y+=poly_v->c[k]*xcoefv[j*nc+k]*ycoefv[i*nc+k];
		  if (y < fl0y) continue;
		  if (y >= flly)	continue;
		  SizeT px = x;
		  SizeT py = y;
		  res[i + j * nCols] = data[px + py * lx];
		}
	  }
	} else {
	  TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for collapse(2) num_threads(GDL_NTHREADS)
	  for (OMPInt j = 0; j < nRows; ++j) {
		for (OMPInt i = 0; i < nCols; ++i) {
		  // Compute the original source for this pixel, note order of j and i in P and Q definition of IDL doc.
          DFloat x = poly_u->c[0]; for (auto k=1; k< nc; ++k) x+=poly_u->c[k]*xcoefu[j*nc+k]*ycoefu[i*nc+k];
		  if (x < fl0x) continue;
		  if (x >= fllx) continue; // already initialised to 'missing' value.
          DFloat y = poly_v->c[0]; for (auto k=1; k< nc; ++k) y+=poly_v->c[k]*xcoefv[j*nc+k]*ycoefv[i*nc+k];
		  if (y < fl0y) continue;
		  if (y >= flly)	continue;
		  SizeT px = x;
		  SizeT py = y;
		  res[i + j * nCols] = data[px + py * lx];
		}
	  }
	}
	} else {
	  fllx -= 1; //restrict range by 1 for following interger pixel computation to work. 
	  flly -= 1; //restrict range by 1 for following interger pixel computation to work. 
	  SizeT llx = fllx;
	  SizeT lly = flly;
	  SizeT l0x = fl0x;
	  SizeT l0y = fl0y;
	/* Double loop on the output image  */
	if ((GDL_NTHREADS = parallelize(nEl)) == 1) {
	  for (OMPInt j = 0; j < nRows; ++j) {
		for (OMPInt i = 0; i < nCols; ++i) {
          DFloat x = poly_u->c[0];
		  DFloat y = poly_v->c[0];
		  for (auto k=1; k< nc; ++k) {
			x+=poly_u->c[k]*xcoefu[j*nc+k]*ycoefu[i*nc+k];
            y+=poly_v->c[k]*xcoefv[j*nc+k]*ycoefv[i*nc+k];
		  }
		  SizeT px=x;
		  SizeT py=y;
		  if (x < fl0x) px = l0x; else if (x > fllx) px = llx;
		  if (y < fl0y) py = l0y; else if (y > flly) py = lly;
		  res[i + j * nCols] = data[px + py * lx];
		}
	  }
	} else {
	  TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for collapse(2) num_threads(GDL_NTHREADS)
	  for (OMPInt j = 0; j < nRows; ++j) {
		for (OMPInt i = 0; i < nCols; ++i) {
		  // Compute the original source for this pixel, note order of j and i in P and Q definition of IDL doc.
          DFloat x = poly_u->c[0];
		  DFloat y = poly_v->c[0];
		  for (auto k=1; k< nc; ++k) {
			x+=poly_u->c[k]*xcoefu[j*nc+k]*ycoefu[i*nc+k];
            y+=poly_v->c[k]*xcoefv[j*nc+k]*ycoefv[i*nc+k];
		  }
		  SizeT px=x;
		  SizeT py=y;
		  if (x < fl0x) px = l0x; else if (x > fllx) px = llx;
		  if (y < fl0y) py = l0y; else if (y > flly) py = lly;
		  res[i + j * nCols] = data[px + py * lx];
		}
	  }
	}
	}
	free(xcoefu);
	free(ycoefu);
	free(xcoefv);
	free(ycoefv);
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
  BaseGDL* warp_linear1(
   const SizeT nCols,
    const SizeT nRows,
    BaseGDL* data_,
    DFloat * const P,
    DFloat * const Q,
    DFloat const initvalue_,
	const bool doMissing) {
//	std::cerr << "warp_linear1\n";
	SizeT lx = data_->Dim(0);
	SizeT ly = data_->Dim(1);

	dimension dim(nCols, nRows);
	T1* res_ = new T1(dim, BaseGDL::NOZERO);
	T2 initvalue = initvalue_;
	SizeT nEl = nCols*nRows;

	T2* res = (T2*) res_->DataAddr();
	T2 * const data = (T2 * const) data_->DataAddr();
	if (doMissing) {
	  if ((GDL_NTHREADS = parallelize(nEl)) == 1) {
		for (OMPInt i = 0; i < nCols * nRows; ++i) res[i] = initvalue;
	  } else {
		TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
		  for (OMPInt i = 0; i < nCols * nRows; ++i) res[i] = initvalue;
	  }
	}
	//these are accelerators - not using them increase exec time by 2 or 3.
	const DFloat xmax = lx;
	const DFloat ymax = ly;
	const DFloat xmin = 0;
	const DFloat ymin = 0;
	const DFloat xbound = lx - 1; //-1 for linear
	const DFloat ybound = ly - 1;
	const DFloat p0 = P[0];
	const DFloat q0 = Q[0];
	const DFloat p1 = P[1];
	const DFloat q1 = Q[1];
	const DFloat p2 = P[2];
	const DFloat q2 = Q[2];
	const DFloat p3 = P[3];
	const DFloat q3 = Q[3];
	DFloat p1j, p3j, q1j, q3j;

	/* Double loop on the output internal image  */
	if ((GDL_NTHREADS = parallelize(nEl)) == 1) {
	  for (OMPInt j = 0; j < nRows; ++j) {
		p1j = p0 + p1 * j;
		p3j = p2 + p3 * j;
		q1j = q0 + q1 * j;
		q3j = q2 + q3 * j;
		for (OMPInt i = 0; i < nCols; ++i) {
		  // Compute the original source for this pixel, note order of j and i in P and Q definition of IDL doc.
		  DFloat x = p1j + p3j * i;
		  if (doMissing) {
			if (x < xmin) continue;
			if (x >= xmax) continue;
		  }
		  DFloat y = q1j + q3j * i;
		  if (doMissing) {
			if (y < ymin) continue;
			if (y >= ymax) continue;
		  }
		  SizeT px;
		  SizeT py;
		  DFloat dx;
		  DFloat dy;
		  if (x >= xbound) {
			px = lx - 2;
			dx = 1;
		  } else {
			px = x;
			dx = x - px;
		  }
		  if (y >= ybound) {
			py = ly - 2;
			dy = 1;
		  } else {
			py = y;
			dy = y - py;
		  }
		  SizeT pix[4];
		  pix[0] = px + py * lx;
		  pix[1] = pix[0] + 1;
		  pix[2] = pix[0] + lx;
		  pix[3] = pix[2] + 1;

		  DFloat a = data[pix[0]] + dx * (data[pix[1]] - data[pix[0]]);
		  DFloat b = data[pix[2]] + dx * (data[pix[3]] - data[pix[2]]);
		  res[i + j * nCols] = a + dy * (b - a);
		}
	  }
	} else {
	  TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) private(p1j,p3j,q1j,q3j)
		for (OMPInt j = 0; j < nRows; ++j) {
		p1j = p0 + p1 * j;
		p3j = p2 + p3 * j;
		q1j = q0 + q1 * j;
		q3j = q2 + q3 * j;
		for (OMPInt i = 0; i < nCols; ++i) {
		  // Compute the original source for this pixel, note order of j and i in P and Q definition of IDL doc.
		  DFloat x = p1j + p3j * i;
		  if (doMissing) {
			if (x < xmin) continue;
			if (x >= xmax) continue;
		  }
		  DFloat y = q1j + q3j * i;
		  if (doMissing) {
			if (y < ymin) continue;
			if (y >= ymax) continue;
		  }
		  SizeT px;
		  SizeT py;
		  DFloat dx;
		  DFloat dy;
		  if (x >= xbound) {
			px = lx - 2;
			dx = 1;
		  } else {
			px = x;
			dx = x - px;
		  }
		  if (y >= ybound) {
			py = ly - 2;
			dy = 1;
		  } else {
			py = y;
			dy = y - py;
		  }
		  SizeT pix[4];
		  pix[0] = px + py * lx;
		  pix[1] = pix[0] + 1;
		  pix[2] = pix[0] + lx;
		  pix[3] = pix[2] + 1;

		  DFloat a = data[pix[0]] + dx * (data[pix[1]] - data[pix[0]]);
		  DFloat b = data[pix[2]] + dx * (data[pix[3]] - data[pix[2]]);
		  res[i + j * nCols] = a + dy * (b - a);
		}
	  }
	}
	return res_;
  }

  template< typename T1, typename T2>
  BaseGDL* warp1(
    const SizeT nCols,
    const SizeT nRows,
    BaseGDL* const data_,
    poly2d* const poly_u,
    poly2d* const poly_v,
    const DFloat initvalue_,
    bool doMissing) {
//	std::cerr<<"warp1\n";
	const SizeT lx = data_->Dim(0);
	const SizeT ly = data_->Dim(1);

    dimension dim(nCols, nRows);
    T1* res_ = new T1(dim, BaseGDL::NOZERO);
	T2 initvalue = initvalue_;
	const SizeT nEl = nCols*nRows;

    T2* res = (T2*) res_->DataAddr();
	T2* const data = (T2* const) data_->DataAddr();
    if (doMissing) {
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (OMPInt i = 0; i < nCols * nRows; ++i) res[i] = initvalue;
      } else {
      TRACEOMP(__FILE__,__LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < nCols * nRows; ++i) res[i] = initvalue;
      }
    }
	//these are accelerators - not using them increase exec time by 2 or 3.
	DFloat fllx = lx;
	DFloat flly = ly;
	DFloat fl0x=0;
	DFloat fl0y=0;
	DLong nc=poly_u->nc;
	DFloat * const xcoefu=poly2d_compute_init_x(poly_u,lx);
	DFloat * const ycoefu=poly2d_compute_init_y(poly_u,lx);
	DFloat * const xcoefv=poly2d_compute_init_x(poly_v,ly);
	DFloat * const ycoefv=poly2d_compute_init_y(poly_v,ly);
	/* Double loop on the output image  */
	if (doMissing) {
	  if ((GDL_NTHREADS = parallelize(nEl)) == 1) {
		for (OMPInt j = 0; j < nRows; ++j) {
		  for (OMPInt i = 0; i < nCols; ++i) {
			DFloat x = poly_u->c[0];
			for (auto k = 1; k < nc; ++k) x += poly_u->c[k] * xcoefu[j * nc + k] * ycoefu[i * nc + k];
			if (x < fl0x) continue;
			if (x >= fllx) continue;
			DFloat y = poly_v->c[0];
			for (auto k = 1; k < nc; ++k) y += poly_v->c[k] * xcoefv[j * nc + k] * ycoefv[i * nc + k];
			if (y < fl0y) continue;
			if (y >= flly) continue;
			SizeT px = x;
			SizeT py = y;
			SizeT pixnum[4];
			DFloat dx = x - px;
			DFloat dy = y - py;
			pixnum[0] = px + py * lx;
			pixnum[1] = pixnum[0] + 1;
			pixnum[2] = pixnum[0] + lx;
			pixnum[3] = pixnum[2] + 1;
			DFloat a = data[pixnum[0]] + dx * (data[pixnum[1]] - data[pixnum[0]]);
			DFloat b = data[pixnum[2]] + dx * (data[pixnum[3]] - data[pixnum[2]]);
			res[i + j * nCols] = a + dy * (b - a);
		  }
		}
	  } else {
		TRACEOMP(__FILE__, __LINE__)
#pragma omp parallel for collapse(2) num_threads(GDL_NTHREADS)
		  for (OMPInt j = 0; j < nRows; ++j) {
		  for (OMPInt i = 0; i < nCols; ++i) {
			DFloat x = poly_u->c[0];
			for (auto k = 1; k < nc; ++k) x += poly_u->c[k] * xcoefu[j * nc + k] * ycoefu[i * nc + k];
			if (x < fl0x) continue;
			if (x >= fllx) continue;
			DFloat y = poly_v->c[0];
			for (auto k = 1; k < nc; ++k) y += poly_v->c[k] * xcoefv[j * nc + k] * ycoefv[i * nc + k];
			if (y < fl0y) continue;
			if (y >= flly) continue;
			SizeT px = x;
			SizeT py = y;
			SizeT pixnum[4];
			DFloat dx = x - px;
			DFloat dy = y - py;
			pixnum[0] = px + py * lx;
			pixnum[1] = pixnum[0] + 1;
			pixnum[2] = pixnum[0] + lx;
			pixnum[3] = pixnum[2] + 1;
			DFloat a = data[pixnum[0]] + dx * (data[pixnum[1]] - data[pixnum[0]]);
			DFloat b = data[pixnum[2]] + dx * (data[pixnum[3]] - data[pixnum[2]]);
			res[i + j * nCols] = a + dy * (b - a);
		  }
		}
	  }
	} else {
	  fllx -= 1; //restrict range by 1 for following interger pixel computation to work. 
	  flly -= 1; //restrict range by 1 for following interger pixel computation to work. 
	  SizeT llx = fllx;
	  SizeT lly = flly;
	  SizeT l0x = fl0x;
	  SizeT l0y = fl0y;
	   if ((GDL_NTHREADS = parallelize(nEl)) == 1) {
		for (OMPInt j = 0; j < nRows; ++j) {
		  for (OMPInt i = 0; i < nCols; ++i) {
			DFloat x = poly_u->c[0];
			DFloat y = poly_v->c[0];
			for (auto k=1; k< nc; ++k) {
			  x+=poly_u->c[k]*xcoefu[j*nc+k]*ycoefu[i*nc+k];
			  y+=poly_v->c[k]*xcoefv[j*nc+k]*ycoefv[i*nc+k];
			}
			SizeT px=x;
			SizeT py=y;
			if (x < fl0x) px = l0x; else if (x > fllx) px = llx;
			if (y < fl0y) py = l0y; else if (y > flly) py = lly;
			SizeT pixnum[4];
			DFloat dx = x - px;
			DFloat dy = y - py;
			pixnum[0]=px + py * lx;
			pixnum[1]=pixnum[0]+1;
			pixnum[2]=pixnum[0]+lx;
			pixnum[3]=pixnum[2]+1;
			DFloat a = data[pixnum[0]] + dx * (data[pixnum[1]]-data[pixnum[0]]);
			DFloat b = data[pixnum[2]] + dx * (data[pixnum[3]]-data[pixnum[2]]);
			res[i + j * nCols] = a + dy * (b-a);
		  }
		}
	  } else {
	  TRACEOMP(__FILE__,__LINE__)
  #pragma omp parallel for collapse(2) num_threads(GDL_NTHREADS)
		for (OMPInt j = 0; j < nRows; ++j) {
		  for (OMPInt i = 0; i < nCols; ++i) {
			DFloat x = poly_u->c[0];
			DFloat y = poly_v->c[0];
			for (auto k=1; k< nc; ++k) {
			  x+=poly_u->c[k]*xcoefu[j*nc+k]*ycoefu[i*nc+k];
			  y+=poly_v->c[k]*xcoefv[j*nc+k]*ycoefv[i*nc+k];
			}
			SizeT px=x;
			SizeT py=y;
			if (x < fl0x) px = l0x; else if (x > fllx) px = llx;
			if (y < fl0y) py = l0y; else if (y > flly) py = lly;
			SizeT pixnum[4];
			DFloat dx = x - px;
			DFloat dy = y - py;
			pixnum[0]=px + py * lx;
			pixnum[1]=pixnum[0]+1;
			pixnum[2]=pixnum[0]+lx;
			pixnum[3]=pixnum[2]+1;
			DFloat a = data[pixnum[0]] + dx * (data[pixnum[1]]-data[pixnum[0]]);
			DFloat b = data[pixnum[2]] + dx * (data[pixnum[3]]-data[pixnum[2]]);
			res[i + j * nCols] = a + dy * (b-a);
		  }
		}
	  }
	}
	free(xcoefu);
	free(ycoefu);
	free(xcoefv);
	free(ycoefv);
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
  BaseGDL* warp_linear2(
   const SizeT nCols,
    const SizeT nRows,
    BaseGDL* data_,
    DFloat * const P,
    DFloat * const Q,
    const DFloat cubicParameter,
    DFloat const initvalue_,
	const bool doMissing) {
//	std::cerr<<"warp_linear2\n";
    const SizeT lx = data_->Dim(0);
    const SizeT ly = data_->Dim(1);

	DFloat* kernel=cubicKernel; //already computed
	bool destroyKernel=false;
	//initialize kernel if not default 
	if (cubicParameter!=DEFAULT_CUBIC_PARAMETER) {
	  kernel = generate_interpolation_kernel(/*2, */cubicParameter);
	  destroyKernel = true;
	}	
    dimension dim(nCols, nRows);
    T1* res_ = new T1(dim, BaseGDL::NOZERO);
	T2 initvalue = initvalue_;
    const SizeT nEl = nCols*nRows;

    T2* res = (T2*) res_->DataAddr();
	T2* const data = (T2* const) data_->DataAddr();
    if (doMissing) {
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (OMPInt i = 0; i < nCols * nRows; ++i) res[i] = initvalue;
      } else {
      TRACEOMP(__FILE__,__LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < nCols * nRows; ++i) res[i] = initvalue;
      }
    }
   /* Pre compute leaps for 16 closest neighbors positions */
    SizeT leaps[4][4];
    leaps[0][0] = -1 - lx;
    leaps[0][1] = 0  - lx;
    leaps[0][2] = 1  - lx;
    leaps[0][3] = 2  - lx;

    leaps[1][0] = -1;
    leaps[1][1] = 0;
    leaps[1][2] = 1;
    leaps[1][3] = 2;

    leaps[2][0] = -1 + lx;
    leaps[2][1] = 0  +lx;
    leaps[2][2] = 1 + lx;
    leaps[2][3] = 2 + lx;

    leaps[3][0] = -1 + 2 * lx;
    leaps[3][1] = 0 + 2 * lx;
    leaps[3][2] = 1 + 2 * lx;
    leaps[3][3] = 2 + 2 * lx;

	//these are accelerators - not using them increase exec time by 2 or 3.
	const DFloat xmax = lx;
	const DFloat ymax = ly;
	const DFloat xmin = 1;
	const DFloat ymin = 1;
	const DFloat xbound = lx - 2; //-2 for cubic
	const DFloat ybound = ly - 2;
	const DFloat p0 = P[0];
	const DFloat q0 = Q[0];
	const DFloat p1 = P[1];
	const DFloat q1 = Q[1];
	const DFloat p2 = P[2];
	const DFloat q2 = Q[2];
	const DFloat p3 = P[3];
	const DFloat q3 = Q[3];
	DFloat p1j, p3j, q1j, q3j;

    /* Double loop on the output internal image  */
    if ((GDL_NTHREADS=parallelize( nEl ))==1) {
		for (OMPInt j = 0; j < nRows; ++j) {
		p1j = p0 + p1 * j;
		p3j = p2 + p3 * j;
		q1j = q0 + q1 * j;
		q3j = q2 + q3 * j;
		for (OMPInt i = 0; i < nCols; ++i) {
		  // Compute the original source for this pixel, note order of j and i in P and Q definition of IDL doc.
		  DFloat x = p1j + p3j * i;
		  if (doMissing) {
			if (x < xmin) continue;
			if (x >= xmax) continue;
		  }
		  DFloat y = q1j + q3j * i;
		  if (doMissing) {
			if (y < ymin) continue;
			if (y >= ymax) continue;
		  }
		  SizeT px;
		  SizeT py;
		  DFloat dx;
		  DFloat dy;
		  if (x >= xbound) {
			px = lx - 3;
			dx = 1;
		  } else if (x < xmin) {
			px = 1;
			dx = 0;
		  } else {
			px = x;
			dx = x - px;
		  }
		  if (y >= ybound) {
			py = ly - 3;
			dy = 1;
		  } else if (y < ymin) {
			py = 1;
			dy = 0;
		  } else {
			py = y;
			dy = y - py;
		  }
		  DFloat pix[4][4];
		  SizeT curpix=py*lx+px;
		  pix[0][0]=data[curpix+leaps[0][0]];
		  pix[0][1]=data[curpix+leaps[0][1]];
		  pix[0][2]=data[curpix+leaps[0][2]];
		  pix[0][3]=data[curpix+leaps[0][3]];
		  pix[1][0]=data[curpix+leaps[1][0]];
		  pix[1][1]=data[curpix+leaps[1][1]];
		  pix[1][2]=data[curpix+leaps[1][2]];
		  pix[1][3]=data[curpix+leaps[1][3]];
		  pix[2][0]=data[curpix+leaps[2][0]];
		  pix[2][1]=data[curpix+leaps[2][1]];
		  pix[2][2]=data[curpix+leaps[2][2]];
		  pix[2][3]=data[curpix+leaps[2][3]];
		  pix[3][0]=data[curpix+leaps[3][0]];
		  pix[3][1]=data[curpix+leaps[3][1]];
		  pix[3][2]=data[curpix+leaps[3][2]];
		  pix[3][3]=data[curpix+leaps[3][3]];
		  DFloat val = bicubicInterpolate(pix, dx, dy, kernel); 
		  if (data_->Type()==GDL_BYTE) {
			res[i + j * nCols] = val > 255 ? 255:val < 0? 0: val;
			} else  res[i + j * nCols] = val;
          }
        }
	  } else {
    TRACEOMP(__FILE__,__LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS) private(p1j,p3j,q1j,q3j)
		for (OMPInt j = 0; j < nRows; ++j) {
		p1j = p0 + p1 * j;
		p3j = p2 + p3 * j;
		q1j = q0 + q1 * j;
		q3j = q2 + q3 * j;
		for (OMPInt i = 0; i < nCols; ++i) {
		  // Compute the original source for this pixel, note order of j and i in P and Q definition of IDL doc.
		  DFloat x = p1j + p3j * i;
		  if (doMissing) {
			if (x < xmin) continue;
			if (x >= xmax) continue;
		  }
		  DFloat y = q1j + q3j * i;
		  if (doMissing) {
			if (y < ymin) continue;
			if (y >= ymax) continue;
		  }
		  SizeT px;
		  SizeT py;
		  DFloat dx;
		  DFloat dy;
		  if (x >= xbound) {
			px = lx - 3 ;
			dx = 1;
		  } else if (x < xmin) {
			px = 0;
			dx = 0;
		  } else {
			px = x;
			dx = x - px;
		  }
		  if (y >= ybound) {
			py = ly - 3;
			dy = 1;
		  } else if (y < ymin) {
			py = 1;
			dy = 0;
		  } else {
			py = y;
			dy = y - py;
		  }
		  DFloat pix[4][4];
		  SizeT curpix=py*lx+px;
		  pix[0][0]=data[curpix+leaps[0][0]];
		  pix[0][1]=data[curpix+leaps[0][1]];
		  pix[0][2]=data[curpix+leaps[0][2]];
		  pix[0][3]=data[curpix+leaps[0][3]];
		  pix[1][0]=data[curpix+leaps[1][0]];
		  pix[1][1]=data[curpix+leaps[1][1]];
		  pix[1][2]=data[curpix+leaps[1][2]];
		  pix[1][3]=data[curpix+leaps[1][3]];
		  pix[2][0]=data[curpix+leaps[2][0]];
		  pix[2][1]=data[curpix+leaps[2][1]];
		  pix[2][2]=data[curpix+leaps[2][2]];
		  pix[2][3]=data[curpix+leaps[2][3]];
		  pix[3][0]=data[curpix+leaps[3][0]];
		  pix[3][1]=data[curpix+leaps[3][1]];
		  pix[3][2]=data[curpix+leaps[3][2]];
		  pix[3][3]=data[curpix+leaps[3][3]];
		  DFloat val = bicubicInterpolate(pix, dx, dy, kernel);
		  if (data_->Type()==GDL_BYTE) {
			res[i + j * nCols] = val > 255 ? 255:val < 0? 0: val;
			} else  res[i + j * nCols] =val;
          }
        }
	}
	if (destroyKernel) free(kernel);
    return res_;
  }
  
  template< typename T1, typename T2>
  BaseGDL* warp2(
    const SizeT nCols,
    const SizeT nRows,
    BaseGDL* const data_,
    const DFloat cubicParameter,
    poly2d* const poly_u,
    poly2d* const poly_v,
    const DFloat initvalue_,
    bool doMissing) {
//	std::cerr<<"warp2\n";
	const SizeT lx = data_->Dim(0);
	const SizeT ly = data_->Dim(1);

	DFloat* kernel=cubicKernel; //already computed
	bool destroyKernel=false;
	//initialize kernel if not default 
	if (cubicParameter!=DEFAULT_CUBIC_PARAMETER) {
	  kernel = generate_interpolation_kernel(/*2, */cubicParameter);
	  destroyKernel = true;
	}	
    dimension dim(nCols, nRows);
    T1* res_ = new T1(dim, BaseGDL::NOZERO);
	T2 initvalue = initvalue_;
    const SizeT nEl = nCols*nRows;

    T2* res = (T2*) res_->DataAddr();
	T2* const data = (T2* const) data_->DataAddr();
    if (doMissing) {
      if ((GDL_NTHREADS=parallelize( nEl))==1) {
        for (OMPInt i = 0; i < nCols * nRows; ++i) res[i] = initvalue;
      } else {
      TRACEOMP(__FILE__,__LINE__)
#pragma omp parallel for num_threads(GDL_NTHREADS)
        for (OMPInt i = 0; i < nCols * nRows; ++i) res[i] = initvalue;
      }
    }
   /* Pre compute leaps for 16 closest neighbors positions */
    SizeT leaps[4][4];
    leaps[0][0] = -1 - lx;
    leaps[0][1] = 0  - lx;
    leaps[0][2] = 1  - lx;
    leaps[0][3] = 2  - lx;

    leaps[1][0] = -1;
    leaps[1][1] = 0;
    leaps[1][2] = 1;
    leaps[1][3] = 2;

    leaps[2][0] = -1 + lx;
    leaps[2][1] = 0  +lx;
    leaps[2][2] = 1 + lx;
    leaps[2][3] = 2 + lx;

    leaps[3][0] = -1 + 2 * lx;
    leaps[3][1] = 0 + 2 * lx;
    leaps[3][2] = 1 + 2 * lx;
    leaps[3][3] = 2 + 2 * lx;

	//these are accelerators - not using them increase exec time by 2 or 3.
	const DFloat xmax = lx;
	const DFloat ymax = ly;
	const DFloat xmin = 1;
	const DFloat ymin = 1;
	const DFloat xbound = lx - 2; //-2 for cubic
	const DFloat ybound = ly - 2;
	DLong nc = poly_u->nc;
	DFloat * const xcoefu=poly2d_compute_init_x(poly_u,lx);
	DFloat * const ycoefu=poly2d_compute_init_y(poly_u,lx);
	DFloat * const xcoefv=poly2d_compute_init_x(poly_v,ly);
	DFloat * const ycoefv=poly2d_compute_init_y(poly_v,ly);

    /* Double loop on the output internal image  */
    if ((GDL_NTHREADS=parallelize( nEl ))==1) {
		for (OMPInt j = 0; j < nRows; ++j) {
		for (OMPInt i = 0; i < nCols; ++i) {
		  // Compute the original source for this pixel, note order of j and i in P and Q definition of IDL doc.
			DFloat x = poly_u->c[0];
			for (auto k = 1; k < nc; ++k) x += poly_u->c[k] * xcoefu[j * nc + k] * ycoefu[i * nc + k];
		  if (doMissing) {
			if (x < xmin) continue;
			if (x >= xmax) continue;
		  }
			DFloat y = poly_v->c[0];
			for (auto k = 1; k < nc; ++k) y += poly_v->c[k] * xcoefv[j * nc + k] * ycoefv[i * nc + k];
		  if (doMissing) {
			if (y < ymin) continue;
			if (y >= ymax) continue;
		  }
		  SizeT px;
		  SizeT py;
		  DFloat dx;
		  DFloat dy;
		  if (x >= xbound) {
			px = lx - 3;
			dx = 1;
		  } else if (x < xmin) {
			px = 1;
			dx = 0;
		  } else {
			px = x;
			dx = x - px;
		  }
		  if (y >= ybound) {
			py = ly - 3;
			dy = 1;
		  } else if (y < ymin) {
			py = 1;
			dy = 0;
		  } else {
			py = y;
			dy = y - py;
		  }
		  DFloat pix[4][4];
		  SizeT curpix=py*lx+px;
		  pix[0][0]=data[curpix+leaps[0][0]];
		  pix[0][1]=data[curpix+leaps[0][1]];
		  pix[0][2]=data[curpix+leaps[0][2]];
		  pix[0][3]=data[curpix+leaps[0][3]];
		  pix[1][0]=data[curpix+leaps[1][0]];
		  pix[1][1]=data[curpix+leaps[1][1]];
		  pix[1][2]=data[curpix+leaps[1][2]];
		  pix[1][3]=data[curpix+leaps[1][3]];
		  pix[2][0]=data[curpix+leaps[2][0]];
		  pix[2][1]=data[curpix+leaps[2][1]];
		  pix[2][2]=data[curpix+leaps[2][2]];
		  pix[2][3]=data[curpix+leaps[2][3]];
		  pix[3][0]=data[curpix+leaps[3][0]];
		  pix[3][1]=data[curpix+leaps[3][1]];
		  pix[3][2]=data[curpix+leaps[3][2]];
		  pix[3][3]=data[curpix+leaps[3][3]];
		  DFloat val = bicubicInterpolate(pix, dx, dy, kernel); 
		  if (data_->Type()==GDL_BYTE) {
			res[i + j * nCols] = val > 255 ? 255:val < 0? 0: val;
			} else  res[i + j * nCols] = val;
          }
        }
	  } else {
    TRACEOMP(__FILE__,__LINE__)
#pragma omp parallel for collapse(2) num_threads(GDL_NTHREADS)
		for (OMPInt j = 0; j < nRows; ++j) {
		for (OMPInt i = 0; i < nCols; ++i) {
		  // Compute the original source for this pixel, note order of j and i in P and Q definition of IDL doc.
			DFloat x = poly_u->c[0];
			for (auto k = 1; k < nc; ++k) x += poly_u->c[k] * xcoefu[j * nc + k] * ycoefu[i * nc + k];
		  if (doMissing) {
			if (x < xmin) continue;
			if (x >= xmax) continue;
		  }
			DFloat y = poly_v->c[0];
			for (auto k = 1; k < nc; ++k) y += poly_v->c[k] * xcoefv[j * nc + k] * ycoefv[i * nc + k];
		  if (doMissing) {
			if (y < ymin) continue;
			if (y >= ymax) continue;
		  }
		  SizeT px;
		  SizeT py;
		  DFloat dx;
		  DFloat dy;
		  if (x >= xbound) {
			px = lx - 3 ;
			dx = 1;
		  } else if (x < xmin) {
			px = 0;
			dx = 0;
		  } else {
			px = x;
			dx = x - px;
		  }
		  if (y >= ybound) {
			py = ly - 3;
			dy = 1;
		  } else if (y < ymin) {
			py = 1;
			dy = 0;
		  } else {
			py = y;
			dy = y - py;
		  }
		  DFloat pix[4][4];
		  SizeT curpix=py*lx+px;
		  pix[0][0]=data[curpix+leaps[0][0]];
		  pix[0][1]=data[curpix+leaps[0][1]];
		  pix[0][2]=data[curpix+leaps[0][2]];
		  pix[0][3]=data[curpix+leaps[0][3]];
		  pix[1][0]=data[curpix+leaps[1][0]];
		  pix[1][1]=data[curpix+leaps[1][1]];
		  pix[1][2]=data[curpix+leaps[1][2]];
		  pix[1][3]=data[curpix+leaps[1][3]];
		  pix[2][0]=data[curpix+leaps[2][0]];
		  pix[2][1]=data[curpix+leaps[2][1]];
		  pix[2][2]=data[curpix+leaps[2][2]];
		  pix[2][3]=data[curpix+leaps[2][3]];
		  pix[3][0]=data[curpix+leaps[3][0]];
		  pix[3][1]=data[curpix+leaps[3][1]];
		  pix[3][2]=data[curpix+leaps[3][2]];
		  pix[3][3]=data[curpix+leaps[3][3]];
		  DFloat val = bicubicInterpolate(pix, dx, dy, kernel);
		  if (data_->Type()==GDL_BYTE) {
			res[i + j * nCols] = val > 255 ? 255:val < 0? 0: val;
			} else  res[i + j * nCols] =val;
          }
        }
	}
	if (destroyKernel) free(kernel);
    return res_;
  }


  BaseGDL* poly_2d_fun(EnvT* e) {
    /*
    The poly_2d code is inspired by "ECLIPSE", the  ESO C Library for an Image Processing Software Environment
    N. Devillard, "The eclipse software", The messenger No 87 - March 1997 http://www.eso.org/projects/aot/eclipse/
     */

    SizeT nParam = e->NParam(3);

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
    DFloat cubicParameter = DEFAULT_CUBIC_PARAMETER;
	//initialize default kernels if necessary 
	if (!cubicKernelInitialized) {
	  cubicKernel = generate_interpolation_kernel(/*2,*/ DEFAULT_CUBIC_PARAMETER);
	  cubicKernelInitialized=true;
	}
	// but will create a temporary kernel if needed
    static int CUBICIx = e->KeywordIx("CUBIC");
    if (e->KeywordSet(CUBICIx)) {
      e->AssureFloatScalarKWIfPresent(CUBICIx, cubicParameter);
      interp = 2; //this is what IDL does, whatever the previous velue of 'interp'
	  if (cubicParameter > 0) cubicParameter=-1;
//	  if (cubicParameter < -1) cubicParameter=0; // no, IDL does not filter those negative values
    }

    static int MISSINGIx = e->KeywordIx("MISSING");
    DFloat missing = 0.0;
    bool doMissing = (e->KeywordPresent(MISSINGIx));
    if (doMissing) {
      e->AssureFloatScalarKWIfPresent(MISSINGIx, missing);
    }
	
    static int PIXELIx = e->KeywordIx("PIXEL_CENTER");
    bool doPix = (e->KeywordPresent(PIXELIx));
	DFloat pixcenter = 0.0;
	if (doPix) {
	  e->AssureFloatScalarKWIfPresent(PIXELIx, pixcenter);
	}
	
	// check P dimension first
    DLong Psize=p1->N_Elements();
    if (Psize < 4) e->Throw("Value of Polynomial degree is out of allowed range.");
    
    DFloat dPdim = sqrt((DFloat) Psize);
    DLong nDegree = (DLong) dPdim -1;
    DLong nc=(nDegree + 1) * (nDegree + 1);
    if ( p2->N_Elements() < nc ) e->Throw("Coefficient arrays must have (degree+1)^2 elements");

    DFloatGDL* P = static_cast<DFloatGDL*>
      (p1->Convert2(GDL_FLOAT, BaseGDL::COPY));

    DFloatGDL* Q = static_cast<DFloatGDL*>
      (p2->Convert2(GDL_FLOAT, BaseGDL::COPY));
	(*P)[0]+=pixcenter;
	(*Q)[0]+=pixcenter;

    if (nDegree == 1 ) { //&& (*P)[3] == 0 && (*Q)[3] == 0 ) { //LINEAR CASE
      //return p0 if identity.
      if ((*P)[0] == 0 && (*P)[1] == 0 && (*P)[2] == 1 && (*P)[3] == 0 && (*Q)[0] == 0 && (*Q)[1] == 1 && (*Q)[2] == 0 && (*Q)[3] == 0) {
        return p0->Dup();
      }
      if (interp==0) {
	if (p0->Type() == GDL_BYTE) {
          return warp_linear0< DByteGDL, DByte>(nCol, nRow, p0, (DFloat*) P->DataAddr(),(DFloat*) Q->DataAddr(), missing, doMissing);
        } else if (p0->Type() == GDL_INT) {
          return warp_linear0< DIntGDL, DInt>(nCol, nRow, p0, (DFloat*) P->DataAddr(),(DFloat*) Q->DataAddr(), missing, doMissing);
        } else if (p0->Type() == GDL_UINT) {
          return warp_linear0< DUIntGDL, DUInt>(nCol, nRow, p0, (DFloat*) P->DataAddr(),(DFloat*) Q->DataAddr(), missing, doMissing);
        } else if (p0->Type() == GDL_LONG) {
          return warp_linear0< DLongGDL, DLong>(nCol, nRow, p0, (DFloat*) P->DataAddr(),(DFloat*) Q->DataAddr(), missing, doMissing);
        } else if (p0->Type() == GDL_ULONG) {
          return warp_linear0< DULongGDL, DULong>(nCol, nRow, p0, (DFloat*) P->DataAddr(),(DFloat*) Q->DataAddr(), missing, doMissing);
        } else if (p0->Type() == GDL_LONG64) {
          return warp_linear0< DLong64GDL, DLong64>(nCol, nRow, p0, (DFloat*) P->DataAddr(),(DFloat*) Q->DataAddr(), missing, doMissing);
        } else if (p0->Type() == GDL_ULONG64) {
          return warp_linear0< DULong64GDL, DULong64>(nCol, nRow, p0, (DFloat*) P->DataAddr(),(DFloat*) Q->DataAddr(), missing, doMissing);
        } else if (p0->Type() == GDL_FLOAT) {
          return warp_linear0< DFloatGDL, DFloat>(nCol, nRow, p0, (DFloat*) P->DataAddr(),(DFloat*) Q->DataAddr(), missing, doMissing);
        } else if (p0->Type() == GDL_DOUBLE) {
          return warp_linear0< DDoubleGDL, DDouble>(nCol, nRow, p0, (DFloat*) P->DataAddr(),(DFloat*) Q->DataAddr(), missing, doMissing);
        }
      } else if (interp==1) {
         if (p0->Type() == GDL_BYTE) {
          return warp_linear1< DByteGDL, DByte>(nCol, nRow, p0, (DFloat*) P->DataAddr(),(DFloat*) Q->DataAddr(), missing, doMissing);
        } else if (p0->Type() == GDL_INT) {
          return warp_linear1< DIntGDL, DInt>(nCol, nRow, p0, (DFloat*) P->DataAddr(),(DFloat*) Q->DataAddr(), missing, doMissing);
        } else if (p0->Type() == GDL_UINT) {
          return warp_linear1< DUIntGDL, DUInt>(nCol, nRow, p0, (DFloat*) P->DataAddr(),(DFloat*) Q->DataAddr(), missing, doMissing);
        } else if (p0->Type() == GDL_LONG) {
          return warp_linear1< DLongGDL, DLong>(nCol, nRow, p0, (DFloat*) P->DataAddr(),(DFloat*) Q->DataAddr(), missing, doMissing);
        } else if (p0->Type() == GDL_ULONG) {
          return warp_linear1< DULongGDL, DULong>(nCol, nRow, p0, (DFloat*) P->DataAddr(),(DFloat*) Q->DataAddr(), missing, doMissing);
        } else if (p0->Type() == GDL_LONG64) {
          return warp_linear1< DLong64GDL, DLong64>(nCol, nRow, p0, (DFloat*) P->DataAddr(),(DFloat*) Q->DataAddr(), missing, doMissing);
        } else if (p0->Type() == GDL_ULONG64) {
          return warp_linear1< DULong64GDL, DULong64>(nCol, nRow, p0, (DFloat*) P->DataAddr(),(DFloat*) Q->DataAddr(), missing, doMissing);
        } else if (p0->Type() == GDL_FLOAT) {
          return warp_linear1< DFloatGDL, DFloat>(nCol, nRow, p0, (DFloat*) P->DataAddr(),(DFloat*) Q->DataAddr(), missing, doMissing);
        } else if (p0->Type() == GDL_DOUBLE) {
          return warp_linear1< DDoubleGDL, DDouble>(nCol, nRow, p0, (DFloat*) P->DataAddr(),(DFloat*) Q->DataAddr(), missing, doMissing);
        }
      } else if (interp==2) {
         if (p0->Type() == GDL_BYTE) {
          return warp_linear2< DByteGDL, DByte>(nCol, nRow, p0, (DFloat*) P->DataAddr(),(DFloat*) Q->DataAddr(), cubicParameter, missing, doMissing);
        } else if (p0->Type() == GDL_INT) {
          return warp_linear2< DIntGDL, DInt>(nCol, nRow, p0, (DFloat*) P->DataAddr(),(DFloat*) Q->DataAddr(), cubicParameter, missing, doMissing);
        } else if (p0->Type() == GDL_UINT) {
          return warp_linear2< DUIntGDL, DUInt>(nCol, nRow, p0, (DFloat*) P->DataAddr(),(DFloat*) Q->DataAddr(), cubicParameter, missing, doMissing);
        } else if (p0->Type() == GDL_LONG) {
          return warp_linear2< DLongGDL, DLong>(nCol, nRow, p0, (DFloat*) P->DataAddr(),(DFloat*) Q->DataAddr(), cubicParameter, missing, doMissing);
        } else if (p0->Type() == GDL_ULONG) {
          return warp_linear2< DULongGDL, DULong>(nCol, nRow, p0, (DFloat*) P->DataAddr(),(DFloat*) Q->DataAddr(), cubicParameter, missing, doMissing);
        } else if (p0->Type() == GDL_LONG64) {
          return warp_linear2< DLong64GDL, DLong64>(nCol, nRow, p0, (DFloat*) P->DataAddr(),(DFloat*) Q->DataAddr(), cubicParameter, missing, doMissing);
        } else if (p0->Type() == GDL_ULONG64) {
          return warp_linear2< DULong64GDL, DULong64>(nCol, nRow, p0, (DFloat*) P->DataAddr(),(DFloat*) Q->DataAddr(), cubicParameter, missing, doMissing);
        } else if (p0->Type() == GDL_FLOAT) {
          return warp_linear2< DFloatGDL, DFloat>(nCol, nRow, p0, (DFloat*) P->DataAddr(),(DFloat*) Q->DataAddr(), cubicParameter, missing, doMissing);
        } else if (p0->Type() == GDL_DOUBLE) {
          return warp_linear2< DDoubleGDL, DDouble>(nCol, nRow, p0, (DFloat*) P->DataAddr(),(DFloat*) Q->DataAddr(), cubicParameter, missing, doMissing);
        }
      }
    } 
	
	else {
    
    //NON-LINEAR Polynomial

    poly2d* poly_u;
    poly2d* poly_v;

    poly_u = (poly2d *) malloc(sizeof (poly2d));
    poly_u->nc = nc;
    poly_u->px = (DLong *) malloc(nc * sizeof (DLong));
    poly_u->py = (DLong *) malloc(nc * sizeof (DLong));
    poly_u->c = (DFloat *) malloc(nc * sizeof (DFloat));

    for (SizeT i = 0; i < (nDegree + 1)*(nDegree + 1); ++i) {
      poly_u->px[i] = i / (nDegree + 1);
      poly_u->py[i] = i - (poly_u->px[i] * (nDegree + 1));
      poly_u->c[i] = (*P)[poly_u->px[i]+(nDegree + 1) * poly_u->py[i]];
    }

    poly_v = (poly2d *) malloc(sizeof (poly2d));
    poly_v->nc = nc;
    poly_v->px = (DLong *) malloc(nc * sizeof (DLong));
    poly_v->py = (DLong *) malloc(nc * sizeof (DLong));
    poly_v->c = (DFloat *) malloc(nc * sizeof (DFloat));

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
}
