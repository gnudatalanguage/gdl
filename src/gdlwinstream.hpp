/* *************************************************************************
                          gdlwinstream.hpp  -  graphic stream M$ windows
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
/** 
  \file gdlwinstream.hpp
  \brief graphic stream M$ windows
  \struc wingcc_Dev
  \class GDLWINStream
*/

#ifndef GDLWINSTREAM_HPP_
#define GDLWINSTREAM_HPP_

#include "gdlgstream.hpp"
#include <Windows.h>
#include <map>

// Copied from wingcc.c
// Struct to hold device-specific info.

struct wingcc_Dev
{
	PLFLT scale;                     // scaling factor to "blow up" to the "virtual" page in removing hidden lines
	PLINT width;                     // Window width (which can change)
	PLINT height;                    // Window Height

	PLFLT PRNT_scale;
	PLINT PRNT_width;
	PLINT PRNT_height;

	char  FT_smooth_text;
	//
	// WIN32 API variables
	//

	COLORREF          colour;                       // Current Colour
	COLORREF          oldcolour;                    // Used for high-speed background erasing
	MSG               msg;                          // A Win32 message structure.
	WNDCLASSEX        wndclass;                     // An extended window class structure.
	HWND              hwnd;                         // Handle for the main window.
	HPEN              pen;                          // Windows pen used for drawing
	HDC               hdc;                          // Driver Context
	HDC               hdc2;                         // Driver Context II - used for Blitting
	HDC               SCRN_hdc;                     // The screen's context
	HDC               PRNT_hdc;                     // used for printing
	PAINTSTRUCT       ps;                           // used to paint the client area of a window owned by that application
	RECT              rect;                         // defines the coordinates of the upper-left and lower-right corners of a rectangle
	RECT              oldrect;                      // used for re-sizing comparisons
	RECT              paintrect;                    // used for painting etc...
	HBRUSH            fillbrush;                    // brush used for fills
	HCURSOR           cursor;                       // Current windows cursor for this window
	HBITMAP           bitmap;                       // Bitmap of current display; used for fast redraws via blitting
	HGDIOBJ           oldobject;                    // Used for tracking objects probably not really needed but
	HMENU             PopupMenu;

	PLINT             draw_mode;
	char              truecolour;      // Flag to indicate 24 bit mode
	char              waiting;         // Flag to indicate drawing is done, and it is waiting;
	// we only do a windows redraw if plplot is plotting
	char              enterresize;     // Used to keep track of reszing messages from windows
	char              already_erased;  // Used to track first and only first backgroudn erases
	struct wingcc_Dev  *push;
};

typedef struct {
	BITMAPINFO bi;
	RGBQUAD *lpbitmap;
	bool has_data;
} tv_buf_t;

class GDLWINStream : public GDLGStream
{
	//Atom wm_protocols;
	//Atom wm_delete_window;
	HWND refocus;

	PLStream* pls;
	plstream *plst;

	tv_buf_t tv_buf;
    int _mode;
    PLGraphicsIn *_gin;
    POINT GinPoint;
    bool rbutton, xbutton, mbutton, buttonpressed = false;
public:
    std::map<UINT, void (CALLBACK GDLWINStream::*)(UINT, WPARAM, LPARAM)> msghooks;

	GDLWINStream(int nx, int ny) :
		GDLGStream(nx, ny, "wingcc")
	{
		pls = 0;
		// get the command interpreter window's handle
		refocus = GetForegroundWindow();
	}

	~GDLWINStream();
	void Init();
	void EventHandler();

	//	  static int   GetImageErrorHandler(Display *display, XErrorEvent *error);

	void GetGeometry(long& xSize, long& ySize, long& xoff, long& yoff);
	bool GetGin(PLGraphicsIn *gin, int mode);

	unsigned long GetWindowDepth();
	DLong GetVisualDepth();
	//  DString GetVisualName();

	bool UnsetFocus();
	//  bool SetBackingStore(int value);
	//  bool SetGraphicsFunction(long value );
	bool GetWindowPosition(long& xpos, long& ypos);
	void Clear();
	void Raise();
	void Lower();
	void Iconic();
	void DeIconic();
	void Flush();
	void CheckValid();
	bool PaintImage(unsigned char *idata, PLINT nx, PLINT ny, DLong *pos, DLong tru, DLong chan);

	//  bool SetGraphicsFunction(long value );
	virtual bool HasCrossHair() { return true; }

	void SetWindowTitle(char* buf);
	HWND GetHwnd(void);
	void RedrawTV();

    void CALLBACK GinCallback(UINT message, WPARAM wParam, LPARAM lParam);
};

typedef std::map<UINT, void (CALLBACK GDLWINStream::*)(UINT, WPARAM, LPARAM)>::iterator msghookiter;

#endif
