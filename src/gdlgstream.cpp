/* *************************************************************************
                          gdlgstream.cpp  -  graphic stream
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

#include <iostream>

#include "graphics.hpp"
#include "gdlgstream.hpp"
#include "initsysvar.hpp"

using namespace std;

void PLPlotAbortHandler(char *c)
{
  cout << "PLPlot abort handler: " << c << endl;
}

int PLPlotExitHandler(char *c)
{
  cout << "PLPlot exit handler: " << c << endl;
  return 0;
}

void GDLGStream::SetErrorHandlers()
{
  plsexit( PLPlotExitHandler);
  plsabort( PLPlotAbortHandler);
}

void GDLGStream::Color( ULong c, UInt ix)
{
  DByte r,g,b;
  if( c < ctSize)
    {
      Graphics::GetCT()->Get( c, r, g, b);
    }
  else
    {
      r = c & 0xFF;
      g = (c >> 8)  & 0xFF;
      b = (c >> 16) & 0xFF;
    }
  plstream::scol0( ix, r, g, b);
  plstream::col0( ix);
}

void GDLGStream::Background( ULong c)
{
  DByte r,g,b;
  if( c < ctSize)
    {
      Graphics::GetCT()->Get( c, r, g, b);
    }
  else
    {
      r = c & 0xFF;
      g = (c >> 8)  & 0xFF;
      b = (c >> 16) & 0xFF;
    }
  plstream::scolbg( r, g, b);
}

// plplot 5.3 does not provide the clear function for c++
void GDLGStream::Clear()
{
  // dummy call to get private function set_stream() called
  PLFLT a=0.0,b=0.0,c=0.0,d,e,f;
  RGB_HLS( a,b,c,&d,&e,&f);

  ::c_plclear();
}

void GDLGStream::NextPlot( bool erase)
{
  DLongGDL* pMulti = SysVar::GetPMulti();

  DLong nx = (*pMulti)[ 1];
  DLong ny = (*pMulti)[ 2];
  DLong nz = (*pMulti)[ 3];

  DLong dir = (*pMulti)[ 4];

  nx = (nx>0)?nx:1;
  ny = (ny>0)?ny:1;
  nz = (nz>0)?nz:1;

  plstream::ssub( nx, ny);

  if( (*pMulti)[ 0] <= 0)
    {
      if( erase)
	{
	  plstream::eop();
	  plstream::bop();
	}
      plstream::adv(1);
      (*pMulti)[ 0] = nx*ny*nz-1;
    }
  else
    {
      DLong pMod = (*pMulti)[ 0] % (nx*ny);
      if( dir == 0)
	plstream::adv(nx*ny - pMod + 1);
      else
	{
	  int p = nx*ny - pMod;
	  int pp = p*nx % (nx*ny) + p/ny + 1;
	  plstream::adv(pp);
	}
      if( erase) --(*pMulti)[ 0];
    }
}
