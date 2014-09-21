/***************************************************************************
  gdlwxstream.hpp  - adapted from plplot wxWidgets driver documentation
                             -------------------
    begin                : Wed Oct 16 2013
    copyright            : (C) 2013 by Marc Schellens
    email                : m_schellens@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

//#ifdef FOR_IMPORT

#ifndef GDLWXSTREAM_HPP_
#define GDLWXSTREAM_HPP_

#ifdef HAVE_LIBWXWIDGETS

#include "gdlgstream.hpp"
#include "gdlwidget.hpp"

#include <wx/dc.h>
#include <wx/rawbmp.h>

class GDLWXStream: public GDLGStream 
{
private:
    wxMemoryDC*  	m_dc;
    wxBitmap*    	m_bitmap;

//     wxDC* m_dc;   //!< Pointer to wxDC to plot into.
    int m_width;   //!< Width of dc/plot area.
    int m_height;   //!< Height of dc/plot area.

    GDLDrawPanel* gdlWindow; // for Update()
	GDLFrame* gdlFrame;
public:
	GDLWXStream(int width, int height)
		: GDLGStream(width, height, "wxwidgets")
		, m_dc(NULL)
		, m_bitmap(NULL)
		, m_width(width), m_height(height)
		, gdlWindow(NULL)
		, gdlFrame(NULL)
	{};
    ~GDLWXStream();  //!< Constructor.
    
    wxMemoryDC* GetDC() const { return m_dc;}

//     void set_stream();   //!< Calls some code before every PLplot command.
    void SetSize( int width, int height );   //!< Set new size of plot area.
    void RenewPlot();   //!< Redo plot.
    void Update();
    void SetGDLDrawPanel(GDLDrawPanel*);

    void Init();
    //void EventHandler(); //    
    
    //static int   GetImageErrorHandler(Display *display, XErrorEvent *error); //

    void GetGeometry( long& xSize, long& ySize, long& xoff, long& yoff);
    unsigned long GetWindowDepth() ;   

  //DString GetVisualName();
  //bool SetFocus();
  //bool UnsetFocus();
  //bool SetBackingStore(int value);
  //bool SetGraphicsFunction(long value );
  //bool GetWindowPosition(long& xpos, long& ypos );
  //bool CursorStandard(int cursorNumber);
  void Clear();
    void Clear( DLong bColor); //
  //void Raise();
  //void Lower();
  //void Iconic();
  //void DeIconic();
  //bool GetGin(PLGraphicsIn *gin, int mode);
  //bool GetExtendedGin(PLGraphicsIn *gin, int mode);
  void WarpPointer(DLong x, DLong y);
  //void Flush();
  //void SetDoubleBuffering();
  //void UnSetDoubleBuffering();
  //bool HasDoubleBuffering();
  //bool HasSafeDoubleBuffering();
    bool PaintImage(unsigned char *idata, PLINT nx, PLINT ny, DLong *pos,
		   DLong trueColorOrder, DLong channel); //
  virtual bool HasCrossHair() {return true;}
};



#endif

#endif
