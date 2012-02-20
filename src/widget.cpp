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

#ifdef HAVE_LIBWXWIDGETS
#  include "gdlwidget.hpp"
#endif

namespace lib {
  using namespace std;

  void executeString2( EnvBaseT* caller, istringstream *istr)
  {
    RefDNode theAST;

    GDLLexer lexer(*istr, "", GDLParser::NONE);
    GDLParser& parser = lexer.Parser();
    parser.interactive();
    theAST = parser.getAST();
    RefDNode trAST;
    GDLTreeParser treeParser( caller);
    treeParser.interactive(theAST);
    trAST = treeParser.getAST();
    ProgNodeP progAST = ProgNode::NewProgNode( trAST);
    auto_ptr< ProgNode> progAST_guard( progAST);

    // necessary for correct FOR loop handling
    assert( dynamic_cast<EnvUDT*>(caller) != NULL);
    EnvUDT* env = static_cast<EnvUDT*>(caller);
    int nForLoopsIn = env->NForLoops();
    int nForLoops = ProgNode::NumberForLoops( progAST, nForLoopsIn);
    env->ResizeForLoops( nForLoops);
    env->ResizeForLoops( nForLoopsIn);

    RetCode retCode = caller->Interpreter()->execute( progAST);
  }


  BaseGDL* widget_base( EnvT* e)
  {
#ifndef HAVE_LIBWXWIDGETS
    e->Throw("GDL was compiled without support for wxWidgets");
#else
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

    bool mapWid = true;
    if ( e->KeywordPresent( mapIx))
      if ( !e->KeywordSet( mapIx))
	mapWid = false;
    //    std::cout << "Map in widget_base: " << mapWid << std::endl;

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
#endif
  }


  // WIDGET_BUTTON
  BaseGDL* widget_button( EnvT* e)
  {
#ifndef HAVE_LIBWXWIDGETS
    e->Throw("GDL was compiled without support for wxWidgets");
#else
    DLongGDL* p0L = e->GetParAs<DLongGDL>( 0);
    WidgetIDT parentID = (*p0L)[0];
    GDLWidget *widget = GDLWidget::GetWidget( parentID);

    static int valueIx = e->KeywordIx( "VALUE");
    DString value = "";
    e->AssureStringScalarKWIfPresent( valueIx, value);

    static int uvalueIx = e->KeywordIx( "UVALUE");
    BaseGDL* uvalue = e->GetKW( uvalueIx);
    if( uvalue != NULL)
      uvalue = uvalue->Dup();

    GDLWidgetButton* button = new GDLWidgetButton( parentID, uvalue, value);

    button->SetWidgetType( "BUTTON");

    button->SetButtonOff();

    return new DLongGDL( button->WidgetID());
#endif
  }


  // WIDGET_DROPLIST
  BaseGDL* widget_droplist( EnvT* e)
  {
#ifndef HAVE_LIBWXWIDGETS
    e->Throw("GDL was compiled without support for wxWidgets");
#else
    DLongGDL* p0L = e->GetParAs<DLongGDL>( 0);
    WidgetIDT parentID = (*p0L)[0];
    GDLWidget *widget = GDLWidget::GetWidget( parentID);

    DLong xsize = -1;
    static int xsizeIx = e->KeywordIx( "XSIZE");
    e->AssureLongScalarKWIfPresent( xsizeIx, xsize);

    static int titleIx = e->KeywordIx( "TITLE");
    DString title = "";
    e->AssureStringScalarKWIfPresent( titleIx, title);

    static int valueIx = e->KeywordIx( "VALUE");
    //    DStringGDL* value = e->IfDefGetKWAs<DStringGDL>( valueIx);
    BaseGDL* value = e->GetKW( valueIx);
    if( value != NULL)
      value = value->Dup();

    static int uvalueIx = e->KeywordIx( "UVALUE");
    BaseGDL* uvalue = e->GetKW( uvalueIx);
    if( uvalue != NULL)
      uvalue = uvalue->Dup();

    GDLWidgetLabel* label = 
      new GDLWidgetLabel( parentID, uvalue, title, xsize);

    DLong style = wxCB_READONLY;
    GDLWidgetDropList* droplist = new GDLWidgetDropList( parentID, uvalue, value,
							 title, xsize, style);
    droplist->SetWidgetType( "DROPLIST");

    return new DLongGDL( droplist->WidgetID());
#endif
  }


  // WIDGET_TEXT
  BaseGDL* widget_text( EnvT* e)
  {
#ifndef HAVE_LIBWXWIDGETS
    e->Throw("GDL was compiled without support for wxWidgets");
#else
    DLongGDL* p0L = e->GetParAs<DLongGDL>( 0);
    WidgetIDT parentID = (*p0L)[0];
    GDLWidget *widget = GDLWidget::GetWidget( parentID);

    DLong xsize = -1;
    static int xsizeIx = e->KeywordIx( "XSIZE");
    e->AssureLongScalarKWIfPresent( xsizeIx, xsize);

    static int valueIx = e->KeywordIx( "VALUE");
    DString value = "";
    e->AssureStringScalarKWIfPresent( valueIx, value);

    static int uvalueIx = e->KeywordIx( "UVALUE");
    BaseGDL* uvalue = e->GetKW( uvalueIx);
    if( uvalue != NULL)
      uvalue = uvalue->Dup();

    GDLWidgetText* text = new GDLWidgetText( parentID, uvalue, value, xsize);
 
    text->SetWidgetType( "TEXT");

    return new DLongGDL( text->WidgetID());
#endif
  }


