/* *************************************************************************
                          gdlsvgstream.cpp  -  graphic stream SVG
                             -------------------
    begin                : December 26 2008
    copyright            : (C) 2002 by Sylwester Arabas
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

#include "includefirst.hpp"
#include "gdlsvgstream.hpp"

using namespace std;

void GDLSVGStream::Init()
{
   plstream::init();
}
bool  GDLSVGStream::PaintImage(unsigned char *idata, PLINT nx, PLINT ny, DLong *pos,
		   DLong trueColorOrder, DLong channel) {
//need to : check position in file Ok; update bounding box values, convert image to png.
  return false;
//long xs,ys,xo,yo;
//GDLSVGStream::GetGeometry(xs,ys,xo,yo);
//#define XSIZE         (int) ( pls->xlength * ( pls->xdpi / 72. ) )
//#define YSIZE         (int) ( pls->ylength * ( pls->ydpi / 72. ) )
//#define ENLARGE       5
//#define XPSSIZE       ENLARGE * XSIZE
//#define YPSSIZE       ENLARGE * YSIZE
//#define BUFLEN 256
//  char   outbuf[BUFLEN];
//  static DLong ncomp;
//  if (channel>0) ncomp=1; else ncomp=(trueColorOrder==0)?1:3;
//  snprintf(outbuf,BUFLEN,"<image preserveAspectRatio=\"none\" x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" xlink:href=\"data:image/svg;base64,",
//          pos[0],pos[2],pos[1],pos[3]);  pls->bytecnt += 1 + (PLINT) strlen( outbuf ); fprintf(pls->OutFile,outbuf); 
//  if (pls->diorot==1.0) {
//  } else  {
//  }
//  if (channel == 0) 
//  {
//   if (trueColorOrder == 0) { //indexed value 0->255: image
//    } else { 
//    switch (trueColorOrder)
//    {
//      case 1:
//      break;
//      case 2:
//        break;
//      case 3:
//    }
//    }
//  } else { //channel = 1 to 3
//  }
//  strncpy(outbuf,"\"/>",BUFLEN);  pls->bytecnt += 1 + (PLINT) strlen( outbuf ); fprintf(pls->OutFile,outbuf);
//  return true;
#undef BUFLEN
}
