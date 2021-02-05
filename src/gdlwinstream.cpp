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
#include "gdleventhandler.hpp"
#ifndef PLESC_TELLME
#define PLESC_TELLME  41
#endif

BYTE ANDmaskCursor[128] =
{
	0xff, 0xfe, 0xff, 0xff,	0xff, 0xff, 0xff, 0xff,	0xff, 0xfe, 0xff, 0xff,	0xff, 0xff, 0xff, 0xff,
	0xff, 0xfe, 0xff, 0xff,	0xff, 0xff, 0xff, 0xff,	0xff, 0xfe, 0xff, 0xff,	0xff, 0xff, 0xff, 0xff,
	0xff, 0xfe, 0xff, 0xff,	0xff, 0xff, 0xff, 0xff,	0xff, 0xfe, 0xff, 0xff,	0xff, 0xff, 0xff, 0xff,
	0xff, 0xfe, 0xff, 0xff,	0xff, 0xff, 0xff, 0xff,	0xaa, 0xaa, 0xaa, 0xab,	0xff, 0xff, 0xff, 0xff,
	0xff, 0xfe, 0xff, 0xff,	0xff, 0xff, 0xff, 0xff,	0xff, 0xfe, 0xff, 0xff,	0xff, 0xff, 0xff, 0xff,
	0xff, 0xfe, 0xff, 0xff,	0xff, 0xff, 0xff, 0xff,	0xff, 0xfe, 0xff, 0xff,	0xff, 0xff, 0xff, 0xff,
	0xff, 0xfe, 0xff, 0xff,	0xff, 0xff, 0xff, 0xff,	0xff, 0xfe, 0xff, 0xff,	0xff, 0xff, 0xff, 0xff,
	0xff, 0xfe, 0xff, 0xff,	0xff, 0xff, 0xff, 0xff,	0xff, 0xff, 0xff, 0xff,	0xff, 0xff, 0xff, 0xff,
};

