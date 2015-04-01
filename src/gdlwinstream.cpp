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

void GDLWINStream::Init()
{
	this->plstream::init();
	plgpls(&pls);
	wingcc_Dev* dev = (wingcc_Dev *)pls->dev;
	dev->waiting = 1;
	UnsetFocus();
}

void GDLWINStream::SetWindowTitle(char* buf) {
	wchar_t wbuf[256] = { 0, };
	MultiByteToWideChar(CP_UTF8, 0, buf, -1, wbuf, 256);
	wingcc_Dev* dev = (wingcc_Dev *)pls->dev;

	SetWindowTextW(dev->hwnd, wbuf);
}

void GDLWINStream::EventHandler()
{
	if (!valid) return;

	if (pls->dev == NULL) {
		cerr << "Invalid window." << endl;
		valid = false;
		return;
	}

	MSG Message;
	if (PeekMessage(&Message, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&Message);
		if (Message.message == WM_DESTROY) {
			valid = false;
			return;
		} else DispatchMessage(&Message);
	}

	// plplot event handler
	plstream::cmd(PLESC_EH, NULL);
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
	wingcc_Dev *dev = (wingcc_Dev *)pls->dev;

	HCURSOR    cursor;
	HCURSOR    previous;
	RECT rcClient;
	//RECT rcOldClip;
	POINT Point;
	UINT SWP = (SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	HWND resetFG;
	resetFG = GetForegroundWindow();

	HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(NULL);
	// Create a custom cursor at run time. 
	cursor = CreateCursor(hInstance,   // app. instance 
		15,                // horizontal position of hot spot 
		14,                // vertical position of hot spot 
		32,                // cursor width 
		32,                // cursor height
		ANDmaskCursor,     // AND mask 
		XORmaskCursor);   // XOR mask 
#ifdef _WIN64
	SetClassLongPtr( dev->hwnd, GCLP_HCURSOR, (LONG_PTR) cursor );
#else
	SetClassLong(dev->hwnd, GCLP_HCURSOR, (LONG)cursor);
#endif
	//   SetClassLongPtr( dev->hwnd, GCLP_HCURSOR, (LONG_PTR) dev->cursor );
	previous = SetCursor(cursor);

	SWP = (SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	SetWindowPos(dev->hwnd, HWND_TOP, 0, 0, 0, 0, SWP);

	bool buttonpressed = false;
	//   NOWAIT = 0,    WAIT, //1    CHANGE, //2    DOWN, //3    UP //4
	// http://msdn.microsoft.com/en-us/library/windows/desktop/ms645602(v=vs.85).aspx
	bool rbutton, xbutton, mbutton;
	gin->button = 0;
	while (!buttonpressed)
	{
		GetClientRect(dev->hwnd, &rcClient); // https://msdn.microsoft.com/library/windows/desktop/ms633503%28v=vs.85%29.aspx
		ClientToScreen(dev->hwnd, (LPPOINT)&rcClient.left); // http://support.microsoft.com/en-us/kb/11570
		ClientToScreen(dev->hwnd, (LPPOINT)&rcClient.right);

		rbutton = false; xbutton = false; mbutton = false;
		GetMessage(&dev->msg, NULL, 0, 0);
		TranslateMessage(&dev->msg);
		if (GetForegroundWindow() == dev->hwnd) {
			switch ((int)dev->msg.message)
			{
			case WM_XBUTTONDOWN:  xbutton = true;
			case WM_RBUTTONDOWN:  rbutton = true;
			case WM_MBUTTONDOWN:  mbutton = true;
			case WM_LBUTTONDOWN:
				if (mode == 4) break;   // Looking for button up
				gin->button = 1;
				GetCursorPos(&Point);
				buttonpressed = true;
				break;
			case WM_XBUTTONUP:	xbutton = true;
			case WM_RBUTTONUP:	rbutton = true;
			case WM_MBUTTONUP:  mbutton = true;
			case WM_LBUTTONUP:
				if (mode == 3) break;  // Looking for button down
				gin->button = 1;
				GetCursorPos(&Point);
				buttonpressed = true;
				break;
			case WM_CHAR:
				GetCursorPos(&Point);
				gin->keysym = dev->msg.wParam;
				buttonpressed = true;
				break;
			default:
				if (mode == 0) {
					GetCursorPos(&Point);
					buttonpressed = true;
				}
			}
			if (!buttonpressed || !rcClient.left > Point.x || Point.x > rcClient.right || rcClient.top > Point.y || Point.y > rcClient.bottom) {
				DispatchMessage(&dev->msg);
				buttonpressed = false;
			}
		}
	}
	ScreenToClient(dev->hwnd, &Point); // https://msdn.microsoft.com/library/windows/desktop/dd162952%28v=vs.85%29.aspx

	gin->pX = Point.x;
	gin->pY = (rcClient.bottom - rcClient.top) - Point.y;
	if (xbutton) gin->button = 4; else
		if (rbutton) gin->button = 3; else
			if (mbutton) gin->button = 2;
	gin->dX = ((PLFLT)gin->pX) / (rcClient.right - rcClient.left - 1);
	gin->dY = ((PLFLT)gin->pY) / (rcClient.bottom - rcClient.top - 1);

	SetForegroundWindow(resetFG);
	SetCursor(previous);

	Sleep(2);
	BringWindowToTop(dev->hwnd);
	Sleep(2);
	SetFocus(resetFG);

	return true;
}

bool GDLWINStream::PaintImage(unsigned char *idata, PLINT nx, PLINT ny,
	DLong *pos, DLong tru, DLong chan)
{
	plstream::cmd(PLESC_FLUSH, NULL);

	wingcc_Dev *dev = (wingcc_Dev *)pls->dev;
	HDC hdc = dev->hdc;

	PLINT xoff = (PLINT)pos[0];
	PLINT yoff = (PLINT)pos[2];
	PLINT kx, ky;

	PLINT xsize = pls->phyxma;
	PLINT ysize = pls->phyyma;

	PLINT kxLimit = xsize - xoff;
	PLINT kyLimit = ysize - yoff;

	HBITMAP hbitmap;
	BITMAPINFO bi = { 0 };

	RECT rt;

	if (nx < kxLimit) kxLimit = nx;
	if (ny < kyLimit) kyLimit = ny;

	hbitmap = CreateCompatibleBitmap(hdc, kxLimit, kyLimit);

	if (nx > 0 && ny > 0) {
		unsigned char iclr1, ired, igrn, iblu;
		long curcolor;

		bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bi.bmiHeader.biWidth = kxLimit;
		bi.bmiHeader.biHeight = kyLimit;
		bi.bmiHeader.biPlanes = 1;
		bi.bmiHeader.biBitCount = 24;
		bi.bmiHeader.biCompression = BI_RGB;
		unsigned long ulBmpSize = kxLimit * kyLimit;
		RGBTRIPLE *lpbitmap = new RGBTRIPLE[ulBmpSize];
		GetDIBits(hdc, hbitmap, 0, kyLimit, lpbitmap, &bi, DIB_RGB_COLORS);
		for (SizeT ix = 0; ix < kxLimit; ++ix) {
			for (SizeT iy = 0; iy < kyLimit; ++iy) {
				if (tru == 0 && chan == 0) {
					iclr1 = idata[iy * nx + ix];
					//curcolor = RGB( pls->cmap1[iclr1].r, pls->cmap1[iclr1].g, pls->cmap1[iclr1].b );
					lpbitmap[iy*kxLimit + ix].rgbtBlue = pls->cmap1[iclr1].b;
					lpbitmap[iy*kxLimit + ix].rgbtGreen = pls->cmap1[iclr1].g;
					lpbitmap[iy*kxLimit + ix].rgbtRed = pls->cmap1[iclr1].r;
					//	 			 printf("ix: %d  iy: %d  pixel: %d\n", ix,iy,curcolor.pixel);

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
						lpbitmap[iy*kxLimit + ix].rgbtBlue = iblu;
						lpbitmap[iy*kxLimit + ix].rgbtGreen = igrn;
						lpbitmap[iy*kxLimit + ix].rgbtRed = ired;
					}
					else if (chan == 1) {
						ired = idata[1 * (iy * nx + ix) + 0];
						lpbitmap[iy*kxLimit + ix].rgbtRed = ired;
					}
					else if (chan == 2) {
						igrn = idata[1 * (iy * nx + ix) + 1];
						lpbitmap[iy*kxLimit + ix].rgbtGreen = igrn;
					}
					else if (chan == 3) {
						iblu = idata[1 * (iy * nx + ix) + 2];
						lpbitmap[iy*kxLimit + ix].rgbtBlue = iblu;
					} // if (chan == 0) else
				} // if (tru == 0  && chan == 0) else
			} // for() inner (indent error)
		} // for() outer

		GetClientRect(dev->hwnd, &rt);
		SetDIBitsToDevice(hdc, xoff, (rt.bottom - rt.top) - kyLimit - yoff, kxLimit, kyLimit, 0, 0, 0, kyLimit, lpbitmap, &bi, DIB_RGB_COLORS);
		delete[] lpbitmap;
	}
	DeleteObject(hbitmap);
	return true;
}

void GDLWINStream::Raise()
{
	wingcc_Dev *dev = (wingcc_Dev *)pls->dev;
	BringWindowToTop(dev->hwnd);
	return;
}
void GDLWINStream::GetGeometry(long& xSize, long& ySize, long& xoff, long& yoff) {
	// GetGeometry is called from
	//  1. 'plotting_contour.cpp' to calculate plot area,
	//  2. 'initsysvar.cpp' to update '!D'.
	wingcc_Dev *dev = (wingcc_Dev *)pls->dev;

	// http://support.microsoft.com/en-us/kb/11570
	RECT Rect;
	GetClientRect(dev->hwnd, &Rect);
	ClientToScreen(dev->hwnd, (LPPOINT)&Rect.left);
	ClientToScreen(dev->hwnd, (LPPOINT)&Rect.right);

	xSize = Rect.right - Rect.left;
	ySize = Rect.bottom - Rect.top;
	xoff = Rect.left;
	yoff = GetSystemMetrics(SM_CYSCREEN) - Rect.bottom;
}
bool GDLWINStream::GetWindowPosition(long& xpos, long& ypos) {
	/* 
	   GET_WINDOW_POSITION (WIN, X)

	   Set this keyword to a named variable that returns a two-element array
	 containing the (X,Y) position of the lower left corner of the current window
	 on the screen. The origin is also in the lower left corner of the screen.
	*/
	wingcc_Dev *dev = (wingcc_Dev *)pls->dev;

	GetWindowInfo(dev->hwnd, &Winfo);
	xpos = Winfo.rcWindow.left;
	ypos = GetSystemMetrics(SM_CYSCREEN) - Winfo.rcWindow.bottom;
	return true;
}

void GDLWINStream::Lower()
{
	wingcc_Dev *dev = (wingcc_Dev *)pls->dev;
	UINT SWP = (SWP_NOMOVE | SWP_NOSIZE);
	SetWindowPos(dev->hwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP);
	return;
}

void GDLWINStream::Iconic() {
	wingcc_Dev *dev = (wingcc_Dev *)pls->dev;
	UINT SWP = (SWP_NOMOVE | SWP_NOSIZE | SWP_HIDEWINDOW);  // this does nothinc
	SetWindowPos(dev->hwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP);
	return;
}

void GDLWINStream::DeIconic() {
	wingcc_Dev *dev = (wingcc_Dev *)pls->dev;
	UINT SWP = (SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	SetWindowPos(dev->hwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP);
	return;

}
void GDLWINStream::CheckValid() {
	wingcc_Dev *dev = (wingcc_Dev *)pls->dev;
	if (!IsWindow(dev->hwnd)) this->SetValid(false);
}

void GDLWINStream::Flush() {
	GdiFlush();
}

DLong GDLWINStream::GetVisualDepth(){
	wingcc_Dev *dev = (wingcc_Dev *)pls->dev;
	return GetDeviceCaps(dev->hdc, PLANES);
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
