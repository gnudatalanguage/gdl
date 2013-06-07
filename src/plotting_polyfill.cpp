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
#include "math_utl.hpp"

namespace lib
{

  using namespace std;

  class polyfill_call: public plotting_routine_call
  {

  private:
    DDoubleGDL *xVal, *yVal, *zVal;
    Guard<BaseGDL> xval_guard, yval_guard, zval_guard;
    DDouble xStart, xEnd, yStart, yEnd, zStart, zEnd;
    DLong psym;
    bool xLog, yLog, zLog;
    SizeT xEl, yEl, zEl;
    bool append;
    bool doClip;
    bool restoreClipBox;
    PLFLT savebox[4];
    bool doT3d, real3d;
    DDouble zValue;
    DDoubleGDL* plplot3d;
    Guard<BaseGDL> plplot3d_guard;
//    DLongGDL *color;

  private:

    bool handle_args(EnvT* e) // {{{
    {
      real3d=false;
      //T3D
      static int t3dIx = e->KeywordIx( "T3D");
      doT3d=(e->KeywordSet(t3dIx)|| T3Denabled(e));

      //note: Z (VALUE) will be used uniquely if Z is not effectively defined.
      static int zvIx = e->KeywordIx( "Z");
      zValue=0.0;
      e->AssureDoubleScalarKWIfPresent ( zvIx, zValue );

      if ( nParam()==1 )
      {
        BaseGDL* p0;
        p0=e->GetParDefined(0);
        SizeT dim0=p0->Dim(0);
        if ( dim0<2 || dim0>3 )
          e->Throw("When only 1 param, dims must be (2,n) or (3,n)");
        if (p0->Dim(1) < 3 ) e->Throw("Not enough valid and unique points specified.");

        DDoubleGDL *val=e->GetParAs< DDoubleGDL>(0);
        xEl=p0->N_Elements()/dim0;
        xVal=new DDoubleGDL(dimension(xEl), BaseGDL::NOZERO);
        xval_guard.Reset(xVal); // delete upon exit

        yEl=p0->N_Elements()/dim0;
        yVal=new DDoubleGDL(dimension(yEl), BaseGDL::NOZERO);
        yval_guard.Reset(yVal); // delete upon exit

        for ( SizeT i=0; i<xEl; i++ )
        {
          (*xVal)[i]=(*val)[dim0*i];
          (*yVal)[i]=(*val)[dim0*i+1];
        }

        zEl=p0->N_Elements()/dim0;
        zVal=new DDoubleGDL(dimension(zEl), BaseGDL::NOZERO);
        zval_guard.Reset(zVal); // delete upon exit
        if (dim0==3) for ( SizeT i=0; i<zEl; i++ ) (*zVal)[i]=(*val)[dim0*i+2];
        else for (SizeT i=0; i< zEl ; ++i) (*zVal)[i]=zValue;
      }
      else if ( nParam()==2 )
      {
        xVal=e->GetParAs< DDoubleGDL>(0);
        xEl=xVal->N_Elements();

        yVal=e->GetParAs< DDoubleGDL>(1);
        yEl=yVal->N_Elements();

        if ( xEl < 3 || yEl < 3 ) e->Throw("Not enough valid and unique points specified.");

        //silently drop unmatched values
        if ( yEl!=xEl )
        {
          SizeT size;
          size=min(xEl, yEl);
          xEl=size;
          yEl=size;
        }
        //z will be set at Zero unless Z=value is given
        zEl=xEl;
        zVal=new DDoubleGDL(dimension(zEl));
        zval_guard.Reset(zVal); // delete upon exit
        for (SizeT i=0; i< zEl ; ++i) (*zVal)[i]=zValue;
      }
      else if ( nParam()==3 )
      {
        if (doT3d) real3d=true;
        zVal=e->GetParAs< DDoubleGDL>(2);
        zEl=zVal->N_Elements();

        xVal=e->GetParAs< DDoubleGDL>(0);
        xEl=xVal->N_Elements();

        yVal=e->GetParAs< DDoubleGDL>(1);
        yEl=yVal->N_Elements();

        if ( xEl < 3 || yEl < 3  || zEl < 3) e->Throw("Not enough valid and unique points specified.");

        if ( !(xEl==yEl&&yEl==zEl) )
        {
          SizeT size;
          size=min(xEl, yEl);
          size=min(size, zEl);
          xEl=size;
          yEl=size;
          zEl=size;
        }
      }
      return false;
//      return true;
    } // }}}

