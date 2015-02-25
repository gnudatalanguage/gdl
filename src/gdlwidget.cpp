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


#define UPDATE_WINDOW \
  GetWidget( parentID )->GetSizer()->Layout(); //\
//  if(widgetPanel->IsShownOnScreen()) \ //not useful now (loop force TLB refresh)
//  {\
//    GDLWidgetBase *tlb=GetTopLevelBaseWidget(this->WidgetID()); \
//    tlb->GetSizer()->Layout(); \
//    static_cast<wxFrame*>(tlb->GetWxWidget())->Show(); \
//  }   // or : static_cast<wxFrame*>(tlb->GetWxWidget())->Fit();
    
const WidgetIDT GDLWidget::NullID = 0;

// instantiation
WidgetListT GDLWidget::widgetList;

GDLEventQueue GDLWidget::eventQueue; // the event queue
GDLEventQueue GDLWidget::readlineEventQueue; // for process at command line level

void GDLEventQueue::Purge()
{
  wxMutexLocker lock( mutex );
  for ( SizeT i = 0; i < dq.size( ); ++i )
    delete dq[i];
  dq.clear( );
  //   isEmpty = true;
}

// removes all events for TLB 'topID'
void GDLEventQueue::Purge( WidgetIDT topID)
{
  wxMutexLocker lock( mutex );
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

inline wxSizer* GetBaseSizer( DLong col, DLong row, bool grid) 
{
  wxSizer* sizer = NULL;
  if ( row <= 0  && col <= 0) {sizer = new wxGridBagSizer(0,0); }
  else if ( row == 0  && col <= 1) {sizer = new wxBoxSizer( wxVERTICAL ); }
  else if ( row == 0 && col > 1) {sizer = (grid)?new wxGridSizer( 0, col, 0, 0 ):new wxFlexGridSizer( 0, col, 0, 0 );}
  else if ( col == 0 && row <= 1) {sizer = new wxBoxSizer( wxHORIZONTAL );}
  else if ( col == 0 && row > 1) {sizer = (grid)?new wxGridSizer( row, 0, 0, 0 ):new wxFlexGridSizer( row, 0, 0, 0 );}
  else sizer = new wxFlexGridSizer( row, col, 0, 0 ); //which should not happen.
  return sizer;
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
      return GDLWidget::NullID;
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

void GDLWidget::RefreshWidgets()
{
  //it is necessary to refresh only the draw widgets whose changes are not "seen" by our crude event loop. (fixme)
  //normally we should even restrict ourselves to draw panels that have actually been changed.
  WidgetListT::iterator it = widgetList.begin();
  while ( it != widgetList.end( ) ) {
    GDLWidget* widget = it->second;
//    if ( widget->IsBase( ) && widget->parentID==0 ) {static_cast<wxWindow*>(widget->GetWxWidget())->Refresh();
////      wxEvent* event=new wxSizeEvent(wxSize(800,800));
////      event->SetEventObject( widget->GetWxWidget() );
////      static_cast<wxPanel*>(widget->GetWxWidget())->AddPendingEvent(*event);
//    }
    if ( widget && widget->IsDraw( ) ) static_cast<wxWindow*>(widget->GetWxWidget())->Refresh();
//    if ( widget && widget->changed ) static_cast<wxWindow*>(widget->GetWxWidget())->Refresh();
    it++;
  }
}

int GDLWidget::HandleEvents()
{
//make one loop for wxWidgets Events...
//    if (theGDLApp != NULL) {theGDLApp->OnRun();}
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
    WidgetIDT tlb = (*static_cast<DLongGDL*> (ev->GetTag( topIx, 0 )))[0];

    ev = CallEventHandler( /*id,*/ ev );

    if( ev != NULL)
    {
      Warning( "Unhandled event. ID: " + i2s( id ) );
      GDLDelete( ev );
      ev = NULL;
      return 0;
    } 
  //refresh the wxWigdet window after the event...    
  }
  wxEndBusyCursor( );
  RefreshWidgets();
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
    //We really need to cure this problem:wigets can be destroyed and replaced by others 'live'.
  } else cerr << "NULL baseWidget (possibly Destroyed?) found in GDLWidget::PushEvent( WidgetIDT vaseWidgetID=" << baseWidgetID << ", DStructGDL* ev=" << ev << "), please report!\n";
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
  // Called by InitObjects() in object.cpp
  // std::cout << " In GDLWidget::Init()" << std::endl;
  // widgetIx = wxID_HIGHEST; // use same wx ID and GDL ID 
  wxInitialize( );
  //   wxMutexGuiLeave();
}



GDLWidget::GDLWidget( WidgetIDT p, EnvT* e, bool map_/*=true*/, BaseGDL* vV/*=NULL*/, DULong eventFlags_/*=0*/ )
: wxWidget( NULL )
, parentID( p )
, uValue( NULL )
, vValue( vV )
, exclusiveMode( 0 )
, topWidgetSizer( NULL )
, widgetSizer( NULL )
, widgetPanel( NULL )
, scrollSizer( NULL )
, scrollPanel( NULL )
, frameSizer( NULL )
, framePanel( NULL )
, managed( false )
, map( map_ )
, eventFlags( eventFlags_ ) {
  if ( e != NULL ) SetCommonKeywords( e ); //defines own alignment.

  widgetID = wxWindow::NewControlId( );

  if ( parentID != GDLWidget::NullID ) {
    GDLWidget* gdlParent = GetWidget( parentID );
    if ( gdlParent->IsBase( ) ) {
      GDLWidgetBase* base = static_cast<GDLWidgetBase*> (gdlParent);
      //       assert( base != NULL); // should be already checked elsewhere
      base->AddChild( widgetID );
      if (alignment == -1) alignment=base->getChildrenAlignment(); //which can be ALIGN_NOT 
    } else {
      GDLWidgetBase* base = GetBaseWidget( parentID );
      if ( base != NULL )
        base->AddChild( widgetID );
    }
  }
  
  widgetList.insert( widgetList.end( ), std::pair<WidgetIDT, GDLWidget*>(widgetID, this) );
#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputStderr( ).Printf( _T( "inserted: ID: %d  parentID: %d\n" ), widgetID, parentID );
#endif
}


