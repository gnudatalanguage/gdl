/***************************************************************************
            eigenvalues_solvers.hpp  -  interface to ELMHES/TRIRED/HQR
                             -------------------
    begin                : 2024
    copyright            : (C) 2024 by G. Duvert
    email                : gilles.duvert@free.fr
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

  // AC 2024/04/22 all tests to Eigen3/GSL now in "eigenvalues_solvers.cpp" 
  
  BaseGDL* la_elmhes_fun(EnvT* e);
  void la_trired_pro(EnvT* e);
  BaseGDL* elmhes_fun(EnvT* e);
  BaseGDL* hqr_fun(EnvT* e);
  void trired_pro(EnvT* e);
  void triql_pro(EnvT* e);
  } // namespace

#endif
