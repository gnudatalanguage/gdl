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
      display = XOpenDisplay(":0");//IDL also opens :0 when DISPLAY is not set.
      if (display == NULL) {
      valid = false;
      ThrowGDLException("Cannot connect to X server");
      }
    }
    XGetInputFocus(display, &term_window, &revert_to);
    //avoid doing impossible things if term_window is *not* a bona fide window.
    if (term_window == PointerRoot || term_window == None) term_window=0;
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
//  XFlush(xwd->display);
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
      GraphicsDevice::GetDevice()->TidyWindowsList(); //necessary since we removed TidyWindowList() from GraphicsMultiDevice::EventHandler()
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

void GDLXStream::GetGeometry(long& xSize, long& ySize) {
//for some reason, with the x11 driver, this is better than plstream::gpage to get actual sizes.
  XwDev *dev = (XwDev *) pls->dev;
  xSize=dev->width;
  ySize=dev->height;
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
  Clear(-1);
//  // this mimics better the *DL behaviour but plbop create a new page, etc..
//  //plclear clears only the current subpage. But it clears it. One has
//  //just to set the number of subpages to 1
//  PLINT red, green, blue;
//  DByte r, g, b;
//  PLINT red0, green0, blue0;
//
//  GraphicsDevice::GetCT( )->Get( 0, r, g, b );
//  red = r;
//  green = g;
//  blue = b;
////we get around the index 0=background color "feature" of plplot. GDL uses a separate backgroud color.
//  red0 = GraphicsDevice::GetDevice( )->BackgroundR( );
//  green0 = GraphicsDevice::GetDevice( )->BackgroundG( );
//  blue0 = GraphicsDevice::GetDevice( )->BackgroundB( );
//  plstream::scolbg( red0, green0, blue0 ); //overwrites col[0]
//  ::c_plbop( );
////  ::c_plclear( );
//  plstream::scolbg( red, green, blue ); //resets col[0]
}

void GDLXStream::Clear(DLong chan) {
  XwDev *dev = (XwDev *) pls->dev;
  XwDisplay *xwd = (XwDisplay *) dev->xwd;

  DByte r = (GraphicsDevice::GetDevice()->BackgroundR());
  DByte g = (GraphicsDevice::GetDevice()->BackgroundG());
  DByte b = (GraphicsDevice::GetDevice()->BackgroundB());
  unsigned long curcolor= 
    ((unsigned long)(r) << (ffs(xwd->visual->red_mask)-1) ) +
    ((unsigned long)(g) << (ffs(xwd->visual->green_mask)-1) ) +
    ((unsigned long)(b) << (ffs(xwd->visual->blue_mask)-1) );

  XSetForeground( xwd->display, dev->gc, curcolor); // myColor.pixel );

  switch(chan){
      case 0:
        XSetPlaneMask(xwd->display,dev->gc,xwd->visual->red_mask);
        break;
      case 1:
        XSetPlaneMask(xwd->display,dev->gc,xwd->visual->green_mask);
        break;
      case 2:
        XSetPlaneMask(xwd->display,dev->gc,xwd->visual->blue_mask);
        break;
    }
  if (dev->write_to_pixmap==1)
    XFillRectangle(xwd->display, dev->pixmap, dev->gc, 0, 0, dev->width, dev->height);
  if (dev->write_to_window)
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
BaseGDL* GDLXStream::GetFontnames(DString pattern) {
  if (pattern.length()<=0) return NULL;
  XwDev *dev = (XwDev *) pls->dev;
  XwDisplay *xwd = (XwDisplay *) dev->xwd;
  int actual_count_return;
  char **list= XListFonts(xwd->display, pattern.c_str(), 30000, &actual_count_return);
  if (!actual_count_return) return NULL;
  DStringGDL* myList=new DStringGDL(dimension(actual_count_return));
  for (int i=0; i< actual_count_return; ++i) (*myList)[i].assign(list[i]);
  XFreeFontNames(list);
  return myList;
}
DLong GDLXStream::GetFontnum(DString pattern){
  // never occurs ...  if (pattern.length()<0) return -1;
  if (this->GetFontnames(pattern) == NULL) return -1;
  if (pattern.length()==0) return 0;
  return this->GetFontnames(pattern)->N_Elements();
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
      XSetInputFocus(xwd->display, DefaultRootWindow(xwd->display),RevertToParent,CurrentTime);
    }
    return true;
  }  
  
