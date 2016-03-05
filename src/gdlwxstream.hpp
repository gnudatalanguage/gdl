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

//#include <wx/dc.h>
//#include <wx/rawbmp.h>
class GDLDrawPanel;

class GDLWXStream: public GDLGStream 
{
private:
    wxMemoryDC*  	m_dc;
    wxBitmap*    	m_bitmap;

//     wxDC* m_dc;   //!< Pointer to wxDC to plot into.
    int m_width;   //!< Width of dc/plot area.
    int m_height;   //!< Height of dc/plot area.

    GDLDrawPanel* gdlWindow; // for Update()
public:
    GDLWXStream( int width, int height );  //!< Constructor.
    ~GDLWXStream();  //!< Constructor.
    
    wxMemoryDC* GetDC() const { return m_dc;}

//     void set_stream();   //!< Calls some code before every PLplot command.
    void SetSize( int width, int height );   //!< Set new size of plot area.
    void RenewPlot();   //!< Redo plot.
    void Update();
    void SetGDLDrawPanel(GDLDrawPanel*);
    GDLDrawPanel* GetGDLDrawPanel(){return gdlWindow;}

    void Init();
    //void EventHandler(); //    
    
    //static int   GetImageErrorHandler(Display *display, XErrorEvent *error); //

    void GetGeometry( long& xSize, long& ySize, long& xoff, long& yoff);
    unsigned long GetWindowDepth() ;   

    bool UnsetFocus();
    bool SetBackingStore(int value){return true;}
    void Clear();
    void Clear( DLong bColor);
    void Raise();
    void Lower();
    void Iconic();
    void DeIconic();
    bool GetGin(PLGraphicsIn *gin, int mode);
    //bool GetExtendedGin(PLGraphicsIn *gin, int mode);
    void WarpPointer(DLong x, DLong y);
    void Flush(){  plstream::cmd( PLESC_FLUSH, NULL );}
    //void SetDoubleBuffering();
    //void UnSetDoubleBuffering();
    bool HasDoubleBuffering(){return true;}
    bool HasSafeDoubleBuffering(){return ( m_dc->GetLogicalFunction() == wxCOPY);};
    bool PaintImage(unsigned char *idata, PLINT nx, PLINT ny, DLong *pos,
		   DLong trueColorOrder, DLong channel);
    virtual bool HasCrossHair() {return true;}
    bool SetGraphicsFunction( long value);
    bool GetWindowPosition(long& xpos, long& ypos );
    bool CursorStandard(int cursorNumber);
    DLong GetVisualDepth();
    DString GetVisualName();
    bool GetScreenResolution(double& resx, double& resy);
    DByteGDL* GetBitmapData();
};



#endif

#endif
