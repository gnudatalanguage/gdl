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

namespace lib {

  BaseGDL* machar_fun( EnvT* e);
  BaseGDL* finite_fun( EnvT* e);
  BaseGDL* check_math_fun( EnvT* e);
  BaseGDL* radon_fun( EnvT* e);
  BaseGDL* rk4jmg_fun( EnvT* e);

} // namespace

#endif


