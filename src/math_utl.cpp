/***************************************************************************
                          math_utl.cpp  -  math utilities GDL library function
                             -------------------
    begin                : Feb 11 2004
    copyright            : (C) 2004 by Joel Gales
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

#include <complex>
#include <cmath>
#include <iostream>

#include "math_utl.hpp"
#include "dimension.hpp"
#include "initsysvar.hpp"

//#define GDL_DEBUG
#undef GDL_DEBUG

namespace lib {

/*

Based on Algorithm 513, a revised version of Algorithm 380.

From ACM Trans. Math. Soft. Vol 3, #1, March 1977, pg 104-110

Esko G. Cate & David W. Twigg

*/

//   int trans513(char *array, SizeT n, SizeT m, SizeT sz, DType type)
//   {
//     int i,j,k;
//     int ncount;
//     int ir0, ir1, ir2;
//     int im, i1, kmi, i1c, i2, i2c, n1, j1;
//     int max;
//     int move[2000];
//     int iwrk;

//     char b[32], c[32], d[32];

//     iwrk = (n + m) / 2;

//     if (m < 2 || n < 2) {
//       return 0;
//     }

//     if (m == n) goto lbl_130;

//     ncount = 2;
//     /*    printf("%d\n", ncount);*/
//     k = (m*n) - 1;

//     if (m < 3 || n < 3) goto lbl_30;

//     ir2 = m - 1;
//     ir1 = n - 1;
//     ir0 = 1;

//     /* GCD (m-1, n-1) */
//     /* -------------- */
//     while (ir0 != 0) {
//       ir0 = ir2 % ir1;
//       ir2 = ir1;
//       ir1 = ir0;
//       /*      printf("in gcd\n");*/
//     }
    
//     ncount += ir2 - 1;
//     /*    printf("%d\n", ncount);*/

//   lbl_30:
//     i = 1;
//     im = m;

//     goto lbl_80;

//   lbl_40:
//     max = k - i;
//     i++;
//     if (i > max) goto lbl_160;
//     im += m;
//     if (im > k) im -= k;
//     i2 = im;

//     if (i == i2) goto lbl_40;
//     if (i > iwrk) goto lbl_60;
//     if (move[i-1] == 0) goto lbl_80;

//     goto lbl_40;

//   lbl_50:
//     i2 = m * i1 - k * (i1/n);
//   lbl_60:
//     if (i2 <= i || i2 >= max) goto lbl_70;
//     i1 = i2;
//     goto lbl_50;

//   lbl_70:
//     if (i2 != i) goto lbl_40;
    
//   lbl_80:
//     i1 = i;
//     kmi = k - i;
//     i1c = kmi;

//     switch (type) {
//     case BYTE:
//       *(DByte *) b = *(DByte *) &array[sz*i1];
//       *(DByte *) c = *(DByte *) &array[sz*i1c];
//       break;

//     case INT:
//       *(DInt *) b = *(DInt *) &array[sz*i1];
//       *(DInt *) c = *(DInt *) &array[sz*i1c];
//       break;

//     case UINT:
//       *(DUInt *) b = *(DUInt *) &array[sz*i1];
//       *(DUInt *) c = *(DUInt *) &array[sz*i1c];
//       break;

//     case LONG:
//       *(DLong *) b = *(DLong *) &array[sz*i1];
//       *(DLong *) c = *(DLong *) &array[sz*i1c];
//       break;

//     case ULONG:
//       *(DULong *) b = *(DULong *) &array[sz*i1];
//       *(DULong *) c = *(DULong *) &array[sz*i1c];
//       break;

//     case FLOAT:
//       *(DFloat *) b = *(DFloat *) &array[sz*i1];
//       *(DFloat *) c = *(DFloat *) &array[sz*i1c];
//       break;

//     case DOUBLE:
//       *(DDouble *) b = *(DDouble *) &array[sz*i1];
//       *(DDouble *) c = *(DDouble *) &array[sz*i1c];
//       break;

//     case COMPLEX:
//       *(DComplex *) b = *(DComplex *) &array[sz*i1];
//       *(DComplex *) c = *(DComplex *) &array[sz*i1c];
//       break;

//     case COMPLEXDBL:
//       *(DComplexDbl *) b = *(DComplexDbl *) &array[sz*i1];
//       *(DComplexDbl *) c = *(DComplexDbl *) &array[sz*i1c];
//       break;
//     }
    

//   lbl_90:
//     i2 = m * i1 - k * (i1/n);
//     i2c = k - i2;
//     if (i1 <= iwrk) move[i1-1] = 2;
//     if (i1c <= iwrk) move[i1c-1] = 2;
//     ncount += 2;
//     if (i2 == i) goto lbl_110;
//     if (i2 == kmi) goto lbl_100;

//     switch (type) {
//     case BYTE:
//       *(DByte *) &array[sz*i1] = *(DByte *) &array[sz*i2];
//       *(DByte *) &array[sz*i1c] = *(DByte *) &array[sz*i2c];
//       break;

//     case INT:
//       *(DInt *) &array[sz*i1] = *(DInt *) &array[sz*i2];
//       *(DInt *) &array[sz*i1c] = *(DInt *) &array[sz*i2c];
//       break;

