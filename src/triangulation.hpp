/***************************************************************************
                          triangulation.hpp  -  GDL header
                             -------------------
    begin                : Aug 30 2017
    copyright            : (C) 2017 by Gilles Duvert

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


#ifndef TRIANGULATION_HPP_
#define TRIANGULATION_HPP_

#include "datatypes.hpp"
#include "envt.hpp"
namespace lib {
  void GDL_Triangulate(EnvT* e);
  BaseGDL* trigrid_fun( EnvT* e);
  void grid_input (EnvT* e);
#ifdef PL_HAVE_QHULL
  void qhull ( EnvT* e);
  BaseGDL* qgrid3_fun ( EnvT* e);
#endif
}
#endif
