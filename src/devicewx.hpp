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

#include "gdlwxstream.hpp"

class DeviceWX : public GraphicsMultiDevice {
  
public:
 
 bool DoesNotDrawSinglePoints() {return true;}
//is called "WIN" as there is no other choice on WINDOWS, and "WIN" is still ok on linux and macOSX
    DeviceWX(std::string name_="WX") : GraphicsMultiDevice( 1, 3, 3, 0) { //force decomposed=true until we find a better way (::wxDispayDepth() crashes)
        name = name_; 
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
        dStruct->InitTag("Y_CH_SIZE",  DLongGDL(10));
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
	      int xSize, int ySize, int xPos, int yPos, bool hide=false)
  {


  if (wIx >= winList.size() || wIx < 0) return false;

    if( winList[ wIx] != NULL) winList[ wIx]->SetValid(false); TidyWindowsList();

  // set initial window size
  int x_scroll_size;
  int y_scroll_size;
  int xoff;
  int yoff;

  DLong xMaxSize=640;
  DLong yMaxSize=512;
  MaxXYSize(&xMaxSize, &yMaxSize);
  bool noPosx = (xPos == -1);
  bool noPosy = (yPos == -1);
  xPos = max(1, xPos); //starts at 1 to avoid problems plplot!
  yPos = max(1, yPos);

  bool scrolled = false;
  if (xSize > xMaxSize || ySize > yMaxSize) scrolled = true;

  if (scrolled) {
   x_scroll_size = min(xSize, xMaxSize/2);
   y_scroll_size = min(ySize, yMaxSize/2);
  } else {
   x_scroll_size = min(xSize, xMaxSize);
   y_scroll_size = min(ySize, yMaxSize);
  }
  if (xPos + x_scroll_size > xMaxSize) xPos = xMaxSize - x_scroll_size - 1;
  if (yPos + y_scroll_size > yMaxSize) yPos = yMaxSize - y_scroll_size - 1;
  // dynamic allocation needed!    
  PLINT Quadx[4] = {xMaxSize - x_scroll_size - 1, xMaxSize - x_scroll_size - 1, 1, 1};
#ifdef __APPLE__
  PLINT Quady[4] = {24, yMaxSize - y_scroll_size - 1, 24, yMaxSize - y_scroll_size - 1}; //do not cover 24 pix taskbar.
#else
  PLINT Quady[4] = {1, yMaxSize - y_scroll_size - 1, 1, yMaxSize - y_scroll_size - 1}; //IDL covers the linux (bottom) taskbar too
#endif
  int locOnScreen=(wIx>31)?(wIx+2)%4:wIx % 4; //IDL shifts /FREE windows by 2
  if (noPosx && noPosy) { //no init given, use 4 quadrants:
   xoff = Quadx[locOnScreen];
   yoff = Quady[locOnScreen];
  } else if (noPosx) {
   xoff = Quadx[locOnScreen];
   yoff = yMaxSize - yPos - y_scroll_size;
  } else if (noPosy) {
   xoff = xPos;
   yoff = Quady[locOnScreen];
  } else {
   xoff = xPos;
   yoff = yMaxSize - yPos - y_scroll_size;
  }

  //1) a frame
  wxString titleWxString = wxString(title.c_str(), wxConvUTF8);
//on wxMAC, frame will not appear if style is not exactly this (!!!)
#ifdef __WXMAC__
long style = (wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxRESIZE_BORDER | wxCAPTION | wxCLOSE_BOX);
#else
long style = (wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxRESIZE_BORDER | wxCAPTION | wxCLOSE_BOX ); //| wxFRAME_TOOL_WINDOW); //no focus 
#endif
if(hide) {
  style=wxFRAME_TOOL_WINDOW;
}
  gdlwxPlotFrame* plotFrame = new gdlwxPlotFrame(titleWxString, wxPoint(xoff,yoff), wxDefaultSize, style, scrolled);
  // Associate a sizer immediately
  wxSizer* tfSizer = new wxBoxSizer(wxVERTICAL);
  plotFrame->SetSizer(tfSizer);

  // 3) Sizes:
  wxSize wSize = wxSize(xSize, ySize);
  wxSize wScrollSize = wxSize(x_scroll_size, y_scroll_size);
  gdlwxPlotPanel* plot = new gdlwxPlotPanel(plotFrame);
  if (scrolled) {
   plot->SetMinClientSize(wScrollSize);
   plot->SetClientSize(wScrollSize);
  } else {
   plot->SetMinClientSize(wSize);
   plot->SetSize(wSize);
  }
  plot->SetVirtualSize(wSize);
  plot->InitDrawSize(wSize);

  if (scrolled) {
   plot->SetScrollbars(gdlSCROLL_RATE, gdlSCROLL_RATE, wSize.x / gdlSCROLL_RATE, wSize.y / gdlSCROLL_RATE);
   plot->ShowScrollbars(wxSHOW_SB_ALWAYS, wxSHOW_SB_ALWAYS);
  }

  plot->SetCursor(wxCURSOR_CROSS);
  tfSizer->Add(plot, DONOTALLOWSTRETCH, wxALL, 0);

  //create stream
  GDLWXStream* me = new GDLWXStream(xSize, ySize);
  me->SetCurrentFont(fontname);
  winList[ wIx] = me;
  oList[ wIx] = oIx++;
  // sets actWin and updates !D
  SetActWin(wIx);
  //associate stream with plot panel both ways:
  me->SetGdlxwGraphicsPanel(plot, true);
  plot->SetStream(me);
  plot->SetPStreamIx(wIx);

  plotFrame->Fit();
  plotFrame->SetBackgroundColour(*wxBLACK); //set black background --> for "window" comand.
  plotFrame->Realize();
  if (hide) {
    winList[ wIx]->UnMapWindowAndSetPixmapProperty(); //needed: will set the "pixmap" property
  } else {
    plotFrame->Show(); //WithoutActivating(); --> this does nothing good. Better tailor your window manager to 'focus under mouse"
  //    plotFrame->UpdateWindowUI(); not useful
  plotFrame->Raise();
  plotFrame->Refresh();
  //really show by letting the loop do its magic. Necessary ?.
//  CallWXEventLoop();
   }
   
  // these widget specific events are always set:
    plot->Connect(wxEVT_PAINT, wxPaintEventHandler(gdlwxGraphicsPanel::OnPaint));
    //disable flicker see https://wiki.wxwidgets.org/Flicker-Free_Drawing
    plot->Connect(wxEVT_ERASE_BACKGROUND, wxEraseEventHandler(gdlwxGraphicsPanel::OnErase));

    plotFrame->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(gdlwxPlotFrame::OnUnhandledClosePlotFrame));
    //OnPlotSizeWithTimer does not work on GNOME. OnPlotWindowSize works.
#ifdef __WXMSW__
    plotFrame->Connect(wxEVT_SIZE, wxSizeEventHandler(gdlwxPlotFrame::OnPlotWindowSize)); //Timer resize do not work on MSW
#else
      plotFrame->Connect(wxEVT_SIZE, wxSizeEventHandler(gdlwxPlotFrame::OnPlotSizeWithTimer));
#endif
    return true;
 }

 GDLGStream* GetStream(bool open = true) {
  TidyWindowsList();
  if (actWin == -1) {
   if (!open) return NULL;

   DString title = "GDL 0";
   DLong xSize = 640;
   DLong ySize = 512;
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
        DIntGDL* res = new DIntGDL(2, BaseGDL::NOZERO);
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

 BaseGDL* GetWxFontnames(DString pattern) {
  if (pattern.length() <= 0) return NULL;
  wxFontEnumerator fontEnumerator;
  wxArrayString fn=fontEnumerator.GetFacenames();
//  fontEnumerator.EnumerateFacenames();
  int nFacenames = fn.GetCount();
  for (int i = 0; i < nFacenames; ++i) fn.Item(i).Upper();
  // we are supposed to select only entries lexically corresponding to 'pattern'.
  //first check who passes (ugly)
  wxString wxPattern(pattern);
  wxPattern.Upper();
  int* good=new int[nFacenames];
  long total=0;
  for (int i = 0; i < nFacenames; ++i) {good[i]=fn.Item(i).Matches(wxPattern);total+=good[i];}
  if (total == 0) return NULL;
  DStringGDL* myList = new DStringGDL(dimension(total),BaseGDL::NOZERO);
  int k=0;
  for (int i = 0; i < nFacenames; ++i) if (good[i]) (*myList)[k++].assign(fn.Item(i).mb_str(wxConvUTF8));
  delete[] good;
  return myList;
 }

 DLong GetWxFontnum(DString pattern) {
  if (GetWxFontnames(pattern) == NULL) return 0;
  if (pattern.length() == 0) return 0;
  return this->GetWxFontnames(pattern)->N_Elements();
 }

 DLong GetVisualDepth() {
  return 24;
 } //no use opening a window, the answer is 24!

 DString GetVisualName() {
  return DString("TrueColor");
 }

 BaseGDL* GetFontnames() {
  return GetWxFontnames(fontname);
 }

 DLong GetFontnum() {
  return GetWxFontnum(fontname);
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
    if (standard) return CursorStandard(-1);
    else return CursorStandard(-2);
 }

 bool CursorImage(char* v, int x, int y, char* m) {
     this->GetStream(); //to open a window if none opened.
     bool ret;
     for (int i = 0; i < winList.size(); i++) {
         if (winList[i] != NULL) {
           ret = winList[i]->CursorImage(v,x,y,m);
           if (ret == false) return ret;
         }
     }
  return true;
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

   GDLGStream* GUIOpen( int wIx, int xSize, int ySize, void* draw)
  {
    if( wIx >= winList.size() || wIx < 0) return NULL;

    if( winList[ wIx] != NULL) winList[ wIx]->SetValid(false); TidyWindowsList();
    GDLWXStream* me=new GDLWXStream( xSize, ySize);
    me->SetCurrentFont(fontname);
    me->SetGdlxwGraphicsPanel( static_cast<gdlwxGraphicsPanel*>(draw), false );
    winList[ wIx] = me;
    oList[ wIx]   = oIx++;
    // sets actWin and updates !D
    SetActWin( wIx);
    return winList[ wIx]; 
  } // GUIOpen  

};
#endif

#endif
