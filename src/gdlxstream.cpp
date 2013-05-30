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
  if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"GDLXStream::GetGeometry(%ld %ld %ld %ld)\n", xSize, ySize, xoff, yoff);
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
void GDLXStream::Flush()
{
  XwDev *dev = (XwDev *) pls->dev;
  XwDisplay *xwd = (XwDisplay *) dev->xwd;
  XFlush( xwd->display);
}
void GDLXStream::WarpPointer(DLong x, DLong y)
{
  XwDev *dev = (XwDev *) pls->dev;
  XwDisplay *xwd = (XwDisplay *) dev->xwd;
  XWarpPointer( xwd->display, dev->window, None, 0, 0, 0, 0, x, y );
}
void GDLXStream::setDoubleBuffering()
{
  XwDev *dev = (XwDev *) pls->dev;
  dev->write_to_window = 0;
  pls->db = 1;
}
void GDLXStream::unSetDoubleBuffering()
{
  XwDev *dev = (XwDev *) pls->dev;
  dev->write_to_window = 1;
  pls->db = 0;
}
bool GDLXStream::hasDoubleBuffering()
{
  return true;
}
bool GDLXStream::GetGin( PLGraphicsIn *gin, int mode)
{
  bool status=true;
  bool warp=false;
  int dx,dy;
  XwDev *dev = (XwDev *) pls->dev;
  XwDisplay *xwd = (XwDisplay *) dev->xwd;
  if (mode == 0)
  {
    Window root, child;
    int root_x, root_y, x,y;
    unsigned int state;
    XQueryPointer(xwd->display, dev->window, &root, &child,
               &root_x, &root_y, &x, &y, &state ) ;
    gin->pX = x;
    gin->pY = dev->height - y;
    gin->state = state;
    gin->dX = (PLFLT) gin->pX / ( dev->width - 1 );
    gin->dY = (PLFLT) gin->pY / ( dev->height - 1 );
    gin->string[0] = '\0';
    gin->keysym = 0x20;
    gin->button = 0;
    return true;
  }
  int  x, x1, xmin = 0, xmax = (int) dev->width - 1;
  int  y, y1, ymin = 0, ymax = (int) dev->height - 1;
  XWMHints gestw;
  XSizeHints sizehints,initialstate;
  long hints_supplied;
  /* get normal state of the window */
  XGetWMNormalHints(xwd->display, dev->window,&initialstate,&hints_supplied);
  /* force fixed size to prevent a change of window size with the cursor*/
  sizehints.min_width=sizehints.max_width=dev->width;
  sizehints.min_height=sizehints.max_height=dev->height;
  sizehints.flags=(PMinSize|PMaxSize);
  XSetWMNormalHints(xwd->display, dev->window,&sizehints);
  /* add focus to the window on (all) displays */
  gestw.input = TRUE;
  gestw.flags = InputHint;
  XSetWMHints(xwd->display, dev->window, &gestw);

  unsigned long event_mask= PointerMotionMask | KeyPressMask;
  switch (mode)
  {
    case 1:
    case 3:
      event_mask |= ButtonPressMask;
      break;
    case 4:
      event_mask |= ButtonPressMask | ButtonReleaseMask;
      break;
    case 2:
      event_mask |= ButtonPressMask | ButtonReleaseMask;
  }

  XEvent event;
  //do our own event handling
  int first=0;
  XSelectInput(xwd->display, dev->window, event_mask);
  XRaiseWindow(xwd->display, dev->window);
  XFlush(xwd->display);
  while(1)
  {
    XWindowEvent(xwd->display, dev->window, event_mask, &event);
    switch (event.type)
    {
      int nchars;
      KeySym mykey;

      case KeyPress:    // exit in error if ^C
        gin->pX = event.xkey.x;
        gin->pY = event.xkey.y;
        gin->state = event.xkey.state;
        nchars = XLookupString(&event.xkey, gin->string, PL_MAXKEY - 1, &mykey, NULL);
        gin->string[nchars] = '\0';
        gin->keysym = (unsigned int) mykey;
        if (gin->state&4 && (gin->keysym==67 || gin->keysym==99)) 
        {
          status=false;
          goto end;
        }
        warp=false;
        dx=0;
        dy=0;
        switch(mykey)
        {
          case XK_Cancel:
          case XK_Break:
            status = false;
            goto end;
          case XK_Left:
            dx=-1; warp=true;
            break;
          case XK_Up:
            dy=-1; warp=true;
            break;
          case XK_Right:
            dx=1; warp=true;
            break;
          case XK_Down:
            dy=1; warp=true;
            break;
        }
        if (warp)
        {
        // Each modifier key added increases the multiplication factor by 5
        // Shift
        if ( gin->state & 0x01 )
        {
            dx *= 5;
            dy *= 5;
        }
        // Caps Lock
        if ( gin->state & 0x02 )
        {
            dx *= 5;
            dy *= 5;
        }
        // Control
        if ( gin->state & 0x04 )
        {
            dx *= 5;
            dy *= 5;
        }
        // Alt
        if ( gin->state & 0x08 )
        {
            dx *= 5;
            dy *= 5;
        }
        // Bounds checking so that we don't send cursor out of window
        x1 = gin->pX + dx;
        y1 = gin->pY + dy;
        if ( x1 < xmin )
            dx = xmin - gin->pX;
        if ( y1 < ymin )
            dy = ymin - gin->pY;
        if ( x1 > xmax )
            dx = xmax - gin->pX;
        if ( y1 > ymax )
            dy = ymax - gin->pY;
        XWarpPointer( xwd->display, dev->window, None, 0, 0, 0, 0, dx, dy );
        }
        break;
      case MotionNotify:
        gin->pX = event.xmotion.x;
        gin->pY = event.xmotion.y;
        gin->state = event.xmotion.state;
        gin->string[0] = '\0';
        gin->keysym = 0x20;
  //      fprintf(stderr,"motion %d %d, state %d\n",gin->pX,gin->pY,gin->state);
        if (mode==2) goto end; // crosshair not available if we exit on motion!!!
        if(event.type==MotionNotify){
            x=event.xmotion.x; y=event.xmotion.y;
        }
        else {
            x=event.xcrossing.x; y=event.xcrossing.y;
        }
        if (!first)
            first=1;
        else
        {
          XDrawLines( xwd->display, dev->window, xwd->gcXor, dev->xhair_x, 2,
              CoordModeOrigin );
          XDrawLines( xwd->display, dev->window, xwd->gcXor, dev->xhair_y, 2,
              CoordModeOrigin );
        }
        dev->xhair_x[0].x = (short) xmin; dev->xhair_x[0].y = (short) y;
        dev->xhair_x[1].x = (short) xmax; dev->xhair_x[1].y = (short) y;
        dev->xhair_y[0].x = (short) x; dev->xhair_y[0].y = (short) ymin;
        dev->xhair_y[1].x = (short) x; dev->xhair_y[1].y = (short) ymax;
        XDrawLines( xwd->display, dev->window, xwd->gcXor, dev->xhair_x, 2,
            CoordModeOrigin );
        XDrawLines( xwd->display, dev->window, xwd->gcXor, dev->xhair_y, 2,
            CoordModeOrigin );
        break;
      case ButtonPress:
        gin->pX = event.xbutton.x;
        gin->pY = event.xbutton.y;
        gin->state = event.xbutton.state;
        gin->button = event.xbutton.button;
        gin->string[0] = '\0';
        gin->keysym = 0x20;
        XSync(xwd->display, true);
        if (mode==4) break;
        goto end; //always exit on this event
      case ButtonRelease:
        gin->pX = event.xbutton.x;
        gin->pY = event.xbutton.y;
        gin->state = event.xbutton.state;
        gin->button = event.xbutton.button;
        gin->string[0] = '\0';
        gin->keysym = 0x20;
        XSync(xwd->display, true);
        goto end; //always exit on this event
      default:
        break;
    }
  }
  end: if(first) {
        XDrawLines( xwd->display, dev->window, xwd->gcXor, dev->xhair_x, 2,
            CoordModeOrigin );
        XDrawLines( xwd->display, dev->window, xwd->gcXor, dev->xhair_y, 2,
            CoordModeOrigin );
  }
  gin->pY = dev->height - gin->pY;
  gin->dX = (PLFLT) gin->pX / ( dev->width - 1 );
  gin->dY = (PLFLT) gin->pY / ( dev->height - 1 );
  //give back plplot's handling:
  XSelectInput(xwd->display, dev->window,dev->event_mask);
   /* restore old hints */
  XSetWMNormalHints(xwd->display, dev->window, &initialstate);
  /* remove focus to the window on (all) displays */
  gestw.input = FALSE;
  gestw.flags = InputHint;
  XSetWMHints(xwd->display, dev->window, &gestw);
    /* give back the right to change the window size*/
  sizehints.min_width=0;
  sizehints.max_width=XWidthOfScreen(XDefaultScreenOfDisplay(xwd->display));
  sizehints.min_height=32;
  sizehints.max_height=XHeightOfScreen(XDefaultScreenOfDisplay(xwd->display));
  sizehints.flags=(PMinSize|PMaxSize);
  XSetWMNormalHints(xwd->display, dev->window,&sizehints);
  XFlush(xwd->display);
  return status;
}

#endif
