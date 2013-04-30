/* *************************************************************************
                          gdlpsstream.cpp  -  graphic stream postscript
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

#include "includefirst.hpp"

//#include <iostream>
#include "graphics.hpp"
#include "gdlpsstream.hpp"

using namespace std;

void GDLPSStream::Init()
{
   plstream::init();
   page = 0;
}
 
void GDLPSStream::eop()
{
   if (page != 0) 
   {
     if (encapsulated) 
     {
       Warning("Warning: multi-page output violates Encapsulated PostScript specification");
     }
     else
     {
#ifdef USE_PSLIB
       Warning("Warning: multi-page PostScript not supported yet (FIXME!)");
#endif
     }
     plstream::eop();
   }
   page++;
};
  //overload Background to 1) set background and 2) clear to have the same behaviour
  //as for device='x'.
  void GDLPSStream::Background( ULong c, DLong decomposed)
  {
    DByte r,g,b;
    if (decomposed == 0) c = c & 0x0000FF;

    if( c < ctSize && decomposed == 0)
      {
        Graphics::GetCT()->Get( c, r, g, b);
      }
    else {
        r = c & 0xFF;
        g = (c >> 8)  & 0xFF;
        b = (c >> 16) & 0xFF;
      }
     plstream::scolbg( r, g, b);
     plstream::clear();  //only this is added...
  }
