/* *************************************************************************
                          gdlwinstream.hpp  -  graphic stream M$ windows
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
    email                : m_schellens@hotmail.com
 ***************************************************************************/

/* *************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GDLWINSTREAM_HPP_
#define GDLWINSTREAM_HPP_

#include "gdlgstream.hpp"

class GDLWINStream: public GDLGStream
{
  Atom wm_protocols;
  Atom wm_delete_window;

  PLStream* pls;

public:
  GDLWINStream( int nx, int ny):
    GDLGStream( nx, ny, "win3")
  {
  }

  ~GDLWINStream()
  {
  }
  
  void Init();
  void EventHandler();
};

#endif
