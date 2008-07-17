/***************************************************************************
             math_fun_ng.hpp  -  mathematical GDL library function
                             -------------------
    begin                : 26 May 2008
    copyright            : (C) 2007 by Nicolas Galmiche
    email                : n.galmiche AT gmail.com

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "datatypes.hpp"
#include "dinterpreter.hpp"

namespace lib {

  BaseGDL* rk4_fun( EnvT* e);
  BaseGDL* voigt_fun( EnvT* e);
  float humlik( float A, float U);

} // namespace
