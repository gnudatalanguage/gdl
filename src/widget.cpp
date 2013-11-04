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
#include "dinterpreter.hpp"
#include "gdleventhandler.hpp"

#ifdef HAVE_LIBWXWIDGETS
#  include "gdlwidget.hpp"
#endif

#ifdef HAVE_LIBWXWIDGETS
void GDLWidget::SetCommonKeywords( EnvT* e)
{
  static int frameIx = e->KeywordIx( "FRAME");
  static int event_funcIx = e->KeywordIx( "EVENT_FUNC");
  static int event_proIx  = e->KeywordIx( "EVENT_PRO");
  static int func_get_valueIx = e->KeywordIx( "FUNC_GET_VALUE");
  static int pro_set_valueIx  = e->KeywordIx( "PRO_SET_VALUE");
  static int notify_realizeIx = e->KeywordIx( "NOTIFY_REALIZE");
  static int kill_notifyIx = e->KeywordIx( "KILL_NOTIFY");
  static int group_leaderIx   = e->KeywordIx( "GROUP_LEADER");
  static int no_copyIx     = e->KeywordIx( "NO_COPY");
  static int scr_xsizeIx = e->KeywordIx( "SCR_XSIZE");
  static int scr_ysizeIx = e->KeywordIx( "SCR_YSIZE");
  static int scrollIx    = e->KeywordIx( "SCROLL");
  static int sensitiveIx = e->KeywordIx( "SENSITIVE");
  static int unameIx = e->KeywordIx( "UNAME");
  static int unitsIx = e->KeywordIx( "UNITS");
  static int uvalueIx = e->KeywordIx( "UVALUE");
  static int xoffsetIx = e->KeywordIx( "XOFFSET");
  static int xsizeIx = e->KeywordIx( "XSIZE");
  static int yoffsetIx = e->KeywordIx( "YOFFSET");
  static int ysizeIx = e->KeywordIx( "YSIZE");

  scroll = e->KeywordSet( scrollIx);

  sensitive = e->KeywordSet( sensitiveIx);

  groupLeader = 0;
  e->AssureLongScalarKWIfPresent( group_leaderIx, groupLeader);

  frame = 0;
  e->AssureLongScalarKWIfPresent( frameIx, frame);
  units = 0;
  e->AssureLongScalarKWIfPresent( unitsIx, units);
  xSize = -1;
  e->AssureLongScalarKWIfPresent( xsizeIx, xSize);
  ySize = -1;
  e->AssureLongScalarKWIfPresent( ysizeIx, ySize);
  xOffset = -1;
  e->AssureLongScalarKWIfPresent( xoffsetIx, xOffset);
  yOffset = -1;
  e->AssureLongScalarKWIfPresent( yoffsetIx, yOffset);
  scrXSize = 0;
  e->AssureLongScalarKWIfPresent( scr_xsizeIx, scrXSize);
  scrYSize = 0;
  e->AssureLongScalarKWIfPresent( scr_ysizeIx, scrYSize);
  uValue = e->GetKW( uvalueIx);
  if( uValue != NULL) 
  { 
    bool no_copy = e->KeywordSet( no_copyIx);
    if( no_copy) 
      e->GetKW( uvalueIx) = NULL;
    else 
      uValue = uValue->Dup();
  } 
  eventFun = "";
  e->AssureStringScalarKWIfPresent( event_funcIx, eventFun);
  StrUpCaseInplace(eventFun);
  eventPro = "";
  e->AssureStringScalarKWIfPresent( event_proIx, eventPro);
  StrUpCaseInplace(eventPro);
  killNotify = "";
  e->AssureStringScalarKWIfPresent( kill_notifyIx, killNotify);
  StrUpCaseInplace(killNotify);
  notifyRealize = "";
  e->AssureStringScalarKWIfPresent( notify_realizeIx, notifyRealize);
  StrUpCaseInplace(notifyRealize);
  proValue = "";
  e->AssureStringScalarKWIfPresent( pro_set_valueIx, proValue);
  StrUpCaseInplace(proValue);
  funcValue = "";
  e->AssureStringScalarKWIfPresent( func_get_valueIx, funcValue);
  StrUpCaseInplace(funcValue);
  uName = "";
  e->AssureStringScalarKWIfPresent( unameIx, uName);
  // no case change
}    
#endif    

    
// non library functions
// these reside here because gdlwidget.hpp is only included if wxWidgets are used
// and hence putting them there would cause a compiler error without wxWidgets
BaseGDL* CallEventFunc( const std::string& f, BaseGDL* ev)
{
  StackGuard<EnvStackT> guard( BaseGDL::interpreter->CallStack());

  int funIx = GDLInterpreter::GetFunIx( f);

  ProgNodeP callingNode = NULL;//BaseGDL::interpreter->GetRetTree();
  
  EnvUDT* newEnv= new EnvUDT( callingNode, funList[ funIx], NULL);
  newEnv->SetNextPar( ev); // pass as local
  
  BaseGDL::interpreter->CallStack().push_back( newEnv); 

  // make the call
  newEnv->SetCallContext( EnvUDT::RFUNCTION);
  BaseGDL* res = BaseGDL::interpreter->call_fun(static_cast<DSubUD*>(newEnv->GetPro())->GetTree());
  return res;
}

void CallEventPro( const std::string& p, BaseGDL* p0, BaseGDL* p1)
{
  StackGuard<EnvStackT> guard( BaseGDL::interpreter->CallStack());

  int proIx = GDLInterpreter::GetProIx( p);

  ProgNodeP callingNode = NULL;//BaseGDL::interpreter->GetRetTree();
  
  EnvUDT* newEnv= new EnvUDT( callingNode, proList[ proIx], NULL);
  newEnv->SetNextPar( p0); // pass as local
  if( p1 != NULL)
    newEnv->SetNextPar( p1); // pass as local
  
  BaseGDL::interpreter->CallStack().push_back( newEnv); 

  // make the call
  BaseGDL::interpreter->call_pro(static_cast<DSubUD*>(newEnv->GetPro())->GetTree());
}

