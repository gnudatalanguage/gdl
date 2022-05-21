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

namespace lib {

  using namespace std;

  class oplot_call : public plotting_routine_call 
  {
    DDoubleGDL *yVal, *xVal, *xTemp, *yTemp;
    SizeT xEl, yEl, zEl;
    DDouble minVal, maxVal, xStart, xEnd, yStart, yEnd, zValue, zStart, zEnd;
    bool doMinMax;
    bool xLog, yLog;
    bool restorelayout;
    Guard<BaseGDL> xval_guard, yval_guard, xtemp_guard;
    DLong iso;
    bool doT3d;

private:

  bool handle_args(EnvT* e) 
    {
      //T3D?
      static int t3dIx = e->KeywordIx( "T3D");
      doT3d=(e->BooleanKeywordSet(t3dIx)|| T3Denabled());

      //note: Z (VALUE) will be used uniquely if Z is not effectively defined.
      static int zvIx = e->KeywordIx( "ZVALUE");
      zValue=0.0;
      if (doT3d) {
      e->AssureDoubleScalarKWIfPresent ( zvIx, zValue );
        zValue=min(zValue,ZVALUEMAX); //to avoid problems with plplot
        zValue = max(zValue, 0.0);
      }
    // system variable !P.NSUM first
      DLong nsum=(*static_cast<DLongGDL*>(SysVar::P()-> GetTag(SysVar::P()->Desc()->TagIndex("NSUM"), 0)))[0];
      static int NSUMIx = e->KeywordIx( "NSUM");
      e->AssureLongScalarKWIfPresent( NSUMIx, nsum);

      static int polarIx = e->KeywordIx( "POLAR");
      bool polar = (e->KeywordSet(polarIx));

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

    DDouble minVal, maxVal;
    bool doMinMax;
    gdlGetAxisType(XAXIS, xLog);
    gdlGetAxisType(YAXIS, yLog);

   gdlGetCurrentAxisRange(XAXIS, xStart, xEnd); 
   gdlGetCurrentAxisRange(YAXIS, yStart, yEnd);

    actStream->OnePageSaveLayout();
    
   //CLIPPING (or not) is just defining the adequate viewport and world coordinates, all of them normalized since this is what plplot will get in the end.
      static int NOCLIPIx = e->KeywordIx("NOCLIP");
      bool noclip = e->BooleanKeywordSet(NOCLIPIx);
      // Clipping is enabled by default for OPLOT.
      int CLIP = e->KeywordIx("CLIP");
      bool doClip=(e->KeywordSet(CLIP) && !(noclip));

      if (doClip) { //by Default
        //define a default clipbox (DATA coords):
        DDouble clipBox[4]={xStart,yStart,xEnd,yEnd};
        DFloatGDL* clipBoxGDL = e->IfDefGetKWAs<DFloatGDL>(CLIP);
        if (clipBoxGDL != NULL && clipBoxGDL->N_Elements()< 4) for (auto i=0; i<4; ++i) clipBox[i]=0; //set clipbox to 0 0 0 0 apparently this is what IDL does.
        if (clipBoxGDL != NULL && clipBoxGDL->N_Elements()==4) for (auto i=0; i<4; ++i) clipBox[i]=(*clipBoxGDL)[i];

        //convert clipBox to normalized coordinates:
        PLFLT xnormmin,xnormmax,ynormmin,ynormmax;
        actStream->WorldToNormedDevice(clipBox[0],clipBox[1],xnormmin,ynormmin);
        actStream->WorldToNormedDevice(clipBox[2],clipBox[3],xnormmax,ynormmax);
        actStream->vpor(xnormmin,xnormmax,ynormmin,ynormmax);
        actStream->wind(xnormmin,xnormmax,ynormmin,ynormmax);
      } else {
        actStream->vpor(0, 1, 0, 1);
        actStream->wind(0, 1, 0, 1);
      }


    minVal = yStart; //to give a reasonable value...
    maxVal = yEnd;   //idem
    doMinMax = false; //although we will not use it...
    static int MIN_VALUEIx = e->KeywordIx("MIN_VALUE");
    static int MAX_VALUEIx = e->KeywordIx("MAX_VALUE");
    if( e->KeywordSet(MIN_VALUEIx) || e->KeywordSet(MAX_VALUEIx)) doMinMax = true; //...unless explicitely required
    e->AssureDoubleScalarKWIfPresent( MIN_VALUEIx, minVal);
    e->AssureDoubleScalarKWIfPresent( MAX_VALUEIx, maxVal);

    // final coordinates conversions (in NORM) before plotting
    bool mapSet = false;
#ifdef USE_LIBPROJ
      get_mapset(mapSet);
#endif
      COORDSYS coordinateSystem=DATA;
      SelfConvertToNormXY(xEl, (PLFLT*) xVal->DataAddr(), xLog, (PLFLT*) yVal->DataAddr(), yLog, coordinateSystem);
      //convert minval, maxVal to normalized coords too:
      DataCoordYToNorm(minVal, yLog);
      DataCoordYToNorm(maxVal, yLog);

      if (doT3d) //convert X,Y,Z in X',Y' as per T3D perspective.
      {
        //reproject using P.T transformation in [0..1] cube during the actual plot using pltransform() (to reproject also the PSYMs is possible with plplot only if z=0, using this trick:
        actStream->stransform(PDotTTransformXYZval, &zValue);
      }
    
    // start drawing. Graphic Keywords accepted:CLIP(YES), COLOR(YES), LINESTYLE(YES), NOCLIP(YES),
    //                                          PSYM(YES), SYMSIZE(YES), T3D(YES), ZVALUE(YES)
    gdlSetGraphicsForegroundColorFromKw(e, actStream);
    
    // it is important to fix symsize before changing vpor or win 
    gdlGetPsym(e, psym);
    gdlSetPenThickness(e, actStream);
    gdlSetSymsize(e, actStream);
    gdlSetLineStyle(e, actStream);
    actStream->setSymbolSizeConversionFactors();

#ifdef USE_LIBPROJ
        if ( mapSet && psym < 1) {
          GDLgrProjectedPolygonPlot(actStream, ref, NULL, xVal, yVal, false, false, NULL);  //will use LIBPROJ
          psym=-psym;
          if (psym > 0) draw_polyline(actStream, xVal, yVal, minVal, maxVal, doMinMax, xLog, yLog, psym);
        }
        else draw_polyline(actStream, xVal, yVal, minVal, maxVal, doMinMax, xLog, yLog, psym);
#else
    draw_polyline(actStream, xVal, yVal, minVal, maxVal, doMinMax, xLog, yLog, psym);
#endif
  } 

    private: void call_plplot(EnvT* e, GDLGStream* actStream) 
    {
    } 

    private: void post_call(EnvT* e, GDLGStream* actStream)
    {
      actStream->RestoreLayout();
      actStream->stransform(NULL,NULL);
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

