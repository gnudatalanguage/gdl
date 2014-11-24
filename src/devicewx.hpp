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


#include "initsysvar.hpp"
#include "gdlexception.hpp"

#ifdef HAVE_OLDPLPLOT
#define SETOPT SetOpt
#else
#define SETOPT setopt
#endif

#define maxWin 65  //IDL has 65...
#define maxWinReserve 256

class DeviceWX : public GraphicsDevice {
private:
    std::vector<GDLGStream*> winList;
    std::vector<long> oList;
    long oIx;
    int actWin;
    int decomposed; // false -> use color table
    int cursorId; //should be 3 by default.
    long gcFunction;
    int backingStoreMode;

    int getCursorId() {
        return cursorId;
    }

    long getGCFunction() {
        return gcFunction;
    }

    int GetBackingStore() {
        return backingStoreMode;
    }

    void SetActWin(int wIx) {
        // update !D
        if (wIx >= 0 && wIx < winList.size()) {
            long xsize, ysize, xoff, yoff;
            winList[wIx]->GetGeometry(xsize, ysize, xoff, yoff);

            (*static_cast<DLongGDL*> (dStruct->GetTag(xSTag)))[0] = xsize;
            (*static_cast<DLongGDL*> (dStruct->GetTag(ySTag)))[0] = ysize;
            (*static_cast<DLongGDL*> (dStruct->GetTag(xVSTag)))[0] = xsize;
            (*static_cast<DLongGDL*> (dStruct->GetTag(yVSTag)))[0] = ysize;
            // number of colors
            //        (*static_cast<DLongGDL*>( dStruct->GetTag( n_colorsTag)))[0] = 1 << winList[ wIx]->GetWindowDepth();
            // set !D.N_COLORS and !P.COLORS according to decomposed value.
            unsigned long nSystemColors = (1 << winList[wIx]->GetWindowDepth());
            unsigned long oldColor = (*static_cast<DLongGDL*> (SysVar::P()->GetTag(SysVar::P()->Desc()->TagIndex("COLOR"), 0)))[0];
            unsigned long oldNColor = (*static_cast<DLongGDL*> (dStruct->GetTag(n_colorsTag)))[0];
            if (this->decomposed == -1) decomposed = this->GetDecomposed();
            if (this->decomposed == 1 && oldNColor == 256) {
                (*static_cast<DLongGDL*> (dStruct->GetTag(n_colorsTag)))[0] = nSystemColors;
                if (oldColor == 255) (*static_cast<DLongGDL*> (SysVar::P()->GetTag(SysVar::P()->Desc()->TagIndex("COLOR"), 0)))[0] = nSystemColors - 1;
            } else if (this->decomposed == 0 && oldNColor == nSystemColors) {
                (*static_cast<DLongGDL*> (dStruct->GetTag(n_colorsTag)))[0] = 256;
                if (oldColor == nSystemColors - 1) (*static_cast<DLongGDL*> (SysVar::P()->GetTag(SysVar::P()->Desc()->TagIndex("COLOR"), 0)))[0] = 255;
            }
        }
        // window number
        (*static_cast<DLongGDL*> (dStruct->GetTag(wTag)))[0] = wIx;

        actWin = wIx;
    }

    void RaiseWin(int wIx) {
        if (wIx >= 0 && wIx < winList.size()) winList[wIx]->Raise();
    }

    void LowerWin(int wIx) {
        if (wIx >= 0 && wIx < winList.size()) winList[wIx]->Lower();
    }

    void IconicWin(int wIx) {
        if (wIx >= 0 && wIx < winList.size()) winList[wIx]->Iconic();
    }

    void DeIconicWin(int wIx) {
        if (wIx >= 0 && wIx < winList.size()) winList[wIx]->DeIconic();
    }

    // process user deleted windows
    // should be done in a thread

