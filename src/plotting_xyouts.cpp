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

namespace lib
{

  using namespace std;

  static DDouble lastTextPosX=0.0;
  static DDouble lastTextPosY=0.0;

  class xyouts_call: public plotting_routine_call
  {
    PLFLT currentBoxXmin, currentBoxXmax, currentBoxYmin, currentBoxYmax, currentBoxZmin, currentBoxZmax;
    PLFLT vpXmin, vpXmax, vpYmin, vpYmax;
    
    DDoubleGDL *xVal, *yVal, *zVal;
    Guard<BaseGDL> xval_guard, yval_guard, zval_guard;
    DDouble xStart, xEnd, yStart, yEnd, zStart, zEnd;
    
    DStringGDL* strVal;
    SizeT xEl, yEl, zEl, strEl;
    SizeT minEl;
    bool xLog, yLog, zLog;
    bool restorelayout;
    bool doClip, restoreClipBox;
    PLFLT savebox[4];
    bool kwWidth;
    PLFLT width;
    DLongGDL *color;
    DFloatGDL *spacing,*orientation,*charthick,*alignement,*size;
    Guard<BaseGDL> alignement_guard, orientation_guard,size_guard;
    bool doT3d;
    DDoubleGDL* plplot3d;
    Guard<BaseGDL> plplot3d_guard;
    bool singleArg;
    DDouble zValue;
    ORIENTATION3D axisExchangeCode;
    DDouble az, alt, ay, scale;

      enum
      {
        DATA=0,
        NORMAL,
        DEVICE
      } coordinateSystem;
      
      
  private:

    bool handle_args(EnvT* e)
    {
      //T3D
      static int t3dIx = e->KeywordIx( "T3D");
      doT3d=(e->KeywordSet(t3dIx) || T3Denabled()); 

      //note: Z (VALUE) will be used uniquely if Z is not effectively defined.
      // Then Z is useful only if (doT3d).
      static int zvIx = e->KeywordIx( "Z");
      zValue=0.0;
      if (doT3d) e->AssureDoubleScalarKWIfPresent ( zvIx, zValue );
      singleArg=false;
      if ( nParam()==1 )
      {
        singleArg=true;
        //string only...
        xVal=new DDoubleGDL(1, BaseGDL::ZERO);
        xval_guard.Reset(xVal); // delete upon exit
        yVal=new DDoubleGDL(1, BaseGDL::ZERO);
        yval_guard.Reset(yVal); // delete upon exit
        xEl=yEl=xVal->N_Elements();
        strVal=e->GetParAs<DStringGDL>(0);
        strEl=strVal->N_Elements();
        zVal=new DDoubleGDL(1);
        zval_guard.Reset(zVal); // delete upon exit
        (*zVal)[0]=zValue;
        minEl=strEl; //in this case only
      }
      else if ( nParam()==3 )
      {
        xVal=e->GetParAs< DDoubleGDL>(0);
        xEl=xVal->N_Elements();
        yVal=e->GetParAs< DDoubleGDL>(1);
        yEl=yVal->N_Elements();
        strVal=e->GetParAs<DStringGDL>(2);
        strEl=strVal->N_Elements();
        //behaviour: if x or y are not an array, they are repeated to match minEl
        //if x or y have less elements than s, minEl is max(x,y) else minEl is size(s)
         //z will be set at Zero unless Z=value is given
        if ( (xVal->Dim(0)==0) && (yVal->Dim(0)==0) ) {
          minEl=strEl;
          DDoubleGDL* tmpxVal=e->GetParAs< DDoubleGDL>(0);
          xVal=new DDoubleGDL(minEl, BaseGDL::NOZERO); //should remove previous xVal if allocated -- fixme.
          xval_guard.Reset(xVal); // delete upon exit
          for (SizeT i=0; i< minEl ; ++i) (*xVal)[i]=(*tmpxVal)[0];
          DDoubleGDL* tmpyVal=e->GetParAs< DDoubleGDL>(1);
          yVal=new DDoubleGDL(minEl, BaseGDL::NOZERO); //idem and below
          yval_guard.Reset(yVal); // delete upon exit
          for (SizeT i=0; i< minEl ; ++i) (*yVal)[i]=(*tmpyVal)[0];
        } else if (xVal->Dim(0)==0) {
          minEl=(yEl<strEl)?yEl:strEl;
          DDoubleGDL* tmpxVal=e->GetParAs< DDoubleGDL>(0);
          xVal=new DDoubleGDL(minEl, BaseGDL::NOZERO);
          xval_guard.Reset(xVal); // delete upon exit
          for (SizeT i=0; i< minEl ; ++i) (*xVal)[i]=(*tmpxVal)[0];
        } else if (yVal->Dim(0)==0) {
          minEl=(xEl<strEl)?xEl:strEl;
          DDoubleGDL* tmpyVal=e->GetParAs< DDoubleGDL>(1);
          yVal=new DDoubleGDL(minEl, BaseGDL::NOZERO);
          yval_guard.Reset(yVal); // delete upon exit
          for (SizeT i=0; i< minEl ; ++i) (*yVal)[i]=(*tmpyVal)[0];
         } else {
          minEl=(xEl<yEl)?xEl:yEl;
          minEl=(minEl<strEl)?minEl:strEl;          
         }
        zEl=minEl;
        zVal=new DDoubleGDL(dimension(zEl));
        zval_guard.Reset(zVal); // delete upon exit
        for (SizeT i=0; i< zEl ; ++i) (*zVal)[i]=zValue;
      }
      else
      {
        e->Throw("Not enough parameters. Either 1 parameter or 3 "
                 "parameters valid.");
      }
      if ( doT3d ) { //test to avois passing a non-rotation matrix to plplots's stransform. plplot limitation-> FIXME!
        plplot3d = gdlConvertT3DMatrixToPlplotRotationMatrix( zValue, az, alt, ay, scale, axisExchangeCode);
        if (plplot3d == NULL)
        {
          e->Throw("Illegal 3D transformation. (FIXME)");
        } else GDLDelete(plplot3d);
      }
      return true;
    }

