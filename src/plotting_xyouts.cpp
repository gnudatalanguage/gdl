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

#define DPI (double)(4*atan(1.0))
#define DEGTORAD DPI/180.0

static GDL_3DTRANSFORMDEVICE PlotDevice3d;

namespace lib {

  using namespace std;

  static DDouble lastTextPosX = 0.0;
  static DDouble lastTextPosY = 0.0;

  class xyouts_call : public plotting_routine_call {
    DDoubleGDL *xVal, *yVal;
    Guard<BaseGDL> xval_guard, yval_guard, zval_guard;
    DDouble xStart, xEnd, yStart, yEnd, zStart, zEnd;

    DStringGDL* strVal;
    SizeT xEl, yEl, strEl;
    SizeT minEl;
    bool xLog, yLog;
    bool doClip, restoreClipBox;
    bool doT3d;
    bool singleArg;
    DDouble zPosition;

    COORDSYS coordinateSystem=DATA;


  private:

    void getTextPos(GDLGStream *a, DDouble &wx, DDouble &wy) {
      wx = lastTextPosX;
      wy = lastTextPosY;
      if (GDL_DEBUG_PLSTREAM) fprintf(stderr, "getTextPos: Got norm: %lf %lf\n", lastTextPosX, lastTextPosY);
    }

    bool handle_args(EnvT* e) {
      //T3D
      static int t3dIx = e->KeywordIx("T3D");
      doT3d = (e->BooleanKeywordSet(t3dIx) || T3Denabled());

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
      singleArg = false;
      if (nParam() == 1) {
        singleArg = true;
        //string only...
        xVal = new DDoubleGDL(1, BaseGDL::ZERO);
        xval_guard.Reset(xVal); // delete upon exit
        yVal = new DDoubleGDL(1, BaseGDL::ZERO);
        yval_guard.Reset(yVal); // delete upon exit
        xEl = yEl = xVal->N_Elements();
        strVal = e->GetParAs<DStringGDL>(0);
        strEl = strVal->N_Elements();
        minEl = strEl; //in this case only
      } else if (nParam() == 3) {
        xVal = e->GetParAs< DDoubleGDL>(0);
        xEl = xVal->N_Elements();
        yVal = e->GetParAs< DDoubleGDL>(1);
        yEl = yVal->N_Elements();
        strVal = e->GetParAs<DStringGDL>(2);
        strEl = strVal->N_Elements();
        //behaviour: if x or y are not an array, they are repeated to match minEl
        //if s is not a vector, minel is min(xel,yel) and S is repeated.
        //if x or y have less elements than s, minEl is max(x,y) else minEl is size(s)
        //z will be set at Zero unless Z=value is given
        if ((xVal->Dim(0) == 0) && (yVal->Dim(0) == 0)) {
          minEl = strEl;
          DDoubleGDL* tmpxVal = e->GetParAs< DDoubleGDL>(0);
          xVal = new DDoubleGDL(minEl, BaseGDL::NOZERO); //should remove previous xVal if allocated -- fixme.
          xval_guard.Reset(xVal); // delete upon exit
          for (SizeT i = 0; i < minEl; ++i) (*xVal)[i] = (*tmpxVal)[0];
          DDoubleGDL* tmpyVal = e->GetParAs< DDoubleGDL>(1);
          yVal = new DDoubleGDL(minEl, BaseGDL::NOZERO); //idem and below
          yval_guard.Reset(yVal); // delete upon exit
          for (SizeT i = 0; i < minEl; ++i) (*yVal)[i] = (*tmpyVal)[0];
        } else if (xVal->Dim(0) == 0) {
          if (strEl == 1) minEl = yEl;
          else minEl = (yEl < strEl) ? yEl : strEl;
          DDoubleGDL* tmpxVal = e->GetParAs< DDoubleGDL>(0);
          xVal = new DDoubleGDL(minEl, BaseGDL::NOZERO);
          xval_guard.Reset(xVal); // delete upon exit
          for (SizeT i = 0; i < minEl; ++i) (*xVal)[i] = (*tmpxVal)[0];
        } else if (yVal->Dim(0) == 0) {
          if (strEl == 1) minEl = xEl;
          else minEl = (xEl < strEl) ? xEl : strEl;
          DDoubleGDL* tmpyVal = e->GetParAs< DDoubleGDL>(1);
          yVal = new DDoubleGDL(minEl, BaseGDL::NOZERO);
          yval_guard.Reset(yVal); // delete upon exit
          for (SizeT i = 0; i < minEl; ++i) (*yVal)[i] = (*tmpyVal)[0];
        } else {
          minEl = (xEl < yEl) ? xEl : yEl;
        }
      } else {
        e->Throw("Not enough parameters. Either 1 parameter or 3 "
          "parameters valid.");
      }

      return false; //do not abort
    }

