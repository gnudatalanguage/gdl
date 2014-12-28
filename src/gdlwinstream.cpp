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
#ifndef PLESC_TELLME
#define PLESC_TELLME  41
#endif
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
 
	plgpls(&pls);
 // plstream::cmd( PLESC_GETC, gin );
   wingcc_Dev *dev = (wingcc_Dev *) pls->dev;

    HCURSOR    cursor;
    HCURSOR    crosshair;
    HCURSOR    previous;
    RECT rcClip;
    RECT rcOldClip;
	POINT Point;
    UINT SWP = (SWP_NOMOVE | SWP_NOSIZE |SWP_SHOWWINDOW);
	HWND resetFG;
	resetFG = GetForegroundWindow();
	
    cursor = LoadCursor( NULL, IDC_CROSS );
#ifdef _WIN64
    SetClassLongPtr( dev->hwnd, GCLP_HCURSOR, (LONG_PTR) cursor );
#else
    SetClassLong( dev->hwnd, GCLP_HCURSOR, (LONG) cursor );
#endif
//   SetClassLongPtr( dev->hwnd, GCLP_HCURSOR, (LONG_PTR) dev->cursor );
    previous = SetCursor( cursor );

    SWP = (SWP_NOMOVE | SWP_NOSIZE |SWP_SHOWWINDOW);


    SetWindowPos(dev->hwnd, HWND_TOP, 0,0,0,0, SWP);
	GetClipCursor(&rcOldClip);
    GetWindowRect(dev->hwnd, &rcClip);  // I tried every which-way,this is the way!
	ClipCursor(&rcClip);                

 	gin->pX=-1;   // negative Xs are unlikely.
//   NOWAIT = 0,    WAIT, //1    CHANGE, //2    DOWN, //3    UP //4
// http://msdn.microsoft.com/en-us/library/windows/desktop/ms645602(v=vs.85).aspx

	bool rbutton, xbutton, mbutton;
	gin->button=0;
    while ( gin->pX < 0 )
    {
		rbutton=false; xbutton=false; mbutton=false;
        GetMessage( &dev->msg, NULL, 0, 0 );
        TranslateMessage( &dev->msg );
        switch ( (int) dev->msg.message )
        {
        case WM_XBUTTONDOWN:  xbutton=true;
        case WM_RBUTTONDOWN:  rbutton=true;
        case WM_MBUTTONDOWN:  mbutton=true;

        case WM_LBUTTONDOWN:
			if( mode == 4) {   // Looking for button up
				DispatchMessage(&dev->msg);
				break;
				}
			gin->button=1;
            gin->pX = dev->msg.pt.x;
            break;
        case WM_XBUTTONUP:		xbutton=true;
        case WM_RBUTTONUP:		rbutton=true;
        case WM_MBUTTONUP:  mbutton=true;
		case WM_LBUTTONUP:
			if( mode == 3) {  // Looking for button down
				DispatchMessage(&dev->msg);
				break;
				}
			gin->button=1;
            gin->pX = dev->msg.pt.x;
            break;
        case WM_CHAR:
            gin->pX = dev->msg.pt.x;
            gin->keysym = dev->msg.wParam;
            break;
		default:
			if(mode == 0) {
        		GetCursorPos(&Point);
		        gin->pX = Point.x;        gin->pY = Point.y;
	 		} else	DispatchMessage(&dev->msg);
        }  
	    gin->pY = dev->msg.pt.y;
    }
	if(xbutton) gin->button=4; else 
		if(rbutton) gin->button=3; else
			if(mbutton) gin->button=2;
    gin->pX -= rcClip.left; 
	gin->pY = rcClip.bottom - gin->pY; //  switch it to the X convention.
    int xcorr =  8;  // yeah, really.
    int ycorr =  8;  // a reserve in the borders?
    gin->dX = ((PLFLT) gin->pX - xcorr) / ( dev->width - 1); // !! this is the reversible answer.
    gin->dY = ((PLFLT) gin->pY - ycorr) / ( dev->height - 1); // re-plotting in normal coordinates
    ClipCursor(&rcOldClip);

 	SetForegroundWindow(resetFG);
    SetCursor( previous );

    Sleep(2);
	BringWindowToTop(dev->hwnd);
    Sleep( 2);
    SetFocus(resetFG);

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
void GDLWINStream::CheckValid() {
  wingcc_Dev *dev = (wingcc_Dev *) pls->dev;
  if(!IsWindow(dev->hwnd)) this->SetValid(false);
}

void GDLWINStream::Flush() {
   GdiFlush();
}

DLong GDLWINStream::GetVisualDepth(){
  wingcc_Dev *dev = (wingcc_Dev *) pls->dev;
  return GetDeviceCaps(dev->hdc,PLANES);
  }

unsigned long  GDLWINStream::GetWindowDepth(){
  return GetVisualDepth();
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
