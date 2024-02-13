/***************************************************************************
        sparse_matrix.hpp  -  GDL sparse matrix functions
							 -------------------
	begin                : Dec 9 2023
	copyright            : (C) 2023 by Gilles Duvert
	email                : surname dot name at free dot fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef SPARSE_MATRIX_HPP_
#define SPARSE_MATRIX_HPP_

#include "includefirst.hpp"
#include "datatypes.hpp"
#include "envt.hpp"

namespace lib {

  BaseGDL* sprsin_fun(EnvT* e);
  BaseGDL* sprsax_fun(EnvT* e);
  BaseGDL* sprsab_fun(EnvT* e);
  BaseGDL* sprstp_fun(EnvT* e);
  BaseGDL* fulstr_fun(EnvT* e);
  BaseGDL* linbcg_fun(EnvT* e);
  
}

#endif
