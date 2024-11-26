/***************************************************************************
                          poly_2d.cpp  -  2D polynomial interpolation
                             -------------------
    begin                : March 03 2004
    copyright            : (C) 2002 by Joel Gales 2024 by Gilles Duvert
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

#ifndef POLY2D_FUN_HPP_
#define PLOY2D_FUN_HPP_

#include "envt.hpp"

typedef struct _2D_POLY_ {
        DLong  nc;            /* number of coefficients in px, py, c */
        DLong* px;            /* powers of x                         */
        DLong* py;            /* powers of y                         */
        DFloat* c;           /* polynomial coefficients             */
} poly2d ;

namespace lib {
  BaseGDL* poly_2d_fun( EnvT* e);
} // namespace

#endif
