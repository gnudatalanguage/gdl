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
  void set_plot( EnvT* e);
  void device( EnvT* e);
  void plot( EnvT* e);
  void oplot( EnvT* e);
  void plots( EnvT* e);
  void surface( EnvT* e);
  void contour( EnvT* e);
  void xyouts( EnvT* e);
  void wset( EnvT* e);
  void window( EnvT* e);
  void wshow( EnvT* e);
  void wdelete( EnvT* e);
  void tvlct( EnvT* e);
  void axis( EnvT* e);
  void erase( EnvT* e);



  //helper functions
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

  void gkw_axis_charsize(EnvT* e, string axis, DFloat &charsize);

  void gkw_axis_style(EnvT *e, string axis,DLong &style);
  void gkw_axis_title(EnvT *e, string axis,DString &title);
  void gkw_axis_range(EnvT *e, string axis,
		      DDouble &start, DDouble &end, DLong & ynozero);

  void mesh_nr(PLFLT *, PLFLT *, PLFLT **, PLINT, PLINT, PLINT);


  // Map stuff
#ifdef USE_LIBPROJ4
#define COMPLEX COMPLEX2

extern "C" {
#include "lib_proj.h"
}

  void map_init();
  BaseGDL* map_proj_forward_fun( EnvT* e);
  BaseGDL* map_proj_inverse_fun( EnvT* e);
#endif

  BaseGDL* convert_coord( EnvT* e);

} // namespace

#endif
