/***************************************************************************
                          gshhs.hpp  -  GSHHS-related routines
                             -------------------
    begin                : Apr 18 2010
    copyright            : (C) 2010 by Sylwester Arabas
    email                : slayoo@users.sf.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GSHHS_HPP_
#define GSHHS_HPP_

#include "datatypes.hpp"
#include "envt.hpp"

namespace lib {
  BaseGDL* gshhg_exists(EnvT* e);
  BaseGDL* proj4_exists(EnvT* e);
  BaseGDL* proj4new_exists(EnvT* e);

  void map_continents(EnvT* e);

} // namespace

#endif
