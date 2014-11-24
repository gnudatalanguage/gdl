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
//#include <wx-2.8/wx/gtk/font.h>

#include "basegdl.hpp"
#include "dstructgdl.hpp"

#include "dinterpreter.hpp"


#include "gdlwxstream.hpp"

#include "gdlwidget.hpp"

#include "widget.hpp"
#include "graphicsdevice.hpp"


//#define GDL_DEBUG_WIDGETS



const WidgetIDT GDLWidget::NullID = 0;

// instantiation
// WidgetIDT	GDLWidget::widgetIx;
WidgetListT GDLWidget::widgetList;

GDLGUIThread* GDLGUIThread::gdlGUIThread = NULL;

// VarListT                    eventVarList;
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


wxSizer* GetNewSizer( DLong col, DLong row, DLong frameBox, wxPanel *panel) 
{
  wxSizer* sizer = NULL;
  if ( frameBox == 0 ) {
    if ( row == 0  && col <= 1) {sizer = new wxBoxSizer( wxVERTICAL ); }
    else if ( row == 0 && col > 1) {sizer = new wxFlexGridSizer( 0, col, 0, 0 );}
    else if ( col == 0 && row <= 1) {sizer = new wxBoxSizer( wxHORIZONTAL );}
    else if ( col == 0 && row > 1) {sizer = new wxFlexGridSizer( row, 0, 0, 0 );}
    else sizer = new wxFlexGridSizer( row, col, 0, 0 ); //which should not happen.
    } else {
    if ( row == 0  && col <= 1) {
      wxStaticBox *box = new wxStaticBox( panel, wxID_ANY, _T( "" ) );
      sizer = new wxStaticBoxSizer( box, wxVERTICAL );
    } else if ( row == 0 && col > 1) {
      sizer = new wxFlexGridSizer( 0, col, 0, 0 );
    } else if ( col == 0 && row <= 1 ) {
      wxStaticBox *box = new wxStaticBox( panel, wxID_ANY, _T( "" ) );
      sizer = new wxStaticBoxSizer( box, wxHORIZONTAL );
    } else if ( col == 0 && row > 1) {
      sizer = new wxFlexGridSizer( row, 0, 0, 0 );
    }else {
      sizer = new wxFlexGridSizer( row, col, 0, 0 );
    }
  }
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



int GDLWidget::HandleEvents()
{
  DStructGDL* ev = NULL;
  while( (ev = GDLWidget::readlineEventQueue.Pop()) != NULL)
  {
    //     DStructGDL* ev = GDLWidget::readlineEventQueue.Pop();
    //     if( ev == NULL)
    //       continue;

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
// The FOLLOWING ALWAYS CRASHES GDL in "complicated" cases such as "atv.pro".
// Removed until further notice.
// Necessity of "refreshing" TLBs is not obvious yet for me.
//    // refresh (if tlb still exists (handler might have destroyed it))
//    // even on unhandled event as it might have been rewritten by a handler
//    // use 'old' tlb as id might no longer exist
//    GDLWidget *tlw = GetWidget( tlb );
//    if( tlw != NULL)
//    {
//      assert( dynamic_cast<GDLFrame*> (tlw->wxWidget) != NULL );
//      // Pause 50 millisecs then refresh widget . This avoids a lot of crashes... to be tested further!
//           wxMilliSleep( 10 ); //passed to 10 (GD: 50 is way too long for atv.pro! 
//      GUIMutexLockerWidgetsT gdlMutexGuiEnterLeave;
//     static_cast<GDLFrame*> (tlw->wxWidget)->Refresh( ); //apparently we crash too often here...
//     static_cast<GDLFrame*> (tlw->wxWidget)->Update(); //...let's try Update, which is immediate (Refresh() waits for the next event loop).
//      gdlMutexGuiEnterLeave.Leave( );
//    }
  }
  return 0;
}

void GDLWidget::PushEvent( WidgetIDT baseWidgetID, DStructGDL* ev) {
  //end eventual busycursor since this is the last stage of processing an event:
  wxEndBusyCursor( );
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
//void GDLWidget::SetSizer( wxSizer *sizer)
//{
//  widgetSizer = sizer;
//}

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
, managed( false )
, map( map_ )
, eventFlags( eventFlags_ ) {
  if ( e != NULL )
    SetCommonKeywords( e );

  widgetID = wxWindow::NewControlId( );

  if ( parentID != GDLWidget::NullID ) {
    GDLWidget* gdlParent = GetWidget( parentID );
    if ( gdlParent->IsBase( ) ) {
      GDLWidgetBase* base = static_cast<GDLWidgetBase*> (gdlParent);
      //       assert( base != NULL); // should be already checked elsewhere
      base->AddChild( widgetID );
    } else {
      GDLWidgetBase* base = GetBaseWidget( parentID );
      if ( base != NULL )
        base->AddChild( widgetID );
    }
  }

  widgetList.insert( widgetList.end( ), std::pair<WidgetIDT, GDLWidget*>(widgetID, this) );
#ifdef GDL_DEBUG_WIDGETS
  //     GUIMutexLockerWidgetsT gdlMutexGuiEnterLeave;
  wxMessageOutputStderr( ).Printf( _T( "inserted: ID: %d  parentID: %d\n" ), widgetID, parentID );
  //     std::cout << "inserted: ID: " << widgetID << "  parentID: " << parentID << "   uname: " << uName << std::endl;
#endif
}


void GDLWidget::CreateWidgetPanel(int borderWidth, wxBorder bb )
{
  GDLWidget* gdlParent = GetWidget( parentID );

  wxPanel *parentPanel = gdlParent->GetPanel( );

  wxPanel *panel = new wxPanel( parentPanel, wxID_ANY
  , wxDefaultPosition
  , wxDefaultSize
  , bb  //   , wxBORDER_SIMPLE 
  );
  widgetPanel = panel;

  gdlParent->GetSizer( )->Add( panel, 0, wxEXPAND | wxALL, borderWidth);
}

void GDLWidget::SetSensitive(bool value)
{
   GDLFrame *frame = static_cast<GDLFrame*> (this->wxWidget);
   frame->Enable(value);
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
  //bad:  frame->SetClientSize(widgetPanel->GetSize());
  //bad:  frame->Layout();

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
    if( frame->LastShowRequest() != map)
    {
      this->OnRealize( );
      frame->SendShowRequestEvent( map );
    }
  }

  // start GUI thread

  if (GDLGUIThread::gdlGUIThread == NULL)
  {
    GDLGUIThread::gdlGUIThread = new GDLGUIThread( );
    std::cout << "Created thread: " << GDLGUIThread::gdlGUIThread << std::endl;
#ifdef GDL_DEBUG_WIDGETS
    std::cout << "Created thread: " << GDLGUIThread::gdlGUIThread << std::endl;
    std::cout << "This IsMainThread: " << wxIsMainThread( ) << std::endl;
#endif    

    GDLGUIThread::gdlGUIThread->Create( );
    if ( GDLGUIThread::gdlGUIThread->Run() != wxTHREAD_NO_ERROR )
    {
      //       delete gdlGUIThread;
      GDLGUIThread::gdlGUIThread = NULL;
      throw GDLException( "Failed to create GUI thread." );
    }
  }
}

GDLWidget::~GDLWidget()
{
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "in ~GDLWidget(): " << widgetID << std::endl;
#endif
  //   managed = false;

  //   if( parentID != 0) 
  //     {
  //       GDLWidgetBase* base = dynamic_cast< GDLWidgetBase*>( GetWidget( parentID));
  //       assert( base != NULL);
  //       base->RemoveChild( widgetID);
  //     }
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
  GUIMutexLockerWidgetsT gdlMutexGuiEnterLeave;
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
  GUIMutexLockerWidgetsT gdlMutexGuiEnterLeave;
    if (win!=NULL)  win->Lower();
}

void GDLWidget::updateFlags()
{
  cerr<<"unhandled eventFlag "<<this->GetEventFlags()<<" for widget id "<<this->widgetID<<endl;
}

/*********************************************************/
// for WIDGET_BASE
/*********************************************************/
GDLWidgetBase::GDLWidgetBase( WidgetIDT parentID, EnvT* e,
bool mapWid,
WidgetIDT& mBarIDInOut, bool modal_,
DLong col, DLong row,
long events, // events handled
int exclusiveMode_,
bool floating_,
const DString& resource_name, const DString& rname_mbar,
const DString& title_,
const DString& display_name,
DLong xpad, DLong ypad,
DLong x_scroll_size, DLong y_scroll_size )
: GDLWidget( parentID, e, mapWid )
, modal( modal_ )
, mbarID( mBarIDInOut )
, lastRadioSelection( NullID )
, m_gdlFrameOwnerMutexP( NULL)
, nrows(0)
, ncols(0)
{
  //  std::cout << "In GDLWidgetBase::GDLWidgetBase: " << widgetID << std::endl

  xmanActCom = false;
  wxWindow *wxParent = NULL;
  //get immediately rid of scroll sizes in case of scroll or not... Here is the logic:
  if (x_scroll_size > 0) {scrolled=TRUE;x_scroll_size+=(SCROLL_WIDTH+2*DEFAULT_BORDER_SIZE);} 
  if (y_scroll_size > 0) {scrolled=TRUE;y_scroll_size+=(SCROLL_WIDTH+2*DEFAULT_BORDER_SIZE);}
  if (scrolled) x_scroll_size=(x_scroll_size<100)?100:x_scroll_size;
  if (scrolled) y_scroll_size=(y_scroll_size<100)?100:y_scroll_size;
  //ncols, nrows
  ncols=col;
  nrows=row;
  
  // Set exclusiveMode
  // If exclusive then set to -1 to signal first radiobutton
  if ( exclusiveMode_ == BGEXCLUSIVE )
  this->SetExclusiveMode( BGEXCLUSIVE1ST );
  else
    this->SetExclusiveMode( exclusiveMode_ );

  // If first base widget ...
  if ( parentID == 0) 
  {
    // obsolete: thread need to be created here (in realize it is too late)


    //     if( modal) // ???
    // 	wxWidget = new wxDialog( wxParent, widgetID, wxString(title_.c_str(), wxConvUTF8));
    //     else
    // GDLFrame is derived from wxFrame
    wxString titleWxString = wxString( title_.c_str( ), wxConvUTF8 );
    GDLFrame *gdlFrame = new GDLFrame( this, wxParent, widgetID, titleWxString );
    wxWidget = gdlFrame;
    
    m_gdlFrameOwnerMutexP = gdlFrame->m_gdlFrameOwnerMutexP;
    assert( m_gdlFrameOwnerMutexP != NULL );
    //     gdlFrame->Freeze();

//    GUIMutexLockerWidgetsT gdlMutexGuiEnterLeave;

    if( mbarID != 0)
    {
        GDLWidgetMBar* mBar = new GDLWidgetMBar( widgetID );
        // already called in constructor:
        // mbarID = GDLWidget::NewWidget( mBar);
        mbarID = mBar->WidgetID( );
        mBarIDInOut = mbarID;

        gdlFrame->SetMenuBar( static_cast<wxMenuBar*> (mBar->GetWxWidget( )) );

      }

    gdlFrame->SetSize(xOffset, yOffset, xSize, ySize, wxDEFAULT_FRAME_STYLE );
    
    wxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
    topWidgetSizer = topSizer;
    gdlFrame->SetSizer( topSizer );

    wxPanel *panel = new wxPanel( gdlFrame, wxID_ANY );
    widgetPanel = panel;
    //    std::cout << "Creating Panel: " << panel << std::endl;

    wxSizer *sizer = GetNewSizer( ncols, nrows, frame, panel );
    widgetSizer = sizer;


    //std::cout << "SetSizer: " << *sizerPtr << std::endl;
    panel->SetSizer( sizer );

    topSizer->Add( panel );
  } 
  else 
  {
    // If parent base widget exists ....
    GDLWidget* gdlParent = GetWidget( parentID );
    wxSizer* parentSizer = gdlParent->GetSizer( );

//    GUIMutexLockerWidgetsT gdlMutexGuiEnterLeave;

    if( gdlParent->IsTab())
    {
      GDLWidgetTab* parentTab = static_cast<GDLWidgetTab*> (gdlParent);
      wxNotebook* wxParent = static_cast<wxNotebook*> (parentTab->GetWxWidget( ));

      wxPanel *panel = new wxPanel( wxParent, wxID_ANY );
      widgetPanel = panel;
      wxWidget = panel;

      wxSizer * sizer = GetNewSizer( ncols, nrows, frame, panel );
      widgetSizer = sizer;
      panel->SetSizer( sizer );

      wxString titleWxString = wxString( title_.c_str( ), wxConvUTF8 );
      wxParent->AddPage( panel, titleWxString );
      //       parentSizer->SetSizeHints( wxParent);
    }
    else
    {
      wxWindow* wxParent = static_cast<wxWindow*> (gdlParent->GetWxWidget( ));
      if ( scrolled ) { //the right way to do the job!!!
        wxScrolledWindow *scrw;
        scrw = new wxScrolledWindow( wxParent, wxID_ANY , wxPoint(xOffset,yOffset), wxSize(x_scroll_size, y_scroll_size ));
        scrw->SetScrollbars(1, 1, 1, 1); //needed to have scrollbars appear
        wxSizer * scrwsizer = new wxBoxSizer( wxVERTICAL );
        scrw->SetSizer( scrwsizer );
        
        wxPanel *panel;
        panel = new wxPanel(scrw, wxID_ANY,  wxDefaultPosition, wxSize(xSize, ySize ), wxBORDER_SUNKEN); 
        wxSizer * panelsizer = GetNewSizer( ncols, nrows, frame, panel );
        panel->SetSizer( panelsizer );
        scrwsizer->Add( panel, 0, wxGROW);     // important to fix sizes
        parentSizer->Add(scrw, 0, wxFIXED_MINSIZE, DEFAULT_BORDER_SIZE);    //--->very important to fix sizes here instead! 
        widgetPanel = panel;
        wxWidget = panel;
        widgetSizer = panelsizer ;

      } else {
        wxPanel *panel;
        panel = new wxPanel( wxParent, wxID_ANY, wxPoint(xOffset,yOffset), wxSize(xSize, ySize ), wxBORDER_DEFAULT);
        widgetPanel = panel;
        wxWidget = panel;
        wxSizer * panelsizer = GetNewSizer( ncols, nrows, frame, panel );
        widgetSizer = panelsizer;
        panel->SetSizer( panelsizer );
        parentSizer->Add( panel, 0, wxGROW, DEFAULT_BORDER_SIZE);//wxEXPAND|wxALL);
       }

      // If map is true check for parent map value
        if ( mapWid )
        mapWid = gdlParent->GetMap( );
        this->SetMap( mapWid );
        }
  }
}

GDLWidgetBase::~GDLWidgetBase()
{
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "in ~GDLWidgetBase(): " << widgetID << std::endl;
#endif

  // call KILL_NOTIFY procedures
  this->OnKill( );

  // delete all children in reverse order...
  for( SizeT i=children.size(); i>0;  i--) 
  {
    GDLWidget* child = GetWidget( children[i-1] );
    delete child;
  }

  GUIMutexLockerWidgetsT gdlMutexGuiEnterLeave;

  // Close widget frame (might be already closed)
  if( this->parentID == 0)
  {
    if ( m_gdlFrameOwnerMutexP != NULL ) {
      if( static_cast<GDLFrame*>(this->wxWidget) != NULL)
      {
        m_gdlFrameOwnerMutexP->Lock( );
        static_cast<GDLFrame*> (this->wxWidget)->NullGDLOwner( );
        // this seems to provoke: LIBDBUSMENU-GLIB-WARNING **: Trying to remove a child that doesn't believe we're it's parent.
        // on wxWidgets < 2.9.5
        static_cast<GDLFrame*> (this->wxWidget)->Destroy( ); // instead of delete operator
        m_gdlFrameOwnerMutexP->Unlock( );
      }
      else
      {
        delete m_gdlFrameOwnerMutexP;
        m_gdlFrameOwnerMutexP=NULL;
      }
    }//else the widget has already been destroyed!à
  }

  // remove all widgets still in the queue for current TLB
  eventQueue.Purge( widgetID);
  readlineEventQueue.Purge( widgetID);
}


// called from event handling thread
// sends a destroy event for itself
void GDLWidgetBase::Destroy()
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
  //  GUIMutexLockerWidgetsT gdlMutexGuiEnterLeave;

  GDLWidget* gdlParent = GetWidget( p );
  wxWindow *wxParent = static_cast<wxWindow*> (gdlParent->GetWxWidget( ));

  wxPanel *panel = gdlParent->GetPanel( );

  long style = wxNB_TOP;
  if ( location == 1 ) style = wxNB_BOTTOM;
  if ( location == 2 ) style = wxNB_LEFT;
  if ( location == 3 ) style = wxNB_RIGHT;

  if ( multiline != 0 )
    style |= wxNB_MULTILINE;

  wxNotebook * notebook = new wxNotebook( panel, widgetID,
  wxPoint( xOffset, yOffset ),
  wxSize( xSize, ySize ),
  style );
  this->wxWidget = notebook;

  widgetSizer = gdlParent->GetSizer( );
  widgetSizer->Add( notebook, 0, wxEXPAND | wxALL, DEFAULT_BORDER_SIZE );
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
, xScrollSize( xScrollSize_ )
,yScrollSize( yScrollSize_)
{
  CreateWidgetPanel( );
  wxSize widgetSize = wxDefaultSize;
  if ( xSize == widgetSize.x ) xSize=300; //yes, has a default value!
  if ( ySize == widgetSize.y ) ySize=300;
}

void GDLWidgetTable::OnShow()
{
//  GUIMutexLockerWidgetsT gdlMutexGuiEnterLeave;
  GDLWidget* gdlParent = GetWidget( parentID );

  long style = wxWANTS_CHARS;
  wxGrid *grid = new wxGrid( widgetPanel, widgetID,
  wxPoint( xOffset, yOffset ), wxSize( xSize, ySize ), style );
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
}



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
  GDLWidget* gdlParent = GetWidget( parentID );
    if( gdlParent->IsBase())
    {
    CreateWidgetPanel( );
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


void GDLWidgetTree::OnShow()
{
//  GUIMutexLockerWidgetsT gdlMutexGuiEnterLeave;

//  GDLWidget* gdlParent = GetWidget( parentID );

//  wxSizer *boxSizer = gdlParent->GetSizer( );
//  boxSizer->Layout( );

}




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
//  GUIMutexLockerWidgetsT gdlMutexGuiEnterLeave;

  GDLWidget* gdlParent = GetWidget( p );
  wxWindow *wxParent = static_cast<wxWindow*> (gdlParent->GetWxWidget( ));

  wxPanel *panel = gdlParent->GetPanel( );
  
  wxSize widgetSize = wxDefaultSize;
  if (( xSize == widgetSize.x ) && !vertical) xSize=100; //yes, has a default value!
  if (( ySize == widgetSize.y ) && vertical) ySize=100;

  long style = wxSL_HORIZONTAL;
  if ( vertical )
  style |= wxSL_VERTICAL;
    if ( !suppressValue )
    style |= wxSL_LABELS;

    wxSlider * slider = new wxSlider( panel, widgetID, value, minimum, maximum,
    wxPoint( xOffset, yOffset ),
    wxSize( xSize, ySize ),
    style );
    this->wxWidget = slider;

      widgetSizer = gdlParent->GetSizer( );
      widgetSizer->Add( slider, 0, wxEXPAND | wxALL);
      }

//why overcast inherited ~GDLWidget????
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
//  GUIMutexLockerWidgetsT gdlMutexGuiEnterLeave;

  GDLWidget* gdlParent = GetWidget( parentID );
  wxObject *wxParentObject = gdlParent->GetWxWidget( );

  //  std::cout << "In Button: " << widgetID << " Parent: " << p << " xMode:" <<
  //gdlParent->GetExclusiveMode() << " " << value << std::endl;

  //update vValue
  delete(vValue);
  vValue = new DStringGDL( value );
  
  if ( gdlParent->IsMenuBar( ) )
  {
    //A button in a menubar is automatically a MENU
    //     cout << "MenuBar: " << widgetID << endl;
    wxMenuBar *menuBar = static_cast<wxMenuBar*> (wxParentObject);
    this->wxWidget = new wxMenu( );
    wxString valueWxString = wxString( value.c_str( ), wxConvUTF8 );
    menuBar->Append( static_cast<wxMenu*> (this->wxWidget), valueWxString );
    buttonType = MBAR;
  }
  else
  {
    //     cout << "Menu: " << widgetID << endl;
    if( gdlParent->IsButton())
    {
      if ( dynamic_cast<wxMenu*> (wxParentObject) == NULL ) e->Throw("Parent is of incorrect type.");

        wxMenu *menu = static_cast<wxMenu*> (wxParentObject);
        if (isMenu) {
          if (addSeparatorAbove) menu->AppendSeparator();
          this->wxWidget = new wxMenu( );
          wxString valueWxString = wxString( value.c_str( ), wxConvUTF8 );
          menu->AppendSubMenu( static_cast<wxMenu*> (this->wxWidget), valueWxString );
          buttonType = MENU;
        }
        else //only an entry.
        {
        // wxMenuItem
        // 	this->wxWidget = menu->Append( widgetID, wxString(value.c_str(), wxConvUTF8));
        // at destruction this seems to provoke: LIBDBUSMENU-GLIB-WARNING **: Trying to remove a child that doesn't believe we're it's parent.
        // on wxWidgets < 2.9.5
        wxString valueWxString = wxString( value.c_str( ), wxConvUTF8 );
        wxMenuItem* menuItem = new wxMenuItem( menu, widgetID, valueWxString );
        menu->Append( menuItem );
        this->wxWidget = menuItem;
        buttonType = ENTRY;
      // 	this->wxWidget = menu->Append( widgetID, wxString(value.c_str(), wxConvUTF8));
        }
      }
    else 
    {
      //       cout << "Button: " << widgetID << endl;
      wxPanel *panel = gdlParent->GetPanel( );

      wxSizer *boxSizer = gdlParent->GetSizer( );

      if ( gdlParent->GetExclusiveMode() == BGNORMAL) 
      {
        wxString valueWxString = wxString( value.c_str( ), wxConvUTF8 );
        wxButton *button = new wxButton( panel, widgetID, valueWxString,
        wxPoint( xOffset, yOffset ) , wxSize(xSize,ySize), alignment);
        boxSizer->Add( button, 0, wxEXPAND | wxALL);
        this->wxWidget = button;
        buttonType = NORMAL;
        // 	cout << "wxButton: " << widgetID << endl;
      }
      else if ( gdlParent->GetExclusiveMode() == BGEXCLUSIVE1ST) 
      {
        wxString valueWxString = wxString( value.c_str( ), wxConvUTF8 );
        wxRadioButton *radioButton = new wxRadioButton( panel, widgetID, valueWxString,
        wxPoint( xOffset, yOffset ), wxSize( xSize, ySize ), //wxDefaultSize,
        wxRB_GROUP );
        gdlParent->SetExclusiveMode( 1 );
        static_cast<GDLWidgetBase*> (gdlParent)->SetLastRadioSelection( widgetID );
        boxSizer->Add( radioButton, 0, wxEXPAND | wxALL);
        this->wxWidget = radioButton;
        // 	cout << "wxRadioButton1: " << widgetID << endl;
        buttonType = RADIO;
      } 
      else if ( gdlParent->GetExclusiveMode() == BGEXCLUSIVE) 
      {
        wxString valueWxString = wxString( value.c_str( ), wxConvUTF8 );
        wxRadioButton *radioButton = new wxRadioButton( panel, widgetID, valueWxString,
        wxPoint( xOffset, yOffset ), wxSize( xSize, ySize ) );
        boxSizer->Add( radioButton, 0, wxEXPAND | wxALL);
        this->wxWidget = radioButton;
        // 	cout << "wxRadioButton: " << widgetID << endl;
        buttonType = RADIO;
      } 
      else if ( gdlParent->GetExclusiveMode() == BGNONEXCLUSIVE) 
      {
        wxString valueWxString = wxString( value.c_str( ), wxConvUTF8 );
        wxCheckBox *checkBox = new wxCheckBox( panel, widgetID, valueWxString,
        wxPoint( xOffset, yOffset ), wxSize( xSize, ySize ) );
        boxSizer->Add( checkBox, 0, wxEXPAND | wxALL);
        this->wxWidget = checkBox;
        // 	cout << "wxCheckBox: " << widgetID << endl;
        buttonType = CHECKBOX;
      }
    }
  }

//  gdlMutexGuiEnterLeave.Leave( );
}

  void GDLWidgetButton::SetButtonWidgetLabelText( const DString& value_ )
  {
    //update vValue
    delete(vValue);
    vValue = new DStringGDL( value_ );
    
    GUIMutexLockerWidgetsT gdlMutexGuiEnterLeave;
    if ( this->wxWidget != NULL ) {
          (static_cast<wxButton*> (wxWidget))->SetLabel( wxString( value_.c_str( ), wxConvUTF8 ) ); //SetLabel, unless SetLabelText, interprets markup (<b></b> etc)
           static_cast<wxButton*> (wxWidget)->Refresh();  //not useful
      } else std::cerr << "Null widget in GDLWidgetButton::SetButtonWidgetLabelText(), please report!" << std::endl;
  }
  
