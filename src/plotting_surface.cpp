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

#define TRACEMATRIX_ROWMAJOR(var__,mrows__,ncolumns__)\
      {\
        fprintf(stderr,"row-major matrix[%d,%d]\n",mrows__,ncolumns__);\
      for (int row=0; row < mrows__; row++)\
      {\
        for (int col=0; col < ncolumns__ ; col++)\
        {\
          fprintf(stderr,"%lf, ",var__[col*mrows__ + row]);\
        }\
        fprintf(stderr,"\n");\
      }\
        fprintf(stderr,"\n");\
       }

#define TRACEMATRIX_COLMAJOR(var__,mrows__,ncolumns__)\
      {\
        fprintf(stderr,"col-major matrix[%d,%d]\n",mrows__,ncolumns__);\
      for (int row=0; row < mrows__; row++)\
      {\
        for (int col=0; col < ncolumns__ ; col++)\
        {\
          fprintf(stderr,"%lf, ",var__[row*ncolumns__ + col]);\
        }\
        fprintf(stderr,"\n");\
      }\
        fprintf(stderr,"\n");\
       }

namespace lib
{
  //TBD:
  //LINESTYLE THICK & [XYZ]THICK TICKLEN XYZCHARSIZE XYGRIDSTYLE(TICKLEN=1)
  //XYZMINOR XYZTICKFORMAT, INTERVAL, LAYOUT, LEN, NAME, TICKS, TICKUNITS, TICKV, TICK_GET
  //XRANGE etc behaviour not as IDL (in some ways, better!)
  //XYZ STYLE: remove axes if style=4 etc
  //ZVALUE and T3D, SAVE
  //BOTTOM (color) easy..
  //SAVE for T3D
  //SHADES (impossible!)
  //LEGO (idem!)
  //ZAXIS=[1,2,3,4] difficult!

  using namespace std;

// shared parameter
  static bool xLog;
  static bool yLog;

  class surface_call: public plotting_routine_call
  {

