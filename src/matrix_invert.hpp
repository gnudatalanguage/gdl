/***************************************************************************
                          matrix_invert.hpp 
                          -------------------
    begin                : May 17 2013
    copyright            : (C) 2004 by Joel Gales, 2013: Nodar K. and Alain C.
    email                : jomoga@users.sourceforge.net
 ***************************************************************************/

/* history:

At the beginning, this code was only available using the GSL library.
At that time, it was included in the "gsl_fun.hpp/cpp" code

In 2013 we start to use Eigen3 in conjonction with the GSL.
The GSL is mandatory. Eigen3 is not mandatory.

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

// current versions are based on Eigen3
// #if defined(HAVE_LIBGSL) && defined(HAVE_LIBGSLCBLAS)

#include "datatypes.hpp"
#include "envt.hpp"

namespace lib {

  BaseGDL* invert_eigen_fun( EnvT* e, bool hasDouble);
  BaseGDL* invert_gsl_fun( EnvT* e, bool hasDouble);

  BaseGDL* AC_invert_fun( EnvT* e);
 
}