  private:

    void getTextPos(GDLGStream *a, DDouble &wx, DDouble &wy)
    {
      a->DeviceToWorld(lastTextPosX, lastTextPosY, wx, wy);
      if (GDL_DEBUG_PLSTREAM) fprintf(stderr,"getTextPos: Got norm: %lf %lf giving %lf %lf world\n", lastTextPosX, lastTextPosY, wx, wy);
    }

    void old_body(EnvT* e, GDLGStream* actStream)
    {

      // WIDTH keyword (read, write)
      static int widthIx=e->KeywordIx("WIDTH");
      kwWidth=e->KeywordPresent(widthIx);
      width=0.;

      //check presence of DATA,DEVICE and NORMAL options
      static int DATAIx=e->KeywordIx("DATA");
      static int DEVICEIx=e->KeywordIx("DEVICE");
      static int NORMALIx=e->KeywordIx("NORMAL");
      coordinateSystem = DATA;
    //check presence of DATA,DEVICE and NORMAL options
      if (e->KeywordSet(DATAIx)) coordinateSystem = DATA;
      if (e->KeywordSet(DEVICEIx)) coordinateSystem = DEVICE;
      if (e->KeywordSet(NORMALIx)) coordinateSystem = NORMAL;
      
    //T3D incompatible with DEVICE option.
      if (coordinateSystem == DEVICE) doT3d =false;
      
      // get_axis_type
      gdlGetAxisType("X", xLog);
      gdlGetAxisType("Y", yLog);
      gdlGetAxisType("Z", zLog);

      //get DATA limits (not necessary CRANGE, see AXIS / SAVE behaviour!)
      GetCurrentUserLimits(actStream, xStart, xEnd, yStart, yEnd);
      // get !Z.CRANGE
      gdlGetCurrentAxisRange("Z", zStart, zEnd);

      if (zStart != 0.0 && zStart == zEnd)
      {
        Message("PLOTS: !Z.CRANGE ERROR, setting to [0,1]");
        zStart = 0;
        zEnd = 1;
      }

      int noclipvalue=1;
      static int NOCLIPIx = e->KeywordIx("NOCLIP");
      e->AssureLongScalarKWIfPresent( NOCLIPIx, noclipvalue);
      doClip=(noclipvalue==0); //XYOUTS by default does not clip, even if clip is defined by CLIP= or !P.CLIP, and CONTRARY TO THE DOCUMENTATION!!!!
      restorelayout=true;
      
      bool mapSet=false;
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
        restorelayout=true;

      }
#endif
      if ( doT3d ) {
        doClip=false; //impossible to clip in 3d using plplot. we should do it ourselves.
        restorelayout=false;
      } else {

        if (restorelayout) actStream->OnePageSaveLayout(); // one page

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
        else //with XYOUTS, we can plot *outside* the box(e)s in DATA coordinates.
        {
          setPlplotScale(actStream);
        }
      }

