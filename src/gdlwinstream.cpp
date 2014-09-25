/* *************************************************************************
						  gdlwinstream.cpp  -  graphic stream M$ windows
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

#include <iostream>

#include "graphicsdevice.hpp"
using namespace std;
#include "gdlwinstream.hpp"
#include "devicewin.hpp"

void GDLWINStream::Init()
{
	this->plstream::init();
	plgpls(&pls);
	wingcc_Dev* dev = (wingcc_Dev *)pls->dev;
	dev->waiting = 1;

	UnsetFocus();
}

void GDLWINStream::EventHandler()
{
	MSG Message;
	if (PeekMessage(&Message, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
}
bool GDLWINStream::GetGin(PLGraphicsIn *gin, int mode) {
    LPPOINT lpt;

  enum CursorOpt {
    NOWAIT = 0,
    WAIT, //1
    CHANGE, //2
    DOWN, //3
    UP //4
    };
 
  gin->button = mode; // passing our mode into the event loop.

  plstream::cmd( PLESC_GETC, gin );
  return true;  
}

bool GDLWINStream::PaintImage( unsigned char *idata, PLINT nx, PLINT ny, 
				DLong *pos, DLong tru, DLong chan ) 
  {
  plstream::cmd( PLESC_FLUSH, NULL );
  
  wingcc_Dev *dev = (wingcc_Dev *) pls->dev;
  HDC hdc = dev->hdc;

  PLINT xoff = (PLINT) pos[0];
  PLINT yoff = (PLINT) pos[2];
  PLINT kx, ky;

  PLINT xsize = pls->phyxma;
  PLINT ysize = pls->phyyma;

  PLINT kxLimit = xsize - xoff;
  PLINT kyLimit = ysize - yoff;

  if ( nx < kxLimit ) kxLimit = nx;
  if ( ny < kyLimit ) kyLimit = ny;

  if ( nx > 0 && ny > 0 ) {
    char iclr1, ired, igrn, iblu;
    long curcolor;

    for ( SizeT ix = 0; ix < kxLimit; ++ix ) {
      for ( SizeT iy = 0; iy < kyLimit; ++iy ) {

        kx = xoff + ix;
        ky = yoff + iy;

        if ( tru == 0 && chan == 0 ) {
          iclr1 = idata[iy * nx + ix];

          curcolor = RGB( pls->cmap1[iclr1].r,
          pls->cmap1[iclr1].g, pls->cmap1[iclr1].b );

          //	 			 printf("ix: %d  iy: %d  pixel: %d\n", ix,iy,curcolor.pixel);

        } else {
          if ( chan == 0 ) {
            if ( tru == 1 ) {
              ired = idata[3 * (iy * nx + ix) + 0];
              igrn = idata[3 * (iy * nx + ix) + 1];
              iblu = idata[3 * (iy * nx + ix) + 2];
            } else if ( tru == 2 ) {
              ired = idata[nx * (iy * 3 + 0) + ix];
              igrn = idata[nx * (iy * 3 + 1) + ix];
              iblu = idata[nx * (iy * 3 + 2) + ix];
            } else if ( tru == 3 ) {
              ired = idata[nx * (0 * ny + iy) + ix];
              igrn = idata[nx * (1 * ny + iy) + ix];
              iblu = idata[nx * (2 * ny + iy) + ix];
            }
            curcolor = RGB( ired, igrn, iblu ); // It is reverse!
          } else if ( chan == 1 ) {
            unsigned long pixel =
            GetPixel( hdc, ix, dev->height - 1 - ky ) & 0xffff00;
            ired = idata[1 * (iy * nx + ix) + 0];
            curcolor = RGB( ired, 0, 0 ) + pixel; // =ired*256*256 + pixel
          } else if ( chan == 2 ) {
            unsigned long pixel =
            GetPixel( hdc, ix, dev->height - 1 - ky ) & 0xff00ff;
            igrn = idata[1 * (iy * nx + ix) + 1];
            curcolor = RGB( 0, igrn, 0 ) + pixel; // = igrn * 256 + pixel
          } else if ( chan == 3 ) {
            unsigned long pixel =
            GetPixel( hdc, ix, dev->height - 1 - ky ) & 0x00ffff;
            iblu = idata[1 * (iy * nx + ix) + 2];
            curcolor = RGB( 0, 0, iblu ) + pixel; // = iblu + pixel
          } // if (chan == 0) else
        } // if (tru == 0  && chan == 0) else

        if ( ky < dev->height && kx < dev->width )
          SetPixel( hdc, kx, dev->height - 1 - ky, curcolor );
      } // for() inner (indent error)
    } // for() outer
  }
  return true;
}
void GDLWINStream::Raise() 
{
  wingcc_Dev *dev = (wingcc_Dev *) pls->dev;
  BringWindowToTop(dev->hwnd);
  return;
}
void GDLWINStream::GetGeometry( long& xSize, long& ySize, long& xoff, long& yoff) {
  wingcc_Dev *dev = (wingcc_Dev *) pls->dev;

  GetWindowInfo(dev->hwnd, &Winfo);
  xSize = Winfo.rcWindow.right - Winfo.rcWindow.left;
  ySize = Winfo.rcWindow.bottom - Winfo.rcWindow.top;
  xoff = Winfo.rcWindow.left;
  yoff = Winfo.rcWindow.top;
}
bool GDLWINStream::GetWindowPosition( long& xpos, long& ypos) {
  wingcc_Dev *dev = (wingcc_Dev *) pls->dev;
  
  GetWindowInfo(dev->hwnd, &Winfo);
  xpos = Winfo.rcWindow.left;
  ypos = Winfo.rcWindow.top;
  return true;
}

void GDLWINStream::Lower()
{
  wingcc_Dev *dev = (wingcc_Dev *) pls->dev;
  UINT SWP = (SWP_NOMOVE | SWP_NOSIZE);
  SetWindowPos(dev->hwnd, HWND_BOTTOM, 0,0,0,0, SWP);
  return;
}

void GDLWINStream::Iconic() {
  wingcc_Dev *dev = (wingcc_Dev *) pls->dev;
 UINT SWP = (SWP_NOMOVE | SWP_NOSIZE |SWP_HIDEWINDOW);  // this does nothinc
  SetWindowPos(dev->hwnd, HWND_BOTTOM, 0,0,0,0, SWP);
  return;
}

void GDLWINStream::DeIconic() {
  wingcc_Dev *dev = (wingcc_Dev *) pls->dev;
 UINT SWP = (SWP_NOMOVE | SWP_NOSIZE |SWP_SHOWWINDOW);
  SetWindowPos(dev->hwnd, HWND_BOTTOM, 0,0,0,0, SWP);
  return;

}

void GDLWINStream::Flush() {
   GdiFlush();
}

bool GDLWINStream::UnsetFocus()
{ 
    SetFocus(this->refocus); 
    return true;
 }


void GDLWINStream::Clear()
{
    ::c_plbop();
    ::c_plclear();
}