DStructGDL* CallEventHandler( /*DLong id,*/ DStructGDL* ev)
{
#ifdef HAVE_LIBWXWIDGETS
  static int idIx = ev->Desc()->TagIndex("ID"); // 0
  static int topIx = ev->Desc()->TagIndex("TOP"); // 1
  static int handlerIx = ev->Desc()->TagIndex("HANDLER"); // 2

  DLong actID = (*static_cast<DLongGDL*>(ev->GetTag(idIx,0)))[0];

  // note that such a struct name is illegal in GDL
  // therefore it cannot be used in user code.
  // This is safer than choosing a legal name
  // as it could collide with user code
  if( ev->Desc()->Name() == "*WIDGET_MESSAGE*")
  {
    GDLWidget* widget = GDLWidget::GetWidget( actID);
    if( widget == NULL)
    {
      Warning("CallEventHandler: *WIDGET_MESSAGE*: Internal error: Destroy request for already destroyed widget. ID: "+i2s(actID));
      return NULL;
    }
    
    static int messageIx = ev->Desc()->TagIndex("MESSAGE");
    DLong message = (*static_cast<DLongGDL*>(ev->GetTag(messageIx,0)))[0];

    GDLDelete( ev);

    assert( message == 0); // only '0' -> Destroy for now

    assert( widget->IsBase());

//     std::cout << "CallEventHandler: *WIDGET_MESSAGE*: Deleting widget: "+i2s(actID) << std::endl;

    delete widget; // removes itself from widgetList

    return NULL;
  }

  do {
    GDLWidget *widget = GDLWidget::GetWidget( actID);
    if( widget == NULL)
    {
      Warning("CallEventHandler: Widget no longer valid. ID: " + i2s(actID));
      actID = GDLWidget::NullID;
    }
    else
    {
      DString eventHandlerPro = widget->GetEventPro();
      if( eventHandlerPro != "")
      {
	(*static_cast<DLongGDL*>(ev->GetTag(handlerIx, 0)))[0] = actID;
	CallEventPro( eventHandlerPro, ev); // grabs ev
	ev = NULL;
	break; // out of while
      }
      DString eventHandlerFun = widget->GetEventFun();
      if( eventHandlerFun != "")
      {
	(*static_cast<DLongGDL*>(ev->GetTag(handlerIx, 0)))[0] = actID;
	BaseGDL* retVal = CallEventFunc( eventHandlerFun, ev); // grabs ev
	if( retVal->Type() == GDL_STRUCT)
	{
	  // ev is already deleted
	  ev = static_cast<DStructGDL*>( retVal); 
	  if( ev->Desc()->TagIndex("ID") != idIx ||
	    ev->Desc()->TagIndex("TOP") != topIx ||
	    ev->Desc()->TagIndex("HANDLER") != handlerIx)
	  {
	    GDLDelete( ev);
	    throw GDLException(eventHandlerFun+ ": Event handler return struct must contain ID, TOP, HANDLER as first tags.");
	  }
	  // no break!
	}
	else
	{
	  GDLDelete( retVal);
	  ev = NULL;
	  break; // out of while   
	}
      }
      actID = widget->GetParentID();
    }
  }
  while( actID != GDLWidget::NullID);
#endif
  return ev;
}




namespace lib {
  using namespace std;

BaseGDL* widget_table( EnvT* e)
{
#ifndef HAVE_LIBWXWIDGETS
    e->Throw("GDL was compiled without support for wxWidgets");
    return NULL; // avoid warning
#else
    SizeT nParam=e->NParam(1);

    DLongGDL* p0L = e->GetParAs<DLongGDL>( 0);
    WidgetIDT parentID = (*p0L)[0];
    GDLWidget* p = GDLWidget::GetWidget( parentID);
    if( p == NULL)
        e->Throw( "Invalid widget identifier: "+i2s(parentID));

    static int  ALIGNMENT             = e->KeywordIx( "ALIGNMENT");
    static int  ALL_EVENTS            = e->KeywordIx( "ALL_EVENTS");
    static int  AM_PM                 = e->KeywordIx( "AM_PM");
    static int  BACKGROUND_COLOR      = e->KeywordIx( "BACKGROUND_COLOR");
    static int  COLUMN_LABELS         = e->KeywordIx( "COLUMN_LABELS");
    static int  COLUMN_MAJOR          = e->KeywordIx( "COLUMN_MAJOR");
    static int  ROW_MAJOR             = e->KeywordIx( "ROW_MAJOR");
    static int  COLUMN_WIDTHS         = e->KeywordIx( "COLUMN_WIDTHS");
    static int  CONTEXT_EVENTS        = e->KeywordIx( "CONTEXT_EVENTS");
    static int  DAYS_OF_WEEK          = e->KeywordIx( "DAYS_OF_WEEK");
    static int  DISJOINT_SELECTION    = e->KeywordIx( "DISJOINT_SELECTION");
    static int  EDITABLE              = e->KeywordIx( "EDITABLE");
    static int  FONT                  = e->KeywordIx( "FONT");
    static int  FOREGROUND_COLOR      = e->KeywordIx( "FOREGROUND_COLOR");
    static int  FORMAT                = e->KeywordIx( "FORMAT");
    static int  GROUP_LEADER          = e->KeywordIx( "GROUP_LEADER");
    static int  IGNORE_ACCELERATORS   = e->KeywordIx( "IGNORE_ACCELERATORS");
    static int  KBRD_FOCUS_EVENTS     = e->KeywordIx( "KBRD_FOCUS_EVENTS");
    static int  MONTHS                = e->KeywordIx( "MONTHS");
    static int  NO_COLUMN_HEADERS     = e->KeywordIx( "NO_COLUMN_HEADERS");
    static int  NO_HEADERS            = e->KeywordIx( "NO_HEADERS");
    static int  NO_ROW_HEADERS        = e->KeywordIx( "NO_ROW_HEADERS");
    static int  RESIZEABLE_COLUMNS    = e->KeywordIx( "RESIZEABLE_COLUMNS");
    static int  RESIZEABLE_ROWS       = e->KeywordIx( "RESIZEABLE_ROWS");
    static int  ROW_HEIGHTS           = e->KeywordIx( "ROW_HEIGHTS");
    static int  ROW_LABELS            = e->KeywordIx( "ROW_LABELS");
    static int  TAB_MODE              = e->KeywordIx( "TAB_MODE");
    static int  TRACKING_EVENTS       = e->KeywordIx( "TRACKING_EVENTS");
    static int  VALUE                 = e->KeywordIx( "VALUE");

    static int x_scroll_sizeIx = e->KeywordIx( "X_SCROLL_SIZE");
    DLong x_scroll_size = 0;
    e->AssureLongScalarKWIfPresent( x_scroll_sizeIx, x_scroll_size);
    static int y_scroll_sizeIx = e->KeywordIx( "Y_SCROLL_SIZE");
    DLong y_scroll_size = 0;
    e->AssureLongScalarKWIfPresent( y_scroll_sizeIx, y_scroll_size);
    

    // TODO

    return 0;
#endif
} // widget_table
  
