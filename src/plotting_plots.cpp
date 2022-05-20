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
    SizeT nEl;
    bool append;
    bool doClip;
    bool doT3d, real3d;
    DDouble zValue;
    DDoubleGDL* plplot3d;
    Guard<BaseGDL> plplot3d_guard;
    DLongGDL *color;
    bool mapSet;
    T3DEXCHANGECODE axisExchangeCode;
    DDouble az, alt, ay, scale;

      COORDSYS coordinateSystem=DATA;
      
      
  private:

    bool handle_args(EnvT* e)
    {
      gdlGetPsym(e, psym); //PSYM
      if (psym==10) e->Throw("PSYM (plotting symbol) out of range"); //not allowed for PLOTS!
      real3d=false;
      //T3D
      static int t3dIx = e->KeywordIx( "T3D");
      doT3d=(e->BooleanKeywordSet(t3dIx) || T3Denabled()); 

      //note: Z (VALUE) will be used uniquely if Z is not effectively defined.
      // Then Z is useful only if (doT3d).
      static int zvIx = e->KeywordIx( "Z");
      zValue=0.0;
      e->AssureDoubleScalarKWIfPresent ( zvIx, zValue );

      static int continueIx = e->KeywordIx( "CONTINUE");
      append=e->KeywordSet(continueIx);
      if ( nParam()==1 )
      {
        BaseGDL* p0;
        p0=e->GetParDefined(0);
        SizeT dim0=p0->Dim(0);
        if ( dim0<2 || dim0>3 ) e->Throw("When only 1 param, dims must be (2,n) or (3,n)");
        if ( dim0==3) real3d=true;
        
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

        zVal=new DDoubleGDL(dimension(nEl), BaseGDL::NOZERO);
        zval_guard.Reset(zVal); // delete upon exit
        if (dim0==3) for ( SizeT i=0; i<nEl; i++ ) (*zVal)[i]=(*val)[dim0*i+2];
        else for (SizeT i=0; i< nEl ; ++i) (*zVal)[i]=zValue;
      }
      //behaviour: if x or y are not an array, they are repeated to match minEl
      //if x or y have less elements than s, minEl is max(x,y) else minEl is size(s)
       //z ignored unless T3D is given or !P.T3D not 0
      else if ( nParam()==2 || (nParam()==3 && !doT3d) )
      {
        xVal=e->GetParAs< DDoubleGDL>(0);
        SizeT xEl=xVal->N_Elements();

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
        //z will be set at zValue
        zVal=new DDoubleGDL(dimension(nEl), BaseGDL::NOZERO);
        zval_guard.Reset(zVal); // delete upon exit
        for (SizeT i=0; i< nEl ; ++i) (*zVal)[i]=zValue;
      }
      else if ( nParam()==3 )
      {
        if (doT3d) {
          real3d=true;
          doT3d=false;
        }
        zVal=e->GetParAs< DDoubleGDL>(2);
        SizeT zEl=zVal->N_Elements();

        xVal=e->GetParAs< DDoubleGDL>(0);
        SizeT xEl=xVal->N_Elements();

        yVal=e->GetParAs< DDoubleGDL>(1);
        SizeT yEl=yVal->N_Elements();
        //Z has no effect if T3D is not active, either through the T3D kw or through the !P.T3D sysvar.
        
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
      
    //T3D incompatible with DEVICE option.
      if (coordinateSystem == DEVICE) doT3d =false;
      
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
    // it is important to fix symsize before changing vpor or win 
      gdlSetSymsize(e, actStream); //SYMSIZE

      int noclipvalue=1;
      static int NOCLIPIx = e->KeywordIx("NOCLIP");
      e->AssureLongScalarKWIfPresent( NOCLIPIx, noclipvalue);
      doClip=(noclipvalue==0); //PLOTS by default does not clip, even if clip is defined by CLIP=

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
          if (doT3d || real3d) {
            actStream->wind(0,1,0,1); //transformed (plotted) coords will be in NORM
            xLog=false;
            yLog=false;
          } else setPlplotScale(actStream);
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

      //reproject using P.T transformation in [0..1] cube
      if (doT3d || real3d) SelfPDotTTransformXYZ(nEl, (PLFLT*) xVal->DataAddr(), (PLFLT*) yVal->DataAddr(), (PLFLT*) zVal->DataAddr(), coordinateSystem);

      bool stopClip=false;
      if ( doClip )  if ( startClipping(e, actStream, true)==true ) stopClip=true;  //will use pClip if needed

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
      if (stopClip) stopClipping(actStream);
    }

  private:

    virtual void post_call(EnvT*, GDLGStream* actStream)
    {
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
