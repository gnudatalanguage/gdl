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


#ifndef free_mem
#define free_mem(a)					\
if (a != NULL) { free((void *)a); a = NULL; }
#endif

#ifdef HAVE_OLDPLPLOT
#define SETOPT SetOpt
#else
#define SETOPT setopt
#endif

#define maxWin 65  //IDL has 65...
#define maxWinReserve 256

class DeviceWX : public GraphicsDevice
{
private:
	std::vector<GDLGStream*> winList;
	std::vector<long>        oList;
	long oIx;
	int  actWin;
	int decomposed; // false -> use color table
	int cursorId; //should be 3 by default.
	long gcFunction;
	int backingStoreMode;

  int getCursorId(){ return cursorId; }
  long getGCFunction(){ return gcFunction; }
  int GetBackingStore(){ return backingStoreMode; }
  
	void SetActWin(int wIx)
	{
		// update !D
		if (wIx >= 0 && wIx < winList.size())
		{
			long xsize, ysize, xoff, yoff;
			winList[wIx]->GetGeometry(xsize, ysize, xoff, yoff);
	
			(*static_cast<DLongGDL*>(dStruct->GetTag(xSTag)))[0] = xsize;
			(*static_cast<DLongGDL*>(dStruct->GetTag(ySTag)))[0] = ysize;
			(*static_cast<DLongGDL*>(dStruct->GetTag(xVSTag)))[0] = xsize;
			(*static_cast<DLongGDL*>(dStruct->GetTag(yVSTag)))[0] = ysize;
			// number of colors
			//        (*static_cast<DLongGDL*>( dStruct->GetTag( n_colorsTag)))[0] = 1 << winList[ wIx]->GetWindowDepth();
			// set !D.N_COLORS and !P.COLORS according to decomposed value.
			unsigned long nSystemColors = (1 << winList[wIx]->GetWindowDepth());
			unsigned long oldColor = (*static_cast<DLongGDL*>(SysVar::P()->GetTag(SysVar::P()->Desc()->TagIndex("COLOR"), 0)))[0];
			unsigned long oldNColor = (*static_cast<DLongGDL*>(dStruct->GetTag(n_colorsTag)))[0];
			if (this->decomposed == -1) decomposed = this->GetDecomposed();
			if (this->decomposed == 1 && oldNColor == 256) {
				(*static_cast<DLongGDL*>(dStruct->GetTag(n_colorsTag)))[0] = nSystemColors;
				if (oldColor == 255) (*static_cast<DLongGDL*>(SysVar::P()->GetTag(SysVar::P()->Desc()->TagIndex("COLOR"), 0)))[0] = nSystemColors - 1;
			}			else if (this->decomposed == 0 && oldNColor == nSystemColors) {
				(*static_cast<DLongGDL*>(dStruct->GetTag(n_colorsTag)))[0] = 256;
				if (oldColor == nSystemColors - 1) (*static_cast<DLongGDL*>(SysVar::P()->GetTag(SysVar::P()->Desc()->TagIndex("COLOR"), 0)))[0] = 255;
			}
		}
		// window number
		(*static_cast<DLongGDL*>(dStruct->GetTag(wTag)))[0] = wIx;

		actWin = wIx;
	}

	void RaiseWin(int wIx)
	{
		if (wIx >= 0 && wIx < winList.size()) winList[wIx]->Raise();
	}

	void LowerWin(int wIx)
	{
		if (wIx >= 0 && wIx < winList.size()) winList[wIx]->Lower();
	}

	void IconicWin(int wIx)
	{
		if (wIx >= 0 && wIx < winList.size()) winList[wIx]->Iconic();
	}
	void DeIconicWin(int wIx)
	{
		if (wIx >= 0 && wIx < winList.size()) winList[wIx]->DeIconic();
	}

