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

#include "graphicsdevice.hpp"
#include "gdlxstream.hpp"

#ifndef HAVE_X
#else

using namespace std;

// bool GDLXStream::plstreamInitCalled = false;


// the code below (up to "binding _visual_classes") is extracted from "xwininfo.c"
// xwininfo.c - MIT Project Athena, X Window system window information utility.
// Author: Mark Lillibridge, MIT Project Athena	16-Jun-87
 /* 
 * Lookup: lookup a code in a table.
 */
typedef struct {
	long code;
	char *name;
} binding;

static char _lookup_buffer[100];

char *LookupL(long code, binding * table)
{
  char *name;
  
  sprintf(_lookup_buffer, "unknown (code = %ld. = 0x%lx)", code, code);
  name = _lookup_buffer;
  
  while (table->name) {
    if (table->code == code) {
      name = table->name;
      break;
    }
    table++;
  }  
  return(name);
}

char *Lookup(int code, binding *table)
{
  return LookupL((long)code, table);
}

static binding _visual_classes[] = {
  { StaticGray, "StaticGray" },
  { GrayScale, "GrayScale" },
  { StaticColor, "StaticColor" },
  { PseudoColor, "PseudoColor" },
  { TrueColor, "TrueColor" },
  { DirectColor, "DirectColor" },
  { 0, 0 }};

// end of code copy from "xwininfo.c"


