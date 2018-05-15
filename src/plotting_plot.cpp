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
//  using std::isinf;
#ifndef _MSC_VER
  using std::isnan;
#endif

  class plot_call : public plotting_routine_call 
  {
    DDoubleGDL *yVal, *xVal, *xTemp, *yTemp;
    SizeT xEl, yEl, zEl;
    DDouble minVal, maxVal, xStart, xEnd, yStart, yEnd, zValue;
    bool doMinMax;
    bool xLog, yLog, wasBadxLog, wasBadyLog;
    Guard<BaseGDL> xval_guard, yval_guard, xtemp_guard;
    DLong iso;
    bool doT3d;
    DFloat  UsymConvX, UsymConvY;
    int calendar_codex;
    int calendar_codey;
private:

  bool handle_args(EnvT* e) 
  {

    //T3D ?
    static int t3dIx = e->KeywordIx( "T3D");
    doT3d=(e->KeywordSet(t3dIx)|| T3Denabled());

    //note: Z (VALUE) will be used uniquely if Z is not effectively defined.
    static int zvIx = e->KeywordIx( "ZVALUE");
    zValue=0.0;
    e->AssureDoubleScalarKWIfPresent ( zvIx, zValue );
    zValue=min(zValue,0.999999); //to avoid problems with plplot
    zValue=max(zValue,0.0);

    // system variable !P.NSUM first
    DLong nsum=(*static_cast<DLongGDL*>(SysVar::P()-> GetTag(SysVar::P()->Desc()->TagIndex("NSUM"), 0)))[0];
      static int NSUMIx = e->KeywordIx( "NSUM");
      e->AssureLongScalarKWIfPresent( NSUMIx, nsum);

      static int polarIx = e->KeywordIx( "POLAR");
      bool polar = (e->KeywordSet(polarIx));

    DDoubleGDL *yValBis, *xValBis;
    Guard<BaseGDL> xvalBis_guard, yvalBis_guard;
    //test and transform eventually if POLAR and/or NSUM!
    if (nParam() == 1)
    {
      yTemp = e->GetParAs< DDoubleGDL > (0);
      if (yTemp->Rank() == 0)
        e->Throw("Expression must be an array in this context: " + e->GetParString(0));
      yEl=yTemp->N_Elements();
      xEl=yEl;
      xTemp = new DDoubleGDL(dimension(xEl), BaseGDL::INDGEN);
      xtemp_guard.Reset(xTemp); // delete upon exit
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
        xval_guard.Reset(xVal); // delete upon exit
        yVal = new DDoubleGDL(dimension(yEl), BaseGDL::NOZERO);
        yval_guard.Reset(yVal); // delete upon exit
        for (int i = 0; i < xEl; i++) (*xVal)[i] = (*xTemp)[i] * cos((*yTemp)[i]);
        for (int i = 0; i < yEl; i++) (*yVal)[i] = (*xTemp)[i] * sin((*yTemp)[i]);
      }
      else
      { //careful about previously set autopointers!
        if (nParam() == 1) xval_guard.Init( xtemp_guard.release());
        xVal = xTemp;
        yVal = yTemp;
      }
    }
    else
    {
      int i, j, k;
      DLong size = (DLong)xEl / nsum;
      xVal = new DDoubleGDL(size, BaseGDL::ZERO); //SHOULD BE ZERO, IS NOT!
      xval_guard.Reset(xVal); // delete upon exit
      yVal = new DDoubleGDL(size, BaseGDL::ZERO); //IDEM
      yval_guard.Reset(yVal); // delete upon exit
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

    xLog=FALSE;
    yLog=FALSE;

    // handle Log options passing via Functions names PLOT_IO/OO/OI
    // the behavior can be superseed by [xy]log or [xy]type
    string ProName = e->GetProName( );
    if ( ProName != "PLOT" ) {
      if ( ProName == "PLOT_IO" ) yLog = TRUE;
      if ( ProName == "PLOT_OI" ) xLog = TRUE;
      if ( ProName == "PLOT_OO" ) {
        xLog = TRUE;
        yLog = TRUE;
      }
    }

    // handle Log options passing via Keywords
    // note: undocumented keywords [xyz]type still exist and
    // have priority on [xyz]log !
    static int xLogIx = e->KeywordIx( "XLOG" );
    static int yLogIx = e->KeywordIx( "YLOG" );
    if (e->KeywordPresent(xLogIx)) xLog = e->KeywordSet(xLogIx);
    if (e->KeywordPresent(yLogIx)) yLog = e->KeywordSet(yLogIx);

    // note: undocumented keywords [xyz]type still exist and
    // have priority on [xyz]log ! In fact, it is the modulo (1, 3, 5 ... --> /log)   
    static int xTypeIx = e->KeywordIx( "XTYPE" );
    static int yTypeIx = e->KeywordIx( "YTYPE" );
    static int xType, yType;
    if (e->KeywordPresent(xTypeIx)) {
      e->AssureLongScalarKWIfPresent(xTypeIx, xType);
      if ((xType % 2) == 1) xLog= TRUE; else xLog= FALSE;
    }
    if (e->KeywordPresent(yTypeIx)) {
      e->AssureLongScalarKWIfPresent( yTypeIx, yType);
      if ((yType % 2) == 1) yLog= TRUE; else yLog= FALSE;
    }
    
    //  cout << xType<< " " <<xLog << " "<<yType <<" " << yLog << endl;

    static int xTickunitsIx = e->KeywordIx( "XTICKUNITS" );
    static int yTickunitsIx = e->KeywordIx( "YTICKUNITS" );

    calendar_codex = gdlGetCalendarCode(e,"X");
    calendar_codey = gdlGetCalendarCode(e,"Y");
    if ( e->KeywordSet( xTickunitsIx ) && xLog) {
      Message( "PLOT: LOG setting ignored for Date/Time TICKUNITS." );
      xLog = FALSE;
    }
    if ( e->KeywordSet( yTickunitsIx ) && yLog) {
      Message( "PLOT: LOG setting ignored for Date/Time TICKUNITS." );
      yLog = FALSE;
    }


    //    cout << xLog << " " << yLog << endl;

    // compute adequate values for log scale, warn adequately...
    wasBadxLog = FALSE;
    wasBadyLog = FALSE;
    if (xLog)
    {
      DLong minEl, maxEl;
      xVal->MinMax(&minEl, &maxEl, NULL, NULL, true,0,xEl); //restrict minmax to xEl fist elements!!!!
      if ((*xVal)[minEl] <= 0.0) wasBadxLog = TRUE;
      xValBis = new DDoubleGDL(dimension(xEl), BaseGDL::NOZERO);
      xvalBis_guard.Reset(xValBis); // delete upon exit
      for (int i = 0; i < xEl; i++) (*xValBis)[i] = log10((*xVal)[i]);
    }
    else xValBis = xVal;
    if (yLog)
    {
      DLong minEl, maxEl;
      yVal->MinMax(&minEl, &maxEl, NULL, NULL, true,0,yEl);
      if ((*yVal)[minEl] <= 0.0) wasBadyLog = TRUE;
      yValBis = new DDoubleGDL(dimension(yEl), BaseGDL::NOZERO);
      yvalBis_guard.Reset(yValBis); // delete upon exit
      for (int i = 0; i < yEl; i++) (*yValBis)[i] = log10((*yVal)[i]);
    }
    else yValBis = yVal;

#define UNDEF_RANGE_VALUE 1E-12
    {
      DLong minEl, maxEl;
      xValBis->MinMax(&minEl, &maxEl, NULL, NULL, true, 0, xEl); //restrict minmax to xEl fist elements!!!!
      xStart = (*xVal)[minEl];
      xEnd = (*xVal)[maxEl];
      if (isnan(xStart)) xStart = UNDEF_RANGE_VALUE;
      if (isnan(xEnd)) xEnd = 1.0;
      if (xStart==xEnd) xStart=xEnd-UNDEF_RANGE_VALUE;

      yValBis->MinMax(&minEl, &maxEl, NULL, NULL, true, 0, yEl);
      yStart = (*yVal)[minEl];
      yEnd = (*yVal)[maxEl];
      if (isnan(yStart)) yStart = UNDEF_RANGE_VALUE;
      if (isnan(yEnd)) yEnd = 1.0;
      if (yStart==yEnd) yStart=yEnd-UNDEF_RANGE_VALUE;
    }
    //MIN_VALUE and MAX_VALUE overwrite yStart/yEnd eventually (note: the points will not be "seen" at all in plots)
    minVal = yStart; //to give a reasonable value...
    maxVal = yEnd;   //idem
    doMinMax = false; //although we will not use it...
    static int MIN_VALUEIx = e->KeywordIx("MIN_VALUE");
    static int MAX_VALUEIx = e->KeywordIx("MAX_VALUE");
    if( e->KeywordSet(MIN_VALUEIx) || e->KeywordSet(MAX_VALUEIx))
      doMinMax = true; //...unless explicitely required
    e->AssureDoubleScalarKWIfPresent( MIN_VALUEIx, minVal);
    e->AssureDoubleScalarKWIfPresent( MAX_VALUEIx, maxVal);
    yStart=gdlPlot_Max(yStart,minVal);
    yEnd=gdlPlot_Min(yEnd,maxVal);
    if (yEnd <= yStart) yEnd=yStart+1;
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
    //handle Nozero option after all that! (gdlAxisNoZero test if /ynozero option is valid (ex: no YRANGE)
    if(!gdlYaxisNoZero(e) && yStart >0 && !yLog ) yStart=0.0;
#undef UNDEF_RANGE_VALUE

     //ISOTROPIC
    iso=0;
    static int ISOTROPICIx = e->KeywordIx("ISOTROPIC");
    e->AssureLongScalarKWIfPresent( ISOTROPICIx, iso);

    return false;
  }

  private: void old_body( EnvT* e, GDLGStream* actStream) 
  {
    // background BEFORE next plot since it is the only place plplot may redraw the background...
    gdlSetGraphicsBackgroundColorFromKw(e, actStream);
   //start a plot
    gdlNextPlotHandlingNoEraseOption(e, actStream);     //NOERASE


    // [XY]STYLE
    DLong xStyle=0, yStyle=0;
    gdlGetDesiredAxisStyle(e, "X", xStyle);
    gdlGetDesiredAxisStyle(e, "Y", yStyle);

     //xStyle and yStyle apply on range values
    if ((xStyle & 1) != 1) {
      PLFLT intv = gdlAdjustAxisRange(e, "X", xStart, xEnd, xLog, calendar_codex);
    }
    if ((yStyle & 1) != 1) {
      PLFLT intv = gdlAdjustAxisRange(e, "Y", yStart, yEnd, yLog, calendar_codey);
    }

    // MARGIN
    DFloat xMarginL, xMarginR, yMarginB, yMarginT;
    gdlGetDesiredAxisMargin(e, "X", xMarginL, xMarginR);
    gdlGetDesiredAxisMargin(e, "Y", yMarginB, yMarginT);

    // viewport and world coordinates
    // set the PLOT charsize before setting viewport (margin depend on charsize)
    gdlSetPlotCharsize(e, actStream);
    
    if (doT3d)
    {

      static DDouble x0,y0,xs,ys; //conversion to normalized coords
      x0=(xLog)?-log10(xStart):-xStart;
      y0=(yLog)?-log10(yStart):-yStart;
      xs=(xLog)?(log10(xEnd)-log10(xStart)):xEnd-xStart;xs=1.0/xs;
      ys=(yLog)?(log10(yEnd)-log10(yStart)):yEnd-yStart;ys=1.0/ys;
    
      DDoubleGDL* plplot3d;
      DDouble az, alt, ay, scale;
      ORIENTATION3D axisExchangeCode;

      plplot3d = gdlConvertT3DMatrixToPlplotRotationMatrix( zValue, az, alt, ay, scale, axisExchangeCode);
      if (plplot3d == NULL)
      {
        e->Throw("Illegal 3D transformation. (FIXME)");
      }
      if (gdlSet3DViewPortAndWorldCoordinates(e, actStream, plplot3d, xLog, yLog,
        xStart, xEnd, yStart, yEnd) == FALSE) return;
      gdlSetGraphicsForegroundColorFromKw(e, actStream);

      DDouble  t3xStart, t3xEnd, t3yStart, t3yEnd, t3zStart, t3zEnd;
      switch (axisExchangeCode) {
        case NORMAL3D: //X->X Y->Y plane XY
          t3xStart=(xLog)?log10(xStart):xStart,
          t3xEnd=(xLog)?log10(xEnd):xEnd,
          t3yStart=(yLog)?log10(yStart):yStart,
          t3yEnd=(yLog)?log10(yEnd):yEnd,
          t3zStart=0;
          t3zEnd=1.0;
          actStream->w3d(scale, scale, scale*(1.0 - zValue),
          t3xStart,t3xEnd,t3yStart,t3yEnd,t3zStart,t3zEnd,
          alt, az);
          gdlAxis3(e, actStream, "X", xStart, xEnd, xLog);
          gdlAxis3(e, actStream, "Y", yStart, yEnd, yLog);
          break;
        case XY: // X->Y Y->X plane XY
          t3yStart=(xLog)?log10(xStart):xStart,
          t3yEnd=(xLog)?log10(xEnd):xEnd,
          t3xStart=(yLog)?log10(yStart):yStart,
          t3xEnd=(yLog)?log10(yEnd):yEnd,
          t3zStart=0;
          t3zEnd=1.0;
          actStream->w3d(scale, scale, scale*(1.0 - zValue),
          t3xStart,t3xEnd,t3yStart,t3yEnd,t3zStart,t3zEnd,
          alt, az);
          gdlAxis3(e, actStream, "Y", xStart, xEnd, xLog);
          gdlAxis3(e, actStream, "X", yStart, yEnd, yLog);
          break;
        case XZ: // Y->Y X->Z plane YZ
          t3zStart=(xLog)?log10(xStart):xStart,
          t3zEnd=(xLog)?log10(xEnd):xEnd,
          t3yStart=(yLog)?log10(yStart):yStart,
          t3yEnd=(yLog)?log10(yEnd):yEnd,
          t3xStart=0;
          t3xEnd=1.0;
          actStream->w3d(scale, scale, scale,
          t3xStart,t3xEnd,t3yStart,t3yEnd,t3zStart,t3zEnd,
          alt, az);
          gdlAxis3(e, actStream, "Z", xStart, xEnd, xLog,0);
          gdlAxis3(e, actStream, "Y", yStart, yEnd, yLog);
          break;
        case YZ: // X->X Y->Z plane XZ
          t3xStart=(xLog)?log10(xStart):xStart,
          t3xEnd=(xLog)?log10(xEnd):xEnd,
          t3zStart=(yLog)?log10(yStart):yStart,
          t3zEnd=(yLog)?log10(yEnd):yEnd,
          t3yStart=0;
          t3yEnd=1.0;
          actStream->w3d(scale, scale, scale,
          t3xStart,t3xEnd,t3yStart,t3yEnd,t3zStart,t3zEnd,
          alt, az);
          gdlAxis3(e, actStream, "X", xStart, xEnd, xLog);
          gdlAxis3(e, actStream, "Z", yStart, yEnd, yLog,1);
          break;
        case XZXY: //X->Y Y->Z plane YZ
          t3yStart=(xLog)?log10(xStart):xStart,
          t3yEnd=(xLog)?log10(xEnd):xEnd,
          t3zStart=(yLog)?log10(yStart):yStart,
          t3zEnd=(yLog)?log10(yEnd):yEnd,
          t3xStart=0;
          t3xEnd=1.0;
          actStream->w3d(scale, scale, scale,
          t3xStart,t3xEnd,t3yStart,t3yEnd,t3zStart,t3zEnd,
          alt, az);
          gdlAxis3(e, actStream, "Y", xStart, xEnd, xLog);
          gdlAxis3(e, actStream, "Z", yStart, yEnd, yLog);
          break;
        case XZYZ: //X->Z Y->X plane XZ
          t3zStart=(xLog)?log10(xStart):xStart,
          t3zEnd=(xLog)?log10(xEnd):xEnd,
          t3xStart=(yLog)?log10(yStart):yStart,
          t3xEnd=(yLog)?log10(yEnd):yEnd,
          t3yStart=0;
          t3yEnd=1.0;
          actStream->w3d(scale, scale, scale,
          t3xStart,t3xEnd,t3yStart,t3yEnd,t3zStart,t3zEnd,
          alt, az);
          gdlAxis3(e, actStream, "Z", xStart, xEnd, xLog,1);
          gdlAxis3(e, actStream, "X", yStart, yEnd, yLog);
          break;
      }
      //finish box: since there is no option in plplot for upper box 3d axes, do it ourselve:
      Data3d.zValue = 0;
      Data3d.Matrix = plplot3d; //try to change for !P.T in future?
            Data3d.x0=x0;
            Data3d.y0=y0;
            Data3d.xs=xs;
            Data3d.ys=ys;
      switch (axisExchangeCode) {
          case NORMAL3D: //X->X Y->Y plane XY
            Data3d.code = code012;
            actStream->stransform(gdl3dTo2dTransform, &Data3d);      
            actStream->join(t3xStart,t3yEnd,t3xEnd,t3yEnd);
            actStream->join(t3xEnd,t3yStart,t3xEnd,t3yEnd);
            break;
          case XY: // X->Y Y->X plane XY
            Data3d.code = code012;
            actStream->stransform(gdl3dTo2dTransform, &Data3d);      
            actStream->join(t3yStart,t3xEnd,t3yEnd,t3xEnd);
            actStream->join(t3yEnd,t3xStart,t3yEnd,t3xEnd);
            break;
          case XZ: // Y->Y X->Z plane YZ
            Data3d.code = code210;
            actStream->stransform(gdl3dTo2dTransform, &Data3d);      
            actStream->join(t3zStart,t3yStart,t3zEnd,t3yStart);
            actStream->join(t3zEnd,t3yStart,t3zEnd,t3yEnd);
          default:
            break;
//          case YZ: // X->X Y->Z plane XZ
//            cerr<<"yz"<<endl;
//            Data3d.code = code021;
//            actStream->stransform(gdl3dTo2dTransform, &Data3d);      
//            break;
//          case XZXY: //X->Y Y->Z plane YZ
//            cerr<<"xzxy"<<endl;
//            Data3d.code = code120;
//            actStream->stransform(gdl3dTo2dTransform, &Data3d);      
//            break;
//          case XZYZ: //X->Z Y->X plane XZ
//            cerr<<"xzyz"<<endl;
//            Data3d.code = code201;
//            actStream->stransform(gdl3dTo2dTransform, &Data3d);      
//            break;
        }
      
      //data: will plot using coordinates transform.
      //TODO: unless PSYM=0 (optimize)
      
      Data3d.zValue = zValue;
      Data3d.Matrix = plplot3d; //try to change for !P.T in future?
            Data3d.x0=x0;
            Data3d.y0=y0;
            Data3d.xs=xs;
            Data3d.ys=ys;
      switch (axisExchangeCode) {
          case NORMAL3D: //X->X Y->Y plane XY
            Data3d.code = code012;
            break;
          case XY: // X->Y Y->X plane XY
            Data3d.code = code102;
            break;
          case XZ: // Y->Y X->Z plane YZ
            Data3d.code = code210;
            break;
          case YZ: // X->X Y->Z plane XZ
            Data3d.code = code021;
            break;
          case XZXY: //X->Y Y->Z plane YZ
            Data3d.code = code120;
            break;
          case XZYZ: //X->Z Y->X plane XZ
            Data3d.code = code201;
            break;
        }

      actStream->stransform(gdl3dTo2dTransform, &Data3d);
      // title and sub title
      gdlWriteTitleAndSubtitle(e, actStream);

    } else
    {
    //fix viewport and coordinates for non-3D box. this permits to have correct UsymConv values.
    // it is important to fix simsize before!
    gdlSetSymsize(e, actStream);
    if (gdlSetViewPortAndWorldCoordinates(e, actStream,
        xLog, yLog,
        xMarginL, xMarginR, yMarginB, yMarginT,
        xStart, xEnd, yStart, yEnd, iso)==FALSE) return; //no good: should catch an exception to get out of this mess.
    actStream->setSymbolSizeConversionFactors();
    //current pen color...
      gdlSetGraphicsForegroundColorFromKw(e, actStream);
      gdlBox(e, actStream, xStart, xEnd, yStart, yEnd, xLog, yLog);
    }
  } 
  
    private: void call_plplot(EnvT* e, GDLGStream* actStream) 
    {
      DLong psym;
      // plot the data
      static int nodataIx = e->KeywordIx( "NODATA"); 
      if ( !e->KeywordSet(nodataIx) )
      {
        bool stopClip=false;
        if ( startClipping(e, actStream, false)==TRUE ) stopClip=true;
        // here graphic properties
        gdlSetPenThickness(e, actStream);
        gdlSetLineStyle(e, actStream);
        gdlGetPsym(e, psym); //PSYM
        draw_polyline(actStream, xVal, yVal, minVal, maxVal, doMinMax, xLog, yLog, psym);
        if (stopClip) stopClipping(actStream);
      }
    } 

    private: void post_call(EnvT* e, GDLGStream* actStream) 
    {
//      gdlBox(e, actStream, xStart, xEnd, yStart, yEnd, xLog, yLog);
      if (doT3d) actStream->stransform(NULL,NULL);
      actStream->lsty(1);//reset linestyle
      actStream->sizeChar(1.0);
    } 

  };

  void plot(EnvT* e)
  {
    plot_call plot;
    plot.call(e, 1);
  }

  void plot_io(EnvT* e) {plot(e);}
  void plot_oo(EnvT* e) {plot(e);}
  void plot_oi(EnvT* e) {plot(e);}

} // namespace

