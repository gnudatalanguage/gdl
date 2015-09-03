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

#ifdef _MSC_VER
#define isnan _isnan
#endif

namespace lib {

  using namespace std;
//  using std::isinf;
#ifndef _MSC_VER
  using std::isnan;
#endif

  class oplot_call : public plotting_routine_call 
  {
    DDoubleGDL *yVal, *xVal, *xTemp, *yTemp;
    SizeT xEl, yEl, zEl;
    DDouble minVal, maxVal, xStart, xEnd, yStart, yEnd, zValue;
    bool doMinMax;
    bool xLog, yLog, wasBadxLog, wasBadyLog;
    Guard<BaseGDL> xval_guard, yval_guard, xtemp_guard;
    DLong iso;
    bool doT3d;

private:

  bool handle_args(EnvT* e) 
    {

      //T3D?
      static int t3dIx = e->KeywordIx( "T3D");
      doT3d=(e->KeywordSet(t3dIx)|| T3Denabled(e));

      //note: Z (VALUE) will be used uniquely if Z is not effectively defined.
      static int zvIx = e->KeywordIx( "ZVALUE");
      zValue=0.0;
      e->AssureDoubleScalarKWIfPresent ( zvIx, zValue );
      zValue=min(zValue,0.999999); //to avoid problems with plplot
      zValue=max(zValue,0.0);

    // system variable !P.NSUM first
      DLong nsum=(*static_cast<DLongGDL*>(SysVar::P()-> GetTag(SysVar::P()->Desc()->TagIndex("NSUM"), 0)))[0];
      e->AssureLongScalarKWIfPresent( "NSUM", nsum);

      bool polar = (e->KeywordSet("POLAR"));

//    DDoubleGDL *yValBis, *xValBis;
//    Guard<BaseGDL> xvalBis_guard, yvalBis_guard;
      //test and transform eventually if POLAR and/or NSUM!
      if( nParam() == 1)
      {
        yTemp = e->GetParAs< DDoubleGDL>( 0);
        if (yTemp->Rank() == 0)
          e->Throw("Expression must be an array in this context: "+e->GetParString(0));
        yEl=yTemp->N_Elements();
        xEl=yEl;
        xTemp = new DDoubleGDL( dimension( xEl), BaseGDL::INDGEN);
        xtemp_guard.Reset( xTemp); // delete upon exit
      }
      else
      {
        xTemp = e->GetParAs< DDoubleGDL>( 0);
        if (xTemp->Rank() == 0)
          e->Throw("Expression must be an array in this context: "+e->GetParString(0));
        xEl=xTemp->N_Elements();
        yTemp = e->GetParAs< DDoubleGDL>( 1);
        if (yTemp->Rank() == 0)
          e->Throw("Expression must be an array in this context: "+e->GetParString(1));
        yEl=yTemp->N_Elements();
        //silently drop unmatched values
        if (yEl != xEl)
        {
          SizeT size;
          size = min(xEl, yEl);
          xEl = size;
          yEl = size;
        }
      }
      //check nsum validity
      nsum=max(1,nsum);
      nsum=min(nsum,(DLong)xEl);

      if (nsum == 1)
      {
        if (polar)
        {
          xVal = new DDoubleGDL(dimension(xEl), BaseGDL::NOZERO);
          xval_guard.Reset(xVal); // delete upon exit
          yVal = new DDoubleGDL(dimension(yEl), BaseGDL::NOZERO);
          yval_guard.Reset(yVal); // delete upon exit
          for (int i = 0; i < xEl; i++) (*xVal)[i] = (*xTemp)[i] * cos((*yTemp)[i]);
          for (int i = 0; i < yEl; i++) (*yVal)[i] = (*xTemp)[i] * sin((*yTemp)[i]);
        }
        else
        { //careful about previously set autopointers!
          if (nParam() == 1) xval_guard.Init( xtemp_guard.release());
          xVal = xTemp;
          yVal = yTemp;
        }
      }
      else
      {
        int i, j, k;
        DLong size = (DLong)xEl / nsum;
        xVal = new DDoubleGDL(size, BaseGDL::ZERO); //SHOULD BE ZERO, IS NOT!
        xval_guard.Reset(xVal); // delete upon exit
        yVal = new DDoubleGDL(size, BaseGDL::ZERO); //IDEM
        yval_guard.Reset(yVal); // delete upon exit
        for (i = 0, k = 0; i < size; i++)
        {
          (*xVal)[i] = 0.0;
          (*yVal)[i] = 0.0;
          for (j = 0; j < nsum; j++, k++)
          {
            (*xVal)[i] += (*xTemp)[k];
            (*yVal)[i] += (*yTemp)[k];
          }
        }
        for (i = 0; i < size; i++) (*xVal)[i] /= nsum;
        for (i = 0; i < size; i++) (*yVal)[i] /= nsum;

        if (polar)
        {
          DDouble x, y;
          for (i = 0; i < size; i++)
          {
            x = (*xVal)[i] * cos((*yVal)[i]);
            y = (*xVal)[i] * sin((*yVal)[i]);
            (*xVal)[i] = x;
            (*yVal)[i] = y;
          }
        }
      }
    return false;
    }