//void GDLWidget::CreateWidgetPanel(int borderWidth, wxBorder bb )
//{
//  GDLWidget* gdlParent = GetWidget( parentID );
//
//  wxPanel *parentPanel = gdlParent->GetPanel( );
//
//  wxPanel *panel = new wxPanel( parentPanel, wxID_ANY
//  , wxDefaultPosition
//  , wxDefaultSize
//  , bb  //   , wxBORDER_SIMPLE 
////  , wxBORDER_SUNKEN //for tests, shows underlying panel
//  );
//  widgetPanel = panel;
//  widgetSizer = gdlParent->GetSizer( );
//  topWidgetSizer = this->GetTopLevelBaseWidget(parentID)->widgetSizer;
//  widgetSizer->Add( panel);//, 0, wxEXPAND | wxALL, borderWidth);
//}

void GDLWidget::SetSensitive(bool value)
{
  wxWindow *me=static_cast<wxWindow*>(this->GetWxWidget()); if (me!=NULL) {if (value) me->Enable(); else me->Disable();}
}

void GDLWidget::SetFocus() //gives focus to the CHILD of the panel.
{
  if (this->GetWidgetType()=="DRAW") static_cast<wxPanel*>(this->wxWidget)->SetFocus(); 
  else  static_cast<wxPanel*> (this->widgetPanel)->SetFocus();
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
    static_cast<GDLWidgetBase*>(this)->setStretchX((sizex==0)); 
    static_cast<GDLWidgetBase*>(this)->setStretchY((sizey==0));
  }
  xSize=(sizex==0)?currentSize.x:sizex;
  ySize=(sizey==0)?currentSize.y:sizey;
  me->SetSize(xSize,ySize);
  widgetSizer->SetItemMinSize(me,xSize,ySize);
  //me->Refresh(); 
  //  me->Update();

//The following is not IDL behaviour (automatically adjusting frame size) 
//  tlb->GetSizer( )->Layout( );
  if (tlb->IsStretchable()) {static_cast<wxFrame*> (tlb->GetWxWidget( ))->Fit( );}
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
    if( frame->LastShowRequest() != map)
    {
      this->OnRealize( );
      frame->SendShowRequestEvent( map );
//      GDLWidgetBase *tlb = GetTopLevelBaseWidget( this->WidgetID( ) );
//      tlb->GetSizer( )->Layout( );
//      static_cast<wxFrame*> (tlb->GetWxWidget( ))->Fit( );
    }
    GDLApp * theGDLApp;
    frame->SetTheApp(theGDLApp);
    theGDLApp=new GDLApp;
    theGDLApp->OnInit();
    theGDLApp->OnRun();
  }
  else
  {
#ifdef GDL_DEBUG_WIDGETS
    wxMessageOutputStderr( ).Printf( _T( "GDLWidget:Realize TLB of: %d\n"), this->widgetID );
#endif
    GDLWidgetBase* tlb = GetTopLevelBaseWidget( parentID );
    assert( tlb != NULL );

    GDLFrame *frame = static_cast<GDLFrame *> (tlb->wxWidget);
    if( frame->LastShowRequest() != map)
    {
      this->OnRealize( );
      frame->SendShowRequestEvent( map );
    }
  }
}

