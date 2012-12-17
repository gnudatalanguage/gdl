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

#ifdef _MSC_VER
#define isnan _isnan
#endif

namespace lib {

  using namespace std;

  class plot_call : public plotting_routine_call 
  {
    DDoubleGDL *yVal, *xVal, *xTemp, *yTemp;
    SizeT xEl, yEl;
    DDouble minVal, maxVal, xStart, xEnd, yStart, yEnd;
    bool doMinMax;
    bool xLog, yLog, wasBadxLog, wasBadyLog;
    DLong psym;
    auto_ptr<BaseGDL> xval_guard,yval_guard,xtemp_guard;

private:

  bool handle_args(EnvT* e) // {{{
  {
    bool polar = FALSE;
    DLong nsum = 1;
    e->AssureLongScalarKWIfPresent("NSUM", nsum);
    if (e->KeywordSet("POLAR"))
    {
      polar = TRUE;
      // e->Throw( "Sorry, POLAR keyword not ready");
    }

    DDoubleGDL *yValBis, *xValBis;
    auto_ptr<BaseGDL> xvalBis_guard, yvalBis_guard;
    //test and transform eventually if POLAR and/or NSUM!
    if (nParam() == 1)
    {
      yTemp = e->GetParAs< DDoubleGDL > (0);
      if (yTemp->Rank() == 0)
        e->Throw("Expression must be an array in this context: " + e->GetParString(0));
      yEl=yTemp->N_Elements();
      xEl=yEl;
      xTemp = new DDoubleGDL(dimension(xEl), BaseGDL::INDGEN);
      xtemp_guard.reset(xTemp); // delete upon exit
    }
    else
    {
      xTemp = e->GetParAs< DDoubleGDL > (0);
      if (xTemp->Rank() == 0)
        e->Throw("Expression must be an array in this context: " + e->GetParString(0));
      xEl=xTemp->N_Elements();
      yTemp = e->GetParAs< DDoubleGDL > (1);
      if (yTemp->Rank() == 0)
        e->Throw("Expression must be an array in this context: " + e->GetParString(1));
      yEl=yTemp->N_Elements();
      //silently drop unmatched values
      if (yEl!= xEl)
      {
        SizeT size;
        size = min(xEl, yEl);
        xEl = size;
        yEl = size;
      }
    }
    //check nsum validity
    nsum = max(1, nsum);
    nsum = min(nsum, (DLong)xEl);

    if (nsum == 1)
    {
      if (polar)
      {
        xVal = new DDoubleGDL(dimension(xEl), BaseGDL::NOZERO);
        xval_guard.reset(xVal); // delete upon exit
        yVal = new DDoubleGDL(dimension(yEl), BaseGDL::NOZERO);
        yval_guard.reset(yVal); // delete upon exit
        for (int i = 0; i < xEl; i++) (*xVal)[i] = (*xTemp)[i] * cos((*yTemp)[i]);
        for (int i = 0; i < yEl; i++) (*yVal)[i] = (*xTemp)[i] * sin((*yTemp)[i]);
      }
      else
      { //careful about previously set autopointers!
        if (nParam() == 1) xval_guard = xtemp_guard;
        xVal = xTemp;
        yVal = yTemp;
      }
    }
    else
    {
      int i, j, k;
      DLong size = (DLong)xEl / nsum;
      xVal = new DDoubleGDL(size, BaseGDL::ZERO); //SHOULD BE ZERO, IS NOT!
      xval_guard.reset(xVal); // delete upon exit
      yVal = new DDoubleGDL(size, BaseGDL::ZERO); //IDEM
      yval_guard.reset(yVal); // delete upon exit
      for (i = 0, k = 0; i < size; i++)
      {
        (*xVal)[i] = 0.0;
        (*yVal)[i] = 0.0;
        for (j = 0; j < nsum; j++, k++)
        {
          (*xVal)[i] += (*xTemp)[k];
          (*yVal)[i] += (*yTemp)[k];
        }
      }
      for (i = 0; i < size; i++) (*xVal)[i] /= nsum;
      for (i = 0; i < size; i++) (*yVal)[i] /= nsum;

      if (polar)
      {
        DDouble x, y;
        for (i = 0; i < size; i++)
        {
          x = (*xVal)[i] * cos((*yVal)[i]);
          y = (*xVal)[i] * sin((*yVal)[i]);
          (*xVal)[i] = x;
          (*yVal)[i] = y;
        }
      }
    }
    // keyword overrides
    static int xLogIx = e->KeywordIx("XLOG");
    static int yLogIx = e->KeywordIx("YLOG");
    xLog = e->KeywordSet(xLogIx);
    yLog = e->KeywordSet(yLogIx);
    // compute adequate values for log scale, warn adequately...
    wasBadxLog = FALSE;
    wasBadyLog = FALSE;
    if (xLog)
    {
      DLong minEl, maxEl;
      xVal->MinMax(&minEl, &maxEl, NULL, NULL, true);
      if ((*xVal)[minEl] <= 0.0) wasBadxLog = TRUE;
      xValBis = new DDoubleGDL(dimension(xEl), BaseGDL::NOZERO);
      xvalBis_guard.reset(xValBis); // delete upon exit
      for (int i = 0; i < xEl; i++) (*xValBis)[i] = log10((*xVal)[i]);
    }
    else xValBis = xVal;
    if (yLog)
    {
      DLong minEl, maxEl;
      yVal->MinMax(&minEl, &maxEl, NULL, NULL, true);
      if ((*yVal)[minEl] <= 0.0) wasBadyLog = TRUE;
      yValBis = new DDoubleGDL(dimension(yEl), BaseGDL::NOZERO);
      yvalBis_guard.reset(yValBis); // delete upon exit
      for (int i = 0; i < yEl; i++) (*yValBis)[i] = log10((*yVal)[i]);
    }
    else yValBis = yVal;
    //   BaseGDL *x, *y;
    {
      DLong minEl, maxEl, debug=0;

      xValBis->MinMax(&minEl, &maxEl, NULL, NULL, true);
      xStart = (*xVal)[minEl];
      if (isnan(xStart)) xStart = 1e-12;
      if (wasBadxLog) xStart = 1e-12;
      xEnd = (*xVal)[maxEl];
      if (isnan(xEnd)) xEnd = 1.0;
     if (wasBadxLog) {
	xStart = 1e-20;
	xEnd = 1.;
      }

      if (debug) cout << "X Min/Max : " << xStart << " " << xEnd << endl;
      if (debug) cout << "xLog mode : " << xLog << endl;

      yValBis->MinMax(&minEl, &maxEl, NULL, NULL, true);
      yStart = (*yVal)[minEl];
      if (wasBadyLog) yStart = 1e-12;
      if (isnan(yStart)) yStart = 1e-12;
      yEnd = (*yVal)[maxEl];
      if (isnan(yEnd)) yEnd = 1.0;
      if (wasBadyLog) {
	yStart = 1e-20;
	yEnd = 1.;
      }


      if (debug) cout << "Y Min/Max : " << yStart << " " << yEnd << endl;
      if (debug) cout << "yLog mode : " << yLog << endl;

    }
    return false;
  } // }}}

