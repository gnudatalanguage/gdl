/* *************************************************************************
                          devicewin.hpp  -  M$ windows device
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

#ifndef DEVICEWIN_HPP_
#define DEVICEWIN_HPP_

#include <algorithm>
#include <vector>
#include <cstring>

#include <plplot/drivers.h>

#include "graphicsdevice.hpp"
#include "gdlwinstream.hpp"
#include "initsysvar.hpp"
#include "gdlexception.hpp"

#ifdef HAVE_OLDPLPLOT
#define SETOPT SetOpt
#else
#define SETOPT setopt
#endif

#define maxWin 65
#define maxWinReserve 100

class DeviceWIN : public GraphicsDevice
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
  void EventHandler();

  bool WDelete( int );
  bool WOpen( int , const std::string& , 
	      int , int , int , int );
  bool WState( int );
 // bool WSize( int , int*, int*, int*, int*);
  bool WSet( int );
 // bool WShow( int, bool, bool);
  int WAdd();
//> 	******					Device::SetActWin
  void SetActWin( int wIx)
  {
	// update !D
    if (wIx >= 0 && wIx < winList.size()) {	// window size and pos
	  PLFLT xp; PLFLT yp;
	  PLINT xleng; PLINT yleng;
	  PLINT xoff; PLINT yoff;
	  winList[wIx]->gpage(xp, yp, xleng, yleng, xoff, yoff);
// GetGeometry, GetWindowsize are not impleented in gdlwinstream.
	  (*static_cast<DLongGDL*>(dStruct->GetTag(xSTag)))[0] = xleng;
	  (*static_cast<DLongGDL*>(dStruct->GetTag(ySTag)))[0] = yleng;
	  (*static_cast<DLongGDL*>(dStruct->GetTag(xVSTag)))[0] = xleng;
	  (*static_cast<DLongGDL*>(dStruct->GetTag(yVSTag)))[0] = yleng;
    }

		// window number
    (*static_cast<DLongGDL*>(dStruct->GetTag(wTag)))[0] = wIx;

    actWin = wIx;
  }

	// process user deleted windows
//> 	******					DeviceWIN::TidyWindowsList
  void TidyWindowsList()
  {
    int wLSize = winList.size();

    for( int i=0; i<wLSize; i++) {
      if( winList[ i] != NULL && !winList[ i]->GetValid()) {
	    delete winList[ i];
	    winList[ i] = NULL;	    oList[ i] = 0;
	  }
    }
    // set new actWin IF NOT VALID ANY MORE
    if( actWin < 0 || actWin >= wLSize || 
			  winList[ actWin] == NULL ||
			 !winList[ actWin]->GetValid())      {
	  // set to most recently created
	  std::vector< long>::iterator mEl = 
	  		std::max_element( oList.begin(), oList.end());
    
	  // no window open
	  if( *mEl == 0)  {
	    SetActWin( -1);	    oIx = 1;
	  }	else {
	    SetActWin( std::distance( oList.begin(), mEl)); 
      }
    }
  }


public:
  DeviceWIN(): GraphicsDevice(), oIx( 1), actWin( -1), decomposed( -1)
  {
    name = "WIN";

    DLongGDL origin( dimension( 2));
    DLongGDL zoom( dimension( 2));
    zoom[0] = 1;
    zoom[1] = 1;

    dStruct = new DStructGDL( "!DEVICE");
    dStruct->InitTag("NAME",       DStringGDL( name)); 
    dStruct->InitTag("X_SIZE",     DLongGDL( 640)); 
    dStruct->InitTag("Y_SIZE",     DLongGDL( 512)); 
    dStruct->InitTag("X_VSIZE",    DLongGDL( 640)); 
    dStruct->InitTag("Y_VSIZE",    DLongGDL( 512)); 
    dStruct->InitTag("X_CH_SIZE",  DLongGDL(9));
    dStruct->InitTag("Y_CH_SIZE",  DLongGDL(12));
    dStruct->InitTag("X_PX_CM",    DFloatGDL( 40.0)); 
    dStruct->InitTag("Y_PX_CM",    DFloatGDL( 40.0)); 
    dStruct->InitTag("N_COLORS",   DLongGDL( 256)); 
    dStruct->InitTag("TABLE_SIZE", DLongGDL( ctSize)); 
    dStruct->InitTag("FILL_DIST",  DLongGDL( 0)); 
    dStruct->InitTag("WINDOW",     DLongGDL( -1)); 
    dStruct->InitTag("UNIT",       DLongGDL( 0)); 
    dStruct->InitTag("FLAGS",      DLongGDL( 328124)); 
    dStruct->InitTag("ORIGIN",     origin); 
    dStruct->InitTag("ZOOM",       zoom); 

    winList.reserve( maxWinReserve);
    winList.resize( maxWin);    
    for( int i=0; i < maxWin; i++) winList[ i] = NULL;
    oList.reserve( maxWinReserve);
    oList.resize( maxWin);
    for( int i=0; i < maxWin; i++) oList[ i] = 0;

    //     GDLGStream::SetErrorHandlers();
  }
  
	~DeviceWIN()
  {
    std::vector<GDLGStream*>::iterator i;
    for(i = winList.begin(); i != winList.end(); ++i) 
      { delete *i; /* *i = NULL;*/}
  }

  GDLGStream* GetStreamAt( int wIx) const 
  { 
    return winList[ wIx];
  }
  

 
  // should check for valid streams
  GDLGStream* GetStream( bool open=true)
   { /* This routine looks broken gvj 8/2014 */
    TidyWindowsList();
    if( actWin == -1) {
	  if( !open) return NULL;

	  DString title = "GDL 0";
	  DLong xSize, ySize;
			//DefaultXYSize(&xSize, &ySize);
	  xSize = 640; ySize = 480;
	  bool success = WOpen( 0, title, xSize, ySize, -1, -1);
	  if( !success)	  return NULL;
	  if( actWin == -1)	  {
	    std::cerr << "Internal error: plstream not set." << std::endl;
	    exit( EXIT_FAILURE);
	  }
    }
    return winList[ actWin];
  }

	bool Decomposed(bool value)
	{
		decomposed = value;
		return true;
	}




	int MaxWin() { TidyWindowsList(); return maxWin; }
	int ActWin() { TidyWindowsList(); return actWin; }

};
#undef maxWin
#undef maxWinReserve
#endif
