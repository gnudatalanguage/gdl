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

namespace lib {

  using namespace std;

  class plots_call : public plotting_routine_call
  {

    DDoubleGDL *xVal, *yVal, *zVal;
    auto_ptr<BaseGDL> xval_guard, yval_guard;
    DLong psym;
    DDouble xStart, xEnd, yStart, yEnd, zStart, zEnd;
    bool xLog, yLog;
    SizeT xEl, yEl, zEl;
    DDouble minVal, maxVal;

    private: void handle_args(EnvT* e) // {{{
    {
      if( nParam() == 1)
      {
        BaseGDL* p0;
        p0 = e->GetParDefined( 0);  
        if (p0->Dim(0) != 2)
          e->Throw( "When only 1 param, dims must be (2,n)");

        if (e->KeywordSet("T3D")) e->Throw("Only 3-argument case supported for T3D - FIXME!");
        // TODO: the (3,n) case

        DDoubleGDL *val = e->GetParAs< DDoubleGDL>( 0);

        xEl = p0->N_Elements() / p0->Dim(0);
        xVal = new DDoubleGDL( dimension( xEl), BaseGDL::NOZERO);
        xval_guard.reset( xVal); // delete upon exit

        yEl = p0->N_Elements() / p0->Dim(0);
        yVal = new DDoubleGDL( dimension( yEl), BaseGDL::NOZERO);
        yval_guard.reset( yVal); // delete upon exit

        for( SizeT i = 0; i < xEl; i++) 
        {
	  (*xVal)[i] = (*val)[2*i];
	  (*yVal)[i] = (*val)[2*i+1];
        }
      }
      else if(nParam() == 2)
      {
        if (e->KeywordSet("T3D")) e->Throw("Only 3-argument case supported for T3D - FIXME!"); // TODO

        xVal = e->GetParAs< DDoubleGDL>( 0);
        xEl = xVal->N_Elements();

        yVal = e->GetParAs< DDoubleGDL>( 1);
        yEl = yVal->N_Elements();
      }
      else if(nParam() == 3)
      {
        zVal = e->GetParAs< DDoubleGDL>( 2);
        zEl = zVal->N_Elements();

        //if ((*zVal)[0] == 0 && zEl == 1) {
          xVal = e->GetParAs< DDoubleGDL>( 0);
          xEl = xVal->N_Elements();

          yVal = e->GetParAs< DDoubleGDL>( 1);
          yEl = yVal->N_Elements();
	//}  
        //else 
        //{
        //}
      }
    } // }}}

