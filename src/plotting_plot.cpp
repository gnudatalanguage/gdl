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
    auto_ptr<BaseGDL> xval_guard,yval_guard,xtemp_guard;
    DLong iso;

private:

  bool handle_args(EnvT* e) 
  {
    bool polar = FALSE;
    DLong nsum = 1;
    e->AssureLongScalarKWIfPresent("NSUM", nsum);
    if (e->KeywordSet("POLAR"))
    {
      polar = TRUE;
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
    // handle Log options
    int xLogIx = e->KeywordIx("XLOG");
    int yLogIx = e->KeywordIx("YLOG");
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

#define UNDEF_RANGE_VALUE 1E-12
    {
      DLong minEl, maxEl;
      xValBis->MinMax(&minEl, &maxEl, NULL, NULL, true);
      xStart = (*xVal)[minEl];
      if (isnan(xStart)) xStart = UNDEF_RANGE_VALUE;
      xEnd = (*xVal)[maxEl];
      if (isnan(xEnd)) xEnd = 1.0;

      yValBis->MinMax(&minEl, &maxEl, NULL, NULL, true);
      yStart = (*yVal)[minEl];
      if (isnan(yStart)) yStart = UNDEF_RANGE_VALUE;
      yEnd = (*yVal)[maxEl];
      if (isnan(yEnd)) yEnd = 1.0;
    }
    //MIN_VALUE and MAX_VALUE overwrite yStart/yEnd eventually (note: the points will not be "seen" at all in plots)
    minVal = yStart; //to give a reasonable value...
    maxVal = yEnd;   //idem
    doMinMax = false; //although we will not use it...
    if( e->KeywordSet( "MIN_VALUE") || e->KeywordSet( "MAX_VALUE"))
      doMinMax = true; //...unless explicitely required
    e->AssureDoubleScalarKWIfPresent( "MIN_VALUE", minVal);
    e->AssureDoubleScalarKWIfPresent( "MAX_VALUE", maxVal);
    yStart=gdlPlot_Max(yStart,minVal);
    yEnd=gdlPlot_Min(yEnd,maxVal);
    //XRANGE and YRANGE overrides all that, but  Start/End should be recomputed accordingly
    DDouble xAxisStart, xAxisEnd, yAxisStart, yAxisEnd;
    bool setx=gdlGetDesiredAxisRange(e, "X", xAxisStart, xAxisEnd);
    bool sety=gdlGetDesiredAxisRange(e, "Y", yAxisStart, yAxisEnd);
    if(setx && sety)
    {
      xStart=xAxisStart;
      xEnd=xAxisEnd;
      yStart=yAxisStart;
      yEnd=yAxisEnd;
    }
    else if (sety)
    {
      yStart=yAxisStart;
      yEnd=yAxisEnd;
      //must compute min-max for other axis!
      {
        gdlDoRangeExtrema(yVal,xVal,xStart,xEnd,yStart,yEnd);
      }
    }
    else if (setx)
    {
      xStart=xAxisStart;
      xEnd=xAxisEnd;
      //must compute min-max for other axis!
      {
        gdlDoRangeExtrema(xVal,yVal,yStart,yEnd,xStart,xEnd,doMinMax,minVal,maxVal);
      }
    }
    //handle Nozero option after all that!
    if(!gdlYaxisNoZero(e) && yStart >0 && !yLog ) yStart=0.0;
#undef UNDEF_RANGE_VALUE

     //ISOTROPIC
    iso=0;
    e->AssureLongScalarKWIfPresent( "ISOTROPIC", iso);
    return false;
  }

  private: void old_body( EnvT* e, GDLGStream* actStream) 
  {
    //start a plot
    gdlNextPlotHandlingNoEraseOption(e, actStream);     //NOERASE

    // *** start drawing. Graphic Keywords accepted: BACKGROUND, CHARSIZE, CHARTHICK, CLIP, COLOR, DATA, 
    //DEVICE, FONT, LINESTYLE, NOCLIP, NODATA, NOERASE, NORMAL, POSITION, PSYM, SUBTITLE, SYMSIZE, T3D,
    //THICK, TICKLEN, TITLE, [XYZ]CHARSIZE, [XYZ]GRIDSTYLE, [XYZ]MARGIN(OK), [XYZ]MINOR, [XYZ]RANGE,
    //[XYZ]STYLE, [XYZ]THICK, [XYZ]TICKFORMAT, [XYZ]TICKINTERVAL, [XYZ]TICKLAYOUT, [XYZ]TICKLEN,
    //[XYZ]TICKNAME, [XYZ]TICKS, [XYZ]TICKUNITS, [XYZ]TICKV, [XYZ]TICK_GET, [XYZ]TITLE, ZVALUE

    // [XY]STYLE
    DLong xStyle=0, yStyle=0;
    gdlGetDesiredAxisStyle(e, "X", xStyle);
    gdlGetDesiredAxisStyle(e, "Y", yStyle);

     //xStyle and yStyle apply on range values
    if ((xStyle & 1) != 1) {
      PLFLT intv = AutoIntvAC(xStart, xEnd, xLog);
    }
    if ((yStyle & 1) != 1) {
      PLFLT intv = AutoIntvAC(yStart, yEnd, yLog);
    }

    // MARGIN
    DFloat xMarginL, xMarginR, yMarginB, yMarginT;
    gdlGetDesiredAxisMargin(e, "X", xMarginL, xMarginR);
    gdlGetDesiredAxisMargin(e, "Y", yMarginB, yMarginT);

    // viewport and world coordinates
    // use POSITION
    int positionIx = e->KeywordIx( "POSITION");
    DFloatGDL* boxPosition = e->IfDefGetKWAs<DFloatGDL>( positionIx);
    if (boxPosition == NULL) boxPosition = (DFloatGDL*) 0xF;
    // set the PLOT charsize before setting viewport (margin depend on charsize)
    gdlSetPlotCharsize(e, actStream);
    //fix viewport and coordinates for box
    if (gdlSetViewPortAndWorldCoordinates( e, actStream, boxPosition,
			    xLog, yLog,
			    xMarginL, xMarginR, yMarginB, yMarginT,
			    xStart, xEnd, yStart, yEnd, iso)==FALSE) return; //no good: should catch an exception to get out of this mess.
    // background...

    gdlSetGraphicsBackgroundColorFromKw(e, actStream);
    //current pen color...
    gdlSetGraphicsForegroundColorFromKw(e, actStream);
    gdlSetPlotCharthick(e,actStream); 

    gdlBox(e, actStream, xStart, xEnd, yStart, yEnd, xLog, yLog);
  } 
  
    private: void call_plplot(EnvT* e, GDLGStream* actStream) 
    {
      DLong psym;
      // plot the data
      int nodataIx = e->KeywordIx( "NODATA"); 
      if ( !e->KeywordSet(nodataIx) )
      {
        //get psym
        gdlGetPsym(e, psym); //PSYM
        //handle clipping
        bool doClip=(e->KeywordSet("CLIP")||e->KeywordSet("NOCLIP"));
        // make all clipping computations BEFORE setting graphic properties (color, size)
        bool stopClip=false;
        if ( doClip )  if ( startClipping(e, actStream, false)==TRUE ) stopClip=true;
        // here graphic properties
        gdlSetPenThickness(e, actStream);
        gdlSetSymsize(e, actStream);
        gdlSetLineStyle(e, actStream);
        // TODO: handle "valid"!
        bool valid=draw_polyline(e, actStream, xVal, yVal, minVal, maxVal, doMinMax, xLog, yLog, psym, FALSE);
        if (stopClip) stopClipping(actStream);
      }
    } 

    private: void post_call(EnvT* e, GDLGStream* actStream) 
    {
      actStream->lsty(1);//reset linestyle
      actStream->sizeChar(1.0);
    } 

  };

  void plot(EnvT* e)
  {
    plot_call plot;
    plot.call(e, 1);
  }

} // namespace

