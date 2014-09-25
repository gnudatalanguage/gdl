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
#include "devicex.hpp"

#ifndef HAVE_X
#else

using namespace std;

void GDLXStream::Init() {
  // plstream::init() calls exit() if it cannot establish a connection with X-server
  bool okToRevertToTerminal=false;
  int revert_to;
  {
    Display* display = XOpenDisplay(NULL);
    if (display == NULL) {
      valid = false;
      ThrowGDLException("Cannot connect to X server");
    }
    XGetInputFocus(display, &term_window, &revert_to);
    XCloseDisplay(display);
  }

  this->plstream::init();

  XwDev *dev = (XwDev *) pls->dev;
  XwDisplay *xwd = (XwDisplay *) dev->xwd;

  wm_protocols = XInternAtom(xwd->display, "WM_PROTOCOLS", false);
  wm_delete_window = XInternAtom(xwd->display, "WM_DELETE_WINDOW", false);

  XSetWMProtocols(xwd->display, dev->window, &wm_delete_window, 1);
  //give back focus to caller -- hopefully the terminal.
  XWindowAttributes from_attr;
  if(term_window) {
    XGetWindowAttributes(xwd->display,term_window,&from_attr);
    if(from_attr.map_state==IsViewable) okToRevertToTerminal=true;
  }
  if ( okToRevertToTerminal ) XSetInputFocus(xwd->display,term_window,RevertToParent,CurrentTime);
  else UnsetFocus(); //desperate method, since it prevents iconifying etc...
  XFlush(xwd->display);
  GraphicsDevice* actDevice = GraphicsDevice::GetDevice();
  //current cursor:
  CursorStandard(actDevice->getCursorId());
  //current graphics function
  SetGraphicsFunction(actDevice->GetGraphicsFunction());
  //BackingStore 
  SetBackingStore(actDevice->getBackingStore());
}