  BaseGDL* widget_draw( EnvT* e)
  {
#ifndef HAVE_LIBWXWIDGETS
    e->Throw("GDL was compiled without support for wxWidgets");
    return NULL; // avoid warning
#else
    SizeT nParam=e->NParam(1);

    DLongGDL* p0L = e->GetParAs<DLongGDL>( 0);
    WidgetIDT parentID = (*p0L)[0];
    GDLWidget* p = GDLWidget::GetWidget( parentID);
    if( p == NULL)
      e->Throw( "Invalid widget identifier: "+i2s(parentID));
    
    GDLWidgetBase* base = dynamic_cast< GDLWidgetBase*>( p);
    if( base == NULL)
      e->Throw( "Parent is of incorrect type.");

    static int x_scroll_sizeIx = e->KeywordIx( "X_SCROLL_SIZE");\
    DLong x_scroll_size = 0;\
    e->AssureLongScalarKWIfPresent( x_scroll_sizeIx, x_scroll_size);\
    static int y_scroll_sizeIx = e->KeywordIx( "Y_SCROLL_SIZE");\
    DLong y_scroll_size = 0;\
    e->AssureLongScalarKWIfPresent( y_scroll_sizeIx, y_scroll_size);\

    static int DROP_EVENTS = e->KeywordIx( "DROP_EVENTS");
    static int EXPOSE_EVENTS = e->KeywordIx( "EXPOSE_EVENTS");
    static int MOTION_EVENTS = e->KeywordIx( "MOTION_EVENTS");
    static int TRACKING_EVENTS = e->KeywordIx( "TRACKING_EVENTS");
    static int VIEWPORT_EVENTS = e->KeywordIx( "VIEWPORT_EVENTS");
    static int WHEEL_EVENTS = e->KeywordIx( "WHEEL_EVENTS");

    // flags
    bool drop_events = e->KeywordSet( DROP_EVENTS);
    bool expose_events = e->KeywordSet( EXPOSE_EVENTS);
    bool motion_events = e->KeywordSet( MOTION_EVENTS);
    bool tracking_events = e->KeywordSet( TRACKING_EVENTS);
    bool viewport_events = e->KeywordSet( VIEWPORT_EVENTS);
    bool wheel_events = e->KeywordSet( WHEEL_EVENTS);

    // TODO non-flags
    static int APP_SCROLL = e->KeywordIx( "APP_SCROLL");
    static int BUTTON_EVENTS = e->KeywordIx( "BUTTON_EVENTS");
    static int CLASSNAME = e->KeywordIx( "CLASSNAME"); // string
    static int COLOR_MODEL = e->KeywordIx( "COLOR_MODEL");
    static int COLORS = e->KeywordIx( "COLORS"); // long
    static int DRAG_NOTIFY = e->KeywordIx( "DRAG_NOTIFY"); //string
    static int GRAPHICS_LEVEL = e->KeywordIx( "GRAPHICS_LEVEL");
    static int IGNORE_ACCELERATORS = e->KeywordIx( "IGNORE_ACCELERATORS");
    static int KEYBOARD_EVENTS = e->KeywordIx( "KEYBOARD_EVENTS"); // 1, 2
    static int RENDERER = e->KeywordIx( "RENDERER");
    static int RESOURCE_NAME = e->KeywordIx( "RESOURCE_NAME"); // string
    static int RETAIN = e->KeywordIx( "RETAIN");
    static int TOOLTIP = e->KeywordIx( "TOOLTIP");

//     static int FRAME = e->KeywordIx( "FRAME");  // width
//     DLong frame = 0;
//     e->AssureLongScalarKWIfPresent( FRAME, frame);


    GDLWidgetDraw* draw = new GDLWidgetDraw( parentID, e,
					  x_scroll_size, y_scroll_size);

    // return widget ID
    return new DLongGDL( draw->WidgetID());
#endif  
  } // widget_draw
  
  
  
    
  BaseGDL* widget_base( EnvT* e)
  {
#ifndef HAVE_LIBWXWIDGETS
    e->Throw("GDL was compiled without support for wxWidgets");
    return NULL; // avoid warning
#else
    SizeT nParam = e->NParam();

    WidgetIDT parentID = 0;
    if( nParam == 1) // no TLB
      e->AssureLongScalarPar( 0, parentID);
    
    // see widget_draw
//     SET_COMMON_GRAPHICS_KEYWORDS

    // handle some more keywords
    static int x_scroll_sizeIx = e->KeywordIx( "X_SCROLL_SIZE");
    DLong x_scroll_size = 0;
    e->AssureLongScalarKWIfPresent( x_scroll_sizeIx, x_scroll_size);
    static int y_scroll_sizeIx = e->KeywordIx( "Y_SCROLL_SIZE");
    DLong y_scroll_size = 0;
    e->AssureLongScalarKWIfPresent( y_scroll_sizeIx, y_scroll_size);

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
//     static int event_funcIx = e->KeywordIx( "EVENT_FUNC");
//     static int event_proIx  = e->KeywordIx( "EVENT_PRO");
    static int exclusiveIx    = e->KeywordIx( "EXCLUSIVE");
    static int nonexclusiveIx = e->KeywordIx( "NONEXCLUSIVE");
    static int floatingIx = e->KeywordIx( "FLOATING");
    static int frameIx    = e->KeywordIx( "FRAME");
//     static int func_get_valueIx = e->KeywordIx( "FUNC_GET_VALUE");
    static int grid_layoutIx    = e->KeywordIx( "GRID_LAYOUT");
//     static int group_leaderIx   = e->KeywordIx( "GROUP_LEADER");
    static int kbrd_focus_eventsIx = e->KeywordIx( "KBRD_FOCUS_EVENTS");
//     static int kill_notifyIx = e->KeywordIx( "KILL_NOTIFY");
    static int mapIx         = e->KeywordIx( "MAP");
//     static int no_copyIx     = e->KeywordIx( "NO_COPY");
//     static int notify_realizeIx = e->KeywordIx( "NOTIFY_REALIZE");
//     static int pro_set_valueIx  = e->KeywordIx( "PRO_SET_VALUE");
//     static int scr_xsizeIx = e->KeywordIx( "SCR_XSIZE");
//     static int scr_ysizeIx = e->KeywordIx( "SCR_YSIZE");
//     static int scrollIx    = e->KeywordIx( "SCROLL");
//     static int sensitiveIx = e->KeywordIx( "SENSITIVE");
    static int spaceIx     = e->KeywordIx( "SPACE");
    static int titleIx     = e->KeywordIx( "TITLE");
    static int tlb_frame_attrIx = e->KeywordIx( "TLB_FRAME_ATTR");
    static int tlb_iconify_eventsIx = e->KeywordIx( "TLB_ICONIFY_EVENTS");
    static int tlb_kill_request_eventsIx = e->KeywordIx( "TLB_KILL_REQUEST_EVENTS");
    static int tlb_move_eventsIx = e->KeywordIx( "TLB_MOVE_EVENTS");
    static int tlb_size_eventsIx = e->KeywordIx( "TLB_SIZE_EVENTS");
    static int toolbarIx = e->KeywordIx( "TOOLBAR");
    static int tracking_eventsIx = e->KeywordIx( "TRACKING_EVENTS");
//     static int unameIx = e->KeywordIx( "UNAME");
//     static int unitsIx = e->KeywordIx( "UNITS");
//     static int uvalueIx = e->KeywordIx( "UVALUE");
//     static int xoffsetIx = e->KeywordIx( "XOFFSET");
    static int xpadIx = e->KeywordIx( "XPAD");
//     static int xsizeIx = e->KeywordIx( "XSIZE");
//     static int x_scroll_sizeIx = e->KeywordIx( "X_SCROLL_SIZE");
//     static int yoffsetIx = e->KeywordIx( "YOFFSET");
    static int ypadIx = e->KeywordIx( "YPAD");
//     static int ysizeIx = e->KeywordIx( "YSIZE");
//     static int y_scroll_sizeIx = e->KeywordIx( "Y_SCROLL_SIZE");
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

    bool mapWid = true;
    if ( e->KeywordPresent( mapIx))
      if ( !e->KeywordSet( mapIx))
	mapWid = false;
    //    std::cout << "Map in widget_base: " << mapWid << std::endl;

//     bool no_copy = e->KeywordSet( no_copyIx);
//     bool scroll = e->KeywordSet( scrollIx);
//     bool sensitive = e->KeywordSet( sensitiveIx);
    bool space = e->KeywordSet( spaceIx);
    bool tlb_frame_attr = e->KeywordSet( tlb_frame_attrIx);
    bool tlb_iconify_events = e->KeywordSet( tlb_iconify_eventsIx);
    bool tlb_kill_request_events = e->KeywordSet( tlb_kill_request_eventsIx);
    bool tlb_move_events = e->KeywordSet( tlb_move_eventsIx);
    bool tlb_size_events = e->KeywordSet( tlb_size_eventsIx);
    bool toolbar = e->KeywordSet( toolbarIx);
    bool tracking_events = e->KeywordSet( tracking_eventsIx);

    // non-bool
//     WidgetIDT group_leader = 0;
//     e->AssureLongScalarKWIfPresent( group_leaderIx, group_leader);
 
    bool mbarPresent = e->KeywordPresent( mbarIx);

    DLong column = 0;
    e->AssureLongScalarKWIfPresent( columnIx, column);
    DLong row = 0;
    e->AssureLongScalarKWIfPresent( rowIx, row);

    DLong xpad = 0;
    e->AssureLongScalarKWIfPresent( xpadIx, xpad);
    DLong ypad = 0;
    e->AssureLongScalarKWIfPresent( ypadIx, ypad);

    DLong frame = 0;
    e->AssureLongScalarKWIfPresent( frameIx, frame);

    DString resource_name = "";
    e->AssureStringScalarKWIfPresent( resource_nameIx, resource_name);

    DString rname_mbar = "";
    e->AssureStringScalarKWIfPresent( rname_mbarIx, rname_mbar);

    DString title = "GDL";
    e->AssureStringScalarKWIfPresent( titleIx, title);

    DString display_name = "";
    e->AssureStringScalarKWIfPresent( display_nameIx, display_name);

    // consistency
    if( nonexclusive && exclusive)
      e->Throw( "Conflicting keywords: [NON]EXCLUSIVE");

    if( mbarPresent)
      {
	if( parentID != 0)
	  e->Throw( "Only top level bases allow a menubar.");
	e->AssureGlobalKW( mbarIx);
      }

    if( modal)
    {
	static int group_leaderIx = e->KeywordIx( "GROUP_LEADER");
	DLong groupLeader = 0;
	e->AssureLongScalarKWIfPresent( group_leaderIx, groupLeader);
	if( groupLeader == 0)
	  e->Throw( "MODAL top level bases must have a group leader specified.");
	if( parentID != 0)
	  e->Throw( "Only top level bases can be MODAL.");
    }
    if( parentID != 0)
      { 
	GDLWidget* p = GDLWidget::GetWidget( parentID);
	if( p == NULL)
	  e->Throw( "Invalid widget identifier: "+i2s(parentID));
	
// 	GDLWidgetBase* bp = dynamic_cast< GDLWidgetBase*>( p);
	if( !p->IsBase() && !p->IsTab())
	  e->Throw( "Parent must be a WIDGET_BASE or WIDGET_TAB.");
      }
    //...

    // generate widget
    WidgetIDT mBarID = mbarPresent ? 1 : 0;

    int exclusiveMode = GDLWidget::BGNORMAL;
    if( exclusive)    exclusiveMode = GDLWidget::BGEXCLUSIVE;
    if( nonexclusive) exclusiveMode = GDLWidget::BGNONEXCLUSIVE;

    DLong events = 0;

    GDLWidgetBase* base = new GDLWidgetBase( parentID, e, 
					      mapWid,
					     /*ref*/ mBarID, modal,
					     column, row,
					     events,
					     exclusiveMode, 
					     floating,
					     resource_name, rname_mbar,
					     title,
					     display_name,
					     xpad, ypad,
					     x_scroll_size, y_scroll_size);
//     GDLWidgetBase* base = new GDLWidgetBase( parentID, 
// 					     uvalue, uname,
// 					     sensitive, mapWid,
// 					     /*ref*/ mBarID, modal, group_leader,
// 					     column, row,
// 					     events,
// 					     exclusiveMode, 
// 					     floating,
// 					     event_func, event_pro,
// 					     pro_set_value, func_get_value,
// 					     notify_realize, kill_notify,
// 					     resource_name, rname_mbar,
// 					     title,
// 					     frame, units,
// 					     display_name,
// 					     xpad, ypad,
// 					     xoffset, yoffset,
// 					     xsize, ysize,
// 					     scr_xsize, scr_ysize,
// 					     x_scroll_size, y_scroll_size);
    
    // some more properties
    if( mbarPresent)
    {
      e->SetKW( mbarIx, new DLongGDL( mBarID));
    }

    // return widget ID
    return new DLongGDL( base->WidgetID());
#endif
  }