// This helps cursor window leave focus.
//
  bool GDLXStream::setFocus(bool value=true)
  {
    XwDev *dev = (XwDev *) pls->dev;
    if( dev == NULL) return false;
    XwDisplay *xwd = (XwDisplay *) dev->xwd;
    XWMHints gestw;
    gestw.input = value;
    gestw.flags = InputHint;
    XSetWMHints(xwd->display, dev->window, &gestw);
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

void GDLXStream::UnMapWindow() {
  //Used for /PIXMAP windows: 1) insure write_to_pixmap and not write_to_window, and 2) hide the window.
  XwDev *dev = (XwDev *) pls->dev;
  dev->write_to_pixmap=1;
  dev->write_to_window=0;
  XwDisplay *xwd = (XwDisplay *) dev->xwd;
  XWithdrawWindow(xwd->display, dev->window, xwd->screen);
  usedAsPixmap=true;
}
void GDLXStream::Flush() {
//  XwDev *dev = (XwDev *) pls->dev;
//  XwDisplay *xwd = (XwDisplay *) dev->xwd;
//  XFlush(xwd->display);
}
void GDLXStream::Update() {
  XFlush(static_cast<XwDisplay *>(static_cast<XwDev *>(pls->dev)->xwd)->display);
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
  setFocus(false);  // first try to get out of focus.
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
//  XFlush(xwd->display);
  return status;
}

bool GDLXStream::PaintImage(unsigned char *idata, PLINT nx, PLINT ny, DLong *pos,
  DLong trueColorOrder, DLong chan) {

  XwDev *dev = (XwDev *) pls->dev;
  XwDisplay *xwd = (XwDisplay *) dev->xwd;
  
  //how much do we need?
  PLINT xoff = (PLINT) pos[0]; //(pls->wpxoff / 32767 * dev->width + 1);
  PLINT yoff = (PLINT) pos[2]; //(pls->wpyoff / 24575 * dev->height + 1);
  PLINT xmax = dev->width - xoff;
  PLINT ymax = dev->height - yoff;
  if (nx < xmax) xmax = nx;
  if (ny < ymax) ymax = ny;

  PLINT rint[ctSize], gint[ctSize], bint[ctSize];
  //load original table
  GDLCT* actCT = GraphicsDevice::GetCT();
  actCT->Get(rint,gint,bint,ctSize);
  DLong decomposed=GraphicsDevice::GetDevice()->GetDecomposed();
//define & populate if necessary XImage img.  
  XImage *ximg = NULL;
  if (chan>0) { //we need to get the destination part of screen image back to write on it
    int x, y;
//   XFlush(xwd->display); //could be overkill...

    int (*oldErrorHandler)(Display*, XErrorEvent*);
    oldErrorHandler = XSetErrorHandler(GetImageErrorHandler);
    if (dev->write_to_pixmap == 1) {
      ximg = XGetImage(xwd->display, dev->pixmap, xoff, dev->height-yoff-ymax, xmax, ymax, AllPlanes, ZPixmap);
    } else {
      ximg = XGetImage(xwd->display, dev->window, xoff, dev->height-yoff-ymax, xmax, ymax, AllPlanes, ZPixmap);
    }
    if (ximg == NULL) { //last chance!!!
//      XSync(xwd->display, 0); //could be overkill...
      x = 0;
      y = 0;
      if (dev->write_to_pixmap == 1) {
        XCopyArea(xwd->display, dev->pixmap, dev->window, dev->gc, xoff, dev->height-yoff-ymax, xmax, ymax, x, y);
//        XSync(xwd->display, 0); //could be overkill...
      }
    }
    XSetErrorHandler(oldErrorHandler);
    if (ximg == NULL) {
      cerr << "Unhandled unsuccessful XCopyArea, returning." << endl;
      return false;
    }
  } else {
    void *imgData=malloc(xmax*ymax*4); //too much size allocated, but keeps X11 happy (no need of scanline length computation)
    ximg = XCreateImage(xwd->display, xwd->visual, xwd->depth, ZPixmap, 0, (char*)imgData , xmax, ymax, 8, 0); //8 seems reasonable, see XCreateImage doc.
  }

  PLINT ix, iy;
  XColor curcolor;
  curcolor = xwd->fgcolor; //default
  PLINT iclr1, ired, igrn, iblu;
  // parallelize does not work when using XGet[Put]Pixel in the loop below, otherwise would be OK!
  // please allow parallelization only after removing this problem ;^)
  //#ifdef _OPENMP
  //  SizeT nOp = kxLimit * kyLimit;
  //#endif
  //  #pragma omp parallel if (nOp >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nOp)) private(ired,igrn,iblu,kx,ky,iclr1,curcolor)
  //  {
  //  #pragma omp for
  for (ix = 0; ix < xmax; ++ix) {
    for (iy = 0; iy < ymax; ++iy) {

      if (xwd->color) {
        if (trueColorOrder == 0 && chan == 0) {
           
          iclr1 = idata[iy * nx + ix];
          if (decomposed == 1){
            curcolor.pixel = iclr1 * 256 * 256 + iclr1 * 256 + iclr1;
          } else {
            if (xwd->rw_cmap==0) {
              ired = rint[iclr1];
              igrn = gint[iclr1];
              iblu = bint[iclr1];
            } else {
              ired = pls->cmap0[iclr1].r;
              igrn = pls->cmap0[iclr1].g;
              iblu = pls->cmap0[iclr1].b;
            }
            curcolor.pixel = ired * 256 * 256 + igrn * 256 + iblu;
          }
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
          } else { //channel mode -> ximg was got from display -> is reversed
            unsigned long pixel = XGetPixel(ximg, ix, ymax-1-iy);
            if (chan == 1) { //1 byte bitmap passed
              pixel &= 0x00ffff;
              ired = idata[1 * (iy * nx + ix) + 0];
              curcolor.pixel = ired * 256 * 256 + pixel;
            } else if (chan == 2) {
              pixel &= 0xff00ff;
              igrn = idata[1 * (iy * nx + ix) + 0];
              curcolor.pixel = igrn * 256 + pixel;
            } else if (chan == 3) {
              pixel &= 0xffff00;
              iblu = idata[1 * (iy * nx + ix) + 0];
              curcolor.pixel = iblu + pixel;
            }
          }
        }
      }
      XPutPixel(ximg, ix, ymax-1-iy, curcolor.pixel); //ximg IS reversed allways.
    }
  }
  //  } //end parallelize 
  if (dev->write_to_pixmap == 1)
    XPutImage(xwd->display, dev->pixmap, dev->gc, ximg, 0, 0,
    xoff, dev->height-yoff-ymax, xmax, ymax);

  if (dev->write_to_window==1) //always write
    XPutImage(xwd->display, dev->window, dev->gc, ximg, 0, 0,
    xoff, dev->height-yoff-ymax, xmax, ymax);

  XDestroyImage(ximg);
  return true;
}

