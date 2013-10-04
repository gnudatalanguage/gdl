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

#include "gdlwinstream.hpp"
#include "initsysvar.hpp"

#ifdef HAVE_OLDPLPLOT
#define SETOPT SetOpt
#else
#define SETOPT setopt
#endif

const int maxWin=32;  

class DeviceWIN: public Graphics
{
  std::vector<GDLGStream*>	winList;
  std::vector<long>			oList;
  long oIx;
  int  actWin;

  void SetActWin( int wIx)
  {
	  // update !D
	  if( wIx >= 0 && wIx < winList.size()) 
	  {
		  // window size and pos
		  PLFLT xp; PLFLT yp; 
		  PLINT xleng; PLINT yleng;
		  PLINT xoff; PLINT yoff;
		  winList[ wIx]->gpage( xp, yp, xleng, yleng, xoff, yoff);
		  (*static_cast<DLongGDL*>( dStruct->GetTag( xSTag)))[0] = xleng;
		  (*static_cast<DLongGDL*>( dStruct->GetTag( ySTag)))[0] = yleng;
		  (*static_cast<DLongGDL*>( dStruct->GetTag( xVSTag)))[0] = xleng;
		  (*static_cast<DLongGDL*>( dStruct->GetTag( yVSTag)))[0] = yleng;
	  }	

	  // window number
	  (*static_cast<DLongGDL*>( dStruct->GetTag( wTag)))[0] = wIx;

	  actWin = wIx;
  }

  // process user deleted windows
  // should be done in a thread
  void ProcessDeleted()
  {
	  int wLSize = winList.size();

	  //     bool redo;
	  //     do { // it seems that the event queue is only searched a few events deep
	  //       redo = false;
	  for( int i=0; i<wLSize; i++)
	  {
		  if( winList[ i] != NULL && !winList[ i]->Valid()) 
		  {
			  delete winList[ i];
			  winList[ i] = NULL;
			  oList[ i] = 0;
			  // 	    redo = true;
		  }
		  //     } while( redo);
	  }

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
  }

public:
  DeviceWIN(): Graphics(), oIx( 1), actWin( -1)
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
    dStruct->InitTag("X_CH_SIZE",  DLongGDL( 9)); 
    dStruct->InitTag("Y_CH_SIZE",  DLongGDL( 12)); 
    dStruct->InitTag("X_PX_CM",    DFloatGDL( 40.0)); 
    dStruct->InitTag("Y_PX_CM",    DFloatGDL( 40.0)); 
    dStruct->InitTag("N_COLORS",   DLongGDL( 256)); 
    dStruct->InitTag("TABLE_SIZE", DLongGDL( 0)); 
    dStruct->InitTag("FILL_DIST",  DLongGDL( 0)); 
    dStruct->InitTag("WINDOW",     DLongGDL( -1)); 
    dStruct->InitTag("UNIT",       DLongGDL( 0)); 
    dStruct->InitTag("FLAGS",      DLongGDL( 328124)); 
    dStruct->InitTag("ORIGIN",     origin); 
    dStruct->InitTag("ZOOM",       zoom); 

    winList.resize( maxWin);
    for( int i=0; i < maxWin; i++) winList[ i] = NULL;
    oList.resize( maxWin);
    for( int i=0; i < maxWin; i++) oList[ i] = 0;

    //GDLGStream::SetErrorHandlers();
  }
  
  ~DeviceWIN()
  {
    std::vector<GDLGStream*>::iterator i;
    for(i = winList.begin(); i != winList.end(); ++i) 
      { delete *i; *i = NULL;}
  }

  void EventHandler()
  {
    int wLSize = winList.size();
    for( int i=0; i<wLSize; i++)
      if( winList[ i] != NULL)
	winList[ i]->EventHandler();

    ProcessDeleted();
  }

  bool WDelete( int wIx)
  {
    ProcessDeleted();

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

  bool WOpen( int wIx, const std::string& title, 
	      int xSize, int ySize, int xPos, int yPos)
  {
    ProcessDeleted();

    int wLSize = winList.size();
    if( wIx >= wLSize || wIx < 0)
      return false;

    if( winList[ wIx] != NULL)
      delete winList[ wIx];

    DLongGDL* pMulti = SysVar::GetPMulti();
    DLong nx = (*pMulti)[ 1];
    DLong ny = (*pMulti)[ 2];

    if( nx <= 0) nx = 1;
    if( ny <= 0) ny = 1;

    winList[ wIx] = new GDLWINStream( nx, ny);
    oList[ wIx]   = oIx++;

    // set initial window size
    PLFLT xp; PLFLT yp; 
    PLINT xleng; PLINT yleng;
    PLINT xoff; PLINT yoff;
    winList[ wIx]->gpage( xp, yp, xleng, yleng, xoff, yoff);

    xleng = xSize;
    yleng = ySize;
    xoff  = xPos;
    yoff  = yPos;

    winList[ wIx]->spage( xp, yp, xleng, yleng, xoff, yoff);

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
    winList[ wIx]->SETOPT( "plwindow", buf);

    // we want color (and the driver options need to be overwritten)
    // winList[ wIx]->SETOPT( "drvopt","color=1");

    // set color map
    PLINT r[ctSize], g[ctSize], b[ctSize];
    actCT.Get( r, g, b);
    //    winList[ wIx]->scmap0( r, g, b, ctSize); 
    winList[ wIx]->scmap1( r, g, b, ctSize); 

    winList[ wIx]->Init();
    // need to be called initially. permit to fix things
    winList[ wIx]->ssub(1,1);
    winList[ wIx]->adv(0);
    // load font
    winList[ wIx]->font( 1);
    winList[ wIx]->vpor(0,1,0,1);
    winList[ wIx]->wind(0,1,0,1);
//    winList[ wIx]->DefaultCharSize();
    //in case these are not initalized, here is a good place to do it.
    if (winList[ wIx]->updatePageInfo()==true)
    {
        winList[ wIx]->GetPlplotDefaultCharSize(); //initializes everything in fact..

    }
    // sets actWin and updates !D
    SetActWin( wIx);

    return true; //winList[ wIx]->Valid(); // Valid() need to called once
  }

  bool WSet( int wIx)
  {
    ProcessDeleted();

    int wLSize = winList.size();
    if( wIx >= wLSize || wIx < 0 || winList[ wIx] == NULL)
      return false;

    SetActWin( wIx);
    return true;
  }

  int WAdd()
  {
    ProcessDeleted();

    int wLSize = winList.size();
    for( int i=maxWin; i<wLSize; i++)
      if( winList[ i] == NULL) return i;

    // plplot allows only 101 windows
    if( wLSize == 101) return -1;

    winList.push_back( NULL);
    oList.push_back( 0);
    return wLSize;
  }

  // should check for valid streams
  GDLGStream* GetStream( bool open=true)
  {
    ProcessDeleted();
    if( actWin == -1)
      {
	if( !open) return NULL;

	DString title = "GDL 0";
        DLong xSize, ySize;
        //DefaultXYSize(&xSize, &ySize);
		xSize = 640; ySize = 480;
	bool success = WOpen( 0, title, xSize, ySize, 0, 0);
	if( !success)
	  return NULL;
	if( actWin == -1)
	  {
	    std::cerr << "Internal error: plstream not set." << std::endl;
	    exit( EXIT_FAILURE);
	  }
      }
    return winList[ actWin];
  }

  int MaxWin() { ProcessDeleted(); return maxWin;}
  int ActWin() { ProcessDeleted(); return actWin;}
};

#endif
