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
  BaseGDL* randomu_fun( EnvT* e);
  BaseGDL* histogram_fun( EnvT* e);

  int unpack_real_radix2(double*, SizeT);
  int unpack_real_mxradix(double*, SizeT);
  int unpack_real_mxradix_float(float*, SizeT);

} // namespace

#endif

#endif
