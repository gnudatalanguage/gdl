/***************************************************************************
                          math_fun_jmg.hpp  -  mathematical GDL library function
                             -------------------
    begin                : 2004
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

#ifndef MATH_FUN_HPP_
#define MATH_FUN_HPP_

#include "envt.hpp"

typedef struct _2D_POLY_ {
        DLong  nc;            /* number of coefficients in px, py, c */
        DLong* px;            /* powers of x                         */
        DLong* py;            /* powers of y                         */
        DDouble* c;           /* polynomial coefficients             */
} poly2d ;

typedef double	pixelvalue ;

typedef struct _image_
{
	/* Size of the image in x and y */
    int             	lx, ly ;
	/* Pointer to pixel buffer as a 1d buffer */
    pixelvalue      *	data ;
} image_t ;

#define TABSPERPIX      (1000)
#define KERNEL_WIDTH    (2.0)
#define KERNEL_SAMPLES  (1+(int)(TABSPERPIX * KERNEL_WIDTH))
#define PI_NUMB     (3.1415926535897932384626433832795)
#define MAX_COLUMN_NUMBER               (40000)
#define MAX_LINE_NUMBER                 (40000)

#define LINEAR  1
#define GENERIC 2
 
namespace lib {

//   BaseGDL* transpose_fun( EnvT* e);
  BaseGDL* machar_fun( EnvT* e);
  BaseGDL* finite_fun( EnvT* e);
  BaseGDL* check_math_fun( EnvT* e);
  BaseGDL* radon_fun( EnvT* e);
  BaseGDL* poly_2d_fun( EnvT* e);
  BaseGDL* poly_2d_funnew( EnvT* e);
  BaseGDL* rk4jmg_fun( EnvT* e);

  double ipow(double x, int p);
//  double sinc(double x);
  double poly2d_compute(poly2d *p, double x, double y);
  DDouble * generate_interpolation_kernel(int kernel_type, DDouble cubic);

} // namespace

#endif