  // WIDGET_LABEL
  BaseGDL* widget_label( EnvT* e)
  {
#ifndef HAVE_LIBWXWIDGETS
    e->Throw("GDL was compiled without support for wxWidgets");
#else
    DLongGDL* p0L = e->GetParAs<DLongGDL>( 0);
    WidgetIDT parentID = (*p0L)[0];
    GDLWidget *widget = GDLWidget::GetWidget( parentID);

    DLong xsize = -1;
    static int xsizeIx = e->KeywordIx( "XSIZE");
    e->AssureLongScalarKWIfPresent( xsizeIx, xsize);

    static int valueIx = e->KeywordIx( "VALUE");
    DString value = "";
    e->AssureStringScalarKWIfPresent( valueIx, value);

    static int uvalueIx = e->KeywordIx( "UVALUE");
    BaseGDL* uvalue = e->GetKW( uvalueIx);
    if( uvalue != NULL)
      uvalue = uvalue->Dup();

    GDLWidgetLabel* label = 
      new GDLWidgetLabel( parentID, uvalue, value, xsize);

    label->SetWidgetType( "LABEL");

    return new DLongGDL( label->WidgetID());
#endif
  }


  // WIDGET_INFO
  BaseGDL* widget_info( EnvT* e)
  {
#ifndef HAVE_LIBWXWIDGETS
    e->Throw("GDL was compiled without support for wxWidgets");
#else
    SizeT nParam=e->NParam();

    DLongGDL* p0L;
    SizeT nEl;
    SizeT rank;

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
	  DLong nChildren = widget->GetChild( -1);
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
	    DLong nChildren = widget->GetChild( -1);
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
      return new DLongGDL( GDLWidget::GetXmanagerBlock());
    }
    // End /XMANAGER_BLOCK
#endif
  }


  // WIDGET_EVENT
  BaseGDL* widget_event( EnvT* e)
  {
#ifndef HAVE_LIBWXWIDGETS
    e->Throw("GDL was compiled without support for wxWidgets");
#else
    SizeT nParam=e->NParam();

    DLongGDL* p0L;

    if ( nParam > 0) {
      p0L = e->GetParAs<DLongGDL>( 0);
    }

    static int xmanagerBlockIx = e->KeywordIx( "XMANAGER_BLOCK");
    bool xmanagerBlock = e->KeywordSet( xmanagerBlockIx);

    EnvBaseT* caller;

    static DLong lasttop = -1;

    DLong id;
    DLong top;
    DLong handler;
    DLong select;
    //    int i; cin >> i;

    while ( 1) { // outer while loop
      std::cout << "In PollEvents loop (widget_event)" << std::endl;
      while ( 1) {
	// Sleep a bit to prevent CPU overuse
	wxMilliSleep( 50);
	if ( GDLWidget::PollEvents( &id, &top, &handler, &select))
	  break;
      }

      std::cout << "break from PollEvents (widget_event)" << std::endl;
      std::cout << "top: " << top << std::endl;
      std::cout << "id:  " << id << std::endl;

      // Get Event Pro
      GDLWidget *widget = GDLWidget::GetWidget( top);
      DString eventHandler = widget->GetEventPro();

      // Build event handler command
      ostringstream ostr;
      ostr << "EV={WIDGET_BUTTON, ";
      ostr << "ID: " << id << "L, TOP: " << top << "L, ";
      ostr << "HANDLER: " << handler << "L, SELECT: " << select << "L } ";
      ostr << "& " << eventHandler.c_str() << ", EV";

      DString line = ostr.rdbuf()->str();
      istringstream istr(line+"\n");


      // Call widget event handler routine
      if ( lasttop != top) {
	caller = e->Caller();
// ms: commented out to comply with new stack handling
	// 	e->Interpreter()->CallStack().pop_back();
	//	std::cout << "before delete e" << std::endl;
// 	delete e;
      }

      executeString2( caller, &istr);

      /*
      RefDNode theAST;

      GDLLexer lexer(istr, "", GDLParser::NONE);
      GDLParser& parser = lexer.Parser();
      parser.interactive();

      theAST = parser.getAST();
      RefDNode trAST;
      GDLTreeParser treeParser( caller);
      treeParser.interactive(theAST);
      trAST = treeParser.getAST();

      ProgNodeP progAST = ProgNode::NewProgNode( trAST);
      auto_ptr< ProgNode> progAST_guard( progAST);

      // necessary for correct FOR loop handling
      assert( dynamic_cast<EnvUDT*>(caller) != NULL);
      EnvUDT* env = static_cast<EnvUDT*>(caller);
      int nForLoopsIn = env->NForLoops();
      int nForLoops = ProgNode::NumberForLoops( progAST, nForLoopsIn);
      env->ResizeForLoops( nForLoops);

      RetCode retCode =
	caller->Interpreter()->execute( progAST);

	env->ResizeForLoops( nForLoopsIn);
      */

      // Return from GDL/IDL event handler procedure
      std::cout << "return from event handler (widget_event)" << 
	std::endl << std::endl;

      if ( GDLWidget::GetWidget( top) == NULL) {
	std::cout << "widget NULLed" << std::endl;
	break;
      }

      GDLWidgetButton *buttonWidget = 
	( GDLWidgetButton *) GDLWidget::GetWidget( id);
      buttonWidget->SetSelectOff();
      buttonWidget->SetManaged( false);

      lasttop = top;
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
      cout << "Set xmanager active command: " << widgetID << endl;
      widget->SetXmanagerActiveCommand();
    }

    if ( destroy) {
      std::cout << "Destroy widget" << std::endl;
      delete widget;
    }

    if ( eventpro) {
      e->AssureStringScalarKWIfPresent( eventproIx, eventPro);
      widget->SetEventPro( eventPro);
    }


    if ( getuvalue) {
      BaseGDL** uvalueKW = &e->GetKW( getuvalueIx);
      delete (*uvalueKW);

      BaseGDL *widval = widget->GetUvalue();
      //      *uvalueKW = widget->GetUvalue();
      *uvalueKW = widval->Dup();

      /*
      if ( *uvalueKW != NULL) {
	if( (*uvalueKW)->Type() == STRING)
	  *uvalueKW = new DStringGDL( (*( DStringGDL*) (*uvalueKW))[0]);
	if( (*uvalueKW)->Type() == LONG)
	  *uvalueKW = new DLongGDL( (*( DLongGDL*) (*uvalueKW))[0]);
	if( (*uvalueKW)->Type() == STRUCT) {
	  DStructGDL* s = static_cast<DStructGDL*>( *uvalueKW);
	  //	  DStructGDL* parStruct = dynamic_cast<DStructGDL*>( *uvalueKW);
	  cout << s->Desc()->Name() << endl;
	}
      }
      */

    }

    if ( setuvalue) {
      BaseGDL* uvalue = e->GetKW( setuvalueIx);
      if( uvalue->Type() == STRUCT) {
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
      DLong buttonVal;
      e->AssureLongScalarKWIfPresent( setbuttonIx, buttonVal);
      if ( buttonVal == 0)
	widget->SetButtonOff();
      else
	widget->SetButtonOn();
    }

    if ( setvalue) {
      DString wType = widget->GetWidgetType();

      if ( wType == "") {
	BaseGDL* value = e->GetKW( setvalueIx);
	if( value != NULL) value = value->Dup();

	DString setProName = widget->GetProValue();
	if ( setProName != "") {

	  // Build call to SETV procedure
	  ostringstream ostr;
	  ostr << setProName.c_str() << ", " << widgetID << ", [";

	  DLongGDL* values = e->IfDefGetKWAs<DLongGDL>( setvalueIx);
	  DLong nEl = values->N_Elements();
	  for( SizeT i=0; i<nEl; i++) {
	    ostr << (*values)[i];
	    if ( i != (nEl-1)) ostr << ", ";
	  }
	  ostr << "]";

	  DString line = ostr.rdbuf()->str();
	  istringstream istr(line+"\n");

	  // Call SETV procedure
// ms: commented out to comply with new stack handling
	  EnvBaseT* caller = e->Caller();
// 	  e->Interpreter()->CallStack().pop_back();
	  executeString2( caller, &istr);
	}


	widget->SetVvalue( value);
      }

      if ( wType == "TEXT") {
	DString value = "";
	e->AssureStringScalarKWIfPresent( setvalueIx, value);
	//	std::cout << "settextvalue: " << value.c_str() << std::endl;
	GDLWidgetText *textWidget = ( GDLWidgetText *) widget;
	textWidget->SetTextValue( value);
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
      delete (*valueKW);

      DString getFuncName = widget->GetFuncValue();
      if ( getFuncName != "") {
	StackGuard<EnvStackT> guard( e->Interpreter()->CallStack());

	DString callF;
	// this is a function name -> convert to UPPERCASE
	callF = getFuncName.c_str();
	callF = StrUpCase( callF);

	SizeT funIx = GDLInterpreter::GetFunIx( callF);
	EnvUDT* newEnv= new EnvUDT( e, funList[ funIx], NULL);

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
	BaseGDL** valueKW = &e->GetKW( getvalueIx);
	delete (*valueKW);

	*valueKW = widget->GetVvalue();
	if ( *valueKW != NULL) {
	  if( (*valueKW)->Type() == STRING)
	    *valueKW = new DStringGDL( (*( DStringGDL*) (*valueKW))[0]);
	  if( (*valueKW)->Type() == LONG)
	    *valueKW = new DLongGDL( (*( DLongGDL*) (*valueKW))[0]);
	} else {
	  DLongGDL* res = new DLongGDL( 0);
	  *valueKW = res;
	}
      }
    }
#endif
  }

} // namespace