  // WIDGET_BUTTON
  BaseGDL* widget_button( EnvT* e)
  {
#ifndef HAVE_LIBWXWIDGETS
    e->Throw("GDL was compiled without support for wxWidgets");
    return NULL; // avoid warning
#else
    SizeT nParam=e->NParam(1);

    DLongGDL* p0L = e->GetParAs<DLongGDL>( 0);
    WidgetIDT parentID = (*p0L)[0];
    GDLWidget *widget = GDLWidget::GetWidget( parentID);

    static int valueIx = e->KeywordIx( "VALUE");
    DString value = "";
    e->AssureStringScalarKWIfPresent( valueIx, value);

    GDLWidgetButton* button = new GDLWidgetButton( parentID, e, value);

    button->SetWidgetType( "BUTTON");

    return new DLongGDL( button->WidgetID());
#endif
  }

// // WIDGET CW_BGROUP
// BaseGDL* widget_bgroup( EnvT* e)
// {
// #ifndef HAVE_LIBWXWIDGETS
//     e->Throw("GDL was compiled without support for wxWidgets");
//     return NULL; // avoid warning
// #else
//     SizeT nParam=e->NParam(1);
// 
//     //SizeT nParam = e->NParam();
//     DLongGDL* p0L = e->GetParAs<DLongGDL>( 0);
//     WidgetIDT parentID = (*p0L)[0];
// 
//     DStringGDL* names = e->GetParAs<DStringGDL>(1);
// 
//     GDLWidget *widget = GDLWidget::GetWidget( parentID);
// 
//     DLong xsize = -1;
//     static int xsizeIx = e->KeywordIx( "XSIZE");
//     e->AssureLongScalarKWIfPresent( xsizeIx, xsize);
// 
//     DLong ysize = -1;
//     static int ysizeIx = e->KeywordIx( "YSIZE");
//     e->AssureLongScalarKWIfPresent( ysizeIx, ysize);
// 
//     static int buttonuvalueIx = e->KeywordIx( "BUTTON_UVALUE");
//     DString buttonuvalue = "";
//     e->AssureStringScalarKWIfPresent(buttonuvalueIx, buttonuvalue);
// 
//     static int uvalueIx = e->KeywordIx( "UVALUE");
//     BaseGDL* uvalue = e->GetKW( uvalueIx);
//     if( uvalue != NULL)
//         uvalue = uvalue->Dup();
// 
//     static int labelIx = e->KeywordIx( "LABEL_TOP");
//     DString labeltop = "";
//     e->AssureStringScalarKWIfPresent( labelIx, labeltop);
// 
//     GDLWidgetBGroup::BGroupMode mode = GDLWidgetBGroup::NORMAL;
//     static int modeIx = e->KeywordIx( "EXCLUSIVE");
//     if(e->KeywordSet( modeIx)) {
//         mode = GDLWidgetBGroup::EXCLUSIVE;
//     } else {
//         modeIx = e->KeywordIx( "NONEXCLUSIVE");
//         if(e->KeywordSet(modeIx)) {
//             mode = GDLWidgetBGroup::NONEXCLUSIVE;
//         }
//     }
// 
//     GDLWidgetBGroup::BGroupReturn ret = GDLWidgetBGroup::RETURN_ID;
//     static int retIx = e->KeywordIx( "RETURN_INDEX");
//     if(e->KeywordSet( retIx)) {
//         ret = GDLWidgetBGroup::RETURN_INDEX;
//     } else {
//         retIx = e->KeywordIx( "RETURN_NAME");
//         if(e->KeywordSet(retIx)) {
//             ret = GDLWidgetBGroup::RETURN_NAME;
//         }
//     }
// 
//     DLong rows = -1;
//     static int rowIx = e->KeywordIx("ROW");
//     e->AssureLongScalarKWIfPresent( rowIx, rows);
// 
//     DLong cols = -1;
//     static int colIx = e->KeywordIx("COLUMN");
//     e->AssureLongScalarKWIfPresent( colIx, cols);
// 
//     GDLWidgetBGroup* group = new GDLWidgetBGroup( parentID, names,
//             uvalue, buttonuvalue,
//             xsize, ysize, labeltop,
//             rows, cols, mode, ret);
//     group->SetWidgetType("GROUP");
// 
//     return new DLongGDL( group->WidgetID());
// 
// #endif
// }
// 



// WIDGET_LIST
BaseGDL* widget_list( EnvT* e)
{
#ifndef HAVE_LIBWXWIDGETS
    e->Throw("GDL was compiled without support for wxWidgets");
    return NULL; // avoid warning
#else
    SizeT nParam=e->NParam(1);

    DLongGDL* p0L = e->GetParAs<DLongGDL>( 0);
    WidgetIDT parentID = (*p0L)[0];
    GDLWidget *widget = GDLWidget::GetWidget( parentID);

    static int valueIx = e->KeywordIx( "VALUE");
    BaseGDL* value = e->GetKW( valueIx);
    if( value != NULL)
        value = value->Dup();

    static int multipleIx = e->KeywordIx( "MULTIPLE");
    bool multiple = e->KeywordSet( multipleIx);

    DLong style = multiple ? wxLB_EXTENDED /*wxLB_MULTIPLE*/ : wxLB_SINGLE;
    GDLWidgetList* list = new GDLWidgetList( parentID, e, value, style);
    list->SetWidgetType( "LIST");

    return new DLongGDL( list->WidgetID());
#endif
}

