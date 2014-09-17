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
  if (row==0 && col==0) cerr<<"invalid null rows and cols in base widget, please report!"<<endl;
  if ( frameBox == 0 ) {
    if ( row == 0  && col == 1) {sizer = new wxBoxSizer( wxVERTICAL ); }
    else if ( row == 0 && col > 1) {sizer = new wxFlexGridSizer( 0, col, 0, 0 );}
    else if ( col == 0 && row == 1) {sizer = new wxBoxSizer( wxHORIZONTAL );}
    else if ( col == 0 && row > 1) {sizer = new wxFlexGridSizer( row, 0, 0, 0 );}
    else sizer = new wxFlexGridSizer( row, col, 0, 0 ); //which should not happen.
    } else {
    if ( row == 0 ) {
      wxStaticBox *box = new wxStaticBox( panel, wxID_ANY, _T( "" ) );
      sizer = new wxStaticBoxSizer( box, wxVERTICAL );
    } else if ( row != 0 && col == 0 ) {
      wxStaticBox *box = new wxStaticBox( panel, wxID_ANY, _T( "" ) );
      sizer = new wxStaticBoxSizer( box, wxHORIZONTAL );
    } else {
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
    }

    // refresh (if tlb still exists (handler might have destroyed it))
    // even on unhandled event as it might have been rewritten by a handler
    // use 'old' tlb as id might no longer exist
    GDLWidget *tlw = GetWidget( tlb );
    if( tlw != NULL)
    {
      assert( dynamic_cast<GDLFrame*> (tlw->wxWidget) != NULL );
      // Pause 50 millisecs then refresh widget 
      //       wxMilliSleep( 50); // (why?)
      GUIMutexLockerWidgetsT gdlMutexGuiEnterLeave;
      static_cast<GDLFrame*> (tlw->wxWidget)->Refresh( );
      gdlMutexGuiEnterLeave.Leave( );
    }
  }
  return 0;
}