BYTE XORmaskCursor[128] =
{
	0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0x55, 0x55, 0x54, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

GDLWINStream::~GDLWINStream()
{
//
// wingdi.c is a plplot driver in development which may
// in future be preferable to the use of wingcc.
// The macro USE_WINGDI_NOT_WINGCC is sufficient to
// accomodate that driver in this same code 
// that uses wingcc.c.  GetHwnd() and GetHdc() allow
// access to the window and device context handles,
// respectively, for either case.
//
	if (tv_buf.has_data) {
		tv_buf.has_data = false;
		delete[] tv_buf.lpbitmap;
	}
	DestroyWindow(GetHwnd()); // Manually destroy window
}

void GDLWINStream::Init()
{
	this->plstream::init();
	plgpls(&pls);
#ifdef USE_WINGDI_NOT_WINGCC
#else
	wingcc_Dev* dev = (wingcc_Dev *)pls->dev;
	dev->waiting = 1;
#endif
	UnsetFocus();
	tv_buf.has_data = false;

    HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(NULL);
    CrosshairCursor = CreateCursor(hInstance, 15, 14, 32, 32, ANDmaskCursor, XORmaskCursor);
}

void GDLWINStream::SetWindowTitle(char* buf) {

	SetWindowTextA(GetHwnd(), buf);
}

void GDLWINStream::EventHandler()
{
	if (!valid) return;

	if (GetHwnd() == NULL) {
		cerr << "Invalid window." << endl;
		valid = false;
		return;
	}

// if wxWidgets is available, then use wxWidgets' event handler instead.
#ifndef HAVE_LIBWXWIDGETS
    MSG Message;
    if (PeekMessage(&Message, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&Message);
        if (Message.message == WM_DESTROY) {
            this->SetValid(false);
            cout << "WM_DESTROY detected in GDLWINSTREAM:: " << endl;
        }
        DispatchMessage(&Message);
    }
#endif
}
void GDLWINStream::Load_gin(HWND window){
    RECT rcClient;

    GetClientRect(window, &rcClient);
    ScreenToClient(window, &GinPoint);
    _gin->pX = GinPoint.x;
    _gin->pY = (rcClient.bottom - rcClient.top) - GinPoint.y;
    if (xbutton) _gin->button = 4; else
        if (rbutton) _gin->button = 3; else
            if (mbutton) _gin->button = 2;
    _gin->dX = ((PLFLT)_gin->pX) / (rcClient.right - rcClient.left - 1);
    _gin->dY = ((PLFLT)_gin->pY) / (rcClient.bottom - rcClient.top - 1);
	return;
}

void CALLBACK GDLWINStream::GinCallback(UINT message, WPARAM wParam, LPARAM lParam) {
    HWND window = GetHwnd();
    RECT rcClient;

    _gin->button = 0;
    buttonpressed = false; rbutton = false; xbutton = false; mbutton = false;

    GetClientRect(window, &rcClient); // https://msdn.microsoft.com/library/windows/desktop/ms633503%28v=vs.85%29.aspx
    ClientToScreen(window, (LPPOINT)&rcClient.left); // http://support.microsoft.com/en-us/kb/11570
    ClientToScreen(window, (LPPOINT)&rcClient.right);

    switch (message)
    {
        case WM_XBUTTONDOWN:  xbutton = true; break;
        case WM_RBUTTONDOWN:  rbutton = true; break;
        case WM_MBUTTONDOWN:  mbutton = true; break;
        case WM_LBUTTONDOWN:
            if (wParam != MK_LBUTTON && wParam != MK_MBUTTON && 
	        wParam != MK_RBUTTON && wParam != MK_XBUTTON1 && wParam != MK_XBUTTON1)
                return;
            if (_mode == 4) break;   // Looking for button up
            _gin->button = 1;
            buttonpressed = true;
            break;
        case WM_XBUTTONUP:	xbutton = true; break;
        case WM_RBUTTONUP:	rbutton = true; break;
        case WM_MBUTTONUP:  mbutton = true;  break;
        case WM_LBUTTONUP:
            if (wParam != MK_LBUTTON && wParam != MK_MBUTTON && wParam != MK_RBUTTON && wParam != MK_XBUTTON1 && wParam != MK_XBUTTON1)
                return;
            if (_mode == 3) break;  // Looking for button down
            _gin->button = 1;
            buttonpressed = true;
            break;
        case WM_CHAR:
            _gin->keysym = wParam;
            buttonpressed = true;
            break;
        default:
            if (_mode == 0) {
                GetCursorPos(&GinPoint);
                buttonpressed = true;
            }
    }
    if (_mode == 2)  buttonpressed = true;
    GetCursorPos(&GinPoint);
    if (!buttonpressed ||
		 rcClient.left > GinPoint.x ||
         GinPoint.x > rcClient.right ||
		 rcClient.top > GinPoint.y    ||
		GinPoint.y > rcClient.bottom)
    			{
        buttonpressed = false;
        return;
    			}
	Load_gin(window);
	return;
}

bool GDLWINStream::GetGin(PLGraphicsIn *gin, int mode) {
	LPPOINT lpt;
    _gin = gin;
    _mode = mode;

	HWND window = GetHwnd();

	HCURSOR    previous;

	UINT SWP = (SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
 	HWND resetFG = GetForegroundWindow();

	previous = SetCursor(CrosshairCursor);
	SetClassLongPtr(window, GCLP_HCURSOR, reinterpret_cast<LONG_PTR>(CrosshairCursor));
    SetWindowPos(window, HWND_BOTTOM,               // DeIconic()
        0,0,0,0, (SWP_NOMOVE | SWP_NOSIZE |SWP_SHOWWINDOW));
    BringWindowToTop(window);


	if(_mode == 0) {
        GetCursorPos(&GinPoint);
        buttonpressed = true;
	    Load_gin(window);
	    SetCursor(previous); Sleep(2);
	    return true;	
	    }

    buttonpressed = false;
    msghooks[WM_XBUTTONDOWN] = &GDLWINStream::GinCallback;
    msghooks[WM_RBUTTONDOWN] = &GDLWINStream::GinCallback;
    msghooks[WM_MBUTTONDOWN] = &GDLWINStream::GinCallback;
    msghooks[WM_LBUTTONDOWN] = &GDLWINStream::GinCallback;
    msghooks[WM_XBUTTONUP] = &GDLWINStream::GinCallback;
    msghooks[WM_RBUTTONUP] = &GDLWINStream::GinCallback;
    msghooks[WM_MBUTTONUP] = &GDLWINStream::GinCallback;
    msghooks[WM_LBUTTONUP] = &GDLWINStream::GinCallback;
    msghooks[WM_CHAR] = &GDLWINStream::GinCallback;

    while (!buttonpressed)
	{
        if (valid == false)
            return false; // TODO: Instead this, it should spawn a new window!
        GDLEventHandler();
        RedrawTV();
        Sleep(10);
	}

    msghooks.erase(WM_XBUTTONDOWN);
    msghooks.erase(WM_RBUTTONDOWN);
    msghooks.erase(WM_MBUTTONDOWN);
    msghooks.erase(WM_LBUTTONDOWN);
    msghooks.erase(WM_XBUTTONUP);
    msghooks.erase(WM_RBUTTONUP);
    msghooks.erase(WM_MBUTTONUP);
    msghooks.erase(WM_LBUTTONUP);
    msghooks.erase(WM_CHAR);

	SetForegroundWindow(resetFG);
	SetCursor(previous);

	Sleep(2);
	BringWindowToTop(window);
	Sleep(2);
	SetFocus(resetFG);

	return true;
}

bool GDLWINStream::PaintImage(unsigned char *idata, PLINT nx, PLINT ny,
	DLong *pos, DLong tru, DLong chan)
{
	plstream::cmd(PLESC_FLUSH, NULL);
	

	RECT rt;

	PLINT kxLimit, kyLimit;
	PLINT kx, ky;
	PLINT xoff, yoff;

	xoff = (PLINT)pos[0];
	yoff = (PLINT)pos[2];

	kxLimit = pls->phyxma - xoff;
	kyLimit = pls->phyyma - yoff;

	if (nx < kxLimit) kxLimit = nx;
	if (ny < kyLimit) kyLimit = ny;

	if (nx > 0 && ny > 0) {
		unsigned char iclr1, ired, igrn, iblu;

		GetClientRect(GetHwnd(), &rt);
		if (tv_buf.has_data && (tv_buf.bi.bmiHeader.biWidth != rt.right + 1 || -(tv_buf.bi.bmiHeader.biHeight) != rt.bottom + 1))
		{
			// Resize tv_buf.lpbitmap
			RGBQUAD* lpbitmap = new RGBQUAD[(rt.right + 1) * (rt.bottom + 1)];
			memset(lpbitmap, 0, sizeof(RGBQUAD) * (rt.right + 1) * (rt.bottom + 1));
			for (SizeT ix = 0; ix < min(tv_buf.bi.bmiHeader.biWidth, rt.right + 1); ix++)
				for (SizeT iy = 0; iy < min((-tv_buf.bi.bmiHeader.biHeight), rt.bottom + 1); iy++)
					lpbitmap[iy*(rt.right + 1) + ix] = tv_buf.lpbitmap[iy*tv_buf.bi.bmiHeader.biWidth + ix];
			delete[] tv_buf.lpbitmap;
			tv_buf.lpbitmap = lpbitmap;
		} else if (!tv_buf.has_data) {
			tv_buf.lpbitmap = new RGBQUAD[(rt.right + 1) * (rt.bottom + 1)];
		}

		memset(&tv_buf.bi, 0, sizeof(BITMAPINFO));
		tv_buf.bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		tv_buf.bi.bmiHeader.biWidth = rt.right + 1;
		tv_buf.bi.bmiHeader.biHeight = -(rt.bottom + 1);
		tv_buf.bi.bmiHeader.biPlanes = 1;
		tv_buf.bi.bmiHeader.biBitCount = 32;
		tv_buf.bi.bmiHeader.biCompression = BI_RGB;

		for (SizeT ix = 0; ix < kxLimit; ++ix) {
			for (SizeT iy = 0; iy < kyLimit; ++iy) {
				kx = xoff + ix;
				ky = rt.bottom  - (yoff + iy + 1); // To be the same as IDL
				if (ky < 0 || kx >= rt.right + 1)
					continue;
				if (tru == 0 && chan == 0) {
					iclr1 = idata[iy * nx + ix];
					tv_buf.lpbitmap[ky * (rt.right + 1) + kx].rgbBlue = pls->cmap1[iclr1].b;
					tv_buf.lpbitmap[ky * (rt.right + 1) + kx].rgbGreen = pls->cmap1[iclr1].g;
					tv_buf.lpbitmap[ky * (rt.right + 1) + kx].rgbRed = pls->cmap1[iclr1].r;
				}
				else {
					if (chan == 0) {
						if (tru == 1) {
							ired = idata[3 * (iy * nx + ix) + 0];
							igrn = idata[3 * (iy * nx + ix) + 1];
							iblu = idata[3 * (iy * nx + ix) + 2];
						}
						else if (tru == 2) {
							ired = idata[nx * (iy * 3 + 0) + ix];
							igrn = idata[nx * (iy * 3 + 1) + ix];
							iblu = idata[nx * (iy * 3 + 2) + ix];
						}
						else if (tru == 3) {
							ired = idata[nx * (0 * ny + iy) + ix];
							igrn = idata[nx * (1 * ny + iy) + ix];
							iblu = idata[nx * (2 * ny + iy) + ix];
						}
						tv_buf.lpbitmap[ky * (rt.right + 1) + kx].rgbBlue = iblu;
						tv_buf.lpbitmap[ky * (rt.right + 1) + kx].rgbGreen = igrn;
						tv_buf.lpbitmap[ky * (rt.right + 1) + kx].rgbRed = ired;
					}
					else if (chan == 1) { //1 byte bitmap passed
						ired = idata[1 * (iy * nx + ix) + 0];
						tv_buf.lpbitmap[ky * (rt.right + 1) + kx].rgbRed = ired;
					}
					else if (chan == 2) {
						igrn = idata[1 * (iy * nx + ix) + 0];
						tv_buf.lpbitmap[ky * (rt.right + 1) + kx].rgbGreen = igrn;
					}
					else if (chan == 3) {
						iblu = idata[1 * (iy * nx + ix) + 0];
						tv_buf.lpbitmap[ky * (rt.right + 1) + kx].rgbBlue = iblu;
					} // if (chan == 0) else
				} // if (tru == 0  && chan == 0) else
			} // for() inner (indent error)
		} // for() outer
		SetDIBitsToDevice(GetHdc(), 0, 0, rt.right + 1, rt.bottom + 1, 0, 0, 0, rt.bottom + 1, tv_buf.lpbitmap, &tv_buf.bi, DIB_RGB_COLORS);

		BringWindowToTop(GetHwnd()); UnsetFocus();
		tv_buf.has_data = true;
	}
	return true;
}

void GDLWINStream::Raise()
{
  BringWindowToTop(GetHwnd());
	return;
}
void GDLWINStream::GetGeometry(long& xSize, long& ySize) {
	// GetGeometry is called from
	//  1. 'plotting_contour.cpp' to calculate plot area,
	//  2. 'initsysvar.cpp' to update '!D'.

	// http://support.microsoft.com/en-us/kb/11570
	RECT Rect;
    HWND window = GetHwnd();
	GetClientRect(window, &Rect);
	ClientToScreen(window, (LPPOINT)&Rect.left);
	ClientToScreen(window, (LPPOINT)&Rect.right);

	xSize = Rect.right - Rect.left + 1;
	ySize = Rect.bottom - Rect.top + 1;
//	xoff = Rect.left;
//	yoff = GetSystemMetrics(SM_CYSCREEN) - Rect.bottom;
    return;
}
bool GDLWINStream::GetWindowPosition(long& xpos, long& ypos) {
	/* 
	   GET_WINDOW_POSITION (WIN, X)

	   Set this keyword to a named variable that returns a two-element array
	 containing the (X,Y) position of the lower left corner of the current window
	 on the screen. The origin is also in the lower left corner of the screen.
	*/

	RECT rt;

	GetWindowRect(GetHwnd(), &rt);
	xpos = rt.left + 1;
	ypos = GetSystemMetrics(SM_CYSCREEN) - rt.bottom + 1;

	return true;
}

void GDLWINStream::Lower()
{

  SetWindowPos(GetHwnd(), HWND_BOTTOM,
    0,0,0,0, (SWP_NOMOVE | SWP_NOSIZE));

	return;
}

void GDLWINStream::Iconic() {


  SetWindowPos(GetHwnd(), HWND_BOTTOM,
    0,0,0,0, (SWP_NOMOVE | SWP_NOSIZE |SWP_HIDEWINDOW));
  return;
}

void GDLWINStream::DeIconic() {

  SetWindowPos(GetHwnd(), HWND_BOTTOM,
    0,0,0,0, (SWP_NOMOVE | SWP_NOSIZE |SWP_SHOWWINDOW));  return;

}
void GDLWINStream::CheckValid() {
  if(!IsWindow(GetHwnd())) this->SetValid(false);
}

void GDLWINStream::Flush() {
	GdiFlush();
}

DLong GDLWINStream::GetVisualDepth(){
//  return GetDeviceCaps(GetHdc(),PLANES); // This doesn't work (returns 1)
    HDC hscreenDC = GetWindowDC(GetDesktopWindow());
    int bitsperpixel = GetDeviceCaps(hscreenDC, BITSPIXEL);
    ReleaseDC(NULL, hscreenDC);
    return bitsperpixel;
// This code is repeated in devicewin.cpp; do windo need be opened
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
      PLINT red,green,blue;
      DByte r,g,b;
      PLINT red0,green0,blue0;
      
      GraphicsDevice::GetCT()->Get(0,r,g,b);red=r;green=g;blue=b;
      
      red0=GraphicsDevice::GetDevice()->BackgroundR();
      green0=GraphicsDevice::GetDevice()->BackgroundG();
      blue0=GraphicsDevice::GetDevice()->BackgroundB();
      plstream::scolbg(red0,green0,blue0); //overwrites col[0]
	::c_plbop();
 //     ::c_plclear();
      plstream::scolbg(red,green,blue); //resets col[0]
}

HWND GDLWINStream::GetHwnd()
{

	if (pls) {
       #ifdef USE_WINGDI_NOT_WINGCC
        	wingdi_Dev *dev = (wingdi_Dev *)pls->dev;
        #else
        	wingcc_Dev* dev = (wingcc_Dev *)pls->dev;
        #endif

		if (dev)
			return dev->hwnd;
	}
    return NULL;
}
HDC GDLWINStream::GetHdc()
{

	if (pls) {
        #ifdef USE_WINGDI_NOT_WINGCC
        	wingdi_Dev *dev = (wingdi_Dev *)pls->dev;
        #else
        	wingcc_Dev* dev = (wingcc_Dev *)pls->dev;
        #endif
		if (dev)	return dev->hdc;
	}
    return NULL;
}
DByteGDL* GDLWINStream::GetBitmapData() {
    // Get the client area for size calculation
    HDC hdc=GetHdc();
    HGDIOBJ           previous;
    HBITMAP hbitmap = NULL;
    BITMAP wbitmap;
    RECT rt;
    HDC hdc_bmp = CreateCompatibleDC( hdc );
    if( ! hdc_bmp ) {
        cout << " GDLwinstream::CreateCompatibleDC() failed !! " << endl;
        return NULL;
    }

    GetClientRect( GetHwnd(), &rt);

    hbitmap = CreateCompatibleBitmap( hdc, rt.right, rt.bottom );
    if ( ! hbitmap) {
        cout << " GDLwinstream::CreateCompatibleBitmap() failed !! " << endl;
        return NULL;
    }
//    previous = SelectObject( hdc_bmp, hbitmap );
    SelectObject( hdc_bmp, hbitmap );
    if ( !BitBlt( hdc_bmp, 0, 0, rt.right, rt.bottom, hdc, 0, 0, SRCCOPY )) {
        cout << " winstream::GetBitmap() Bit blt failed !! " << endl;
        DeleteObject( hbitmap );
        DeleteDC (hdc_bmp);
        return NULL;
        }
// from example code https://docs.microsoft.com/en-us/windows/desktop/gdi/capturing-an-image

    GetObject( hbitmap, sizeof(BITMAP), &wbitmap);
    BITMAPFILEHEADER   bmfHeader;    
    BITMAPINFOHEADER   bi;
     
    bi.biSize = sizeof(BITMAPINFOHEADER);    
    bi.biWidth = wbitmap.bmWidth;    
    bi.biHeight = wbitmap.bmHeight;  
    bi.biPlanes = 1;    
    bi.biBitCount = 32;    
    bi.biCompression = BI_RGB;    
    bi.biSizeImage = 0;  
    bi.biXPelsPerMeter = 0;    
    bi.biYPelsPerMeter = 0;    
    bi.biClrUsed = 0;    
    bi.biClrImportant = 0;
    DWORD dwBmpSize = ((wbitmap.bmWidth * bi.biBitCount + 31) / 32) * 4 * wbitmap.bmHeight;
    HANDLE hDIB = GlobalAlloc(GHND,dwBmpSize); 
    char *lpbitmap = (char *)GlobalLock(hDIB);    

    // Gets the "bits" from the bitmap and copies them into a buffer 
    // which is pointed to by lpbitmap.
    GetDIBits(hdc_bmp, hbitmap, 0,
        (UINT)wbitmap.bmHeight,
        lpbitmap,
        (BITMAPINFO *)&bi, DIB_RGB_COLORS);

// Now convert the data in bitmap to bytes for GDL.
    int nx, ny;
    nx = rt.right;
    ny = rt.bottom;
//    cout << " nx="<<nx<<" ny="<<ny<<" rank:";
    SizeT datadims[3];
    datadims[0] = rt.right; //nx;
    datadims[1] = rt.bottom; //ny;
    datadims[2] = 3;
    dimension datadim(datadims, (SizeT) 3);
    DByteGDL *bitmap = new DByteGDL( datadim, BaseGDL::NOZERO);
    cout << static_cast<BaseGDL*>(bitmap)->Rank() << endl;
    SizeT kpad = 0;
    for ( SizeT iy = 0; iy < ny ; ++iy ) {
      for ( SizeT ix = 0; ix < nx ; ++ix ) {
        (*bitmap)[3 * ((ny-1-iy) * nx + ix) + 2] = lpbitmap[kpad++];
        (*bitmap)[3 * ((ny-1-iy) * nx + ix) + 1] = lpbitmap[kpad++];
        (*bitmap)[3 * ((ny-1-iy) * nx + ix) + 0] = lpbitmap[kpad++];
        kpad++; //pad to 4
      }
    }
    //Unlock and Free the DIB from the heap
    GlobalUnlock(hDIB);    
    GlobalFree(hDIB);
    DeleteObject( hbitmap );
    DeleteDC( hdc_bmp );
    
    return bitmap;

 }
void GDLWINStream::RedrawTV()
{
	RECT rt;
    HWND tvwnd = GetHwnd();

    // If tv_buf has data, draw it on screen
	if (tv_buf.has_data && tvwnd)
	{
	    GetClientRect(tvwnd, &rt);
		if (tv_buf.bi.bmiHeader.biWidth != rt.right + 1 || -tv_buf.bi.bmiHeader.biHeight != rt.bottom + 1)
		{
			// Resize tv_buf.lpbitmap
			RGBQUAD* lpbitmap = new RGBQUAD[(rt.right + 1) * (rt.bottom + 1)];
			memset(lpbitmap, 0, sizeof(RGBQUAD) * (rt.right + 1) * (rt.bottom + 1));
			for (SizeT ix = 0; ix < min(tv_buf.bi.bmiHeader.biWidth, rt.right + 1); ix++)
				for (SizeT iy = 0; iy < min(-tv_buf.bi.bmiHeader.biHeight, rt.bottom + 1); iy++)
					lpbitmap[iy*(rt.right + 1) + ix] = tv_buf.lpbitmap[iy*tv_buf.bi.bmiHeader.biWidth + ix];
			delete[] tv_buf.lpbitmap;
			tv_buf.lpbitmap = lpbitmap;
			tv_buf.bi.bmiHeader.biWidth = rt.right + 1;
			tv_buf.bi.bmiHeader.biHeight = -(rt.bottom + 1);
		}
		SetDIBitsToDevice(GetHdc(), 0, 0, rt.right + 1, rt.bottom + 1, 0, 0, 0, rt.bottom + 1, tv_buf.lpbitmap, &tv_buf.bi, DIB_RGB_COLORS);
	}
}
