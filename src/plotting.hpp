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
#define gdlPlot_Min(a, b) ((a) < (b) ? (a) : (b))
#define gdlPlot_Max(a, b) ((a) > (b) ? (a) : (b))

#include "envt.hpp"
#include "graphics.hpp"
#include "initsysvar.hpp"

  struct GDL_TICKNAMEDATA
  {
    SizeT counter;
    SizeT nTickName;
    DStringGDL* TickName;
  };

  struct GDL_MULTIAXISTICKDATA
  {
    EnvT *e;
    SizeT counter;
    int what;
    SizeT nTickFormat;
    DDouble axismin;
    DDouble axismax;
    DStringGDL* TickFormat;
    SizeT nTickUnits;
    DStringGDL* TickUnits;
  };

#define GDL_NONE -1
#define GDL_TICKFORMAT 0
#define GDL_TICKUNITS 1
#define GDL_TICKFORMAT_AND_UNITS 2

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
  void usersym( EnvT* e);

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
    private: bool overplot;

    // common helper methods
    protected: inline SizeT nParam() { return _nParam; }

    // prototypes for methods defining various steps
    private: virtual bool handle_args(EnvT*) = 0; // return value = overplot
    private: virtual void old_body(EnvT*, GDLGStream*) = 0;
    private: virtual void call_plplot(EnvT*, GDLGStream*) = 0;
    private: virtual void post_call(EnvT*, GDLGStream*) = 0;

    // all steps combined (virtual methods cannot be called from ctor)
    public: void call(EnvT* e, SizeT n_params_required) // {{{
    {
      _nParam = e->NParam(n_params_required);

      overplot = handle_args(e);

      GDLGStream* actStream = Graphics::GetDevice()->GetStream();
      if (actStream == NULL) e->Throw("Unable to create window.");

      old_body(e, actStream); // TODO: to be removed!
      call_plplot(e, actStream);

      actStream->flush();

      post_call(e, actStream);
    } // }}}
  };
  template <typename T>
  void gdlDoRangeExtrema(T* xVal, T* yVal, DDouble &min, DDouble &max, DDouble xmin, DDouble xmax, bool doMinMax=FALSE, DDouble minVal=0, DDouble maxVal=0);
  template <typename T> 
  bool draw_polyline(EnvT *e,  GDLGStream *a, T * xVal, T* yVal, 
             DDouble minVal, DDouble maxVal, bool doMinMax,
		     bool xLog, bool yLog, 
		     DLong psym=0, bool append=FALSE);
  DDouble gdlEpsDouble();
  DDouble gdlAbsoluteMinValueDouble();
  //protect from (inverted, strange) axis log values
  void gdlHandleUnwantedAxisValue(DDouble &min, DDouble &max, bool log);
  //set the background color
  void gdlSetGraphicsBackgroundColorFromKw(EnvT * e, GDLGStream * a,bool kw=true);
  //set the foreground color
  void gdlSetGraphicsForegroundColorFromKw(EnvT * e, GDLGStream * a);
  //advance to next plot unless the noerase flag is set
  void gdlNextPlotHandlingNoEraseOption(EnvT * e, GDLGStream * a,bool noe=0);
  //set the symbol shape
  void gdlGetPsym(EnvT *e, DLong &psym);
  //set the symbol size
  void gdlSetSymsize(EnvT * e, GDLGStream * a);
  //set the PLOT character size (including MULTI subscaling)
  void gdlSetPlotCharsize(EnvT *e, GDLGStream *a, bool accept_sizeKw=false);
  //set the PLOT Char Thickness
  void gdlSetPlotCharthick(EnvT *e, GDLGStream *a);
  //set the line thickness
  void gdlSetPenThickness(EnvT * e, GDLGStream * a);
  //set the linestyle
  void gdlLineStyle(GDLGStream *a, DLong style);
  void gdlSetLineStyle(EnvT * e, GDLGStream * a);
  //set axis linewidth
  void gdlSetAxisThickness(EnvT *e, GDLGStream *a, string axis);
  //title
  void gdlWriteTitleAndSubtitle(EnvT* e, GDLGStream *a);
  //set the !axis.crange vector
  void gdlStoreAxisCRANGE(string axis, DDouble Start, DDouble End, bool log);
  //set the !axis.s vector
  void gdlStoreAxisSandWINDOW(GDLGStream* actStream, string axis, DDouble Start, DDouble End, bool log=false);

  // mapset
  void get_mapset(bool &mapset);
  void set_mapset(bool mapset);

  //axis_type
  void gdlGetAxisType(string axis, bool &log);
  //get the !axis.crange vector
  void gdlGetCurrentAxisRange(string axis, DDouble &Start, DDouble &End);
  void gdlGetDesiredAxisMargin(EnvT *e, string axis,DFloat &start, DFloat &end);
  void gdlGetDesiredAxisCharsize(EnvT* e, string axis, DFloat &charsize);
  void gdlGetDesiredAxisStyle(EnvT *e, string axis,DLong &style);
  void gdlGetDesiredAxisTitle(EnvT *e, string axis,DString &title);
  bool gdlGetDesiredAxisRange(EnvT *e, string axis,
		      DDouble &start, DDouble &end);
  //set the axis 'axis' charsize (including MULTI subscaling)
  void gdlSetAxisCharsize(EnvT *e, GDLGStream *a, string axis);
  void gdlStoreAxisType(string axis, bool type);

  void mesh_nr(PLFLT *, PLFLT *, PLFLT **, PLINT, PLINT, PLINT);

  //length and height of a char in normalized coords, using trick
  void gdlGetCharSizes(GDLGStream *a, PLFLT &nsx, PLFLT &nsy, DDouble &wsx, DDouble &wsy, DDouble &dsx, DDouble &dsy, DDouble &lsx, DDouble &lsy);
  void GetSFromPlotStructs(DDouble **sx, DDouble **sy);
  void GetWFromPlotStructs(DFloat **wx, DFloat **wy);
  bool startClipping(EnvT *e, GDLGStream *a, bool UsePClip);
  void stopClipping(GDLGStream *a);
  void gdlStoreCLIP(DLongGDL* clipBox);
  void DataCoordLimits(DDouble *sx, DDouble *sy, DFloat *wx, DFloat *wy, 
    DDouble *xStart, DDouble *xEnd, DDouble *yStart, DDouble *yEnd, bool);

  PLFLT AutoIntvAC(DDouble &val_min, DDouble &val_max, bool log = false);
  PLFLT AutoTick(DDouble x);
  PLFLT gdlComputeTickInterval(EnvT *e, string axis, DDouble &min, DDouble &max, bool log);
  bool gdlYaxisNoZero(EnvT* e);
  void AdjustAxisOpts(string& xOpt, string& yOpt,
    DLong xStyle, DLong yStyle, DLong xTicks, DLong yTicks,
    string& xTickformat, string& yTickformat, DLong xLog, DLong yLog);
  bool gdlSetViewPortAndWorldCoordinates( EnvT* e, GDLGStream* actStream, DFloatGDL* plotPosition, bool xLog, bool yLog,
                 DFloat xMarginL, DFloat xMarginR, DFloat yMarginB, DFloat yMarginT, // input/output
                 DDouble xStart, DDouble xEnd, DDouble minVal, DDouble maxVal, DLong iso);
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
  void GetPData2 (pstruct& p);
    void CheckMargin( EnvT* e, GDLGStream* actStream,
                    DFloat xMarginL, DFloat xMarginR, DFloat yMarginB, DFloat yMarginT,
                    PLFLT& xMR, PLFLT& xML, PLFLT& yMB, PLFLT& yMT);
    void handle_pmulti_position(EnvT *e, GDLGStream *a);
    void UpdateSWPlotStructs(GDLGStream* actStream, DDouble xStart, DDouble xEnd, DDouble yStart, DDouble yEnd, bool xLog, bool yLog);
     bool gdlAxis(EnvT *e, GDLGStream *a, string axis, DDouble Start, DDouble End, bool Log, DLong modifierCode=0);
    bool gdlBox(EnvT *e, GDLGStream *a, DDouble xStart, DDouble xEnd, DDouble yStart, DDouble yEnd, bool xLog, bool yLog);
} // namespace

#endif
