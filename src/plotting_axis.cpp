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
    DDouble zValue;
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

      //note: Z (VALUE) will be used uniquely if Z is not effectively defined.
      static int zvIx = e->KeywordIx("Z");
      zValue = 0.0;
      e->AssureDoubleScalarKWIfPresent(zvIx, zValue);

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
      }

      gdlGetAxisType(XAXIS, xAxisWasLog);
      gdlGetAxisType(YAXIS, yAxisWasLog);
      gdlGetAxisType(ZAXIS, zAxisWasLog);
      xLog = xAxisWasLog;
      yLog = yAxisWasLog; //by default logness is similar until another option is set
      zLog = zAxisWasLog;

      static int DATAIx = e->KeywordIx("DATA");
      static int DEVICEIx = e->KeywordIx("DEVICE");
      static int NORMALIx = e->KeywordIx("NORMAL");
      //check presence of DATA,DEVICE and NORMAL options
      if (e->KeywordSet(DATAIx)) coordinateSystem = DATA;
      if (e->KeywordSet(DEVICEIx)) coordinateSystem = DEVICE;
      if (e->KeywordSet(NORMALIx)) coordinateSystem = NORMAL;

      // get viewport coordinates in normalised units

      gdlGetCurrentAxisWindow(XAXIS, xnormmin, xnormmax);
      gdlGetCurrentAxisWindow(YAXIS, ynormmin, ynormmax);
      gdlGetCurrentAxisWindow(ZAXIS, znormmin, znormmax);
      //undefined or null previous viewport, seems IDL returns without complain:
      if ((xnormmin == xnormmax) || (ynormmin == ynormmax)) return true; //abort


      // old x and y range
      DDouble oxStart, oxEnd;
      DDouble oyStart, oyEnd;
      DDouble ozStart, ozEnd;

      // get ![XY].CRANGE
      gdlGetCurrentAxisRange(XAXIS, oxStart, oxEnd, FALSE); //ignore projection limits, convert to linear values if necessary.
      gdlGetCurrentAxisRange(YAXIS, oyStart, oyEnd, FALSE);
      gdlGetCurrentAxisRange(ZAXIS, ozStart, ozEnd, FALSE);

      if ((oyStart == oyEnd) || (oxStart == oxEnd) || (ozStart == ozEnd)) {
        if (oyStart != 0.0 && oyStart == oyEnd) {
          oyStart = 0;
          oyEnd = 1;
        }
        if (oxStart != 0.0 && oxStart == oxEnd) {
          oxStart = 0;
          oxEnd = 1;
        }
        if (ozStart != 0.0 && ozStart == ozEnd) {
          ozStart = 0;
          ozEnd = 1;
        }

      } else {
        if (xLog) {
          oxStart = pow(oxStart, 10);
          oxEnd = pow(oxEnd, 10);
        }
        if (yLog) {
          oyStart = pow(oyStart, 10);
          oyEnd = pow(oxEnd, 10);
        }
        if (zLog) {
          ozStart = pow(ozStart, 10);
          ozEnd = pow(ozEnd, 10);
        }
      }

      viewportXSize = xnormmax - xnormmin;
      viewportYSize = ynormmax - ynormmin;
      viewportZSize = znormmax - znormmin;
      xStart = oxStart;
      xEnd = oxEnd;
      yStart = oyStart;
      yEnd = oyEnd;
      zStart = ozStart;
      zEnd = ozEnd;

      // handle Log options passing via Keywords
      // note: undocumented keywords [xyz]type still exist and
      // have priority on [xyz]log !
      static int xTypeIx = e->KeywordIx("XTYPE");
      static int yTypeIx = e->KeywordIx("YTYPE");
      static int xLogIx = e->KeywordIx("XLOG");
      static int yLogIx = e->KeywordIx("YLOG");
      static int xTickunitsIx = e->KeywordIx("XTICKUNITS");
      static int yTickunitsIx = e->KeywordIx("YTICKUNITS");

      if (e->KeywordPresent(xTypeIx)) xLog = e->KeywordSet(xTypeIx);
      if (e->KeywordPresent(yTypeIx)) yLog = e->KeywordSet(yTypeIx);

      if (xLog && e->KeywordSet(xTickunitsIx)) {
        Message("PLOT: LOG setting ignored for Date/Time TICKUNITS.");
        xLog = FALSE;
      }
      if (yLog && e->KeywordSet(yTickunitsIx)) {
        Message("PLOT: LOG setting ignored for Date/Time TICKUNITS.");
        yLog = FALSE;
      }

      //XRANGE and YRANGE overrides all that, but  Start/End should be recomputed accordingly
      DDouble xAxisStart, xAxisEnd, yAxisStart, yAxisEnd, zAxisStart, zAxisEnd;
      bool setx = gdlGetDesiredAxisRange(e, XAXIS, xAxisStart, xAxisEnd);
      bool sety = gdlGetDesiredAxisRange(e, YAXIS, yAxisStart, yAxisEnd);
      bool setz = gdlGetDesiredAxisRange(e, ZAXIS, zAxisStart, zAxisEnd);
      if (sety) {
        yStart = yAxisStart;
        yEnd = yAxisEnd;
      }
      if (setx) {
        xStart = xAxisStart;
        xEnd = xAxisEnd;
      }
      if (setz) {
        zStart = zAxisStart;
        zEnd = zAxisEnd;
      }
      //handle Nozero option after all that!
      if (!gdlYaxisNoZero(e) && yStart > 0 && !yLog) yStart = 0.0;

      //Box adjustement:
      gdlAdjustAxisRange(e, XAXIS, xStart, xEnd, xLog);
      gdlAdjustAxisRange(e, YAXIS, yStart, yEnd, yLog);
      gdlAdjustAxisRange(e, ZAXIS, zStart, zEnd, zLog);

      xPos = (standardNumPos) ? oxStart : oxEnd;
      yPos = (standardNumPos) ? oyStart : oyEnd;
      zPos = (standardNumPos) ? ozStart : ozEnd;
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

      return false; //do not abort
    }

  private:

    bool old_body(EnvT* e, GDLGStream* actStream) {
      gdlSetGraphicsForegroundColorFromKw(e, actStream); //COLOR
      //    contrary to the documentation axis does not erase the plot (fortunately!)
      //    gdlNextPlotHandlingNoEraseOption(e, actStream, true);     //NOERASE -- not supported
      PLFLT vpXL, vpXR, vpYB, vpYT; //define new viewport in relative units
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
      SelfConvertToNormXY(1, &xPos, xAxisWasLog , &yPos, yAxisWasLog, coordinateSystem);

      //compute new temporary viewport in relative coords
      if (standardNumPos) {
        vpXL = (xAxis || xnodef) ? xnormmin : xPos;
        vpXR = (xAxis || xnodef) ? xnormmax : xPos + viewportXSize;
        vpYB = (yAxis || ynodef) ? ynormmin : yPos;
        vpYT = (yAxis || ynodef) ? ynormmax : yPos + viewportYSize;
      } else {
        vpXL = (xAxis || xnodef) ? xnormmin : xPos - viewportXSize;
        vpXR = (xAxis || xnodef) ? xnormmax : xPos;
        vpYB = (yAxis || ynodef) ? ynormmin : yPos - viewportYSize;
        vpYT = (yAxis || ynodef) ? ynormmax : yPos;
      }

      actStream->OnePageSaveLayout(); // one page

      actStream->vpor(vpXL, vpXR, vpYB, vpYT);
      if (xLog) {
        xStart = log10(xStart);
        xEnd = log10(xEnd);
      }
      if (yLog) {
        yStart = log10(yStart);
        yEnd = log10(yEnd);
      }
      if (zLog) {
        zStart = log10(zStart);
        zEnd = log10(zEnd);
      }
      //insure 'wind' arguments are 
      actStream->wind(xStart, xEnd, yStart, yEnd);

      if (doT3d) { //call for driver to perform special transform for all further drawing
        gdlFillWithT3DMatrix(PlotDevice3d.T);
        PlotDevice3d.zValue = zValue;
        actStream->cmd(PLESC_3D, &PlotDevice3d);
      }

      static int SAVEIx = e->KeywordIx("SAVE");
      bool doSave = e->KeywordSet(SAVEIx);

      if (xAxis) { //special ID "XAXIS2" needed because we artificially changed size of box
        gdlAxis(e, actStream, XAXIS2, xStart, xEnd, xLog, standardNumPos ? 1 : 2, viewportYSize);

        if (doSave) {
          gdlStoreAxisCRANGE(XAXIS, xStart, xEnd, xLog);
          gdlStoreAxisType(XAXIS, xLog);
          gdlStoreXAxisParameters(actStream, xStart, xEnd);
        }
      }

      if (yAxis) {//special id "YAXIS2" needed because we artificially changed size of box
        gdlAxis(e, actStream, YAXIS2, yStart, yEnd, yLog, standardNumPos ? 1 : 2, viewportXSize);

        if (doSave) {
          gdlStoreAxisCRANGE(YAXIS, yStart, yEnd, yLog);
          gdlStoreAxisType(YAXIS, yLog);
          gdlStoreYAxisParameters(actStream, yStart, yEnd);
        }
      }

      if (zAxis) {//special id "ZAXIS2" needed because we artificially changed size of box
        gdlAxis(e, actStream, ZAXIS2, yStart, yEnd, yLog, standardNumPos ? 1 : 2, viewportZSize);

        if (doSave) {
          gdlStoreAxisCRANGE(ZAXIS, zStart, zEnd, zLog);
          gdlStoreAxisType(ZAXIS, zLog);
          gdlStoreYAxisParameters(actStream, zStart, zEnd);
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

    void call_plplot(EnvT* e, GDLGStream* actStream) { }

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