    void TidyWindowsList() {
        int wLSize = winList.size();

        //     bool redo;
        //     do { // it seems that the event queue is only searched a few events deep
        //       redo = false;
        for (int i = 0; i < wLSize; i++)
            if (winList[i] != NULL && !winList[i]->GetValid()) {
                delete winList[i];
                winList[i] = NULL;
                oList[i] = 0;
                // 	    redo = true;
            }
        //     } while( redo);


        // set new actWin IF NOT VALID ANY MORE
        if (actWin < 0 || actWin >= wLSize ||
                winList[actWin] == NULL || !winList[actWin]->GetValid()) {
            // set to most recently created
            std::vector< long>::iterator mEl =
                    std::max_element(oList.begin(), oList.end());

            // no window open
            if (*mEl == 0) {
                SetActWin(-1);
                oIx = 1;
            } else
                SetActWin(std::distance(oList.begin(), mEl));
        }
    }

public:

    DeviceWX() : GraphicsDevice(), oIx(1), actWin(-1), decomposed(-1), gcFunction(3), backingStoreMode(0) {
        name = "WX";

        DLongGDL origin(dimension(2));
        DLongGDL zoom(dimension(2));
        zoom[0] = 1;
        zoom[1] = 1;

        dStruct = new DStructGDL("!DEVICE");
        dStruct->InitTag("NAME", DStringGDL(name));
        dStruct->InitTag("X_SIZE", DLongGDL(640));
        dStruct->InitTag("Y_SIZE", DLongGDL(512));
        dStruct->InitTag("X_VSIZE", DLongGDL(640));
        dStruct->InitTag("Y_VSIZE", DLongGDL(512));
        dStruct->InitTag("X_CH_SIZE", DLongGDL(6));
        dStruct->InitTag("Y_CH_SIZE", DLongGDL(9));
        dStruct->InitTag("X_PX_CM", DFloatGDL(40.0));
        dStruct->InitTag("Y_PX_CM", DFloatGDL(40.0));
        dStruct->InitTag("N_COLORS", DLongGDL(256));
        dStruct->InitTag("TABLE_SIZE", DLongGDL(ctSize));
        dStruct->InitTag("FILL_DIST", DLongGDL(0));
        dStruct->InitTag("WINDOW", DLongGDL(-1));
        dStruct->InitTag("UNIT", DLongGDL(0));
        dStruct->InitTag("FLAGS", DLongGDL(328124));
        dStruct->InitTag("ORIGIN", origin);
        dStruct->InitTag("ZOOM", zoom);

        winList.reserve(maxWinReserve);
        winList.resize(maxWin);
        for (int i = 0; i < maxWin; i++) winList[i] = NULL;
        oList.reserve(maxWinReserve);
        oList.resize(maxWin);
        for (int i = 0; i < maxWin; i++) oList[i] = 0;

        //     GDLGStream::SetErrorHandlers();
    }

    ~DeviceWX() {
        std::vector<GDLGStream*>::iterator i;
        for (i = winList.begin(); i != winList.end(); ++i) {
            delete *i;
            /* *i = NULL;*/
        }
    }

    GDLGStream* GetStream(int wIx) const {
        return winList[ wIx];
    }

    void EventHandler() {
        if (actWin < 0) return; //would this have side effects?  
        int wLSize = winList.size();
        for (int i = 0; i < wLSize; i++)
            if (winList[i] != NULL)
                winList[i]->EventHandler();

        TidyWindowsList();
    }

    bool WDelete(int wIx) {
        TidyWindowsList();

        int wLSize = winList.size();
        if (wIx >= wLSize || wIx < 0 || winList[wIx] == NULL)
            return false;

        if (dynamic_cast<GDLWXStream*> (winList[wIx]) != NULL) {
            Warning("Attempt to delete widget (ID=" + i2s(wIx) + "). Will be auto-deleted upon window destruction.");
            return false;
        }

        delete winList[wIx];
        winList[wIx] = NULL;
        oList[wIx] = 0;

        // set to most recently created
        std::vector< long>::iterator mEl =
                std::max_element(oList.begin(), oList.end());

        // no window open
        if (*mEl == 0) {
            SetActWin(-1);
            oIx = 1;
        } else
            SetActWin(std::distance(oList.begin(), mEl));

        return true;
    }