//     case UINT:
//       *(DUInt *) &array[sz*i1] = *(DUInt *) &array[sz*i2];
//       *(DUInt *) &array[sz*i1c] = *(DUInt *) &array[sz*i2c];
//       break;

//     case LONG:
//       *(DLong *) &array[sz*i1] = *(DLong *) &array[sz*i2];
//       *(DLong *) &array[sz*i1c] = *(DLong *) &array[sz*i2c];
//       break;

//     case ULONG:
//       *(DULong *) &array[sz*i1] = *(DULong *) &array[sz*i2];
//       *(DULong *) &array[sz*i1c] = *(DULong *) &array[sz*i2c];
//       break;

//     case FLOAT:
//       *(DFloat *) &array[sz*i1] = *(DFloat *) &array[sz*i2];
//       *(DFloat *) &array[sz*i1c] = *(DFloat *) &array[sz*i2c];
//       break;

//     case DOUBLE:
//       *(DDouble *) &array[sz*i1] = *(DDouble *) &array[sz*i2];
//       *(DDouble *) &array[sz*i1c] = *(DDouble *) &array[sz*i2c];
//       break;

//     case COMPLEX:
//       *(DComplex *) &array[sz*i1] = *(DComplex *) &array[sz*i2];
//       *(DComplex *) &array[sz*i1c] = *(DComplex *) &array[sz*i2c];
//       break;

//     case COMPLEXDBL:
//       *(DComplexDbl *) &array[sz*i1] = *(DComplexDbl *) &array[sz*i2];
//       *(DComplexDbl *) &array[sz*i1c] = *(DComplexDbl *) &array[sz*i2c];
//       break;
//     }

//     i1 = i2;
//     i1c = i2c;
//     goto lbl_90;

//   lbl_100:
//     memcpy(d, b, sz);
//     memcpy(b, c, sz);
//     memcpy(c, d, sz);

//   lbl_110:
//     memcpy(&array[sz*i1], b, sz);
//     memcpy(&array[sz*i1c], c, sz);
//     if (ncount < m*n) goto lbl_40;

//   lbl_120:
//     /*    printf("%d\n", ncount);*/
//     return 0;

//   lbl_130:
//     n1 = n - 1;

//     for (i=1; i<=n1; i++) {
//       j1 = i + 1;
//       for (j=j1; j<=n; j++) {
// 	i1 = i + (j-1) * n;
// 	i2 = j + (i-1) * m;

// 	switch (type) {
// 	case BYTE:
// 	  *(DByte *) b = *(DByte *) &array[sz * (i1-1)];
// 	  *(DByte *) &array[sz * (i1-1)] = *(DByte *) &array[sz * (i2-1)];
// 	  *(DByte *) &array[sz * (i2-1)] = *(DByte *) b;
// 	  break;

// 	case INT:
// 	  *(DInt *) b = *(DInt *) &array[sz * (i1-1)];
// 	  *(DInt *) &array[sz * (i1-1)] = *(DInt *) &array[sz * (i2-1)];
// 	  *(DInt *) &array[sz * (i2-1)] = *(DInt *) b;
// 	  break;

// 	case UINT:
// 	  *(DUInt *) b = *(DUInt *) &array[sz * (i1-1)];
// 	  *(DUInt *) &array[sz * (i1-1)] = *(DUInt *) &array[sz * (i2-1)];
// 	  *(DUInt *) &array[sz * (i2-1)] = *(DUInt *) b;
// 	  break;

// 	case LONG:
// 	  *(DLong *) b = *(DLong *) &array[sz * (i1-1)];
// 	  *(DLong *) &array[sz * (i1-1)] = *(DLong *) &array[sz * (i2-1)];
// 	  *(DLong *) &array[sz * (i2-1)] = *(DLong *) b;
// 	  break;

// 	case ULONG:
// 	  *(DULong *) b = *(DULong *) &array[sz * (i1-1)];
// 	  *(DULong *) &array[sz * (i1-1)] = *(DULong *) &array[sz * (i2-1)];
// 	  *(DULong *) &array[sz * (i2-1)] = *(DULong *) b;
// 	  break;

// 	case FLOAT:
// 	  *(DFloat *) b = *(DFloat *) &array[sz * (i1-1)];
// 	  *(DFloat *) &array[sz * (i1-1)] = *(DFloat *) &array[sz * (i2-1)];
// 	  *(DFloat *) &array[sz * (i2-1)] = *(DFloat *) b;
// 	  break;

// 	case DOUBLE:
// 	  *(DDouble *) b = *(DDouble *) &array[sz * (i1-1)];
// 	  *(DDouble *) &array[sz * (i1-1)] = *(DDouble *) &array[sz * (i2-1)];
// 	  *(DDouble *) &array[sz * (i2-1)] = *(DDouble *) b;
// 	  break;

// 	case COMPLEX:
// 	  *(DComplex *) b = *(DComplex *) &array[sz * (i1-1)];
// 	  *(DComplex *) &array[sz * (i1-1)] = *(DComplex *) &array[sz * (i2-1)];
// 	  *(DComplex *) &array[sz * (i2-1)] = *(DComplex *) b;
// 	  break;

