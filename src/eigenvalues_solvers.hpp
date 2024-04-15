/***************************************************************************
                          |FILENAME|  -  description
                             -------------------
    begin                : |DATE|
    copyright            : (C) |YEAR| by |AUTHOR|
    email                : |EMAIL|
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef EIGENVALUES_SOLVERS_HPP_
#define EIGENVALUES_SOLVERS_HPP_

#include "envt.hpp"
namespace lib {
#if defined(USE_EIGEN)
  BaseGDL* la_elmhes_fun(EnvT* e);
  void la_trired_pro(EnvT* e);
#else
  BaseGDL* elmhes_fun(EnvT* e);
#endif
  BaseGDL* elmhes_fun(EnvT* e);
  BaseGDL* hqr_fun(EnvT* e);
  void trired_pro(EnvT* e);
  void triql_pro(EnvT* e);
  } // namespace

#endif
