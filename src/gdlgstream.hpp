/* *************************************************************************
                          gdlgstream.hpp  -  graphic stream
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

#ifndef GDLGSTREAM_HPP_
#define GDLGSTREAM_HPP_

#include <plplot/plstream.h>
#include <plplot/plxwd.h>
#include <plplot/plplot.h>

#include "typedefs.hpp"

class GDLGStream: public plstream
{
  void init(); // prevent plstream::init from being called directly

protected:
  bool valid;

public:
    GDLGStream( int nx, int ny, 
		const char *driver, 
		const char *file=NULL):
      plstream( nx, ny, driver, file), valid( true)
  {
  }

  virtual ~GDLGStream()
  {
  }
  
  static void SetErrorHandlers();

  virtual void Init()=0;
  virtual void EventHandler() {}
  virtual void GetGeometry( long& xSize, long& ySize, long& xoff, long& yoff);

  virtual void eop()          { plstream::eop();}

  bool Valid() { return valid;}

  void Clear();
  void Color( ULong c, ULong ncolor=256, UInt ix=1);
  void Background( ULong c);

  void DefaultCharSize();
  void NextPlot( bool erase=true); // handles multi plots

  void NoSub(); // no subwindows (/NORM, /DEVICE)
};

#endif
