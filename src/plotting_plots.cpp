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
    Guard<BaseGDL> xvalnative_guard, yvalnative_guard, zvalnative_guard;
    DDouble xStart, xEnd, yStart, yEnd, zStart, zEnd;
    DLong psym;
    bool xLog, yLog, zLog;
    SizeT nEl;
    bool append;
    bool doClip;
    bool doT3d, flat3d;
    DLongGDL *color;
    bool mapSet;
    DDouble az, alt, ay, scale;
    COORDSYS coordinateSystem=DATA;
    bool xnative,ynative,znative;  //tell if xVal etc are a copy of the variables or the real thing. When the real thing, they should not be modified.
      
  private:

    bool handle_args(EnvT* e)
    {
      //for cases where 3D is enabled, but z is not defined (since zVal is not an argument of PLOTS() )
      flat3d=true; //by default
      DFloat * position=gdlGetRegion();
      DDoubleGDL* zInit = new DDoubleGDL(position[4]);
      
      xnative=false;
      ynative=false;
      znative=false;
      
      gdlGetPsym(e, psym); //PSYM
      if (psym==10) e->Throw("PSYM (plotting symbol) out of range"); //not allowed for PLOTS!
      //T3D
      static int t3dIx = e->KeywordIx( "T3D");
      doT3d=(e->BooleanKeywordSet(t3dIx) || T3Denabled()); 
      
      static int continueIx = e->KeywordIx( "CONTINUE");
      append=e->KeywordSet(continueIx);
      if ( nParam()==1 )
      {
        BaseGDL* p0;
        p0=e->GetParDefined(0);
        SizeT dim0=p0->Dim(0);
        if ( dim0<2 || dim0>3 ) e->Throw("When only 1 param, dims must be (2,n) or (3,n)");
        
        DDoubleGDL *val=e->GetParAs< DDoubleGDL>(0);
        nEl=p0->N_Elements()/dim0;
        xVal=new DDoubleGDL(dimension(nEl), BaseGDL::NOZERO);
        xval_guard.Reset(xVal); // delete upon exit

        yVal=new DDoubleGDL(dimension(nEl), BaseGDL::NOZERO);
        yval_guard.Reset(yVal); // delete upon exit

        for ( SizeT i=0; i<nEl; i++ )
        {
          (*xVal)[i]=(*val)[dim0*i];
          (*yVal)[i]=(*val)[dim0*i+1];
        }

        zVal=zInit->New(dimension(nEl),BaseGDL::INIT);//inherits current Z.WINDOW[0]
        zval_guard.Reset(zVal); // delete upon exit
        if (dim0==3 && doT3d) {
          for ( SizeT i=0; i<nEl; i++ ) (*zVal)[i]=(*val)[dim0*i+2];
          flat3d=false;
        } 
      }
      //behaviour: if x or y are not an array, they are repeated to match minEl
      //if x or y have less elements than s, minEl is max(x,y) else minEl is size(s)
       //z ignored unless T3D is given or !P.T3D not 0
      else if ( nParam()==2 || (nParam()==3 && !doT3d) )
      {
        if (e->GetPar(0)->Type() == GDL_DOUBLE) xnative=true;
        xVal=e->GetParAs< DDoubleGDL>(0);
        SizeT xEl=xVal->N_Elements();

        if (e->GetPar(1)->Type() == GDL_DOUBLE) ynative=true;
        yVal=e->GetParAs< DDoubleGDL>(1);
        SizeT yEl=yVal->N_Elements();

        SizeT maxEl;
        maxEl=(xEl>yEl)?xEl:yEl;
        SizeT minEl=maxEl;
        
        if (maxEl > 1) { //there is at least one non-single
          minEl=(xVal->Dim(0)==0)?maxEl:((xEl<maxEl)?xEl:maxEl);
          minEl=(yVal->Dim(0)==0)?minEl:((yEl<minEl)?yEl:minEl);
        nEl=minEl;
        //replicate singletons if any
        if (xVal->Dim(0)==0) {
          DDoubleGDL* tmpxVal=e->GetParAs< DDoubleGDL>(0);
          xVal=new DDoubleGDL(nEl, BaseGDL::NOZERO);
          xval_guard.Reset(xVal); // delete upon exit
          for (SizeT i=0; i< nEl ; ++i) (*xVal)[i]=(*tmpxVal)[0];
        }
        if (yVal->Dim(0)==0) {
          DDoubleGDL* tmpyVal=e->GetParAs< DDoubleGDL>(1);
          yVal=new DDoubleGDL(nEl, BaseGDL::NOZERO);
          yval_guard.Reset(yVal); // delete upon exit
          for (SizeT i=0; i< nEl ; ++i) (*yVal)[i]=(*tmpyVal)[0];
        }
      }
        zVal=zInit->New(dimension(nEl),BaseGDL::INIT);//inherits current Z.WINDOW[0]
        zval_guard.Reset(zVal); // delete upon exit
      }
      else if ( nParam()==3 ) // here we have doT3d 
      { 
        if(doT3d) flat3d=false;

        if (e->GetPar(0)->Type() == GDL_DOUBLE) xnative=true;
        xVal=e->GetParAs< DDoubleGDL>(0);
        SizeT xEl=xVal->N_Elements();

        if (e->GetPar(1)->Type() == GDL_DOUBLE) ynative=true;
        yVal=e->GetParAs< DDoubleGDL>(1);
        SizeT yEl=yVal->N_Elements();

        if (e->GetPar(2)->Type() == GDL_DOUBLE) znative=true;
        zVal=e->GetParAs< DDoubleGDL>(2);
        SizeT zEl=zVal->N_Elements();
        
        SizeT maxEl;
        maxEl=(xEl>yEl)?xEl:yEl;
        maxEl=(maxEl>zEl)?maxEl:zEl;
        SizeT minEl=maxEl;
        
        if (maxEl > 1) { //there is at least one non-single
          minEl=(xVal->Dim(0)==0)?maxEl:((xEl<maxEl)?xEl:maxEl);
          minEl=(yVal->Dim(0)==0)?minEl:((yEl<minEl)?yEl:minEl);
          minEl=(zVal->Dim(0)==0)?minEl:((zEl<minEl)?zEl:minEl);
          nEl=minEl;

          //replicate singletons.
          if (xVal->Dim(0)==0) {
            DDoubleGDL* tmpxVal=e->GetParAs< DDoubleGDL>(0);
            xVal=new DDoubleGDL(nEl, BaseGDL::NOZERO);
            xval_guard.Reset(xVal); // delete upon exit
            for (SizeT i=0; i< nEl ; ++i) (*xVal)[i]=(*tmpxVal)[0];
          }
          if (yVal->Dim(0)==0) {
            DDoubleGDL* tmpyVal=e->GetParAs< DDoubleGDL>(1);
            yVal=new DDoubleGDL(nEl, BaseGDL::NOZERO);
            yval_guard.Reset(yVal); // delete upon exit
            for (SizeT i=0; i< nEl ; ++i) (*yVal)[i]=(*tmpyVal)[0];
          }
          if (zVal->Dim(0)==0) {
            DDoubleGDL* tmpzVal=e->GetParAs< DDoubleGDL>(2);
            zVal=new DDoubleGDL(nEl, BaseGDL::NOZERO);
            zval_guard.Reset(zVal); // delete upon exit
            for (SizeT i=0; i< nEl ; ++i) (*zVal)[i]=(*tmpzVal)[0];
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

      return false;
    }

    void old_body(EnvT* e, GDLGStream* actStream)
    {

      //check presence of DATA,DEVICE and NORMAL options
      static int DATAIx=e->KeywordIx("DATA");
      static int DEVICEIx=e->KeywordIx("DEVICE");
      static int NORMALIx=e->KeywordIx("NORMAL");
      coordinateSystem = DATA;
    //check presence of DATA,DEVICE and NORMAL options
      if (e->KeywordSet(DATAIx)) coordinateSystem = DATA;
      if (e->KeywordSet(DEVICEIx)) coordinateSystem = DEVICE;
      if (e->KeywordSet(NORMALIx)) coordinateSystem = NORMAL;
      
      // get_axis_type
      gdlGetAxisType(XAXIS, xLog);
      gdlGetAxisType(YAXIS, yLog);
      gdlGetAxisType(ZAXIS, zLog);
      
      //get DATA limits (not necessary CRANGE, see AXIS / SAVE behaviour!)
      GetCurrentUserLimits(actStream, xStart, xEnd, yStart, yEnd, zStart, zEnd);
      // get !Z.CRANGE
      gdlGetCurrentAxisRange(ZAXIS, zStart, zEnd);

      if (zStart != 0.0 && zStart == zEnd)
      {
        zStart = 0;
        zEnd = 1;
      }
      

      mapSet=false;
#ifdef USE_LIBPROJ
      get_mapset(mapSet);
      mapSet=(mapSet && coordinateSystem==DATA);
      if ( mapSet )
      {
        ref=map_init();
        if ( ref==NULL )
        {
          e->Throw("Projection initialization failed.");
        }
      }
#endif
        
      actStream->OnePageSaveLayout(); // one page
    
   //CLIPPING (or not) is just defining the adequate viewport and world coordinates, all of them normalized since this is what plplot will get in the end.
      static int NOCLIPIx = e->KeywordIx("NOCLIP");
      // Clipping is not enabled by default for PLOTS: noclip is true by default
      bool noclip = e->BooleanKeywordAbsentOrSet(NOCLIPIx);
      int CLIP = e->KeywordIx("CLIP");
      bool doClip=(e->KeywordSet(CLIP) && !(noclip) && !doT3d);
      
      PLFLT xnormmin=0;
      PLFLT xnormmax=1;
      PLFLT ynormmin=0;
      PLFLT ynormmax=1;

      if (doClip) { //redefine default viewport & world
        //define a default clipbox (DATA coords):
        DDouble clipBox[4]={xStart,yStart,xEnd,yEnd};
        DFloatGDL* clipBoxGDL = e->IfDefGetKWAs<DFloatGDL>(CLIP);
        if (clipBoxGDL != NULL && clipBoxGDL->N_Elements()< 4) for (auto i=0; i<4; ++i) clipBox[i]=0; //set clipbox to 0 0 0 0 apparently this is what IDL does.
        if (clipBoxGDL != NULL && clipBoxGDL->N_Elements()==4) for (auto i=0; i<4; ++i) clipBox[i]=(*clipBoxGDL)[i];
        //clipBox is defined accordingly to /NORM /DEVICE /DATA:
        //convert clipBox to normalized coordinates:
        switch (coordinateSystem) {
        case DATA:
          actStream->WorldToNormedDevice(clipBox[0],clipBox[1],xnormmin,ynormmin);
          actStream->WorldToNormedDevice(clipBox[2],clipBox[3],xnormmax,ynormmax);
          break;
        case DEVICE:
          actStream->DeviceToNormedDevice(clipBox[0],clipBox[1],xnormmin,ynormmin);
          actStream->DeviceToNormedDevice(clipBox[2],clipBox[3],xnormmax,ynormmax);
          break;
        default:
          xnormmin=clipBox[0];
          xnormmax=clipBox[2];
          ynormmin=clipBox[1];
          ynormmax=clipBox[3];
        }
      }

      // it is important to fix symsize before changing vpor or win 
      gdlSetSymsize(e, actStream); //SYMSIZE
      actStream->vpor(xnormmin,xnormmax,ynormmin,ynormmax);
      actStream->wind(xnormmin,xnormmax,ynormmin,ynormmax); //transformed (plotted) coords will be in NORM. Conversion will be made on the data values.
      
      if (doT3d) {
        xLog = false;
        yLog = false;
      } else {
        if (coordinateSystem == DEVICE) {
          xLog = false;
          yLog = false;
        } else if (coordinateSystem == NORMAL) {
          xLog = false;
          yLog = false;
        } 

      }
      actStream->setSymbolSizeConversionFactors();
    }

  private:

    void call_plplot(EnvT* e, GDLGStream* actStream) 
    {
      // start drawing. Graphic Keywords accepted: CLIP(YES), COLOR(OK), DATA(YES), DEVICE(YES),
      //LINESTYLE(OK), NOCLIP(YES), NORMAL(YES), PSYM(OK), SYMSIZE(OK), T3D(NO), THICK(OK), Z(NO)
      static int colorIx=e->KeywordIx ( "COLOR" ); bool doColor=false;
      if ( e->GetKW ( colorIx )!=NULL )
      {
        color=e->GetKWAs<DLongGDL>( colorIx ); doColor=true;
      }

      if (doT3d) {
        if (flat3d) {
        //reproject using P.T transformation in [0..1] cube during the actual plot using pltransform() (to reproject also the PSYMs is possible with plplot only if z=0, using this trick:
          SelfConvertToNormXYZ(nEl, (PLFLT*) xVal->DataAddr(), (PLFLT*) yVal->DataAddr(), (PLFLT*) zVal->DataAddr(), coordinateSystem);
          actStream->stransform(PDotTTransformXYZval, &((*zVal)[0]));
        } else {
          SelfPDotTTransformXYZ(nEl, (PLFLT*) xVal->DataAddr(), (PLFLT*) yVal->DataAddr(), (PLFLT*) zVal->DataAddr(), coordinateSystem);
        }
      } else {
        SelfConvertToNormXY(nEl, (PLFLT*) xVal->DataAddr(), (PLFLT*) yVal->DataAddr(), coordinateSystem);
        //this is important!
        coordinateSystem=NORMAL;
      }

      //properties
      if (!doColor || color->N_Elements() == 1){
        //if no KW or only 1 color, no need to complicate things
                                                //at draw_polyline level!
        gdlSetGraphicsForegroundColorFromKw(e, actStream); //COLOR
        doColor=false;
      }
      gdlSetLineStyle(e, actStream); //LINESTYLE
      gdlSetPenThickness(e, actStream); //THICK

#ifdef USE_LIBPROJ
        if ( mapSet && psym < 1) {
          GDLgrProjectedPolygonPlot(actStream, ref, NULL, xVal, yVal, false, false, NULL); //connect lines correctly
          psym=-psym;
          if (psym > 0) draw_polyline(actStream, xVal, yVal, 0.0, 0.0, false, xLog, yLog, psym, mapSet,  append, doColor?color:NULL);
        }
        else draw_polyline(actStream, xVal, yVal, 0.0, 0.0, false, xLog, yLog, psym, mapSet, append, doColor?color:NULL);
#else
      else draw_polyline(actStream, xVal, yVal, 0.0, 0.0, false, xLog, yLog, psym, false, append, doColor?color:NULL);
#endif
    }

  private:

    virtual void post_call(EnvT*, GDLGStream* actStream)
    {
      actStream->stransform(NULL, NULL);
      actStream->RestoreLayout();
      actStream->lsty(1); 
    }

  }; 

  void plots(EnvT* e)
  {
    plots_call plots;
    plots.call(e, 1);
  }

} // namespace
