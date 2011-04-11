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

namespace lib {

  using namespace std;

  void plots( EnvT* e)
  {
    SizeT nParam=e->NParam( 1); 
    bool valid, line;
    valid=true;
    DLong psym;
    DDoubleGDL* xVal;
    DDoubleGDL* yVal;
    DDoubleGDL* zVal;
    SizeT xEl, yEl, zEl;
    auto_ptr<BaseGDL> xval_guard;
    auto_ptr<BaseGDL> yval_guard;

    if( nParam == 1)
      {
	BaseGDL* p0;
	p0 = e->GetParDefined( 0);  
	if (p0->Dim(0) != 2)
	  e->Throw( "When only 1 param, dims must be (2,n)");

	DDoubleGDL *val = e->GetParAs< DDoubleGDL>( 0);

	xEl = p0->N_Elements() / p0->Dim(0);
	xVal = new DDoubleGDL( dimension( xEl), BaseGDL::NOZERO);
	xval_guard.reset( xVal); // delete upon exit

	yEl = p0->N_Elements() / p0->Dim(0);
	yVal = new DDoubleGDL( dimension( yEl), BaseGDL::NOZERO);
	yval_guard.reset( yVal); // delete upon exit

	for( SizeT i = 0; i < xEl; i++) {
	  (*xVal)[i] = (*val)[2*i];
	  (*yVal)[i] = (*val)[2*i+1];
	}
      }
    else if(nParam == 2)
      {
	xVal = e->GetParAs< DDoubleGDL>( 0);
	xEl = xVal->N_Elements();

	yVal = e->GetParAs< DDoubleGDL>( 1);
	yEl = yVal->N_Elements();
      }
    else if(nParam == 3)
      {
	zVal = e->GetParAs< DDoubleGDL>( 2);
	zEl = zVal->N_Elements();

	if ((*zVal)[0] == 0 && zEl == 1) {
	  xVal = e->GetParAs< DDoubleGDL>( 0);
	  xEl = xVal->N_Elements();

	  yVal = e->GetParAs< DDoubleGDL>( 1);
	  yEl = yVal->N_Elements();
	} else {
	  e->Throw( "Three dimensional PLOTS not yet implemented.");
	}
      }

    // !X, !Y (also used below)
    DFloat xMarginL, xMarginR, yMarginB, yMarginT; 
    get_axis_margin("X",xMarginL, xMarginR);
    get_axis_margin("Y",yMarginB, yMarginT);
    // get ![XY].CRANGE
    DDouble xStart, xEnd, yStart, yEnd;
    bool xLog, yLog;
    get_axis_crange("X", xStart, xEnd);
    get_axis_crange("Y", yStart, yEnd);
    DDouble minVal, maxVal;
    get_axis_type("X", xLog);
    get_axis_type("Y", yLog);
    
//     if( xLog)
//       {
// 	xStart = pow(10.0,xStart);
// 	xEnd   = pow(10.0,xEnd);
//       }
//     if( yLog)
//       {
// 	yStart = pow(10.0,yStart);
// 	yEnd   = pow(10.0,yEnd);
//       }
    
    //    int just = (e->KeywordSet("ISOTROPIC"))? 1 : 0;
    /*    DLong background = p_background;
    static int cix=e->KeywordIx("COLOR");
    BaseGDL* color_arr=e->GetKW(cix);
    DLongGDL* l_color_arr;

    if(color_arr != NULL)
      {
	l_color_arr=static_cast<DLongGDL*>
	  (color_arr->Convert2(LONG, BaseGDL::COPY));
	if(color_arr->N_Elements() < minEl && color_arr->N_Elements() > 1)
	  e->Throw( "Array "+e->GetParString(cix)+
		    " does not have enough elements for COLOR keyword.");
      }
    DLong color = p_color;

    if(color_arr != NULL)  
      if(color_arr->N_Elements() >= 1) 
	  	color=(*l_color_arr)[0];
    */

    GDLGStream* actStream = GetPlotStream( e); 
    
    // start drawing
    gkw_background(e, actStream, false);
    gkw_color(e, actStream);

    gkw_psym(e, actStream, line, psym);
    gkw_linestyle(e, actStream);
    gkw_symsize(e, actStream);
    gkw_thick(e, actStream);
    DFloat charsize;
    gkw_charsize(e,actStream, charsize, false);

    // plplot stuff
    PLFLT scrXL, scrXR, scrYB, scrYT;
    actStream->gspa( scrXL, scrXR, scrYB, scrYT);
    PLFLT scrX = scrXR-scrXL;
    PLFLT scrY = scrYT-scrYB;

    PLFLT xMR, xML, yMB, yMT;

    CheckMargin( e, actStream,
		 xMarginL, 
		 xMarginR, 
		 yMarginB, 
		 yMarginT,
		 xMR,
		 xML,
		 yMB,
		 yMT);

    bool mapSet=false;
#ifdef USE_LIBPROJ4
    // Map Stuff (xtype = 3)
    LPTYPE idata;
    XYTYPE odata;

    get_mapset(mapSet);

    if ( mapSet) {
      ref = map_init();
      if ( ref == NULL) {
	e->Throw( "Projection initialization failed.");
      }
    }
#endif

    DDouble *sx, *sy;
    DFloat *wx, *wy;
    GetSFromPlotStructs(&sx, &sy);
    GetWFromPlotStructs(&wx, &wy);

    int toto=0;

    if(e->KeywordSet("DEVICE")) {
      PLFLT xpix, ypix;
      PLINT xleng, yleng, xoff, yoff;
      actStream->gpage(xpix, ypix,xleng, yleng, xoff, yoff);
      xStart=0; xEnd=xleng;
      yStart=0; yEnd=yleng;
      xLog = false; yLog = false;
      actStream->NoSub();
      actStream->vpor(0, 1, 0, 1);
    } else if(e->KeywordSet("NORMAL")) {
      xStart = 0;
      xEnd   = 1;
      yStart = 0;
      yEnd   = 1;
      actStream->NoSub();
      actStream->vpor(0, 1, 0, 1);
      xLog = false; yLog = false;
    } else {
      toto=1;
      actStream->NoSub();
      if (xLog || yLog) actStream->vpor(wx[0], wx[1], wy[0], wy[1]);
      else actStream->vpor(0, 1, 0, 1); // (to be merged with the condition on DataCoordLimits...)
    }

    // Determine data coordinate limits (if mapSet is true)
    // These are computed from window and scaling axis system
    // variables because map routines change these directly.
    
    if (e->KeywordSet("DATA") || (toto == 1)) {
      //    if (e->KeywordSet("NORMAL") || e->KeywordSet("DATA")) {
      DataCoordLimits(sx, sy, wx, wy, &xStart, &xEnd, &yStart, &yEnd, xLog || yLog);
    }

    minVal=yStart; maxVal=yEnd;

    //CLIPPING
    DLong noclip = 1;
    e->AssureLongScalarKWIfPresent( "NOCLIP", noclip);
    if( noclip == 0)
      {
	static int clippingix = e->KeywordIx( "CLIP"); 
	DDoubleGDL* clippingD = e->IfDefGetKWAs<DDoubleGDL>( clippingix);
	if( clippingD != NULL)
	    Clipping( clippingD, xStart, xEnd, minVal, maxVal);
      }

//    Comment out to fix bug [1560714] JMG 06/09/27
//    if( xLog)
//      {
//	if( xStart <= 0.0) xStart = 0.0; else xStart = log10( xStart);
//	if( xEnd   <= 0.0) return; else xEnd = log10( xEnd);
//      }

    if( yLog)
      {
	if( yStart <= 0.0) yStart = 0.0; else yStart = log10( yStart);
	if( yEnd   <= 0.0) return; else yEnd = log10( yEnd);
      }

    // SA: following a patch from Joanna (3029409) TODO: this is repeated in PLOTS POLYFILL and XYOUTS
    if ( xEnd - xStart == 0 || yEnd - yStart == 0 || isnan(xStart) || isnan(yStart) ) {
      actStream->wind( 0, 1, 0, 1 ); 
    } else {
      actStream->wind( xStart, xEnd, yStart, yEnd);
    } 

    // pen thickness for plot
    gkw_thick(e, actStream);
    gkw_symsize(e, actStream);
    gkw_linestyle(e, actStream);

    // plot the data
    if(valid)
      valid=draw_polyline(e, actStream, 
			  xVal, yVal, xLog, yLog, 
			  yStart, yEnd, psym);

    actStream->lsty(1);//reset linestyle
    actStream->flush();
  } // plots

} // namespace
