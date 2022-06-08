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

static GDL_3DTRANSFORMDEVICE PlotDevice3d;

namespace lib {

  using namespace std;

  class axis_call : public plotting_routine_call {
    DDouble xPos, yPos, zPos;
    bool doT3d;
    bool standardNumPos;
    bool xAxis, yAxis, zAxis;
    COORDSYS coordinateSystem = DATA;
    // will handle axis logness..
    bool xLog, yLog, zLog;
    // is current box log or not?
    bool xAxisWasLog, yAxisWasLog, zAxisWasLog;
    //in absence of arguments we will have:
    bool ynodef = true;
    bool xnodef = true;
    bool znodef = true;
    PLFLT viewportXSize, viewportYSize, viewportZSize;
    PLFLT xnormmin, xnormmax, ynormmin, ynormmax, znormmin, znormmax;
    // new x and y range, real values (not log)
    DDouble xStart, xEnd, yStart, yEnd, zStart, zEnd;

  private:

    bool handle_args(EnvT* e) {

      //T3D ?
      static int t3dIx = e->KeywordIx("T3D");
      doT3d = (e->BooleanKeywordSet(t3dIx) || T3Denabled());

      xAxis = true, yAxis = false, zAxis = false;
      static int xaxisIx = e->KeywordIx("XAXIS");
      static int yaxisIx = e->KeywordIx("YAXIS");
      static int zaxisIx = e->KeywordIx("ZAXIS");

      PLINT xaxis_value, yaxis_value, zaxis_value;
      //IDL behaviour for XAXIS and YAXIS and ZAXIS options: only one option is considered, and ZAXIS above YAXIS above XAXIS
      standardNumPos = true;
      if ((e->GetKW(xaxisIx) != NULL)) {
        xAxis = true;
        e->AssureLongScalarKWIfPresent(xaxisIx, xaxis_value);
        if (xaxis_value == 0) {
          standardNumPos = true;
        } else {
          standardNumPos = false;
        }
      }
      if (e->GetKW(yaxisIx) != NULL) {
        yAxis = true;
        xAxis = false; // like in IDL, yaxis overrides xaxis
        e->AssureLongScalarKWIfPresent(yaxisIx, yaxis_value);
        if (yaxis_value == 0) {
          standardNumPos = true;
        } else {
          standardNumPos = false;
        }
      }
      if (e->GetKW(zaxisIx) != NULL) {
        zAxis = true;
        xAxis = false;
        yAxis = false; // like in IDL, zaxis overrides all
        e->AssureLongScalarKWIfPresent(zaxisIx, zaxis_value);
        if (zaxis_value == 0) {
          standardNumPos = true;
        } else {
          standardNumPos = false;
        }
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
      //undefined or null previous viewport, seems IDL returns without complain:
      if ((xnormmin == xnormmax) || (ynormmin == ynormmax)|| (doT3d && (znormmin == znormmax)) ) return true; //abort

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
      gdlAdjustAxisRange(e, XAXIS, oxStart, oxEnd, xAxisWasLog); //Box adjustement
      gdlAdjustAxisRange(e, YAXIS, oyStart, oyEnd, yAxisWasLog); //Box adjustement
      gdlAdjustAxisRange(e, ZAXIS, ozStart, ozEnd, zAxisWasLog); //Box adjustement

      //position values are in the 'old' system just defined above.
      xPos = (standardNumPos) ? oxStart : oxEnd;
      if (xAxisWasLog) xPos=pow(10,xPos);
      yPos = (standardNumPos) ? oyStart : oyEnd;
      if (yAxisWasLog) yPos=pow(10,yPos);
      zPos = (standardNumPos) ? ozStart : ozEnd; 
      if (xAxisWasLog) zPos=pow(10,zPos);
      //read arguments 
      if (nParam() == 1) {
        e->AssureDoubleScalarPar(0, yPos);
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

      //default values: start with 'old' values.
      
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
      if (e->KeywordPresent(yLogIx)) zLog = e->KeywordSet(zLogIx);

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
      gdlAdjustAxisRange(e, XAXIS, xAxisStart, xAxisEnd, xLog); //Box adjustement
      bool sety = gdlGetDesiredAxisRange(e, YAXIS, yAxisStart, yAxisEnd);
      gdlAdjustAxisRange(e, YAXIS, yAxisStart, yAxisEnd, yLog); //Box adjustement
      bool setz = gdlGetDesiredAxisRange(e, ZAXIS, zAxisStart, zAxisEnd);
      gdlAdjustAxisRange(e, ZAXIS, zAxisStart, zAxisEnd, zLog); //Box adjustement

      // here LOG will apply ONLY if the RANGE was set-up.
      //IMHO this is an IDL BUG, but IDL interprets the 'CURRENT AXIS RANGE' as LOG values if [xyz]Log is typed.
      // meaning that if !Y.CRANGE=[2,200] and we ask for axis,0.5,0.5,/norm,yax=0,color='ff00'x,/ylo then the axis is in LOG between 10^2 and 10^200!
      if (setx) {
        xStart = (xLog)?log10(xAxisStart):xAxisStart;
        xEnd = (xLog)?log10(xAxisEnd):xAxisEnd;
     }
      if (sety) {
       yStart = (yLog)?log10(yAxisStart):yAxisStart;
        yEnd = (yLog)?log10(yAxisEnd):yAxisEnd;
// forgotten by IDL apparently also...
//        //handle Nozero option after all that!
//        if (!gdlYaxisNoZero(e) && yAxisStart > 0 && !yLog) yAxisStart = 0.0;
      }
      if (setz) {
        zStart = (zLog)?log10(zAxisStart):zAxisStart;
        zEnd = (zLog)?log10(zAxisEnd):zAxisEnd;
      }

      return false; //do not abort
    }

  private:

    bool prepareDrawArea(EnvT* e, GDLGStream* actStream) {
      gdlSetGraphicsForegroundColorFromKw(e, actStream); //COLOR
      //    contrary to the documentation axis does not erase the plot (fortunately!)
      //    gdlNextPlotHandlingNoEraseOption(e, actStream, true);     //NOERASE -- not supported
      PLFLT vpXL, vpXR, vpYB, vpYT, vpZB, vpZT; //define new viewport in relative units
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
      SelfConvertToNormXYZ(1, &xPos, xAxisWasLog , &yPos, yAxisWasLog, &zPos, zAxisWasLog, coordinateSystem);

      //compute new temporary viewport in relative coords
      if (standardNumPos) {
        vpXL = (xAxis || xnodef) ? xnormmin : xPos;
        vpXR = (xAxis || xnodef) ? xnormmax : xPos + viewportXSize;
        vpYB = (yAxis || ynodef) ? ynormmin : yPos;
        vpYT = (yAxis || ynodef) ? ynormmax : yPos + viewportYSize;
        vpZB = (zAxis || znodef) ? znormmin : zPos - viewportXSize;
        vpZT = (zAxis || znodef) ? znormmax : zPos;
      } else {
        vpXL = (xAxis || xnodef) ? xnormmin : xPos - viewportXSize;
        vpXR = (xAxis || xnodef) ? xnormmax : xPos;
        vpYB = (yAxis || ynodef) ? ynormmin : yPos - viewportYSize;
        vpYT = (yAxis || ynodef) ? ynormmax : yPos;
        vpZB = (zAxis || znodef) ? znormmin : zPos;
        vpZT = (zAxis || znodef) ? znormmax : zPos - viewportYSize;
      }

      actStream->OnePageSaveLayout(); // one page

      static int SAVEIx = e->KeywordIx("SAVE");
      bool doSave = e->KeywordSet(SAVEIx);

      if (xAxis) { //special ID "XAXIS2" needed because we artificially changed size of box
        actStream->vpor(vpXL, vpXR, vpYB, vpYT);
        //insure 'wind' arguments are given, otherwise BAM! in plplot
        actStream->wind(xStart, xEnd, yStart, yEnd);

        if (doT3d) { //call for driver to perform special transform for all further drawing
          gdlGetT3DMatrixForDriverTransform(PlotDevice3d.T);
          PlotDevice3d.zValue = zPos;
          actStream->cmd(PLESC_3D, &PlotDevice3d);
        }
        
        gdlAxis(e, actStream, XAXIS, xStart, xEnd, xLog, standardNumPos ? 1 : 2, viewportYSize);

        if (doSave) {
//          gdlStoreAxisCRANGE(XAXIS, xStart, xEnd, xLog);
//          gdlStoreAxisType(XAXIS, xLog);
          gdlStoreXAxisParameters(actStream, xStart, xEnd, xLog);
        }
      }

      if (yAxis) {//special id "YAXIS2" needed because we artificially changed size of box
        actStream->vpor(vpXL, vpXR, vpYB, vpYT);
        //insure 'wind' arguments are given, otherwise BAM! in plplot
        actStream->wind(xStart, xEnd, yStart, yEnd);

        if (doT3d) { //call for driver to perform special transform for all further drawing
          gdlGetT3DMatrixForDriverTransform(PlotDevice3d.T);
          PlotDevice3d.zValue = zPos;
          actStream->cmd(PLESC_3D, &PlotDevice3d);
        }
        
        gdlAxis(e, actStream, YAXIS, yStart, yEnd, yLog, standardNumPos ? 1 : 2, viewportXSize);

        if (doSave) {
//          gdlStoreAxisCRANGE(YAXIS, yStart, yEnd, yLog);
//          gdlStoreAxisType(YAXIS, yLog);
          gdlStoreYAxisParameters(actStream, yStart, yEnd, yLog);
        }
      }
      
      if (doT3d && zAxis) { //no use drawing something unseen
        
        actStream->vpor(vpXL, vpXR, vpZB, vpZT);
        //insure 'wind' arguments are given, otherwise BAM! in plplot
        actStream->wind(xStart, xEnd, zStart, zEnd);

        gdlGetT3DMatrixForDriverTransform(PlotDevice3d.T);
        yzaxisExch(PlotDevice3d.T);
        PlotDevice3d.zValue = yPos;
        actStream->cmd(PLESC_3D, &PlotDevice3d);
        gdlAxis(e, actStream, YAXIS, zStart, zEnd, zLog, standardNumPos ? 1 : 2, viewportXSize);
        if (doSave) {
//          gdlStoreAxisCRANGE(ZAXIS, zStart, zEnd, zLog);
//          gdlStoreAxisType(ZAXIS, zLog);
          gdlStoreYAxisParameters(actStream, zStart, zEnd, zLog);
        }
      }
      // reset the viewport and world coordinates to the original values
      actStream->RestoreLayout();
      // if save, update world coordinates to the new values -- should not make any difference.
      if (doSave) {
        actStream->wind(xStart, xEnd, yStart, yEnd);
      }
      return false;
    }

  private:

    void applyGraphics(EnvT* e, GDLGStream* actStream) { }

  private:

    virtual void post_call(EnvT* e, GDLGStream* actStream) {
      if (doT3d) { //reset driver to 2D plotting routines.
        actStream->cmd(PLESC_2D, NULL);
      }
      actStream->sizeChar(1.0);
    }

  };

  void axis(EnvT* e) {
    axis_call axis;
    axis.call(e, 0);
  }

} // namespace