  // WIDGET_DROPLIST
  BaseGDL* widget_droplist( EnvT* e)
  {
#ifndef HAVE_LIBWXWIDGETS
    e->Throw("GDL was compiled without support for wxWidgets");
    return NULL; // avoid warning
#else
    SizeT nParam=e->NParam(1);

    DLongGDL* p0L = e->GetParAs<DLongGDL>( 0);
    WidgetIDT parentID = (*p0L)[0];
    GDLWidget *widget = GDLWidget::GetWidget( parentID);

    static int titleIx = e->KeywordIx( "TITLE");
    DString title = "";
    e->AssureStringScalarKWIfPresent( titleIx, title);

    static int valueIx = e->KeywordIx( "VALUE");
    //    DStringGDL* value = e->IfDefGetKWAs<DStringGDL>( valueIx);
    BaseGDL* value = e->GetKW( valueIx);
    if( value != NULL)
      value = value->Dup();

    DLong style = wxCB_READONLY;
    GDLWidgetDropList* droplist = new GDLWidgetDropList( parentID, e, value, title, style);
    droplist->SetWidgetType( "DROPLIST");

    return new DLongGDL( droplist->WidgetID());
#endif
  }

  
// WIDGET_COMBOBOX
  BaseGDL* widget_combobox( EnvT* e)
  {
#ifndef HAVE_LIBWXWIDGETS
    e->Throw("GDL was compiled without support for wxWidgets");
    return NULL; // avoid warning
#else
    SizeT nParam=e->NParam(1);

    DLongGDL* p0L = e->GetParAs<DLongGDL>( 0);
    WidgetIDT parentID = (*p0L)[0];
    GDLWidget *widget = GDLWidget::GetWidget( parentID);

    static int titleIx = e->KeywordIx( "TITLE");
    DString title = "";
    e->AssureStringScalarKWIfPresent( titleIx, title);

    static int valueIx = e->KeywordIx( "VALUE");
    //    DStringGDL* value = e->IfDefGetKWAs<DStringGDL>( valueIx);
    BaseGDL* value = e->GetKW( valueIx);
    if( value != NULL)
      value = value->Dup();

    static int editableIx = e->KeywordIx( "EDITABLE");
    bool editable = e->KeywordSet( editableIx);

    DLong style = wxCB_DROPDOWN;
    if( !editable)
      style = wxCB_READONLY;

    GDLWidgetDropList* droplist = new GDLWidgetDropList( parentID, e, value, title, style);
    droplist->SetWidgetType( "COMBOBOX");

    return new DLongGDL( droplist->WidgetID());
#endif
  }

 
  BaseGDL* widget_tab( EnvT* e)
  {
#ifndef HAVE_LIBWXWIDGETS
    e->Throw("GDL was compiled without support for wxWidgets");
    return NULL; // avoid warning
#else
    SizeT nParam=e->NParam(1);

    DLongGDL* p0L = e->GetParAs<DLongGDL>( 0);
    WidgetIDT parentID = (*p0L)[0];
    GDLWidget *widget = GDLWidget::GetWidget( parentID);

    DLong multiline = 0;
    static int multilineIx = e->KeywordIx( "MULTILINE");
    e->AssureLongScalarKWIfPresent( multilineIx, multiline);

    DLong location = 0;
    static int locationIx = e->KeywordIx( "LOCATION");
    e->AssureLongScalarKWIfPresent( locationIx, location);

    GDLWidgetTab* tab = new GDLWidgetTab( parentID, e, location, multiline);
    tab->SetWidgetType( "TAB");

    return new DLongGDL( tab->WidgetID());
#endif
  }
  
  BaseGDL* widget_slider( EnvT* e)
  {
#ifndef HAVE_LIBWXWIDGETS
    e->Throw("GDL was compiled without support for wxWidgets");
    return NULL; // avoid warning
#else
    SizeT nParam=e->NParam(1);

    DLongGDL* p0L = e->GetParAs<DLongGDL>( 0);
    WidgetIDT parentID = (*p0L)[0];
    GDLWidget *widget = GDLWidget::GetWidget( parentID);

    DLong minimum = 0;
    static int minimumIx = e->KeywordIx( "MINIMUM");
    e->AssureLongScalarKWIfPresent( minimumIx, minimum);
    DLong maximum = 0;
    static int maximumIx = e->KeywordIx( "MAXIMUM");
    e->AssureLongScalarKWIfPresent( maximumIx, maximum);

    DLong value = minimum;
    static int valueIx = e->KeywordIx( "VALUE");
    e->AssureLongScalarKWIfPresent( valueIx, value);

    static int dragIx = e->KeywordIx( "DRAG");
    bool drag = e->KeywordSet( dragIx);
    
    static int verticalIx = e->KeywordIx( "VERTICAL");
    bool vertical = e->KeywordSet( verticalIx);
    
    static int suppressValueIx = e->KeywordIx( "SUPPRESS_VALUE");
    bool suppressValue = e->KeywordSet( suppressValueIx);
    
    GDLWidgetSlider* sl = new GDLWidgetSlider( parentID, e, 
					     value, minimum, maximum, 
					     vertical, 
					     suppressValue);
    sl->SetWidgetType( "SLIDER");

    return new DLongGDL( sl->WidgetID());
#endif
  }
  
