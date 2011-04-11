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
#include "math_utl.hpp"
#define PLK_Escape            0x1B

namespace lib {

  using namespace std;

  // get cursor from plPlot     AC February 2008
  // known limitations : WAIT parameter and similar keywords not fully managed (wait, nowait ...)

  void cursor( EnvT* e)
  {
    Graphics* actDevice = Graphics::GetDevice();
    //cout << actDevice->Name() << endl;
    if (actDevice->Name() != "X") {
      e->Throw( "Routine is not defined for current graphics device.");
    }

    int debug=0;
    SizeT nParam=e->NParam( 1); 

    if ( nParam < 2 || nParam > 3) 
      {
	e->Throw( "Incorrect number of arguments.");
      }

    e->AssureGlobalPar( 0);
    e->AssureGlobalPar( 1);
    
    GDLGStream *plg  = GetPlotStream( e);

    static PLGraphicsIn gin;

    // content of : plGetCursor();
    // [retval, state, keysym, button, string, pX, pY, dX, dY, wX, wY, subwin]
    
    PLINT plplot_level;
    plg->glevel (plplot_level);   
    if (debug) cout << "Plplot_level : " << plplot_level<< endl;
    // when level < 2, we have to read if ![x|y].crange exist
    // if not, we have to build a [0,1]/[0,1] window
    if (plplot_level < 2) {
      plg->NextPlot();

      plg->vpor(0,1,0,1);
      plg->wind(0,1,0,1);

      /* we are not ready for the correct way (rebuilding a window following
	 stored info)

      // we have to read them back from !x.crange and !y.crange
      PLFLT xStart, xEnd, yStart, yEnd;
      get_axis_crange("X", xStart, xEnd);
      get_axis_crange("Y", yStart, yEnd);
      if ((xStart == xEnd ) || ( xEnd ==0.0)) {
	xStart=0.0;
	xEnd=1.0;
      }
      if ((yStart == yEnd ) || ( yEnd ==0.0)) {
	yStart=0.0;
	yEnd=1.0;
      }
      AC_debug("crange", xStart, xEnd, yStart, yEnd);

      bool okVPWC = SetVP_WC( e, plg, NULL, NULL, 0, 0,
			      xStart, xEnd, yStart, yEnd, false, false);
      if( !okVPWC) return;
      */
    }

    DLong wait=1;

    if (nParam == 3) {
      e->AssureLongScalarPar( 2, wait);
    }

    if ((wait == 1) || (wait == 3) || (wait == 4) ||
	e->KeywordSet("WAIT") ||
	e->KeywordSet("DOWN") ||
	e->KeywordSet("UP") ) {
      //cout << "Sorry, this option is currently not *really* managed. Help welcome" << endl;
      // we toggle to "wait == 1" (the mouse can move but we return if mouse is pressed)
      wait=1;
    }

    int mode=0; // just a flag to manage the general case (cursor,x,y)
    
    if ((wait == 0) || e->KeywordSet("NOWAIT")) {
      gin.button=1;
      plg->GetCursor(&gin);
      gin.button=0;
      mode=1;
      wait=0;
    }
    if (wait == 1) {
      while (1) {
	plg->GetCursor(&gin);
	//	cout << gin.button << endl;
	if (gin.button > 0) break;
 	if( sigControlC)
 		return;
      }
      mode=1;
    }    
    if ((wait == 2) || e->KeywordSet("CHANGE")) {
      plg->GetCursor(&gin);
      long RefX, RefY;
      RefX=gin.pX;
      RefY=gin.pY;
      if (gin.button == 0) {
	while (1) {
	  plg->GetCursor(&gin);
	  if (abs(RefX-gin.dX) >0 || abs(RefY-gin.dY) >0) break;
	  if (gin.button > 0) break;
 	if( sigControlC)
 		return;
	}
      }
      mode=1;
    }

    if (mode == 0) {
      while (1) {
	plg->GetCursor(&gin);
	// TODO: When no Mouse, should be extended later to any key of the keyboard 
	if (gin.keysym == PLK_Escape) break;
	if (gin.button > 0) break;
 	if( sigControlC)
 		return;
      }
    }
    
    if (debug) {
      // plg->text();
      cout << "mouse button : " << gin.button << endl;
      cout << "keysym : " << gin.keysym << endl;
      //plg->gra();
      /* if (gin.keysym < 0xFF && isprint(gin.keysym))
	 cout << "wx = " << gin.wX << ", wy = " << gin.wY <<
	 ", dx = " << gin.dX << ",  dy = " << gin.dY <<
	 ",  c = '" << gin.keysym << "'" << endl;
	 plg->gra(); */      
    }

    if (e->KeywordSet("DEVICE")) {
      PLFLT xp, yp;
      PLINT xleng, yleng, xoff, yoff;
      plg->gpage (xp, yp, xleng, yleng, xoff, yoff);

      DLongGDL* xLong;
      DLongGDL* yLong;
      xLong=new DLongGDL(gin.pX);
      yLong=new DLongGDL(yleng-gin.pY);
      
      e->SetPar(0, xLong);
      e->SetPar(1, yLong);
    } else {
      DDoubleGDL* x;
      DDoubleGDL* y;
      if (e->KeywordSet("NORMAL")) {
	x=new DDoubleGDL(gin.dX );
	y=new DDoubleGDL(gin.dY );
      } else { // default (/data)
	// bad info outside the window (following PLPlot meaning)
	// TODO : we can compute that using !x.s and !y.s
#ifdef USE_LIBPROJ4
        bool mapSet = false;
        get_mapset(mapSet);
        if (!mapSet) 
        {
#endif
 	  x = new DDoubleGDL(gin.wX );
	  y = new DDoubleGDL(gin.wY );
#ifdef USE_LIBPROJ4
        } 
        else 
        {
          PROJTYPE* ref = map_init();
          if (ref == NULL) e->Throw("Projection initialization failed.");
          XYTYPE idata;
          idata.x = gin.wX;
          idata.y = gin.wY;
          LPTYPE odata = PJ_INV(idata, ref);
          x = new DDoubleGDL(odata.lam * RAD_TO_DEG);
          y = new DDoubleGDL(odata.phi * RAD_TO_DEG);
        }
#endif
      }
      e->SetPar(0, x);
      e->SetPar(1, y);
    }
    
    // we update the !Mouse structure (4 fields, only 3 managed up to now)
    DStructGDL* Struct= SysVar::Mouse();
    if (Struct!=NULL) {
      static unsigned xMouseTag = Struct->Desc()->TagIndex("X");
      (*static_cast<DLongGDL*>(Struct->GetTag(xMouseTag)))[0] = gin.pX;
      static unsigned yMouseTag = Struct->Desc()->TagIndex("Y");
      (*static_cast<DLongGDL*>(Struct->GetTag(yMouseTag)))[0] = gin.pY;
      static unsigned ButtonMouseTag = Struct->Desc()->TagIndex("BUTTON");
      if (gin.button == 3) gin.button=4; // 4 values only (0,1,2,4)
      (*static_cast<DLongGDL*>(Struct->GetTag(ButtonMouseTag)))[0]=gin.button;
    }
  }
 
} // namespace
