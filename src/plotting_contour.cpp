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
#define LABELSPACING 0.25

namespace lib
{

  using namespace std;

// shared parameter
  static bool xLog;
  static bool yLog;

//  void myfill( PLINT n, PLFLT *x, PLFLT *y )
  // a possible implementation of path_recording. However, using our
  // own contouring function could be easier.
  void myfill( PLINT n, const PLFLT *x, const PLFLT *y )
  {
    static int count=0;
    count++;
    for (SizeT i=0; i<n; ++i)
    {
     fprintf(stderr,"%d %f %f\n",count, x[i],y[i]);
    }
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
    SizeT xEl, yEl, zEl;
    DDouble xStart, xEnd, yStart, yEnd, zStart, zEnd, datamax, datamin;
    bool zLog, isLog;
    bool overplot, nodata;
    DLongGDL *colors,*thick,*labels,*style;
    Guard<BaseGDL> colors_guard,thick_guard,labels_guard,style_guard;
    DFloatGDL *spacing,*orientation;
    Guard<BaseGDL> spacing_guard,orientation_guard;
  private:
    bool handle_args (EnvT* e)
    {
      if ( nParam ( )==1 )
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
      else if ( nParam ( )==2||nParam ( )>3 )
      {
        e->Throw ( "Incorrect number of arguments." );
      }
      else
      {
        //we should chek differently for presence of IRREGULAR KW however this one is not yet supported.
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
        if ( xVal->Rank ( )==0 || yVal->Rank ( )==0 ) e->Throw ( "X, Y, or Z array dimensions are incompatible." );

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

    void old_body (EnvT* e, GDLGStream* actStream) // {{{
    {
      // we need to define the NaN value
      static DStructGDL *Values=SysVar::Values ( );
      static DDouble d_nan=( *static_cast<DDoubleGDL*> ( Values->GetTag ( Values->Desc ( )->TagIndex ( "D_NAN" ), 0 ) ) )[0];
      static DDouble minmin=gdlAbsoluteMinValueDouble();

      //projection: would work only with 2D X and Y.
      bool mapSet=false;
#ifdef USE_LIBPROJ4
      static LPTYPE idata;
      static XYTYPE odata;
      static PROJTYPE* ref;
      get_mapset ( mapSet );
      if ( mapSet )
      {
        ref=map_init ( );
        if ( ref==NULL ) e->Throw ( "Projection initialization failed." );
      }
#endif
     //NODATA
      int nodataIx = e->KeywordIx( "NODATA");
      nodata=e->KeywordSet(nodataIx);
      //We could RECORD PATH this way. Not developed since PATH_INFO seems not to be used
      bool recordPath;
      int pathinfoIx = e->KeywordIx( "PATH_INFO");
      int pathxyIx= e->KeywordIx( "PATH_XY");
      recordPath=(e->KeywordSet(pathinfoIx)||e->KeywordSet(pathxyIx));
      if (recordPath)
      {
        Warning( "PATH_INFO, PATH_XY not yet supported, (FIXME)");
        recordPath=false;
      }

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

      if ( minVal>zEnd || maxVal<zStart) nodata=true; //do not complain but do nothing.
      if ( minVal>zStart ) zStart=minVal;
      if ( maxVal<zEnd ) zEnd=maxVal;

      // then only apply expansion  of axes:
      if ( ( zStyle&1 )!=1 )
      {
        PLFLT intv=AutoIntvAC ( zStart, zEnd, zLog );
      }
      //OVERPLOT: get stored range values instead to use them!
      static int overplotKW=e->KeywordIx ( "OVERPLOT" );
      overplot=e->KeywordSet(overplotKW);
      if (overplot) //retrieve information in case they are not in the command line ans apply
                    // some computation (alas)!
      {
        gdlGetAxisType("X", xLog);
        gdlGetAxisType("Y", yLog);
        gdlGetAxisType("Z", zLog);
        gdlGetCurrentAxisRange("X", xStart, xEnd);
        gdlGetCurrentAxisRange("Y", yStart, yEnd);
        gdlGetCurrentAxisRange("Z", zStart, zEnd); //we should memorize the number of levels!
        if ( ( xStyle&1 )!=1 )
        {
          PLFLT intv=AutoIntvAC ( xStart, xEnd, xLog );
        }

        if ( ( yStyle&1 )!=1 )
        {
          PLFLT intv=AutoIntvAC ( yStart, yEnd, yLog );
        }

      }
      //start a plot
      if(!overplot)
      {
          // background BEFORE next plot since it is the only place plplot may redraw the background...
          gdlSetGraphicsBackgroundColorFromKw ( e, actStream ); //BACKGROUND
          gdlNextPlotHandlingNoEraseOption(e, actStream);     //NOERASE
      }
      // viewport and world coordinates
      // use POSITION
      int positionIx = e->KeywordIx( "POSITION");
      DFloatGDL* boxPosition = e->IfDefGetKWAs<DFloatGDL>( positionIx);
      if (boxPosition == NULL) boxPosition = (DFloatGDL*) 0xF;
      // set the PLOT charsize before computing box, see plot command.
      gdlSetPlotCharsize(e, actStream);
      if ( !overplot ) if ( gdlSetViewPortAndWorldCoordinates(e, actStream, boxPosition,
        xLog, yLog,
        xMarginL, xMarginR, yMarginB, yMarginT,
        xStart, xEnd, yStart, yEnd, iso)==FALSE ) return; //no good: should catch an exception to get out of this mess.

      gdlSetPlotCharthick(e,actStream); //impossible with plplot to draw labels without axes, so both will have same thickness.

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
        // Jo: added keyword NLEVELS
        if ( e->KeywordSet ( "NLEVELS" ) )
        {
          DLong l_nlevel=nlevel; // GCC 3.4.4 needs that
          e->AssureLongScalarKWIfPresent ( "NLEVELS", l_nlevel );
          nlevel=l_nlevel;
          if ( nlevel<0) nlevel=2; //as IDL 
          if (nlevel==0) nlevel=3; //idem

          // cokhavim: IDL does this...
          zintv=(PLFLT) ( ( zEnd-zStart )/( nlevel+1 ) );
          //gd: we can support ZLOG as well:
          if (zLog) zintv=(PLFLT) ( ( log10(zEnd)-log10(zStart )) / ( nlevel+1 ) );

        }
        else
        {
          zintv=AutoTick ( zEnd-zStart ); if (zLog) zintv=AutoTick ( log10(zEnd)-log10(zStart) );
          nlevel=(PLINT) floor ( ( zEnd-zStart )/zintv ); if (zLog) nlevel=(PLINT) floor ( ( log10(zEnd)-log10(zStart) )/zintv );
          // SA: sanity check to prevent segfaults, e.g. with solely non-finite values
          if ( zintv==0||nlevel<0 ) nlevel=0;
        }

        if (fill)
        {
          nlevel=nlevel+1;
        }
        clevel=new PLFLT[nlevel]; 
        clevel_guard.Reset ( clevel );
        //IDL does this:
        for( SizeT i=1; i<=nlevel; i++) clevel[i-1] = zintv * i + zStart;
        if (zLog) for( SizeT i=1; i<=nlevel; i++) clevel[i-1] = pow(10.0,(log10(zStart)+zintv*i));
      }

      // set label params always since they are not forgotten by plplot.'label' tells if they should be used.
      // IDL default: 3/4 of the axis charsize (CHARSIZE keyword or !P.CHARSIZE)
      // PlPlot default: .3
      // should be: DFloat label_size=.75*actStream->charScale(); however IDL doc false.
      DFloat label_size=0.9; //IDL behaviour, IDL doc false.
      if ( e->KeywordSet ( "C_CHARSIZE" ) ) e->AssureFloatScalarKWIfPresent ( "C_CHARSIZE", label_size );
      actStream->setcontlabelparam ( LABELOFFSET, (PLFLT) label_size, LABELSPACING, (PLINT)label );
      actStream->setcontlabelformat (3, 3 );

      // PLOT ONLY IF NODATA=0
      if (!nodata)
      {
        //use of intermediate map for correct handling of blanking values and nans. We take advantage of the fact that
        //this program makes either filled regions with plshades() [but plshades hates Nans!] or contours with plcont,
        //which needs Nans to avoid blanked regions. The idea is to mark unwanted regions with Nans for plcont, and
        //with a blanking value (minmin) for plshade. Eventually one could use a zdefined() function testing on top of it.
        PLFLT ** map;
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
        if ( xVal->Rank ( )==1&&yVal->Rank ( )==1 )
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
          DDouble z;
          //create 2D grid
          for ( SizeT i=0; i<xEl; i++ )
          {
            for ( SizeT j=0; j<yEl; j++ )
            {
              cgrid2.xg[i][j]=(*xVal)[j*( xEl )+i];
              cgrid2.yg[i][j]=(*yVal)[j*( xEl )+i];
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
                idata.lam= cgrid2.xg[i][j] * DEG_TO_RAD;
                idata.phi= cgrid2.yg[i][j] * DEG_TO_RAD;
                odata=PJ_FWD ( idata, ref );
                cgrid2.xg[i][j]=odata.x;
                cgrid2.yg[i][j]=odata.y;
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

        // Graphic options
        // C_COLORS=vector, (eventually converted to integer), give color index. repated if less than contours.
        // C_LABELS=vector of 0 and 1 (float, double, int) . Implies FOLLOW.
        // C_LINESTYLE =vector of linestyles. Defaults to !P.LINESTYLE
        // C_THICK=vector of thickness. repated if less than contours. defaults to !P.THICK or THICK
        int c_colorsIx=e->KeywordIx ( "C_COLORS" ); bool docolors=false;
        int c_linestyleIx=e->KeywordIx ( "C_LINESTYLE" ); bool dostyle=false;
        int c_thickIx=e->KeywordIx ( "C_THICK" ); bool dothick=false;
        int c_labelsIx=e->KeywordIx ( "C_LABELS" ); bool dolabels=false;
        int c_orientationIx=e->KeywordIx ( "C_ORIENTATION" ); bool doori=false;
        int c_spacingIx=e->KeywordIx ( "C_SPACING" ); bool dospacing=false;
        if ( e->GetKW ( c_colorsIx )!=NULL )
        {
          colors=e->GetKWAs<DLongGDL>( c_colorsIx ); docolors=true;
        }
        if ( e->GetKW ( c_thickIx )!=NULL )
        {
          thick=e->GetKWAs<DLongGDL>( c_thickIx ); dothick=true;
        }
        if ( e->GetKW ( c_labelsIx )!=NULL )
        {
          labels=e->GetKWAs<DLongGDL>( c_labelsIx ); dolabels=true;
        }
        else //every other level
        {
          labels=new DLongGDL  ( dimension (2), BaseGDL::ZERO );
          labels_guard.Init( labels);
          (*labels)[0]=1;(*labels)[1]=0;
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

        // Important: make all clipping computations BEFORE setting graphic properties (color, size)
        bool doClip=(e->KeywordSet("CLIP")||e->KeywordSet("NOCLIP"));
        bool stopClip=false;
        if ( doClip )  if ( startClipping(e, actStream, false)==TRUE ) stopClip=true;

        if (fill)
        {
          if (hachures)
          {
            PLINT ori;
            PLINT spa;
            actStream->psty(1);
            // C_ORIENTATION = vector of angles of lines to  FILL (needs FILL KW) .
            // C_SPACING= vector of spacing in CENTIMETRES of lines to  FILL (needs FILL KW) .
            // if C_SPACING and C_ORIENTATION absent, FILL will do a solid fill .
            for ( SizeT i=0; i<nlevel-1; ++i )
            {
              ori=floor(10.0*(*orientation)[i%orientation->N_Elements()]);
              spa=floor(10000*(*spacing)[i%spacing->N_Elements()]);
              actStream->pat(1,&ori,&spa);

              if (docolors) actStream->Color ( ( *colors )[i%colors->N_Elements ( )], true, 2 );
              if (dothick) actStream->wid ( ( *thick )[i%thick->N_Elements ( )]);
              if (dostyle) gdlLineStyle(actStream, ( *style )[i%style->N_Elements ( )]);
              actStream->shade( map, xEl, yEl, isLog?doIt:NULL, xStart, xEnd, yStart, yEnd,
              clevel[i], clevel[i+1],
              0, 2, 1,  /* we should use the colormap here, not with ->Color above . Idem for width.*/
              0,0,0,0,
              /*(recordPath)?(myfill):*/
                  (plstream::fill), (oneDim),  //example of possible use of recordpath.
              (oneDim)?(plstream::tr1):(plstream::tr2), (oneDim)?(void *)&cgrid1:(void *)&cgrid2);
            }
            actStream->psty(0);
            if (docolors) gdlSetGraphicsForegroundColorFromKw ( e, actStream );
            if (dothick) gdlSetPenThickness(e, actStream);
            if (dostyle) gdlLineStyle(actStream, 0);
          }
          else
          {
            //useful?
            gdlSetGraphicsForegroundColorFromKw ( e, actStream );
            // note that plshade is not protected against 1 level (color formula is
            // "shade_color = color_min + i / (PLFLT) ( nlevel - 2 ) * color_range;"
            // meaning that nlevel must be >2 for plshade!)
            if (nlevel>2)
            {
              actStream->shades( map, xEl, yEl, isLog?doIt:NULL, xStart, xEnd, yStart, yEnd,
                                clevel, nlevel, 1, 0, 0, plstream::fill, (oneDim),
                                (oneDim)?(plstream::tr1):(plstream::tr2),
                                (oneDim)?(void *)&cgrid1:(void *)&cgrid2);
            }
            else
            {
              actStream->shade( map, xEl, yEl, isLog?doIt:NULL,
              xStart, xEnd, yStart, yEnd,
              clevel[0], clevel[1],
              1, 0.5, 1,     /* we should use the colormap here, not with ->Color above . Idem for width.*/
              0,0,0,0,
              plstream::fill, (oneDim), //Onedim is accelerator since rectangles are kept rectangles see plplot doc
              (oneDim)?(plstream::tr1):(plstream::tr2), (oneDim)?(void *)&cgrid1:(void *)&cgrid2);
            }
            //useful?
            gdlSetGraphicsForegroundColorFromKw ( e, actStream ); //needs to be called again or else PS files look wrong
          }
        }
        else
        { 
          //useful?
          gdlSetGraphicsForegroundColorFromKw ( e, actStream );
          gdlSetPenThickness(e, actStream); 
          gdlSetPlotCharsize(e, actStream);
          for ( SizeT i=0; i<nlevel; ++i )
          {
            if (docolors) actStream->Color ( ( *colors )[i%colors->N_Elements ( )], true, 2 );
            if (dothick) actStream->wid ( ( *thick )[i%thick->N_Elements ( )]);
            if (dostyle) gdlLineStyle(actStream, ( *style )[i%style->N_Elements ( )]);
            if (dolabels) actStream->setcontlabelparam ( LABELOFFSET, (PLFLT) label_size, LABELSPACING,
                                                        (PLINT)(*labels)[i%labels->N_Elements()] );
            actStream->cont ( map, xEl, yEl, 1, xEl, 1, yEl, &( clevel[i] ), 1, (oneDim)?(plstream::tr1):(plstream::tr2), (oneDim)?(void *)&cgrid1:(void *)&cgrid2);
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
      if ( !overplot )
      {
       gdlStoreAxisCRANGE("Z", zStart, zEnd, zLog);
       gdlStoreAxisType("Z",zLog);
      }


      //Draw axes after the data because /fill could potentially overlap the axes.
      //... if keyword "OVERPLOT" is not set
      if ( !overplot ) //onlyplace where tick etc is relevant!
      {
        gdlSetGraphicsForegroundColorFromKw ( e, actStream ); //COLOR
        gdlBox(e, actStream, xStart, xEnd, yStart, yEnd, xLog, yLog);
      }
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

  }; // contour_call class

  void contour (EnvT* e)
  {
    contour_call contour;
    contour.call ( e, 1 );
  }

} // namespace