  // WIDGET_TEXT
  BaseGDL* widget_text( EnvT* e)
  {
#ifndef HAVE_LIBWXWIDGETS
    e->Throw("GDL was compiled without support for wxWidgets");
    return NULL; // avoid warning
#else
    SizeT nParam=e->NParam(1);

    DLongGDL* p0L = e->GetParAs<DLongGDL>( 0);
    WidgetIDT parentID = (*p0L)[0];
    GDLWidget *widget = GDLWidget::GetWidget( parentID);

    static int noNewLineIx = e->KeywordIx( "NO_NEWLINE");
    bool noNewLine = e->KeywordSet(noNewLineIx);

    DStringGDL* valueStr = NULL;
    static int valueIx = e->KeywordIx( "VALUE");
    BaseGDL* valueKW = e->GetKW( valueIx);
    if( valueKW != NULL)
    {
      if( valueKW->Type() != GDL_STRING)
	e->Throw("VALUE must be a STRING.");
      valueStr = static_cast<DStringGDL*>(valueKW);
      bool success = e->StealLocalKW(valueIx);
      if( !success)
	valueStr = valueStr->Dup();
    }
    
    DLong edit = 0;
    static int editableIx = e->KeywordIx("EDITABLE");
    e->AssureLongScalarKWIfPresent( editableIx, edit);
    bool editable = edit == 1;

    GDLWidgetText* text = new GDLWidgetText( parentID, e, valueStr, noNewLine, editable);
    text->SetWidgetType( "TEXT");

    return new DLongGDL( text->WidgetID());
#endif
  }


  // WIDGET_LABEL
  BaseGDL* widget_label( EnvT* e)
  {
#ifndef HAVE_LIBWXWIDGETS
    e->Throw("GDL was compiled without support for wxWidgets");
    return NULL; // avoid warning
#else
    SizeT nParam=e->NParam(1);

    DLongGDL* p0L = e->GetParAs<DLongGDL>( 0);
    WidgetIDT parentID = (*p0L)[0];
    GDLWidget *widget = GDLWidget::GetWidget( parentID);

    static int valueIx = e->KeywordIx( "VALUE");
    DString value = "";
    e->AssureStringScalarKWIfPresent( valueIx, value);

    GDLWidgetLabel* label = new GDLWidgetLabel( parentID, e, value);
    label->SetWidgetType( "LABEL");

    return new DLongGDL( label->WidgetID());
#endif
  }


  // WIDGET_INFO
  BaseGDL* widget_info( EnvT* e)
  {
#ifndef HAVE_LIBWXWIDGETS
    e->Throw("GDL was compiled without support for wxWidgets");
    return NULL;
#else
    SizeT nParam=e->NParam();

    DLongGDL* p0L = NULL;
    SizeT nEl = 0;
    SizeT rank = 0;

    if ( nParam > 0) {
      p0L = e->GetParAs<DLongGDL>( 0);
      nEl = p0L->N_Elements();
      rank = p0L->Rank();
    }

    static int validIx = e->KeywordIx( "VALID");
    bool valid = e->KeywordSet( validIx);

    static int modalIx = e->KeywordIx( "MODAL");
    bool modal = e->KeywordSet( modalIx);

    static int managedIx = e->KeywordIx( "MANAGED");
    bool managed = e->KeywordSet( managedIx);

    static int xmanagerBlockIx = e->KeywordIx( "XMANAGER_BLOCK");
    bool xmanagerBlock = e->KeywordSet( xmanagerBlockIx);

    static int childIx = e->KeywordIx( "CHILD");
    bool child = e->KeywordSet( childIx);

    static int versionIx = e->KeywordIx( "VERSION");
    bool version = e->KeywordSet( versionIx);

    // VERSION keyword
    if ( version) {
      DStructGDL* res = new DStructGDL( "WIDGET_VERSION");
      return res;
    }

    // CHILD keyword
    if ( child) {
      if ( rank == 0) {
	// Scalar Input
	WidgetIDT widgetID = (*p0L)[0];
	GDLWidget *widget = GDLWidget::GetWidget( widgetID);
	if ( widget == NULL) {
	  return new DLongGDL( 0);
	} else {
	  DLong nChildren = widget->NChildren();
	  if ( nChildren != 0)
	    return new DLongGDL( widget->GetChild( 0));
	  else
	    return new DLongGDL( 0);
	}
      } else {
	// Array Input
	DLongGDL* res = new DLongGDL(p0L->Dim(), BaseGDL::NOZERO);
	for( SizeT i=1; i<nEl; i++) {
	  WidgetIDT widgetID = (*p0L)[i];
	  GDLWidget *widget = GDLWidget::GetWidget( widgetID);
	  if ( widget == NULL) {
	    (*res)[ i] = (DLong) 0;
	  } else {
	    DLong nChildren = widget->NChildren();
	    if ( nChildren != 0)
	      (*res)[ i] = (DLong) widget->GetChild( 0);
	    else
	      (*res)[ i] = (DLong) 0;
	  }
	}
	return res;
      }
    }

    // VALID keyword
    if ( valid) {
      if ( rank == 0) {
	// Scalar Input
	WidgetIDT widgetID = (*p0L)[0];
	GDLWidget *widget = GDLWidget::GetWidget( widgetID);
	if ( widget == NULL)
	  return new DLongGDL( 0);
	else
	  return new DLongGDL( 1);
      } else {
	// Array Input
	DLongGDL* res = new DLongGDL(p0L->Dim(), BaseGDL::NOZERO);
	for( SizeT i=1; i<nEl; i++) {
	  WidgetIDT widgetID = (*p0L)[i];
	  GDLWidget *widget = GDLWidget::GetWidget( widgetID);
	  if ( widget == NULL)
	    (*res)[ i] = (DLong) 0;
	  else
	    (*res)[ i] = (DLong) 1;
	}
	return res;
      }
    }
    // End /VALID

    // MODAL keyword (stub)
    if ( modal) {
      return new DLongGDL( 0);
    }
    // End /MODAL

    // MANAGED keyword
    if ( managed) {
      if ( rank == 0) {
	// Scalar Input
	WidgetIDT widgetID = (*p0L)[0];
	GDLWidget *widget = GDLWidget::GetWidget( widgetID);

	// Check if valid widgetID else exit with 0
	if ( widget == NULL) return new DLongGDL( 0);

	if ( widget->GetManaged() == true)
	  return new DLongGDL( 1);
	else
	  return new DLongGDL( 0);
      } else {
	// Array Input
	DLongGDL* res = new DLongGDL(p0L->Dim(), BaseGDL::NOZERO);
	for( SizeT i=0; i<nEl; i++) {
	  WidgetIDT widgetID = (*p0L)[i];
	  GDLWidget *widget = GDLWidget::GetWidget( widgetID);

	  if ( widget == NULL)
	    (*res)[ i] = (DLong) 0;
	  else {
	    if ( widget->GetManaged() == true)
	      (*res)[ i] = (DLong) 1;
	    else
	      (*res)[ i] = (DLong) 0;
	  }
	}
	return res;
      }
    }
    // End /MANAGED


    // XMANAGER_BLOCK keyword
    if ( xmanagerBlock) {
      return new DLongGDL( GDLWidget::GetXmanagerBlock() ? 1 : 0);
    }
    // End /XMANAGER_BLOCK
    assert( false);
    return NULL;
#endif
  }


