/***************************************************************************
                          basic_fun.hpp  -  basic GDL library functions
                             -------------------
    begin                : 2004
    copyright            : (C) 2004 by Joel Gales
    email                : jomoga@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BASIC_PRO_JMG_HPP_
#define BASIC_PRO_JMG_HPP_

#include <dlfcn.h>
#include <time.h>

namespace lib {

  void point_lun( EnvT* e);
  void linkimage( EnvT* e);
  void wait( EnvT* e);

} // namespace

#endif