  private: void old_body( EnvT* e, GDLGStream* actStream) // {{{
  {
    bool line;


    // !X, !Y (also used below)
    DFloat xMarginL, xMarginR, yMarginB, yMarginT; 
    get_axis_margin("X",xMarginL, xMarginR);
    get_axis_margin("Y",yMarginB, yMarginT);
    // get ![XY].CRANGE
    get_axis_crange("X", xStart, xEnd);
    get_axis_crange("Y", yStart, yEnd);
    if (e->KeywordSet("T3D")) get_axis_crange("Z", zStart, zEnd);

    get_axis_type("X", xLog);
    get_axis_type("Y", yLog);
    
//     if( xLog)
//       {
// 	xStart = pow(10.0,xStart);
// 	xEnd   = pow(10.0,xEnd);
//       }
//     if( yLog)
//       {
// 	yStart = pow(10.0,yStart);
// 	yEnd   = pow(10.0,yEnd);
//       }
    
    //    int just = (e->KeywordSet("ISOTROPIC"))? 1 : 0;
    /*    DLong background = p_background;
    static int cix=e->KeywordIx("COLOR");
    BaseGDL* color_arr=e->GetKW(cix);
    DLongGDL* l_color_arr;

    if(color_arr != NULL)
      {
	l_color_arr=static_cast<DLongGDL*>
	  (color_arr->Convert2(LONG, BaseGDL::COPY));
	if(color_arr->N_Elements() < minEl && color_arr->N_Elements() > 1)
	  e->Throw( "Array "+e->GetParString(cix)+
		    " does not have enough elements for COLOR keyword.");
      }
    DLong color = p_color;

    if(color_arr != NULL)  
      if(color_arr->N_Elements() >= 1) 
	  	color=(*l_color_arr)[0];
    */

    // start drawing
    gkw_background(e, actStream, false);
    gkw_color(e, actStream);

    gkw_psym(e, actStream, line, psym);
    gkw_linestyle(e, actStream);
    gkw_symsize(e, actStream);
    gkw_thick(e, actStream);
    DFloat charsize;
    gkw_charsize(e,actStream, charsize, false);

    // plplot stuff
    PLFLT scrXL, scrXR, scrYB, scrYT;
    actStream->gspa( scrXL, scrXR, scrYB, scrYT);
    PLFLT scrX = scrXR-scrXL;
    PLFLT scrY = scrYT-scrYB;

    PLFLT xMR, xML, yMB, yMT;

    CheckMargin( e, actStream,
		 xMarginL, 
		 xMarginR, 
		 yMarginB, 
		 yMarginT,
		 xMR,
		 xML,
		 yMB,
		 yMT);

    bool mapSet=false;
#ifdef USE_LIBPROJ4
    // Map Stuff (xtype = 3)
    LPTYPE idata;
    XYTYPE odata;

    get_mapset(mapSet);

    if ( mapSet) {
      ref = map_init();
      if ( ref == NULL) {
	e->Throw( "Projection initialization failed.");
      }
    }
#endif

    DDouble *sx, *sy;
    DFloat *wx, *wy;
    GetSFromPlotStructs(&sx, &sy);
    GetWFromPlotStructs(&wx, &wy);

    int toto=0;

    if (!e->KeywordSet("T3D"))
    {
    if(e->KeywordSet("DEVICE")) {
      PLFLT xpix, ypix;
      PLINT xleng, yleng, xoff, yoff;
      actStream->gpage(xpix, ypix,xleng, yleng, xoff, yoff);
      xStart=0; xEnd=xleng;
      yStart=0; yEnd=yleng;
      xLog = false; yLog = false;
      actStream->NoSub();
      actStream->vpor(0, 1, 0, 1);
    } else if(e->KeywordSet("NORMAL")) {
      xStart = 0;
      xEnd   = 1;
      yStart = 0;
      yEnd   = 1;
      actStream->NoSub();
      actStream->vpor(0, 1, 0, 1);
      xLog = false; yLog = false;
    } else {
      toto=1;
      actStream->NoSub();
      if (xLog || yLog) actStream->vpor(wx[0], wx[1], wy[0], wy[1]);
      else actStream->vpor(0, 1, 0, 1); // TODO (to be merged with the condition on DataCoordLimits...)
    }
    }

    // Determine data coordinate limits (if mapSet is true)
    // These are computed from window and scaling axis system
    // variables because map routines change these directly.
    
    if (e->KeywordSet("DATA") || (toto == 1)) {
      //    if (e->KeywordSet("NORMAL") || e->KeywordSet("DATA")) {
      DataCoordLimits(sx, sy, wx, wy, &xStart, &xEnd, &yStart, &yEnd, xLog || yLog);
    }

    minVal=yStart; maxVal=yEnd;

    //CLIPPING
    DLong noclip = 1;
    e->AssureLongScalarKWIfPresent( "NOCLIP", noclip);
    if( noclip == 0)
    {
    static int clippingix = e->KeywordIx( "CLIP"); 
    DDoubleGDL* clippingD = e->IfDefGetKWAs<DDoubleGDL>( clippingix);
    if( clippingD != NULL)
      Clipping( clippingD, xStart, xEnd, minVal, maxVal);
    }

//    Comment out to fix bug [1560714] JMG 06/09/27
//    if( xLog)
//      {
//	if( xStart <= 0.0) xStart = 0.0; else xStart = log10( xStart);
//	if( xEnd   <= 0.0) return; else xEnd = log10( xEnd);
//      }

    if( yLog)
      {
	if( yStart <= 0.0) yStart = 0.0; else yStart = log10( yStart);
	if( yEnd   <= 0.0) return; else yEnd = log10( yEnd);
      }

    if (!e->KeywordSet("T3D")) 
    {
      // SA: following a patch from Joanna (3029409) TODO: this is repeated in PLOTS POLYFILL and XYOUTS
      if ( xEnd - xStart == 0 || yEnd - yStart == 0 || isnan(xStart) || isnan(yStart) ) {
        actStream->wind( 0, 1, 0, 1 ); 
      } else {
        actStream->wind( xStart, xEnd, yStart, yEnd);
      } 
    }

    // pen thickness for plot
    gkw_thick(e, actStream);
    gkw_symsize(e, actStream);
    gkw_linestyle(e, actStream);

  } // }}}

    private: void call_plplot(EnvT* e, GDLGStream* actStream) // {{{
    {
      if (e->KeywordSet("T3D")) // TODO: check !P.T3D
      {
        Warning("PLOTS: 3D plotting does not really work yet (!P.T and !P.T3D are ignored for the moment)");
 /*       actStream->w3d(1.2, 1.2, 2.2, // TODO!
          xStart, xEnd, yStart, yEnd, xStart, zEnd,
          30, 30  // TODO!
        );
*/
        PLINT n = xVal->N_Elements();
        if (yEl > 1 && yEl < n) n = yEl;
        if (zEl > 1 && zEl < n) n = zEl;

        PLFLT *x = new PLFLT[n];
        {
          int a = (xEl != 1); 
          for (PLINT i = 0; i < n; ++i) x[i] = (*xVal)[a * i];
        }

        PLFLT *y = new PLFLT[n];
        {
          int a = (yEl != 1); 
          for (PLINT i = 0; i < n; ++i) y[i] = (*yVal)[a * i];
        }

        PLFLT *z = new PLFLT[n];
        {
          int a = (zEl != 1); 
          for (PLINT i = 0; i < n; ++i) z[i] = (*zVal)[a * i];
        }

        actStream->line3(n, x, y, z);

        delete[] x;
        delete[] y;
        delete[] z;
      }
      else 
      {
        bool valid = draw_polyline(e, actStream, xVal, yVal, xLog, yLog, yStart, yEnd, psym);
        // TODO: handle valid?
      }
    } // }}}

    private: virtual void post_call(EnvT*, GDLGStream* actStream) // {{{
    {
      actStream->lsty(1);//reset linestyle
    } // }}}

  }; // oplot_call class 

  void plots(EnvT* e)
  {
    plots_call plots;
    plots.call(e, 1);
  }  

} // namespace
