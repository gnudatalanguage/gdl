/* *************************************************************************
                          gdlnullstream.hpp  -  graphic stream z-buffer
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2025 by G. Duvert
    email                : see github
 ***************************************************************************/

/* *************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GDLNULLSTREAM_HPP_
#define GDLNULLSTREAM_HPP_

#include <iostream>

#include "graphicsdevice.hpp"
#include "gdlgstream.hpp"

class GDLNULLStream: public GDLGStream
{
public:
  GDLNULLStream( int nx, int ny): GDLGStream( nx, ny, "null")
  {
  }

  ~GDLNULLStream()
  {
  }
//No graphic commands 
 void Init()
{
   this->plstream::init();
}
};
#endif
