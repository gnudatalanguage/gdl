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
    auto_ptr<BaseGDL> xval_guard,yval_guard,xtempval_guard;

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

      DDoubleGDL *yValBis, *xValBis;
      auto_ptr<BaseGDL> xvalBis_guard, yvalBis_guard;
      //test and transform eventually if POLAR and/or NSUM!
      if( nParam() == 1)
      {
        yTemp = e->GetParAs< DDoubleGDL>( 0);
        if (yTemp->Rank() == 0)
          e->Throw("Expression must be an array in this context: "+e->GetParString(0));
        xTemp = new DDoubleGDL( dimension( yVal->N_Elements()), BaseGDL::INDGEN);
        xtempval_guard.reset( xTemp); // delete upon exit
      }
      else
      {
        xTemp = e->GetParAs< DDoubleGDL>( 0);
        if (xTemp->Rank() == 0)
          e->Throw("Expression must be an array in this context: "+e->GetParString(0));
        yTemp = e->GetParAs< DDoubleGDL>( 1);
        if (yTemp->Rank() == 0)
          e->Throw("Expression must be an array in this context: "+e->GetParString(1));
      }
      //check nsum validity
      nsum=max(1,nsum);
      nsum=min(nsum,(DLong)xTemp->N_Elements());
      if (nsum == 1)
      {
        if (polar)
        {
          xVal = new DDoubleGDL(dimension(xTemp->N_Elements()), BaseGDL::NOZERO);
          xval_guard.reset(xVal); // delete upon exit
          yVal = new DDoubleGDL(dimension(yTemp->N_Elements()), BaseGDL::NOZERO);
          yval_guard.reset(yVal); // delete upon exit
          for (int i = 0; i < xVal->N_Elements(); i++) (*xVal)[i] = (*xTemp)[i] * cos((*yTemp)[i]);
          for (int i = 0; i < yVal->N_Elements(); i++) (*yVal)[i] = (*xTemp)[i] * sin((*yTemp)[i]);
        }
        else
        { //careful about previously set autopointers!
          if (nParam() == 1) xval_guard = xtempval_guard;
          xVal = xTemp;
          yVal = yTemp;
        }
      }
      else
      {
        int i, j, k;
        DLong size = xTemp->N_Elements() / nsum;
        xVal = new DDoubleGDL(size, BaseGDL::ZERO); //SHOULD BE ZERO, IS NOT!
        xval_guard.reset(xVal); // delete upon exit
        yVal = new DDoubleGDL(size, BaseGDL::ZERO); //IDEM
        yval_guard.reset(yVal); // delete upon exit
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
    }

  private: void old_body( EnvT* e, GDLGStream* actStream) // {{{
  {
    bool valid;
    valid=true;
    DLong psym;

    // get ![XY].CRANGE
    DDouble xStart, xEnd, yStart, yEnd;
    get_axis_crange("X", xStart, xEnd);
    get_axis_crange("Y", yStart, yEnd);
    DDouble minVal;
    DDouble maxVal;

    bool xLog;
    bool yLog;
    get_axis_type("X", xLog);
    get_axis_type("Y", yLog);

    if( (yStart == yEnd) || (xStart == xEnd))
      {
	if( yStart != 0.0 && yStart == yEnd)
	  Message("OPLOT: !Y.CRANGE ERROR, setting to [0,1]");
	yStart = 0; //yVal->min();
	yEnd   = 1; //yVal->max();
	    
	if(xStart != 0.0 && xStart == xEnd)
	  Message("OPLOT: !X.CRANGE ERROR, resetting range to data");
	xStart = 0; //xVal->min();
	xEnd   = 1; //xVal->max();
	
	set_axis_crange("X", xStart, xEnd);
	set_axis_crange("Y", yStart, yEnd);
      }	    
    
    minVal = yStart;
    maxVal = yEnd;
    e->AssureDoubleScalarKWIfPresent( "MIN_VALUE", minVal);
    e->AssureDoubleScalarKWIfPresent( "MAX_VALUE", maxVal);

    // CLIPPING
    DDoubleGDL* clippingD=NULL;
    DLong noclip=0;
    e->AssureLongScalarKWIfPresent( "NOCLIP", noclip);
    if(noclip == 0)
      {
	static int clippingix = e->KeywordIx( "CLIP"); 
	clippingD = e->IfDefGetKWAs<DDoubleGDL>( clippingix);
      }

    actStream->wid( 0);
    // start drawing. Graphic Keywords accepted:CLIP(NO), COLOR(YES), LINESTYLE(YES), NOCLIP(YES),
    //                                          PSYM(YES), SYMSIZE(YES), T3D(NO), ZVALUE(NO)
    gkw_background(e, actStream, false);
    gkw_color(e, actStream);
    //    gkw_noerase(e, actStream, true);
    gkw_psym(e, psym);
    DFloat charsize;
    gkw_charsize(e,actStream, charsize, false); //set !P.CHARSIZE
    gkw_thick(e, actStream);
    gkw_symsize(e, actStream);
    gkw_linestyle(e, actStream);

    // plot the data
    if(valid) //invalid is not yet possible. Could be done by a severe clipping for example.
      valid=draw_polyline(e, actStream, 
			  xVal, yVal, xLog, yLog, 
			  psym, FALSE);


    actStream->lsty(1);//reset linestyle
  } // }}}

    private: void call_plplot(EnvT* e, GDLGStream* actStream) // {{{
    {
    } // }}}

    private: void post_call(EnvT* e, GDLGStream* actStream) // {{{
    {
    } // }}}

  }; // oplot_call class 

  void oplot(EnvT* e)
  {
    oplot_call oplot;
    oplot.call(e, 1);
  }

} // namespace