GDLWidgetList::GDLWidgetList( WidgetIDT p, EnvT* e, BaseGDL *value, DLong style )
    : GDLWidget( p, e, true, value)
{
//  GUIMutexLockerWidgetsT gdlMutexGuiEnterLeave;

  GDLWidget* gdlParent = GetWidget( p );

  wxPanel *panel = gdlParent->GetPanel( );

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
  wxListBox * list = new wxListBox( panel, widgetID, wxPoint( xOffset, yOffset ),
  wxSize( xSize, ySize ),
  choices, style );
  this->wxWidget = list;

  wxSizer * boxSizer = gdlParent->GetSizer( );
  boxSizer->Add( list, 0, wxEXPAND | wxALL);
}

GDLWidgetDropList::GDLWidgetDropList( WidgetIDT p, EnvT* e, BaseGDL *value,
const DString& title_, DLong style_ )
: GDLWidget( p, e, true, static_cast<DStringGDL*> (value->Convert2( GDL_STRING, BaseGDL::CONVERT )) )
, title( title_ )
    , style( style_)
{
  CreateWidgetPanel( );
}

void GDLWidgetDropList::OnShow()  
{
  //  std::cout << "In DropList: " << widgetID << " " << p << std::endl;
//  GUIMutexLockerWidgetsT gdlMutexGuiEnterLeave;

  GDLWidget* gdlParent = GetWidget( parentID );

  DStringGDL* val = static_cast<DStringGDL*> (vValue);
  DLong n = val->N_Elements( );
  wxArrayString choices; // = new wxString[n];
  for ( SizeT i = 0; i < n; ++i )
  choices.Add( wxString( (*val)[i].c_str( ), wxConvUTF8 ) );

  wxChoice * droplist = new wxChoice( widgetPanel, widgetID, 
  wxPoint( xOffset, yOffset ), wxSize( xSize, ySize ), choices, style );
  droplist->SetSelection(0);
  this->wxWidget = droplist;
    }