    void old_body(EnvT* e, GDLGStream* actStream) // {{{
    {
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

      // get_axis_type
      gdlGetAxisType("X", xLog);
      gdlGetAxisType("Y", yLog);
      gdlGetAxisType("Z", zLog);
      // get ![XY].CRANGE
      gdlGetCurrentAxisRange("X", xStart, xEnd);
      gdlGetCurrentAxisRange("Y", yStart, yEnd);
      gdlGetCurrentAxisRange("Z", zStart, zEnd);

      if ((yStart == yEnd) || (xStart == xEnd) || (zStart == zEnd))
      {
        if (zStart != 0.0 && zStart == zEnd)
          Message("POLYFILL: !Z.CRANGE ERROR, setting to [0,1]");
          zStart = 0;
          zEnd = 1;

        if (yStart != 0.0 && yStart == yEnd)
          Message("POLYFILL: !Y.CRANGE ERROR, setting to [0,1]");
        yStart = 0;
        yEnd = 1;

        if (xStart != 0.0 && xStart == xEnd)
          Message("POLYFILL: !X.CRANGE ERROR, setting to [0,1]");
        xStart = 0;
        xEnd = 1;
      }

      bool mapSet=false;
#ifdef USE_LIBPROJ4
      // Map Stuff (xtype = 3)

      get_mapset(mapSet);

      if ( mapSet )
      {
        ref=map_init();
        if ( ref==NULL )
        {
          e->Throw("Projection initialization failed.");
        }
      }
#endif


      PLFLT wun, wdeux, wtrois, wquatre;
      if ( coordinateSystem==DATA) //with POLYFILL, we can plot *outside* the box(e)s in DATA coordinates.
                                   // convert to device coords in this case
      {
        actStream->pageWorldCoordinates(wun, wdeux, wtrois, wquatre);
      }

      actStream->OnePageSaveLayout(); // one page

      actStream->vpor(0, 1, 0, 1);
      if ( coordinateSystem==DEVICE )
      {
        actStream->wind(0.0, actStream->xPageSize(), 0.0, actStream->yPageSize());
        xLog=false;
        yLog=false;
      }
      else if ( coordinateSystem==NORMAL )
      {
        actStream->wind(0, 1, 0, 1);
        xLog=false;
        yLog=false;
      }
      else //with POLYFILL, we can plot *outside* the box(e)s in DATA coordinates.
      {
        actStream->wind(wun, wdeux, wtrois, wquatre);
      }

    } // }}}

  private:

    void call_plplot(EnvT* e, GDLGStream* actStream) // {{{
    {
      static DDouble x0,y0,xs,ys; //conversion to normalized coords
      x0=(xLog)?-log10(xStart):-xStart;
      y0=(yLog)?-log10(yStart):-yStart;
      xs=(xLog)?(log10(xEnd)-log10(xStart)):xEnd-xStart;xs=1.0/xs;
      ys=(yLog)?(log10(yEnd)-log10(yStart)):yEnd-yStart;ys=1.0/ys;

      if ( doT3d && !real3d) { //if X,Y and Z are passed, we will use !P.T and not our plplot "interpretation" of !P.T
                               //if the x and y scaling is OK, using !P.T directly permits to use other projections
                               //than those used implicitly by plplot. See @showhaus example for *DL
        // case where we project 2D data on 3D: use plplot-like matrix.
        DDouble az, alt, ay, scale;
        ORIENTATION3D axisExchangeCode;

        plplot3d = gdlConvertT3DMatrixToPlplotRotationMatrix( zValue, az, alt, ay, scale, axisExchangeCode);
        if (plplot3d == NULL)
        {
          e->Throw("Illegal 3D transformation. (FIXME)");
        }

        Data3d.zValue = zValue;
        Data3d.Matrix = plplot3d; //try to change for !P.T in future?
        switch (axisExchangeCode) {
          case NORMAL: //X->X Y->Y plane XY
            Data3d.x0=x0;
            Data3d.y0=y0;
            Data3d.xs=xs;
            Data3d.ys=ys;
            Data3d.code = code012;
            break;
          case XY: // X->Y Y->X plane XY
            Data3d.x0=0;
            Data3d.y0=x0;
            Data3d.xs=ys;
            Data3d.ys=xs;
            Data3d.code = code102;
            break;
          case XZ: // Y->Y X->Z plane YZ
            Data3d.x0=x0;
            Data3d.y0=y0;
            Data3d.xs=xs;
            Data3d.ys=ys;
            Data3d.code = code210;
            break;
          case YZ: // X->X Y->Z plane XZ
            Data3d.x0=x0;
            Data3d.y0=y0;
            Data3d.xs=xs;
            Data3d.ys=ys;
            Data3d.code = code021;
            break;
          case XZXY: //X->Y Y->Z plane YZ
            Data3d.x0=x0;
            Data3d.y0=y0;
            Data3d.xs=xs;
            Data3d.ys=ys;
            Data3d.code = code120;
            break;
          case XZYZ: //X->Z Y->X plane XZ
            Data3d.x0=x0;
            Data3d.y0=y0;
            Data3d.xs=xs;
            Data3d.ys=ys;
            Data3d.code = code201;
            break;
        }
        actStream->stransform(gdl3dTo2dTransform, &Data3d);
      }
      //handle clipping
      bool doClip=(e->KeywordSet("CLIP")||e->KeywordSet("NOCLIP"));
      // make all clipping computations BEFORE setting graphic properties (color, size)
      bool stopClip=false;
      if ( doClip )  if ( startClipping(e, actStream, false)==TRUE ) stopClip=true;
      //properties
//      int colorIx=e->KeywordIx ( "COLOR" ); bool doColor=false;
//      if ( e->GetKW ( colorIx )!=NULL )
//      {
//        color=e->GetKWAs<DLongGDL>( colorIx ); doColor=true;
//      }

      // LINE_FILL, SPACING, LINESTYLE, ORIENTATION, THICK (thanks to JW)
      static int line_fillIx=e->KeywordIx("LINE_FILL");
      if ( e->KeywordSet(line_fillIx) )
      {
        PLINT ori=0, spa=1500;

        static int orientationIx=e->KeywordIx("ORIENTATION");
        if ( e->KeywordSet(orientationIx) ) ori=PLINT(1e1*(*e->GetKWAs<DFloatGDL>(orientationIx))[0]);
        static int spacingIx=e->KeywordIx("SPACING");
        if ( e->KeywordSet(spacingIx) ) spa=PLINT(1e4*(*e->GetKWAs<DFloatGDL>(spacingIx))[0]);

        gdlSetPenThickness(e, actStream);
        gdlSetLineStyle(e, actStream);

        actStream->psty(8);
        actStream->pat(1, &ori, &spa);
      }
      else
      {
        actStream->psty(0);
      }
      gdlSetGraphicsForegroundColorFromKw(e, actStream); //COLOR
      gdlSetLineStyle(e, actStream); //LINESTYLE
      gdlSetPenThickness(e, actStream); //THICK

      if (real3d)
      {
        //try first if the matrix is a plplot-compatible one
        DDouble az, alt, ay, scale;
        ORIENTATION3D axisExchangeCode;
        plplot3d = gdlConvertT3DMatrixToPlplotRotationMatrix( zValue, az, alt, ay, scale, axisExchangeCode);

        if (plplot3d == NULL) //use the original !P.T matrix (better than nothing)
        {
          Warning("Using Illegal 3D transformation, continuing. (FIXME)");
          plplot3d=gdlGetT3DMatrix(); //the original one
          plplot3d_guard.Reset(plplot3d);
          Data3d.code = code012;
        } else
        {
          switch (axisExchangeCode) {
          case NORMAL: //X->X Y->Y plane XY
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
        }
        DDoubleGDL *xValou=new DDoubleGDL(dimension(xEl));
        DDoubleGDL *yValou=new DDoubleGDL(dimension(yEl));
        Guard<BaseGDL> xval_guard, yval_guard;
        xval_guard.reset(xValou);
        yval_guard.reset(yValou);
        //rescale to normalized box before conversions --- works for both matrices.
        gdl3dto2dProjectDDouble(gdlGetScaledNormalizedT3DMatrix(plplot3d),xVal,yVal,zVal,xValou,yValou,Data3d.code);

        actStream->fill(xEl, static_cast<PLFLT*>(&(*xValou)[0]), static_cast<PLFLT*>(&(*yValou)[0]));

      }
      else  actStream->fill(xEl, static_cast<PLFLT*>(&(*xVal)[0]), static_cast<PLFLT*>(&(*yVal)[0]));
      if (stopClip) stopClipping(actStream);
    } // }}}

  private:

    virtual void post_call(EnvT*, GDLGStream *actStream) // {{{
    {
      if (doT3d && !real3d)
      {
        plplot3d_guard.Reset(plplot3d);
        actStream->stransform(NULL,NULL);
      }
      actStream->RestoreLayout();
      actStream->lsty(1); //reset linestyle
      actStream->psty(0); //reset fill
    }
  };

  void polyfill(EnvT* e)
  {
    polyfill_call polyfill;
    polyfill.call(e, 1);
  }

} // namespace