// 	case COMPLEXDBL:
// 	  *(DComplexDbl *) b = *(DComplexDbl *) &array[sz * (i1-1)];
// 	  *(DComplexDbl *) &array[sz * (i1-1)] = *(DComplexDbl *) &array[sz * (i2-1)];
// 	  *(DComplexDbl *) &array[sz * (i2-1)] = *(DComplexDbl *) b;
// 	  break;
// 	}

//       }
//     }
//     goto lbl_120;

//   lbl_160:
//     return i;

//   lbl_180:
//     return -1;

//   lbl_190:
//     return -2;
//   }


//   int transpose(char *array, SizeT Rank, SizeT dims[], SizeT sz, 
// 		DType type)
//   {
//     int i,j;
//     int nEl, nEl_sub;
//     int offset;
//     int status;
//     char *temp;

//     nEl = dims[0];
//     nEl_sub = 1;
//     for (i=1; i<Rank; i++) {
//       nEl *= dims[i];
//       nEl_sub *= dims[i];
//     }

//     if (Rank == 1) {
//       return 0;
//     } else if (Rank == 2) {
//       status = trans513(array, dims[1], dims[0], sz, type);
//       return status;
//     } else if (Rank > 2) {
//       //      printf("recursive\n");
//       temp = (char *) calloc(nEl,sz);

//       for (j=0; j<dims[0]; j++) {
// 	offset = nEl_sub*j;
// 	for (i=0; i<nEl_sub; i++) 
// 	  memcpy(&temp[(offset+i)*sz], &array[(i*dims[0]+j)*sz],sz);
// 	status = transpose(&temp[offset*sz], Rank-1, &dims[1], sz, type);
//       }
//       memcpy(&array[0], temp, nEl*sz);

//       free(temp);
//       return status;
//     }

//   }


//   int transpose_perm(char *array, SizeT Rank, SizeT dims[], SizeT sz,
// 		     DType type, long perm[])
//   {
//     int i,j,pnt,n_trans,sub_size;
//     int status;
//     int cur_perm[MAXRANK];
//     SizeT tmp;

//     if (perm == NULL) {
//       status = transpose(array, Rank, dims, sz, type);
//     }
//     else {
//       for (i=0; i<Rank; i++) cur_perm[i] = i;

//       pnt = Rank - 1;

//       while (pnt > 0) {

// 	for (i=0; i<pnt; i++) if (perm[pnt] == cur_perm[i]) break;

// 	if (i != pnt) {

// 	  if (i != 0) {
// 	    n_trans = 1;
// 	    for (j=i+1; j<Rank; j++) n_trans *= dims[j];

// 	    sub_size = sz;
// 	    for (j=0; j<i+1; j++) sub_size *= dims[j];

// 	    for (j=0; j<n_trans; j++)
// 	      status = transpose(&array[sub_size*j], i+1, dims, sz, type);

// 	    for (j=0; j<=i/2; j++) {
// 	      tmp = cur_perm[j];
// 	      cur_perm[j] = cur_perm[i-j];
// 	      cur_perm[i-j] = tmp;

// 	      tmp = dims[j];
// 	      dims[j] = dims[i-j];
// 	      dims[i-j] = tmp;
// 	    }
// 	  }


// 	  n_trans = 1;
// 	  for (j=pnt+1; j<Rank; j++) n_trans *= dims[j];

// 	  sub_size = sz;
// 	  for (j=0; j<pnt+1; j++) sub_size *= dims[j];

// 	  for (j=0; j<n_trans; j++)
// 	    status = transpose(&array[sub_size*j], pnt+1, dims, sz, type);

// 	  for (j=0; j<=pnt/2; j++) {
// 	    tmp = cur_perm[j];
// 	    cur_perm[j] = cur_perm[pnt-j];
// 	    cur_perm[pnt-j] = tmp;

// 	    tmp = dims[j];
// 	    dims[j] = dims[pnt-j];
// 	    dims[pnt-j] = tmp;
// 	  }
// 	}

// 	pnt = --pnt;
//       }
//     }

