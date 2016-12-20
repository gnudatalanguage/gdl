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
  GDLXStream( int nx, int ny)
    : GDLGStream( nx, ny, "xwin",XOpenDisplay(NULL)==NULL?":0":NULL) //IDL also opens :0 when DISPLAY is not set.
    , term_window(0)
  {
  }

  ~GDLXStream()
  {}
  
  void Init();
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
  void UnMapWindow();
  DByteGDL* GetBitmapData();
  void Color( ULong color, DLong decomposed);
//  void Update(){plstream::cmd(PLESC_EXPOSE, NULL);}
  
  //GD: overloading scmap0 to accelerate plots for X11 and possibly others

 void SetColorMap0(const PLINT *r, const PLINT *g, const PLINT *b, PLINT ncol0) {
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