GDLWidget::~GDLWidget()
{
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "in ~GDLWidget(): " << widgetID << std::endl;
#endif
  //   managed = false; 
  if (scrollSizer!=NULL) this->UnScrollWidget();
  if (frameSizer!=NULL) this->UnFrameWidget();
//widgets //apparently are *not really removed (thread problem?)* by the delete() command.
//THE FOLLOWING NEEDS MORE WORK!
  if ( parentID != GDLWidget::NullID ) {
    GDLWidget* gdlParent = GetWidget( parentID );
    if ( gdlParent->IsBase( ) ) {
      GDLWidgetBase* base = static_cast<GDLWidgetBase*> (gdlParent);
      //       assert( base != NULL); // should be already checked elsewhere
      base->RemoveChild( widgetID );
    } else {
      GDLWidgetBase* base = GetBaseWidget( parentID );
      if ( base != NULL )
        base->RemoveChild( widgetID );
    }
//    if ( this->GetPanel( ) != NULL ) {wxWindow *me=static_cast<wxWindow*>(this->GetWxWidget()); if (me!=NULL) me->Destroy();}
  }
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

void GDLWidget::updateFlags()
{
//Do Nothing
//  cerr<<"unhandled eventFlag "<<this->GetEventFlags()<<" for widget id "<<this->widgetID<<endl;
}

/*********************************************************/
// for WIDGET_BASE
/*********************************************************/
GDLWidgetBase::GDLWidgetBase( WidgetIDT parentID, EnvT* e,
bool mapWid,
WidgetIDT& mBarIDInOut, bool modal_,
DLong col, DLong row,
int exclusiveMode_,
bool floating_,
const DString& resource_name, const DString& rname_mbar,
const DString& title_,
const DString& display_name,
DLong xpad, DLong ypad,
DLong x_scroll_size, DLong y_scroll_size, bool grid, long children_alignment)
: GDLWidget( parentID, e, mapWid)
, modal( modal_ )
, mbarID( mBarIDInOut )
, lastRadioSelection( NullID )
, nrows(row)
, ncols(col)
, stretchX(FALSE)
, stretchY(FALSE)
, childrenAlignment( children_alignment )
{
  //  std::cout << "In GDLWidgetBase::GDLWidgetBase: " << widgetID << std::endl

  xmanActCom = false;
  //get immediately rid of scroll sizes in case of scroll or not... Here is the logic:
  if (x_scroll_size > 0) {scrolled=TRUE;x_scroll_size+=(SCROLL_WIDTH+2*DEFAULT_BORDER_SIZE);} 
  if (y_scroll_size > 0) {scrolled=TRUE;y_scroll_size+=(SCROLL_WIDTH+2*DEFAULT_BORDER_SIZE);}
  if (scrolled) x_scroll_size=(x_scroll_size<100)?100:x_scroll_size;
  if (scrolled) y_scroll_size=(y_scroll_size<100)?100:y_scroll_size;
  
    if ( xSize <= 0 ) {stretchX=TRUE; xSize=scrolled?120:10;} //provide a default value!
    if ( ySize <= 0 ) {stretchY=TRUE; ySize=scrolled?120:10;} 

  // Set exclusiveMode
  // If exclusive then set to -1 to signal first radiobutton
  if ( exclusiveMode_ == BGEXCLUSIVE )
  this->SetExclusiveMode( BGEXCLUSIVE1ST );
  else
    this->SetExclusiveMode( exclusiveMode_ );

  // If first base widget = Top Level Base Widget:
  // can receive events: size, icon and kill.
  if ( parentID == 0) 
  {
    //     if( modal) // ???
    // 	wxWidget = new wxDialog( wxParent, widgetID, wxString(title_.c_str(), wxConvUTF8));
    //     else
    // GDLFrame is derived from wxFrame
    wxString titleWxString = wxString( title_.c_str( ), wxConvUTF8 );
    GDLFrame *gdlFrame = new GDLFrame( this, NULL, widgetID, titleWxString , wxPoint(xOffset,yOffset));

#ifdef GDL_DEBUG_WIDGETS
    gdlFrame->SetBackgroundColour(wxColour(255,0,0)); //for tests!
#endif
    
    wxWidget = gdlFrame;
    
    if( mbarID != 0)
    {
        GDLWidgetMBar* mBar = new GDLWidgetMBar( widgetID );
        // already called in constructor:
        // mbarID = GDLWidget::NewWidget( mBar);
        mbarID = mBar->WidgetID( );
        mBarIDInOut = mbarID;

        gdlFrame->SetMenuBar( static_cast<wxMenuBar*> (mBar->GetWxWidget( )) );
    }

    topWidgetSizer = new wxBoxSizer( wxVERTICAL);
    gdlFrame->SetSizer( topWidgetSizer );

    //create the first panel, fix size. Offset is not taken into account.
    widgetPanel = new wxPanel( gdlFrame, wxID_ANY , wxDefaultPosition, wxSize(xSize,ySize) );
#ifdef GDL_DEBUG_WIDGETS
    widgetPanel->SetBackgroundColour(wxColour(0,255,0)); //for tests!
#endif
    //give size hints for this panel. I cannot succeed to have a correct size if the
    //base sizer is gridbag and no sizes are given. Thus in the case xsize=ysize=undefined and col=0,
    //I force col=1.
    if (stretchX && stretchY && ncols==0 && nrows==0) ncols=1;
    //Allocate the sizer for children according to col or row layout
    widgetSizer = GetBaseSizer( ncols, nrows, grid );
    if (!stretchX && !stretchY) widgetSizer->SetMinSize(xSize, ySize);
    else if (!stretchX) widgetSizer->SetMinSize(xSize, -1);
    else if (!stretchY) widgetSizer->SetMinSize(-1, ySize);
    //Attach sizer to panel
    widgetPanel->SetSizer( widgetSizer );

    //force frame to wrap around panel
    topWidgetSizer->Add( widgetPanel, 1, wxEXPAND);
    if (scrolled) {
      scrollPanel = new wxScrolledWindow(gdlFrame, wxID_ANY, wxDefaultPosition, wxSize(x_scroll_size, y_scroll_size ), wxALWAYS_SHOW_SB|wxBORDER_SUNKEN);
#ifdef GDL_DEBUG_WIDGETS
      scrollPanel->SetBackgroundColour(wxColour(0,255,255)); //for tests!
#endif
      scrollPanel->SetScrollRate(20,20); //show scrollbars
      scrollSizer = new wxBoxSizer(wxVERTICAL );
      widgetSizer->SetMinSize(x_scroll_size, y_scroll_size);
      scrollPanel->SetSizer( scrollSizer );

      widgetPanel->Reparent(scrollPanel);
      scrollSizer->Add(widgetPanel);//, 1, wxFIXED_MINSIZE|wxALL);
      topWidgetSizer->Detach(widgetPanel);
      topWidgetSizer->Add(scrollPanel, 1, wxEXPAND|wxFIXED_MINSIZE|wxALL,0);
    }
  } 
  else 
  {
    // If parent base widget exists ....
    GDLWidget* gdlParent = GetWidget( parentID );
    wxSizer* parentSizer = gdlParent->GetSizer( );

    if( gdlParent->IsTab())
    {
      GDLWidgetTab* parentTab = static_cast<GDLWidgetTab*> (gdlParent);
      wxNotebook* wxParent = static_cast<wxNotebook*> (parentTab->GetWxWidget( ));
      //create the panel, fix size. Offset is not taken into account.
      widgetPanel = new wxPanel( wxParent, wxID_ANY , wxPoint(xOffset,yOffset), wxSize(xSize,ySize) );
#ifdef GDL_DEBUG_WIDGETS
      widgetPanel->SetBackgroundColour(wxColour(64,128,33)); //for tests!
#endif
      wxWidget = widgetPanel;
      //Allocate the sizer for children according to col or row layout
      widgetSizer = GetBaseSizer( ncols, nrows, grid );
      //give size hints for this panel
      if (!stretchX && !stretchY) widgetSizer->SetMinSize(xSize, ySize);
      else if (!stretchX) widgetSizer->SetMinSize(xSize, -1);
      else if (!stretchY) widgetSizer->SetMinSize(-1, ySize);
      //Attach sizer to panel
      widgetPanel->SetSizer( widgetSizer );

      wxString titleWxString = wxString( title_.c_str( ), wxConvUTF8 );
      wxParent->AddPage( widgetPanel, titleWxString );
    }
    else
    {
      wxWindow* wxParent = static_cast<wxWindow*> (gdlParent->GetWxWidget( ));
      widgetPanel = new wxPanel( wxParent, wxID_ANY , wxPoint(xOffset,yOffset), wxSize(xSize,ySize) );
#ifdef GDL_DEBUG_WIDGETS
      widgetPanel->SetBackgroundColour(wxColour(255,255,0)); //for tests!
#endif
      wxWidget = widgetPanel;
      parentSizer->Add( widgetPanel,0,widgetAlignment());

      widgetSizer = GetBaseSizer( ncols, nrows, grid );
      if (!stretchX && !stretchY) widgetSizer->SetMinSize(xSize, ySize);
      else if (!stretchX) widgetSizer->SetMinSize(xSize, -1);
      else if (!stretchY) widgetSizer->SetMinSize(-1, ySize);
      widgetPanel->SetSizer( widgetSizer );

      if (scrolled) {
        scrollPanel = new wxScrolledWindow(wxParent, wxID_ANY, wxPoint(xOffset,yOffset), wxSize(x_scroll_size, y_scroll_size ), wxALWAYS_SHOW_SB|wxBORDER_SUNKEN);
#ifdef GDL_DEBUG_WIDGETS
        scrollPanel->SetBackgroundColour(wxColour(0,255,255)); //for tests!
#endif
        scrollPanel->SetScrollRate(20,20); //show scrollbars
        scrollSizer = new wxBoxSizer(wxVERTICAL );
        widgetSizer->SetMinSize(x_scroll_size, y_scroll_size);
        scrollPanel->SetSizer( scrollSizer );

        widgetPanel->Reparent(scrollPanel);
        scrollSizer->Add(widgetPanel);//, 1, wxFIXED_MINSIZE|wxALL);
        parentSizer->Detach(widgetPanel);
        parentSizer->Add(scrollPanel, 0, widgetAlignment()|wxGROW|wxFIXED_MINSIZE);
      }
      // If map is true check for parent map value
        if ( mapWid )
        mapWid = gdlParent->GetMap( );
        this->SetMap( mapWid );
    }
    parentSizer->Layout();
  }
}

GDLWidgetBase::~GDLWidgetBase()
{
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "in ~GDLWidgetBase(): " << widgetID << std::endl;
#endif

  // call KILL_NOTIFY procedures
  this->OnKill( );

  // delete all children (in reverse order ?)
  for( SizeT i=children.size(); i>0;  i--) 
  {
    GDLWidget* child = GetWidget( children[i-1] );
    delete child;
  }

  // Close widget frame (might be already closed)
  if( this->parentID == 0)
  {
      if( static_cast<GDLFrame*>(this->wxWidget) != NULL)
      {
        static_cast<GDLFrame*> (this->wxWidget)->NullGDLOwner( );
        delete static_cast<GDLFrame*> (this->wxWidget); //closes the frame etc.
      }
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
  if ( !this->GetXmanagerActiveCommand( ) ){
  eventQueue.PushFront( ev ); // push front (will be handled next)
  } else {
  readlineEventQueue.PushFront( ev ); // push front (will be handled next)
  }
}

DLong GDLWidgetBase::NChildren() const
{
  return children.size( );
}
WidgetIDT GDLWidgetBase::GetChild( DLong childIx) const
{
  assert( childIx >= 0 );
  assert( childIx < children.size( ) );
  return children[childIx];
}

// void  GDLWidgetBase::SetEventPro( DString eventPro)
// {
//   std::cout << "Setting up event handler: " << eventPro.c_str() << std::endl;
//   eventHandler = eventPro;
// }


/*********************************************************/
// for WIDGET_TAB
/*********************************************************/
GDLWidgetTab::GDLWidgetTab( WidgetIDT p, EnvT* e, DLong location, DLong multiline )
: GDLWidget( p, e ) {

  GDLWidget* gdlParent = GetWidget( p );

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
  wxSize( xSize, ySize ),
  style );
  this->wxWidget = notebook;
  long widgetStyle=(wxEXPAND|wxALL)|widgetAlignment();
  widgetSizer->Add( notebook, 0, widgetStyle, DEFAULT_BORDER_SIZE );
  widgetSizer->Layout();
}

//why overcast inherited ~GDLWidget????
GDLWidgetTab::~GDLWidgetTab(){
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "in ~GDLWidgetTable(): " << widgetID << std::endl;
#endif
}



/*********************************************************/
// for WIDGET_TABLE
/*********************************************************/

GDLWidgetTable::GDLWidgetTable( WidgetIDT p, EnvT* e,
DLongGDL* alignment_,
DStringGDL* amPm_,
DByteGDL* backgroundColor_,
DByteGDL* foregroundColor_,
DStringGDL* columnLabels_,
bool columnMajor_,
DLongGDL* columnWidth_,
DStringGDL* daysOfWeek_,
bool disjointSelection_,
bool editable_,
DStringGDL* format_,
DLong groupLeader_,
bool ignoreAccelerators_,
DStringGDL* month_,
bool noColumnHeaders_,
bool noRowHeaders_,
bool resizeableColumns_,
bool resizeableRows_,
DLongGDL* rowHeights_,
DStringGDL* rowLabels_,
bool rowMajor_,
DLong tabMode_,
BaseGDL* value_,
DLong xScrollSize_,
DLong yScrollSize_
)
: GDLWidget( p, e, true, value_ )
, alignment( alignment_ )
, amPm( amPm_ )
, backgroundColor( backgroundColor_ )
, foregroundColor( foregroundColor_ )
, columnLabels( columnLabels_ )
, columnMajor( columnMajor_ )
, columnWidth( columnWidth_ )
, daysOfWeek( daysOfWeek_ )
, disjointSelection( disjointSelection_ )
, editable( editable_ )
, format( format_ )
, groupLeader( groupLeader_ )
, ignoreAccelerators( ignoreAccelerators_ )
, month( month_ )
, noColumnHeaders( noColumnHeaders_ )
, noRowHeaders( noRowHeaders_ )
, resizeableColumns( resizeableColumns_ )
, resizeableRows( resizeableRows_ )
, rowHeights( rowHeights_ )
, rowLabels( rowLabels_ )
, rowMajor( rowMajor_ )
, tabMode( tabMode_ )
, x_scroll_size( xScrollSize_ )
, y_scroll_size( yScrollSize_)
{
  GDLWidget* gdlParent = GetWidget( p );
  wxPanel *panel = gdlParent->GetPanel( );
  widgetPanel = panel;
  widgetSizer = gdlParent->GetSizer( );
  topWidgetSizer = this->GetTopLevelBaseWidget(parentID)->GetSizer();

  wxSize widgetSize = wxDefaultSize;
  if ( xSize == widgetSize.x ) xSize=300; //yes, has a default value!
  if ( ySize == widgetSize.y ) ySize=300;

  long style = wxWANTS_CHARS;
  
  //get immediately rid of scroll sizes in case of scroll or not... Here is the logic:
  if (x_scroll_size > 0) {scrolled=TRUE;x_scroll_size+=(SCROLL_WIDTH+2*DEFAULT_BORDER_SIZE);} 
  if (y_scroll_size > 0) {scrolled=TRUE;y_scroll_size+=(SCROLL_WIDTH+2*DEFAULT_BORDER_SIZE);}
  if (scrolled) x_scroll_size=(x_scroll_size<100)?100:x_scroll_size;
  if (scrolled) y_scroll_size=(y_scroll_size<100)?100:y_scroll_size;

  wxGrid *grid = new wxGrid( widgetPanel, widgetID,
  wxPoint( xOffset, yOffset ), scrolled?wxSize( x_scroll_size, y_scroll_size ):wxSize( xSize, ySize ), style );
//use example (does not work yet!!! ???????)
  // (100 rows and 10 columns in this example)
grid->CreateGrid( 100, 10 );
// We can set the sizes of individual rows and columns
// in pixels
grid->SetRowSize( 0, 60 );
grid->SetColSize( 0, 120 );
// And set grid cell contents as strings
grid->SetCellValue( 0, 0, wxString("wxGrid is good", wxConvUTF8 ) );
// We can specify that some cells are read->only
grid->SetCellValue( 0, 3,  wxString("This is read->only", wxConvUTF8 ) );
grid->SetReadOnly( 0, 3 );
// Colours can be specified for grid cell contents
grid->SetCellValue(3, 3,  wxString("green on grey", wxConvUTF8 ));
grid->SetCellTextColour(3, 3, *wxGREEN);
grid->SetCellBackgroundColour(3, 3, *wxLIGHT_GREY);
// We can specify the some cells will store numeric
// values rather than strings. Here we set grid column 5
// to hold floating point values displayed with width of 6
// and precision of 2
grid->SetColFormatFloat(5, 6, 2);
grid->SetCellValue(0, 6,  wxString("3.1415", wxConvUTF8 ));

this->wxWidget = grid;
//scrollbars are automatic for this widget
//if (scrolled) this->ScrollWidget(xSize, ySize);
if (frame) this->FrameWidget();
if (!frame && !scrolled)  widgetSizer->Add(grid);//, 0, wxEXPAND | wxALL); 
UPDATE_WINDOW
}

//void GDLWidgetTable::OnShow()
//{
//}



/*********************************************************/
// for WIDGET_TREE
/*********************************************************/
GDLWidgetTree::GDLWidgetTree( WidgetIDT p, EnvT* e, DString value_,
bool alignBottom_,
bool alignCenter_,
bool alignLeft_,
bool alignRight_,
bool alignTop_,
BaseGDL* bitmap_,
bool checkbox_,
DLong checked_,
DString dragNotify_,
bool draggable_,
bool expanded_,
bool folder_,
DLong groupLeader_,
DLong index_,
bool mask_,
bool multiple_,
bool noBitmaps_,
DLong tabMode_,
DString toolTip_ )
: GDLWidget( p, e ),
alignBottom( alignBottom_ ),
alignCenter( alignCenter_ ),
alignLeft( alignLeft_ ),
alignRight( alignRight_ ),
alignTop( alignTop_ ),
bitmap( bitmap_ ),
checkbox( checkbox_ ),
checked( checked_ ),
dragNotify( dragNotify_ ),
draggable( draggable_ ),
expanded( expanded_ ),
folder( folder_ ),
groupLeader( groupLeader_ ),
index( index_ ),
mask( mask_ ),
multiple( multiple_ ),
noBitmaps( noBitmaps_ ),
tabMode( tabMode_ ),
toolTip( toolTip_ ),
value( value_ ),
treeItemID( 0L)
{
    GDLWidget* gdlParent = GetWidget( p );
    wxPanel *panel = gdlParent->GetPanel( );
    widgetPanel = panel;
    widgetSizer = gdlParent->GetSizer( );
    topWidgetSizer = this->GetTopLevelBaseWidget(parentID)->GetSizer();

    if( gdlParent->IsBase())
    {
    wxSize widgetSize = wxDefaultSize;
    if ( xSize == widgetSize.x ) xSize=300; //yes, has a default value!
    if ( ySize == widgetSize.y ) ySize=300;
    
    long style = wxTR_DEFAULT_STYLE;
    wxTreeCtrl* tree = new wxTreeCtrl( widgetPanel, widgetID,
    wxPoint( xOffset, yOffset ),
    wxSize( xSize, ySize ),
    style );
    this->wxWidget = tree;
    treeItemID = tree->AddRoot( wxString( value.c_str( ), wxConvUTF8 ) );
    if (frame) this->FrameWidget();  else  widgetSizer->Add(tree,0,widgetAlignment());//, 0, wxEXPAND | wxALL); 
  UPDATE_WINDOW    
    }
    else
    {
    assert( gdlParent->IsTree( ) );
    GDLWidgetTree* parentTree = static_cast<GDLWidgetTree*> (gdlParent);

    this->wxWidget = parentTree->GetWxWidget( );

    wxTreeCtrl * tree = static_cast<wxTreeCtrl*> (this->wxWidget);
    treeItemID = tree->AppendItem( parentTree->treeItemID, wxString( value.c_str( ), wxConvUTF8 ) );
    }

}


//void GDLWidgetTree::OnShow()
//{
////  GDLWidget* gdlParent = GetWidget( parentID );
//
////  wxSizer *boxSizer = gdlParent->GetSizer( );
////  boxSizer->Layout( );
//}




/*********************************************************/
// for WIDGET_SLIDER
/*********************************************************/
GDLWidgetSlider::GDLWidgetSlider( WidgetIDT p, EnvT* e, DLong value_
, DLong minimum_
, DLong maximum_
, bool vertical
, bool suppressValue
, DString title_ )
: GDLWidget( p, e )
, value( value_ )
, minimum( minimum_ )
, maximum( maximum_ )
, title( title_)
{
  GDLWidget* gdlParent = GetWidget( p );
  widgetPanel = gdlParent->GetPanel( );
  widgetSizer = gdlParent->GetSizer( );
  topWidgetSizer = this->GetTopLevelBaseWidget(parentID)->GetSizer();

  wxSize widgetSize = wxDefaultSize;
  if (( xSize == widgetSize.x ) && !vertical) xSize=120; //yes, has a default value!
  if (( ySize == widgetSize.y ) && vertical) ySize=120;

  long style = wxSL_HORIZONTAL;
  if ( vertical )
  style |= wxSL_VERTICAL;
  if ( !suppressValue ) style |= wxSL_LABELS;

  wxSlider * slider = new wxSlider( widgetPanel, widgetID, value, minimum, maximum,
  wxPoint( xOffset, yOffset ),
  wxSize( xSize, ySize ),
  style );
  this->wxWidget = slider;
        
  if (frame) this->FrameWidget(wxEXPAND | wxALL);  else  widgetSizer->Add(slider, 0, wxEXPAND | wxALL); 
UPDATE_WINDOW
}

GDLWidgetSlider::~GDLWidgetSlider(){
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "in ~GDLWidgetSlider(): " << widgetID << std::endl;
#endif
}

GDLWidgetButton::GDLWidgetButton( WidgetIDT p, EnvT* e,
const DString& value , bool isMenu, bool hasSeparatorAbove)
: GDLWidget( p, e )
, buttonType( UNDEFINED )
, addSeparatorAbove(hasSeparatorAbove)
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
    this->wxWidget = new wxMenu( );
    wxString valueWxString = wxString( value.c_str( ), wxConvUTF8 );
    menuBar->Append( static_cast<wxMenu*> (this->wxWidget), valueWxString );
    buttonType = MBAR;
  }
  else
  {
    if( gdlParent->IsButton())
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
        wxString valueWxString = wxString( value.c_str( ), wxConvUTF8 );
        wxButton *button = new wxButton( widgetPanel, widgetID, valueWxString,
        wxPoint( xOffset, yOffset ) , wxSize(xSize,ySize), textAlignment());
        this->wxWidget = button;
        buttonType = NORMAL;
        if (frame) this->FrameWidget();  else  widgetSizer->Add( button, 0, widgetAlignment(),0); //, 0, wxEXPAND | wxALL);
        GetWidget( parentID )->GetSizer()->Layout(); 
      }
      else if ( gdlParent->GetExclusiveMode() == BGEXCLUSIVE1ST) 
      {
        wxString valueWxString = wxString( value.c_str( ), wxConvUTF8 );
        wxRadioButton *radioButton = new wxRadioButton( widgetPanel, widgetID, valueWxString,
        wxPoint( xOffset, yOffset ), wxSize( xSize, ySize ), //wxDefaultSize,
        wxRB_GROUP );
        gdlParent->SetExclusiveMode( 1 );
        static_cast<GDLWidgetBase*> (gdlParent)->SetLastRadioSelection( widgetID );
        widgetSizer->Add( radioButton); //, 0, wxEXPAND | wxALL);
        this->wxWidget = radioButton;
        buttonType = RADIO;
      } 
      else if ( gdlParent->GetExclusiveMode() == BGEXCLUSIVE) 
      {
        wxString valueWxString = wxString( value.c_str( ), wxConvUTF8 );
        wxRadioButton *radioButton = new wxRadioButton( widgetPanel, widgetID, valueWxString,
        wxPoint( xOffset, yOffset ), wxSize( xSize, ySize ) );
        widgetSizer->Add( radioButton); // , 0, wxEXPAND | wxALL);
        this->wxWidget = radioButton;
        buttonType = RADIO;
      } 
      else if ( gdlParent->GetExclusiveMode() == BGNONEXCLUSIVE) 
      {
        wxString valueWxString = wxString( value.c_str( ), wxConvUTF8 );
        wxCheckBox *checkBox = new wxCheckBox( widgetPanel, widgetID, valueWxString,
        wxPoint( xOffset, yOffset ), wxSize( xSize, ySize ) );
        widgetSizer->Add( checkBox); //, 0, wxEXPAND | wxALL);
        this->wxWidget = checkBox;
        buttonType = CHECKBOX;
      }
    }
  }
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
  
