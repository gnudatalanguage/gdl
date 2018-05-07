/***************************************************************************
                          gdlwidget.cpp  -  GDL widget system implementation 
                             -------------------
    begin                : Fri May 7 2004
    copyright            : (C) 2004 by Marc Schellens
    email                : m_schellens@users.sourceforge.net
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

#ifdef HAVE_LIBWXWIDGETS

#include <memory> 

#include <wx/notebook.h>
#include <wx/grid.h>
#include <wx/gbsizer.h>

#include "basegdl.hpp"
#include "dstructgdl.hpp"

#include "dinterpreter.hpp"


#include "gdlwxstream.hpp"

#include "gdlwidget.hpp"

#include "widget.hpp"
#include "graphicsdevice.hpp"



#define TIDY_WIDGET {this->SetSensitive(sensitive);\
if (!font.IsSameAs(wxNullFont)) {\
    wxWindow* ww=static_cast<wxWindow*>(wxWidget); if (ww) ww->SetFont(font);\
  }\
    ConnectToDesiredEvents();\
}
#define UPDATE_WINDOW {\
  GDLWidgetBase *tlb = GetTopLevelBaseWidget( this->WidgetID( ) );\
  GDLFrame *tlbFrame=static_cast<GDLFrame*> (tlb->GetWxWidget( ));\
/*  this->RefreshWidget();*/ \
  if (tlb->IsStretchable()) {\
  tlbFrame->Fit( );\
  }\
}

const WidgetIDT GDLWidget::NullID = 0;

// instantiation
WidgetListT GDLWidget::widgetList;

GDLEventQueue GDLWidget::eventQueue; // the event queue
GDLEventQueue GDLWidget::readlineEventQueue; // for process at command line level
bool GDLWidget::wxIsOn=false;
bool GDLWidget::handlersOk=false;

void GDLEventQueue::Purge()
{
  for ( SizeT i = 0; i < dq.size( ); ++i )
    delete dq[i];
  dq.clear( );
  //   isEmpty = true;
}

// removes all events for TLB 'topID'
void GDLEventQueue::Purge( WidgetIDT topID)
{
  for( long i=dq.size()-1; i>=0;--i)
  {
    DStructGDL* ev = dq[i];
    static int topIx = ev->Desc( )->TagIndex( "TOP" );
    assert( topIx == 1 );
    DLong top = (*static_cast<DLongGDL*> (ev->GetTag( topIx, 0 )))[0];
    if( top == topID)
    {
      delete ev;
      dq.erase( dq.begin( ) + i );
    }
  }
  //   isEmpty = true;
}

inline long GDLWidget::textAlignment()
{
      long myAlign = 0;
      if ( alignment & gdlwALIGN_TOP ) myAlign |= wxTE_LEFT;
      if ( alignment & gdlwALIGN_BOTTOM ) myAlign |= wxTE_LEFT;
      if ( alignment & gdlwALIGN_LEFT ) myAlign |= wxTE_LEFT;
      if ( alignment & gdlwALIGN_CENTER ) myAlign |= wxTE_CENTER;
      if ( alignment & gdlwALIGN_RIGHT ) myAlign |= wxTE_RIGHT;
      if (myAlign == 0) myAlign = wxTE_LEFT; //center is the default
      return myAlign;
}
inline long GDLWidget::buttonTextAlignment()
{
      long myAlign = 0;
      if ( alignment & gdlwALIGN_TOP ) myAlign |= wxBU_TOP;
      if ( alignment & gdlwALIGN_BOTTOM ) myAlign |= wxBU_BOTTOM;
      if ( alignment & gdlwALIGN_LEFT ) myAlign |= wxBU_LEFT;
      if ( alignment & gdlwALIGN_CENTER ) myAlign |= wxBU_EXACTFIT;
      if ( alignment & gdlwALIGN_RIGHT ) myAlign |= wxBU_RIGHT;
      if (myAlign == 0) myAlign = wxBU_EXACTFIT; //center is the default
      return myAlign;
}
inline long GDLWidget::widgetAlignment()
{
      GDLWidgetBase* base = static_cast<GDLWidgetBase*>(this->GetWidget(parentID));
      if (base) { if (alignment == gdlwALIGN_NOT ) alignment=base->getChildrenAlignment(); }  
      if (alignment == gdlwALIGN_NOT ) return wxEXPAND;
      long myAlign= 0;
      if ( alignment & gdlwALIGN_TOP ) myAlign |= wxALIGN_TOP;
      if ( alignment & gdlwALIGN_BOTTOM ) myAlign |= wxALIGN_BOTTOM;
      if ( alignment & gdlwALIGN_LEFT ) myAlign |= wxALIGN_LEFT;
      if ( alignment & gdlwALIGN_CENTER ) myAlign |= wxALIGN_CENTER;
      if ( alignment & gdlwALIGN_RIGHT ) myAlign |= wxALIGN_RIGHT;
      return myAlign;
}

inline wxSizer* GetBaseSizer( DLong col, DLong row, bool grid, long space) 
{
  wxSizer* sizer = NULL;
  if ( row <= 0  && col <= 0) {sizer = new wxGridBagSizer(space,space); }
//  else if ( row == 0  && col == 1) {sizer = new wxBoxSizer( wxVERTICAL ); if (space) sizer->AddSpacer(space);}//no no space is not OK with boxsizer!
  else if ( row == 0 && col >= 1) {sizer = (grid)?new wxGridSizer( 0, col, space, space ):new wxFlexGridSizer( 0, col, space, space );}
//  else if ( col == 0 && row == 1) {sizer = new wxBoxSizer( wxHORIZONTAL );if (space) sizer->AddSpacer(space);}
  else if ( col == 0 && row >= 1) {sizer = (grid)?new wxGridSizer( row, 0, space, space ):new wxFlexGridSizer( row, 0, space, space );}
  else sizer = new wxFlexGridSizer( row, col, space, space ); //which should not happen.
  return sizer;
}

inline wxSize GDLWidgetText::computeWidgetSize()
{
  //widget text size is in LINES in Y and CHARACTERS in X. But overridden by scr_xsize et if present
  wxSize widgetSize = wxDefaultSize;
  //note: apparently GetPixelSize can return 0 at least under windows. Here this will only put widgetsize to 20 x 20. In other places;
  //it should be protected agains zero divides.
  wxSize fontSize = wxSystemSettings::GetFont( wxSYS_SYSTEM_FONT ).GetPixelSize();
  if (!font.IsSameAs(wxNullFont)) fontSize = font.GetPixelSize(); 
  if ( xSize > 0 ) { widgetSize.x = (xSize+0.5) * fontSize.x; if ( widgetSize.x < 20 ) widgetSize.x=20;}
  else { 
    widgetSize.x = (maxlinelength+0.5) * fontSize.x;
    if ( widgetSize.x < 140 ) widgetSize.x=20* fontSize.x; 
  } //TBC
//but..
  if (scrXSize > 0) widgetSize.x=scrXSize;
  if ( ySize > 0 )  widgetSize.y = (ySize * 1.5 * fontSize.y); 
  else widgetSize.y = fontSize.y*1.5; //instead of nlines*fontSize.y to be compliant with *DL
  if (widgetSize.y < 20) widgetSize.y = 20;
//but..
   if (scrYSize > 0) widgetSize.y=scrYSize;
  
  return widgetSize;
}
inline wxSize GDLWidgetList::computeWidgetSize()
{
  //widget text size is in LINES in Y and CHARACTERS in X. But overridden by scr_xsize et if present
  //note: apparently GetPixelSize can return 0 at least under windows. Here this will only put widgetsize to 20 x 20. In other places;
  //it should be protected agains zero divides.
  wxSize fontSize = wxSystemSettings::GetFont( wxSYS_SYSTEM_FONT ).GetPixelSize();
  if (!font.IsSameAs(wxNullFont)) fontSize = font.GetPixelSize(); //wxSystemSettings::GetFont( wxSYS_SYSTEM_FONT ).GetPixelSize();
  wxSize widgetSize = wxDefaultSize;
  if ( xSize > 0 ) {widgetSize.x = (xSize+0.5) * fontSize.x;if ( widgetSize.x < 20 ) widgetSize.x=20;}
  else {
    widgetSize.x = (maxlinelength+1.5) * fontSize.x;
    if ( widgetSize.x < 140 ) widgetSize.x=20* fontSize.x;
  }
//but..
  if (scrXSize > 0) widgetSize.x=scrXSize;
  
  if ( ySize > 0 )  widgetSize.y = (ySize * 1.5 * fontSize.y); 
  else widgetSize.y = fontSize.y+1.5; //instead of nlines*fontSize.y to be compliant with *DL
  if (widgetSize.y < 20) widgetSize.y = 20;
//but..
   if (scrYSize > 0) widgetSize.y=scrYSize;
  
  return widgetSize;
}
inline wxSize GDLWidget::computeWidgetSize()
{
//here is a good place to make dynamic widgets static, since dynamic_resize is permitted only if size is not given.
  if (xSize > 0 || ySize > 0 || scrXSize > 0 || scrYSize > 0) dynamicResize=-1;
  wxSize widgetSize;
  if ( xSize > 0 ) widgetSize.x = xSize*unitConversionFactor.x;
  else widgetSize.x = wxDefaultSize.x;
//but..
  if (scrXSize > 0) widgetSize.x=scrXSize;
  
  if ( ySize > 0 )  widgetSize.y = ySize * unitConversionFactor.y; 
  else widgetSize.y = wxDefaultSize.y;
//but..
   if (scrYSize > 0) widgetSize.y=scrYSize;
  
  return widgetSize;
}

// widget from ID
GDLWidget* GDLWidget::GetWidget( WidgetIDT widID)
{
  if ( widID == GDLWidget::NullID )
    return NULL;
  WidgetListT::iterator it = widgetList.find( widID );
  if ( it == widgetList.end( ) ) return NULL;
  return it->second;
}

// widget parent from ID
GDLWidget* GDLWidget::GetParent( WidgetIDT widID)
{
  GDLWidget *widget = GetWidget( widID );
  if ( widget == NULL )
    return NULL;
  WidgetIDT parentID = widget->parentID;
  GDLWidget *parent = GetWidget( parentID );
  return parent;
}
void  GDLWidget::widgetUpdate(bool update){
//   GDLWidgetBase *tlb = GetTopLevelBaseWidget( this->WidgetID( ) );
//   wxWindow * me = static_cast<wxWindow*>(tlb->GetWxWidget());
   wxWindow * me = static_cast<wxWindow*>(wxWidget);
   if (me) {if (update) {if ( me->IsFrozen()) me->Thaw(); else me->Refresh(); } else me->Freeze();} else cerr<<"freezing unknown widget\n";
}
  
// base widget ID from ID
GDLWidgetBase* GDLWidget::GetBaseWidget( WidgetIDT widID)
{
  WidgetIDT actID = widID;
  while ( 1 ) {
    GDLWidget* widget = GetWidget( actID );
    if ( widget == NULL )
      return NULL;
    if ( widget->IsBase( ) ) 
      return static_cast<GDLWidgetBase*> (widget);
    assert( widget->parentID != GDLWidget::NullID );
    actID = widget->parentID;
  }
}
WidgetIDT GDLWidget::GetBase( WidgetIDT widID)
{
  GDLWidget *widget;
  WidgetIDT actID = widID;
  while ( 1 ) {
    GDLWidget* widget = GetWidget( actID );
    if ( widget == NULL )
      return GDLWidget::NullID;
    if ( widget->IsBase( ) )
      return actID;
    assert( widget->parentID != GDLWidget::NullID );
    actID = widget->parentID;
  }
}

GDLWidgetBase* GDLWidget::GetTopLevelBaseWidget( WidgetIDT widID)
{
  WidgetIDT actID = widID;
  while ( 1 ) {
    GDLWidget *widget = GetWidget( actID );
    if ( widget == NULL )
      return (GDLWidgetBase*)GDLWidget::NullID;
    if ( widget->parentID == GDLWidget::NullID )
      return static_cast<GDLWidgetBase*> (widget);
    else
      actID = widget->parentID;
  }
}
WidgetIDT GDLWidget::GetTopLevelBase( WidgetIDT widID)
{
  WidgetIDT actID = widID;
  while ( 1 ) {
    GDLWidget *widget = GetWidget( actID );
    if ( widget == NULL )
      return GDLWidget::NullID;
    if ( widget->parentID == GDLWidget::NullID )
      return actID;
    else
      actID = widget->parentID;
  }
}

void GDLWidget::RefreshWidget( )
{
  if ( widgetPanel->IsShownOnScreen( ) ) {
    if ( this->parentID == GDLWidget::NullID ) return;
    GDLWidget* gdlParent = GetWidget( parentID );
    while ( gdlParent->IsBase() ) {
      wxSizer * s = gdlParent->GetSizer( );
      if ( s ) s->Layout( );
      if ( gdlParent->parentID == GDLWidget::NullID ) break;
      gdlParent = GetWidget( gdlParent->GetParentID( ) );
    } 
    static_cast<wxWindow*>(this->GetWxWidget())->Refresh(); //next event loop
//    wxWindow* me=static_cast<wxWindow*>(this->GetWxWidget()); if (me) me->Update(); else cerr<<"Refresh unknown widget!\n"; //immediate
  }
}

int GDLWidget::HandleEvents()
{
  //make one loop for wxWidgets Events...
  if (wxIsStarted() && wxTheApp) {
      wxTheApp->OnRun(); //wxTheApp may not be started
  //treat our GDL events...
    DStructGDL* ev = NULL;
    while( (ev = GDLWidget::readlineEventQueue.Pop()) != NULL)
    {
      static int idIx = ev->Desc( )->TagIndex( "ID" ); // 0
      static int topIx = ev->Desc( )->TagIndex( "TOP" ); // 1
      static int handlerIx = ev->Desc( )->TagIndex( "HANDLER" ); // 2
      assert( idIx == 0 );
      assert( topIx == 1 );
      assert( handlerIx == 2 );

      WidgetIDT id = (*static_cast<DLongGDL*> (ev->GetTag( idIx, 0 )))[0];

      ev = CallEventHandler( ev );

      if( ev != NULL)
      {
        Warning( "Unhandled event. ID: " + i2s( id ) );
        GDLDelete( ev );
        ev = NULL;
        return 0;
      } 
    }
    if (wxIsBusy()) wxEndBusyCursor( );
  }
  return 0;
}

void GDLWidget::PushEvent( WidgetIDT baseWidgetID, DStructGDL* ev) {
  // Get XmanagerActiveCommand status
  GDLWidget *baseWidget = GDLWidget::GetWidget( baseWidgetID );
  if ( baseWidget != NULL ) {
    bool xmanActCom = baseWidget->GetXmanagerActiveCommand( );
    if ( !xmanActCom ) { //Not blocking: events in eventQueue.
      //     wxMessageOutputStderr().Printf(_T("eventQueue.Push: %d\n"),baseWidgetID);
      eventQueue.Push( ev );
    } else { //Blocking: events in readlineeventQueue.
      //     wxMessageOutputStderr().Printf(_T("readLineEventQueue.Push: %d\n"),baseWidgetID);
      readlineEventQueue.Push( ev );
    }
  } else cerr << "NULL baseWidget (possibly Destroyed?) found in GDLWidget::PushEvent( WidgetIDT baseWidgetID=" << baseWidgetID << ", DStructGDL* ev=" << ev << "), please report!\n";
}

bool GDLWidget::GetXmanagerBlock() 
{
  bool xmanBlock = false;
  WidgetListT::iterator it;
  // (*it).first is widgetID
  // (*it).second is pointer to widget

  bool managed;
  bool xmanActCom;

#ifdef GDL_DEBUG_WIDGETS
  std::cout << "+ GetXmanagerBlock: widgetList:" << std::endl;
  for ( it = widgetList.begin( ); it != widgetList.end( ); ++it ) {
    std::cout << (*it).first << ": " << (*it).second->widgetID << "  parentID: " <<
    (*it).second->parentID << "  uname: " << (*it).second->uName << std::endl;
  }
  std::cout << "- GetXmanagerBlock: widgetList end" << std::endl;
#endif
  for ( it = widgetList.begin( ); it != widgetList.end( ); ++it ) {
    // Only consider base widgets
    if ( (*it).second->parentID == GDLWidget::NullID ) {
      managed = (*it).second->GetManaged( );
      xmanActCom = (*it).second->GetXmanagerActiveCommand( );
    }
    if ( managed && !xmanActCom ) {
      xmanBlock = true;
      break;
    }
  }
  return xmanBlock;
}
DLong GDLWidget::GetNumberOfWidgets() {
  WidgetListT::iterator it;
  DLong result=0;
  for ( it = widgetList.begin( ); it != widgetList.end( ); ++it ) result++;
  return result;
}
BaseGDL* GDLWidget::GetWidgetsList() {
  DLong nw=GetNumberOfWidgets();
  if (nw<=0) return new DLongGDL(0);
  WidgetListT::iterator it;
  SizeT index=0;
  DLongGDL* result=new DLongGDL(nw,BaseGDL::NOZERO);
  for ( it = widgetList.begin( ); it != widgetList.end( ); ++it ) {
    (*result)[index]=(*it).second->widgetID; index++;
  }
  return result;
}
BaseGDL* GDLWidget::GetManagedWidgetsList() {
  DLong nw=GetNumberOfWidgets();
  if (nw<=0) return new DLongGDL(0);
  WidgetListT::iterator it;
  SizeT index;
  for (index=0, it = widgetList.begin( ); it != widgetList.end( ); ++it ) {
    if ((*it).second->GetManaged() == true) index++;
  }
  if (index<=0) return new DLongGDL(0);
  DLongGDL* result=new DLongGDL(index,BaseGDL::NOZERO);
  for (index=0, it = widgetList.begin( ); it != widgetList.end( ); ++it ) {
    if ((*it).second->GetManaged() == true) (*result)[index++]=(*it).second->widgetID;
  }
  return result;
}
// Init
void GDLWidget::Init()
{
 if( ! wxInitialize( ) ) cerr << "WARNING: wxWidgets not initializing" <<endl;
 SetWxStarted();
}
// UnInit
void GDLWidget::UnInit()
{
  if (wxIsStarted()) {
   WidgetListT::iterator it;
   for ( it = widgetList.begin( ); it != widgetList.end( ); ++it ) {
     GDLWidget* w=(*it).second;
     if ( w != NULL) delete w;
      }
    GDLWidget::HandleEvents();
// the following cannot be done: once unitialized, the wxWidgets library cannot be safely initilized again.
//    wxUninitialize( );
    UnsetWxStarted();//reset handlersOk too.
  }
}

void GDLWidget::ConnectToDesiredEvents(){
  if ( eventFlags & GDLWidget::EV_TRACKING ) {
    static_cast<wxWindow*>(wxWidget)->Connect(widgetID,wxEVT_ENTER_WINDOW, wxMouseEventHandler(GDLFrame::OnEnterWindow));
    static_cast<wxWindow*>(wxWidget)->Connect(widgetID,wxEVT_LEAVE_WINDOW, wxMouseEventHandler(GDLFrame::OnLeaveWindow));
  }
  if ( eventFlags & GDLWidget::EV_CONTEXT ) static_cast<wxWindow*>(wxWidget)->Connect(widgetID,wxEVT_CONTEXT_MENU, wxContextMenuEventHandler(GDLFrame::OnContextEvent));
  if ( eventFlags & GDLWidget::EV_KBRD_FOCUS ) {
      static_cast<wxWindow*>(wxWidget)->Connect(widgetID,wxEVT_SET_FOCUS, wxFocusEventHandler(GDLFrame::OnKBRDFocusChange));
      static_cast<wxWindow*>(wxWidget)->Connect(widgetID,wxEVT_KILL_FOCUS, wxFocusEventHandler(GDLFrame::OnKBRDFocusChange));
  }
}
//initialize static member
int GDLWidget::gdl_lastControlId=0;

GDLWidget::GDLWidget( WidgetIDT p, EnvT* e, BaseGDL* vV, DULong eventFlags_)
: wxWidget( NULL )
, widgetID (0)
, parentID( p )
, uValue( NULL )
, vValue( vV )
, scrolled(FALSE)
, sensitive(TRUE)
, managed( false )
, eventFlags( eventFlags_ )
, buttonState(FALSE)
, exclusiveMode( 0 )
, xOffset(-1), yOffset(-1), xSize(-1), ySize(-1), scrXSize(-1), scrYSize(-1)
, topWidgetSizer( NULL )
, widgetSizer( NULL )
, widgetPanel( NULL )
, scrollSizer( NULL )
, scrollPanel( NULL )
, frameSizer( NULL )
, framePanel( NULL )
, widgetType(GDLWidget::WIDGET_UNKNOWN)
, widgetName("")
, groupLeader(GDLWidget::NullID)
, unitConversionFactor(wxRealPoint(1.0,1.0)) //no conversion at start.
, frameWidth(-1)
, font(wxNullFont)
, valid(TRUE)
, alignment(gdlwALIGN_NOT)
, widgetStyle(wxSTRETCH_NOT)
, dynamicResize(-1) //not permitted
{
  if ( e != NULL ) GetCommonKeywords( e ); else DefaultValuesInAbsenceofEnv();

// was   widgetID =  wxWindow::NewControlId( ); // but some scripts use the fact that widget ids are positive (graffer.pro)
  widgetID = GDLNewControlId( );

  if ( parentID != GDLWidget::NullID ) {
    GDLWidget* gdlParent = GetWidget( parentID );
    assert( gdlParent != NULL);
    if ( gdlParent->IsBase( ) ) {
      GDLWidgetBase* base = static_cast<GDLWidgetBase*> (gdlParent);
      base->AddChild( widgetID );
//      if (alignment == -1) alignment=base->getChildrenAlignment();  
    } else if ( gdlParent->IsTab( ) ) {
      GDLWidgetTab* base = static_cast<GDLWidgetTab*> (gdlParent);
      base->AddChild( widgetID );
    }
    else 
    { 
      GDLWidget* w = GetBaseWidget( parentID );
      if (w && w->IsBase())  static_cast<GDLWidgetBase*>(w)->AddChild( widgetID );
      if (w && w->IsTab())   static_cast<GDLWidgetTab*>(w)->AddChild( widgetID );
    }
  }
  
  widgetList.insert( widgetList.end( ), std::pair<WidgetIDT, GDLWidget*>(widgetID, this) );
#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputStderr( ).Printf( _T( "inserted: ID: %d  parentID: %d\n" ), widgetID, parentID );
#endif
  if (groupLeader != 0) {
    GDLWidget* leader=this->GetWidget(groupLeader);
    if (leader) leader->AddToFollowers(widgetID);
  }
}

void GDLWidget::SetSensitive(bool value)
{
    wxWindow *me=static_cast<wxWindow*>(this->GetWxWidget()); 
    if (me!=NULL) {
      if (value) me->Enable(); else me->Disable();
    } else {
      if (this->IsButton()) static_cast<GDLWidgetButton*>(this)->SetSensitive(value); else cerr<<"Making (Un)Sensitive unknown widget!\n";
    }
  }

void GDLWidget::SetFocus() //gives focus to the CHILD of the panel.
{
//  if (this->GetWidgetName()=="DRAW") static_cast<GDLDrawPanel*>(this->wxWidget)->SetFocus(); 
//  else if (this->GetWidgetName()=="TABLE") static_cast<GDLGrid*>(this->wxWidget)->SetFocus(); 
//  else  static_cast<wxPanel*> (this->widgetPanel)->SetFocus();
  wxWindow *me=static_cast<wxWindow*>(this->GetWxWidget()); if (me!=NULL) me->SetFocus(); else cerr<<"Setting Focus for unknown widget!\n";
}

void GDLWidget::SetSizeHints()
{
  assert( parentID == NullID );
#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputStderr( ).Printf( _T( "GDLWidget:SetSizeHints: %d\n" ), this->widgetID );
#endif
  GDLFrame *frame = static_cast<GDLFrame*> (this->wxWidget);
  if (frame) topWidgetSizer->SetSizeHints( frame ); else cerr<<"Setting size hints for unknown widget!\n";
}

void GDLWidget::SetSize(DLong sizex, DLong sizey) //in pixels. Always.
{
  //Sizes are in pixels. Units must be converted before calling this function.
  wxWindow* me=static_cast<wxWindow*>(this->GetWxWidget());
  wxSize currentSize;
  if (me) currentSize=me->GetClientSize(); else {cerr<<"Setting size of unknown widget!\n"; return;}
  if (currentSize==wxSize(sizex,sizey)) return;
  
  //note particular case of 0 for base widgets: 0 means stretch otherwise not.
  if (this->IsBase()) {
    static_cast<GDLWidgetBase*>(this)->SetStretchX((sizex<=0)); 
    static_cast<GDLWidgetBase*>(this)->SetStretchY((sizey<=0));
  }
  //is this needed?
  xSize=(sizex<=0)?currentSize.x:sizex;
  ySize=(sizey<=0)?currentSize.y:sizey;

  GDLWidgetBase *tlb = GetTopLevelBaseWidget( this->WidgetID( ) );
  GDLFrame *tlbFrame=static_cast<GDLFrame*>(tlb->GetWxWidget( ));
//we should prevent resize event
  if ( (tlb->GetEventFlags() & GDLWidget::EV_SIZE) == GDLWidget::EV_SIZE ) tlbFrame->Disconnect(tlb->WidgetID(), wxEVT_SIZE, gdlSIZE_EVENT_HANDLER); //(GDLFrame::OnSizeWithTimer));
  me->SetClientSize(xSize,ySize);
  widgetSizer->SetItemMinSize(me,xSize,ySize);
  this->RefreshWidget();
  if (tlb->IsStretchable()) {
    tlbFrame->SetMinSize(wxDefaultSize); 
    tlbFrame->Fit( ); //makes a mess in atv.pro while resizing. TB checked.
  }
  if ((tlb->GetEventFlags() & GDLWidget::EV_SIZE) == GDLWidget::EV_SIZE) tlbFrame->Connect(tlb->WidgetID(), wxEVT_SIZE, gdlSIZE_EVENT_HANDLER);//wxSizeEventHandler(GDLFrame::OnSizeWithTimer));
}

