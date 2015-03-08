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
#include "gdlwidget.hpp"
#include "basic_pro_jmg.hpp"
#endif

#ifdef HAVE_LIBWXWIDGETS
void GDLWidget::GetCommonKeywords( EnvT* e)
{
  static int frameIx = e->KeywordIx( "FRAME" );
  static int event_funcIx = e->KeywordIx( "EVENT_FUNC" );
  static int event_proIx = e->KeywordIx( "EVENT_PRO" );
  static int func_get_valueIx = e->KeywordIx( "FUNC_GET_VALUE" );
  static int pro_set_valueIx = e->KeywordIx( "PRO_SET_VALUE" );
  static int notify_realizeIx = e->KeywordIx( "NOTIFY_REALIZE" );
  static int kill_notifyIx = e->KeywordIx( "KILL_NOTIFY" );
  static int group_leaderIx = e->KeywordIx( "GROUP_LEADER" );
  static int no_copyIx = e->KeywordIx( "NO_COPY" );
  static int scr_xsizeIx = e->KeywordIx( "SCR_XSIZE" );
  static int scr_ysizeIx = e->KeywordIx( "SCR_YSIZE" );
  static int scrollIx = e->KeywordIx( "SCROLL" );
  static int sensitiveIx = e->KeywordIx( "SENSITIVE" );
  static int unameIx = e->KeywordIx( "UNAME" );
  static int unitsIx = e->KeywordIx( "UNITS" );
  static int uvalueIx = e->KeywordIx( "UVALUE" );
  static int xoffsetIx = e->KeywordIx( "XOFFSET" );
  static int xsizeIx = e->KeywordIx( "XSIZE" );
  static int yoffsetIx = e->KeywordIx( "YOFFSET" );
  static int ysizeIx = e->KeywordIx( "YSIZE" );
  static int ALIGN_CENTER = e->KeywordIx( "ALIGN_CENTER" );
  static int ALIGN_LEFT = e->KeywordIx( "ALIGN_LEFT" );
  static int ALIGN_RIGHT = e->KeywordIx( "ALIGN_RIGHT" );
  static int FONT = e->KeywordIx( "FONT" );

  font="";
    e->AssureStringScalarKWIfPresent( FONT, font );
    
  alignment=-1;
  if (e->KeywordSet(ALIGN_LEFT)) alignment=wxALIGN_LEFT;
  if (e->KeywordSet(ALIGN_CENTER)) alignment=wxALIGN_CENTER;
  if (e->KeywordSet(ALIGN_RIGHT)) alignment=wxALIGN_RIGHT;
  
  scrolled = e->KeywordSet( scrollIx );

  sensitive=TRUE;
  if (e->KeywordPresent( sensitiveIx )) sensitive=e->KeywordSet( sensitiveIx);

  groupLeader = 0;
  if (e->KeywordPresent( group_leaderIx )){
    e->AssureLongScalarKWIfPresent( group_leaderIx, groupLeader );
    GDLWidget *widget = GDLWidget::GetWidget( groupLeader );
    if ( widget == NULL ) {
        e->Throw( "Invalid widget identifier:" + i2s( groupLeader ) );
    }
  }

  frame = 0;
  e->AssureLongScalarKWIfPresent( frameIx, frame );
  int the_units = 0;
  e->AssureLongScalarKWIfPresent( unitsIx, the_units );
  //convert unit to the factor in pixels
  DDouble sx=wxGetDisplaySizeMM().x;
  DDouble sy=wxGetDisplaySizeMM().y;
  sx/=wxGetDisplaySize().x;
  sx/=25.4;
  sy/=wxGetDisplaySize().y;
  sy/=25.4;
  if (the_units==0) units=wxSize(1,1);
  int ix=sx;
  int iy=sy;
  if (the_units==1) units=wxSize(ix,iy);
  ix=sx/2.54;
  iy=sy/2.54;
  if (the_units==2) units=wxSize(ix,iy);

  //SCREEN_XSIZE and YSIZE are equivalent to xSize, ySize for most widgets.
//the following trick permits to transfer these values to xSize etc.
  scrXSize = -1;
  xSize = -1;
  e->AssureLongScalarKWIfPresent( scr_xsizeIx, xSize );
  e->AssureLongScalarKWIfPresent( scr_xsizeIx, scrXSize );
  e->AssureLongScalarKWIfPresent( xsizeIx, xSize );
  scrYSize = -1;
  ySize = -1;
  e->AssureLongScalarKWIfPresent( scr_ysizeIx, ySize );
  e->AssureLongScalarKWIfPresent( scr_ysizeIx, scrYSize );
  e->AssureLongScalarKWIfPresent( ysizeIx, ySize );
  xOffset = 0;
  e->AssureLongScalarKWIfPresent( xoffsetIx, xOffset );
  yOffset = 0;
  e->AssureLongScalarKWIfPresent( yoffsetIx, yOffset );

  uValue = e->GetKW( uvalueIx );
  if ( uValue != NULL ) {
    bool no_copy = e->KeywordSet( no_copyIx );
    if ( no_copy )
      e->GetKW( uvalueIx ) = NULL;
    else
      uValue = uValue->Dup( );
  }
  eventFun = "";
  e->AssureStringScalarKWIfPresent( event_funcIx, eventFun );
  StrUpCaseInplace( eventFun );
  eventPro = "";
  e->AssureStringScalarKWIfPresent( event_proIx, eventPro );
  StrUpCaseInplace( eventPro );
  killNotify = "";
  e->AssureStringScalarKWIfPresent( kill_notifyIx, killNotify );
  StrUpCaseInplace( killNotify );
  notifyRealize = "";
  e->AssureStringScalarKWIfPresent( notify_realizeIx, notifyRealize );
  StrUpCaseInplace( notifyRealize );
  proValue = "";
  e->AssureStringScalarKWIfPresent( pro_set_valueIx, proValue );
  StrUpCaseInplace( proValue );
  funcValue = "";
  e->AssureStringScalarKWIfPresent( func_get_valueIx, funcValue );
  StrUpCaseInplace( funcValue );
  uName = "";
  e->AssureStringScalarKWIfPresent( unameIx, uName );
  // no case change
}
#endif    


// non library functions
// these reside here because gdlwidget.hpp is only included if wxWidgets are used
// and hence putting them there would cause a compiler error without wxWidgets
BaseGDL* CallEventFunc( const std::string& f, BaseGDL* ev)
{
  StackGuard<EnvStackT> guard( BaseGDL::interpreter->CallStack( ) );

  int funIx = GDLInterpreter::GetFunIx( f );

  ProgNodeP callingNode = NULL; //BaseGDL::interpreter->GetRetTree();

  EnvUDT* newEnv = new EnvUDT( callingNode, funList[ funIx], NULL );
  newEnv->SetNextPar( ev ); // pass as local

  BaseGDL::interpreter->CallStack( ).push_back( newEnv );

  // make the call
  newEnv->SetCallContext( EnvUDT::RFUNCTION );
  BaseGDL* res = BaseGDL::interpreter->call_fun( static_cast<DSubUD*> (newEnv->GetPro( ))->GetTree( ) );
  return res;
}

void CallEventPro( const std::string& p, BaseGDL* p0, BaseGDL* p1 ) {
  StackGuard<EnvStackT> guard( BaseGDL::interpreter->CallStack( ) );

  int proIx = GDLInterpreter::GetProIx( p );

  ProgNodeP callingNode = NULL; //BaseGDL::interpreter->GetRetTree();

  EnvUDT* newEnv = new EnvUDT( callingNode, proList[ proIx], NULL );
  newEnv->SetNextPar( p0 ); // pass as local
  if ( p1 != NULL )
    newEnv->SetNextPar( p1 ); // pass as local

  BaseGDL::interpreter->CallStack( ).push_back( newEnv );

  // make the call
  BaseGDL::interpreter->call_pro( static_cast<DSubUD*> (newEnv->GetPro( ))->GetTree( ) );
}

DStructGDL* CallEventHandler( /*DLong id,*/ DStructGDL* ev ) {
#ifdef HAVE_LIBWXWIDGETS
  static int idIx = ev->Desc( )->TagIndex( "ID" ); // 0
  static int topIx = ev->Desc( )->TagIndex( "TOP" ); // 1
  static int handlerIx = ev->Desc( )->TagIndex( "HANDLER" ); // 2

  DLong actID = (*static_cast<DLongGDL*> (ev->GetTag( idIx, 0 )))[0];

  // note that such a struct name is illegal in GDL
  // therefore it cannot be used in user code.
  // This is safer than choosing a legal name
  // as it could collide with user code
  if ( ev->Desc( )->Name( ) == "*WIDGET_DESTROY*" ) {
    GDLWidget* widget = GDLWidget::GetWidget( actID );
    if ( widget == NULL ) {
      Warning( "CallEventHandler: *WIDGET_DESTROY*: Internal error: Destroy request for already destroyed widget. ID: " + i2s( actID ) );
      GDLDelete( ev );
      return NULL;
    }

    static int messageIx = ev->Desc( )->TagIndex( "MESSAGE" );
    DLong message = (*static_cast<DLongGDL*> (ev->GetTag( messageIx, 0 )))[0];

    GDLDelete( ev );

    assert( message == 0 ); // only '0' -> Destroy for now

    assert( widget->IsBase( ) );

    //     std::cout << "CallEventHandler: *WIDGET_DESTROY*: Deleting widget: "+i2s(actID) << std::endl;

    delete widget; // removes itself from widgetList

    return NULL;
  }

  do {
    GDLWidget *widget = GDLWidget::GetWidget( actID );
    if ( widget == NULL ) {
      Warning( "CallEventHandler: Widget no longer valid. ID: " + i2s( actID ) );
      actID = GDLWidget::NullID;
    } else {
      DString eventHandlerPro = widget->GetEventPro( );
      if ( eventHandlerPro != "" ) {
        (*static_cast<DLongGDL*> (ev->GetTag( handlerIx, 0 )))[0] = actID;
        CallEventPro( eventHandlerPro, ev ); // grabs ev
        ev = NULL;
        break; // out of while
      }
      DString eventHandlerFun = widget->GetEventFun( );
      if ( eventHandlerFun != "" ) {
        (*static_cast<DLongGDL*> (ev->GetTag( handlerIx, 0 )))[0] = actID;
        BaseGDL* retVal = CallEventFunc( eventHandlerFun, ev ); // grabs ev
        if ( retVal->Type( ) == GDL_STRUCT ) {
          // ev is already deleted
          ev = static_cast<DStructGDL*> (retVal);
          if ( ev->Desc( )->TagIndex( "ID" ) != idIx ||
          ev->Desc( )->TagIndex( "TOP" ) != topIx ||
          ev->Desc( )->TagIndex( "HANDLER" ) != handlerIx ) {
            GDLDelete( ev );
            throw GDLException( eventHandlerFun + ": Event handler return struct must contain ID, TOP, HANDLER as first tags." );
          }
          // no break!
        } else {
          GDLDelete( retVal );
          ev = NULL;
          break; // out of while   
        }
      }
      actID = widget->GetParentID( );
    }
  }  while ( actID != GDLWidget::NullID );
#endif
  return ev;
}

template< typename T>
T* GetKeywordAs( EnvT* e, int ix)
{
  BaseGDL* kwBaseGDL = e->GetKW( ix );
  if ( kwBaseGDL == NULL )
    return NULL;

  if ( kwBaseGDL->Type( ) != T::t )
    return static_cast<T*> (kwBaseGDL->Convert2( T::t, BaseGDL::COPY ));

  bool stolen = e->StealLocalKW( ix );
  if ( stolen )
    return static_cast<T*> (kwBaseGDL);

  return static_cast<T*> (kwBaseGDL->Dup( ));
}


