/* *************************************************************************
   devicex.hpp  -  X windows device
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

#ifndef DEVICEX_HPP_
#define DEVICEX_HPP_

#ifndef HAVE_X
#else

#include <algorithm>
#include <vector>
#include <cstring>

#include <plplot/drivers.h>

#include "graphicsdevice.hpp"
#include "gdlxstream.hpp"
#include "initsysvar.hpp"
#include "gdlexception.hpp"

#ifdef HAVE_OLDPLPLOT
#define SETOPT SetOpt
#else
#define SETOPT setopt
#endif

//defined in graphicsdevice.hpp
//#define MAX_WIN 32  //IDL free and widgets start at 32 ...
//#define MAX_WIN_RESERVE 256

class DeviceX : public GraphicsMultiDevice {
    
public:
    
    DeviceX() : GraphicsMultiDevice( -1, XC_crosshair, 3, 0) {
        name = "X";
        DLongGDL origin(dimension(2));
        DLongGDL zoom(dimension(2));
        zoom[0] = 1;
        zoom[1] = 1;
        Display* display = XOpenDisplay(NULL);
        if (display != NULL) {
            int Depth;
            Depth=DefaultDepth(display, DefaultScreen(display));      
            decomposed = (Depth >= 15 ? 1 : 0);
        }

        dStruct = new DStructGDL("!DEVICE");
        dStruct->InitTag("NAME",       DStringGDL(name));
        dStruct->InitTag("X_SIZE",     DLongGDL(640));
        dStruct->InitTag("Y_SIZE",     DLongGDL(512));
        dStruct->InitTag("X_VSIZE",    DLongGDL(640));
        dStruct->InitTag("Y_VSIZE",    DLongGDL(512));
        dStruct->InitTag("X_CH_SIZE",  DLongGDL(6));
        dStruct->InitTag("Y_CH_SIZE",  DLongGDL(9));
        dStruct->InitTag("X_PX_CM",    DFloatGDL(40.0));
        dStruct->InitTag("Y_PX_CM",    DFloatGDL(40.0));
        dStruct->InitTag("N_COLORS",   DLongGDL( (decomposed==1)?256*256*256:256)); 
        dStruct->InitTag("TABLE_SIZE", DLongGDL(ctSize));
        dStruct->InitTag("FILL_DIST",  DLongGDL(1));
        dStruct->InitTag("WINDOW",     DLongGDL(-1));
        dStruct->InitTag("UNIT",       DLongGDL(0));
        dStruct->InitTag("FLAGS",      DLongGDL(328124));
        dStruct->InitTag("ORIGIN",     origin); 
        dStruct->InitTag("ZOOM",       zoom); 

    }
  
  bool WOpen( int wIx, const std::string& title, 
	      int xSize, int ySize, int xPos, int yPos, bool hide=false)
  {


    if( wIx >= winList.size() || wIx < 0) return false;

    if( winList[ wIx] != NULL) winList[ wIx]->SetValid(false);

    TidyWindowsList();

    // set initial window size
    PLFLT xp; PLFLT yp; 
    PLINT xleng; PLINT yleng;
    PLINT xoff; PLINT yoff;

    DLong xMaxSize, yMaxSize;
    DeviceX::MaxXYSize(&xMaxSize, &yMaxSize);

    bool noPosx=(xPos==-1);
    bool noPosy=(yPos==-1);
    xPos=max(1,xPos); //starts at 1 to avoid problems plplot!
    yPos=max(1,yPos);

    xleng = min(xSize,xMaxSize); if (xPos+xleng > xMaxSize) xPos=xMaxSize-xleng-1;
    yleng = min(ySize,yMaxSize); if (yPos+yleng > yMaxSize) yPos=yMaxSize-yleng-1;
// dynamic allocation needed!    
    PLINT Quadx[4]={xMaxSize-xleng-1,xMaxSize-xleng-1,1,1};
    PLINT Quady[4]={1,yMaxSize-yleng-1,1,yMaxSize-yleng-1};
    if (noPosx && noPosy) { //no init given, use 4 quadrants:
      xoff = Quadx[wIx%4];
      yoff = Quady[wIx%4];
    } else if (noPosx) {
      xoff = Quadx[wIx%4];
      yoff = yMaxSize-yPos-yleng;
    } else if (noPosy) {
      xoff = xPos;
      yoff = Quady[wIx%4];
    } else {
      xoff  = xPos;
      yoff  = yMaxSize-yPos-yleng;
    }
    //apparently this is OK to get same results as IDL on X11...
    yoff+=1;
    xp=1;
    yp=1;
    
    winList[ wIx] = new GDLXStream( xleng, yleng);
    
    oList[ wIx]   = oIx++;    
    winList[ wIx]->spage( xp, yp, xleng, yleng, xoff, yoff); //must be before 'Init'

    // no pause on win destruction
    winList[ wIx]->spause( false);

    // extended fonts
    winList[ wIx]->fontld( 1);

    // we want color
    winList[ wIx]->scolor( 1);

    // window title
    static char buf[ 256];
    strncpy( buf, title.c_str(), 255);
    buf[ 255] = 0;
//    winList[ wIx]->setopt( "db", 0); //handled elsewhere
//    winList[ wIx]->setopt( "debug", 0);
    winList[ wIx]->SETOPT( "plwindow", buf);
// Do not init colors --- we handle colors ourseves, very much faster!
    winList[ wIx]->SETOPT( "drvopt","noinitcolors=1");

// Please no threads, no gain especially in remote X11 
   winList[ wIx]->SETOPT( "drvopt","usepth=0");

//all the options must be passed BEFORE INIT=plinit.
    winList[ wIx]->Init();
    // get actual size, and resize to it (overcomes some window managers problems, solves bug #535)
    // bug #535 had other causes. removed until further notice.
    //    bool success = WSize( wIx ,&xleng, &yleng, &xoff, &yoff);
    //    ResizeWin((UInt)xleng, (UInt) yleng);
    // need to be called initially. permit to fix things
    winList[ wIx]->ssub(1,1);
    winList[ wIx]->adv(0);
    // load font
    winList[ wIx]->font( 1);
    winList[ wIx]->vpor(0,1,0,1);
    winList[ wIx]->wind(0,1,0,1);
    winList[ wIx]->DefaultCharSize();
    //in case these are not initalized, here is a good place to do it.
    if (winList[ wIx]->updatePageInfo()==true)
    {
      winList[ wIx]->GetPlplotDefaultCharSize(); //initializes everything in fact..
    }
    // sets actWin and updates !D
    SetActWin( wIx);
    bool success;
    if ( hide )
    {
      success=this->Hide();
    }
    else success=this->UnsetFocus();
    return true; //winList[ wIx]->Valid(); // Valid() need to called once
    }
  
    GDLGStream* GetStream(bool open = true) {
        TidyWindowsList();
        if (actWin == -1) {
            if (!open) return NULL;

            DString title = "GDL 0";
            DLong xSize, ySize;
            DefaultXYSize(&xSize, &ySize);
            bool success = WOpen(0, title, xSize, ySize, -1, -1, false);
            if (!success)
                return NULL;
            if (actWin == -1) {
                std::cerr << "Internal error: plstream not set." << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        return winList[actWin];
    }

    bool SetGraphicsFunction(DLong value) {
        gcFunction = max(0, min(value, 15));
    this->GetStream(); //to open a window if none opened.
    bool ret=false;
        for (int i = 0; i < winList.size(); i++) {
            if (winList[i] != NULL) {
              ret = winList[i]->SetGraphicsFunction(gcFunction);
              if (ret == false) return ret;
      }
    }
    return true;
  }
  
    DLong GetGraphicsFunction() {
    this->GetStream(); //MUST open a window if none opened (even  if it is not useful with GDL, this is to mimic IDL).
    return gcFunction;
  }
  
    DIntGDL* GetScreenSize(char* disp) { 
      Display* display = XOpenDisplay(disp);
      if (display == NULL) ThrowGDLException("Cannot connect to X server");
      int screen_num, screen_width, screen_height;
      screen_num = DefaultScreen(display);
      screen_width = DisplayWidth(display, screen_num);
      screen_height = DisplayHeight(display, screen_num);
      XCloseDisplay(display);

      DIntGDL* res;
      res = new DIntGDL(2, BaseGDL::NOZERO);
      (*res)[0]= screen_width;
      (*res)[1]= screen_height;
      return res;
    }
    
    DDoubleGDL* GetScreenResolution(char* disp)
    { 
      Display* display = XOpenDisplay(disp);
      if (display == NULL) ThrowGDLException("Cannot connect to X server: "+string(disp));
      int screen_num, screen_width, screen_height;
      screen_num = DefaultScreen(display);
      screen_width = DisplayWidth(display, screen_num);
      screen_height = DisplayHeight(display, screen_num);
      int screen_width_mm;
      int screen_height_mm;
      screen_width_mm = DisplayWidthMM(display, screen_num);
      screen_height_mm = DisplayHeightMM(display, screen_num);
      XCloseDisplay(display);

      DDoubleGDL* resolution;
      resolution = new DDoubleGDL(2, BaseGDL::NOZERO);
      (*resolution)[0]=(screen_width_mm/10.)/screen_width;
      (*resolution)[1]=(screen_height_mm/10.)/screen_height;
      return resolution;
    }

    DIntGDL* GetWindowPosition() {
        this->GetStream(); //MUST open a window if none opened.
        long xpos, ypos;
        if (winList[actWin]->GetWindowPosition(xpos, ypos)) {
            DIntGDL* res;
            res = new DIntGDL(2, BaseGDL::NOZERO);
            (*res)[0] = xpos;
            (*res)[1] = ypos;
            return res;
        } else return NULL;
        }
    
    DLong GetVisualDepth() {
        TidyWindowsList();
        if (actWin == -1) {
          this->GetStream(true); //this command SHOULD NOT open a window if none opened.
          DLong val=winList[actWin]->GetVisualDepth();
          WDelete(actWin);
          return val;
        } else {
          return winList[actWin]->GetVisualDepth();
        }
    }

    DString GetVisualName() {
        TidyWindowsList();
        if (actWin == -1) {
          this->GetStream(true); //this command SHOULD NOT open a window if none opened.
          DString val=winList[actWin]->GetVisualName();
          WDelete(actWin);
          return val;
        } else {
          return winList[actWin]->GetVisualName();
        }
    }
    BaseGDL* GetFontnames(){
        TidyWindowsList();
        if (actWin == -1) {
          this->GetStream(true); //this command SHOULD NOT open a window if none opened.
          BaseGDL* val=winList[actWin]->GetFontnames(fontname);
          WDelete(actWin);
          return val;
        } else {
          return winList[actWin]->GetFontnames(fontname);
        }
    }
    DLong GetFontnum(){
        TidyWindowsList();
        if (actWin == -1) {
          this->GetStream(true); //this command SHOULD NOT open a window if none opened.
          DLong val=winList[actWin]->GetFontnum(fontname);
          WDelete(actWin);
          return val;
        } else {
          return winList[actWin]->GetFontnum(fontname);
        }
    }    
    bool CursorStandard(int cursorNumber) {
        cursorId = cursorNumber;
    this->GetStream(); //to open a window if none opened.
    bool ret;
        for (int i = 0; i < winList.size(); i++) {
            if (winList[i] != NULL) {
              ret = winList[i]->CursorStandard(cursorNumber);
              if (ret == false) return ret;
      }
    }
    return true;
  }
  
  bool CursorCrosshair()
  {
    return CursorStandard(XC_crosshair);
  }
  
  void DefaultXYSize(DLong *xSize, DLong *ySize) {
    *xSize = 640;
    *ySize = 512;

    Display* display = XOpenDisplay(NULL);
    if (display != NULL)
      {   
	*xSize = DisplayWidth(display, DefaultScreen(display)) / 2; 
	*ySize = DisplayHeight(display, DefaultScreen(display)) / 2;
	XCloseDisplay(display);
      }   

    bool noQscreen=true;
    string gdlQscreen=GetEnvString("GDL_GR_X_QSCREEN");
    if( gdlQscreen == "1") noQscreen=false;
    string gdlXsize=GetEnvString("GDL_GR_X_WIDTH");
    if( gdlXsize != "" && noQscreen ) *xSize=atoi(gdlXsize.c_str()); 
    string gdlYsize=GetEnvString("GDL_GR_X_HEIGHT");
    if( gdlYsize != "" && noQscreen) *ySize=atoi(gdlYsize.c_str()); 
  }
  
  void MaxXYSize(DLong *xSize, DLong *ySize) {
    *ySize = 640;
    *ySize = 512;

    Display* display = XOpenDisplay(NULL);
    if (display != NULL)
      {
	*xSize = DisplayWidth(display, DefaultScreen(display));
	*ySize = DisplayHeight(display, DefaultScreen(display));
	XCloseDisplay(display);
      }
    }
void TidyWindowsList() {
  int wLSize = winList.size();
  for (int i = 0; i < wLSize; i++) if (winList[i] != NULL && !winList[i]->GetValid()) {
    
    //general purpose winlist cleaning with destruction of "closed" plstreams and (eventually) associated widgets:
    //in case winList groups X11 streams (or WIN streams) *and* wxWidgets streams (GDL_USE_WX="NO") the following
    //permits to delete the widget_draw also, not only the plplot stream.    
      delete winList[i];
    winList[i] = NULL;
    oList[i] = 0;
  }
  // set new actWin IF NOT VALID ANY MORE
  if (actWin < 0 || actWin >= wLSize || winList[actWin] == NULL || !winList[actWin]->GetValid()) {
    std::vector< long>::iterator mEl = std::max_element(oList.begin(), oList.end()); // set to most recently created
    if (*mEl == 0) { // no window open
      SetActWin(-1);
      oIx = 1;
    } else SetActWin(std::distance(oList.begin(), mEl));
  }
}  
};

//#undef MAX_WIN
//#undef MAX_WIN_RESERVE

#endif

#endif