GDLWidgetList::GDLWidgetList( WidgetIDT p, EnvT* e, BaseGDL *value, DLong style )
    : GDLWidget( p, e, true, value)
{
  GDLWidget* gdlParent = GetWidget( p );
  widgetPanel = gdlParent->GetPanel( );
  widgetSizer = gdlParent->GetSizer( );
  topWidgetSizer = this->GetTopLevelBaseWidget(parentID)->GetSizer();

  if( vValue->Type() != GDL_STRING)
  {
    vValue = static_cast<DStringGDL*> (vValue->Convert2( GDL_STRING, BaseGDL::CONVERT ));
  }
  DStringGDL* val = static_cast<DStringGDL*> (vValue);

  DLong n = val->N_Elements( );
  wxArrayString choices; // = new wxString[n];
  for ( SizeT i = 0; i < n; ++i )
  choices.Add( wxString( (*val)[i].c_str( ), wxConvUTF8 ) );
  if (ySize<2) {
    ySize=(wxSystemSettings::GetFont( wxSYS_SYSTEM_FONT ).GetPixelSize()).y*2; //one line as in *DL -- annoying with the scrollbar, should step by line!
  }
  
  wxListBox * list = new wxListBox( gdlParent->GetPanel( ), widgetID, wxPoint( xOffset, yOffset ),
  wxSize( xSize, ySize ),
  choices, style|wxLB_NEEDED_SB );
  this->wxWidget = list;

  if (frame) this->FrameWidget();  else  widgetSizer->Add(list, 0, wxEXPAND | wxALL); //IDL behaviour
UPDATE_WINDOW
}

