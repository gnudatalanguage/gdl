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

#ifdef _MSC_VER
#  define isnan _isnan
#endif

namespace lib
{

  using namespace std;
//  using std::isinf;
  using std::isnan;

  class polyfill_call: public plotting_routine_call
  {

  private:
    DDoubleGDL* yVal, *xVal;
  private:
    SizeT xEl, yEl;

  private:

    bool handle_args(EnvT* e) // {{{
    {
      return true;
    } // }}}

    void old_body(EnvT* e, GDLGStream* actStream) // {{{
    {

      bool mapSet=false;
#ifdef USE_LIBPROJ4
      LPTYPE idata;
      XYTYPE odata;
      get_mapset(mapSet);
      if ( mapSet )
      {
        ref=map_init();
        if ( ref==NULL ) e->Throw("Projection initialization failed.");
      }
#endif

      if ( nParam()==1||nParam()==3 )
      {
        e->Throw("1- and 3-argument case not implemented yet");
      }
      else
      {
        xVal= !mapSet
        ?e->GetParAs< DDoubleGDL>(0)
        :static_cast<DDoubleGDL*>(
                                  e->GetNumericArrayParDefined(0)->Convert2(GDL_DOUBLE, BaseGDL::COPY)
                                  );
        yVal= !mapSet
        ?e->GetParAs< DDoubleGDL>(1)
        :static_cast<DDoubleGDL*>(
                                  e->GetNumericArrayParDefined(1)->Convert2(GDL_DOUBLE, BaseGDL::COPY)
                                  );

        xEl=xVal->N_Elements();
        yEl=yVal->N_Elements();
      }

      if ( xEl!=yEl )
        e->Throw("xEl != yEl"); // TODO

      if ( xEl<3 )
        e->Throw("Input arrays must define at least three points");

      DLong xStyle, yStyle;
      DString xTitle, yTitle;
      DFloat x_CharSize, y_CharSize;
      DFloat xMarginL, xMarginR, yMarginB, yMarginT;
      DFloat xTicklen, yTicklen;
//      DFloat xMarginL, xMarginR, yMarginB, yMarginT;
//      gdlGetDesiredAxisMargin(e, "X", xMarginL, xMarginR);
//      gdlGetDesiredAxisMargin(e, "Y", yMarginB, yMarginT);
      GetAxisData(SysVar::X(), xStyle, xTitle, x_CharSize, xMarginL, xMarginR, xTicklen);
      GetAxisData(SysVar::Y(), yStyle, yTitle, y_CharSize, yMarginB, yMarginT, yTicklen);
      DDouble xStart, xEnd, yStart, yEnd;
      bool xLog, yLog;

      gdlGetCurrentAxisRange("X", xStart, xEnd);
      gdlGetCurrentAxisRange("Y", yStart, yEnd);
      gdlGetAxisType("X", xLog);
      gdlGetAxisType("Y", yLog);

      gdlSetGraphicsForegroundColorFromKw(e, actStream);

      PLFLT xMR, xML, yMB, yMT;
      CheckMargin(e, actStream, xMarginL, xMarginR, yMarginB, yMarginT, xMR, xML, yMB, yMT);

      DDouble *sx, *sy;
      DFloat *wx, *wy;
      GetSFromPlotStructs(&sx, &sy);
      GetWFromPlotStructs(&wx, &wy);

      int toto=0;

      if ( e->KeywordSet("DEVICE") )
      {
        PLFLT xpix, ypix;
        PLINT xleng, yleng, xoff, yoff;
        actStream->gpage(xpix, ypix, xleng, yleng, xoff, yoff);
        xStart=0;
        xEnd=xleng;
        yStart=0;
        yEnd=yleng;
        xLog=false;
        yLog=false;
        actStream->NoSub();
        actStream->vpor(0, 1, 0, 1);
      }
      else if ( e->KeywordSet("NORMAL") )
      {
        xStart=0;
        xEnd=1;
        yStart=0;
        yEnd=1;
        actStream->NoSub();
        actStream->vpor(0, 1, 0, 1);
        xLog=false;
        yLog=false;
      }
      else
      {
        toto=1;
        actStream->NoSub();
        if ( xLog||yLog ) actStream->vpor(wx[0], wx[1], wy[0], wy[1]);
        else actStream->vpor(0, 1, 0, 1); // (to be merged with the condition on DataCoordLimits...)
      }

      // Determine data coordinate limits
      // These are computed from window and scaling axis system
      // variables because map routines change these directly.
      //if (e->KeywordSet("NORMAL") || e->KeywordSet("DATA")) {
      if ( e->KeywordSet("DATA")||(toto==1) )
      {
        DataCoordLimits(sx, sy, wx, wy, &xStart, &xEnd, &yStart, &yEnd, false);
      }
      DDouble minVal=yStart, maxVal=yEnd;

      // SA: following a patch from Joanna (3029409) TODO: this is repeated in PLOTS POLYFILL and XYOUTS
      if ( xEnd-xStart==0||yEnd-yStart==0||isnan(xStart)||isnan(yStart) )
      {
        actStream->wind(0, 1, 0, 1);
      }
      else
      {
        actStream->wind(xStart, xEnd, yStart, yEnd);
      }

      // LINE_FILL, SPACING, LINESTYLE, ORIENTATION, THICK (thanks to JW)
      static int line_fillIx=e->KeywordIx("LINE_FILL");
      if ( e->KeywordSet(line_fillIx) )
      {
        PLINT inc=0, del=1500;

        static int orientationIx=e->KeywordIx("ORIENTATION");
        if ( e->KeywordSet(orientationIx) ) inc=PLINT(1e1*(*e->GetKWAs<DFloatGDL>(orientationIx))[0]);

        static int spacingIx=e->KeywordIx("SPACING");
        if ( e->KeywordSet(spacingIx) ) del=PLINT(1e4*(*e->GetKWAs<DFloatGDL>(spacingIx))[0]);

        gdlSetPenThickness(e, actStream);
        gdlSetLineStyle(e, actStream);

        actStream->psty(8);
        actStream->pat(1, &inc, &del);
      }
      else
      {
        actStream->psty(0);
      }

#ifdef USE_LIBPROJ4
      if ( mapSet )
      {
        for ( SizeT i=0; i<xEl; ++i )
        {
          idata.lam=(*xVal)[i]*DEG_TO_RAD;
          idata.phi=(*yVal)[i]*DEG_TO_RAD;
          odata=PJ_FWD(idata, ref);
          (*xVal)[i]=odata.x;
          (*yVal)[i]=odata.y;
        }
      }
#endif

    } // }}}

  private:

    void call_plplot(EnvT* e, GDLGStream* actStream) // {{{
    {
      actStream->fill(xEl, static_cast<PLFLT*>(&(*xVal)[0]), static_cast<PLFLT*>(&(*yVal)[0]));
    } // }}}

  private:

    virtual void post_call(EnvT*, GDLGStream*) // {{{
    {
    } // }}}

  };

  void polyfill(EnvT* e)
  {
    polyfill_call polyfill;
    polyfill.call(e, 1);
  }

} // namespace