void GDLXStream::Init() {
  // plstream::init() calls exit() if it cannot establish a connection with X-server
  {
    Display* display = XOpenDisplay(NULL);
    if (display == NULL) {
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
  plgpls(&pls);
  XwDev *dev = (XwDev *) pls->dev;
  XwDisplay *xwd = (XwDisplay *) dev->xwd;

  wm_protocols = XInternAtom(xwd->display, "WM_PROTOCOLS", false);
  wm_delete_window = XInternAtom(xwd->display, "WM_DELETE_WINDOW", false);

  XSetWMProtocols(xwd->display, dev->window, &wm_delete_window, 1);
  XFlush(xwd->display);
}

void GDLXStream::EventHandler() {
  if (!valid) return;

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

  if (dev == NULL) {
    cerr << "X window invalid." << endl;
    valid = false;
    return;
  }

  XwDisplay *xwd = (XwDisplay *) dev->xwd;

  if (xwd == NULL) {
    cerr << "X window not set." << endl;
    valid = false;
    return;
  }

  XEvent event;
  if (XCheckTypedWindowEvent(xwd->display, dev->window,
          ClientMessage, &event)) {
    if (event.xclient.message_type == wm_protocols &&
            event.xclient.data.l[0] == wm_delete_window) {
      valid = false;
      return; // no more event handling
    } else
      XPutBackEvent(xwd->display, &event);
  }

  // plplot event handler
  plstream::cmd(PLESC_EH, NULL);
}

void GDLXStream::Get_X11_VisualClassName(std::string &VisualClassName){

  XwDev *dev = (XwDev *) pls->dev;
  XwDisplay *xwd = (XwDisplay *) dev->xwd;  
  XWindowAttributes win_attributes;

  /* query the window's attributes. */
  Status rc = XGetWindowAttributes(xwd->display, dev->window, &win_attributes);

  /* need some works to go to Visual Name */
  int junk;
  XVisualInfo vistemplate, *vinfo; 
  vistemplate.visualid = XVisualIDFromVisual(win_attributes.visual);
  vinfo = XGetVisualInfo(xwd->display, VisualIDMask, &vistemplate, &junk);

  // in very old X11 code, "c_class" is "class" which cannot compile within C++
  VisualClassName=std::string(Lookup(vinfo->c_class, _visual_classes));

  int debug=0;
  if (debug)
    {
      printf("Depth: %d\n", win_attributes.depth);
      printf("Visual Class (2): %s\n", VisualClassName.c_str());
    }
}

void GDLXStream::Get_X11_WindowSize(long& xSize, long& ySize){

  XwDev *dev = (XwDev *) pls->dev;
  XwDisplay *xwd = (XwDisplay *) dev->xwd;
  XWindowAttributes win_attributes;
  Status rc = XGetWindowAttributes(xwd->display, dev->window, &win_attributes);
  xSize = win_attributes.width;
  ySize = win_attributes.height;

}

void GDLXStream::Get_X11_WindowGeometry(long& xSize, long& ySize, long& xOffset, long& yOffset) {

  XwDev *dev = (XwDev *) pls->dev;
  XwDisplay *xwd = (XwDisplay *) dev->xwd;

  XWindowAttributes win_attributes;

  /* query the full display (screen)'s attributes. */
  /* these values are used below to compute offsets */
  int screen_num, screen_width, screen_height;
  screen_num = DefaultScreen(xwd->display);
  screen_width = DisplayWidth(xwd->display, screen_num);
  screen_height = DisplayHeight(xwd->display, screen_num);

  /* query the window's attributes. */
  Status rc = XGetWindowAttributes(xwd->display, dev->window, &win_attributes);

  xSize = win_attributes.width;
  ySize = win_attributes.height;
  int junk;

  int rx, ry;
  Window junkwin;

  /* recovering the true offset */
  (void) XTranslateCoordinates (xwd->display, dev->window, win_attributes.root, 
				-win_attributes.border_width,
				-win_attributes.border_width,
				&rx, &ry, &junkwin);
  xOffset=(long)rx;
  yOffset=(long)(screen_height-ry-win_attributes.height);
  
  int debug=0;
  if (debug) {
    cout << "---- Begin Inside GetX11Geometry ----" << endl;
    cout << "display size : " << screen_width << " " << screen_height << endl;
    cout << "win_attributes W/H: " << win_attributes.width << " " << win_attributes.height << endl;
    cout << "win_attributes border width: " << win_attributes.border_width << endl;
    cout << "win_attributes X/Y: " << win_attributes.x << " " << win_attributes.y << endl;
    cout << "RX/RY: " << rx << " " << ry << endl;
    cout << "results: " << endl;
    cout << "xSize/ySize: " << xSize << " " << ySize << endl;
    cout << "xOffset/yOffset: " << xOffset << " " << yOffset << endl;
    cout << "---- End Inside GetX11Geometry ----" << endl;
  }

}

//warning neither X11 nor plplot give directly the good value for the position of the window!!!!
// you need to recover it using XQueryTree() or XTranslateCoordinates (see GDLXStream::GetX11Geometry() above)

void GDLXStream::GetGeometry(long& xSize, long& ySize, long& xOffset, long& yOffset) {

  GDLXStream::Get_X11_WindowGeometry(xSize ,ySize, xOffset, yOffset);

  /*  PLFLT xp, yp;
  PLINT xleng, yleng;
  PLINT plxOffset, plyOffset;

  plstream::gpage(xp, yp, xleng, yleng, plxOffset, plyOffset);

  xSize=xleng;
  ySize=yleng;
  xOffset = plxOffset; //not good either!!!
  yOffset = plyOffset; // idem
  */
  if (GDL_DEBUG_PLSTREAM) fprintf(stderr, "GDLXStream::GetGeometry(%ld %ld %ld %ld)\n", xSize, ySize, xOffset, yOffset);
}

// plplot 5.3 does not provide the clear function for c++

void GDLXStream::Clear() {
  // dummy call to get private function set_stream() called
  //  PLFLT a=0.0,b=0.0,c=0.0,d,e,f;
  //  RGB_HLS( a,b,c,&d,&e,&f);
  char dummy;
  gesc(&dummy);
  // this mimics better the *DL behaviour.
  ::c_plbop();
  //plclear clears only the current subpage.
  //  ::c_plclear();
}

void GDLXStream::Clear(DLong bColor) {
  // dummy call to get private function set_stream() called
  //  PLFLT a=0.0,b=0.0,c=0.0,d,e,f;
  //  RGB_HLS( a,b,c,&d,&e,&f);
  char dummy;
  gesc(&dummy);

  PLINT r0, g0, b0;
  PLINT r1, g1, b1;
  DByte rb, gb, bb;

  // Get current background color
  plgcolbg(&r0, &g0, &b0);

  // Get desired background color
  GDLCT* actCT = GraphicsDevice::GetCT();
  actCT->Get(bColor, rb, gb, bb);

  // Convert to PLINT from GDL_BYTE
  r1 = (PLINT) rb;
  g1 = (PLINT) gb;
  b1 = (PLINT) bb;
  // this mimics better the *DL behaviour.
  ::c_plbop();
  plscolbg(r1, g1, b1);

  //plclear clears only the current subpage.
  //  ::c_plclear();
  //
  //  plscolbg (r0, g0, b0);
}

void GDLXStream::Raise() {
  XwDev *dev = (XwDev *) pls->dev;
  XwDisplay *xwd = (XwDisplay *) dev->xwd;
  XRaiseWindow(dev->xwd->display, dev->window);
}

void GDLXStream::Lower() {
  XwDev *dev = (XwDev *) pls->dev;
  XwDisplay *xwd = (XwDisplay *) dev->xwd;
  XLowerWindow(dev->xwd->display, dev->window);
}

void GDLXStream::Iconic() {
  XwDev *dev = (XwDev *) pls->dev;
  XwDisplay *xwd = (XwDisplay *) dev->xwd;
  XIconifyWindow(xwd->display, dev->window, xwd->screen);
}

void GDLXStream::DeIconic() {
  XwDev *dev = (XwDev *) pls->dev;
  XwDisplay *xwd = (XwDisplay *) dev->xwd;
  XMapWindow(dev->xwd->display, dev->window);
}

void GDLXStream::Flush() {
  XwDev *dev = (XwDev *) pls->dev;
  XwDisplay *xwd = (XwDisplay *) dev->xwd;
  XFlush(xwd->display);
}

void GDLXStream::WarpPointer(DLong x, DLong y) {
  XwDev *dev = (XwDev *) pls->dev;
  XwDisplay *xwd = (XwDisplay *) dev->xwd;
  XWarpPointer(xwd->display, None, dev->window, 0, 0, 0, 0, x, dev->height - y);
}

void GDLXStream::SetDoubleBuffering() {
  XwDev *dev = (XwDev *) pls->dev;
  dev->write_to_window = 0;
  pls->db = 1;
}

void GDLXStream::UnSetDoubleBuffering() {
  XwDev *dev = (XwDev *) pls->dev;
  dev->write_to_window = 1;
  pls->db = 0;
}

bool GDLXStream::HasDoubleBuffering() {
  return true;
}
//modified version. Will not tell double buffering is available if current graphic function is not pure "copy".
bool GDLXStream::HasSafeDoubleBuffering() {
    XwDev *dev = (XwDev *) pls->dev;
    if( dev == NULL || dev->xwd == NULL)    return false;
    XwDisplay *xwd = (XwDisplay *) dev->xwd;
    XGCValues gcValues;
    XGetGCValues(xwd->display, dev->gc, GCFunction, &gcValues);
    if (gcValues.function == GXcopy ) return true; else return false;  
}
//simple GetGin that reproduces IDL's own.
//However, trapping  controlC should be done inside, to clean properly.
//Below, a more complicated version that adds other functionalities, and ^C trapping, but
//grabs the keyboard, which is not recommended.
bool GDLXStream::GetGin(PLGraphicsIn *gin, int mode) {

  enum CursorOpt {
    NOWAIT = 0,
    WAIT, //1
    CHANGE, //2
    DOWN, //3
    UP //4
  };
  XwDev *dev = (XwDev *) pls->dev;
  XwDisplay *xwd = (XwDisplay *) dev->xwd;
  Window root, child;
  int root_x, root_y, oldx, oldy;
  unsigned int ostate;
  XQueryPointer(xwd->display, dev->window, &root, &child,
          &root_x, &root_y, &oldx, &oldy, &ostate);
  gin->pX = oldx;
  gin->pY = dev->height - oldy;
  gin->state = ostate;
  gin->dX = (PLFLT) gin->pX / (dev->width - 1);
  gin->dY = (PLFLT) gin->pY / (dev->height - 1);
  gin->string[0] = '\0';
  gin->keysym = 0x20;
  gin->button = 0;
  if (ostate & Button1Mask) gin->button = 1;
  if (ostate & Button2Mask) gin->button = 2;
  if (ostate & Button3Mask) gin->button = 3;
  if (ostate & Button4Mask) gin->button = 4;
  if (ostate & Button5Mask) gin->button = 5; //IDL does not support buttons 4-5 but we may?
  //return if NOWAIT
  if (mode == NOWAIT) return true;
  
  unsigned long event_mask = (PointerMotionMask|ButtonMotionMask);
  switch (mode) {
    case WAIT:
      if (gin->button > 0) return true; //else wait below for a down...
    case DOWN:
      event_mask |= ButtonPressMask;
      break;
    case UP:
    case CHANGE:
      event_mask |= (ButtonPressMask | ButtonReleaseMask);
  }

  XEvent event;
  XRaiseWindow(xwd->display, dev->window);
  XSelectInput(xwd->display, dev->window, event_mask);
  XSync(xwd->display, true);  //useful?
  while (1) {
    XWindowEvent(xwd->display, dev->window, event_mask, &event);

    switch (event.type) {
        int nchars;
        KeySym mykey;
      case ButtonRelease:
        gin->pX = event.xbutton.x;
        gin->pY = event.xbutton.y;
        gin->state = event.xbutton.state;
        gin->button = event.xbutton.button;
        gin->string[0] = '\0';
        gin->keysym = 0x20;
        if (mode==UP || mode==CHANGE) goto end;
        break;
      case ButtonPress:
        gin->pX = event.xbutton.x;
        gin->pY = event.xbutton.y;
        gin->state = event.xbutton.state;
        gin->button = event.xbutton.button;
        gin->string[0] = '\0';
        gin->keysym = 0x20;
        if (mode==WAIT || mode==DOWN || mode==CHANGE) goto end;
        break;
      case MotionNotify:
        if (mode==CHANGE) {
          gin->pX = event.xmotion.x;
          gin->pY = event.xmotion.y;
          gin->state = event.xmotion.state;
          gin->string[0] = '\0';
          gin->keysym = 0x20;
          goto end;
        }
        break;
      default:
        break;
    }
  }
end:
  gin->pY = dev->height - gin->pY;
  gin->dX = (PLFLT) gin->pX / (dev->width - 1);
  gin->dY = (PLFLT) gin->pY / (dev->height - 1);
  return true;
}

bool GDLXStream::GetExtendedGin(PLGraphicsIn *gin, int mode) {

  enum CursorOpt {
    NOWAIT = 0,
    WAIT, //1
    CHANGE, //2
    DOWN, //3
    UP //4
  };
  bool status = true;
  bool warp = false;
  int dx, dy;
  XwDev *dev = (XwDev *) pls->dev;
  XwDisplay *xwd = (XwDisplay *) dev->xwd;
  Window root, child;
  int root_x, root_y, oldx, oldy;
  unsigned int ostate;
  XQueryPointer(xwd->display, dev->window, &root, &child,
          &root_x, &root_y, &oldx, &oldy, &ostate);
  gin->pX = oldx;
  gin->pY = dev->height - oldy;
  gin->state = ostate;
  gin->dX = (PLFLT) gin->pX / (dev->width - 1);
  gin->dY = (PLFLT) gin->pY / (dev->height - 1);
  bool alreadyInside=(gin->dX >=0.0 && gin->dX <1.0 && gin->dY >=0.0 && gin->dY <1.0);
  gin->string[0] = '\0';
  gin->keysym = 0x20;
  gin->button = 0;
  if (ostate & Button1Mask) gin->button = 1;
  if (ostate & Button2Mask) gin->button = 2;
  if (ostate & Button3Mask) gin->button = 3;
  if (ostate & Button4Mask) gin->button = 4;
  if (ostate & Button5Mask) gin->button = 5; //IDL does not support buttons 4-5 but we may?
  //return if NOWAIT
  if (mode == NOWAIT) return true;
  
  int x, x1, xmin = 0, xmax = (int) dev->width - 1;
  int y, y1, ymin = 0, ymax = (int) dev->height - 1;
  // If we want to trap keyboard events we need to set focus (complicated) OR grab keyboard:
  bool kbgrabbed= (XGrabKeyboard(xwd->display, dev->window, True, GrabModeAsync, GrabModeAsync, CurrentTime)==GrabSuccess);

unsigned long event_mask = (EnterWindowMask| LeaveWindowMask | KeyPressMask  | KeyReleaseMask | PointerMotionMask);
  switch (mode) {
    case WAIT:
      if (gin->button > 0) return true; //else wait below for a down...
    case DOWN:
      event_mask |= ButtonPressMask;
      break;
    case UP:
    case CHANGE:
      event_mask |= ButtonPressMask | ButtonReleaseMask;
      break;
  }  
  bool grabbed = false;
  unsigned long grab_mask = (EnterWindowMask|LeaveWindowMask|PointerMotionMask|ButtonPressMask | ButtonReleaseMask);
  if (alreadyInside)  grabbed=(XGrabPointer(xwd->display, dev->window, False, grab_mask,GrabModeAsync,GrabModeAsync,dev->window,None,CurrentTime)==GrabSuccess);

  XEvent event;
  //we do our own event handling and furthermore we need to grab the pointer since plplot owns this window, too.
  int first=0;
  XSelectInput(xwd->display, dev->window, event_mask);
  XRaiseWindow(xwd->display, dev->window);
  XSync(xwd->display, true);
  while (1) {
    XWindowEvent(xwd->display, dev->window, event_mask, &event);
    switch (event.type) {
        int nchars;
        KeySym mykey;
      case EnterNotify:
         if(!grabbed) grabbed=(XGrabPointer(xwd->display, dev->window, False, grab_mask,GrabModeAsync,GrabModeAsync,None,None,CurrentTime)==GrabSuccess);
          break;
      case LeaveNotify:
       XUngrabPointer(xwd->display,CurrentTime);
       grabbed = false;
       break;
      case ButtonRelease:
        gin->pX = event.xbutton.x;
        gin->pY = event.xbutton.y;
        gin->state = event.xbutton.state;
        gin->button = event.xbutton.button;
        gin->string[0] = '\0';
        gin->keysym = 0x20;
        if (mode==UP || mode==CHANGE) goto end;
        break;

      case ButtonPress:
        gin->pX = event.xbutton.x;
        gin->pY = event.xbutton.y;
        gin->state = event.xbutton.state;
        gin->button = event.xbutton.button;
        gin->string[0] = '\0';
        gin->keysym = 0x20;
        if (mode==WAIT || mode==DOWN || mode==CHANGE) goto end;
        break;

      case KeyPress: // exit in error if ^C
        gin->pX = event.xkey.x;
        gin->pY = event.xkey.y;
        gin->state = event.xkey.state;
        nchars = XLookupString(&event.xkey, gin->string, PL_MAXKEY - 1, &mykey, NULL);
        gin->string[nchars] = '\0';
        gin->keysym = (unsigned int) mykey; 
        goto end;
        if (gin->state&0x4 && (gin->keysym==67 || gin->keysym==99)) 
        {
          status = false;
          goto end;
        }
        warp = false;
        dx = 0;
        dy = 0;
        switch (mykey) {
          case XK_Cancel:
          case XK_Break:
            status = false;
            goto end;
          case XK_Left:
            dx = -1;
            warp = true;
            break;
          case XK_Up:
            dy = -1;
            warp = true;
            break;
          case XK_Right:
            dx = 1;
            warp = true;
            break;
          case XK_Down:
            dy = 1;
            warp = true;
            break;
        }
        if (warp) {
          // Each modifier key added increases the multiplication factor by 5
          // Shift
          if (gin->state & 0x01) {
            dx *= 5;
            dy *= 5;
          }
          // Caps Lock
          if (gin->state & 0x02) {
            dx *= 5;
            dy *= 5;
          }
          // Control
          if (gin->state & 0x04) {
            dx *= 5;
            dy *= 5;
          }
          // Alt
          if (gin->state & 0x08) {
            dx *= 5;
            dy *= 5;
          }
          // Bounds checking so that we don't send cursor out of window
          x1 = gin->pX + dx;
          y1 = gin->pY + dy;
          if (x1 < xmin)
            dx = xmin - gin->pX;
          if (y1 < ymin)
            dy = ymin - gin->pY;
          if (x1 > xmax)
            dx = xmax - gin->pX;
          if (y1 > ymax)
            dy = ymax - gin->pY;
          XWarpPointer(xwd->display, dev->window, None, 0, 0, 0, 0, dx, dy);
          if (mode==CHANGE) goto end;
        }
        break;
      case MotionNotify:
        gin->pX = event.xmotion.x;
        gin->pY = event.xmotion.y;
        gin->state = event.xmotion.state;
        gin->string[0] = '\0';
        gin->keysym = 0x20;
        if (mode==CHANGE) goto end; // crosshair not available if we exit on motion!!!
        if(event.type==MotionNotify){
            x=event.xmotion.x; y=event.xmotion.y;
        }
        else {
            x=event.xcrossing.x; y=event.xcrossing.y;
        }
        break;
      default:
        break;
    }
  }
  end: 
  gin->pY = dev->height - gin->pY;
  gin->dX = (PLFLT) gin->pX / (dev->width - 1);
  gin->dY = (PLFLT) gin->pY / (dev->height - 1);
  if(grabbed)     XUngrabPointer(xwd->display,CurrentTime);
  if(kbgrabbed)   XUngrabKeyboard(xwd->display,CurrentTime);
  XFlush(xwd->display);
  return status;
}

#endif
