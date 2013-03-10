/***************************************************************************
                          math_fun_ac.hpp  -  mathematical GDL library function
                             -------------------
    begin                : 20 April 2007
    copyright            : (C) 2007 by Alain Coulais
    email                : alaingdl@users.sourceforge.net
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
#include "envt.hpp"

#if defined(USE_EIGEN)
#include <Eigen/Dense>
//using namespace Eigen; // never in header files!
#endif

namespace lib {

  BaseGDL* beseli_fun( EnvT* e);
  BaseGDL* beselj_fun( EnvT* e);
  BaseGDL* beselk_fun( EnvT* e);
  BaseGDL* besely_fun( EnvT* e);

  BaseGDL* spl_init_fun( EnvT* e);
  BaseGDL* spl_interp_fun( EnvT* e);

  BaseGDL* sobel_fun( EnvT* e);
  BaseGDL* roberts_fun( EnvT* e);
  BaseGDL* prewitt_fun( EnvT* e);
  BaseGDL* matmul_fun( EnvT* e);
  BaseGDL* matmulold_fun( EnvT* e);

} // namespace

