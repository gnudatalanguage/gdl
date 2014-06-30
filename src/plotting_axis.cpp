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

    private: bool handle_args(EnvT* e)
    {
      return true;
    } 

  private: void old_body( EnvT* e, GDLGStream* actStream) 
  { 
    bool doT3d, real3d;
    DDouble zValue;
    //note: Z (VALUE) will be used uniquely if Z is not effectively defined.
    static int zvIx = e->KeywordIx( "Z");
    zValue=0.0;
    e->AssureDoubleScalarKWIfPresent ( zvIx, zValue );
    //T3D
    static int t3dIx = e->KeywordIx( "T3D");
    doT3d=(e->KeywordSet(t3dIx) || T3Denabled(e));

    DFloat xMarginL, xMarginR, yMarginB, yMarginT; 

    bool xAxis=false, yAxis=false, zAxis=false;
    static int xaxisIx = e->KeywordIx( "XAXIS");
    static int yaxisIx = e->KeywordIx( "YAXIS");
    static int zaxisIx = e->KeywordIx( "ZAXIS");
    
    PLINT xaxis_value, yaxis_value, zaxis_value;
    bool standardNumPos;
    //IDL behaviour for XAXIS and YAXIS and ZAXIS options: only one option is considered, and ZAXIS above YAXIS above XAXIS
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
    if( e->GetKW( zaxisIx) != NULL) {
      zAxis = true; xAxis = false; yAxis=false; // like in IDL, zaxis overrides all
      e->AssureLongScalarKWIfPresent( "ZAXIS", zaxis_value);
    }
    if( (e->GetKW( xaxisIx) == NULL) && (e->GetKW( yaxisIx) == NULL )  && ((e->GetKW( zaxisIx) == NULL )||!doT3d))  {
      xAxis = true; standardNumPos = true; 
    }
    
    // MARGIN
    gdlGetDesiredAxisMargin(e, "X",xMarginL, xMarginR);
    gdlGetDesiredAxisMargin(e, "Y",yMarginB, yMarginT);

    // will handle axis logness..
    bool xLog, yLog, zLog;
    // is current box log or not?
    bool xAxisWasLog, yAxisWasLog, zAxisWasLog;
    gdlGetAxisType("X", xAxisWasLog);
    gdlGetAxisType("Y", yAxisWasLog);
    gdlGetAxisType("Z", zAxisWasLog);
    xLog=xAxisWasLog;
    yLog=yAxisWasLog; //by default logness is similar until another option is set
    zLog=zAxisWasLog;

    enum
    {
      DATA=0,
      NORMAL,
      DEVICE
    } coordinateSystem=DATA;
    //check presence of DATA,DEVICE and NORMAL options
    if ( e->KeywordSet("DATA") ) coordinateSystem=DATA;
    if ( e->KeywordSet("DEVICE") ) coordinateSystem=DEVICE;
    if ( e->KeywordSet("NORMAL") ) coordinateSystem=NORMAL;

    // x and y range, old and new
    DDouble oxStart, oxEnd;
    DDouble oyStart, oyEnd;
    DDouble xStart, xEnd;
    DDouble yStart, yEnd;
    // get viewport coordinates in normalised units
    PLFLT ovpXL, ovpXR, ovpYB, ovpYT;
    actStream->gvpd(ovpXL, ovpXR, ovpYB, ovpYT);
    //undefined previous viewport, seems IDL returns without complain:
    if ((ovpXL==0.0 && ovpXR==0.0) || (ovpYB==0.0 && ovpYT==0.0)) return;
    PLFLT ovpSizeX, ovpSizeY;
    ovpSizeX=ovpXR-ovpXL;
    ovpSizeY=ovpYT-ovpYB;
    //get wiewport window in world coordinates
    PLFLT xmin, xmax, ymin, ymax;
    actStream->gvpw(xmin, xmax, ymin, ymax);
    xStart=oxStart=xmin;
    xEnd=oxEnd=xmax;
    yStart=oyStart=ymin;
    yEnd=oyEnd=ymax;
    //convert these values to real values if box was log
    if (xAxisWasLog) {xStart=pow(10,xStart);xEnd=pow(10,xEnd);}
    if (yAxisWasLog) {yStart=pow(10,yStart);yEnd=pow(10,yEnd);}

    static int xLogIx = e->KeywordIx( "XLOG" );
    static int yLogIx = e->KeywordIx( "YLOG" );
    xLog = (xLog || e->KeywordSet( xLogIx ));
    yLog = (yLog || e->KeywordSet( yLogIx ));

    //YNOZERO corrects yStart
    if ( e->KeywordSet( "YNOZERO") && yStart >0 && !yLog ) yStart=0.0;
	bool setdummy;
    setdummy=gdlGetDesiredAxisRange( e, "X", xStart, xEnd);
	setdummy=gdlGetDesiredAxisRange( e, "Y", yStart, yEnd);
    if (xStart == xEnd && yStart == yEnd) {
      e->Throw("Invalid plotting ranges.  Set up a plot window first.");
    }
    gdlHandleUnwantedAxisValue(xStart, xEnd, xLog);
    gdlHandleUnwantedAxisValue(yStart, yEnd, yLog);

    DDouble yVal, xVal;
    //in absence of arguments we will have:
    yVal=(standardNumPos)?oyStart:oyEnd;
    xVal=(standardNumPos)?oxStart:oxEnd;
    //read arguments 
    if (nParam() == 1) {
      e->AssureDoubleScalarPar( 0, xVal);
    }
    if (nParam() == 2) {
      e->AssureDoubleScalarPar( 0, xVal);
      e->AssureDoubleScalarPar( 1, yVal);
    }
    if (nParam() == 3) {
      e->Throw( "Sorry, we do not yet support the 3D case");
    }

    // *** start drawing
    gdlSetGraphicsForegroundColorFromKw(e, actStream);       //COLOR
    //    contrary to the documentation axis does not erase the plot (fortunately!)
    //    gdlNextPlotHandlingNoEraseOption(e, actStream, true);     //NOERASE -- not supported
    gdlSetPlotCharthick(e,actStream); 
    gdlSetPlotCharsize(e, actStream);    //CHARSIZE

    PLFLT vpXL, vpXR, vpYB, vpYT; //define new viewport in relative units
    // where is point of world coords xVal, yVal in viewport relative coords?
    DDouble vpX,vpY;
    if ( coordinateSystem==DEVICE )
    {
      actStream->DeviceToNormedDevice(xVal, yVal, vpX, vpY);
    }
    else if ( coordinateSystem==NORMAL )
    {
      vpX=xVal;
      vpY=yVal;
    }
    else
    {
      if (xAxisWasLog) xVal=log10(xVal);
      if (yAxisWasLog) yVal=log10(yVal);
      actStream->WorldToNormedDevice(xVal, yVal, vpX, vpY);
    }
    //compute new temporary viewport in relative coords
#define ADDEPSILON 0.1
    if ( standardNumPos )
    {
      vpXL=(xAxis)?ovpXL:vpX;
      vpXR=(xAxis)?ovpXR:vpX+ovpSizeY;
      vpYB=(xAxis)?vpY:ovpYB;
      vpYT=(xAxis)?vpY+ovpSizeX:ovpYT;
    }
    else
    {
      vpXL=(xAxis)?ovpXL:vpX-ovpSizeY;
      vpXR=(xAxis)?ovpXR:vpX;
      vpYB=(xAxis)?vpY-ovpSizeX:ovpYB;
      vpYT=(xAxis)?vpY:ovpYT;
    }

    actStream->OnePageSaveLayout(); // one page

    actStream->vpor(vpXL, vpXR, vpYB, vpYT);
    if (xLog) {xStart=log10(xStart);xEnd=log10(xEnd);}
    if (yLog) {yStart=log10(yStart);yEnd=log10(yEnd);}
    actStream->wind(xStart, xEnd, yStart, yEnd);

    if ( xAxis )
    { //special name "axisX" needed because we artificially changed size of box
      gdlAxis(e, actStream, "axisX", xStart, xEnd, xLog, standardNumPos?1:2, ovpSizeY);

      if ( e->KeywordSet("SAVE") )
      {
        gdlStoreAxisCRANGE("X", xStart, xEnd, xLog);
        gdlStoreAxisType("X", xLog);
        gdlStoreAxisSandWINDOW(actStream, "X", xStart, xEnd, xLog);
      }
    }

    if ( yAxis )
    {//special name "axisY" needed because we artificially changed size of box
      gdlAxis(e, actStream, "axisY", yStart, yEnd, yLog, standardNumPos?1:2, ovpSizeX);

      if ( e->KeywordSet("SAVE") )
      {
        gdlStoreAxisCRANGE("Y", yStart, yEnd, yLog);
        gdlStoreAxisType("Y", yLog);
        gdlStoreAxisSandWINDOW(actStream, "Y", yStart, yEnd, yLog);
      }
    }
    // reset the viewport and world coordinates to the original values
    actStream->RestoreLayout();

  }

    private: void call_plplot(EnvT* e, GDLGStream* actStream) 
    {
    } 
 
    private: virtual void post_call(EnvT* e, GDLGStream* actStream)
    {
       actStream->sizeChar(1.0);
    } 

  }; 

  void axis(EnvT* e)
  {
    axis_call axis;
    axis.call(e, 0);
  }

} // namespace
