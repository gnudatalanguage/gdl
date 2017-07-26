/***************************************************************************
                          linearprogramming.hpp  -  GDL header
                             -------------------
    begin                : Jul 12 2017
    copyright            : (C) 2017 by Gilles Duvert as interface to GLPK lib.

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


#ifndef GDLGLPK_HPP_
#define GDLGLPK_HPP_

#if defined(HAVE_GLPK)
#include "datatypes.hpp"
#include "envt.hpp"
namespace lib {
  BaseGDL* simplex(EnvT* e);
}
#endif


#endif
