/***************************************************************************
                       plotting_plot.cpp  -  GDL routines for plotting
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

  void plot( EnvT* e)
  {
    SizeT nParam=e->NParam( 1); 
    bool valid=true;
    DDoubleGDL *yVal, *xVal;
    SizeT xEl, yEl;
    auto_ptr<BaseGDL> xval_guard;

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

    if ( e->KeywordSet( "POLAR")) {
       e->Throw( "Sorry, POLAR keyword not ready");
      }

    DLong psym;
    bool line;

    // !X, !Y (also used below)

    DLong xStyle=0, yStyle=0; 
    DString xTitle, yTitle; 
    DFloat x_CharSize, y_CharSize; 
    DFloat xMarginL, xMarginR, yMarginB, yMarginT; 

    // [XY]STYLE
    gkw_axis_style(e, "X", xStyle);
    gkw_axis_style(e, "Y", yStyle);

    e->AssureLongScalarKWIfPresent( "XSTYLE", xStyle);
    e->AssureLongScalarKWIfPresent( "YSTYLE", yStyle);

    // AXIS TITLE
    gkw_axis_title(e, "X", xTitle);
    gkw_axis_title(e, "Y", yTitle);
    // MARGIN
    gkw_axis_margin(e, "X", xMarginL, xMarginR);
    gkw_axis_margin(e, "Y", yMarginB, yMarginT);

    // x and y range
    //    DDouble xStart = xVal->min(); 
    //DDouble xEnd   = xVal->max(); 
    //DDouble yStart = yVal->min(); 
    //DDouble yEnd   = yVal->max(); 
    //    cout << xStart << " " << xEnd << endl;
    
    bool debug=false;
    DLong MyXminEl;
    DLong MyXmaxEl;
    //   BaseGDL *x, *y;
    xVal->MinMax( &MyXminEl, &MyXmaxEl, NULL, NULL, true);
    DDouble xStart = (*xVal)[MyXminEl];
    DDouble xEnd = (*xVal)[MyXmaxEl];
    if (debug) { cout << "X indices (min,max) " << MyXminEl << " " << MyXmaxEl << endl;
    cout << "X values (min,max)  " << xStart << " " << xEnd << endl;}

    DLong MyYminEl;
    DLong MyYmaxEl;
    //   BaseGDL *x, *y;
    yVal->MinMax( &MyYminEl, &MyYmaxEl, NULL, NULL, true);
    DDouble yStart = (*yVal)[MyYminEl];
    DDouble yEnd = (*yVal)[MyYmaxEl];
    if (debug) {cout << "Y indices (min,max) " << MyYminEl << " " << MyYmaxEl << endl;
    cout << "Y values (min,max)  " << yStart << " " << yEnd << endl;}

    //    cout << yStart << " " << yEnd << endl;

    DDouble xStartRaw = xStart;
    DDouble yStartRaw = yStart;

//     bool x0Range = (xStart == xEnd);
//     bool y0Range = (yStart == yEnd);

    // PLOT defines the setting but does not read it
    //    get_axis_type("X", xLog);
    //    get_axis_type("Y", yLog);

    // keyword overrides
    bool xLog, yLog;
    static int xLogIx = e->KeywordIx( "XLOG");
    static int yLogIx = e->KeywordIx( "YLOG");
    xLog = e->KeywordSet( xLogIx);
    yLog = e->KeywordSet( yLogIx);

    DLong xnozero=1, ynozero=0;
    if ( e->KeywordSet( "YNOZERO")) ynozero = 1;

    if ((xStyle & 1) != 1 && xLog == false) {
      PLFLT intv;
      intv = AutoIntvAC(xStart, xEnd, xnozero);
    }
    if ((yStyle & 1) != 1 && yLog == false) {
      PLFLT intv;
      intv = AutoIntvAC(yStart, yEnd, ynozero);
    }

    // Please remember the {X|Y}range overwrite the data range
    //[x|y]range keyword
    gkw_axis_range(e, "X", xStart, xEnd, xnozero);
    gkw_axis_range(e, "Y", yStart, yEnd, ynozero);

    if ( e->KeywordSet( "YNOZERO")) ynozero = 1;

    // AC: should be now useless (done in AutoIntvAC)
    //if( xStart > 0 && xnozero == 0 && xLog == false) xStart = 0; 
    //if( yStart > 0 && ynozero == 0 && yLog == false) yStart = 0; 

    // no more useful
    //    if(xEnd == xStart) xEnd=xStart+1;
    // AC
    //if(yEnd == yStart) yEnd=yStart+1;
     if (debug) {cout << " Y range" << yStart << " e "<< yEnd << endl;}

    DDouble minVal = yStart;
    DDouble maxVal = yEnd;
    e->AssureDoubleScalarKWIfPresent( "MIN_VALUE", minVal);
    e->AssureDoubleScalarKWIfPresent( "MAX_VALUE", maxVal);

    DLong xTicks=0, yTicks=0; 
    e->AssureLongScalarKWIfPresent( "XTICKS", xTicks);
    e->AssureLongScalarKWIfPresent( "YTICKS", yTicks);

    DLong xMinor=0, yMinor=0; 
    e->AssureLongScalarKWIfPresent( "XMINOR", xMinor);
    e->AssureLongScalarKWIfPresent( "YMINOR", yMinor);

    DString xTickformat, yTickformat;
    e->AssureStringScalarKWIfPresent( "XTICKFORMAT", xTickformat);
    e->AssureStringScalarKWIfPresent( "YTICKFORMAT", yTickformat);

    if( xLog && xStartRaw <= 0.0)
      Warning( "PLOT: Infinite x plot range.");
    if( yLog && yStartRaw <= 0.0)
      Warning( "PLOT: Infinite y plot range.");

    //    int just = (e->KeywordSet("ISOTROPIC"))? 1 : 0;

    DDouble ticklen = 0.02;
    e->AssureDoubleScalarKWIfPresent( "TICKLEN", ticklen);

    DFloat xTicklen, yTicklen;
    e->AssureFloatScalarKWIfPresent( "XTICKLEN", xTicklen);
    e->AssureFloatScalarKWIfPresent( "YTICKLEN", yTicklen);
    // plsmin (def, scale);

						 
    // POSITION
    PLFLT xScale = 1.0;
    PLFLT yScale = 1.0;

    //    PLFLT scale = 1.0;
    static int positionIx = e->KeywordIx( "POSITION"); 
    DFloatGDL* pos = e->IfDefGetKWAs<DFloatGDL>( positionIx);
    if (pos == NULL) pos = (DFloatGDL*) 0xF;
    /*
    PLFLT position[ 4] = { 0.0, 0.0, 1.0, 1.0};
    if( pos != NULL)
      {
      for( SizeT i=0; i<4 && i<pos->N_Elements(); ++i)
	position[ i] = (*pos)[ i];

      xScale = position[2]-position[0];
      yScale = position[3]-position[1];
      //      scale = sqrt( pow( xScale,2) + pow( yScale,2));
      }
    */


    DFloat charsize, xCharSize, yCharSize;
    // *** start drawing
    GDLGStream* actStream = GetPlotStream( e); 
    gkw_background(e, actStream);  //BACKGROUND
    gkw_color(e, actStream);       //COLOR

    gkw_psym(e, actStream, line, psym);//PSYM
    gkw_charsize(e, actStream, charsize);    //CHARSIZE
    gkw_axis_charsize(e, "X",xCharSize);//XCHARSIZE
    gkw_axis_charsize(e, "Y",yCharSize);//YCHARSIZE


    // Turn off map projection processing
    set_mapset(0);

    gkw_noerase(e, actStream);     //NOERASE

    DLong noErase = 0;
    if( e->KeywordSet( "NOERASE")) noErase = 1;
    if( !noErase) actStream->Clear();

    // Get device name
    DStructGDL* dStruct = SysVar::D();
    static unsigned nameTag = dStruct->Desc()->TagIndex( "NAME");
    DString d_name = 
      (*static_cast<DStringGDL*>( dStruct->GetTag( nameTag, 0)))[0];
    // if PS and not noErase (ie, erase) then set !p.noerase=0    
    if ((d_name == "PS" || d_name == "SVG") && !noErase) {
      static DStructGDL* pStruct = SysVar::P();
      static unsigned noEraseTag = pStruct->Desc()->TagIndex( "NOERASE");
      (*static_cast<DLongGDL*>( pStruct->GetTag( noEraseTag, 0)))[0] = 0;
    }

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
    bool okVPWC = SetVP_WC( e, actStream, pos, clippingD, 
			    xLog, yLog,
			    xMarginL, xMarginR, yMarginB, yMarginT,
			    xStart, xEnd, minVal, maxVal);
    if( !okVPWC) return;

    // pen thickness for axis
    actStream->wid( 0);

    // axis
    string xOpt="bc", yOpt="bc";
    AdjustAxisOpts(xOpt, yOpt, xStyle, yStyle, xTicks, yTicks, xTickformat, yTickformat, xLog, yLog);

    // axis titles
    actStream->schr( 0.0, actH/defH * xCharSize);
    actStream->mtex("b",3.5,0.5,0.5,xTitle.c_str());

    // the axis (separate for x and y axis because of charsize)
    PLFLT xintv;
    if (xTicks == 0) {
      xintv = AutoTick(xEnd-xStart);
    } else {
      xintv = (xEnd - xStart) / xTicks;
    }
    actStream->box( xOpt.c_str(), xintv, xMinor, "", 0.0, 0);

    actStream->schr( 0.0, actH/defH * yCharSize);
    actStream->mtex("l",5.0,0.5,0.5,yTitle.c_str());
    // the axis (separate for x and y axis because of charsize)
    PLFLT yintv;
    if (yTicks == 0) {
      yintv = AutoTick(yEnd-yStart);
    } else {
      yintv = (yEnd - yStart) / yTicks;
    }
    actStream->box( "", 0.0, 0, yOpt.c_str(), yintv, yMinor);

    // title and sub title
    gkw_title(e, actStream, actH/defH);

    // pen thickness for plot
    gkw_thick(e, actStream);
    gkw_symsize(e, actStream);
    gkw_linestyle(e, actStream);

    // Get viewpoint parameters and store in WINDOW & S
    PLFLT p_xmin, p_xmax, p_ymin, p_ymax;
    actStream->gvpd (p_xmin, p_xmax, p_ymin, p_ymax);

    DStructGDL* Struct=NULL;
    Struct = SysVar::X();
    static unsigned windowTag = Struct->Desc()->TagIndex( "WINDOW");
    static unsigned sTag = Struct->Desc()->TagIndex( "S");
    if(Struct != NULL) {
      (*static_cast<DFloatGDL*>( Struct->GetTag( windowTag, 0)))[0] = p_xmin;
      (*static_cast<DFloatGDL*>( Struct->GetTag( windowTag, 0)))[1] = p_xmax;

      (*static_cast<DDoubleGDL*>( Struct->GetTag( sTag, 0)))[0] = 
	(p_xmin*xEnd - p_xmax*xStart) / (xEnd - xStart);
      (*static_cast<DDoubleGDL*>( Struct->GetTag( sTag, 0)))[1] = 
	(p_xmax - p_xmin) / (xEnd - xStart);
      
    }

    Struct = SysVar::Y();
    if(Struct != NULL) {
      (*static_cast<DFloatGDL*>( Struct->GetTag( windowTag, 0)))[0] = p_ymin;
      (*static_cast<DFloatGDL*>( Struct->GetTag( windowTag, 0)))[1] = p_ymax;

      (*static_cast<DDoubleGDL*>( Struct->GetTag( sTag, 0)))[0] = 
	(p_ymin*yEnd - p_ymax*yStart) / (yEnd - yStart);
      (*static_cast<DDoubleGDL*>( Struct->GetTag( sTag, 0)))[1] = 
	(p_ymax - p_ymin) / (yEnd - yStart);
    }


    // plot the data
    if(!e->KeywordSet("NODATA"))
      if(valid)
	valid=draw_polyline(e, actStream, 
			    xVal, yVal, xLog, yLog, 
			    yStart, yEnd, psym);

    actStream->lsty(1);//reset linestyle
    actStream->flush();

    // set ![XY].CRANGE
    set_axis_crange("X", xStart, xEnd);
    set_axis_crange("Y", minVal, maxVal);    


    //set ![x|y].type
    set_axis_type("X",xLog);
    set_axis_type("Y",yLog);
  } // plot

} // namespace