GDLWidgetDropList::GDLWidgetDropList( WidgetIDT p, EnvT* e, BaseGDL *value,
const DString& title_, DLong style_ )
: GDLWidget( p, e, true, static_cast<DStringGDL*> (value->Convert2( GDL_STRING, BaseGDL::CONVERT )) )
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
    , (frame)?wxBORDER_SIMPLE:wxBORDER_NONE 
    );
    wxSizer * box = new wxBoxSizer(wxHORIZONTAL);
    panel->SetSizer(box);
    wxStaticText *text=new wxStaticText(panel,wxID_ANY,wxString( title.c_str( ), wxConvUTF8 ));
    wxChoice * droplist = new wxChoice( panel, widgetID, wxPoint( xOffset, yOffset ), wxSize( xSize, ySize ), choices, style );
    droplist->SetSelection(0);
    this->wxWidget = droplist;
    box->Add(text);
    box->Add( droplist);
    box->Layout();
    widgetSizer->Add(panel);//,0,widgetAlignment());
 }else{
#endif    
   wxChoice * droplist = new wxChoice( widgetPanel, widgetID, 
   wxPoint( xOffset, yOffset ), wxSize( xSize, ySize ), choices, style );
   droplist->SetSelection(0);
   this->wxWidget = droplist;
   if (title.size()>0){
     wxStaticBoxSizer *sz = new wxStaticBoxSizer(wxHORIZONTAL,widgetPanel,wxString( title.c_str( ), wxConvUTF8 ));
     sz->Add(droplist);
     widgetSizer->Add(sz,0,widgetAlignment());
   } else {if (frame) this->FrameWidget(); else widgetSizer->Add( droplist,0,widgetAlignment());} //, 0, wxEXPAND | wxALL);
    
UPDATE_WINDOW
}

