/***************************************************************************
                          image.hpp  -  GDL image routines
                             -------------------
    begin                : Jul 20 2004
    copyright            : (C) 2004 by Joel Gales
    email                : jomoga@users.sourceforge.net
 ***************************************************************************/

/* *************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef IMAGE_HPP_
#define IMAGE_HPP_

#include <plplot/plstream.h>
#include <plplot/plplotP.h>

#include "envt.hpp"

namespace lib {

  void plimage_gdl(unsigned char *idata, PLINT nx, PLINT ny, DInt tru);

  void tv( EnvT* e);
  void loadct( EnvT* e);

  BaseGDL* tvrd( EnvT* e);

} // namespace

#endif