void GDLWidget::PushEvent( WidgetIDT baseWidgetID, DStructGDL* ev) 
{
  // Get XmanagerActiveCommand status
  GDLWidget *baseWidget = GDLWidget::GetWidget( baseWidgetID );
  bool xmanActCom = baseWidget->GetXmanagerActiveCommand( );
  if( !xmanActCom)
  {
    //     wxMessageOutputDebug().Printf(_T("eventQueue.Push: %d\n",baseWidgetID);
    eventQueue.Push( ev );
  }
  else
  {
    //     wxMessageOutputDebug().Printf(_T("readLineEventQueue.Push: %d\n",baseWidgetID);
    readlineEventQueue.Push( ev );
  }
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
//   , buttonSet(false)
, exclusiveMode( 0 )
, topWidgetSizer( NULL )
, widgetSizer( NULL )
, widgetPanel( NULL )
, managed( false )
, map( map_ )
  , eventFlags(eventFlags_)
{
  if ( e != NULL )
    SetCommonKeywords( e );

  widgetID = wxWindow::NewControlId( );

    if( parentID != GDLWidget::NullID)
    {
    GDLWidget* gdlParent = GetWidget( parentID );
        if( gdlParent->IsBase())
        {
      GDLWidgetBase* base = static_cast<GDLWidgetBase*> (gdlParent);
      //       assert( base != NULL); // should be already checked elsewhere
      base->AddChild( widgetID );
        }
        else
        {
      GDLWidgetBase* base = GetBaseWidget( parentID );
      if ( base != NULL )
        base->AddChild( widgetID );
    }
  }

  widgetList.insert( widgetList.end( ), std::pair<WidgetIDT, GDLWidget*>(widgetID, this) );
#ifdef GDL_DEBUG_WIDGETS
  //     GUIMutexLockerWidgetsT gdlMutexGuiEnterLeave;
  wxMessageOutputDebug( ).Printf( _T( "inserted: ID: %d  parentID: %d\n", widgetID, parentID );
  //     std::cout << "inserted: ID: " << widgetID << "  parentID: " << parentID << "   uname: " << uName << std::endl;
#endif
}


void GDLWidget::CreateWidgetPanel()
{
  GDLWidget* gdlParent = GetWidget( parentID );

  wxPanel *parentPanel = gdlParent->GetPanel( );

  wxPanel *panel = new wxPanel( parentPanel, wxID_ANY
  , wxDefaultPosition
  , wxDefaultSize
  //   , wxBORDER_SIMPLE 
  , wxNO_BORDER
  );
  widgetPanel = panel;

  wxSizer *boxSizer = gdlParent->GetSizer( );
  boxSizer->Add( panel, 0, wxEXPAND | wxALL, 1);
}

void GDLWidget::SetSizeHints()
{
  assert( parentID == NullID );
  wxMessageOutputDebug( ).Printf( _T( "GDLWidget:SetSizeHints: %d\n" ), this->widgetID );
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
    wxMessageOutputDebug( ).Printf( _T( "GDLWidget:Realize: %d\n", this->widgetID );
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
    wxMessageOutputDebug( ).Printf( _T( "GDLWidget:Realize TLB of: %d\n", this->widgetID );
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
, scrolled(false)
, nrows(0)
, ncols(0)
{
  //  std::cout << "In GDLWidgetBase::GDLWidgetBase: " << widgetID << std::endl

  xmanActCom = false;
  wxWindow *wxParent = NULL;
  
  //get immediately rid of scroll sizes in case of scroll or not... Here is the logic:
  if (x_scroll_size >0 || y_scroll_size >0) scroll=TRUE;
  if (scroll) {
    x_scroll_size=(x_scroll_size<100)?100:x_scroll_size;
    y_scroll_size=(y_scroll_size<100)?100:y_scroll_size;
  } 
  //ncols, nrows
  ncols=col;
  nrows=row;
  scrolled=scroll;
  
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

    GUIMutexLockerWidgetsT gdlMutexGuiEnterLeave;

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

    gdlFrame->SetSize( xSize, ySize );

    if( mbarID != 0)
    {
        GDLWidgetMBar* mBar = new GDLWidgetMBar( widgetID );
        // already called in constructor:
        // mbarID = GDLWidget::NewWidget( mBar);
        mbarID = mBar->WidgetID( );
        mBarIDInOut = mbarID;

        gdlFrame->SetMenuBar( static_cast<wxMenuBar*> (mBar->GetWxWidget( )) );

        gdlFrame->SetSize( xSize, ySize );
      }

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

    GUIMutexLockerWidgetsT gdlMutexGuiEnterLeave;

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
      if ( scroll ) {
        wxScrolledWindow *scrw;
        scrw = new wxScrolledWindow( wxParent, wxID_ANY ,  wxDefaultPosition, wxSize(x_scroll_size, y_scroll_size ), wxBORDER_SUNKEN);
        scrw->SetScrollbars( 20, 20, 50, 50 );
        wxSizer * sizer = GetNewSizer( ncols, nrows, frame, scrw );
        scrw->SetSizer( sizer );
        widgetPanel = scrw;
        wxWidget = scrw; 
        widgetSizer = sizer;
        parentSizer->Add( scrw, 0, wxEXPAND|wxALL, 10);
       
      } else {
        wxPanel *panel;
        panel = new wxPanel( wxParent, wxID_ANY );
        widgetPanel = panel;
        wxWidget = panel;
        wxSizer * sizer = GetNewSizer( ncols, nrows, frame, panel );
        widgetSizer = sizer;
        panel->SetSizer( sizer );
        parentSizer->Add( panel, 0, wxEXPAND|wxALL, 1);
       }

      // If map is true check for parent map value
        if ( mapWid )
        mapWid = gdlParent->GetMap( );
        this->SetMap( mapWid );
     
//               parentSizer->Layout();
//         
//               wxPanel* parentPanel = gdlParent->GetPanel();
//         
//               parentSizer->Fit( parentPanel);
        }
  }
}

GDLWidgetBase::~GDLWidgetBase()
{
  // call KILL_NOTIFY procedures
  this->OnKill( );

  // delete all children
  for( SizeT i=0; i<children.size(); i++) 
  {
    GDLWidget* child = GetWidget( children[i] );
    delete child;
  }

  GUIMutexLockerWidgetsT gdlMutexGuiEnterLeave;

  // Close widget frame (might be already closed)
  if( this->parentID == 0)
  {
    assert( m_gdlFrameOwnerMutexP != NULL );
    m_gdlFrameOwnerMutexP->Lock( );
    if( this->wxWidget != NULL)
    {
      static_cast<GDLFrame*> (this->wxWidget)->NullGDLOnwer( );
      // this seems to provoke: LIBDBUSMENU-GLIB-WARNING **: Trying to remove a child that doesn't believe we're it's parent.
      // on wxWidgets < 2.9.5
      static_cast<GDLFrame*> (this->wxWidget)->Destroy( ); // instead of delete operator

      m_gdlFrameOwnerMutexP->Unlock( );
    }
    else
    {
      m_gdlFrameOwnerMutexP->Unlock( );
      delete m_gdlFrameOwnerMutexP;
    }
  }

  // remove all widgets still in the queue for current TLB
  eventQueue.Purge( widgetID );
  readlineEventQueue.Purge( widgetID );
}


// called form event handling thread
// sends an destroy event for itself
void GDLWidgetBase::Destroy()
{
  assert( parentID == NullID );

  // create GDL event struct
  DStructGDL* ev = new DStructGDL( "*WIDGET_MESSAGE*" );
  ev->InitTag( "ID", DLongGDL( widgetID ) );
  ev->InitTag( "TOP", DLongGDL( widgetID ) );
  ev->InitTag( "HANDLER", DLongGDL( 0 ) );
  ev->InitTag( "MESSAGE", DLongGDL( 0 ) );

  readlineEventQueue.PushFront( ev ); // push front (will be handled next)
}

void GDLWidgetBase::FitInside()
{
  //this is supposed to make appear the contents of a panel created with /row,/scroll but no avail!
  static_cast<wxScrolledWindow*>(wxWidget)->Fit();
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
: GDLWidget( p, e)
{
  GUIMutexLockerWidgetsT gdlMutexGuiEnterLeave;

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
          widgetSizer->Add( notebook, 0, wxEXPAND | wxALL, 1);
          }

GDLWidgetTab::~GDLWidgetTab()
{}



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
DStringGDL* font_,
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
, font( font_ )
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
}

GDLWidgetTable::~GDLWidgetTable()
{
  GDLDelete( alignment );
  GDLDelete( amPm );
  GDLDelete( backgroundColor );
  GDLDelete( foregroundColor );
  GDLDelete( columnLabels );
  GDLDelete( columnWidth );
  GDLDelete( daysOfWeek );
  GDLDelete( font );
  GDLDelete( format );
  GDLDelete( month );
  GDLDelete( rowHeights );
  GDLDelete( rowLabels );
}

void GDLWidgetTable::OnShow()
{
  GUIMutexLockerWidgetsT gdlMutexGuiEnterLeave;

  GDLWidget* gdlParent = GetWidget( parentID );

  long style = wxWANTS_CHARS;
  wxGrid *grid = new wxGrid( widgetPanel, widgetID,
  wxPoint( xOffset, yOffset ), wxSize( xSize, ySize ), style );
  this->wxWidget = grid;

  widgetPanel->SetClientSize( grid->GetSize( ) );

  wxSizer * boxSizer = gdlParent->GetSizer( );
  boxSizer->Layout( );
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

GDLWidgetTree::~GDLWidgetTree()
{
  GDLDelete( bitmap );
}

void GDLWidgetTree::OnShow()
{
  GUIMutexLockerWidgetsT gdlMutexGuiEnterLeave;

  GDLWidget* gdlParent = GetWidget( parentID );

  wxSizer *boxSizer = gdlParent->GetSizer( );
  boxSizer->Layout( );

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
  GUIMutexLockerWidgetsT gdlMutexGuiEnterLeave;

  GDLWidget* gdlParent = GetWidget( p );
  wxWindow *wxParent = static_cast<wxWindow*> (gdlParent->GetWxWidget( ));

  wxPanel *panel = gdlParent->GetPanel( );

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
      widgetSizer->Add( slider, 0, wxEXPAND | wxALL, 1);
      }

GDLWidgetSlider::~GDLWidgetSlider()
{}

  

GDLWidgetButton::GDLWidgetButton( WidgetIDT p, EnvT* e,
const DString& value )
: GDLWidget( p, e )
, buttonType( UNDEFINED )
, buttonState( false)
{
  GUIMutexLockerWidgetsT gdlMutexGuiEnterLeave;

  GDLWidget* gdlParent = GetWidget( parentID );
  wxObject *wxParentObject = gdlParent->GetWxWidget( );

  //  std::cout << "In Button: " << widgetID << " Parent: " << p << " xMode:" <<
  //gdlParent->GetExclusiveMode() << " " << value << std::endl;

  //update vValue
  delete(vValue);
  vValue = new DStringGDL( value );

  if ( gdlParent->IsMenuBar( ) )
  {
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
      assert( dynamic_cast<wxMenu*> (wxParentObject) != NULL );

      wxMenu *menu = static_cast<wxMenu*> (wxParentObject);
      // wxMenuItem
      // 	this->wxWidget = menu->Append( widgetID, wxString(value.c_str(), wxConvUTF8));
      // at destruction this seems to provoke: LIBDBUSMENU-GLIB-WARNING **: Trying to remove a child that doesn't believe we're it's parent.
      // on wxWidgets < 2.9.5
      wxString valueWxString = wxString( value.c_str( ), wxConvUTF8 );
      wxMenuItem* menuItem = new wxMenuItem( menu, widgetID, valueWxString );
      menu->Append( menuItem );
      this->wxWidget = menuItem;
      buttonType = MENU;
      // 	this->wxWidget = menu->Append( widgetID, wxString(value.c_str(), wxConvUTF8));
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
        wxPoint( xOffset, yOffset ) );
        boxSizer->Add( button, 0, wxEXPAND | wxALL, 1);
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
        boxSizer->Add( radioButton, 0, wxEXPAND | wxALL, 1);
        this->wxWidget = radioButton;
        // 	cout << "wxRadioButton1: " << widgetID << endl;
        buttonType = RADIO;
      } 
      else if ( gdlParent->GetExclusiveMode() == BGEXCLUSIVE) 
      {
        wxString valueWxString = wxString( value.c_str( ), wxConvUTF8 );
        wxRadioButton *radioButton = new wxRadioButton( panel, widgetID, valueWxString,
        wxPoint( xOffset, yOffset ), wxSize( xSize, ySize ) );
        boxSizer->Add( radioButton, 0, wxEXPAND | wxALL, 1);
        this->wxWidget = radioButton;
        // 	cout << "wxRadioButton: " << widgetID << endl;
        buttonType = RADIO;
      } 
      else if ( gdlParent->GetExclusiveMode() == BGNONEXCLUSIVE) 
      {
        wxString valueWxString = wxString( value.c_str( ), wxConvUTF8 );
        wxCheckBox *checkBox = new wxCheckBox( panel, widgetID, valueWxString,
        wxPoint( xOffset, yOffset ), wxSize( xSize, ySize ) );
        boxSizer->Add( checkBox, 0, wxEXPAND | wxALL, 1);
        this->wxWidget = checkBox;
        // 	cout << "wxCheckBox: " << widgetID << endl;
        buttonType = CHECKBOX;
        }

      boxSizer->Layout( ); // maybe not necessary
      boxSizer->Fit( panel );
      //       boxSizer->Layout();

      //       wxWindow *wxParent = dynamic_cast< wxWindow*>( wxParentObject);
      //       if ( wxParent != NULL) {
      // 	//      std::cout << "SetSizeHints: " << wxParent << std::endl;
      // 	boxSizer->SetSizeHints( wxParent);
      //       }
    } // GetMap()
  }

  gdlMutexGuiEnterLeave.Leave( );
}

  void GDLWidgetButton::SetButtonWidgetLabelText( const DString& value_ )
  {
    //update vValue
    delete(vValue);
    vValue = new DStringGDL( value_ );
    
    GUIMutexLockerWidgetsT gdlMutexGuiEnterLeave;
    wxString valueWxString = wxString( value_.c_str( ), wxConvUTF8 );
    if ( this->wxWidget != NULL ) {
      wxWindowID id = static_cast<wxButton*> (wxWidget)->GetId( );
      static_cast<wxButton*> (wxWidget)->SetLabel( valueWxString ); //SetLabel, unless SetLabelText, interprets markup (<b></b> etc)
    } else std::cerr << "Null widget in GDLWidgetLabel::SetButtonWidgetLabelText(), please report!" << std::endl;
  }
  
GDLWidgetList::GDLWidgetList( WidgetIDT p, EnvT* e, BaseGDL *value, DLong style )
    : GDLWidget( p, e, true, value)
{
  GUIMutexLockerWidgetsT gdlMutexGuiEnterLeave;

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

  wxListBox * list = new wxListBox( panel, widgetID, wxPoint( xOffset, yOffset ),
  wxSize( xSize, ySize ),
  choices, style );
  this->wxWidget = list;

    wxSizer * boxSizer = gdlParent->GetSizer( );
    boxSizer->Add( list, 0, wxEXPAND | wxALL, 1);

    boxSizer->Layout( ); // maybe not necessary
    boxSizer->Fit( panel );
    }

//GDLWidgetDropList::GDLWidgetDropList( WidgetIDT p, BaseGDL *uV, DStringGDL *value,
GDLWidgetDropList::GDLWidgetDropList( WidgetIDT p, EnvT* e, BaseGDL *value,
const DString& title_, DLong style_ )
: GDLWidget( p, e, true, static_cast<DStringGDL*> (value->Convert2( GDL_STRING, BaseGDL::CONVERT )) )
, title( title_ )
    , style( style_)
{
  //   if( vValue->Type() != GDL_STRING)
  //   {
  //       vValue = static_cast<DStringGDL*>( vValue->Convert2(GDL_STRING,BaseGDL::CONVERT));
  //   }

  CreateWidgetPanel( );
}

void GDLWidgetDropList::OnShow()  
{
  //  std::cout << "In DropList: " << widgetID << " " << p << std::endl;
  GUIMutexLockerWidgetsT gdlMutexGuiEnterLeave;

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

    widgetPanel->SetClientSize( combo->GetSize( ) );

    wxSizer * boxSizer = gdlParent->GetSizer( );
    boxSizer->Layout( );
    }


GDLWidgetText::GDLWidgetText( WidgetIDT p, EnvT* e, DStringGDL* valueStr, bool noNewLine_,
bool editable_ )
: GDLWidget( p, e, true, valueStr )
, noNewLine( noNewLine_ )
, editable(editable_)
{
  DString value = "";
  if( vValue != NULL)
  {
      for( int i=0; i<vValue->N_Elements(); ++i)
      {
      value += (*static_cast<DStringGDL*> (vValue))[i];
      if ( !noNewLine && (i + 1) < vValue->N_Elements( ) )
      value += '\n';
      }
  }
  lastValue = value;

  CreateWidgetPanel( );
  //   GDLWidget* gdlParent = GetWidget( parentID);
  // 
  //   wxPanel *parentPanel = gdlParent->GetPanel();
  // 
  //   wxPanel *panel = new wxPanel( parentPanel, wxID_ANY
  //   , wxDefaultPosition
  //   , wxDefaultSize
  // //   , wxBORDER_SIMPLE 
  //   );
  //   widgetPanel = panel;
  //   
  //   wxSizer *boxSizer = gdlParent->GetSizer();
  //   boxSizer->Add( panel, 0, wxEXPAND | wxALL, 1);  
}

void GDLWidgetText::OnShow()
{
  //  std::cout << "In Text: " << widgetID << " " << p << std::endl;
  GUIMutexLockerWidgetsT gdlMutexGuiEnterLeave;

  GDLWidget* gdlParent = GetWidget( parentID );

  wxSize fontSize = wxNORMAL_FONT->GetPixelSize( );
  wxSize widgetSize = wxDefaultSize;
  if ( xSize != widgetSize.x )
  widgetSize.x = xSize * fontSize.x;
    if ( ySize != widgetSize.y )
    widgetSize.y = ySize * fontSize.y;

    DLong style = wxTE_NOHIDESEL;
      if ( !editable )
      style |= wxTE_READONLY;
        if ( ySize > 1 )
        style |= wxTE_MULTILINE;

        wxString valueWxString = wxString( lastValue.c_str( ), wxConvUTF8 );
        wxTextCtrl * text = new wxTextCtrl( widgetPanel, widgetID, valueWxString,
        wxPoint( xOffset, yOffset ), widgetSize, style );
        this->wxWidget = text;

          //  widgetPanel->SetClientSize(text->GetSize());

//   wxSizer *parentSizer = gdlParent->GetSizer();
//   parentSizer->Layout(); // maybe not necessary
//   parentSizer->Fit( gdlParent->GetPanel());
          }


void GDLWidgetText::SetTextValue( DStringGDL* valueStr, bool noNewLine)
{
  delete vValue;
  vValue = valueStr;
  DString value = "";
    for( int i=0; i<valueStr->N_Elements(); ++i)
    {
    value += (*valueStr)[ i];
    if ( !noNewLine && (i + 1) < valueStr->N_Elements( ) )
    value += '\n';
    }
  lastValue = value;

  GUIMutexLockerWidgetsT gdlMutexGuiEnterLeave;

  wxString valueWxString = wxString( value.c_str( ), wxConvUTF8 );
  if ( this->wxWidget != NULL ) {
    static_cast<wxTextCtrl*> (wxWidget)->ChangeValue( valueWxString ); //by contrast with SetValue, does not generate an EVENT -- IDL does not either.    
    //    static_cast<wxTextCtrl*>(wxWidget)->Refresh();  //not useful
  }  else std::cerr << "Null widget in GDLWidgetText::SetTextValue(), please report!" << std::endl;
  }



GDLWidgetLabel::GDLWidgetLabel( WidgetIDT p, EnvT* e, const DString& value_ )
: GDLWidget( p, e )
, value(value_)
{
  CreateWidgetPanel( );
}

void GDLWidgetLabel::OnShow()
{
  GUIMutexLockerWidgetsT gdlMutexGuiEnterLeave;
  //  std::cout << "In Label: " << widgetID << " " << p << std::endl;

  GDLWidget* gdlParent = GetWidget( parentID );

  //     wxSize fontSize = wxNORMAL_FONT->GetPixelSize(); 
  wxString valueWxString = wxString( value.c_str( ), wxConvUTF8 );

  wxStaticText* label = new wxStaticText( widgetPanel, widgetID, valueWxString,
  wxPoint( xOffset, yOffset ), wxSize( xSize, ySize ), wxALIGN_LEFT | wxST_NO_AUTORESIZE );
  this->wxWidget = label;

  widgetPanel->SetClientSize( label->GetSize( ) );

  wxSizer * boxSizer = gdlParent->GetSizer( );
  boxSizer->Layout( );
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
//      static_cast<wxStaticText*> (wxWidget)->Refresh( ); //not useful
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
  //   std::cout << "~GDLFrame: " << this << std::endl;
  //   std::cout << "This IsMainThread: " << wxIsMainThread() << std::endl;

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
GDLDrawPanel::GDLDrawPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name )
: wxPanel( parent, id, pos, size, style, name )
, pstreamIx( -1 )
, pstreamP( NULL )
, m_dc( NULL)
{
  // initialization of stream is done in GDLWidgetDraw::OnRealize()
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
  //   std::cout << "~GDLDrawPanel: " << this << std::endl;
  //   std::cout << "This IsMainThread: " << wxIsMainThread() << std::endl;
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

