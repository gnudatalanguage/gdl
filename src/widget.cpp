/***************************************************************************
                             widget.cpp  -  GDL WIDGET_XXX library functions
                                            the actual library routines
                                            (widget system in gdlwidget.cpp)
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
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

#include <iostream>

#include "datatypes.hpp"
#include "envt.hpp"

#ifdef HAVE_LIBWXWIDGETS

#include "gdlwidget.hpp"

namespace lib {
  using namespace std;

  BaseGDL* widget_base( EnvT* e)
  {
    SizeT nParam = e->NParam();

    WidgetIDT parentID = 0;
    if( nParam == 1) // no TLB
      e->AssureLongScalarPar( 0, parentID);

    // handle some keywords
    static int align_bottomIx = e->KeywordIx( "ALIGN_BOTTOM");
    static int align_centerIx = e->KeywordIx( "ALIGN_CENTER");
    static int align_leftIx   = e->KeywordIx( "ALIGN_LEFT");
    static int align_rightIx  = e->KeywordIx( "ALIGN_RIGHT");
    static int align_topIx    = e->KeywordIx( "ALIGN_TOP");
    static int mbarIx  = e->KeywordIx( "MBAR");
    static int modalIx = e->KeywordIx( "MODAL");
    static int base_align_bottomIx = e->KeywordIx( "BASE_ALIGN_BOTTOM");
    static int base_align_centerIx = e->KeywordIx( "BASE_ALIGN_CENTER");
    static int base_align_leftIx   = e->KeywordIx( "BASE_ALIGN_LEFT");
    static int base_align_rightIx  = e->KeywordIx( "BASE_ALIGN_RIGHT");
    static int base_align_topIx    = e->KeywordIx( "BASE_ALIGN_TOP");
    static int columnIx = e->KeywordIx( "COLUMN");
    static int rowIx    = e->KeywordIx( "ROW");
    static int context_eventsIx = e->KeywordIx( "CONTEXT_EVENTS");
    static int context_menuIx   = e->KeywordIx( "CONTEXT_MENU");
    static int event_funcIx = e->KeywordIx( "EVENT_FUNC");
    static int event_proIx  = e->KeywordIx( "EVENT_PRO");
    static int exclusiveIx    = e->KeywordIx( "EXCLUSIVE");
    static int nonexclusiveIx = e->KeywordIx( "NONEXCLUSIVE");
    static int floatingIx = e->KeywordIx( "FLOATING");
    static int frameIx    = e->KeywordIx( "FRAME");
    static int func_get_valueIx = e->KeywordIx( "FUNC_GET_VALUE");
    static int grid_layoutIx    = e->KeywordIx( "GRID_LAYOUT");
    static int group_leaderIx   = e->KeywordIx( "GROUP_LEADER");
    static int kbrd_focus_eventsIx = e->KeywordIx( "KBRD_FOCUS_EVENTS");
    static int kill_notifyIx = e->KeywordIx( "KILL_NOTIFY");
    static int mapIx         = e->KeywordIx( "MAP");
    static int no_copyIx     = e->KeywordIx( "NO_COPY");
    static int notify_realizeIx = e->KeywordIx( "NOTIFY_REALIZE");
    static int pro_set_valueIx  = e->KeywordIx( "PRO_SET_VALUE");
    static int scr_xsizeIx = e->KeywordIx( "SCR_XSIZE");
    static int scr_ysizeIx = e->KeywordIx( "SCR_YSIZE");
    static int scrollIx    = e->KeywordIx( "SCROLL");
    static int sensitiveIx = e->KeywordIx( "SENSITIVE");
    static int spaceIx     = e->KeywordIx( "SPACE");
    static int titleIx     = e->KeywordIx( "TITLE");
    static int tlb_frame_attrIx = e->KeywordIx( "TLB_FRAME_ATTR");
    static int tlb_iconify_eventsIx = e->KeywordIx( "TLB_ICONIFY_EVENTS");
    static int tlb_kill_request_eventsIx = e->KeywordIx( "TLB_KILL_REQUEST_EVENTS");
    static int tlb_move_eventsIx = e->KeywordIx( "TLB_MOVE_EVENTS");
    static int tlb_size_eventsIx = e->KeywordIx( "TLB_SIZE_EVENTS");
    static int toolbarIx = e->KeywordIx( "TOOLBAR");
    static int tracking_eventsIx = e->KeywordIx( "TRACKING_EVENTS");
    static int unitsIx = e->KeywordIx( "UNITS");
    static int uvalueIx = e->KeywordIx( "UVALUE");
    static int unameIx = e->KeywordIx( "UNAME");
    static int xoffsetIx = e->KeywordIx( "XOFFSET");
    static int xpadIx = e->KeywordIx( "XPAD");
    static int xsizeIx = e->KeywordIx( "XSIZE");
    static int x_scroll_sizeIx = e->KeywordIx( "X_SCROLL_SIZE");
    static int yoffsetIx = e->KeywordIx( "YOFFSET");
    static int ypadIx = e->KeywordIx( "YPAD");
    static int ysizeIx = e->KeywordIx( "YSIZE");
    static int y_scroll_sizeIx = e->KeywordIx( "Y_SCROLL_SIZE");
    static int display_nameIx = e->KeywordIx( "DISPLAY_NAME");
    static int resource_nameIx = e->KeywordIx( "RESOURCE_NAME");
    static int rname_mbarIx = e->KeywordIx( "RNAME_MBAR");

    // own alignment
    bool align_bottom = e->KeywordSet( align_bottomIx);
    bool align_center = e->KeywordSet( align_centerIx);
    bool align_left = e->KeywordSet( align_leftIx);
    bool align_right = e->KeywordSet( align_rightIx);
    bool align_top = e->KeywordSet( align_topIx);

    // children default alignment
    bool base_align_bottom = e->KeywordSet( base_align_bottomIx);
    bool base_align_center = e->KeywordSet( base_align_centerIx);
    bool base_align_left = e->KeywordSet( base_align_leftIx);
    bool base_align_right = e->KeywordSet( base_align_rightIx);
    bool base_align_top = e->KeywordSet( base_align_topIx);

    bool modal = e->KeywordSet( modalIx);

    bool context_events = e->KeywordSet( context_eventsIx);
    bool context_menu = e->KeywordSet( context_menuIx);

    bool exclusive = e->KeywordSet( exclusiveIx);
    bool nonexclusive = e->KeywordSet( nonexclusiveIx);

    bool floating = e->KeywordSet( floatingIx);
    bool grid_layout = e->KeywordSet( grid_layoutIx);
    bool kbrd_focus_events = e->KeywordSet( kbrd_focus_eventsIx);
    bool mapWid = e->KeywordSet( mapIx);
    bool no_copy = e->KeywordSet( no_copyIx);
    bool scroll = e->KeywordSet( scrollIx);
    bool sensitive = e->KeywordSet( sensitiveIx);
    bool space = e->KeywordSet( spaceIx);
    bool tlb_frame_attr = e->KeywordSet( tlb_frame_attrIx);
    bool tlb_iconify_events = e->KeywordSet( tlb_iconify_eventsIx);
    bool tlb_kill_request_events = e->KeywordSet( tlb_kill_request_eventsIx);
    bool tlb_move_events = e->KeywordSet( tlb_move_eventsIx);
    bool tlb_size_events = e->KeywordSet( tlb_size_eventsIx);
    bool toolbar = e->KeywordSet( toolbarIx);
    bool tracking_events = e->KeywordSet( tracking_eventsIx);

    // non-bool
    WidgetIDT group_leader = 0;
    e->AssureLongScalarKWIfPresent( group_leaderIx, group_leader);

    bool mbarPresent = e->KeywordPresent( mbarIx);

    DLong column = 0;
    e->AssureLongScalarKWIfPresent( columnIx, column);
    DLong row = 0;
    e->AssureLongScalarKWIfPresent( rowIx, row);

    DLong xoffset = 0;
    e->AssureLongScalarKWIfPresent( xoffsetIx, xoffset);
    DLong xpad = 0;
    e->AssureLongScalarKWIfPresent( xpadIx, xpad);
    DLong yoffset = 0;
    e->AssureLongScalarKWIfPresent( yoffsetIx, yoffset);
    DLong ypad = 0;
    e->AssureLongScalarKWIfPresent( ypadIx, ypad);

    DLong frame = 0;
    e->AssureLongScalarKWIfPresent( frameIx, frame);

    DLong units = 0;
    e->AssureLongScalarKWIfPresent( unitsIx, units);

    DLong xsize = -1;
    e->AssureLongScalarKWIfPresent( xsizeIx, xsize);
    DLong ysize = -1;
    e->AssureLongScalarKWIfPresent( ysizeIx, ysize);

    DLong scr_xsize = 0;
    e->AssureLongScalarKWIfPresent( scr_xsizeIx, scr_xsize);
    DLong scr_ysize = 0;
    e->AssureLongScalarKWIfPresent( scr_ysizeIx, scr_ysize);

    DLong x_scroll_size = 0;
    e->AssureLongScalarKWIfPresent( x_scroll_sizeIx, x_scroll_size);
    DLong y_scroll_size = 0;
    e->AssureLongScalarKWIfPresent( y_scroll_sizeIx, y_scroll_size);
    
    BaseGDL* uvalue = e->GetKW( uvalueIx);
    if( uvalue != NULL)
      if( no_copy)
	e->GetKW( uvalueIx) = NULL;
      else
	uvalue = uvalue->Dup();
    
    DString event_func = "";
    e->AssureStringScalarKWIfPresent( event_funcIx, event_func);

    DString event_pro = "";
    e->AssureStringScalarKWIfPresent( event_proIx, event_pro);

    DString kill_notify = "";
    e->AssureStringScalarKWIfPresent( kill_notifyIx, kill_notify);

    DString notify_realize = "";
    e->AssureStringScalarKWIfPresent( notify_realizeIx, notify_realize);

    DString pro_set_value = "";
    e->AssureStringScalarKWIfPresent( pro_set_valueIx, pro_set_value);

    DString func_get_value = "";
    e->AssureStringScalarKWIfPresent( func_get_valueIx, func_get_value);


    DString resource_name = "";
    e->AssureStringScalarKWIfPresent( resource_nameIx, resource_name);

    DString rname_mbar = "";
    e->AssureStringScalarKWIfPresent( rname_mbarIx, rname_mbar);

    DString title = "GDL";
    e->AssureStringScalarKWIfPresent( titleIx, title);

    DString uname = "";
    e->AssureStringScalarKWIfPresent( unameIx, uname);

    DString display_name = "";
    e->AssureStringScalarKWIfPresent( display_nameIx, display_name);

    // consistency
    if( nonexclusive && exclusive)
      e->Throw( "Conflicting keywords.");

    if( mbarPresent)
      {
	if( parentID != 0)
	  e->Throw( "Only top level bases allow a menubar.");
	e->AssureGlobalKW( mbarIx);
      }

    if( modal && group_leader == 0)
      e->Throw( "MODAL top level bases must have a group leader specified.");

    if( parentID != 0)
      { 
	GDLWidget* p = GDLWidget::GetWidget( parentID);
	if( p == NULL)
	  e->Throw( "Invalid widget identifier: "+i2s(parentID));
	
	GDLWidgetBase* bp = dynamic_cast< GDLWidgetBase*>( p);
	if( p == NULL)
	  e->Throw( "Parent is of incorrect type.");
      }
    //...

    // generate widget
    WidgetIDT mBarID = 0;
    if( mbarPresent)
      {
	GDLWidgetMBar* mBar = new GDLWidgetMBar(); 
	mBarID = GDLWidget::NewWidget( mBar);
	e->SetKW( mbarIx, new DLongGDL( mBarID));
      }

    int exclusiveMode = 0;
    if( exclusive)    exclusiveMode = 1;
    if( nonexclusive) exclusiveMode = 2;

    DLong events;
    
    GDLWidgetBase* base = new GDLWidgetBase( parentID, 
					     uvalue, uname,
					     sensitive, mapWid,
					     mBarID, modal, group_leader,
					     column, row,
					     events,
					     exclusiveMode, 
					     floating,
					     event_func, event_pro,
					     pro_set_value, func_get_value,
					     notify_realize, kill_notify,
					     resource_name, rname_mbar,
					     title,
					     frame, units,
					     display_name,
					     xpad, ypad,
					     xoffset, yoffset,
					     xsize, ysize,
					     scr_xsize, scr_ysize,
					     x_scroll_size, y_scroll_size);
    
    // some more properties

    // return widget ID
    return new DLongGDL( base->WidgetID());
  }



  BaseGDL* widget_button( EnvT* e)
  {
    DLongGDL* p0L = e->GetParAs<DLongGDL>( 0);
    WidgetIDT parentID = (*p0L)[0];
    GDLWidget *widget = GDLWidget::GetWidget( parentID);

    static int valueIx     = e->KeywordIx( "VALUE");
    DString value = "";
    e->AssureStringScalarKWIfPresent( valueIx, value);

    GDLWidgetButton* button = new GDLWidgetButton( parentID, value);
 
    return new DLongGDL( button->WidgetID());
  }


  void widget_control( EnvT* e)
  {
    DLongGDL* p0L = e->GetParAs<DLongGDL>( 0);
    WidgetIDT widgetID = (*p0L)[0];
    GDLWidget *widget = GDLWidget::GetWidget( widgetID);

    static int realizeIx = e->KeywordIx( "REALIZE");
    bool realize = e->KeywordSet( realizeIx);

    if (realize) {
      widget->Realize();
    }
  }

} // namespace

#endif