      PLFLT x,y,aspectw,aspectd;
      aspectw=actStream->boxAspectWorld();
      aspectd=actStream->boxAspectDevice();

      static int colorIx=e->KeywordIx ( "COLOR" ); bool docolor=false;
      static int charthickIx=e->KeywordIx ( "CHARTHICK" ); bool docharthick=false;
      static int charsizeIx=e->KeywordIx ( "CHARSIZE" ); bool docharsize=false;
      if ( e->GetKW ( colorIx )!=NULL )
      {
        color=e->GetKWAs<DLongGDL>( colorIx ); docolor=true;
      }
      if ( e->GetKW ( charthickIx )!=NULL )
      {
        charthick=e->GetKWAs<DFloatGDL>( charthickIx ); docharthick=true;
      }
      if ( e->GetKW ( charsizeIx )!=NULL )
      {
        size=e->GetKWAs<DFloatGDL>( charsizeIx ); docharsize=true;
      }
      else  //for security in future conditional evaluation...
      {
        size=new DFloatGDL  ( dimension (1), BaseGDL::ZERO );
        size_guard.Init ( size);
        (*size)[0]=1.0;
      }
      static int orientationIx=e->KeywordIx ( "ORIENTATION" );
      if ( e->GetKW ( orientationIx )!=NULL )
      {
        orientation=e->GetKWAs<DFloatGDL>( orientationIx ); 
      }
      else
      {
        orientation=new DFloatGDL  ( dimension (1), BaseGDL::ZERO );
        orientation_guard.Init ( orientation);
        (*orientation)[0]=0;
      }
      static int alignIx=e->KeywordIx ( "ALIGNMENT" );
      if ( e->GetKW ( alignIx )!=NULL )
      {
        alignement=e->GetKWAs<DFloatGDL>( alignIx );
      }
      else
      {
        alignement=new DFloatGDL  ( dimension (1), BaseGDL::ZERO );
        alignement_guard.Init (alignement);
        (*alignement)[0]=0;
      }


      // make all clipping computations BEFORE setting graphic properties (color, size)
      bool stopClip=false;
      if ( doClip )  if ( startClipping(e, actStream, true)==true ) stopClip=true;

      // *** start drawing by defalut values
      gdlSetGraphicsForegroundColorFromKw(e, actStream);
      if (!docharthick) gdlSetPlotCharthick(e, actStream);
      if (!docharsize) gdlSetPlotCharsize(e, actStream, true); //accept SIZE kw!

