/***************************************************************************
                          math_fun.hpp  -  mathematical GDL library function
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
    email                : m_schellens@users.sf.net
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

namespace lib {

  BaseGDL* sin_fun( EnvT* e);
  BaseGDL* cos_fun( EnvT* e);
  BaseGDL* tan_fun( EnvT* e);

  BaseGDL* sinh_fun( EnvT* e);
  BaseGDL* cosh_fun( EnvT* e);
  BaseGDL* tanh_fun( EnvT* e);

  BaseGDL* asin_fun( EnvT* e);
  BaseGDL* acos_fun( EnvT* e);
  BaseGDL* atan_fun( EnvT* e);

  BaseGDL* alog_fun( EnvT* e);
  BaseGDL* alog10_fun( EnvT* e);

  BaseGDL* sqrt_fun( EnvT* e);
  BaseGDL* abs_fun( EnvT* e);

  BaseGDL* round_fun( EnvT* e);
  BaseGDL* floor_fun( EnvT* e);
  BaseGDL* ceil_fun( EnvT* e);

  BaseGDL* conj_fun( EnvT* e);
  BaseGDL* imaginary_fun( EnvT* e);

  BaseGDL* exp_fun( EnvT* e);

} // namespace

#endif
