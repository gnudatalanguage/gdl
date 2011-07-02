/* *************************************************************************
                          plotting.hpp  -  GDL routines for plotting
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
    email                : m_schellens@users.sf.net
 ***************************************************************************/

/* *************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PLOTTING_HPP_
#define PLOTTING_HPP_

#include "envt.hpp"
#include "graphics.hpp"
#include "initsysvar.hpp"

namespace lib {

  using namespace std;

  // main plotting routine (all defined using the plotting_routine_call class)
  void plot( EnvT* e);
  void oplot( EnvT* e);
  void plots( EnvT* e);
  void surface( EnvT* e);
  void contour( EnvT* e);
  void xyouts( EnvT* e);
  void axis( EnvT* e);
  void polyfill( EnvT* e);

  // other plotting routines
  void erase( EnvT* e);
  void tvlct( EnvT* e);
  void wshow( EnvT* e);
  void wdelete( EnvT* e);
  void wset( EnvT* e);
  void window( EnvT* e);
  void set_plot( EnvT* e);
  BaseGDL* get_screen_size( EnvT* e);
  void device( EnvT* e);
  void cursor( EnvT* e);

  // Map stuff
  BaseGDL* map_proj_forward_fun( EnvT* e);
  BaseGDL* map_proj_inverse_fun( EnvT* e);

  BaseGDL* convert_coord( EnvT* e);

  //helper functions / classes

  class plotting_routine_call
  {
    // ensure execution of child-class destructors
    public: virtual ~plotting_routine_call() {};
    
    // private fields
    private: SizeT _nParam;
    private: bool xlog, ylog;

    // common helper methods
    protected: inline SizeT nParam() { return _nParam; }

    // prototypes for methods defining various steps
    private: virtual void handle_args(EnvT*) = 0;
    private: virtual void old_body(EnvT*, GDLGStream*) = 0;
    private: virtual void call_plplot(EnvT*, GDLGStream*) = 0;
    private: virtual void post_call(EnvT*, GDLGStream*) = 0;

    // all steps combined (virtual methods cannot be called from ctor)
    public: void call(EnvT* e, SizeT n_params_required) // {{{
    {
      _nParam = e->NParam(n_params_required);

      handle_args(e);

      GDLGStream* actStream = Graphics::GetDevice()->GetStream();
      if (actStream == NULL) e->Throw("Unable to create window.");

      old_body(e, actStream); // TODO: to be removed!
      call_plplot(e, actStream);

      actStream->flush();

      post_call(e, actStream);
    } // }}}
  };

  template <typename T> 
  bool draw_polyline(EnvT *e,  GDLGStream *a, T * xVal, T* yVal, 
		     bool xLog, bool yLog, 
		     DDouble yStart, DDouble yEnd, DLong psym=0);
  
  void gkw_axis_margin(EnvT *e, string axis,DFloat &start, DFloat &end);

  //set the background color
  void gkw_background(EnvT * e, GDLGStream * a,bool kw=true);
  //set the foreground color
  void gkw_color(EnvT * e, GDLGStream * a);
  //set the noerase flag
  void gkw_noerase(EnvT * e, GDLGStream * a,bool noe=0);
  //set the symbol shape
  void gkw_psym(EnvT *e, GDLGStream *a, bool &line, DLong &psym);
  //set the symbol size
  void gkw_symsize(EnvT * e, GDLGStream * a);
  //set the character size
  void gkw_charsize(EnvT * e, GDLGStream * a, DFloat& charsize, bool kw=true);
  //set the line thickness
  void gkw_thick(EnvT * e, GDLGStream * a);
  //set the linestyle
  void gkw_linestyle(EnvT * e, GDLGStream * a);
  //title
  void gkw_title(EnvT* e, GDLGStream *a, PLFLT ad);
  //set the !axis.crange vector
  void set_axis_crange(string axis, DDouble Start, DDouble End);
  //get the !axis.crange vector
  void get_axis_crange(string axis, DDouble &Start, DDouble &End);
  void get_axis_margin(string axis, DFloat &low, DFloat &high);
  //axis_type
  void get_axis_type(string axis, bool &log);
  void set_axis_type(string axis, bool type);

  // mapset
  void get_mapset(bool &mapset);
  void set_mapset(bool mapset);

  void gkw_axis_charsize(EnvT* e, string axis, DFloat &charsize);

  void gkw_axis_style(EnvT *e, string axis,DLong &style);
  void gkw_axis_title(EnvT *e, string axis,DString &title);
  void gkw_axis_range(EnvT *e, string axis,
		      DDouble &start, DDouble &end, DLong & ynozero);

  void mesh_nr(PLFLT *, PLFLT *, PLFLT **, PLINT, PLINT, PLINT);

  void GetSFromPlotStructs(DDouble **sx, DDouble **sy);
  void GetWFromPlotStructs(DFloat **wx, DFloat **wy);
  void DataCoordLimits(DDouble *sx, DDouble *sy, DFloat *wx, DFloat *wy, 
    DDouble *xStart, DDouble *xEnd, DDouble *yStart, DDouble *yEnd, bool);

  PLFLT AutoIntvAC(DDouble &val_min, DDouble &val_max, DLong NoZero);
  PLFLT AutoTick(DDouble x);
  void AdjustAxisOpts(string& xOpt, string& yOpt,
    DLong xStyle, DLong yStyle, DLong xTicks, DLong yTicks,
    string& xTickformat, string& yTickformat, DLong xLog, DLong yLog);
  bool SetVP_WC( EnvT* e, GDLGStream* actStream, DFloatGDL* pos, DDoubleGDL* clippingD, bool xLog, bool yLog,
                 DFloat xMarginL, DFloat xMarginR, DFloat yMarginB, DFloat yMarginT, // input/output
                 DDouble& xStart, DDouble& xEnd, DDouble& minVal, DDouble& maxVal);
  void GetMinMaxVal( DDoubleGDL* val, double* minVal, double* maxVal);
  void GetAxisData( DStructGDL* xStruct,
                    DLong& style, DString& title, DFloat& charSize,
                    DFloat& margin0, DFloat& margin1, DFloat& ticklen);
  void GetMinMaxVal( DDoubleGDL* val, double* minVal, double* maxVal);
  void GetPData( DLong& p_background,
                 DLong& p_noErase, DLong& p_color, DLong& p_psym,
                 DLong& p_linestyle,
                 DFloat& p_symsize, DFloat& p_charsize, DFloat& p_thick,
                 DString& p_title, DString& p_subTitle, DFloat& p_ticklen);
    void CheckMargin( EnvT* e, GDLGStream* actStream,
                    DFloat xMarginL, DFloat xMarginR, DFloat yMarginB, DFloat yMarginT,
                    PLFLT& xMR, PLFLT& xML, PLFLT& yMB, PLFLT& yMT);
    void Clipping( DDoubleGDL* clippingD, 
                 DDouble& xStart, DDouble& xEnd, DDouble& minVal, DDouble& maxVal);
    void handle_pmulti_position(EnvT *e, GDLGStream *a);
    void UpdateSWPlotStructs(GDLGStream* actStream, DDouble xStart, DDouble xEnd, DDouble yStart, DDouble yEnd);

} // namespace

#endif