GDLWidgetComboBox::GDLWidgetComboBox( WidgetIDT p, EnvT* e, BaseGDL *value,
const DString& title_, DLong style_ )
: GDLWidget( p, e, true, static_cast<DStringGDL*> (value->Convert2( GDL_STRING, BaseGDL::CONVERT )) )
, title( title_ )
, style( style_)
{
  CreateWidgetPanel( );
}

void GDLWidgetComboBox::OnShow()  
{
  //  std::cout << "In ComboBox: " << widgetID << " " << p << std::endl;
//  GUIMutexLockerWidgetsT gdlMutexGuiEnterLeave;

  GDLWidget* gdlParent = GetWidget( parentID );

  DStringGDL* val = static_cast<DStringGDL*> (vValue);
  DLong n = val->N_Elements( );
  wxArrayString choices; // = new wxString[n];
  for ( SizeT i = 0; i < n; ++i )
  choices.Add( wxString( (*val)[i].c_str( ), wxConvUTF8 ) );

  wxString val0WxString = wxString( (*val)[0].c_str( ), wxConvUTF8 );
  wxComboBox * combo = new wxComboBox( widgetPanel, widgetID, val0WxString,
  wxPoint( xOffset, yOffset ), wxSize( xSize, ySize ), choices, style );
  this->wxWidget = combo;
    }

