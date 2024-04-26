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

#include "gdlxstream.hpp"

class DeviceX : public GraphicsMultiDevice {
    
public:
    
    DeviceX(std::string name_="X") : GraphicsMultiDevice( -1, XC_crosshair, 3, 0) {
        name = name_;
        DLongGDL origin(dimension(2));
        DLongGDL zoom(dimension(2));
        zoom[0] = 1;
        zoom[1] = 1;
        Display* display = XOpenDisplay(NULL);
        if (display != NULL) {
            int Depth;
            Depth=DefaultDepth(display, DefaultScreen(display));      
            decomposed = (Depth >= 15 ? 1 : 0);
        } else { //try ":0" //IDL also opens :0 when DISPLAY is not set.
            display = XOpenDisplay(":0");
            if (display != NULL) {
            int Depth;
            Depth=DefaultDepth(display, DefaultScreen(display));      
            decomposed = (Depth >= 15 ? 1 : 0);
          } 
        }
        if (display != NULL) {
          Visual *visual=DefaultVisual( display, DefaultScreen(display) );
          switch ( visual->c_class )
          {
           case TrueColor:
           case StaticColor:
           case StaticGray:
            staticDisplay=1;
               break; //ok, are static
           default: //dynamic: problems if cmaps not initialized.
            staticDisplay=0;
          }
        } else {
          staticDisplay=0;
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

    if( winList[ wIx] != NULL) winList[ wIx]->SetValid(false); TidyWindowsList();

    // set initial window size
    PLFLT xp; PLFLT yp; 
    PLINT xleng; PLINT yleng;
    PLINT xoff; PLINT yoff;

    DLong xMaxSize=640;
    DLong yMaxSize=512;
    MaxXYSize(&xMaxSize, &yMaxSize);
    bool noPosx=(xPos==-1);
    bool noPosy=(yPos==-1);
    xPos=max(1,xPos); //starts at 1 to avoid problems plplot!
    yPos=max(1,yPos);

    xleng = min(xSize,xMaxSize); if (xPos+xleng > xMaxSize) xPos=xMaxSize-xleng-1;
    yleng = min(ySize,yMaxSize); if (yPos+yleng > yMaxSize) yPos=yMaxSize-yleng-1;
// dynamic allocation needed!    
    PLINT Quadx[4]={xMaxSize-xleng-1,xMaxSize-xleng-1,1,1};
    PLINT Quady[4]={1,yMaxSize-yleng-1,1,yMaxSize-yleng-1};
  int locOnScreen=(wIx>31)?(wIx+2)%4:wIx % 4; //IDL shifts /FREE windows by 2
  if (noPosx && noPosy) { //no init given, use 4 quadrants:
      xoff = Quadx[locOnScreen];
      yoff = Quady[locOnScreen];
    } else if (noPosx) {
      xoff = Quadx[locOnScreen];
      yoff = yMaxSize-yPos-yleng;
    } else if (noPosy) {
      xoff = xPos;
      yoff = Quady[locOnScreen];
    } else {
      xoff  = xPos;
      yoff  = yMaxSize-yPos-yleng;
    }
    //apparently this is OK to get same results as IDL on X11...
    yoff+=1;
    xp=(*static_cast<DFloatGDL*>( dStruct->GetTag(dStruct->Desc()->TagIndex("X_PX_CM"))))[0]*2.5;
    yp=(*static_cast<DFloatGDL*>( dStruct->GetTag(dStruct->Desc()->TagIndex("Y_PX_CM"))))[0]*2.5;

    winList[ wIx] = new GDLXStream(xp , yp, xleng, yleng, xoff, yoff, title, hide);
    oList[ wIx]   = oIx++;    

    SetActWin( wIx);
  //set current cursor:
  winList[wIx]->CursorStandard(cursorId);
  winList[wIx]->SetGraphicsFunction(gcFunction);
  //backingStore (dummy call at the moment)
  winList[wIx]->SetBackingStore(-1);

    bool success;
    if ( !hide ) success=this->UnsetFocus();
    return true; 
    }
  
    GDLGStream* GetStream(bool open = true) {
        TidyWindowsList();
        if (actWin == -1) {
            if (!open) return NULL;

            DString title = "GDL 0";
            DLong xSize=640;
            DLong ySize=512;
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
  
    DLongGDL* GetScreenSize(char* disp) { 
      Display* display = XOpenDisplay(disp);
      int screen_num, screen_width, screen_height;
      DLongGDL* res;

      if (display == NULL) {
         screen_width = 0;
         screen_height = 0;
      } else {
         screen_num = DefaultScreen(display);
         screen_width = DisplayWidth(display, screen_num);
         screen_height = DisplayHeight(display, screen_num);
         XCloseDisplay(display);
      }
      res = new DLongGDL(2, BaseGDL::NOZERO);
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
          this->GetStream(true); //this command SHOULD NOT open a window if none opened, but how to do it?
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
          this->GetStream(true); //this command SHOULD NOT open a window if none opened, but how to do it?
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
          this->GetStream();
          BaseGDL* val=winList[actWin]->GetFontnames(fontname);
          return val;
        } else {
          return winList[actWin]->GetFontnames(fontname);
        }
    }
    DLong GetFontnum(){
        TidyWindowsList();
        if (actWin == -1) {
          this->GetStream();
          DLong val=winList[actWin]->GetFontnum(fontname);
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
  
  bool CursorCrosshair(bool standard)
  {
   if (standard) return CursorStandard(XC_arrow);
   else return CursorStandard(XC_crosshair);
  }

 void DefaultXYSize(DLong *xSize, DLong *ySize) {
  Display* display = XOpenDisplay(NULL);
  if (display != NULL) {
   *xSize = DisplayWidth(display, DefaultScreen(display)) / 2;
   *ySize = DisplayHeight(display, DefaultScreen(display)) / 2;
   XCloseDisplay(display);
  }

  bool noQscreen = true;
  string gdlQscreen = GetEnvString("GDL_GR_X_QSCREEN");
  if (gdlQscreen == "1") noQscreen = false;
  string gdlXsize = GetEnvString("GDL_GR_X_WIDTH");
  if (gdlXsize != "" && noQscreen) *xSize = atoi(gdlXsize.c_str());
  string gdlYsize = GetEnvString("GDL_GR_X_HEIGHT");
  if (gdlYsize != "" && noQscreen) *ySize = atoi(gdlYsize.c_str());
  }

 void MaxXYSize(DLong *xSize, DLong *ySize) {
  Display* display = XOpenDisplay(NULL);
  if (display != NULL) {
   *xSize = DisplayWidth(display, DefaultScreen(display));
   *ySize = DisplayHeight(display, DefaultScreen(display));
   XCloseDisplay(display);
  }
 }  
};

#endif

#endif
