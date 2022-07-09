/***************************************************************************
                       plotting_shade_surf.cpp  -  GDL routines for plotting
                             -------------------
    begin                : May 07 2013
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
#include "dinterpreter.hpp"
#define GDL_PI     double(3.1415926535897932384626433832795)

namespace lib
{
  using namespace std;

// shared parameter
  static PLFLT lightSourcePos[3]={1.0,1.0,0.0};

  class shade_surf_call: public plotting_routine_call
  {
    DDoubleGDL *zVal, *yVal, *xVal;
    Guard<BaseGDL> xval_guard, yval_guard, zval_guard, p0_guard;
    SizeT xEl, yEl, zEl;
    DDouble xStart, xEnd, yStart, yEnd, zStart, zEnd, zValue, datamax, datamin;
    DDouble minVal,maxVal;
    bool hasMinVal, hasMaxVal;
    bool doT3d;
    bool nodata;
    bool setZrange;
    bool xLog;
    bool yLog;
    bool zLog;
    T3DEXCHANGECODE axisExchangeCode;
    PLFLT alt = 30.0;
    PLFLT az = 30.0;
    PLFLT ay = 0;
    bool below=false;
    
 private:
    bool handle_args (EnvT* e)
    {
      //T3D ?
      static int t3dIx = e->KeywordIx("T3D");
      doT3d = (e->BooleanKeywordSet(t3dIx) || T3Denabled());

      zValue = std::numeric_limits<DDouble>::quiet_NaN(); //NAN = no zValue?
      static int zvIx = e->KeywordIx("ZVALUE");
      if (e->KeywordPresent(zvIx)){
        e->AssureDoubleScalarKWIfPresent(zvIx, zValue);
        zValue = min(zValue, ZVALUEMAX); //to avoid problems with plplot
        zValue = max(zValue, 0.0);
      }
      
     // in all cases, we have to exit here
      if ( nParam()==2 || nParam()>3 )	{
	  e->Throw ( "Incorrect number of arguments." );
	}

    // handle Log options passing via Keywords
    // note: undocumented keywords [xyz]type still exist and
    // have priority on [xyz]log ! 
    static int xTypeIx = e->KeywordIx( "XTYPE" );
    static int yTypeIx = e->KeywordIx( "YTYPE" );
    static int zTypeIx = e->KeywordIx( "ZTYPE" );
    static int xLogIx = e->KeywordIx( "XLOG" );
    static int yLogIx = e->KeywordIx( "YLOG" );
    static int zLogIx = e->KeywordIx( "ZLOG" );

    if ( e->KeywordPresent( xTypeIx ) ) xLog = e->KeywordSet( xTypeIx ); else xLog = e->KeywordSet( xLogIx );
    if ( e->KeywordPresent( yTypeIx ) ) yLog = e->KeywordSet( yTypeIx ); else yLog = e->KeywordSet( yLogIx );
    if ( e->KeywordPresent( zTypeIx ) ) zLog = e->KeywordSet( zTypeIx ); else zLog = e->KeywordSet( zLogIx );

    if ( nParam ( ) > 0 ) {
	// By testing here using EquivalentRank() we avoid computing zval if there was a problem.
	// AC 2018/04/24
	// a sub-array like: a=RANDOMU(seed, 3,4,5) & (this procedure name), a[1,*,*]
	// should be OK ...
	    if ( (e->GetNumericArrayParDefined ( 0 ))->EquivalentRank ( )!=2 ) e->Throw ( "Array must have 2 dimensions: "+e->GetParString ( 0 ) );
    }

    if ( nParam ( )==1) {
       BaseGDL* p0=e->GetNumericArrayParDefined ( 0 )->Transpose ( NULL );
        p0_guard.Init ( p0 ); // delete upon exit

        zVal=static_cast<DDoubleGDL*>( p0->Convert2 ( GDL_DOUBLE, BaseGDL::COPY ) );
        zval_guard.Init ( zVal ); // delete upon exit

        xEl=zVal->Dim ( 1 );
        yEl=zVal->Dim ( 0 );

        xVal=new DDoubleGDL ( dimension ( xEl ), BaseGDL::INDGEN );
        xval_guard.Init ( xVal ); // delete upon exit
        if (xLog) xVal->Inc();
        yVal=new DDoubleGDL ( dimension ( yEl ), BaseGDL::INDGEN );
        yval_guard.Init ( yVal ); // delete upon exit
        if (yLog) yVal->Inc();
      } else {
        BaseGDL* p0 = e->GetNumericArrayParDefined(0)->Transpose(NULL);
        p0_guard.Init(p0); // delete upon exit

        zVal = static_cast<DDoubleGDL*> (p0->Convert2(GDL_DOUBLE, BaseGDL::COPY));
        zval_guard.Init(zVal); // delete upon exit

        xVal = e->GetWriteableParAs< DDoubleGDL>(1);
        yVal = e->GetWriteableParAs< DDoubleGDL>(2);
          //filter out incompatible ranks >2 or ==0
        if (xVal->Rank() > 2)
          e->Throw("X, Y, or Z array dimensions are incompatible.");
        if (yVal->Rank() > 2)
          e->Throw("X, Y, or Z array dimensions are incompatible.");
        if (xVal->Rank() == 0 || yVal->Rank() == 0)
          e->Throw("X, Y, or Z array dimensions are incompatible.");
          //filter out incompatible 1D dimensions
        if (xVal->Rank() == 1) {
          xEl = xVal->Dim(0);
          if (xEl != zVal->Dim(1))
            e->Throw("X, Y, or Z array dimensions are incompatible.");
        }
        if (yVal->Rank() == 1) {
          yEl = yVal->Dim(0);
          if (yEl != zVal->Dim(0))
            e->Throw("X, Y, or Z array dimensions are incompatible.");
        }
          //filter out incompatible 2D dimensions
        if (xVal->Rank() == 2) {
          //plplot is unable to handle such subtetlies, better to throw?
          e->Throw("Sorry, plplot cannot handle 2D X coordinates in its 3D plots.");
          xEl = xVal->Dim(0);
          if ((xVal->Dim(0) != zVal->Dim(1))&&(xVal->Dim(1) != zVal->Dim(0)))
            e->Throw("X, Y, or Z array dimensions are incompatible.");
        }
        if (yVal->Rank() == 2) {
        //plplot is unable to handle such subtetlies, better to throw?
          e->Throw("Sorry, plplot cannot handle 2D Y coordinates in its 3D plots.");
          yEl = yVal->Dim(1);
          if ((yVal->Dim(0) != zVal->Dim(1))&&(yVal->Dim(1) != zVal->Dim(0)))
            e->Throw("X, Y, or Z array dimensions are incompatible.");
        }
        
        //plplot is unable to handle such subtetlies, better to throw?
        
//        // But if X is 2D and Y is 1D (or reciprocally), we need to promote the 1D to 2D since this is supported by IDL
//        if (xVal->Rank() == 1 && yVal->Rank() == 2) {
//          DDoubleGDL* xValExpanded = new DDoubleGDL(zVal->Dim(), BaseGDL::NOZERO);
//          SizeT k = 0;
//          for (SizeT j = 0; j < zVal->Dim(1); ++j) for (SizeT i = 0; i < zVal->Dim(0); ++i) (*xValExpanded)[k++] = (*xVal)[i];
//          xval_guard.Init(xValExpanded); // delete upon exit
//          xVal = xValExpanded;
//        } else if (xVal->Rank() == 2 && yVal->Rank() == 1) {
//          DDoubleGDL* yValExpanded = new DDoubleGDL(zVal->Dim(), BaseGDL::NOZERO);
//          SizeT k = 0;
//          for (SizeT j = 0; j < zVal->Dim(1); ++j) for (SizeT i = 0; i < zVal->Dim(0); ++i) (*yValExpanded)[k++] = (*yVal)[j];
//          xval_guard.Init(yValExpanded); // delete upon exit
//          yVal = yValExpanded;
//        }
      }
      
      GetMinMaxVal ( xVal, &xStart, &xEnd );
      GetMinMaxVal ( yVal, &yStart, &yEnd );
      //XRANGE and YRANGE overrides all that, but  Start/End should be recomputed accordingly
      DDouble xAxisStart, xAxisEnd, yAxisStart, yAxisEnd;
      bool setx=gdlGetDesiredAxisRange(e, XAXIS, xAxisStart, xAxisEnd);
      bool sety=gdlGetDesiredAxisRange(e, YAXIS, yAxisStart, yAxisEnd);
      if (setx && sety) {
        xStart=xAxisStart;
        xEnd=xAxisEnd;
        yStart=yAxisStart;
        yEnd=yAxisEnd;
      } else if (sety) {
        yStart = yAxisStart;
        yEnd = yAxisEnd;
      } else if (setx) {
        xStart = xAxisStart;
        xEnd = xAxisEnd;
        //must compute min-max for other axis!
        {
          gdlDoRangeExtrema(xVal, yVal, yStart, yEnd, xStart, xEnd, false);
        }
      }
      // z range
      datamax=0.0;
      datamin=0.0;
      GetMinMaxVal ( zVal, &datamin, &datamax );
      zStart=datamin;
      zEnd=datamax;
      setZrange = gdlGetDesiredAxisRange(e, ZAXIS, zStart, zEnd);

      //check here since after AutoIntvAC values will be good but arrays passed
      //to plplot will be bad...
      if (xLog && xStart <= 0.0) Warning("SURFACE: Infinite x plot range.");
      if (yLog && yStart <= 0.0) Warning("SURFACE: Infinite y plot range.");
      if (zLog && zStart <= 0.0) Warning("SURFACE: Infinite z plot range.");

      static int MIN_VALUEIx = e->KeywordIx( "MIN_VALUE");
      static int MAX_VALUEIx = e->KeywordIx( "MAX_VALUE");

      hasMinVal = e->KeywordPresent(MIN_VALUEIx);
      hasMaxVal = e->KeywordPresent(MAX_VALUEIx);
      minVal = datamin;
      maxVal = datamax;
      e->AssureDoubleScalarKWIfPresent ( MIN_VALUEIx, minVal );
      e->AssureDoubleScalarKWIfPresent ( MAX_VALUEIx, maxVal );

      if (!setZrange) {
        zStart=max(minVal,zStart);
        zEnd=min(zEnd,maxVal);
      }

      //Box adjustement:
      gdlAdjustAxisRange(e, XAXIS, xStart, xEnd, xLog);
      gdlAdjustAxisRange(e, YAXIS, yStart, yEnd, yLog);
      gdlAdjustAxisRange(e, ZAXIS, zStart, zEnd, zLog);

        return false; //do not abort
    } 

  private:
    bool prepareDrawArea (EnvT* e, GDLGStream* actStream) // {{{
    {
      static int savet3dIx = e->KeywordIx("SAVE");
      bool saveT3d = e->KeywordSet(savet3dIx);

      // background BEFORE next plot since it is the only place plplot may redraw the background...
      gdlSetGraphicsBackgroundColorFromKw ( e, actStream );
      //start a plot
      gdlNextPlotHandlingNoEraseOption(e, actStream);     //NOERASE

      // viewport and world coordinates
      // set the PLOT charsize before setting viewport (margin depend on charsize)
      gdlSetPlotCharsize(e, actStream);
      zValue=gdlSetViewPortAndWorldCoordinates(e, actStream, xStart, xEnd, xLog, yStart, yEnd, yLog, zStart, zEnd, zLog, zValue);

      // Deal with T3D options -- either present and we have to deduce az and alt contained in it,
      // or absent and we have to compute !P.T from az and alt.

      PLFLT scale[3]={SCALEBYDEFAULT,SCALEBYDEFAULT,SCALEBYDEFAULT};
      if (!doT3d) { // or absent and we have to compute !P.T from az and alt.
      //set az and ax (alt)
        DFloat az_change = az;
        static int AZIx = e->KeywordIx("AZ");
        e->AssureFloatScalarKWIfPresent(AZIx, az_change);
        az = az_change;
        
      DFloat alt_change=alt;
      static int AXIx=e->KeywordIx("AX");
      e->AssureFloatScalarKWIfPresent(AXIx, alt_change);
      alt=alt_change;
        alt=atan2(sin(alt * GDL_PI/180.0), cos(alt * GDL_PI/180.0)) * 180.0/GDL_PI;
        alt=fmod((alt+180),360.0);
        if (alt > 90 && alt <= 270) {
          az+=180.;
          if (alt > 180) {below=true; alt-=180; alt*=-1;} else alt=180-alt;
        } else if (alt > 270) {
          below=true;
          alt=-(360.-alt);
      }
        //Compute special transformation matrix for the BOX and give it to the driver
        DDoubleGDL* gdlBox3d=gdlDefinePlplotRotationMatrix( az, alt, scale, saveT3d);
        GDL_3DTRANSFORMDEVICE T3DForAXes;
        for (int i = 0; i < 16; ++i)T3DForAXes.T[i] =(*gdlBox3d)[i];
        T3DForAXes.zValue = (std::isfinite(zValue))?zValue:0;
        gdlStartSpecial3DDriverTransform(actStream,T3DForAXes);
      } else {
        //just ask for P.T3D transform with the driver:
        bool ok=gdlInterpretT3DMatrixAsPlplotRotationMatrix(az, alt, ay, scale, axisExchangeCode, below);
        if (!ok) Warning ( "SHADE_SURF: Illegal 3D transformation." );
        gdlStartT3DMatrixDriverTransform(actStream, zValue);
      }
      // We could have kept the old code where the box was written by plplot's box3(), but it would not be compatible with the rest of the eventual other (over)plots
      //Draw axes with normal color!
      gdlSetGraphicsForegroundColorFromKw ( e, actStream ); //COLOR
      //write OUR box using our 3D PLESC tricks:
      gdlBox3(e, actStream, xStart, xEnd, xLog, yStart, yEnd, yLog, zStart, zEnd, zLog, zValue);
      // title and sub title
      gdlWriteTitleAndSubtitle(e, actStream);
       //reset driver to 2D plotting routines, further 3D is just plplot drawing a mesh.
      gdlStop3DDriverTransform(actStream); 
      
      //we now pass EVERYTHING in normalized coordinates w/o clipping and set up a transformation to have plplot mesh correct on the 2D vpor.
//      gdlSwitchToClippedNormalizedCoordinates(e, actStream, true); //true=noclip
      const COORDSYS coordinateSystem = DATA;
      SelfConvertToNormXYZ(xStart, xLog, yStart, yLog, zStart, zLog, coordinateSystem); 
      SelfConvertToNormXYZ(xEnd, xLog, yEnd, yLog, zEnd, zLog, coordinateSystem);
      if (hasMinVal) ConvertToNormZ(1, &minVal, zLog, coordinateSystem);
      if (hasMaxVal) ConvertToNormZ(1, &maxVal, zLog, coordinateSystem);
      //WARNING Following sets coordinateSystem=NORM!
      COORDSYS coordinateSystem2 = DATA;
      SelfConvertToNormXY(xVal, xLog, yVal, yLog, coordinateSystem2); //always DATA for PLOT X,Y values
      ConvertToNormZ(xEl*yEl, (DDouble*) zVal->DataAddr(), zLog, coordinateSystem);
// the mapping between data 3-d and world 3-d coordinates is given by:
//
//   x = xmin   =>   wx = -0.5*basex
//   x = xmax   =>   wx =  0.5*basex
//   y = ymin   =>   wy = -0.5*basey
//   y = ymax   =>   wy =  0.5*basey
//   z = zmin   =>   wz =  0.0
//   z = zmax   =>   wz =  height
      actStream->vpor(0,1,0,1);
      actStream->wind(-0.5/scale[0],0.5/scale[0],-0.5/scale[1],0.5/scale[1]);
      if (alt < 0) { actStream->w3d(1,1,1,0,1,0,1,0.5,1.5, -alt, az);
      gdlFlipYPlotDirection(actStream); //special trick, not possible with plplot
      } else actStream->w3d(1,1,1,0,1,0,1,0.5,1.5, alt, az);
      
      if (zValue < 0.5) Message("SHADE_SURF: due to plplot restrictions, shaded surface is not entirely visible. Please try with zvalue=0.5 or greater.");
      
      return false;
      }

void applyGraphics(EnvT* e, GDLGStream * actStream) {
      //NODATA
      static int nodataIx = e->KeywordIx("NODATA");
      nodata = e->KeywordSet(nodataIx);
      //SHADES
      static int shadesIx = e->KeywordIx("SHADES");
      bool doShade=false;
      DLongGDL* shadevalues=NULL;
      if (e->GetKW(shadesIx) != NULL) {
        shadevalues = e->GetKWAs<DLongGDL>(shadesIx);
        doShade=true;
      }
      // Get decomposed value for shades
      DLong decomposed=GraphicsDevice::GetDevice()->GetDecomposed();
      if (doShade && decomposed==0) actStream->SetColorMap1Table(shadevalues->N_Elements(), shadevalues, decomposed); 
      else if (doShade && decomposed==1) actStream->SetColorMap1DefaultColors(256,  decomposed );
      else actStream->SetColorMap1Ramp(decomposed, 0.1);

      //Draw 3d mesh before axes
      // PLOT ONLY IF NODATA=0
      if (!nodata)
      {
        //use of intermediate map for correct handling of blanking values and nans.
        PLFLT ** map;
        actStream->Alloc2dGrid( &map, xEl, yEl);
        for ( SizeT i=0, k=0; i<xEl; i++ )
        {
          for ( SizeT j=0; j<yEl; j++)
          { //plplot does not like NaNs and any other terribly large gradient!
            PLFLT v=(*zVal)[k++];
              if ( !isfinite(v) ) v=minVal;
              if ( hasMinVal && v < minVal) v=minVal;
              if ( hasMaxVal && v > maxVal) v=maxVal;
            map[i][j] = (below)?1-v:v;
          }
        }
        // 1 types of grid only: 1D X and Y.
        PLcGrid cgrid1; // X and Y independent deformation
        PLFLT* xg1;
        PLFLT* yg1;
        xg1 = new PLFLT[xEl];
        yg1 = new PLFLT[yEl];
        cgrid1.xg = xg1;
        cgrid1.yg = yg1;
        cgrid1.nx = xEl;
        cgrid1.ny = yEl;
        for ( SizeT i=0; i<xEl; i++ ) cgrid1.xg[i] = (*xVal)[i];
        for ( SizeT i=0; i<yEl; i++ ) cgrid1.yg[i] = (*yVal)[i];
        
        //apply projection transformations:
        //not until plplot accepts 2D X Y!

        gdlSetGraphicsForegroundColorFromKw ( e, actStream );
        //mesh option
        PLINT meshOpt;
        meshOpt=(doShade)?MAG_COLOR:0;

        //position of light Source. Plplot does not use only the direction of the beam but the position of the illuminating
        //source. And its illumination looks strange. We try to make the ill. source a bit far in the good direction.
        PLFLT sun[3];
        sun[0]=xStart+(xEnd-xStart)*(0.5+lightSourcePos[0]);
        sun[1]=yStart+(yEnd-yStart)*(0.5+lightSourcePos[1]);
        sun[2]=zStart+(zEnd-zStart)*((1.0-zValue)+lightSourcePos[2]);
        actStream->lightsource(sun[0],sun[1],sun[2]);
        actStream->surf3d(xg1,yg1,map,cgrid1.nx,cgrid1.ny,meshOpt,NULL,0);

//Clean alllocated data struct
        delete[] xg1;
        delete[] yg1;
        actStream->Free2dGrid(map, xEl, yEl);
      }
    } 

    virtual void post_call (EnvT*, GDLGStream* actStream) 
    {
      actStream->lsty(1);//reset linestyle
      actStream->sizeChar(1.0);
    } 

 }; // SHADE_SURF_call class

  void shade_surf(EnvT* e)
  {
    shade_surf_call shade_surf;
    shade_surf.call(e, 1);
  }

 void set_shading(EnvT* e)
 {
    DDoubleGDL *light;
    static int lightIx=e->KeywordIx ( "LIGHT" );
    if ( e->GetKW ( lightIx )!=NULL )
    {
      light=e->GetKWAs<DDoubleGDL>( lightIx );
      if (light->N_Elements()>3) e->Throw("Keyword array parameter LIGHT must have from 1 to 3 elements.");
      for (SizeT i=0; i< light->N_Elements(); ++i) lightSourcePos[i]=(*light)[i];
    }
 }
} // namespace