void GDLWidget::SendWidgetTimerEvent(DDouble secs)
{
  if( parentID == NullID)
  {
    assert( this->IsBase( ) );
    GDLFrame *frame = static_cast<GDLFrame*> (this->wxWidget);
    frame->SendWidgetTimerEvent(secs,widgetID);
  }
  else
  {
    GDLWidgetBase* tlb = GetTopLevelBaseWidget( parentID );
    assert( tlb != NULL );
    GDLFrame *frame = static_cast<GDLFrame *> (tlb->wxWidget);
    frame->SendWidgetTimerEvent(secs,widgetID);
  }
}

void GDLWidget::Realize( bool map)
{
  if( parentID == NullID)
  {
    assert( this->IsBase( ) );
#ifdef GDL_DEBUG_WIDGETS
    wxMessageOutputStderr( ).Printf( _T( "GDLWidget:Realize: %d\n"), this->widgetID );
#endif
    GDLFrame *frame = static_cast<GDLFrame*> (this->wxWidget);
    GDLApp* theGDLApp = new GDLApp;
    theGDLApp->OnInit();
    // add an idle event seems necessary for Linux (wxGTK2) and does not harm Windows either
    wxIdleEvent idlevent;
    theGDLApp->AddPendingEvent(idlevent);
    theGDLApp->OnRun();
    frame->SetTheApp(theGDLApp);
    if( frame->IsMapped() != map)
    {
      this->OnRealize( );
      if (map) frame->SendShowRequestEvent(); else frame->SendHideRequestEvent();
      }
    }
  else
  {
#ifdef GDL_DEBUG_WIDGETS
    wxMessageOutputStderr( ).Printf( _T( "GDLWidget:Realize TLB of: %d\n"), this->widgetID );
#endif
    GDLWidgetBase* tlb = GetTopLevelBaseWidget( parentID );
    assert( tlb != NULL );

    GDLFrame *frame = static_cast<GDLFrame *> (tlb->wxWidget);
    if( frame->IsMapped() != map)
    {
      this->OnRealize( );
      if (map) frame->SendShowRequestEvent(); else frame->SendHideRequestEvent();
    }
  }
}

bool GDLWidget::GetRealized() {
    GDLWidgetBase *tlb = GetTopLevelBaseWidget( widgetID );
    assert( tlb != NULL );
    GDLFrame* frame=static_cast<GDLFrame*>(tlb->GetWxWidget());
    return (frame->GetTheApp()!=NULL);
  }

#define GetSysC(x)   { col=wxSystemSettings::GetColour(x); r=col.Red();g=col.Green();b=col.Blue(); (*val)[0]=r;(*val)[1]=g;(*val)[2]=b; }
BaseGDL * GDLWidget::getSystemColours()
{
  DStructGDL* colo = new DStructGDL( "WIDGET_SYSTEM_COLORS");
  DIntGDL*  val=new DIntGDL( dimension(3), BaseGDL::NOZERO);
  int  r,g,b;
  wxColour col;
  GetSysC(wxSYS_COLOUR_3DDKSHADOW );
  colo->InitTag("DARK_SHADOW_3D",(*val));
  colo->InitTag("SHADOW_3D", (*val));
  GetSysC(wxSYS_COLOUR_3DLIGHT );
  colo->InitTag("FACE_3D", (*val));
  colo->InitTag("LIGHT_EDGE_3D", (*val));
  colo->InitTag("LIGHT_3D", (*val));
  GetSysC(wxSYS_COLOUR_ACTIVEBORDER  );
  colo->InitTag("ACTIVE_BORDER", (*val));
  GetSysC(wxSYS_COLOUR_ACTIVECAPTION   );
  colo->InitTag("ACTIVE_CAPTION", (*val));
  GetSysC(wxSYS_COLOUR_APPWORKSPACE    );
  colo->InitTag("APP_WORKSPACE", (*val));
  GetSysC(wxSYS_COLOUR_DESKTOP     );
  colo->InitTag("DESKTOP", (*val));
  GetSysC(wxSYS_COLOUR_BTNTEXT     );
  colo->InitTag("BUTTON_TEXT", (*val));
  GetSysC(wxSYS_COLOUR_CAPTIONTEXT     );
  colo->InitTag("CAPTION_TEXT", (*val));
  GetSysC(wxSYS_COLOUR_GRAYTEXT     );
  colo->InitTag("GRAY_TEXT", (*val));
  GetSysC(wxSYS_COLOUR_HIGHLIGHT     );
  colo->InitTag("HIGHLIGHT", (*val));
  GetSysC(wxSYS_COLOUR_HIGHLIGHTTEXT     );
  colo->InitTag("HIGHLIGHT_TEXT", (*val));
  GetSysC(wxSYS_COLOUR_INACTIVEBORDER     );
  colo->InitTag("INACTIVE_BORDER", (*val));
  GetSysC(wxSYS_COLOUR_INACTIVECAPTION     );
  colo->InitTag("INACTIVE_CAPTION", (*val));
  GetSysC(wxSYS_COLOUR_INACTIVECAPTIONTEXT     );
  colo->InitTag("INACTIVE_CAPTION_TEXT", (*val));
  GetSysC(wxSYS_COLOUR_INFOBK     );
  colo->InitTag("TOOLTIP_BK", (*val));
  GetSysC(wxSYS_COLOUR_INFOTEXT     );
  colo->InitTag("TOOLTIP_TEXT", (*val));
  GetSysC(wxSYS_COLOUR_MENU     );
  colo->InitTag("MENU", (*val));
  GetSysC(wxSYS_COLOUR_MENUTEXT     );
  colo->InitTag("MENU_TEXT", (*val));
  GetSysC(wxSYS_COLOUR_SCROLLBAR     );
  colo->InitTag("SCROLLBAR", (*val));
  GetSysC(wxSYS_COLOUR_WINDOW     );
  colo->InitTag("WINDOW_BK", (*val));
  GetSysC(wxSYS_COLOUR_WINDOWFRAME     );
  colo->InitTag("WINDOW_FRAME", (*val));
  GetSysC(wxSYS_COLOUR_WINDOWTEXT     );
  colo->InitTag("WINDOW_TEXT", (*val));
  return colo;
}

GDLWidget::~GDLWidget( ) {
  //TBD DESTROY CLEANLY EVERYTHING!!!
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "in ~GDLWidget(): " << widgetID << std::endl;
#endif

  assert( this->IsValid( ) ); //for debug
  if (!this->IsValid()) return; //to avoid crashing if asserts are not honored
  //unvalidate widget to prevent some further actions
  this->SetUnValid( );

  // call KILL_NOTIFY procedures
  this->OnKill( );
  
  // kill followers (here?)
  // delete all children (in reverse order ?)
  while (followers.size()) {
      GDLWidget* follower = GetWidget(followers[followers.size() - 1]);
      if (follower) delete follower;
      else followers.pop_back(); // Maybe should not be reachable
  }
  
  //   managed = false;
  DInt type = this->GetWidgetType( );
  bool badtype = (type == GDLWidget::WIDGET_BASE || type == GDLWidget::WIDGET_MBAR || type == GDLWidget::WIDGET_TAB);

  if ( scrollSizer != NULL ) {
    if ( badtype ) {
#ifdef GDL_DEBUG_WIDGETS
      cerr << "Warning, found and ignored a Scrolled Container." << endl;
#endif
    } else this->UnScrollWidget( );
  }
  if ( frameSizer != NULL ) {
    if ( badtype ) {
#ifdef GDL_DEBUG_WIDGETS
      cerr << "Warning, found and ignored a Framed Container." << endl;
#endif
    } else this->UnFrameWidget( );
  }
  
  //destroy, unless...
  if ( widgetType == GDLWidget::WIDGET_MBAR ) { //widget is a MBAR ---> do nothing? deleted with TLB
#ifdef GDL_DEBUG_WIDGETS
    std::cout << "in ~GDLWidget(): not destroying WIDGET_MBAR container" << widgetName << ": " << widgetID << endl;
#endif
  } else
  if ( widgetType == GDLWidget::WIDGET_TREE ) { //deleted elsewhere.
#ifdef GDL_DEBUG_WIDGETS
    std::cout << "in ~GDLWidget(): not destroying WIDGET_TREE container" << widgetName << ": " << widgetID << endl;
#endif    
  }  else if ( parentID != GDLWidget::NullID ) { //not the TLB
    //parent is a panel or a tab or a buttonmenu
    GDLWidget* gdlParent = GetWidget( parentID );
    assert( gdlParent != NULL );
    if ( gdlParent->IsContainer( ) ) {
#ifdef GDL_DEBUG_WIDGETS
      std::cout << "in ~GDLWidget(): destroy container-parent " << widgetName << ": " << widgetID << endl;
#endif
      GDLWidgetContainer* container = static_cast<GDLWidgetContainer*> (gdlParent);
      if(container) container->RemoveChild( widgetID );
      wxWindow *me = static_cast<wxWindow*> (this->GetWxWidget( ));
      if ( me ) { if ( gdlParent->IsTab( ) ) me->Hide( );
        else me->Destroy( ); } //do not delete the page, it will be removed by the notebook deletion!!!
    } else if ( gdlParent->IsButton( ) ) {
#ifdef GDL_DEBUG_WIDGETS
      std::cout << "in ~GDLWidget(): destroy button-container-parent " << widgetName << ": " << widgetID << endl;
#endif
      GDLWidgetButton* container = static_cast<GDLWidgetButton*> (gdlParent);
      if(container) container->RemoveChild( widgetID );
        wxWindow *me = static_cast<wxWindow*> (this->GetWxWidget( ));
        if ( me ) { me->Destroy( ); }
    } else {
#ifdef GDL_DEBUG_WIDGETS
      std::cout << "in ~GDLWidget(): destroy " << widgetName << ": " << widgetID << endl;
#endif
      wxWindow *me = static_cast<wxWindow*> (this->GetWxWidget( ));
      if ( me ) me->Destroy( ); 
#ifdef GDL_DEBUG_WIDGETS
      else cerr<<"Found unknown widget "<<widgetName << ": " << widgetID << endl;
#endif
    }
  }
#ifdef GDL_DEBUG_WIDGETS
  else std::cout << "in ~GDLWidget(): not destroying TLB " << widgetName << ": " << widgetID << endl;
#endif
  GDLDelete( uValue );
  GDLDelete( vValue );
  widgetList.erase( widgetID );
}

void GDLWidget::Raise()
{
  wxWindow * win;
  if( parentID == NullID)
  {
    assert( this->IsBase( ) );
    win = static_cast<wxWindow*> (this->wxWidget);
  }
  else
  {
    GDLWidgetBase* tlb = GetTopLevelBaseWidget( parentID );
    assert( tlb != NULL );

    win = static_cast<wxWindow *> (tlb->wxWidget);
  }
    if (win!=NULL) win->Raise();
}

void GDLWidget::Lower()
{
  wxWindow * win;
  if( parentID == NullID)
  {
    assert( this->IsBase( ) );
    win = static_cast<wxWindow*> (this->wxWidget);
  }
  else
  {
    GDLWidgetBase* tlb = GetTopLevelBaseWidget( parentID );
    assert( tlb != NULL );

    win = static_cast<wxWindow *> (tlb->wxWidget);
  }
    if (win!=NULL)  win->Lower();
}

DStructGDL* GDLWidget::GetGeometry( wxRealPoint fact ) {
  int ixs=0, iys=0, ixscr=0, iyscr=0;
  float xs, ys, xscr, yscr, xoff, yoff, margin=0;
  wxPoint position;
  wxWindow* test = static_cast<wxWindow*> (wxWidget);
  if ( test != NULL ) {
    test->GetClientSize( &ixs, &iys );
    ixscr=ixs;
    iyscr=iys;
    position = test->GetPosition( );
  }
  if (frameSizer != NULL) {framePanel->GetSize(&ixscr,&iyscr);  margin = gdlFRAME_MARGIN / fact.x;}
  if (scrollSizer != NULL) {scrollPanel->GetSize(&ixscr,&iyscr);ixs=ixscr-gdlSCROLL_WIDTH;iys=iyscr-gdlSCROLL_WIDTH;}
  //size is in pixels, pass in requested units (1.0 default)
  xs = ixs / fact.x;
  ys = iys / fact.y;
  xscr = ixscr / fact.x;
  yscr = iyscr / fact.y; 
  xoff = position.x / fact.x;
  yoff = position.y / fact.y;

  DStructGDL* ex = new DStructGDL( "WIDGET_GEOMETRY" );
  ex->InitTag( "XOFFSET", DFloatGDL( xoff) );
  ex->InitTag( "YOFFSET", DFloatGDL( yoff ) );
  ex->InitTag( "XSIZE", DFloatGDL( xs ) );
  ex->InitTag( "YSIZE", DFloatGDL( ys ) );
  ex->InitTag( "SCR_XSIZE", DFloatGDL( xscr ) );
  ex->InitTag( "SCR_YSIZE", DFloatGDL( yscr ) );
  ex->InitTag( "MARGIN", DFloatGDL( margin ) );
  return ex;
}

  GDLWidgetContainer::GDLWidgetContainer( WidgetIDT parentID, EnvT* e, ULong eventFlags_, bool map)
: GDLWidget( parentID, e, NULL, eventFlags_)
, map(map)
{
}
 GDLWidgetGraphicWindowBase::GDLWidgetGraphicWindowBase(WidgetIDT mbarID, int xoff, int yoff, DString title)
 : GDLWidgetBase(GDLWidget::NullID, NULL, (ULong) GDLWidget::EV_NONE,
    false,
    /*ref*/ mbarID, false,
    1, 0,
    BGNORMAL,
    true,
    "", "",
    title,
    "",
    0, 0,
    -1, -1, false, 0, 0, false, true),
   child(NULL)
{
   GDLFrame* f=static_cast<GDLFrame*>(wxWidget);
  f->SetPosition(wxPoint(xoff,yoff));
  f->Connect(f->GetId(), wxEVT_SIZE, gdlSIZE_EVENT_HANDLER);
}

