/* *************************************************************************
                          gdlzstream.cpp  -  graphic stream z-buffer
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

//#include <iostream>

#include "includefirst.hpp"

#include "gdlzstream.hpp"

using namespace std;

void GDLZStream::eop()
{
  GraphicsDevice* actDevice=GraphicsDevice::GetDevice();
  actDevice->ClearStream( (DLong) 0);
}

void GDLZStream::Clear()
{
  GraphicsDevice* actDevice=GraphicsDevice::GetDevice();
  actDevice->ClearStream( (DLong) 0);
}

void GDLZStream::Clear( DLong bColor)
{
  GraphicsDevice* actDevice=GraphicsDevice::GetDevice();
  actDevice->ClearStream( bColor);
}

void GDLZStream::Init()
{
   plstream::init();
}