  private: void old_body( EnvT* e, GDLGStream* actStream) 
  {
    DLong psym;

    // get ![XY].CRANGE
    DDouble xStart, xEnd, yStart, yEnd;
    gdlGetCurrentAxisRange("X", xStart, xEnd, FALSE); //we want screen coordinates, not projection boundaries
    gdlGetCurrentAxisRange("Y", yStart, yEnd, FALSE);
    DDouble minVal, maxVal;
    bool doMinMax;

    bool xLog;
    bool yLog;
    gdlGetAxisType("X", xLog);
    gdlGetAxisType("Y", yLog);

    if ((yStart == yEnd) || (xStart == xEnd))
    {
      if (yStart != 0.0 && yStart == yEnd) {
        Message("OPLOT: !Y.CRANGE ERROR, setting to [0,1]");
        yStart = 0; 
        yEnd = 1; 
      }
      if (xStart != 0.0 && xStart == xEnd) {
        Message("OPLOT: !X.CRANGE ERROR, setting to [0,1]");
        xStart = 0;
        xEnd = 1;
      }
    }
    
    //now we can setup minVal and maxVal to defaults: Start-End and overload if KW present

    minVal = yStart; //to give a reasonable value...
    maxVal = yEnd;   //idem
    doMinMax = false; //although we will not use it...
    if( e->KeywordSet( "MIN_VALUE") || e->KeywordSet( "MAX_VALUE"))
      doMinMax = true; //...unless explicitely required
    e->AssureDoubleScalarKWIfPresent( "MIN_VALUE", minVal);
    e->AssureDoubleScalarKWIfPresent( "MAX_VALUE", maxVal);

    int noclipvalue=0;
    e->AssureLongScalarKWIfPresent( "NOCLIP", noclipvalue);
    // Clipping is enabled by default for OPLOT.
    // make all clipping computations BEFORE setting graphic properties (color, size)
    bool doClip=(e->KeywordSet("CLIP")||noclipvalue==1);
    bool stopClip=false;
    if ( doClip )  if ( startClipping(e, actStream, false)==TRUE ) stopClip=true;

    // start drawing. Graphic Keywords accepted:CLIP(YES), COLOR(YES), LINESTYLE(YES), NOCLIP(YES),
    //                                          PSYM(YES), SYMSIZE(YES), T3D(YES), ZVALUE(YES)
    gdlSetGraphicsForegroundColorFromKw(e, actStream);
    gdlGetPsym(e, psym);
    gdlSetPenThickness(e, actStream);
    gdlSetSymsize(e, actStream);
    gdlSetLineStyle(e, actStream);

    bool mapSet=false;
#ifdef USE_LIBPROJ4
    get_mapset(mapSet);
    if ( mapSet )
    {
      ref=map_init();
      if ( ref==NULL )
      {
        e->Throw("Projection initialization failed.");
      }
    }
#endif

    DDouble *sx, *sy;
    GetSFromPlotStructs( &sx, &sy );
    
    DFloat *wx, *wy;
    GetWFromPlotStructs( &wx, &wy );
    
    DDouble pxStart, pxEnd, pyStart, pyEnd;
    DataCoordLimits( sx, sy, wx, wy, &pxStart, &pxEnd, &pyStart, &pyEnd, true );
    actStream->vpor( wx[0], wx[1], wy[0], wy[1] );
    actStream->wind( pxStart, pxEnd, pyStart, pyEnd );

    if ( doT3d ) //convert X,Y,Z in X',Y' as per T3D perspective.
    {
      DDoubleGDL* plplot3d;
      DDouble az, alt, ay, scale;
      ORIENTATION3D axisExchangeCode;

      plplot3d = gdlConvertT3DMatrixToPlplotRotationMatrix( zValue, az, alt, ay, scale, axisExchangeCode);
      if (plplot3d == NULL)
      {
        e->Throw("Illegal 3D transformation. (FIXME)");
      }
      
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

#ifdef USE_LIBPROJ4
        if ( mapSet && psym < 1) {
          GDLgrProjectedPolygonPlot(e, actStream, ref, NULL, xVal, yVal, false, false, NULL);
          psym=-psym;
          if (psym > 0) bool valid=draw_polyline(e, actStream, xVal, yVal, 0.0, 0.0, false, xLog, yLog, psym, FALSE);
        }
        else 
          bool valid=draw_polyline(e, actStream, xVal, yVal, 0.0, 0.0, false, xLog, yLog, psym, FALSE);
#else
    bool valid=draw_polyline(e, actStream, xVal, yVal, minVal, maxVal, doMinMax, xLog, yLog, psym, FALSE);
#endif
    if (stopClip) stopClipping(actStream);
  } 

    private: void call_plplot(EnvT* e, GDLGStream* actStream) 
    {
    } 

    private: void post_call(EnvT* e, GDLGStream* actStream)
    {
     if (doT3d) actStream->stransform(NULL,NULL);
      actStream->lsty(1);//reset linestyle
      actStream->sizeChar(1.0);
    } 

  }; // oplot_call class 

  void oplot(EnvT* e)
  {
    oplot_call oplot;
    oplot.call(e, 1);
  }

} // namespace

