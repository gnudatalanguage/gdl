/* *************************************************************************
                          devicewx.hpp  -  WXwidgets device
                             -------------------
    begin                : Sep 19 2014
    author               : Jeongbin Park
    email                : pjb7687@snu.ac.kr
 ***************************************************************************/

/* *************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DEVICEWX_HPP_
#define DEVICEWX_HPP_

#ifndef HAVE_LIBWXWIDGETS
#else

#include <algorithm>
#include <vector>
#include <cstring>

#include <plplot/drivers.h>

#include "graphicsdevice.hpp"
#include "gdlwxstream.hpp"
#include <wx/settings.h>
#include <wx/gdicmn.h> 
#include "initsysvar.hpp"
#include "gdlexception.hpp"

//#define MAX_WIN 32  //IDL free and widgets start at 32 ...
//#define MAX_WIN_RESERVE 256

class DeviceWX : public GraphicsMultiDevice {
  
public:

    DeviceWX(string name_="MAC") : GraphicsMultiDevice( 1, 3, 3, 0) { //force decomposed=true until we find a better way (::wxDispayDepth() crashes)
        name = name_; //temporary hack to avoid coyoteGraphics crash in ATV.PRO
        DLongGDL origin(dimension(2));
        DLongGDL zoom(dimension(2));
        zoom[0] = 1;
        zoom[1] = 1;
        
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

 bool WOpen(int wIx, const std::string& title,
   int xSize, int ySize, int xPos, int yPos, bool hide = false) {
  if (wIx >= winList.size() || wIx < 0) return false;

  if (winList[ wIx] != NULL) winList[ wIx]->SetValid(false);

  TidyWindowsList();

  // set initial window size
  int xleng;
  int yleng;
  int xoff;
  int yoff;

  DLong xMaxSize, yMaxSize;
  DeviceWX::MaxXYSize(&xMaxSize, &yMaxSize);

  bool noPosx = (xPos == -1);
  bool noPosy = (yPos == -1);
  xPos = max(1, xPos); //starts at 1 to avoid problems plplot!
  yPos = max(1, yPos);

  xleng = min(xSize, xMaxSize);
  if (xPos + xleng > xMaxSize) xPos = xMaxSize - xleng - 1;
  yleng = min(ySize, yMaxSize);
  if (yPos + yleng > yMaxSize) yPos = yMaxSize - yleng - 1;
  // dynamic allocation needed!    
  PLINT Quadx[4] = {xMaxSize - xleng - 1, xMaxSize - xleng - 1, 1, 1};
  PLINT Quady[4] = {1, yMaxSize - yleng - 1, 1, yMaxSize - yleng - 1};
  if (noPosx && noPosy) { //no init given, use 4 quadrants:
   xoff = Quadx[wIx % 4];
   yoff = Quady[wIx % 4];
  } else if (noPosx) {
   xoff = Quadx[wIx % 4];
   yoff = yMaxSize - yPos - yleng;
  } else if (noPosy) {
   xoff = xPos;
   yoff = Quady[wIx % 4];
  } else {
   xoff = xPos;
   yoff = yMaxSize - yPos - yleng;
  }

  WidgetIDT mbarID = 0;
  GDLWidgetGraphicWindowBase* base = new GDLWidgetGraphicWindowBase(mbarID, xoff, yoff, title);
  GDLWidgetDraw* draw = new GDLWidgetDraw(base->WidgetID(), NULL, wIx, xleng, yleng, -1, -1, false, 0);
  base->setWindow(static_cast<GDLDrawPanel*>(draw->GetWxWidget()));
  base->Realize(!hide); //just avoid to map the widget.
  if(hide) winList[ wIx]->UnMapWindow();   //needed: will set the "pixmap" property
 // else { winList[ wIx]->UnsetFocus(); winList[wIx]->Raise();}
  return true;
 }

    // should check for valid streams
     
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
        this->GetStream(); //to open a window if none opened.
        return gcFunction;
    }

    DIntGDL* GetScreenSize(char* disp) {
        DIntGDL* res;
        res = new DIntGDL(2, BaseGDL::NOZERO);
        (*res)[0] = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
        (*res)[1] = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);
        return res;
    }

    DDoubleGDL* GetScreenResolution(char* disp) {
        this->GetStream(); //to open a window if none opened.
        double resx, resy;
        if (winList[actWin]->GetScreenResolution(resx, resy)) {
            DDoubleGDL* res;
            res = new DDoubleGDL(2, BaseGDL::NOZERO);
            (*res)[0] = resx;
            (*res)[1] = resy;
            return res;
        } else return NULL;
    }

    DIntGDL* GetWindowPosition() {
        this->GetStream(); //to open a window if none opened.
        long xpos, ypos;
        if (winList[actWin]->GetWindowPosition(xpos, ypos)) {
            DIntGDL* res;
            res = new DIntGDL(2, BaseGDL::NOZERO);
            (*res)[0] = xpos;
            (*res)[1] = ypos;
            return res;
        } else return NULL;
    }

//    DLong GetVisualDepth() {
//        this->GetStream(); //to open a window if none opened.
//        return winList[actWin]->GetVisualDepth();
//    }
//
//    DString GetVisualName() {
//        this->GetStream(); //to open a window if none opened.
//        return winList[actWin]->GetVisualName();
//    }
//    BaseGDL* GetFontnames(){
//        this->GetStream(); //to open a window if none opened.
//        return winList[actWin]->GetFontnames(fontname);
//    }
//    DLong GetFontnum(){
//        this->GetStream(); //to open a window if none opened.
//        return winList[actWin]->GetFontnum(fontname);
//    } 
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

  bool CursorCrosshair()
  {
    return CursorStandard(33);
  }

    void DefaultXYSize(DLong *xSize, DLong *ySize) {
        *xSize = wxSystemSettings::GetMetric(wxSYS_SCREEN_X) / 2;
        *ySize = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y) / 2;

        bool noQscreen = true;
        string gdlQscreen = GetEnvString("GDL_GR_X_QSCREEN");
        if (gdlQscreen == "1") noQscreen = false;
        string gdlXsize = GetEnvString("GDL_GR_X_WIDTH");
        if (gdlXsize != "" && noQscreen) *xSize = atoi(gdlXsize.c_str());
        string gdlYsize = GetEnvString("GDL_GR_X_HEIGHT");
        if (gdlYsize != "" && noQscreen) *ySize = atoi(gdlYsize.c_str());
    }

    void MaxXYSize(DLong *xSize, DLong *ySize) {
        *xSize = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
        *ySize = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);
    }
    
    bool GUIOpen( int wIx, int xSize, int ySize)//, int xPos, int yPos)
  {
    if( wIx >= winList.size() || wIx < 0) return false;

    if( winList[ wIx] != NULL) winList[ wIx]->SetValid(false);
    TidyWindowsList();

    winList[ wIx] = new GDLWXStream( xSize, ySize);
    oList[ wIx]   = oIx++;
    winList[ wIx]->SetCurrentFont(fontname);
    // sets actWin and updates !D
    SetActWin( wIx);
    return true; 
  } // GUIOpen
    
bool SetCharacterSize( DLong x, DLong y)     {
   DStructGDL* dStruct=SysVar::D();
   int tagx = dStruct->Desc()->TagIndex( "X_CH_SIZE");
   int tagy = dStruct->Desc()->TagIndex( "Y_CH_SIZE");
   DLongGDL* newxch = static_cast<DLongGDL*>( dStruct->GetTag( tagx));
   DLongGDL* newych = static_cast<DLongGDL*>( dStruct->GetTag( tagy));
   (*newxch)[0]=x;
   (*newych)[0]=y;

   int tagxppcm = dStruct->Desc()->TagIndex( "X_PX_CM");
   int tagyppcm = dStruct->Desc()->TagIndex( "Y_PX_CM");
   DFloat xppm = (*static_cast<DFloatGDL*>(dStruct->GetTag(tagxppcm)))[0]*0.1;
   DFloat yppm = (*static_cast<DFloatGDL*>(dStruct->GetTag(tagyppcm)))[0]*0.1;

   PLFLT newsize=x/xppm/1.5; //1.5 is probably due to height / width ratio . 
   PLFLT newSpacing=y/yppm;
   GDLGStream* actStream=GetStream(false);
   if( actStream != NULL) {actStream->setLineSpacing(newSpacing); actStream->RenewPlplotDefaultCharsize(newsize);}
   return true;
  }

//Please find how to specialize TidyWindowsList for wx and x11 widgets when this function is called
//as GraphicsDevice::GetDevice()->TidyWindowsList(); which does not return a specialized version.
// Util then, do not uncomment the following.
// void TidyWindowsList() {
//  int wLSize = winList.size();
//  for (int i = 0; i < wLSize; i++) if (winList[i] != NULL && !winList[i]->GetValid()) {
//    if (dynamic_cast<GDLWXStream*> (winList[i]) != NULL) {
//     GDLDrawPanel* panel = NULL;
//     panel = dynamic_cast<GDLDrawPanel*> (static_cast<GDLWXStream*> (winList[i])->GetGDLDrawPanel());
//     //test if stream is associated to graphic window or widget_draw. If graphic, destroy directly TLB widget.
//     GDLWidgetDraw *draw = panel->GetGDLWidgetDraw();
//     if (draw) {
//      //parent of panel may be a GDLFrame. If frame is actually made by the WOpen function, destroy everything.
//      GDLWidgetBase* container = NULL;
//      container = static_cast<GDLWidgetBase*> (draw->GetTopLevelBaseWidget(draw->WidgetID()));
//      if (container && container->IsGraphicWindowFrame()) container->SelfDestroy();
//      else delete draw;
//     } else delete winList[i];
//    } else delete winList[i];
//    winList[i] = NULL;
//    oList[i] = 0;
//   }
//  // set new actWin IF NOT VALID ANY MORE
//  if (actWin < 0 || actWin >= wLSize || winList[actWin] == NULL || !winList[actWin]->GetValid()) {
//   // set to most recently created
//   std::vector< long>::iterator mEl = std::max_element(oList.begin(), oList.end());
//   if (*mEl == 0) { // no window open
//    SetActWin(-1); //sets    oIx = 1;
//   } else SetActWin(GraphicsDevice::GetDevice()->GetNonManagedWidgetActWin(false)); //get first non-managed window. false is needed. 
//  }
// }
};
#endif

#endif