//     return status;
//   }


  //**********************************************************************

  void machar_s ( long int *ibeta, long int *it, long int *irnd, long int *ngrd,
		  long int *machep, long int *negep, long int *iexp, long int *minexp,
		  long int *maxexp, float *eps, float *epsneg, float *xmin, float *xmax ) 

    //**********************************************************************
    //
    //  Purpose:
    //
    //    MACHAR_S computes machine constants for floating point arithmetic.
    //
    //  Discussion:
    //
    //    This routine determines the parameters of the floating-point 
    //    arithmetic system specified below.  The determination of the first 
    //    three uses an extension of an algorithm due to Malcolm, 
    //    incorporating some of the improvements suggested by Gentleman and 
    //    Marovich.  
    //
    //    A FORTRAN version of this routine appeared as ACM algorithm 665.
    //
    //    This routine is a C translation of the FORTRAN code, and appeared
    //    as part of ACM algorithm 722.
    //
    //    An earlier version of this program was published in Cody and Waite.
    //
    //  Reference:
    //
    //    W J Cody,
    //    ACM Algorithm 665, MACHAR, a subroutine to dynamically determine 
    //      machine parameters,
    //    ACM Transactions on Mathematical Software,
    //    Volume 14, Number 4, pages 303-311, 1988.
    //
    //    W J Cody and W Waite,
    //    Software Manual for the Elementary Functions,
    //    Prentice Hall, 1980.
    //
    //    M Gentleman and S Marovich,
    //    Communications of the ACM,
    //    Volume 17, pages 276-277, 1974.
    //
    //    M. Malcolm,
    //    Communications of the ACM,
    //    Volume 15, pages 949-951, 1972.
    //
    //  Author:
    //
    //    W. J. Cody
    //    Argonne National Laboratory
    //
    //  Parameters:
    //
    //    Output, long int* IBETA, the radix for the floating-point representation.
    //
    //    Output, long int* IT, the number of base IBETA digits in the floating-point
    //    significand.
    //
    //    Output, long int* IRND:
    //    0, if floating-point addition chops.
    ///   1, if floating-point addition rounds, but not in the IEEE style.
    //    2, if floating-point addition rounds in the IEEE style.
    //    3, if floating-point addition chops, and there is partial underflow.
    //    4, if floating-point addition rounds, but not in the IEEE style, and 
    //      there is partial underflow.
    //    5, if floating-point addition rounds in the IEEE style, and there is 
    //      partial underflow.
    //
    //    Output, long int* NGRD, the number of guard digits for multiplication with
    //    truncating arithmetic.  It is
    //    0, if floating-point arithmetic rounds, or if it truncates and only 
    //      IT base IBETA digits participate in the post-normalization shift of the
    //      floating-point significand in multiplication;
    //    1, if floating-point arithmetic truncates and more than IT base IBETA
    //      digits participate in the post-normalization shift of the floating-point
    //      significand in multiplication.
    //
    //    Output, long int* MACHEP, the largest negative integer such that
    //      1.0 + ( float ) IBETA ^ MACHEP != 1.0, 
    //    except that MACHEP is bounded below by - ( IT + 3 ).
    //
    //    Output, long int* NEGEPS, the largest negative integer such that
    //      1.0 - ( float ) IBETA ) ^ NEGEPS != 1.0, 
    //    except that NEGEPS is bounded below by - ( IT + 3 ).
    //
    //    Output, long int* IEXP, the number of bits (decimal places if IBETA = 10)
    //    reserved for the representation of the exponent (including the bias or
    //    sign) of a floating-point number.
    //
    //    Output, long int* MINEXP, the largest in magnitude negative integer such 
    //    that
    //      ( float ) IBETA ^ MINEXP 
    //    is positive and normalized.
    //
    //    Output, long int* MAXEXP, the smallest positive power of BETA that overflows.
    // 
    //    Output, float* EPS, the smallest positive floating-point number such
    //    that  
    //      1.0 + EPS != 1.0. 
    //    in particular, if either IBETA = 2  or IRND = 0, 
    //      EPS = ( float ) IBETA ^ MACHEP.
    //    Otherwise,  
    //      EPS = ( ( float ) IBETA ^ MACHEP ) / 2.
    //
    //    Output, float* EPSNEG, a small positive floating-point number such that
    //      1.0 - EPSNEG != 1.0. 
    //    In particular, if IBETA = 2 or IRND = 0, 
    //    EPSNEG = ( float ) IBETA ^ NEGEPS.
    //    Otherwise,  
    //      EPSNEG = ( float ) IBETA ^ NEGEPS ) / 2.  
    //    Because NEGEPS is bounded below by - ( IT + 3 ), EPSNEG might not be the
    //    smallest number that can alter 1.0 by subtraction.
    //
    //    Output, float* XMIN, the smallest non-vanishing normalized floating-point
    //    power of the radix:
    //      XMIN = ( float ) IBETA ^ MINEXP
    //
    //    Output, float* XMAX, the largest finite floating-point number.  In
    //    particular,
    //      XMAX = ( 1.0 - EPSNEG ) * ( float ) IBETA ^ MAXEXP
    //    On some machines, the computed value of XMAX will be only the second, 
    //    or perhaps third, largest number, being too small by 1 or 2 units in 
    //    the last digit of the significand.
    //
  {
    float a;
    float b;
    float beta;
    float betah;
    float betain;
    int i;
    int itmp;
    int iz;
    int j;
    int k;
    int mx;
    int nxres;
    float one;
    float t;
    float tmp;
    float tmp1;
    float tmpa;
    float two;
    float y;
    float z;
    float zero;

    (*irnd) = 1;
    one = (float) (*irnd);
    two = one + one;
    a = two;
    b = a;
    zero = 0.0e0;
    //
    //  Determine IBETA and BETA ala Malcolm.
    //
    tmp = ( ( a + one ) - a ) - one;

    while ( tmp == zero )
      {
	a = a + a;
	tmp = a + one;
	tmp1 = tmp - a;
	tmp = tmp1 - one;
      }
    
    tmp = a + b;
    itmp = ( int ) ( tmp - a );
    
    while ( itmp == 0 )
      {
	b = b + b;
	tmp = a + b;
	itmp = ( int ) ( tmp - a );
      }

    *ibeta = itmp;
    beta = ( float ) ( *ibeta );
    //
    //  Determine IRND, IT.
    //
    ( *it ) = 0;
    b = one;
    tmp = ( ( b + one ) - b ) - one;

    while ( tmp == zero )
      {
	*it = *it + 1;
	b = b * beta;
	tmp = b + one;
	tmp1 = tmp - b;
	tmp = tmp1 - one;
      }

    *irnd = 0;
    betah = beta / two;
    tmp = a + betah;
    tmp1 = tmp - a;
    
    if ( tmp1 != zero )
      {
	*irnd = 1;
      }

    tmpa = a + beta;
    tmp = tmpa + betah;

    if ( ( *irnd == 0 ) && ( tmp - tmpa != zero ) )
      {
	*irnd = 2;
      }
    //
    //  Determine NEGEP, EPSNEG.
    //
    (*negep) = (*it) + 3;
    betain = one / beta;
    a = one;
 
    for ( i = 1; i <= (*negep); i++ )
      {
	a = a * betain;
      }
    
    b = a;
    tmp = ( one - a );
    tmp = tmp - one;

    while ( tmp == zero )
      {
	a = a * beta;
	*negep = *negep - 1;
	tmp1 = one - a;
	tmp = tmp1 - one;
      }

    (*negep) = -(*negep);
    (*epsneg) = a;
    //
    //  Determine MACHEP, EPS.
    //
    
    (*machep) = -(*it) - 3;
    a = b;
    tmp = one + a;

    while ( tmp - one == zero)
      {
	a = a * beta;
	*machep = *machep + 1;
	tmp = one + a;
      }

    *eps = a;
    //
    //  Determine NGRD.
    //
    (*ngrd) = 0;
    tmp = one + *eps;
    tmp = tmp * one;
    
    if ( ( (*irnd) == 0 ) && ( tmp - one ) != zero )
      {
	(*ngrd) = 1;
      }
    //
    //  Determine IEXP, MINEXP and XMIN.
    //
    //  Loop to determine largest I such that (1/BETA) ** (2**(I))
    //  does not underflow.  Exit from loop is signaled by an underflow.
    //

    i = 0;
    k = 1;
    z = betain;
    t = one + *eps;
    nxres = 0;

    for ( ; ; )
      {
	y = z;
	z = y * y;
	//
	//  Check for underflow
	//

	a = z * one;
	tmp = z * t;

	if ( ( a + a == zero ) || ( abs( z ) > y ) )
	  {
	    break;
	  }

	tmp1 = tmp * betain;

	if ( tmp1 * beta == z )
	  {
	    break;
	  }

	i = i + 1;
	k = k + k;
      }
    //
    //  Determine K such that (1/BETA)**K does not underflow.
    //  First set  K = 2 ** I.
    //
    (*iexp) = i + 1;
    mx = k + k;

    if ( *ibeta == 10 )
      {
	//
	//  For decimal machines only
	//

	(*iexp) = 2;
	iz = *ibeta;
	while ( k >= iz )
	  {
	    iz = iz * ( *ibeta );
	    (*iexp) = (*iexp) + 1;
	  }
	mx = iz + iz - 1;
      }
 
    //
    //  Loop to determine MINEXP, XMIN.
    //  Exit from loop is signaled by an underflow.
    //
    for ( ; ; )
      {
	(*xmin) = y;
	y = y * betain;
	a = y * one;
	tmp = y * t;
	tmp1 = a + a;
	
	if ( ( tmp1 == zero ) || ( abs( y ) >= ( *xmin ) ) )
	  {
	    break;
	  }

	k = k + 1;
	tmp1 = tmp * betain;
	tmp1 = tmp1 * beta;

	if ( ( tmp1 == y ) && ( tmp != y ) )
	  {
	    nxres = 3;
	    *xmin = y;
	    break;
	  }

      }
    
    (*minexp) = -k;

    //
    //  Determine MAXEXP, XMAX.
    //
    if ( ( mx <= k + k - 3 ) && ( ( *ibeta ) != 10 ) )
      {
	mx = mx + mx;
	(*iexp) = (*iexp) + 1;
      }

    (*maxexp) = mx + (*minexp);
    //
    //  Adjust IRND to reflect partial underflow.
    //
    (*irnd) = (*irnd) + nxres;
    //
    //  Adjust for IEEE style machines.
    //
    if ( ( *irnd) >= 2 )
      {
	(*maxexp) = (*maxexp) - 2;
      }
    //
    //  Adjust for machines with implicit leading bit in binary
    //  significand and machines with radix point at extreme
    //  right of significand.
    //
    i = (*maxexp) + (*minexp);

    if ( ( ( *ibeta ) == 2 ) && ( i == 0 ) )
      {
	(*maxexp) = (*maxexp) - 1;
      }

    if ( i > 20 )
      {
	(*maxexp) = (*maxexp) - 1;
      }

    if ( a != y )
      {
	(*maxexp) = (*maxexp) - 2;
      }

    (*xmax) = one - (*epsneg);
    tmp = (*xmax) * one;
    
    if ( tmp != (*xmax) )
      {
	(*xmax) = one - beta * (*epsneg);
      }

    (*xmax) = (*xmax) / ( beta * beta * beta * (*xmin) );
    i = (*maxexp) + (*minexp) + 3;

    if ( i > 0 )
      {
 
	for ( j = 1; j <= i; j++ )
	  {
	    if ( (*ibeta) == 2 )
	      {
		(*xmax) = (*xmax) + (*xmax);
	      }
	    if ( (*ibeta) != 2 )
	      {
		(*xmax) = (*xmax) * beta;
	      }
	  }
	
      }
    
    return;

  }

  void machar_d ( long int *ibeta, long int *it, long int *irnd, long int *ngrd,
		  long int *machep, long int *negep, long int *iexp, long int *minexp,
		  long int *maxexp, double *eps, double *epsneg, double *xmin, double *xmax ) 
  {
    double a;
    double b;
    double beta;
    double betah;
    double betain;
    int i;
    int itmp;
    int iz;
    int j;
    int k;
    int mx;
    int nxres;
    double one;
    double t;
    double tmp;
    double tmp1;
    double tmpa;
    double two;
    double y;
    double z;
    double zero;

    (*irnd) = 1;
    one = (double) (*irnd);
    two = one + one;
    a = two;
    b = a;
    zero = 0.0e0;
    //
    //  Determine IBETA and BETA ala Malcolm.
    //
    tmp = ( ( a + one ) - a ) - one;

    while ( tmp == zero )
      {
	a = a + a;
	tmp = a + one;
	tmp1 = tmp - a;
	tmp = tmp1 - one;
      }
    
    tmp = a + b;
    itmp = ( int ) ( tmp - a );
    
    while ( itmp == 0 )
      {
	b = b + b;
	tmp = a + b;
	itmp = ( int ) ( tmp - a );
      }

    *ibeta = itmp;
    beta = ( double ) ( *ibeta );
    //
    //  Determine IRND, IT.
    //
    ( *it ) = 0;
    b = one;
    tmp = ( ( b + one ) - b ) - one;

    while ( tmp == zero )
      {
	*it = *it + 1;
	b = b * beta;
	tmp = b + one;
	tmp1 = tmp - b;
	tmp = tmp1 - one;
      }

    *irnd = 0;
    betah = beta / two;
    tmp = a + betah;
    tmp1 = tmp - a;
    
    if ( tmp1 != zero )
      {
	*irnd = 1;
      }

    tmpa = a + beta;
    tmp = tmpa + betah;

    if ( ( *irnd == 0 ) && ( tmp - tmpa != zero ) )
      {
	*irnd = 2;
      }
    //
    //  Determine NEGEP, EPSNEG.
    //
    (*negep) = (*it) + 3;
    betain = one / beta;
    a = one;
 
    for ( i = 1; i <= (*negep); i++ )
      {
	a = a * betain;
      }
    
    b = a;
    tmp = ( one - a );
    tmp = tmp - one;

    while ( tmp == zero )
      {
	a = a * beta;
	*negep = *negep - 1;
	tmp1 = one - a;
	tmp = tmp1 - one;
      }

    (*negep) = -(*negep);
    (*epsneg) = a;
    //
    //  Determine MACHEP, EPS.
    //
    
    (*machep) = -(*it) - 3;
    a = b;
    tmp = one + a;

    while ( tmp - one == zero)
      {
	a = a * beta;
	*machep = *machep + 1;
	tmp = one + a;
      }

    *eps = a;
    //
    //  Determine NGRD.
    //
    (*ngrd) = 0;
    tmp = one + *eps;
    tmp = tmp * one;
    
    if ( ( (*irnd) == 0 ) && ( tmp - one ) != zero )
      {
	(*ngrd) = 1;
      }
    //
    //  Determine IEXP, MINEXP and XMIN.
    //
    //  Loop to determine largest I such that (1/BETA) ** (2**(I))
    //  does not underflow.  Exit from loop is signaled by an underflow.
    //

    i = 0;
    k = 1;
    z = betain;
    t = one + *eps;
    nxres = 0;

    for ( ; ; )
      {
	y = z;
	z = y * y;
	//
	//  Check for underflow
	//

	a = z * one;
	tmp = z * t;

	if ( ( a + a == zero ) || ( abs( z ) > y ) )
	  {
	    break;
	  }

	tmp1 = tmp * betain;

	if ( tmp1 * beta == z )
	  {
	    break;
	  }

	i = i + 1;
	k = k + k;
      }
    //
    //  Determine K such that (1/BETA)**K does not underflow.
    //  First set  K = 2 ** I.
    //
    (*iexp) = i + 1;
    mx = k + k;

    if ( *ibeta == 10 )
      {
	//
	//  For decimal machines only
	//

	(*iexp) = 2;
	iz = *ibeta;
	while ( k >= iz )
	  {
	    iz = iz * ( *ibeta );
	    (*iexp) = (*iexp) + 1;
	  }
	mx = iz + iz - 1;
      }
 
    //
    //  Loop to determine MINEXP, XMIN.
    //  Exit from loop is signaled by an underflow.
    //
    for ( ; ; )
      {
	(*xmin) = y;
	y = y * betain;
	a = y * one;
	tmp = y * t;
	tmp1 = a + a;
	
	if ( ( tmp1 == zero ) || ( abs( y ) >= ( *xmin ) ) )
	  {
	    break;
	  }

	k = k + 1;
	tmp1 = tmp * betain;
	tmp1 = tmp1 * beta;

	if ( ( tmp1 == y ) && ( tmp != y ) )
	  {
	    nxres = 3;
	    *xmin = y;
	    break;
	  }

      }
    
    (*minexp) = -k;

    //
    //  Determine MAXEXP, XMAX.
    //
    if ( ( mx <= k + k - 3 ) && ( ( *ibeta ) != 10 ) )
      {
	mx = mx + mx;
	(*iexp) = (*iexp) + 1;
      }

    (*maxexp) = mx + (*minexp);
    //
    //  Adjust IRND to reflect partial underflow.
    //
    (*irnd) = (*irnd) + nxres;
    //
    //  Adjust for IEEE style machines.
    //
    if ( ( *irnd) >= 2 )
      {
	(*maxexp) = (*maxexp) - 2;
      }
    //
    //  Adjust for machines with implicit leading bit in binary
    //  significand and machines with radix point at extreme
    //  right of significand.
    //
    i = (*maxexp) + (*minexp);

    if ( ( ( *ibeta ) == 2 ) && ( i == 0 ) )
      {
	(*maxexp) = (*maxexp) - 1;
      }

    if ( i > 20 )
      {
	(*maxexp) = (*maxexp) - 1;
      }

    if ( a != y )
      {
	(*maxexp) = (*maxexp) - 2;
      }

    (*xmax) = one - (*epsneg);
    tmp = (*xmax) * one;
    
    if ( tmp != (*xmax) )
      {
	(*xmax) = one - beta * (*epsneg);
      }

    (*xmax) = (*xmax) / ( beta * beta * beta * (*xmin) );
    i = (*maxexp) + (*minexp) + 3;

    if ( i > 0 )
      {
 
	for ( j = 1; j <= i; j++ )
	  {
	    if ( (*ibeta) == 2 )
	      {
		(*xmax) = (*xmax) + (*xmax);
	      }
	    if ( (*ibeta) != 2 )
	      {
		(*xmax) = (*xmax) * beta;
	      }
	  }
	
      }
    
    return;

  }

