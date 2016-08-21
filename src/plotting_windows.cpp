/***************************************************************************
                       plotting.cpp  -  GDL routines for plotting
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002-2011 by Marc Schellens et al.
    email                : m_schellens@users.sf.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "includefirst.hpp"
#include "plotting.hpp"


namespace lib {

  using namespace std;

  void window( EnvT* e)
  {
    GraphicsDevice* actDevice = GraphicsDevice::GetDevice();
    int maxWin = actDevice->MaxWin();
    if( maxWin == 0)
      e->Throw( "Routine is not defined for current graphics device.");

    SizeT nParam=e->NParam();

    DLong wIx = 0;
    static int FREEIx = e->KeywordIx("FREE");
    static int TITLEIx = e->KeywordIx("TITLE");
    static int XPOSIx = e->KeywordIx("XPOS");
    static int YPOSIx = e->KeywordIx("YPOS");
    static int XSIZEIx = e->KeywordIx("XSIZE");
    static int YSIZEIx = e->KeywordIx("YSIZE");
    static int RETAINIx = e->KeywordIx("RETAIN");
    static int PIXMAPIx = e->KeywordIx("PIXMAP");
    if( e->KeywordSet(FREEIx))
      {
	wIx = actDevice->WAddFree();
	if( wIx == -1)
	  e->Throw( "No more window handles left.");
      }
    else
      {
	if( nParam == 1)
	  {
	    e->AssureLongScalarPar( 0, wIx);
	    if( wIx < 0 || wIx >= actDevice->MaxNonFreeWin()) //to comply with IDL
	      e->Throw( "Window number "+i2s(wIx)+
			" out of range or no more windows.");
	  }
      }

    DString title;
    if( e->KeywordPresent(TITLEIx))
      {
	e->AssureStringScalarKWIfPresent( TITLEIx, title);
      }
    else
      {
	title = "GDL "+i2s( wIx);
      }

    DLong xPos=-1, yPos=-1; //NOTE: xPos=-1 and yPos=-1 are when XPOS and YPOS options were not used!
    e->AssureLongScalarKWIfPresent( XPOSIx, xPos);
    e->AssureLongScalarKWIfPresent( YPOSIx, yPos);

    DLong xSize, ySize;

    actDevice->DefaultXYSize(&xSize, &ySize);

    e->AssureLongScalarKWIfPresent( XSIZEIx, xSize);
    e->AssureLongScalarKWIfPresent( YSIZEIx, ySize);

    int debug=0;
    if (debug) {
      cout << "input values :"<< endl;
      cout << "xPos/yPos   :"<<  xPos << " " << yPos << endl;
      cout << "xSize/ySize :"<<  xSize << " " << ySize << endl;      
    }
    
    if (xSize < 0) xSize=10000;
    if (ySize < 0) ySize=10000;


//NOTE: xPos=-1 and yPos=-1 are when XPOS and YPOS options were not used!   

// AC 2014/02/26: this seems to be not need as long as xSize/ySize > 0
// which is done above

//    if( xSize <= 0 || ySize <= 0 || xPos < -1 || yPos < -1)
//      e->Throw(  "Unable to create window "
//		 "(BadValue (integer parameter out of range for operation)).");
    
    DLong retainType = 0; 
    if( e->KeywordPresent( RETAINIx))
    {
      e->AssureLongScalarKWIfPresent( RETAINIx, retainType);
    }
    bool success = actDevice->SetBackingStore(retainType);
    bool hide=e->KeywordSet( PIXMAPIx);
    success = actDevice->WOpen( wIx, title, xSize, ySize, xPos, yPos, hide);
    if( !success)
      e->Throw(  "Unable to create window.");
    if ( e->KeywordSet( PIXMAPIx))
    {
      success = actDevice->Hide();
    }
    else success = actDevice->UnsetFocus();
    actDevice->GetStream()->DefaultBackground();
    actDevice->GetStream()->Clear();

 }

  void wset( EnvT* e)
  {
    GraphicsDevice* actDevice = GraphicsDevice::GetDevice();
    int maxWin = actDevice->MaxWin();
    if( maxWin == 0)
      e->Throw( "Routine is not defined for current graphics device.");

    SizeT nParam=e->NParam();
    DLong wIx = 0;
    if( nParam != 0)
      {
	e->AssureLongScalarPar( 0, wIx);
      }
    if( wIx == -1) wIx = actDevice->ActWin();
    if( wIx == -1) 
      e->Throw( "Window is closed and unavailable.");

    if( wIx == 0)
      {
	if( actDevice->ActWin() == -1)
	  {
            DLong xSize, ySize;
            actDevice->DefaultXYSize(&xSize, &ySize);
	    bool success = actDevice->WOpen( 0, "GDL 0", xSize, ySize, -1, -1, false);
	    if( !success)
	      e->Throw( "Unable to create window.");
//        success = actDevice->UnsetFocus();  // following a deviceXXX->WOpen
        //FIXME: ADD support for RETAIN (BackingSTORE))
        actDevice->GetStream()->DefaultBackground();
        actDevice->GetStream()->Clear();
        return;
	  }
      }

    bool success = actDevice->WSet( wIx);
    if( !success)
      e->Throw( "Window is closed and unavailable.");
  }

  void wshow( EnvT* e)
  {
    GraphicsDevice* actDevice = GraphicsDevice::GetDevice();
    int maxWin = actDevice->MaxWin();
    if( maxWin == 0)
      e->Throw( "Routine is not defined for current graphics device.");

    SizeT nParam=e->NParam();
    DLong wIx = 0;
    if (nParam != 0) e->AssureLongScalarPar( 0, wIx);
    else wIx = actDevice->ActWin();

    // note by AC on 2012-Aug-16
    // On the system I tested (Ubuntu 10.4), I was not able to have
    // the expected SHOW behavior, with IDL 7.0 and GDL :(
    // Help/suggestions welcome
    // works for me (GD) on mandriva 2010.2
    bool show = true;
    if (nParam == 2) { 
      DIntGDL *showval = e->GetParAs<DIntGDL>(1);
      show = (*showval)[0] != 0;
    }

    // note by AC on 2012-Aug-16
    // I don't know how to find the sub-window number (third parametre
    // in call XIconifyWindow())
    // Help/suggestions welcome
    //GD: it is not a sub-window, but a screen number: xwd->screen, but that does not make window iconic any better!

    bool iconic = false;
    static int ICONICIx = e->KeywordIx("ICONIC");
    if( e->KeywordSet(ICONICIx)) iconic=true;

    if (!actDevice->WShow( wIx, show, iconic)) 
      e->Throw( "Window number "+i2s(wIx)+" out of range or no more windows.");
  }

  void wdelete( EnvT* e)
  {
    GraphicsDevice* actDevice = GraphicsDevice::GetDevice();
    int maxWin = actDevice->MaxWin();
    if( maxWin == 0)
      e->Throw( "Routine is not defined for current graphics device.");

    SizeT nParam=e->NParam();
    if( nParam == 0)
      {
	DLong wIx = actDevice->ActWin();
	bool success = actDevice->WDelete( wIx);
	if( !success)
	  e->Throw( "Window number "+i2s(wIx)+
			      " invalid or no more windows.");
	return;
      }

    for( SizeT i=0; i<nParam; i++)
      {
	DLong wIx;
	e->AssureLongScalarPar( i, wIx);
	bool success = actDevice->WDelete( wIx);
	if( !success)
	  e->Throw( "Window number "+i2s(wIx)+
		    " invalid or no more windows.");
      }
  }

    

} // namespace