GDLWidgetText::GDLWidgetText( WidgetIDT p, EnvT* e, DStringGDL* valueStr, bool noNewLine_,
bool editable_ )
: GDLWidget( p, e, true, valueStr )
, noNewLine( noNewLine_ )
, editable(editable_)
{
  DString value = "";
  maxlinelength = 0;
  nlines=0;
  if (ySize<2) noNewLine=TRUE;
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

  CreateWidgetPanel( );

}

void GDLWidgetText::OnShow()
{
  //  std::cout << "In Text: " << widgetID << " " << p << std::endl;
//  GUIMutexLockerWidgetsT gdlMutexGuiEnterLeave;
  //widget text size is in LINES in Y and CHARACTERS in X
  wxSize fontSize = wxSystemSettings::GetFont( wxSYS_SYSTEM_FONT ).GetPixelSize();
  wxSize widgetSize = wxDefaultSize;
  if ( xSize != widgetSize.x )
  widgetSize.x = xSize * fontSize.x;
  else widgetSize.x = maxlinelength * fontSize.x;
  if ( widgetSize.x < 100 ) widgetSize.x=20* fontSize.x; //default
  if ( ySize != widgetSize.y )  widgetSize.y = ySize * fontSize.y; 
  else widgetSize.y = fontSize.y; //instead of nlines*fontSize.y to be compliant with *DL 

  wxString valueWxString = wxString( lastValue.c_str( ), wxConvUTF8 );
  long textAlignment=wxTE_LEFT;
  switch(alignment){
    case wxALIGN_RIGHT:
      textAlignment=wxTE_RIGHT; break;
    case wxALIGN_CENTER:
      textAlignment=wxTE_CENTRE; break;
  }
  DLong style = wxTE_NOHIDESEL|wxTE_PROCESS_ENTER|textAlignment;

 if ( !editable ) style |= wxTE_READONLY; //no, because *DL provides READONLY and still give events.
// this permits to a procedure (cw_field.pro for example) to filter the editing of text fields.
// so wxTextCtrl::OnChar is overwritten in gdlwidgeteventhandler.cpp just for this reason.
  if ( nlines > 1 ) style |= wxTE_MULTILINE;
  wxTextCtrl * text = new wxTextCtrl( widgetPanel, widgetID, valueWxString,
  wxPoint( xOffset, yOffset ), widgetSize, style );
  text->SetInsertionPoint(0);
  text->SetSelection(0,0);
  this->wxWidget = text;
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

  GUIMutexLockerWidgetsT gdlMutexGuiEnterLeave;

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
  GUIMutexLockerWidgetsT gdlMutexGuiEnterLeave;
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
  GUIMutexLockerWidgetsT gdlMutexGuiEnterLeave;
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

GDLWidgetLabel::GDLWidgetLabel( WidgetIDT p, EnvT* e, const DString& value_ )
: GDLWidget( p, e )
, value(value_)
{
  CreateWidgetPanel( );
}

void GDLWidgetLabel::OnShow()
{
//  GUIMutexLockerWidgetsT gdlMutexGuiEnterLeave;
  //  std::cout << "In Label: " << widgetID << " " << p << std::endl;

  GDLWidget* gdlParent = GetWidget( parentID );

  wxString valueWxString = wxString( value.c_str( ), wxConvUTF8 );

  wxStaticText* label = new wxStaticText( widgetPanel, widgetID, valueWxString,
  wxPoint( xOffset, yOffset ), wxSize( xSize, ySize ), alignment|wxST_NO_AUTORESIZE );
  this->wxWidget = label;
  }

void GDLWidgetLabel::SetLabelValue( const DString& value_)
{
    value = value_;
    //update vValue
    delete(vValue);
    vValue = new DStringGDL( value );
    
    GUIMutexLockerWidgetsT gdlMutexGuiEnterLeave;
    wxString valueWxString = wxString( value_.c_str( ), wxConvUTF8 );
    if ( this->wxWidget != NULL ) {
      wxWindowID id = static_cast<wxStaticText*> (wxWidget)->GetId( );
      static_cast<wxStaticText*> (wxWidget)->SetLabel( valueWxString ); //SetLabel, unless SetLabelText, interprets markup (<b></b> etc)
      static_cast<wxStaticText*> (wxWidget)->Refresh( ); //not useful
  }    else std::cerr << "Null widget in GDLWidgetLabel::SetLabelValue(), please report!" << std::endl;
}

// GDL widgets =====================================================
// GDLFrame ========================================================
GDLFrame::GDLFrame( GDLWidgetBase* gdlOwner_, wxWindow* parent, wxWindowID id, const wxString& title )
: wxFrame( parent, id, title )
, lastShowRequest( false )
, gdlOwner( gdlOwner_)
{
  m_gdlFrameOwnerMutexP = new wxMutex( );
}

GDLFrame::~GDLFrame()
{ 
#ifdef GDL_DEBUG_WIDGETS
    std::cout << "~GDLFrame: " << this << std::endl;
    std::cout << "This IsMainThread: " << wxIsMainThread() << std::endl;
#endif  
 
  m_gdlFrameOwnerMutexP->Lock( );
  if( gdlOwner != NULL)
  {
    gdlOwner->NullWxWidget( );
    gdlOwner->Destroy( ); // send delete request to GDL owner
    m_gdlFrameOwnerMutexP->Unlock( );
  }
  else
  {
    m_gdlFrameOwnerMutexP->Unlock( );
    delete m_gdlFrameOwnerMutexP;
  }
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
  pstreamIx = GraphicsDevice::GetGUIDevice( )->WAdd( );
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

void GDLDrawPanel::Update()
{
  //   cout << "in GDLDrawPanel::Update()" << endl;
  SendPaintEvent( );
  //   wxClientDC dc( this);
  //   dc.SetDeviceClippingRegion( GetUpdateRegion() );
  //   GUIMutexLockerT gdlMutexGuiEnterLeave;
  //   dc.Blit( 0, 0, drawSize.x, drawSize.y, m_dc, 0, 0 );
  //   wxPanel::Update();
  //   gdlMutexGuiEnterLeave.Leave();
}




// GDLGUIThread ==================================================
wxThread::ExitCode GDLGUIThread::Entry()
{
  try {
    wxTheApp->OnRun( );
    // Calls GDLApp::OnRun()
  }
  catch( exception& e)
  {
    cout << "GDLGUIThread::Entry(): Exception caught: " << e.what( ) << endl;
  }
  catch( ...)
  {
    cout << "GDLGUIThread::Entry(): Unknown exception caught." << endl;
  }

  return NULL;
}


void GDLGUIThread::OnExit()
{
  // Called by GDLApp::OnExit() in gdlwidget.cpp
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "In guiThread::OnExit()." << std::endl;
  std::cout << "IsMainThread: " << wxIsMainThread( ) << std::endl;
#endif
  GDLGUIThread::gdlGUIThread = NULL;
}


// void GDLGUIThread::Exit()
// {
//   // Called by GDLApp::OnExit() in gdlwidget.cpp
// #ifdef GDL_DEBUG_WIDGETS
//   std::cout << "In GDLGUIThread::Exit()." << std::endl;
// #endif
//   delete this;
// }

GDLGUIThread::~GDLGUIThread()
{
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "In ~GDLGUIThread(). exited." << std::endl;
#endif
}



// GDLApp =================================================
int GDLApp::OnRun()
{
  // Called by guiThread::Entry()

  //   std::cout << " In OnRun()" << std::endl;

  int exitcode = wxApp::OnRun( );
  // Note: Calls wxAppBase::OnRun() in appcmn.cpp (wxWidgets)
  // MainLoop() etc

  //   if (exitcode!=0)
  return exitcode;
}

int GDLApp::OnExit()
{
  std::cout << " In GDLApp::OnExit()" << std::endl;

  // Defined in guiThread::OnExit() in gdlwidget.cpp
  //  std::cout << "Exiting thread (GDLApp::OnExit): " << thread << std::endl;
  if (GDLGUIThread::gdlGUIThread != NULL)
  {
    //      delete gdlGUIThread;
    GDLGUIThread::gdlGUIThread = NULL;
  }

  return 0;
}


#endif

