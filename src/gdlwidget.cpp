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


//#define GDL_DEBUG_WIDGETS

#define TIDY_WIDGET {this->SetSensitive(sensitive);}
#define UPDATE_WINDOW {this->RefreshWidget();}
    
const WidgetIDT GDLWidget::NullID = 0;

// instantiation
WidgetListT GDLWidget::widgetList;

GDLEventQueue GDLWidget::eventQueue; // the event queue
GDLEventQueue GDLWidget::readlineEventQueue; // for process at command line level

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
  switch(alignment){
          case wxALIGN_RIGHT:
            return wxTE_RIGHT;
            break;
          case wxALIGN_CENTER:
            return wxTE_CENTER;
            break;
          case wxALIGN_LEFT:
            return wxTE_LEFT; 
            break;
          default:
            return wxALIGN_NOT;
  }
}
inline long GDLWidget::buttonTextAlignment()
{
  switch(alignment){
          case wxALIGN_RIGHT:
            return wxBU_RIGHT;
            break;
          case wxALIGN_CENTER:
            return wxBU_EXACTFIT;
            break;
          case wxALIGN_LEFT:
            return wxBU_LEFT; 
            break;
          default:
            return wxALIGN_NOT;
  }
}
inline long GDLWidget::widgetAlignment()
{
        switch(alignment){
          case wxALIGN_RIGHT:
            return wxALIGN_RIGHT;
            break;
          case wxALIGN_CENTER:
            return wxALIGN_CENTER;
            break;
          case wxALIGN_LEFT:
            return wxALIGN_LEFT;
            break;
          default:
            return wxALIGN_NOT;
        }
}

inline wxSizer* GetBaseSizer( DLong col, DLong row, bool grid, long pad) 
{
  wxSizer* sizer = NULL;
  if ( row <= 0  && col <= 0) {sizer = new wxGridBagSizer(pad,pad); }
  else if ( row == 0  && col <= 1) {sizer = new wxBoxSizer( wxVERTICAL ); if (pad) sizer->AddSpacer(pad);}
  else if ( row == 0 && col > 1) {sizer = (grid)?new wxGridSizer( 0, col, pad, pad ):new wxFlexGridSizer( 0, col, pad, pad );}
  else if ( col == 0 && row <= 1) {sizer = new wxBoxSizer( wxHORIZONTAL );if (pad) sizer->AddSpacer(pad);}
  else if ( col == 0 && row > 1) {sizer = (grid)?new wxGridSizer( row, 0, pad, pad ):new wxFlexGridSizer( row, 0, pad, pad );}
  else sizer = new wxFlexGridSizer( row, col, pad, pad ); //which should not happen.
  return sizer;
}

inline wxSize GDLWidgetText::computeWidgetSize()
{
  //widget text size is in LINES in Y and CHARACTERS in X. But overridden by scr_xsize et if present
  wxSize fontSize = wxSystemSettings::GetFont( wxSYS_SYSTEM_FONT ).GetPixelSize();
  wxSize widgetSize = wxDefaultSize;
  if ( xSize != widgetSize.x ) widgetSize.x = (xSize+1) * fontSize.x;
  else { widgetSize.x = (maxlinelength+1) * fontSize.x;  if ( widgetSize.x < 140 ) widgetSize.x=20* fontSize.x; }
//but..
  if (scrXSize>0) widgetSize.x=scrXSize;
  
  if ( ySize != widgetSize.y )  widgetSize.y = (ySize * fontSize.y)+nlines*1; //1 pixel between lines
  else widgetSize.y = fontSize.y+1; //instead of nlines*fontSize.y to be compliant with *DL
  if (widgetSize.y < 20) widgetSize.y = 20;
//but..
   if (scrYSize>0) widgetSize.y=scrYSize;
  
  return widgetSize;
}
inline wxSize GDLWidgetList::computeWidgetSize()
{
  //widget text size is in LINES in Y and CHARACTERS in X. But overridden by scr_xsize et if present
  wxSize fontSize = wxSystemSettings::GetFont( wxSYS_SYSTEM_FONT ).GetPixelSize();
  wxSize widgetSize = wxDefaultSize;
  if ( xSize != widgetSize.x ) widgetSize.x = (xSize+1) * fontSize.x;
  else { widgetSize.x = (maxlinelength+1) * fontSize.x;  if ( widgetSize.x < 140 ) widgetSize.x=20* fontSize.x; }
//but..
  if (scrXSize>0) widgetSize.x=scrXSize;
  
  if ( ySize != widgetSize.y )  widgetSize.y = (ySize * fontSize.y)+nlines*1; //1 pixel between lines
  else widgetSize.y = fontSize.y+1; //instead of nlines*fontSize.y to be compliant with *DL
  if (widgetSize.y < 20) widgetSize.y = 20;
//but..
   if (scrYSize>0) widgetSize.y=scrYSize;
  
  return widgetSize;
}
inline wxSize GDLWidget::computeWidgetSize()
{
  wxSize widgetSize = wxDefaultSize;
  if ( xSize != widgetSize.x ) widgetSize.x = xSize*units.x;
  else { widgetSize.x = -1; }
//but..
  if (scrXSize>0) widgetSize.x=scrXSize;
  
  if ( ySize != widgetSize.y )  widgetSize.y = ySize * units.y; 
  else widgetSize.y = -1;
//but..
   if (scrYSize>0) widgetSize.y=scrYSize;
  
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
   if (me) {if (update) me->Thaw(); else me->Freeze();} else cerr<<"freezing unknown widget\n";
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
//Not useful anymore
//void GDLWidget::RefreshWidgets()
//{
//  //it is necessary to refresh only the draw widgets whose changes are not "seen" by our crude event loop. (fixme)
//  //normally we should even restrict ourselves to draw panels that have actually been changed.
////  WidgetListT::iterator it = widgetList.begin();
////  while ( it != widgetList.end( ) ) {
////    GDLWidget* widget = it->second;
////    if ( widget && widget->IsDraw( ) ) static_cast<wxWindow*>(widget->GetWxWidget())->Refresh();
////    it++;
////  }
//}

void GDLWidget::RefreshWidget( )
{
  if ( widgetPanel->IsShownOnScreen( ) ) {
    GDLWidget* gdlParent = GetWidget( parentID );
    if ( gdlParent == GDLWidget::NullID ) return;
    while ( gdlParent->IsBase() ) {
      wxSizer * s = gdlParent->GetSizer( );
      if ( s ) s->Layout( );
      gdlParent = GetWidget( gdlParent->GetParentID( ) );
      if ( gdlParent == GDLWidget::NullID ) break;
    } 
    static_cast<wxWindow*>(this->GetWxWidget())->Refresh();
  }
}

int GDLWidget::HandleEvents()
{
//make one loop for wxWidgets Events...
  wxTheApp->OnRun();
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
    
    ev = CallEventHandler( /*id,*/ ev );

    if( ev != NULL)
    {
      Warning( "Unhandled event. ID: " + i2s( id ) );
      GDLDelete( ev );
      ev = NULL;
      return 0;
    } 
  }
  wxEndBusyCursor( );
  return 0;
}

