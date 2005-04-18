/* *************************************************************************
                          plotting.hpp  -  GDL routines for plotting
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
    email                : m_schellens@users.sf.net
 ***************************************************************************/

/* *************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PLOTTING_HPP_
#define PLOTTING_HPP_

#include "envt.hpp"

namespace lib {

  void set_plot( EnvT* e);
  void device( EnvT* e);
  void plot( EnvT* e);
  void oplot( EnvT* e);
  void plots( EnvT* e);
  void surface( EnvT* e);
  void xyouts( EnvT* e);
  void wset( EnvT* e);
  void window( EnvT* e);
  void wshow( EnvT* e);
  void wdelete( EnvT* e);
  void tvlct( EnvT* e);

} // namespace

#endif