    bool WOpen(int wIx, const std::string& title,
            int xSize, int ySize, int xPos, int yPos) {
        TidyWindowsList();

        int wLSize = winList.size();
        if (wIx >= wLSize || wIx < 0)
            return false;

        if (winList[ wIx] != NULL) {
            delete winList[ wIx];
            winList[ wIx] = NULL;
        }

        wxWindow *wxParent = NULL;

        GUIMutexLockerWidgetsT gdlMutexGuiEnterLeave;

        wxString titleWxString = wxString(title.c_str(), wxConvUTF8);
        GDLFrame *gdlFrame = new GDLFrame(0, 0, 0, titleWxString);
        //    m_gdlFrameOwnerMutexP = gdlFrame->m_gdlFrameOwnerMutexP;
        //    assert( m_gdlFrameOwnerMutexP != NULL );
        //     gdlFrame->Freeze();

        gdlFrame->SetSize(xSize, ySize);

        wxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
        gdlFrame->SetSizer(topSizer);

        wxPanel *panel = new wxPanel(gdlFrame, wxID_ANY);
        wxSizer *sizer = new wxBoxSizer(wxVERTICAL);
        panel->SetSizer(sizer);
        topSizer->Add(panel);
        GDLDrawPanel* gdlWindow = new GDLDrawPanel(gdlFrame, panel->GetId(), wxDefaultPosition, wxSize(xSize, ySize), wxBORDER_SIMPLE);
        topSizer->Add(gdlWindow, 0, wxEXPAND | wxALL, 5);

        gdlWindow->InitStream();
        winList[ wIx] = static_cast<GDLGStream*> (GraphicsDevice::GetGUIDevice()->GetStreamAt(gdlWindow->PStreamIx()));
        static_cast<GDLWXStream*> (winList[ wIx])->SetGDLDrawPanel(gdlWindow);
        gdlFrame->Show();
        // no pause on win destruction
        winList[ wIx]->spause(false);

        // extended fonts
        winList[ wIx]->fontld(1);

        // we want color
        winList[ wIx]->scolor(1);

        PLINT r[ctSize], g[ctSize], b[ctSize];
        actCT.Get(r, g, b);
        winList[ wIx]->scmap0(r, g, b, ctSize); //set colormap 0 to 256 values

        // need to be called initially. permit to fix things
        winList[ wIx]->ssub(1, 1);
        winList[ wIx]->adv(0);
        // load font
        winList[ wIx]->font(1);
        winList[ wIx]->vpor(0, 1, 0, 1);
        winList[ wIx]->wind(0, 1, 0, 1);
        winList[ wIx]->DefaultCharSize();
        //in case these are not initalized, here is a good place to do it.
        if (winList[ wIx]->updatePageInfo() == true) {
            winList[ wIx]->GetPlplotDefaultCharSize(); //initializes everything in fact..

        }
        //see wxDriver code -it redefines x and y dpi according to the size of the page.
        //to insure a x (mm) charsize in height, similar in shape to what X11 does with its 4dpm (250mu pitch)
        //we need to rescale the default value 
        PLFLT defhmm, scalhmm;
        plgchr(&defhmm, &scalhmm); // height of a letter in millimetres
        PLFLT xp, yp;
        PLINT xleng, yleng, xoff, yoff;
        winList[ wIx]->gpage(xp, yp, xleng, yleng, xoff, yoff);
        PLFLT newsize=(defhmm*4)/(yp/25.4);
        winList[ wIx]->RenewPlplotDefaultCharsize(newsize);
        // sets actWin and updates !D
        SetActWin( wIx);
        return true; //winList[ wIx]->Valid(); // Valid() need to called once
    }

    bool WState(int wIx) {
        return wIx >= 0 && wIx < oList.size() && oList[wIx] != 0;
    }