    DDoubleGDL *zVal, *yVal, *xVal;
    Guard<BaseGDL> xval_guard, yval_guard, p0_guard;
    SizeT xEl, yEl, zEl;
    DDouble xStart, xEnd, yStart, yEnd, zStart, zEnd, datamax, datamin;
    bool zLog, isLog;
    bool nodata;
//    DLongGDL *colors,*thick,*labels,*style;
//    Guard<BaseGDL> colors_guard,thick_guard,labels_guard,style_guard;
//    DFloatGDL *spacing,*orientation;
 //   Guard<BaseGDL> spacing_guard,orientation_guard;
  private:
    bool handle_args (EnvT* e)
    {
      if ( nParam ( )==1 )
      {
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
        yVal=new DDoubleGDL ( dimension ( yEl ), BaseGDL::INDGEN );
        yval_guard.reset ( yVal ); // delete upon exit
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
        //must compute min-max for other axis!
        {
          gdlDoRangeExtrema(yVal,xVal,xStart,xEnd,yStart,yEnd);
        }
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
      gdlGetDesiredAxisRange(e, "Z", zStart, zEnd);

        return false;
    } 

  private:
#define DPI (double)(4*atan(1.0))
#define DEGTORAD DPI/180.0

    void old_body (EnvT* e, GDLGStream* actStream) // {{{
    {
      // we need to define the NaN value
      static DStructGDL *Values=SysVar::Values ( );
      static DDouble d_nan=( *static_cast<DDoubleGDL*> ( Values->GetTag ( Values->Desc ( )->TagIndex ( "D_NAN" ), 0 ) ) )[0];
      static DDouble minmin=gdlAbsoluteMinValueDouble();
      // To enable a very dirty trick if X11 and /LOWER option
      Graphics* actDevice = Graphics::GetDevice();
      DLong currentGraphicsFunction = actDevice->GetGraphicsFunction();
      bool no_lower=(currentGraphicsFunction==-1);
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
     //NODATA
      int nodataIx = e->KeywordIx( "NODATA");
      nodata=e->KeywordSet(nodataIx);

      // [XYZ]STYLE
      DLong xStyle=0, yStyle=0, zStyle=0; ;
      gdlGetDesiredAxisStyle(e, "X", xStyle);
      gdlGetDesiredAxisStyle(e, "Y", yStyle);
      gdlGetDesiredAxisStyle(e, "Z", zStyle);

      // [XYZ]MARGIN
      DFloat xMarginL, xMarginR, yMarginB, yMarginT, zMarginF, zMarginB;
      gdlGetDesiredAxisMargin(e, "X", xMarginL, xMarginR);
      gdlGetDesiredAxisMargin(e, "Y", yMarginB, yMarginT);
      gdlGetDesiredAxisMargin(e, "Z", zMarginF, zMarginB);

      xLog=e->KeywordSet ( "XLOG" );
      yLog=e->KeywordSet ( "YLOG" );
      if (xLog || yLog) isLog=true; else isLog=false;
      zLog=e->KeywordSet ( "ZLOG" );

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

      if ( minVal>zEnd || maxVal<zStart) nodata=true; //do not complain but do nothing.

      // then only apply expansion  of axes:
      if ( ( zStyle&1 )!=1 )
      {
        PLFLT intv=AutoIntvAC ( zStart, zEnd, zLog );
      }
      //start a plot

      // background BEFORE next plot since it is the only place plplot may redraw the background...
      gdlSetGraphicsBackgroundColorFromKw ( e, actStream ); //BACKGROUND
      gdlNextPlotHandlingNoEraseOption(e, actStream);     //NOERASE

      // viewport and world coordinates
      // use POSITION
      int positionIx = e->KeywordIx( "POSITION");
      DFloatGDL* boxPosition = e->IfDefGetKWAs<DFloatGDL>( positionIx);
      if (boxPosition == NULL) boxPosition = (DFloatGDL*) 0xF;
      // set the PLOT charsize before computing box, see plot command.
      gdlSetPlotCharsize(e, actStream);

      PLFLT alt=30.0;
      DFloat alt_change=alt;
      e->AssureFloatScalarKWIfPresent("AX", alt_change);
      alt=alt_change;

      alt=fmod(alt,360.0); //restrict between 0 and 90 for plplot!
      if (alt > 90)
      {
        e->Throw ( "SURFACE: AX restricted to [0-90] range by plplot (fix plplot!)" );
      }
//      DDoubleGDL* Test=(new DDoubleGDL(dimension(4,8),BaseGDL::INDGEN));
//      TRACEMATRIX_ROWMAJOR((*Test),Test->Dim(0),Test->Dim(1))

      PLFLT az=30.0;
      DFloat az_change=az;
      e->AssureFloatScalarKWIfPresent("AZ", az_change);
      az=az_change;

      DDouble zoom=0.9;
      DLongGDL* pMulti=SysVar::GetPMulti();
      if ( (*pMulti)[1]>2||(*pMulti)[2]>2 ) zoom*=0.9; //better for multiboxes

      //compute size of 1,1,1 box centered at [0.5,0.5,0] projected at ax,az using internal GDL maths!
      //The box
      static DDouble v[32]=
      {
        0,0,0,1,
        1,0,0,1,
        0,1,0,1,
        1,1,0,1,
        0,0,1,1,
        1,0,1,1,
        0,1,1,1,
        1,1,1,1,
      };
      DDoubleGDL* V=(new DDoubleGDL(dimension(4,8),BaseGDL::NOZERO));
      memcpy(V->DataAddr(),v,8*4*sizeof(double));
//      cout<<"Box"<<endl;
//      TRACEMATRIX_ROWMAJOR((*V),V->Dim(0),V->Dim(1))
      //Deplact at [0.5,0.5,0]
      DDouble depla[16]={1,0,0,-0.5, 0,1,0,-0.5, 0,0,1,0, 0,0,0,1};
      DDoubleGDL* Depla=(new DDoubleGDL(dimension(4,4),BaseGDL::NOZERO));
      memcpy(Depla->DataAddr(),depla,16*sizeof(double));
//      cout<<"Depla"<<endl;
//      TRACEMATRIX_ROWMAJOR((*Depla),Depla->Dim(0),Depla->Dim(1))

      DDoubleGDL* pV=Depla->MatrixOp(V, true, false);
//      cout<<"After deplacement"<<endl;
//      TRACEMATRIX_ROWMAJOR((*pV),pV->Dim(0),pV->Dim(1))

      double cz=cos(az*DEGTORAD);
      double sz=sin(az*DEGTORAD);
      DDouble matz[16]={cz,-sz,0,0, sz,cz,0,0, 0,0,1,0, 0,0,0,1};
      DDoubleGDL* Matz=(new DDoubleGDL(dimension(4,4),BaseGDL::NOZERO));
      memcpy(Matz->DataAddr(),matz,16*sizeof(double));
//      cout<<"After z rotation"<<endl;
      pV=Matz->MatrixOp( pV, true, false);
//      TRACEMATRIX_ROWMAJOR((*pV),pV->Dim(0),pV->Dim(1))

      double cx=cos(alt*DEGTORAD);
      double sx=sin(alt*DEGTORAD);
      DDouble matx[16]={1,0,0,0, 0,sx,cx,0, 0,-cx,sx,0, 0,0,0,1};
      DDoubleGDL* Matx=(new DDoubleGDL(dimension(4,4),BaseGDL::NOZERO));
      memcpy(Matx->DataAddr(),matx,16*sizeof(double));
//      cout<<"x rotation Matrix"<<endl;
//      TRACEMATRIX_ROWMAJOR(( *Matx ),Matx->Dim(0),Matx->Dim(1))
//      cout<<"After x rotation"<<endl;
      pV=Matx->MatrixOp( pV, true, false);
//      TRACEMATRIX_ROWMAJOR((*pV),pV->Dim(0),pV->Dim(1))


      DDouble xmin,xmax,ymin,ymax;
      DLong iMin,iMax;
      pV->MinMax(&iMin,&iMax,NULL,NULL,false,0,0,4);
      xmin=(*pV)[iMin];
      xmax=(*pV)[iMax];
      pV->MinMax(&iMin,&iMax,NULL,NULL,false,1,0,4);
      ymin=(*pV)[iMin];
      ymax=(*pV)[iMax];

      if ( gdlSetViewPortAndWorldCoordinates(e, actStream, boxPosition,
      false, false,
      xMarginL, xMarginR, yMarginB, yMarginT,
      xmin, xmax, ymin, ymax, false)==FALSE ) return; //no good: should catch an exception to get out of this mess.

      gdlSetPlotCharthick(e,actStream); //impossible with plplot to draw labels without axes, so both will have same thickness.

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

      //Draw axes
      gdlSetGraphicsForegroundColorFromKw ( e, actStream ); //COLOR

      if (xLog) xStart=log10(xStart);
      if (yLog) yStart=log10(yStart);
      if (zLog) zStart=log10(zStart);
      if (xLog) xEnd=log10(xEnd);
      if (yLog) yEnd=log10(yEnd);
      if (zLog) zEnd=log10(zEnd);

      //We unzoom a little to have the axis seen and make the box...
      actStream->w3d(zoom,zoom,zoom,
                     xStart, xEnd, yStart, yEnd, zStart, zEnd,
                     alt, az);
      // title and sub title
     gdlWriteTitleAndSubtitle(e, actStream);

     DString xTitle,yTitle,zTitle;
     gdlGetDesiredAxisTitle(e, "X", xTitle);
     gdlGetDesiredAxisTitle(e, "Y", yTitle);
     gdlGetDesiredAxisTitle(e, "Z", zTitle);
     string xs,ys,zs;
     xs=ys=zs="bnstu";
     if (xLog) xs+="l";
     if (yLog) ys+="l";
     if (zLog) zs+="l";
     actStream->box3(xs.c_str(), xTitle.c_str(), 0.0, 0,
                      ys.c_str(), yTitle.c_str(), 0.0, 0,
                      zs.c_str(), zTitle.c_str(), 0.0, 4);


      bool up,low;
      up=e->KeywordSet ( "UPPER_ONLY" );
      low=e->KeywordSet ( "LOWER_ONLY" );
      if (low && no_lower) Warning ("LOWER_ONLY option not supported on this device - (fix in plplot library needed)");
      if (up && low) nodata=true; //IDL behaviour
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
        PLINT meshOpt=DRAW_LINEXY;
        if (e->KeywordSet ( "HORIZONTAL" )) meshOpt=DRAW_LINEX;
        if (e->KeywordSet ( "SKIRT" )) meshOpt+=DRAW_SIDES;
        //mesh plots both sides, so use it when UPPER_ONLY is not set.
        //if UPPER_ONLY is set, use plot3d
        //if LOWER_ONLY is set, use mesh and remove by plot3d!
        if (up)
        {
          actStream->plot3dc(xg1,yg1,map,cgrid1.nx,cgrid1.ny,meshOpt,NULL,0);
        }
        else if (low && !no_lower)
        {
          actStream->meshc(xg1,yg1,map,cgrid1.nx,cgrid1.ny,meshOpt,NULL,0);
          actDevice->SetGraphicsFunction(0); //TRICK !!!!!
          if (e->KeywordSet ( "SKIRT" )) meshOpt-=DRAW_SIDES;
          actStream->plot3dc(xg1,yg1,map,cgrid1.nx,cgrid1.ny,meshOpt,NULL,0);
          actDevice->SetGraphicsFunction(currentGraphicsFunction);
        }
        else
        {
          actStream->meshc(xg1,yg1,map,cgrid1.nx,cgrid1.ny,meshOpt,NULL,0);
        }


        if (stopClip) stopClipping(actStream);
//Clean alllocated data struct
        delete[] xg1;
        delete[] yg1;
        actStream->Free2dGrid(map, xEl, yEl);
      }
      //finished? Store Zrange and Loginess 
      gdlStoreAxisCRANGE("Z", zStart, zEnd, zLog);
      gdlStoreAxisType("Z",zLog);
      

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