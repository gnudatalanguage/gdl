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

  void svdc( EnvT* e);

  BaseGDL* sin_fun( BaseGDL* p0, bool isReference);//( EnvT* e);
  BaseGDL* cos_fun( BaseGDL* p0, bool isReference);//( EnvT* e);
  BaseGDL* tan_fun( BaseGDL* p0, bool isReference);//( EnvT* e);

  BaseGDL* sinh_fun( BaseGDL* p0, bool isReference);//( EnvT* e);
  BaseGDL* cosh_fun( BaseGDL* p0, bool isReference);//( EnvT* e);
  BaseGDL* tanh_fun( BaseGDL* p0, bool isReference);//( EnvT* e);

  BaseGDL* asin_fun( BaseGDL* p0, bool isReference);//( EnvT* e);
  BaseGDL* acos_fun( BaseGDL* p0, bool isReference);//( EnvT* e);
  BaseGDL* atan_fun( EnvT* e);

  BaseGDL* alog_fun( BaseGDL* p0, bool isReference);//( EnvT* e);
  BaseGDL* alog10_fun( BaseGDL* p0, bool isReference);//( EnvT* e);

  BaseGDL* sqrt_fun( BaseGDL* p0, bool isReference);//( EnvT* e);
  BaseGDL* abs_fun( BaseGDL* p0, bool isReference);//( EnvT* e);

  BaseGDL* round_fun( EnvT* e);
  BaseGDL* floor_fun( EnvT* e);
  BaseGDL* ceil_fun( EnvT* e);

  BaseGDL* conj_fun( BaseGDL* p0, bool isReference);//( EnvT* e);
  BaseGDL* imaginary_fun( BaseGDL* p0, bool isReference);//( EnvT* e);

  BaseGDL* exp_fun( BaseGDL* p0, bool isReference);//( EnvT* e);

  // by medericboquien@users.sourceforge.net
  BaseGDL* gauss_pdf(EnvT* e);
  BaseGDL* gauss_cvf(EnvT* e);
  BaseGDL* t_pdf(EnvT* e);
  BaseGDL* laguerre(EnvT* e);

  // by Sylwester Arabas
  BaseGDL* ll_arc_distance(EnvT* e);
  BaseGDL* crossp(EnvT* e);
  BaseGDL* gdl_erfinv_fun(EnvT* e);

} // namespace

#endif
