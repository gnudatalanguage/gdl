/***************************************************************************
                       plotting_surface.cpp  -  GDL routines for plotting
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

#include "plotting.hpp"

#define GDL_PI     double(3.1415926535897932384626433832795)

namespace lib
{

  //XRANGE etc behaviour not as IDL (in some ways, better!)
  //TBD: LEGO

  using namespace std;


  class surface_call: public plotting_routine_call
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
    DDouble* Current3DMatrix;
    DDoubleGDL* gdlBox3d;
	PLFLT xratio, yratio, zratio, trans[3];
	PLINT nx, ny;    
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
        BaseGDL* p0 = e->GetNumericArrayParDefined(0)->Transpose(NULL);
        p0_guard.Init(p0); // delete upon exit

        zVal = static_cast<DDoubleGDL*> (p0->Convert2(GDL_DOUBLE, BaseGDL::COPY));
        zval_guard.Init(zVal); // delete upon exit

        xEl = zVal->Dim(1);
        yEl = zVal->Dim(0);

        xVal = new DDoubleGDL(dimension(xEl), BaseGDL::INDGEN);
        xval_guard.Init(xVal); // delete upon exit
        if (xLog) xVal->Inc();
        yVal = new DDoubleGDL(dimension(yEl), BaseGDL::INDGEN);
        yval_guard.Init(yVal); // delete upon exit
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

      static int MIN_VALUEIx = e->KeywordIx("MIN_VALUE");
      static int MAX_VALUEIx = e->KeywordIx("MAX_VALUE");

      hasMinVal = e->KeywordPresent(MIN_VALUEIx);
      hasMaxVal = e->KeywordPresent(MAX_VALUEIx);
      minVal = datamin;
      maxVal = datamax;
      e->AssureDoubleScalarKWIfPresent(MIN_VALUEIx, minVal);
      e->AssureDoubleScalarKWIfPresent(MAX_VALUEIx, maxVal);

      if (!setZrange) {
        zStart = max(minVal, zStart);
        zEnd = min(zEnd, maxVal);
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
	  bool zAxis=true;
	  static int zaxisIx = e->KeywordIx("ZAXIS");
	  DLong zaxis_value=0;
      if (e->GetKW(zaxisIx) != NULL) {
        e->AssureLongScalarKWIfPresent(zaxisIx, zaxis_value);
		if (zaxis_value > 4) zaxis_value=0;
	  }
	  if (zaxis_value < 0) zAxis = false;
 
      // background BEFORE next plot since it is the only place plplot may redraw the background...
      gdlSetGraphicsBackgroundColorFromKw ( e, actStream );
      //start a plot
      gdlNextPlotHandlingNoEraseOption(e, actStream);     //NOERASE


	  bool nosub=false;
	  
      // viewport and world coordinates
      // set the PLOT charsize before setting viewport (margin depend on charsize)
      gdlSetPlotCharsize(e, actStream);
      zValue=gdlSetViewPortAndWorldCoordinates(e, actStream, xStart, xEnd, xLog, yStart, yEnd, yLog, zStart, zEnd, zLog, zValue);

	  //save region info
	  PLFLT* save_region = gdlGetRegion();
		
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
        alt=fmod((alt+360),360.0);
        if (alt > 90 && alt <= 270) {
          az+=180.;
          if (alt > 180) {below=true; alt-=180; alt*=-1;} else alt=180-alt;
        } else if (alt > 270) {
          below=true;
          alt=-(360.-alt);
		}
		//Compute special transformation matrix for the BOX and give it to the driver. Subpage info is important
		gdlBox3d = gdlDefinePlplotRotationMatrix(az, alt, scale, saveT3d);
		//now that 3D matrix is OK, we pass in 'No Sub'. The plot will be scaled ans offsetted to the size of the actual subpage or position
		get3DMatrixParametersFor2DPosition(xratio, yratio, zratio, trans);
		actStream->NoSub();
		//recompute viewport etc now that ratio and offests will permit to displace the NoSub() plot at the correct position and scale:
		zValue=gdlSetViewPortAndWorldCoordinates(e, actStream, xStart, xEnd, xLog, yStart, yEnd, yLog, zStart, zEnd, zLog, zValue);
		gdlMakeSubpageRotationMatrix3d(gdlBox3d, xratio, yratio, zratio,trans);

        GDL_3DTRANSFORMDEVICE T3DForAXes;
        for (int i = 0; i < 16; ++i)T3DForAXes.T[i] =(*gdlBox3d)[i];
        T3DForAXes.zValue = (std::isfinite(zValue))?zValue:0;
        gdlStartSpecial3DDriverTransform(actStream,T3DForAXes);
        Current3DMatrix=static_cast<DDouble*>(gdlBox3d->DataAddr());
      } else {
        //just ask for P.T3D transform with the driver:
        bool ok=gdlInterpretT3DMatrixAsPlplotRotationMatrix(az, alt, ay, scale, axisExchangeCode, below);
        if (!ok) Warning ( "SURFACE: Illegal 3D transformation." );
        gdlStartT3DMatrixDriverTransform(actStream, zValue);

        DStructGDL* pStruct = SysVar::P(); //MUST NOT BE STATIC, due to .reset
        static unsigned tTag = pStruct->Desc()->TagIndex("T");
        Current3DMatrix = static_cast<DDouble*> (pStruct->GetTag(tTag, 0)->DataAddr());
        
      }
      // We could have kept the old code where the box was written by plplot's box3(), but it would not be compatible with the rest of the eventual other (over)plots
      //Draw axes with normal color!
      gdlSetGraphicsForegroundColorFromKw ( e, actStream ); //COLOR
      //write OUR box using our 3D PLESC tricks:
      gdlBox3(e, actStream, xStart, xEnd, xLog, yStart, yEnd, yLog, zStart, zEnd, zLog, zValue, zAxis, zaxis_value);
       //reset driver to 2D plotting routines, further 3D is just plplot drawing a mesh.
      gdlStop3DDriverTransform(actStream); 
      
      //we now pass EVERYTHING in normalized coordinates w/o clipping and set up a transformation to have plplot mesh correct on the 2D vpor.
      //however we need to check that clip values are OK to reproduce IDL's behaviour (no plot at all):
      if (gdlTestClipValidity(e, actStream)) return true; //note clip meaning is normal
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
  
// SHADE is not yet supported since plplot cannot handle it properly. Still, the code is here in // shade // comments.      
//Due to a bug in plplot, shading (here) and surface (in plotting_surface) do not behave similarly WRT the vpor and wind.
//So depending on doShade, the handling is different:
// shade //      static int shadesIx = e->KeywordIx("SHADES");
// shade //      bool doShade = false;
// shade //      if (e->KeywordPresent(shadesIx)) doShade = true;
// shade //      
// shade //      if (!doShade) {
        //This is the good version for surface without the shade argument.
        actStream->vpor(0, 1, 0, 1);
        actStream->wind(-0.5/scale[0],0.5/scale[0],-0.5/scale[1],0.5/scale[1]); //mandatory: to center in (0,0,0) for 3D Matrix rotation.
	  if (below) {
		actStream->w3d(1, 1, 1, 0, 1, 0, 1, 0.5, 1.5, -alt, az);
		gdlFlipYPlotDirection(actStream); //special trick, not possible with plplot
	  } else {
		actStream->w3d(1, 1, 1, 0, 1, 0, 1, 0.5, 1.5, alt, az);
	  // shade //      } else {
	  // shade //        //This is the good version for shade_surf and surface with shade option
	  // shade //        // (needs shifting the plplot plot by some amount in the 3DDriverTransform of the driver.)     
	  // shade //        actStream->vpor(0, 1, 0, 1);
	  // shade //        actStream->wind(-0.5 / scale[0], 0.5 / scale[0], -0.5 / scale[1], 0.5 / scale[1]); //mandatory: to center in (0,0,0) for 3D Matrix rotation.
	  // shade //        if (below) {
	  // shade //          actStream->w3d(1, 1, 1, 0, 1, 0, 1, 0, 1, -alt, az);
	  // shade //          DDouble xp = 0;
	  // shade //          DDouble yp1 = 0;
	  // shade //          DDouble yp2 = 0;
	  // shade //          Matrix3DTransformXYZval(0, 0, 0, &xp, &yp1,Current3DMatrix);
	  // shade //          Matrix3DTransformXYZval(0, 0, 0.5, &xp, &yp2,Current3DMatrix);
	  // shade //          gdlShiftYaxisUsing3DDriverTransform(actStream, 1 - (yp1 - yp2), true);
	  // shade //        } else {
	  // shade //          actStream->w3d(1, 1, 1, 0, 1, 0, 1, 0, 1, alt, az); //mandatory: in order to have shades plotted correctly, z must go from 0 to 1, not -0.5 to 0.5
	  // shade //          //as the code in plplot prevents negative "normalized" values.
	  // shade //          // To insure this (and shade_surf) to work in all cases, we must rely on the 3DDriverTransform, once again, to shift the [0,1] plot in [-0.5, 0.5]
	  // shade //          // 
	  // shade //          //compute vertical displacement of point [0,0,0] in projected coordinates between zv=0 and zv=0.5
	  // shade //          DDouble xp = 0;
	  // shade //          DDouble yp1 = 0;
	  // shade //          DDouble yp2 = 0;
	  // shade //          Matrix3DTransformXYZval(0, 0, 0, &xp, &yp1,Current3DMatrix);
	  // shade //          Matrix3DTransformXYZval(0, 0, 0.5, &xp, &yp2,Current3DMatrix);
	  // shade //          gdlShiftYaxisUsing3DDriverTransform(actStream, yp1 - yp2, false);
	  // shade //        }
	  }
		if (!doT3d) { //use a special matrix to shift and scale into current subpage
		  gdlMakeSubpageRotationMatrix2d(gdlBox3d, xratio, yratio, zratio,trans);
		  GDL_3DTRANSFORMDEVICE T3DForAXes;
		  for (int i = 0; i < 16; ++i)T3DForAXes.T[i] = (*gdlBox3d)[i];
		  T3DForAXes.zValue = (std::isfinite(zValue)) ? zValue : 0;
		  gdlStartSpecial3DDriverTransform(actStream, T3DForAXes);
          Guard<BaseGDL> g(gdlBox3d);
		//restore region info
		gdlStoreXAxisRegion(actStream, save_region);
		gdlStoreYAxisRegion(actStream, save_region);
		}
      return false;
    }
    
void applyGraphics(EnvT* e, GDLGStream * actStream) {
      //NODATA
      static int nodataIx = e->KeywordIx("NODATA");
      nodata = e->KeywordSet(nodataIx);
// shade //      //SHADES (again)
// shade //      static int shadesIx = e->KeywordIx("SHADES");
// shade //      bool doShade=false;
// shade //      DLongGDL* shadevalues=NULL;
// shade //      if (e->GetKW(shadesIx) != NULL) {
// shade //        shadevalues = e->GetKWAs<DLongGDL>(shadesIx);
// shade //        doShade=true;
// shade //      }
      // Get decomposed value for shades
// shade //      DLong decomposed=GraphicsDevice::GetDevice()->GetDecomposed();
// shade //     if (doShade) actStream->SetColorMap1Table(shadevalues->N_Elements(), shadevalues, decomposed); //SetColorMap1DefaultColors(256,  decomposed ); //actStream->SetColorMap1DefaultColors(256,  decomposed );
// shade //     else 
// shade //      actStream->SetColorMap1Ramp(decomposed, 0.5); 
      static int UPPER_ONLYIx = e->KeywordIx( "UPPER_ONLY");
      static int LOWER_ONLYIx = e->KeywordIx( "LOWER_ONLY");
      bool up=e->KeywordSet ( UPPER_ONLYIx );
      bool low=e->KeywordSet ( LOWER_ONLYIx );
      if (up && low) nodata=true; //IDL behaviour

      DLong bottomColorIndex=-1;
      static int BOTTOMIx = e->KeywordIx( "BOTTOM");
      e->AssureLongScalarKWIfPresent(BOTTOMIx, bottomColorIndex);

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
        
        //mesh option
        PLINT meshOpt;
        meshOpt=DRAW_LINEXY;
        static int HORIZONTALIx = e->KeywordIx("HORIZONTAL");
        if (e->KeywordSet ( HORIZONTALIx )) meshOpt=DRAW_LINEX;
        static int SKIRTIx = e->KeywordIx("SKIRT");
        if (e->KeywordSet ( SKIRTIx )) meshOpt+=DRAW_SIDES;
        //mesh plots both sides, so use it when UPPER_ONLY is not set.
        //if UPPER_ONLY is set, use plot3d/plot3dc
        //if LOWER_ONLY is set, use mesh/meshc and remove by plot3d!
        //in not up not low: mesh since mesh plots both sides
        if (up)
        {
// shade //          if (doShade)
// shade //            actStream->plot3dc(xg1,yg1,map,cgrid1.nx,cgrid1.ny,meshOpt+MAG_COLOR,NULL,0);
// shade //          else
            actStream->plot3dc(xg1,yg1,map,cgrid1.nx,cgrid1.ny,meshOpt,NULL,0);
        }
        else //mesh (both sides) but contains 'low' (remove top) and/or bottom
        {
           if (bottomColorIndex!=-1)
           {
             gdlSetGraphicsForegroundColorFromKw ( e, actStream, "BOTTOM" );
             actStream->meshc(xg1,yg1,map,cgrid1.nx,cgrid1.ny,meshOpt,NULL,0);
             gdlSetGraphicsForegroundColorFromKw ( e, actStream );
             if (!low) //redraw top with top color
             {
// shade //               if (doShade) actStream->plot3dc(xg1,yg1,map,cgrid1.nx,cgrid1.ny,meshOpt+MAG_COLOR,NULL,0);
// shade //               else 
                 actStream->plot3dc(xg1,yg1,map,cgrid1.nx,cgrid1.ny,meshOpt,NULL,0);
             }
           }
           else
           {
// shade //             if (doShade) actStream->meshc(xg1,yg1,map,cgrid1.nx,cgrid1.ny,meshOpt+MAG_COLOR,NULL,0);
// shade //             else 
               actStream->mesh(xg1,yg1,map,cgrid1.nx,cgrid1.ny,meshOpt);
           }
           //redraw upper part with background color to remove it... Not 100% satisfying though.
           if (low)
           {
            if (e->KeywordSet ( SKIRTIx )) meshOpt-=DRAW_SIDES;
            gdlSetGraphicsPenColorToBackground(actStream);
            actStream->plot3dc(xg1,yg1,map,cgrid1.nx,cgrid1.ny,meshOpt,NULL,0);
            gdlSetGraphicsForegroundColorFromKw ( e, actStream );
           }
        }

//Clean allocated data struct
        delete[] xg1;
        delete[] yg1;
        actStream->Free2dGrid(map, xEl, yEl);
      }
    }

    virtual void post_call(EnvT*, GDLGStream* actStream) {
      gdlStop3DDriverTransform(actStream); 
      actStream->lsty(1); //reset linestyle
      actStream->sizeChar(1.0);
    } 

 }; // surface_call class

  void surface(EnvT* e)
  {
    surface_call surface;
    surface.call(e, 1);
  }
} // namespace
