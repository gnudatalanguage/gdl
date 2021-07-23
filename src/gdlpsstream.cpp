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
#include "graphicsdevice.hpp"
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
     plstream::eop();
   }
   page++;
};
void image_compress(unsigned char *idata, PLINT size, long bpp)
{
  unsigned char byt=0x0;
  SizeT i,j,k;
//  static unsigned char mask[]={0x0,0x1,0x3,0x0,0x7};
  static short count[]={0,7,3,0,1};
  static short shiftLeft[] ={0,1,2,0,4};
  static short shiftRight[] ={0,7,6,0,4};
  bool complete=false;
  for (i=0, j=0, k=0; i<size; ++i) {
    byt=((byt<<shiftLeft[bpp]) | (idata[i]>> shiftRight[bpp]));
    if (j == count[bpp]) {idata[k++]=byt; byt=0x0; j=0; complete=true;} else {j++;complete=false;}
  }
  if (!complete) { //something left undone...
    for (int l=0; l< count[bpp]-j+1; ++l ) byt=(byt<<shiftLeft[bpp]); //pad on left
    idata[k] = byt;
  }
  return;
}
bool GDLPSStream::PaintImage(unsigned char *idata, PLINT nx, PLINT ny, DLong *pos,
        DLong trueColorOrder, DLong channel) {
  bool bw = (((*static_cast<DLongGDL*> (SysVar::D()->GetTag(SysVar::D()->Desc()->TagIndex("FLAGS"), 0)))[0] & 16) == 0); 
  SizeT nelem;
  if (channel > 0) {
    cerr << "TV: Value of CHANNEL (use TRUE instead) is out of allowed range." << endl;
    return false;
  } 
  pls->bytecnt += fprintf(pls->OutFile, "\n%%BeginObject: Image\n");
  
  // note PLPLOT_PS_MAGIC_FACTOR = 1000 (dpi) /72 * 2.54 = 0.028346458 

  if (portrait) {
  pls->bytecnt += fprintf(pls->OutFile, "%d 0.028346458 mul XScale div %d 0.028346458 mul YScale div translate\n", pos[0], pos[2]);
  } else {
  pls->bytecnt += fprintf(pls->OutFile, "%d 0.028346458 mul YScale div hs 0.028346458 div %d sub 0.028346458 mul XScale div translate 270 rotate\n", pos[2], pos[0]);
  }
  pls->bytecnt += fprintf(pls->OutFile, "%d 0.028346458 mul XScale div %d 0.028346458 mul YScale div scale\n", pos[1], pos[3]);
#define LINEWIDTH 80

  if (trueColorOrder == 0) { 
    if  (bw) { //black and white native value 0->255: image
      pls->bytecnt += fprintf(pls->OutFile, "/gdlImagePixString %ld string def ", (long)ceil(1.0 * nx * bitsPerPix / 8.));
      pls->bytecnt += fprintf(pls->OutFile, "%d %d %ld ", nx, ny, bitsPerPix);

      pls->bytecnt += fprintf(pls->OutFile, "[%d 0 0 %d 0 0] \n {currentfile gdlImagePixString readhexstring pop} bind\n  image\n", nx, ny);
      //if bpp is not 8, convert to, else use it directly
      if (bitsPerPix != 8) image_compress(idata,1*nx*ny,bitsPerPix);
      //output data in lines of LINEWIDTH chars:
      nelem=(long)ceil(1.0 * nx * ny * bitsPerPix / 8.);
      for (SizeT i = 0, k=0 ; i < nelem ; ++i) {
        fprintf(pls->OutFile, "%2.2X", idata[i]); k+=2; if( (k % LINEWIDTH) == 0 ) fprintf(pls->OutFile, "\n");
        }
      pls->bytecnt += (nelem*2  + nelem*2/LINEWIDTH);
    } else {
      pls->bytecnt += fprintf(pls->OutFile, "/gdlImagePixString 1 string def ");
      pls->bytecnt += fprintf(pls->OutFile, "%d %d %ld ", nx, ny, bitsPerPix);
      pls->bytecnt += fprintf(pls->OutFile, "[%d 0 0 %d 0 0]\n", nx, ny);
      PLINT r[ctSize], g[ctSize], b[ctSize];
      GraphicsDevice::GetCT()->Get( r, g, b); //OUR colors, *NOT* plplot's colors (background mess)
      unsigned char *data=(unsigned char*)malloc(nx*ny*3*sizeof(unsigned char));
      //following is false for bits_per_pix not equal to 8. Colortable must be written differently. FIXME!
      for (SizeT i = 0; i < nx*ny; ++i) {data[i*3+0]=r[idata[i]];data[i*3+1]=g[idata[i]];data[i*3+2]=b[idata[i]];}
      pls->bytecnt += fprintf(pls->OutFile, "{currentfile gdlImagePixString readhexstring pop} bind false 3 colorimage\n");
      //if bpp is not 8, convert to, else use it directly
      if (bitsPerPix != 8) image_compress(data,3*nx*ny,bitsPerPix);
      //output data in lines of LINEWIDTH chars:
      nelem=(long)ceil(3.0 * nx * ny * bitsPerPix / 8.);
      for (SizeT i = 0, k = 0 ; i < nelem ; ++i) {
        fprintf(pls->OutFile, "%2.2X", data[i]); k+=2; if( (k % LINEWIDTH) == 0 ) fprintf(pls->OutFile, "\n");
        }
      free(data);
      pls->bytecnt += (nelem*2  + nelem*2/LINEWIDTH);
    }
  } else { //true color: native value (degrees of R, G and B)
    switch (trueColorOrder) {
      case 1:
        pls->bytecnt += fprintf(pls->OutFile, "/gdlImagePixString %d string def\n", nx * 3);
        pls->bytecnt += fprintf(pls->OutFile, "%d %d %ld\n", nx, ny, bitsPerPix);
        pls->bytecnt += fprintf(pls->OutFile, "[%d 0 0 %d 0 0] {currentfile gdlImagePixString readhexstring pop} bind\n  false 3 colorimage\n", nx, ny);
        break;
      case 2:
        pls->bytecnt += fprintf(pls->OutFile, "/gdlImagePixStringR %d string def /gdlImagePixStringG %d string def /gdlImagePixStringB %d string def\n", nx, nx, nx);
        pls->bytecnt += fprintf(pls->OutFile, "%d %d %ld\n", nx, ny, bitsPerPix);
        pls->bytecnt += fprintf(pls->OutFile, "[%d 0 0 %d 0 0] {currentfile gdlImagePixStringR readhexstring pop} bind\n  {currentfile gdlImagePixStringG readhexstring pop} bind\n  {currentfile gdlImagePixStringB readhexstring pop} bind\n  true 3 colorimage\n", nx, ny);
        break;
      case 3:
        pls->bytecnt += fprintf(pls->OutFile, "/gdlImagePixStringR %d string def /gdlImagePixStringG %d string def /gdlImagePixStringB %d string def\n", nx*ny, nx*ny, nx*ny);
        pls->bytecnt += fprintf(pls->OutFile, "%d %d %ld\n", nx, ny, bitsPerPix);
        pls->bytecnt += fprintf(pls->OutFile, "[%d 0 0 %d 0 0] {currentfile gdlImagePixStringR readhexstring pop} bind\n  {currentfile gdlImagePixStringG readhexstring pop} bind\n  {currentfile gdlImagePixStringB readhexstring pop} bind\n  true 3 colorimage\n",  nx, ny);
        break;
      default:
        cerr<<"you should not get here!"<<endl;
    }
    //if bpp is not 8, convert to, else use it directly
    if (bitsPerPix != 8) image_compress(idata,3*nx*ny,bitsPerPix);
    //output data in lines of LINEWIDTH chars:
    nelem=(long)ceil(3.0 * nx * ny * bitsPerPix / 8.);
    for (SizeT i = 0, k=0 ; i < nelem; ++i) {
      fprintf(pls->OutFile, "%2.2X", idata[i]); k+=2; if( (k % LINEWIDTH) == 0 ) fprintf(pls->OutFile, "\n");
      }
    pls->bytecnt += (nelem*2 + nelem*2/LINEWIDTH);
  }
  pls->bytecnt += fprintf(pls->OutFile, "\ngrestore\n%%EndObject: Image\n       ");
  return true;
#undef LINEWIDTH
}
