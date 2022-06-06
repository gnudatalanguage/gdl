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

  class plots_call : public plotting_routine_call {
    DDoubleGDL *xVal, *yVal, *zVal;
    Guard<BaseGDL> xval_guard, yval_guard, zval_guard;
    Guard<BaseGDL> xvalnative_guard, yvalnative_guard, zvalnative_guard;
    DDouble xStart, xEnd, yStart, yEnd, zStart, zEnd;
    DDouble zPosition;
    DLong psym;
    bool xLog, yLog, zLog;
    SizeT nEl;
    bool append;
    bool doClip;
    bool doT3d, flat3d;
    DLongGDL *color;
    bool mapSet;
    COORDSYS coordinateSystem = DATA;
    bool xnative, ynative, znative; //tell if xVal etc are a copy of the variables or the real thing. When the real thing, they should not be modified.

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

      xnative = false;
      ynative = false;
      znative = false;

      gdlGetPsym(e, psym); //PSYM
      if (psym == 10) e->Throw("PSYM (plotting symbol) out of range"); //not allowed for PLOTS!
      //T3D
      static int t3dIx = e->KeywordIx("T3D");
      doT3d = (e->BooleanKeywordSet(t3dIx) || T3Denabled());
      flat3d = doT3d; //by default

      //note: Z (VALUE) will be used uniquely if Z is not effectively defined.
      // Then Z is useful only if (doT3d).
      static int zvIx = e->KeywordIx("Z");
      zPosition = 0.0; //it is NOT a zValue.
      if (doT3d) {
        e->AssureDoubleScalarKWIfPresent(zvIx, zPosition);
        //norm directly here, we are in 3D mode
        DDouble *sx, *sy, *sz;
        GetSFromPlotStructs(&sx, &sy, &sz);
        zPosition = zPosition * sz[1] + sz[0];
      }

      static int continueIx = e->KeywordIx("CONTINUE");
      append = e->KeywordSet(continueIx);
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
      }        //behaviour: if x or y are not an array, they are repeated to match minEl
        //if x or y have less elements than s, minEl is max(x,y) else minEl is size(s)
        //z ignored unless T3D is given or !P.T3D not 0
      else if (nPar == 2 || (nPar == 3 && !doT3d)) {
        if (p0->Type() == GDL_DOUBLE) xnative = true;
        xVal = e->GetParAs< DDoubleGDL>(0);
        SizeT xEl = xVal->N_Elements();

        if (p1->Type() == GDL_DOUBLE) ynative = true;
        yVal = e->GetParAs< DDoubleGDL>(1);
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

        if (p0->Type() == GDL_DOUBLE) xnative = true;
        xVal = e->GetParAs< DDoubleGDL>(0);
        SizeT xEl = xVal->N_Elements();

        if (p1->Type() == GDL_DOUBLE) ynative = true;
        yVal = e->GetParAs< DDoubleGDL>(1);
        SizeT yEl = yVal->N_Elements();

        if (p2->Type() == GDL_DOUBLE) znative = true;
        zVal = e->GetParAs< DDoubleGDL>(2);
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
      //in all cases, we need to replace the native arrays by a copy as they will be converted to normed values
      if (xnative) {
        xVal = xVal->Dup();
        xvalnative_guard.Reset(xVal);
      }
      if (ynative) {
        yVal = yVal->Dup();
        yvalnative_guard.Reset(yVal);
      }
      if (znative) {
        zVal = zVal->Dup();
        zvalnative_guard.Reset(zVal);
      }

      return false; //do not abort
    }

    bool old_body(EnvT* e, GDLGStream* actStream) {

      //check presence of DATA,DEVICE and NORMAL options
      static int DATAIx = e->KeywordIx("DATA");
      static int DEVICEIx = e->KeywordIx("DEVICE");
      static int NORMALIx = e->KeywordIx("NORMAL");
      coordinateSystem = DATA;
      //check presence of DATA,DEVICE and NORMAL options
      if (e->KeywordSet(DATAIx)) coordinateSystem = DATA;
      if (e->KeywordSet(DEVICEIx)) {coordinateSystem = DEVICE; doT3d=false;}
      if (e->KeywordSet(NORMALIx)) coordinateSystem = NORMAL;

      // get_axis_type
      gdlGetAxisType(XAXIS, xLog);
      gdlGetAxisType(YAXIS, yLog);
      gdlGetAxisType(ZAXIS, zLog);

      //get DATA limits (not necessary CRANGE, see AXIS / SAVE behaviour!)
      GetCurrentUserLimits(actStream, xStart, xEnd, yStart, yEnd, zStart, zEnd);

      actStream->OnePageSaveLayout(); // one page

      //CLIPPING (or not) is just defining the adequate viewport and world coordinates, all of them normalized since this is what plplot will get in the end.
      static int NOCLIPIx = e->KeywordIx("NOCLIP");
      // Clipping is not enabled by default for PLOTS: noclip is true by default
      bool noclip = e->BooleanKeywordAbsentOrSet(NOCLIPIx);
      int CLIP = e->KeywordIx("CLIP");
      bool doClip = (e->KeywordSet(CLIP) && !(noclip) && !doT3d);

      PLFLT xnormmin = 0;
      PLFLT xnormmax = 1;
      PLFLT ynormmin = 0;
      PLFLT ynormmax = 1;

      if (doClip) { //redefine default viewport & world
        //define a default clipbox (DATA coords):
        PLFLT clipBox[4] = {xStart, yStart, xEnd, yEnd};
        DDoubleGDL* clipBoxGDL = e->IfDefGetKWAs<DDoubleGDL>(CLIP);
        if (clipBoxGDL != NULL && clipBoxGDL->N_Elements() < 4) for (auto i = 0; i < 4; ++i) clipBox[i] = 0; //set clipbox to 0 0 0 0 apparently this is what IDL does.
        if (clipBoxGDL != NULL && clipBoxGDL->N_Elements() == 4) for (auto i = 0; i < 4; ++i) clipBox[i] = (*clipBoxGDL)[i];
        //clipBox is defined accordingly to /NORM /DEVICE /DATA:
        //convert clipBox to normalized coordinates:
        switch (coordinateSystem) {
        case DATA:  //will know about projections
        {
          SelfProjectXY(1, &clipBox[0], &clipBox[1], coordinateSystem); //here for eventual projection
          SelfProjectXY(1, &clipBox[2], &clipBox[3], coordinateSystem); //here for eventual projection
          bool f = false;
          SelfConvertToNormXY(1, &clipBox[0], f, &clipBox[1], f, coordinateSystem); //input coordinates converted to NORMAL
          SelfConvertToNormXY(1, &clipBox[2], f, &clipBox[3], f, coordinateSystem); //input coordinates converted to NORMAL
          xnormmin=clipBox[0];ynormmin=clipBox[1];
          xnormmax=clipBox[2];ynormmax=clipBox[3];
          break;
        }
        case DEVICE:
          actStream->DeviceToNormedDevice(clipBox[0], clipBox[1], xnormmin, ynormmin);
          actStream->DeviceToNormedDevice(clipBox[2], clipBox[3], xnormmax, ynormmax);
          break;
        case NORMAL:
        default:
          xnormmin = clipBox[0];
          xnormmax = clipBox[2];
          ynormmin = clipBox[1];
          ynormmax = clipBox[3];
        }
      }

      if (xnormmin==xnormmax || ynormmin==ynormmax) {
        this->post_call(e,actStream);
        return true; //nothing to see and plpot complains.
      }
      // it is important to fix symsize before changing vpor or win 
      gdlSetSymsize(e, actStream); //SYMSIZE
      actStream->vpor(xnormmin, xnormmax, ynormmin, ynormmax);
      actStream->wind(xnormmin, xnormmax, ynormmin, ynormmax); //transformed (plotted) coords will be in NORM. Conversion will be made on the data values.
      actStream->setSymbolSizeConversionFactors();
      
      return false;
    }

    void call_plplot(EnvT* e, GDLGStream* actStream) {
      // start drawing. Graphic Keywords accepted: CLIP(YES), COLOR(OK), DATA(YES), DEVICE(YES),
      //LINESTYLE(OK), NOCLIP(YES), NORMAL(YES), PSYM(OK), SYMSIZE(OK), T3D(NO), THICK(OK), Z(NO)
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
      gdlSetLineStyle(e, actStream); //LINESTYLE
      gdlSetPenThickness(e, actStream); //THICK

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
        bool doFill = false;
        bool doLines = (psym < 1);
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
            draw_polyline(actStream, x, y, 0.0, 0.0, false, false, false, psym, append, doColor ? color : NULL);
            GDLDelete(x);
            GDLDelete(y);
          }
          GDLDelete(lonlat);
          if (doLines || doFill) GDLDelete(conn); //conn may be null if no line-drawing or fill was requested.
        }
#endif 
      } else { //just as if LIBPROJ WAS NOT present
        if (doT3d && !flat3d) {
          SelfConvertToNormXYZ(xVal, xLog, yVal, yLog, zVal, zLog, coordinateSystem);
          SelfPDotTTransformXYZ(xVal, yVal, zVal);
          draw_polyline(actStream, xVal, yVal, 0.0, 0.0, false, xLog, yLog, psym, append, doColor ? color : NULL);
        } else {
          if (flat3d) actStream->stransform(PDotTTransformXYZval, &zPosition);
          SelfConvertToNormXY(xVal, xLog, yVal, yLog, coordinateSystem); //DATA
          draw_polyline(actStream, xVal, yVal, 0.0, 0.0, false, xLog, yLog, psym, append, doColor ? color : NULL);
        }
      }
    } //end of call_plplot

    virtual void post_call(EnvT*, GDLGStream* actStream) {
      actStream->stransform(NULL, NULL);
      actStream->RestoreLayout();
      actStream->lsty(1);
    }

  };

  void plots(EnvT* e) {
    plots_call plots;
    plots.call(e, 1);
  }

} // namespace
