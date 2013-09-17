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

  //XRANGE etc behaviour not as IDL (in some ways, better!)
  //TBD: LEGO

  using namespace std;

// shared parameter
  class surface_call: public plotting_routine_call
  {
    DDoubleGDL *zVal, *yVal, *xVal;
    Guard<BaseGDL> xval_guard, yval_guard, p0_guard;
    SizeT xEl, yEl, zEl;
    DDouble xStart, xEnd, yStart, yEnd, zStart, zEnd, datamax, datamin;
    bool nodata;
    bool setZrange;
    bool xLog;
    bool yLog;
    bool zLog;
    ORIENTATION3D axisExchangeCode;
  private:
    bool handle_args (EnvT* e)
    {
      xLog=e->KeywordSet ( "XLOG" );
      yLog=e->KeywordSet ( "YLOG" );
      zLog=e->KeywordSet ( "ZLOG" );
      if ( nParam ( )==1 )
      {
        if ( (e->GetNumericArrayParDefined ( 0 ))->Rank ( )!=2 )
          e->Throw ( "Array must have 2 dimensions: "
                     +e->GetParString ( 0 ) );

        BaseGDL* p0=e->GetNumericArrayParDefined ( 0 )->Transpose ( NULL );
        zVal=static_cast<DDoubleGDL*>
        ( p0->Convert2 ( GDL_DOUBLE, BaseGDL::COPY ) );
        p0_guard.reset ( p0 ); // delete upon exit

        if ( zVal->Rank ( )!=2 )
          e->Throw ( "Array must have 2 dimensions: "
                     +e->GetParString ( 0 ) );

        xEl=zVal->Dim ( 1 );
        yEl=zVal->Dim ( 0 );

        xVal=new DDoubleGDL ( dimension ( xEl ), BaseGDL::INDGEN );
        xval_guard.reset ( xVal ); // delete upon exit
        if (xLog) xVal->Inc();
        yVal=new DDoubleGDL ( dimension ( yEl ), BaseGDL::INDGEN );
        yval_guard.reset ( yVal ); // delete upon exit
        if (yLog) yVal->Inc();
      }
      else if ( nParam ( )==2||nParam ( )>3 )
      {
        e->Throw ( "Incorrect number of arguments." );
      }
      else
      {
        BaseGDL* p0=e->GetNumericArrayParDefined ( 0 )->Transpose ( NULL );
        zVal=static_cast<DDoubleGDL*>
        ( p0->Convert2 ( GDL_DOUBLE, BaseGDL::COPY ) );
        p0_guard.reset ( p0 ); // delete upon exit

        if ( zVal->Rank ( )!=2 )
          e->Throw ( "Array must have 2 dimensions: "
                     +e->GetParString ( 0 ) );
        xVal=e->GetParAs< DDoubleGDL>( 1 );
        yVal=e->GetParAs< DDoubleGDL>( 2 );

        if ( xVal->Rank ( )!=1 )
          e->Throw ( "Unable to handle non-vectorial array "+e->GetParString ( 1 )+" (FIXME!)" );

        if ( yVal->Rank ( )!=1 )
          e->Throw ( "Unable to handle non-vectorial array "+e->GetParString ( 1 )+" (FIXME!)" );

        if ( xVal->Rank ( )==1 )
        {
          xEl=xVal->Dim ( 0 );

          if ( xEl!=zVal->Dim ( 1 ) )
            e->Throw ( "X, Y, or Z array dimensions are incompatible." );
        }

        if ( yVal->Rank ( )==1 )
        {
          yEl=yVal->Dim ( 0 );

          if ( yEl!=zVal->Dim ( 0 ) )
            e->Throw ( "X, Y, or Z array dimensions are incompatible." );
        }

      }

      GetMinMaxVal ( xVal, &xStart, &xEnd );
      GetMinMaxVal ( yVal, &yStart, &yEnd );
      //XRANGE and YRANGE overrides all that, but  Start/End should be recomputed accordingly
      DDouble xAxisStart, xAxisEnd, yAxisStart, yAxisEnd;
      bool setx=gdlGetDesiredAxisRange(e, "X", xAxisStart, xAxisEnd);
      bool sety=gdlGetDesiredAxisRange(e, "Y", yAxisStart, yAxisEnd);
      if(setx && sety)
      {
        xStart=xAxisStart;
        xEnd=xAxisEnd;
        yStart=yAxisStart;
        yEnd=yAxisEnd;
      }
      else if (sety)
      {
        yStart=yAxisStart;
        yEnd=yAxisEnd;
      }
      else if (setx)
      {
        xStart=xAxisStart;
        xEnd=xAxisEnd;
        //must compute min-max for other axis!
        {
          gdlDoRangeExtrema(xVal,yVal,yStart,yEnd,xStart,xEnd);
        }
      }
  #undef UNDEF_RANGE_VALUE
      // z range
      datamax=0.0;
      datamin=0.0;
      GetMinMaxVal ( zVal, &datamin, &datamax );
      zStart=datamin;
      zEnd=datamax;
      setZrange = gdlGetDesiredAxisRange(e, "Z", zStart, zEnd);

        return false;
    } 

  private:
#define DPI (double)(4*atan(1.0))
#define DEGTORAD DPI/180.0

    void old_body (EnvT* e, GDLGStream* actStream) // {{{
    {
     //projection: would work only with 2D X and Y. Not supported here
      bool mapSet=false;
#ifdef USE_LIBPROJ4
      static LPTYPE idata;
      static XYTYPE odata;
      static PROJTYPE* ref;
      get_mapset ( mapSet );
      if ( mapSet )
      { // do nothing
//        ref=map_init ( );
//        if ( ref==NULL ) e->Throw ( "Projection initialization failed." );
        // but warn that projection is not taken into account
        Warning ( "SURFACE: Projection is set, but not taken into account (ony 1d X and Y) (FIX plplot first!)." );
      }
#endif
      //T3D
      static int t3dIx = e->KeywordIx( "T3D");
      bool doT3d=(e->KeywordSet(t3dIx)|| T3Denabled(e));
      //ZVALUE
      static int zvIx = e->KeywordIx( "ZVALUE");
      DDouble zValue=0.0;
      e->AssureDoubleScalarKWIfPresent ( zvIx, zValue );
      zValue=min(zValue,0.999999); //to avoid problems with plplot
      //SAVE
      static int savet3dIx = e->KeywordIx( "SAVE");
      bool saveT3d=e->KeywordSet(savet3dIx);
      //NODATA
      static int nodataIx = e->KeywordIx( "NODATA");
      nodata=e->KeywordSet(nodataIx);
      //SHADES
      static int shadesIx = e->KeywordIx( "SHADES");
      BaseGDL* shadevalues=e->GetKW ( shadesIx );
      bool doShade=(shadevalues != NULL); //... But 3d mesh will be colorized anyway!
      if (doShade) Warning ( "SHADE_SURF: Using Fixed (Z linear) Shade Values Only (FIXME)." );

      // [XYZ]STYLE
      DLong xStyle=0, yStyle=0, zStyle=0; ;
      gdlGetDesiredAxisStyle(e, "X", xStyle);
      gdlGetDesiredAxisStyle(e, "Y", yStyle);
      gdlGetDesiredAxisStyle(e, "Z", zStyle);

      //check here since after AutoIntvAC values will be good but arrays passed
      //to plplot will be bad...
      if ( xLog && xStart<=0.0 )
      {
        Warning ( "SURFACE: Infinite x plot range." );
        nodata=true;
      }
      if ( yLog && yStart<=0.0 )
      {
        Warning ( "SURFACE: Infinite y plot range." );
        nodata=true;
      }
      if ( zLog && zStart<=0.0 ) Warning ( "SURFACE: Infinite z plot range." );


      if ( ( xStyle&1 )!=1 )
      {
        PLFLT intv=AutoIntvAC ( xStart, xEnd, xLog );
      }

      if ( ( yStyle&1 )!=1 )
      {
        PLFLT intv=AutoIntvAC ( yStart, yEnd, yLog );
      }

      bool hasMinVal=e->KeywordPresent("MIN_VALUE");
      bool hasMaxVal=e->KeywordPresent("MAX_VALUE");
      DDouble minVal=datamin;
      DDouble maxVal=datamax;
      e->AssureDoubleScalarKWIfPresent ( "MIN_VALUE", minVal );
      e->AssureDoubleScalarKWIfPresent ( "MAX_VALUE", maxVal );

      if (!setZrange) {
        zStart=max(minVal,zStart);
        zEnd=min(zEnd,maxVal);
      }

      // then only apply expansion  of axes:
      if ( ( zStyle&1 )!=1 )
      {
        PLFLT intv=AutoIntvAC ( zStart, zEnd, zLog );
      }

      // background BEFORE next plot since it is the only place plplot may redraw the background...
      gdlSetGraphicsBackgroundColorFromKw ( e, actStream ); //BACKGROUND
      gdlNextPlotHandlingNoEraseOption(e, actStream);     //NOERASE

      gdlSetPlotCharsize(e, actStream);

      // Deal with T3D options -- either present and we have to deduce az and alt contained in it,
      // or absent and we have to compute !P.T from az and alt.

      PLFLT alt=30.0;
      PLFLT az=30.0;
      //set az and ax (alt)
      DFloat alt_change=alt;
      e->AssureFloatScalarKWIfPresent("AX", alt_change);
      alt=alt_change;

      alt=fmod(alt,360.0); //restrict between 0 and 90 for plplot!
      if (alt > 90.0 || alt < 0.0)
      {
        e->Throw ( "SURFACE: AX restricted to [0-90] range by plplot (fix plplot!)" );
      }
      DFloat az_change=az;
      e->AssureFloatScalarKWIfPresent("AZ", az_change);
      az=az_change;

      //now we are in plplot different kind of 3d
      DDoubleGDL* plplot3d;
      DDouble ay, scale; //not useful at this time
      if (doT3d) //convert to this world...
      {

        plplot3d=gdlConvertT3DMatrixToPlplotRotationMatrix(zValue, az, alt, ay, scale, axisExchangeCode);
        if (plplot3d == NULL)
        {
          e->Throw ( "SURFACE: Illegal 3D transformation." );
        }
      }
      else //make the transformation ourselves
      {
        scale=1/sqrt(3.0);
        //Compute transformation matrix with plplot conventions:
        plplot3d=gdlComputePlplotRotationMatrix( az, alt, zValue,scale);
        // save !P.T if asked to...
        if (saveT3d) //will use ax and az values...
        {
          DDoubleGDL* t3dMatrix=plplot3d->Dup();
          SelfTranspose3d(t3dMatrix);
          static DStructGDL* pStruct=SysVar::P();
          static unsigned tTag=pStruct->Desc()->TagIndex("T");
          for (int i=0; i<t3dMatrix->N_Elements(); ++i )(*static_cast<DDoubleGDL*>(pStruct->GetTag(tTag, 0)))[i]=(*t3dMatrix)[i];
          GDLDelete(t3dMatrix);
        }
      }

      if ( gdlSet3DViewPortAndWorldCoordinates(e, actStream, plplot3d, xLog, yLog,
        xStart, xEnd, yStart, yEnd, zStart, zEnd, zLog)==FALSE ) return;

      gdlSetPlotCharthick(e,actStream);


      if (xLog) xStart=log10(xStart);
      if (yLog) yStart=log10(yStart);
      if (zLog) zStart=log10(zStart);
      if (xLog) xEnd=log10(xEnd);
      if (yLog) yEnd=log10(yEnd);
      if (zLog) zEnd=log10(zEnd);

       actStream->w3d(scale,scale,scale*(1.0-zValue),
                     xStart, xEnd, yStart, yEnd, zStart, zEnd,
                     alt, az);


      bool up=e->KeywordSet ( "UPPER_ONLY" );
      bool low=e->KeywordSet ( "LOWER_ONLY" );
      if (up && low) nodata=true; //IDL behaviour

      DLong bottomColorIndex=-1;
      e->AssureLongScalarKWIfPresent("BOTTOM", bottomColorIndex);

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
            if (zLog)
            {
              v= log10(v);
              PLFLT miv=log10(minVal);
              PLFLT mav=log10(maxVal);
              if ( !isfinite(v) ) v=miv;
              if ( hasMinVal && v < miv) v=miv;
              if ( hasMaxVal && v > mav) v=mav;
            }
            else
            {
              if ( !isfinite(v) ) v=minVal;
              if ( hasMinVal && v < minVal) v=minVal;
              if ( hasMaxVal && v > maxVal) v=maxVal;
            }
            map[i][j] = v;
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
        for ( SizeT i=0; i<cgrid1.nx; i++ ) cgrid1.xg[i] = (*xVal)[i];
        for ( SizeT i=0; i<cgrid1.ny; i++ ) cgrid1.yg[i] = (*yVal)[i];
        //apply projection transformations:
        //not until plplot accepts 2D X Y!
        //apply plot options transformations:
        if (xLog) for ( SizeT i=0; i<cgrid1.nx; i++ ) cgrid1.xg[i] = cgrid1.xg[i]>0?log10(cgrid1.xg[i]):1E-12;  // #define EXTENDED_DEFAULT_LOGRANGE 12
        if (yLog) for ( SizeT i=0; i<cgrid1.ny; i++ ) cgrid1.yg[i] = cgrid1.yg[i]>0?log10(cgrid1.yg[i]):1E-12;

        // Important: make all clipping computations BEFORE setting graphic properties (color, size)
        bool doClip=(e->KeywordSet("CLIP")||e->KeywordSet("NOCLIP"));
        bool stopClip=false;
        if ( doClip )  if ( startClipping(e, actStream, false)==TRUE ) stopClip=true;

        gdlSetGraphicsForegroundColorFromKw ( e, actStream );
        //mesh option
        PLINT meshOpt;
        meshOpt=DRAW_LINEXY;
        if (e->KeywordSet ( "HORIZONTAL" )) meshOpt=DRAW_LINEX;
        if (e->KeywordSet ( "SKIRT" )) meshOpt+=DRAW_SIDES;
        //mesh plots both sides, so use it when UPPER_ONLY is not set.
        //if UPPER_ONLY is set, use plot3d/plot3dc
        //if LOWER_ONLY is set, use mesh/meshc and remove by plot3d!
        //in not up not low: mesh since mesh plots both sides
        if (up)
        {
          if (doShade)
            actStream->plot3dc(xg1,yg1,map,cgrid1.nx,cgrid1.ny,meshOpt+MAG_COLOR,NULL,0);
          else
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
               if (doShade) actStream->plot3dc(xg1,yg1,map,cgrid1.nx,cgrid1.ny,meshOpt+MAG_COLOR,NULL,0);
               else actStream->plot3dc(xg1,yg1,map,cgrid1.nx,cgrid1.ny,meshOpt,NULL,0);
             }
           }
           else
           {
             if (doShade) actStream->meshc(xg1,yg1,map,cgrid1.nx,cgrid1.ny,meshOpt+MAG_COLOR,NULL,0);
             else actStream->meshc(xg1,yg1,map,cgrid1.nx,cgrid1.ny,meshOpt,NULL,0);
           }
           //redraw upper part with background color to remove it... Not 100% satisfying though.
           if (low)
           {
            if (e->KeywordSet ( "SKIRT" )) meshOpt-=DRAW_SIDES;
            gdlSetGraphicsPenColorToBackground(actStream);
            actStream->plot3dc(xg1,yg1,map,cgrid1.nx,cgrid1.ny,meshOpt,NULL,0);
            gdlSetGraphicsForegroundColorFromKw ( e, actStream );
           }
        }

        if (stopClip) stopClipping(actStream);
//Clean alllocated data struct
        delete[] xg1;
        delete[] yg1;
        actStream->Free2dGrid(map, xEl, yEl);
      }
      //Draw axes with normal color!
      gdlSetGraphicsForegroundColorFromKw ( e, actStream ); //COLOR
      gdlBox3(e, actStream, xStart, xEnd, yStart, yEnd, zStart, zEnd, xLog, yLog, zLog, true);
    } 

  private:

    void call_plplot (EnvT* e, GDLGStream* actStream) 
    {
    } 

  private:

    virtual void post_call (EnvT*, GDLGStream* actStream) 
    {
      actStream->lsty(1);//reset linestyle
      actStream->sizeChar(1.0);
    } 

 }; // surface_call class

  void surface(EnvT* e)
  {
    surface_call surface;
    surface.call(e, 1);
  }
} // namespace
