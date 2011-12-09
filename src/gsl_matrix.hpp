/***************************************************************************
                          gsl_matrix.hpp  -  GSL GDL library function
                             -------------------
    begin                : Dec 9 2011
    copyright            : (C) 2011 by Alain Coulais
    email                : alaingdl@users.sourceforge.net
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

  void ludc_pro( EnvT* e);
  BaseGDL* lusol_fun( EnvT* e);
  BaseGDL* determ_fun( EnvT* e);

} // namespace

#endif

#endif