namespace lib {
using namespace std;

BaseGDL* widget_table( EnvT* e)
{
#ifndef HAVE_LIBWXWIDGETS
  e->Throw( "GDL was compiled without support for wxWidgets" );
  return NULL; // avoid warning
#else
  DLongGDL* p0L = e->GetParAs<DLongGDL>(0);
  WidgetIDT parentID = (*p0L)[0];
  GDLWidget* parent = GDLWidget::GetWidget( parentID );
  if ( parent == NULL )  e->Throw( "Invalid widget identifier: " + i2s( parentID ) );
  if (!parent->IsBase()) e->Throw( "Parent is of incorrect type." );

  static int ALIGNMENT = e->KeywordIx( "ALIGNMENT" );
  static int BACKGROUND_COLOR = e->KeywordIx( "BACKGROUND_COLOR" );
  static int COLUMN_LABELS = e->KeywordIx( "COLUMN_LABELS" );
  static int COLUMN_MAJOR = e->KeywordIx( "COLUMN_MAJOR" );
  static int ROW_MAJOR = e->KeywordIx( "ROW_MAJOR" );
  static int COLUMN_WIDTHS = e->KeywordIx( "COLUMN_WIDTHS" );
  static int DISJOINT_SELECTION = e->KeywordIx( "DISJOINT_SELECTION" );
  static int EDITABLE = e->KeywordIx( "EDITABLE" );
  static int FOREGROUND_COLOR = e->KeywordIx( "FOREGROUND_COLOR" );
  static int FORMAT = e->KeywordIx( "FORMAT" );
//  static int IGNORE_ACCELERATORS = e->KeywordIx( "IGNORE_ACCELERATORS" );
  static int NO_COLUMN_HEADERS = e->KeywordIx( "NO_COLUMN_HEADERS" );
  static int NO_HEADERS = e->KeywordIx( "NO_HEADERS" );
  static int NO_ROW_HEADERS = e->KeywordIx( "NO_ROW_HEADERS" );
  static int RESIZEABLE_COLUMNS = e->KeywordIx( "RESIZEABLE_COLUMNS" );
  static int RESIZEABLE_ROWS = e->KeywordIx( "RESIZEABLE_ROWS" );
  static int ROW_HEIGHTS = e->KeywordIx( "ROW_HEIGHTS" );
  static int ROW_LABELS = e->KeywordIx( "ROW_LABELS" );
//  static int TAB_MODE = e->KeywordIx( "TAB_MODE" );
  static int VALUE = e->KeywordIx( "VALUE" );
  
  bool disjointSelection = e->KeywordSet( DISJOINT_SELECTION );
//  bool ignoreAccelerators = e->KeywordSet( IGNORE_ACCELERATORS );
  bool noHeaders = e->KeywordSet( NO_HEADERS );
  bool noColumnHeaders = e->KeywordSet( NO_COLUMN_HEADERS ) || noHeaders;
  bool noRowHeaders = e->KeywordSet( NO_ROW_HEADERS ) || noHeaders;
  bool resizeableColumns = e->KeywordSet( RESIZEABLE_COLUMNS );
  bool resizeableRows = e->KeywordSet( RESIZEABLE_ROWS );

  DByteGDL* editable = GetKeywordAs<DByteGDL>(e, EDITABLE );
  DByteGDL* alignment = GetKeywordAs<DByteGDL>(e, ALIGNMENT);
  //not checked by idl
  //if (alignment) {if (!(alignment->Rank()==0 ||alignment->Rank()==2)) e->Throw("Table grid color attribute has the wrong dimensions.");}
 

  DByteGDL* backgroundColor = GetKeywordAs<DByteGDL>(e, BACKGROUND_COLOR);
  DByteGDL* foregroundColor = GetKeywordAs<DByteGDL>(e, FOREGROUND_COLOR);
  //exit if problems
  if (backgroundColor) {if (backgroundColor->N_Elements()%3 != 0) e->Throw("Table grid color attribute has the wrong dimensions.");}
  if (foregroundColor) {if (foregroundColor->N_Elements()%3 != 0) e->Throw("Table grid color attribute has the wrong dimensions.");}
 
  DStringGDL* columnLabels = GetKeywordAs<DStringGDL>(e, COLUMN_LABELS);
  DLongGDL* columnWidth = GetKeywordAs<DLongGDL>(e, COLUMN_WIDTHS);

  static int AM_PM = e->KeywordIx( "AM_PM" );
  DStringGDL* amPm = GetKeywordAs<DStringGDL>(e, AM_PM);
  //check
  if (amPm) {if (amPm->N_Elements()!=2) e->Throw("% Keyword array parameter AM_PM must have 2 elements.");}
  static int DAYS_OF_WEEK = e->KeywordIx( "DAYS_OF_WEEK" );
  DStringGDL* daysOfWeek = GetKeywordAs<DStringGDL>(e, DAYS_OF_WEEK);
  //check
  if (daysOfWeek) {if (daysOfWeek->N_Elements()!=2) e->Throw("% Keyword array parameter DAYS_OF_WEEK must have 7 elements.");}
  static int MONTHS = e->KeywordIx( "MONTHS" );
  DStringGDL* month = GetKeywordAs<DStringGDL>(e, MONTHS);
  //check
  if (month) {if (month->N_Elements()!=12) e->Throw("% Keyword array parameter MONTH must have 12 elements.");}
  
  DStringGDL* format = GetKeywordAs<DStringGDL>(e, FORMAT);

  DLongGDL* rowHeights = GetKeywordAs<DLongGDL>(e, ROW_HEIGHTS);
  DStringGDL* rowLabels = GetKeywordAs<DStringGDL>(e, ROW_LABELS);

//  DLong tabMode = 0;
//  e->AssureLongScalarKWIfPresent( TAB_MODE, tabMode );

  int majority = GDLWidgetTable::NONE_MAJOR;

  static int x_scroll_sizeIx = e->KeywordIx( "X_SCROLL_SIZE" );
  DLong x_scroll_size = 0;
  e->AssureLongScalarKWIfPresent( x_scroll_sizeIx, x_scroll_size );
  static int y_scroll_sizeIx = e->KeywordIx( "Y_SCROLL_SIZE" );
  DLong y_scroll_size = 0;
  e->AssureLongScalarKWIfPresent( y_scroll_sizeIx, y_scroll_size );

  static int TRACKING_EVENTS = e->KeywordIx( "TRACKING_EVENTS" );
  static int ALL_EVENTS = e->KeywordIx( "ALL_EVENTS" );
  static int KBRD_FOCUS_EVENTS = e->KeywordIx( "KBRD_FOCUS_EVENTS" );
  static int CONTEXT_EVENTS = e->KeywordIx( "CONTEXT_EVENTS" );
  bool trackingevents = e->KeywordSet( TRACKING_EVENTS );
  bool allevents = e->KeywordSet( ALL_EVENTS );
  bool kbrdfocusevents = e->KeywordSet( KBRD_FOCUS_EVENTS );
  bool contextevents = e->KeywordSet( CONTEXT_EVENTS );

  DULong eventFlags=0;
  if (allevents)  eventFlags |= GDLWidget::EV_ALL;
  if (trackingevents)  eventFlags |= GDLWidget::EV_TRACKING;
  if (kbrdfocusevents) eventFlags |= GDLWidget::EV_KBRD_FOCUS;
  if (contextevents) eventFlags |= GDLWidget::EV_CONTEXT;


  BaseGDL* value = e->GetKW( VALUE ); //value may not exist!!!! Dangerous!!!
  DStringGDL* valueAsStrings;
  //test of non-conformity
  if (value != NULL) value=value->Dup();
  if (value && value->Rank() > 2 ) e->Throw("Value has greater than 2 dimensions.");
  else if (value && value->Rank() < 1 ) e->Throw("Expression must be an array in this context: "+e->GetParString(0));
  //local check of size given, changes number of lines/columns
  DLong xSize, ySize;
  static int xsizeIx = e->KeywordIx( "XSIZE" );
  static int ysizeIx = e->KeywordIx( "YSIZE" );
  xSize = -1;
  e->AssureLongScalarKWIfPresent( xsizeIx, xSize );
  ySize = -1;
  e->AssureLongScalarKWIfPresent( ysizeIx, ySize );
   
  if ( value == NULL ) { //set valueAsString. 
    SizeT dims[2];
    dims[0] = (xSize>0)?xSize:6;
    dims[1] = (ySize>0)?ySize:6;
    dimension dim(dims, 2); 
    valueAsStrings=new DStringGDL ( dim );
  } 
  else if (value->Type()==GDL_STRING) {valueAsStrings=static_cast<DStringGDL*>(value->Dup());} 
  else if (value->Type()==GDL_STRUCT) {
    if (value->Rank() > 1) e->Throw("Multi dimensional arrays of structures not allowed.");
    majority = GDLWidgetTable::ROW_MAJOR;
    if (e->KeywordSet( COLUMN_MAJOR )) majority = GDLWidgetTable::COLUMN_MAJOR;
    //convert to STRING
    DStructGDL *input=static_cast<DStructGDL*>(value);
    SizeT nTags = input->NTags();
    //further check:
    for (SizeT iTag=0; iTag<nTags; ++iTag) {
      BaseGDL* tested=input->GetTag(iTag);
      if (tested->Rank() > 0 || tested->Type()==GDL_STRUCT) e->Throw("Structures cannot include arrays or other structures.");
    }
    SizeT nEl   = input->N_Elements();
    SizeT dims[2];
    if (majority== GDLWidgetTable::COLUMN_MAJOR ) {
      dims[0] = nEl;
      dims[1] = nTags;
    } else {
      dims[1] = nEl;
      dims[0] = nTags;      
    }
    dimension dim(dims, 2); 
    valueAsStrings=new DStringGDL ( dim );
    stringstream os;
    input->ToStreamRaw(os);
    valueAsStrings->FromStream(os); //simple as that if we manage the dimensions and transpose accordingly....
    if ( majority == GDLWidgetTable::ROW_MAJOR ) {
      valueAsStrings=static_cast<DStringGDL*>(valueAsStrings->Transpose(NULL));
    //transpose back sizes only...
      SizeT dims[2];
      dims[1] = nEl;
      dims[0] = nTags;      
      dimension dim(dims, 2); 
      (static_cast<BaseGDL*>(valueAsStrings))->SetDim(dim);
    }
  } else {
    //convert to STRING using FORMAT.
    static int stringIx = LibFunIx("STRING");
    assert( stringIx >= 0);
    EnvT* newEnv= new EnvT(e, libFunList[stringIx], NULL);
    Guard<EnvT> guard( newEnv);
    // add parameters
    newEnv->SetNextPar( value->Dup());
    if (format) newEnv->SetNextPar( format->Dup() );
    // make the call
    valueAsStrings = static_cast<DStringGDL*>(static_cast<DLibFun*>(newEnv->GetPro())->Fun()(newEnv));
    //give back invalue's rank and dims to value, they have been lost in the process
    (static_cast<BaseGDL*>(valueAsStrings))->SetDim(value->Dim());
  }
  
  GDLWidgetTable* table = new GDLWidgetTable( parentID, e,
  alignment,
  amPm,
  backgroundColor,
  foregroundColor,
  columnLabels,
  majority,
  columnWidth,
  daysOfWeek,
  disjointSelection,
  editable,
  format,
//  ignoreAccelerators,
  month,
  noColumnHeaders,
  noRowHeaders,
  resizeableColumns,
  resizeableRows,
  rowHeights,
  rowLabels,
//  tabMode,
  value,
  x_scroll_size,
  y_scroll_size,
  valueAsStrings,
  eventFlags
  );
  table->SetWidgetType( GDLWidget::WIDGET_TABLE );
//  table->SetEventFlags(eventFlags);
  // return widget ID
  return new DLongGDL( table->WidgetID( ) );
#endif
} // widget_table


BaseGDL* widget_tree( EnvT* e)
{
#ifndef HAVE_LIBWXWIDGETS
  e->Throw( "GDL was compiled without support for wxWidgets" );
  return NULL; // avoid warning
#else
  SizeT nParam = e->NParam( 1 );

  DLongGDL* p0L = e->GetParAs<DLongGDL>(0);
  WidgetIDT parentID = (*p0L)[0];
  GDLWidget* parent = GDLWidget::GetWidget( parentID );
  if ( parent == NULL )  e->Throw( "Invalid widget identifier: " + i2s( parentID ) );
  if (!parent->IsBase()) e->Throw( "Parent is of incorrect type." );

  static int ALIGN_BOTTOM = e->KeywordIx( "ALIGN_BOTTOM" );
  static int ALIGN_CENTER = e->KeywordIx( "ALIGN_CENTER" );
  static int ALIGN_LEFT = e->KeywordIx( "ALIGN_LEFT" );
  static int ALIGN_RIGHT = e->KeywordIx( "ALIGN_RIGHT" );
  static int ALIGN_TOP = e->KeywordIx( "ALIGN_TOP" );
  static int BITMAP = e->KeywordIx( "BITMAP" );
  static int CHECKBOX = e->KeywordIx( "CHECKBOX" );
  static int CHECKED = e->KeywordIx( "CHECKED" );
  static int DRAG_NOTIFY = e->KeywordIx( "DRAG_NOTIFY" );
  static int DRAGGABLE = e->KeywordIx( "DRAGGABLE" );
  static int EXPANDED = e->KeywordIx( "EXPANDED" );
  static int FOLDER = e->KeywordIx( "FOLDER" );
  static int INDEX = e->KeywordIx( "INDEX" );
  static int MASK = e->KeywordIx( "MASK" );
  static int MULTIPLE = e->KeywordIx( "MULTIPLE" );
  static int NO_BITMAPS = e->KeywordIx( "NO_BITMAPS" );
  static int TAB_MODE = e->KeywordIx( "TAB_MODE" );
  static int TOOLTIP = e->KeywordIx( "TOOLTIP" );
  static int VALUE = e->KeywordIx( "VALUE" );

  bool alignBottom = e->KeywordSet( ALIGN_BOTTOM );
  bool alignCenter = e->KeywordSet( ALIGN_CENTER );
  bool alignLeft = e->KeywordSet( ALIGN_LEFT );
  bool alignRight = e->KeywordSet( ALIGN_RIGHT );
  bool alignTop = e->KeywordSet( ALIGN_TOP );
  bool checkbox = e->KeywordSet( CHECKBOX );
  bool draggable = e->KeywordSet( DRAGGABLE );
  bool expanded = e->KeywordSet( EXPANDED );
  bool folder = e->KeywordSet( FOLDER );
  bool mask = e->KeywordSet( MASK );
  bool multiple = e->KeywordSet( MULTIPLE );
  bool noBitmaps = e->KeywordSet( NO_BITMAPS );

  BaseGDL* bitmap = e->GetKW( BITMAP );

  DLong checked = 0;
  e->AssureLongScalarKWIfPresent( CHECKED, checked );
  DLong tabMode = 0;
  e->AssureLongScalarKWIfPresent( TAB_MODE, tabMode );
  DLong index = 0;
  e->AssureLongScalarKWIfPresent( INDEX, index );
  DString dragNotify;
  e->AssureStringScalarKWIfPresent( DRAG_NOTIFY, dragNotify );
  DString toolTip;
  e->AssureStringScalarKWIfPresent( TOOLTIP, toolTip );

  DString value=""; //important to init to a zero-length string!!!
  e->AssureStringScalarKWIfPresent( VALUE, value ); //important to init to a zero-length string!!!

  cout<<"Warning, WIDGET_TREE is not fully functional in GDL (FIXME)"<<endl;
  
  GDLWidgetTree* tree = new GDLWidgetTree( parentID, e, value,
  alignBottom,
  alignCenter,
  alignLeft,
  alignRight,
  alignTop,
  bitmap,
  checkbox,
  checked,
  dragNotify,
  draggable,
  expanded,
  folder,
  index,
  mask,
  multiple,
  noBitmaps,
  tabMode,
  toolTip );
  
  tree->SetWidgetType( GDLWidget::WIDGET_TREE );

  // return widget ID
  return new DLongGDL( tree->WidgetID( ) );
#endif
} // widget_tree

BaseGDL* widget_draw( EnvT* e ) {
#ifndef HAVE_LIBWXWIDGETS
  e->Throw( "GDL was compiled without support for wxWidgets" );
  return NULL; // avoid warning
#else
  SizeT nParam = e->NParam( 1 );

  DLongGDL* p0L = e->GetParAs<DLongGDL>(0);
  WidgetIDT parentID = (*p0L)[0];
  GDLWidget* p = GDLWidget::GetWidget( parentID );
  if ( p == NULL )
    e->Throw( "Invalid widget identifier: " + i2s( parentID ) );

  GDLWidgetBase* base = dynamic_cast<GDLWidgetBase*> (p);
  if ( base == NULL )
    e->Throw( "Parent is of incorrect type." );

  static int APP_SCROLL = e->KeywordIx( "APP_SCROLL" );
  bool app_scroll = e->KeywordSet(APP_SCROLL);
  static int x_scroll_sizeIx = e->KeywordIx( "X_SCROLL_SIZE" );
  DLong x_scroll_size = -1;
  e->AssureLongScalarKWIfPresent( x_scroll_sizeIx, x_scroll_size );
  static int y_scroll_sizeIx = e->KeywordIx( "Y_SCROLL_SIZE" );
  DLong y_scroll_size = -1;
  e->AssureLongScalarKWIfPresent( y_scroll_sizeIx, y_scroll_size );

  static int DROP_EVENTS = e->KeywordIx( "DROP_EVENTS" );
  static int EXPOSE_EVENTS = e->KeywordIx( "EXPOSE_EVENTS" );
  static int MOTION_EVENTS = e->KeywordIx( "MOTION_EVENTS" );
  static int TRACKING_EVENTS = e->KeywordIx( "TRACKING_EVENTS" );
  static int VIEWPORT_EVENTS = e->KeywordIx( "VIEWPORT_EVENTS" );
  static int WHEEL_EVENTS = e->KeywordIx( "WHEEL_EVENTS" );
  static int BUTTON_EVENTS = e->KeywordIx( "BUTTON_EVENTS" );
  static int KEYBOARD_EVENTS = e->KeywordIx( "KEYBOARD_EVENTS" );

  static int TOOLTIP = e->KeywordIx( "TOOLTIP" );

  // flags
  bool drop_events = e->KeywordSet( DROP_EVENTS );
  bool expose_events = e->KeywordSet( EXPOSE_EVENTS );
  bool motion_events = e->KeywordSet( MOTION_EVENTS );
  bool tracking_events = e->KeywordSet( TRACKING_EVENTS );
  bool viewport_events = e->KeywordSet( VIEWPORT_EVENTS );
  bool wheel_events = e->KeywordSet( WHEEL_EVENTS );
  bool button_events = e->KeywordSet( BUTTON_EVENTS );
  bool keyboard_events = e->KeywordPresent( KEYBOARD_EVENTS );

//  // TODO non-flags

  //  static int CLASSNAME = e->KeywordIx( "CLASSNAME" ); // string
//  static int COLOR_MODEL = e->KeywordIx( "COLOR_MODEL" );
//  static int COLORS = e->KeywordIx( "COLORS" ); // long
//  static int DRAG_NOTIFY = e->KeywordIx( "DRAG_NOTIFY" ); //string
//  static int GRAPHICS_LEVEL = e->KeywordIx( "GRAPHICS_LEVEL" );
//  static int IGNORE_ACCELERATORS = e->KeywordIx( "IGNORE_ACCELERATORS" );
//  static int RENDERER = e->KeywordIx( "RENDERER" );
//  static int RESOURCE_NAME = e->KeywordIx( "RESOURCE_NAME" ); // string
//  static int RETAIN = e->KeywordIx( "RETAIN" );
//  static int TOOLTIP = e->KeywordIx( "TOOLTIP" );

  DULong eventFlags=GDLWidget::EV_NONE;
  if (motion_events)  eventFlags |= GDLWidget::EV_MOTION;
  if (drop_events) eventFlags |= GDLWidget::EV_DROP;
  if (expose_events) eventFlags |= GDLWidget::EV_EXPOSE;
  if (tracking_events) eventFlags |= GDLWidget::EV_TRACKING;
  if (viewport_events) eventFlags |=  GDLWidget::EV_VIEWPORT;
  if (wheel_events) eventFlags |=  GDLWidget::EV_WHEEL;
  if (button_events) eventFlags |=  GDLWidget::EV_BUTTON;
  if (keyboard_events) {
    DLong val= (*e->GetKWAs<DLongGDL>(KEYBOARD_EVENTS))[0];
    if (val==2) { eventFlags |=  GDLWidget::EV_KEYBOARD2;}
    else if (val==1)  {eventFlags |=  GDLWidget::EV_KEYBOARD;}
  }
  GDLWidgetDraw* draw;
  if (e->KeywordPresent(TOOLTIP)) {
   DStringGDL* tooltipgdl = e->GetKWAs<DStringGDL>(TOOLTIP) ;
   draw = new GDLWidgetDraw( parentID, e,  x_scroll_size, y_scroll_size, app_scroll, eventFlags, tooltipgdl);
  }
  else draw = new GDLWidgetDraw( parentID, e,  x_scroll_size, y_scroll_size, app_scroll, eventFlags);

  draw->SetWidgetType( GDLWidget::WIDGET_DRAW );
  if (keyboard_events) draw->SetFocus();
  // return widget ID
  return new DLongGDL( draw->WidgetID( ) );
#endif  
} // widget_draw

  
  
    
  BaseGDL* widget_base( EnvT* e)
  {
#ifndef HAVE_LIBWXWIDGETS
  e->Throw( "GDL was compiled without support for wxWidgets" );
  return NULL; // avoid warning
#else
  SizeT nParam = e->NParam( );

  WidgetIDT parentID = 0;
  if ( nParam == 1 ) // no TLB
    e->AssureLongScalarPar( 0, parentID );

  // handle some more keywords over widget
  
//  static int align_bottomIx = e->KeywordIx( "ALIGN_BOTTOM" );
//  static int align_centerIx = e->KeywordIx( "ALIGN_CENTER" );
//  static int align_leftIx = e->KeywordIx( "ALIGN_LEFT" );
//  static int align_rightIx = e->KeywordIx( "ALIGN_RIGHT" );
//  static int align_topIx = e->KeywordIx( "ALIGN_TOP" );
  static int mbarIx = e->KeywordIx( "MBAR" );
  static int obsolete_app_mbarIx = e->KeywordIx( "APP_MBAR" );
  static int modalIx = e->KeywordIx( "MODAL" );
//  static int base_align_bottomIx = e->KeywordIx( "BASE_ALIGN_BOTTOM" );
  static int base_align_centerIx = e->KeywordIx( "BASE_ALIGN_CENTER" );
  static int base_align_leftIx = e->KeywordIx( "BASE_ALIGN_LEFT" );
  static int base_align_rightIx = e->KeywordIx( "BASE_ALIGN_RIGHT" );
//  static int base_align_topIx = e->KeywordIx( "BASE_ALIGN_TOP" );
  static int columnIx = e->KeywordIx( "COLUMN" );
  static int rowIx = e->KeywordIx( "ROW" );
  static int context_eventsIx = e->KeywordIx( "CONTEXT_EVENTS" );
//  static int context_menuIx = e->KeywordIx( "CONTEXT_MENU" );
  static int exclusiveIx = e->KeywordIx( "EXCLUSIVE" );
  static int nonexclusiveIx = e->KeywordIx( "NONEXCLUSIVE" );
  static int floatingIx = e->KeywordIx( "FLOATING" );
  static int grid_layoutIx = e->KeywordIx( "GRID_LAYOUT" );
  static int kbrd_focus_eventsIx = e->KeywordIx( "KBRD_FOCUS_EVENTS" );
  static int mapIx = e->KeywordIx( "MAP" );
  static int spaceIx = e->KeywordIx( "SPACE" );
  static int titleIx = e->KeywordIx( "TITLE" );
//  static int tlb_frame_attrIx = e->KeywordIx( "TLB_FRAME_ATTR" );
  static int tlb_iconify_eventsIx = e->KeywordIx( "TLB_ICONIFY_EVENTS" );
  static int tlb_kill_request_eventsIx = e->KeywordIx( "TLB_KILL_REQUEST_EVENTS" );
  static int tlb_move_eventsIx = e->KeywordIx( "TLB_MOVE_EVENTS" );
  static int tlb_size_eventsIx = e->KeywordIx( "TLB_SIZE_EVENTS" );
//  static int toolbarIx = e->KeywordIx( "TOOLBAR" );
  static int tracking_eventsIx = e->KeywordIx( "TRACKING_EVENTS" );
  static int xpadIx = e->KeywordIx( "XPAD" );
  static int x_scroll_sizeIx = e->KeywordIx( "X_SCROLL_SIZE" );
  static int ypadIx = e->KeywordIx( "YPAD" );
  static int y_scroll_sizeIx = e->KeywordIx( "Y_SCROLL_SIZE" );
  static int display_nameIx = e->KeywordIx( "DISPLAY_NAME" );
  static int resource_nameIx = e->KeywordIx( "RESOURCE_NAME" );
  static int rname_mbarIx = e->KeywordIx( "RNAME_MBAR" );

//  // own alignment: done in SetCommonKeywords
//  bool align_bottom = e->KeywordSet( align_bottomIx );
//  bool align_center = e->KeywordSet( align_centerIx );
//  bool align_left = e->KeywordSet( align_leftIx );
//  bool align_right = e->KeywordSet( align_rightIx );
//  bool align_top = e->KeywordSet( align_topIx );
//
//  // children default alignment
//  bool base_align_bottom = e->KeywordSet( base_align_bottomIx );
//  bool base_align_top = e->KeywordSet( base_align_topIx );

  long children_alignment=wxALIGN_NOT;
  if (e->KeywordSet(base_align_leftIx )) children_alignment=wxALIGN_LEFT;
  if (e->KeywordSet(base_align_centerIx)) children_alignment=wxALIGN_CENTER;
  if (e->KeywordSet(base_align_rightIx)) children_alignment=wxALIGN_RIGHT;

  bool modal = e->KeywordSet( modalIx );

  bool context_events = e->KeywordSet( context_eventsIx );
//  bool context_menu = e->KeywordSet( context_menuIx );

  bool exclusive = e->KeywordSet( exclusiveIx );
  bool nonexclusive = e->KeywordSet( nonexclusiveIx );

  bool floating = e->KeywordSet( floatingIx );
  bool grid_layout = e->KeywordSet( grid_layoutIx );
  bool kbrd_focus_events = e->KeywordSet( kbrd_focus_eventsIx );

  bool mapWid = true;
  if ( e->KeywordPresent( mapIx ) ) if ( !e->KeywordSet( mapIx ) )  mapWid = false;
  //    std::cout << "Map in widget_base: " << mapWid << std::endl;

  //     bool scroll = e->KeywordSet( scrollIx);
//  bool tlb_frame_attr = e->KeywordSet( tlb_frame_attrIx );
  bool tlb_iconify_events = e->KeywordSet( tlb_iconify_eventsIx );
  bool tlb_kill_request_events = e->KeywordSet( tlb_kill_request_eventsIx );
  bool tlb_move_events = e->KeywordSet( tlb_move_eventsIx );
  bool tlb_size_events = e->KeywordSet( tlb_size_eventsIx );
  bool tracking_events = e->KeywordSet( tracking_eventsIx );
//  bool toolbar = e->KeywordSet( toolbarIx );

  DLong x_scroll_size = -1;
  e->AssureLongScalarKWIfPresent( x_scroll_sizeIx, x_scroll_size );
  DLong y_scroll_size = -1;
  e->AssureLongScalarKWIfPresent( y_scroll_sizeIx, y_scroll_size );


  bool mbarPresent = e->KeywordPresent( mbarIx )||e->KeywordPresent( obsolete_app_mbarIx );

  DLong space= 0;
  e->AssureLongScalarKWIfPresent( spaceIx, space );
  DLong column = 0;
  e->AssureLongScalarKWIfPresent( columnIx, column );
  DLong row = 0;
  e->AssureLongScalarKWIfPresent( rowIx, row );

  if (column>0 && row>0) e->Throw( "Conflicting keywords: row vs. col" );

  DLong xpad = 0;
  e->AssureLongScalarKWIfPresent( xpadIx, xpad );
  DLong ypad = 0;
  e->AssureLongScalarKWIfPresent( ypadIx, ypad );

  DString resource_name = "";
  e->AssureStringScalarKWIfPresent( resource_nameIx, resource_name );

  DString rname_mbar = "";
  e->AssureStringScalarKWIfPresent( rname_mbarIx, rname_mbar );

  DString title = "GDL";
  e->AssureStringScalarKWIfPresent( titleIx, title );

  DString display_name = "";
  e->AssureStringScalarKWIfPresent( display_nameIx, display_name );

  // consistency
  if ( nonexclusive && exclusive )
    e->Throw( "Conflicting keywords: [NON]EXCLUSIVE" );

    if( mbarPresent)
      {
    if ( parentID != 0 )
      e->Throw( "Only top level bases allow a menubar." );
    if (e->KeywordPresent( mbarIx )){ //prefer MBAR over obsolete app_mbar
    e->AssureGlobalKW( mbarIx );}
    else if (e->KeywordPresent( obsolete_app_mbarIx )){ 
    e->AssureGlobalKW( obsolete_app_mbarIx );}
    
  }

    if( modal)
    {
      //we must test groupleader even before it is set up by SetCommonKeywords.
    DLong groupLeader = 0;
    static int group_leaderIx = e->KeywordIx( "GROUP_LEADER" );
    e->AssureLongScalarKWIfPresent( group_leaderIx, groupLeader );
    if ( groupLeader == 0 )
      e->Throw( "MODAL top level bases must have a group leader specified." );
    if ( parentID != 0 )
      e->Throw( "Only top level bases can be MODAL." );
  }
    if( parentID != 0)
      { 
    GDLWidget* p = GDLWidget::GetWidget( parentID );
    if ( p == NULL )
      e->Throw( "Invalid widget identifier: " + i2s( parentID ) );
    if ( !p->IsBase( ) && !p->IsTab( ) ) e->Throw( "Parent is of incorrect type." );
  }
  //...

  // generate widget
  WidgetIDT mBarID = mbarPresent ? 1 : 0;

  int exclusiveMode = GDLWidget::BGNORMAL;
  if ( exclusive ) exclusiveMode = GDLWidget::BGEXCLUSIVE;
  if ( nonexclusive ) exclusiveMode = GDLWidget::BGNONEXCLUSIVE;

  DULong eventFlags=0;
  if (context_events) eventFlags |= GDLWidget::EV_CONTEXT;
  if (kbrd_focus_events) eventFlags |= GDLWidget::EV_KBRD_FOCUS;
  if (tlb_move_events)  eventFlags |= GDLWidget::EV_TRACKING;
  if (tlb_size_events) eventFlags |= GDLWidget::EV_SIZE;
  if (tlb_iconify_events) eventFlags |= GDLWidget::EV_ICONIFY;
  if (tlb_kill_request_events) eventFlags |= GDLWidget::EV_KILL;
  if (tracking_events) eventFlags |= GDLWidget::EV_TRACKING;
  
  GDLWidgetBase* base = new GDLWidgetBase( parentID, e,
  mapWid,
  /*ref*/ mBarID, modal,
  column, row,
  exclusiveMode,
  floating,
  resource_name, rname_mbar,
  title,
  display_name,
  xpad, ypad,
  x_scroll_size, y_scroll_size, grid_layout , children_alignment, space);

  // some more properties
  if ( mbarPresent ) {
    if ( e->KeywordPresent( mbarIx ) ) { //prefer MBAR over obsolete app_mbar
      e->SetKW( mbarIx, new DLongGDL( mBarID ) );
    } else if ( e->KeywordPresent( obsolete_app_mbarIx ) ) {
      e->SetKW( obsolete_app_mbarIx, new DLongGDL( mBarID ) );
    }
  }
  
  base->SetWidgetType( GDLWidget::WIDGET_BASE );
  // Set flags 
  base->SetEventFlags(eventFlags);
  //unmap if necessary
  if (!mapWid) base->mapBase(mapWid);
  // return widget ID
  return new DLongGDL( base->WidgetID( ) );
#endif
}


// WIDGET_BUTTON
  BaseGDL* widget_button( EnvT* e)
  {
#ifndef HAVE_LIBWXWIDGETS
  e->Throw( "GDL was compiled without support for wxWidgets" );
  return NULL; // avoid warning
#else
  static SizeT buttonNumber=1; //for default value
  //"HELP" is not supported (yet).
  SizeT nParam = e->NParam( 1 );

  DLongGDL* p0L = e->GetParAs<DLongGDL>(0);
  WidgetIDT parentID = (*p0L)[0];
  GDLWidget *widget = GDLWidget::GetWidget( parentID );
  if (widget==NULL) e->Throw( "Invalid widget identifier: " + i2s( parentID ) );
// Note that when parent is a button but not a menu button, is reported in GDLWidgetButton()
  if (!widget->IsBase() && !widget->IsMenuBar() && !widget->IsButton() ) e->Throw( "Parent is of incorrect type." );

  static int bitmapIx = e->KeywordIx( "BITMAP" );
  static int menuIx = e->KeywordIx( "MENU" );
  static int SeparatorIx = e->KeywordIx( "SEPARATOR" );
  static int TOOLTIP = e->KeywordIx( "TOOLTIP" );
  static int valueIx = e->KeywordIx( "VALUE" );


  BaseGDL* invalue = e->GetKW( valueIx );

  bool isBitmap =  e->KeywordSet( bitmapIx );
  bool isMenu =  e->KeywordSet( menuIx );
  bool hasSeparatorAbove= e->KeywordSet(SeparatorIx) ;
  
  DString value = "button"+i2s(buttonNumber++); //tested default!

  wxBitmap * bitmap=NULL;
  //value=filename if /BITMAP present, bitmap if array.
  if (invalue) {
    if (isBitmap) {
      e->AssureStringScalarKWIfPresent( valueIx, value ); //value is a filename
      //try loading file
      wxInitAllImageHandlers();
      wxImage * tryImage=new wxImage(wxString(value.c_str(),wxConvUTF8),wxBITMAP_TYPE_ANY);
      if (tryImage->IsOk()) bitmap = new wxBitmap(*tryImage);
      else e->Throw( "Unable to read image file: " + value );
      value.clear();
  //     if (tryImage.LoadFile(_(value))) 
  //    { bitmap = wxBitmap(tryImage); } else {e->Throw( "Unable to read image file: " + value );}
    }else if (invalue->Type()==GDL_STRING) {
      e->AssureStringScalarKWIfPresent( valueIx, value );
    } else {
      DByteGDL* testByte=e->GetKWAs<DByteGDL>(valueIx);
      if (testByte) { //must be n x m or n x m x 3
        if (testByte->Rank() < 2 || testByte->Rank() > 3) e->Throw( "Array must be a [X,Y] or [X,Y,3] array." );
        if (testByte->Rank() == 3 && testByte->Dim(2) != 3) e->Throw( "Array must be a [X,Y] or [X,Y,3] array." );
        if (testByte->Rank() == 2) {
          bitmap = new wxBitmap(static_cast<char*>(testByte->DataAddr()),testByte->Dim(0)*8,testByte->Dim(1),1);
        } else {
          BaseGDL* transpose=testByte->Transpose(NULL);
          wxImage * tryImage=new wxImage(transpose->Dim(1),transpose->Dim(2),static_cast<unsigned char*>(transpose->DataAddr()),TRUE); //STATIC DATA I BELIEVE.
          GDLDelete( transpose );
          bitmap = new wxBitmap(*tryImage);
        }
      } else  e->Throw( "Unsupported VALUE Keyword type, please report!" );
    }
  }
  //    cout << value << ",  ParentID : "<<  parentID <<  endl;

  GDLWidgetButton* button;
  if (e->KeywordPresent(TOOLTIP)) {
   DStringGDL* tooltipgdl = e->GetKWAs<DStringGDL>(TOOLTIP) ;
   button = new GDLWidgetButton( parentID, e, value, isMenu, hasSeparatorAbove, bitmap, tooltipgdl);
  }
  else button = new GDLWidgetButton( parentID, e, value, isMenu, hasSeparatorAbove, bitmap);
  
  button->SetWidgetType( GDLWidget::WIDGET_BUTTON );
  return new DLongGDL( button->WidgetID( ) );
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

BaseGDL* widget_list( EnvT* e ) {
#ifndef HAVE_LIBWXWIDGETS
  e->Throw( "GDL was compiled without support for wxWidgets" );
  return NULL; // avoid warning
#else
  SizeT nParam = e->NParam( 1 );

  DLongGDL* p0L = e->GetParAs<DLongGDL>(0);
  WidgetIDT parentID = (*p0L)[0];
  GDLWidget *widget = GDLWidget::GetWidget( parentID );
  if (widget==NULL) e->Throw( "Invalid widget identifier: " + i2s( parentID ) );
  if (!widget->IsBase()) e->Throw( "Parent is of incorrect type." );

  static int valueIx = e->KeywordIx( "VALUE" );
  BaseGDL* value = e->GetKW( valueIx );
  if ( value != NULL ) value = value->Dup( ); else value = new DStringGDL(""); //protect!

  static int multipleIx = e->KeywordIx( "MULTIPLE" );
  bool multiple = e->KeywordSet( multipleIx );

  DLong style = multiple ? wxLB_EXTENDED /*wxLB_MULTIPLE*/ : wxLB_SINGLE;
  GDLWidgetList* list = new GDLWidgetList( parentID, e, value, style );
  list->SetWidgetType( GDLWidget::WIDGET_LIST );

  return new DLongGDL( list->WidgetID( ) );
#endif
}

// WIDGET_DROPLIST
  BaseGDL* widget_droplist( EnvT* e)
  {
#ifndef HAVE_LIBWXWIDGETS
  e->Throw( "GDL was compiled without support for wxWidgets" );
  return NULL; // avoid warning
#else
  SizeT nParam = e->NParam( 1 );

  DLongGDL* p0L = e->GetParAs<DLongGDL>(0);
  WidgetIDT parentID = (*p0L)[0];
  GDLWidget *widget = GDLWidget::GetWidget( parentID );
  if (widget==NULL) e->Throw( "Invalid widget identifier: " + i2s( parentID ) );
  if (!widget->IsBase()) e->Throw( "Parent is of incorrect type." );

  static int titleIx = e->KeywordIx( "TITLE" );
  DString title = "";
  e->AssureStringScalarKWIfPresent( titleIx, title );

  static int valueIx = e->KeywordIx( "VALUE" );
  BaseGDL* value = e->GetKW( valueIx );
  if ( value != NULL )  value = value->Dup( ); else value = new DStringGDL(""); //protect!

  DLong style = 0;
  GDLWidgetDropList* droplist = new GDLWidgetDropList( parentID, e, value, title, style );
  droplist->SetWidgetType( GDLWidget::WIDGET_DROPLIST );

  return new DLongGDL( droplist->WidgetID( ) );
#endif
}


// WIDGET_COMBOBOX

BaseGDL* widget_combobox( EnvT* e ) {
#ifndef HAVE_LIBWXWIDGETS
  e->Throw( "GDL was compiled without support for wxWidgets" );
  return NULL; // avoid warning
#else
  SizeT nParam = e->NParam( 1 );

  DLongGDL* p0L = e->GetParAs<DLongGDL>(0);
  WidgetIDT parentID = (*p0L)[0];
  GDLWidget *widget = GDLWidget::GetWidget( parentID );
  if (widget==NULL) e->Throw( "Invalid widget identifier: " + i2s( parentID ) );
  if (!widget->IsBase()) e->Throw( "Parent is of incorrect type." );

  static int titleIx = e->KeywordIx( "TITLE" );
  DString title = "";
  e->AssureStringScalarKWIfPresent( titleIx, title );

  static int valueIx = e->KeywordIx( "VALUE" );
  BaseGDL* value = e->GetKW( valueIx );
  if ( value != NULL ) value = value->Dup( );
  else value=new DStringGDL("");
  
  static int editableIx = e->KeywordIx( "EDITABLE" );
  bool editable = e->KeywordSet( editableIx );

  DLong style = wxCB_SIMPLE;
  if ( !editable )
    style = wxCB_READONLY;

  GDLWidgetComboBox* combobox = new GDLWidgetComboBox( parentID, e, value, title, style );
  combobox->SetWidgetType( GDLWidget::WIDGET_COMBOBOX );

  return new DLongGDL( combobox->WidgetID( ) );
#endif
}

 
  BaseGDL* widget_tab( EnvT* e)
  {
#ifndef HAVE_LIBWXWIDGETS
  e->Throw( "GDL was compiled without support for wxWidgets" );
  return NULL; // avoid warning
#else
  SizeT nParam = e->NParam( 1 );

  DLongGDL* p0L = e->GetParAs<DLongGDL>(0);
  WidgetIDT parentID = (*p0L)[0];
  GDLWidget *widget = GDLWidget::GetWidget( parentID );
  if (widget==NULL) e->Throw( "Invalid widget identifier: " + i2s( parentID ) );
  if (!widget->IsBase()) e->Throw( "Parent is of incorrect type." );

  DLong multiline = 0;
  static int multilineIx = e->KeywordIx( "MULTILINE" );
  e->AssureLongScalarKWIfPresent( multilineIx, multiline );

  DLong location = 0;
  static int locationIx = e->KeywordIx( "LOCATION" );
  e->AssureLongScalarKWIfPresent( locationIx, location );

  GDLWidgetTab* tab = new GDLWidgetTab( parentID, e, location, multiline );
  
//  WidgetIDT id=0;
//  GDLWidgetBase* tab = new GDLWidgetBase( parentID, e,
//		 FALSE,
//		 id, FALSE, 
//		 (DLong) 0, (DLong) 0,
//		 (int) 0, 
//		 FALSE,
//		 "", "",
//		 "title",
//		 "",
//		 (DLong) -1, (DLong) -1,
//		 (DLong) -1, (DLong) -1, FALSE, (long) 0);
  
  tab->SetWidgetType( GDLWidget::WIDGET_TAB );

  return new DLongGDL( tab->WidgetID( ) );
#endif
}

BaseGDL* widget_slider( EnvT* e ) {
#ifndef HAVE_LIBWXWIDGETS
  e->Throw( "GDL was compiled without support for wxWidgets" );
  return NULL; // avoid warning
#else
  SizeT nParam = e->NParam( 1 );

  DLongGDL* p0L = e->GetParAs<DLongGDL>(0);
  WidgetIDT parentID = (*p0L)[0];
  GDLWidget *widget = GDLWidget::GetWidget( parentID );
  if (widget==NULL) e->Throw( "Invalid widget identifier: " + i2s( parentID ) );
  if (!widget->IsBase()) e->Throw( "Parent is of incorrect type." );

  DLong minimum = 0;
  static int minimumIx = e->KeywordIx( "MINIMUM" );
  e->AssureLongScalarKWIfPresent( minimumIx, minimum );
  DLong maximum = 100;
  static int maximumIx = e->KeywordIx( "MAXIMUM" );
  e->AssureLongScalarKWIfPresent( maximumIx, maximum );

  DLong value = minimum;
  static int valueIx = e->KeywordIx( "VALUE" );
  e->AssureLongScalarKWIfPresent( valueIx, value );

  static int dragIx = e->KeywordIx( "DRAG" );
  bool drag = e->KeywordSet( dragIx );

  static int verticalIx = e->KeywordIx( "VERTICAL" );
  bool vertical = e->KeywordSet( verticalIx );

  static int suppressValueIx = e->KeywordIx( "SUPPRESS_VALUE" );
  bool suppressValue = e->KeywordSet( suppressValueIx );

  DString title;
  static int titleIx = e->KeywordIx( "TITLE" );
  e->AssureStringScalarKWIfPresent( titleIx, title );

  GDLWidgetSlider* sl = new GDLWidgetSlider( parentID, e,
  value, minimum, maximum,
  vertical,
  suppressValue,
  title
  );
  sl->SetWidgetType( GDLWidget::WIDGET_SLIDER );

  return new DLongGDL( sl->WidgetID( ) );
#endif
}

// WIDGET_TEXT
  BaseGDL* widget_text( EnvT* e)
  {
#ifndef HAVE_LIBWXWIDGETS
  e->Throw( "GDL was compiled without support for wxWidgets" );
  return NULL; // avoid warning
#else
  SizeT nParam = e->NParam( 1 );

  DLongGDL* p0L = e->GetParAs<DLongGDL>(0);
  WidgetIDT parentID = (*p0L)[0];
  GDLWidget *widget = GDLWidget::GetWidget( parentID );
  if (widget==NULL) e->Throw( "Invalid widget identifier: " + i2s( parentID ) );
  
  static int TRACKING_EVENTS = e->KeywordIx( "TRACKING_EVENTS" );
  static int ALL_EVENTS = e->KeywordIx( "ALL_EVENTS" );
  static int KBRD_FOCUS_EVENTS = e->KeywordIx( "KBRD_FOCUS_EVENTS" );
  static int CONTEXT_EVENTS = e->KeywordIx( "CONTEXT_EVENTS" );
  bool trackingevents = e->KeywordSet( TRACKING_EVENTS );
  bool allevents = e->KeywordSet( ALL_EVENTS );
  bool kbrdfocusevents = e->KeywordSet( KBRD_FOCUS_EVENTS );
  bool contextevents = e->KeywordSet( CONTEXT_EVENTS );

  DULong eventFlags=0;
  if (allevents)  eventFlags |= GDLWidget::EV_ALL;
  if (trackingevents)  eventFlags |= GDLWidget::EV_TRACKING;
  if (kbrdfocusevents) eventFlags |= GDLWidget::EV_KBRD_FOCUS;
  if (contextevents) eventFlags |= GDLWidget::EV_CONTEXT;

  static int noNewLineIx = e->KeywordIx( "NO_NEWLINE" );
  bool noNewLine = e->KeywordSet( noNewLineIx );

  DStringGDL* valueStr = NULL;
  static int valueIx = e->KeywordIx( "VALUE" );
  BaseGDL* valueKW = e->GetKW( valueIx );
  if ( valueKW != NULL ) {
    if ( valueKW->Type( ) != GDL_STRING )
      e->Throw( "VALUE must be a STRING." );
    valueStr = static_cast<DStringGDL*> (valueKW);
    bool success = e->StealLocalKW( valueIx );
    if ( !success )
      valueStr = valueStr->Dup( );
  } else valueStr=new DStringGDL(""); //important to init to a zero-length string!!!

  DLong edit = 0;
  static int editableIx = e->KeywordIx( "EDITABLE" );
  e->AssureLongScalarKWIfPresent( editableIx, edit );
  bool editable = (edit == 1);
  
  GDLWidgetText* text = new GDLWidgetText( parentID, e, valueStr, noNewLine, editable);
  text->SetWidgetType( GDLWidget::WIDGET_TEXT );
  text->SetEventFlags(eventFlags);

  return new DLongGDL( text->WidgetID( ) );
#endif
}


// WIDGET_LABEL
  BaseGDL* widget_label( EnvT* e)
  {
#ifndef HAVE_LIBWXWIDGETS
  e->Throw( "GDL was compiled without support for wxWidgets" );
  return NULL; // avoid warning
#else
  SizeT nParam = e->NParam( 1 );

  DLongGDL* p0L = e->GetParAs<DLongGDL>(0);
  WidgetIDT parentID = (*p0L)[0];
  GDLWidget *widget = GDLWidget::GetWidget( parentID );
  if (widget==NULL) e->Throw( "Invalid widget identifier: " + i2s( parentID ) );
  if (!widget->IsBase()) e->Throw( "Parent is of incorrect type." );

  static int valueIx = e->KeywordIx( "VALUE" );
  DString value = "label"; //tested default!
  e->AssureStringScalarKWIfPresent( valueIx, value );
  
  static int sunkenIx = e->KeywordIx( "SUNKEN_FRAME" );
  bool isSunken=e->KeywordSet(sunkenIx);
  
  static int TRACKING_EVENTS = e->KeywordIx( "TRACKING_EVENTS" );
  bool trackingevents = e->KeywordPresent( TRACKING_EVENTS );
  DULong eventFlags=0;
  if (trackingevents)  eventFlags |= GDLWidget::EV_TRACKING;

  GDLWidgetLabel* label = new GDLWidgetLabel( parentID, e, value , isSunken);
  label->SetWidgetType( GDLWidget::WIDGET_LABEL );
  if (trackingevents){
   if (e->KeywordSet(TRACKING_EVENTS))  label->AddEventType(GDLWidget::EV_TRACKING);
   else label->RemoveEventType(GDLWidget::EV_TRACKING);
  }
  return new DLongGDL( label->WidgetID( ) );
#endif
}


// WIDGET_INFO
//TBD: % WIDGET_INFO: Keyword parameter PARENT not allowed in call to: WIDGET_INFO
BaseGDL* widget_info( EnvT* e ) {
#ifndef HAVE_LIBWXWIDGETS
  e->Throw( "GDL was compiled without support for wxWidgets" );
  return NULL;
#else
  SizeT nParam = e->NParam( );

  DLongGDL* p0L = NULL;
  SizeT nEl = 0;
  SizeT rank = 0;
  if ( nParam > 1 ) {
    e->Throw("Incorrect number of arguments.");
  }
  
  static int activeIx = e->KeywordIx( "ACTIVE" );
  bool active = e->KeywordSet( activeIx );
  
  static int validIx = e->KeywordIx( "VALID_ID" );
  bool valid = e->KeywordSet( validIx );

  static int modalIx = e->KeywordIx( "MODAL" );
  bool modal = e->KeywordSet( modalIx );

  static int managedIx = e->KeywordIx( "MANAGED" );
  bool managed = e->KeywordSet( managedIx );

  static int xmanagerBlockIx = e->KeywordIx( "XMANAGER_BLOCK" );
  bool xmanagerBlock = e->KeywordSet( xmanagerBlockIx );

  static int childIx = e->KeywordIx( "CHILD" );
  bool child = e->KeywordSet( childIx );

  static int versionIx = e->KeywordIx( "VERSION" );
  bool version = e->KeywordSet( versionIx );
  
  static int geometryIx = e->KeywordIx( "GEOMETRY" );
  bool geometry = e->KeywordSet( geometryIx );
  
  static int nameIx = e->KeywordIx( "NAME");
  bool name = e->KeywordSet( nameIx );
  static int typeIx = e->KeywordIx( "TYPE");
  bool type = e->KeywordSet( typeIx );

  static int unameIx = e->KeywordIx( "UNAME");
  bool uname = e->KeywordSet( unameIx );

  static int findbyunameIx = e->KeywordIx( "FIND_BY_UNAME");
  bool findbyuname = e->KeywordPresent( findbyunameIx );

  static int fontnameIx = e->KeywordIx( "FONTNAME");
  bool fontname = e->KeywordSet( fontnameIx );

  static int buttonsetIx = e->KeywordIx( "BUTTON_SET");
  bool buttonset = e->KeywordSet( buttonsetIx );
  
  static int parentIx = e->KeywordIx( "PARENT");
  bool parent = e->KeywordSet( parentIx );
  
  static int textselectIx = e->KeywordIx( "TEXT_SELECT");
  bool textselect = e->KeywordPresent( textselectIx );
  
  static int tabledisjointselectionIx = e->KeywordIx( "TABLE_DISJOINT_SELECTION"); 
  bool tabledisjointselection = e->KeywordSet( tabledisjointselectionIx );

  static int tableselectIx = e->KeywordIx( "TABLE_SELECT");
  bool tableselect = e->KeywordSet( tableselectIx );

  static int USE_TABLE_SELECT = e->KeywordIx( "USE_TABLE_SELECT");

  static int COLUMN_WIDTHS = e->KeywordIx( "COLUMN_WIDTHS" );
  bool giveColumnWidths = e->KeywordSet(COLUMN_WIDTHS);
  static int ROW_HEIGHTS = e->KeywordIx( "ROW_HEIGHTS" );
  bool giveRowHeights = e->KeywordSet(ROW_HEIGHTS);
     
  static int SYSTEM_COLORS = e->KeywordIx( "SYSTEM_COLORS" );
  bool giveSystemColors = e->KeywordSet(SYSTEM_COLORS);
  
  //find a string, return a long
  if (findbyuname) {
    DStringGDL* myUname = e->GetKWAs<DStringGDL>(findbyunameIx);
    if (myUname == NULL) return new DLongGDL( 0 );
    DLongGDL* list = static_cast<DLongGDL*>( GDLWidget::GetWidgetsList( ) );
    for (SizeT i=0; i< list->N_Elements(); ++i) {
      GDLWidget* widget = GDLWidget::GetWidget( (*list)[i] );
      if ( widget != NULL ){
        if ((*myUname)[0] == widget->GetUname() ) return new DLongGDL(widget->WidgetID());
      }
    }
    return new DLongGDL( 0 );
  }
  
  if ( nParam > 0 ) {
    p0L = e->GetParAs<DLongGDL>(0);
    nEl = p0L->N_Elements( );
    rank = p0L->Rank( );
  } else {
  //only possible with ACTIVE, VERSION or MANAGED.  
  }

  //active is also used for the moment to list all windows hierarchy for debug purposes.
  if (active) {
      DLongGDL* res = static_cast<DLongGDL*>( GDLWidget::GetWidgetsList( ) );
      if ((*res)[0]==0) return new DLongGDL(0);
      cerr<<" widgets: "; for ( SizeT i = 0; i < res->N_Elements(); i++ ) cerr<<(*res)[i]<<","; cerr<<endl;
      return new DLongGDL( (GDLWidget::GetNumberOfWidgets( ) > 0)?1:0 );
    }
  
  // Returns a String, empty if no result:
  // UNAME, FONTNAME keywords
  if ( uname || fontname || name ) {
    if ( rank == 0 ) {
      // Scalar Input
      WidgetIDT widgetID = (*p0L)[0];
      GDLWidget *widget = GDLWidget::GetWidget( widgetID );
      if ( widget == NULL ) {
        e->Throw("Invalid widget identifier:"+i2s(widgetID));
      } else {
        if (uname) return new DStringGDL(widget->GetUname());
        else if (name) return new DStringGDL(widget->GetWidgetName());
        // else if (fontname) //not programmed yet
      }
    } else {
      // Array Input
      DStringGDL* res = new DStringGDL( p0L->Dim( ), BaseGDL::NOZERO );
      bool atLeastOneFound=false;
      for ( SizeT i = 1; i < nEl; i++ ) {
        WidgetIDT widgetID = (*p0L)[i];
        GDLWidget *widget = GDLWidget::GetWidget( widgetID );
        DString result = "";
        if ( widget != NULL ) {
          atLeastOneFound=TRUE;
          if (uname) result = widget->GetUname();
          else if (name) result = widget->GetWidgetName();
          // else if (fontname) //not programmed yet
        }
        (*res)[i] = result;
      }
      if (atLeastOneFound) return res; else e->Throw("Invalid widget identifier:"+i2s((*p0L)[0]));
    }
  }

  // returns a long where 0 is "no info"
  // PARENT, CHILD keyword
  if ( child || parent || type ) {
    if ( rank == 0 ) {
      // Scalar Input
      WidgetIDT widgetID = (*p0L)[0];
      GDLWidget *widget = GDLWidget::GetWidget( widgetID );
      if ( widget == NULL ) {
        e->Throw("Invalid widget identifier:"+i2s(widgetID));
      } else {
        DLong result=0;
        if (child) {
          if ( widget->NChildren( ) != 0 ) result = widget->GetChild( 0 ); //may not have children
        }
        else if (parent)  result = widget->GetParentID( ); //but parent is always defined...
        else if (type)  result = widget->GetWidgetType( ); 
        return new DLongGDL( result );
      }
    } else {
      // Array Input
      DLongGDL* res = new DLongGDL( p0L->Dim( ), BaseGDL::NOZERO );
      bool atLeastOneFound=false;
      for ( SizeT i = 1; i < nEl; i++ ) {
        WidgetIDT widgetID = (*p0L)[i];
        GDLWidget *widget = GDLWidget::GetWidget( widgetID );
        if ( widget == NULL ) {
          (*res)[ i] = (DLong) 0;
        } else {
          atLeastOneFound=TRUE;
          DLong result=0;
          if (child) {
            if ( widget->NChildren( ) != 0 ) result = widget->GetChild( 0 ); //may not have children
          }
          else if (parent)  result = widget->GetParentID( ); //but parent is always defined...
          else if (parent)  result = widget->GetWidgetType( );
          ( *res )[ i] = result;
        }
      }
      if (atLeastOneFound) return res; else e->Throw("Invalid widget identifier:"+i2s((*p0L)[0]));
    }
  }

  //SYSTEM_COLORS
  if (giveSystemColors){
      WidgetIDT widgetID = (*p0L)[0];
      GDLWidget *widget = GDLWidget::GetWidget( widgetID );
      if ( widget == NULL )
        e->Throw("Invalid widget identifier:"+i2s(widgetID));
      else return widget->getSystemColours();
  }
  // GEOMETRY keyword
  if ( geometry ) {
    if ( rank == 0 ) {
      // Scalar Input
      WidgetIDT widgetID = (*p0L)[0];
      GDLWidget *widget = GDLWidget::GetWidget( widgetID );
      if ( widget == NULL )
        e->Throw("Invalid widget identifier:"+i2s(widgetID));
      else
      {
        int xs,ys;
        int xvs,yvs;
        wxSize bord;
        wxWindow* test=static_cast<wxWindow*>(widget->GetWxWidget());
        if ( test != NULL) {
          test->GetSize(&xs,&ys);
          test->GetVirtualSize(&xvs,&yvs);
          bord=test->GetWindowBorderSize();
        }
        DStructGDL* ex = new DStructGDL( "WIDGET_GEOMETRY" );
        ex->InitTag("XOFFSET",DFloatGDL(bord.x));  
        ex->InitTag("YOFFSET",DFloatGDL(bord.y)); 
        ex->InitTag("XSIZE",DFloatGDL(xs)); 
        ex->InitTag("YSIZE",DFloatGDL(ys)); 
        ex->InitTag("SCR_XSIZE",DFloatGDL(xvs)); 
        ex->InitTag("SCR_YSIZE",DFloatGDL(yvs)); 
        ex->InitTag("DRAW_XSIZE",DFloatGDL(0.0));  
        ex->InitTag("DRAW_YSIZE",DFloatGDL(0.0));  
        ex->InitTag("MARGIN",DFloatGDL(0.0));  
        ex->InitTag("XPAD",DFloatGDL(0.0));  
        ex->InitTag("YPAD",DFloatGDL(0.0));  
        ex->InitTag("SPACE",DFloatGDL(0.0));
        return ex; 
      }
    } else {
      // Array Input
      DStructDesc* dWidgeomDesc = FindInStructList( structList, "WIDGET_GEOMETRY");
      DStructGDL* ex = new DStructGDL(dWidgeomDesc, p0L->Dim( ), BaseGDL::NOZERO );
      ex->Clear();
      static unsigned tag1=ex->Desc()->TagIndex("XOFFSET");
      static unsigned tag2=ex->Desc()->TagIndex("YOFFSET");
      static unsigned tag3=ex->Desc()->TagIndex("XSIZE");
      static unsigned tag4=ex->Desc()->TagIndex("YSIZE");
      static unsigned tag5=ex->Desc()->TagIndex("SCR_XSIZE");
      static unsigned tag6=ex->Desc()->TagIndex("SCR_YSIZE");
      bool atLeastOneFound=false;
      for ( SizeT i = 0; i < nEl; i++ ) {
        WidgetIDT widgetID = (*p0L)[i];
        GDLWidget *widget = GDLWidget::GetWidget( widgetID );
        int xs,ys;
        int xvs,yvs;
        wxSize bord;
        if ( widget != NULL ) {
          atLeastOneFound=TRUE;
          static_cast<wxWindow*>(widget->GetWxWidget())->GetSize(&xs,&ys);
          static_cast<wxWindow*>(widget->GetWxWidget())->GetVirtualSize(&xvs,&yvs);
          bord=(static_cast<wxWindow*>(widget->GetWxWidget()))->GetWindowBorderSize();
          (*static_cast<DFloatGDL*>(ex->GetTag(tag1, i)))[0]=bord.x;
          (*static_cast<DFloatGDL*>(ex->GetTag(tag2, i)))[0]=bord.y;
          (*static_cast<DFloatGDL*>(ex->GetTag(tag3, i)))[0]=xs;
          (*static_cast<DFloatGDL*>(ex->GetTag(tag4, i)))[0]=ys;
          (*static_cast<DFloatGDL*>(ex->GetTag(tag5, i)))[0]=xvs;
          (*static_cast<DFloatGDL*>(ex->GetTag(tag6, i)))[0]=yvs;
//        ex->InitTag("DRAW_XSIZE",DFloatGDL(0.0));  
//        ex->InitTag("DRAW_YSIZE",DFloatGDL(0.0));  
//        ex->InitTag("MARGIN",DFloatGDL(0.0));  
//        ex->InitTag("XPAD",DFloatGDL(0.0));  
//        ex->InitTag("YPAD",DFloatGDL(0.0));  
//        ex->InitTag("SPACE",DFloatGDL(0.0));
        }
      }
      if (atLeastOneFound) return ex; else e->Throw("Invalid widget identifier:"+i2s((*p0L)[0]));
    }
  }  // End /GEOMETRY

  // MODAL keyword (stub)
  if ( modal ) {
    return new DLongGDL( 0 );
  }
  // End /MODAL

  // VALID , MANAGED, BUTTONSET etc keywords giving back 0 or 1
  if ( valid || managed || buttonset) {
    if ( rank == 0 ) {
      // Scalar Input
      WidgetIDT widgetID = (*p0L)[0];
      GDLWidget *widget = GDLWidget::GetWidget( widgetID );

      // Check if valid widgetID else exit with 0
      if ( widget == NULL ) return new DLongGDL( 0 ); //solves valid = 0 too.
      bool result=false;
      if (valid) result=( widget != NULL );
      else if (managed) result=( widget->GetManaged( ) == true );
      else if (buttonset) result=( widget->GetButtonSet() == true );
      if ( result ) return new DLongGDL( 1 ); 
      else          return new DLongGDL( 0 );
    } else {
      // Array Input
      DLongGDL* res = new DLongGDL( p0L->Dim( ), BaseGDL::NOZERO );
      bool atLeastOneFound=false;
      for ( SizeT i = 0; i < nEl; i++ ) {
        WidgetIDT widgetID = (*p0L)[i];
        GDLWidget *widget = GDLWidget::GetWidget( widgetID );

        if ( widget == NULL ) ( *res )[ i] = (DLong) 0;
        else {
          atLeastOneFound=true;
          bool result=false;
          if (valid) result=( widget != NULL );
          else if (managed) result=( widget->GetManaged( ) == true );
          else if (buttonset) result=( widget->GetButtonSet( ) == true );         
          if ( result ) ( *res )[ i] = (DLong) 1;
          else          ( *res )[ i] = (DLong) 0;
        }
      }
      if (atLeastOneFound) return res; else e->Throw("Invalid widget identifier:"+i2s((*p0L)[0]));
    }
  }
  // End VALID , MANAGED, BUTTONSET

  if (textselect) {
      // Always Scalar Input
      WidgetIDT widgetID = (*p0L)[0];
      GDLWidget *widget = GDLWidget::GetWidget( widgetID );
      if ( widget == NULL ) e->Throw("Invalid widget identifier:"+i2s(widgetID));
      return static_cast<GDLWidgetText*>(widget)->GetTextSelection();
  }


  // XMANAGER_BLOCK keyword
  if ( xmanagerBlock ) {
    return new DLongGDL( GDLWidget::GetXmanagerBlock( ) ? 1 : 0 );
  }
  
  if (tableselect||tabledisjointselection||giveColumnWidths||giveRowHeights)
  {
      WidgetIDT widgetID = (*p0L)[0];
      GDLWidget *widget = GDLWidget::GetWidget( widgetID );
      if ( widget == NULL || !widget->IsTable() ) e->Throw("Invalid widget identifier:"+i2s(widgetID));
      GDLWidgetTable *table = (GDLWidgetTable *) widget;
      
      if (tableselect) return table->GetSelection();
      if (tabledisjointselection)  return new DLongGDL( table->GetDisjointSelection()?1:0 );
      
      bool useATableSelection = e->KeywordSet(USE_TABLE_SELECT);
      DLongGDL* tableSelectionToUse = GetKeywordAs<DLongGDL>(e, USE_TABLE_SELECT);
      if (useATableSelection && tableSelectionToUse->Rank()==0 & !table->IsSomethingSelected())
        { e->Throw( "USE_TABLE_SELECT value out of range.");}
      if (useATableSelection && tableSelectionToUse->Rank()>0) { //check further a bit...
        if (table->GetDisjointSelection()) {
          if (tableSelectionToUse->Dim(0) != 2) e->Throw( "Array must have dimensions of (2, N): " + e->GetString( USE_TABLE_SELECT ) );
        } else {
          if (tableSelectionToUse->Rank() != 1 || tableSelectionToUse->Dim(0) != 4 ) e->Throw( "Array must have dimensions of (4): " + e->GetString( USE_TABLE_SELECT ) );
        }
      }
      
      if (giveColumnWidths){
        if (useATableSelection) return table->GetColumnWidth(tableSelectionToUse); else return table->GetColumnWidth();
      }
      if (giveRowHeights){
        if (useATableSelection) return table->GetRowHeight(tableSelectionToUse); else return table->GetRowHeight();
      }
  }
  
  // End /XMANAGER_BLOCK
  // if code pointer arrives here, give WIDGET_VERSION:
  // if you get here and should not, you forgot to return the value you got...
  //it is as if /version was set.
  DStructGDL* res = new DStructGDL( "WIDGET_VERSION" );
  res->InitTag("STYLE",DStringGDL(std::string(wxPlatformInfo::Get().GetPortIdName().mb_str(wxConvUTF8))));
  res->InitTag("TOOLKIT",DStringGDL("wxWidgets"));
  std::ostringstream stringStream;
  stringStream << wxPlatformInfo::Get().GetToolkitMajorVersion() <<"."<<wxPlatformInfo::Get().GetToolkitMinorVersion();
  std::string release=stringStream.str();
  res->InitTag("RELEASE",DStringGDL(release)); 
  return res;
#endif
}


// WIDGET_EVENT

BaseGDL* widget_event( EnvT* e ) {
#ifndef HAVE_LIBWXWIDGETS
  e->Throw( "GDL was compiled without support for wxWidgets" );
  return NULL;
#else
  DStructGDL* defaultRes = new DStructGDL( "WIDGET_NOEVENT" );
  static int savehourglassIx = e->KeywordIx( "SAVE_HOURGLASS" );
  bool savehourglass = e->KeywordSet( savehourglassIx );
  // it is said in the doc: 1) that WIDGET_CONTROL,/HOURGLASS busyCursor ends at the first WIDGET_EVENT processed. 
  // And 2) that /SAVE_HOURGLASS exist to prevent just that, ending.
  if ( !savehourglass ) wxEndBusyCursor( );
  //xmanager_block (not a *DL standard) is used to block until TLB is killed
  static int xmanagerBlockIx = e->KeywordIx( "XMANAGER_BLOCK" );
  bool xmanagerBlock = e->KeywordSet( xmanagerBlockIx );
  static int nowaitIx = e->KeywordIx( "NOWAIT" );
  bool nowait = e->KeywordSet( nowaitIx );
  static int badidIx = e->KeywordIx( "BAD_ID" );
  bool dobadid = e->KeywordPresent( badidIx );
  if (dobadid) e->AssureGlobalKW(badidIx);
  
  SizeT nParam = e->NParam( );
  std::vector<WidgetIDT> widgetID;
  DLongGDL* p0L = NULL;
  SizeT nEl = 0;
  SizeT rank = 0;

  bool all = true;
  if ( nParam > 0 ) {
    p0L = e->GetParAs<DLongGDL>(0);
    all = false;
    nEl = p0L->N_Elements( );
    for ( SizeT i = 0; i < nEl; i++ ) {
      GDLWidget *widget = GDLWidget::GetWidget( (*p0L)[i] );
      if ( widget == NULL ) {
        if (dobadid) { 
          e->SetKW( badidIx, new DLongGDL( (*p0L)[i]  ) );
          return defaultRes; //important!!!
        } else { e->Throw( "Invalid widget identifier:" + i2s( (*p0L)[i] ) ); }
      }
      widgetID.push_back( (*p0L)[i] );
    }
  }

  if (dobadid) e->SetKW( badidIx, new DLongGDL(0) ); //if id is OK, but BAD_ID was given, we must return 0 in BAD_ID.

  EnvBaseT* caller;

  DLong id;
  DLong tlb;
  int infinity = 1;
  while ( infinity ) { // outer while loop, will run once if NOWAIT
    if ( nowait ) {
      infinity = 0;
//      std::cout << "WIDGET_EVENT: Polling event queue only once (/NOWAIT) ..." << std::endl;
    } // else std::cout << "WIDGET_EVENT: Polling event queue ..." << std::endl;

    DStructGDL* ev = NULL;

    if ( !all ) { //specific widget(s)
      while ( 1 ) {
        //note: when a widgetId is passed, all the other events in IDL block until the good one is found (or ^C).
        //Apparently this behaviour is not dependent on GetXmanagerActiveCommand( ) status, so I check both eventLists.
        if ( (ev = GDLWidget::eventQueue.Pop( )) != NULL ) {
          static int idIx = ev->Desc( )->TagIndex( "ID" ); // examine it
          id = (*static_cast<DLongGDL*> (ev->GetTag( idIx, 0 )))[0];

          for ( SizeT i = 0; i < widgetID.size( ); i++ ) {
            if ( widgetID.at( i ) == id ) {
              static int topIx = ev->Desc( )->TagIndex( "TOP" ); // 1
              static int handlerIx = ev->Desc( )->TagIndex( "HANDLER" ); // 2
              tlb = (*static_cast<DLongGDL*> (ev->GetTag( topIx, 0 )))[0];
              return ev ;
            } 
          }
        } else if ( (ev = GDLWidget::readlineEventQueue.Pop( )) != NULL ) {
          static int idIx = ev->Desc( )->TagIndex( "ID" ); // examine it
          id = (*static_cast<DLongGDL*> (ev->GetTag( idIx, 0 )))[0];

          for ( SizeT i = 0; i < widgetID.size( ); i++ ) {
            if ( widgetID.at( i ) == id ) {
              static int topIx = ev->Desc( )->TagIndex( "TOP" ); // 1
              static int handlerIx = ev->Desc( )->TagIndex( "HANDLER" ); // 2
              tlb = (*static_cast<DLongGDL*> (ev->GetTag( topIx, 0 )))[0];
              return ev ;
            } 
          }
        }
          
        wxMilliSleep( 50 );      // Sleep a bit to prevent CPU overuse
        
        if ( sigControlC )
          return defaultRes;//new DLongGDL( 0 );
      }
    } else { //wait for ALL (and /XMANAGER_BLOCK for example) 
      while ( 1 ) {
        // handle global GUI events as well as plot events
        // handling is completed on return
        // calls GDLWidget::HandleEvents()
        // which calls GDLWidget::CallEventHandler()
        GDLEventHandler( );

        // the polling event handler
        if ( (ev = GDLWidget::eventQueue.Pop( )) != NULL ) {
          // ev = GDLWidget::eventQueue.Pop();
          static int idIx = ev->Desc( )->TagIndex( "ID" ); // 0
          static int topIx = ev->Desc( )->TagIndex( "TOP" ); // 1
          static int handlerIx = ev->Desc( )->TagIndex( "HANDLER" ); // 2

          id = (*static_cast<DLongGDL*> (ev->GetTag( idIx, 0 )))[0];
          tlb = (*static_cast<DLongGDL*> (ev->GetTag( topIx, 0 )))[0];
          break;
        } 
        // if poll event handler found an event this is not reached due to the
        // 'break' statement
        wxMilliSleep( 50 );      // Sleep a bit to prevent CPU overuse

        if ( sigControlC )
          return defaultRes; //new DLongGDL( 0 );

      } // inner while
    } //ALL

    ev = CallEventHandler( /*id,*/ ev ); //will block waiting for XMANAGER 
    if ( ev != NULL ) {
      Warning( "WIDGET_EVENT: No event handler found. ID: " + i2s( id ) );
      GDLDelete( ev );
      ev = NULL;
    }
    GDLWidget *tlw = GDLWidget::GetWidget( tlb );
    if ( tlw == NULL ) { //possible if a kill_notify procedure was called 
//      std::cout << "WIDGET_EVENT: widget no longer valid." << std::endl;  //no use to report, then.
      break;
    }
  } // outer while loop

  return defaultRes; //new DLongGDL( 0 );
#endif
}

void widget_control( EnvT* e ) {
#ifndef HAVE_LIBWXWIDGETS
  e->Throw( "GDL was compiled without support for wxWidgets" );
#else
  static int hourglassIx =  e->KeywordIx( "HOURGLASS" );
  bool sethourglass = e->KeywordPresent( hourglassIx );  
  
  if (sethourglass){ //Ignore it for the moment!
    if (e->KeywordSet( hourglassIx )) wxBeginBusyCursor();
    else wxEndBusyCursor();
    return;} //need to return immediately if /HOURGLASS!

  static int showIx = e->KeywordIx( "SHOW" );
  bool show = e->KeywordPresent( showIx );
  
  static int realizeIx = e->KeywordIx( "REALIZE" );
  bool realize = e->KeywordSet( realizeIx );

  static int sensitiveControlIx = e->KeywordIx( "SENSITIVE" );
  bool sensitiveControl = e->KeywordPresent( sensitiveControlIx );
  
  static int inputfocusIx = e->KeywordIx( "INPUT_FOCUS" );
  bool inputfocus = e->KeywordSet( inputfocusIx );

  static int managedIx = e->KeywordIx( "MANAGED" );
  bool managed = e->KeywordSet( managedIx );

  static int mapIx = e->KeywordIx( "MAP" );
  bool do_map=e->KeywordPresent( mapIx );

  static int xmanActComIx = e->KeywordIx( "XMANAGER_ACTIVE_COMMAND" ); //set by xmanager.pro when /NO_BLOCK is given.
  bool xmanActCom = e->KeywordSet( xmanActComIx );

  static int destroyIx = e->KeywordIx( "DESTROY" );
  static int delay_destroyIx = e->KeywordIx( "DELAY_DESTROY" );
  bool destroy = (e->KeywordSet( destroyIx ) || e->KeywordSet( delay_destroyIx )); //silently ignore 'delay' and maintain 'destroy'.
  
  static int timerIx = e->KeywordIx( "TIMER" );
  bool doTimer = e->KeywordPresent( timerIx );

  DString eventPro = "";
  static int eventproIx = e->KeywordIx( "EVENT_PRO" );
  bool eventpro = e->KeywordSet( eventproIx );

  DString eventFun = "";
  static int eventfunIx = e->KeywordIx( "EVENT_FUNC" );
  bool eventfun = e->KeywordSet( eventfunIx );

  DString killNotifyFunName = "";
  static int killnotifyIx = e->KeywordIx( "KILL_NOTIFY" );
  bool killnotify = e->KeywordSet( killnotifyIx );

  DString notifyRealizeFunName = "";
  static int notifyrealizeIx = e->KeywordIx( "NOTIFY_REALIZE" );
  bool notifyrealize = e->KeywordSet(notifyrealizeIx);

  static int getuvalueIx = e->KeywordIx( "GET_UVALUE" );
  bool getuvalue = e->KeywordPresent( getuvalueIx );

  static int setuvalueIx = e->KeywordIx( "SET_UVALUE" );
  bool setuvalue = e->KeywordPresent( setuvalueIx );

  static int setvalueIx = e->KeywordIx( "SET_VALUE" );
  bool setvalue = e->KeywordPresent( setvalueIx );
  
  static int settextselectIx  = e->KeywordIx( "SET_TEXT_SELECT" );
  bool settextselect = e->KeywordPresent( settextselectIx ); 

  static int getvalueIx = e->KeywordIx( "GET_VALUE" );
  bool getvalue = e->KeywordPresent( getvalueIx );

  static int setunameIx = e->KeywordIx( "SET_UNAME" );
  bool setuname = e->KeywordPresent( setunameIx );
  DString uname = "";
  e->AssureStringScalarKWIfPresent( setunameIx, uname );

  static int prosetvalueIx = e->KeywordIx( "PRO_SET_VALUE" );
  bool prosetvalue = e->KeywordPresent( prosetvalueIx );
  DString setvaluepro = "";
  e->AssureStringScalarKWIfPresent( prosetvalueIx, setvaluepro );

  static int funcgetvalueIx = e->KeywordIx( "FUNC_GET_VALUE" );
  bool funcgetvalue = e->KeywordPresent( funcgetvalueIx );
  DString setvaluefunc = "";
  e->AssureStringScalarKWIfPresent( funcgetvalueIx, setvaluefunc );

  static int setbuttonIx = e->KeywordIx( "SET_BUTTON" );
  bool setbutton = e->KeywordPresent( setbuttonIx );

  static int bitmapIx = e->KeywordIx( "BITMAP" );
  bool isBitmap =  e->KeywordSet( bitmapIx );

  static int tlbgetsizeIx =  e->KeywordIx( "TLB_GET_SIZE" );
  bool givetlbsize = e->KeywordPresent( tlbgetsizeIx );
  static int tlbsettitleIx =  e->KeywordIx( "TLB_SET_TITLE" );
  bool settlbtitle = e->KeywordPresent( tlbsettitleIx );  

  static int SEND_EVENT = e->KeywordIx( "SEND_EVENT" );
  static int CLEAR_EVENTS = e->KeywordIx( "CLEAR_EVENTS" );
//  static int PUSHBUTTON_EVENTS = e->KeywordIx( "PUSHBUTTON_EVENTS" );
  static int TRACKING_EVENTS = e->KeywordIx( "TRACKING_EVENTS" );
  static int DRAW_BUTTON_EVENTS = e->KeywordIx( "DRAW_BUTTON_EVENTS" );
  static int DRAW_EXPOSE_EVENTS = e->KeywordIx( "DRAW_EXPOSE_EVENTS" );
  static int DRAW_KEYBOARD_EVENTS = e->KeywordIx( "DRAW_KEYBOARD_EVENTS" );
  static int DRAW_MOTION_EVENTS = e->KeywordIx( "DRAW_MOTION_EVENTS" );
  static int DRAW_VIEWPORT_EVENTS = e->KeywordIx( "DRAW_VIEWPORT_EVENTS" );
  static int DRAW_WHEEL_EVENTS = e->KeywordIx( "DRAW_WHEEL_EVENTS" );
  static int SET_DROP_EVENTS = e->KeywordIx( "SET_DROP_EVENTS" );
  static int TLB_KILL_REQUEST_EVENTS = e->KeywordIx( "TLB_KILL_REQUEST_EVENTS" );
  static int TLB_MOVE_EVENTS = e->KeywordIx( "TLB_MOVE_EVENTS" );
  static int TLB_SIZE_EVENTS = e->KeywordIx( "TLB_SIZE_EVENTS" );
  static int KBRD_FOCUS_EVENTS = e->KeywordIx( "KBRD_FOCUS_EVENTS" );
  static int CONTEXT_EVENTS = e->KeywordIx( "CONTEXT_EVENTS" );
  static int TLB_ICONIFY_EVENTS = e->KeywordIx( "TLB_ICONIFY_EVENTS" );
  static int ALL_TABLE_EVENTS = e->KeywordIx( "ALL_TABLE_EVENTS" );
  static int ALL_TEXT_EVENTS = e->KeywordIx( "ALL_TEXT_EVENTS" );

  bool send_event = e->KeywordPresent( SEND_EVENT );
  bool clear_events = e->KeywordSet( CLEAR_EVENTS );
  bool tracking_events = e->KeywordPresent( TRACKING_EVENTS );

  bool draw_button_events = e->KeywordPresent( DRAW_BUTTON_EVENTS );
// not supported (spacebar handling of buttons...)
//  bool pushbutton_events = e->KeywordPresent( PUSHBUTTON_EVENTS );
  bool draw_expose_events = e->KeywordPresent( DRAW_EXPOSE_EVENTS );
  bool draw_keyboard_events = e->KeywordPresent( DRAW_KEYBOARD_EVENTS );
  bool draw_motion_events = e->KeywordPresent( DRAW_MOTION_EVENTS );
  bool draw_wheel_events = e->KeywordPresent( DRAW_WHEEL_EVENTS );
  bool draw_viewport_events = e->KeywordPresent( DRAW_VIEWPORT_EVENTS );
  bool drop_events = e->KeywordPresent( SET_DROP_EVENTS );
  bool tlb_kill_request_events = e->KeywordPresent( TLB_KILL_REQUEST_EVENTS );
  bool tlb_move_events = e->KeywordPresent( TLB_MOVE_EVENTS );
  bool tlb_size_events = e->KeywordPresent( TLB_SIZE_EVENTS );
  bool tlb_iconify_events = e->KeywordPresent( TLB_ICONIFY_EVENTS );
  bool kbrdfocusevents = e->KeywordPresent( KBRD_FOCUS_EVENTS );
  bool contextevents = e->KeywordPresent( CONTEXT_EVENTS );
  bool all_table_events = e->KeywordPresent( ALL_TABLE_EVENTS );
  bool all_text_events = e->KeywordPresent( ALL_TEXT_EVENTS );
  
  static int SCR_XSIZE = e->KeywordIx( "SCR_XSIZE" );
  static int SCR_YSIZE= e->KeywordIx( "SCR_YSIZE" );
  static int XSIZE = e->KeywordIx( "XSIZE" );
  static int YSIZE = e->KeywordIx( "YSIZE" );
  static int DRAW_XSIZE = e->KeywordIx( "DRAW_XSIZE" );
  static int DRAW_YSIZE = e->KeywordIx( "DRAW_YSIZE" );
  
  bool hasScr_xsize  = e->KeywordPresent( SCR_XSIZE );
  bool hasScr_ysize  = e->KeywordPresent( SCR_YSIZE );
  bool hasXsize      = e->KeywordPresent( XSIZE );
  bool hasYsize      = e->KeywordPresent( YSIZE );
  bool hasDraw_xsize = e->KeywordPresent( DRAW_XSIZE );
  bool hasDraw_ysize = e->KeywordPresent( DRAW_YSIZE );

  static int UPDATE = e->KeywordIx( "UPDATE" );
  bool update=e->KeywordPresent(UPDATE);
  
  static int badidIx = e->KeywordIx( "BAD_ID" );
  bool dobadid = e->KeywordPresent( badidIx );
  if (dobadid) e->AssureGlobalKW(badidIx);

  DLongGDL* p0L = e->GetParAs<DLongGDL>(0);

  WidgetIDT widgetID = (*p0L)[0];
  GDLWidget *widget = GDLWidget::GetWidget( widgetID );
  if ( widget == NULL ) {
    if ( dobadid ) {
      BaseGDL** badidKW = &e->GetKW( badidIx );
      GDLDelete( (*badidKW) );
      *badidKW=new DLongGDL( widgetID );
      return;
    } else {
      e->Throw( "Widget ID not valid: " + i2s( widgetID ) );
    }
  }

  // start with set/ or get/value
  // this insures that all widgets, i.e. including invalid widgets (in the process of being deleted)
  // still respond to set or get values.
  if ( setvalue ) {
    DString wType = widget->GetWidgetName( );
    BaseGDL* value = e->GetKW( setvalueIx );
    if ( value != NULL ) value = value->Dup( );
//process eventual PRO_SETVALUE call
    DString setProName = widget->GetProValue( );
    if ( setProName != "" ) CallEventPro( setProName, p0L->Dup( ), value );
    if ( wType == "TEXT" ) {
      static int noNewLineIx = e->KeywordIx( "NO_NEWLINE" );
      bool noNewLine = e->KeywordSet( noNewLineIx );
      static int usetextselectIx  = e->KeywordIx( "USE_TEXT_SELECT" );
      bool usetextselect = e->KeywordPresent( usetextselectIx ); 
      static int appendvalueIx  = e->KeywordIx( "APPEND" );
      bool appendvalue = e->KeywordPresent( appendvalueIx );
  
      BaseGDL* valueKW = e->GetKW( setvalueIx );
      DStringGDL* valueStr = NULL;
      if ( valueKW != NULL ) {
        if ( valueKW->Type( ) != GDL_STRING )
          e->Throw( "VALUE must be a STRING for WIDGET_TEXT." );
        valueStr = static_cast<DStringGDL*> (valueKW);
        bool success = e->StealLocalKW( setvalueIx );
        if ( !success )
          valueStr = valueStr->Dup( );

        GDLWidgetText *textWidget = (GDLWidgetText *) widget;
        if (appendvalue) textWidget->InsertText(valueStr, noNewLine, TRUE); //aka insertAtEnd
        else if (usetextselect) textWidget->InsertText( valueStr, noNewLine);
        else textWidget->ChangeText( valueStr, noNewLine);
      }
    } else if ( wType == "LABEL" ) {
      DString value = "";
      e->AssureStringScalarKWIfPresent( setvalueIx, value );
	//	std::cout << "setlabelvalue: " << value.c_str() << std::endl;
      GDLWidgetLabel *labelWidget = (GDLWidgetLabel *) widget;
      labelWidget->SetLabelValue( value );
    } else if ( wType == "COMBOBOX" ) {
    } else if ( wType == "LIST" ) {
    } else if ( wType == "DROPLIST" ) {
    } else if ( wType == "BUTTON" ) {
      DString value = "";
      wxBitmap * bitmap=NULL;
      //value=filename if /BITMAP present, bitmap if array.
      BaseGDL* valueKW = e->GetKW( setvalueIx );
      if (isBitmap) {
        e->AssureStringScalarKWIfPresent( setvalueIx, value ); //value is a filename
        //try loading file
        wxInitAllImageHandlers();
        wxImage * tryImage=new wxImage(wxString(value.c_str(),wxConvUTF8),wxBITMAP_TYPE_JPEG);
        if (tryImage->IsOk()) bitmap = new wxBitmap(*tryImage);
        else e->Throw( "Unable to read image file: " + value );
        value.clear();
        GDLWidgetButton *bb = (GDLWidgetButton *) widget;
        bb->SetButtonWidgetBitmap( bitmap );
      } else if (valueKW->Type()==GDL_STRING) {
        e->AssureStringScalarKWIfPresent( setvalueIx, value );
        GDLWidgetButton *bb = (GDLWidgetButton *) widget;
        bb->SetButtonWidgetLabelText( value );
      } else {
        DByteGDL* testByte=e->GetKWAs<DByteGDL>(setvalueIx);
        if (testByte) { //must be n x m or n x m x 3
          if (testByte->Rank() < 2 || testByte->Rank() > 3) e->Throw( "Array must be a [X,Y] or [X,Y,3] array." );
          if (testByte->Rank() == 3 && testByte->Dim(2) != 3) e->Throw( "Array must be a [X,Y] or [X,Y,3] array." );
          if (testByte->Rank() == 2) {
            bitmap = new wxBitmap(static_cast<char*>(testByte->DataAddr()),testByte->Dim(0)*8,testByte->Dim(1),1);
          } else {
            BaseGDL* transpose=testByte->Transpose(NULL);
            wxImage * tryImage=new wxImage(transpose->Dim(1),transpose->Dim(2),static_cast<unsigned char*>(transpose->DataAddr()),TRUE); //STATIC DATA I BELIEVE.
            GDLDelete( transpose );
            bitmap = new wxBitmap(*tryImage);
          }
          GDLWidgetButton *bb = (GDLWidgetButton *) widget;
          bb->SetButtonWidgetBitmap( bitmap );
        } else  e->Throw( "Unsupported VALUE Keyword type, please report!" );
      }

    } else if (widget->IsTable( )) {
        GDLWidgetTable *table = (GDLWidgetTable *) widget;
        static int USE_TABLE_SELECT = e->KeywordIx("USE_TABLE_SELECT");
        bool useATableSelection = e->KeywordSet(USE_TABLE_SELECT);
        DLongGDL* tableSelectionToUse = GetKeywordAs<DLongGDL>(e, USE_TABLE_SELECT);

        if (useATableSelection && tableSelectionToUse->Rank()>0) { //check further a bit...
          if (table->GetDisjointSelection()) {
            if (tableSelectionToUse->Dim(0) != 2) e->Throw( "Array must have dimensions of (2, N): " + e->GetString( USE_TABLE_SELECT ) );
          } else {
            if (tableSelectionToUse->Rank() != 1 || tableSelectionToUse->Dim(0) != 4 ) e->Throw( "Array must have dimensions of (4): " + e->GetString( USE_TABLE_SELECT ) );
          }
        }
        DStringGDL* valueAsStrings;
        static int FORMAT = e->KeywordIx( "FORMAT" );
        DStringGDL* format = GetKeywordAs<DStringGDL>(e, FORMAT);
        //test of non-conformity
        if (useATableSelection && table->GetDisjointSelection()) { 
          //everything works in this case
        } else if (useATableSelection && !table->GetDisjointSelection()) {
          if (value && value->Rank() > 2 ) e->Throw("Value has greater than 2 dimensions.");
        } else {
          if (value && value->Rank() > 2 ) e->Throw("Value has greater than 2 dimensions.");
          else if (value && value->Rank() < 1 ) e->Throw("Expression must be an array in this context: "+e->GetString( setvalueIx ));
        }
        
        if (value->Type()==GDL_STRING) {valueAsStrings=static_cast<DStringGDL*>(value->Dup());} 
        else if (value->Type()==GDL_STRUCT) { //FIXME we support only complete replacement with structures. Note that IDL crashes in many of those cases! 
          if (useATableSelection) e->Throw("Table Structure replacement with USE_TABLE_SELECT is NOT IMPLEMENTED, Sorry!");
          if (value->Rank() > 1) e->Throw("Multi dimensional arrays of structures not allowed.");
          //convert to STRING
          DStructGDL *input=static_cast<DStructGDL*>(value);
          SizeT nTags = input->NTags();
          //further check:
          for (SizeT iTag=0; iTag<nTags; ++iTag) {
            BaseGDL* tested=input->GetTag(iTag);
            if (tested->Rank() > 0 || tested->Type()==GDL_STRUCT) e->Throw("Structures cannot include arrays or other structures.");
          }
          SizeT nEl   = input->N_Elements();
          SizeT dims[2];
          if (table->GetMajority() == GDLWidgetTable::COLUMN_MAJOR ) {
            dims[0] = nEl;
            dims[1] = nTags;
          } else {
            dims[1] = nEl;
            dims[0] = nTags;      
          }
          dimension dim(dims, 2); 
          valueAsStrings=new DStringGDL ( dim );
          stringstream os;
          input->ToStreamRaw(os);
          valueAsStrings->FromStream(os); //simple as that if we manage the dimensions and transpose accordingly....
          if ( table->GetMajority() ==  GDLWidgetTable::ROW_MAJOR ) {
            valueAsStrings=static_cast<DStringGDL*>(valueAsStrings->Transpose(NULL));
          //transpose back sizes only...
            SizeT dims[2];
            dims[1] = nEl;
            dims[0] = nTags;      
            dimension dim(dims, 2); 
            (static_cast<BaseGDL*>(valueAsStrings))->SetDim(dim);
          }
        } else {
          //convert to STRING using FORMAT.
          static int stringIx = LibFunIx("STRING");
          assert( stringIx >= 0);
          EnvT* newEnv= new EnvT(e, libFunList[stringIx], NULL);
          Guard<EnvT> guard( newEnv);
          // add parameters
          newEnv->SetNextPar( value->Dup());
          if (format) newEnv->SetNextPar( format->Dup() );
          // make the call
          valueAsStrings = static_cast<DStringGDL*>(static_cast<DLibFun*>(newEnv->GetPro())->Fun()(newEnv));
          //give back invalue's rank and dims to value, they have been lost in the process
          (static_cast<BaseGDL*>(valueAsStrings))->SetDim(value->Dim());
        }

        if (useATableSelection) table->SetTableValues(valueAsStrings,tableSelectionToUse); else {
//value replaces completely old value, even value type may be changed
          table->SetValue(value);
          table->SetTableValues(valueAsStrings);
        }
      }
  } //end SetValue

  if ( getvalue ) {

    BaseGDL** valueKW = &e->GetKW( getvalueIx );
    GDLDelete( (*valueKW) );

    DString getFuncName = widget->GetFuncValue( );
    if ( !(getFuncName.empty()) ) {
      StackGuard<EnvStackT> guard( e->Interpreter( )->CallStack( ) );

      DString callF = StrUpCase( getFuncName );
//      cerr<<"calling funcname="<<callF<<endl;

      SizeT funIx = GDLInterpreter::GetFunIx( callF );
      EnvUDT* newEnv = new EnvUDT( e->CallingNode( ), funList[ funIx], (DObjGDL**) NULL );

      // add parameter
      newEnv->SetNextPar( new DLongGDL( widgetID ) ); // pass as local
      e->Interpreter( )->CallStack( ).push_back( newEnv );

      // make the call
      BaseGDL* res = new BaseGDL;
      res = e->Interpreter( )->call_fun( static_cast<DSubUD*> (newEnv->GetPro( ))->GetTree( ) );

      // set the keyword to the function's return value which can be anything!!!
      *valueKW = res;
    } else {
      if ( widget->IsText( ) || widget->IsDropList( ) || widget->IsComboBox() ) {
        string rawValue;
        if ( widget->IsText( ) ) {rawValue = static_cast<GDLWidgetText*> (widget)->GetLastValue( );}
        else if ( widget->IsComboBox( ) )
        {rawValue = static_cast<GDLWidgetComboBox*> (widget)->GetLastValue( );}
        else //Droplist
        {
          assert( widget->IsDropList());
          rawValue = static_cast<GDLWidgetDropList*> (widget)->GetLastValue( );
        }
	  if( rawValue.length() == 0)
	  {
        *valueKW = new DStringGDL( dimension( 1 ) );
	  }
	  else
	  {
        vector<DString> strArr;
        strArr.reserve( rawValue.length( ) );
        string actStr = "";
	    for( int i=0; i<rawValue.length(); ++i)
	    {
          if ( rawValue[i] != '\n' ) actStr += rawValue[i];
		  else
          {
            strArr.push_back( actStr );
            actStr.clear( );
          }
        }
        strArr.push_back( actStr ); //was missing!!
        if (strArr.size() > 0) {
          DStringGDL* valueStr = new DStringGDL( dimension(strArr.size()));
          for( int i=0; i<strArr.size(); ++i)
          {
            (*valueStr)[i] = strArr[i];
          }
            *valueKW = valueStr;
          } else *valueKW = new DStringGDL( dimension( 1 ) );
        }           
      } else if (widget->IsTable( )) {
        GDLWidgetTable *table = (GDLWidgetTable *) widget;
        static int USE_TABLE_SELECT = e->KeywordIx("USE_TABLE_SELECT");
        bool useATableSelection = e->KeywordSet(USE_TABLE_SELECT);
        DLongGDL* tableSelectionToUse = GetKeywordAs<DLongGDL>(e, USE_TABLE_SELECT);

        if (useATableSelection && tableSelectionToUse->Rank()==0 & !table->IsSomethingSelected())
          { e->Throw( "USE_TABLE_SELECT value out of range.");}
        if (useATableSelection && tableSelectionToUse->Rank()>0) { //check further a bit...
          if (table->GetDisjointSelection()) {
            if (tableSelectionToUse->Dim(0) != 2) e->Throw( "Array must have dimensions of (2, N): " + e->GetString( USE_TABLE_SELECT ) );
          } else {
            if (tableSelectionToUse->Rank() != 1 || tableSelectionToUse->Dim(0) != 4 ) e->Throw( "Array must have dimensions of (4): " + e->GetString( USE_TABLE_SELECT ) );
          }
        }
        
        DStringGDL *retval;
        if (useATableSelection) retval=table->GetTableValues(tableSelectionToUse); else retval=table->GetTableValues();
        if (retval == NULL) e->Throw("USE_TABLE_SELECT value out of range.");
        else if (table->GetVvalue() == NULL) {e->Throw(" Class of specified widget has no value: 1");} //Just as IDL does!
        else if (table->GetVvalue()->Type()==GDL_STRING) { *valueKW = retval->Dup();} 
        else if (table->GetVvalue()->Type()==GDL_STRUCT) {
          BaseGDL* val;
          //use a special case handling transpositions due to column or row majority.
          if (useATableSelection) val=table->GetTableValuesAsStruct(tableSelectionToUse); else val=table->GetTableValuesAsStruct();
          if (val == NULL) e->Throw("USE_TABLE_SELECT value out of range."); //superfluous.
        *valueKW = val->Dup();
        } 
        else {
          BaseGDL* val;
          switch(table->GetVvalue()->Type()){
            case GDL_BYTE:
              val=new DByteGDL(retval->Dim());
              break;
            case GDL_INT: 
              val=new DIntGDL(retval->Dim());
              break;
            case GDL_LONG:
              val=new DLongGDL(retval->Dim());
              break;
            case GDL_FLOAT:
              val=new DFloatGDL(retval->Dim());
              break;
            case GDL_DOUBLE:
              val=new DDoubleGDL(retval->Dim());
              break;
            case GDL_COMPLEX:
              val=new DComplexGDL(retval->Dim());
              break;
            case GDL_COMPLEXDBL:
              val=new DComplexDblGDL(retval->Dim());
              break;
            case GDL_UINT:
              val=new DUIntGDL(retval->Dim());
              break;
            case GDL_ULONG:
              val=new DULongGDL(retval->Dim());
              break;
            case GDL_LONG64:
              val=new DLong64GDL(retval->Dim());
              break;
            case GDL_ULONG64:
              val=new DULong64GDL(retval->Dim());
              break;
          }
          stringstream is;
          for( SizeT i = 0; i < val->N_Elements(); i++)  is << (*retval)[ i] << '\n';
          val->FromStream( is);
          *valueKW = val->Dup();
        }
      } else {
         BaseGDL *widval = widget->GetVvalue();
        if ( widval != NULL) { *valueKW = widval->Dup();}
      }
    }
  } //end getValue
  
  //at that point, invalid widgets will not respond to widget_control.
  if (!widget->IsValid()) return;
  
  if (send_event){
    BaseGDL* event = e->GetKW(SEND_EVENT)->Dup();
    if (event && event->Type()!=GDL_STRUCT) e->Throw("Expression must be a structure in this context: "+e->GetString( SEND_EVENT ));
          DStructGDL* ev = static_cast<DStructGDL*>(event);
          SizeT id1=ev->Desc( )->TagIndex( "ID" );
          SizeT id2=ev->Desc( )->TagIndex( "TOP" );
          SizeT id3=ev->Desc( )->TagIndex( "HANDLER" );
          if (  id1  == -1 || id2 == -1 || id3 == -1 ) e->Throw("Invalid SEND_EVENT value.");
          BaseGDL* val1=ev->GetTag(id1);
          BaseGDL* val2=ev->GetTag(id2);
          BaseGDL* val3=ev->GetTag(id3);
          if ( ( val1->Type() != GDL_LONG ) || ( val2->Type() != GDL_LONG ) || ( val3->Type() != GDL_LONG ) ) {
            e->Throw("Invalid SEND_EVENT value.");
          }
          DLongGDL* lval1=static_cast<DLongGDL*>(val1);
          DLongGDL* lval2=static_cast<DLongGDL*>(val2);
          DLongGDL* lval3=static_cast<DLongGDL*>(val3);
          WidgetIDT baseWidgetID = widget->GetBase(widgetID); 
          if ((*lval1)[0]==0) (*lval1)[0]=widgetID; 
          if ((*lval2)[0]==0) (*lval2)[0]=baseWidgetID; 
          if ((*lval3)[0]==0) (*lval3)[0]=baseWidgetID;
          GDLWidget::PushEvent( baseWidgetID, ev);
  }
  
  if (hasXsize || hasYsize || hasScr_xsize || hasScr_ysize ) {
    DLong xs,ys,xsize, ysize;
    static_cast<wxWindow*>(widget->GetWxWidget())->GetSize(&xs,&ys);
    xsize=xs;
    ysize=ys;
    if (hasXsize || hasScr_xsize ) {
      if (hasScr_xsize) xsize= (*e->GetKWAs<DLongGDL>(SCR_XSIZE))[0]; 
      else if (hasXsize) xsize= (*e->GetKWAs<DLongGDL>(XSIZE))[0];
      if (xsize < 0) xsize=xs; //0 means: stretch for base widgets
    }
    if (hasYsize || hasScr_ysize ) {
      if (hasScr_ysize) ysize= (*e->GetKWAs<DLongGDL>(SCR_YSIZE))[0];
      else if (hasYsize) ysize= (*e->GetKWAs<DLongGDL>(YSIZE))[0];
      if (ysize < 0) ysize=ys; //0 means:stretch for base widgets
    }
    widget->SetSize(xsize,ysize);
  }
  static int FRAME = e->KeywordIx( "FRAME" );
  bool frame=e->KeywordPresent( FRAME );
  if (frame) if (e->KeywordSet( FRAME)) widget->FrameWidget(); else widget->UnFrameWidget();
  static int SCROLL = e->KeywordIx( "SCROLL" );
  bool scroll=e->KeywordPresent( SCROLL );
  if (scroll) if (e->KeywordSet( SCROLL)) widget->ScrollWidget(120,120); else widget->UnScrollWidget();

  if (clear_events) { 
    GDLWidgetBase * w=widget->GetBaseWidget(widgetID);
    if (w!=NULL) w->ClearEvents();
  }

  if (tlb_kill_request_events) {
    if (e->KeywordSet(TLB_KILL_REQUEST_EVENTS)) widget->AddEventType(GDLWidget::EV_KILL);
    else widget->RemoveEventType(GDLWidget::EV_KILL);
  }

  if (tlb_move_events && widget->IsBase()) {
    if (e->KeywordSet(TLB_MOVE_EVENTS)) widget->AddEventType(GDLWidget::EV_MOVE);
    else widget->RemoveEventType(GDLWidget::EV_MOVE);
  }

  if (tlb_size_events && widget->IsBase()) {
    if (e->KeywordSet(TLB_SIZE_EVENTS)) widget->AddEventType(GDLWidget::EV_SIZE);
    else widget->RemoveEventType(GDLWidget::EV_SIZE);
  }

  if (tlb_iconify_events && widget->IsBase()) {
    if (e->KeywordSet(TLB_ICONIFY_EVENTS)) widget->AddEventType(GDLWidget::EV_ICONIFY);
    else widget->RemoveEventType(GDLWidget::EV_ICONIFY);
  }

  if (kbrdfocusevents && widget->IsBase()||widget->IsTable()||widget->IsText()) {
    if (e->KeywordSet(KBRD_FOCUS_EVENTS)) widget->AddEventType(GDLWidget::EV_KBRD_FOCUS);
    else widget->RemoveEventType(GDLWidget::EV_KBRD_FOCUS);
  }
  
  if (contextevents && widget->IsBase()||widget->IsList()||widget->IsTable()||widget->IsText()||widget->IsTree()) {
    if (e->KeywordSet(CONTEXT_EVENTS)) widget->AddEventType(GDLWidget::EV_CONTEXT);
    else widget->RemoveEventType(GDLWidget::EV_CONTEXT);
  }

  if (drop_events && widget->IsDraw()||widget->IsTree() ) { 
    if (e->KeywordSet(SET_DROP_EVENTS)) widget->AddEventType(GDLWidget::EV_DROP);
    else widget->RemoveEventType(GDLWidget::EV_DROP);
  }
  if (draw_motion_events && widget->IsDraw()) {
    if (e->KeywordSet(DRAW_MOTION_EVENTS)) widget->AddEventType(GDLWidget::EV_MOTION);
    else widget->RemoveEventType(GDLWidget::EV_MOTION);
  }
  if (draw_expose_events && widget->IsDraw()) {
    if (e->KeywordSet(DRAW_EXPOSE_EVENTS)) widget->AddEventType(GDLWidget::EV_EXPOSE);
    else widget->RemoveEventType(GDLWidget::EV_EXPOSE);
  }
  if (tracking_events) {
    if (e->KeywordSet(TRACKING_EVENTS))  widget->AddEventType(GDLWidget::EV_TRACKING);
    else widget->RemoveEventType(GDLWidget::EV_TRACKING);
  }
   if (draw_viewport_events && widget->IsDraw())  {
    if (e->KeywordSet(DRAW_VIEWPORT_EVENTS))  widget->AddEventType(GDLWidget::EV_VIEWPORT);
    else widget->RemoveEventType(GDLWidget::EV_VIEWPORT);
  }
  if (draw_wheel_events && widget->IsDraw())  {
    if (e->KeywordSet(DRAW_WHEEL_EVENTS))   widget->AddEventType(GDLWidget::EV_WHEEL);
    else widget->RemoveEventType(GDLWidget::EV_WHEEL);
  }
  if (draw_button_events && widget->IsDraw())  {
    if (e->KeywordSet(DRAW_BUTTON_EVENTS))    widget->AddEventType(GDLWidget::EV_BUTTON);
    else widget->RemoveEventType(GDLWidget::EV_BUTTON);
  }
  if (draw_keyboard_events && widget->IsDraw()) {
    widget->RemoveEventType(GDLWidget::EV_KEYBOARD2);
    widget->RemoveEventType(GDLWidget::EV_KEYBOARD);
    DLong val= (*e->GetKWAs<DLongGDL>(DRAW_KEYBOARD_EVENTS))[0];
    if (val==2) { widget->SetFocus();  widget->AddEventType(GDLWidget::EV_KEYBOARD2);}
    else if (val==1)  { widget->SetFocus(); widget->AddEventType(GDLWidget::EV_KEYBOARD);}
  }
  
  if (all_table_events && widget->IsTable()) {
    if (e->KeywordSet(ALL_TABLE_EVENTS)) widget->AddEventType(GDLWidget::EV_ALL);
    else widget->RemoveEventType(GDLWidget::EV_ALL);
  }
   
  if (all_text_events && widget->IsText()) {
    if (e->KeywordSet(ALL_TEXT_EVENTS)) widget->AddEventType(GDLWidget::EV_ALL);
    else widget->RemoveEventType(GDLWidget::EV_ALL);
  }

//end events 
  if ( update ) {
    widget->widgetUpdate(e->KeywordSet(UPDATE));
  }
  
  if ( realize ) {
    if (do_map) widget->Realize(e->KeywordSet(mapIx)); else widget->Realize(TRUE);
  } else if (do_map) { //alone... 
    GDLWidgetBase * w=widget->GetBaseWidget(widgetID);
    if (w!=NULL) w->mapBase(e->KeywordSet(mapIx));
  } 
  
  if (show) {
    if (e->KeywordSet(showIx)) widget->Raise(); else widget->Lower();
  }
  
  if ( managed ) {
    widget->SetManaged( true );
  }

  if ( xmanActCom ) {
    //       cout << "Set xmanager active command: " << widgetID << endl;
    widget->SetXmanagerActiveCommand( );
  }

  if ( destroy ) {
    delete widget;
    return;
  }

  if ( sensitiveControl) {
    if (e->KeywordSet(sensitiveControlIx)) widget->SetSensitive( TRUE );
    else widget->SetSensitive( FALSE );
  }
  
  if ( inputfocus && (widget->IsDraw()||widget->IsButton()||widget->IsText())) widget->SetFocus();

  if (doTimer) {
    DDouble seconds=0;
    e->AssureDoubleScalarKWIfPresent( timerIx, seconds );
    widget->SendWidgetTimerEvent(seconds);
  }
  
  if ( eventpro ) {
    e->AssureStringScalarKWIfPresent( eventproIx, eventPro );
    widget->SetEventPro( eventPro );
  }

  if ( eventfun ) {
    e->AssureStringScalarKWIfPresent( eventfunIx, eventFun );
//    cout << eventFun << endl;
    widget->SetEventFun( eventFun );
  }
  
  if ( killnotify ) {
    e->AssureStringScalarKWIfPresent( killnotifyIx, killNotifyFunName );
//    cout << killNotifyFunName << endl;
    widget->SetKillNotify( killNotifyFunName );
  }
   if ( notifyrealize ) {
    e->AssureStringScalarKWIfPresent( notifyrealizeIx, notifyRealizeFunName );
//    cout << killNotifyFunName << endl;
    widget->SetNotifyRealize( notifyRealizeFunName );
  }
 
  if (settlbtitle ) {
     GDLWidgetBase* tlb = widget->GetTopLevelBaseWidget(widgetID );
     //following should not happen I believe
     if ( tlb == NULL ) e->Throw("Widget "+i2s( widgetID )+" has no top-level Base (please report!).");
     DStringGDL* tlbTitle=e->GetKWAs<DStringGDL>( tlbsettitleIx );
     wxString tlbName = wxString( (*tlbTitle)[0].c_str( ), wxConvUTF8 );
     static_cast<wxWindow*>(tlb->GetWxWidget())->SetName(tlbName);
  }

  if (givetlbsize) { 
    BaseGDL** tlbsizeKW = &e->GetKW( tlbgetsizeIx );
    GDLDelete((*tlbsizeKW));
    GDLWidgetBase* tlb = widget->GetTopLevelBaseWidget(widgetID );
    if ( tlb == NULL ) e->Throw("Widget "+i2s( widgetID )+" has no top-level Base (please report!).");
    *tlbsizeKW = new DLongGDL(2,BaseGDL::ZERO);
    DLong *retsize=&(*static_cast<DLongGDL*>(*tlbsizeKW))[0];
    int i,j;
    static_cast<wxWindow*>(tlb->GetWxWidget())->GetSize(&i,&j);
    retsize[0]=i;
    retsize[1]=j;
  }

  if ( getuvalue ) {
    BaseGDL** uvalueKW = &e->GetKW( getuvalueIx );
      GDLDelete((*uvalueKW));
    
      BaseGDL *widval = widget->GetUvalue();
      //      *uvalueKW = widget->GetUvalue();
     if ( widval != NULL) { *uvalueKW = widval->Dup();}
  }

  if ( setuvalue ) {
    BaseGDL* uvalue = e->GetKW( setuvalueIx );
    if ( uvalue != NULL ) uvalue = uvalue->Dup( );
    widget->SetUvalue( uvalue );
  }

  if ( setuname ) {
    widget->SetUname( uname );
  }

  if ( prosetvalue ) {
    widget->SetProValue( setvaluepro );
  }

  if ( funcgetvalue ) {
    widget->SetFuncValue( setvaluefunc );
  }

  if ( setbutton ) {
      if( !widget->IsButton())
      {
      e->Throw( "Only WIDGET_BUTTON are allowed with keyword SET_BUTTON." );
    }
    GDLWidgetButton* button = static_cast<GDLWidgetButton*> (widget);
    DLong buttonVal;
    e->AssureLongScalarKWIfPresent( setbuttonIx, buttonVal );
    if ( buttonVal == 0 )
      button->SetButtonWidget( false );
    else
      button->SetButtonWidget( true );
  }
  

  if ( settextselect ) {
    DString wType = widget->GetWidgetName( );
    if ( wType == "TEXT" ) {
      DLongGDL* value=e->GetKWAs<DLongGDL>( settextselectIx );
      if (value->N_Elements() > 2) e->Throw( "Keyword array parameter SET_TEXT_SELECT must have from 1 to 2 elements." );
      GDLWidgetText *textWidget = (GDLWidgetText *) widget;
      textWidget->SetTextSelection( value );
    } else if ( wType == "TABLE" ) e->Throw( "SET_TEXT_SELECT not ready for Table Widgets, FIXME." );
  }
  
  if (widget->IsDropList()){
    GDLWidgetDropList *droplist = (GDLWidgetDropList *) widget;
    
    static int SET_DROPLIST_SELECT = e->KeywordIx( "SET_DROPLIST_SELECT" );
    if (e->KeywordSet(SET_DROPLIST_SELECT)) {
      DLongGDL* droplistSelection =  e->GetKWAs<DLongGDL>(SET_DROPLIST_SELECT);
      if (droplistSelection->N_Elements() > 1) e->Throw( "Expression must be a scalar or 1 element array in this context:");
      droplist->SelectEntry((*droplistSelection)[0]);
    }    
  }
  
  if (widget->IsList()){
    GDLWidgetList *list = (GDLWidgetList *) widget;
    
    static int SET_LIST_SELECT = e->KeywordIx( "SET_LIST_SELECT" );
    if (e->KeywordSet(SET_LIST_SELECT)) {
      DLongGDL* listSelection =  e->GetKWAs<DLongGDL>(SET_LIST_SELECT);
      if (listSelection->N_Elements() > 1) e->Throw( "Expression must be a scalar or 1 element array in this context:");
      list->SelectEntry((*listSelection)[0]);
    }
  }
    if (widget->IsComboBox()){
    GDLWidgetComboBox *combo = (GDLWidgetComboBox *) widget;
    
    static int SET_COMBOBOX_SELECT = e->KeywordIx( "SET_COMBOBOX_SELECT" );
    if (e->KeywordSet(SET_COMBOBOX_SELECT)) {
      DLongGDL* comboSelection =  e->GetKWAs<DLongGDL>(SET_COMBOBOX_SELECT);
      if (comboSelection->N_Elements() > 1) e->Throw( "Expression must be a scalar or 1 element array in this context:");
      combo->SelectEntry((*comboSelection)[0]);
    }
  }

  if (widget->IsTable()) {
    GDLWidgetTable *table = (GDLWidgetTable *) widget;
    static int ALIGNMENT = e->KeywordIx( "ALIGNMENT" );
    static int BACKGROUND_COLOR = e->KeywordIx( "BACKGROUND_COLOR" );
    static int COLUMN_LABELS = e->KeywordIx( "COLUMN_LABELS" );
    static int COLUMN_WIDTHS = e->KeywordIx( "COLUMN_WIDTHS" );
    static int DELETE_COLUMNS = e->KeywordIx( "DELETE_COLUMNS" );
    static int DELETE_ROWS = e->KeywordIx( "DELETE_ROWS" );
//    static int EDITABLE = e->KeywordIx( "EDITABLE" ); 
//    static int FONT = e->KeywordIx( "FONT" );
    static int FOREGROUND_COLOR = e->KeywordIx( "FOREGROUND_COLOR" );
//    static int FORMAT = e->KeywordIx( "FORMAT" ); //NOT USED IN ALL THE PRO I KNOW OF
//    static int IGNORE_ACCELERATORS = e->KeywordIx( "IGNORE_ACCELERATORS" );
    static int INSERT_COLUMNS = e->KeywordIx( "INSERT_COLUMNS" );
    static int INSERT_ROWS = e->KeywordIx( "INSERT_ROWS" );
    static int ROW_LABELS = e->KeywordIx( "ROW_LABELS" );
    static int ROW_HEIGHTS = e->KeywordIx( "ROW_HEIGHTS" );
    static int SET_TABLE_SELECT = e->KeywordIx( "SET_TABLE_SELECT" );
    static int SET_TABLE_VIEW = e->KeywordIx( "SET_TABLE_VIEW" );
//    static int TAB_MODE = e->KeywordIx( "TAB_MODE" );
//    static int TABLE_BLANK = e->KeywordIx( "TABLE_BLANK" );
    static int TABLE_DISJOINT_SELECTION = e->KeywordIx( "TABLE_DISJOINT_SELECTION" );
    static int TABLE_XSIZE = e->KeywordIx( "TABLE_XSIZE" );
    static int TABLE_YSIZE = e->KeywordIx( "TABLE_YSIZE" );
    static int USE_TABLE_SELECT = e->KeywordIx("USE_TABLE_SELECT");
    static int EDIT_CELL = e->KeywordIx("EDIT_CELL");
//
//    DByteGDL* editable = GetKeywordAs<DByteGDL>(e, EDITABLE );

    static int AM_PM = e->KeywordIx( "AM_PM" );
    DStringGDL* amPm = GetKeywordAs<DStringGDL>(e, AM_PM);
    //check
    if (amPm) {if (amPm->N_Elements()!=2) e->Throw("% Keyword array parameter AM_PM must have 2 elements.");else table->SetAmPm(amPm);}
    static int DAYS_OF_WEEK = e->KeywordIx( "DAYS_OF_WEEK" );
    DStringGDL* daysOfWeek = GetKeywordAs<DStringGDL>(e, DAYS_OF_WEEK);
    //check
    if (daysOfWeek) {if (daysOfWeek->N_Elements()!=2) e->Throw("% Keyword array parameter DAYS_OF_WEEK must have 7 elements."); else table->SetDOW(daysOfWeek);}
    static int MONTHS = e->KeywordIx( "MONTHS" );
    DStringGDL* month = GetKeywordAs<DStringGDL>(e, MONTHS);
    //check
    if (month) {if (month->N_Elements()!=12) e->Throw("% Keyword array parameter MONTH must have 12 elements."); else table->SetMonth(month);}

    DByteGDL* alignment = GetKeywordAs<DByteGDL>(e, ALIGNMENT);
    DByteGDL* backgroundColor = GetKeywordAs<DByteGDL>(e, BACKGROUND_COLOR);
    DStringGDL* columnLabels = GetKeywordAs<DStringGDL>(e, COLUMN_LABELS);
    DLongGDL* columnWidth = GetKeywordAs<DLongGDL>(e, COLUMN_WIDTHS);
    bool hasColumnsToDelete = e->KeywordPresent(DELETE_COLUMNS); //Present is sufficient to trig column deletion (IDL feature).
    bool hasRowsToDelete = e->KeywordPresent(DELETE_ROWS); //Present is sufficient to trig column deletion (IDL feature).

    bool insertColumns = e->KeywordSet(INSERT_COLUMNS);
    int columnsToInsert = 0;
    if (insertColumns) e->AssureLongScalarKWIfPresent(INSERT_COLUMNS,columnsToInsert);

    bool insertRows = e->KeywordSet(INSERT_ROWS);
    int rowsToInsert = 0;
    if (insertRows) e->AssureLongScalarKWIfPresent(INSERT_ROWS,rowsToInsert);
    
    DByteGDL* foregroundColor = GetKeywordAs<DByteGDL>(e, FOREGROUND_COLOR);
    DLongGDL* rowHeights = GetKeywordAs<DLongGDL>(e, ROW_HEIGHTS);
    DStringGDL* rowLabels = GetKeywordAs<DStringGDL>(e, ROW_LABELS);

    bool setATableView = e->KeywordSet(SET_TABLE_VIEW);
    DLongGDL* tableView = GetKeywordAs<DLongGDL>(e, SET_TABLE_VIEW);

    bool editcell = e->KeywordSet(EDIT_CELL);
    DLongGDL* cellToEdit = GetKeywordAs<DLongGDL>(e, EDIT_CELL);
   
    bool setATableSelection = e->KeywordSet(SET_TABLE_SELECT);
    DLongGDL* tableSelectionToSet = GetKeywordAs<DLongGDL>(e, SET_TABLE_SELECT);
    if (setATableSelection) { //check further a bit...
      if (table->GetDisjointSelection()) {
        if (tableSelectionToSet->Dim(0) != 2) e->Throw( "Array must have dimensions of (2, N): " + e->GetString( SET_TABLE_SELECT ) );
      } else {
        if (tableSelectionToSet->Rank() != 1 || tableSelectionToSet->Dim(0) != 4 ) e->Throw( "Array must have dimensions of (4): " + e->GetString( SET_TABLE_SELECT ) );
      }
      table->SetSelection(tableSelectionToSet);
    }
    
    bool useATableSelection = e->KeywordSet(USE_TABLE_SELECT);
    DLongGDL* tableSelectionToUse = GetKeywordAs<DLongGDL>(e, USE_TABLE_SELECT);
    if (useATableSelection && tableSelectionToUse->Rank()==0 & !table->IsSomethingSelected())
      { e->Throw( "USE_TABLE_SELECT value out of range.");}
    if (useATableSelection && tableSelectionToUse->Rank()>0) { //check further a bit...
      if (table->GetDisjointSelection()) {
        if (tableSelectionToUse->Dim(0) != 2) e->Throw( "Array must have dimensions of (2, N): " + e->GetString( USE_TABLE_SELECT ) );
      } else {
        if (tableSelectionToUse->Rank() != 1 || tableSelectionToUse->Dim(0) != 4 ) e->Throw( "Array must have dimensions of (4): " + e->GetString( USE_TABLE_SELECT ) );
      }
    }
    
    bool tablexsize=e->KeywordSet(TABLE_XSIZE);
    bool tableysize=e->KeywordSet(TABLE_YSIZE);

    bool hasTableDisjointSelection = e->KeywordPresent(TABLE_DISJOINT_SELECTION);
    if (hasTableDisjointSelection) {
      bool oldDisjoint = table->GetDisjointSelection();
      bool newDisjoint = e->KeywordSet( TABLE_DISJOINT_SELECTION );
      if (oldDisjoint != newDisjoint) {
        table->ClearSelection();
        table->SetDisjointSelection(newDisjoint);
      }
    }
    
    bool hasAlignment=(alignment!=NULL);
    if (hasAlignment) {
      table->SetAlignment(alignment);
       if (useATableSelection) table->DoAlign(tableSelectionToUse); else table->DoAlign();
    }
    bool hasBackgroundColor=(backgroundColor!=NULL);
    if (hasBackgroundColor) {
      if (backgroundColor->N_Elements()%3 !=0 ) e->Throw("Table grid color attribute has the wrong dimensions.");
      table->SetBackgroundColor(backgroundColor);
       if (useATableSelection) table->DoBackgroundColor(tableSelectionToUse); else table->DoBackgroundColor();
    }
    bool hasForegroundColor=(foregroundColor!=NULL);
    if (hasForegroundColor) {
      if (foregroundColor->N_Elements()%3 !=0 ) e->Throw("Table grid color attribute has the wrong dimensions.");
      table->SetForegroundColor(foregroundColor);
      if (useATableSelection) table->DoForegroundColor(tableSelectionToUse); else table->DoForegroundColor(); 
    }
    bool hasColumnLabels=(columnLabels!=NULL);
    if (hasColumnLabels) {
      table->SetColumnLabels(columnLabels);
      table->DoColumnLabels();
    }
    bool hasRowLabels=(rowLabels!=NULL);
    if (hasRowLabels) {
      table->SetRowLabels(rowLabels);
      table->DoRowLabels();
    }
    bool hasColumnWidth=(columnWidth!=NULL);
    if (hasColumnWidth) {
      table->SetColumnWidth(columnWidth);
      if (useATableSelection) table->DoColumnWidth(tableSelectionToUse); else  table->DoColumnWidth();
    }
    bool hasRowHeights=(rowHeights!=NULL);
    if (hasRowHeights) {
      table->SetRowHeights(rowHeights);
      if (useATableSelection) table->DoRowHeights(tableSelectionToUse); else table->DoRowHeights();
    }
    if (hasColumnsToDelete && table->GetMajority()!=GDLWidgetTable::COLUMN_MAJOR) {
      if (useATableSelection) table->DeleteColumns(tableSelectionToUse); else table->DeleteColumns();
    }
    if (hasRowsToDelete && table->GetMajority()!=GDLWidgetTable::ROW_MAJOR) {
      if (useATableSelection) table->DeleteRows(tableSelectionToUse); else table->DeleteRows();
    }    
    if (insertRows && table->GetMajority()!=GDLWidgetTable::ROW_MAJOR) {
      bool success;
      if (useATableSelection)  success=table->InsertRows(rowsToInsert,tableSelectionToUse); else  success=table->InsertRows(rowsToInsert);
      if (!success) e->Throw("Error adding Row(s).");
    }
    if (insertColumns && table->GetMajority()!=GDLWidgetTable::COLUMN_MAJOR) {
      bool success;
      if (useATableSelection) success=table->InsertColumns(columnsToInsert,tableSelectionToUse); else success=table->InsertColumns(columnsToInsert);
      if (!success) e->Throw("Error adding Column(s).");
    }
    if (setATableView) {if ( tableView->N_Elements() !=2 ) e->Throw("Table view attribute has the wrong dimensions.");
      table->SetTableView(tableView);
    }
    if (editcell) {if ( cellToEdit->N_Elements() !=2 ) e->Throw("Edit cell attribute has the wrong dimensions.");
      table->EditCell(cellToEdit);
    }
    if (tablexsize) {
      DLong xsize= (*e->GetKWAs<DLongGDL>(TABLE_XSIZE))[0];
      table->SetTableNumberOfColumns(xsize);
    }
    if (tableysize) {
      DLong ysize= (*e->GetKWAs<DLongGDL>(TABLE_YSIZE))[0];
      table->SetTableNumberOfRows(ysize);
    }
  }  
#endif
}
#ifdef HAVE_WXWIDGETS_PROPERTYGRID

// WIDGET_PROPERTYSHEET
  BaseGDL* widget_property( EnvT* e)
  {
#ifndef HAVE_LIBWXWIDGETS
  e->Throw( "GDL was compiled without support for wxWidgets" );
  return NULL; // avoid warning
#else
  SizeT nParam = e->NParam( 1 );

  DLongGDL* p0L = e->GetParAs<DLongGDL>(0);
  WidgetIDT parentID = (*p0L)[0];
  GDLWidget *widget = GDLWidget::GetWidget( parentID );
  if (widget==NULL) e->Throw( "Invalid widget identifier: " + i2s( parentID ) );
  
  GDLWidgetPropertySheet* ps = new GDLWidgetPropertySheet( parentID, e);
  ps->SetWidgetType( GDLWidget::WIDGET_PROPERTYSHEET );

  return new DLongGDL( ps->WidgetID( ) );
  }
#endif
#endif

} // namespace library
