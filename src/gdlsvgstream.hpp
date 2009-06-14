/* *************************************************************************
                          gdlsvgstream.hpp  -  graphic stream SVG
                             -------------------
    begin                : December 26 2008
    copyright            : (C) 2008 by Sylwester Arabas
    email                : slayoo@users.sf.net
 ***************************************************************************/

/* *************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GDLSVGSTREAM_HPP_
#define GDLSVGSTREAM_HPP_

#include "gdlgstream.hpp"

class GDLSVGStream: public GDLGStream
{
public:
  GDLSVGStream( int nx, int ny):
    GDLGStream( nx, ny, "svgcairo")
  {
  }

  ~GDLSVGStream()
  {
  }
 
  //  void eop() {};

  void Init();
};

#endif