  // WIDGET_EVENT
BaseGDL* widget_event( EnvT* e)
{
#ifndef HAVE_LIBWXWIDGETS
    e->Throw("GDL was compiled without support for wxWidgets");
    return NULL;
#else
    SizeT nParam=e->NParam();

    DLongGDL* p0L = NULL;

    if ( nParam > 0) {
        p0L = e->GetParAs<DLongGDL>( 0);
    }

    static int xmanagerBlockIx = e->KeywordIx( "XMANAGER_BLOCK");
    bool xmanagerBlock = e->KeywordSet( xmanagerBlockIx);

    EnvBaseT* caller;

    DLong id;
    DLong tlb;
//     DLong handler;
//     DLong select;
    //    int i; cin >> i;
//     GDLEventQueuePolledGuard polledGuard( &GDLWidget::eventQueue);
    while ( 1)
    {   // outer while loop
        std::cout << "WIDGET_EVENT: Polling event queue ..." << std::endl;

        DStructGDL* ev = NULL;

        while ( 1)
        {
            // handle global GUI events as well as plot events
            // handling is completed on return
            // calls GDLWidget::HandleEvents()
            // which calls GDLWidget::CallEventHandler()
            GDLEventHandler();

            // the polling event handler
            if( (ev = GDLWidget::eventQueue.Pop()) != NULL)
            {
                // ev = GDLWidget::eventQueue.Pop();
                static int idIx = ev->Desc()->TagIndex("ID"); // 0
                static int topIx = ev->Desc()->TagIndex("TOP"); // 1
                static int handlerIx = ev->Desc()->TagIndex("HANDLER"); // 2

                id = (*static_cast<DLongGDL*>
                      (ev->GetTag(idIx, 0)))[0];
                tlb = (*static_cast<DLongGDL*>
                       (ev->GetTag(topIx, 0)))[0];
                break;
            }

            // if poll event handler found an event this is not reached due to the
            // 'break' statement
            // Sleep a bit to prevent CPU overuse
            wxMilliSleep( 50);

            if( sigControlC)
                return new DLongGDL( 0);
	    
	    if( GDLGUIThread::gdlGUIThread == NULL)
	    {
	      std::cout << "WIDGET_EVENT: GUI thread has exited." << std::endl;
	      return new DLongGDL( 0);
	    }
        } // inner while

        std::cout << "WIDGET_EVENT: Event found" << std::endl;
        std::cout << "top: " << tlb << std::endl;
        std::cout << "id:  " << id << "   thread:" << GDLGUIThread::gdlGUIThread << std::endl;

        ev = CallEventHandler( /*id,*/ ev);

        if( ev != NULL)
        {
            Warning( "WIDGET_EVENT: No event handler found. ID: " + i2s(id));
            GDLDelete( ev);
            ev = NULL;
        }

        GDLWidget *tlw = GDLWidget::GetWidget( tlb);
        if ( tlw == NULL)
        {
            std::cout << "WIDGET_EVENT: widget no longer valid." << std::endl;
            break;
        }

        // see comment in GDLWidget::HandleEvents()
        // WidgetIDT tlb = GDLWidget::GetTopLevelBase( id);
        assert( dynamic_cast<GDLFrame*>(tlw->GetWxWidget()) != NULL);
        // Pause 50 millisecs then refresh widget
//       wxMilliSleep( 50); // (why?)
        GUIMutexLockerWidgetsT gdlMutexGuiEnterLeave;
        static_cast<GDLFrame*>(tlw->GetWxWidget())->Refresh();
        gdlMutexGuiEnterLeave.Leave();

    } // outer while loop

    return new DLongGDL( 0);
#endif
}