    bool prepareDrawArea(EnvT* e, GDLGStream* actStream) {


      // get_axis_type
      gdlGetAxisType(XAXIS, xLog);
      gdlGetAxisType(YAXIS, yLog);

      //get DATA limits (not necessary CRANGE, see AXIS / SAVE behaviour!)
      GetCurrentUserLimits(xStart, xEnd, yStart, yEnd, zStart, zEnd);
      
      //box plotted, we pass in normalized coordinates w/clipping if needed 
      gdlSwitchToClippedNormalizedCoordinates(e, actStream);

      SelfProjectXY(minEl, (DDouble*) xVal->DataAddr(), (DDouble*) yVal->DataAddr(), coordinateSystem);
      //input coordinates converted to NORMAL
      SelfConvertToNormXY(xVal, xLog, yVal, yLog, coordinateSystem); //DATA
      
      return false;
    }

    void applyGraphics(EnvT* e, GDLGStream* actStream) {

      PLFLT stringWidth = 0.;

      DFloatGDL  *orientation, *charthick, *alignement, *size;
      Guard<BaseGDL> alignement_guard, orientation_guard, size_guard;

      // WIDTH keyword (read, write)
      static int widthIx = e->KeywordIx("WIDTH");
      bool returnWidth = e->KeywordPresent(widthIx);

      PLFLT aspectw, aspectd;
      aspectw = actStream->boxAspectWorld();
      aspectd = actStream->boxAspectDevice();

      static int colorIx = e->KeywordIx("COLOR");
      bool docolor = false;
      static int charthickIx = e->KeywordIx("CHARTHICK");
      bool docharthick = false;
      static int charsizeIx = e->KeywordIx("CHARSIZE");
      bool docharsize = false;

      DLongGDL *color;
      if (e->GetKW(colorIx) != NULL) {
        color = e->GetKWAs<DLongGDL>(colorIx);
        docolor = true;
      }
      if (e->GetKW(charthickIx) != NULL) {
        charthick = e->GetKWAs<DFloatGDL>(charthickIx);
        docharthick = true;
      }
      if (e->GetKW(charsizeIx) != NULL) {
        size = e->GetKWAs<DFloatGDL>(charsizeIx);
        docharsize = true;
      } else //for security in future conditional evaluation...
      {
        size = new DFloatGDL(dimension(1), BaseGDL::ZERO);
        size_guard.Init(size);
        (*size)[0] = 1.0;
      }
      static int orientationIx = e->KeywordIx("ORIENTATION");
      if (e->GetKW(orientationIx) != NULL) {
        orientation = e->GetKWAs<DFloatGDL>(orientationIx);
      } else {
        orientation = new DFloatGDL(dimension(1), BaseGDL::ZERO);
        orientation_guard.Init(orientation);
        (*orientation)[0] = 0;
      }
      static int alignIx = e->KeywordIx("ALIGNMENT");
      if (e->GetKW(alignIx) != NULL) {
        alignement = e->GetKWAs<DFloatGDL>(alignIx);
      } else {
        alignement = new DFloatGDL(dimension(1), BaseGDL::ZERO);
        alignement_guard.Init(alignement);
        (*alignement)[0] = 0;
      }

      // *** start drawing by defalut values
      gdlSetGraphicsForegroundColorFromKw(e, actStream);
      if (!docharthick) gdlSetPlotCharthick(e, actStream);
      if (!docharsize) gdlSetPlotCharsize(e, actStream, true); //accept SIZE kw!

      // Get decomposed value for colors
      DLong decomposed = GraphicsDevice::GetDevice()->GetDecomposed();

      DDouble *sx, *sy, *sz;
      GetSFromPlotStructs(&sx, &sy, &sz); 
      
      if (doT3d) { //call for driver to perform special transform for all further drawing
        gdlGetT3DMatrixForDriverTransform(PlotDevice3d.T);
        PlotDevice3d.zValue = zPosition;
        actStream->cmd(PLESC_3D, &PlotDevice3d);
      }
            for (SizeT i = 0; i < minEl; ++i) {
        //if string only, fill empty Xval Yval with current value:
        if (nParam() == 1) {
          DDouble s, t;
          getTextPos(actStream, s, t);
          (*xVal)[0] = s;
          (*yVal)[0] = t;
        }
        PLFLT x = static_cast<PLFLT> ((*xVal)[i % xVal->N_Elements()]); //insure even 1 parameter, string array
        PLFLT y = static_cast<PLFLT> ((*yVal)[i % xVal->N_Elements()]);

        if (!isfinite(x) || !isfinite(y)) continue; //no plot
        if (docharsize && (*size)[i % size->N_Elements()] < 0) continue; //no plot either

        if (docharsize) actStream->sizeChar((*size)[i % size->N_Elements()]);
        if (docolor) actStream->Color((*color)[i % color->N_Elements()], decomposed);
        if (docharthick) actStream->Thick((*charthick)[i % charthick->N_Elements()]);

        //orientation word is not orientation page depending on axes increment direction [0..1] vs. [1..0]
        PLFLT oriD = ((*orientation)[i % orientation->N_Elements()]); //ori DEVICE
        PLFLT oriW = oriD; //ori WORLD
        oriD *= DEGTORAD;
        if (sx[1] < 0) oriW = 180.0 - oriW;
        if (sy[1] < 0) oriW *= -1;
        oriW *= DEGTORAD;

        PLFLT cosOriD = cos(oriD);
        PLFLT sinOriD = sin(oriD);
        PLFLT cosOriW = cos(oriW);
        PLFLT sinOriW = sin(oriW);

        PLFLT align = (*alignement)[i % alignement->N_Elements()];
        align = max(align, 0.0);
        align = min(align, 1.0);
        
        PLFLT ptex_x, ptex_y, chsize, device_x, device_y;
        // displacement due to offset (reference in IDL is baseline,
        // in plplot it's the half-height) is best computed in device coords
        chsize = actStream->dCharHeight()*0.5;
        actStream->NormedDeviceToDevice(x, y, device_x, device_y);
        actStream->DeviceToNormedDevice(device_x - chsize*sinOriD, device_y + chsize*cosOriD, ptex_x, ptex_y);
     
        string out = (*strVal)[i % strVal->N_Elements()];
        double returnedStringCharLength;
        actStream->ptex(ptex_x, ptex_y, cosOriW, sinOriW * aspectw / aspectd, align, out.c_str(), &returnedStringCharLength);


        if (singleArg || (i == minEl - 1)) //then x and y are not given and whatever the number of strings, are retrieved
          // from lastTextPos. We must thus remember lastTextPos.
        {
          //we want normed size:
          stringWidth = returnedStringCharLength * actStream->nCharLength();
          //save position - compute must be in DEVICE coords, or in normed*aspect!
          lastTextPosX=x + (1.0 - align) * stringWidth*cosOriD;
          lastTextPosY=y + (1.0 - align) * stringWidth * sinOriD / aspectd;
        }
      }

      if (returnWidth) {
        // width is in "normalized coordinates", and is the width of the last string written (in case of an array of strings)
        e->SetKW(widthIx, new DFloatGDL(stringWidth));
      }
    }

  private:

    virtual void post_call(EnvT* e, GDLGStream* actStream) {
      actStream->stransform(NULL, NULL);
      actStream->sizeChar(1.0);
    }
  };

  void xyouts(EnvT* e) {
    xyouts_call xyouts;
    xyouts.call(e, 1);
  }
} // namespace
