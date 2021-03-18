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
/* 
  For mingw-org, setting WINVER to at least Win2k (windef.h)
  This is required for the cursor callback method.
*/
#ifndef WINVER
#  define WINVER 0x0500
#else
#  if (WINVER < 0x0500)
#    define WINVER 0x0500
#  endif
#endif

#include <map>

#ifdef USE_WINGDI_NOT_WINGCC
// Copied from wingdi.c
// Driver viewer types
enum _dev_viewer
{
    VIEWER_MINIMAL = 0,       // Display just a plot area window
    VIEWER_FULL,              // Display the full function window
    VIEWER_PLOT               // A child only plot area
};

// Enumerated type for the device states
enum _dev_state
{
    DEV_WAITING = 0,  // Device is idle
    DEV_ACTIVE,       // Device is ready for next plot
    DEV_SIZEMOVE,     // Device might be sizing or moving the window
    DEV_RESIZE,       // Device is resizing the window
    DEV_DRAWING       // Device is actively drawing
};

// Enumerated type used to indicate the device type
// for updating page metrics
enum _dev_type
{
    DEV_WINDOW,          // Setup page metrics for a window
    DEV_PRINTER          // Setup page metrics for a printer
};
// Struct to hold device-specific info.
struct wingdi_Dev
{
    //
    // Members that are common to interactive GUI devices
    //
    PLFLT            xdpmm;  // Device x pixel per mm
    PLFLT            ydpmm;  // Device y pixel per mm
    PLFLT            xscale; // Virtual x pixels to device pixel scaling
    PLFLT            yscale; // Virtual y pixels to device pixel scaling
    PLINT            width;  // Window Width (which can change)
    PLINT            height; // Window Height

    enum _dev_viewer viewer;
    enum _dev_type   type;
    enum _dev_state  state;             // Current state of the device
    enum _dev_state  prev_state;        // Previous state of the device
    // Used to restore after redraw
    union
    {
        unsigned int status_bar : 1;
        unsigned int menu_bar   : 1;
    } feature;

    //
    // WIN32 API variables
    //
    HDC      hdc;                 // Plot window device context
    HPEN     pen;                 // Current pen used for drawing
    COLORREF color;               // Current color
    HDC      hdc_bmp;             // Bitmap device context
    HBITMAP  bitmap;              // Bitmap of current display
    HWND     frame;               // Handle for the main window.
    HWND     plot;                // Handle for the plot area
    HWND     status_bar;          // Handle for the status bar

    //
    // Image rasterization variables
    HDC     save_hdc;                          // Save the original plot window DC
    HBITMAP raster_bmp;                        // Bitmap for the raster image
    RECT    raster_rect;                       // Location of the image
};
#else
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
#endif

typedef struct {
	BITMAPINFO bi;
	RGBQUAD *lpbitmap;
	bool has_data;
} tv_buf_t;

class GDLWINStream : public GDLGStream
{
private:
	//Atom wm_protocols;
	//Atom wm_delete_window;
	HWND refocus;

	PLStream* pls;
	plstream *plst;

	tv_buf_t tv_buf;
    int _mode;
    PLGraphicsIn *_gin;
    POINT GinPoint;
    HCURSOR CrosshairCursor;
    bool rbutton, xbutton, mbutton, buttonpressed;
public:
    std::map<UINT, void (CALLBACK GDLWINStream::*)(UINT, WPARAM, LPARAM)> msghooks;

	GDLWINStream(int nx, int ny) :
#ifdef USE_WINGDI_NOT_WINGCC
		GDLGStream(nx, ny, "wingdi")
#else
		GDLGStream(nx, ny, "wingcc")
#endif
	{
		pls = 0;
		// get the command interpreter window's handle
//		plsetopt("drvopt","nofocus"); // avoid stealing focus on window creation
//		plsetopt("drvopt","text");  // use freetype fonts
		refocus = GetForegroundWindow();
	}

	~GDLWINStream();
	void Init();
	void EventHandler();

	//	  static int   GetImageErrorHandler(Display *display, XErrorEvent *error);

	void SetState(char);

	void GetGeometry(long& xSize, long& ySize);
	bool GetGin(PLGraphicsIn *gin, int mode);

	unsigned long GetWindowDepth();
	DLong GetVisualDepth();
	//  DString GetVisualName();

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
  void Update(){ this->eop();}
	bool PaintImage(unsigned char *idata, PLINT nx, PLINT ny, DLong *pos, DLong tru, DLong chan);

	//  bool SetGraphicsFunction(long value );
	virtual bool HasCrossHair() { return true; }

	void SetWindowTitle(char* buf);
	HWND GetHwnd(void);
	HDC GetHdc(void);
    void Load_gin(HWND window);
	void RedrawTV();
  DByteGDL* GetBitmapData();

    void CALLBACK GinCallback(UINT message, WPARAM wParam, LPARAM lParam);
};

typedef std::map<UINT, void (CALLBACK GDLWINStream::*)(UINT, WPARAM, LPARAM)>::iterator msghookiter;

#endif