//void GDLWidgetDropList::OnShow()  
//{
//  //  std::cout << "In DropList: " << widgetID << " " << p << std::endl;
//}

GDLWidgetComboBox::GDLWidgetComboBox( WidgetIDT p, EnvT* e, BaseGDL *value,
const DString& title_, DLong style_ )
: GDLWidget( p, e, true, static_cast<DStringGDL*> (value->Convert2( GDL_STRING, BaseGDL::CONVERT )) )
, title( title_ )
, style( style_)
{
  GDLWidget* gdlParent = GetWidget( p );
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
  wxPoint( xOffset, yOffset ), wxSize( xSize, ySize ), choices, style );
  this->wxWidget = combo;
  long widgetStyle=(wxEXPAND|wxALL)|widgetAlignment();
  if (frame) this->FrameWidget(widgetStyle); else widgetSizer->Add(combo, 0,widgetStyle);
UPDATE_WINDOW
}

//void GDLWidgetComboBox::OnShow()  
//{
//  //  std::cout << "In ComboBox: " << widgetID << " " << p << std::endl;
//}

GDLWidgetText::GDLWidgetText( WidgetIDT p, EnvT* e, DStringGDL* valueStr, bool noNewLine_,
bool editable_ )
: GDLWidget( p, e, true, valueStr )
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

  GDLWidget* gdlParent = GetWidget( p );
  widgetPanel = gdlParent->GetPanel( );
  widgetSizer = gdlParent->GetSizer( );
  topWidgetSizer = this->GetTopLevelBaseWidget(parentID)->GetSizer();
  
  //widget text size is in LINES in Y and CHARACTERS in X
  wxSize fontSize = wxSystemSettings::GetFont( wxSYS_SYSTEM_FONT ).GetPixelSize();
  wxSize widgetSize = wxDefaultSize;
  if ( xSize != widgetSize.x )
  widgetSize.x = xSize * fontSize.x;
  else widgetSize.x = maxlinelength * fontSize.x;
  if ( widgetSize.x < 100 ) widgetSize.x=20* fontSize.x; //default
  if ( ySize != widgetSize.y )  widgetSize.y = (ySize * fontSize.y)+nlines*1; //1 pixel between lines
  else widgetSize.y = fontSize.y +1 ; //instead of nlines*fontSize.y to be compliant with *DL 

  wxString valueWxString = wxString( lastValue.c_str( ), wxConvUTF8 );
  long style = wxTE_NOHIDESEL|wxTE_PROCESS_ENTER|textAlignment();

 if ( !editable ) style |= wxTE_READONLY; //no, because *DL provides READONLY and still give events.
