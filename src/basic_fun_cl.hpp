/***************************************************************************
                          basic_fun.hpp  -  basic GDL library functions
                             -------------------
    begin                : March 14 2004
    copyright            : (C) 2004 by Christopher Lee
    email                : leec_gdl@publius.co.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BASIC_FUN_HPP_CL
#define BASIC_FUN_HPP_CL

namespace lib {

  BaseGDL* timestamp(EnvT* e);
  void timestamptovalues(EnvT* e);
  BaseGDL* systime(EnvT* e);
  BaseGDL* legendre(EnvT* e);
  BaseGDL* gsl_exp(EnvT* e);

} // namespace

#endif