  private: void old_body( EnvT* e, GDLGStream* actStream) // {{{
  {
    //ISOTROPIC
    DLong iso=0;
    e->AssureLongScalarKWIfPresent( "ISOTROPIC", iso);

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

    // Please remember the {X|Y}range overwrite the data range
    //[x|y]range keyword
    gkw_axis_range(e, "X", xStart, xEnd, xnozero);
    gkw_axis_range(e, "Y", yStart, yEnd, ynozero);

    if ((xLog && xStart <= 0.0) || wasBadxLog) Warning( "PLOT: Infinite x plot range.");
    if ((yLog && yStart <= 0.0) || wasBadyLog) Warning( "PLOT: Infinite y plot range.");
    //xStyle and yStyle apply on range values

//    // style applies on the final values
    if ((xStyle & 1) != 1) {
      PLFLT intv = AutoIntvAC(xStart, xEnd, xnozero, xLog);
    }
    if ((yStyle & 1) != 1) {
      PLFLT intv = AutoIntvAC(yStart, yEnd, ynozero, yLog);
    }

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
    static int positionIx = e->KeywordIx( "POSITION"); 
    DFloatGDL* pos = e->IfDefGetKWAs<DFloatGDL>( positionIx);
    if (pos == NULL) pos = (DFloatGDL*) 0xF;

    // *** start drawing. Graphic Keywords accepted: BACKGROUND, CHARSIZE, CHARTHICK, CLIP, COLOR, DATA, DEVICE, FONT, LINESTYLE, NOCLIP, NODATA, NOERASE, NORMAL, POSITION, PSYM, SUBTITLE, SYMSIZE, T3D, THICK, TICKLEN, TITLE, [XYZ]CHARSIZE, [XYZ]GRIDSTYLE, [XYZ]MARGIN(OK), [XYZ]MINOR, [XYZ]RANGE, [XYZ]STYLE, [XYZ]THICK, [XYZ]TICKFORMAT, [XYZ]TICKINTERVAL, [XYZ]TICKLAYOUT, [XYZ]TICKLEN, [XYZ]TICKNAME, [XYZ]TICKS, [XYZ]TICKUNITS, [XYZ]TICKV, [XYZ]TICK_GET, [XYZ]TITLE, ZVALUE
    gkw_background(e, actStream);  //BACKGROUND
    gkw_color(e, actStream);       //COLOR

    gkw_psym(e, psym);//PSYM

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
			    xStart, xEnd, yStart, yEnd, iso);
    if( !okVPWC) return;


