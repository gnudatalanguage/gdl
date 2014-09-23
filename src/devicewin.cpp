/* *************************************************************************
                          devicewin.cpp  -  win32 gdi interface
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
#include "gdlwinstream.hpp"
#include "devicewin.hpp"

#include "plotting.hpp"

 
  void DeviceWIN::DefaultXYSize(DLong *xSize,DLong  *ySize) 
  {
   *xSize=680; *ySize=480; 
#if defined (_WIN32) 
    // the following not tested (no access to windows) (GD)
    bool noQscreen=true;
    string gdlQscreen=GetEnvString("GDL_GR_WIN_QSCREEN");
    if( gdlQscreen == "1") noQscreen=false;
    string gdlXsize=GetEnvString("GDL_GR_WIN_WIDTH");
    if( gdlXsize != "" && noQscreen) xSize=atoi(gdlXsize.c_str()); 
    string gdlYsize=GetEnvString("GDL_GR_WIN_HEIGHT");
    if( gdlYsize != "" && noQscreen) ySize=atoi(gdlYsize.c_str()); 
#endif
  return;}

  void DeviceWIN::MaxXYSize(DLong *xSize, DLong *ySize)
  {
	*xSize = GetSystemMetrics(SM_CXSCREEN);
	*ySize = GetSystemMetrics(SM_CYSCREEN);

   }


  void DeviceWIN::EventHandler()
  {
    int wLSize = winList.size();
    for( int i=0; i<wLSize; i++) {
      if( winList[ i] != NULL)	winList[ i]->EventHandler();
    }
    TidyWindowsList();
  }

  bool DeviceWIN::WDelete( int wIx)
  {
	TidyWindowsList();

    int wLSize = winList.size();
    if( wIx >= wLSize || wIx < 0 || winList[ wIx] == NULL)
      return false;

    delete winList[ wIx];
    winList[ wIx] = NULL;
    oList[ wIx] = 0;

    // set to most recently created
    std::vector< long>::iterator mEl = 
      std::max_element( oList.begin(), oList.end());
    
    // no window open
    if( *mEl == 0) 
      {
	SetActWin( -1);
	oIx = 1;
      }
    else
      SetActWin( std::distance( oList.begin(), mEl)); 

    return true;
  }

  bool DeviceWIN::WOpen( int wIx, const std::string& title, 
	      int xSize, int ySize, int xPos, int yPos)
  {

    int debug=0;
    if(debug) cout << " DeviceWIN:WOpen : " << xSize <<" "<< ySize<<" "<< xPos<<" "<< yPos;
    TidyWindowsList();

    int wLSize = winList.size();
    if( wIx >= wLSize || wIx < 0)
      return false;

    if( winList[ wIx] != NULL)
      {
	delete winList[ wIx];
	winList[ wIx] = NULL;
      }

    DLongGDL* pMulti = SysVar::GetPMulti();
    DLong nx = (*pMulti)[ 1];
    DLong ny = (*pMulti)[ 2];

    if( nx <= 0) nx = 1;
    if( ny <= 0) ny = 1;

    winList[ wIx] = new GDLWINStream( nx, ny);
    
    // as wxwidgets never set this, they can be intermixed
    oList[ wIx]   = oIx++;

    // set initial window size
    PLFLT xp; PLFLT yp; 
    PLINT xleng; PLINT yleng;
    PLINT xoff; PLINT yoff;
    if(debug) cout << "WOpen: ->plstream:gpage(";
    winList[ wIx]->plstream::gpage( xp, yp, xleng, yleng, xoff, yoff);
// <<<< alternate block in devicex.hpp
		xleng = xSize;
		yleng = ySize;
		xoff = xPos;
		yoff = yPos;
// >>>>
    if(debug) cout << " )WOpen: ->spage(";
    winList[ wIx]->spage( xp, yp, xleng, yleng, xoff, yoff);

    if(debug) cout << " )\n WOpen: ->spause(";

    // no pause on win destruction
    winList[ wIx]->spause( false);
    if(debug) cout << " ) WOpen: ->fontld(";

    // extended fonts
    winList[ wIx]->fontld( 1);
    if(debug) cout << " ) WOpen: ->scolor(";


    // we want color
    winList[ wIx]->scolor( 1);

    // window title
    static char buf[ 256];
    strncpy( buf, title.c_str(), 255);
    buf[ 255] = 0;
   if(debug) cout << " )\n WOpen: ->SETOPT(plwindow,buf) \n";
    winList[ wIx]->SETOPT( "plwindow", buf);

		// we want color (and the driver options need to be overwritten)
		// winList[ wIx]->SETOPT( "drvopt","color=1");
// ---- alternate SETOPT in devicex.hpp
		// set color map
    PLINT r[256], g[256], b[256];
    actCT.Get( r, g, b);
    // winList[ wIx]->scmap0( r, g, b, actCT.size());
    winList[ wIx]->scmap1( r, g, b, ctSize);

   if(debug) cout << " WOpen:winList[ wIx]->Init(";

    winList[ wIx]->Init();
if(debug) cout << " )\n WOpen:winList[ wIx]->ssub(1,1)" 
               << "adv(0) font(1) vpor(0,1,0,1) wind(0,1,0,1";
			   //----------------------
				//----------------------
   // need to be called initially. permit to fix things
    winList[ wIx]->ssub(1,1);
    winList[ wIx]->adv(0);
    // load font
    winList[ wIx]->font( 1);
    winList[ wIx]->vpor(0,1,0,1);
    winList[ wIx]->wind(0,1,0,1);
   if(debug) cout << " )\n WOpen:winList[ wIx]->DefaultCharsize(";

    winList[ wIx]->DefaultCharSize();
   if(debug) cout << " )\n WOpen:winList[ wIx]->updatePageInfo(";
    //in case these are not initalized, here is a good place to do it.
    if (winList[ wIx]->updatePageInfo()==true)
      {
        winList[ wIx]->GetPlplotDefaultCharSize(); //initializes everything in fact..

      }
    // sets actWin and updates !D
    SetActWin( wIx);
   if(debug) cout << " WOpen return true;} \n ";

    return true; //winList[ wIx]->Valid(); // Valid() need to called once
  }

  bool DeviceWIN::WState( int wIx)
  { 
    return wIx >= 0 && wIx < oList.size() && oList[ wIx] != 0;
  }

  bool DeviceWIN::WSet(int wIx)
  {
  	TidyWindowsList();

  	int wLSize = winList.size();
  	if (wIx >= wLSize || wIx < 0 || winList[wIx] == NULL)
  		return false;

  	SetActWin(wIx);
  	return true;
  }
  bool DeviceWIN::WSize( int wIx, int *xSize, int *ySize, int *xPos, int *yPos)
  {
    TidyWindowsList();

    int wLSize = winList.size();
    if( wIx > wLSize || wIx < 0)      return false;
    long xleng, yleng, xpos, ypos;
    winList[ wIx]->GetGeometry( xleng, yleng, xpos, ypos);
    *xSize=xleng; *ySize=yleng;
    *xPos =xpos;  *yPos =ypos;
    return true;
  }

  bool DeviceWIN::WShow( int ix, bool show, bool iconic)
  {
    TidyWindowsList();

    int wLSize = winList.size();
    if (ix >= wLSize || ix < 0 || winList[ ix] == NULL) return false;

    if (iconic) winList[ ix]->Iconic();   else winList[ ix]->DeIconic();
    if (show) winList[ ix]->Raise();      else winList[ ix]->Lower();

    UnsetFocus();

    return true;
  }

  int DeviceWIN::WAdd()
  {
    TidyWindowsList();

    int wLSize = winList.size();
    for( int i=0; i<wLSize; i++)
      if( winList[ i] == NULL) return i;

    // plplot allows only 101 windows
    if( wLSize == 101) return -1;

    winList.push_back( NULL);
    oList.push_back( 0);
    return wLSize;
  }

 
  DIntGDL* DeviceWIN::GetWindowPosition()
  {
      TidyWindowsList();
      this->GetStream(); //to open a window if none opened.
      long xpos,ypos;
      if ( winList[ actWin]->GetWindowPosition(xpos,ypos) ) {
          DIntGDL* res;
          res = new DIntGDL(2, BaseGDL::NOZERO);
          (*res)[0] = xpos;
          (*res)[1] = ypos;
          return res;
      }
      else return NULL;
  }
