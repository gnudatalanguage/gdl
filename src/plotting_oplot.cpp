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
    SizeT xEl, yEl;
    Guard<BaseGDL> xval_guard,yval_guard,xtempval_guard;

    private: bool handle_args( EnvT* e) // {{{
    {
      bool polar=FALSE;
      DLong nsum=1;
      e->AssureLongScalarKWIfPresent( "NSUM", nsum);
      if ( e->KeywordSet( "POLAR"))
      {
        polar=TRUE;
       // e->Throw( "Sorry, POLAR keyword not ready");
      }

      //test and transform eventually if POLAR and/or NSUM!
      if( nParam() == 1)
      {
        yTemp = e->GetParAs< DDoubleGDL>( 0);
        if (yTemp->Rank() == 0)
          e->Throw("Expression must be an array in this context: "+e->GetParString(0));
        yEl=yTemp->N_Elements();
        xEl=yEl;
        xTemp = new DDoubleGDL( dimension( xEl), BaseGDL::INDGEN);
        xtempval_guard.Reset( xTemp); // delete upon exit
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
          if (nParam() == 1) xval_guard.Init( xtempval_guard.release());
          xVal = xTemp;
          yVal = yTemp;
        }
      }
      else
      {
        int i, j, k;
        DLong size = xEl / nsum;
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
	  return 0;
    }

  private: void old_body( EnvT* e, GDLGStream* actStream) 
  {
    DLong psym;

    // get ![XY].CRANGE
    DDouble xStart, xEnd, yStart, yEnd;
    gdlGetCurrentAxisRange("X", xStart, xEnd);
    gdlGetCurrentAxisRange("Y", yStart, yEnd);
    DDouble minVal, maxVal;
    bool doMinMax;

    bool xLog;
    bool yLog;
    gdlGetAxisType("X", xLog);
    gdlGetAxisType("Y", yLog);

    if ((yStart == yEnd) || (xStart == xEnd))
    {
      if (yStart != 0.0 && yStart == yEnd)
        Message("OPLOT: !Y.CRANGE ERROR, setting to [0,1]");
      yStart = 0; //yVal->min();
      yEnd = 1; //yVal->max();

      if (xStart != 0.0 && xStart == xEnd)
        Message("OPLOT: !X.CRANGE ERROR, setting to [0,1]");
      xStart = 0; //xVal->min();
      xEnd = 1; //xVal->max();
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
    //                                          PSYM(YES), SYMSIZE(YES), T3D(NO), ZVALUE(NO)
    gdlSetGraphicsBackgroundColorFromKw(e, actStream, false);
    gdlSetGraphicsForegroundColorFromKw(e, actStream);
    gdlGetPsym(e, psym);
    gdlSetPenThickness(e, actStream);
    gdlSetSymsize(e, actStream);
    gdlSetLineStyle(e, actStream);


      // TODO: handle "valid"!
    bool valid=draw_polyline(e, actStream, xVal, yVal, minVal, maxVal, doMinMax, xLog, yLog, psym, FALSE);
    if (stopClip) stopClipping(actStream);


    actStream->lsty(1);//reset linestyle
  } 

    private: void call_plplot(EnvT* e, GDLGStream* actStream) 
    {
    } 

    private: void post_call(EnvT* e, GDLGStream* actStream)
    {
    } 

  }; // oplot_call class 

  void oplot(EnvT* e)
  {
    oplot_call oplot;
    oplot.call(e, 1);
  }

} // namespace

