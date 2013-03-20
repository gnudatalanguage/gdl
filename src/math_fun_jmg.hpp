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

#include "datatypes.hpp"
#include "envt.hpp"
#include "math_utl.hpp"

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
#ifdef PL_HAVE_QHULL
  void     triangulate( EnvT* e);
  void qhull ( EnvT* e);
  void grid_input (EnvT* e);
  BaseGDL* qgrid3_fun ( EnvT* e);
  BaseGDL* sph_scat_fun ( EnvT* e);
#endif
  BaseGDL* trigrid_fun( EnvT* e);
  BaseGDL* poly_2d_fun( EnvT* e);
  BaseGDL* rk4jmg_fun( EnvT* e);

  double ipow(double x, int p);
  double sinc(double x);
  double poly2d_compute(poly2d *p, double x, double y);
  double * generate_interpolation_kernel(char * kernel_type, DDouble cubic);

  image_t * image_warp(SizeT, SizeT, SizeT, SizeT, DType, void*, 
		       char *kernel_type,
		       DDouble *param, poly2d *poly_u, poly2d *poly_v, 
		       DLong interp, DDouble cubic, DLong warpType, DDouble initvalue);

  image_t * image_new(int size_x, int size_y, DDouble initvalue);
  void image_del(image_t *d);

} // namespace

#endif


