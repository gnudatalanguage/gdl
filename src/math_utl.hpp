/***************************************************************************
                        math_utl.hpp  -  math utilities GDL library function
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

#ifndef MATH_UTL_HPP_
#define MATH_UTL_HPP_

#include "basegdl.hpp"

//#define ABS(xxx) (( xxx > -xxx)?(xxx):(-xxx))

template< typename T>
inline T abs( T a) { return (a>=T(0))?a:-a;}

namespace lib {

//   int trans513(char *, int, int, int, DType);
//   int transpose(char *, SizeT, SizeT [], SizeT, DType);
//   int transpose_perm(char *, SizeT, SizeT [], SizeT, DType, long []);

  void machar_s ( long int *ibeta, long int *it, long int *irnd, long int *ngrd,
		  long int *machep, long int *negep, long int *iexp, long int *minexp,
		  long int *maxexp, float *eps, float *epsneg, float *xmin, float *xmax );
  void machar_d ( long int *ibeta, long int *it, long int *irnd, long int *ngrd,
		  long int *machep, long int *negep, long int *iexp, long int *minexp,
		  long int *maxexp, double *eps, double *epsneg, double *xmin, double *xmax );

} // namespace

#endif
