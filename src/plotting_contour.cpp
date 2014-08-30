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

#define LABELOFFSET 0.003
#define LABELSPACING 0.1

namespace lib
{

  using namespace std;

// shared parameter
  bool xLog;
  bool yLog;
  bool restorelayout;
  void myrecordingfunction(PLFLT x, PLFLT y, PLFLT *xt, PLFLT *yt, PLPointer data)
  {
    *xt=x;
    *yt=y;
    fprintf(stderr,"x=%f,y=%f\n",x,y);
  }
  PLINT doIt( PLFLT x, PLFLT y )
  {
    if (xLog && x<=0 ) return 0;
    if (yLog && y<=0 ) return 0;
    return 1;
  }

  class contour_call: public plotting_routine_call
  {

    DDoubleGDL *zVal, *yVal, *xVal;
    Guard<BaseGDL> xval_guard, yval_guard, p0_guard;
    DDoubleGDL *yValTemp, *xValTemp;
    Guard<BaseGDL> xval_temp_guard, yval_temp_guard;
    SizeT xEl, yEl, zEl, ixEl, iyEl;
    DDouble xStart, xEnd, yStart, yEnd, zStart, zEnd, datamax, datamin;
    bool zLog, isLog;
    bool overplot, make2dBox, make3dBox, nodata;
    DLongGDL *colors,*labels,*style;
    DFloatGDL* thick;
    Guard<BaseGDL> colors_guard,thick_guard,labels_guard,style_guard;
    DFloatGDL *spacing,*orientation;
    Guard<BaseGDL> spacing_guard,orientation_guard;
    bool doT3d;
    bool irregular;
    bool setZrange;