void GDLXStream::Color( ULong color, DLong decomposed ) {
  XwDev *dev = (XwDev *) pls->dev;
  XwDisplay *xwd = (XwDisplay *) dev->xwd;
  if (xwd->rw_cmap)  { //not treated here, revert to safety with plplot's overhead.
    if ( decomposed == 0 ) { //get current colormap
      plstream::col0( color & 0xFF ); //just set color index [0..255]. simple and fast.
    } else {
      GDLGStream::SetColorMap1SingleColor(color);
      plstream::col1(1);
    }
    return;
  }
  unsigned int curcolor;
  DByte r, g, b;
  if ( decomposed == 0 ) { //get current colormap
    //load original table
    GDLCT* actCT = GraphicsDevice::GetCT();
    actCT->Get(color & 0xFF,r,g,b);
  } else { 
  //truecolor=decomposed case
    r = (color & 0xFF);
    g = (color >> 8 & 0xFF);
    b = (color >> 16 & 0xFF);
  }
  curcolor= ( (unsigned long) (r) << (ffs(xwd->visual->red_mask)-1)) +
    ((unsigned long)(g) << (ffs(xwd->visual->green_mask)-1) ) +
    ((unsigned long)(b) << (ffs(xwd->visual->blue_mask)-1) );
  XSetForeground( xwd->display, dev->gc, curcolor);
}

//Read X11 bitmapdata -- normally on 4BPP=Allplanes, return 3BPP ignoring Alpha plane.
DByteGDL* GDLXStream::GetBitmapData() {
//  plstream::cmd( PLESC_FLUSH, NULL );
  GraphicsDevice* actDevice = GraphicsDevice::GetDevice();
  
  XwDev *dev = (XwDev *) pls->dev;
  XwDisplay *xwd = (XwDisplay *) dev->xwd;
  XImage *ximg = NULL;
  XWindowAttributes win_attr;

  /* query the window's attributes. */
  Status rc = XGetWindowAttributes(xwd->display, dev->window, &win_attr);
  unsigned int nx = win_attr.width;
  unsigned int ny = win_attr.height;

    int (*oldErrorHandler)(Display*, XErrorEvent*);
    oldErrorHandler = XSetErrorHandler(GetImageErrorHandler);
    if (dev->write_to_pixmap==1) {
      ximg = XGetImage(xwd->display, dev->pixmap, 0, 0, nx, ny, AllPlanes, ZPixmap);
    } else {
      ximg = XGetImage( xwd->display, dev->window, 0, 0, nx, ny, AllPlanes, ZPixmap);
    }
    XSetErrorHandler(oldErrorHandler);
    
    if (ximg == NULL) return NULL;
    if (ximg->bits_per_pixel != 32) return NULL;

    SizeT datadims[3];
    datadims[0] = nx;
    datadims[1] = ny;
    datadims[2] = 3;
    dimension datadim(datadims, (SizeT) 3);
    DByteGDL *bitmap = new DByteGDL( datadim, BaseGDL::NOZERO);
    //PADDING is 4BPP -- we take 3BPP and revert Y to respect IDL default
    SizeT kpad = 0;
    for ( SizeT iy =0; iy < ny ; ++iy ) {
      for ( SizeT ix = 0; ix < nx; ++ix ) {
        (*bitmap)[3 * ((ny-1-iy) * nx + ix) + 2] = ximg->data[kpad++];
        (*bitmap)[3 * ((ny-1-iy) * nx + ix) + 1] = ximg->data[kpad++];
        (*bitmap)[3 * ((ny-1-iy) * nx + ix) + 0] = ximg->data[kpad++];
        kpad++; //pad to 4
      }
    }
    XDestroyImage(ximg);
    return bitmap;
}

#endif
