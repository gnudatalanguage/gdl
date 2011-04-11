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

  void oplot( EnvT* e)
  {
    SizeT nParam=e->NParam( 1); 
    bool valid, line;
    valid=true;
    DLong psym;
    DDoubleGDL* yVal;
    DDoubleGDL* xVal;
    auto_ptr<BaseGDL> xval_guard;

    if ( e->KeywordSet( "POLAR")) {
       e->Throw( "Soory, POLAR keyword not ready");
      }

    SizeT xEl;
    SizeT yEl;
    if( nParam == 1)
      {
	yVal = e->GetParAs< DDoubleGDL>( 0);
        if (yVal->Rank() == 0) 
          e->Throw("Expression must be an array in this context: "+e->GetParString(0));
	yEl = yVal->N_Elements();
	
	xVal = new DDoubleGDL( dimension( yEl), BaseGDL::INDGEN);
	xval_guard.reset( xVal); // delete upon exit
	xEl = yEl;
      }
    else
      {
	xVal = e->GetParAs< DDoubleGDL>( 0);
        if (xVal->Rank() == 0) 
          e->Throw("Expression must be an array in this context: "+e->GetParString(0));
	xEl = xVal->N_Elements();

	yVal = e->GetParAs< DDoubleGDL>( 1);
        if (yVal->Rank() == 0) 
          e->Throw("Expression must be an array in this context: "+e->GetParString(1));
	yEl = yVal->N_Elements();
      }
    DLong minEl = (xEl < yEl)? xEl : yEl;

    // !X, !Y (also used below)
    DString xTitle, yTitle; 
    DFloat xMarginL, xMarginR, yMarginB, yMarginT; 

    get_axis_margin("X",xMarginL, xMarginR);
    get_axis_margin("Y",yMarginB, yMarginT);
    // get ![XY].CRANGE
    DDouble xStart, xEnd, yStart, yEnd;
    get_axis_crange("X", xStart, xEnd);
    get_axis_crange("Y", yStart, yEnd);
    DDouble minVal;
    DDouble maxVal;

    bool xLog;
    bool yLog;
    get_axis_type("X", xLog);
    get_axis_type("Y", yLog);
    
    //    int just = (e->KeywordSet("ISOTROPIC"))? 1 : 0;

    GDLGStream* actStream = GetPlotStream( e); 
    
    // start drawing
    gkw_background(e, actStream,false);
    gkw_color(e, actStream);
    //    gkw_noerase(e, actStream, true);
    gkw_psym(e, actStream, line, psym);
    DFloat charsize;
    gkw_charsize(e,actStream, charsize, false);


    if( (yStart == yEnd) || (xStart == xEnd))
      {
	if( yStart != 0.0 && yStart == yEnd)
	  Message("OPLOT: !Y.CRANGE ERROR, setting to [0,1]");
	yStart = 0; //yVal->min();
	yEnd   = 1; //yVal->max();
	    
	if(xStart != 0.0 && xStart == xEnd)
	  Message("OPLOT: !X.CRANGE ERROR, resetting range to data");
	xStart = 0; //xVal->min();
	xEnd   = 1; //xVal->max();
	
	set_axis_crange("X", xStart, xEnd);
	set_axis_crange("Y", yStart, yEnd);
      }	    
    
    minVal = yStart;
    maxVal = yEnd;
    e->AssureDoubleScalarKWIfPresent( "MIN_VALUE", minVal);
    e->AssureDoubleScalarKWIfPresent( "MAX_VALUE", maxVal);

    // CLIPPING
    DDoubleGDL* clippingD=NULL;
    DLong noclip=0;
    e->AssureLongScalarKWIfPresent( "NOCLIP", noclip);
    if(noclip == 0)
      {
	static int clippingix = e->KeywordIx( "CLIP"); 
	clippingD = e->IfDefGetKWAs<DDoubleGDL>( clippingix);
      }
    

    // viewport and world coordinates
    bool okVPWC = SetVP_WC( e, actStream, NULL, clippingD, 
			    xLog, yLog,
			    xMarginL, xMarginR, yMarginB, yMarginT,
			    xStart, xEnd, minVal, maxVal);
    if( !okVPWC) return;
    // pen thickness for axis
    actStream->wid( 0);

    // axis
    //    string xOpt = "bcnst";
    //string yOpt = "bcnstv";

    //    if( xLog) xOpt += "l";
    //if( yLog) yOpt += "l";
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
  } // oplot

} // namespace

