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

namespace lib {

  // Function to return a^n

  int powerOptimised(OMPInt a, OMPInt n) {

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


  DFloat * poly2d_compute_init_x(poly2d * p,	SizeT n) {
	DFloat * res= (DFloat*) malloc(p->nc*n*sizeof(DFloat));
	for (auto i=0, s=0; i< n; ++i) for (DLong k = 0; k < p->nc; k++) res[s++]=powerOptimised(i, p->px[k]);
	return res;
  }
  DFloat* poly2d_compute_init_y(poly2d * p,	SizeT n) {
	DFloat * res= (DFloat*) malloc(p->nc*n*sizeof(DFloat));
	for (auto i=0,s=0; i< n; ++i) for (DLong k = 0; k < p->nc; k++) res[s++]=powerOptimised(i, p->py[k]);
	return res;
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
	std::cerr<<"warp_linear0\n";
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
	DFloat fllx=lx-1;
	DFloat flly=ly-1;
	SizeT llx = fllx;
	SizeT lly = flly;
	DFloat fl0x=0;
	DFloat fl0y=0;
	SizeT l0x = fl0x;
	SizeT l0y = fl0y;
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
			if (x >= lx) continue;
			DFloat y = q1j + q3j * i;
			if (y < fl0y) continue;
			if (y >= ly) continue;
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
			if (x >= lx) continue;
			DFloat y = q1j + q3j * i;
			if (y < fl0y) continue;
			if (y >= ly) continue;
			SizeT px=x;
			SizeT py=y;
			res[i + j * nCols] = data[px + py * lx];
		  }
		}
	  }
	  
	} else {
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
	std::cerr<<"warp0\n";
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
	//these are accelerators - not using them increase exec time by 2 or 3.
	DFloat fllx = lx - 1;
	DFloat flly = ly - 1;
	SizeT llx = fllx;
	SizeT lly = flly;
	DFloat fl0x=0;
	DFloat fl0y=0;
	SizeT l0x = fl0x;
	SizeT l0y = fl0y;
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
		  if (x >= lx) continue; // already initialised to 'missing' value.
          DFloat y = poly_v->c[0]; for (auto k=1; k< nc; ++k) y+=poly_v->c[k]*xcoefv[j*nc+k]*ycoefv[i*nc+k];
		  if (y < fl0y) continue;
		  if (y >= ly)	continue;
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
		  if (x >= lx) continue; // already initialised to 'missing' value.
          DFloat y = poly_v->c[0]; for (auto k=1; k< nc; ++k) y+=poly_v->c[k]*xcoefv[j*nc+k]*ycoefv[i*nc+k];
		  if (y < fl0y) continue;
		  if (y >= ly)	continue;
		  SizeT px = x;
		  SizeT py = y;
		  res[i + j * nCols] = data[px + py * lx];
		}
	  }
	}
	} else {
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
	std::cerr<<"warp_linear0\n";
    SizeT lx = data_->Dim(0);
    SizeT ly = data_->Dim(1);

    dimension dim(nCols, nRows);
    T1* res_ = new T1(dim, BaseGDL::NOZERO);
	T2 initvalue = initvalue_;
    SizeT nEl = nCols*nRows;

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
	DFloat fllx=lx-2;  //-2 for linear
	DFloat flly=ly-2;
	SizeT llx = fllx;
	SizeT lly = flly;
	DFloat fl0x=1; //1 for linear
	DFloat fl0y=1;
	SizeT l0x = fl0x;
	SizeT l0y = fl0y;
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
			if (x < fl0x) continue;
			if (x >= fllx) continue;
			DFloat y = q1j + q3j * i;
			if (y < fl0y) continue;
			if (y >= flly) continue;
			SizeT px=x;
			SizeT py=y;
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
			//			DFloat umdx=1 - dx;
			//			DFloat umdy=1 - dy;
            // same time:			res[i + j * nCols] = data[pixnum[0]]*umdx*umdy+data[pixnum[1]]*dx*umdy+data[pixnum[2]]*dy*umdx+data[pixnum[3]]*dx*dy;
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
		  if (x < fl0x) continue;
		  if (x >= fllx) continue;
		  DFloat y = q1j + q3j * i;
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
		  //			DFloat umdx=1 - dx;
		  //			DFloat umdy=1 - dy;
		  // same time:			res[i + j * nCols] = data[pixnum[0]]*umdx*umdy+data[pixnum[1]]*dx*umdy+data[pixnum[2]]*dy*umdx+data[pixnum[3]]*dx*dy;
		}
	  }
	}
  }	else {
	  /* Double loop on the output internal image  */
	  if ((GDL_NTHREADS = parallelize(nEl)) == 1) {
     for (OMPInt j = 0; j < nRows; ++j) {
		  p1j = p0 + p1 * j;
		  p3j = p2 + p3 * j;
		  q1j = q0 + q1 * j;
		  q3j = q2 + q3 * j;
        for (OMPInt i = 0; i < nCols; ++i) {
		  DFloat x = p1j + p3j * i;
		  SizeT px=x;
		  if (x < fl0x) px = l0x; else if (x > fllx) px = llx;
		  DFloat y = q1j + q3j * i;
		  SizeT py=y;
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
#pragma omp parallel for num_threads(GDL_NTHREADS) private(p1j,p3j,q1j,q3j)
		for (OMPInt j = 0; j < nRows; ++j) {
		p1j = p0 + p1 * j;
		p3j = p2 + p3 * j;
		q1j = q0 + q1 * j;
		q3j = q2 + q3 * j;
		for (OMPInt i = 0; i < nCols; ++i) {
		  DFloat x = p1j + p3j * i;
		  SizeT px=x;
		  if (x < fl0x) px = l0x; else if (x > fllx) px = llx;
	      DFloat y = q1j + q3j * i;
		  SizeT py=y;
		  if (y < fl0y) py = l0y; else if (y > flly) py = lly;
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
	std::cerr<<"warp1\n";
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
	DFloat fllx = lx - 2;
	DFloat flly = ly - 2;
	SizeT llx = fllx;
	SizeT lly = flly;
	DFloat fl0x=1;
	DFloat fl0y=1;
	SizeT l0x = fl0x;
	SizeT l0y = fl0y;
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
			if (x >= lx-1) continue;
			DFloat y = poly_v->c[0];
			for (auto k = 1; k < nc; ++k) y += poly_v->c[k] * xcoefv[j * nc + k] * ycoefv[i * nc + k];
			if (y < fl0y) continue;
			if (y >= ly-1) continue;
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
			if (x >= lx-1) continue;
			DFloat y = poly_v->c[0];
			for (auto k = 1; k < nc; ++k) y += poly_v->c[k] * xcoefv[j * nc + k] * ycoefv[i * nc + k];
			if (y < fl0y) continue;
			if (y >= ly-1) continue;
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
	std::cerr<<"warp_linear0\n";
    SizeT lx = data_->Dim(0);
    SizeT ly = data_->Dim(1);

    dimension dim(nCols, nRows);
    T1* res_ = new T1(dim, BaseGDL::NOZERO);
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
	std::cerr<<"warp2\n";
	const SizeT lx = data_->Dim(0);
	const SizeT ly = data_->Dim(1);

    dimension dim(nCols, nRows);
    T1* res_ = new T1(dim, BaseGDL::NOZERO);
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
    static int CUBICIx = e->KeywordIx("CUBIC");
    DFloat cubicParameter = -0.5;
    if (e->KeywordSet(CUBICIx)) {
      e->AssureFloatScalarKWIfPresent(CUBICIx, cubicParameter);
      interp = 2;
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