#ifdef USE_LIBPROJ4
  PJ *map_init()
  {
    // Checks for changes to projection parameters and calls
    // pj_init if they have changed or if first time.

    static DStructGDL* mapStruct = SysVar::Map();
    static unsigned projectionTag = mapStruct->Desc()->TagIndex( "PROJECTION");
    static unsigned p0lonTag = mapStruct->Desc()->TagIndex( "P0LON");
    static unsigned p0latTag = mapStruct->Desc()->TagIndex( "P0LAT");
    static unsigned aTag = mapStruct->Desc()->TagIndex( "A");
    static unsigned e2Tag = mapStruct->Desc()->TagIndex( "E2");
    static unsigned pTag = mapStruct->Desc()->TagIndex( "P");

    DLong map_projection = 
      (*static_cast<DLongGDL*>( mapStruct->GetTag( projectionTag, 0)))[0];
    DDouble map_p0lon = 
      (*static_cast<DDoubleGDL*>( mapStruct->GetTag( p0lonTag, 0)))[0];
    DDouble map_p0lat = 
      (*static_cast<DDoubleGDL*>( mapStruct->GetTag( p0latTag, 0)))[0];
    DDouble map_a = 
      (*static_cast<DDoubleGDL*>( mapStruct->GetTag( aTag, 0)))[0];
    DDouble map_e2 = 
      (*static_cast<DDoubleGDL*>( mapStruct->GetTag( e2Tag, 0)))[0];
    DDouble map_lat1 = 
      (*static_cast<DDoubleGDL*>( mapStruct->GetTag( pTag, 0)))[3];
    DDouble map_lat2 = 
      (*static_cast<DDoubleGDL*>( mapStruct->GetTag( pTag, 0)))[4];

    char proj[64];
    char p0lon[64];
    char p0lat[64];
    char a[64];
    char e2[64];
    char lat_1[64];
    char lat_2[64];
    char lat_ts[64];

    // Oblique projection parameters
    char ob_proj[64];
    char ob_lon[64];
    char ob_lat[64];

    static char *parms[32];
    static DLong last_proj = 0;
    static DDouble last_p0lon = -9999;
    static DDouble last_p0lat = -9999;
    static DDouble last_a = -9999;
    static DDouble last_e2 = -9999;
    static DDouble last_lat1 = -9999;
    static DDouble last_lat2 = -9999;

    if (map_projection != last_proj ||
	map_p0lon != last_p0lon ||
	map_p0lat != last_p0lat ||
	map_a != last_a ||
	map_e2 != last_e2 || 
	map_lat1 != last_lat1 ||
	map_lat2 != last_lat2) {

      if (map_p0lon >= 0) {
	sprintf(p0lon, "lon_0=%lf", map_p0lon);
	strcat(p0lon, "E");
      } else {
	sprintf(p0lon, "lon_0=%lf", fabs(map_p0lon));
	strcat(p0lon, "W");
      }
      
      if (map_p0lat >= 0) {
	sprintf(p0lat, "lat_0=%lf", map_p0lat);
	strcat(p0lat, "N");
      } else {
	sprintf(p0lat, "lat_0=%lf", fabs(map_p0lat));
	strcat(p0lat, "S");
      }

      if (map_e2 == 0.0) {
	sprintf(a, "R=%lf", map_a);
      } else {
	sprintf(a, "a=%lf", map_a);
	sprintf(e2, "es=%lf", map_e2);
      }


      //	strcpy(parms[1], "ellps=clrk66");

      DLong nparms = 0;
      parms[nparms++] = &a[0];
      if (map_e2 != 0.0) parms[nparms++] = &e2[0];

      // stereographic iproj =  1
      // orthographic  iproj =  2
      // conic         iproj =  3
      // lambert       iproj =  4
      // gnomic        iproj =  5
      // azimuth       iproj =  6
      // satellite     iproj =  7
      // mercator      iproj =  9
      // mollweide     iproj = 10
      // sinusoidal    iproj = 11
      // aitoff        iproj = 12
      // hammer        iproj = 13
      // albers        iproj = 14
      // utm           iproj = 15
      // miller        iproj = 16
      // robinson      iproj = 17
      // goodes        iproj = 19

      // Stereographic Projection
      if (map_projection == 1) {
	strcpy(proj, "proj=stere");
	parms[nparms++] = &proj[0];
	parms[nparms++] = &p0lon[0];
	parms[nparms++] = &p0lat[0];
      }

      // Orthographic Projection
      if (map_projection == 2) {
	strcpy(proj, "proj=ortho");
	parms[nparms++] = &proj[0];
	parms[nparms++] = &p0lon[0];
	parms[nparms++] = &p0lat[0];
      }

      // Lambert Conformal Conic
      if (map_projection == 3) {
	strcpy(proj, "proj=lcc");
	parms[nparms++] = &proj[0];
	parms[nparms++] = &p0lon[0];
	parms[nparms++] = &p0lat[0];
	sprintf(lat_1, "lat_1=%lf", map_lat1 * RAD_TO_DEG);
	sprintf(lat_2, "lat_2=%lf", map_lat2 * RAD_TO_DEG);
	parms[nparms++] = &lat_1[0];
	parms[nparms++] = &lat_2[0];
      }

      // Lambert Equal Area Conic
      if (map_projection == 4) {
	strcpy(proj, "proj=leac");
	parms[nparms++] = &proj[0];
	parms[nparms++] = &p0lon[0];
	parms[nparms++] = &p0lat[0];
      }

      // Gnomonic
      if (map_projection == 5) {
	strcpy(proj, "proj=gnom");
	parms[nparms++] = &proj[0];
	parms[nparms++] = &p0lon[0];
	parms[nparms++] = &p0lat[0];
      }

      // Azimuthal Equidistant
      if (map_projection == 6) {
	strcpy(proj, "proj=aeqd");
	parms[nparms++] = &proj[0];
	parms[nparms++] = &p0lon[0];
	parms[nparms++] = &p0lat[0];
      }

      // Cylindrical Equidistant
      if (map_projection == 8) {
	if (map_p0lat == 0) {
	  strcpy(proj, "proj=eqc");
	  parms[nparms++] = &proj[0];
	  parms[nparms++] = &p0lon[0];
	} else {
	  strcpy(ob_proj, "proj=ob_tran");
	  parms[nparms++] = &ob_proj[0];
	  strcpy(proj, "o_proj=eqc");
	  parms[nparms++] = &proj[0];
	  parms[nparms++] = &p0lon[0];

	  /*
	  if (map_p0lon >= 0) {
	    sprintf(ob_lon, "o_lon_0=%lf", map_p0lon);
	    strcat(ob_lon, "E");
	  } else {
	    sprintf(ob_lon, "o_lon_0=%lf", fabs(map_p0lon));
	    strcat(ob_lon, "W");
	  }
	  parms[nparms++] = &ob_lon[0];
	  */

	  sprintf(ob_lat, "o_lat_p=%lf", 90-map_p0lat);
	  parms[nparms++] = &ob_lat[0];
	}
      }

      // Mercator
      if (map_projection == 9) {
	strcpy(proj, "proj=merc");
	sprintf(lat_ts, "lat_ts=%lf", 0);
	parms[nparms++] = &proj[0];
	//	parms[nparms++] = &lat_ts[0];
      }

      // Aitoff
      if (map_projection == 12) {
	strcpy(proj, "proj=aitoff");
	parms[nparms++] = &proj[0];
	parms[nparms++] = &p0lon[0];
	parms[nparms++] = &p0lat[0];
      }

      last_proj = map_projection;
      last_p0lon = map_p0lon;
      last_p0lat = map_p0lat;
      last_a = map_a;
      last_e2 = map_e2;
      last_lat1 = map_lat1;
      last_lat2 = map_lat2;

      prev_ref = pj_init(nparms, parms);
    }
    return prev_ref;
  }
#endif

} // namespace


