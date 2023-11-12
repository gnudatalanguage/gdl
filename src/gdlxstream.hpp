/* *************************************************************************
                          gdlxstream.hpp  -  graphic stream X windows
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

#ifndef GDLXSTREAM_HPP_
#define GDLXSTREAM_HPP_

#include "gdlgstream.hpp"
#ifdef HAVE_X
#  include <plplot/plxwd.h>

class GDLXStream: public GDLGStream
{
  Atom wm_protocols;
  Atom wm_delete_window;
  Window term_window;
public:
  GDLXStream( PLINT xp , PLINT yp, PLINT nx, PLINT ny, PLINT xoff, PLINT yoff, const std::string &title)
    : GDLGStream( nx, ny, "xwin") 
    , term_window(0)
{
    spage( xp , yp, nx, ny, xoff, yoff); 

    // window title
    static char buf[ 256];
    strncpy(buf, title.c_str(), 255);
    buf[ 255] = 0;
    setopt("plwindow", buf);

    // no pause on win destruction
    plstream::spause(false);

    // extended fonts
    plstream::fontld(1);

    // we want color
    plstream::scolor(1);

    PLINT r[ctSize], g[ctSize], b[ctSize];
    GDLCT* myCT = GraphicsDevice::GetGUIDevice()->GetCT();
    myCT->Get(r, g, b);
    SetColorMap0(r, g, b, ctSize); //set colormap 0 to 256 values


    // Do not init colors --- we handle colors ourseves, very much faster!
    // Do not use command buffer
    // Please no threads, no gain especially in remote X11 
    setopt("drvopt", "noinitcolors=1,nobuffered=1,usepth=0");

    FindTerminalWindow(); //to pro
    //all the options must be passed BEFORE INIT=plinit.
    init(); //creates the X11 window.
    PostInit(); //finish setting different things, including giving back focus to terminal
    
    // need to be called initially. permit to fix things
    plstream::ssub(1, 1); // plstream below stays with ONLY ONE page
    plstream::adv(0); //-->this one is the 1st and only pladv
    // load font
    plstream::font(1);
    plstream::vpor(0, 1, 0, 1);
    plstream::wind(0, 1, 0, 1);

    ssub(1, 1);
    SetPageDPMM();
    DefaultCharSize();
    adv(0); //this is for us (counters) //needs DefaultCharSize
    clear();
  }

  ~GDLXStream()
  {}
  void Init(){};
  void FindTerminalWindow();
  void PostInit();
  void EventHandler();

  /*------------------------------------------------------------------------*\
   * GetImageErrorHandler()
   *
   * Error handler used in XGetImage() to catch errors when pixmap or window
   * are not completely viewable.
   \*-----------------------------------------------------------------------*/
  static int   GetImageErrorHandler(Display *display, XErrorEvent *error)
  {
    if (error->error_code != BadMatch) {
      char buffer[256];
      XGetErrorText(display, error->error_code, buffer, 256);
      std::cerr << "xwin: Error in XGetImage: " << buffer << std::endl;
    }
    return 1;
  }
  void Update();
  void GetGeometry( long& xSize, long& ySize);
  unsigned long GetWindowDepth();
  DLong GetVisualDepth();
  DString GetVisualName();
  BaseGDL* GetFontnames(DString pattern);
  DLong GetFontnum(DString pattern);
  bool setFocus(bool value);

  bool UnsetFocus();
  bool SetBackingStore(int value);
  bool SetGraphicsFunction(long value );
  bool GetWindowPosition(long& xpos, long& ypos );
  bool CursorStandard(int cursorNumber);
  void Clear();
  void Clear( DLong chan);
  void Raise();
  void Lower();
  void Iconic();
  void DeIconic();
  bool GetGin(PLGraphicsIn *gin, int mode);
  bool GetExtendedGin(PLGraphicsIn *gin, int mode);
  void WarpPointer(DLong x, DLong y);
  void Flush();
  void SetDoubleBuffering();
  void UnSetDoubleBuffering();
  bool HasSafeDoubleBuffering();
  bool PaintImage(unsigned char *idata, PLINT nx, PLINT ny,  DLong *pos, DLong tru, DLong chan);
  virtual bool HasCrossHair() {return true;}
  void UnMapWindowAndSetPixmapProperty();
  virtual DByteGDL* GetBitmapData(int xoff, int yoff, int nx, int ny) final;
  void Color( ULong color, DLong decomposed);
//  void Update(){plstream::cmd(PLESC_EXPOSE, NULL);}
  
  //GD: overloading scmap0 to accelerate plots for X11 and possibly others

 void SetColorMap0(PLINT *r, PLINT *g, PLINT *b, PLINT ncol0) {
 } //DO NOTHING!
 //GD probably impossible to avoid plplot's XAllocColor for contours (since plcol is called inside driver) unless
 //we make our own contouring function (quite easy in fact). 
// void SetColorMap1(const PLINT *r, const PLINT *g, const PLINT *b, PLINT ncol1) {
// } //DO NOTHING!
//
// void SetColorMap1l(bool itype, PLINT npts, const PLFLT *intensity, const PLFLT *coord1, 
//   const PLFLT *coord2, const PLFLT *coord3, const bool *rev = NULL) {
// } //DO NOTHING
//
// void SetColorMap1n(PLINT ncol1) {
// } //DO NOTHING
};

#endif

#endif
