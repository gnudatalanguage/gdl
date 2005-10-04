/***************************************************************************
                          fftw.hpp  -  FFTW GDL library function
                             -------------------
    begin                : Sep 21 2005
    copyright            : (C) 2005 by Joel Gales
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


#ifndef FFTW_HPP_
#define FFTW_HPP_

#include "datatypes.hpp"
#include "envt.hpp"

namespace lib {

  BaseGDL* fftw_fun( EnvT* e);

} // namespace


#endif