void GDLWidget::PushEvent( WidgetIDT baseWidgetID, DStructGDL* ev) {
  // Get XmanagerActiveCommand status
  GDLWidget *baseWidget = GDLWidget::GetWidget( baseWidgetID );
  if ( baseWidget != NULL ) {
    bool xmanActCom = baseWidget->GetXmanagerActiveCommand( );
    if ( !xmanActCom ) {
      //     wxMessageOutputStderr().Printf(_T("eventQueue.Push: %d\n"),baseWidgetID);
      eventQueue.Push( ev );
    } else {
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
// Init
void GDLWidget::Init()
{
  wxInitialize( );
}
// UnInit
void GDLWidget::UnInit()
{
  wxUninitialize( );
}



GDLWidget::GDLWidget( WidgetIDT p, EnvT* e, BaseGDL* vV/*=NULL*/, DULong eventFlags_/*=0*/ )
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
, groupLeader(GDLWidget::NullID)
, frame(0)
, valid(TRUE)
, updating(FALSE)
{
  if ( e != NULL ) GetCommonKeywords( e ); //defines own alignment.

  widgetID = wxWindow::NewControlId( );

  if ( parentID != GDLWidget::NullID ) {
    GDLWidget* gdlParent = GetWidget( parentID );
    assert( gdlParent != NULL);
    if ( gdlParent->IsBase( ) ) {
      GDLWidgetBase* base = static_cast<GDLWidgetBase*> (gdlParent);
      base->AddChild( widgetID );
      if (alignment == -1) alignment=base->getChildrenAlignment(); //which can be ALIGN_NOT 
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
  wxWindow *me=static_cast<wxWindow*>(this->GetWxWidget()); if (me!=NULL) {if (value) me->Enable(); else me->Disable();}
}

void GDLWidget::SetFocus() //gives focus to the CHILD of the panel.
{
//  if (this->GetWidgetName()=="DRAW") static_cast<GDLDrawPanel*>(this->wxWidget)->SetFocus(); 
//  else if (this->GetWidgetName()=="TABLE") static_cast<GDLGrid*>(this->wxWidget)->SetFocus(); 
//  else  static_cast<wxPanel*> (this->widgetPanel)->SetFocus();
  static_cast<wxWindow*>(wxWidget)->SetFocus();
}

void GDLWidget::SetSizeHints()
{
  assert( parentID == NullID );
#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputStderr( ).Printf( _T( "GDLWidget:SetSizeHints: %d\n" ), this->widgetID );
#endif
  GDLFrame *frame = static_cast<GDLFrame*> (this->wxWidget);
  topWidgetSizer->SetSizeHints( frame );
}

void GDLWidget::SetSize(DLong sizex, DLong sizey)
{
  wxWindow* me=static_cast<wxWindow*>(this->GetWxWidget());
  wxSize currentSize=me->GetSize();
  
  if (currentSize==wxSize(sizex,sizey)) return;

  GDLWidgetBase *tlb = GetTopLevelBaseWidget( this->WidgetID( ) );
  
  //note particular case of 0 for base widgets: 0 means stretch otherwise not.
  if (this->IsBase()) {
    static_cast<GDLWidgetBase*>(this)->SetStretchX((sizex<=0)); 
    static_cast<GDLWidgetBase*>(this)->SetStretchY((sizey<=0));
  }
  xSize=(sizex<=0)?currentSize.x:sizex;
  ySize=(sizey<=0)?currentSize.y:sizey;
  
  //prevent event
  updating=TRUE;
  me->SetSize(xSize,ySize);
  widgetSizer->SetItemMinSize(me,xSize,ySize);
  this->RefreshWidget();
  if (tlb->IsStretchable()) {static_cast<wxFrame*> (tlb->GetWxWidget( ))->Fit( );}
  updating=FALSE;
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
    theGDLApp->OnRun();
    frame->SetTheApp(theGDLApp);
    if (frame->IsMapped() != map)
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
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "in ~GDLWidget(): " << widgetID << std::endl;
#endif

  assert( this->IsValid( ) );
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
  } else if ( widgetType == GDLWidget::WIDGET_TREE ) { //deleted elsewhere.
#ifdef GDL_DEBUG_WIDGETS
    std::cout << "in ~GDLWidget(): not destroying WIDGET_TREE container" << widgetName << ": " << widgetID << endl;
#endif
  } else if ( parentID != GDLWidget::NullID ) { //not the TLB
    //parent is a panel or a tab
    GDLWidget* gdlParent = GetWidget( parentID );
    assert( gdlParent != NULL );
    if ( gdlParent->IsContainer( ) ) {
#ifdef GDL_DEBUG_WIDGETS
      std::cout << "in ~GDLWidget(): destroy container-parent " << widgetName << ": " << widgetID << endl;
#endif
      GDLWidgetContainer* container = static_cast<GDLWidgetContainer*> (gdlParent);
      container->RemoveChild( widgetID );
      wxWindow *me = static_cast<wxWindow*> (this->GetWxWidget( ));
      if ( me ) { if ( gdlParent->IsTab( ) ) me->Hide( );
        else me->Destroy( ); } //do not delete the page, it will be removed by the notebook deletion!!!
    } else {
#ifdef GDL_DEBUG_WIDGETS
      std::cout << "in ~GDLWidget(): destroy non container-parent " << widgetName << ": " << widgetID << endl;
#endif
      wxWindow *me = static_cast<wxWindow*> (this->GetWxWidget( ));
      if ( me ) me->Destroy( );
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

GDLWidgetContainer::GDLWidgetContainer( WidgetIDT parentID, EnvT* e, ULong eventFlags_, bool map)
: GDLWidget( parentID, e, NULL, eventFlags_)
, map(map)
{
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
DLong xpad, DLong ypad,
DLong x_scroll_size, DLong y_scroll_size, bool grid, long children_alignment, long space_, bool iscontextmenu)
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
, IsContextMenu(iscontextmenu)
{
  // All bases can receive events: EV_CONTEXT, EV_KBRD_FOCUS, EV_TRACKING

  xmanActCom = false;
  //get immediately rid of scroll sizes in case of scroll or not... Here is the logic:
  if (x_scroll_size > 0) {scrolled=TRUE;}//x_scroll_size+=(SCROLL_WIDTH+2*DEFAULT_BORDER_SIZE);} 
  if (y_scroll_size > 0) {scrolled=TRUE;}//y_scroll_size+=(SCROLL_WIDTH+2*DEFAULT_BORDER_SIZE);}
  if (scrolled && x_scroll_size <=0) x_scroll_size=-1;
  if (scrolled && y_scroll_size <=0) y_scroll_size=-1;
  
    if ( xSize <= 0 ) {stretchX=TRUE;}// xSize=100;} //provide a default value!
    if ( ySize <= 0 ) {stretchY=TRUE;}// ySize=100;} 

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
    GDLFrame *gdlFrame = new GDLFrame( this, widgetID, titleWxString , wxPoint(xOffset,yOffset));

   if (eventFlags & GDLWidget::EV_SIZE ) gdlFrame->Connect(widgetID, wxEVT_SIZE, wxSizeEventHandler(GDLFrame::OnSize));
   if (eventFlags & GDLWidget::EV_MOVE ) gdlFrame->Connect(widgetID, wxEVT_MOVE, wxMoveEventHandler(GDLFrame::OnMove));
   if (eventFlags & GDLWidget::EV_ICONIFY ) gdlFrame->Connect(widgetID, wxEVT_ICONIZE, wxIconizeEventHandler(GDLFrame::OnIconize)); 
   if (eventFlags & GDLWidget::EV_KILL ) {
     gdlFrame->Connect(widgetID, wxEVT_CLOSE_WINDOW, wxCloseEventHandler(GDLFrame::OnCloseFrame));
   } else gdlFrame->Connect(widgetID, wxEVT_CLOSE_WINDOW, wxCloseEventHandler(GDLFrame::OnUnhandledCloseFrame));
#ifdef GDL_DEBUG_WIDGETS
    gdlFrame->SetBackgroundColour(wxColour(255,0,255)); //for tests!
#endif
    
    wxWidget = gdlFrame;
    
    if( mbarID != 0)
    {
        GDLWidgetMBar* mBar = new GDLWidgetMBar( widgetID, e );
        mbarID = mBar->WidgetID( );
        mBarIDInOut = mbarID;

        gdlFrame->SetMenuBar( static_cast<wxMenuBar*> (mBar->GetWxWidget( )) );
    }

    topWidgetSizer = new wxBoxSizer( wxVERTICAL);
    gdlFrame->SetSizer( topWidgetSizer );

    //create the first panel, fix size. Offset is not taken into account.
    widgetPanel = new wxPanel( gdlFrame, widgetID , wxDefaultPosition, wxSize(xSize,ySize), frame?wxBORDER_SUNKEN:wxBORDER_NONE);
#ifdef GDL_DEBUG_WIDGETS
    widgetPanel->SetBackgroundColour(wxColour(255,0,0)); //for tests!
#endif
    //give size hints for this panel. 
    if (!stretchX && !stretchY) widgetPanel->SetSizeHints(xSize, ySize);
    else if (!stretchX) widgetPanel->SetSizeHints(xSize, -1);
    else if (!stretchY) widgetPanel->SetSizeHints(-1, ySize);

    //force frame to wrap around panel
    topWidgetSizer->Add( widgetPanel, 1, wxEXPAND);

    if (scrolled) {
      //do something clever!
    }
    //I cannot succeed to have a correct size if the
    //base sizer is gridbag and no sizes are given. Thus in the case xsize=ysize=undefined and col=0,
    //I force col=1.
    if (stretchX && stretchY && ncols==0 && nrows==0) ncols=1;
    //Allocate the sizer for children according to col or row layout
    widgetSizer = GetBaseSizer( ncols, nrows, grid , space);
    //Attach sizer to panel
    widgetPanel->SetSizer( widgetSizer );
  } 
  else if ( IsContextMenu ) 
  {
    GDLWidget* gdlParent = GetWidget( parentID );
    assert( gdlParent != NULL);
    wxWindow* parentWindow=static_cast<wxWindow*>(gdlParent->GetWxWidget());
    wxPopupTransientWindow* transient = new wxPopupTransientWindow(parentWindow );

    wxWidget = transient;
    topWidgetSizer = new wxBoxSizer( wxVERTICAL);
    transient->SetSizer( topWidgetSizer);

    widgetPanel = new wxPanel( transient,wxID_ANY, wxDefaultPosition, wxSize(500,500));
    topWidgetSizer->Add( widgetPanel  ,1, wxEXPAND);

    widgetSizer = new wxBoxSizer( wxVERTICAL);
    widgetPanel->SetSizer( widgetSizer );
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
      wxNotebook* wxParent = static_cast<wxNotebook*> (parentTab->GetWxWidget( ));
      //create the panel, fix size. Offset is not taken into account unless we create an intermediate container panel.
      widgetPanel = new wxPanel( wxParent, wxID_ANY , wxPoint(xOffset,yOffset), wxSize(xSize,ySize) , frame?wxBORDER_SUNKEN:wxBORDER_NONE);
#ifdef GDL_DEBUG_WIDGETS
      widgetPanel->SetBackgroundColour(wxColour(64,128,33)); //for tests!
#endif
      wxWidget = widgetPanel;
      widgetPanel->SetSize((xSize>0)?xSize:100, (ySize>0)?ySize:100);

      wxString titleWxString = wxString( title_.c_str( ), wxConvUTF8 );
      wxParent->AddPage( widgetPanel, titleWxString );
      if (scrolled) {
        scrollPanel = new wxScrolledWindow(wxParent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN|wxALWAYS_SHOW_SB);
#ifdef GDL_DEBUG_WIDGETS
        scrollPanel->SetBackgroundColour(wxColour(0,255,255)); //for tests!
#endif
        scrollPanel->SetScrollRate(20,20); //show scrollbars
        scrollSizer = new wxBoxSizer(wxVERTICAL );
        scrollPanel->SetSizer( scrollSizer );
        scrollPanel->SetSizeHints((x_scroll_size>0)?x_scroll_size:(xSize>0)?xSize:-1,(y_scroll_size>0)?y_scroll_size:(ySize>0)?ySize:-1);

        widgetPanel->Reparent(scrollPanel);
        scrollSizer->Add(widgetPanel);
        wxParent->AddPage(scrollPanel, titleWxString );
      }
    }
    else
    {
      wxWindow* wxParent = static_cast<wxWindow*> (gdlParent->GetWxWidget( ));
      widgetPanel = new wxPanel( wxParent, wxID_ANY , wxPoint(xOffset,yOffset), wxSize(xSize,ySize), frame?wxBORDER_SUNKEN:wxBORDER_NONE);
#ifdef GDL_DEBUG_WIDGETS
      widgetPanel->SetBackgroundColour(wxColour(255,255,0)); //for tests!
#endif
      wxWidget = widgetPanel;
      widgetPanel->SetSizeHints((xSize>0)?xSize:-1, (ySize>0)?ySize:-1);
      parentSizer->Add( widgetPanel,0,widgetAlignment());
        
      if (scrolled) {
        scrollPanel = new wxScrolledWindow(wxParent, wxID_ANY, wxPoint(xOffset,yOffset), wxSize(x_scroll_size, y_scroll_size ), wxBORDER_SUNKEN|wxALWAYS_SHOW_SB);
#ifdef GDL_DEBUG_WIDGETS
        scrollPanel->SetBackgroundColour(wxColour(0,255,255)); //for tests!
#endif
        scrollSizer = new wxBoxSizer(wxVERTICAL );
        scrollPanel->SetSizer( scrollSizer );
        scrollPanel->SetSizeHints((x_scroll_size>0)?x_scroll_size:(xSize>0)?xSize:-1,(y_scroll_size>0)?y_scroll_size:(ySize>0)?ySize:-1);

        parentSizer->Detach(widgetPanel);
        widgetPanel->Reparent(scrollPanel);
        scrollSizer->Add(widgetPanel);
        parentSizer->Add(scrollPanel, 0, widgetAlignment());
        scrollPanel->SetScrollRate(20,20); //show scrollbars
      }
    }
    //Allocate the sizer for children according to col or row layout
    //I cannot succeed to have a correct size if the
    //base sizer is gridbag and no sizes are given. Thus in the case xsize=ysize=undefined and col=0, I force col=1.
    if (stretchX && stretchY && ncols==0 && nrows==0) ncols=1;
    //Allocate the sizer for children according to col or row layout
    widgetSizer = GetBaseSizer( ncols, nrows, grid, space );
    //Attach sizer to panel
    widgetPanel->SetSizer( widgetSizer );

    TIDY_WIDGET;
    UPDATE_WINDOW;
  }
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

  if( this->parentID == GDLWidget::NullID)
  {
      // Close widget frame (might be already closed)
      if( static_cast<GDLFrame*>(this->wxWidget) != NULL)
      {
        static_cast<GDLFrame*> (this->wxWidget)->NullGDLOwner( );
        delete static_cast<GDLFrame*> (this->wxWidget); //closes the frame etc.
      }
      //IMPORTANT: unxregister TLB if was managed 
      CallEventPro( "UNXREGISTER" , new DLongGDL(widgetID)); 
  }
  // remove all widgets still in the queue for current TLB
  eventQueue.Purge( widgetID);
  readlineEventQueue.Purge( widgetID);
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
    me->Show(val);  
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
  long widgetStyle=(wxEXPAND|wxALL)|widgetAlignment();
  widgetSizer->Add( notebook, 0, widgetStyle, DEFAULT_BORDER_SIZE );
  widgetSizer->Layout();
  TIDY_WIDGET;
  UPDATE_WINDOW
}

BaseGDL* GDLWidgetTab::GetTabNumber(){
  wxNotebook * notebook=static_cast<wxNotebook*>(wxWidget);
  return new DIntGDL(notebook->GetPageCount());
}

BaseGDL* GDLWidgetTab::GetTabCurrent(){
  wxNotebook * notebook=static_cast<wxNotebook*>(wxWidget);
  return new DIntGDL(notebook->GetSelection());
}

void GDLWidgetTab::SetTabCurrent(int val){
  wxNotebook * notebook=static_cast<wxNotebook*>(wxWidget);
  if (val<notebook->GetPageCount()){
//   notebook->GetPage(val)->Raise();    
   notebook->ChangeSelection(val);
  }
}

BaseGDL* GDLWidgetTab::GetTabMultiline(){
  wxNotebook * notebook=static_cast<wxNotebook*>(wxWidget);
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
, alignment( alignment_ )
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
    grid->SetDefaultColSize((*columnWidth)[0]) ;
    hasColumnWidth=FALSE;
  }
}
//RowHeight
bool hasRowHeights=(rowHeights!=NULL);
if (hasRowHeights) { //one value set for all?
  if (rowHeights->N_Elements()==1) {
    grid->SetDefaultRowSize((*rowHeights)[0]) ;
    hasRowHeights=FALSE;
  }
}
//Alignment
bool hasAlignment=(alignment!=NULL);
if (hasAlignment) {
  if (alignment->N_Elements()==1) { //singleton case
    switch( (*alignment)[0] ){
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
// Set grid cell contents as strings. Note that there may be less or more cells than valueAsStrings, du to possibly different xSize,ySize :

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
int currentColWidth=grid->GetDefaultColSize();
int currentRowHeight=grid->GetDefaultRowSize();
int currentColLabelHeight = grid->GetColLabelSize();
int currentRowLabelWidth = grid->GetRowLabelSize();
int fullsizex=currentColWidth*numCols+currentRowLabelWidth+SCROLL_WIDTH; 
int fullsizey=currentRowHeight*numRows+currentColLabelHeight+SCROLL_WIDTH; 

int sizex=-1;
int sizey=-1;
int scr_sizex=-1;
int scr_sizey=-1;
  if ( xSize > 0 ) { //columns
    sizex=min(xSize*currentColWidth+currentRowLabelWidth+SCROLL_WIDTH,fullsizex);
  } else {sizex=fullsizex;}
  if ( ySize > 0 ) { //rows
    sizey=min(ySize*currentRowHeight+currentColLabelHeight+SCROLL_WIDTH,fullsizey);
  } else {sizey=fullsizey;}
  if ( x_scroll_size > 0 ) { //columns
    scrolled=TRUE;
    scr_sizex=min(x_scroll_size*currentColWidth+currentRowLabelWidth+SCROLL_WIDTH,fullsizex);
    if (y_scroll_size <=0) y_scroll_size=x_scroll_size;
  }
  if ( y_scroll_size > 0 ) { //rows
    scrolled=TRUE;
    scr_sizey=min(y_scroll_size*currentRowHeight+currentColLabelHeight+SCROLL_WIDTH,fullsizey);
    if (x_scroll_size <=0) {x_scroll_size=y_scroll_size;scr_sizex=min(x_scroll_size*currentColWidth+currentRowLabelWidth,fullsizex);}
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
grid->SetScrollLineX(currentColWidth);
grid->SetScrollLineY(currentRowHeight);
//grid->SetScrollbar(wxHORIZONTAL,0,xSize,grid_ncols);
//grid->SetScrollbar(wxVERTICAL,0,ySize,grid_nrows);

widgetSizer->Add(grid);
if (frame) this->FrameWidget();
TIDY_WIDGET;
UPDATE_WINDOW
}

bool GDLWidgetTable::IsSomethingSelected(){
    gdlGrid * grid = static_cast<gdlGrid*> (wxWidget);
    return grid->IsSomethingSelected();
}

DLongGDL* GDLWidgetTable::GetSelection( ) {
  gdlGrid * grid = static_cast<gdlGrid*> (wxWidget);
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
  updating=TRUE;
  grid->ClearSelection();
  updating=FALSE;
UPDATE_WINDOW
}

void GDLWidgetTable::DoAlign() {
  if (alignment->N_Elements( )==0) {return;}
  gdlGrid * grid = static_cast<gdlGrid*> (wxWidget);
  int nRows = grid->GetNumberRows( );
  int nCols = grid->GetNumberCols( );
  SizeT k = 0;
  grid->BeginBatch();
  for ( SizeT i = 0; i < nRows; ++i ) {
    for ( SizeT j = 0; j < nCols; ++j ) {
      switch ( (*alignment)[k % alignment->N_Elements( )] ) {
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
      if ( alignment->N_Elements( ) > 1 ) if ( k == alignment->N_Elements( ) ) break;
    }
    if ( alignment->N_Elements( ) > 1 ) if ( k == alignment->N_Elements( ) ) break;
  }
  grid->EndBatch();
UPDATE_WINDOW
}

void GDLWidgetTable::DoAlign(DLongGDL* selection) {
  if (alignment->N_Elements( )==0) {return;}
  gdlGrid * grid = static_cast<gdlGrid*> (wxWidget);
  SizeT k = 0;
  grid->BeginBatch();
  if (selection->Rank()==0) { //use current wxWidgets selection
   std::vector<wxPoint> list=grid->GetSelectedDisjointCellsList();
   for ( std::vector<wxPoint>::iterator it = list.begin(); it !=list.end(); ++it) {
     int ali;
      switch ( (*alignment)[k % alignment->N_Elements( )] ) {
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
        switch ( (*alignment)[k % alignment->N_Elements( )] ) {
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
        switch ( (*alignment)[k % alignment->N_Elements( )] ) {
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
  int nCols = grid->GetNumberCols( );
  grid->BeginBatch();
  if ( columnWidth->N_Elements( ) == 1 ) for ( SizeT j = 0; j < nCols; ++j ) grid->SetColSize(j,(*columnWidth)[0]); 
  else {
      for ( SizeT j = 0; j < nCols; ++j ) {
        if ( j > (columnWidth->N_Elements( ) - 1) ) break;
        grid->SetColSize(j,(*columnWidth)[j]);
      }
  }
  grid->EndBatch();
  UPDATE_WINDOW
}

void GDLWidgetTable::DoColumnWidth( DLongGDL* selection ) {
  SizeT nbCols = columnWidth->N_Elements( );
  if ( nbCols == 0 ) {return;}
  gdlGrid * grid = static_cast<gdlGrid*> (wxWidget);
  grid->BeginBatch( );

  SizeT k=0;

  if ( selection->Rank( ) == 0 ) { //use current wxWidgets selection
   wxArrayInt list=grid->GetSortedSelectedColsList();
   //find concerned cols
   for ( int it = 0; it <list.GetCount(); ++it) {
       grid->SetColSize( list[it], (*columnWidth)[it % nbCols] );
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
          grid->SetColSize( theCol, (*columnWidth)[k % nbCols] );
          k++;
        }
      }
    } else { //4 values
     int colTL = (*selection)[0];
     int colBR = (*selection)[2];
     for (int j=colTL; j<=colBR; ++j)
     {
       grid->SetColSize( j, (*columnWidth)[k % nbCols] );
       k++;
     }
    }
  }
  
  grid->EndBatch( );
  UPDATE_WINDOW
}  
DLongGDL* GDLWidgetTable::GetColumnWidth(DLongGDL* selection){
  gdlGrid * grid = static_cast<gdlGrid*> (wxWidget);
  SizeT k=0;
  int nCols = grid->GetNumberCols( );
  
  if ( selection == NULL) {
    DLongGDL* res=new DLongGDL(dimension(nCols));
    for ( SizeT j = 0; j < nCols; ++j ) (*res)[j]=grid->GetColSize(j);
    return res;
  } else if ( selection->Rank( ) == 0 ) { //use current wxWidgets selection
    wxArrayInt list=grid->GetSortedSelectedColsList();
   //find concerned cols
    if (list.GetCount()==0) return NULL;
   DLongGDL* res=new DLongGDL(dimension(list.GetCount()));
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
     DLongGDL* res=new DLongGDL(dimension(theCols.size()));
     for ( iter = theCols.begin(); iter !=theCols.end(); ++iter) {
       (*res)[k++]=grid->GetColSize( (*iter));
      }     
     return res;
    } else { //4 values
     int colTL = (*selection)[0];
     int colBR = (*selection)[2];
     int count = colBR-colTL+1;
     if (count==0) return NULL;
     DLongGDL* res=new DLongGDL(dimension(count));
     for (int j=colTL; j<=colBR; ++j)
     {
       (*res)[k++]=grid->GetColSize(j); 
     }
    }
  }
  return new DLongGDL(0); //to keep compiler happy
}
DLongGDL* GDLWidgetTable::GetRowHeight(DLongGDL* selection){
  gdlGrid * grid = static_cast<gdlGrid*> (wxWidget);
  SizeT k=0;
  int nRows = grid->GetNumberRows( );
  
  if ( selection == NULL) {
    DLongGDL* res=new DLongGDL(dimension(nRows));
    for ( SizeT i = 0; i < nRows; ++i ) (*res)[i]=grid->GetRowSize(i);
    return res;
  } else if ( selection->Rank( ) == 0 ) { //use current wxWidgets selection
    wxArrayInt list=grid->GetSortedSelectedRowsList();
   //find concerned rows
    if (list.GetCount()==0) return NULL;
   DLongGDL* res=new DLongGDL(dimension(list.GetCount()));
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
     DLongGDL* res=new DLongGDL(dimension(theRows.size()));
     for ( iter = theRows.begin(); iter !=theRows.end(); ++iter) {
       (*res)[k++]=grid->GetRowSize( (*iter));
      }     
     return res;
    } else { //4 values
     int rowTL = (*selection)[1];
     int rowBR = (*selection)[3];
     int count = rowBR-rowTL+1;
     if (count==0) return NULL;
     DLongGDL* res=new DLongGDL(dimension(count));
     for (int j=rowTL; j<=rowBR; ++j)
     {
       (*res)[k++]=grid->GetRowSize(j); 
     }
    }
  }
  return new DLongGDL(0); //to keep compiler happy
}

void GDLWidgetTable::DoRowHeights( ) {
  if (rowHeights->N_Elements( )==0) {return;}
  gdlGrid * grid = static_cast<gdlGrid*> (wxWidget);
  int nRows = grid->GetNumberRows( );
  grid->BeginBatch();
  if ( rowHeights->N_Elements( ) == 1 ) for ( SizeT i = 0; i < nRows; ++i ) grid->SetRowSize(i,(*rowHeights)[0]); 
  else {
      for ( SizeT i = 0; i < nRows; ++i ) {
        if ( i > (rowHeights->N_Elements( ) - 1) ) break;
        grid->SetRowSize(i,(*rowHeights)[i]);
      }
  }
  grid->EndBatch();
  UPDATE_WINDOW
}

void GDLWidgetTable::DoRowHeights( DLongGDL* selection ) {
  SizeT nbRows = rowHeights->N_Elements( );
  if ( nbRows == 0 ) { return; }
  gdlGrid * grid = static_cast<gdlGrid*> (wxWidget);
  grid->BeginBatch( );

  SizeT k=0;

  if ( selection->Rank( ) == 0 ) { //use current wxWidgets selection
   wxArrayInt list=grid->GetSortedSelectedRowsList();
   for ( int it = 0; it <list.GetCount(); ++it) {
       grid->SetRowSize( list[it], (*rowHeights)[it % nbRows] );
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
          grid->SetRowSize( theRow, (*rowHeights)[k % nbRows] );
          k++;
        }
      }
    } else { //4 values
     int rowTL = (*selection)[1];
     int rowBR = (*selection)[3];
     for (int i=rowTL; i<=rowBR; ++i)
     {
       grid->SetRowSize( i, (*rowHeights)[k % nbRows] );
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
      vector<wxPoint> list;
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
  grid->MakeCellVisible((*pos)[1],(*pos)[0]);
}
void GDLWidgetTable::EditCell(DLongGDL* pos)
{  
  gdlGrid * grid = static_cast<gdlGrid*> (wxWidget);
  grid->SetReadOnly((*pos)[0],(*pos)[1],FALSE);
}
void GDLWidgetTable::SetTableNumberOfColumns( DLong ncols){
  gdlGrid * grid = static_cast<gdlGrid*> (wxWidget);
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

GDLWidgetTable::~GDLWidgetTable()
{
#ifdef GDL_DEBUG_WIDGETS
    std::cout << "~GDLWidgetTable: " << this << std::endl;
#endif  
  GDLDelete( alignment );
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
    wxSize widgetSize = wxDefaultSize;
    if ( xSize == widgetSize.x ) xSize = 300; //yes, has a default value!
    if ( ySize == widgetSize.y ) ySize = 300;

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

    widgetSizer->Add( tree, 0, widgetAlignment( ) ); //, 0, wxEXPAND | wxALL); 
    if ( frame ) this->FrameWidget( );
    draggable=(dragability == 1);
    droppable=(dropability == 1);
    tree->Expand(treeItemID);
    
    TIDY_WIDGET;
    UPDATE_WINDOW
  } else {
    GDLWidgetTree* parentTree = static_cast<GDLWidgetTree*> (gdlParent);
    wxWidget = parentTree->GetWxWidget( );
    rootID =  parentTree->GetRootID();
    treeItemData=new gdlTreeItemData(widgetID);
    gdlTreeCtrl * tree = static_cast<gdlTreeCtrl*> (wxWidget);

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
      else treeItemID = tree->AppendItem( parentTree->treeItemID, wxString( (*value)[0].c_str( ), wxConvUTF8 ) ,2,3, treeItemData);
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
      cerr <<childID<<endl;
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
  tree->SetItemText(treeItemID, wxString( val.c_str( ), wxConvUTF8 ));
}

/*********************************************************/
// for WIDGET_SLIDER
/*********************************************************/
GDLWidgetSlider::GDLWidgetSlider( WidgetIDT p, EnvT* e, DULong eventFlags_
, DLong value_
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

  wxSize widgetSize = wxDefaultSize;
  if (( xSize == widgetSize.x ) && !vertical) xSize=120; //yes, has a default value!
  if (( ySize == widgetSize.y ) && vertical) ySize=120;

  long style = wxSL_HORIZONTAL;
  if ( vertical ) style = wxSL_VERTICAL;
  style |= wxSL_AUTOTICKS; 
  if ( !suppressValue ) style |= wxSL_LABELS;


 
  wxSlider * slider = new wxSlider( widgetPanel, widgetID, value, minimum, maximum,
  wxPoint( xOffset, yOffset ),
  computeWidgetSize( ),
  style );
  this->wxWidget = slider;
  slider->Connect(widgetID,wxEVT_SCROLL_THUMBRELEASE,wxScrollEventHandler(GDLFrame::OnThumbRelease));
  //dynamically select drag, saves resources! (note: there is no widget_control,/drag for sliders)
  if ( eventFlags & GDLWidget::EV_DRAG ) slider->Connect(widgetID,wxEVT_SCROLL_THUMBTRACK,wxScrollEventHandler(GDLFrame::OnThumbTrack));
   if (title.size()>0){
      wxStaticBoxSizer *sz = new wxStaticBoxSizer(wxHORIZONTAL,widgetPanel,wxString( title.c_str( ), wxConvUTF8 ));
      sz->Add(slider, 1, wxEXPAND | wxALL);
//      widgetSizer->Add(sz, 0, wxEXPAND | wxALL); 
      widgetSizer->Add(sz,0,wxEXPAND | wxALL);
    } else {
  widgetSizer->Add(slider, 0, wxEXPAND | wxALL); 
//      widgetSizer->Add( slider,0,widgetAlignment());
  if (frame) this->FrameWidget(wxEXPAND | wxALL);
    } 
  TIDY_WIDGET;
  UPDATE_WINDOW
}
void GDLWidgetSlider::ControlSetValue(DLong v){
  value=v;
  wxSlider* s=static_cast<wxSlider*>(wxWidget);
  s->SetValue(v);
}
GDLWidgetSlider::~GDLWidgetSlider(){
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetSlider(): " << widgetID << std::endl;
#endif
}

GDLWidgetButton::GDLWidgetButton( WidgetIDT p, EnvT* e,
const DString& value , bool isMenu, bool hasSeparatorAbove, wxBitmap* bitmap_, DStringGDL* buttonToolTip)
: GDLWidget( p, e )
, buttonType( UNDEFINED )
, addSeparatorAbove(hasSeparatorAbove)
, buttonBitmap(bitmap_)
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
    wxMenu* menu = new wxMenu( );
    this->wxWidget = menu;
    wxString valueWxString = wxString( value.c_str( ), wxConvUTF8 );
    menuBar->Append( static_cast<wxMenu*> (this->wxWidget), valueWxString );
    buttonType = MBAR;
  }
  else
  {
    if ( gdlParent->IsBase( ) && isMenu )
    {      
     //A menu button in a base is a button starting a popup menu
      wxString valueWxString = wxString( value.c_str( ), wxConvUTF8 );
      gdlMenuButton *button = new gdlMenuButton( widgetPanel, widgetID, valueWxString,
          wxPoint( xOffset, yOffset ) , computeWidgetSize( ), buttonTextAlignment());
      wxWidget = button;
      buttonType = MBAR;
      if (buttonToolTip) static_cast<wxWindow*>(this->wxWidget)->SetToolTip( wxString((*buttonToolTip)[0].c_str(),wxConvUTF8));
      widgetSizer->Add( static_cast<wxWindow*>(wxWidget), 0, widgetAlignment(),0);
      if (frame) this->FrameWidget();
      widgetSizer->Layout();
      TIDY_WIDGET;
      UPDATE_WINDOW
      wxWidget = button->GetPopupPanel();
      widgetType=GDLWidget::WIDGET_MBAR;
    } 
    else if( gdlParent->IsButton())
    {
      if ( dynamic_cast<wxMenu*> (gdlParent->GetWxWidget( )) == NULL ) e->Throw("Parent is of incorrect type.");

        wxMenu *menu = static_cast<wxMenu*> (gdlParent->GetWxWidget( ));
        if (isMenu) {
          if (addSeparatorAbove) menu->AppendSeparator();
          this->wxWidget = new wxMenu( );
          wxString valueWxString = wxString( value.c_str( ), wxConvUTF8 );
          menu->AppendSubMenu( static_cast<wxMenu*> (this->wxWidget), valueWxString );
          buttonType = MENU;
        }
        else //only an entry.
        {
        wxString valueWxString = wxString( value.c_str( ), wxConvUTF8 );
        wxMenuItem* menuItem = new wxMenuItem( menu, widgetID, valueWxString );
        menu->Append( menuItem );
        this->wxWidget = NULL; // should be menuItem; but is not even a wxWindow!
        buttonType = ENTRY;
        }
      }
    else 
    {
      if ( gdlParent->GetExclusiveMode() == BGNORMAL) 
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
        static_cast<GDLWidgetBase*> (gdlParent)->SetLastRadioSelection( widgetID );
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
      if (buttonToolTip) static_cast<wxWindow*>(this->wxWidget)->SetToolTip( wxString((*buttonToolTip)[0].c_str(),wxConvUTF8));
      widgetSizer->Add( static_cast<wxWindow*>(wxWidget), 0, widgetAlignment(),0);
      if (frame) this->FrameWidget();
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
}
  void GDLWidgetButton::SetButtonWidgetLabelText( const DString& value_ )
  {
    //update vValue
    delete(vValue);
    vValue = new DStringGDL( value_ );
    
    if ( this->wxWidget != NULL ) {
          (static_cast<wxButton*> (wxWidget))->SetLabel( wxString( value_.c_str( ), wxConvUTF8 ) ); //SetLabel, unless SetLabelText, interprets markup (<b></b> etc)
           static_cast<wxButton*> (wxWidget)->Refresh();  //not useful
      } else std::cerr << "Null widget in GDLWidgetButton::SetButtonWidgetLabelText(), please report!" << std::endl;
  }
  
  void GDLWidgetButton::SetButtonWidgetBitmap( wxBitmap* bitmap_ )
  {
    if (buttonType!=BITMAP) return;
    if ( this->wxWidget != NULL ) {
          (static_cast<wxBitmapButton*> (wxWidget))->SetBitmapLabel(*bitmap_);
           static_cast<wxBitmapButton*> (wxWidget)->Refresh();  //not useful?
      } else std::cerr << "Null widget in GDLWidgetButton::SetButtonWidgetBitmap(), please report!" << std::endl;
  }
  
GDLWidgetList::GDLWidgetList( WidgetIDT p, EnvT* e, BaseGDL *value, DLong style, DULong eventflags )
    : GDLWidget( p, e, value, eventflags), maxlinelength(0), nlines(0)
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
//  if (ySize<2) {
//    ySize=(wxSystemSettings::GetFont( wxSYS_SYSTEM_FONT ).GetPixelSize()).y*2; //one line as in *DL -- annoying with the scrollbar, should step by line!
//  }
  
  wxListBox * list = new wxListBox( gdlParent->GetPanel( ), widgetID, wxPoint( xOffset, yOffset ),
  computeWidgetSize( ),
  choices, style|wxLB_NEEDED_SB );
  this->wxWidget = list;

  list->Connect(widgetID,wxEVT_COMMAND_LISTBOX_DOUBLECLICKED,wxCommandEventHandler(GDLFrame::OnListBoxDoubleClicked));
  list->Connect(widgetID,wxEVT_COMMAND_LISTBOX_SELECTED,wxCommandEventHandler(GDLFrame::OnListBox));

  if (frame) this->FrameWidget();  else  widgetSizer->Add(list, 0, wxEXPAND | wxALL); //IDL behaviour
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
  list->Clear();
  list->InsertItems(newchoices,0);
  list->SetSelection(0);
}

void GDLWidgetList::SelectEntry(DLong entry_number){
 wxListBox * list=static_cast<wxListBox*>(wxWidget);
 list->Select(entry_number); 
}

BaseGDL* GDLWidgetList::GetSelectedEntries(){
 wxListBox * list=static_cast<wxListBox*>(wxWidget);
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

GDLWidgetList::~GDLWidgetList(){
#ifdef GDL_DEBUG_WIDGETS
    std::cout << "~GDLWidgetList: " << this << std::endl;
#endif  
}

GDLWidgetDropList::GDLWidgetDropList( WidgetIDT p, EnvT* e, BaseGDL *value,
const DString& title_, DLong style_ )
: GDLWidget( p, e, static_cast<DStringGDL*> (value->Convert2( GDL_STRING, BaseGDL::CONVERT )) )
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

#ifdef TEST_ANOTHER_FASHION
  if (title.size()>0) { //provide room for a title at left, with a supplementary panel enclosing all.
    wxPanel *panel = new wxPanel( widgetPanel, wxID_ANY
    , wxDefaultPosition
    , wxDefaultSize
    , (frame)?wxBORDER_SUNKEN:wxBORDER_NONE 
    );
    wxSizer * box = new wxBoxSizer(wxHORIZONTAL);
    panel->SetSizer(box);
    wxStaticText *text=new wxStaticText(panel,wxID_ANY,wxString( title.c_str( ), wxConvUTF8 ));
    wxChoice * droplist = new wxChoice( panel, widgetID, wxPoint( xOffset, yOffset ), computeWidgetSize( ), choices, style );
    droplist->SetSelection(0);
    this->wxWidget = droplist;
    box->Add(text);
    box->Add( droplist);
    box->Layout();
    widgetSizer->Add(panel);//,0,widgetAlignment());
 }else{
#endif    
   wxChoice * droplist = new wxChoice( widgetPanel, widgetID, 
   wxPoint( xOffset, yOffset ), computeWidgetSize( ), choices, style );
   droplist->SetSelection(0);
   this->wxWidget = droplist;
   if (title.size()>0){
      wxStaticBoxSizer *sz = new wxStaticBoxSizer(wxHORIZONTAL,widgetPanel,wxString( title.c_str( ), wxConvUTF8 ));
      sz->Add(droplist);
      widgetSizer->Add(sz,0,widgetAlignment());
    } else {
      widgetSizer->Add( droplist,0,widgetAlignment());
      if (frame) this->FrameWidget();
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
  droplist->Clear();
  droplist->Append(newchoices);
  droplist->SetSelection(0);
}

void GDLWidgetDropList::SelectEntry(DLong entry_number){
 wxChoice * droplist=static_cast<wxChoice*>(wxWidget);
 droplist->Select(entry_number); 
}

BaseGDL* GDLWidgetDropList::GetSelectedEntry(){
  wxChoice * droplist=static_cast<wxChoice*>(wxWidget);
  return new DIntGDL(droplist->GetSelection());
}

GDLWidgetDropList::~GDLWidgetDropList(){
#ifdef GDL_DEBUG_WIDGETS
    std::cout << "~GDLWidgetDroplist: " << this << std::endl;
#endif  
}
  
GDLWidgetComboBox::GDLWidgetComboBox( WidgetIDT p, EnvT* e, BaseGDL *value,
const DString& title_, DLong style_ )
: GDLWidget( p, e, static_cast<DStringGDL*> (value->Convert2( GDL_STRING, BaseGDL::CONVERT )) )
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
  long widgetStyle=(wxEXPAND|wxALL)|widgetAlignment();
  widgetSizer->Add(combo, 0,widgetStyle);
  if (frame) this->FrameWidget(widgetStyle);
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
  combo->Clear();
  combo->Append(newchoices);
  combo->SetSelection(0);
}

void GDLWidgetComboBox::AddItem(DString value, DLong pos) {
  wxComboBox * combo = static_cast<wxComboBox*>(wxWidget);
  int nvalues=combo->GetCount();
  if (pos == -1) combo->Append( wxString( value.c_str( ), wxConvUTF8 ) );
  else if (pos > -1  && pos < nvalues) combo->Insert( wxString( value.c_str( ), wxConvUTF8 ), pos);
}

void GDLWidgetComboBox::DeleteItem(DLong pos) {
  wxComboBox * combo = static_cast<wxComboBox*>(wxWidget);
  int nvalues=combo->GetCount();
  int selected=combo->GetSelection();
  if (pos > -1  && pos < nvalues) combo->Delete( pos );
  if (pos==selected && (nvalues-1) > 0 ) combo->Select((pos+1)%(nvalues-1)); else if (selected==-1 && pos==0) combo->Select(0);
}

void GDLWidgetComboBox::SelectEntry(DLong entry_number){
 wxComboBox * combo=static_cast<wxComboBox*>(wxWidget);
 combo->Select(entry_number); 
}

BaseGDL* GDLWidgetComboBox::GetSelectedEntry(){
  wxComboBox * combo=static_cast<wxComboBox*>(wxWidget);
  DStringGDL* stringres=new DStringGDL(dimension(1));
  (*stringres)[0]=combo->GetStringSelection().mb_str(wxConvUTF8); //return null string if no selection
  return stringres;
}

GDLWidgetComboBox::~GDLWidgetComboBox(){
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetComboBox(): " << widgetID << std::endl;
#endif
}

GDLWidgetText::GDLWidgetText( WidgetIDT p, EnvT* e, DStringGDL* valueStr, bool noNewLine_,
bool editable_ )
: GDLWidget( p, e, valueStr )
, noNewLine( noNewLine_ )
, editable(editable_)
{
  //Alignment is align_left for text, whatever align option has been used. Normally /ALIGN_xxx is forbidden for WIDGET_TEXT,
  // but it is used in SetCommonKeywords. We save the day here.
  alignment=wxALIGN_LEFT;
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
        if ( !noNewLine && (i + 1) < vValue->N_Elements( ) )
        {value += '\n'; nlines++;}
      }
  }
  lastValue = value;

  GDLWidget* gdlParent = GetWidget( parentID );
  widgetPanel = gdlParent->GetPanel( );
  widgetSizer = gdlParent->GetSizer( );
  topWidgetSizer = this->GetTopLevelBaseWidget(parentID)->GetSizer();
  
  wxString valueWxString = wxString( lastValue.c_str( ), wxConvUTF8 );
  long style = wxTE_NOHIDESEL|wxTE_PROCESS_ENTER|textAlignment();
  if ( nlines > 1 || scrolled ) style |= wxTE_MULTILINE;
  
  wxTextCtrl * text;
  if ( !editable ) {
   style |= wxTE_READONLY; //no, because *DL provides READONLY and still give events.
   text = new gdlTextCtrl( widgetPanel, widgetID, valueWxString,  wxPoint( xOffset, yOffset ), computeWidgetSize(), style ); // this permits to a procedure (cw_field.pro for example) to filter the editing of text fields.
// so wxTextCtrl::OnChar is overwritten in gdlwidgeteventhandler.cpp just for this reason.
 } else {
   text = new wxTextCtrl( widgetPanel, widgetID, valueWxString, wxPoint( xOffset, yOffset ), computeWidgetSize(), style );
 }
  text->SetInsertionPoint(0);
  text->SetSelection(0,0);
  this->wxWidget = text;
  
  long widgetStyle=(wxEXPAND|wxALL)|widgetAlignment();
  widgetSizer->Add(text, 0,widgetStyle);
  if (frame) this->FrameWidget(widgetStyle); 
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
  nlines=0;
    for( int i=0; i<valueStr->N_Elements(); ++i)
    {
    value += (*valueStr)[ i];
    if ( !noNewLine && (i + 1) < valueStr->N_Elements( ) )
    {value += '\n'; nlines++;}
    }
  lastValue = value;

  wxString valueWxString = wxString( value.c_str( ), wxConvUTF8 );
  if ( this->wxWidget != NULL ) {
    wxTextCtrl* txt=static_cast<wxTextCtrl*> (wxWidget);
    txt->ChangeValue( valueWxString ); //by contrast with SetValue, does not generate an EVENT -- IDL does not either.    
    txt->Refresh();
    txt->Update();
  }  else std::cerr << "Null widget in GDLWidgetText::SetTextValue(), please report!" << std::endl;
}

void GDLWidgetText::InsertText( DStringGDL* valueStr, bool noNewLine, bool insertAtEnd)
{
  long from,to;
  wxTextCtrl* txt=static_cast<wxTextCtrl*> (wxWidget);
  txt->GetSelection(&from,&to);
  if (insertAtEnd) {from=txt->GetLastPosition(); to=from;}
  DString value = "";
  nlines=0;
  for ( int i = 0; i < valueStr->N_Elements( ); ++i ) {
    value += (*valueStr)[ i];
    if ( !noNewLine && (i + 1) < valueStr->N_Elements( ) ) {
      value += '\n'; nlines++;
    }
  }
  lastValue.replace(from,to-from,value);
  delete vValue;
  vValue = new DStringGDL(lastValue);
//  cout<<from<<"->"<<to<<":"<<lastValue.c_str( )<<endl;
  wxString valueWxString = wxString( lastValue.c_str( ), wxConvUTF8 );
  if ( this->wxWidget != NULL ) {
    txt->ChangeValue( valueWxString ); //by contrast with SetValue, does not generate an EVENT (neither does *DL).    
    txt->Refresh();
    txt->Update();
    txt->SetSelection(from,from);
  }  else std::cerr << "Null widget in GDLWidgetText::SetTextValue(), please report!" << std::endl;
}

void GDLWidgetText::SetTextSelection(DLongGDL* pos)
{ //*DL selection are [start,length] [3,20] -> pos (3,23)
  //numerous cases to handle with invalid values (negative, too large).
  //Hope it works for all cases:
  long from,to,len;
  long oldSelStart, oldSelEnd;
  static_cast<wxTextCtrl*>(wxWidget)->GetSelection(&oldSelStart,&oldSelEnd);
  len = static_cast<wxTextCtrl*>(wxWidget)->GetLastPosition()+1;

  if ((*pos)[0] < 0 ) from=0;
  else if ((*pos)[0] > len ) from=len;
  else from=(*pos)[0];

  
  if (pos->N_Elements()<2) {
    //selection = insertion
    static_cast<wxTextCtrl*>(wxWidget)->SetSelection(from,from);
    return;
  }
  //2 elements, define selection
  to=(*pos)[1];

  if ( from+to >= len ) {
    //set insertion position at end of last selection (?)
    static_cast<wxTextCtrl*>(wxWidget)->SetSelection(oldSelEnd,oldSelEnd);
    return;
  }
  if ( to < 0 ) {
    //set insertion position at from
    static_cast<wxTextCtrl*>(wxWidget)->SetSelection(from,from);
    return;
  }
  static_cast<wxTextCtrl*>(wxWidget)->SetSelection(from,from+to);
}

DLongGDL* GDLWidgetText::GetTextSelection()
{
  DLongGDL* pos=new DLongGDL(dimension(2),BaseGDL::ZERO);
  long from,to;
  static_cast<wxTextCtrl*>(wxWidget)->GetSelection(&from,&to);
  (*pos)[0]=from; (*pos)[1]=to-from;
  return pos;
}

DStringGDL* GDLWidgetText::GetSelectedText()
{
  wxTextCtrl* txt=static_cast<wxTextCtrl*>(wxWidget);
  return new DStringGDL(txt->GetStringSelection().mb_str(wxConvUTF8).data());
}

GDLWidgetLabel::GDLWidgetLabel( WidgetIDT p, EnvT* e, const DString& value_ , bool sunken)
: GDLWidget( p, e )
, value(value_)
{
  GDLWidget* gdlParent = GetWidget( parentID );
  widgetPanel = gdlParent->GetPanel( );
  widgetSizer = gdlParent->GetSizer( );
  topWidgetSizer = this->GetTopLevelBaseWidget(parentID)->GetSizer();

  wxString valueWxString = wxString( value.c_str( ), wxConvUTF8 );
  wxStaticText* label = new wxStaticText( widgetPanel, widgetID, valueWxString,
  wxPoint( xOffset, yOffset ), computeWidgetSize(), textAlignment()|wxST_NO_AUTORESIZE );
  this->wxWidget = label;
  widgetStyle=widgetAlignment();
  if (sunken) widgetStyle|=wxBORDER_SUNKEN;
  widgetSizer->Add(label,0,widgetStyle);
  if (frame||sunken) this->FrameWidget(widgetStyle);     
  TIDY_WIDGET;
  UPDATE_WINDOW
}

GDLWidgetLabel::~GDLWidgetLabel(){
#ifdef GDL_DEBUG_WIDGETS
    std::cout << "~GDLWidgetLabel: " << this << std::endl;
#endif  
}

void GDLWidget::FrameWidget(long style)
{
  if (this->IsBase()) return; //function invalid with base widgets.
  if (frameSizer==NULL) { //protect against potential problems
    //panel style:
    long panelStyle=wxBORDER_NONE;
    panelStyle=widgetStyle&wxBORDER_MASK;
    framePanel = new wxPanel(this->widgetPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, panelStyle);
    if (!panelStyle) { wxStaticBox *box = new wxStaticBox( framePanel, wxID_ANY, wxEmptyString);
    frameSizer = new wxStaticBoxSizer( box, wxVERTICAL );} else frameSizer = new wxBoxSizer(wxVERTICAL );
    framePanel->SetSizer( frameSizer );
    if (scrollSizer==NULL) {
      widgetSizer->Detach(static_cast<wxWindow*>(this->wxWidget));
      static_cast<wxWindow*>(this->wxWidget)->Reparent(framePanel);
      frameSizer->Add(static_cast<wxWindow*>(this->wxWidget), 0,widgetStyle);
    } else { 
      widgetSizer->Detach(scrollPanel);
      static_cast<wxWindow*>(this->scrollPanel)->Reparent(framePanel);
      frameSizer->Add(static_cast<wxWindow*>(this->scrollPanel), 0,widgetStyle);
    }
    widgetSizer->Add(framePanel,0,widgetAlignment());
    frameSizer->Layout();
    widgetSizer->Layout();
  }
}

void GDLWidget::UnFrameWidget()
{
  if (this->IsBase()) return; //function invalid with base widgets.
  if (frameSizer!=NULL) { //protect against potential problems
    if (scrollSizer==NULL) {
      frameSizer->Detach(static_cast<wxWindow*>(this->wxWidget));
      static_cast<wxWindow*>(this->wxWidget)->Reparent(widgetPanel);
      widgetSizer->Add(static_cast<wxWindow*>(this->wxWidget), 0,widgetAlignment());
    } else {
      frameSizer->Detach(static_cast<wxWindow*>(this->scrollPanel));
      static_cast<wxWindow*>(this->scrollPanel)->Reparent(widgetPanel);
      widgetSizer->Add(static_cast<wxWindow*>(this->scrollPanel), 0,widgetAlignment());
    }
    widgetSizer->Detach(framePanel);
    delete framePanel;
    frameSizer=NULL;
    framePanel=NULL;
    widgetSizer->Layout();
  }
}

void GDLWidget::ScrollWidget(  DLong x_scroll_size,  DLong y_scroll_size)
{
  if (this->IsBase()) return; //function invalid with base widgets.
  if (scrollSizer==NULL) { //protect against potential problems
    scrollPanel = new wxScrolledWindow(this->widgetPanel, wxID_ANY, wxPoint(xOffset,yOffset), wxSize(x_scroll_size, y_scroll_size ), wxBORDER_SUNKEN);
    scrollPanel->SetScrollRate(20,20); //show scrollbars
    scrollSizer = new wxBoxSizer(wxVERTICAL );
    scrollPanel->SetSizer( scrollSizer );

    static_cast<wxWindow*>(this->wxWidget)->Reparent(scrollPanel);
    scrollSizer->Add(static_cast<wxWindow*>(this->wxWidget));
    if (frameSizer!=NULL) {
      frameSizer->Detach(static_cast<wxWindow*>(this->wxWidget));
      frameSizer->Add(scrollPanel,0, wxFIXED_MINSIZE|wxALL, DEFAULT_BORDER_SIZE);
    } else {
      widgetSizer->Detach(static_cast<wxWindow*>(this->wxWidget));
      widgetSizer->Add(scrollPanel, 0, wxFIXED_MINSIZE|widgetStyle);
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
     frameSizer->Add(static_cast<wxWindow*>(this->wxWidget), 0, widgetStyle);
     frameSizer->Layout();
    } else {
     static_cast<wxWindow*>(this->wxWidget)->Reparent(widgetPanel);
     widgetSizer->Detach(scrollPanel);
     widgetSizer->Add(static_cast<wxWindow*>(this->wxWidget), 0, widgetStyle);
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
      static_cast<wxStaticText*> (wxWidget)->SetLabel( valueWxString ); //SetLabel, unless SetLabelText, interprets markup (<b></b> etc)
      static_cast<wxStaticText*> (wxWidget)->Refresh( ); //not useful
  }    else std::cerr << "Null widget in GDLWidgetLabel::SetLabelValue(), please report!" << std::endl;
}
//propertysheet
#ifdef HAVE_WXWIDGETS_PROPERTYGRID

 GDLWidgetPropertySheet::GDLWidgetPropertySheet( WidgetIDT parentID, EnvT* e)
 : GDLWidget( p, e )
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

 GDLWidgetPropertySheet::~GDLWidgetPropertySheet(){
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetPropertySheet(): " << widgetID << std::endl;
#endif

#endif
// GDL widgets =====================================================
// GDLFrame ========================================================
GDLFrame::GDLFrame( GDLWidgetBase* gdlOwner_, wxWindowID id, const wxString& title , const wxPoint& pos )
: wxFrame( NULL, id, title, pos, wxDefaultSize, wxDEFAULT_FRAME_STYLE )
, mapped( false )
, frameSize(wxDefaultSize)
, gdlOwner( gdlOwner_)
, appOwner(NULL)
{
  m_resizeTimer = new wxTimer(this,RESIZE_TIMER);
  m_windowTimer = new wxTimer(this,WINDOW_TIMER);
}

GDLFrame::~GDLFrame()
{ 
    if (m_resizeTimer->IsRunning()) m_resizeTimer->Stop(); //really necessary, try stopping xdice.pro when rolling dices..
    if (m_windowTimer->IsRunning()) m_windowTimer->Stop();
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
{
//  m_resizeTimer = new wxTimer(this,RESIZE_TIMER);
  // initialization of stream is done in GDLWidgetDraw::OnRealize()
}

void GDLDrawPanel::InitStream()
{
  pstreamIx = GraphicsDevice::GetGUIDevice( )->WAddFree( );
  if ( pstreamIx == -1 )
  throw GDLException( "Failed to allocate GUI stream." );

//  drawSize = this->GetSize( ); //do not set drawsize here, needs to be initialized only in size event handler.
  bool success = GraphicsDevice::GetGUIDevice( )->GUIOpen( pstreamIx, this->GetSize( ).x, this->GetSize( ).y );
  if( !success)
  {
    throw GDLException( "Failed to open GUI stream: " + i2s( pstreamIx ) );
  }
  pstreamP = static_cast<GDLWXStream*> (GraphicsDevice::GetGUIDevice( )->GetStreamAt( pstreamIx ));
  pstreamP->SetGDLDrawPanel( this );

  m_dc = pstreamP->GetDC( );
}

GDLDrawPanel::~GDLDrawPanel()
{  
#ifdef GDL_DEBUG_WIDGETS
     std::cout << "~GDLDrawPanel: " << this << std::endl;
//     std::cout << "This IsMainThread: " << wxIsMainThread() << std::endl;
#endif
//  if (m_resizeTimer->IsRunning()) m_resizeTimer->Stop(); 
  if ( pstreamP != NULL )
  pstreamP->SetValid( false );
}

//why overcast inherited ~GDLWidget????
GDLWidgetDraw::~GDLWidgetDraw()
{
#ifdef GDL_DEBUG_WIDGETS
    std::cout << "~GDLWidgetDraw: " << this << std::endl;
#endif  
  // handled in GDLDrawPanel (which is deleted by wxWidgets)
//   GraphicsDevice::GetGUIDevice()->WDelete( pstreamIx);
}


GDLWidgetDraw::GDLWidgetDraw( WidgetIDT p, EnvT* e,
			      DLong x_scroll_size_, DLong y_scroll_size_, bool app_scroll, DULong eventFlags_, DStringGDL* drawToolTip)
  : GDLWidget( p, e, NULL, eventFlags_)
  , pstreamIx(-1)
  , x_scroll_size(x_scroll_size_)
  , y_scroll_size(y_scroll_size_)
{
  //  std::cout << "In GDLWidgetDraw::GDLWidgetDraw: " << widgetID << std::endl
  assert( parentID != GDLWidget::NullID);

  //get immediately rid of scroll sizes in case of scroll or not... Here is the logic:
  if (app_scroll) scrolled=TRUE;
  if (x_scroll_size > 0) {scrolled=TRUE;x_scroll_size+=(SCROLL_WIDTH+2*DEFAULT_BORDER_SIZE);} 
  if (y_scroll_size > 0) {scrolled=TRUE;y_scroll_size+=(SCROLL_WIDTH+2*DEFAULT_BORDER_SIZE);}
  if (scrolled) x_scroll_size=(x_scroll_size<100)?100:x_scroll_size;
  if (scrolled) y_scroll_size=(y_scroll_size<100)?100:y_scroll_size;

  wxSize widgetSize = wxDefaultSize;
  if ( xSize == widgetSize.x ) xSize=scrolled?120:100; //provide a default value!
  if ( ySize == widgetSize.y ) ySize=scrolled?120:100; 

  wxWindow *wxParent = NULL;

  // If parent base widget exists ....
  GDLWidget* gdlParent = GetWidget( parentID);
  widgetPanel = gdlParent->GetPanel( );
  widgetSizer = gdlParent->GetSizer( );
  topWidgetSizer = this->GetTopLevelBaseWidget(parentID)->GetSizer();

  wxParent = static_cast< wxWindow*>( gdlParent->GetWxWidget());
  //    std::cout << "Getting Parent: " << parent << " " << gdlParent << " " 
  //      << wxParent << std::endl;

  long style = 0;
  if( frame == 1)
    style = wxBORDER_SIMPLE;
  else if( frame > 1)
    style = wxBORDER_DOUBLE;
  
  GDLDrawPanel* draw = new GDLDrawPanel( widgetPanel, widgetID, wxPoint(xOffset,yOffset), wxSize(xSize,ySize), style);
  
  draw->SetCursor(wxCURSOR_CROSS);
  if (drawToolTip) static_cast<wxWindow*>(draw)->SetToolTip( wxString((*drawToolTip)[0].c_str(),wxConvUTF8));
  wxWidget = draw;
  widgetSizer->Add( draw, 0, wxALL, DEFAULT_BORDER_SIZE);
  if (frame) this->FrameWidget();
  if (scrolled) this->ScrollWidget(x_scroll_size, y_scroll_size );

  static_cast<GDLDrawPanel*>(wxWidget)->InitStream();
  
  pstreamIx = static_cast<GDLDrawPanel*>(wxWidget)->PStreamIx();
  GDLDelete( vValue);
  this->vValue = new DLongGDL(pstreamIx);  
  this->SetSensitive(sensitive);
//here UPDATE_WINDOW is useful.  
  gdlParent->GetSizer()->Layout();
  if(widgetPanel->IsShownOnScreen()) 
  {
    GDLWidgetBase *tlb=GetTopLevelBaseWidget(this->WidgetID());
//    tlb->GetSizer()->Layout();
    static_cast<wxFrame*>(tlb->GetWxWidget())->Show();
  }
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

