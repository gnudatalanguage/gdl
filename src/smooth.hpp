/***************************************************************************
                          smooth.hpp  -  mathematical GDL library function
                             -------------------
    begin                : 05 September 2014
    copyright            : (C) 2014 by Levan Loria  (with Alain Coulais)
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
#ifdef HAVE_CONFIG_H
 #include <config.h>
 #endif


#ifndef SMOOTH_HPP_
#define SMOOTH_HPP_

#include "datatypes.hpp"
#include "envt.hpp"

namespace lib {

 BaseGDL* smooth_fun( EnvT* e);

} // namespace

#endif
