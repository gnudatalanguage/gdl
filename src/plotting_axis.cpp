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

  class axis_call : public plotting_routine_call 
  {

    private: bool handle_args(EnvT* e) // {{{
    {
      return true;
    } // }}}

  private: void old_body( EnvT* e, GDLGStream* actStream) // {{{
  { 
    bool valid=true;
    // !X, !Y (also used below)

    DLong xStyle=0, yStyle=0; 
    DString xTitle, yTitle; 
    DFloat x_CharSize, y_CharSize; 
    DFloat xMarginL, xMarginR, yMarginB, yMarginT; 
    DFloat xTicklen, yTicklen;

    bool xAxis=false, yAxis=false; 
    static int xaxisIx = e->KeywordIx( "XAXIS");
    static int yaxisIx = e->KeywordIx( "YAXIS");
    
    PLINT xaxis_value, yaxis_value; 
    bool standardNumPos;
    //IDL behaviour for XAXIS and YAXIS options
    if( (e->GetKW( xaxisIx) != NULL) ) {
      xAxis = true;
      e->AssureLongScalarKWIfPresent( "XAXIS", xaxis_value);
      if (xaxis_value == 0) {standardNumPos = true;} else {standardNumPos = false;}
    }      
    if( e->GetKW( yaxisIx) != NULL) {
      yAxis = true; xAxis = false; // like in IDL, yaxis overrides xaxis
      e->AssureLongScalarKWIfPresent( "YAXIS", yaxis_value);
      if (yaxis_value == 0) {standardNumPos = true;} else {standardNumPos = false;}
    }
    if( (e->GetKW( xaxisIx) == NULL) & (e->GetKW( yaxisIx) == NULL ) )  {
      xAxis = true; standardNumPos = true; 
    }
    
    // [XY]STYLE
    gkw_axis_style(e, "X", xStyle);
    gkw_axis_style(e, "Y", yStyle);

    e->AssureLongScalarKWIfPresent( "XSTYLE", xStyle);
    e->AssureLongScalarKWIfPresent( "YSTYLE", yStyle);

    // AXIS TITLE
    gkw_axis_title(e, "X", xTitle);
    gkw_axis_title(e, "Y", yTitle);
    // MARGIN
    gkw_axis_margin(e, "X",xMarginL, xMarginR);
    gkw_axis_margin(e, "Y",yMarginB, yMarginT);

    // x and y range
    DDouble xStart, xEnd;
    DDouble yStart, yEnd;

    get_axis_crange("X", xStart, xEnd);
    get_axis_crange("Y", yStart, yEnd);
    
    DLong xnozero=1, ynozero=0;
	gkw_axis_range( e, "X", xStart, xEnd, xnozero);
	gkw_axis_range( e, "Y", yStart, yEnd, ynozero);

    if (xStart == xEnd && yStart == yEnd) {
      e->Throw("Invalid plotting ranges.  Set up a plot window first.");
    }

    /*
    if ((xStyle & 1) != 1 && xAxis) {
      PLFLT intv;
      intv = AutoIntvAC(xStart, xEnd, false );
      // xEnd = ceil(xEnd/intv) * intv;
      // xStart = floor(xStart/intv) * intv;
    }

    if ((yStyle & 1) != 1 && yAxis) {
      PLFLT intv;
      intv = AutoIntvAC(yStart, yEnd, false );
      //yEnd = ceil(yEnd/intv) * intv;
      //yStart = floor(yStart/intv) * intv;
    }
    */

    DDouble zVal, yVal, xVal;
    //read arguments 
    if (nParam() == 1) {
      e->AssureDoubleScalarPar( 0, xVal);
      yVal=0.; //IDL behaviour
    }
    if (nParam() == 2) {
      e->AssureDoubleScalarPar( 0, xVal);
      e->AssureDoubleScalarPar( 1, yVal);
    }
    if (nParam() == 3) {
      e->Throw( "Sorry, we do not yet support the 3D case");
    }
    if (nParam() == 0 && standardNumPos) { xVal = xStart; yVal = yStart; }
    if (nParam() == 0 && !standardNumPos) { xVal = xEnd; yVal = yEnd; }

    /*
    DLong ynozero, xnozero;
    //[x|y]range keyword
    gkw_axis_range(e, "X", xStart, xEnd, ynozero);
    gkw_axis_range(e, "Y", yStart, yEnd, xnozero);
    */

    // AC nomore useful
    // if(xEnd == xStart) xEnd=xStart+1;

    DDouble minVal = yStart;
    DDouble maxVal = yEnd;

    DLong xTicks=0, yTicks=0; 
    e->AssureLongScalarKWIfPresent( "XTICKS", xTicks);
    e->AssureLongScalarKWIfPresent( "YTICKS", yTicks);

    DLong xMinor=0, yMinor=0; 
    e->AssureLongScalarKWIfPresent( "XMINOR", xMinor);
    e->AssureLongScalarKWIfPresent( "YMINOR", yMinor);

    DString xTickformat, yTickformat;
    e->AssureStringScalarKWIfPresent( "XTICKFORMAT", xTickformat);
    e->AssureStringScalarKWIfPresent( "YTICKFORMAT", yTickformat);

    bool xLog, yLog;
    //get_axis_type("X", xLog);
    //get_axis_type("Y", yLog);

    // keyword overrides
    static int xLogIx = e->KeywordIx( "XLOG");
    static int yLogIx = e->KeywordIx( "YLOG");
    xLog = e->KeywordSet( xLogIx);
    yLog = e->KeywordSet( yLogIx);

    // test for x/yVal
    if (xLog) { if (xVal <= 0.) xVal=xStart; else xVal=log10(xVal);}
    if (yLog) { if (yVal <= 0.) yVal=yStart; else yVal=log10(yVal);}

    DDouble ticklen = 0.02;
    e->AssureDoubleScalarKWIfPresent( "TICKLEN", ticklen);
						 
    DFloat charsize, xCharSize, yCharSize;
    // *** start drawing
    gkw_color(e, actStream);       //COLOR
    gkw_noerase(e, actStream, true);     //NOERASE
    gkw_charsize(e, actStream, charsize);    //CHARSIZE
    gkw_axis_charsize(e, "X",xCharSize);//XCHARSIZE
    gkw_axis_charsize(e, "Y",yCharSize);//YCHARSIZE

    // plplot stuff
    // set the charsize (scale factor)
    DDouble charScale = 1.0;
    DLongGDL* pMulti = SysVar::GetPMulti();
    if( (*pMulti)[1] > 2 || (*pMulti)[2] > 2) charScale = 0.5;
    actStream->schr( 0.0, charsize * charScale);

    // get subpage in mm
    PLFLT scrXL, scrXR, scrYB, scrYT;
    actStream->gspa( scrXL, scrXR, scrYB, scrYT); 
    PLFLT scrX = scrXR-scrXL;
    PLFLT scrY = scrYT-scrYB;

    // get char size in mm (default, actual)
    PLFLT defH, actH;
    actStream->gchr( defH, actH);

    // get viewport coordinates in normalised units
    PLFLT vpXL, vpXR, vpYB, vpYT;
    actStream->gvpd(vpXL, vpXR, vpYB, vpYT);
    PLFLT vpX = vpXR-vpXL;
    PLFLT vpY = vpYT-vpYB;

    // create new viewport and draw only the relevant side (viewport
    // will be reset to its original values later)
    PLFLT svpXL, svpXR, svpYB, svpYT; //new viewport coordinates
    if (xAxis) {
      //keep the X values the same
      svpXL=vpXL; svpXR=vpXR;
      if (standardNumPos) { //our axis is the bottom of viewport
	svpYB=vpY*(yVal-yStart)/(yEnd-yStart)+vpYB;
	svpYT=svpYB+0.2; //value doesn't matter, as long as svpYT>svpYB
      } else { //our axis is the top of viewport
	svpYT=vpYT-vpY*(yEnd-yVal)/(yEnd-yStart);
	svpYB=svpYT-0.2; //value doesn't matter, as long as svpYT>svpYB
      }
    }
    if (yAxis) {
      //keep the top and bottom the same
      svpYT=vpYT; svpYB=vpYB;
      if (standardNumPos) { //our axis is the left of the viewport
	svpXL=vpX*(xVal-xStart)/(xEnd-xStart)+vpXL;
	svpXR=svpXL+0.2; //value doesn't matter, as long as svpXR>svpXL
      } else { //our axis is the right of the viewport
	svpXR=vpXR-vpX*(xEnd-xVal)/(xEnd-xStart);
	svpXL=svpXR-0.2; //value doesn't matter, as long as svpXR>svpXL
      }
    }
    actStream->vpor(svpXL, svpXR, svpYB, svpYT);
    //The world coordinates for the relevant axis should be same as
    //the originals, while the other axis doesn't matter.  
    actStream->wind(xStart, xEnd, yStart,yEnd);

    // POSITION
    //DFloatGDL* pos = (DFloatGDL*) 0xF;

    /*
    // viewport and world coordinates
    bool okVPWC = SetVP_WC( e, actStream, pos, NULL,
			    xLog, yLog,
			    xMarginL, xMarginR, yMarginB, yMarginT,
			    xStart, xEnd, minVal, maxVal);
    if( !okVPWC) return;
    */

    // pen thickness for axis
    actStream->wid( 0);

    // axis
    string xOpt, yOpt;
    if (standardNumPos) { xOpt = "b"; yOpt = "b";} else { xOpt = "c"; yOpt = "c"; }

    if (xTicks == 1) xOpt += "t"; else xOpt += "st";
    if (yTicks == 1) yOpt += "tv"; else yOpt += "stv";

    if (xTickformat != "(A1)" && standardNumPos) xOpt += "nf";
    if (xTickformat != "(A1)" && !standardNumPos) xOpt += "mf";
    if (yTickformat != "(A1)" && standardNumPos) yOpt += "nf";
    if (yTickformat != "(A1)" && !standardNumPos) yOpt += "mf";

    if( xLog) xOpt += "l";
    if( yLog) yOpt += "l";

    if ((xStyle & 4) == 4) xOpt = "";
    if ((yStyle & 4) == 4) yOpt = "";

    string titleOpt;
    if (xAxis) {

      // axis titles
      actStream->schr( 0.0, actH/defH * xCharSize);
      if (standardNumPos) { titleOpt = "b"; } else { titleOpt = "t"; }
      actStream->mtex(titleOpt.c_str(),3.5,0.5,0.5,xTitle.c_str());

      // the axis (separate for x and y axis because of charsize)
      PLFLT xintv;
      if (xTicks == 0) {
	xintv = AutoTick(xEnd-xStart);
      } else {
	xintv = (xEnd - xStart) / xTicks;
      }
      actStream->box( xOpt.c_str(), xintv, xMinor, "", 0.0, 0);

      if (e->KeywordSet("SAVE"))
      {
        // X.CRANGE
        set_axis_crange("X", xStart, xEnd);

        // X.TYPE
        set_axis_type("X",xLog);

        // X.S ... TODO: set_axis_s()
        DStructGDL* Struct=NULL;
        Struct = SysVar::X();
        static unsigned sTag = Struct->Desc()->TagIndex( "S");
        PLFLT p_xmin, p_xmax, p_ymin, p_ymax;
        actStream->gvpd (p_xmin, p_xmax, p_ymin, p_ymax);
        if(Struct != NULL)
        {
          (*static_cast<DDoubleGDL*>( Struct->GetTag( sTag, 0)))[0] =
            (p_xmin*xEnd - p_xmax*xStart) / (xEnd - xStart);
          (*static_cast<DDoubleGDL*>( Struct->GetTag( sTag, 0)))[1] =
            (p_xmax - p_xmin) / (xEnd - xStart);
        }
      }
    }

    if (yAxis) {

      // axis titles
      actStream->schr( 0.0, actH/defH * yCharSize);
      if (standardNumPos) { titleOpt = "l"; } else { titleOpt = "r"; }
      actStream->mtex(titleOpt.c_str(),5.0,0.5,0.5,yTitle.c_str());

      // the axis (separate for x and y axis because of charsize)
      PLFLT yintv;
      if (yTicks == 0) {
	yintv = AutoTick(yEnd-yStart);
      } else {
	yintv = (yEnd - yStart) / yTicks;
      }
      actStream->box( "", 0.0, 0, yOpt.c_str(), yintv, yMinor);

      if (e->KeywordSet("SAVE"))
      {
        // Y.CRANGE
        set_axis_crange("Y", yStart, yEnd);

        // Y.TYPE
        set_axis_type("Y",yLog);

        // Y.S ... TODO: set_axis_s()
        DStructGDL* Struct=NULL;
        Struct = SysVar::Y();
        static unsigned sTag = Struct->Desc()->TagIndex( "S");
        PLFLT p_xmin, p_xmax, p_ymin, p_ymax;
        actStream->gvpd (p_xmin, p_xmax, p_ymin, p_ymax);
        if(Struct != NULL)
        {
          (*static_cast<DDoubleGDL*>( Struct->GetTag( sTag, 0)))[0] =
            (p_ymin*yEnd - p_ymax*yStart) / (yEnd - yStart);
          (*static_cast<DDoubleGDL*>( Struct->GetTag( sTag, 0)))[1] =
            (p_ymax - p_ymin) / (yEnd - yStart);
        }
      }
    }

    // reset the viewport and world coordinates to the original values
    actStream->vpor(vpXL, vpXR, vpYB, vpYT);
    actStream->wind(xStart, xEnd, yStart,yEnd);

    // title and sub title
    // axis has subtitle but no title, gkw_title requires both
    //    gkw_title(e, actStream, actH/defH);

  } // }}}

    private: void call_plplot(EnvT* e, GDLGStream* actStream) // {{{
    {
    } // }}}
 
    private: virtual void post_call(EnvT*, GDLGStream*) // {{{
    {
    } // }}}

  }; // axis_call

  void axis(EnvT* e)
  {
    axis_call axis;
    axis.call(e, 0);
  }

} // namespace
