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

  class oplot_call : public plotting_routine_call {
    DDoubleGDL *yVal, *xVal, *xTemp, *yTemp;
    SizeT xEl, yEl, zEl, nEl;
    DDouble xStart, xEnd, yStart, yEnd, zValue, zStart, zEnd;
    Guard<BaseGDL> xval_guard, yval_guard, xtemp_guard;
    bool doT3d;
    DLongGDL *color;
    DLong psym;

  private:

    bool handle_args(EnvT* e) {
      //T3D?
      static int t3dIx = e->KeywordIx("T3D");
      doT3d = (e->BooleanKeywordSet(t3dIx) || T3Denabled());

      //note: Z (VALUE) will be used uniquely if Z is not effectively defined.
      static int zvIx = e->KeywordIx("ZVALUE");
      zValue = 0.0;
      if (doT3d) {
        e->AssureDoubleScalarKWIfPresent(zvIx, zValue);
        zValue = min(zValue, ZVALUEMAX); //to avoid problems with plplot
        zValue = max(zValue, 0.0);
      }
      // system variable !P.NSUM first
      DLong nsum = (*static_cast<DLongGDL*> (SysVar::P()-> GetTag(SysVar::P()->Desc()->TagIndex("NSUM"), 0)))[0];
      static int NSUMIx = e->KeywordIx("NSUM");
      e->AssureLongScalarKWIfPresent(NSUMIx, nsum);

      static int polarIx = e->KeywordIx("POLAR");
      bool polar = (e->KeywordSet(polarIx));

      //test and transform eventually if POLAR and/or NSUM!
      if (nParam() == 1) {
        yTemp = e->GetWriteableParAs< DDoubleGDL>(0);
        if (yTemp->Rank() == 0)
          e->Throw("Expression must be an array in this context: " + e->GetParString(0));
        yEl = yTemp->N_Elements();
        xEl = yEl;
        xTemp = new DDoubleGDL(dimension(xEl), BaseGDL::INDGEN);
        xtemp_guard.Reset(xTemp); // delete upon exit
      } else {
        xTemp = e->GetWriteableParAs< DDoubleGDL>(0);
        if (xTemp->Rank() == 0)
          e->Throw("Expression must be an array in this context: " + e->GetParString(0));
        xEl = xTemp->N_Elements();
        yTemp = e->GetWriteableParAs< DDoubleGDL>(1);
        if (yTemp->Rank() == 0)
          e->Throw("Expression must be an array in this context: " + e->GetParString(1));
        yEl = yTemp->N_Elements();
        //silently drop unmatched values
        if (yEl != xEl) {
          SizeT size;
          size = min(xEl, yEl);
          xEl = size;
          yEl = size;
        }
      }

      static int MIN_VALUEIx = e->KeywordIx("MIN_VALUE");
      static int MAX_VALUEIx = e->KeywordIx("MAX_VALUE");
      bool doMin = e->KeywordPresentAndDefined(MIN_VALUEIx);
      bool doMax = e->KeywordPresentAndDefined(MAX_VALUEIx);

      //check nsum validity
      nsum = max(1, nsum);
      nsum = min(nsum, (DLong) xEl);

      if (nsum == 1) {
        if (polar) {
          xVal = new DDoubleGDL(dimension(xEl), BaseGDL::NOZERO);
          xval_guard.Reset(xVal); // delete upon exit
          yVal = new DDoubleGDL(dimension(yEl), BaseGDL::NOZERO);
          yval_guard.Reset(yVal); // delete upon exit
          for (int i = 0; i < xEl; i++) (*xVal)[i] = (*xTemp)[i] * cos((*yTemp)[i]);
          for (int i = 0; i < yEl; i++) (*yVal)[i] = (*xTemp)[i] * sin((*yTemp)[i]);
        } else { //careful about previously set autopointers!
          if (nParam() == 1) xval_guard.Init(xtemp_guard.release());
          xVal = xTemp;
          yVal = yTemp;
        }
      } else {
        int i, j, k;
        DLong size = (DLong) xEl / nsum;
        xVal = new DDoubleGDL(size, BaseGDL::ZERO); //SHOULD BE ZERO, IS NOT!
        xval_guard.Reset(xVal); // delete upon exit
        yVal = new DDoubleGDL(size, BaseGDL::ZERO); //IDEM
        yval_guard.Reset(yVal); // delete upon exit
        for (i = 0, k = 0; i < size; i++) {
          (*xVal)[i] = 0.0;
          (*yVal)[i] = 0.0;
          for (j = 0; j < nsum; j++, k++) {
            (*xVal)[i] += (*xTemp)[k];
            (*yVal)[i] += (*yTemp)[k];
          }
        }
        for (i = 0; i < size; i++) (*xVal)[i] /= nsum;
        for (i = 0; i < size; i++) (*yVal)[i] /= nsum;

        if (polar) {
          DDouble x, y;
          for (i = 0; i < size; i++) {
            x = (*xVal)[i] * cos((*yVal)[i]);
            y = (*xVal)[i] * sin((*yVal)[i]);
            (*xVal)[i] = x;
            (*yVal)[i] = y;
          }
        }
      }
      
      nEl=xVal->N_Elements();
      if (doMin) { //remove data out of this range here, will be easier than doing it afterwards
        DDouble minVal;
        e->AssureDoubleScalarKWIfPresent(MIN_VALUEIx, minVal);
        for (auto i = 0; i < nEl; ++i) {
          if ((*yVal)[i] < minVal) (*yVal)[i] = std::numeric_limits<double>::quiet_NaN();
        }
      }
      if (doMax) { //remove data out of this range here, will be easier than doing it afterwards
        DDouble maxVal;
        e->AssureDoubleScalarKWIfPresent(MAX_VALUEIx, maxVal);
        for (auto i = 0; i < nEl; ++i) {
          if ((*yVal)[i] > maxVal) (*yVal)[i] = std::numeric_limits<double>::quiet_NaN();
        }
      }

      return false; //do not abort
    }

    bool prepareDrawArea(EnvT* e, GDLGStream* actStream) {
      //box defined in previous PLOT command, we pass in normalized coordinates w/clipping if needed 
      gdlSetSymsize(e, actStream); //set symsize BEFORE switching (TBC)
      gdlSwitchToClippedNormalizedCoordinates(e, actStream, false, false); //normal clip meaning but does not know about DATA, DEVICE and NORMAL
      return false; //do not abort
    }

    void applyGraphics(EnvT* e, GDLGStream* actStream) {
      // start drawing. Graphic Keywords accepted:CLIP(YES), COLOR(YES), LINESTYLE(YES), NOCLIP(YES),
      //                                          PSYM(YES), SYMSIZE(YES), T3D(YES), ZVALUE(YES)
      static int colorIx = e->KeywordIx("COLOR");
      bool doColor = false;
      if (e->GetKW(colorIx) != NULL) {
        color = e->GetKWAs<DLongGDL>(colorIx);
        doColor = true;
      }

      //properties
      if (!doColor || color->N_Elements() == 1) {
        //if no KW or only 1 color, no need to complicate things
        //at draw_polyline level!
        gdlSetGraphicsForegroundColorFromKw(e, actStream); //COLOR
        doColor = false;
      } else if (color->N_Elements() > 1) e->Throw("Expression must be a scalar or 1 element array in this context:"+ e->GetString(colorIx));

      gdlSetPenThickness(e, actStream); //THICK
      gdlSetLineStyle(e, actStream); //LINESTYLE
      gdlGetPsym(e, psym); //PSYM


      // reproject using P.T transformation in [0..1] cube during the actual plot using pltransform() (to reproject also the PSYMs is possible with plplot only if z = 0, using this trick :
      if (doT3d) actStream->stransform(PDotTTransformXYZval, &zValue);

      bool mapSet = false;
      get_mapset(mapSet);

      if (mapSet) {
#ifdef USE_LIBPROJ
        ref = map_init();
        if (ref == NULL) e->Throw("Projection initialization failed.");

        DLongGDL *conn = NULL; //tricky as xVal and yVal will be probably replaced by connectivity
        bool doFill = false;
        bool doLines = (psym < 1);
        bool isRadians = false;
        //if doT3d and !flat3d, the projected polygon needs to keep track of Z.
        DDoubleGDL *lonlat = GDLgrGetProjectPolygon(actStream, ref, NULL, xVal, yVal, NULL, isRadians, doFill, doLines, conn);

        //lonlat is still in radians.
        //GDLgrPlotProjectedPolygon or draw_polyline() will make the 3d projection if flat3d=true through the use of stransform()
        //if doT3d and !flat3d, we need to apply the 3D rotation ourselves:

        if (lonlat != NULL) {
          SelfNormLonLat(lonlat); //lonlat is now converted to norm
          if (psym < 1) { //lines must be specially explored
            GDLgrPlotProjectedPolygon(actStream, lonlat, doFill, conn);
            psym = -psym;
          } //now that lines are plotted, do the points:
          if (psym > 0) {
            SizeT npts = lonlat->Dim(0); //lonlat is [npts,2]
            //temporary create x and y to pass to draw_polyline. Not very efficient!
            DDoubleGDL* x = new DDoubleGDL(dimension(npts), BaseGDL::NOZERO);
            for (auto i = 0; i < npts; ++i) (*x)[i] = (*lonlat)[i];
            DDoubleGDL* y = new DDoubleGDL(dimension(npts), BaseGDL::NOZERO);
            for (auto i = 0; i < npts; ++i) (*y)[i] = (*lonlat)[i + npts];
            draw_polyline(actStream, x, y, psym, false, doColor ? color : NULL); //x and y are normed values.
            GDLDelete(x);
            GDLDelete(y);
          }
          GDLDelete(lonlat);
          if (doLines || doFill) GDLDelete(conn); //conn may be null if no line-drawing or fill was requested.
        }
#endif 
      } else { //just as if LIBPROJ WAS NOT present
        COORDSYS coordinateSystem = DATA;
        //LOG?
        bool xLog, yLog;
        gdlGetAxisType(XAXIS, xLog);
        gdlGetAxisType(YAXIS, yLog);
        SelfConvertToNormXY(xVal, xLog, yVal, yLog, coordinateSystem); //DATA
        draw_polyline(actStream, xVal, yVal, psym, false, doColor ? color : NULL); //x and y are normed values.
      }
    } //end of call_oplot

    virtual void post_call(EnvT*, GDLGStream* actStream) {
      actStream->stransform(NULL, NULL);
      actStream->lsty(1);
    }
  };

  void oplot(EnvT* e) {
    oplot_call oplot;
    oplot.call(e, 1);
  }

} // namespace
