/* *************************************************************************
                          gdlxstream.cpp  -  graphic stream x windows
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
    email                : m_schellens@hotmail.com
 ***************************************************************************/

/* *************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <iostream>

#include "gdlxstream.hpp"

using namespace std;

void GDLXStream::Init()
{
  plstream::init();

  //  set_stream(); // private
  plgpls( &pls);
  XwDev *dev = (XwDev *) pls->dev;
  XwDisplay *xwd = (XwDisplay *) dev->xwd;

  wm_protocols = XInternAtom( xwd->display, "WM_PROTOCOLS", false);  
  wm_delete_window = XInternAtom( xwd->display, "WM_DELETE_WINDOW", false);
  
  XSetWMProtocols( xwd->display, dev->window, &wm_delete_window, 1);
  XFlush( xwd->display);
}

void GDLXStream::EventHandler()
{
  if( !valid) return;

  XwDev *dev = (XwDev *) pls->dev;
  XwDisplay *xwd = (XwDisplay *) dev->xwd;
  
  XEvent event;
  if( XCheckTypedWindowEvent( xwd->display, dev->window, 
			      ClientMessage, &event))
    {
      if( event.xclient.message_type == wm_protocols &&
	  event.xclient.data.l[0] == wm_delete_window)  
	{
	  valid = false;
	  return; // no more event handling
	}
      else
	XPutBackEvent( xwd->display, &event);
    }

  // plplot event handler
  plstream::cmd( PLESC_EH, NULL);
}

void GDLXStream::GetGeometry( long& xSize, long& ySize, long& xoff, long& yoff)
{
  // plplot does not return the real size
  XwDev *dev = (XwDev *) pls->dev;
  XwDisplay *xwd = (XwDisplay *) dev->xwd;

  XWindowAttributes win_attr;

  /* query the window's attributes. */
  Status rc = XGetWindowAttributes(xwd->display,
				   dev->window,
				   &win_attr);
  xSize = win_attr.width;
  ySize = win_attr.height;

  PLFLT xp; PLFLT yp; 
  PLINT xleng; PLINT yleng;
  PLINT plxoff; PLINT plyoff;
  gpage( xp, yp, xleng, yleng, plxoff, plyoff);

  xoff = plxoff;
  yoff = plyoff;
}
