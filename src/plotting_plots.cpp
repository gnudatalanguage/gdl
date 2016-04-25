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

namespace lib
{

  using namespace std;

  class plots_call: public plotting_routine_call
  {

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
    DLongGDL *color;
    bool mapSet;
    ORIENTATION3D axisExchangeCode;
    DDouble az, alt, ay, scale;
  private:

    bool handle_args(EnvT* e)
    {
      gdlGetPsym(e, psym); //PSYM
      if (psym==10) e->Throw("PSYM (plotting symbol) out of range"); //not allowed for PLOTS!
      real3d=false;
      //T3D
      static int t3dIx = e->KeywordIx( "T3D");
      doT3d=(e->KeywordSet(t3dIx) || T3Denabled(e)); 

      //note: Z (VALUE) will be used uniquely if Z is not effectively defined.
      // Then Z is useful only if (doT3d).
      static int zvIx = e->KeywordIx( "Z");
      zValue=0.0;
      e->AssureDoubleScalarKWIfPresent ( zvIx, zValue );

      append=e->KeywordSet("CONTINUE");
      if ( nParam()==1 )
      {
        BaseGDL* p0;
        p0=e->GetParDefined(0);
        SizeT dim0=p0->Dim(0);
        if ( dim0<2 || dim0>3 )
          e->Throw("When only 1 param, dims must be (2,n) or (3,n)");

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
      //behaviour: if x or y are not an array, they are repeated to match minEl
      //if x or y have less elements than s, minEl is max(x,y) else minEl is size(s)
       //z ignored unless T3D is given or !P.T3D not 0
      else if ( nParam()==2 || (nParam()==3 && !doT3d) )
      {
        xVal=e->GetParAs< DDoubleGDL>(0);
        xEl=xVal->N_Elements();

        yVal=e->GetParAs< DDoubleGDL>(1);
        yEl=yVal->N_Elements();

        SizeT maxEl;
        maxEl=(xEl>yEl)?xEl:yEl;
        
        if (maxEl > 1) { //there is at least one non-single
          SizeT minEl;
          minEl=(xVal->Dim(0)==0)?maxEl:((xEl<maxEl)?xEl:maxEl);
          minEl=(yVal->Dim(0)==0)?minEl:((yEl<minEl)?yEl:minEl);

          //replicate singletons.
          if (xVal->Dim(0)==0) {
            DDoubleGDL* tmpxVal=e->GetParAs< DDoubleGDL>(0);
            xVal=new DDoubleGDL(minEl, BaseGDL::NOZERO);
            xEl=minEl;
            xval_guard.Reset(xVal); // delete upon exit
            for (SizeT i=0; i< minEl ; ++i) (*xVal)[i]=(*tmpxVal)[0];
          }
          if (yVal->Dim(0)==0) {
            DDoubleGDL* tmpyVal=e->GetParAs< DDoubleGDL>(1);
            yVal=new DDoubleGDL(minEl, BaseGDL::NOZERO);
            yEl=minEl;
            yval_guard.Reset(yVal); // delete upon exit
            for (SizeT i=0; i< minEl ; ++i) (*yVal)[i]=(*tmpyVal)[0];
          }
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
        //Z has no effect if T3D is not active, either through the T3D kw or through the !P.T3D sysvar.
        
        SizeT maxEl;
        maxEl=(xEl>yEl)?xEl:yEl;
        maxEl=(maxEl>zEl)?maxEl:zEl;
        
        if (maxEl > 1) { //there is at least one non-single
          SizeT minEl;
          minEl=(xVal->Dim(0)==0)?maxEl:((xEl<maxEl)?xEl:maxEl);
          minEl=(yVal->Dim(0)==0)?minEl:((yEl<minEl)?yEl:minEl);
          minEl=(zVal->Dim(0)==0)?minEl:((zEl<minEl)?zEl:minEl);

          //replicate singletons.
          if (xVal->Dim(0)==0) {
            DDoubleGDL* tmpxVal=e->GetParAs< DDoubleGDL>(0);
            xVal=new DDoubleGDL(minEl, BaseGDL::NOZERO);
            xEl=minEl;
            xval_guard.Reset(xVal); // delete upon exit
            for (SizeT i=0; i< minEl ; ++i) (*xVal)[i]=(*tmpxVal)[0];
          }
          if (yVal->Dim(0)==0) {
            DDoubleGDL* tmpyVal=e->GetParAs< DDoubleGDL>(1);
            yVal=new DDoubleGDL(minEl, BaseGDL::NOZERO);
            yEl=minEl;
            yval_guard.Reset(yVal); // delete upon exit
            for (SizeT i=0; i< minEl ; ++i) (*yVal)[i]=(*tmpyVal)[0];
          }
          if (zVal->Dim(0)==0) {
            DDoubleGDL* tmpzVal=e->GetParAs< DDoubleGDL>(2);
            zVal=new DDoubleGDL(minEl, BaseGDL::NOZERO);
            zEl=minEl;
            zval_guard.Reset(zVal); // delete upon exit
            for (SizeT i=0; i< minEl ; ++i) (*zVal)[i]=(*tmpzVal)[0];
          }
        }
      }
      if ( doT3d && !real3d) { //test to throw before plot values changes 
        plplot3d = gdlConvertT3DMatrixToPlplotRotationMatrix( zValue, az, alt, ay, scale, axisExchangeCode);
        if (plplot3d == NULL)
        {
          e->Throw("Illegal 3D transformation. (FIXME)");
        }
      }
      return false;
    }

    void old_body(EnvT* e, GDLGStream* actStream)
    {
      int clippingix=e->KeywordIx("CLIP");
      DFloatGDL* clipBox=NULL;

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
      
      //get DATA limits (not necessary CRANGE, see AXIS / SAVE behaviour!)
      GetCurrentUserLimits(e, actStream, xStart, xEnd, yStart, yEnd);
      // get !Z.CRANGE
      gdlGetCurrentAxisRange("Z", zStart, zEnd);

      if (zStart != 0.0 && zStart == zEnd)
      {
        Message("PLOTS: !Z.CRANGE ERROR, setting to [0,1]");
        zStart = 0;
        zEnd = 1;
      }

      restoreClipBox=false;
      int noclipvalue=1;
      e->AssureLongScalarKWIfPresent( "NOCLIP", noclipvalue);
      doClip=(noclipvalue==0); //PLOTS by default does not clip, even if clip is defined by CLIP= or !P.CLIP
      clipBox=e->IfDefGetKWAs<DFloatGDL>(clippingix);
      if (doClip && clipBox!=NULL && clipBox->N_Elements()>=4 ) //clipbox exist, will be used: convert to device coords
                                   //and save in !P.CLIP...
      {
        restoreClipBox=true; //restore later
        // save current !P.CLIP box, replace by our current clipbox in whatever coordinates, will
        // give back the !P.CLIP box at end...
        static DStructGDL* pStruct=SysVar::P();
        static unsigned clipTag=pStruct->Desc()->TagIndex("CLIP"); //must be in device coordinates
        static PLFLT tempbox[4];
        for ( int i=0; i<4; ++i ) savebox[i]=(*static_cast<DLongGDL*>(pStruct->GetTag(clipTag, 0)))[i];
        if ( coordinateSystem==DEVICE )
        {
          for ( int i=0; i<4; ++i ) tempbox[i]=(*clipBox)[i];
        }
        else if ( coordinateSystem==DATA )
        {
          //handle log: if existing box is already in log, use log of clipbox values.
          PLFLT worldbox[4];
          for ( int i=0; i<4; ++i ) worldbox[i]=(*clipBox)[i];
          if (xLog) {worldbox[0]=log10(worldbox[0]); worldbox[2]=log10(worldbox[2]);}
          if (yLog) {worldbox[1]=log10(worldbox[1]); worldbox[3]=log10(worldbox[3]);}
          bool okClipBox=true;
          for ( int i=0; i<4; ++i )
          {
            if (!isfinite(worldbox[i])) //NaN
            {
              okClipBox=false;restoreClipBox=false;doClip=false;
            }
          }
          if (okClipBox)
          {
            actStream->WorldToDevice(worldbox[0], worldbox[1], tempbox[0], tempbox[1]);
            actStream->WorldToDevice(worldbox[2], worldbox[3], tempbox[2], tempbox[3]);
          }
        }
        else
        {
          actStream->NormedDeviceToDevice((*clipBox)[0],(*clipBox)[1], tempbox[0], tempbox[1]);
          actStream->NormedDeviceToDevice((*clipBox)[2],(*clipBox)[3], tempbox[2], tempbox[3]);
        }
        //place in !P.CLIP
        for ( int i=0; i<4; ++i ) (*static_cast<DLongGDL*>(pStruct->GetTag(clipTag, 0)))[i]=tempbox[i];
      }

      mapSet=false;
      actStream->OnePageSaveLayout(); // one page

#ifdef USE_LIBPROJ4
      get_mapset(mapSet);
      mapSet=(mapSet && coordinateSystem==DATA);
      if ( mapSet )
      {
        ref=map_init();
        if ( ref==NULL )
        {
          e->Throw("Projection initialization failed.");
        }
        // below code is necessary for PLOTS, however we should try to avoid it. How???
        DDouble *sx, *sy;
        GetSFromPlotStructs( &sx, &sy );

        DFloat *wx, *wy;
        GetWFromPlotStructs( &wx, &wy );

        DDouble pxStart, pxEnd, pyStart, pyEnd;
        DataCoordLimits( sx, sy, wx, wy, &pxStart, &pxEnd, &pyStart, &pyEnd, true );
        actStream->vpor( wx[0], wx[1], wy[0], wy[1] );
        actStream->wind( pxStart, pxEnd, pyStart, pyEnd );
      }
#endif

    
      PLFLT wun, wdeux, wtrois, wquatre;
      if ( coordinateSystem==DATA) //with PLOTS, we can plot *outside* the box(e)s in DATA coordinates.
                                   // convert to device coords in this case
      {
        //All this is too complicated. IDL writes the plots in the NORMALIZED mode always using !X/Y.S and !X/Y.WINDOW. But we have to use plplot's
        // wind and vpor and keep trace of the size changes (for symbols) etc... Perhaps it is possible to avoid all those changes and use only
        // normalized coordinates. TO BE CHECKED.
        actStream->pageWorldCoordinates(wun, wdeux, wtrois, wquatre);
      }

      actStream->vpor(0, 1, 0, 1); //ALL PAGE
      
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
      else //with PLOTS, we can plot *outside* the box(e)s in DATA coordinates.
      {
        actStream->wind(wun, wdeux, wtrois, wquatre);
      }

    }

  private:

    void call_plplot(EnvT* e, GDLGStream* actStream) 
    {
      // start drawing. Graphic Keywords accepted: CLIP(YES), COLOR(OK), DATA(YES), DEVICE(YES),
      //LINESTYLE(OK), NOCLIP(YES), NORMAL(YES), PSYM(OK), SYMSIZE(OK), T3D(NO), THICK(OK), Z(NO)
      int colorIx=e->KeywordIx ( "COLOR" ); bool doColor=false;
      if ( e->GetKW ( colorIx )!=NULL )
      {
        color=e->GetKWAs<DLongGDL>( colorIx ); doColor=true;
      }

      if ( doT3d && !real3d) { //if X,Y and Z are passed, we will use !P.T and not our plplot "interpretation" of !P.T
                               //if the x and y scaling is OK, using !P.T directly permits to use other projections
                               //than those used implicitly by plplot. See @showhaus example for *DL
        // case where we project 2D data on 3D: use plplot-like matrix.
        plplot3d = gdlConvertT3DMatrixToPlplotRotationMatrix( zValue, az, alt, ay, scale, axisExchangeCode);

        static DDouble x0,y0,xs,ys; //conversion to normalized coords
        x0=(xLog)?-log10(xStart):-xStart;
        y0=(yLog)?-log10(yStart):-yStart;
        xs=(xLog)?(log10(xEnd)-log10(xStart)):xEnd-xStart;xs=1.0/xs;
        ys=(yLog)?(log10(yEnd)-log10(yStart)):yEnd-yStart;ys=1.0/ys;

        Data3d.zValue = zValue;
        Data3d.Matrix = plplot3d; //try to change for !P.T in future?
            Data3d.x0=x0;
            Data3d.y0=y0;
            Data3d.xs=xs;
            Data3d.ys=ys;
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
        actStream->stransform(gdl3dTo2dTransform, &Data3d);
      }
      // make all clipping computations BEFORE setting graphic properties (color, size)
      bool stopClip=false;
      if ( doClip )  if ( startClipping(e, actStream, true)==TRUE ) stopClip=true;
      //properties
      if (!doColor || color->N_Elements() == 1){ //if no KW or only 1 color, no need to complicate things
                                                //at draw_polyline level!
        gdlSetGraphicsForegroundColorFromKw(e, actStream); //COLOR
        doColor=false;
      }
      gdlSetLineStyle(e, actStream); //LINESTYLE
      gdlSetSymsize(e, actStream); //SYMSIZE
      gdlSetPenThickness(e, actStream); //THICK

      if (real3d) {
        //try first if the matrix is a plplot-compatible one
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
#ifdef USE_LIBPROJ4
        if ( mapSet ) GDLgrProjectedPolygonPlot(e, actStream, ref, NULL, xVal, yVal, false, false, NULL);
        else 
          bool valid=draw_polyline(e, actStream, xValou, yValou, 0.0, 0.0, false, xLog, yLog, psym, append, doColor?color:NULL);
      }
      else
      {
        if ( mapSet && psym < 1) {
          GDLgrProjectedPolygonPlot(e, actStream, ref, NULL, xVal, yVal, false, false, NULL);
          psym=-psym;
          if (psym > 0) bool valid=draw_polyline(e, actStream, xVal, yVal, 0.0, 0.0, false, xLog, yLog, psym, append, doColor?color:NULL);
        }
        else 
          bool valid=draw_polyline(e, actStream, xVal, yVal, 0.0, 0.0, false, xLog, yLog, psym, append, doColor?color:NULL);
      }
#else
          bool valid=draw_polyline(e, actStream, xValou, yValou, 0.0, 0.0, false, xLog, yLog, psym, append, doColor?color:NULL);
      }
      else
      {
          bool valid=draw_polyline(e, actStream, xVal, yVal, 0.0, 0.0, false, xLog, yLog, psym, append, doColor?color:NULL);
      }
#endif
      if (stopClip) stopClipping(actStream);
    }

  private:

    virtual void post_call(EnvT*, GDLGStream* actStream)
    {
      if (doT3d && !real3d)
      {
        plplot3d_guard.Reset(plplot3d);
        actStream->stransform(NULL,NULL);
      }
      actStream->RestoreLayout();
      actStream->lsty(1); //reset linestyle
      if (restoreClipBox)
      {
        static DStructGDL* pStruct=SysVar::P();
        static unsigned clipTag=pStruct->Desc()->TagIndex("CLIP"); //must be in device coordinates
        for ( int i=0; i<4; ++i ) (*static_cast<DLongGDL*>(pStruct->GetTag(clipTag, 0)))[i]=savebox[i];
      }
    }

  }; 

  void plots(EnvT* e)
  {
    plots_call plots;
    plots.call(e, 1);
  }

} // namespace
