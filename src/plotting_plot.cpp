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

  class plot_call : public plotting_routine_call 
  {
    DDoubleGDL *yVal, *xVal;
    DDouble minVal, maxVal, xStart, xEnd, yStart, yEnd;
    bool xLog, yLog;
    DLong psym;
    auto_ptr<BaseGDL> xval_guard;

    private: bool handle_args( EnvT* e) // {{{
    {
      if( nParam() == 1)
      {
        yVal = e->GetParAs< DDoubleGDL>( 0);
        if (yVal->Rank() == 0) 
          e->Throw("Expression must be an array in this context: "+e->GetParString(0));
        xVal = new DDoubleGDL( dimension( yVal->N_Elements()), BaseGDL::INDGEN);
        xval_guard.reset( xVal); // delete upon exit
      }
      else
      {
        xVal = e->GetParAs< DDoubleGDL>( 0);
        if (xVal->Rank() == 0) 
          e->Throw("Expression must be an array in this context: "+e->GetParString(0));
        yVal = e->GetParAs< DDoubleGDL>( 1);
        if (yVal->Rank() == 0) 
          e->Throw("Expression must be an array in this context: "+e->GetParString(1));
      }

      if ( e->KeywordSet( "POLAR")) 
      {
        e->Throw( "Sorry, POLAR keyword not ready");
      }

      //   BaseGDL *x, *y;
      {
        DLong minEl, maxEl;

        xVal->MinMax( &minEl, &maxEl, NULL, NULL, true);
        xStart = (*xVal)[minEl];
        xEnd = (*xVal)[maxEl];

        yVal->MinMax( &minEl, &maxEl, NULL, NULL, true);
        yStart = (*yVal)[minEl];
        yEnd = (*yVal)[maxEl];
      }
      return false;
    } // }}}

  private: void old_body( EnvT* e, GDLGStream* actStream) // {{{
  {

    // [XY]STYLE
    DLong xStyle=0, yStyle=0; 
    gkw_axis_style(e, "X", xStyle);
    gkw_axis_style(e, "Y", yStyle);

    e->AssureLongScalarKWIfPresent( "XSTYLE", xStyle);
    e->AssureLongScalarKWIfPresent( "YSTYLE", yStyle);

    // AXIS TITLE
    DString xTitle, yTitle; 
    gkw_axis_title(e, "X", xTitle);
    gkw_axis_title(e, "Y", yTitle);

    // MARGIN
    DFloat xMarginL, xMarginR, yMarginB, yMarginT; 
    gkw_axis_margin(e, "X", xMarginL, xMarginR);
    gkw_axis_margin(e, "Y", yMarginB, yMarginT);

    DLong xnozero=1, ynozero=0;
    if ( e->KeywordSet( "YNOZERO")) ynozero = 1;

    // keyword overrides
    static int xLogIx = e->KeywordIx( "XLOG");
    static int yLogIx = e->KeywordIx( "YLOG");
    xLog = e->KeywordSet( xLogIx);
    yLog = e->KeywordSet( yLogIx);

    if (xLog && xStart <= 0.0)
      Warning( "PLOT: Infinite x plot range.");
    if (yLog && yStart <= 0.0)
      Warning( "PLOT: Infinite y plot range.");

    if ( e->KeywordSet( "YNOZERO")) ynozero = 1;

    if ((xStyle & 1) != 1) {
      PLFLT intv = AutoIntvAC(xStart, xEnd, xnozero, xLog);
    }
    if ((yStyle & 1) != 1) {
      PLFLT intv = AutoIntvAC(yStart, yEnd, ynozero, yLog);
    }

    // Please remember the {X|Y}range overwrite the data range
    //[x|y]range keyword
    gkw_axis_range(e, "X", xStart, xEnd, xnozero);
    gkw_axis_range(e, "Y", yStart, yEnd, ynozero);

    if (xLog && xStart <= 0.0)
      Warning( "PLOT: Infinite x plot range.");
    if (yLog && yStart <= 0.0)
      Warning( "PLOT: Infinite y plot range.");

    minVal = yStart;
    maxVal = yEnd;
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

    // *** start drawing
    gkw_background(e, actStream);  //BACKGROUND
    gkw_color(e, actStream);       //COLOR

    {
      bool line;
      gkw_psym(e, actStream, line, psym);//PSYM
    }

    DFloat charsize, xCharSize, yCharSize;
    gkw_charsize(e, actStream, charsize);  //CHARSIZE
    gkw_axis_charsize(e, "X", xCharSize);   //XCHARSIZE
    gkw_axis_charsize(e, "Y", yCharSize);   //YCHARSIZE

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

    UpdateSWPlotStructs(actStream, xStart, xEnd, yStart, yEnd);

  } // }}}
  
    private: void call_plplot(EnvT* e, GDLGStream* actStream) // {{{
    {
      // plot the data
      static int nodataIx = e->KeywordIx( "NODATA"); 
      if (!e->KeywordSet(nodataIx)) 
      {
        bool valid = draw_polyline(e, actStream, xVal, yVal, xLog, yLog, yStart, yEnd, psym);
        // TODO: handle valid?
      }
    } // }}}

    private: void post_call(EnvT* e, GDLGStream* actStream) // {{{
    {
      actStream->lsty(1);//reset linestyle

      // set ![XY].CRANGE
      set_axis_crange("X", xStart, xEnd);
      set_axis_crange("Y", minVal, maxVal);    

      //set ![x|y].type
      set_axis_type("X",xLog);
      set_axis_type("Y",yLog);
    } // }}}

  };

  void plot(EnvT* e)
  {
    plot_call plot;
    plot.call(e, 1);
  }

} // namespace

