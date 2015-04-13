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
   this->plstream::init();
}

void GDLZStream::GetGeometry(long& xSize, long& ySize, long& xoff, long& yoff)
{
  xSize=pls->phyxma;
  ySize=pls->phyyma;
  xoff=0;
  yoff=0;
}
  
unsigned long GDLZStream::GetWindowDepth(){
   GraphicsDevice* actDevice=GraphicsDevice::GetDevice();
   return actDevice->GetPixelDepth(); 
}

bool GDLZStream::PaintImage( unsigned char *idata, PLINT nx, PLINT ny, DLong *pos,
DLong trueColorOrder, DLong chan ) {

  plstream::cmd( PLESC_FLUSH, NULL );
  unsigned char *mem = (unsigned char *) pls->dev;

  PLINT xoff = (PLINT) pos[0];
  PLINT yoff = (PLINT) pos[2];

  PLINT xsize = pls->phyxma;
  PLINT ysize = pls->phyyma;

  PLINT kxLimit = xsize - xoff;
  PLINT kyLimit = ysize - yoff;

  if ( nx < kxLimit ) kxLimit = nx;
  if ( ny < kyLimit ) kyLimit = ny;

  if ( nx > 0 && ny > 0 ) {
    SizeT p = (ysize - yoff - 1)*3*xsize;
    for ( int iy = 0; iy < kyLimit; ++iy ) {
      SizeT rowStart = p;
      p += xoff*3;
      for ( int ix = 0; ix < kxLimit; ++ix ) {
        if ( trueColorOrder == 0 && chan == 0 ) {
          mem[p++] = pls->cmap0[idata[iy * nx + ix]].r;
          mem[p++] = pls->cmap0[idata[iy * nx + ix]].g;
          mem[p++] = pls->cmap0[idata[iy * nx + ix]].b;
        } else {
          if ( chan == 0 ) {
            if ( trueColorOrder == 1 ) {
              mem[p++] = idata[3 * (iy * nx + ix) + 0];
              mem[p++] = idata[3 * (iy * nx + ix) + 1];
              mem[p++] = idata[3 * (iy * nx + ix) + 2];
            } else if ( trueColorOrder == 2 ) {
              mem[p++] = idata[nx * (iy * 3 + 0) + ix];
              mem[p++] = idata[nx * (iy * 3 + 1) + ix];
              mem[p++] = idata[nx * (iy * 3 + 2) + ix];
            } else if ( trueColorOrder == 3 ) {
              mem[p++] = idata[nx * (0 * ny + iy) + ix];
              mem[p++] = idata[nx * (1 * ny + iy) + ix];
              mem[p++] = idata[nx * (2 * ny + iy) + ix];
            }
          } else {
            if ( chan == 1 ) {
              mem[p++] = idata[1 * (iy * nx + ix) + 0];
              p += 2;
            } else if ( chan == 2 ) {
              p ++;
              mem[p++] = idata[1 * (iy * nx + ix) + 1];
              p ++;
            } else if ( chan == 3 ) {
              p += 2;
              mem[p++] = idata[1 * (iy * nx + ix) + 2];
            }
          }
        }
      }
      p = rowStart - (xsize*3);  
    }
  }
  return true;
}
DByteGDL* GDLZStream::GetBitmapData() {
    plstream::cmd( PLESC_FLUSH, NULL );
    unsigned char *mem = (unsigned char *) pls->dev;
    if (mem == NULL) return NULL;
    PLINT nx = pls->phyxma;
    PLINT ny = pls->phyyma;

    SizeT datadims[3];
    datadims[0] = nx;
    datadims[1] = ny;
    datadims[2] = 3;
    dimension datadim(datadims, (SizeT) 3);
    DByteGDL *bitmap = new DByteGDL( datadim, BaseGDL::NOZERO);
    //PADDING is 3BPP -- we revert Y to respect IDL default
    SizeT kpad = 0;
    for ( SizeT iy =0; iy < ny ; ++iy ) {
      for ( SizeT ix = 0; ix < nx; ++ix ) {
        (*bitmap)[3 * ((ny-1-iy) * nx + ix) + 0] =  mem[kpad++];
        (*bitmap)[3 * ((ny-1-iy) * nx + ix) + 1] =  mem[kpad++];
        (*bitmap)[3 * ((ny-1-iy) * nx + ix) + 2] =  mem[kpad++];
      }
    }
    return bitmap;
}