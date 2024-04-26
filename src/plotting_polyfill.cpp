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

  class polyfill_call : public plotting_routine_call {
    DDoubleGDL *xVal, *yVal, *zVal;
    Guard<BaseGDL> xval_guard, yval_guard, zval_guard;
    DDouble xStart, xEnd, yStart, yEnd, zStart, zEnd;
    DDouble zPosition;
    bool xLog, yLog, zLog;
    SizeT nEl;
    bool doClip;
    bool doT3d, flat3d;
    DLongGDL *color;
    bool mapSet;
    COORDSYS coordinateSystem = DATA;

  private:

    bool handle_args(EnvT* e) {
      //for cases where 3D is enabled, but z is not defined (since zVal is not an argument of PLOTS() )
      DFloat * position = gdlGetRegion();
      DDoubleGDL* zInit = new DDoubleGDL(position[4]);
      Guard<BaseGDL> zinit_guard(zInit);

      //3 parameters max, may be null, so test them.
      SizeT nPar = e->NParam(1);
      BaseGDL* p0 = e->GetPar(0);
      BaseGDL* p1 = e->GetPar(1);
      BaseGDL* p2 = e->GetPar(2);
      if (p0 == NULL) e->Throw("Variable is undefined: " + e->GetParString(0));
      if (nPar >= 2 && p1 == NULL) e->Throw("Variable is undefined: " + e->GetParString(1));
      if (nPar == 3 && p2 == NULL) e->Throw("Variable is undefined: " + e->GetParString(2));

      //T3D
      static int t3dIx = e->KeywordIx("T3D");
      doT3d = (e->BooleanKeywordSet(t3dIx) || T3Denabled());
      flat3d = doT3d; //by default

      //note: Z (VALUE) will be used uniquely if Z is not effectively defined.
      // Then Z is useful only if (doT3d).
      static int zvIx = e->KeywordIx("Z");
      zPosition = 0.0; //it is NOT a zValue.
      if (e->KeywordPresent(zvIx)) {
        e->AssureDoubleScalarKWIfPresent(zvIx, zPosition);
        //norm directly here, we are in 3D mode
        DDouble *sz;
        GetSFromPlotStructs(NULL, NULL, &sz);
        zPosition = zPosition * sz[1] + sz[0];
      }
      
      //LOG nexcessary
      gdlGetAxisType(XAXIS, xLog);
      gdlGetAxisType(YAXIS, yLog);
      //check presence of DATA,DEVICE and NORMAL options
      static int DATAIx = e->KeywordIx("DATA");
      static int DEVICEIx = e->KeywordIx("DEVICE");
      static int NORMALIx = e->KeywordIx("NORMAL");
      coordinateSystem = DATA;
      //check presence of DATA,DEVICE and NORMAL options
      if (e->KeywordSet(DATAIx)) coordinateSystem = DATA;
      if (e->KeywordSet(DEVICEIx)) {coordinateSystem = DEVICE; doT3d=false; flat3d=false;}
      if (e->KeywordSet(NORMALIx)) coordinateSystem = NORMAL;
      
      if (nPar == 1) {
        SizeT dim0 = p0->Dim(0);
        if (dim0 < 2 || dim0 > 3) e->Throw("When only 1 param, dims must be (2,n) or (3,n)");
        if (p0->Dim(1) < 3) e->Throw("Not enough valid and unique points specified.");

        DDoubleGDL *val = e->GetParAs< DDoubleGDL>(0);
        nEl = p0->N_Elements() / dim0;
        xVal = new DDoubleGDL(dimension(nEl), BaseGDL::NOZERO);
        xval_guard.Reset(xVal); // delete upon exit

        yVal = new DDoubleGDL(dimension(nEl), BaseGDL::NOZERO);
        yval_guard.Reset(yVal); // delete upon exit

        for (SizeT i = 0; i < nEl; i++) {
          (*xVal)[i] = (*val)[dim0 * i];
          (*yVal)[i] = (*val)[dim0 * i + 1];
        }

        zVal = zInit->New(dimension(nEl), BaseGDL::INIT); //inherits current Z.WINDOW[0]
        zval_guard.Reset(zVal); // delete upon exit
        if (dim0 == 3 && doT3d) {
          for (SizeT i = 0; i < nEl; i++) (*zVal)[i] = (*val)[dim0 * i + 2];
          flat3d = false;
        }
      }//behaviour: if x or y are not an array, they are repeated to match minEl
        //if x or y have less elements than s, minEl is max(x,y) else minEl is size(s)
        //z ignored unless T3D is given or !P.T3D not 0
      else if (nPar == 2 || (nPar == 3 && !doT3d)) {
        xVal = e->GetWriteableParAs< DDoubleGDL>(0);
        SizeT xEl = xVal->N_Elements();

        yVal = e->GetWriteableParAs< DDoubleGDL>(1);
        SizeT yEl = yVal->N_Elements();

        nEl = (xEl > yEl) ? xEl : yEl;

        if (nEl > 1) { //there is at least one non-single
          SizeT maxEl = nEl;
          SizeT minEl = (xVal->Dim(0) == 0) ? maxEl : ((xEl < maxEl) ? xEl : maxEl);
          minEl = (yVal->Dim(0) == 0) ? minEl : ((yEl < minEl) ? yEl : minEl);
          nEl = minEl;
          //replicate singletons if any
          if (xVal->Dim(0) == 0) {
            DDoubleGDL* tmpxVal = e->GetParAs< DDoubleGDL>(0);
            xVal = new DDoubleGDL(nEl, BaseGDL::NOZERO);
            xval_guard.Reset(xVal); // delete upon exit
            for (SizeT i = 0; i < nEl; ++i) (*xVal)[i] = (*tmpxVal)[0];
          }
          if (yVal->Dim(0) == 0) {
            DDoubleGDL* tmpyVal = e->GetParAs< DDoubleGDL>(1);
            yVal = new DDoubleGDL(nEl, BaseGDL::NOZERO);
            yval_guard.Reset(yVal); // delete upon exit
            for (SizeT i = 0; i < nEl; ++i) (*yVal)[i] = (*tmpyVal)[0];
          }
        }
        zVal = zInit->New(dimension(nEl), BaseGDL::INIT); //inherits current Z.WINDOW[0]
        zval_guard.Reset(zVal); // delete upon exit
      } else if (nPar == 3) // here we have doT3d 
      {
        flat3d = false;

        xVal = e->GetWriteableParAs< DDoubleGDL>(0);
        SizeT xEl = xVal->N_Elements();

        yVal = e->GetWriteableParAs< DDoubleGDL>(1);
        SizeT yEl = yVal->N_Elements();

        zVal = e->GetWriteableParAs< DDoubleGDL>(2);
        SizeT zEl = zVal->N_Elements();

        nEl = (xEl > yEl) ? xEl : yEl;
        nEl = (nEl > zEl) ? nEl : zEl;

        if (nEl > 1) { //there is at least one non-single
          SizeT maxEl = nEl;
          SizeT minEl = (xVal->Dim(0) == 0) ? maxEl : ((xEl < maxEl) ? xEl : maxEl);
          minEl = (yVal->Dim(0) == 0) ? minEl : ((yEl < minEl) ? yEl : minEl);
          minEl = (zVal->Dim(0) == 0) ? minEl : ((zEl < minEl) ? zEl : minEl);
          nEl = minEl;

          //replicate singletons.
          if (xVal->Dim(0) == 0) {
            DDoubleGDL* tmpxVal = e->GetParAs< DDoubleGDL>(0);
            xVal = new DDoubleGDL(nEl, BaseGDL::NOZERO);
            xval_guard.Reset(xVal); // delete upon exit
            for (SizeT i = 0; i < nEl; ++i) (*xVal)[i] = (*tmpxVal)[0];
          }
          if (yVal->Dim(0) == 0) {
            DDoubleGDL* tmpyVal = e->GetParAs< DDoubleGDL>(1);
            yVal = new DDoubleGDL(nEl, BaseGDL::NOZERO);
            yval_guard.Reset(yVal); // delete upon exit
            for (SizeT i = 0; i < nEl; ++i) (*yVal)[i] = (*tmpyVal)[0];
          }
          if (zVal->Dim(0) == 0) { //must give the same behaviuor as if z=... was passed.
            //for safety we populate zVal
            DDoubleGDL* tmpzVal = e->GetParAs< DDoubleGDL>(2);
            zVal = new DDoubleGDL(nEl, BaseGDL::NOZERO);
            zval_guard.Reset(zVal); // delete upon exit
            for (SizeT i = 0; i < nEl; ++i) (*zVal)[i] = (*tmpzVal)[0];
            //but if fact this is the equivalent of z=.. :
            flat3d = true;
            //norm directly here, we are in 3D mode
            DDouble *sz;
            GetSFromPlotStructs(NULL, NULL, &sz);
            zPosition = (*tmpzVal)[0] * sz[1] + sz[0];
          }
        }
      }

      return false; //do not abort
    }

    bool prepareDrawArea(EnvT* e, GDLGStream* actStream) {
      //box defined in previous PLOT command, we pass in normalized coordinates w/clipping if needed 
      if (gdlSwitchToClippedNormalizedCoordinates(e, actStream, true)) return true; //inverted clip meaning
      return false; //do not abort
    }

    void applyGraphics(EnvT* e, GDLGStream* actStream) {

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
      }
      gdlSetPenThickness(e, actStream); //THICK
      gdlSetLineStyle(e, actStream); //LINESTYLE
      
      // LINE_FILL, SPACING, LINESTYLE, ORIENTATION, THICK old code: should be put in line with CONTOUR code (FIXME)
      static int linestyleIx = e->KeywordIx("LINESTYLE");
      static int orientationIx = e->KeywordIx("ORIENTATION");
      static int line_fillIx = e->KeywordIx("LINE_FILL");
      static int spacingIx = e->KeywordIx("SPACING");
      bool hachures=(e->KeywordSet(line_fillIx)||e->KeywordSet(orientationIx)||e->KeywordSet(spacingIx)||e->KeywordSet(linestyleIx));
      if (hachures) {
        PLINT ori = 0, spa = 1500;

        if (e->KeywordSet(orientationIx)) ori = PLINT(1e1 * (*e->GetKWAs<DFloatGDL>(orientationIx))[0]);
        if (e->KeywordSet(spacingIx)) spa = PLINT(1e4 * (*e->GetKWAs<DFloatGDL>(spacingIx))[0]);

        gdlSetPenThickness(e, actStream);
        gdlSetLineStyle(e, actStream);

        actStream->psty(8);
        actStream->pat(1, &ori, &spa);
      } else {
        actStream->psty(0);
      }
      
     //Take care of projections: 
      //projections: X & Y to be converted to u,v BEFORE plotting in NORM coordinates
      mapSet = false;
      get_mapset(mapSet);
      mapSet = (mapSet && coordinateSystem == DATA);

      if (mapSet) {
#ifdef USE_LIBPROJ
        ref = map_init();
        if (ref == NULL) e->Throw("Projection initialization failed.");

        //everything goes through map transformation, including cuts at horizon, then conversion to normalized coordinates (and eventually stransform (3Dprojection)  when plotted)
        if (flat3d) actStream->stransform(PDotTTransformXYZval, &zPosition); //3D projection will be done at plplot level

        DLongGDL *conn = NULL; //tricky as xVal and yVal will be probably replaced by connectivity
        bool doFill = true;
        bool doLines = false;
        bool isRadians = false;
        //if doT3d and !flat3d, the projected polygon needs to keep track of Z.
        DDoubleGDL *lonlat = GDLgrGetProjectPolygon(actStream, ref, NULL, xVal, yVal, zVal, isRadians, doFill, doLines, conn);

        //lonlat is still in radians.
        //GDLgrPlotProjectedPolygon or draw_polyline() will make the 3d projection if flat3d=true through the use of stransform()
        //if doT3d and !flat3d, we need to apply the 3D rotation ourselves:

        if (lonlat != NULL) {
          if (doT3d && !flat3d) {
            SelfPDotTTransformProjectedPolygonTable(lonlat); //lonlat 3D is now projected 2D  
          } else SelfNormLonLat(lonlat); //lonlat is now converted to norm
          GDLgrPlotProjectedPolygon(actStream, lonlat, doFill, conn);
          GDLDelete(lonlat);
          GDLDelete(conn); 
        }
#endif 
      } else { //just as if LIBPROJ WAS NOT present
        SelfConvertToNormXY(xVal, xLog, yVal, yLog, coordinateSystem); //DATA
        if (doT3d && !flat3d) {
         SelfConvertToNormXYZ(xVal, xLog, yVal, yLog, zVal, zLog, coordinateSystem);
          SelfPDotTTransformXYZ(xVal, yVal, zVal);
          actStream->fill(nEl, static_cast<PLFLT*> (&(*xVal)[0]), static_cast<PLFLT*> (&(*yVal)[0])); //draw_polyline(actStream, xVal, yVal, 0.0, 0.0, false, xLog, yLog, psym, append, doColor ? color : NULL);
        } else {
          if (flat3d) actStream->stransform(PDotTTransformXYZval, &zPosition);
          SelfConvertToNormXY(xVal, xLog, yVal, yLog, coordinateSystem); //DATA
          actStream->fill(nEl, static_cast<PLFLT*> (&(*xVal)[0]), static_cast<PLFLT*> (&(*yVal)[0])); //draw_polyline(actStream, xVal, yVal, 0.0, 0.0, false, xLog, yLog, psym, append, doColor ? color : NULL);
        }
      }
    } //end of call_plplot

    virtual void post_call(EnvT*, GDLGStream *actStream) {
     actStream->stransform(NULL, NULL);
     gdlStop3DDriverTransform(actStream);
     actStream->lsty(1); //reset linestyle
      actStream->psty(0); //reset fill
    }

  };

  void polyfill(EnvT* e) {
    polyfill_call polyfill;
    polyfill.call(e, 1);
  }

} // namespace
