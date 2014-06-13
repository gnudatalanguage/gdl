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

#ifndef GDLWINSTREAM_HPP_
#define GDLWINSTREAM_HPP_

#include "gdlgstream.hpp"
#include <Windows.h>

// Copied from wingcc.c
// Struct to hold device-specific info.

typedef struct
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
} wingcc_Dev;

class GDLWINStream : public GDLGStream
{
	//Atom wm_protocols;
	//Atom wm_delete_window;

	PLStream* pls;
	plstream *plst;

public:
	GDLWINStream(int nx, int ny) :
		GDLGStream(nx, ny, "wingcc")
	{
		plst = new plstream(nx, ny, "wingcc");
	}

	~GDLWINStream()
	{
	}

	void Init();
	void EventHandler();
};

#endif