void GDLXStream::EventHandler() {
  if (!valid) return;

  XwDev *dev = (XwDev *) pls->dev;

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

bool GDLXStream::SetGraphicsFunction( long value) {
    XGCValues gcValues;
    gcValues.function = (value<0)?0:(value>15)?15:value;
    XwDev *dev = (XwDev *) pls->dev;
    XwDisplay *xwd = (XwDisplay *) dev->xwd;
    return XChangeGC( xwd->display, dev->gc, GCFunction, &gcValues );
}

bool GDLXStream::GetWindowPosition(long& xpos, long& ypos ) {
  XwDev *dev = (XwDev *) pls->dev;
  XwDisplay *xwd = (XwDisplay *) dev->xwd;
  XWindowAttributes wa;
  int addx, addy;
  Window child;
  XGetWindowAttributes( xwd->display, dev->window, &wa );
  if ( XTranslateCoordinates( xwd->display, dev->window, wa.root, 0, 0, &addx, &addy, &child ) ) {
    xpos = addx - wa.x;
    ypos = DisplayHeight( xwd->display, DefaultScreen( xwd->display ) ) - wa.height + 1 - (addy - wa.y) + 1;
    return true;
  } else return false;
}

void GDLXStream::GetGeometry(long& xSize, long& ySize, long& xOffset, long& yOffset) {

  XwDev *dev = (XwDev *) pls->dev;
  XwDisplay *xwd = (XwDisplay *) dev->xwd;
  XWindowAttributes wa;
  /* query the window's attributes. */
  if (XGetWindowAttributes(xwd->display, dev->window, &wa)){
    int addx,addy;
    Window child;
    XTranslateCoordinates( xwd->display, dev->window, wa.root, 0, 0, &addx, &addy, &child );
    xSize=wa.width;
    ySize=wa.height;
    xOffset = addx-wa.x;
    yOffset = dev->height-addy+wa.y+1;
  } else GDLGStream::GetGeometry(xSize, ySize, xOffset, yOffset);
  
  int debug=0;
  if (debug) {
    int screen_num, screen_width, screen_height;
    screen_num = DefaultScreen(xwd->display);
    screen_width = DisplayWidth(xwd->display, screen_num);
    screen_height = DisplayHeight(xwd->display, screen_num);
    cout << "---- Begin Inside GetX11Geometry ----" << endl;
    cout << "display size : " << screen_width << " " << screen_height << endl;
    cout << "win_attributes W/H: " << wa.width << " " << wa.height << endl;
    cout << "win_attributes border width: " << wa.border_width << endl;
    cout << "win_attributes X/Y: " << wa.x << " " << wa.y << endl;
    cout << "results: " << endl;
    cout << "xSize/ySize: " << xSize << " " << ySize << endl;
    cout << "xOffset/yOffset: " << xOffset << " " << yOffset << endl;
    cout << "---- End Inside GetX11Geometry ----" << endl;
  }

}

  bool GDLXStream::CursorStandard(int cursorNumber)
  {
    int num=max(0,min(XC_num_glyphs-1,cursorNumber));
    XwDev *dev = (XwDev *) pls->dev;
    XwDisplay *xwd = (XwDisplay *) dev->xwd;
    XDefineCursor(xwd->display,dev->window,XCreateFontCursor(xwd->display,num));
    return true;
  }
  
// plplot 5.3 does not provide the clear function for c++

void GDLXStream::Clear() {
  // dummy call to get private function set_stream() called
  // gd: tested, fail to see why use this dummy call ?
//  char dummy;
//  gesc(&dummy);
  // this mimics better the *DL behaviour but plbob create a new page, etc..
  ::c_plbop();
  //plclear clears only the current subpage. But it clears it. One has
  //just to set the number of subpages to 1
  ::c_plclear();
}

void GDLXStream::Clear(DLong chan) {
  static const int planemask[3]={0x0000FF,0x00FF00,0xFF0000};
//fill screen with background (since plotting erase has called background) on channel chan
  XwDev *dev = (XwDev *) pls->dev;
  XwDisplay *xwd = (XwDisplay *) dev->xwd;
  XSetForeground(xwd->display,dev->gc,xwd->cmap0[0].pixel);
  XSetPlaneMask(xwd->display,dev->gc,planemask[chan]);
  if (dev->write_to_pixmap)
    XFillRectangle(xwd->display, dev->pixmap, dev->gc, 0, 0, dev->width, dev->height);
  if (1) // not (dev->write_to_window): always!
    XFillRectangle(xwd->display, dev->window, dev->gc, 0, 0, dev->width, dev->height);
  XSetForeground(xwd->display,dev->gc,dev->curcolor.pixel);
  XSetPlaneMask(xwd->display,dev->gc,AllPlanes);
}
  
unsigned long GDLXStream::GetWindowDepth() {
  XwDev *dev = (XwDev *) pls->dev;
  XwDisplay *xwd = (XwDisplay *) dev->xwd;
  return xwd->depth;
}
DLong GDLXStream::GetVisualDepth() {
  XwDev *dev = (XwDev *) pls->dev;
  XwDisplay *xwd = (XwDisplay *) dev->xwd;
  XWindowAttributes wa;
  if(XGetWindowAttributes( xwd->display, dev->window, &wa )) return (long)wa.depth;
  else return -1;
}
DString GDLXStream::GetVisualName() {
  static const char* visual_classes_names[] = {
   "StaticGray" , 
   "GrayScale" ,
   "StaticColor" ,
    "PseudoColor" ,
   "TrueColor" ,
   "DirectColor" };
  XwDev *dev = (XwDev *) pls->dev;
  XwDisplay *xwd = (XwDisplay *) dev->xwd;
  XWindowAttributes wa;
  if(XGetWindowAttributes( xwd->display, dev->window, &wa )) {
    /* need some works to go to Visual Name */
    int junk;
    XVisualInfo vistemplate, *vinfo; 
    vistemplate.visualid = XVisualIDFromVisual(wa.visual);
    vinfo = XGetVisualInfo(xwd->display, VisualIDMask, &vistemplate, &junk);
    if (vinfo->c_class < 5){
        std::string ret;
        ret=std::string(visual_classes_names[vinfo->c_class]);
        return ret;
    } else return "";
  }
  else return "";
  }

  bool GDLXStream::UnsetFocus()
  {
    XwDev *dev = (XwDev *) pls->dev;
    if( dev == NULL) return false;
    XwDisplay *xwd = (XwDisplay *) dev->xwd;
    if(term_window) {
      XWindowAttributes from_attr;
      XGetWindowAttributes(xwd->display,term_window,&from_attr);
      if(from_attr.map_state==IsViewable) XSetInputFocus(xwd->display,term_window,RevertToParent,CurrentTime);
    } 
    else 
    { 
      XWMHints gestw;
      gestw.input = FALSE;
      gestw.flags = InputHint;
      XSetInputFocus(xwd->display, DefaultRootWindow(xwd->display),RevertToParent,CurrentTime);
    }
    return true;
  }  
  
  bool GDLXStream::SetBackingStore(int value)
  {
    XwDev *dev = (XwDev *) pls->dev;
    if( dev == NULL) return false;
    XwDisplay *xwd = (XwDisplay *) dev->xwd;
    XSetWindowAttributes attr;
    if (value > 0)
    {
      attr.backing_store = Always;
    }
    else
    {
      attr.backing_store = NotUseful;
    }
    XChangeWindowAttributes(xwd->display, dev->window,CWBackingStore,&attr);
    return true;
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
// really hides the window  XWithdrawWindow(xwd->display, dev->window, xwd->screen);
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
#define ToXColor(a) (((0xFF & (a)) << 8) | (a))

bool GDLXStream::PaintImage(unsigned char *idata, PLINT nx, PLINT ny, DLong *pos,
        DLong trueColorOrder, DLong chan) {

  PLINT ix, iy;
  XwDev *dev = (XwDev *) pls->dev;
  XwDisplay *xwd = (XwDisplay *) dev->xwd;
  XImage *ximg = NULL, *ximg_pixmap = NULL;

  int x, y;

  int (*oldErrorHandler)(Display*, XErrorEvent*);

  oldErrorHandler = XSetErrorHandler(DeviceX::GetImageErrorHandler);

  XFlush(xwd->display);
  
  if (dev->write_to_pixmap) {
    ximg = XGetImage(xwd->display, dev->pixmap, 0, 0,
            dev->width, dev->height,
            AllPlanes, ZPixmap);
    ximg_pixmap = ximg;
  }

  if (dev->write_to_window)
    ximg = XGetImage(xwd->display, dev->window, 0, 0,
          dev->width, dev->height,
          AllPlanes, ZPixmap);

  XSetErrorHandler(oldErrorHandler);

  if (ximg == NULL) {
    XSync(xwd->display, 0);
    x = 0;
    y = 0;
    if (dev->write_to_pixmap) {
      XCopyArea(xwd->display, dev->pixmap, dev->window, dev->gc,
              x, y, dev->width, dev->height, x, y);
      XSync(xwd->display, 0);
      ximg = ximg_pixmap;
    }
  }


  int ncolors;
  PLINT iclr1, ired, igrn, iblu;
  if (trueColorOrder == 0 && chan == 0) {

    ncolors = 256;

    if (xwd->ncol1 != ncolors) {
      //was free_mem from plplotP.h which is forbidden - thanks to GJ for pointing this.
      if ( xwd->cmap1 != NULL ) { free( (void *)  xwd->cmap1); xwd->cmap1 = NULL; } 
      xwd->cmap1 = (XColor *) calloc(ncolors, (size_t) sizeof (XColor));
    }
    //#endif

    for (SizeT i = 0; i < ncolors; i++) {

      xwd->cmap1[i].red = ToXColor(pls->cmap0[i].r);
      xwd->cmap1[i].green = ToXColor(pls->cmap0[i].g);
      xwd->cmap1[i].blue = ToXColor(pls->cmap0[i].b);
      xwd->cmap1[i].flags = DoRed | DoGreen | DoBlue;

      if (XAllocColor(xwd->display, xwd->map, &xwd->cmap1[i]) == 0)
        break;
    }
    xwd->ncol1 = ncolors;
  }
  PLINT xoff = (PLINT) pos[0]; //(pls->wpxoff / 32767 * dev->width + 1);
  PLINT yoff = (PLINT) pos[2]; //(pls->wpyoff / 24575 * dev->height + 1);
  PLINT kx, ky;

  XColor curcolor;

  PLINT kxLimit = dev->width - xoff;
  PLINT kyLimit = dev->height - yoff;

  if (nx < kxLimit) kxLimit = nx;
  if (ny < kyLimit) kyLimit = ny;

  /*#ifdef _OPENMP
  SizeT nOp = kxLimit * kyLimit;
#endif
  #pragma omp parallel if (nOp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nOp)) private(ired,igrn,iblu,kx,ky,iclr1,curcolor)
  {
  #pragma omp for*/
  for (ix = 0; ix < kxLimit; ++ix) {
    for (iy = 0; iy < kyLimit; ++iy) {

      kx = xoff + ix;
      ky = yoff + iy;

      if (trueColorOrder == 0 && chan == 0) {
        iclr1 = idata[iy * nx + ix];

        if (xwd->color)
          curcolor = xwd->cmap1[iclr1];
        else
          curcolor = xwd->fgcolor;

        //	  printf("ix: %d  iy: %d  pixel: %d\n", ix,iy,curcolor.pixel);

      } else {
        if (chan == 0) {
          if (trueColorOrder == 1) {
            ired = idata[3 * (iy * nx + ix) + 0];
            igrn = idata[3 * (iy * nx + ix) + 1];
            iblu = idata[3 * (iy * nx + ix) + 2];
          } else if (trueColorOrder == 2) {
            ired = idata[nx * (iy * 3 + 0) + ix];
            igrn = idata[nx * (iy * 3 + 1) + ix];
            iblu = idata[nx * (iy * 3 + 2) + ix];
          } else if (trueColorOrder == 3) {
            ired = idata[nx * (0 * ny + iy) + ix];
            igrn = idata[nx * (1 * ny + iy) + ix];
            iblu = idata[nx * (2 * ny + iy) + ix];
          }
          curcolor.pixel = ired * 256 * 256 + igrn * 256 + iblu;
        }else{
          unsigned long pixel = XGetPixel(ximg, kx, dev->height - 1 - ky);
          if (chan == 1) {
            pixel &= 0x00ffff;
            ired = idata[1 * (iy * nx + ix) + 0];
            curcolor.pixel = ired * 256 * 256 + pixel;
          } else if (chan == 2) {
            pixel &= 0xff00ff;
            igrn = idata[1 * (iy * nx + ix) + 1];
            curcolor.pixel = igrn * 256 + pixel;
          } else if (chan == 3) {
            pixel &= 0xffff00;
            iblu = idata[1 * (iy * nx + ix) + 2];
            curcolor.pixel = iblu + pixel;
          }
        }
      }

      //std::cout << "XPutPixel: "<<kx<<"  "<< dev->height-ky-1 << std::endl;
      // TODO check if XPutPixel() and XGetPixel() are thread save
      if (ky < dev->height && kx < dev->width)
        XPutPixel(ximg, kx, dev->height - 1 - ky, curcolor.pixel);
    }
  }
  //}

  if (dev->write_to_pixmap)
    XPutImage(xwd->display, dev->pixmap, dev->gc, ximg, 0, 0,
          0, 0, dev->width, dev->height);

  if (1) //(dev->write_to_window) //always write
    XPutImage(xwd->display, dev->window, dev->gc, ximg, 0, 0,
          0, 0, dev->width, dev->height);

  if (ximg != ximg_pixmap) {
    XDestroyImage(ximg);
    XDestroyImage(ximg_pixmap);
  } else {
    XDestroyImage(ximg);
  }
  return true;
}
#undef ToXColor


#endif
