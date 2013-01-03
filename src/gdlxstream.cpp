/* *************************************************************************
                          gdlxstream.cpp  -  graphic stream x windows
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

#include "includefirst.hpp"
#include <iostream>

#include "graphics.hpp"
#include "gdlxstream.hpp"

#ifndef HAVE_X
#else

using namespace std;

// bool GDLXStream::plstreamInitCalled = false;

void GDLXStream::Init()
{
  // plstream::init() calls exit() if it cannot establish a connection with X-server
  {
    Display* display = XOpenDisplay(NULL);
    if (display == NULL) 
    {
      valid = false;
      ThrowGDLException("Cannot connect to X server");
    }
    XCloseDisplay(display);
  }

//      if( !plstreamInitCalled)
//    {
		this->plstream::init();
//  		plstreamInitCalled = true;
// 	}
		
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

  // dummy call to get private function set_stream() called
//   char dummy;
//   gesc( &dummy);
// 
//   plgpls( &pls);

  XwDev *dev = (XwDev *) pls->dev;

// 	if( dev == NULL)
// 		this->plstream::init();
// 
//   plgpls( &pls);
//   
//   dev = (XwDev *) pls->dev;

	if( dev == NULL)
	{
		cerr << "X window invalid." << endl;
		valid = false;
		return;
    }
    
  XwDisplay *xwd = (XwDisplay *) dev->xwd;
  
	if( xwd == NULL)
	{
		cerr << "X window not set." << endl;
		valid = false;
		return;
    }
    
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
  plstream::gpage( xp, yp, xleng, yleng, plxoff, plyoff);

  xoff = plxoff;
  yoff = plyoff;
  if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"GDLXStream::GetGeometry(%d %d %d %d)\n", xSize, ySize, xoff, yoff);
}

// plplot 5.3 does not provide the clear function for c++
void GDLXStream::Clear()
{
  // dummy call to get private function set_stream() called
  //  PLFLT a=0.0,b=0.0,c=0.0,d,e,f;
  //  RGB_HLS( a,b,c,&d,&e,&f);
  char dummy;
  gesc( &dummy);

  ::c_plclear();
}

void GDLXStream::Clear( DLong bColor)
{
  // dummy call to get private function set_stream() called
  //  PLFLT a=0.0,b=0.0,c=0.0,d,e,f;
  //  RGB_HLS( a,b,c,&d,&e,&f);
  char dummy;
  gesc( &dummy);

  PLINT r0,g0,b0;
  PLINT r1,g1,b1;
  DByte rb, gb, bb;

  // Get current background color
  plgcolbg (&r0, &g0, &b0);

  // Get desired background color
  GDLCT* actCT = Graphics::GetCT();
  actCT->Get( bColor, rb, gb, bb);

  // Convert to PLINT from GDL_BYTE
  r1 = (PLINT) rb;
  g1 = (PLINT) gb;
  b1 = (PLINT) bb;
// this mimics better the *DL behaviour.
  ::c_plbop();
  plscolbg (r1, g1, b1);

//  ::c_plclear();
//
//  plscolbg (r0, g0, b0);
}

void GDLXStream::Raise()
{
  XwDev *dev = (XwDev *) pls->dev;
  XwDisplay *xwd = (XwDisplay *) dev->xwd;
  XRaiseWindow(dev->xwd->display, dev->window);
}

void GDLXStream::Lower()
{
  XwDev *dev = (XwDev *) pls->dev;
  XwDisplay *xwd = (XwDisplay *) dev->xwd;
  XLowerWindow(dev->xwd->display, dev->window);
}

// note by AC on 2012-Aug-16    Help/suggestions welcome
// I don't know how to find the sub-window number (third parametre
// in call XIconifyWindow())

void GDLXStream::Iconic()
{
  XwDev *dev = (XwDev *) pls->dev;
  XwDisplay *xwd = (XwDisplay *) dev->xwd;
  XIconifyWindow(dev->xwd->display, dev->window,0);
}

void GDLXStream::DeIconic()
{
  XwDev *dev = (XwDev *) pls->dev;
  XwDisplay *xwd = (XwDisplay *) dev->xwd;
  XMapWindow(dev->xwd->display, dev->window);
}

#endif
