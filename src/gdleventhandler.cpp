/***************************************************************************
            gdleventhandler.hpp  -  global event handler routine 
                             -------------------
    begin                : February 23 2004
    copyright            : (C) 2004 by Marc Schellens
    email                : m_schellens@users.sf.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// later this should become a thread
// right now its called in DInterpreter::NoReadline(...) or via readline

#include "includefirst.hpp"

#ifdef __APPLE__
#include <time.h>
#endif
#ifdef _WIN32
#include <winsock2.h>
#endif

#include "gdleventhandler.hpp"
#include "graphicsdevice.hpp"

#ifdef HAVE_LIBWXWIDGETS
#include "gdlwidget.hpp"
#endif

using namespace std;

int GDLEventHandler()
{
  GraphicsDevice::HandleEvents();

#ifdef HAVE_LIBWXWIDGETS
  GDLWidget::HandleEvents();
  const long OS_X_DELAY_NS = 5000000; // 5ms
#else
  const long OS_X_DELAY_NS = 20000000; // 20ms
#endif

#ifdef __APPLE__
  // under OS X the event loop burns to much CPU time
  struct timespec delay;
  delay.tv_sec=0;
  delay.tv_nsec = OS_X_DELAY_NS; // 20ms
  nanosleep(&delay,NULL);
#endif
#ifdef _WIN32 
  Sleep(10);  // this just to quiet down the character input from readline. 2 was not enough. 20 was ok.
#endif
  return 0;
}