    //PATH_XY etc: use actStream->stransform with a crafted recording function per level [lev-maxmax].
    //disentangle positive and negative contours with their rotation signature.
  private:
    bool handle_args (EnvT* e)
    {
      static int irregIx = e->KeywordIx( "IRREGULAR");
      irregular=e->KeywordSet(irregIx);
      
      // in all cases, we have to exit here
      if ( nParam()==2 || nParam()>3 )
	{
	  e->Throw ( "Incorrect number of arguments." );
	}
                
      if (nParam()==1) {
	if (irregular)
	  {
	    e->Throw ( "Incorrect number of arguments." );
	  } 
	else 
	  {
	    BaseGDL* p0=e->GetNumericArrayParDefined ( 0 )->Transpose ( NULL );
	    
	    zVal=static_cast<DDoubleGDL*>
	      ( p0->Convert2 ( GDL_DOUBLE, BaseGDL::COPY ) );
	    p0_guard.Init ( p0 ); // delete upon exit
	    
	    xEl=zVal->Dim ( 1 );
	    yEl=zVal->Dim ( 0 );
	  
	    if ( zVal->Rank ( )!=2 )
	      e->Throw ( "Array must have 2 dimensions: "
			 +e->GetParString ( 0 ) );
	  
	    xVal=new DDoubleGDL ( dimension ( xEl ), BaseGDL::INDGEN );
	    xval_guard.Init ( xVal ); // delete upon exit
	    yVal=new DDoubleGDL ( dimension ( yEl ), BaseGDL::INDGEN );
	    yval_guard.Init ( yVal ); // delete upon exit
	  }
      }

      if ( nParam()==3) {
	if (irregular)
	  { 
	    //ZVal will be treated as 1 dim array and X and Y must have the same number of elements.
	    BaseGDL* p0=e->GetNumericArrayParDefined ( 0 )->Transpose ( NULL );
	    zVal=static_cast<DDoubleGDL*>
	      ( p0->Convert2 ( GDL_DOUBLE, BaseGDL::COPY ) );
	    p0_guard.Init( p0 ); // delete upon exit
	    xValTemp=e->GetParAs< DDoubleGDL>( 1 );
	    yValTemp=e->GetParAs< DDoubleGDL>( 2 );
	    
	    if (xValTemp->N_Elements() != zVal->N_Elements() )
	      e->Throw ( "X, Y, or Z array dimensions are incompatible." );
	    if (yValTemp->N_Elements() != zVal->N_Elements() )
	      e->Throw ( "X, Y, or Z array dimensions are incompatible." );
        xEl=xValTemp->N_Elements();
        yEl=yValTemp->N_Elements(); //all points inside
        xVal=xValTemp;
        yVal=yValTemp;//for the time being, will be update later
	  }
	else
	  {
	    BaseGDL* p0=e->GetNumericArrayParDefined ( 0 )->Transpose ( NULL );
	    zVal=static_cast<DDoubleGDL*>
	      ( p0->Convert2 ( GDL_DOUBLE, BaseGDL::COPY ) );
	    p0_guard.Init( p0 ); // delete upon exit

	    if ( zVal->Dim ( 0 )==1 )
	      e->Throw ( "Array must have 2 dimensions: "
			 +e->GetParString ( 0 ) );

	    xVal=e->GetParAs< DDoubleGDL>( 1 );
	    yVal=e->GetParAs< DDoubleGDL>( 2 );

	    if ( xVal->Rank ( )>2 )
	      e->Throw ( "X, Y, or Z array dimensions are incompatible." );

	    if ( yVal->Rank ( )>2 )
	      e->Throw ( "X, Y, or Z array dimensions are incompatible." );
	    if ( xVal->Rank ( )==0 || yVal->Rank ( )==0 )
	      e->Throw ( "X, Y, or Z array dimensions are incompatible." );

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

	    if ( xVal->Rank ( )==2 )
	      {
		xEl=xVal->Dim ( 0 );
		if ( ( xVal->Dim ( 0 )!=zVal->Dim ( 1 ) )&&( xVal->Dim ( 1 )!=zVal->Dim ( 0 ) ) )
		  e->Throw ( "X, Y, or Z array dimensions are incompatible." );
	      }

	    if ( yVal->Rank ( )==2 )
	      {
		yEl=yVal->Dim ( 1 );
		if ( ( yVal->Dim ( 0 )!=zVal->Dim ( 1 ) )&&( yVal->Dim ( 1 )!=zVal->Dim ( 0 ) ) )
		  e->Throw ( "X, Y, or Z array dimensions are incompatible." );
	      }
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

    void old_body (EnvT* e, GDLGStream* actStream) // {{{
    {
      // we need to define the NaN value
      static DStructGDL *Values=SysVar::Values ( );
      static DDouble d_nan=( *static_cast<DDoubleGDL*> ( Values->GetTag ( Values->Desc ( )->TagIndex ( "D_NAN" ), 0 ) ) )[0];
      static DDouble minmin=gdlAbsoluteMinValueDouble();
      static DDouble maxmax=gdlAbsoluteMaxValueDouble();
      //for 3D
      DDoubleGDL* plplot3d;
      DDouble az, alt, ay, scale;
      ORIENTATION3D axisExchangeCode;
      restorelayout=false;
      //T3D
      static int t3dIx = e->KeywordIx( "T3D");
      doT3d=(e->KeywordSet(t3dIx)|| T3Denabled(e));
      //ZVALUE
      static int zvIx = e->KeywordIx( "ZVALUE");
      DDouble zValue=0.0;
      bool hasZvalue=false;
      if( e->KeywordPresent(zvIx))
      {
        e->AssureDoubleScalarKW( zvIx, zValue );
        zValue=min(zValue,0.999999); //to avoid problems with plplot
        zValue=max(zValue,0.0);
        hasZvalue=true;
      }
      //NODATA
      static int nodataIx = e->KeywordIx( "NODATA");
      nodata=e->KeywordSet(nodataIx);
      //We could RECORD PATH this way. Not developed since PATH_INFO seems not to be used
      bool recordPath;
      static int pathinfoIx = e->KeywordIx( "PATH_INFO");
      static int pathxyIx= e->KeywordIx( "PATH_XY");
      recordPath=(e->KeywordSet(pathinfoIx)||e->KeywordSet(pathxyIx));
      if (recordPath)
      {
      //recordPath--> use actStream->stransform(myrecordingfunction, &data);
      //       actStream->stransform(myrecordingfunction, NULL);
        Warning( "PATH_INFO, PATH_XY not yet supported, (FIXME)");
        recordPath=false;
      }
//      else actStream->stransform(NULL, NULL);
      //ISOTROPIC
      DLong iso=0;
      e->AssureLongScalarKWIfPresent( "ISOTROPIC", iso);

      // [XY]STYLE
      DLong xStyle=0, yStyle=0, zStyle=0; ;
      gdlGetDesiredAxisStyle(e, "X", xStyle);
      gdlGetDesiredAxisStyle(e, "Y", yStyle);
      gdlGetDesiredAxisStyle(e, "Z", zStyle);

      // MARGIN
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

      // then only apply expansion  of axes:
      if ( ( zStyle&1 )!=1 )
      {
        PLFLT intv=AutoIntvAC ( zStart, zEnd, zLog );
      }

      //OVERPLOT: get stored range values instead to use them!
      static int overplotKW=e->KeywordIx ( "OVERPLOT" );
      overplot=e->KeywordSet(overplotKW);
      make2dBox=(!overplot&&!doT3d);
      make3dBox=(!overplot&& doT3d);

      //projection: would work only with 2D X and Y.
      bool mapSet=false;
#ifdef USE_LIBPROJ4
      static LPTYPE idata;
      static XYTYPE odata;
      get_mapset ( mapSet );
      mapSet = mapSet && overplot; 
      if ( mapSet )
      {
        ref=map_init ( );
        if ( ref==NULL ) e->Throw ( "Projection initialization failed." );
      }
#endif


      if (overplot) //retrieve information in case they are not in the command line ans apply
                    // some computation (alas)!
      {
        gdlGetAxisType("X", xLog);
        gdlGetAxisType("Y", yLog);
        gdlGetAxisType("Z", zLog);
        gdlGetCurrentAxisRange("X", xStart, xEnd);
        gdlGetCurrentAxisRange("Y", yStart, yEnd);
        gdlGetCurrentAxisRange("Z", zStart, zEnd); //we should memorize the number of levels!

        if (!doT3d) {
          restorelayout=true;
      actStream->OnePageSaveLayout(); // we'll give back actual plplot's setup at end
      
      DDouble *sx, *sy;
      GetSFromPlotStructs( &sx, &sy );

      DFloat *wx, *wy;
      GetWFromPlotStructs( &wx, &wy );

      DDouble xStart, xEnd, yStart, yEnd;
      DataCoordLimits( sx, sy, wx, wy, &xStart, &xEnd, &yStart, &yEnd, true );

      actStream->vpor( wx[0], wx[1], wy[0], wy[1] );
      actStream->wind( xStart, xEnd, yStart, yEnd );
        }
      }

      static DDouble x0,y0,xs,ys; //conversion to normalized coords
      x0=(xLog)?-log10(xStart):-xStart;
      y0=(yLog)?-log10(yStart):-yStart;
      xs=(xLog)?(log10(xEnd)-log10(xStart)):xEnd-xStart;xs=1.0/xs;
      ys=(yLog)?(log10(yEnd)-log10(yStart)):yEnd-yStart;ys=1.0/ys;

      if (!setZrange) {
        zStart=max(minVal,zStart);
        zEnd=min(zEnd,maxVal);
      }
      if(!overplot) {
        // background BEFORE next plot since it is the only place plplot may redraw the background...
        gdlSetGraphicsBackgroundColorFromKw ( e, actStream ); //BACKGROUND
        gdlNextPlotHandlingNoEraseOption(e, actStream);     //NOERASE
      }

      if(make2dBox) {       //start a plot
        // viewport and world coordinates
        // use POSITION
        static int positionIx = e->KeywordIx( "POSITION");
        DFloatGDL* boxPosition = e->IfDefGetKWAs<DFloatGDL>( positionIx);
        if (boxPosition == NULL) boxPosition = (DFloatGDL*) 0xF;
        // set the PLOT charsize before computing box, see plot command.
        gdlSetPlotCharsize(e, actStream);
        if ( gdlSetViewPortAndWorldCoordinates(e, actStream, boxPosition,
        xLog, yLog,
        xMarginL, xMarginR, yMarginB, yMarginT,
        xStart, xEnd, yStart, yEnd, iso)==FALSE ) return; //no good: should catch an exception to get out of this mess.
      }

      if (doT3d) {
        plplot3d = gdlConvertT3DMatrixToPlplotRotationMatrix( zValue, az, alt, ay, scale, axisExchangeCode);
        if (plplot3d == NULL)
        {
          e->Throw("Illegal 3D transformation. (FIXME)");
        }

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

        //necessary even if overplot
        // set the PLOT charsize before computing box, see plot command.
        gdlSetPlotCharsize(e, actStream);
        if (gdlSet3DViewPortAndWorldCoordinates(e, actStream, plplot3d, xLog, yLog,
        xStart, xEnd, yStart, yEnd, zStart, zEnd, zLog) == FALSE) return;
        //start 3D->2D coordinate conversions in plplot
        actStream->stransform(gdl3dTo2dTransformContour, &Data3d);
      }

      if ( xLog && xStart<=0.0 ) Warning ( "CONTOUR: Infinite x plot range." );
      if ( yLog && yStart<=0.0 ) Warning ( "CONTOUR: Infinite y plot range." );
      if ( zLog && zStart<=0.0 ) Warning ( "CONTOUR: Infinite z plot range." );

      // labeling
      // initiated by /FOLLOW.
      // C_ANNOTATION=vector of strings:  strings replace the default numerical values. Implies FOLLOW. Impossible with PLPLOT!
      // C_CHARSIZE. IDL doc false: c_charsize independent from /CHARSIZE. Charsize should be 3/4 size of AXIS LABELS,
      // but in fact c_charsize is independent from all [XYZ]charsize setups. Implies FOLLOW.
      // C_CHARTHICK Implies FOLLOW.
      // C_COLORS=vector, (eventually converted to integer), give color index. repated if less than contours.
      // C_LABELS=vector of 0 and 1 (float, double, int) . Implies FOLLOW.
      // C_LINESTYLE =vector of linestyles. Defaults to !P.LINESTYLE
      // C_ORIENTATION = vector of angles of lines to  FILL (needs FILL KW) .
      // C_SPACING= vector of spacing in CENTIMETRES of lines to  FILL (needs FILL KW) .
      // if C_SPACING and C_ORIENTATION absent, FILL will do a solid fill .
      // C_THICK=vector of thickness. repated if less than contours. defaults to !P.THICK or THICK

      bool label=( e->KeywordSet ( "FOLLOW" ) || e->KeywordSet ( "C_CHARSIZE" ) || e->KeywordSet("C_CHARTHICK") || e->KeywordSet("C_LABELS") );
      bool fill=( e->KeywordSet("FILL") || e->KeywordSet ("C_SPACING") || e->KeywordSet ("C_ORIENTATION") );
      if (fill) label=false; //mutually exclusive
      if (recordPath) {fill=true;}

      // managing the levels list OR the nlevels value
      // LEVELS=vector_of_values_in_increasing_order
      // NLEVELS=[1..60]
      PLINT nlevel;
      PLFLT *clevel;
      ArrayGuard<PLFLT> clevel_guard;
      static int levelsix=e->KeywordIx ( "LEVELS" );
      BaseGDL* b_levels=e->GetKW ( levelsix );
      if ( b_levels!=NULL )
      {
        DDoubleGDL* d_levels=e->GetKWAs<DDoubleGDL>( levelsix );
        nlevel=d_levels-> N_Elements ( );
        clevel=( PLFLT * ) &( *d_levels )[0];
        // are the levels ordered ?
        for ( SizeT i=1; i<nlevel; i++ )
        {
          if ( clevel[i]<=clevel[i-1] )
            e->Throw ( "Contour levels must be in increasing order." );
        }
      }
      else
      {
        PLFLT zintv;
        DDouble mapmax,mapmin;
        mapmax=(zLog)?log10(zEnd):zEnd;
        mapmin=(zLog)?log10(zStart):zStart;
        // first, compute autolevel interval...
        zintv=AutoTick ( mapmax-mapmin); //zintv never null.

        if ( e->KeywordSet ( "NLEVELS" ) )
        {
          DLong l_nlevel=nlevel; // GCC 3.4.4 needs that
          e->AssureLongScalarKWIfPresent ( "NLEVELS", l_nlevel );
          nlevel=l_nlevel;
          if ( nlevel<0) nlevel=2; //as IDL
          if (nlevel==0) nlevel=3; //idem
        }
        else
        {
          nlevel=(PLINT) floor ( ( mapmax-mapmin )/zintv );
          if ( nlevel<0 ) nlevel=1; //never happens i guess
        }
        //levels values tries to be as rounded as possible aka IDL.

        //trick to round
        DDouble cmax,cmin;
        cmax=ceil(mapmax/zintv)*zintv;
        cmin=floor(mapmin/zintv)*zintv;
        zintv=(cmax-cmin)/(nlevel+1);

        if (!setZrange) { //update CRANGE if it was rounded above.
          zStart=cmin;
          zEnd=cmax;
        }

        if (fill)
        {
          nlevel=nlevel+1;
        }
        clevel=new PLFLT[nlevel];
        clevel_guard.Reset ( clevel );
        //IDL does this:
        
        if (zLog) for( SizeT i=1; i<=nlevel; i++) clevel[i-1] = pow(10.0,(zintv * i + cmin)); 
        else      for( SizeT i=1; i<=nlevel; i++) clevel[i-1] = zintv * i + cmin;
      }

      // set label params always since they are not forgotten by plplot.'label' tells if they should be used.
      // IDL default: 3/4 of the axis charsize (CHARSIZE keyword or !P.CHARSIZE)
      // PlPlot default: .3
      // should be: DFloat label_size=.75*actStream->charScale(); however IDL doc false (?).
      DFloat label_size=0.9; //IDL behaviour, IDL doc is false, label of contours is not 3/4 of !P.CHARSIZE or CHARSIZE
      if ( e->KeywordSet ( "C_CHARSIZE" ) ) e->AssureFloatScalarKWIfPresent ( "C_CHARSIZE", label_size );
// set up after:      actStream->setcontlabelparam ( LABELOFFSET, (PLFLT) label_size, LABELSPACING, (label)?1:0 );
      DFloat label_thick=1; 
      if ( e->KeywordSet ( "C_CHARTHICK" ) ) e->AssureFloatScalarKWIfPresent ( "C_CHARTHICK", label_thick );
      actStream->setcontlabelformat (4, 3 );

      // PLOT ONLY IF NODATA=0
      if (!nodata)
      {
        //use of intermediate map for correct handling of blanking values and nans. We take advantage of the fact that
        //this program makes either filled regions with plshades() [but plshades hates Nans!] or contours with plcont,
        //which needs Nans to avoid blanked regions. The idea is to mark unwanted regions with Nans for plcont, and
        //with a blanking value (minmin) for plshade. Eventually one could use a zdefined() function testing on top of it.
        PLFLT ** map;

        if (irregular)
        {
          //x-y ranges:
          DDouble xmin,xmax,ymin,ymax;
          long xsize,ysize,xoff,yoff;        
          actStream->GetGeometry(xsize,ysize,xoff,yoff);
          GetMinMaxVal ( xValTemp, &xmin, &xmax );
          GetMinMaxVal ( yValTemp, &ymin, &ymax );
          // find a good compromise for default size of gridded map...
          ixEl=max(51.0,2*sqrt((double)xEl)+1); //preferably odd
          iyEl=max(51.0,2*sqrt((double)yEl)+1);
          ixEl=ixEl<xsize?ixEl:xsize; //no more than pixels on screen!
          iyEl=iyEl<ysize?iyEl:ysize;
          xVal=new DDoubleGDL ( dimension ( ixEl ), BaseGDL::NOZERO );
          yVal=new DDoubleGDL ( dimension ( iyEl ), BaseGDL::NOZERO );
          for(SizeT i=0; i<ixEl; ++i) (*xVal)[i]=xmin+i*(xmax-xmin)/ixEl;
          for(SizeT i=0; i<iyEl; ++i) (*yVal)[i]=ymin+i*(ymax-ymin)/iyEl;
          actStream->Alloc2dGrid( &map, ixEl, iyEl);
          PLFLT data=0;
          actStream->griddata(&(*xValTemp)[0],&(*yValTemp)[0],&(*zVal)[0],xEl,
              &(*xVal)[0],ixEl,&(*yVal)[0],iyEl,map,GRID_DTLI,data);
          for ( SizeT i=0, k=0; i<ixEl; i++ )
          {
            for ( SizeT j=0; j<iyEl; j++)
            {
              PLFLT v=map[i][j];
              if ( !isfinite(v) ) v=(fill)?minmin:d_nan; //note: nan regions could eventually be filled.
              if ( hasMinVal && v < minVal) v=(fill)?minmin:d_nan;
              if ( hasMaxVal && v > maxVal) v=(fill)?maxmax:d_nan;
              map[i][j] = v;
            }
          }
          xEl=ixEl;
          yEl=iyEl;
        }else{
          actStream->Alloc2dGrid( &map, xEl, yEl);
          for ( SizeT i=0, k=0; i<xEl; i++ )
          {
            for ( SizeT j=0; j<yEl; j++)
            {
              PLFLT v=( *zVal )[k++];
              if ( !isfinite(v) ) v=(fill)?minmin:d_nan; //note: nan regions could eventually be filled.
              if ( hasMinVal && v < minVal) v=(fill)?minmin:d_nan;
              if ( hasMaxVal && v > maxVal) v=(fill)?minmin:d_nan;
              map[i][j] = v;
            }
          }
        }
        // provision for 2 types of grids.
        PLcGrid cgrid1; // X and Y independent deformation
        PLFLT* xg1;
        PLFLT* yg1;
        PLcGrid2 cgrid2; // Dependent X Y (e.g., rotation)
        bool tidyGrid1WorldData=false;
        bool tidyGrid2WorldData=false;
        bool oneDim=true;
        // the Grids:
        // 1 DIM X & Y
        if ( xVal->Rank ( )==1&&yVal->Rank ( )==1 && !mapSet) //mapSet: must create a 2d grid 
        {
          oneDim=true;
          xg1 = new PLFLT[xEl];
          yg1 = new PLFLT[yEl];
          cgrid1.xg = xg1;
          cgrid1.yg = yg1;
          cgrid1.nx = xEl;
          cgrid1.ny = yEl;
          for ( SizeT i=0; i<xEl; i++ ) cgrid1.xg[i] = (*xVal)[i];
          for ( SizeT i=0; i<yEl; i++ ) cgrid1.yg[i] = (*yVal)[i];
          //apply plot options transformations:
          if (xLog) for ( SizeT i=0; i<xEl; i++ ) cgrid1.xg[i] = cgrid1.xg[i]>0?log10(cgrid1.xg[i]):1E-12;  // #define EXTENDED_DEFAULT_LOGRANGE 12
          if (yLog) for ( SizeT i=0; i<yEl; i++ ) cgrid1.yg[i] = cgrid1.yg[i]>0?log10(cgrid1.yg[i]):1E-12;
          tidyGrid1WorldData=true;
        }
        else //if ( xVal->Rank ( )==2&&yVal->Rank ( )==2 )
        {
          oneDim=false;

          actStream->Alloc2dGrid ( &cgrid2.xg, xEl, yEl );
          actStream->Alloc2dGrid ( &cgrid2.yg, xEl, yEl );
          tidyGrid2WorldData=true;
          cgrid2.nx=xEl;
          cgrid2.ny=yEl;
          //create 2D grid
          for ( SizeT i=0; i<xEl; i++ )
          {
            for ( SizeT j=0; j<yEl; j++ )
            {
              cgrid2.xg[i][j]=mapSet?(*xVal)[i]:(*xVal)[j*( xEl )+i];
              cgrid2.yg[i][j]=mapSet?(*yVal)[j]:(*yVal)[j*( xEl )+i];
            }
          }
          //apply projection transformations:
#ifdef USE_LIBPROJ4
          if ( mapSet )
          {
            for ( SizeT i=0; i<xEl; i++ )
            {
              for ( SizeT j=0; j<yEl; j++ )
              {
                idata.u= cgrid2.xg[i][j] * DEG_TO_RAD;
                idata.v= cgrid2.yg[i][j] * DEG_TO_RAD;
                odata=PJ_FWD ( idata, ref );
                cgrid2.xg[i][j]=odata.u;
                cgrid2.yg[i][j]=odata.v;
              }
            }
          }
#endif
          //apply plot options transformations:
          if (xLog) for ( SizeT i=0; i<xEl; i++ ) for ( SizeT j=0; j<yEl; j++ )
          {
            cgrid2.xg[i][j]=(cgrid2.xg[i][j]>0)?log10(cgrid2.xg[i][j]):1E-12;
          }
          if (yLog) for ( SizeT i=0; i<xEl; i++ ) for ( SizeT j=0; j<yEl; j++ )
          {
            cgrid2.yg[i][j]=(cgrid2.yg[i][j]>0)?log10(cgrid2.yg[i][j]):1E-12;
          }
        }

        //Colors.
        static DStructGDL* pStruct=SysVar::P();
        static DStructGDL* dStruct=SysVar::D();
        DLong n_colors=(*static_cast<DLongGDL*>(dStruct->GetTag(dStruct->Desc()->TagIndex("N_COLORS"), 0)))[0];
        static int c_colorsIx=e->KeywordIx ( "C_COLORS" ); bool docolors=false;
        // Get decomposed value for colors
        DLong decomposed=GraphicsDevice::GetDevice()->GetDecomposed();
        if ( e->GetKW ( c_colorsIx )!=NULL )
        {
          colors=e->GetKWAs<DLongGDL>( c_colorsIx ); docolors=true;
        } else {
          // Get COLOR from PLOT system variable
          colors=new DLongGDL( 1, BaseGDL::NOZERO );
	      colors_guard.Init ( colors ); // delete upon exit
          colors=static_cast<DLongGDL*>(pStruct->GetTag(pStruct->Desc()->TagIndex("COLOR"), 0)); docolors=false;
        }
        //treat decomposed problem here (simpler: colors for table 0 will already be truncated to 0xFF)
        if (!decomposed) for ( SizeT i=0; i<colors->N_Elements(); ++i ){(*colors)[i]&=0xFF;}

        static int c_linestyleIx=e->KeywordIx ( "C_LINESTYLE" ); bool dostyle=false;
        static int c_thickIx=e->KeywordIx ( "C_THICK" ); bool dothick=false;
        static int c_labelsIx=e->KeywordIx ( "C_LABELS" ); bool dolabels=false;
        static int c_orientationIx=e->KeywordIx ( "C_ORIENTATION" ); bool doori=false;
        static int c_spacingIx=e->KeywordIx ( "C_SPACING" ); bool dospacing=false;

        if ( e->GetKW ( c_thickIx )!=NULL )
        {
          thick=e->GetKWAs<DFloatGDL>( c_thickIx ); dothick=true;
        } else {
          // Get THICK from PLOT system variable
          thick=new DFloatGDL( 1, BaseGDL::NOZERO );
	      thick_guard.Init ( thick ); // delete upon exit
          static DStructGDL* pStruct=SysVar::P();
          thick=static_cast<DFloatGDL*>(pStruct->GetTag(pStruct->Desc()->TagIndex("THICK"), 0)); dothick=false;          
        }
        if ( e->GetKW ( c_labelsIx )!=NULL )
        {
          labels=e->GetKWAs<DLongGDL>( c_labelsIx ); dolabels=true;
        }
        else //every other level
        {
          labels=new DLongGDL  ( dimension (nlevel), BaseGDL::ZERO );
          labels_guard.Init( labels);
          for ( SizeT i=0; i<nlevel-1; ++i )(*labels)[i]=(i+1)%2;
          if (label) dolabels=true; //yes!
        }
        if ( e->GetKW ( c_linestyleIx )!=NULL )
        {
          style=e->GetKWAs<DLongGDL>( c_linestyleIx ); dostyle=true;
        }
        if ( e->GetKW ( c_orientationIx )!=NULL )
        {
          orientation=e->GetKWAs<DFloatGDL>( c_orientationIx ); doori=true;
        }
        else
        {
          orientation=new DFloatGDL  ( dimension (1), BaseGDL::ZERO );
          orientation_guard.Init( orientation);
          (*orientation)[0]=0;
        }
        if ( e->GetKW ( c_spacingIx )!=NULL )
        {
          spacing=e->GetKWAs<DFloatGDL>( c_spacingIx ); dospacing=true;
        }
        else
        {
          spacing=new DFloatGDL  ( dimension (1), BaseGDL::ZERO );
          spacing_guard.Init(spacing);
          (*spacing)[0]=0.25;
        }
        bool hachures=(dospacing || doori);
        // Get decomposed value for colors

        // Important: make all clipping computations BEFORE setting graphic properties (color, size)
        bool doClip=(e->KeywordSet("CLIP")||e->KeywordSet("NOCLIP"));
        bool stopClip=false;
        if ( doClip )  if ( startClipping(e, actStream, false)==TRUE ) stopClip=true;

        //provides some defaults:
        if (!docolors) gdlSetGraphicsForegroundColorFromKw ( e, actStream );
        if (!dothick) gdlSetPenThickness(e, actStream);
        gdlSetPlotCharsize(e, actStream);
        actStream->psty(0); //solid fill by default!
            
        if (fill) {
          const PLINT COLORTABLE0 = 0;
          const PLINT COLORTABLE1 = 1;
          const PLINT colorindex_table_0_color=2;
          PLFLT colorindex_table_1_color=0;
          if (hachures) {
            PLINT ori;
            PLINT spa;

            actStream->psty(1);
            // C_ORIENTATION = vector of angles of lines to  FILL (needs FILL KW) .
            // C_SPACING= vector of spacing in CENTIMETRES of lines to  FILL (needs FILL KW) .
            // if C_SPACING and C_ORIENTATION absent, FILL will do a solid fill .
            for ( SizeT i=0; i<nlevel-1; ++i ) {
              if (doT3d & !hasZvalue) {
                Data3d.zValue=clevel[i]/(zEnd-zStart);
                actStream->stransform(gdl3dTo2dTransformContour, &Data3d);
              }
              ori=floor(10.0*(*orientation)[i%orientation->N_Elements()]);
              spa=floor(10000*(*spacing)[i%spacing->N_Elements()]);
              actStream->pat(1,&ori,&spa);

              if (docolors) actStream->Color( ( *colors )[i%colors->N_Elements ( )], decomposed, colorindex_table_0_color );
              if (dothick) actStream->Thick(( *thick )[i%thick->N_Elements()]);
              if (dostyle) gdlLineStyle(actStream, ( *style )[i%style->N_Elements ( )]);
              actStream->shade( map, xEl, yEl, isLog?doIt:NULL, xStart, xEnd, yStart, yEnd,
              clevel[i], clevel[i+1],
              COLORTABLE0, colorindex_table_0_color, 
              static_cast<PLINT>(( *thick )[i%thick->N_Elements()]),
              0,0,0,0,
              (plstream::fill), (oneDim),
              (oneDim)?(plstream::tr1):(plstream::tr2), (oneDim)?(void *)&cgrid1:(void *)&cgrid2);
            }
            actStream->psty(0);
            if (docolors) gdlSetGraphicsForegroundColorFromKw( e, actStream );
            if (dothick) gdlSetPenThickness(e, actStream);
            if (dostyle) gdlLineStyle(actStream, 0);
          } //end FILL with equispaced lines
          else  if (doT3d & !hasZvalue) { //contours will be filled with solid color and displaced in Z according to their value
            for ( SizeT i=0; i<nlevel; ++i ) {
              Data3d.zValue=clevel[i]/(zEnd-zStart); //displacement in Z
              colorindex_table_1_color=(PLFLT)(i+1)/PLFLT(nlevel);
              actStream->stransform(gdl3dTo2dTransformContour, &Data3d);
                if (docolors)
                {
                  actStream->Color ( ( *colors )[i%colors->N_Elements ( )], decomposed, colorindex_table_0_color );
                  actStream->shade( map, xEl, yEl, isLog?doIt:NULL,
                  xStart, xEnd, yStart, yEnd,
                  clevel[i], maxmax, //clevel[nlevel-1],
                  COLORTABLE0, colorindex_table_0_color, 1,
                  0,0,0,0,
                  plstream::fill, (oneDim), //Onedim is accelerator since rectangles are kept rectangles see plplot doc
                  (oneDim)?(plstream::tr1):(plstream::tr2), (oneDim)?(void *)&cgrid1:(void *)&cgrid2);
                }
                else
                {
                  actStream->shade( map, xEl, yEl, isLog?doIt:NULL,
                  xStart, xEnd, yStart, yEnd,
                  clevel[i], maxmax, //clevel[nlevel-1],
                  COLORTABLE1, colorindex_table_1_color, 1,   //colorindex is a double [0.0..1.0] in case map1
                  0,0,0,0,
                  plstream::fill, (oneDim), //Onedim is accelerator since rectangles are kept rectangles see plplot doc
                  (oneDim)?(plstream::tr1):(plstream::tr2), (oneDim)?(void *)&cgrid1:(void *)&cgrid2);
                }
             }
            if (docolors) gdlSetGraphicsForegroundColorFromKw ( e, actStream );
          }
          else {
            //useful?
            gdlSetGraphicsForegroundColorFromKw ( e, actStream );
            // note that plshade is not protected against 1 level (color formula is
            // "shade_color = color_min + i / (PLFLT) ( nlevel - 2 ) * color_range;"
            // meaning that nlevel=xx must be xx>=2 for plshades to work!)
            if (nlevel>2 && !(docolors)) { //acceleration with shades when no c_colors are given. use continuous table1, decomposed or not.
              //acceleration is most sensible when a (x,y) transform (rotate, stretch) is in place since plplot does not recompute the map.
              actStream->shades( map, xEl, yEl, isLog?doIt:NULL, xStart, xEnd, yStart, yEnd,
                                clevel, nlevel, 1, 0, 0, plstream::fill, (oneDim),
                                (oneDim)?(plstream::tr1):(plstream::tr2),
                                (oneDim)?(void *)&cgrid1:(void *)&cgrid2);
            }
            else { //fill with colors defined with c_colors or n<=2
              for ( SizeT i=0; i<nlevel; ++i ) 
              {
                if (docolors) actStream->Color ( ( *colors )[i%colors->N_Elements ( )], decomposed, colorindex_table_0_color );
                actStream->shade( map, xEl, yEl, isLog?doIt:NULL,
                xStart, xEnd, yStart, yEnd,
                clevel[i], maxmax,
                COLORTABLE0,  colorindex_table_0_color , 1,
                0,0,0,0,
                plstream::fill, (oneDim), //Onedim is accelerator since rectangles are kept rectangles see plplot doc
                (oneDim)?(plstream::tr1):(plstream::tr2), (oneDim)?(void *)&cgrid1:(void *)&cgrid2);
              }
            }
          }
        } else { //no fill = contours . use normal pen procedures.
          if (!docolors) gdlSetGraphicsForegroundColorFromKw ( e, actStream );
          DFloat referencePenThickness;
          if (!dothick) {gdlSetPenThickness(e, actStream); referencePenThickness = gdlGetPenThickness(e, actStream);}
          gdlSetPlotCharsize(e, actStream);
          for ( SizeT i=0; i<nlevel; ++i ) {
            if (doT3d & !hasZvalue) {
              Data3d.zValue=clevel[i]/(zEnd-zStart);
              actStream->stransform(gdl3dTo2dTransformContour, &Data3d);
            }
            if (docolors) actStream->Color ( ( *colors )[i%colors->N_Elements ( )], decomposed, 2);
            if (dothick) { actStream->Thick (( *thick )[i%thick->N_Elements ( )]); referencePenThickness = ( *thick )[i%thick->N_Elements ( )]; }
            if (dostyle) gdlLineStyle(actStream, ( *style )[i%style->N_Elements ( )]);
            if (doT3d) { //no label in T3D , bug in plplot...
              actStream->setcontlabelparam ( LABELOFFSET, (PLFLT) label_size, LABELSPACING, 0 );
              actStream->cont ( map, xEl, yEl, 1, xEl, 1, yEl, &( clevel[i] ), 1,
                (oneDim)?(plstream::tr1):(plstream::tr2), (oneDim)?(void *)&cgrid1:(void *)&cgrid2);
            } else {
              if (dolabels && i<labels->N_Elements()) 
              {
                if ( label_thick < referencePenThickness ) { //one pass with (current) thick without labels, over with (smaller) label+contour.
                                                              //else (lables thicker than contours) impossible with plplot...
                  actStream->setcontlabelparam ( LABELOFFSET, (PLFLT) label_size, LABELSPACING, 0 ); 
                  actStream->cont ( map, xEl, yEl, 1, xEl, 1, yEl, &( clevel[i] ), 1,
                  (oneDim)?(plstream::tr1):(plstream::tr2), (oneDim)?(void *)&cgrid1:(void *)&cgrid2); //thick contours, no label
                  actStream->Thick(label_thick);
                } 
                actStream->setcontlabelparam ( LABELOFFSET, (PLFLT) label_size, LABELSPACING*sqrt(label_size),(PLINT)(*labels)[i] ); 
                actStream->cont ( map, xEl, yEl, 1, xEl, 1, yEl, &( clevel[i] ), 1,
                (oneDim)?(plstream::tr1):(plstream::tr2), (oneDim)?(void *)&cgrid1:(void *)&cgrid2);
                if (!dothick) gdlSetPenThickness(e, actStream);
              } else {
                actStream->cont ( map, xEl, yEl, 1, xEl, 1, yEl, &( clevel[i] ), 1,
                (oneDim)?(plstream::tr1):(plstream::tr2), (oneDim)?(void *)&cgrid1:(void *)&cgrid2);
              }
            }
          }
          if (docolors) gdlSetGraphicsForegroundColorFromKw ( e, actStream );
          if (dothick) gdlSetPenThickness(e, actStream);
          if (dostyle) gdlLineStyle(actStream, 0);
        }
        if (tidyGrid2WorldData)
        {
          actStream->Free2dGrid ( cgrid2.xg, xEl, yEl );
          actStream->Free2dGrid ( cgrid2.yg, xEl, yEl );
        }
        if (tidyGrid1WorldData)
        {
          delete[] xg1;
          delete[] yg1;
        }

        if (stopClip) stopClipping(actStream);
        actStream->Free2dGrid(map, xEl, yEl);
      }
      //finished? Store Zrange and Loginess unless we are overplot:
      if ( make2dBox || make3dBox )
      {
       gdlStoreAxisCRANGE("Z", zStart, zEnd, zLog);
       gdlStoreAxisType("Z",zLog);
      }

      if (doT3d) {
        actStream->stransform(NULL,NULL); //remove transform BEFORE writing axes, ticks..
      }
      //Draw axes after the data because /fill could potentially overlap the axes.
      //... if keyword "OVERPLOT" is not set
      if ( make2dBox ) //onlyplace where tick etc is relevant!
      {
        gdlSetGraphicsForegroundColorFromKw ( e, actStream ); //COLOR
        gdlBox(e, actStream, xStart, xEnd, yStart, yEnd, xLog, yLog);
      }
      if(make3dBox) {  //overplot box
        DDouble t3xStart, t3xEnd, t3yStart, t3yEnd, t3zStart, t3zEnd;
        switch (axisExchangeCode) {
          case NORMAL: //X->X Y->Y plane XY
            t3xStart=(xLog)?log10(xStart):xStart,
            t3xEnd=(xLog)?log10(xEnd):xEnd,
            t3yStart=(yLog)?log10(yStart):yStart,
            t3yEnd=(yLog)?log10(yEnd):yEnd,
            t3zStart=0;
            t3zEnd=1.0;
            actStream->w3d(scale, scale, scale*(1.0 - zValue),
            t3xStart,t3xEnd,t3yStart,t3yEnd,t3zStart,t3zEnd,
            alt, az);
            gdlAxis3(e, actStream, "X", xStart, xEnd, xLog);
            gdlAxis3(e, actStream, "Y", yStart, yEnd, yLog);
            break;
          case XY: // X->Y Y->X plane XY
            t3yStart=(xLog)?log10(xStart):xStart,
            t3yEnd=(xLog)?log10(xEnd):xEnd,
            t3xStart=(yLog)?log10(yStart):yStart,
            t3xEnd=(yLog)?log10(yEnd):yEnd,
            t3zStart=0;
            t3zEnd=1.0;
            actStream->w3d(scale, scale, scale*(1.0 - zValue),
            t3xStart,t3xEnd,t3yStart,t3yEnd,t3zStart,t3zEnd,
            alt, az);
            gdlAxis3(e, actStream, "Y", xStart, xEnd, xLog);
            gdlAxis3(e, actStream, "X", yStart, yEnd, yLog);
            break;
          case XZ: // Y->Y X->Z plane YZ
            t3zStart=(xLog)?log10(xStart):xStart,
            t3zEnd=(xLog)?log10(xEnd):xEnd,
            t3yStart=(yLog)?log10(yStart):yStart,
            t3yEnd=(yLog)?log10(yEnd):yEnd,
            t3xStart=0;
            t3xEnd=1.0;
            actStream->w3d(scale, scale, scale,
            t3xStart,t3xEnd,t3yStart,t3yEnd,t3zStart,t3zEnd,
            alt, az);
            gdlAxis3(e, actStream, "Z", xStart, xEnd, xLog, 0);
            gdlAxis3(e, actStream, "Y", yStart, yEnd, yLog);
            break;
          case YZ: // X->X Y->Z plane XZ
            t3xStart=(xLog)?log10(xStart):xStart,
            t3xEnd=(xLog)?log10(xEnd):xEnd,
            t3zStart=(yLog)?log10(yStart):yStart,
            t3zEnd=(yLog)?log10(yEnd):yEnd,
            t3yStart=0;
            t3yEnd=1.0;
            actStream->w3d(scale, scale, scale,
            t3xStart,t3xEnd,t3yStart,t3yEnd,t3zStart,t3zEnd,
            alt, az);
            gdlAxis3(e, actStream, "X", xStart, xEnd, xLog);
            gdlAxis3(e, actStream, "Z", yStart, yEnd, yLog,1);
            break;
          case XZXY: //X->Y Y->Z plane YZ
            t3yStart=(xLog)?log10(xStart):xStart,
            t3yEnd=(xLog)?log10(xEnd):xEnd,
            t3zStart=(yLog)?log10(yStart):yStart,
            t3zEnd=(yLog)?log10(yEnd):yEnd,
            t3xStart=0;
            t3xEnd=1.0;
            actStream->w3d(scale, scale, scale,
            t3xStart,t3xEnd,t3yStart,t3yEnd,t3zStart,t3zEnd,
            alt, az);
            gdlAxis3(e, actStream, "Y", xStart, xEnd, xLog);
            gdlAxis3(e, actStream, "Z", yStart, yEnd, yLog);
            break;
          case XZYZ: //X->Z Y->X plane XZ
            t3zStart=(xLog)?log10(xStart):xStart,
            t3zEnd=(xLog)?log10(xEnd):xEnd,
            t3xStart=(yLog)?log10(yStart):yStart,
            t3xEnd=(yLog)?log10(yEnd):yEnd,
            t3yStart=0;
            t3yEnd=1.0;
            actStream->w3d(scale, scale, scale,
            t3xStart,t3xEnd,t3yStart,t3yEnd,t3zStart,t3zEnd,
            alt, az);
            gdlAxis3(e, actStream, "Z", xStart, xEnd, xLog,1);
            gdlAxis3(e, actStream, "X", yStart, yEnd, yLog);
            break;
        }
        // title and sub title
        gdlWriteTitleAndSubtitle(e, actStream);
      }
    }

  private:

    void call_plplot (EnvT* e, GDLGStream* actStream)
    {
    }

  private:

    virtual void post_call (EnvT*, GDLGStream* actStream)
    {
      if (restorelayout) actStream->RestoreLayout();
      actStream->lsty(1);//reset linestyle
      actStream->sizeChar(1.0);
    }

  }; // contour_call class

  void contour (EnvT* e)
  {
    contour_call contour;
    contour.call ( e, 1 );
  }

} // namespace
