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

  class axis_call : public plotting_routine_call {
    DDouble xPos, yPos, zPos;
    bool doT3d;
    bool xAxis, yAxis, zAxis;
    COORDSYS coordinateSystem = DATA;
    // will handle axis logness..
    bool xLog, yLog, zLog;
    // is current box log or not?
    bool xAxisWasLog, yAxisWasLog, zAxisWasLog;
    DLong xaxis_value, yaxis_value, zaxis_value;
    //in absence of arguments we will have:
    bool ynodef = true;
    bool xnodef = true;
    bool znodef = true;
    PLFLT viewportXSize, viewportYSize, viewportZSize;
    PLFLT xnormmin, xnormmax, ynormmin, ynormmax, znormmin=0, znormmax=1, zValue;
    // new x and y range, real values (not log)
    DDouble xStart, xEnd, yStart, yEnd, zStart, zEnd;

  private:

    bool handle_args(EnvT* e) {
      //NOTE: ZVALUE is not mentioned in doc, but is just ignored
      //T3D ?
      static int t3dIx = e->KeywordIx("T3D");
      doT3d = (e->BooleanKeywordSet(t3dIx) || T3Denabled());

      xAxis = true, yAxis = false, zAxis = false;
      static int xaxisIx = e->KeywordIx("XAXIS");
      static int yaxisIx = e->KeywordIx("YAXIS");
      static int zaxisIx = e->KeywordIx("ZAXIS");

      //IDL behaviour for XAXIS and YAXIS and ZAXIS options: only one option is considered, and ZAXIS above YAXIS above XAXIS
      if ((e->GetKW(xaxisIx) != NULL)) {
        xAxis = true;
        e->AssureLongScalarKWIfPresent(xaxisIx, xaxis_value);
      }
      if (e->GetKW(yaxisIx) != NULL) {
        yAxis = true;
        xAxis = false; // like in IDL, yaxis overrides xaxis
        e->AssureLongScalarKWIfPresent(yaxisIx, yaxis_value);
      }
      if (e->GetKW(zaxisIx) != NULL) {
        zAxis = true;
        xAxis = false;
        yAxis = false; // like in IDL, zaxis overrides all
        e->AssureLongScalarKWIfPresent(zaxisIx, zaxis_value);
      }

      static int DATAIx = e->KeywordIx("DATA");
      static int DEVICEIx = e->KeywordIx("DEVICE");
      static int NORMALIx = e->KeywordIx("NORMAL");
      //check presence of DATA,DEVICE and NORMAL options
      if (e->KeywordSet(DATAIx)) coordinateSystem = DATA;
      if (e->KeywordSet(DEVICEIx)) coordinateSystem = DEVICE;
      if (e->KeywordSet(NORMALIx)) coordinateSystem = NORMAL;

      // get current viewport coordinates in normalised units

      gdlGetCurrentAxisWindow(XAXIS, xnormmin, xnormmax);
      gdlGetCurrentAxisWindow(YAXIS, ynormmin, ynormmax);
      gdlGetCurrentAxisWindow(ZAXIS, znormmin, znormmax);
      zValue=znormmin;
      //undefined or null previous viewport, seems IDL returns without complain:
      if ((xnormmin == xnormmax) || (ynormmin == ynormmax)|| (doT3d && (znormmin == znormmax)) ) {std::cerr<<"AXIS: coordinate system not established\n";return true;} //abort

      viewportXSize = xnormmax - xnormmin;
      viewportYSize = ynormmax - ynormmin;
      viewportZSize = znormmax - znormmin;

      // old x y z range for axes : default in absence of [XYZ]RANGE etc in commandline.
      DDouble oxStart, oxEnd;
      DDouble oyStart, oyEnd;
      DDouble ozStart, ozEnd;

      gdlGetAxisType(XAXIS, xAxisWasLog);
      gdlGetAxisType(YAXIS, yAxisWasLog);
      gdlGetAxisType(ZAXIS, zAxisWasLog);
      
      GetCurrentUserLimits(oxStart,oxEnd, oyStart, oyEnd, ozStart, ozEnd);
      if (ozStart==0 && ozEnd-ozStart==0) ozEnd=1; //this may happen
      if (xAxisWasLog) {oxStart=pow(10,oxStart);oxEnd=pow(10,oxEnd);}
      if (yAxisWasLog) {oyStart=pow(10,oyStart);oyEnd=pow(10,oyEnd);}
      if (zAxisWasLog) {ozStart=pow(10,ozStart);ozEnd=pow(10,ozEnd);}
      
      gdlAdjustAxisRange(e, XAXIS, oxStart, oxEnd, xAxisWasLog); //Box adjustement
      gdlAdjustAxisRange(e, YAXIS, oyStart, oyEnd, yAxisWasLog); //Box adjustement
      gdlAdjustAxisRange(e, ZAXIS, ozStart, ozEnd, zAxisWasLog); //Box adjustement

      //position values are in the 'old' system just defined above.
      xPos = (xaxis_value==0) ? oxStart : oxEnd;
      yPos = (yaxis_value==0) ? oyStart : oyEnd;
      zPos=zValue;
//      zPos = (zaxis_value==0) ? ozStart : ozEnd;
      //read arguments 
      if (nParam() == 1) {
        e->AssureDoubleScalarPar(0, xPos);
        yPos=0;
        ynodef = false;
        xnodef = false;
      }
      if (nParam() == 2) {
        e->AssureDoubleScalarPar(0, xPos);
        xnodef = false;
        e->AssureDoubleScalarPar(1, yPos);
        ynodef = false;
      }
      if (nParam() == 3) {
        e->AssureDoubleScalarPar(0, xPos);
        xnodef = false;
        e->AssureDoubleScalarPar(1, yPos);
        ynodef = false;
        e->AssureDoubleScalarPar(2, zPos);
        znodef = false;
      }
      if (!ynodef) { 
        // where is point of world coords xVal, yVal in viewport relative coords?
        bool mapSet = false;
        get_mapset(mapSet);
        mapSet = (mapSet && coordinateSystem == DATA);
        if (mapSet) {
  #ifdef USE_LIBPROJ
          ref = map_init();
          if (ref == NULL) {
            e->Throw("Projection initialization failed.");
          }
          LPTYPE idata;
          XYTYPE odata;
  #if PROJ_VERSION_MAJOR >= 5
          idata.lam = xPos * DEG_TO_RAD;
          idata.phi = yPos * DEG_TO_RAD;
          odata = protect_proj_fwd_lp(idata, ref);
          xPos = odata.x;
          yPos = odata.y;
  #else
          idata.u = xPos * DEG_TO_RAD;
          idata.v = yPos * DEG_TO_RAD;
          odata = PJ_FWD(idata, ref);
          xPos = odata.u;
          yPos = odata.v;
  #endif
  #endif
        }
      }
      //normalized positions:
      ConvertToNormXY(1, &xPos, xAxisWasLog, &yPos, yAxisWasLog, coordinateSystem);
      ConvertToNormZ(1, &zPos,  zAxisWasLog, coordinateSystem);
           
      //default axis values: start with 'old' values.
      xLog = xAxisWasLog;
      yLog = yAxisWasLog; //by default logness is similar until another option is set
      zLog = zAxisWasLog;

      xStart = oxStart;
      xEnd = oxEnd;
      yStart = oyStart;
      yEnd = oyEnd;
      zStart = ozStart;
      zEnd = ozEnd;

      // handle Log options passing via Keywords
      // note: undocumented keywords [xyz]type still exist and
      // have priority on [xyz]log !
      static int xLogIx = e->KeywordIx("XLOG");
      static int yLogIx = e->KeywordIx("YLOG");
      static int zLogIx = e->KeywordIx("ZLOG");
      if (e->KeywordPresent(xLogIx)) xLog = e->KeywordSet(xLogIx);
      if (e->KeywordPresent(yLogIx)) yLog = e->KeywordSet(yLogIx);
      if (e->KeywordPresent(zLogIx)) zLog = e->KeywordSet(zLogIx);

      // note: undocumented keywords [xyz]type still exist and
      // have priority on [xyz]log ! In fact, it is the modulo (1, 3, 5 ... --> /log)   
      static int xTypeIx = e->KeywordIx("XTYPE");
      static int yTypeIx = e->KeywordIx("YTYPE");
      static int xType, yType;
      if (e->KeywordPresent(xTypeIx)) {
        e->AssureLongScalarKWIfPresent(xTypeIx, xType);
        if ((xType % 2) == 1) xLog = true;
        else xLog = false;
      }
      if (e->KeywordPresent(yTypeIx)) {
        e->AssureLongScalarKWIfPresent(yTypeIx, yType);
        if ((yType % 2) == 1) yLog = true;
        else yLog = false;
      }

      //XRANGE and YRANGE overrides all that, but  Start/End should be recomputed accordingly
      DDouble xAxisStart, xAxisEnd, yAxisStart, yAxisEnd, zAxisStart, zAxisEnd;
      bool setx = gdlGetDesiredAxisRange(e, XAXIS, xAxisStart, xAxisEnd);
      bool sety = gdlGetDesiredAxisRange(e, YAXIS, yAxisStart, yAxisEnd);
      bool setz = gdlGetDesiredAxisRange(e, ZAXIS, zAxisStart, zAxisEnd);

      // here LOG will apply ONLY if the RANGE was set-up.
      //IMHO this is an IDL BUG, but IDL interprets the 'CURRENT AXIS RANGE' as LOG values if [xyz]Log is typed.
      // meaning that if !Y.CRANGE=[2,200] and we ask for axis,0.5,0.5,/norm,yax=0,color='ff00'x,/ylo then the axis is in LOG between 10^2 and 10^200!
      if (setx) {
        xStart = xAxisStart;
        xEnd = xAxisEnd;
        gdlAdjustAxisRange(e, XAXIS, xAxisStart, xAxisEnd, xLog); //Box adjustement
     }
      if (sety) {
       yStart = yAxisStart;
       yEnd = yAxisEnd;
      gdlAdjustAxisRange(e, YAXIS, yAxisStart, yAxisEnd, yLog); //Box adjustement
// forgotten by IDL apparently also...
//        //handle Nozero option after all that!
//        if (!gdlYaxisNoZero(e) && yAxisStart > 0 && !yLog) yAxisStart = 0.0;
      }
      if (setz) {
        zStart = zAxisStart;
        zEnd = zAxisEnd;
        gdlAdjustAxisRange(e, ZAXIS, zAxisStart, zAxisEnd, zLog); //Box adjustement
      }

      return false; //do not abort
    }

    bool prepareDrawArea(EnvT* e, GDLGStream* actStream) {
      
      gdlSetGraphicsForegroundColorFromKw(e, actStream); //COLOR
      //    contrary to the documentation axis does not erase the plot (fortunately!)
      //    gdlNextPlotHandlingNoEraseOption(e, actStream, true);     //NOERASE -- not supported
 

      if (xAxis) {
        PLFLT vpXL, vpXR, vpYB, vpYT; //define new viewport in relative units
        if (xaxis_value == 0) {
          vpXL = (xAxis || xnodef) ? xnormmin : xPos;
          vpXR = (xAxis || xnodef) ? xnormmax : xnormmax;
          vpYB = (yAxis || ynodef) ? ynormmin : yPos;
          vpYT = (yAxis || ynodef) ? ynormmax : ynormmax;
        } else {
          vpXL = (xAxis || xnodef) ? xnormmin : xnormmin;
          vpXR = (xAxis || xnodef) ? xnormmax : xPos;
          vpYB = (yAxis || ynodef) ? ynormmin : ynormmin; //yPos - viewportYSize;
          vpYT = (yAxis || ynodef) ? ynormmax : yPos;
        }
        
      // TBD should not plot  axes if they are not on screen
        actStream->vpor(vpXL, vpXR, vpYB, vpYT);
        //insure 'wind' arguments are given, otherwise BAM! in plplot
        actStream->wind(xStart, xEnd, xLog, yStart, yEnd, yLog);
        if (doT3d) gdlStartT3DMatrixDriverTransform(actStream, zPos); //call for driver to perform special transform for all further drawing
        gdlAxis(e, actStream, XAXIS, xStart, xEnd, xLog, xaxis_value+1, viewportYSize);
      }

      if (yAxis) {
        PLFLT vpXL, vpXR, vpYB, vpYT; //define new viewport in relative units
        if (xaxis_value == 0) {
          vpXL = (xAxis || xnodef) ? xnormmin : xPos;
          vpXR = (xAxis || xnodef) ? xnormmax : xnormmax;
          vpYB = (yAxis || ynodef) ? ynormmin : yPos;
          vpYT = (yAxis || ynodef) ? ynormmax : ynormmax;
        } else {
          vpXL = (xAxis || xnodef) ? xnormmin : xnormmin;
          vpXR = (xAxis || xnodef) ? xnormmax : xPos;
          vpYB = (yAxis || ynodef) ? ynormmin : ynormmin; //yPos - viewportYSize;
          vpYT = (yAxis || ynodef) ? ynormmax : yPos;
        }
        actStream->vpor(vpXL, vpXR, vpYB, vpYT);
        //insure 'wind' arguments are given, otherwise BAM! in plplot
        actStream->wind(xStart, xEnd, xLog, yStart, yEnd, yLog);
        if (doT3d) gdlStartT3DMatrixDriverTransform(actStream, zPos); //call for driver to perform special transform for all further drawing
        gdlAxis(e, actStream, YAXIS, yStart, yEnd, yLog, yaxis_value+1, viewportXSize);
      }
      
      if (doT3d && zAxis) { //no use drawing something unseen
        DLong modifierCode=0;
        switch (zaxis_value) {
        case 0:
          modifierCode = 2;
          yPos = (ynodef) ? ynormmin : yPos;
          xPos = (xnodef) ? xnormmax : xPos;
          viewportXSize=xPos-xnormmin; if (viewportXSize<0.001) viewportXSize=0.01;
          actStream->vpor(xPos - viewportXSize, xPos, znormmin, znormmax);
          break;
        case 3:
          modifierCode = 2;
          yPos = (ynodef) ? ynormmax : yPos;
          xPos = (xnodef) ? xnormmax : xPos;
          viewportXSize=xPos-xnormmin; if (viewportXSize<0.001) viewportXSize=0.01;
          actStream->vpor(xPos - viewportXSize, xPos, znormmin, znormmax);
          break;
        case 1:
          modifierCode = 1;
          yPos = (ynodef) ? ynormmin : yPos;
          xPos = (xnodef) ? xnormmin : xPos;
          viewportXSize=xnormmax-xPos; if (viewportXSize<0.001) viewportXSize=0.01;
          actStream->vpor(xPos, xPos + viewportXSize, znormmin, znormmax);
          break;
        case 2:
          modifierCode = 1;
          yPos = (ynodef) ? ynormmax : yPos;
          xPos = (xnodef) ? xnormmin : xPos;
          viewportXSize=xnormmax-xPos; if (viewportXSize<0.001) viewportXSize=0.01;
          actStream->vpor(xPos, xPos + viewportXSize, znormmin, znormmax);
          break;
        }
        //insure 'wind' arguments are given, otherwise BAM! in plplot
        actStream->wind(xStart, xEnd, xLog, zStart, zEnd, zLog); //as Y but with Z
        //special transform to use 'y' axis code, but with 'z' values and yz exch.

        gdlStartT3DMatrixDriverTransform(actStream, yPos); 
        gdlExchange3DDriverTransform(actStream);
        gdlAxis(e, actStream, ZAXIS, zStart, zEnd, zLog, modifierCode, viewportZSize);
      }

      static int SAVEIx = e->KeywordIx("SAVE");
      if (e->KeywordSet(SAVEIx)) {
        if (xAxis) gdlStoreXAxisParameters(actStream, xStart, xEnd, xLog);
        if (yAxis) gdlStoreYAxisParameters(actStream, yStart, yEnd, yLog);
        if (zAxis) gdlStoreZAxisParameters(actStream, zStart, zEnd, zLog, znormmin, znormmax);
      }
      
      return false;
    }

  private:

    void applyGraphics(EnvT* e, GDLGStream* actStream) { }

  private:

    virtual void post_call(EnvT* e, GDLGStream* actStream) {
      gdlStop3DDriverTransform(actStream);
      actStream->sizeChar(1.0);
    }

  };

  void axis(EnvT* e) {
    axis_call axis;
    axis.call(e, 0);
  }

} // namespace
