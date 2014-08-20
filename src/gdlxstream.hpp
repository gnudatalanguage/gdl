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

#ifndef HAVE_X
#else

class GDLXStream: public GDLGStream
{
  Atom wm_protocols;
  Atom wm_delete_window;

public:
  GDLXStream( int nx, int ny)
    : GDLGStream( nx, ny, "xwin")
  {
  }

  ~GDLXStream()
  {}
  
  void Init();
  void EventHandler();

  static int   GetImageErrorHandler(Display *display, XErrorEvent *error);

  void GetGeometry( long& xSize, long& ySize, long& xoff, long& yoff);
  void GetWindowSize( long& xSize, long& ySize);
  unsigned long GetWindowDepth();
  DLong GetVisualDepth();
  DString GetVisualName();
  bool SetFocus();
  bool UnsetFocus();
  bool EnableBackingStore(bool enable);
  bool SetGraphicsFunction(long value );
  bool GetWindowPosition(long& xpos, long& ypos );
  bool CursorStandard(int cursorNumber);
  void Clear();
  void Clear( DLong bColor);
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
};

#endif

#endif
