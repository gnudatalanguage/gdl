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
#include "graphicsdevice.hpp"

#include <wx/fontenum.h>
#include <wx/fontmap.h>
class gdlwxDrawPanel;

static std::vector<wxCursor> gdlwxCursors;

class GDLWXStream: public GDLGStream 
{
private:
    wxMemoryDC*  	streamDC;
    wxBitmap*    	streamBitmap;

    int m_width;   //!< Width of dc/plot area.
    int m_height;   //!< Height of dc/plot area.
    bool isplot; //precise the status of associated widget: plot (true) or widget_draw (false)
    bool olddriver; //memory of if the wxwidgets driver is old, and thus... OK: reliable and fast!
public:
    gdlwxGraphicsPanel* container; // for Update()

    GDLWXStream( int width, int height );  
    ~GDLWXStream(); 
    
    wxMemoryDC* GetStreamDC() const { return streamDC;}

//     void set_stream();   //!< Calls some code before every PLplot command.
    void SetSize( const wxSize s );   //!< Set new size of plot area.
    void RenewPlot();   //!< Redo plot.
    void Update();
    void SetGdlxwGraphicsPanel(gdlwxGraphicsPanel* w, bool isPlot=true);
    gdlwxGraphicsPanel* GetMyContainer(){return container;}
    void DestroyContainer(){delete container; container=NULL;}
    
    bool IsPlot() {return isplot;}
    
    void Init();
    void EventHandler();
    
    //static int   GetImageErrorHandler(Display *display, XErrorEvent *error); //

    void GetGeometry( long& xSize, long& ySize);
    unsigned long GetWindowDepth() ;   

//    bool UnsetFocus(); UnsetFocus is dangerous: it prevents using wxEvents correctly.
    bool SetBackingStore(int value){return true;}
    void Clear();
    void Clear( DLong bColor);
    void Raise();
    void Lower();
    void Iconic();
    void DeIconic();
    bool UnsetFocus();
    bool GetGin(PLGraphicsIn *gin, int mode);
    //bool GetExtendedGin(PLGraphicsIn *gin, int mode);
    void WarpPointer(DLong x, DLong y);
    void Flush(){ }// plstream::cmd( PLESC_FLUSH, NULL );}
    //void SetDoubleBuffering();
    //void UnSetDoubleBuffering();
    bool HasDoubleBuffering(){return true;}
    bool HasSafeDoubleBuffering(){return ( streamDC->GetLogicalFunction() == wxCOPY);};
    bool PaintImage(unsigned char *idata, PLINT nx, PLINT ny, DLong *pos,
		   DLong trueColorOrder, DLong channel);
    virtual bool HasCrossHair() {return true;}
    bool SetGraphicsFunction( long value);
    bool GetWindowPosition(long& xpos, long& ypos );
    bool CursorStandard(int cursorNumber);
    bool CursorImage(char* v, int x, int y, char* m);
    DLong GetVisualDepth();
    BaseGDL* GetFontnames(DString pattern);
    DLong GetFontnum(DString pattern);
    void SetCurrentFont(std::string fontname);
    DString GetVisualName();
    bool GetScreenResolution(double& resx, double& resy);
    DByteGDL* GetBitmapData();
    static void DefineSomeWxCursors(); //global initialisation of 77 X11-like cursors.
    virtual void fontChanged() final {
     if (olddriver) {
      PLINT doFont = ((PLINT) SysVar::GetPFont()>-1) ? 1 : 0;
      pls->dev_text = doFont;
     }
    }
};



#endif

#endif
