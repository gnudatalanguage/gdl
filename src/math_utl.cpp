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


  //**********************************************************************
// special pragma to prevent optimization by the zealous compiler icc. (when optimized, loops forever on the 'while')
#pragma GCC push_options
#pragma GCC optimize ("O0")
#pragma optimize("", off)
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
#pragma GCC pop_options  
// special pragma to prevent optimization by the zealous compiler icc. (when optimized, loops forever on the 'while')
#pragma GCC push_options
#pragma GCC optimize ("O0")
#pragma optimize("", off) 
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
#pragma GCC pop_options  
} // namespace


