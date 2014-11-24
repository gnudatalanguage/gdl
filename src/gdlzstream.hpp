/* *************************************************************************
                          gdlzstream.hpp  -  graphic stream z-buffer
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

#ifndef GDLZSTREAM_HPP_
#define GDLZSTREAM_HPP_

#include <iostream>

#include "graphicsdevice.hpp"
#include "gdlgstream.hpp"

class GDLZStream: public GDLGStream
{
public:
  GDLZStream( int nx, int ny):
    GDLGStream( nx, ny, "mem")
  {
  }

  ~GDLZStream()
  {
  }

  //    void eop() {} // never eop (mem drivers eop() sets pls->dev to NULL)

  void eop();

  void Clear();
  void Clear( DLong bColor);
  
  void Init();
  bool PaintImage(unsigned char *idata, PLINT nx, PLINT ny,  DLong *pos, DLong tru, DLong chan);
  void GetGeometry( long& xSize, long& ySize, long& xoff, long& yoff);
  unsigned long GetWindowDepth();
  DByteGDL* GetBitmapData();
//to be written. Needed by same needs as for X11
//  bool SetGraphicsFunction(long value );
};

#endif