    bool WSize(int wIx, int *xSize, int *ySize, int *xPos, int *yPos) {
        TidyWindowsList();

        int wLSize = winList.size();
        if (wIx > wLSize || wIx < 0)
            return false;

        long xleng, yleng;
        long xoff, yoff;
        winList[wIx]->GetGeometry(xleng, yleng, xoff, yoff);

        *xSize = xleng;
        *ySize = yleng;
        *xPos = xoff;
        *yPos = yoff;

        return true;
    }

    bool WSet(int wIx) {
        TidyWindowsList();

        int wLSize = winList.size();
        if (wIx >= wLSize || wIx < 0 || winList[wIx] == NULL)
            return false;

        SetActWin(wIx);
        return true;
    }

    bool WShow(int ix, bool show, bool iconic) {
        TidyWindowsList();

        int wLSize = winList.size();
        if (ix >= wLSize || ix < 0 || winList[ix] == NULL) return false;

        if (show) RaiseWin(ix);
        else LowerWin(ix);

        if (iconic) IconicWin(ix);
        else DeIconicWin(ix);

        return true;
    }

    int WAdd() {
        TidyWindowsList();

        int wLSize = winList.size();
        for (int i = maxWin; i < wLSize; i++)
            if (winList[i] == NULL) return i;

        // plplot allows only 101 windows
        if (wLSize == 101) return -1;

        winList.push_back(NULL);
        oList.push_back(0);
        return wLSize;
    }

    GDLGStream* GetStreamAt(int wIx) const {
        return winList[wIx];
    }

    // should check for valid streams