  // WIDGET_CONTROL
  void widget_control( EnvT* e)
  {
#ifndef HAVE_LIBWXWIDGETS
    e->Throw("GDL was compiled without support for wxWidgets");
#else
    DLongGDL* p0L = e->GetParAs<DLongGDL>( 0);

    WidgetIDT widgetID = (*p0L)[0];
    GDLWidget *widget = GDLWidget::GetWidget( widgetID);
    if( widget == NULL)
      e->Throw("Widget ID not valid: "+i2s(widgetID));
    
    static int realizeIx = e->KeywordIx( "REALIZE");
    bool realize = e->KeywordSet( realizeIx);

    static int managedIx = e->KeywordIx( "MANAGED");
    bool managed = e->KeywordSet( managedIx);

    bool map = true;
    static int mapIx = e->KeywordIx( "MAP");
    if ( e->KeywordPresent( mapIx))
      if ( !e->KeywordSet( mapIx))
	map = false;

    static int xmanActComIx = e->KeywordIx( "XMANAGER_ACTIVE_COMMAND");
    bool xmanActCom = e->KeywordSet( xmanActComIx);

    static int destroyIx = e->KeywordIx( "DESTROY");
    bool destroy = e->KeywordSet( destroyIx);

    DString eventPro = "";
    static int eventproIx = e->KeywordIx( "EVENT_PRO");
    bool eventpro = e->KeywordSet( eventproIx);

    static int getuvalueIx = e->KeywordIx( "GET_UVALUE");
    bool getuvalue = e->KeywordPresent( getuvalueIx);

    static int setuvalueIx = e->KeywordIx( "SET_UVALUE");
    bool setuvalue = e->KeywordPresent( setuvalueIx);

    static int setvalueIx = e->KeywordIx( "SET_VALUE");
    bool setvalue = e->KeywordPresent( setvalueIx);

    static int getvalueIx = e->KeywordIx( "GET_VALUE");
    bool getvalue = e->KeywordPresent( getvalueIx);

    static int setunameIx = e->KeywordIx( "SET_UNAME");
    bool setuname = e->KeywordPresent( setunameIx);
    DString uname = "";
    e->AssureStringScalarKWIfPresent( setunameIx, uname);

    static int prosetvalueIx = e->KeywordIx( "PRO_SET_VALUE");
    bool prosetvalue = e->KeywordPresent( prosetvalueIx);
    DString setvaluepro = "";
    e->AssureStringScalarKWIfPresent( prosetvalueIx, setvaluepro);

    static int funcgetvalueIx = e->KeywordIx( "FUNC_GET_VALUE");
    bool funcgetvalue = e->KeywordPresent( funcgetvalueIx);
    DString setvaluefunc = "";
    e->AssureStringScalarKWIfPresent( funcgetvalueIx, setvaluefunc);

    static int setbuttonIx = e->KeywordIx( "SET_BUTTON");
    bool setbutton = e->KeywordPresent( setbuttonIx);

    if ( realize) {
      widget->Realize( map);
    }

    if ( managed) {
      widget->SetManaged( true);
    }

    if ( xmanActCom) {
//       cout << "Set xmanager active command: " << widgetID << endl;
      widget->SetXmanagerActiveCommand();
    }

    if ( destroy) {
//       std::cout << "Destroy widget" << std::endl;
      delete widget;      
    }

    if ( eventpro) {
      e->AssureStringScalarKWIfPresent( eventproIx, eventPro);
      widget->SetEventPro( eventPro);
    }


    if ( getuvalue) {
      BaseGDL** uvalueKW = &e->GetKW( getuvalueIx);
      GDLDelete((*uvalueKW));

      BaseGDL *widval = widget->GetUvalue();
      //      *uvalueKW = widget->GetUvalue();
      *uvalueKW = widval->Dup();

      /*
      if ( *uvalueKW != NULL) {
	if( (*uvalueKW)->Type() == GDL_STRING)
	  *uvalueKW = new DStringGDL( (*( DStringGDL*) (*uvalueKW))[0]);
	if( (*uvalueKW)->Type() == GDL_LONG)
	  *uvalueKW = new DLongGDL( (*( DLongGDL*) (*uvalueKW))[0]);
	if( (*uvalueKW)->Type() == GDL_STRUCT) {
	  DStructGDL* s = static_cast<DStructGDL*>( *uvalueKW);
	  //	  DStructGDL* parStruct = dynamic_cast<DStructGDL*>( *uvalueKW);
	  cout << s->Desc()->Name() << endl;
	}
      }
      */

    }

    if ( setuvalue) {
      BaseGDL* uvalue = e->GetKW( setuvalueIx);
      if( uvalue->Type() == GDL_STRUCT) {
	//	cout << "Structure uvalue" << endl;
	//DStructGDL* s1 = static_cast<DStructGDL*>( uvalue);
	//cout << s1->Desc()->Name() << endl;
      }
      if( uvalue != NULL) uvalue = uvalue->Dup();
      widget->SetUvalue( uvalue);
    }

    if ( setuname) {
      widget->SetUname( uname);
    }

    if ( prosetvalue) {
      widget->SetProValue( setvaluepro);
    }

    if ( funcgetvalue) {
      widget->SetFuncValue( setvaluefunc);
    }

    if ( setbutton) {
      if( !widget->IsButton())
      {
	e->Throw( "Only WIDGET_BUTTON are allowed with keyword SET_BUTTON.");
      }
      GDLWidgetButton* button = static_cast<GDLWidgetButton*>(widget);
      DLong buttonVal;
      e->AssureLongScalarKWIfPresent( setbuttonIx, buttonVal);
      if ( buttonVal == 0)
	button->SetButtonWidget(false);
      else
	button->SetButtonWidget(true);
    }

    if ( setvalue) {
      DString wType = widget->GetWidgetType();

      if ( wType == "") {
	BaseGDL* value = e->GetKW( setvalueIx);
	if( value != NULL) value = value->Dup();

	DString setProName = widget->GetProValue();
	if ( setProName != "") {

	  CallEventPro( setProName, p0L->Dup(), value); // grabs

// 	  // Build call to SETV procedure
// 	  ostringstream ostr;
// 	  ostr << setProName << ", " << widgetID << ", [";
// 
// 	  DLongGDL* values = e->IfDefGetKWAs<DLongGDL>( setvalueIx);
// 	  DLong nEl = values->N_Elements();
// 	  for( SizeT i=0; i<nEl; i++) {
// 	    ostr << (*values)[i];
// 	    if ( i != (nEl-1)) ostr << ", ";
// 	  }
// 	  ostr << "]";
// 
// 	  DString line = ostr.rdbuf()->str();
// 	  istringstream istr(line+"\n");
// 
// 	  // Call SETV procedure
// // ms: commented out to comply with new stack handling
// 	  EnvBaseT* caller = e->Caller();
// // 	  e->Interpreter()->CallStack().pop_back();
// 	  executeString2( caller, &istr);
	}


	widget->SetVvalue( value);
      }

      if ( wType == "TEXT") {
	static int noNewLineIx = e->KeywordIx( "NO_NEWLINE");
	bool noNewLine = e->KeywordSet(noNewLineIx);

	BaseGDL* valueKW = e->GetKW( setvalueIx);
	DStringGDL* valueStr = NULL;
	if( valueKW != NULL)
	{
	  if( valueKW->Type() != GDL_STRING)
	    e->Throw("VALUE must be a STRING for WIDGET_TEXT.");
	  valueStr = static_cast<DStringGDL*>(valueKW);
	  bool success = e->StealLocalKW(setvalueIx);
	  if( !success)
	    valueStr = valueStr->Dup();

	  GDLWidgetText *textWidget = ( GDLWidgetText *) widget;
	  textWidget->SetTextValue( valueStr, noNewLine);
	}
      }

      if ( wType == "LABEL") {
	DString value = "";
	e->AssureStringScalarKWIfPresent( setvalueIx, value);
	//	std::cout << "setlabelvalue: " << value.c_str() << std::endl;
	GDLWidgetLabel *labelWidget = ( GDLWidgetLabel *) widget;
	labelWidget->SetLabelValue( value);
      }
    }

    if ( getvalue) {
      
      BaseGDL** valueKW = &e->GetKW( getvalueIx);
      GDLDelete((*valueKW));

      DString getFuncName = widget->GetFuncValue();
      if ( getFuncName != "") {
	StackGuard<EnvStackT> guard( e->Interpreter()->CallStack());

	DString callF = StrUpCase( getFuncName);

	SizeT funIx = GDLInterpreter::GetFunIx( callF);
	EnvUDT* newEnv= new EnvUDT( e->CallingNode(), funList[ funIx], (DObjGDL**)NULL);

	// add parameter
	newEnv->SetNextPar( new DLongGDL(widgetID)); // pass as local
	e->Interpreter()->CallStack().push_back( newEnv);

	// make the call
	BaseGDL* res = e->Interpreter()->
	  call_fun(static_cast<DSubUD*>(newEnv->GetPro())->GetTree());

	// set the keyword to the function's return value
	*valueKW = new DIntGDL( (*( DIntGDL*) (res))[0]);
      } else {
	// "Regular" getvalue
// 	BaseGDL** valueKW = &e->GetKW( getvalueIx);
// 	GDLDelete((*valueKW));
	DString wType = widget->GetWidgetType();
	if( widget->IsText() || widget->IsDropList())
	{
	  string rawValue;
	  if( widget->IsText())
	    rawValue = static_cast<GDLWidgetText*>(widget)->GetLastValue();
	  else
	    rawValue = static_cast<GDLWidgetDropList*>(widget)->GetLastValue();
	  if( rawValue.length() == 0)
	  {
	    *valueKW = new DStringGDL(dimension(1));
	  }
	  else
	  {
	    vector<DString> strArr;
	    strArr.reserve(rawValue.length());
	    string actStr = "";
	    for( int i=0; i<rawValue.length(); ++i)
	    {
		if( rawValue[i] != '\n')
		  actStr += rawValue[i];
		else
		{
		  strArr.push_back( actStr);
		  actStr.clear();
		}
	    }
	    DStringGDL* valueStr = new DStringGDL( dimension(strArr.size()));
	    for( int i=0; i<strArr.size(); ++i)
	    {
	      (*valueStr)[i] = strArr[i];
	    }
	    *valueKW = valueStr;
	  }
	}	  
	else
	{
	  *valueKW = widget->GetVvalue();
	  if ( *valueKW != NULL) 
	  {
	    if( (*valueKW)->Type() == GDL_STRING)
	      *valueKW = new DStringGDL( (*( DStringGDL*) (*valueKW))[0]);
	    if( (*valueKW)->Type() == GDL_LONG)
	      *valueKW = new DLongGDL( (*( DLongGDL*) (*valueKW))[0]);
	  } 
	  else 
	  {
	    *valueKW = new DLongGDL( 0);
	  }
	}
      }
    }
#endif
  }

} // namespace library
