/* *************************************************************************
                          gdlpsstream.hpp  -  graphic stream postscript
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

#ifndef GDLPSSTREAM_HPP_
#define GDLPSSTREAM_HPP_

#include "initsysvar.hpp"
#include "gdlgstream.hpp"

class GDLPSStream: public GDLGStream
{
private:
  int page;
  bool encapsulated;
  bool portrait;
  long bitsPerPix;
public:
  GDLPSStream( int nx, int ny, int pfont, bool encaps, int color, int bpp, bool orient_portrait):
  GDLGStream::GDLGStream( nx, ny, (color==0)?"ps":"psc")
  {
    encapsulated = encaps;
    page = 0;
    portrait = orient_portrait;
    bitsPerPix=bpp;
  }

  ~GDLPSStream(){}
 
  void eop();

  void Init();
  bool PaintImage(unsigned char *idata, PLINT nx, PLINT ny,  DLong *pos, DLong tru, DLong chan);
  //logically close the svg each time an update is made, then rollback to the last graphic section for further graphics.
  //the adding of pls->stream_closed=XXX is just due to avoid bug https://sourceforge.net/p/plplot/bugs/203/ at least in the sens it does not crash GDL
  void Update(){plstream::cmd(PLESC_EXPOSE, NULL);pls->stream_closed=1;fprintf(pls->OutFile," S\neop\n");fseek(pls->OutFile,-7, SEEK_END);pls->stream_closed=0;}
//  void Update(){}

  virtual void fontChanged() final {
    PLINT doFont = ((PLINT) SysVar::GetPFont()>-1) ? 1 : 0;
    pls->dev_text = doFont;
  }
  
};

#endif