    GDLGStream* GetStream(bool open = true) {
        TidyWindowsList();
        if (actWin == -1) {
            if (!open) return NULL;

            DString title = "GDL 0";
            DLong xSize, ySize;
            DefaultXYSize(&xSize, &ySize);
            bool success = WOpen(0, title, xSize, ySize, -1, -1);
            if (!success)
                return NULL;
            if (actWin == -1) {
                std::cerr << "Internal error: plstream not set." << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        return winList[actWin];
    }

    bool Decomposed(bool value) {
        decomposed = value;
        if (actWin < 0) return true;
        //update relevant values --- this should not be done at window level, but at Display level!!!!
        unsigned long nSystemColors = (1 << winList[actWin]->GetWindowDepth());
        unsigned long oldColor = (*static_cast<DLongGDL*> (SysVar::P()->GetTag(SysVar::P()->Desc()->TagIndex("COLOR"), 0)))[0];
        unsigned long oldNColor = (*static_cast<DLongGDL*> (dStruct->GetTag(n_colorsTag)))[0];
        if (this->decomposed == 1 && oldNColor == 256) {
            (*static_cast<DLongGDL*> (dStruct->GetTag(n_colorsTag)))[0] = nSystemColors;
            if (oldColor == 255) (*static_cast<DLongGDL*> (SysVar::P()->GetTag(SysVar::P()->Desc()->TagIndex("COLOR"), 0)))[0] = nSystemColors - 1;
        } else if (this->decomposed == 0 && oldNColor == nSystemColors) {
            (*static_cast<DLongGDL*> (dStruct->GetTag(n_colorsTag)))[0] = 256;
            if (oldColor == nSystemColors - 1) (*static_cast<DLongGDL*> (SysVar::P()->GetTag(SysVar::P()->Desc()->TagIndex("COLOR"), 0)))[0] = 255;
        }
        return true;
    }

    DLong GetDecomposed() {
        // initial setting (information from the X-server needed)
        if (decomposed == -1) {
            int Depth = wxDisplayDepth();
            decomposed = (Depth >= 15 ? true : false);
            unsigned long nSystemColors = (1 << Depth);
            unsigned long oldColor = (*static_cast<DLongGDL*> (SysVar::P()->GetTag(SysVar::P()->Desc()->TagIndex("COLOR"), 0)))[0];
            unsigned long oldNColor = (*static_cast<DLongGDL*> (dStruct->GetTag(n_colorsTag)))[0];
            if (this->decomposed == 1 && oldNColor == 256) {
                (*static_cast<DLongGDL*> (dStruct->GetTag(n_colorsTag)))[0] = nSystemColors;
                if (oldColor == 255) (*static_cast<DLongGDL*> (SysVar::P()->GetTag(SysVar::P()->Desc()->TagIndex("COLOR"), 0)))[0] = nSystemColors - 1;
            } else if (this->decomposed == 0 && oldNColor == nSystemColors) {
                (*static_cast<DLongGDL*> (dStruct->GetTag(n_colorsTag)))[0] = 256;
                if (oldColor == nSystemColors - 1) (*static_cast<DLongGDL*> (SysVar::P()->GetTag(SysVar::P()->Desc()->TagIndex("COLOR"), 0)))[0] = 255;
            }
            // was initially: 	DLong toto=16777216;
            //	if (Depth == 24) 
            //	  (*static_cast<DLongGDL*>(dStruct->GetTag(n_colorsTag)))[0] = toto;
            int debug = 0;
            if (debug) {
                cout << "Display Depth " << Depth << endl;
                cout << "n_colors " << nSystemColors << endl;
            }
        }
        if (decomposed) return 1;
        return 0;
    }

    bool SetGraphicsFunction(DLong value) {
        gcFunction = max(0, min(value, 15));
        TidyWindowsList();
        this->GetStream(); //to open a window if none opened.
        bool ret;
        for (int i = 0; i < winList.size(); i++) {
            if (winList[i] != NULL) ret = winList[i]->SetGraphicsFunction(gcFunction);
            if (ret == false) return ret;
        }
        return true;
    }

    DLong GetGraphicsFunction() {
        TidyWindowsList();
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
        TidyWindowsList();
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
        TidyWindowsList();
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
        TidyWindowsList();
        this->GetStream(); //to open a window if none opened.
        return winList[actWin]->GetVisualDepth();
    }

    DString GetVisualName() {
        TidyWindowsList();
        this->GetStream(); //to open a window if none opened.
        return winList[actWin]->GetVisualName();
    }

    DByteGDL* WindowState() {
        int maxwin = MaxWin();
        if (maxwin > 0) {
            DByteGDL* ret = new DByteGDL(dimension(maxwin), BaseGDL::NOZERO);
            for (int i = 0; i < maxwin; i++) (*ret)[i] = WState(i);
            return ret;
        } else return NULL;
    }

    bool CursorStandard(int cursorNumber) {
        cursorId = cursorNumber;
        TidyWindowsList();
        this->GetStream(); //to open a window if none opened.
        bool ret;
        for (int i = 0; i < winList.size(); i++) {
            if (winList[i] != NULL) ret = winList[i]->CursorStandard(cursorNumber);
            if (ret == false) return ret;
        }
        return true;
    }

    bool CursorCrosshair() {
        return true;
    }

    bool UnsetFocus() {
        return winList[actWin]->UnsetFocus();
    }

    bool SetBackingStore(int value) {
        backingStoreMode = value;
        return true;
    }

    bool CopyRegion(DLongGDL* me) {
        TidyWindowsList();
        DLong xs, ys, nx, ny, xd, yd;
        DLong source;
        xs = (*me)[0];
        ys = (*me)[1];
        nx = (*me)[2];
        ny = (*me)[3];
        xd = (*me)[4];
        yd = (*me)[5];
        if (me->N_Elements() == 7) source = (*me)[6];
        else source = actWin;
        if (!winList[ source]->GetRegion(xs, ys, nx, ny)) return false;
        return winList[ actWin ]->SetRegion(xd, yd, nx, ny);
    }

    int MaxWin() {
        TidyWindowsList();
        return winList.size();
    }

    int ActWin() {
        TidyWindowsList();
        return actWin;
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

};
#undef maxWin
#undef maxWinReserve
#endif
#endif