	// process user deleted windows
	// should be done in a thread
	void TidyWindowsList()
	{
		int wLSize = winList.size();

		//     bool redo;
		//     do { // it seems that the event queue is only searched a few events deep
		//       redo = false;
		for (int i = 0; i<wLSize; i++)
		if (winList[i] != NULL && !winList[i]->GetValid())
		{
			delete winList[i];
			winList[i] = NULL;
			oList[i] = 0;
			// 	    redo = true;
		}
		//     } while( redo);


		// set new actWin IF NOT VALID ANY MORE
		if (actWin < 0 || actWin >= wLSize ||
			winList[actWin] == NULL || !winList[actWin]->GetValid())
		{
			// set to most recently created
			std::vector< long>::iterator mEl =
				std::max_element(oList.begin(), oList.end());

			// no window open
			if (*mEl == 0)
			{
				SetActWin(-1);
				oIx = 1;
			}
			else
				SetActWin(std::distance(oList.begin(), mEl));
		}
	}

public:
	DeviceWX() : GraphicsDevice(), oIx(1), actWin(-1), decomposed(-1), gcFunction(3), backingStoreMode(0)
	{
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

	~DeviceWX()
	{
		std::vector<GDLGStream*>::iterator i;
		for (i = winList.begin(); i != winList.end(); ++i)
		{ delete *i; /* *i = NULL;*/}
	}

	   GDLGStream* GetStream( int wIx) const 
	   { 
	     return winList[ wIx];
	   }
	   
	void EventHandler()
	{
		if (actWin<0) return; //would this have side effects?  
		int wLSize = winList.size();
		for (int i = 0; i<wLSize; i++)
		if (winList[i] != NULL)
			winList[i]->EventHandler();

		TidyWindowsList();
	}

	bool WDelete(int wIx)
	{
		TidyWindowsList();

		int wLSize = winList.size();
		if (wIx >= wLSize || wIx < 0 || winList[wIx] == NULL)
			return false;

		if (dynamic_cast<GDLWXStream*>(winList[wIx]) != NULL)
		{
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
		if (*mEl == 0)
		{
			SetActWin(-1);
			oIx = 1;
		}
		else
			SetActWin(std::distance(oList.begin(), mEl));

		return true;
	}
        
  bool WOpen( int wIx,  const std::string& title,
          int xSize, int ySize, int xPos, int yPos)
  {
          TidyWindowsList();

    int wLSize = winList.size();
    if( wIx >= wLSize || wIx < 0)
      return false;

    if( winList[ wIx] != NULL)
      {
        delete winList[ wIx];
        winList[ wIx] = NULL;
      }

    wxWindow *wxParent = NULL;
  
    GUIMutexLockerWidgetsT gdlMutexGuiEnterLeave;

    wxString titleWxString = wxString( title.c_str( ), wxConvUTF8 );
    GDLFrame *gdlFrame = new GDLFrame( 0 , 0 , 0, titleWxString );
//    m_gdlFrameOwnerMutexP = gdlFrame->m_gdlFrameOwnerMutexP;
//    assert( m_gdlFrameOwnerMutexP != NULL );
    //     gdlFrame->Freeze();

    gdlFrame->SetSize( xSize, ySize );

    wxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
    gdlFrame->SetSizer( topSizer );

    wxPanel *panel = new wxPanel( gdlFrame, wxID_ANY );
    wxSizer *sizer = new wxBoxSizer( wxVERTICAL );
    panel->SetSizer( sizer );
    topSizer->Add( panel );
    GDLDrawPanel* gdlWindow = new GDLDrawPanel( gdlFrame, panel->GetId(), wxDefaultPosition, wxSize(xSize,ySize), wxBORDER_SIMPLE);
    topSizer->Add( gdlWindow, 0, wxEXPAND|wxALL, 5);
  
    gdlWindow->InitStream();
    winList[ wIx] = static_cast<GDLGStream*> (GraphicsDevice::GetGUIDevice( )->GetStreamAt( gdlWindow->PStreamIx() ));
    static_cast<GDLWXStream*>(winList[ wIx])->SetGDLDrawPanel(gdlWindow);
    gdlFrame->Show();
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
    // sets actWin and updates !D
         SetActWin( wIx);

    return true; //winList[ wIx]->Valid(); // Valid() need to called once
  } 

	bool WState(int wIx)
	{
		return wIx >= 0 && wIx < oList.size() && oList[wIx] != 0;
	}

	bool WSize(int wIx, int *xSize, int *ySize, int *xPos, int *yPos)
	{
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

	bool WSet(int wIx)
	{
		TidyWindowsList();

		int wLSize = winList.size();
		if (wIx >= wLSize || wIx < 0 || winList[wIx] == NULL)
			return false;

		SetActWin(wIx);
		return true;
	}

	bool WShow(int ix, bool show, bool iconic)
	{
		TidyWindowsList();

		int wLSize = winList.size();
		if (ix >= wLSize || ix < 0 || winList[ix] == NULL) return false;

		if (show) RaiseWin(ix); else LowerWin(ix);

		if (iconic) IconicWin(ix); else DeIconicWin(ix);

		return true;
	}

	int WAdd()
	{
		TidyWindowsList();

		int wLSize = winList.size();
		for (int i = maxWin; i<wLSize; i++)
		if (winList[i] == NULL) return i;

		// plplot allows only 101 windows
		if (wLSize == 101) return -1;

		winList.push_back(NULL);
		oList.push_back(0);
		return wLSize;
	}

	GDLGStream* GetStreamAt(int wIx) const
	{
		return winList[wIx];
	}

	// should check for valid streams
	GDLGStream* GetStream(bool open = true)
	{
		TidyWindowsList();
		if (actWin == -1)
		{
			if (!open) return NULL;

			DString title = "GDL 0";
			DLong xSize, ySize;
			DefaultXYSize(&xSize, &ySize);
			bool success = WOpen(0, title, xSize, ySize, -1, -1);
			if (!success)
				return NULL;
			if (actWin == -1)
			{
				std::cerr << "Internal error: plstream not set." << std::endl;
				exit(EXIT_FAILURE);
			}
		}
		return winList[actWin];
	}

	bool Decomposed(bool value)
	{
		decomposed = value;
		if (actWin<0) return true;
		//update relevant values --- this should not be done at window level, but at Display level!!!!
		unsigned long nSystemColors = (1 << winList[actWin]->GetWindowDepth());
		unsigned long oldColor = (*static_cast<DLongGDL*>(SysVar::P()->GetTag(SysVar::P()->Desc()->TagIndex("COLOR"), 0)))[0];
		unsigned long oldNColor = (*static_cast<DLongGDL*>(dStruct->GetTag(n_colorsTag)))[0];
		if (this->decomposed == 1 && oldNColor == 256) {
			(*static_cast<DLongGDL*>(dStruct->GetTag(n_colorsTag)))[0] = nSystemColors;
			if (oldColor == 255) (*static_cast<DLongGDL*>(SysVar::P()->GetTag(SysVar::P()->Desc()->TagIndex("COLOR"), 0)))[0] = nSystemColors - 1;
		}		else if (this->decomposed == 0 && oldNColor == nSystemColors) {
			(*static_cast<DLongGDL*>(dStruct->GetTag(n_colorsTag)))[0] = 256;
			if (oldColor == nSystemColors - 1) (*static_cast<DLongGDL*>(SysVar::P()->GetTag(SysVar::P()->Desc()->TagIndex("COLOR"), 0)))[0] = 255;
		}
		return true;
	}

	DLong GetDecomposed()
	{
		// initial setting (information from the X-server needed)
		if (decomposed == -1)
		{
			int Depth = wxDisplayDepth();
			decomposed = (Depth >= 15 ? true : false);
			unsigned long nSystemColors = (1 << Depth);
			unsigned long oldColor = (*static_cast<DLongGDL*>(SysVar::P()->GetTag(SysVar::P()->Desc()->TagIndex("COLOR"), 0)))[0];
			unsigned long oldNColor = (*static_cast<DLongGDL*>(dStruct->GetTag(n_colorsTag)))[0];
			if (this->decomposed == 1 && oldNColor == 256) {
				(*static_cast<DLongGDL*>(dStruct->GetTag(n_colorsTag)))[0] = nSystemColors;
				if (oldColor == 255) (*static_cast<DLongGDL*>(SysVar::P()->GetTag(SysVar::P()->Desc()->TagIndex("COLOR"), 0)))[0] = nSystemColors - 1;
			}			else if (this->decomposed == 0 && oldNColor == nSystemColors) {
				(*static_cast<DLongGDL*>(dStruct->GetTag(n_colorsTag)))[0] = 256;
				if (oldColor == nSystemColors - 1) (*static_cast<DLongGDL*>(SysVar::P()->GetTag(SysVar::P()->Desc()->TagIndex("COLOR"), 0)))[0] = 255;
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

	bool SetGraphicsFunction(DLong value)
	{
		gcFunction = max(0, min(value, 15));
		TidyWindowsList();
		this->GetStream(); //to open a window if none opened.
		bool ret;
		for (int i = 0; i<winList.size(); i++) {
			if (winList[i] != NULL) ret = winList[i]->SetGraphicsFunction(gcFunction);
			if (ret == false) return ret;
		}
		return true;
	}

	DLong GetGraphicsFunction()
	{
		TidyWindowsList();
		this->GetStream(); //to open a window if none opened.
		return gcFunction;
	}

	DIntGDL* GetScreenSize(char* disp)
	{
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
                if  (winList[actWin]->GetScreenResolution(resx,resy)) {
			DDoubleGDL* res;
			res = new DDoubleGDL(2, BaseGDL::NOZERO);
			(*res)[0] = resx;
			(*res)[1] = resy;
			return res;
		}
		else return NULL;
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
		}
		else return NULL;
	}

	DLong GetVisualDepth()
	{
		TidyWindowsList();
		this->GetStream(); //to open a window if none opened.
		return winList[actWin]->GetVisualDepth();
	}

	DString GetVisualName()
	{
		TidyWindowsList();
		this->GetStream(); //to open a window if none opened.
		return winList[actWin]->GetVisualName();
	}

	DByteGDL* WindowState()
	{
		int maxwin = MaxWin();
		if (maxwin > 0){
			DByteGDL* ret = new DByteGDL(dimension(maxwin), BaseGDL::NOZERO);
			for (int i = 0; i < maxwin; i++) (*ret)[i] = WState(i);
			return ret;
		}		else return NULL;
	}

	bool CursorStandard(int cursorNumber)
	{
		cursorId = cursorNumber;
		TidyWindowsList();
		this->GetStream(); //to open a window if none opened.
		bool ret;
		for (int i = 0; i<winList.size(); i++) {
			if (winList[i] != NULL) ret = winList[i]->CursorStandard(cursorNumber);
			if (ret == false) return ret;
		}
		return true;
	}

	bool CursorCrosshair()
	{
		return true;
	}


	bool UnsetFocus()
	{
		return winList[actWin]->UnsetFocus();
	}

	bool SetFocus()
	{
		return winList[actWin]->SetFocus();
	}

	bool SetBackingStore(int value)
	{
		backingStoreMode = value;
		return true;
	}

	int MaxWin() { TidyWindowsList(); return winList.size(); }
	int ActWin() { TidyWindowsList(); return actWin; }

//	BaseGDL* TVRD(EnvT* e)
//	{
//		// AC 17 march 2012: needed to catch the rigth current window (wset ...)
//		DLong wIx = -1;
//		GraphicsDevice* actDevice = GraphicsDevice::GetDevice();
//		wIx = actDevice->ActWin();
//		bool success = actDevice->WSet(wIx);
//		int debug = 0;
//		if (debug) cout << "wIx :" << wIx << " " << success << endl;
//
//		//everywhere we use XGetImage we need to set an error handler, since GTK crashes on every puny
//		//BadMatch error, and if you read the XGetImage doc you'll see that such errors are prone to happen
//		//as soon as part of the window is obscured.
//
//		PLStream* plsShouldNotBeUsed;
//		plgpls(&plsShouldNotBeUsed);
//		wxPLDevBase *dev = (wxPLDevBase *)plsShouldNotBeUsed->dev;
//		if (dev == NULL || dev->m_frame == NULL)
//		{
//			GDLGStream* newStream = actDevice->GetStream();
//			//already done: newStream->Init();
//			plgpls(&plsShouldNotBeUsed);
//			dev = (wxPLDevBase *)plsShouldNotBeUsed->dev;
//			if (dev == NULL) e->Throw("Device not open.");
//		}
//
//		wxMemoryDC memDC;
//
//		if (e->KeywordSet("WORDS")) e->Throw("WORDS keyword not yet supported.");
//		DLong orderVal = SysVar::TV_ORDER();
//		e->AssureLongScalarKWIfPresent("ORDER", orderVal);
//
//		/* this variable will contain the client size of the window. */
//		wxSize win_attr = dev->m_frame->GetClientSize();
//
//		/* query the window's client size. */
//		unsigned int xMaxSize = win_attr.GetWidth();
//		unsigned int yMaxSize = win_attr.GetHeight();
//
//		SizeT dims[3];
//
//		DByteGDL* res;
//
//		DLong tru = 0;
//		e->AssureLongScalarKWIfPresent("TRUE", tru);
//		if (tru > 3 || tru < 0) e->Throw("Value of TRUE keyword is out of allowed range.");
//
//		DLong channel = -1;
//
//		unsigned int x_gdl = 0;
//		unsigned int y_gdl = 0;
//		unsigned int nx_gdl = xMaxSize;
//		unsigned int ny_gdl = yMaxSize;
//
//		bool error = false;
//		bool hasXsize = false;
//		bool hasYsize = false;
//		int nParam = e->NParam();
//		if (nParam >= 4) {
//			DLongGDL* Ny = e->GetParAs<DLongGDL>(3);
//			ny_gdl = (*Ny)[0];
//			hasYsize = true;
//		}
//		if (nParam >= 3) {
//			DLongGDL* Nx = e->GetParAs<DLongGDL>(2);
//			nx_gdl = (*Nx)[0];
//			hasXsize = true;
//		}
//		if (nParam >= 2) {
//			DLongGDL* y0 = e->GetParAs<DLongGDL>(1);
//			y_gdl = (*y0)[0];
//		}
//		if (nParam >= 1) {
//			DLongGDL* x0 = e->GetParAs<DLongGDL>(0);
//			x_gdl = (*x0)[0];
//		}
//		if (nParam == 5) {
//			DLongGDL* ChannelGdl = e->GetParAs<DLongGDL>(4);
//			channel = (*ChannelGdl)[0];
//		}
//		e->AssureLongScalarKWIfPresent("CHANNEL", channel);
//		if (channel > 3) e->Throw("Value of Channel is out of allowed range.");
//
//		if (debug) {
//			cout << x_gdl << " " << y_gdl << " " << nx_gdl << " " << ny_gdl << " " << channel << endl;
//		}
//		if (!(hasXsize))nx_gdl -= x_gdl;
//		if (!(hasYsize))ny_gdl -= y_gdl;
//
//		DLong xref, xval, xinc, yref, yval, yinc, xmax11, ymin11;
//		int x_11 = 0;
//		int y_11 = 0;
//		xref = 0; xval = 0; xinc = 1;
//		yref = yMaxSize - 1; yval = 0; yinc = -1;
//
//		x_11 = xval + (x_gdl - xref)*xinc;
//		y_11 = yval + (y_gdl - yref)*yinc;
//		xmax11 = xval + (x_gdl + nx_gdl - 1 - xref)*xinc;
//		ymin11 = yval + (y_gdl + ny_gdl - 1 - yref)*yinc;
//		if (debug) {
//			cout << "[" << x_11 << "," << xmax11 << "],[" << ymin11 << "," << y_11 << "]" << endl;
//		}
//		if (y_11 < 0 || y_11 > yMaxSize - 1) error = true;
//		if (x_11 < 0 || x_11 > xMaxSize - 1) error = true;
//		if (xmax11 < 0 || xmax11 > xMaxSize - 1) error = true;
//		if (ymin11 < 0 || ymin11 > yMaxSize - 1) error = true;
//		if (error) e->Throw("Value of Area is out of allowed range.");
//
//		wxBitmap wxBM(nx_gdl, ny_gdl, -1);; // will be converted into Image
//		memDC.SelectObject(wxBM);
//		dev->BlitRectangle(&memDC, (int)x_11, (int)ymin11, nx_gdl, ny_gdl);
//		memDC.SelectObject(wxNullBitmap);
//		wxImage wxImg = wxBM.ConvertToImage();
//#define PAD 4
//		//   printf("\t width = %d\n", ximg->width);
//		//   printf("\t height = %d\n", ximg->height);
//		//   printf("\t xoffset = %d\n", ximg->xoffset);
//		//   printf("\t byte_order = %d\n", ximg->byte_order);
//		//   printf("\t bitmap_unit = %d\n", ximg->bitmap_unit);
//		//   printf("\t bitmap_bit_order = %d\n", ximg->bitmap_bit_order);
//		//   printf("\t bitmap_pad = %d\n", ximg->bitmap_pad);
//		//   printf("\t depth = %d\n", ximg->depth);
//		//   printf("\t bits_per_pixel = %d\n", ximg->bits_per_pixel);
//		//   printf("\t bytes_per_line = %d\n", ximg->bytes_per_line);
//		//   printf("\t red_mask = %x\n", ximg->red_mask);
//		//   printf("\t green_mask = %x\n", ximg->green_mask);
//		//   printf("\t blue_mask = %x\n", ximg->blue_mask);
//
//		if (wxBM.GetDepth() != 32)
//			e->Throw("Sorry, Display of bits_per_pixel different from 32 are unsupported (FIXME).");
//
//		unsigned char* imgdata = wxImg.GetData();
//		if (tru == 0) {
//			dims[0] = nx_gdl;
//			dims[1] = ny_gdl;
//			dimension dim(dims, (SizeT)2);
//			res = new DByteGDL(dim, BaseGDL::ZERO);
//
//			if (&wxImg == NULL) return res;
//
//			if (channel <= 0) { //channel not given, return max of the 3 channels
//				DByte mx, mx1;
//				for (SizeT i = 0; i < dims[0] * dims[1]; ++i) {
//					mx = (DByte)imgdata[PAD * i];
//					mx1 = (DByte)imgdata[PAD * i + 1];
//					if (mx1 > mx) mx = mx1;
//					mx1 = (DByte)imgdata[PAD * i + 2];
//					if (mx1 > mx) mx = mx1;
//					(*res)[i] = mx;
//				}
//			}
//			else {
//				for (SizeT i = 0; i < dims[0] * dims[1]; ++i) {
//					(*res)[i] = imgdata[PAD * i + channel]; //0=R,1:G,2:B,3:Alpha
//				}
//			}
//			// Reflect about y-axis
//			if (orderVal == 0) res->Reverse(1);
//			return res;
//
//		}
//		else {
//			dims[0] = 3;
//			dims[1] = nx_gdl;
//			dims[2] = ny_gdl;
//			dimension dim(dims, (SizeT)3);
//			res = new DByteGDL(dim, BaseGDL::NOZERO);
//			if (&wxBM == NULL) return res;
//
//			for (SizeT i = 0, kpad = 0; i < dims[1] * dims[2]; ++i)
//			{
//				for (SizeT j = 0; j<3; ++j) (*res)[(i + 1) * 3 - (j + 1)] = imgdata[kpad++];
//				kpad++;
//			}
//
//			// Reflect about y-axis
//			if (orderVal == 0) res->Reverse(2);
//
//			DUInt* perm = new DUInt[3];
//			if (tru == 1) {
//				return res;
//			}
//			else if (tru == 2) {
//				perm[0] = 1;
//				perm[1] = 0;
//				perm[2] = 2;
//				return res->Transpose(perm);
//			}
//			else if (tru == 3) {
//				perm[0] = 1;
//				perm[1] = 2;
//				perm[2] = 0;
//				return res->Transpose(perm);
//			}
//		}
//		assert(false);
//		return NULL;
//#undef PAD 
//	}

	void DefaultXYSize(DLong *xSize, DLong *ySize)
	{
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

	void MaxXYSize(DLong *xSize, DLong *ySize)
	{
		*xSize = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
		*ySize = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);
	}

};
#undef maxWin
#undef maxWinReserve
#endif
#endif