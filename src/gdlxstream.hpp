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
    : GDLGStream( nx, ny, "xwin")
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
  void GetGeometry( long& xSize, long& ySize, long& xoff, long& yoff);
  unsigned long GetWindowDepth();
  DLong GetVisualDepth();
  DString GetVisualName();
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
  bool HasDoubleBuffering();
  bool HasSafeDoubleBuffering();
  bool PaintImage(unsigned char *idata, PLINT nx, PLINT ny,  DLong *pos, DLong tru, DLong chan);
  virtual bool HasCrossHair() {return true;}
  void UnMapWindow();
  DByteGDL* GetBitmapData();
  void Color( ULong color, DLong decomposed);



};

#endif

#endif