    //now we can setup minVal and maxVal to defaults: Start-End and overload if KW present

    minVal = yStart; //to give a reasonable value...
    maxVal = yEnd;   //idem
    doMinMax = false; //although we will not use it...
    if( e->KeywordSet( "MIN_VALUE") || e->KeywordSet( "MAX_VALUE"))
      doMinMax = true; //...unless explicitely required
    e->AssureDoubleScalarKWIfPresent( "MIN_VALUE", minVal);
    e->AssureDoubleScalarKWIfPresent( "MAX_VALUE", maxVal);

    //AXES:
    // pen thickness for axis
    actStream->wid( 0);

    // axis
    string xOpt="bc", yOpt="bc";
    AdjustAxisOpts(xOpt, yOpt, xStyle, yStyle, xTicks, yTicks, xTickformat, yTickformat, xLog, yLog);

    DLong charthick=0;
    e->AssureLongScalarKWIfPresent("CHARTHICK",charthick);
    actStream->wid(charthick);

//X
    // axis titles
    actStream->schr( 0.0, actH/defH * xCharSize);
    actStream->mtex("b",3.5,0.5,0.5,xTitle.c_str());

    DLong xthick=0;
    e->AssureLongScalarKWIfPresent("XTHICK",xthick);
    actStream->wid(xthick);
    // the axis (separate for x and y axis because of charsize)
    PLFLT xintv;
    if (xTicks == 0) {
      xintv = AutoTick(xEnd-xStart);
    } else {
      xintv = (xEnd - xStart) / xTicks;
      // changing "xintv" has no effects in plplot in Log Mode ...
      // http://sourceforge.net/tracker/index.php?func=detail&aid=3095515&group_id=2915&atid=202915
      if (yLog) Warning("PLOT: XTICKS keyword not active in plplot in Log mode");
    }
    actStream->box( xOpt.c_str(), xintv, xMinor, "", 0.0, 0);
//Y
    actStream->wid(charthick);
    actStream->schr( 0.0, actH/defH * yCharSize);
    actStream->mtex("l",5.0,0.5,0.5,yTitle.c_str());
    
    DLong ythick=0;
    e->AssureLongScalarKWIfPresent("YTHICK",ythick);
    actStream->wid(ythick);
    // the axis (separate for x and y axis because of charsize)
    PLFLT yintv;
    if (yTicks == 0) {
      yintv = AutoTick(yEnd-yStart);
    } else {
      yintv = (yEnd - yStart) / yTicks;
      if (yLog) Warning("PLOT: YTICKS keyword not active in plplot in Log mode");
    }
    
    int debug=0;
    if (debug) cout << xOpt.c_str() << endl;
    if (debug) cout << yOpt.c_str() << endl;
    if (debug) cout << xintv << " "<< yintv<< endl;

    actStream->box( "", 0.0, 0, yOpt.c_str(), yintv, yMinor);
    // reset pen thickness
    actStream->wid( 0);

    // title and sub title
    gkw_title(e, actStream, actH/defH);

    // pen thickness for plot
    gkw_thick(e, actStream);
    gkw_symsize(e, actStream);
    gkw_linestyle(e, actStream);

    UpdateSWPlotStructs(actStream, xStart, xEnd, yStart, yEnd, xLog, yLog);

  } // }}}
  
    private: void call_plplot(EnvT* e, GDLGStream* actStream) // {{{
    {
      // plot the data
      static int nodataIx = e->KeywordIx( "NODATA"); 
      if (!e->KeywordSet(nodataIx)) 
      {
        bool valid = draw_polyline(e, actStream, xVal, yVal, minVal, maxVal, doMinMax, xLog, yLog, psym, FALSE);
        // TODO: handle valid?
      }
    } // }}}

    private: void post_call(EnvT* e, GDLGStream* actStream) // {{{
    {
      actStream->lsty(1);//reset linestyle

      // set ![XY].CRANGE
      set_axis_crange("X", xStart, xEnd, xLog);
      set_axis_crange("Y", yStart, yEnd, yLog);

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

