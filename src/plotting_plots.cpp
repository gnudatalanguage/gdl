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
    PLFLT xStart, xEnd, yStart, yEnd, zStart, zEnd;
    PLFLT xMarginL, xMarginR, yMarginB, yMarginT;
    bool xLog, yLog, xLogOrig, yLogOrig;
    SizeT xEl, yEl, zEl;
    DDouble minVal, maxVal;
    bool append;

    private: bool handle_args(EnvT* e) // {{{
    {
      append=e->KeywordSet("CONTINUE");
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
      }
    } 

  private: void old_body( EnvT* e, GDLGStream* actStream) // {{{
  {
    DDoubleGDL* clippingD;

    actStream->gvpd(xMarginL,xMarginR,yMarginB,yMarginT);
    if((xMarginL==0.0&&xMarginR==0.0)||(yMarginB==0.0&&yMarginT==0.0)) //if not initialized, set normalized mode
    {
        actStream->NoSub();
        actStream->vpor(0, 1, 0, 1);
        actStream->gvpd(xMarginL,xMarginR,yMarginB,yMarginT);
        actStream->wind(0.0,1.0,0.0,1.0);
   }
    // get current viewport limit in world coords
    actStream->gvpw(xStart,xEnd,yStart,yEnd);
    // Axis type
    get_axis_type("X", xLogOrig); xLog=xLogOrig;
    get_axis_type("Y", yLogOrig); yLog=yLogOrig;
    
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
    //CLIPPING
    DLong noclip = 1;
    e->AssureLongScalarKWIfPresent( "NOCLIP", noclip);
    if( noclip == 0)
    {
    static int clippingix = e->KeywordIx( "CLIP");
    DDoubleGDL* clippingD = e->IfDefGetKWAs<DDoubleGDL>( clippingix);
    }

    if (!e->KeywordSet("T3D"))
    {
      if (e->KeywordSet("DEVICE"))
      {
        actStream->NoSub();
        actStream->vpor(0, 1, 0, 1);
        PLFLT xpix, ypix;
        PLFLT un,deux,trois,quatre;
        PLINT xleng, yleng, xoff, yoff;
        actStream->gpage(xpix, ypix, xleng, yleng, xoff, yoff);
        un=0.0; deux=xleng; trois=0.0; quatre=yleng;
        if( clippingD != NULL) Clipping( clippingD, un, deux, trois, quatre);
        actStream->wind(un, deux, trois, quatre);
        xLog = false;
        yLog = false;
      }
      else if (e->KeywordSet("NORMAL"))
      {
        PLFLT un,deux,trois,quatre;
        actStream->NoSub();
        actStream->vpor(0, 1, 0, 1);
        un=0.0; deux=1.0; trois=0.0; quatre=1.0;
        if( clippingD != NULL) Clipping( clippingD, un, deux, trois, quatre);
        actStream->wind(un, deux, trois, quatre);
        xLog = false;
        yLog = false;
      }
      else if( clippingD != NULL)
      {
         PLFLT un,deux,trois,quatre;
         un=xStart; deux=xEnd; trois=yStart; quatre=yEnd;
         Clipping( clippingD, un, deux, trois, quatre);
         actStream->wind(un, deux, trois, quatre);
       }
   }
    // start drawing. Graphic Keywords accepted: CLIP(YES), COLOR(OK), DATA(YES), DEVICE(YES),
    //LINESTYLE(OK), NOCLIP(YES), NORMAL(YES), PSYM(OK), SYMSIZE(OK), T3D(NO), THICK(OK), Z(NO)
    gkw_color(e, actStream); //COLOR
    gkw_psym(e, psym); //PSYM
    gkw_linestyle(e, actStream); //LINESTYLE
    gkw_symsize(e, actStream); //SYMSIZE
    gkw_thick(e, actStream); //THICK

  } 

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
        bool valid = draw_polyline(e, actStream, xVal, yVal, xLog, yLog, psym, append);
        // TODO: handle valid?
      }
    } 

    private: virtual void post_call(EnvT*, GDLGStream* actStream) 
    {
      actStream->lsty(1);//reset linestyle
      actStream->vpor(xMarginL, xMarginR, yMarginB, yMarginT);
      actStream->wind(xStart, xEnd, yStart, yEnd);
    } 

  }; // oplot_call class 

  void plots(EnvT* e)
  {
    plots_call plots;
    plots.call(e, 1);
  }  

} // namespace