      if (doT3d) { //set up plplot 3D transform
        // case where we project 2D data on 3D: use plplot-like matrix.
        static DDouble x0,y0,xs,ys; //conversion to normalized coords

        if (coordinateSystem==NORMAL) {;
          x0=0;y0=0;xs=1.0;ys=1.0;
        } else {
          x0=(xLog)?-log10(xStart):-xStart;
          y0=(yLog)?-log10(yStart):-yStart;
          xs=(xLog)?(log10(xEnd)-log10(xStart)):xEnd-xStart;xs=1.0/xs;
          ys=(yLog)?(log10(yEnd)-log10(yStart)):yEnd-yStart;ys=1.0/ys;
        }

        DDouble az, alt, ay, scale;

        // here zvalue here is zcoord on Z axis, to be scaled between 0 and 1 for compatibility with call of gdlConvertT3DMatrixToPlplotRotationMatrix()
        zValue /= (zEnd - zStart);
        plplot3d = gdlConvertT3DMatrixToPlplotRotationMatrix(zValue, az, alt, ay, scale, axisExchangeCode);
        //if matrix was not checked to be ok at start, we could authorize a non_rotation matrix, but would have to avoid using stransform.
        Data3d.zValue = zValue;
        Data3d.Matrix = plplot3d; //try to change for !P.T in future?
        Data3d.x0 = x0;
        Data3d.y0 = y0;
        Data3d.xs = xs;
        Data3d.ys = ys;
        switch (axisExchangeCode) {
          case NORMAL3D: //X->X Y->Y plane XY
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
      // Get decomposed value for colors
      DLong decomposed=GraphicsDevice::GetDevice()->GetDecomposed();

      DDouble *sx, *sy, *sz;
      GetSFromPlotStructs(&sx, &sy, &sz); 
        
      for ( SizeT i=0; i<minEl; ++i )
      {
        //if string only, fill empty Xval Yval with current value:
        if ( nParam()==1 )
        {
          DDouble s,t;
          getTextPos(actStream, s, t);
          (*xVal)[0]=s;
          (*yVal)[0]=t;
        }
        x=static_cast<PLFLT>((*xVal)[i%xVal->N_Elements ( )]); //insure even 1 parameter, string array
        y=static_cast<PLFLT>((*yVal)[i%xVal->N_Elements ( )]);

#ifdef USE_LIBPROJ4
        if ( mapSet )
        {
          LPTYPE idata;
          XYTYPE odata;
          idata.u=x * DEG_TO_RAD;
          idata.v=y * DEG_TO_RAD;
          odata=PJ_FWD(idata, ref);
          x=odata.u;
          y=odata.v;
        }
#endif
 
        if (  coordinateSystem==DATA ) { //in case 3d matrix, the matrix transform above is in log, so coordinates must be in log.
          if( xLog ) x=log10(x);
          if( yLog ) y=log10(y);
        }
        
        if ( !isfinite(x)|| !isfinite(y) ) continue; //no plot
        if ( docharsize && ( *size )[i%size->N_Elements ( )] < 0) continue; //no plot either
        
        //plot!
        if (docharsize) actStream->sizeChar(( *size )[i%size->N_Elements ( )]);
        if (docolor) actStream->Color ( ( *color )[i%color->N_Elements ( )], decomposed);
        if (docharthick) actStream->Thick(( *charthick )[i%charthick->N_Elements()]);

	//orientation word is not orientation page depending on axes increment direction [0..1] vs. [1..0]
        PLFLT oriD=(( *orientation )[i%orientation->N_Elements ( )]); //ori DEVICE
        PLFLT oriW=oriD; //ori WORLD
        oriD *= DEGTORAD;
        if (sx[1]<0) oriW=180.0-oriW;
        if (sy[1]<0) oriW*=-1;
        oriW *= DEGTORAD;
        
        PLFLT cosOriD=cos(oriD);
        PLFLT sinOriD=sin(oriD);
        PLFLT cosOriW=cos(oriW);
        PLFLT sinOriW=sin(oriW);
        
        PLFLT align=( *alignement )[i%alignement->N_Elements ( )];
        align=max(align,0.0); align=min(align,1.0);
        PLFLT dispx,dispy, chsize, dx, dy;
        // displacement due to offset (reference in IDL is baseline,
        // in plplot it's the half-height) is best computed in device coords
        chsize=actStream->dCharHeight()*0.5;
        actStream->WorldToDevice(x, y, dx, dy);
        actStream->DeviceToWorld(dx-chsize*sinOriD,dy+chsize*cosOriD,dispx,dispy);
        string out=(*strVal)[i%strVal->N_Elements ( )];
        actStream->ptex(dispx, dispy, cosOriW, sinOriW*aspectw/aspectd, align, out.c_str());

        if (singleArg || (i==minEl-1 ) ) //then x and y are not given and whatever the number of strings, are retrieved
                       // from lastTextPos. We must thus remember lastTextPos.
        {
          width=actStream->gdlGetmmStringLength(out.c_str()); //in mm
          //we want normed size:
          width=actStream->mm2ndx(width);
          //save position - compute must be in DEVICE coords, or in normed*aspect!
          actStream->WorldToNormedDevice(x, y, dx, dy); //normed
          actStream->NormedDeviceToWorld(dx+(1.0-align)*width*cosOriD,dy+(1.0-align)*width*sinOriD/aspectd,dispx,dispy);
          actStream->WorldToDevice(dispx, dispy, lastTextPosX, lastTextPosY);
        }
      }
      if (stopClip) stopClipping(actStream);

      if ( kwWidth )
      {
        // width is in "normalized coordinates"
        e->SetKW(widthIx, new DFloatGDL(width));
      }
    } 

  private:

    void call_plplot(EnvT* e, GDLGStream* actStream) // {{{
    {
    } 

  private:

    virtual void post_call(EnvT* e, GDLGStream* actStream) // {{{
    {
      if (doT3d)
      {
        plplot3d_guard.Reset(plplot3d);
        actStream->stransform(NULL,NULL);
      }
      if (restorelayout) actStream->RestoreLayout();
      actStream->sizeChar(1.0);
    }

  };

  void xyouts(EnvT* e)
  {
    xyouts_call xyouts;
    xyouts.call(e, 1);
  }

} // namespace
