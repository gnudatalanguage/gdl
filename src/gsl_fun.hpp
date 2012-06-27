/***************************************************************************
                          gsl_fun.hpp  -  GSL GDL library function
                             -------------------
    begin                : Jan 20 2004
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if defined(HAVE_LIBGSL) && defined(HAVE_LIBGSLCBLAS)

#ifndef GSL_FUN_HPP_
#define GSL_FUN_HPP_

#include "datatypes.hpp"
#include "envt.hpp"

namespace lib {

  BaseGDL* invert_fun( EnvT* e);
  BaseGDL* fft_fun( EnvT* e);
  BaseGDL* random_fun( EnvT* e);
  BaseGDL* histogram_fun( EnvT* e);
  BaseGDL* interpolate_fun( EnvT* e);

  void la_trired_pro( EnvT* e);

//  int fft_1d( BaseGDL*, void*, SizeT, SizeT, //SizeT, 
//	      double, SizeT);
  int fft_1d( BaseGDL* p0, void* data, SizeT nEl, SizeT offset, SizeT stride, 
	      double direct, SizeT dbl, DLong dimension);

  //  int unpack_real_radix2(double*, SizeT);
  int unpack_real_mxradix(double*, SizeT);
  int unpack_real_mxradix_float(float*, SizeT);

  void inplacemxradixfft(double a[], double b[], 
			 int ntot, int n, int nspan, int isn);

  // the following by AC
  BaseGDL* qromb_fun(EnvT* e);
  BaseGDL* qromo_fun(EnvT* e);
  BaseGDL* fz_roots_fun(EnvT* e);
  BaseGDL* fx_root_fun(EnvT* e);

  // the following by SA
  BaseGDL* newton_broyden(EnvT* e);
  BaseGDL* constant(EnvT* e);
  BaseGDL* binomialcoef(EnvT* e);
  BaseGDL* wtn(EnvT* e);
  BaseGDL* zeropoly(EnvT* e);
  BaseGDL* spher_harm(EnvT* e);

} // namespace

#endif

#endif
