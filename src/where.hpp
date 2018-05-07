 /***************************************************************************
                          where.hpp  -  WHERE() GDL library function
                             -------------------
    begin                : Jun 21 2013
    copyright            : (C) 2013, 2017 G. Duvert
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


#ifndef WHERE_HPP_
#define WHERE_HPP_

#include "datatypes.hpp"
#include "envt.hpp"

namespace lib {

  BaseGDL* where_fun( EnvT* e);

} // namespace


#endif