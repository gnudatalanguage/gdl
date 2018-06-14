/* *************************************************************************
                          devicewin.cpp  -  win32 gdi interface
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

#ifdef _WIN32

#include "includefirst.hpp"
#include <iostream>

#include "graphicsdevice.hpp"
#include "gdlwinstream.hpp"
#include "devicewin.hpp"

#ifdef HAVE_LIBWXWIDGETS
#include "gdlwxstream.hpp"
#endif

#include "plotting.hpp"

// JP Apr 2015, HACK: hook WM_PAINT and WM_DESTROY from plplot window.
HHOOK hHook[2] = {0};
DeviceWIN *p_this;

LRESULT CALLBACK DeviceWIN::_CallWndProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	return p_this->CallWndProc(nCode, wParam, lParam);
}

LRESULT CALLBACK DeviceWIN::_GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	return p_this->GetMsgProc(nCode, wParam, lParam);
}


LRESULT DeviceWIN::CallWndProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	CWPSTRUCT* lpWp = (CWPSTRUCT*)lParam;
	int i;
	BOOL windowfound = false;
	if (nCode >= 0) {
		for (i = 0; i < winList.size(); i++) {
			if (winList[i] && winList[i]->GetValid() && lpWp->hwnd == ((GDLWINStream *)winList[i])->GetHwnd()) {
				windowfound = true;
				break;
			}
		}
		if (windowfound) {
			switch (lpWp->message)
			{
				case WM_PAINT:
				{
					// Redraw image while resizing/moving/etc..
					PAINTSTRUCT ps;
					BeginPaint(lpWp->hwnd, &ps);
					((GDLWINStream *)winList[i])->RedrawTV();
					EndPaint(lpWp->hwnd, &ps);
					break;
				}
				case WM_DESTROY:
				{
					winList[i]->SetValid(false);
					break;
				}
			}
		}
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT DeviceWIN::GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	LRESULT retCode = CallNextHookEx(NULL, nCode, wParam, lParam);

	MSG* lpWp = (MSG*)lParam;
	int i;
	BOOL windowfound = false;
	if (nCode >= 0) {
		for (i = 0; i < winList.size(); i++) {
			if (winList[i] && winList[i]->GetValid() && lpWp->hwnd == ((GDLWINStream *)winList[i])->GetHwnd()) {
				windowfound = true;
				break;
			}
		}
        if (windowfound) {
            if (lpWp->message == WM_PAINT) {
		    	// Redraw image after the window is finally reactivated.
			    //PAINTSTRUCT ps;
			    //BeginPaint(lpWp->hwnd, &ps);
			    ((GDLWINStream *)winList[i])->RedrawTV();
			    //EndPaint(lpWp->hwnd, &ps);
		    }
            msghookiter iter_msg;
            GDLWINStream* winstream = (GDLWINStream *)winList[i];
            for (iter_msg = winstream->msghooks.begin(); iter_msg != winstream->msghooks.end(); ++iter_msg)
            {
                if ((*iter_msg).first == lpWp->message)
                    (winstream->*(*iter_msg).second)(lpWp->message, lpWp->wParam, lpWp->lParam);
            }
        }
	}
	return retCode;
}
// HACK end

void DeviceWIN::EventHandler()
{
	if (actWin < 0) return;
	int wLSize = winList.size();
	for (int i = 0; i < wLSize; i++) {
		if (winList[i] != NULL)	winList[i]->EventHandler();
	}
	TidyWindowsList();
}

bool DeviceWIN::WDelete(int wIx)
{
	TidyWindowsList();

	int wLSize = winList.size();
	if (wIx >= wLSize || wIx < 0 || winList[wIx] == NULL)
		return false;

#ifdef HAVE_LIBWXWIDGETS
	if (dynamic_cast<GDLWXStream*>(winList[wIx]) != NULL)
	{
		Warning("Attempt to delete widget (ID=" + i2s(wIx) +
			"). Will be auto-deleted upon window destruction.");
		return false;
	}
#endif    

	delete winList[wIx];
	winList[wIx] = NULL;
	oList[wIx] = 0;

	// set to most recently created
	std::vector< long>::iterator mEl =
		std::max_element(oList.begin(), oList.end());

	// no window open
	if (*mEl == 0)
	{
		SetActWin(-1);
		oIx = 1;
		if (hHook[0]) {
			UnhookWindowsHookEx(hHook[0]);
			hHook[0] = 0;
		}
		if (hHook[1]) {
			UnhookWindowsHookEx(hHook[1]);
			hHook[1] = 0;
		}
	}
	else
		SetActWin(std::distance(oList.begin(), mEl));
	return true;
}

#ifdef HAVE_LIBWXWIDGETS
bool DeviceWIN::GUIOpen(int wIx, int xSize, int ySize)
{

	TidyWindowsList();

	int wLSize = winList.size();
	if (wIx >= wLSize || wIx < 0)
		return false;

	if (winList[wIx] != NULL)
	{
		delete winList[wIx];
		winList[wIx] = NULL;
	}

	winList[wIx] = new GDLWINStream(xSize, ySize);

	// no pause on win destruction
	winList[wIx]->spause(false);

	// extended fonts
	winList[wIx]->fontld(1);

	// we want color
	winList[wIx]->scolor(1);

	PLINT r[ctSize], g[ctSize], b[ctSize];
	actCT.Get(r, g, b);
	winList[wIx]->SetColorMap0(r, g, b, ctSize); //set colormap 0 to 256 values

	// need to be called initially. permit to fix things
	winList[wIx]->ssub(1, 1);
	winList[wIx]->adv(0);
	// load font
	winList[wIx]->font(1);
	winList[wIx]->vpor(0, 1, 0, 1);
	winList[wIx]->wind(0, 1, 0, 1);
	winList[wIx]->DefaultCharSize();
	//in case these are not initalized, here is a good place to do it.
	if (winList[wIx]->updatePageInfo() == true)
	{
		winList[wIx]->GetPlplotDefaultCharSize(); //initializes everything in fact..

	}
	// sets actWin and updates !D
	SetActWin(wIx);

	return true; //winList[ wIx]->Valid(); // Valid() need to called once
} // GUIOpen
#endif


bool DeviceWIN::WOpen(int wIx, const std::string& title,
	int xSize, int ySize, int xPos, int yPos, bool hide)
{

	int debug = 0;
	if (debug) cout << " DeviceWIN::WOpen : xsize=" << xSize << " y:" << ySize
		<< " Xposition=" << xPos << " Y:" << yPos << endl;
	TidyWindowsList();

	int wLSize = winList.size();
	if (wIx >= wLSize || wIx < 0)
		return false;

	if (winList[wIx] != NULL)
	{
		delete winList[wIx];
		winList[wIx] = NULL;
	}

	DLongGDL* pMulti = SysVar::GetPMulti();
	DLong nx = (*pMulti)[1];
	DLong ny = (*pMulti)[2];

	if (nx <= 0) nx = 1;
	if (ny <= 0) ny = 1;

	winList[wIx] = new GDLWINStream(nx, ny);

	// as wxwidgets never set this, they can be intermixed
	oList[wIx] = oIx++;

	// set initial window size
	PLFLT xp; PLFLT yp;
	PLINT xleng; PLINT yleng;
	PLINT xoff; PLINT yoff;
	winList[wIx]->plstream::gpage(xp, yp, xleng, yleng, xoff, yoff);

	if (debug) cout << "WOpen.gpage xp=" << xp << ", yp=" << yp
		<< ", xleng=" << xleng << ", yleng=" << yleng
		<< ", xoff=" << xoff << ", yoff=" << yoff << endl;

	
	DLong xMaxSize, yMaxSize;
	//DeviceWIN::MaxXYSize(&xMaxSize, &yMaxSize);
	RECT rt;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rt, 0);
	xMaxSize = rt.right; yMaxSize = rt.bottom;

	bool noPosx = (xPos == -1);
	bool noPosy = (yPos == -1);
	xPos = max(1, xPos); //starts at 1 to avoid problems plplot!
	yPos = max(1, yPos);

	xleng = min(xSize, xMaxSize);
	yleng = min(ySize, yMaxSize);

	rt.left = 0; rt.top = 0; rt.right = xleng; rt.bottom = yleng;
	AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, false);

	xleng = rt.right - rt.left;
	yleng = rt.bottom - rt.top;

	if (xPos + xleng > xMaxSize) xPos = xMaxSize - xleng - 1;
	if (yPos + yleng > yMaxSize) yPos = yMaxSize - yleng - 1;

	// dynamic allocation needed!
	PLINT Quadx[4] = { xMaxSize - xleng - 1, xMaxSize - xleng - 1, 1, 1 };
	PLINT Quady[4] = { 1, yMaxSize - yleng - 1, 1, yMaxSize - yleng - 1 };

	if (noPosx && noPosy) { //no init given, use 4 quadrants:
		xoff = Quadx[wIx % 4]; yoff = Quady[wIx % 4];
	}
	else if (noPosx) {
		xoff = Quadx[wIx % 4]; yoff = yMaxSize - yPos - yleng;
	}
	else if (noPosy) {
		xoff = xPos; yoff = Quady[wIx % 4];
	}
	else {
		xoff = xPos; yoff = yMaxSize - yPos - yleng;
	}
	winList[wIx]->spage(xp, yp, xleng, yleng, xoff, yoff);

	// no pause on win destruction
	winList[wIx]->spause(false);
	if (debug) cout << " WOpen: ->fontld( 1) WOpen: ->scolor( ";

	// extended fonts
	winList[wIx]->fontld(1);

	// we want color
	winList[wIx]->scolor(1);

	// window title
	static char buf[256];
	strncpy(buf, title.c_str(), 255);
	buf[255] = 0;
	if (debug) cout << "1) WOpen: ->SETOPT(plwindow,buf)";
	winList[wIx]->SETOPT("plwindow", buf);

	// we want color (and the driver options need to be overwritten)
	// winList[ wIx]->SETOPT( "drvopt","color=1");
	// ---- alternate SETOPT in devicex.hpp
	// set color map
	PLINT r[256], g[256], b[256];
	actCT.Get(r, g, b);
	// winList[ wIx]->scmap0( r, g, b, actCT.size());
	winList[wIx]->SetColorMap1(r, g, b, ctSize);

	if (debug) cout << "; WOpen:winList[ wIx]->Init(";

	winList[wIx]->Init();
	//          if(debug) cout << " ) WOpen:winList[ wIx]->ssub(1,1)" 
	//                        << "adv(0) font(1) vpor(0,1,0,1) wind(0,1,0,1";
	//----------------------
	//----------------------
	// need to be called initially. permit to fix things
	winList[wIx]->ssub(1, 1);
	winList[wIx]->adv(0);
	// load font
	winList[wIx]->font(1);
	winList[wIx]->vpor(0, 1, 0, 1);
	winList[wIx]->wind(0, 1, 0, 1);
	winList[wIx]->DefaultCharSize();
	//in case these are not initalized, here is a good place to do it.
	if (debug) cout << ")... DefaultCharSize(); " << endl;
	if (winList[wIx]->updatePageInfo() == true)
	{
		winList[wIx]->GetPlplotDefaultCharSize(); //initializes everything in fact..

	}
	// sets actWin and updates !D
	SetActWin(wIx);

	// Currently Plplot ignores to update window title on Windows. it should be done manually..
	((GDLWINStream *)winList[wIx])->SetWindowTitle(buf);

	// HACK: setup hook for redrawing/validating windows
	p_this = this;
	if (!hHook[0])
		hHook[0] = SetWindowsHookEx(WH_CALLWNDPROC, (HOOKPROC)_CallWndProc, NULL, GetCurrentThreadId());
	if (!hHook[1])
		hHook[1] = SetWindowsHookEx(WH_GETMESSAGE, (HOOKPROC)_GetMsgProc, NULL, GetCurrentThreadId());
	// HACK end

	return true; //winList[ wIx]->Valid(); // Valid() need to called once
}

bool DeviceWIN::WState(int wIx)
{
	return wIx >= 0 && wIx < oList.size() && oList[wIx] != 0;
}

bool DeviceWIN::WSize(int wIx, int *xSize, int *ySize)
{
	TidyWindowsList();

	int wLSize = winList.size();
	if (wIx > wLSize || wIx < 0)
		return false;

	long xleng, yleng;
	winList[wIx]->GetGeometry(xleng, yleng);

	*xSize = xleng;
	*ySize = yleng;

	return true;
}

bool DeviceWIN::WSet(int wIx)
{
	TidyWindowsList();

	int wLSize = winList.size();
	if (wIx >= wLSize || wIx < 0 || winList[wIx] == NULL)
		return false;

	SetActWin(wIx);
	return true;
}

bool DeviceWIN::WShow(int ix, bool show, int iconic)
{
	TidyWindowsList();

	int wLSize = winList.size();
	if (ix >= wLSize || ix < 0 || winList[ix] == NULL) return false;

  if (iconic!=-1) { //iconic asked. do nothing else.
		if (iconic==1) IconicWin(ix); else DeIconicWin(ix);
	} else {
		if (show) RaiseWin(ix);  else LowerWin(ix);
  }
	UnsetFocus();

	return true;
}

int DeviceWIN::WAdd()
{
	TidyWindowsList();

	int wLSize = winList.size();
	for (int i = 0; i < wLSize; i++)
		if (winList[i] == NULL) return i;

	// plplot allows only 101 windows
	if (wLSize == 101) return -1;

	winList.push_back(NULL);
	oList.push_back(0);
	return wLSize;
}




DIntGDL* 	DeviceWIN::GetScreenSize(char *disp)
{
	DLong xsize, ysize;
	MaxXYSize(&xsize, &ysize);
	DIntGDL* res;
	res = new DIntGDL(2, BaseGDL::NOZERO);
	(*res)[0] = xsize;
	(*res)[1] = ysize;
	return res;
}


DDoubleGDL* DeviceWIN::GetScreenResolution(char* disp)
{

	HDC hscreenDC = GetWindowDC(GetDesktopWindow());
	int screen_width, screen_height;
	int screen_width_mm, screen_height_mm;
	screen_width = GetDeviceCaps(hscreenDC, HORZRES);
	screen_width_mm = GetDeviceCaps(hscreenDC, HORZSIZE);
	screen_height = GetDeviceCaps(hscreenDC, VERTRES);
	screen_height_mm = GetDeviceCaps(hscreenDC, VERTSIZE);

	ReleaseDC(GetDesktopWindow(), hscreenDC);

	DDoubleGDL* resolution;
	resolution = new DDoubleGDL(2, BaseGDL::NOZERO);
	(*resolution)[0] = (screen_width_mm / 10.) / screen_width;
	(*resolution)[1] = (screen_height_mm / 10.) / screen_height;
	return resolution;
}

DIntGDL* DeviceWIN::GetWindowPosition()
{
	TidyWindowsList();
	this->GetStream(); //to open a window if none opened.
	long xpos, ypos;
	if (winList[actWin]->GetWindowPosition(xpos, ypos)) {
		DIntGDL* res;
		res = new DIntGDL(2, BaseGDL::NOZERO);
		(*res)[0] = xpos;
		(*res)[1] = ypos;
		return res;
	}
	else return NULL;
}

DLong DeviceWIN::GetVisualDepth()
{
	TidyWindowsList();
	this->GetStream(); //to open a window if none opened.
	return winList[actWin]->GetVisualDepth();
}

DString DeviceWIN::GetVisualName()
{
	TidyWindowsList();
	this->GetStream(); //to open a window if none opened.
	return winList[actWin]->GetVisualName();
}
//This function is reserved to device Z, should not exist for WIN!
//DLong DeviceWIN::GetPixelDepth()
//{
//	HDC hscreenDC = GetWindowDC(GetDesktopWindow());
//	int bitsperpixel = GetDeviceCaps(hscreenDC, BITSPIXEL);
//	ReleaseDC(NULL, hscreenDC);
//	return bitsperpixel;
//}

DByteGDL* DeviceWIN::WindowState()
{
	int maxwin = MaxWin();
	if (maxwin > 0){
		DByteGDL* ret = new DByteGDL(dimension(maxwin), BaseGDL::NOZERO);
		for (int i = 0; i < maxwin; i++) (*ret)[i] = WState(i);
		return ret;
	}
	else return NULL;
}

bool DeviceWIN::UnsetFocus()
{
	if (actWin == -1) { return false; }
	return winList[actWin]->UnsetFocus();
}

int DeviceWIN::MaxWin() { TidyWindowsList(); return winList.size(); }
int DeviceWIN::ActWin() { TidyWindowsList(); return actWin; }

void DeviceWIN::DefaultXYSize(DLong *xSize, DLong  *ySize)
{
	*xSize = 680; *ySize = 480;
	DLong XS, YS;
	MaxXYSize(&XS, &YS);
	*ySize = YS / 2; *xSize = XS / 2;
	if (*xSize > 1.5*(*ySize)) *xSize = floor(float(*ySize)*.1375) * 10;
	bool noQscreen = true;
	string gdlQscreen = GetEnvString("GDL_GR_WIN_QSCREEN");
	if (gdlQscreen == "1") noQscreen = false;
	string gdlXsize = GetEnvString("GDL_GR_WIN_WIDTH");
	if (gdlXsize != "" && noQscreen) *xSize = atoi(gdlXsize.c_str());
	string gdlYsize = GetEnvString("GDL_GR_WIN_HEIGHT");
	if (gdlYsize != "" && noQscreen) *ySize = atoi(gdlYsize.c_str());

	return;
}

void DeviceWIN::MaxXYSize(DLong *xSize, DLong *ySize)
{
	*xSize = GetSystemMetrics(SM_CXSCREEN);
	*ySize = GetSystemMetrics(SM_CYSCREEN);
}

void DeviceWIN::TidyWindowsList() {
	int wLSize = winList.size();

	for (int i = 0; i < wLSize; i++) {
		if (winList[i] != NULL && !winList[i]->GetValid()) {
			delete winList[i];
			winList[i] = NULL;	    oList[i] = 0;
		}
	}
	// set new actWin IF NOT VALID ANY MORE
	if (actWin < 0 || actWin >= wLSize ||
		winList[actWin] == NULL ||
		!winList[actWin]->GetValid())      {
		// set to most recently created
		std::vector< long>::iterator mEl =
			std::max_element(oList.begin(), oList.end());

		// no window open
		if (*mEl == 0)  {
			SetActWin(-1);	    oIx = 1;
			if (hHook[0]) {
				UnhookWindowsHookEx(hHook[0]);
				hHook[0] = 0;
			}
			if (hHook[1]) {
				UnhookWindowsHookEx(hHook[1]);
				hHook[1] = 0;
			}
		}
		else {
			SetActWin(std::distance(oList.begin(), mEl));
		}
	}
}

#endif
