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
#ifdef USE_PNGLIB
// PNG_SKIP_SETJMP_CHECK seems to be a way to get round a png lib 'feature' see https://bugs.launchpad.net/ubuntu/+source/libpng/+bug/218409
#define PNG_SKIP_SETJMP_CHECK 1
#include "png.h"
#endif

class GDLSVGStream: public GDLGStream
{

  bool PaintImage(unsigned char *idata, PLINT nx, PLINT ny,  DLong *pos, DLong tru, DLong chan);
#ifdef USE_PNGLIB
  std::string svg_to_png64(int height, int width, unsigned char *image, int bit_depth, int bpp, int whattype, int *error);
#endif

public:
  GDLSVGStream( int nx, int ny):
    GDLGStream( nx, ny, "svg")
//cairo is nice but buffering prevents TV to work...
//    GDLGStream( nx, ny, checkPlplotDriver("svgcairo") ? "svgcairo" : "svg")
  {
//    plsetopt( "debug", "1");
  }

  ~GDLSVGStream()
  {
  }

  void eop()
  {
  }

  void Init();
  //logically close the svg each time an update is made, then rollback to the last graphic section for further graphics.
  void Update(){plstream::cmd(PLESC_EXPOSE, NULL);fprintf(pls->OutFile,"</g>\n</svg>\n");fseek(pls->OutFile,-12, SEEK_END);} 
};

#endif
