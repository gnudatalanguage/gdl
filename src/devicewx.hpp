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

#ifdef HAVE_OLDPLPLOT
#define SETOPT SetOpt
#else
#define SETOPT setopt
#endif

#define MAX_WIN 33  //IDL free and widgets start at 33 ...
#define MAX_WIN_RESERVE 256

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

  bool WOpen( int wIx, const std::string& title, 
            int xSize, int ySize, int xPos, int yPos, bool hide=false) 
    {
        if( wIx >= winList.size() || wIx < 0) return false;

        if( winList[ wIx] != NULL) winList[ wIx]->SetValid(false);

        TidyWindowsList();

        // set initial window size
        int xleng; int yleng;
        int xoff; int yoff;

        DLong xMaxSize, yMaxSize;
        DeviceWX::MaxXYSize(&xMaxSize, &yMaxSize);

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

        WidgetIDT widgetID=wxWindow::NewControlId();
        WidgetIDT drawID=wxWindow::NewControlId();
        wxString titleWxString = wxString(title.c_str(), wxConvUTF8);
        GDLFrame *gdlFrame;
        if (xoff>-1&&yoff>-1) {
          gdlFrame = new GDLFrame( NULL, widgetID, titleWxString , wxPoint(xoff,yoff));
        } else {
          gdlFrame = new GDLFrame( NULL, widgetID, titleWxString);
        }
        gdlFrame->Connect(widgetID, wxEVT_SIZE, gdlSIZE_EVENT_HANDLER);
        gdlFrame->Connect(widgetID, wxEVT_CLOSE_WINDOW, wxCloseEventHandler(GDLFrame::OnCloseWindow));

        wxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
        gdlFrame->SetSizer( topSizer );

        GDLDrawPanel* draw = new GDLDrawPanel(gdlFrame, drawID, wxDefaultPosition, wxSize(xleng,yleng));
        draw->SetContainer(gdlFrame); //to be able to delete the surrounding widget
        //connect to event handlers
        draw->Connect(drawID, wxEVT_SIZE, wxSizeEventHandler(GDLDrawPanel::OnSize)); 
        draw->Connect(drawID, wxEVT_PAINT, wxPaintEventHandler(GDLDrawPanel::OnPaint));
        
        winList[ wIx] = new GDLWXStream( xleng, yleng);
        oList[ wIx]   = oIx++;

        // no pause on win destruction
        winList[ wIx]->spause( false);

        // extended fonts
        winList[ wIx]->fontld( 1);

        // we want color
        winList[ wIx]->scolor( 1);

        PLINT r[ctSize], g[ctSize], b[ctSize];
        actCT.Get( r, g, b);
        winList[ wIx]->scmap0( r, g, b, ctSize); //set colormap 0 to 256 values

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
        winList[ wIx]->schr(2.5,1);
        // sets actWin and updates !D
        SetActWin( wIx);

        draw->AssociateStream(static_cast<GDLWXStream*>(winList[ wIx]));
        topSizer->Add(draw, 1, wxEXPAND|wxALL, 2);
        gdlFrame->Fit();
        draw->SetCursor(wxCURSOR_CROSS); 

        GDLApp* theGDLApp=new GDLApp;
        gdlFrame->SetTheApp(theGDLApp);
        theGDLApp->OnInit();
        theGDLApp->OnRun();
        if (hide) {
        gdlFrame->Hide();
        } else {
        gdlFrame->Show();
        gdlFrame->Raise();
        }
        
        //make one loop
        GDLWidget::HandleEvents();

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

    DLong GetVisualDepth() {
        this->GetStream(); //to open a window if none opened.
        return winList[actWin]->GetVisualDepth();
    }

    DString GetVisualName() {
        this->GetStream(); //to open a window if none opened.
        return winList[actWin]->GetVisualName();
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
    
    // no pause on win destruction
    winList[ wIx]->spause( false);

    // extended fonts
    winList[ wIx]->fontld( 1);

    // we want color
    winList[ wIx]->scolor( 1);

    PLINT r[ctSize], g[ctSize], b[ctSize];
    actCT.Get( r, g, b);
    winList[ wIx]->scmap0( r, g, b, ctSize); //set colormap 0 to 256 values

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
    winList[ wIx]->schr(2.5,1);
    // sets actWin and updates !D
    SetActWin( wIx);
    return true; 
  } // GUIOpen

    bool WDelete(int wIx) {
  winList[ wIx]->SetValid(false);
    TidyWindowsList();
        return true;
    }

    void TidyWindowsList() {
        int wLSize = winList.size();
        GDLFrame* container = NULL;
        GDLDrawPanel* panel = NULL;
        bool isAWindow = false;

        for (int i = 0; i < wLSize; i++) {
            if (winList[i] != NULL) {
                if (!winList[i]->GetValid()) {
                    if (dynamic_cast<GDLWXStream*> (winList[i]) != NULL) {
                        panel = dynamic_cast<GDLDrawPanel*> (static_cast<GDLWXStream*> (winList[i])->GetGDLDrawPanel());
                        if (panel != NULL) {  //2 cases: 1) container is not null: it is a window, we delete the container 
                         //or, container is null: it is a GDLDrawPanel, itself child of a GDLWidgetDraw, that we destroy (isAWindow=false)
                            container = panel->GetContainer();
                            isAWindow = true;
                        }
                    }
                    if (container) { 
                      delete winList[i]; //will be deleted with the removal of GDLWidgetDraw below
                      delete container;
                    }else {
                      GDLWidget *draw=panel->GetGDLWidgetDraw();
                      if (draw) {
                       delete draw; 
                      }
                    }
                    winList[i] = NULL;
                    oList[i] = 0;
                }
            }
            // set new actWin IF NOT VALID ANY MORE
            if (actWin < 0 || actWin >= wLSize || winList[actWin] == NULL || !winList[actWin]->GetValid()) {
                // set to most recently created
                std::vector< long>::iterator mEl =
                        std::max_element(oList.begin(), oList.end());

                // no window open
                if (!isAWindow || *mEl == 0) {
                    SetActWin(-1);
                    oIx = 1;
                } else
                    SetActWin(std::distance(oList.begin(), mEl));
            }
        }
    }

};
#undef MAX_WIN
#undef MAX_WIN_RESERVE
#endif

#endif
