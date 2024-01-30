/***************************************************************************
                          least_squares.hpp - GDL library function
                             -------------------
    begin                : July 2015
    copyright            : (C) 2015 by NATCHKEBIA Ilia
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

#ifndef LEAST_SQUARES_HPP_
#define LEAST_SQUARES_HPP_

//#ifdef HAVE_CONFIG_H
//#include <config.h>
//#endif

// this code needs Eigen3

#include "datatypes.hpp"
#include "envt.hpp"

namespace lib {

  //#if defined(USE_EIGEN)
  
  BaseGDL* la_least_squares_fun( EnvT* e);

  /*
#else

  BaseGDL* la_least_squares_fun( EnvT* e) {
    Message("GDL compiled without Eigen3 :  LA_LEAST_SQUARES not available");
    return new DIntGDL(0);
  }
  
#endif 
  */

} // namespace

#endif 

