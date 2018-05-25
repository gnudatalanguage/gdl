/***************************************************************************
                          convol.hpp  -  Convolution GDL library function
                             -------------------
    begin                : Jun 21 2013
    copyright            : (C) 2013, G. Duvert 2017
    email                : 
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

 #ifdef HAVE_CONFIG_H
 #include <config.h>
 #endif


#ifndef CONVOL_HPP_
#define CONVOL_HPP_

#include "datatypes.hpp"
#include "envt.hpp"

namespace lib {

  BaseGDL* convol_fun( EnvT* e);

} // namespace


#endif