/*********************************************************/
// for WIDGET_BASE
/*********************************************************/
GDLWidgetBase::GDLWidgetBase( WidgetIDT parentID, EnvT* e, ULong eventFlags_,
bool mapWid,
WidgetIDT& mBarIDInOut, bool modal_,
DLong col, DLong row,
int exclusiveMode_,
bool floating_,
const DString& resource_name, const DString& rname_mbar,
const DString& title_,
const DString& display_name,
DLong xpad_, DLong ypad_,
DLong x_scroll_size, DLong y_scroll_size, bool grid, long children_alignment, long space_, bool iscontextmenu, bool isagraphicwindow)
: GDLWidgetContainer( parentID, e, eventFlags_, mapWid)
, modal( modal_ )
, mbarID( mBarIDInOut )
, lastRadioSelection( NullID )
, nrows(row)
, ncols(col)
, stretchX(FALSE)
, stretchY(FALSE)
, childrenAlignment( children_alignment )
, space(space_)
, xpad(xpad_)
, ypad(ypad_)
, IsContextMenu(iscontextmenu)
{
  //Cannot find how to implement the notion of xpad and ypad in the sizer of the child widgets (GetBaseSizer).
  //Impossible to have different x and y borders for the panel itself, hence the following:
  int panelBorderFlag;
  int pad;
  if (xpad<1 && ypad<1) {panelBorderFlag=0;pad=0;}
  else if (xpad>0 && ypad>0) {panelBorderFlag=wxALL;pad=max(xpad,ypad);xpad=pad;ypad=pad;}
  else if (xpad>0 ) {panelBorderFlag=(wxLEFT|wxRIGHT);pad=xpad;}
  else{panelBorderFlag=(wxTOP|wxBOTTOM);pad=ypad;}
  
  // All bases can receive events: EV_CONTEXT, EV_KBRD_FOCUS, EV_TRACKING

  xmanActCom = false;
  //get immediately rid of scroll sizes in case of scroll or not... Here is the logic:
  if (x_scroll_size > 0) {scrolled=TRUE;x_scroll_size*=unitConversionFactor.x; x_scroll_size+=(gdlSCROLL_WIDTH);} 
  if (y_scroll_size > 0) {scrolled=TRUE;y_scroll_size*=unitConversionFactor.y; y_scroll_size+=(gdlSCROLL_WIDTH);}
  
  if ( xSize <= 0 ) {stretchX=TRUE; xSize=-1;} else xSize*=unitConversionFactor.x; 
  if ( ySize <= 0 ) {stretchY=TRUE; ySize=-1;} else ySize*=unitConversionFactor.y;

  // Set exclusiveMode
  // If exclusive then set to -1 to signal first radiobutton
  if ( exclusiveMode_ == BGEXCLUSIVE )
  this->SetExclusiveMode( BGEXCLUSIVE1ST );
  else
    this->SetExclusiveMode( exclusiveMode_ );

  // If first base widget = Top Level Base Widget:
  // can receive special events: tlb_size, tlb_move, tlb_icon and tlb_kill.
  if ( parentID == GDLWidget::NullID ) 
  {
    wxString titleWxString = wxString( title_.c_str( ), wxConvUTF8 );
    GDLFrame *gdlFrame; 
    if (xOffset>-1&&yOffset>-1) {
      gdlFrame = new GDLFrame( this, widgetID, titleWxString , isagraphicwindow, wxPoint(xOffset,yOffset));
    } else {
      gdlFrame = new GDLFrame( this, widgetID, titleWxString, isagraphicwindow);
    }
//it is the FRAME that manage all events. Here we dedicate particularly the tlb_* events:
// note that we have the choice for Size Event Handler for Frames, but need to change also is widgets.cpp
   if (eventFlags & GDLWidget::EV_SIZE ) gdlFrame->Connect(widgetID, wxEVT_SIZE, gdlSIZE_EVENT_HANDLER); 
   if (eventFlags & GDLWidget::EV_MOVE ) gdlFrame->Connect(widgetID, wxEVT_MOVE, wxMoveEventHandler(GDLFrame::OnMove));
   if (eventFlags & GDLWidget::EV_ICONIFY ) gdlFrame->Connect(widgetID, wxEVT_ICONIZE, wxIconizeEventHandler(GDLFrame::OnIconize)); 
   if (eventFlags & GDLWidget::EV_KILL ) {
     gdlFrame->Connect(widgetID, wxEVT_CLOSE_WINDOW, wxCloseEventHandler(GDLFrame::OnCloseFrame));
   } else gdlFrame->Connect(widgetID, wxEVT_CLOSE_WINDOW, wxCloseEventHandler(GDLFrame::OnUnhandledCloseFrame));
#ifdef GDL_DEBUG_WIDGETS
    gdlFrame->SetBackgroundColour(wxColour(255,0,255)); //violet
#endif
    
    wxWidget = gdlFrame;
    
    if( mbarID != 0)
    {
        GDLWidgetMBar* mBar = new GDLWidgetMBar( widgetID, e );
        mbarID = mBar->WidgetID( );
        mBarIDInOut = mbarID;
        wxMenuBar* me=static_cast<wxMenuBar*> (mBar->GetWxWidget( ));
        if (me) gdlFrame->SetMenuBar( me ); else cerr<<"Warning: GDLWidgetBase::GDLWidgetBase: Non-existent menubar widget!\n";
    }

    topWidgetSizer = new wxBoxSizer( wxVERTICAL);
    gdlFrame->SetSizer( topWidgetSizer );

    //create the first panel, fix size. Offset is not taken into account.
    widgetPanel = new wxPanel( gdlFrame, wxID_ANY , wxDefaultPosition, wxDefaultSize);// no FRAME here! , (frameWidth>0)?wxBORDER_SUNKEN:wxBORDER_NONE);
//note: wxWidget should probably be widgetPanel, not gdlFrame.
#ifdef GDL_DEBUG_WIDGETS
    widgetPanel->SetBackgroundColour(wxColour(255,0,0)); //rouge
#endif
    //give size hints for this panel. 
    if (!stretchX && !stretchY) widgetPanel->SetSizeHints(xSize, ySize);
    else if (!stretchX) widgetPanel->SetSizeHints(xSize, -1);
    else if (!stretchY) widgetPanel->SetSizeHints(-1, ySize);

    //force frame to wrap around panel
    topWidgetSizer->Add(widgetPanel, 1, widgetAlignment()|panelBorderFlag|wxALL,(frameWidth==-1)?gdlFRAME_MARGIN:frameWidth);

    //I cannot succeed to have a correct size if the
    //base sizer is gridbag and no sizes are given. Thus in the case xsize=ysize=undefined and col=0,
    //I force col=1.
    if (stretchX && stretchY && ncols==0 && nrows==0) ncols=1;
    //Allocate the sizer for children according to col or row layout
    widgetSizer = GetBaseSizer( ncols, nrows, grid , space);
    //Attach sizer to panel
    widgetPanel->SetSizer( widgetSizer );
    if (scrolled) {
      scrollPanel = new wxScrolledWindow(gdlFrame, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
#ifdef GDL_DEBUG_WIDGETS
      scrollPanel->SetBackgroundColour(wxColour(0,255,255)); //bleu clair
#endif
      scrollSizer = new wxBoxSizer(wxVERTICAL );
      scrollPanel->SetSizer( scrollSizer );
      scrollPanel->SetSizeHints((x_scroll_size>0)?x_scroll_size:gdlDEFAULT_SCROLL_SIZE,(y_scroll_size>0)?y_scroll_size:gdlDEFAULT_SCROLL_SIZE);

      topWidgetSizer->Detach(widgetPanel);
      widgetPanel->Reparent(scrollPanel);
      scrollSizer->Add(widgetPanel);
      topWidgetSizer->Add(scrollPanel, 0, wxEXPAND|widgetAlignment()|panelBorderFlag,(frameWidth==-1)?gdlFRAME_MARGIN:frameWidth);
      scrollPanel->SetScrollRate(gdlSCROLL_RATE,gdlSCROLL_RATE); //show scrollbars
    }
    TIDY_WIDGET;
    UPDATE_WINDOW;
  } 
  else if ( IsContextMenu ) //Note: THIS IS WRONG. We should only add a contextMenu using the 'PopupMenu' member function of a wxWindow. 
    // Current implementation as poputptransientwindow makes very different results, in IDL the widgets of a context_menu are only of the "menu" type.
  {
    GDLWidget* gdlParent = GetWidget( parentID );
    assert( gdlParent != NULL);
    wxWindow* parentWindow=static_cast<wxWindow*>(gdlParent->GetWxWidget());
    assert( parentWindow != NULL);

    wxPopupTransientWindow* transient = new wxPopupTransientWindow(parentWindow );
    
    wxWidget = transient;
    topWidgetSizer = new wxBoxSizer( wxVERTICAL);
    transient->SetSizer( topWidgetSizer);
    
    widgetPanel = new wxPanel( transient, widgetID , wxDefaultPosition, wxDefaultSize); // no frame also! , (frameWidth>0)?wxBORDER_SUNKEN:wxBORDER_NONE);
#ifdef GDL_DEBUG_WIDGETS
    widgetPanel->SetBackgroundColour(wxColour(255,0,0)); //rouge
#endif
    //give size hints for this panel. 
    if (!stretchX && !stretchY) widgetPanel->SetSizeHints(xSize, ySize);
    else if (!stretchX) widgetPanel->SetSizeHints(xSize, -1);
    else if (!stretchY) widgetPanel->SetSizeHints(-1, ySize);

    //force frame to wrap around panel
    topWidgetSizer->Add( widgetPanel, 1, wxEXPAND|wxALL, (frameWidth==-1)?gdlFRAME_MARGIN:frameWidth);

    //I cannot succeed to have a correct size if the
    //base sizer is gridbag and no sizes are given. Thus in the case xsize=ysize=undefined and col=0,
    //I force col=1.
    if (stretchX && stretchY && ncols==0 && nrows==0) ncols=1;
    //Allocate the sizer for children according to col or row layout
    widgetSizer = GetBaseSizer( ncols, nrows, grid , space);
    //Attach sizer to panel
    widgetPanel->SetSizer( widgetSizer );
//transients are not like that in IDL. They are much simpler. Anyway, for the moment, let it be like that  
    if (scrolled) {
      scrollPanel = new wxScrolledWindow(transient, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
#ifdef GDL_DEBUG_WIDGETS
      scrollPanel->SetBackgroundColour(wxColour(0,255,255)); //bleu clair
#endif
      scrollSizer = new wxBoxSizer(wxVERTICAL );
      scrollPanel->SetSizer( scrollSizer );
      scrollPanel->SetSizeHints((x_scroll_size>0)?x_scroll_size:gdlDEFAULT_SCROLL_SIZE,(y_scroll_size>0)?y_scroll_size:gdlDEFAULT_SCROLL_SIZE);

      topWidgetSizer->Detach(widgetPanel);
      widgetPanel->Reparent(scrollPanel);
      scrollSizer->Add(widgetPanel);
      topWidgetSizer->Add(scrollPanel, 0, wxEXPAND|wxALL,(frameWidth==-1)?gdlFRAME_MARGIN:frameWidth);
      scrollPanel->SetScrollRate(gdlSCROLL_RATE, gdlSCROLL_RATE); //show scrollbars
    }
    TIDY_WIDGET;
    UPDATE_WINDOW;
  } 
  else 
  {
    // If parent base widget exists ....
    GDLWidget* gdlParent = GetWidget( parentID );
    assert( gdlParent != NULL);
    wxSizer* parentSizer = gdlParent->GetSizer( );

    if( gdlParent->IsTab())
    {
      GDLWidgetTab* parentTab = static_cast<GDLWidgetTab*> (gdlParent);
      assert( parentTab != NULL);

      wxNotebook* wxParent = static_cast<wxNotebook*> (parentTab->GetWxWidget( ));
      assert( wxParent != NULL);

      //create the panel, fix size. Offset is not taken into account unless we create an intermediate container panel.
      widgetPanel = new wxPanel( wxParent, widgetID , wxPoint(xOffset,yOffset), wxDefaultSize, (frameWidth>0)?wxBORDER_SUNKEN:wxBORDER_NONE);
#ifdef GDL_DEBUG_WIDGETS
      widgetPanel->SetBackgroundColour(wxColour(64,128,33)); //for tests!
#endif
      wxWidget = widgetPanel;
      //give size hints for this panel. 
      if (!stretchX && !stretchY) widgetPanel->SetSizeHints(xSize, ySize);
      else if (!stretchX) widgetPanel->SetSizeHints(xSize, -1);
      else if (!stretchY) widgetPanel->SetSizeHints(-1, ySize);
    
      //if no cols or rows are given, MUST at least fix one IF wxWidgets 3
      if (ncols==0 && nrows==0) ncols=1;
      //Allocate the sizer for children according to col or row layout
      widgetSizer = GetBaseSizer( ncols, nrows, grid, space );
      //Attach sizer to panel
      widgetPanel->SetSizer( widgetSizer );
    
      wxString titleWxString = wxString( title_.c_str( ), wxConvUTF8 );
      //TAB-PARENT WIDGET CANNOT BE SCROLLED (crash)!
//      if (scrolled) {
//        scrollPanel = new wxScrolledWindow(wxParent, wxID_ANY, wxPoint(xOffset,yOffset), wxDefaultSize, wxBORDER_SUNKEN);
//#ifdef GDL_DEBUG_WIDGETS
//        scrollPanel->SetBackgroundColour(wxColour(0,255,255)); //bleu clair
//#endif
//        scrollPanel->SetScrollRate(gdlSCROLL_WIDTH, gdlSCROLL_WIDTH); //show scrollbars
//        scrollSizer = new wxBoxSizer(wxVERTICAL );
//        scrollPanel->SetSizer( scrollSizer );
//        scrollPanel->SetSizeHints((x_scroll_size>0)?x_scroll_size:DEFAULT_SCROLL_SIZE,(y_scroll_size>0)?y_scroll_size:DEFAULT_SCROLL_SIZE);
//
//        widgetPanel->Reparent(scrollPanel);
//        scrollSizer->Add(widgetPanel);
//        wxParent->AddPage(scrollPanel, titleWxString );
//      } 
//      else
        wxParent->AddPage( widgetPanel, titleWxString );
    TIDY_WIDGET;
    UPDATE_WINDOW;
    }
    else
    {
      wxWindow* wxParent = static_cast<wxWindow*> (gdlParent->GetPanel());
      assert( wxParent != NULL);
      widgetPanel = new wxPanel( wxParent, widgetID , wxPoint(xOffset,yOffset), wxDefaultSize, (frameWidth>0)?wxBORDER_SUNKEN:wxBORDER_NONE);
#ifdef GDL_DEBUG_WIDGETS
      widgetPanel->SetBackgroundColour(wxColour(255,255,0)); //jaune
#endif
      wxWidget = widgetPanel;
      //give size hints for this panel. 
      if (!stretchX && !stretchY) widgetPanel->SetSizeHints(xSize, ySize);
      else if (!stretchX) widgetPanel->SetSizeHints(xSize, -1);
      else if (!stretchY) widgetPanel->SetSizeHints(-1, ySize);

      parentSizer->Add( widgetPanel, 0, widgetAlignment()|panelBorderFlag, (frameWidth==-1)?gdlFRAME_MARGIN:frameWidth);

      //if no cols or rows are given, MUST at least fix one IF wxWidgets 3
      if (ncols==0 && nrows==0) ncols=1;
      //Allocate the sizer for children according to col or row layout
      widgetSizer = GetBaseSizer( ncols, nrows, grid, space );
      //Attach sizer to panel
      widgetPanel->SetSizer( widgetSizer );
      if (scrolled) {
        scrollPanel = new wxScrolledWindow(wxParent, wxID_ANY, wxPoint(xOffset,yOffset), wxDefaultSize, wxBORDER_SUNKEN);
#ifdef GDL_DEBUG_WIDGETS
        scrollPanel->SetBackgroundColour(wxColour(0,255,255)); //bleu clair
#endif
        scrollSizer = new wxBoxSizer(wxVERTICAL );
        scrollPanel->SetSizer( scrollSizer );
        scrollPanel->SetSizeHints((x_scroll_size>0)?x_scroll_size:gdlDEFAULT_SCROLL_SIZE,(y_scroll_size>0)?y_scroll_size:gdlDEFAULT_SCROLL_SIZE);

        parentSizer->Detach(widgetPanel);
        widgetPanel->Reparent(scrollPanel);
        scrollSizer->Add(widgetPanel);
        parentSizer->Add(scrollPanel, 0, widgetAlignment()|panelBorderFlag, (frameWidth==-1)?gdlFRAME_MARGIN:frameWidth);
        scrollPanel->SetScrollRate(gdlSCROLL_RATE, gdlSCROLL_RATE); //show scrollbars
      }
    TIDY_WIDGET;
    UPDATE_WINDOW;
    }
  }
}
DStructGDL* GDLWidgetBase::GetGeometry( wxRealPoint fact ) {
  int ixs=0, iys=0, ixscr=0, iyscr=0, imargin=0;
  long ixpad=0, iypad=0, ispace=0;
  float xs, ys, xscr, yscr, xoff, yoff, margin, xpad, ypad, space;
  wxPoint position;
  wxWindow* test = static_cast<wxWindow*> (wxWidget);
  if ( test != NULL ) {
    test->GetClientSize( &ixs, &iys );
    ixscr=ixs;
    iyscr=iys;
    position = test->GetPosition( );
    ixpad=this->getXPad(); iypad=this->getYPad(); ispace=this->getSpace();
  }
  if (frameSizer != NULL) framePanel->GetSize(&ixscr,&iyscr);
  if (scrollSizer != NULL) {scrollPanel->GetSize(&ixscr,&iyscr);ixs=ixscr-gdlSCROLL_WIDTH;iys=iyscr-gdlSCROLL_WIDTH;}
  //size is in pixels, pass in requested units (1.0 default)
  xs = ixs / fact.x;
  ys = iys / fact.y;
  xscr = ixscr / fact.x;
  yscr = iyscr / fact.y; 
  xoff = position.x / fact.x;
  yoff = position.y / fact.y;
  xpad = ixpad / fact.x;
  ypad = iypad / fact.y;
  space = ispace / fact.x;
  margin = imargin / fact.x;
  DStructGDL* ex = new DStructGDL( "WIDGET_GEOMETRY" );
  ex->InitTag( "XOFFSET", DFloatGDL( xoff) );
  ex->InitTag( "YOFFSET", DFloatGDL( yoff ) );
  ex->InitTag( "XSIZE", DFloatGDL( xs ) );
  ex->InitTag( "YSIZE", DFloatGDL( ys ) );
  ex->InitTag( "SCR_XSIZE", DFloatGDL( xscr ) );
  ex->InitTag( "SCR_YSIZE", DFloatGDL( yscr ) );
  ex->InitTag( "MARGIN", DFloatGDL( margin ) );
  ex->InitTag( "XPAD", DFloatGDL( xpad ) );
  ex->InitTag( "YPAD", DFloatGDL( ypad ) );
  ex->InitTag( "SPACE", DFloatGDL( space ) );
  return ex;
}

GDLWidgetBase::~GDLWidgetBase()
{
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetBase(): " << widgetID << std::endl;
#endif

  // delete all children (in reverse order ?)
  while (children.size()) {
	  GDLWidget* child = GetWidget( children[children.size()-1]);
	  if (child) delete child;
	  else children.pop_back(); // Maybe should not be reachable
  }
  
  // remove all widgets still in the queue for current TLB
  eventQueue.Purge( widgetID);
  readlineEventQueue.Purge( widgetID);
  
  if( this->parentID == GDLWidget::NullID)
  {
      // Close widget frame (might be already closed)
      if( static_cast<GDLFrame*>(this->wxWidget) != NULL)
      {
        static_cast<GDLFrame*> (this->wxWidget)->NullGDLOwner( );
        delete static_cast<GDLFrame*> (this->wxWidget); //closes the frame etc.
      }
      //IMPORTANT: unxregister TLB if was managed 
      if (this->GetManaged()) CallEventPro( "UNXREGISTER" , new DLongGDL(widgetID));

      //send RIP 
        // create GDL event struct
        DStructGDL* ev = new DStructGDL( "*TOPLEVEL_DESTROYED*" );
        ev->InitTag( "ID", DLongGDL( widgetID ) );
        ev->InitTag( "TOP", DLongGDL( widgetID ) );
        ev->InitTag( "HANDLER", DLongGDL( 0 ) );
        ev->InitTag( "MESSAGE", DLongGDL( 0 ) );
        if ( this->GetXmanagerActiveCommand( ) || !this->GetManaged() ){
          readlineEventQueue.PushFront( ev ); // push front (will be handled next)
        } else {
          eventQueue.PushFront( ev ); // push front (will be handled next)
        }

  }
}


// called from event handling thread
// sends a destroy event for itself
void GDLWidgetBase::SelfDestroy()
{
  assert( parentID == NullID );
  // create GDL event struct
  DStructGDL* ev = new DStructGDL( "*WIDGET_DESTROY*" );
  ev->InitTag( "ID", DLongGDL( widgetID ) );
  ev->InitTag( "TOP", DLongGDL( widgetID ) );
  ev->InitTag( "HANDLER", DLongGDL( 0 ) );
  ev->InitTag( "MESSAGE", DLongGDL( 0 ) );
  if ( this->GetXmanagerActiveCommand( ) || !this->GetManaged() ){
    readlineEventQueue.PushFront( ev ); // push front (will be handled next)
  } else {
    eventQueue.PushFront( ev ); // push front (will be handled next)
  }
}
  void GDLWidgetBase::mapBase(bool val){
    wxWindow* me=static_cast<wxWindow*>(wxWidget);
    if (me) me->Show(val); else {cerr<<"Warning: GDLWidgetBase::mapBase(): Non-existent widget!\n"; return;}
    me->Update(); //immediate
    if (val) this->RefreshWidget();
  }
//Children of a column-oriented base are plotted in successive columns in IDL, whic is not the case for wxWidgets.
  void GDLWidgetBase::ReorderWidgets()  
  {
    wxGridSizer* s=static_cast<wxGridSizer*>(widgetSizer);
    
    int ncols=s->GetCols();//GetEffectiveColsCount();
    int nrows=s->GetRows();//GetEffectiveRowsCount();
    int nchild=widgetPanel->GetChildren().GetCount();
    if (ncols>1 && nchild>ncols) {
    nrows=nchild/ncols;
    if (nrows*ncols<nchild) nrows++;
 
      wxWindowList childrenList=widgetPanel->GetChildren();
      wxWindowList::iterator iter;
      for (iter = childrenList.begin(); iter != childrenList.end(); ++iter)
      {
          wxWindow* w=(*iter);
          s->Detach(*iter); 
      }
      s->SetCols(0); s->SetRows(nrows);
      for (int i = 0; i < nrows; i++) 
      {
        for (int j = 0; j < ncols; j++)
        {
          int index=j*nrows+i;
          if (index<nchild) {
            wxWindow* w=childrenList[index];
            s->Add(w); 
          }
        }
      }
      widgetSizer->Layout();
      widgetPanel->Refresh();
    }
  }
/*********************************************************/
// for WIDGET_TAB
/*********************************************************/
GDLWidgetTab::GDLWidgetTab( WidgetIDT p, EnvT* e, ULong eventFlags_, DLong location, DLong multiline )
: GDLWidgetContainer( p, e, eventFlags ) {

  GDLWidget* gdlParent = GetWidget( parentID );

  widgetPanel =  gdlParent->GetPanel( );
  widgetSizer = gdlParent->GetSizer( );
  topWidgetSizer = this->GetTopLevelBaseWidget(parentID)->GetSizer();
  
  long style = wxNB_TOP;
  if ( location == 1 ) style = wxNB_BOTTOM;
  if ( location == 2 ) style = wxNB_LEFT;
  if ( location == 3 ) style = wxNB_RIGHT;

  if ( multiline != 0 )
    style |= wxNB_MULTILINE;

  wxNotebook * notebook = new wxNotebook( widgetPanel, widgetID,
  wxPoint( xOffset, yOffset ),
  computeWidgetSize( ),
  style );
  this->wxWidget = notebook;
  notebook->Connect(widgetID,wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED,wxNotebookEventHandler(GDLFrame::OnPageChanged));
  widgetStyle=widgetAlignment();
  widgetSizer->Add( notebook, 0, widgetStyle, 0 ); 
  widgetSizer->Layout();
  TIDY_WIDGET;
  UPDATE_WINDOW
}

BaseGDL* GDLWidgetTab::GetTabNumber(){
  wxNotebook * notebook=static_cast<wxNotebook*>(wxWidget);
  assert( notebook != NULL);
  return new DIntGDL(notebook->GetPageCount());
}

BaseGDL* GDLWidgetTab::GetTabCurrent(){
  wxNotebook * notebook=static_cast<wxNotebook*>(wxWidget);
  assert( notebook != NULL);
  return new DIntGDL(notebook->GetSelection());
}

void GDLWidgetTab::SetTabCurrent(int val){
  wxNotebook * notebook=static_cast<wxNotebook*>(wxWidget);
  assert( notebook != NULL);
  if (val<notebook->GetPageCount()){
//   notebook->GetPage(val)->Raise();    
   notebook->ChangeSelection(val);
  }
}

BaseGDL* GDLWidgetTab::GetTabMultiline(){
  wxNotebook * notebook=static_cast<wxNotebook*>(wxWidget);
  assert( notebook != NULL);
  return new DIntGDL(notebook->GetExtraStyle()&wxNB_MULTILINE);
}

GDLWidgetTab::~GDLWidgetTab(){
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "in ~GDLWidgetTab(): " << widgetID << std::endl;
#endif
  // delete all children (in reverse order ?)
  while (children.size()) {
      GDLWidget* child = GetWidget(children[children.size() - 1]);
      if (child) delete child;
      else children.pop_back(); // Maybe should not be reachable
      }
}



/*********************************************************/
// for WIDGET_TABLE
/*********************************************************/
//overrides method to label the columns & lines
  wxString wxGridTableBase::GetRowLabelValue( int row )
{
    wxString s;

    // RD: Starting the rows at zero confuses users,
    // no matter how much it makes sense to us geeks.
    // GD: So IDL and GDL are for geeks.
    s << row ;

    return s;
}
  wxString wxGridTableBase::GetColLabelValue( int col )
{
    wxString s;
    s << col ;

    return s;
}

GDLWidgetTable::GDLWidgetTable( WidgetIDT p, EnvT* e,
DByteGDL* alignment_,
DStringGDL* amPm_,
DByteGDL* backgroundColor_,
DByteGDL* foregroundColor_,
DStringGDL* columnLabels_,
int majority_,
DLongGDL* columnWidth_,
DStringGDL* daysOfWeek_,
bool disjointSelection_,
DByteGDL* editable_,
DStringGDL* format_,
//bool ignoreAccelerators_,
DStringGDL* month_,
bool noColumnHeaders_,
bool noRowHeaders_,
bool resizeableColumns_,
bool resizeableRows_,
DLongGDL* rowHeights_,
DStringGDL* rowLabels_,
//DLong tabMode_,
BaseGDL* value_,
DLong xScrollSize_,
DLong yScrollSize_,
DStringGDL* valueAsStrings_,
DULong eventFlags_
)
: GDLWidget( p, e, value_, eventFlags_ )
, table_alignment( alignment_ )
, amPm( amPm_ )
, backgroundColor( backgroundColor_ )
, foregroundColor( foregroundColor_ )
, columnLabels( columnLabels_ )
, majority ( majority_ )
, columnWidth( columnWidth_ )
, daysOfWeek( daysOfWeek_ )
, disjointSelection( disjointSelection_ )
, editable( editable_)
, format( format_ )
//, ignoreAccelerators( ignoreAccelerators_ )
, month( month_ )
, noColumnHeaders( noColumnHeaders_ )
, noRowHeaders( noRowHeaders_ )
, resizeableColumns( resizeableColumns_ )
, resizeableRows( resizeableRows_ )
, rowHeights( rowHeights_ )
, rowLabels( rowLabels_ )
//, tabMode( tabMode_ )
, x_scroll_size( xScrollSize_ )
, y_scroll_size( yScrollSize_)
, valueAsStrings( valueAsStrings_ )
, updating(FALSE)
{
  GDLWidget* gdlParent = GetWidget( parentID );
  wxPanel *panel = gdlParent->GetPanel( );
  widgetPanel = panel;
  widgetSizer = gdlParent->GetSizer( );
  topWidgetSizer = this->GetTopLevelBaseWidget(parentID)->GetSizer();

//at this stage, valueAsStrings is OK dim 1 or 2 BUT vVALUE MAY BE NULL!
SizeT numRows,numCols;
if (valueAsStrings->Rank()==1) {
  numRows=1;
  numCols=valueAsStrings->Dim(0); //lines
} else {
  numRows=valueAsStrings->Dim(1);
  numCols=valueAsStrings->Dim(0);
}
SizeT grid_nrows=(ySize<=0)?numRows:ySize;
SizeT grid_ncols=(xSize<=0)?numCols:xSize;
  
  gdlGrid *grid = new gdlGrid( widgetPanel, widgetID);
//important:set wxWidget here.
  this->wxWidget = grid;  
//Column Width Before creating
bool hasColumnWidth=(columnWidth!=NULL);
if (hasColumnWidth) { //one value set for all?
  if (columnWidth->N_Elements()==1) {
    grid->SetDefaultColSize((*columnWidth)[0]*unitConversionFactor.x) ;
    hasColumnWidth=FALSE;
  }
}
//RowHeight
bool hasRowHeights=(rowHeights!=NULL);
if (hasRowHeights) { //one value set for all?
  if (rowHeights->N_Elements()==1) {
    grid->SetDefaultRowSize((*rowHeights)[0]*unitConversionFactor.y) ;
    hasRowHeights=FALSE;
  }
}
//Alignment
bool hasAlignment=(table_alignment!=NULL);
if (hasAlignment) {
  if (table_alignment->N_Elements()==1) { //singleton case
    switch( (*table_alignment)[0] ){
      case 0:
        grid->SetDefaultCellAlignment(wxALIGN_LEFT,wxALIGN_CENTRE); break;
      case 1:
        grid->SetDefaultCellAlignment(wxALIGN_CENTRE,wxALIGN_CENTRE); break;
      case 2:
        grid->SetDefaultCellAlignment(wxALIGN_RIGHT,wxALIGN_CENTRE);
    }
    hasAlignment=FALSE;
  }
}
//General Editability
bool isEditable=(editable!=NULL);
if (isEditable) {
  if (editable->N_Elements()==1) { //singleton case
    if ((*editable)[0]==0) isEditable=FALSE;
    else {grid->EnableEditing(TRUE); isEditable=FALSE;}
  }
} else grid->EnableEditing(FALSE); 
if (isEditable) grid->EnableEditing(TRUE); //since now isEditable means "individually editable", which needs global editing set.
//Single Background Colour
bool isBackgroundColored=(backgroundColor!=NULL);
if (isBackgroundColored) { //one value set for all?
  if (backgroundColor->N_Elements()==3) { 
    grid->SetDefaultCellBackgroundColour(wxColour((*backgroundColor)[0],(*backgroundColor)[1],(*backgroundColor)[2])) ;  
    isBackgroundColored=FALSE;
  }
}
//Single Text Colour
bool isForegroundColored=(foregroundColor!=NULL);
if (isForegroundColored) { //one value set for all?
  if (foregroundColor->N_Elements()==3) {
    grid->SetDefaultCellTextColour(wxColour((*foregroundColor)[0],(*foregroundColor)[1],(*foregroundColor)[2])) ;
    isForegroundColored=FALSE;
  }
}
//No column Headers
if (noColumnHeaders) grid->SetColLabelSize(0);
//No row Headers
if (noRowHeaders) grid->SetRowLabelSize(0);
//end General Setup
int selmode = wxGrid::wxGridSelectCells; //wxWidgets's modes do not reflect IDL's. We trick the selections according to modes in eventhandler.
if (!resizeableColumns) grid->DisableDragColSize();
if (!resizeableRows) grid->DisableDragRowSize();
grid->CreateGrid( grid_nrows, grid_ncols, static_cast<wxGrid::wxGridSelectionModes>(selmode));
// Set grid cell contents as strings. Note that there may be less or more cells than valueAsStrings, due to possibly different xSize,ySize :

      for ( int ival=0, i=0; i<grid_nrows; ++i, ++ival) for (int jval=0, j=0; j<grid_ncols; ++j, ++jval)
      {
        if (ival < numRows && jval < numCols ) grid->SetCellValue( i, j ,wxString(((*valueAsStrings)[jval*numRows+ival]).c_str(), wxConvUTF8 ) ); 
      }

//Editability
//take too long as soon as table has a more than a few elements. Fixme!
//if (isEditable) {SizeT k=0; for (SizeT irow=0; irow< grid_nrows; ++irow) for (SizeT icol=0; icol< grid_ncols; ++icol) {grid->SetReadOnly( irow, icol, ((*editable)[k%editable->N_Elements()]==0));++k;}}
//colors per element
if (isBackgroundColored) this->DoBackgroundColor();
if (isForegroundColored) this->DoForegroundColor();
if (hasColumnWidth) this->DoColumnWidth();
if (hasRowHeights) this->DoRowHeights();
//treat other alignment cases.
if (hasAlignment) this->DoAlign();

if (columnLabels!=NULL)this->DoColumnLabels();
if (rowLabels!=NULL) this->DoRowLabels();

//get back on sizes. Do we enforce some size or scroll_size, in columns/rows:
int currentColLabelHeight = grid->GetColLabelSize();
int currentRowLabelWidth = grid->GetRowLabelSize();

int fullsizex=currentRowLabelWidth;
int fullsizey=currentColLabelHeight; 
for (SizeT i=0; i< numCols ; ++i) fullsizex+=(i<grid_ncols)?grid->GetColSize(i):grid->GetDefaultColSize(); 
for (SizeT j=0; j< numRows ; ++j) fullsizey+=(j<grid_nrows)?grid->GetRowHeight(j):grid->GetDefaultRowSize(); 

int visiblesizex=currentRowLabelWidth;
int visiblesizey=currentColLabelHeight; 
for (SizeT i=0; i< grid_ncols ; ++i) visiblesizex+=grid->GetColSize(i); 
for (SizeT j=0; j< grid_nrows ; ++j) visiblesizey+=grid->GetRowHeight(j); 

int sizex=-1;
int sizey=-1;
int scr_sizex=-1;
int scr_sizey=-1;
  if ( xSize > 0 ) { sizex = visiblesizex; //size in columns given
  } else {sizex=fullsizex;}
  if ( ySize > 0 ) { sizey = visiblesizey; //size in rows given
  } else {sizey=fullsizey;}
  if ( x_scroll_size > 0 ) { //scroll size is in columns
    scrolled=TRUE;
    scr_sizex=currentRowLabelWidth+gdlSCROLL_WIDTH;
    for (SizeT i=0; i< x_scroll_size ; ++i) scr_sizex+=grid->GetColSize(i);
    scr_sizex=min(scr_sizex,fullsizex);
    if (y_scroll_size <=0) y_scroll_size=x_scroll_size;
  }
  if ( y_scroll_size > 0 ) { //rows
    scrolled=TRUE;
    scr_sizey=currentColLabelHeight+gdlSCROLL_WIDTH;
    for (SizeT j=0; j< y_scroll_size ; ++j) scr_sizey+=grid->GetRowHeight(j);
    scr_sizey=min(scr_sizey,fullsizey);
  }
//fix size if relevant
if (scrolled && scr_sizex == -1) scr_sizex = (sizex>0)?sizex:fullsizex;
if (scrolled && scr_sizey == -1) scr_sizey = (sizey>0)?sizey:fullsizey;
//scrXSize etc to be considered since sizes are not in pixels:
if (scrXSize>0) {if (scrolled) scr_sizex=scrXSize; else sizex=scrXSize;}  
if (scrYSize>0) {if (scrolled) scr_sizey=scrYSize; else sizey=scrYSize;}  
//wxGrid IS a scrolled window
if (scrolled) {
  grid->SetInitialSize(wxSize(scr_sizex, scr_sizey)); 
} else {
  if (xSize>0||ySize>0) grid->SetInitialSize(wxSize(sizex,sizey)); 
}
grid->SetScrollLineX(grid->GetColSize(0));
grid->SetScrollLineY(grid->GetRowHeight(0));
widgetStyle=widgetAlignment();
widgetSizer->Add(grid, 0, widgetStyle, 0 );
if ((frameWidth>0)) this->FrameWidget();
TIDY_WIDGET;
UPDATE_WINDOW
}

bool GDLWidgetTable::IsSomethingSelected(){
  gdlGrid * grid = static_cast<gdlGrid*> (wxWidget);
  assert( grid != NULL);
  return grid->IsSomethingSelected();
}

DLongGDL* GDLWidgetTable::GetSelection( ) {
  gdlGrid * grid = static_cast<gdlGrid*> (wxWidget);
  assert( grid != NULL);
  SizeT k = 0;
  DLongGDL * sel;
  std::vector<wxPoint> list = grid->GetSelectedDisjointCellsList( );
  if ( disjointSelection ) { //pairs lists
    if ( list.size( ) < 1 ) {sel = new DLongGDL( 2, BaseGDL::ZERO ); sel->Dec(); return sel;} //returns [-1,-1] if nothing selected
    SizeT dims[2];
    dims[0] = 2;
    dims[1] = list.size( );
    dimension dim( dims, 2 );
    sel = new DLongGDL( dim );
    for ( std::vector<wxPoint>::iterator it = list.begin( ); it != list.end( ); ++it ) {
      (*sel)[k++] = (*it).y;
      (*sel)[k++] = (*it).x;
    }
  } else { //4 values
    wxGridCellCoordsArray selectionTL = grid->GetSelectionBlockTopLeft( );
    wxGridCellCoordsArray selectionBR = grid->GetSelectionBlockBottomRight( );
    sel = new DLongGDL( 4, BaseGDL::ZERO ); sel->Dec(); //will return [-1,-1,-1,-1] if nothing selected
    if (!selectionTL.IsEmpty() && !selectionBR.IsEmpty()){ //ok with a block...
      //LEFT TOP BOTTOM RIGHT
      (*sel)[0] = selectionTL[0].GetCol( );
      (*sel)[1] = selectionTL[0].GetRow( );
      (*sel)[2] = selectionBR[0].GetCol( );
      (*sel)[3] = selectionBR[0].GetRow( );   
    } else {
      //try columns, rows, and singletons
      wxArrayInt selectionRow=grid->GetSelectedRows();
      wxArrayInt selectionCol=grid->GetSelectedCols();
      if ( selectionRow.GetCount() >0 ) {
        (*sel)[0] = 0;
        (*sel)[1] = selectionRow[0];
        (*sel)[2] = grid->GetNumberCols()-1;
        (*sel)[3] = selectionRow[selectionRow.GetCount()-1];
      } else if ( selectionCol.GetCount() >0 ) {
        (*sel)[0] = selectionCol[0];
        (*sel)[1] = 0;
        (*sel)[2] = selectionCol[selectionCol.GetCount()-1];
        (*sel)[3] = grid->GetNumberRows()-1;
      } else {
        wxGridCellCoordsArray cellSelection = grid->GetSelectedCells( );
        if (cellSelection.size()>0) {
         int row = cellSelection[0].GetRow();
         int col = cellSelection[0].GetCol();
        (*sel)[0] = col;
        (*sel)[1] = row;
        (*sel)[2] = col;
        (*sel)[3] = row;
        }
      }
      return sel;
    }

  }
  return sel;
}

void GDLWidgetTable::ClearSelection()
{
  gdlGrid * grid = static_cast<gdlGrid*> (wxWidget);
  assert( grid != NULL);
  updating=TRUE;
  grid->ClearSelection();
  updating=FALSE;
UPDATE_WINDOW
}

void GDLWidgetTable::DoAlign() {
  if (table_alignment->N_Elements( )==0) {return;}
  gdlGrid * grid = static_cast<gdlGrid*> (wxWidget);
  assert( grid != NULL);
  int nRows = grid->GetNumberRows( );
  int nCols = grid->GetNumberCols( );
  SizeT k = 0;
  grid->BeginBatch();
  for ( SizeT i = 0; i < nRows; ++i ) {
    for ( SizeT j = 0; j < nCols; ++j ) {
      switch ( (*table_alignment)[k % table_alignment->N_Elements( )] ) {
        case 0:
          grid->SetCellAlignment( i, j, wxALIGN_LEFT, wxALIGN_CENTRE );
          break;
        case 1:
          grid->SetCellAlignment( i, j, wxALIGN_CENTRE, wxALIGN_CENTRE );
          break;
        case 2:
          grid->SetCellAlignment( i, j, wxALIGN_RIGHT, wxALIGN_CENTRE );
      }
      k++;
      if ( table_alignment->N_Elements( ) > 1 ) if ( k == table_alignment->N_Elements( ) ) break;
    }
    if ( table_alignment->N_Elements( ) > 1 ) if ( k == table_alignment->N_Elements( ) ) break;
  }
  grid->EndBatch();
UPDATE_WINDOW
}

void GDLWidgetTable::DoAlign(DLongGDL* selection) {
  if (table_alignment->N_Elements( )==0) {return;}
  gdlGrid * grid = static_cast<gdlGrid*> (wxWidget);
  assert( grid != NULL);
  SizeT k = 0;
  grid->BeginBatch();
  if (selection->Rank()==0) { //use current wxWidgets selection
   std::vector<wxPoint> list=grid->GetSelectedDisjointCellsList();
   for ( std::vector<wxPoint>::iterator it = list.begin(); it !=list.end(); ++it) {
     int ali;
      switch ( (*table_alignment)[k % table_alignment->N_Elements( )] ) {
        case 0:
          ali = wxALIGN_LEFT;
          break;
        case 1:
          ali = wxALIGN_CENTRE;
          break;
        case 2:
          ali = wxALIGN_RIGHT;
      }
      grid->SetCellAlignment( (*it).x, (*it).y, ali, wxALIGN_CENTRE );
      k++;
    }
  } else { //use the passed selection, mode-dependent:
    if (disjointSelection) { //pairs lists
      for (SizeT n=0,l=0; n<selection->Dim(1); ++n) {
        int col = (*selection)[l++];
        int row = (*selection)[l++];
        int ali;
        switch ( (*table_alignment)[k % table_alignment->N_Elements( )] ) {
          case 0:
            ali = wxALIGN_LEFT;
            break;
          case 1:
            ali = wxALIGN_CENTRE;
            break;
          case 2:
            ali = wxALIGN_RIGHT;
        }
        grid->SetCellAlignment( row, col, ali, wxALIGN_CENTRE );
        k++;
      }
    }else{ //4 values
     int colTL = (*selection)[0];
     int rowTL = (*selection)[1];
     int colBR = (*selection)[2];
     int rowBR = (*selection)[3];
     for ( int i=rowTL; i<=rowBR; ++i) for (int j=colTL; j<=colBR; ++j)
     {
        int ali;
        switch ( (*table_alignment)[k % table_alignment->N_Elements( )] ) {
          case 0:
            ali = wxALIGN_LEFT;
            break;
          case 1:
            ali = wxALIGN_CENTRE;
            break;
          case 2:
            ali = wxALIGN_RIGHT;
        }
        grid->SetCellAlignment( i, j, ali, wxALIGN_CENTRE );
        k++;
     }
    }
  }
  grid->EndBatch();
UPDATE_WINDOW
}

void GDLWidgetTable::DoBackgroundColor() {
  SizeT nbColors=backgroundColor->N_Elements( );
  if (nbColors==0) {return;}
  gdlGrid * grid = static_cast<gdlGrid*> (wxWidget);
  assert( grid != NULL);
  int nRows = grid->GetNumberRows( );
  int nCols = grid->GetNumberCols( );
  SizeT k=0; 
  grid->BeginBatch();
  for (SizeT i=0; i< nRows; ++i) for (SizeT j=0; j< nCols; ++j) //Row by Row, from top.
  {
   grid->SetCellBackgroundColour( i, j, wxColour((*backgroundColor)[k%nbColors],(*backgroundColor)[k%nbColors+1],(*backgroundColor)[k%nbColors+2]));
   k+=3;
  }
  grid->EndBatch();
UPDATE_WINDOW
}
void GDLWidgetTable::DoBackgroundColor(DLongGDL* selection) {
  SizeT nbColors=backgroundColor->N_Elements( );
  if (nbColors==0) {return;}
  gdlGrid * grid = static_cast<gdlGrid*> (wxWidget);
  assert( grid != NULL);
  grid->BeginBatch();

  SizeT k=0;

  if (selection->Rank()==0) { //use current wxWidgets selection
   std::vector<wxPoint> list=grid->GetSelectedDisjointCellsList();
   for ( std::vector<wxPoint>::iterator it = list.begin(); it !=list.end(); ++it) {
    grid->SetCellBackgroundColour( (*it).x, (*it).y, wxColour((*backgroundColor)[k%nbColors],(*backgroundColor)[k%nbColors+1],(*backgroundColor)[k%nbColors+2]));
    k+=3;
    }
  } else { //use the passed selection, mode-dependent:
    if (disjointSelection) { //pairs lists
      for (SizeT n=0,l=0; n<selection->Dim(1); ++n) {
        int col = (*selection)[l++];
        int row = (*selection)[l++];
        grid->SetCellBackgroundColour( row, col, wxColour((*backgroundColor)[k%nbColors],(*backgroundColor)[k%nbColors+1],(*backgroundColor)[k%nbColors+2]));
        k+=3;
      }
    }else{ //4 values
     int colTL = (*selection)[0];
     int rowTL = (*selection)[1];
     int colBR = (*selection)[2];
     int rowBR = (*selection)[3];
     for ( int i=rowTL; i<=rowBR; ++i) for (int j=colTL; j<=colBR; ++j)
     {
       grid->SetCellBackgroundColour( i, j, wxColour((*backgroundColor)[k%nbColors],(*backgroundColor)[k%nbColors+1],(*backgroundColor)[k%nbColors+2]));
       k+=3;
     }
    }
  }
  
  grid->EndBatch();
UPDATE_WINDOW
}

void GDLWidgetTable::DoForegroundColor() {
  SizeT nbColors=foregroundColor->N_Elements( );
  if (nbColors==0) {return;}
  gdlGrid * grid = static_cast<gdlGrid*> (wxWidget);
  assert( grid != NULL);
  int nRows = grid->GetNumberRows( );
  int nCols = grid->GetNumberCols( );
  SizeT k=0; 
  grid->BeginBatch();
  for (SizeT i=0; i< nRows; ++i) for (SizeT j=0; j< nCols; ++j) //Row by Row, from top.
  {
    grid->SetCellTextColour( i, j, wxColour((*foregroundColor)[k%nbColors],(*foregroundColor)[k%nbColors+1],(*foregroundColor)[k%nbColors+2]));
    k+=3;
  }
  grid->EndBatch();
UPDATE_WINDOW
}


void GDLWidgetTable::DoForegroundColor(DLongGDL* selection) {
  SizeT nbColors=foregroundColor->N_Elements( );
  if (nbColors==0) {return;}
  gdlGrid * grid = static_cast<gdlGrid*> (wxWidget);
  assert( grid != NULL);
  grid->BeginBatch();

  SizeT k=0;

  if (selection->Rank()==0) { //use current wxWidgets selection
   std::vector<wxPoint> list=grid->GetSelectedDisjointCellsList();
   for ( std::vector<wxPoint>::iterator it = list.begin(); it !=list.end(); ++it) {
    grid->SetCellTextColour( (*it).x, (*it).y, wxColour((*foregroundColor)[k%nbColors],(*foregroundColor)[k%nbColors+1],(*foregroundColor)[k%nbColors+2]));
    k+=3;
    }
  } else { //use the passed selection, mode-dependent:
    if (disjointSelection) { //pairs lists
      for (SizeT n=0,l=0; n<selection->Dim(1); ++n) {
        int col = (*selection)[l++];
        int row = (*selection)[l++];
        grid->SetCellTextColour( row, col, wxColour((*foregroundColor)[k%nbColors],(*foregroundColor)[k%nbColors+1],(*foregroundColor)[k%nbColors+2]));
        k+=3;
      }
    }else{ //4 values
     int colTL = (*selection)[0];
     int rowTL = (*selection)[1];
     int colBR = (*selection)[2];
     int rowBR = (*selection)[3];
     for ( int i=rowTL; i<=rowBR; ++i) for (int j=colTL; j<=colBR; ++j)
     {
       grid->SetCellTextColour( i, j, wxColour((*foregroundColor)[k%nbColors],(*foregroundColor)[k%nbColors+1],(*foregroundColor)[k%nbColors+2]));
       k+=3;
     }
    }
  }
  
  grid->EndBatch();
UPDATE_WINDOW
}
  
void GDLWidgetTable::DoColumnLabels( ) {
  if (columnLabels->N_Elements( )==0) {return;}
  gdlGrid * grid = static_cast<gdlGrid*> (wxWidget);
  assert( grid != NULL);
  int nCols = grid->GetNumberCols( );
  grid->BeginBatch();
  if ( columnLabels->N_Elements( ) == 1 ) { //singleton case
    if ( static_cast<DString> ((*columnLabels)[0]).length( ) == 0 ) {
      for ( SizeT j = 0; j < nCols; ++j ) grid->SetColLabelValue( j, wxEmptyString );
    } else {
      for ( SizeT j = 0; j < nCols; ++j ) {
        if ( j > (columnLabels->N_Elements( ) - 1) ) break;
        grid->SetColLabelValue( j, wxString( static_cast<DString> ((*columnLabels)[j]).c_str( ), wxConvUTF8 ) );
      }
    }
  } else {
    for ( SizeT j = 0; j < nCols; ++j ) {
      if ( j > (columnLabels->N_Elements( ) - 1) ) break;
      grid->SetColLabelValue( j, wxString( static_cast<DString> ((*columnLabels)[j]).c_str( ), wxConvUTF8 ) );
    }
  }
  grid->EndBatch();
  UPDATE_WINDOW
}

void GDLWidgetTable::DoColumnWidth( ) {
  if (columnWidth->N_Elements( )==0) {return;}
  gdlGrid * grid = static_cast<gdlGrid*> (wxWidget);
  assert( grid != NULL);
  int nCols = grid->GetNumberCols( );
  grid->BeginBatch();
  if ( columnWidth->N_Elements( ) == 1 ) for ( SizeT j = 0; j < nCols; ++j ) grid->SetColSize(j,(*columnWidth)[0]*unitConversionFactor.x); 
  else {
      for ( SizeT j = 0; j < nCols; ++j ) {
        if ( j > (columnWidth->N_Elements( ) - 1) ) break;
        grid->SetColSize(j,(*columnWidth)[j]*unitConversionFactor.x);
      }
  }
  grid->EndBatch();
  UPDATE_WINDOW
}

void GDLWidgetTable::DoColumnWidth( DLongGDL* selection ) {
  SizeT nbCols = columnWidth->N_Elements( );
  if ( nbCols == 0 ) {return;}
  gdlGrid * grid = static_cast<gdlGrid*> (wxWidget);
  assert( grid != NULL);
  grid->BeginBatch( );

  SizeT k=0;

  if ( selection->Rank( ) == 0 ) { //use current wxWidgets selection
   wxArrayInt list=grid->GetSortedSelectedColsList();
   //find concerned cols
   for ( int it = 0; it <list.GetCount(); ++it) {
       grid->SetColSize( list[it], (*columnWidth)[it % nbCols]*unitConversionFactor.x);
    }
  } else { //use the passed selection, mode-dependent:
     if (disjointSelection) { //pairs lists
     std::vector<int> allCols;
     std::vector<int>::iterator iter;
     //find concerned cols
     for ( SizeT n=0, l=0 ; n<selection->Dim(1); ++n) {
        int col = (*selection)[l++];l++;
        allCols.push_back(col);
      }
     std::sort (allCols.begin(), allCols.end());
     int theCol=-1;
     for ( iter = allCols.begin(); iter !=allCols.end(); ++iter) {
        if ((*iter)!=theCol) {
          theCol=(*iter);
                  //if index is -1, we want to size the label column instead of the grid columns.
          if (theCol==-1)  grid->SetRowLabelSize((*columnWidth)[k % nbCols]*unitConversionFactor.x);
          else if (theCol>=0 && theCol<grid->GetNumberCols())  grid->SetColSize( theCol, (*columnWidth)[k % nbCols]*unitConversionFactor.x );
          k++;
        }
      }
    } else { //4 values
     int colTL = (*selection)[0];
     int colBR = (*selection)[2];
     for (int j=colTL; j<=colBR; ++j)
     {
       //if index is -1, we want to size the label column instead of the grid columns.
       if (j==-1)  grid->SetRowLabelSize((*columnWidth)[k % nbCols]*unitConversionFactor.x);
       else if (j>=0 && j<grid->GetNumberCols())  grid->SetColSize( j, (*columnWidth)[k % nbCols]*unitConversionFactor.x );
       k++;
     }
    }
  }
  
  grid->EndBatch( );
  UPDATE_WINDOW
}  
DFloatGDL* GDLWidgetTable::GetColumnWidth(DLongGDL* selection){
  gdlGrid * grid = static_cast<gdlGrid*> (wxWidget);
  assert( grid != NULL);
  SizeT k=0;
  int nCols = grid->GetNumberCols( );
  
  if ( selection == NULL) {
    DFloatGDL* res=new DFloatGDL(dimension(nCols));
    for ( SizeT j = 0; j < nCols; ++j ) (*res)[j]=grid->GetColSize(j);
    return res;
  } else if ( selection->Rank( ) == 0 ) { //use current wxWidgets selection
    wxArrayInt list=grid->GetSortedSelectedColsList();
   //find concerned cols
    if (list.GetCount()==0) return NULL;
   DFloatGDL* res=new DFloatGDL(dimension(list.GetCount()));
   for ( int it = 0; it <list.GetCount(); ++it) {
       (*res)[it]=grid->GetColSize( list[it] );
    }
   return res;
  } else { //use the passed selection, mode-dependent:
    if (disjointSelection) { //pairs lists
     std::vector<int> allCols;
     std::vector<int>::iterator iter;
     std::vector<int> theCols;
     //find concerned cols
     for ( SizeT n=0, l=0 ; n<selection->Dim(1); ++n) {
        int col = (*selection)[l++];l++;
        allCols.push_back(col);
      }
     std::sort (allCols.begin(), allCols.end());
     int theCol=-1;
     for ( iter = allCols.begin(); iter !=allCols.end(); ++iter) {
        if ((*iter)!=theCol) {
          theCol=(*iter);
          k++;
          theCols.push_back(theCol);
        }
      }
     //final list:
     if (theCols.size()==0) return NULL;
     DFloatGDL* res=new DFloatGDL(dimension(theCols.size()));
     for ( iter = theCols.begin(); iter !=theCols.end(); ++iter) {
       (*res)[k++]=grid->GetColSize( (*iter));
      }     
     return res;
    } else { //4 values
     int colTL = (*selection)[0];
     int colBR = (*selection)[2];
     int count = colBR-colTL+1;
     if (count==0) return NULL;
     DFloatGDL* res=new DFloatGDL(dimension(count));
     for (int j=colTL; j<=colBR; ++j)
     {
       (*res)[k++]=grid->GetColSize(j); 
     }
    }
  }
  return new DFloatGDL(0); //to keep compiler happy
}
DFloatGDL* GDLWidgetTable::GetRowHeight(DLongGDL* selection){
  gdlGrid * grid = static_cast<gdlGrid*> (wxWidget);
  assert( grid != NULL);
  SizeT k=0;
  int nRows = grid->GetNumberRows( );
  
  if ( selection == NULL) {
    DFloatGDL* res=new DFloatGDL(dimension(nRows));
    for ( SizeT i = 0; i < nRows; ++i ) (*res)[i]=grid->GetRowSize(i);
    return res;
  } else if ( selection->Rank( ) == 0 ) { //use current wxWidgets selection
    wxArrayInt list=grid->GetSortedSelectedRowsList();
   //find concerned rows
    if (list.GetCount()==0) return NULL;
   DFloatGDL* res=new DFloatGDL(dimension(list.GetCount()));
   for ( int it = 0; it <list.GetCount(); ++it) {
       (*res)[it]=grid->GetRowSize( list[it] );
    }
   return res;
  } else { //use the passed selection, mode-dependent:
    if (disjointSelection) { //pairs lists
     std::vector<int> allRows;
     std::vector<int>::iterator iter;
     std::vector<int> theRows;
     //find concerned rows
     for ( SizeT n=0, l=0 ; n<selection->Dim(1); ++n) {
        int row = (*selection)[l++];l++;
        allRows.push_back(row);
      }
     std::sort (allRows.begin(), allRows.end());
     int theRow=-1;
     for ( iter = allRows.begin(); iter !=allRows.end(); ++iter) {
        if ((*iter)!=theRow) {
          theRow=(*iter);
          k++;
          theRows.push_back(theRow);
        }
      }
     //final list:
     if (theRows.size()==0) return NULL;
     DFloatGDL* res=new DFloatGDL(dimension(theRows.size()));
     for ( iter = theRows.begin(); iter !=theRows.end(); ++iter) {
       (*res)[k++]=grid->GetRowSize( (*iter));
      }     
     return res;
    } else { //4 values
     int rowTL = (*selection)[1];
     int rowBR = (*selection)[3];
     int count = rowBR-rowTL+1;
     if (count==0) return NULL;
     DFloatGDL* res=new DFloatGDL(dimension(count));
     for (int j=rowTL; j<=rowBR; ++j)
     {
       (*res)[k++]=grid->GetRowSize(j); 
     }
    }
  }
  return new DFloatGDL(0); //to keep compiler happy
}

void GDLWidgetTable::DoRowHeights( ) {
  if (rowHeights->N_Elements( )==0) {return;}
  gdlGrid * grid = static_cast<gdlGrid*> (wxWidget);
  assert( grid != NULL);
  int nRows = grid->GetNumberRows( );
  grid->BeginBatch();
  if ( rowHeights->N_Elements( ) == 1 ) for ( SizeT i = 0; i < nRows; ++i ) grid->SetRowSize(i,(*rowHeights)[0]*unitConversionFactor.y); 
  else {
      for ( SizeT i = 0; i < nRows; ++i ) {
        if ( i > (rowHeights->N_Elements( ) - 1) ) break;
        grid->SetRowSize(i,(*rowHeights)[i]*unitConversionFactor.y);
      }
  }
  grid->EndBatch();
  UPDATE_WINDOW
}

void GDLWidgetTable::DoRowHeights( DLongGDL* selection ) {
  SizeT nbRows = rowHeights->N_Elements( );
  if ( nbRows == 0 ) { return; }
  gdlGrid * grid = static_cast<gdlGrid*> (wxWidget);
  assert( grid != NULL);
  grid->BeginBatch( );

  SizeT k=0;

  if ( selection->Rank( ) == 0 ) { //use current wxWidgets selection
   wxArrayInt list=grid->GetSortedSelectedRowsList();
   for ( int it = 0; it <list.GetCount(); ++it) {
       if (list[it]<grid->GetNumberRows())  grid->SetRowSize( list[it], (*rowHeights)[it % nbRows]*unitConversionFactor.y );
    }
  } else { //use the passed selection, mode-dependent:
    if (disjointSelection) { //pairs lists
     std::vector<int> allRows;
     std::vector<int>::iterator iter;
     for ( SizeT n=0, l=0 ; n<selection->Dim(1); ++n) {
       l++;
       int row = (*selection)[l++];
       allRows.push_back(row);
      }
     std::sort (allRows.begin(), allRows.end());
     int theRow=-1;
     for ( iter = allRows.begin(); iter !=allRows.end(); ++iter) {
        if ((*iter)!=theRow) {
          theRow=(*iter);
         //if index is -1, we want to size the label column instead of the grid columns.
          if (theRow==-1)  grid->SetColLabelSize((*columnWidth)[k % nbRows]*unitConversionFactor.x);
          else if (theRow>=0 && theRow<grid->GetNumberRows()) grid->SetRowSize( theRow, (*rowHeights)[k % nbRows] *unitConversionFactor.y);
          k++;
        }
      }
    } else { //4 values
     int rowTL = (*selection)[1];
     int rowBR = (*selection)[3];
     for (int i=rowTL; i<=rowBR; ++i)
     {
       //if index is -1, we want to size the label column instead of the grid columns.
       if (i==-1)  grid->SetColLabelSize((*columnWidth)[k % nbRows]*unitConversionFactor.x);
       else if (i>=0 && i<grid->GetNumberRows()) grid->SetRowSize( i, (*rowHeights)[k % nbRows]*unitConversionFactor.y );
       k++;
     }
    }
  }
  
  grid->EndBatch( );
  UPDATE_WINDOW
}

void GDLWidgetTable::DoRowLabels( ) {
  if (rowLabels->N_Elements( )==0) {return;}
  gdlGrid * grid = static_cast<gdlGrid*> (wxWidget);
  assert( grid != NULL);
  int nRows = grid->GetNumberRows( );
  grid->BeginBatch();
  if ( rowLabels->N_Elements( ) == 1 ) { //singleton case
    if ( static_cast<DString> ((*rowLabels)[0]).length( ) == 0 ) {
      for ( SizeT i = 0; i < nRows; ++i ) grid->SetRowLabelValue( i, wxEmptyString );
    } else {
      for ( SizeT i = 0; i < nRows; ++i ) {
        if ( i > (rowLabels->N_Elements( ) - 1) ) break;
        grid->SetRowLabelValue( i, wxString( static_cast<DString> ((*rowLabels)[i]).c_str( ), wxConvUTF8 ) );
      }
    }
  } else {
    for ( SizeT i = 0; i < nRows; ++i ) {
      if ( i > (rowLabels->N_Elements( ) - 1) ) break;
      grid->SetRowLabelValue( i, wxString( static_cast<DString> ((*rowLabels)[i]).c_str( ), wxConvUTF8 ) );
    }
  }
  grid->EndBatch();
  UPDATE_WINDOW
}

void GDLWidgetTable::DeleteColumns(DLongGDL* selection) {
  gdlGrid * grid = static_cast<gdlGrid*> (wxWidget);
  assert( grid != NULL);
  grid->BeginBatch( );

  if ( selection==NULL || selection->Rank( ) == 0 ) { //use current wxWidgets selection
   wxArrayInt list=grid->GetSortedSelectedColsList();
   //delete in reverse order to avoid column-numbering problems
   for ( int it = list.GetCount()-1; it >-1 ; --it) {
       grid->DeleteCols( list[it], 1, TRUE);
    }
  } else { //use the passed selection, mode-dependent:
    if (disjointSelection) { //pairs lists
     std::vector<int> allCols;
     std::vector<int>::reverse_iterator riter;
     //find concerned cols
     for ( SizeT n=0, l=0 ; n<selection->Dim(1); ++n) {
        int col = (*selection)[l++];l++;
        allCols.push_back(col);
      }
     std::sort (allCols.begin(), allCols.end());
     int theCol=-1;
     for ( riter = allCols.rbegin(); riter !=allCols.rend(); ++riter) {
        if ((*riter)!=theCol) {
          theCol=(*riter);
          grid->DeleteCols( theCol, 1, TRUE);
        }
      }
    } else { //4 values, cols are contiguous, easy.
     int colTL = (*selection)[0];
     int colBR = (*selection)[2];
     int count=colBR-colTL+1;
     grid->DeleteCols( colTL , count, TRUE );
    }
  }
  
  grid->EndBatch( );
  UPDATE_WINDOW
}
bool GDLWidgetTable::InsertColumns(DLong count, DLongGDL* selection) {
  gdlGrid * grid = static_cast<gdlGrid*> (wxWidget);
  assert( grid != NULL);
  bool success;
  grid->BeginBatch( );

  if ( selection==NULL ){ //add count to rightmost position
    int pos=grid->GetNumberCols();
    success=grid->InsertCols(pos,count,TRUE);
   // Set new grid cell contents TBD FIXME!
   {SizeT k=0; for (SizeT i=0; i< grid->GetNumberRows(); ++i) for (SizeT j=pos; j<grid->GetNumberCols() ; ++j) {grid->SetCellValue( i, j, wxString( "0" , wxConvUTF8 ) );++k;}}
  }
  else if (selection->Rank( ) == 0 ) { //add left of current wxWidgets selection
   wxArrayInt list=grid->GetSortedSelectedColsList();
   //insert to left of first one
   success=grid->InsertCols( list[0], count, TRUE);
  } else { //use the passed selection, mode-dependent:
    if (disjointSelection) { //pairs lists
     std::vector<int> allCols;
     //find concerned cols
     for ( SizeT n=0, l=0 ; n<selection->Dim(1); ++n) {
        int col = (*selection)[l++];l++;
        allCols.push_back(col);
      }
     std::sort (allCols.begin(), allCols.end());
     success=grid->InsertCols( *(allCols.begin()), 1, TRUE);
    } else { //4 values, cols are contiguous, easy.
     int colTL = (*selection)[0];
     success=grid->InsertCols( colTL , count, TRUE );
    }
  }
  
  grid->EndBatch( );
  UPDATE_WINDOW
  return success;
}

void GDLWidgetTable::DeleteRows(DLongGDL* selection) {
  gdlGrid * grid = static_cast<gdlGrid*> (wxWidget);
  assert( grid != NULL);
  grid->BeginBatch( );

  if ( selection==NULL || selection->Rank( ) == 0 ) { //use current wxWidgets selection
   wxArrayInt list=grid->GetSortedSelectedRowsList();
   //delete in reverse order to avoid column-numbering problems
   for ( int it = list.GetCount()-1; it >-1 ; --it) {
       grid->DeleteRows( list[it], 1, TRUE);
    }
  } else { //use the passed selection, mode-dependent:
    if (disjointSelection) { //pairs lists
     std::vector<int> allRows;
     std::vector<int>::reverse_iterator riter;
     //find concerned Rows
     for ( SizeT n=0, l=0 ; n<selection->Dim(1); ++n) {
        int row = (*selection)[l++];l++;
        allRows.push_back(row);
      }
     std::sort (allRows.begin(), allRows.end());
     int theRow=-1;
     for ( riter = allRows.rbegin(); riter !=allRows.rend(); ++riter) {
        if ((*riter)!=theRow) {
          theRow=(*riter);
          grid->DeleteRows( theRow, 1, TRUE);
        }
      }
    } else { //4 values, Rows are contiguous, easy.
     int rowTL = (*selection)[1];
     int rowBR = (*selection)[3];
     int count=rowBR-rowTL+1;
     grid->DeleteRows( rowTL , count, TRUE );
    }
  }
  
  grid->EndBatch( );
  UPDATE_WINDOW
}

bool GDLWidgetTable::InsertRows(DLong count, DLongGDL* selection) {
  gdlGrid * grid = static_cast<gdlGrid*> (wxWidget);
  assert( grid != NULL);
  bool success;
  grid->BeginBatch( );

  if ( selection==NULL ){ //add count to rightmost position
    int pos=grid->GetNumberRows();
    success=grid->InsertRows(pos,count,TRUE);
   // Set new grid cell contents TBD FIXME!
   {SizeT k=0; for (SizeT i=pos; i<grid->GetNumberRows(); ++i) for (SizeT j=0; j<grid->GetNumberCols() ; ++j) {grid->SetCellValue( i, j, wxString( "0" , wxConvUTF8 ) );++k;}}
  }
  else if (selection->Rank( ) == 0 ) { //add left of current wxWidgets selection
   wxArrayInt list=grid->GetSortedSelectedRowsList();
   //insert to left of first one
   success=grid->InsertRows( list[0], count, TRUE);
  } else { //use the passed selection, mode-dependent:
    if (disjointSelection) { //pairs lists
     std::vector<int> allRows;
     //find concerned rows
     for ( SizeT n=0, l=0 ; n<selection->Dim(1); ++n) {
        int row = (*selection)[l++];l++;
        allRows.push_back(row);
      }
     std::sort (allRows.begin(), allRows.end());
     success=grid->InsertRows( *(allRows.begin()), 1, TRUE);
    } else { //4 values, cols are contiguous, easy.
     int rowTL = (*selection)[1];
     success=grid->InsertRows( rowTL , count, TRUE );
    }
  }
  
  grid->EndBatch( );
  UPDATE_WINDOW
  return success;
}

void GDLWidgetTable::SetTableValues(DStringGDL* val, DLongGDL* selection)
{
  gdlGrid * grid = static_cast<gdlGrid*> (wxWidget);
  assert( grid != NULL);

  grid->BeginBatch();
  
  if ( selection==NULL ){ //reset table to everything. val replaces valueAsStrings.
    GDLDelete(valueAsStrings);
    valueAsStrings=val->Dup();
    SizeT numRows,numCols;
    if (valueAsStrings->Rank()==1) {
      numRows=1;
      numCols=valueAsStrings->Dim(0); //lines
    } else {
      numRows=valueAsStrings->Dim(1);
      numCols=valueAsStrings->Dim(0);
    }
    grid->ClearGrid();
    int curr_rows=grid->GetNumberRows();
    int curr_cols=grid->GetNumberCols();
    //adjust rows and cols:
    if (numRows > curr_rows) grid->AppendRows(numRows-curr_rows);
    if (numCols > curr_cols) grid->AppendCols(numCols-curr_cols);
    // Set grid cell contents as strings
    {SizeT k=0; for (SizeT i=0; i<numRows ; ++i) for (SizeT j=0; j< numCols; ++j) {grid->SetCellValue( i, j, wxString(((*valueAsStrings)[k]).c_str(), wxConvUTF8 ) );++k;}}
    
  } else { //use the wxWidget selection or the passed selection, mode-dependent:
    if (disjointSelection) { //pairs lists
      if (selection->Rank()==0) { 
        std::vector<wxPoint> list=grid->GetSelectedDisjointCellsList();
        SizeT k=0;
        for (std::vector<wxPoint>::iterator it = list.begin(); it !=list.end(); ++it) {
          grid->SetCellValue( (*it).x, (*it).y ,wxString(((*val)[k++]).c_str(), wxConvUTF8 ) ); 
          if (k==val->N_Elements()) break;
        }
       } else {
        for (SizeT k=0,n=0,l=0; n<selection->Dim(1); ++n) {
          int col = (*selection)[l++];
          int row = (*selection)[l++];
          grid->SetCellValue( row, col ,wxString(((*val)[k++]).c_str(), wxConvUTF8 ) ); 
          if (k==val->N_Elements()) break;
        }
      }
    } else { //IDL maintains the 2D-structure of val!
      SizeT numRows,numCols;
      if (val->Rank()==1) {
        numRows=1;
        numCols=val->Dim(0); //lines
      } else {
        numRows=val->Dim(1);
        numCols=val->Dim(0);
      }
      int colTL,colBR,rowTL,rowBR;
      if (selection->Rank()==0) { 
        wxArrayInt block=grid->GetSelectedBlockOfCells();
        //normally only ONE block is available.
        colTL = block[0];
        rowTL = block[1];
        colBR = block[2];
        rowBR = block[3];
      } else {
        colTL = (*selection)[0];
        rowTL = (*selection)[1];
        colBR = (*selection)[2];
        rowBR = (*selection)[3];
      }
      for ( int ival=0, i=rowTL; i<=rowBR; ++i, ++ival) for (int jval=0, j=colTL; j<=colBR; ++j, ++jval)
      {
        if (ival < numRows && jval < numCols ) grid->SetCellValue( i, j ,wxString(((*val)[jval*numRows+ival]).c_str(), wxConvUTF8 ) ); 
      }
    }
  }
  grid->EndBatch( );
  UPDATE_WINDOW
}
BaseGDL* GDLWidgetTable::GetTableValuesAsStruct(DLongGDL* selection)
{
  gdlGrid * grid = static_cast<gdlGrid*> (wxWidget);
  assert( grid != NULL);
  BaseGDL* res;
  int numRows=valueAsStrings->Dim(0);
  int numCols=valueAsStrings->Dim(1);
  DStringGDL* stringres=this->GetTableValues(selection);
  if (stringres==NULL) return NULL; //pass error back.

  if ( selection==NULL ){ //just convert
    res=vValue->Dup();
    stringstream is;
//    if (majority == GDLWidgetTable::ROW_MAJOR ) {
//      BaseGDL* tmp=static_cast<BaseGDL*>(stringres)->Transpose(NULL);
//      GDLDelete(stringres);
//      stringres=static_cast<DStringGDL*>(tmp);
//      for( SizeT i = 0; i < stringres->N_Elements(); i++)  is << (*stringres)[ i] << '\n';
//    } else {
      for( SizeT i = 0; i < stringres->N_Elements(); i++)  is << (*stringres)[ i] << '\n';
//    }
    res->FromStream( is);
  } 
  else { //use the wxWidget selection or the passed selection, mode-dependent:
    if (disjointSelection) { //pairs lists
      std::vector<wxPoint> list;
      if (selection->Rank()==0) { //use current wxWidgets selection. Result is a STRUCT
        list=grid->GetSelectedDisjointCellsList();
      } else {                   //make equivalent vector.
        for (SizeT k=0,n=0,l=0; n<selection->Dim(1); ++n) {
          int col = (*selection)[l++];
          int row = (*selection)[l++];
          list.push_back(wxPoint(row,col));
        }
      }
      SizeT k=0;
      DStructGDL*  typecodes = new DStructGDL( "GDL_TYPECODES_AS_STRUCT"); 
      // creating the output anonymous structure
      DStructDesc* res_desc = new DStructDesc("$truct");
      for ( std::vector<wxPoint>::iterator it = list.begin(); it !=list.end(); ++it, ++k) {
        //get tag values:
        BaseGDL* tested;
        if (majority == GDLWidgetTable::ROW_MAJOR )
          tested=static_cast<DStructGDL*>(vValue)->GetTag((*it).y); //table columns are tags
        else
          tested=static_cast<DStructGDL*>(vValue)->GetTag((*it).x); //table rows are tags
        stringstream os;
        os << std::setfill ('_') << std::setw (12) << k ; //as IDL does
        std::string tagName;
        os >> tagName; 
        res_desc->AddTag(tagName, typecodes->GetTag(tested->Type()));
      }
      stringstream is;
      for( SizeT i = 0; i < stringres->N_Elements(); i++)  is << (*stringres)[ i] << '\n';
      res = new DStructGDL(res_desc, dimension());
      res->FromStream( is);
    } else { //IDL maintains the 2D-structure of val!
      int colTL,colBR,rowTL,rowBR;
      if (selection->Rank()==0) { 
        wxArrayInt block=grid->GetSelectedBlockOfCells();
        //normally only ONE block is available.
        colTL = block[0];
        rowTL = block[1];
        colBR = block[2];
        rowBR = block[3];
      } else {
        colTL = (*selection)[0];
        rowTL = (*selection)[1];
        colBR = (*selection)[2];
        rowBR = (*selection)[3];
      }
      //complication: if only one row (col) is selected, result is an array of <type>.
      //else result is a structure with correct tag names. Very clever!
      if ((majority == GDLWidgetTable::ROW_MAJOR && colTL==colBR)||(majority == GDLWidgetTable::COLUMN_MAJOR && rowTL==rowBR)) {
        DType what=GDL_BYTE;
        SizeT size;
        if (majority == GDLWidgetTable::ROW_MAJOR && colTL==colBR) {
          what=static_cast<DStructGDL*>(vValue)->GetTag(colTL)->Type();
          size=rowBR-rowTL+1;
        } else if (rowTL==rowBR) {
          what=static_cast<DStructGDL*>(vValue)->GetTag(rowTL)->Type();
          size=colBR-colTL+1;
        } 
        switch(what) {
          case GDL_STRING:
            res=new DStringGDL(dimension(size));
            break;
          case GDL_BYTE:
            res=new DByteGDL(dimension(size));
            break;
          case GDL_INT: 
            res=new DIntGDL(dimension(size));
            break;
          case GDL_LONG:
            res=new DLongGDL(dimension(size));
            break;
          case GDL_FLOAT:
            res=new DFloatGDL(dimension(size));
            break;
          case GDL_DOUBLE:
            res=new DDoubleGDL(dimension(size));
            break;
          case GDL_COMPLEX:
            res=new DComplexGDL(dimension(dimension(size)));
            break;
          case GDL_COMPLEXDBL:
            res=new DComplexDblGDL(dimension(size));
            break;
          case GDL_UINT:
            res=new DUIntGDL(dimension(size));
            break;
          case GDL_ULONG:
            res=new DULongGDL(dimension(size));
            break;
          case GDL_LONG64:
            res=new DLong64GDL(dimension(size));
            break;
          case GDL_ULONG64:
            res=new DULong64GDL(dimension(size));
            break;
          default:
            cerr<<"Unhandled Table Type, please report!"<<endl;
            return NULL; //signal error
        }
      } else { //create dedicated struct
        DStructGDL*  typecodes = new DStructGDL( "GDL_TYPECODES_AS_STRUCT"); 
        // creating the output anonymous structure
        DStructDesc* res_desc = new DStructDesc("$truct");
        SizeT size;
        if (majority == GDLWidgetTable::ROW_MAJOR) { //data is in rows of structures. Columns are tags
          size=rowBR-rowTL+1;
          for (SizeT j=colTL; j<=colBR; ++j) {
          //get tag values:
            BaseGDL* tested;
            std::string tagName;
            tested=static_cast<DStructGDL*>(vValue)->GetTag(j);
            tagName=static_cast<DStructGDL*>(vValue)->Desc()->TagName(j); //preserve tag names
            res_desc->AddTag(tagName, typecodes->GetTag(tested->Type()));
          }          
        } else {
          size=colBR-colTL+1;
          for (SizeT i=rowTL; i<=rowBR; ++i) { 
          //get tag values:
            BaseGDL* tested;
            std::string tagName;
            tested=static_cast<DStructGDL*>(vValue)->GetTag(i);
            tagName=static_cast<DStructGDL*>(vValue)->Desc()->TagName(i); //preserve tag names
            res_desc->AddTag(tagName, typecodes->GetTag(tested->Type()));
          }
        }
        //create res with correct dim:
        res = new DStructGDL(res_desc, dimension(size));
      }
      //populate res:
      stringstream is;
      for( SizeT i = 0; i < stringres->N_Elements(); i++)  is << (*stringres)[ i] << '\n';
      res->FromStream( is);        
    }
  }
  return res;
}

DStringGDL* GDLWidgetTable::GetTableValues(DLongGDL* selection)
{
  gdlGrid * grid = static_cast<gdlGrid*> (wxWidget);
  assert( grid != NULL);

  DStringGDL * stringres;
  int numRows=valueAsStrings->Dim(0);
  int numCols=valueAsStrings->Dim(1);

  if ( selection==NULL ){
    int ncols=grid->GetNumberCols();
    int nrows=grid->GetNumberCols();

    SizeT dims[2];
    dims[1]=(nrows>numRows)?numRows:nrows;
    dims[0]=(ncols>numCols)?numCols:ncols;
    dimension dim(dims,2);
    stringres=new DStringGDL(dim);
    
    for ( int ival=0, i=0; i<nrows; ++i, ++ival) for (int jval=0, j=0; j<ncols; ++j, ++jval)
    {
      if (ival < numRows && jval < numCols ) (*stringres)[jval*numRows+ival]= grid->GetCellValue( i, j).mb_str(wxConvUTF8); 
    }
  } else { //use the wxWidget selection or the passed selection, mode-dependent:
    if (disjointSelection) { //pairs lists
      if (selection->Rank()==0) { //use current wxWidgets selection
        std::vector<wxPoint> list=grid->GetSelectedDisjointCellsList();
        stringres=new DStringGDL(list.size()); 
        SizeT k=0;
        for ( std::vector<wxPoint>::iterator it = list.begin(); it !=list.end(); ++it) {
          if ((*it).x >= numRows || (*it).y >= numCols) return static_cast<DStringGDL*>(NULL); 
          (*stringres)[k++]=grid->GetCellValue( (*it).x, (*it).y ).mb_str(wxConvUTF8);
        }
      } else {
        stringres=new DStringGDL(selection->Dim(1));
        for (SizeT k=0,n=0,l=0; n<selection->Dim(1); ++n) {
          int col = (*selection)[l++];
          int row = (*selection)[l++];
          if ( row >= numRows || col >= numCols) return static_cast<DStringGDL*>(NULL); 
          (*stringres)[k++]=grid->GetCellValue( row, col).mb_str(wxConvUTF8);
        }
      }
    } else { //IDL maintains the 2D-structure of val!
      int colTL,colBR,rowTL,rowBR;
      if (selection->Rank()==0) { 
        wxArrayInt block=grid->GetSelectedBlockOfCells();
        //normally only ONE block is available.
        colTL = block[0];
        rowTL = block[1];
        colBR = block[2];
        rowBR = block[3];
      } else {
        colTL = (*selection)[0];
        rowTL = (*selection)[1];
        colBR = (*selection)[2];
        rowBR = (*selection)[3];
      }
      SizeT dims[2];
      dims[1]=(rowBR-rowTL+1);
      dims[0]=(colBR-colTL+1);
      dimension dim(dims,2);
      stringres=new DStringGDL(dim);
      for (SizeT k=0,i=rowTL; i<=rowBR; ++i) for (SizeT j=colTL; j<=colBR; ++j)
      {
        if ( i >= numRows || j >= numCols) return static_cast<DStringGDL*>(NULL); 
        (*stringres)[k++]=grid->GetCellValue(i, j).mb_str(wxConvUTF8);
      }
    }
  }
  //convention: if value is of type struct, string array will always be row_major. thus if we are column major, transpose return string array
  if (vValue->Type()==GDL_STRUCT && majority==GDLWidgetTable::COLUMN_MAJOR) return static_cast<DStringGDL*>(stringres->Transpose(NULL))->Dup();  
  else return stringres;
}

void GDLWidgetTable::SetSelection(DLongGDL* selection)
{
  gdlGrid * grid = static_cast<gdlGrid*> (wxWidget);
  assert( grid != NULL);
  grid->BeginBatch( );
  updating=TRUE; //prevent sending unwanted events
  grid->ClearSelection();
  wxPoint firstVisible=wxPoint(0,0);
  if (disjointSelection) { //pairs lists
    SizeT k=0;
    for (SizeT i=0; i< selection->Dim(1); ++i) {
      int col=(*selection)[k++];
      int row=(*selection)[k++];
      grid->SelectBlock(row,col,row,col,TRUE);
      if (k==2) {firstVisible.x=row;firstVisible.y=col;} 
    }
  } else {
     int colTL = (*selection)[0];
     int rowTL = (*selection)[1];
     int colBR = (*selection)[2];
     int rowBR = (*selection)[3];
     grid->SelectBlock(rowTL,colTL,rowBR,colBR,FALSE);
     firstVisible.x=rowTL;firstVisible.y=colTL; 
  }
  grid->EndBatch( );
  grid->MakeCellVisible(firstVisible.x,firstVisible.y);
  UPDATE_WINDOW
  updating=FALSE; //allow events
}
void GDLWidgetTable::SetTableView(DLongGDL* pos)
{  
  gdlGrid * grid = static_cast<gdlGrid*> (wxWidget);
  assert( grid != NULL);
  grid->MakeCellVisible((*pos)[1],(*pos)[0]);
}
void GDLWidgetTable::EditCell(DLongGDL* pos)
{  
  gdlGrid * grid = static_cast<gdlGrid*> (wxWidget);
  assert( grid != NULL);
  grid->SetReadOnly((*pos)[0],(*pos)[1],FALSE);
}
void GDLWidgetTable::SetTableNumberOfColumns( DLong ncols){
  gdlGrid * grid = static_cast<gdlGrid*> (wxWidget);
  assert( grid != NULL);
  grid->BeginBatch( );
  int old_ncols=grid->GetNumberCols();
  int numRows=valueAsStrings->Dim(0);
  int numCols=valueAsStrings->Dim(1);
  if (ncols > old_ncols) {
    grid->AppendCols(ncols-old_ncols);
    if (numCols > old_ncols) {
      int colTL,colBR,rowTL,rowBR;
      colTL=old_ncols-1;
      colBR=ncols-1;
      rowTL=0;
      rowBR=grid->GetNumberRows()-1;
      for ( int i=rowTL; i<=rowBR; ++i) for (int j=colTL; j<=colBR; ++j)
      {
        if (i < numRows && j < numCols ) grid->SetCellValue( i, j ,wxString(((*valueAsStrings)[j*numRows+i]).c_str(), wxConvUTF8 ) ); 
      }
    }
  }
  else grid->DeleteCols(ncols,old_ncols-ncols);
  grid->EndBatch( );
}
void GDLWidgetTable::SetTableNumberOfRows( DLong nrows){
  gdlGrid * grid = static_cast<gdlGrid*> (wxWidget);
  assert( grid != NULL);
  grid->BeginBatch( );
  SizeT old_nrows=grid->GetNumberRows();
  int numRows=valueAsStrings->Dim(0);
  int numCols=valueAsStrings->Dim(1);
  if (nrows > old_nrows) {
    grid->AppendRows(nrows-old_nrows);
    if (numRows > old_nrows) {
      int colTL,colBR,rowTL,rowBR;
      colTL=0;
      colBR=grid->GetNumberCols()-1;
      rowTL=old_nrows-1;
      rowBR=nrows-1;
      for ( int i=rowTL; i<=rowBR; ++i) for (int j=colTL; j<=colBR; ++j)
      {
        if (i < numRows && j < numCols ) grid->SetCellValue( i, j ,wxString(((*valueAsStrings)[j*numRows+i]).c_str(), wxConvUTF8 ) ); 
      }
    }
  }
  else grid->DeleteRows(nrows,old_nrows-nrows);
  grid->EndBatch( );
}
DStructGDL* GDLWidgetTable::GetGeometry(wxRealPoint fact) 
{
  int ixs=0, iys=0, ixscr=0, iyscr=0;
  float xs, ys, xscr, yscr, xoff, yoff, margin=0;
  wxPoint position;
  int rowsize=1;
  int rowlabelsize=0;
  int colsize=1;
  int collabelsize=0;
  gdlGrid* grid = static_cast<gdlGrid*> (wxWidget);
  assert( grid != NULL);
  if ( grid != NULL ) {
    grid->GetClientSize( &ixs, &iys );
    ixscr=ixs;
    iyscr=iys;
    position = grid->GetPosition( );
    rowsize=grid->GetRowSize(0);
    rowlabelsize=grid->GetRowLabelSize();
    colsize=grid->GetColSize(0);
    collabelsize=grid->GetColLabelSize();
  }
  if (frameSizer != NULL) {framePanel->GetSize(&ixscr,&iyscr);  margin = gdlFRAME_MARGIN / fact.x;}
  if (scrollSizer != NULL) {scrollPanel->GetSize(&ixscr,&iyscr);ixs=ixscr-gdlSCROLL_WIDTH;iys=iyscr-gdlSCROLL_WIDTH;}
  //size is in pixels, pass in requested units (1.0 default)
  xs =  (ixs-rowlabelsize) / colsize  ;
  ys =  (iys-collabelsize) / rowsize  ;
  xscr = ixscr / fact.x;
  yscr = iyscr / fact.y; 
  xoff = position.x / fact.x;
  yoff = position.y / fact.y;

  DStructGDL* ex = new DStructGDL( "WIDGET_GEOMETRY" );
  ex->InitTag( "XOFFSET", DFloatGDL( xoff) );
  ex->InitTag( "YOFFSET", DFloatGDL( yoff ) );
  ex->InitTag( "XSIZE", DFloatGDL( xs ) );
  ex->InitTag( "YSIZE", DFloatGDL( ys ) );
  ex->InitTag( "SCR_XSIZE", DFloatGDL( xscr ) );
  ex->InitTag( "SCR_YSIZE", DFloatGDL( yscr ) );
  ex->InitTag( "MARGIN", DFloatGDL( margin ) );
  return ex;
}

GDLWidgetTable::~GDLWidgetTable()
{
#ifdef GDL_DEBUG_WIDGETS
    std::cout << "~GDLWidgetTable: " << this << std::endl;
#endif  
  GDLDelete( table_alignment );
  GDLDelete( editable );
  GDLDelete( amPm );
  GDLDelete( backgroundColor );
  GDLDelete( foregroundColor );
  GDLDelete( columnLabels );
  GDLDelete( columnWidth );
  GDLDelete( daysOfWeek );
  GDLDelete( format );
  GDLDelete( month );
  GDLDelete( rowHeights );
  GDLDelete( rowLabels );
  GDLDelete( valueAsStrings );
}

/*********************************************************/
// for WIDGET_TREE
/*********************************************************/

GDLWidgetTree::GDLWidgetTree( WidgetIDT p, EnvT* e, BaseGDL* value_, DULong eventFlags_
//,bool alignBottom_
//,bool alignTop_
,wxBitmap* bitmap
//,bool checkbox_
//,DLong checked_
//,DString dragNotify_
,DLong dragability
,DLong dropability
,bool expanded_
,bool folder_
,DLong treeindex
//,bool mask_
//,bool multiple_
//,bool noBitmaps_
//,DLong tabMode_
//,DString toolTip_ 
)
: GDLWidget( p, e, value_, eventFlags_ )
//,alignBottom( alignBottom_ )
//,alignTop( alignTop_ )
//,checkbox( checkbox_ )
//,checked( checked_ )
//,dragNotify( dragNotify_ )
,droppable( FALSE )
,draggable( FALSE )
//,mask( mask_ )
//,multiple( multiple_ )
//,noBitmaps( noBitmaps_ )
//,tabMode( tabMode_ )
//,toolTip( toolTip_ )
,expanded(expanded_)
,folder(folder_)
//,treeItemID( 0L ) //is an object!
,rootID(0L)
,buttonImageId(0L)
,imageId(0L)
,treeItemData(NULL)
{
  GDLWidget* gdlParent = GetWidget( parentID );
  wxPanel *panel = gdlParent->GetPanel( );
  widgetPanel = panel;
  widgetSizer = gdlParent->GetSizer( );
  topWidgetSizer = this->GetTopLevelBaseWidget( parentID )->GetSizer( );
  DStringGDL* value=static_cast<DStringGDL*>(vValue);

  if ( gdlParent->IsBase( ) ) {
    if ( xSize <= 0 ) xSize = 200; //yes, has a default value!
    if ( ySize <= 0 ) ySize = 200;

    long style = (wxTR_HIDE_ROOT|wxTR_DEFAULT_STYLE| wxTR_HAS_BUTTONS | wxSUNKEN_BORDER | wxTR_TWIST_BUTTONS)    ;
    // should be as of 2.9.0:  wxDataViewTreeCtrl* tree = new gdlTreeCtrl( widgetPanel, widgetID,
    gdlTreeCtrl* tree = new gdlTreeCtrl( widgetPanel, widgetID,
    wxPoint( xOffset, yOffset ),
    computeWidgetSize( ), style );
    //our widget will ALWAYS have an image list...
    wxImageList* images=new wxImageList(16,16,TRUE,3);
    images->Add(wxArtProvider::GetBitmap(wxART_FOLDER)); //0
    images->Add(wxArtProvider::GetBitmap(wxART_FOLDER_OPEN)); //1
    images->Add(wxArtProvider::GetBitmap(wxART_NORMAL_FILE)); //2
    images->Add(wxArtProvider::GetBitmap(wxART_FILE_OPEN)); //3

    tree->AssignImageList(images);
    folder=TRUE;
    this->wxWidget = tree;
    rootID=widgetID;
    treeItemData=new gdlTreeItemData(widgetID);
    if (bitmap) {
      int index=images->Add(*bitmap);
      treeItemID = tree->AddRoot(wxString( (*value)[0].c_str( ), wxConvUTF8 ),  index ,-1, treeItemData);
    } else { //use open and closed folder icons
      treeItemID = tree->AddRoot(wxString( (*value)[0].c_str( ), wxConvUTF8 ),  0 ,1, treeItemData);
    }    
    widgetStyle=widgetAlignment( );
    widgetSizer->Add( tree, 0, widgetStyle, 0  ); 
    if ( (frameWidth>0) ) this->FrameWidget( );
    draggable=(dragability == 1);
    droppable=(dropability == 1);
//    tree->Expand(treeItemID); //do not expand root if hidden
    
    TIDY_WIDGET;
    UPDATE_WINDOW
  } else {
    GDLWidgetTree* parentTree = static_cast<GDLWidgetTree*> (gdlParent);
    assert( parentTree != NULL);
    wxWidget = parentTree->GetWxWidget( );
    rootID =  parentTree->GetRootID();
    treeItemData=new gdlTreeItemData(widgetID);
    gdlTreeCtrl * tree = static_cast<gdlTreeCtrl*> (wxWidget);
    assert( tree != NULL);

    wxImageList* images=tree->GetImageList();
    //if image is provided use it otherwise (since no image is a bit disappointing) use an internal wxWigdets icon
    if (bitmap) {
      int imindex=images->Add(*bitmap);
      if (treeindex>-1) treeItemID = tree->InsertItem( parentTree->treeItemID, treeindex, wxString( (*value)[0].c_str( ), wxConvUTF8 ) , imindex ,-1, treeItemData);
      else treeItemID = tree->AppendItem( parentTree->treeItemID, wxString( (*value)[0].c_str( ), wxConvUTF8 ) , imindex ,-1, treeItemData);
    } else { //use open and closed folder icons
      if (folder) {
        if (treeindex>-1) treeItemID = tree->InsertItem( parentTree->treeItemID, treeindex, wxString( (*value)[0].c_str( ), wxConvUTF8 ) ,0,1, treeItemData);
        else treeItemID = tree->AppendItem( parentTree->treeItemID, wxString( (*value)[0].c_str( ), wxConvUTF8 ) ,0,1, treeItemData);
      }
      else if (treeindex>-1) treeItemID = tree->InsertItem( parentTree->treeItemID, treeindex, wxString( (*value)[0].c_str( ), wxConvUTF8 ) ,2,3, treeItemData);
      else  treeItemID = tree->AppendItem( parentTree->treeItemID, wxString( (*value)[0].c_str( ), wxConvUTF8 ) ,2,3, treeItemData);
    }
    if ( parentTree->IsFolder() && parentTree->IsExpanded())  parentTree->DoExpand();
    //dragability inheritance.
    if (dragability == -1) draggable=parentTree->IsDraggable(); else draggable=(dragability == 1);
   //dropability inheritance.
    if (dropability == -1) droppable=parentTree->IsDroppable(); else droppable=(dropability == 1);
  }
}
DInt GDLWidgetTree::GetTreeIndex()
{
  DInt count=0;
  gdlTreeCtrl* tree=static_cast<gdlTreeCtrl*>(wxWidget);
  assert( tree != NULL);
  wxTreeItemId  id=this->treeItemID;
  wxTreeItemId prev_id=tree->GetPrevSibling(id);
  while (prev_id.IsOk()){
    count++; 
    id=prev_id; 
    prev_id=tree->GetPrevSibling(id);
  }
  return count+1; //to give compatible results with idl -- wxwidgets does not behave as idl!
}

GDLWidgetTree::~GDLWidgetTree()
{
#ifdef GDL_DEBUG_WIDGETS
    std::cout << "~GDLWidgetTree: " << this << std::endl;
#endif  
  gdlTreeCtrl* tree=static_cast<gdlTreeCtrl*>(wxWidget);
  if (tree) {
  wxTreeItemId  id=this->treeItemID;
  if (id.IsOk()) {
    wxTreeItemIdValue cookie;
    wxTreeItemId child=tree->GetFirstChild(id, cookie);
    while (child.IsOk()) {
      WidgetIDT childID=static_cast<gdlTreeItemData*>(tree->GetItemData(child))->widgetID;
      GDLWidgetTree* GDLchild= static_cast<GDLWidgetTree*>(GDLWidget::GetWidget(childID));
      child=tree->GetNextSibling(child); //nextChild did not find the last (?) nextSibling does the job.
      delete GDLchild;
    }
    treeItemID.Unset();
    tree->Delete(id);
  }
  }
}

void GDLWidgetTree::SetValue(DString val)
{
  GDLDelete(vValue); 
  vValue=new DStringGDL(val);
  gdlTreeCtrl* tree=static_cast<gdlTreeCtrl*>(wxWidget);
  assert( tree != NULL);
  tree->SetItemText(treeItemID, wxString( val.c_str( ), wxConvUTF8 ));
}

/*********************************************************/
// for WIDGET_SLIDER
/*********************************************************/
GDLWidgetSlider::GDLWidgetSlider( WidgetIDT p, EnvT* e, DLong value_
, DULong eventFlags_
, DLong minimum_
, DLong maximum_
, bool vertical
, bool suppressValue
, DString title_ )
: GDLWidget( p, e, NULL, eventFlags_ )
, value( value_ ) //should disappear , duplicates vValue!
, minimum( minimum_ )
, maximum( maximum_ )
, title( title_)
{
  GDLWidget* gdlParent = GetWidget( parentID );
  widgetPanel = gdlParent->GetPanel( );
  widgetSizer = gdlParent->GetSizer( );
  topWidgetSizer = this->GetTopLevelBaseWidget(parentID)->GetSizer();

  if (( xSize <= 0 ) && !vertical) xSize=200; //yes, has a default value!
  if (( ySize <= 0 ) && vertical) ySize=200;

  long style = wxSL_HORIZONTAL;
  if ( vertical ) style = wxSL_VERTICAL;
  if ( !suppressValue ) {
#if wxCHECK_VERSION(2,9,1)
    style |= wxSL_VALUE_LABEL;
#else
    style |= wxSL_LABELS;
#endif
  }
  wxSlider * slider;

  widgetStyle=widgetAlignment();
  if (title.size()>0){
    wxBoxSizer *sz = new wxBoxSizer(wxVERTICAL);
    wxPanel *p = new wxPanel(widgetPanel, wxID_ANY , wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
    slider = new wxSlider( p, widgetID, value, minimum, maximum, wxPoint( xOffset, yOffset ), computeWidgetSize( ), style );
    wxStaticText *st=new wxStaticText(p,wxID_ANY,wxString( title.c_str( ), wxConvUTF8 ));
    sz->Add(slider);
    sz->Add(st);  
    p->SetSizer(sz);
    widgetSizer->Add(p, 0, widgetStyle, 0);
//      if ((frameWidth>0)) this->FrameWidget(); //do not frame (problems: would need a different widget): already framed by default.

  } else {
    slider = new wxSlider( widgetPanel , widgetID, value, minimum, maximum, wxPoint( xOffset, yOffset ), computeWidgetSize( ), style );
    widgetSizer->Add(slider, 0, widgetStyle, 0 ); 
    widgetStyle=(wxEXPAND | wxALL);
//    if ((frameWidth>0)) this->FrameWidget(); //do not frame, crashes on a bug (FIXME).
  } 
  this->wxWidget = slider;
  slider->Connect(widgetID,wxEVT_SCROLL_CHANGED,wxScrollEventHandler(GDLFrame::OnThumbRelease));
  //dynamically select drag, saves resources! (note: there is no widget_control,/drag for sliders)
  if ( eventFlags & GDLWidget::EV_DRAG ) slider->Connect(widgetID,wxEVT_SCROLL_THUMBTRACK,wxScrollEventHandler(GDLFrame::OnThumbTrack));

  TIDY_WIDGET;
  UPDATE_WINDOW
}
void GDLWidgetSlider::ControlSetValue(DLong v){
  value=v;
  wxSlider* s=static_cast<wxSlider*>(wxWidget);
  assert( s != NULL);
  s->SetValue(v);
}

void GDLWidgetSlider::ControlSetMinValue(DLong v) {
  value = v;
  wxSlider* s = static_cast<wxSlider*> (wxWidget);
  assert(s != NULL);
  s->SetRange(v, s->GetMax());
}

void GDLWidgetSlider::ControlSetMaxValue(DLong v) {
  value = v;
  wxSlider* s = static_cast<wxSlider*> (wxWidget);
  assert(s != NULL);
  s->SetRange(s->GetMin(),v);
}

GDLWidgetSlider::~GDLWidgetSlider(){
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetSlider(): " << widgetID << std::endl;
#endif
}

GDLWidgetButton::GDLWidgetButton( WidgetIDT p, EnvT* e,
const DString& value , DULong eventflags, bool isMenu, bool hasSeparatorAbove, wxBitmap* bitmap_, DStringGDL* buttonToolTip)
: GDLWidget( p, e, NULL, eventflags )
, buttonType( UNDEFINED )
, addSeparatorAbove(hasSeparatorAbove)
, buttonBitmap(bitmap_)
, menuItem(NULL)
{
  GDLWidget* gdlParent = GetWidget( parentID );
  widgetPanel = gdlParent->GetPanel( );
  widgetSizer = gdlParent->GetSizer( );
  topWidgetSizer = this->GetTopLevelBaseWidget(parentID)->GetSizer();

  //update vValue
  delete(vValue);
  vValue = new DStringGDL( value );

  if ( gdlParent->IsMenuBar( ) )
  {
    //A button in a menubar is automatically a MENU
    wxMenuBar *menuBar = static_cast<wxMenuBar*> (gdlParent->GetWxWidget( ));
    assert( menuBar != NULL);
    wxMenu* menu = new wxMenu( );
    wxWidget = menu;
    wxString valueWxString = wxString( value.c_str( ), wxConvUTF8 );
    menuBar->Append( menu, valueWxString );
    buttonType = MENU;
  }
  else
  { 
    if ( gdlParent->IsBase( ) && isMenu )
    {      
     //A menu button in a base is a button starting a popup menu. it is a container also.
      wxString MenuTitle = wxString( value.c_str( ), wxConvUTF8 );
      //2 different buttons, if bitmap or not, waiting for version > 2.9.1 to have bitmaps handled for ALL buttontypes:
      if (bitmap_ == NULL) {
        gdlMenuButton *button = new gdlMenuButton( widgetPanel, widgetID, MenuTitle, 
            wxPoint( xOffset, yOffset ) , computeWidgetSize( ), buttonTextAlignment());
        buttonType = POPUP_NORMAL; //gdlMenuButton is a wxButton --> normal. Bitmaps will be supported starting from 2.9.1 
        wxWidget = button;
//        wxWidget = button->GetPopupMenu();
      wxWindow* win=static_cast<wxWindow*>(button);
      if (buttonToolTip) win->SetToolTip( wxString((*buttonToolTip)[0].c_str(),wxConvUTF8));
      widgetSizer->Add( win, 0, widgetAlignment(), 0);
      } else {
        gdlMenuButtonBitmap *button = new gdlMenuButtonBitmap( widgetPanel, widgetID, *bitmap_, 
            wxPoint( xOffset, yOffset ) , computeWidgetSize( ), buttonTextAlignment());
        buttonType = POPUP_BITMAP; //
        wxWidget = button;
//        wxWidget = button->GetPopupMenu();
      wxWindow* win=static_cast<wxWindow*>(button);
      if (buttonToolTip) win->SetToolTip( wxString((*buttonToolTip)[0].c_str(),wxConvUTF8));
      widgetSizer->Add( win, 0, widgetAlignment(), 0);
      }

      if ((frameWidth>0)) { widgetStyle= widgetAlignment(); this->FrameWidget();}
      widgetSizer->Layout();
      
      TIDY_WIDGET;
      UPDATE_WINDOW 
// Check influence of:
//      widgetType=GDLWidget::WIDGET_MBAR;
    } 
    else if( gdlParent->IsButton()) //so it is a container
    {
      //get default value: a menu. May be NULL here
      wxMenu *menu = static_cast<wxMenu*> (gdlParent->GetWxWidget( ));
      //special treatment for popups menus: the menu is retrieved differently
      GDLWidgetButton* whatSortofBut=static_cast<GDLWidgetButton*>(gdlParent);
      if (whatSortofBut->buttonType==POPUP_NORMAL ) {
        menu=static_cast<gdlMenuButton*>(whatSortofBut->GetWxWidget())->GetPopupMenu();
      } else if (whatSortofBut->buttonType==POPUP_BITMAP ) {
        menu=static_cast<gdlMenuButtonBitmap*>(whatSortofBut->GetWxWidget())->GetPopupMenu();
      } else {
        if ( dynamic_cast<wxMenu*> (gdlParent->GetWxWidget( )) == NULL ) {
          if (e!=NULL) { 
            e->Throw("Parent is of incorrect type.");
          } else { //yes, e may be null. Not here, but for draw windows.
            cerr<<"Impossible case of null environment, please report to authors!"<<endl;
          }
        }
      }

        if (menu) {
          if (isMenu) {
            if (addSeparatorAbove) menu->AppendSeparator();
            wxMenu *submenu=new wxMenu( );
            wxWidget = submenu;
            wxString valueWxString = wxString( value.c_str( ), wxConvUTF8 );
            if (bitmap_) {
              menuItem = menu->AppendSubMenu(submenu, wxString( " ", wxConvUTF8 ) ); //the ' ' is needed. Null strings raise complaints by wxWidgets (???)
              menuItem->SetBitmap(*bitmap_);
            } else {
              menuItem = menu->AppendSubMenu(submenu, valueWxString );
            }
            buttonType = MENU;
            TIDY_WIDGET;
            UPDATE_WINDOW 
          }
          else //only an entry.
          {
            wxString valueWxString = wxString( value.c_str( ), wxConvUTF8 );
            if (bitmap_) {
              menuItem = new wxMenuItem( menu, widgetID,  wxString( " ", wxConvUTF8 ) ); //the ' ' is needed. Null strings raise complaints by wxWidgets (???)
              menuItem->SetBitmap(*bitmap_);
            } else {
              menuItem = new wxMenuItem( menu, widgetID, valueWxString );
            }
            if (addSeparatorAbove) menu->AppendSeparator();
            menu->Append( menuItem );
            wxWidget = NULL; // should be menuItem; but is not even a wxWindow!
            buttonType = ENTRY;
          }
        } else cerr<<"Warning GDLWidgetButton::GDLWidgetButton(): widget type confusion(2)\n";
    }
    else 
    {
      //we deliberately prevent exclusive buttons when bitmap are present (exclusive buttons w/ pixmap do not exist in wxWidgets.
      if ( gdlParent->GetExclusiveMode() == BGNORMAL || bitmap_ ) 
      {
        if (bitmap_){
          wxBitmapButton *button = new wxBitmapButton( widgetPanel, widgetID, *bitmap_,
          wxPoint( xOffset, yOffset ) ,computeWidgetSize( ));
          this->wxWidget = button;
          buttonType = BITMAP;
          button->Connect(widgetID, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GDLFrame::OnButton));
        } else {
          wxString valueWxString = wxString( value.c_str( ), wxConvUTF8 );
          wxButton *button = new wxButton( widgetPanel, widgetID, valueWxString,
          wxPoint( xOffset, yOffset ) , computeWidgetSize( ), buttonTextAlignment());
          this->wxWidget = button;
          buttonType = NORMAL;
          button->Connect(widgetID, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GDLFrame::OnButton));
        }
      }
      else if ( gdlParent->GetExclusiveMode() == BGEXCLUSIVE1ST) 
      {
        wxString valueWxString = wxString( value.c_str( ), wxConvUTF8 );
        wxRadioButton *radioButton = new wxRadioButton( widgetPanel, widgetID, valueWxString,
        wxPoint( xOffset, yOffset ), computeWidgetSize( ), //wxDefaultSize,
        wxRB_GROUP );
        gdlParent->SetExclusiveMode( 1 );
        GDLWidgetBase* b=static_cast<GDLWidgetBase*> (gdlParent);
        if (b) b->SetLastRadioSelection( widgetID ); else cerr<<"Warning GDLWidgetButton::GDLWidgetButton(): widget type confusion(3)\n";

        this->wxWidget = radioButton;
        radioButton->Connect(widgetID, wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(GDLFrame::OnRadioButton));
        buttonType = RADIO;
      } 
      else if ( gdlParent->GetExclusiveMode() == BGEXCLUSIVE) 
      {
        wxString valueWxString = wxString( value.c_str( ), wxConvUTF8 );
        wxRadioButton *radioButton = new wxRadioButton( widgetPanel, widgetID, valueWxString,
        wxPoint( xOffset, yOffset ), computeWidgetSize( ) );
        this->wxWidget = radioButton;
        radioButton->Connect(widgetID, wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(GDLFrame::OnRadioButton));
        buttonType = RADIO;
      } 
      else if ( gdlParent->GetExclusiveMode() == BGNONEXCLUSIVE) 
      {
        wxString valueWxString = wxString( value.c_str( ), wxConvUTF8 );
        wxCheckBox *checkBox = new wxCheckBox( widgetPanel, widgetID, valueWxString,
        wxPoint( xOffset, yOffset ),computeWidgetSize( ) );
        this->wxWidget = checkBox;
        checkBox->Connect(widgetID, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(GDLFrame::OnCheckBox));
        buttonType = CHECKBOX;
      }
      wxWindow *win=static_cast<wxWindow*>(wxWidget);
      if (win) {
        if (buttonToolTip) win->SetToolTip( wxString((*buttonToolTip)[0].c_str(),wxConvUTF8));
        widgetSizer->Add( win, 0, widgetAlignment(), 0);
      } else cerr<<"Warning GDLWidgetButton::GDLWidgetButton(): widget type confusion(4)\n";
      if ((frameWidth>0)) { widgetStyle= widgetAlignment(); this->FrameWidget();}
      widgetSizer->Layout();
      TIDY_WIDGET;
      UPDATE_WINDOW
    }
  }
}
GDLWidgetButton::~GDLWidgetButton(){
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetButton(): " << widgetID << std::endl;
#endif
    switch(buttonType){
      case MENU:
      case ENTRY:
        if (menuItem) {
          menuItem->GetMenu()->Remove(menuItem); //do not destroy the wxWidgets object, troubles will follow.
        }
        break;
      default:
        break;
    }
}

void GDLWidgetButton::SetButtonWidgetLabelText( const DString& value_ ) {
  if ( buttonType == BITMAP ) return;
  if ( buttonType == POPUP_BITMAP ) return;
  if ( buttonType == UNDEFINED ) return;
  //update vValue
  delete(vValue);
  vValue = new DStringGDL( value_ );

  if ( buttonType == NORMAL ) {
    if ( this->wxWidget != NULL ) {
      wxButton *b = static_cast<wxButton*> (wxWidget);
      assert(b!=NULL);
      b->SetLabel( wxString( value_.c_str( ), wxConvUTF8 ) );
      } else std::cerr << "Null widget in GDLWidgetButton::SetButtonWidgetLabelText(), please report!" << std::endl;
    return;
  }
  if ( buttonType == POPUP_NORMAL ) {
    if ( this->wxWidget != NULL ) {
      wxButton *b = static_cast<wxButton*> (wxWidget);
      assert(b!=NULL);
      b->SetLabel( wxString( value_.c_str( ), wxConvUTF8 ) );
      } else std::cerr << "Null widget in GDLWidgetButton::SetButtonWidgetLabelText(), please report!" << std::endl;
    return;
  }
  if ( buttonType == RADIO ) {
    if ( this->wxWidget != NULL ) {
      wxRadioButton *b = static_cast<wxRadioButton*> (wxWidget);
      assert(b!=NULL);
      b->SetLabel( wxString( value_.c_str( ), wxConvUTF8 ) );
      } else std::cerr << "Null widget in GDLWidgetButton::SetButtonWidgetLabelText(), please report!" << std::endl;
    return;
  }
  if ( buttonType == MENU || buttonType ==ENTRY) {
    if ( menuItem != NULL ) {
      menuItem->SetItemLabel( wxString( value_.c_str( ), wxConvUTF8 ) );
      } else std::cerr << "Problem in GDLWidgetButton::SetButtonWidgetLabelText(), please report!" << std::endl;
    return;
  }
  if ( buttonType == CHECKBOX ) {
    if ( this->wxWidget != NULL ) {
      wxCheckBox *b = static_cast<wxCheckBox*> (wxWidget);
      assert(b!=NULL);
      b->SetLabel( wxString( value_.c_str( ), wxConvUTF8 ) );
      } else std::cerr << "Null widget in GDLWidgetButton::SetButtonWidgetLabelText(), please report!" << std::endl;
    return;
  }
}

void GDLWidgetButton::SetButtonWidgetBitmap( wxBitmap* bitmap_ ) {
  if ( buttonType == POPUP_NORMAL ) return;
  if ( buttonType == BITMAP || buttonType == POPUP_BITMAP ) {
    wxBitmapButton *b = static_cast<wxBitmapButton*> (wxWidget);
    if ( b ) b->SetBitmapLabel( *bitmap_ );
    return;
  } 

  if ( buttonType == MENU || buttonType ==ENTRY) {
    if ( menuItem != NULL ) menuItem->SetBitmap( *bitmap_ );
    return;
  }

    //probably not useful yet - TBC
#if wxCHECK_VERSION(2,9,1)
     if ( buttonType == NORMAL ) {
      wxButton *b = static_cast<wxButton*> (wxWidget);
      if ( b ) b->SetBitmapLabel( *bitmap_ );
    } 
#endif    
}
  
GDLWidgetList::GDLWidgetList( WidgetIDT p, EnvT* e, BaseGDL *value, DLong style, DULong eventflags )
    : GDLWidget( p, e, value, eventflags)
, maxlinelength(0)
, nlines(0)
{
  GDLWidget* gdlParent = GetWidget( parentID );
  widgetPanel = gdlParent->GetPanel( );
  widgetSizer = gdlParent->GetSizer( );
  topWidgetSizer = this->GetTopLevelBaseWidget(parentID)->GetSizer();

  if( vValue->Type() != GDL_STRING)
  {
    vValue = static_cast<DStringGDL*> (vValue->Convert2( GDL_STRING, BaseGDL::CONVERT ));
  }
  DStringGDL* val = static_cast<DStringGDL*> (vValue);

  nlines = val->N_Elements( );
  wxArrayString choices; // = new wxString[n];

  for ( SizeT i = 0; i < nlines; ++i ){
    int length=(*val)[i].length();
    maxlinelength=(length>maxlinelength)?length:maxlinelength;
  choices.Add( wxString( (*val)[i].c_str( ), wxConvUTF8 ) );
  }
  
  wxListBox * list = new wxListBox( gdlParent->GetPanel( ), widgetID, wxPoint( xOffset, yOffset ),
  computeWidgetSize( ),
  choices, style|wxLB_NEEDED_SB );
  this->wxWidget = list;

  list->Connect(widgetID,wxEVT_COMMAND_LISTBOX_DOUBLECLICKED,wxCommandEventHandler(GDLFrame::OnListBoxDoubleClicked));
  list->Connect(widgetID,wxEVT_COMMAND_LISTBOX_SELECTED,wxCommandEventHandler(GDLFrame::OnListBox));
  list->SetSelection(wxNOT_FOUND);//necessary to avoid spurious event on first click.
  widgetStyle=widgetAlignment();
  if ((frameWidth>0)) this->FrameWidget();  else  widgetSizer->Add(list, 0, widgetStyle, 0); 
  TIDY_WIDGET;
  UPDATE_WINDOW
}
void GDLWidgetList::SetValue(BaseGDL *value){
  GDLDelete(vValue);
  vValue=value;
 if( vValue->Type() != GDL_STRING)
  {
    vValue = static_cast<DStringGDL*> (vValue->Convert2( GDL_STRING, BaseGDL::CONVERT ));
  }
  DStringGDL* val = static_cast<DStringGDL*> (vValue);

  wxArrayString newchoices; // = new wxString[n];
  for ( SizeT i = 0; i < val->N_Elements( ); ++i )  newchoices.Add( wxString( (*val)[i].c_str( ), wxConvUTF8 ) );
  
  wxListBox * list = static_cast<wxListBox*>(wxWidget);
  assert( list != NULL);
  
  list->Clear();
  list->InsertItems(newchoices,0);
  list->SetSelection(wxNOT_FOUND);
}

void GDLWidgetList::SelectEntry(DLong entry_number){
 wxListBox * list=static_cast<wxListBox*>(wxWidget);
  assert( list != NULL);
 list->Select(entry_number); 
}

BaseGDL* GDLWidgetList::GetSelectedEntries(){
 wxListBox * list=static_cast<wxListBox*>(wxWidget);
  assert( list != NULL);
 wxArrayInt selections;
 list->GetSelections(selections);
 DIntGDL* liste;
 if  (selections.Count()<1) {
   liste=new DIntGDL(-1);
   return liste;
 }
 liste=new DIntGDL(dimension(selections.Count()));
 for (SizeT i=0; i< selections.Count(); ++i) (*liste)[i]=selections[i];
 return liste;
}

DStructGDL* GDLWidgetList::GetGeometry(wxRealPoint fact) 
{
  int ixs=0, iys=0, ixscr=0, iyscr=0;
  float xs, ys, xscr, yscr, xoff, yoff, margin;
  wxPoint position;
  wxSize fontSize=wxNORMAL_FONT->GetPixelSize();
  wxListBox* test = static_cast<wxListBox*> (wxWidget);
  assert( test != NULL);
  if ( test != NULL ) {
    test->GetClientSize( &ixs, &iys );
    ixscr=ixs;
    iyscr=iys;
    position = test->GetPosition( );
    fontSize = test->GetFont().GetPixelSize(); 
  }
  //Apparently Windows may return 0 for fontsize.x (probably if too small)?
  if (fontSize.x == 0) fontSize.x=0.65*fontSize.y; //last chance to get a correct value
  if (fontSize.x == 0) fontSize.x=1;
  if (fontSize.y == 0) fontSize.y=1;
  
  if (frameSizer != NULL) {framePanel->GetSize(&ixscr,&iyscr);  margin = gdlFRAME_MARGIN / fact.x;}
  if (scrollSizer != NULL) {scrollPanel->GetSize(&ixscr,&iyscr);ixs=ixscr-gdlSCROLL_WIDTH;iys=iyscr-gdlSCROLL_WIDTH;}
  //size is in pixels, pass in requested units (1.0 default)
  xs = ixs / fontSize.x;
  ys = iys / fontSize.y;
  xscr = ixscr / fact.x;
  yscr = iyscr / fact.y; 
  xoff = position.x / fact.x;
  yoff = position.y / fact.y;

  DStructGDL* ex = new DStructGDL( "WIDGET_GEOMETRY" );
  ex->InitTag( "XOFFSET", DFloatGDL( xoff) );
  ex->InitTag( "YOFFSET", DFloatGDL( yoff ) );
  ex->InitTag( "XSIZE", DFloatGDL( xs ) );
  ex->InitTag( "YSIZE", DFloatGDL( ys ) );
  ex->InitTag( "SCR_XSIZE", DFloatGDL( xscr ) );
  ex->InitTag( "SCR_YSIZE", DFloatGDL( yscr ) );
  ex->InitTag( "MARGIN", DFloatGDL( margin ) );
  return ex;
}
GDLWidgetList::~GDLWidgetList(){
#ifdef GDL_DEBUG_WIDGETS
    std::cout << "~GDLWidgetList: " << this << std::endl;
#endif  
}

GDLWidgetDropList::GDLWidgetDropList( WidgetIDT p, EnvT* e, BaseGDL *value, DULong eventflags ,
const DString& title_, DLong style_ )
: GDLWidget( p, e, static_cast<DStringGDL*> (value->Convert2( GDL_STRING, BaseGDL::CONVERT )), eventflags)
, title( title_ )
, style( style_)
{
  GDLWidget* gdlParent = GetWidget( parentID );
  widgetPanel = gdlParent->GetPanel( );
  widgetSizer = gdlParent->GetSizer( );
  topWidgetSizer = this->GetTopLevelBaseWidget(parentID)->GetSizer();
  
  DStringGDL* val = static_cast<DStringGDL*> (vValue);
  DLong n = val->N_Elements( );
  wxArrayString choices; // = new wxString[n];
  for ( SizeT i = 0; i < n; ++i )
  {choices.Add( wxString( (*val)[i].c_str( ), wxConvUTF8 ) );}
   
   wxChoice * droplist = new wxChoice( widgetPanel, widgetID, 
   wxPoint( xOffset, yOffset ), computeWidgetSize( ), choices, style );
   droplist->SetSelection(0);
   this->wxWidget = droplist;
   widgetStyle=widgetAlignment();
   if (title.size()>0){
      wxBoxSizer *sz = new wxBoxSizer(wxHORIZONTAL);
      wxPanel *p = new wxPanel(widgetPanel);
      wxStaticText *st=new wxStaticText(p,wxID_ANY,wxString( title.c_str( ), wxConvUTF8 ));
      droplist->Reparent(p);
      sz->Add(st, 0, wxALIGN_CENTRE, 0);  
      sz->AddSpacer(5);
      sz->Add(droplist, 0, wxALIGN_CENTRE, 0);
      p->SetSizer(sz);
      widgetSizer->Add(p, 0, widgetStyle, 0);
    } else {
      widgetSizer->Add( droplist, 0, widgetStyle, 0);
      if ((frameWidth>0)) this->FrameWidget();
    }
    droplist->Connect(widgetID, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(GDLFrame::OnDropList));

    TIDY_WIDGET;   
    UPDATE_WINDOW
}
  
void GDLWidgetDropList::SetValue(BaseGDL *value){
   GDLDelete(vValue);
  vValue=value;
 if( vValue->Type() != GDL_STRING)
  {
    vValue = static_cast<DStringGDL*> (vValue->Convert2( GDL_STRING, BaseGDL::CONVERT ));
  }
  DStringGDL* val = static_cast<DStringGDL*> (vValue);
  wxArrayString newchoices; // = new wxString[n];
  for ( SizeT i = 0; i < val->N_Elements( ); ++i )  newchoices.Add( wxString( (*val)[i].c_str( ), wxConvUTF8 ) );

  wxChoice* droplist=static_cast<wxChoice*>(wxWidget);
  assert( droplist != NULL);
  droplist->Clear();
  droplist->Append(newchoices);
  droplist->SetSelection(0);
}

void GDLWidgetDropList::SelectEntry(DLong entry_number){
 wxChoice * droplist=static_cast<wxChoice*>(wxWidget);
 assert( droplist != NULL);
 droplist->Select(entry_number); 
}

BaseGDL* GDLWidgetDropList::GetSelectedEntry(){
  wxChoice * droplist=static_cast<wxChoice*>(wxWidget);
  assert( droplist != NULL);
  return new DIntGDL(droplist->GetSelection());
}

GDLWidgetDropList::~GDLWidgetDropList(){
#ifdef GDL_DEBUG_WIDGETS
    std::cout << "~GDLWidgetDroplist: " << this << std::endl;
#endif  
}
  
GDLWidgetComboBox::GDLWidgetComboBox( WidgetIDT p, EnvT* e, BaseGDL *value, DULong eventflags,
const DString& title_, DLong style_ )
: GDLWidget( p, e, static_cast<DStringGDL*> (value->Convert2( GDL_STRING, BaseGDL::CONVERT )) , eventflags)
, title( title_ )
, style( style_)
{
  GDLWidget* gdlParent = GetWidget( parentID );
  widgetPanel = gdlParent->GetPanel( );
  widgetSizer = gdlParent->GetSizer( );
  topWidgetSizer = this->GetTopLevelBaseWidget(parentID)->GetSizer();

  DStringGDL* val = static_cast<DStringGDL*> (vValue);
  DLong n = val->N_Elements( );
  wxArrayString choices; // = new wxString[n];
  for ( SizeT i = 0; i < n; ++i )
  choices.Add( wxString( (*val)[i].c_str( ), wxConvUTF8 ) );

  wxString val0WxString = wxString( (*val)[0].c_str( ), wxConvUTF8 );

  wxComboBox * combo = new wxComboBox( widgetPanel, widgetID, val0WxString,
  wxPoint( xOffset, yOffset ), computeWidgetSize( ), choices, style );
  this->wxWidget = combo;
  combo->Connect(widgetID,wxEVT_COMMAND_COMBOBOX_SELECTED,wxCommandEventHandler(GDLFrame::OnComboBox));
  if (style & wxTE_PROCESS_ENTER) combo->Connect(widgetID,wxEVT_COMMAND_TEXT_ENTER,wxCommandEventHandler(GDLFrame::OnComboBoxTextEnter));
  widgetStyle=widgetAlignment();
  widgetSizer->Add(combo, 0, widgetStyle, 0);
  if ((frameWidth>0)) this->FrameWidget();
  TIDY_WIDGET;
  UPDATE_WINDOW
}

void GDLWidgetComboBox::SetValue(BaseGDL *value){
  GDLDelete(vValue);
  vValue=value;
 if( vValue->Type() != GDL_STRING)
  {
    vValue = static_cast<DStringGDL*> (vValue->Convert2( GDL_STRING, BaseGDL::CONVERT ));
  }
  DStringGDL* val = static_cast<DStringGDL*> (vValue);
  wxArrayString newchoices; // = new wxString[n];
  for ( SizeT i = 0; i < val->N_Elements( ); ++i )  newchoices.Add( wxString( (*val)[i].c_str( ), wxConvUTF8 ) );

  wxComboBox * combo = static_cast<wxComboBox*>(wxWidget);
  assert( combo != NULL);
  combo->Clear();
  combo->Append(newchoices);
  combo->SetSelection(0,0);
}

void GDLWidgetComboBox::AddItem(DString value, DLong pos) {
  wxComboBox * combo = static_cast<wxComboBox*>(wxWidget);
  assert( combo != NULL);
  int nvalues=combo->GetCount();
  if (pos == -1) combo->Append( wxString( value.c_str( ), wxConvUTF8 ) );
  else if (pos > -1  && pos < nvalues) combo->Insert( wxString( value.c_str( ), wxConvUTF8 ), pos);
}

void GDLWidgetComboBox::DeleteItem(DLong pos) {
  wxComboBox * combo = static_cast<wxComboBox*>(wxWidget);
  assert( combo != NULL);
  int nvalues=combo->GetCount();
  int selected=combo->GetSelection();
  if (pos > -1  && pos < nvalues) combo->Delete( pos );
  if (pos==selected && (nvalues-1) > 0 ) combo->Select((pos+1)%(nvalues-1)); else if (selected==-1 && pos==0) combo->Select(0);
}

void GDLWidgetComboBox::SelectEntry(DLong entry_number){
 wxComboBox * combo=static_cast<wxComboBox*>(wxWidget);
  assert( combo != NULL);
 combo->Select(entry_number); 
}

BaseGDL* GDLWidgetComboBox::GetSelectedEntry(){
  wxComboBox * combo=static_cast<wxComboBox*>(wxWidget);
  assert( combo != NULL);
  DStringGDL* stringres=new DStringGDL(dimension(1));
  (*stringres)[0]=combo->GetStringSelection().mb_str(wxConvUTF8); //return null string if no selection
  return stringres;
}

GDLWidgetComboBox::~GDLWidgetComboBox(){
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetComboBox(): " << widgetID << std::endl;
#endif
}

GDLWidgetText::GDLWidgetText( WidgetIDT p, EnvT* e, DStringGDL* valueStr, DULong eventflags, bool noNewLine_,
bool editable_ )
: GDLWidget( p, e, valueStr, eventflags )
, noNewLine( noNewLine_ )
, editable(editable_)
{
  DString value = "";
  maxlinelength = 0;
  nlines=0;  
  if (ySize<2 && !scrolled) noNewLine=TRUE;
  if( vValue != NULL)
  {
      for( int i=0; i<vValue->N_Elements(); ++i)
      {
        int length=((*static_cast<DStringGDL*> (vValue))[i]).length();
        value += (*static_cast<DStringGDL*> (vValue))[i]; 
        maxlinelength=(length>maxlinelength)?length:maxlinelength;
        if ( !noNewLine ) //&& (i + 1) < vValue->N_Elements( ) )
#ifdef _WIN32
        {value += "\r\n"; nlines++;}
#else
        {value += '\n'; nlines++;}
#endif
      }
  }
  lastValue = value;

  GDLWidget* gdlParent = GetWidget( parentID );
  widgetPanel = gdlParent->GetPanel( );
  widgetSizer = gdlParent->GetSizer( );
  topWidgetSizer = this->GetTopLevelBaseWidget(parentID)->GetSizer();
  
  wxString valueWxString = wxString( lastValue.c_str( ), wxConvUTF8 );
  long style = wxTE_NOHIDESEL|wxTE_PROCESS_ENTER|textAlignment();
  if ( ySize > 1 || scrolled ) style |= wxTE_MULTILINE;
//  else style |= wxTE_NO_VSCROLL;

  wxTextCtrl * text;
  if ( !editable ) {
//   style |= wxTE_READONLY; //note: *DL provides READONLY and still give events.
   text = new gdlTextCtrl( widgetPanel, widgetID, valueWxString,  wxPoint( xOffset, yOffset ), computeWidgetSize(), style ); // this permits to a procedure (cw_field.pro for example) to filter the editing of text fields.
// so wxTextCtrl::OnChar is overwritten in gdlwidgeteventhandler.cpp just for this reason.
 } else {
   text = new wxTextCtrl( widgetPanel, widgetID, valueWxString, wxPoint( xOffset, yOffset ), computeWidgetSize(), style );
 }
  text->SetInsertionPoint(0);
  text->SetSelection(0,0);
  this->wxWidget = text;
  text->Connect(widgetID,wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(GDLFrame::OnTextEnter));
  text->Connect(widgetID,wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(GDLFrame::OnText));
  widgetStyle=widgetAlignment();  
  widgetSizer->Add(text, 0, widgetStyle , 0);
  if ((frameWidth>0)) this->FrameWidget(); 
  TIDY_WIDGET;
  UPDATE_WINDOW
}
GDLWidgetText::~GDLWidgetText(){
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetText(): " << widgetID << std::endl;
#endif
}

void GDLWidgetText::ChangeText( DStringGDL* valueStr, bool noNewLine)
{
  delete vValue;
  vValue = valueStr;
  DString value = "";
  if (ySize < 2 && !scrolled) noNewLine=TRUE;
  nlines=0; 
    for( int i=0; i<valueStr->N_Elements(); ++i)
    {
    value += (*valueStr)[ i];
    if ( !noNewLine )
#ifdef _WIN32
    {value += "\r\n"; nlines++;}
#else
    {value += '\n'; nlines++;}
#endif
    }
  lastValue = value;

  wxString valueWxString = wxString( lastValue.c_str( ), wxConvUTF8 );
  if ( this->wxWidget != NULL ) {
    wxTextCtrl* txt=static_cast<wxTextCtrl*> (wxWidget);
    assert( txt != NULL);
    txt->ChangeValue( valueWxString ); //by contrast with SetValue, does not generate an EVENT -- IDL does not either.    
//    txt->Refresh();
//    txt->Update();
  }  else std::cerr << "Null widget in GDLWidgetText::SetTextValue(), please report!" << std::endl;
}

void GDLWidgetText::InsertText( DStringGDL* valueStr, bool noNewLine, bool insertAtEnd)
{
  long from,to;
  wxTextCtrl* txt=static_cast<wxTextCtrl*> (wxWidget);
  assert( txt != NULL);
  txt->GetSelection(&from,&to);
  if (insertAtEnd) {from=txt->GetLastPosition(); to=from;}

  if (ySize < 2 && !scrolled) noNewLine=TRUE;

  DString value = "";
  nlines=0;
  for ( int i = 0; i < valueStr->N_Elements( ); ++i ) {
    value += (*valueStr)[ i];
    if ( !noNewLine ) {
#ifdef _WIN32
      value += "\r\n"; nlines++;
#else
      value += '\n'; nlines++;
#endif
    }
  }
  lastValue.replace(from,to-from,value);
  delete vValue;
  vValue = new DStringGDL(lastValue);
//  cout<<from<<"->"<<to<<":"<<lastValue.c_str( )<<endl;
  wxString valueWxString = wxString( lastValue.c_str( ), wxConvUTF8 );
  if ( this->wxWidget != NULL ) {
    txt->ChangeValue( valueWxString ); //by contrast with SetValue, does not generate an EVENT (neither does *DL).    
//    txt->Refresh();
//    txt->Update();
    txt->SetSelection(from,from);
  }  else std::cerr << "Null widget in GDLWidgetText::SetTextValue(), please report!" << std::endl;
}

void GDLWidgetText::SetTextSelection(DLongGDL* pos)
{ //*DL selection are [start,length] [3,20] -> pos (3,23)
  //numerous cases to handle with invalid values (negative, too large).
  //Hope it works for all cases:
  long from,to,len;
  long oldSelStart, oldSelEnd;
  wxTextCtrl* txt=static_cast<wxTextCtrl*> (wxWidget);
  assert( txt != NULL);
  
  txt->GetSelection(&oldSelStart,&oldSelEnd);
  len = txt->GetLastPosition()+1;

  if ((*pos)[0] < 0 ) from=0;
  else if ((*pos)[0] > len ) from=len;
  else from=(*pos)[0];

  
  if (pos->N_Elements()<2) {
    //selection = insertion
    txt->SetSelection(from,from);
    return;
  }
  //2 elements, define selection
  to=(*pos)[1];

  if ( from+to >= len ) {
    //set insertion position at end of last selection (?)
    txt->SetSelection(oldSelEnd,oldSelEnd);
    return;
  }
  if ( to < 0 ) {
    //set insertion position at from
    txt->SetSelection(from,from);
    return;
  }
  txt->SetSelection(from,from+to);
}

DLongGDL* GDLWidgetText::GetTextSelection()
{
  DLongGDL* pos=new DLongGDL(dimension(2),BaseGDL::ZERO);
  long from,to;
  wxTextCtrl* txt=static_cast<wxTextCtrl*> (wxWidget);
  assert( txt != NULL);
  
  txt->GetSelection(&from,&to);
  (*pos)[0]=from; (*pos)[1]=to-from;
  return pos;
}

DStringGDL* GDLWidgetText::GetSelectedText()
{
  wxTextCtrl* txt=static_cast<wxTextCtrl*>(wxWidget);
  assert( txt != NULL);
  
return new DStringGDL(txt->GetStringSelection().mb_str(wxConvUTF8).data());
}

GDLWidgetLabel::GDLWidgetLabel( WidgetIDT p, EnvT* e, const DString& value_ , DULong eventflags, bool sunken)
: GDLWidget( p, e , NULL, eventflags )
, value(value_)
{
  GDLWidget* gdlParent = GetWidget( parentID );
  widgetPanel = gdlParent->GetPanel( );
  widgetSizer = gdlParent->GetSizer( );
  topWidgetSizer = this->GetTopLevelBaseWidget(parentID)->GetSizer();

  wxString valueWxString = wxString( value.c_str( ), wxConvUTF8 );
  wxStaticText* label = new wxStaticText( widgetPanel, widgetID, valueWxString,
  wxPoint( xOffset, yOffset ), computeWidgetSize(), textAlignment()|wxST_NO_AUTORESIZE|(sunken)?wxBORDER_SUNKEN:0 );
  this->wxWidget = label;
  widgetStyle=widgetAlignment();
  widgetSizer->Add(label,0,widgetStyle, 0);
  if ((frameWidth>0)) this->FrameWidget();     
  TIDY_WIDGET;
  UPDATE_WINDOW
}

DStructGDL* GDLWidgetText::GetGeometry(wxRealPoint fact) 
{
  int ixs=0, iys=0, ixscr=0, iyscr=0;
  float xs, ys, xscr, yscr, xoff, yoff, margin;
  wxPoint position;
  wxSize fontSize=wxNORMAL_FONT->GetPixelSize();
  wxTextCtrl* txt = static_cast<wxTextCtrl*> (wxWidget);
  assert( txt != NULL);
  
  if ( txt != NULL ) {
    txt->GetClientSize( &ixs, &iys );
    ixscr=ixs;
    iyscr=iys;
    position = txt->GetPosition( );
    fontSize = txt->GetFont().GetPixelSize(); 
  }
  //Apparently Windows may return 0 for fontsize.x (probably if too small)?
  if (fontSize.x == 0) fontSize.x=0.65*fontSize.y; //last chance to get a correct value
  if (fontSize.x == 0) fontSize.x=1;
  if (fontSize.y == 0) fontSize.y=1;

  if (frameSizer != NULL) {framePanel->GetSize(&ixscr,&iyscr);  margin = gdlFRAME_MARGIN / fact.x;}
  if (scrollSizer != NULL) {scrollPanel->GetSize(&ixscr,&iyscr);ixs=ixscr-gdlSCROLL_WIDTH;iys=iyscr-gdlSCROLL_WIDTH;}
  //size is in pixels, pass in requested units (1.0 default)
  xs = ixs / fontSize.x;
  ys = iys / fontSize.y;
  xscr = ixscr / fact.x;
  yscr = iyscr / fact.y; 
  xoff = position.x / fact.x;
  yoff = position.y / fact.y;

  DStructGDL* ex = new DStructGDL( "WIDGET_GEOMETRY" );
  ex->InitTag( "XOFFSET", DFloatGDL( xoff) );
  ex->InitTag( "YOFFSET", DFloatGDL( yoff ) );
  ex->InitTag( "XSIZE", DFloatGDL( xs ) );
  ex->InitTag( "YSIZE", DFloatGDL( ys ) );
  ex->InitTag( "SCR_XSIZE", DFloatGDL( xscr ) );
  ex->InitTag( "SCR_YSIZE", DFloatGDL( yscr ) );
  ex->InitTag( "MARGIN", DFloatGDL( margin ) );
  return ex;
}


GDLWidgetLabel::~GDLWidgetLabel(){
#ifdef GDL_DEBUG_WIDGETS
    std::cout << "~GDLWidgetLabel: " << this << std::endl;
#endif  
}

void GDLWidget::FrameWidget()
{
  //to frame a widget, create a panel and a staticBoxSizer as panel's child. Put the widget inside the panel
  //an replace the widget wby the panel in the parent sizer.
  if (this->IsBase()) return; //function invalid with base widgets.
  if (frameSizer==NULL) { //protect against potential problems
    //panel style:
//    long panelStyle=widgetStyle&wxBORDER_MASK; //wxBORDER_MASK selects only the 'border' part of the Style, to pass to wxPanel
    long alignment=widgetStyle&wxALIGN_MASK; //same for alignment
    framePanel = new wxPanel(this->widgetPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
    frameSizer = new wxBoxSizer( wxHORIZONTAL );
    framePanel->SetSizer( frameSizer );

    if (scrollSizer==NULL) {
      widgetSizer->Detach(static_cast<wxWindow*>(this->wxWidget));
      static_cast<wxWindow*>(this->wxWidget)->Reparent(framePanel);
      frameSizer->Add(static_cast<wxWindow*>(this->wxWidget),0,wxALL,(frameWidth==-1)?gdlFRAME_MARGIN:frameWidth );
    } else { 
      widgetSizer->Detach(scrollPanel);
      static_cast<wxWindow*>(this->scrollPanel)->Reparent(framePanel);
      frameSizer->Add(static_cast<wxWindow*>(this->scrollPanel), 0,wxALL,(frameWidth==-1)?gdlFRAME_MARGIN:frameWidth);
    }
    widgetSizer->Add(framePanel, 0, alignment, 0);
//    frameSizer->Layout();
//    widgetSizer->Layout();
  }
}

void GDLWidget::UnFrameWidget()
{
  if (this->IsBase()) return; //function invalid with base widgets.
  if (frameSizer!=NULL) { //protect against potential problems
    widgetSizer->Detach(framePanel);
//    long panelStyle=widgetStyle&wxBORDER_MASK; //wxBORDER_MASK selects only the 'border' part of the Style, to pass to wxPanel
    long alignment=widgetStyle&wxALIGN_MASK; //same for alignment
    if (scrollSizer==NULL) {
      frameSizer->Detach(static_cast<wxWindow*>(this->wxWidget));
      static_cast<wxWindow*>(this->wxWidget)->Reparent(widgetPanel);
      widgetSizer->Add(static_cast<wxWindow*>(this->wxWidget), 0, alignment, 0);
    } else {
      frameSizer->Detach(static_cast<wxWindow*>(this->scrollPanel));
      static_cast<wxWindow*>(this->scrollPanel)->Reparent(widgetPanel);
      widgetSizer->Add(static_cast<wxWindow*>(this->scrollPanel),0, alignment, 0);
    }
    delete framePanel;
    frameSizer=NULL;
    framePanel=NULL;
//    widgetSizer->Layout();
  }
}

void GDLWidget::ScrollWidget(  DLong x_scroll_size,  DLong y_scroll_size)
{
  if (this->IsBase()) return; //function invalid with base widgets.
  if (scrollSizer==NULL) { //protect against potential problems
    scrollPanel = new wxScrolledWindow(this->widgetPanel, wxID_ANY, wxPoint(xOffset,yOffset), wxSize(x_scroll_size, y_scroll_size ), wxBORDER_SUNKEN);
    scrollPanel->SetScrollRate(gdlSCROLL_RATE, gdlSCROLL_RATE); //show scrollbars
    scrollSizer = new wxBoxSizer(wxVERTICAL );
    scrollPanel->SetSizer( scrollSizer );

    static_cast<wxWindow*>(this->wxWidget)->Reparent(scrollPanel);
    scrollSizer->Add(static_cast<wxWindow*>(this->wxWidget));
    if (frameSizer!=NULL) {
      frameSizer->Detach(static_cast<wxWindow*>(this->wxWidget));
      frameSizer->Add(scrollPanel,0, wxFIXED_MINSIZE|wxALL, gdlFRAME_MARGIN);
    } else {
      widgetSizer->Detach(static_cast<wxWindow*>(this->wxWidget));
      widgetSizer->Add(scrollPanel, 0, wxFIXED_MINSIZE|widgetStyle, 0);
    }
    widgetSizer->Layout();
  }
}
void GDLWidget::UnScrollWidget()
{
  if (this->IsBase()) return; //function invalid with base widgets.
  if (scrollSizer!=NULL) { //protect against potential problems
    scrollSizer->Detach(static_cast<wxWindow*>(this->wxWidget));
    if (frameSizer!=NULL) {
     static_cast<wxWindow*>(this->wxWidget)->Reparent(framePanel);
     frameSizer->Detach(scrollPanel);
     frameSizer->Add(static_cast<wxWindow*>(this->wxWidget), 0, widgetStyle, gdlFRAME_MARGIN);
     frameSizer->Layout();
    } else {
     static_cast<wxWindow*>(this->wxWidget)->Reparent(widgetPanel);
     widgetSizer->Detach(scrollPanel);
     widgetSizer->Add(static_cast<wxWindow*>(this->wxWidget), 0, widgetStyle, 0);
    }
    delete scrollPanel;
    scrollSizer=NULL;
    scrollPanel=NULL;
    widgetSizer->Layout();
 }
}

//void GDLWidgetLabel::OnShow()
//{
//  //  std::cout << "In Label: " << widgetID << " " << p << std::endl;
//}

void GDLWidgetLabel::SetLabelValue( const DString& value_)
{
    value = value_;
    //update vValue
    delete(vValue);
    vValue = new DStringGDL( value );
    
    wxString valueWxString = wxString( value_.c_str( ), wxConvUTF8 );
    if ( this->wxWidget != NULL ) {
      wxWindowID id = static_cast<wxStaticText*> (wxWidget)->GetId( );
      wxStaticText *txt= static_cast<wxStaticText*> (wxWidget);
      if (txt) txt->SetLabel( valueWxString ); else cerr << "Warning GDLWidgetLabel::SetLabelValue(): widget type confusion\n";
      //SetLabel, unless SetLabelText, interprets markup (<b></b> etc)
  }    else std::cerr << "Null widget in GDLWidgetLabel::SetLabelValue(), please report!" << std::endl;
}
//propertysheet
#ifdef HAVE_WXWIDGETS_PROPERTYGRID

 GDLWidgetPropertySheet::GDLWidgetPropertySheet( WidgetIDT parentID, EnvT* e)
 : GDLWidget( p, e , value, eventflag)
{
  GDLWidget* gdlParent = GetWidget( parentID );
  widgetPanel = gdlParent->GetPanel( );
  widgetSizer = gdlParent->GetSizer( );
  topWidgetSizer = this->GetTopLevelBaseWidget(parentID)->GetSizer();
 // Construct wxPropertyGrid control
  wxPropertyGrid* pg = new wxPropertyGrid(gdlParent,wxID_ANY,wxDefaultPosition,wxDefaultSize,
  // Here are just some of the supported window styles
  wxPG_AUTO_SORT | // Automatic sorting after items added
  wxPG_SPLITTER_AUTO_CENTER | // Automatically center splitter until user manually adjusts it
  // Default style
  wxPG_DEFAULT_STYLE );
// Window style flags are at premium, so some less often needed ones are
// available as extra window styles (wxPG_EX_xxx) which must be set using
// SetExtraStyle member function. wxPG_EX_HELP_AS_TOOLTIPS, for instance,
// allows displaying help strings as tool tips.
  pg->SetExtraStyle( wxPG_EX_HELP_AS_TOOLTIPS );
  
  this->wxWidget=pg;
  
  // Add int property
pg->Append( new wxIntProperty("IntProperty", wxPG_LABEL, 12345678) );
// Add float property (value type is actually double)
pg->Append( new wxFloatProperty("FloatProperty", wxPG_LABEL, 12345.678) );
// Add a bool property
pg->Append( new wxBoolProperty("BoolProperty", wxPG_LABEL, false) );
// A string property that can be edited in a separate editor dialog.
pg->Append( new wxLongStringProperty("LongStringProperty",
wxPG_LABEL,
"This is much longer string than the "
"first one. Edit it by clicking the button."));
// String editor with dir selector button.
pg->Append( new wxDirProperty("DirProperty", wxPG_LABEL, ::wxGetUserHome()) );
// wxArrayStringProperty embeds a wxArrayString.
pg->Append( new wxArrayStringProperty("Label of ArrayStringProperty",
"NameOfArrayStringProp"));
// A file selector property.
pg->Append( new wxFileProperty("FileProperty", wxPG_LABEL, wxEmptyString) );

  if (frame) this->FrameWidget(widgetStyle);
TIDY_WIDGET;
UPDATE_WINDOW
}
#endif
// GDL widgets =====================================================
// GDLFrame ========================================================
GDLFrame::GDLFrame( GDLWidgetBase* gdlOwner_, wxWindowID id, const wxString& title , const bool nofocus, const wxPoint& pos )
: wxFrame( NULL, id, title, pos, wxDefaultSize, (nofocus)? wxMINIMIZE_BOX|wxMAXIMIZE_BOX|wxCLOSE_BOX|wxFRAME_TOOL_WINDOW:wxDEFAULT_FRAME_STYLE )
, mapped( false )
, noFocus ( nofocus ) //for graphic windows. They do not have focus. Never.
, frameSize(wxSize(0,0))
, gdlOwner( gdlOwner_)
, appOwner(NULL)
{
  m_resizeTimer = new wxTimer(this,RESIZE_TIMER);
  m_windowTimer = new wxTimer(this,WINDOW_TIMER);
}

GDLFrame::~GDLFrame()
{ 
    if (m_resizeTimer->IsRunning()) m_resizeTimer->Stop(); 
    if (m_windowTimer->IsRunning()) m_windowTimer->Stop(); //really necessary, try stopping xdice.pro when rolling dices..
#ifdef GDL_DEBUG_WIDGETS
    std::cout << "~GDLFrame: " << this << std::endl;
#endif  
 //frame is part of a TLB. if frame is destroyed, destroy TLB if still existing. 
 if( gdlOwner != NULL)
  {
#ifdef GDL_DEBUG_WIDGETS
    std::cout << "~GDLFrame: Destroying Base Container" << gdlOwner->WidgetID() << std::endl;
#endif
    gdlOwner->NullWxWidget( ); //remove one's reference from container
    gdlOwner->SelfDestroy( ); // send delete request to GDL owner = container.
  }
 }


// GDLDrawPanel ========================================================
GDLDrawPanel::GDLDrawPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
: wxPanel( parent, id, pos, size, style, name )
, pstreamIx( -1 )
, pstreamP( NULL )
, m_dc( NULL)
, GDLWidgetDrawID(id)
, drawSize(size)
{
}

void GDLDrawPanel::InitStream(int windowIndex)
{
  if (windowIndex < 0)  pstreamIx = GraphicsDevice::GetGUIDevice( )->WAddFree( );
  else  pstreamIx = windowIndex;
  if ( pstreamIx == -1 )
  throw GDLException( "Failed to allocate GUI stream." );

  bool success = GraphicsDevice::GetGUIDevice( )->GUIOpen( pstreamIx, this->GetClientSize( ).x, this->GetClientSize( ).y );
  if( !success)
  {
    throw GDLException( "Failed to open GUI stream: " + i2s( pstreamIx ) );
  }
  pstreamP = static_cast<GDLWXStream*> (GraphicsDevice::GetGUIDevice( )->GetStreamAt( pstreamIx ));
  pstreamP->SetGDLDrawPanel( this );

  m_dc = pstreamP->GetDC( );
}

//void GDLDrawPanel::AssociateStream(GDLWXStream* stream)
//{
//  pstreamP = stream;
//  pstreamP->SetGDLDrawPanel( this );
//  m_dc = pstreamP->GetDC( );
//}


void GDLDrawPanel::Resize(int sizex, int sizey)
{
  if (pstreamP != NULL)
  {
// not useful as long we recreate a wxstream!    
//    pstreamP->SetSize(sizex,sizey);
    
//The following should not be necessary . It is a bad idea to create a new stream, but font size handling
//with plplot's wx implementation that is too awkward to do anything else now.
    
  //get a new stream with good dimensions. Only possibility (better than resize) to have correct size of fonts and symbols.
    GDLWXStream * newStream =  new GDLWXStream(sizex, sizey);
  // replace old by new, called function destroys old:
    GraphicsDevice::GetGUIDevice( )->ChangeStreamAt( pstreamIx, newStream );  
    pstreamP = static_cast<GDLWXStream*> (GraphicsDevice::GetGUIDevice( )->GetStreamAt( pstreamIx ));
    pstreamP->SetGDLDrawPanel( this );
    m_dc = pstreamP->GetDC( );
  }
  drawSize=wxSize(sizex,sizey);
} 

GDLDrawPanel::~GDLDrawPanel()
{  
#ifdef GDL_DEBUG_WIDGETS
     std::cout << "~GDLDrawPanel: " << this << std::endl;
//     std::cout << "This IsMainThread: " << wxIsMainThread() << std::endl;
#endif
//  if (m_resizeTimer->IsRunning()) m_resizeTimer->Stop(); 
  if ( pstreamP != NULL )
  pstreamP->SetValid( false ); //panel destroyed in future call to TidyWindowsList().
}

GDLWidgetDraw::GDLWidgetDraw( WidgetIDT p, EnvT* e, int windowIndex, DLong special_xsize, DLong special_ysize,
			      DLong x_scroll_size_, DLong y_scroll_size_, bool app_scroll, DULong eventFlags_, DStringGDL* drawToolTip)
  : GDLWidget( p, e, NULL, eventFlags_)
  , pstreamIx(windowIndex)
  , x_scroll_size(x_scroll_size_)
  , y_scroll_size(y_scroll_size_)
{
  //  std::cout << "In GDLWidgetDraw::GDLWidgetDraw: " << widgetID << std::endl
  assert( parentID != GDLWidget::NullID);

  if (xSize <= 0 && special_xsize > 0) xSize = special_xsize; //override in case this is a graphic window (no env kw used since no command)
  if (ySize <= 0 && special_ysize > 0) ySize = special_ysize; //override in case this is a graphic window (no env kw used since no command)

  //get immediately rid of scroll sizes in case of scroll or not... Here is the logic:
  if (app_scroll) scrolled=TRUE;
  if (x_scroll_size > 0) {scrolled=TRUE;x_scroll_size*=unitConversionFactor.x; x_scroll_size+=(gdlSCROLL_WIDTH);} 
  if (y_scroll_size > 0) {scrolled=TRUE;y_scroll_size*=unitConversionFactor.y; y_scroll_size+=(gdlSCROLL_WIDTH);}
  if (scrolled) x_scroll_size=(x_scroll_size<100)?100:x_scroll_size; //min values
  if (scrolled) y_scroll_size=(y_scroll_size<100)?100:y_scroll_size;

  if ( xSize <= 0 ) xSize=scrolled?100+gdlSCROLL_WIDTH:100; else xSize*=unitConversionFactor.x; //provide a default value!
  if ( ySize <= 0 ) ySize=scrolled?100+gdlSCROLL_WIDTH:100; else ySize*=unitConversionFactor.y;

  GDLWidget* gdlParent = GetWidget( parentID);
  widgetPanel = gdlParent->GetPanel( );
  widgetSizer = gdlParent->GetSizer( );

  long style = 0;
  
  GDLDrawPanel* draw = new GDLDrawPanel( widgetPanel, widgetID, wxPoint(xOffset,yOffset), wxSize(xSize,ySize), style);
 
  //these widget specific events are always set:
   draw->Connect(widgetID, wxEVT_PAINT, wxPaintEventHandler(GDLDrawPanel::OnPaint));
   draw->Connect(widgetID, wxEVT_SIZE, wxSizeEventHandler(GDLDrawPanel::OnSize));
   draw->Connect(widgetID, wxEVT_ERASE_BACKGROUND, wxEraseEventHandler(GDLDrawPanel::OnErase));

  //other set event handling according to flags
  if (eventFlags & GDLWidget::EV_MOTION) draw->Connect(widgetID, wxEVT_MOTION, wxMouseEventHandler(GDLDrawPanel::OnMouseMove));
//  if ( eventFlags & GDLWidget::EV_DROP) nothing to do yet, fixme!;
//  if ( eventFlags & GDLWidget::EV_EXPOSE) nothing to do yet, fixme!;
//  if ( eventFlags &  GDLWidget::EV_VIEWPORT) idem;
  if ( eventFlags &  GDLWidget::EV_WHEEL) draw->Connect(widgetID, wxEVT_MOUSEWHEEL, wxMouseEventHandler(GDLDrawPanel::OnMouseWheel)); 
  if ( eventFlags &  GDLWidget::EV_BUTTON) {
      draw->Connect(widgetID, wxEVT_LEFT_DOWN, wxMouseEventHandler(GDLDrawPanel::OnMouseDown)); 
      draw->Connect(widgetID, wxEVT_LEFT_UP, wxMouseEventHandler(GDLDrawPanel::OnMouseUp)); 
      draw->Connect(widgetID, wxEVT_LEFT_DCLICK, wxMouseEventHandler(GDLDrawPanel::OnMouseDown)); 
      draw->Connect(widgetID, wxEVT_MIDDLE_DOWN, wxMouseEventHandler(GDLDrawPanel::OnMouseDown)); 
      draw->Connect(widgetID, wxEVT_MIDDLE_DCLICK, wxMouseEventHandler(GDLDrawPanel::OnMouseDown)); 
      draw->Connect(widgetID, wxEVT_MIDDLE_UP, wxMouseEventHandler(GDLDrawPanel::OnMouseUp)); 
      draw->Connect(widgetID, wxEVT_RIGHT_DOWN, wxMouseEventHandler(GDLDrawPanel::OnMouseDown)); 
      draw->Connect(widgetID, wxEVT_RIGHT_DCLICK, wxMouseEventHandler(GDLDrawPanel::OnMouseDown)); 
      draw->Connect(widgetID, wxEVT_RIGHT_UP, wxMouseEventHandler(GDLDrawPanel::OnMouseUp)); 
  }
  if (eventFlags &  GDLWidget::EV_KEYBOARD2 || eventFlags & GDLWidget::EV_KEYBOARD){
       draw->Connect(widgetID, wxEVT_KEY_DOWN, wxKeyEventHandler(GDLDrawPanel::OnKey)); 
       draw->Connect(widgetID, wxEVT_KEY_UP, wxKeyEventHandler(GDLDrawPanel::OnKey)); 
  }
  
  draw->SetCursor(wxCURSOR_CROSS);
  if (drawToolTip) static_cast<wxWindow*>(draw)->SetToolTip( wxString((*drawToolTip)[0].c_str(),wxConvUTF8));
  wxWidget = draw;
  widgetStyle= widgetAlignment();
  widgetSizer->Add( draw );// , 0, widgetStyle, 0);
  if (scrolled) this->ScrollWidget(x_scroll_size, y_scroll_size );
  if ((frameWidth>0)) this->FrameWidget();

  static_cast<GDLDrawPanel*>(wxWidget)->InitStream(windowIndex);
  
  pstreamIx = static_cast<GDLDrawPanel*>(wxWidget)->PStreamIx();
  GDLDelete( vValue);
  this->vValue = new DLongGDL(pstreamIx);  
  this->SetSensitive(sensitive);
  TIDY_WIDGET;
  UPDATE_WINDOW  
}

void GDLWidgetDraw::AddEventType( DULong evType){
  GDLDrawPanel* draw=(GDLDrawPanel*)wxWidget;
//this one is for the moment defined globally:
//   if ( evType == GDLWidget::EV_TRACKING) { 
//    draw->Connect(widgetID, wxEVT_ENTER_WINDOW, wxMouseEventHandler(GDLFrame::OnEnterWindow));
//    draw->Connect(widgetID, wxEVT_LEAVE_WINDOW, wxMouseEventHandler(GDLFrame::OnLeaveWindow));
//  } else
  if ( evType == GDLWidget::EV_MOTION) draw->Connect(widgetID, wxEVT_MOTION, wxMouseEventHandler(GDLDrawPanel::OnMouseMove));
//else  if ( evType == GDLWidget::EV_DROP) nothing to do yet, fixme!;
//else  if ( evType == GDLWidget::EV_EXPOSE) nothing to do yet, fixme!;
//else  if ( evType == GDLWidget::EV_VIEWPORT) idem;
  else if ( evType == GDLWidget::EV_WHEEL) draw->Connect(widgetID, wxEVT_MOUSEWHEEL, wxMouseEventHandler(GDLDrawPanel::OnMouseWheel)); 
  else if ( evType == GDLWidget::EV_BUTTON) {
      draw->Connect(widgetID, wxEVT_LEFT_DOWN, wxMouseEventHandler(GDLDrawPanel::OnMouseDown)); 
      draw->Connect(widgetID, wxEVT_LEFT_UP, wxMouseEventHandler(GDLDrawPanel::OnMouseUp)); 
      draw->Connect(widgetID, wxEVT_LEFT_DCLICK, wxMouseEventHandler(GDLDrawPanel::OnMouseDown)); 
      draw->Connect(widgetID, wxEVT_MIDDLE_DOWN, wxMouseEventHandler(GDLDrawPanel::OnMouseDown)); 
      draw->Connect(widgetID, wxEVT_MIDDLE_DCLICK, wxMouseEventHandler(GDLDrawPanel::OnMouseDown)); 
      draw->Connect(widgetID, wxEVT_MIDDLE_UP, wxMouseEventHandler(GDLDrawPanel::OnMouseUp)); 
      draw->Connect(widgetID, wxEVT_RIGHT_DOWN, wxMouseEventHandler(GDLDrawPanel::OnMouseDown)); 
      draw->Connect(widgetID, wxEVT_RIGHT_DCLICK, wxMouseEventHandler(GDLDrawPanel::OnMouseDown)); 
      draw->Connect(widgetID, wxEVT_RIGHT_UP, wxMouseEventHandler(GDLDrawPanel::OnMouseUp)); 
  }
  else if (evType == GDLWidget::EV_KEYBOARD2 || evType == GDLWidget::EV_KEYBOARD){
       draw->Connect(widgetID, wxEVT_KEY_DOWN, wxKeyEventHandler(GDLDrawPanel::OnKey)); 
       draw->Connect(widgetID, wxEVT_KEY_UP, wxKeyEventHandler(GDLDrawPanel::OnKey)); 
  } 
}

void GDLWidgetDraw::RemoveEventType( DULong evType){
  GDLDrawPanel* draw=(GDLDrawPanel*)wxWidget;
//this one is for the moment defined globally:
//   if ( evType == GDLWidget::EV_TRACKING) { 
//    draw->Disconnect(widgetID, wxEVT_ENTER_WINDOW, wxMouseEventHandler(GDLFrame::OnEnterWindow));
//    draw->Disconnect(widgetID, wxEVT_LEAVE_WINDOW, wxMouseEventHandler(GDLFrame::OnLeaveWindow));
//  } else
  if ( evType == GDLWidget::EV_MOTION) draw->Disconnect(widgetID, wxEVT_MOTION, wxMouseEventHandler(GDLDrawPanel::OnMouseMove));
//else  if ( evType == GDLWidget::EV_DROP) nothing to do yet, fixme!;
//else  if ( evType == GDLWidget::EV_EXPOSE) nothing to do yet, fixme!;
//else  if ( evType == GDLWidget::EV_VIEWPORT) idem;
  else if ( evType == GDLWidget::EV_WHEEL) draw->Disconnect(widgetID, wxEVT_MOUSEWHEEL, wxMouseEventHandler(GDLDrawPanel::OnMouseWheel)); 
  else if ( evType == GDLWidget::EV_BUTTON) {
      draw->Disconnect(widgetID, wxEVT_LEFT_DOWN, wxMouseEventHandler(GDLDrawPanel::OnMouseDown)); 
      draw->Disconnect(widgetID, wxEVT_LEFT_UP, wxMouseEventHandler(GDLDrawPanel::OnMouseUp)); 
      draw->Disconnect(widgetID, wxEVT_LEFT_DCLICK, wxMouseEventHandler(GDLDrawPanel::OnMouseDown)); 
      draw->Disconnect(widgetID, wxEVT_MIDDLE_DOWN, wxMouseEventHandler(GDLDrawPanel::OnMouseDown)); 
      draw->Disconnect(widgetID, wxEVT_MIDDLE_DCLICK, wxMouseEventHandler(GDLDrawPanel::OnMouseDown)); 
      draw->Disconnect(widgetID, wxEVT_MIDDLE_UP, wxMouseEventHandler(GDLDrawPanel::OnMouseUp)); 
      draw->Disconnect(widgetID, wxEVT_RIGHT_DOWN, wxMouseEventHandler(GDLDrawPanel::OnMouseDown)); 
      draw->Disconnect(widgetID, wxEVT_RIGHT_DCLICK, wxMouseEventHandler(GDLDrawPanel::OnMouseDown)); 
      draw->Disconnect(widgetID, wxEVT_RIGHT_UP, wxMouseEventHandler(GDLDrawPanel::OnMouseUp)); 
  }
  else if (evType == GDLWidget::EV_KEYBOARD2 || evType == GDLWidget::EV_KEYBOARD){
       draw->Disconnect(widgetID, wxEVT_KEY_DOWN, wxKeyEventHandler(GDLDrawPanel::OnKey)); 
       draw->Disconnect(widgetID, wxEVT_KEY_UP, wxKeyEventHandler(GDLDrawPanel::OnKey)); 
  } 
}
DStructGDL* GDLWidgetDraw::GetGeometry( wxRealPoint fact ) {
  int ixds=0, iyds=0, ixs=0, iys=0, ixscr=0, iyscr=0;
  float xds, yds, xs, ys, xscr, yscr, xoff, yoff, margin;
  wxPoint position;
  GDLDrawPanel* test = static_cast<GDLDrawPanel*> (wxWidget);
  if ( test != NULL ) {
    test->GetSize( &ixs, &iys );
    ixscr=ixs;
    iyscr=iys;
    test->GetClientSize( &ixds, &iyds );
    position = test->GetPosition( );
  }
  if (frameSizer != NULL) {framePanel->GetSize(&ixscr,&iyscr);  margin = gdlFRAME_MARGIN / fact.x;}
  if (scrollSizer != NULL) {scrollPanel->GetSize(&ixscr,&iyscr);ixs=ixscr-gdlSCROLL_WIDTH;iys=iyscr-gdlSCROLL_WIDTH;}
  //size is in pixels, pass in requested units (1.0 default)
  xs = ixs / fact.x;
  ys = iys / fact.y;
  xds = ixds / fact.x;
  yds = iyds / fact.y;
  xscr = ixscr / fact.x;
  yscr = iyscr / fact.y; 
  xoff = position.x / fact.x;
  yoff = position.y / fact.y;

  DStructGDL* ex = new DStructGDL( "WIDGET_GEOMETRY" );
  ex->InitTag( "XOFFSET", DFloatGDL( xoff) );
  ex->InitTag( "YOFFSET", DFloatGDL( yoff ) );
  ex->InitTag( "XSIZE", DFloatGDL( xs ) );
  ex->InitTag( "YSIZE", DFloatGDL( ys ) );
  ex->InitTag( "SCR_XSIZE", DFloatGDL( xscr ) );
  ex->InitTag( "SCR_YSIZE", DFloatGDL( yscr ) );
  ex->InitTag( "DRAW_XSIZE", DFloatGDL( xds ) );
  ex->InitTag( "DRAW_YSIZE", DFloatGDL( yds ) );
  ex->InitTag( "MARGIN", DFloatGDL( margin ) );
  return ex;
}

// GDLApp =================================================

#include "wx/evtloop.h"
#include "wx/ptr_scpd.h"

wxDEFINE_TIED_SCOPED_PTR_TYPE(wxEventLoop);

bool GDLApp::OnInit()
{
    return true;
}
   int GDLApp::MainLoop()
{
    wxEventLoopTiedPtr mainLoop((wxEventLoop **)&m_mainLoop, new wxEventLoop);
    m_mainLoop->SetActive(m_mainLoop);
     wxEventLoop * const loop = (wxEventLoop *)wxEventLoop::GetActive();
        while(loop->Pending()) // Unprocessed events in queue
        { 
          loop->Dispatch(); // Dispatch next event in queue
        }
     return 0;
}

int GDLApp::OnExit()
{
  std::cout << " In GDLApp::OnExit()" << std::endl;
  // Defined in guiThread::OnExit() in gdlwidget.cpp
  //  std::cout << "Exiting thread (GDLApp::OnExit): " << thread << std::endl;
  return 0;
}


#endif