// this permits to a procedure (cw_field.pro for example) to filter the editing of text fields.
// so wxTextCtrl::OnChar is overwritten in gdlwidgeteventhandler.cpp just for this reason.
  if ( nlines > 1 || scrolled ) style |= wxTE_MULTILINE;
  
  wxTextCtrl * text = new wxTextCtrl( widgetPanel, widgetID, valueWxString,
  wxPoint( xOffset, yOffset ), widgetSize, style );
  text->SetInsertionPoint(0);
  text->SetSelection(0,0);
  this->wxWidget = text;
  long widgetStyle=(wxEXPAND|wxALL)|widgetAlignment();
  if (frame) this->FrameWidget(widgetStyle); else widgetSizer->Add(text, 0,widgetStyle);
UPDATE_WINDOW
}

//void GDLWidgetText::OnShow()
//{
//  cerr<<"text:OnShow"<<endl;
//}


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
//runs in main, wants lock with GUI
  DLongGDL* pos=new DLongGDL(dimension(2),BaseGDL::ZERO);
  long from,to;
  static_cast<wxTextCtrl*>(wxWidget)->GetSelection(&from,&to);
  (*pos)[0]=from; (*pos)[1]=to-from;
  return pos;
}

GDLWidgetLabel::GDLWidgetLabel( WidgetIDT p, EnvT* e, const DString& value_ , bool sunken)
: GDLWidget( p, e )
, value(value_)
{
  GDLWidget* gdlParent = GetWidget( p );
  widgetPanel = gdlParent->GetPanel( );
  widgetSizer = gdlParent->GetSizer( );
  topWidgetSizer = this->GetTopLevelBaseWidget(parentID)->GetSizer();

  wxString valueWxString = wxString( value.c_str( ), wxConvUTF8 );
  wxStaticText* label = new wxStaticText( widgetPanel, widgetID, valueWxString,
  wxPoint( xOffset, yOffset ), wxSize( xSize, ySize ), textAlignment()|wxST_NO_AUTORESIZE );
  this->wxWidget = label;
  widgetStyle=(wxEXPAND|wxALL)|widgetAlignment();
  if (sunken) widgetStyle|=wxBORDER_SUNKEN;
  if (frame||sunken) this->FrameWidget(widgetStyle);  else  widgetSizer->Add(label,0,widgetStyle);
UPDATE_WINDOW
}

void GDLWidget::FrameWidget(long style)
{
  if (this->GetWidgetType()=="BASE") return; //function invalid with base widgets.
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
  if (this->GetWidgetType()=="BASE") return; //function invalid with base widgets.
  if (frameSizer!=NULL) { //protect against potential problems
    if (scrollSizer==NULL) {
      frameSizer->Detach(static_cast<wxWindow*>(this->wxWidget));
      static_cast<wxWindow*>(this->wxWidget)->Reparent(widgetPanel);
      widgetSizer->Add(static_cast<wxWindow*>(this->wxWidget), 0,widgetStyle);
    } else {
      frameSizer->Detach(static_cast<wxWindow*>(this->scrollPanel));
      static_cast<wxWindow*>(this->scrollPanel)->Reparent(widgetPanel);
      widgetSizer->Add(static_cast<wxWindow*>(this->scrollPanel), 0,widgetStyle);
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
  if (this->GetWidgetType()=="BASE") return; //function invalid with base widgets.
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
      frameSizer->Layout();
    } else {
      widgetSizer->Detach(static_cast<wxWindow*>(this->wxWidget));
      widgetSizer->Add(scrollPanel, 0, wxFIXED_MINSIZE|widgetStyle);
      widgetSizer->Layout();
    }
  }
}
void GDLWidget::UnScrollWidget()
{
  if (this->GetWidgetType()=="BASE") return; //function invalid with base widgets.
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
     widgetSizer->Layout();
    }
    delete scrollPanel;
    scrollSizer=NULL;
    scrollPanel=NULL;
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

// GDL widgets =====================================================
// GDLFrame ========================================================
GDLFrame::GDLFrame( GDLWidgetBase* gdlOwner_, wxWindow* parent, wxWindowID id, const wxString& title , const wxPoint& pos )
: wxFrame( parent, id, title, pos )
, lastShowRequest( false )
, gdlOwner( gdlOwner_)
{
// m_timer = new wxTimer(this,TIMER_RESIZE);
}

GDLFrame::~GDLFrame()
{ 
#ifdef GDL_DEBUG_WIDGETS
    std::cout << "~GDLFrame: " << this << std::endl;
    std::cout << "This IsMainThread: " << wxIsMainThread() << std::endl;
#endif  
 if( gdlOwner != NULL)
  {
    gdlOwner->NullWxWidget( );
    gdlOwner->SelfDestroy( ); // send delete request to GDL owner
  }
//  delete m_timer;
 }

// GDLDrawPanel ========================================================
GDLDrawPanel::GDLDrawPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, 
DULong eventFlags_, const wxString& name)
: wxPanel( parent, id, pos, size, style, name )
, pstreamIx( -1 )
, pstreamP( NULL )
, m_dc( NULL)
, eventFlags(eventFlags_)
{
  // initialization of stream is done in GDLWidgetDraw::OnRealize()
}
void GDLDrawPanel::SetEventFlags(DULong eventFlags_)
{
  eventFlags=eventFlags_;
}
void GDLDrawPanel::InitStream()
{
  pstreamIx = GraphicsDevice::GetGUIDevice( )->WAddFree( );
  if ( pstreamIx == -1 )
  throw GDLException( "Failed to allocate GUI stream." );

  drawSize = this->GetSize( );
  bool success = GraphicsDevice::GetGUIDevice( )->GUIOpen( pstreamIx, drawSize.x, drawSize.y );
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
     std::cout << "This IsMainThread: " << wxIsMainThread() << std::endl;
#endif
  if ( pstreamP != NULL )
  pstreamP->SetValid( false );
  }

//void GDLDrawPanel::Update()
//{
//  //   cout << "in GDLDrawPanel::Update()" << endl;
//  SendPaintEvent( );
//  //   wxClientDC dc( this);
//  //   dc.SetDeviceClippingRegion( GetUpdateRegion() );
//  //   GUIMutexLockerT gdlMutexGuiEnterLeave;
//  //   dc.Blit( 0, 0, drawSize.x, drawSize.y, m_dc, 0, 0 );
//  //   wxPanel::Update();
//  //   gdlMutexGuiEnterLeave.Leave();
//}


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

