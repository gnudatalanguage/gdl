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

#include <memory> 
#include <wx/notebook.h>

#include "basegdl.hpp"
#include "dstructgdl.hpp"

#include "dinterpreter.hpp"

#ifdef HAVE_LIBWXWIDGETS

#include "gdlwxstream.hpp"

#include "devicex.hpp"

#include "gdlwidget.hpp"

#include "widget.hpp"
#include "graphicsdevice.hpp"


//#define GDL_DEBUG_WIDGETS



const WidgetIDT GDLWidget::NullID = 0;

// instantiation
// WidgetIDT	GDLWidget::widgetIx;
WidgetListT	GDLWidget::widgetList;

// VarListT                    eventVarList;
GDLEventQueue	GDLWidget::eventQueue; // the event queue
GDLEventQueue	GDLWidget::readlineEventQueue; // for process at command line level

void GDLEventQueue::Purge()
{
  wxMutexLocker lock( mutex);
  for( SizeT i=0; i<dq.size();++i)
    delete dq[i];
  dq.clear();
//   isEmpty = true;
}


wxSizer* GetNewSizer( DLong col, DLong row, DLong frameBox, wxPanel *panel) 
{
  wxSizer* sizer = NULL;
  if ( frameBox == 0) 
  {
    if ( row == 0) {
      sizer = new wxBoxSizer( wxVERTICAL);
    } else if ( row != 0 && col == 0) {
      sizer = new wxBoxSizer( wxHORIZONTAL);
    } else {
      sizer = new wxFlexGridSizer( row, col, 0, 0);
    }
  } 
  else 
  {
    if ( row == 0) {
      wxStaticBox *box = new wxStaticBox( panel, wxID_ANY, _T(""));
      sizer = new wxStaticBoxSizer( box, wxVERTICAL);
    } else if ( row != 0 && col == 0) {
      wxStaticBox *box = new wxStaticBox( panel, wxID_ANY, _T(""));
      sizer = new wxStaticBoxSizer( box, wxHORIZONTAL);
    } else {
      sizer = new wxFlexGridSizer( row, col, 0, 0);
    }
  }
  return sizer;
}

GDLWindow::GDLWindow(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
: wxWindow( parent, id, pos, size, style, name)
, pstreamIx(-1)
, pstreamP( NULL)
, m_dc( NULL)
{
  pstreamIx = GraphicsDevice::GetGUIDevice()->WAdd();
  if( pstreamIx == -1)
    throw GDLException("Failed to allocate GUI stream.");

  drawSize = size;
  bool success = GraphicsDevice::GetGUIDevice()->GUIOpen( pstreamIx, drawSize.x, drawSize.y);  
  if( !success)
  {
    throw GDLException("Failed to open GUI stream: "+i2s(pstreamIx));
  }
  pstreamP = static_cast<GDLWXStream*>(GraphicsDevice::GetGUIDevice()->GetStreamAt( pstreamIx));
  pstreamP->SetGDLWindow( this);
  
  m_dc = pstreamP->GetDC();
}

GDLWindow::~GDLWindow()
{
  
  std::cout << "~GDLWindow: " << this << std::endl;
  std::cout << "This IsMainThread: " << wxIsMainThread() << std::endl;
  if( pstreamP != NULL)
      pstreamP->SetValid(false);
}

void GDLWindow::Update()
{
  wxClientDC dc( this);
  dc.SetDeviceClippingRegion( GetUpdateRegion() );
  GUIMutexLockerT gdlMutexGuiEnterLeave;
  dc.Blit( 0, 0, drawSize.x, drawSize.y, m_dc, 0, 0 );
  gdlMutexGuiEnterLeave.Leave();
  wxWindow::Update();
}
    


// removes a widget
// (called from widget's destructor -> don't delete widget here)
void GDLWidget::WidgetRemove( WidgetIDT widID)
{
  widgetList.erase( widID); 
}


// widget from ID
GDLWidget* GDLWidget::GetWidget( WidgetIDT widID)
{
  if( widID == GDLWidget::NullID)
    return NULL;
  WidgetListT::iterator it=widgetList.find( widID);
  if( it == widgetList.end()) return NULL;
  return it->second;
}

// widget parent from ID
GDLWidget* GDLWidget::GetParent( WidgetIDT widID)
{
  GDLWidget *widget = GetWidget( widID);
  if( widget == NULL)
    return NULL;
  WidgetIDT parentID = widget->parentID;
  GDLWidget *parent = GetWidget( parentID);
  return parent;
}

// base widget ID from ID
GDLWidgetBase* GDLWidget::GetBaseWidget( WidgetIDT widID)
{
  WidgetIDT actID = widID;
  while ( 1) {
    GDLWidget* widget = GetWidget( actID);
    if( widget == NULL)
      return NULL;
    if( widget->IsBase())
      return static_cast<GDLWidgetBase*>(widget);     
    assert( widget->parentID != GDLWidget::NullID);
    actID = widget->parentID;
  }
}
WidgetIDT GDLWidget::GetBase( WidgetIDT widID)
{
  GDLWidget *widget;
  WidgetIDT actID = widID;
  while ( 1) {
    GDLWidget* widget = GetWidget( actID);
    if( widget == NULL)
      return GDLWidget::NullID;
    if( widget->IsBase())
      return actID;     
    assert( widget->parentID != GDLWidget::NullID);
    actID = widget->parentID;
  }
}

GDLWidgetBase* GDLWidget::GetTopLevelBaseWidget( WidgetIDT widID)
{
  WidgetIDT actID = widID;
  while ( 1) {
    GDLWidget *widget = GetWidget( actID);
    if( widget == NULL)
      return GDLWidget::NullID;
    if ( widget->parentID == GDLWidget::NullID) 
      return static_cast<GDLWidgetBase*>(widget); 
    else 
      actID = widget->parentID;
  }
}
WidgetIDT GDLWidget::GetTopLevelBase( WidgetIDT widID)
{
  WidgetIDT actID = widID;
  while ( 1) {
    GDLWidget *widget = GetWidget( actID);
    if( widget == NULL)
      return GDLWidget::NullID;
    if ( widget->parentID == GDLWidget::NullID) 
      return actID; 
    else 
      actID = widget->parentID;
  }
}



int GDLWidget::HandleEvents()
{
  int res = 0;
  DStructGDL* ev = NULL;
  while( (ev = GDLWidget::readlineEventQueue.Pop()) != NULL)
  {
    ++res;
//     DStructGDL* ev = GDLWidget::readlineEventQueue.Pop();
//     if( ev == NULL)
//       continue;

    WidgetIDT id = (*static_cast<DLongGDL*>
	    (ev->GetTag(ev->Desc()->TagIndex("ID"), 0)))[0];
    WidgetIDT tlb = (*static_cast<DLongGDL*>
	    (ev->GetTag(ev->Desc()->TagIndex("TOP"), 0)))[0];

    ev = CallEventHandler( id, ev);
    if( ev != NULL)
    {
      Warning( "Unhandled event. ID: " + i2s(id));
      GDLDelete( ev);
      ev = NULL;
    }

    // refresh (if tlb still exists (handler might have destroyed it))
    // even on unhandled event as it might have been rewritten by a handler
    // use 'old' tlb as id might no longer exist
//     WidgetIDT tlb = GetTopLevelBase( id);
    GDLWidget *tlw = GetWidget( tlb);
    if( tlw != NULL)
    {
      assert( dynamic_cast<GDLFrame*>(tlw->wxWidget) != NULL);
      // Pause 50 millisecs then refresh widget 
//       wxMilliSleep( 50); // (why?)
      GUIMutexLockerT gdlMutexGuiEnterLeave;
      static_cast<GDLFrame*>(tlw->wxWidget)->Refresh();
      gdlMutexGuiEnterLeave.Leave();
    }
  }
  return res;
}

void GDLWidget::PushEvent( WidgetIDT baseWidgetID, DStructGDL* ev) 
{
  // Get XmanagerActiveCommand status
  GDLWidget *baseWidget = GDLWidget::GetWidget( baseWidgetID);
  bool xmanActCom = baseWidget->GetXmanagerActiveCommand();
  if( !xmanActCom)
  {
//     wxMessageOutputDebug().Printf("eventQueue.Push: %d\n",baseWidgetID);
    eventQueue.Push(ev);
  }
  else
  {
//     wxMessageOutputDebug().Printf("readLineEventQueue.Push: %d\n",baseWidgetID);
    readlineEventQueue.Push( ev);
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
  for( it = widgetList.begin(); it != widgetList.end(); ++it) {
    std::cout << (*it).first << ": " << (*it).second->widgetID << "  parentID: " <<
    (*it).second->parentID << "  uname: " << (*it).second->uName <<std::endl;   
  }
  std::cout << "- GetXmanagerBlock: widgetList end" << std::endl;
#endif
  for( it = widgetList.begin(); it != widgetList.end(); ++it) {
    // Only consider base widgets
    if ( (*it).second->parentID == GDLWidget::NullID) {
      managed = (*it).second->GetManaged();
      xmanActCom = (*it).second->GetXmanagerActiveCommand();
    }
    if ( managed && !xmanActCom) {
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
  wxInitialize();
}




GDLWidget::GDLWidget( WidgetIDT p, EnvT* e, bool map_/*=true*/,BaseGDL* vV/*=NULL*/)
  : wxWidget( NULL)
  , parentID( p)
  , uValue( NULL)
  , vValue( vV)
  , buttonSet(false)
  , exclusiveMode(0)
  , topWidgetSizer(NULL)
  , widgetSizer(NULL)
  , widgetPanel(NULL)
  , managed( false)
  , map( map_)
  {
    if( e != NULL) 
      SetCommonKeywords( e);

    widgetID = wxWindow::NewControlId();

    if( parentID != GDLWidget::NullID)
    {
        GDLWidget* gdlParent = GetWidget( parentID);
        if( gdlParent->IsBase())
        {
            GDLWidgetBase* base = static_cast< GDLWidgetBase*>( gdlParent);
//       assert( base != NULL); // should be already checked elsewhere
            base->AddChild( widgetID);
        }
        else
        {
            GDLWidgetBase* base = GetBaseWidget( parentID);
            if( base != NULL)
                base->AddChild( widgetID);
        }
    }

    widgetList.insert( widgetList.end(), std::pair<WidgetIDT, GDLWidget*>( widgetID, this));
#ifdef GDL_DEBUG_WIDGETS
//     GUIMutexLockerT gdlMutexGuiEnterLeave;
    wxMessageOutputDebug().Printf("inserted: ID: %d  parentID: %d\n",widgetID,parentID);
//     std::cout << "inserted: ID: " << widgetID << "  parentID: " << parentID << "   uname: " << uName << std::endl;
#endif
  }


  
void GDLWidget::Realize( bool map)
{
  if( parentID == NullID)
  {
    assert( this->IsBase());
#ifdef GDL_DEBUG_WIDGETS
    wxMessageOutputDebug().Printf("GDLWidget:Realize: %d\n",this->widgetID);
#endif
    GDLFrame *frame = static_cast<GDLFrame*>( this->wxWidget);

    GUIMutexLockerT gdlMutexGuiEnterLeave;

    topWidgetSizer->SetSizeHints(frame);
//     frame->SetClientSize(widgetPanel->GetSize());
//     frame->Layout();
    
    frame->SendShowRequestEvent( map);
//     bool stat = frame->Show( map);

    gdlMutexGuiEnterLeave.Leave();
  }
  else
  {
#ifdef GDL_DEBUG_WIDGETS
    wxMessageOutputDebug().Printf("GDLWidget:Realize TLB of: %d\n",this->widgetID);
#endif
    GDLWidgetBase* tlb = GetTopLevelBaseWidget( parentID);
    assert( tlb != NULL);

    GDLFrame *frame = static_cast<GDLFrame *>( tlb->wxWidget);

    GUIMutexLockerT gdlMutexGuiEnterLeave;

    tlb->topWidgetSizer->SetSizeHints(frame);
//     std::cout << "GDLWidget:Realize: SetSizeHints(" << frame << ")" << std::endl;
    frame->SendShowRequestEvent(map);
//     bool stat = frame->Show( map);

    gdlMutexGuiEnterLeave.Leave();  
  }
  
  // start GUI thread

  if (gdlGUIThread == NULL)
  {
    gdlGUIThread = new GDLGUIThread();
#ifdef GDL_DEBUG_WIDGETS
    std::cout << "Created thread: " << gdlGUIThread << std::endl;
    std::cout << "This IsMainThread: " << wxIsMainThread() << std::endl;
#endif    
  
    gdlGUIThread->Create();
    if ( gdlGUIThread->Run() != wxTHREAD_NO_ERROR )
    {
//       delete gdlGUIThread;
      gdlGUIThread = NULL;
      throw GDLException("Failed to create GUI thread.");
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
  GDLDelete(uValue);
  GDLDelete(vValue);
  WidgetRemove( widgetID);
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
DLong x_scroll_size, DLong y_scroll_size)
  : GDLWidget( parentID, e, mapWid)
  , modal( modal_)
  , mbarID( mBarIDInOut)
  , lastRadioSelection( NullID)
{
  //  std::cout << "In GDLWidgetBase::GDLWidgetBase: " << widgetID << std::endl
  
  xmanActCom = false;
  wxWindow *wxParent = NULL;

  // Set exclusiveMode
  // If exclusive then set to -1 to signal first radiobutton
  if ( exclusiveMode_ == BGEXCLUSIVE)
    this->SetExclusiveMode( BGEXCLUSIVE1ST);
  else
    this->SetExclusiveMode(  exclusiveMode_);

  // If first base widget ...
  if ( parentID == 0) 
  {
    // obsolete: thread need to be created here (in realize it is too late)

    GUIMutexLockerT gdlMutexGuiEnterLeave;

//     if( modal) // ???
// 	wxWidget = new wxDialog( wxParent, widgetID, wxString(title_.c_str(), wxConvUTF8));
//     else
    // GDLFrame is derived from wxFrame
    wxString titleWxString = wxString( title_.c_str(), wxConvUTF8);
    GDLFrame *gdlFrame = new GDLFrame( this, wxParent, widgetID, titleWxString);
    wxWidget = gdlFrame;
//     gdlFrame->Freeze();
    
    gdlFrame->SetSize( xSize, ySize);

    if( mbarID != 0)
    {
      GDLWidgetMBar* mBar = new GDLWidgetMBar( widgetID); 
      // already called in constructor:
      // mbarID = GDLWidget::NewWidget( mBar);
      mbarID = mBar->WidgetID();
      mBarIDInOut = mbarID;
      
      gdlFrame->SetMenuBar( static_cast<wxMenuBar*>( mBar->GetWxWidget()));

      gdlFrame->SetSize( xSize, ySize);
    }

    wxSizer *topSizer = new wxBoxSizer( wxVERTICAL);
    topWidgetSizer = topSizer;
    gdlFrame->SetSizer( topSizer);

    wxPanel *panel = new wxPanel( gdlFrame, wxID_ANY);
    widgetPanel = panel;
    //    std::cout << "Creating Panel: " << panel << std::endl;

    wxSizer *sizer = GetNewSizer( col, row, frame, panel);
    widgetSizer = sizer;

    
    //std::cout << "SetSizer: " << *sizerPtr << std::endl;
    panel->SetSizer( sizer);

    topSizer->Add( panel);    
  } 
  else 
  {
    // If parent base widget exists ....
    GDLWidget* gdlParent = GetWidget( parentID);
    wxSizer* parentSizer = gdlParent->GetSizer();
    
    GUIMutexLockerT gdlMutexGuiEnterLeave;

    if( gdlParent->IsTab())
    {
      GDLWidgetTab* parentTab = static_cast<GDLWidgetTab*>(gdlParent);
      wxNotebook* wxParent = static_cast<wxNotebook*>( parentTab->GetWxWidget());

      wxPanel *panel = new wxPanel( wxParent, wxID_ANY);
      widgetPanel = panel;
      wxWidget = panel;

      wxSizer *sizer = GetNewSizer( col, row, frame, panel);
      widgetSizer = sizer;
      panel->SetSizer( sizer);
      
      wxString titleWxString = wxString( title_.c_str(), wxConvUTF8);
      wxParent->AddPage(panel,titleWxString);
//       parentSizer->SetSizeHints( wxParent);
    }
    else
    {
      wxWindow* wxParent = static_cast< wxWindow*>( gdlParent->GetWxWidget());

//       wxPanel *panel = gdlParent->GetPanel();
//       widgetPanel = panel;
//       //    std::cout << "Getting Panel: " << panel << std::endl;

      wxPanel *panel = new wxPanel( wxParent, wxID_ANY);
      widgetPanel = panel;
      wxWidget = panel;

      // If map is true check for parent map value
      if( mapWid) 
	mapWid = gdlParent->GetMap();
      this->SetMap( mapWid);

      wxSizer *sizer = GetNewSizer( col, row, frame, panel);
      widgetSizer = sizer;
      //      std::cout << "Creating Sizer2: " << sizer << std::endl;

      panel->SetSizer( sizer);
    
      parentSizer->Add( panel, 0, wxEXPAND|wxALL, 5);

//       wxWidget = panel;
//       if ( wxParent != NULL) 
// 	parentSizer->SetSizeHints( wxParent);
    }
  }	
}

GDLWidgetBase::~GDLWidgetBase()
{
  std::cout << "In ~GDLWidgetBase(). wxWidget: " << this->wxWidget << std::endl;

  for( SizeT i=0; i<children.size(); i++) 
  {
    delete GetWidget( children[i]);
  }

  GUIMutexLockerT gdlMutexGuiEnterLeave;

  // Close widget frame (might be already closed)
  if( this->parentID == 0)
  {
    // this seems to provoke: LIBDBUSMENU-GLIB-WARNING **: Trying to remove a child that doesn't believe we're it's parent.
    // on wxWidgets < 2.9.5
    if( this->wxWidget != NULL)
    {
      wxMutexLocker lock( static_cast<GDLFrame*>(this->wxWidget)->ownerMutex);//TODO
      // after we got the lock th...
      if( this->wxWidget != NULL)
      {
	static_cast<GDLFrame*>(this->wxWidget)->NullGDLOnwer();
	std::cout << "~GDLWidgetBase: GDLFrame::Destroy(): " << this->wxWidget << std::endl;
	static_cast<GDLFrame*>(this->wxWidget)->Destroy();
	// delete wxWidget;
      }
    }
  }
}



DLong GDLWidgetBase::NChildren() const
{
  return children.size();  
}
WidgetIDT GDLWidgetBase::GetChild( DLong childIx) const
{
  assert( childIx >= 0);
  assert( childIx < children.size());
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
GDLWidgetTab::GDLWidgetTab( WidgetIDT p, EnvT* e, DLong location, DLong multiline)
: GDLWidget( p, e)
{
  GUIMutexLockerT gdlMutexGuiEnterLeave;

  GDLWidget* gdlParent = GetWidget( p);
  wxWindow *wxParent = static_cast< wxWindow*>( gdlParent->GetWxWidget());

  wxPanel *panel = gdlParent->GetPanel();

  long style = wxNB_TOP;
  if( location == 1) style = wxNB_BOTTOM;
  if( location == 2) style = wxNB_LEFT;
  if( location == 3) style = wxNB_RIGHT;
  
  if( multiline != 0)
    style |= wxNB_MULTILINE;
  
  wxNotebook *notebook = new wxNotebook( panel, widgetID,
					  wxPoint(xOffset,yOffset),
					  wxSize(xSize, ySize),
					  style);
  this->wxWidget = notebook;

  widgetSizer = gdlParent->GetSizer();
  widgetSizer->Add( notebook, 0, wxEXPAND | wxALL, 5);
}

GDLWidgetTab::~GDLWidgetTab()
{}

  

/*********************************************************/
// for WIDGET_SLIDER
/*********************************************************/
GDLWidgetSlider::GDLWidgetSlider( WidgetIDT p, EnvT* e, DLong value_, DLong minimum_, DLong maximum_, bool vertical, bool suppressValue)
: GDLWidget( p, e)
, value(value_)
, minimum(minimum_)
, maximum(maximum_)
{
  GUIMutexLockerT gdlMutexGuiEnterLeave;

  GDLWidget* gdlParent = GetWidget( p);
  wxWindow *wxParent = static_cast< wxWindow*>( gdlParent->GetWxWidget());

  wxPanel *panel = gdlParent->GetPanel();

  long style = wxSL_HORIZONTAL;
  if( vertical)
    style |= wxSL_VERTICAL;
  if( !suppressValue)
    style |= wxSL_LABELS;

  wxSlider *slider = new wxSlider( panel, widgetID, value, minimum, maximum,
				    wxPoint(xOffset,yOffset),
				    wxSize(xSize, ySize),
				    style);
  this->wxWidget = slider;

  widgetSizer = gdlParent->GetSizer();
  widgetSizer->Add( slider, 0, wxEXPAND | wxALL, 5);
}

GDLWidgetSlider::~GDLWidgetSlider()
{}

  

GDLWidgetButton::GDLWidgetButton( WidgetIDT p, EnvT* e,
				  const DString& value)
: GDLWidget( p, e)
{
  GUIMutexLockerT gdlMutexGuiEnterLeave;

  GDLWidget* gdlParent = GetWidget( p);
  wxObject *wxParentObject = gdlParent->GetWxWidget();

  //  std::cout << "In Button: " << widgetID << " Parent: " << p << " xMode:" <<
  //gdlParent->GetExclusiveMode() << " " << value << std::endl;

  wxMenuBar *menuBar =  dynamic_cast< wxMenuBar*>( wxParentObject);
  if( menuBar != NULL)
  {
//     cout << "MenuBar: " << widgetID << endl;
    this->wxWidget = new wxMenu();
  wxString valueWxString = wxString( value.c_str(), wxConvUTF8);
    menuBar->Append( static_cast<wxMenu*>(this->wxWidget), valueWxString);
  }
  else
  {
//     cout << "Menu: " << widgetID << endl;
    wxMenu *menu =  dynamic_cast< wxMenu*>( wxParentObject);
    if( menu != NULL)
    {
        // wxMenuItem
// 	this->wxWidget = menu->Append( widgetID, wxString(value.c_str(), wxConvUTF8));
	// at destruction this seems to provoke: LIBDBUSMENU-GLIB-WARNING **: Trying to remove a child that doesn't believe we're it's parent.
	// on wxWidgets < 2.9.5
	wxString valueWxString = wxString( value.c_str(), wxConvUTF8);
	wxMenuItem* menuItem = new wxMenuItem( menu, widgetID, valueWxString);
	menu->Append( menuItem);
	this->wxWidget = menuItem;
// 	this->wxWidget = menu->Append( widgetID, wxString(value.c_str(), wxConvUTF8));
    }
    else if (gdlParent->GetMap()) {
//       cout << "Button: " << widgetID << endl;
      wxPanel *panel = gdlParent->GetPanel();

      wxSizer *boxSizer = gdlParent->GetSizer();

      if ( gdlParent->GetExclusiveMode() == BGNORMAL) 
      {
	wxString valueWxString = wxString( value.c_str(), wxConvUTF8);
	wxButton *button = new wxButton( panel, widgetID, valueWxString,
	  wxPoint(xOffset,yOffset));
	boxSizer->Add( button, 0, wxEXPAND | wxALL, 5);
	this->wxWidget = button;
// 	cout << "wxButton: " << widgetID << endl;
      }
      else if ( gdlParent->GetExclusiveMode() == BGEXCLUSIVE1ST) 
      {
	wxString valueWxString = wxString( value.c_str(), wxConvUTF8);
	wxRadioButton *radioButton = new wxRadioButton( panel, widgetID, valueWxString,
					wxPoint(xOffset,yOffset), wxDefaultSize,
					wxRB_GROUP);
	gdlParent->SetExclusiveMode( 1);
	static_cast<GDLWidgetBase*>(gdlParent)->SetLastRadioSelection( widgetID);
	boxSizer->Add( radioButton, 0, wxEXPAND | wxALL, 5);
	this->wxWidget = radioButton;
// 	cout << "wxRadioButton1: " << widgetID << endl;
      } 
      else if ( gdlParent->GetExclusiveMode() == BGEXCLUSIVE) 
      {
	wxString valueWxString = wxString( value.c_str(), wxConvUTF8);
	wxRadioButton *radioButton = new wxRadioButton( panel, widgetID, valueWxString,
	  wxPoint(xOffset,yOffset));
	boxSizer->Add( radioButton, 0, wxEXPAND | wxALL, 5);
	this->wxWidget = radioButton;
// 	cout << "wxRadioButton: " << widgetID << endl;
      } 
      else if ( gdlParent->GetExclusiveMode() == BGNONEXCLUSIVE) 
      {
	wxString valueWxString = wxString( value.c_str(), wxConvUTF8);
	wxCheckBox *checkBox = new wxCheckBox( panel, widgetID, valueWxString,
	  wxPoint(xOffset,yOffset));
	boxSizer->Add( checkBox, 0, wxEXPAND | wxALL, 5);
	this->wxWidget = checkBox;
// 	cout << "wxCheckBox: " << widgetID << endl;
      }

      wxWindow *wxParent = dynamic_cast< wxWindow*>( wxParentObject);
//       if ( wxParent != NULL) {
// 	//      std::cout << "SetSizeHints: " << wxParent << std::endl;
// 	boxSizer->SetSizeHints( wxParent);
//       }
    } // GetMap()
  }

  gdlMutexGuiEnterLeave.Leave();
}



GDLWidgetBGroup::GDLWidgetBGroup(WidgetIDT p, DStringGDL* names,
                                 BaseGDL *uV, DString buttonuvalue,
                                 DLong xSize, DLong ySize,
                                 DString labeltop, DLong rows, DLong cols,
                                 BGroupMode mode, BGroupReturn ret
                                ):
    GDLWidget( p, NULL)
{
    GUIMutexLockerT gdlMutexGuiEnterLeave;

    GDLWidget* gdlParent = GetWidget( p);
    wxWindow *wxParent = static_cast< wxWindow*>(
                             gdlParent->GetWxWidget());

    wxPanel *panel = gdlParent->GetPanel();

    wxBoxSizer *boxSizer = (wxBoxSizer *) gdlParent->GetSizer();

    //DStringGDL* buttonval = static_cast<DStringGDL*>( buttonvalue);

    DLong n = names->N_Elements();
    wxString *choices = new wxString[n];
    for( SizeT i=0; i<n; ++i) choices[i] = wxString((*names)[i].c_str(),wxConvUTF8);

    wxString labeltopWxString = wxString( labeltop.c_str(), wxConvUTF8);
    wxStaticText* label = new wxStaticText( panel, widgetID,
                                            labeltopWxString,
                                            wxPoint(10,10),
                                            wxDefaultSize, wxALIGN_CENTRE);

    boxSizer->Add( label, 0, wxEXPAND | wxALL, 5);

    // define grid dimension
    if(rows == -1 && cols == -1) {
        cols = 1;
        rows = n;
    } else {
        if(rows == -1) {
            rows = 1;
            cols = n;
        }
        if(cols == -1) {
            cols = 1;
            rows = n;
        }
    }

    // define grid object
    wxFlexGridSizer *buttonSizer = new wxFlexGridSizer( (int)rows, (int) cols, 0, 0 );
    switch(mode)
    {
    case BGNORMAL:
    {
        for( SizeT i=0; i<n; ++i) {
            wxButton* button = new wxButton(panel, widgetID, choices[i]);
            buttonSizer->Add( button, 0, wxEXPAND | wxALL, 5);
        }
    }
    break;
    case BGEXCLUSIVE:
    {
        wxRadioButton* radio = new wxRadioButton(panel, widgetID,
                choices[0],
                wxDefaultPosition,
                wxDefaultSize,
                wxRB_GROUP
                                                );
        buttonSizer->Add( radio, 0, wxEXPAND | wxALL, 5);
        for( SizeT i=1; i<n; ++i) {
            radio = new wxRadioButton(panel, widgetID, choices[i]);
            buttonSizer->Add( radio, 0, wxEXPAND | wxALL, 5);
        }
    }
    break;
    case BGNONEXCLUSIVE:
    {
        wxCheckBox* check;
        for( SizeT i=0; i<n; ++i) {
            check = new wxCheckBox(panel, widgetID, choices[i]);
            buttonSizer->Add( check, 0, wxEXPAND | wxALL, 5);
        }
    }
    }

    boxSizer->Add( buttonSizer,
                   0,                // make vertically unstretchable
                   wxALIGN_CENTER ); // no border and centre horizontally

//     if ( wxParent != NULL) {
//         boxSizer->SetSizeHints( wxParent);
//     }
    gdlMutexGuiEnterLeave.Leave();
}



GDLWidgetList::GDLWidgetList( WidgetIDT p, EnvT* e, BaseGDL *value, DLong style)
    : GDLWidget( p, e, true, value)
{
    GUIMutexLockerT gdlMutexGuiEnterLeave;

    GDLWidget* gdlParent = GetWidget( p);
    wxWindow *wxParent = static_cast< wxWindow*>( gdlParent->GetWxWidget());

    wxPanel *panel = gdlParent->GetPanel();

    if( vValue->Type() != GDL_STRING)
    {
        vValue = static_cast<DStringGDL*>( vValue->Convert2(GDL_STRING,BaseGDL::CONVERT));
    }
    DStringGDL* val = static_cast<DStringGDL*>( vValue);

    DLong n= val->N_Elements();
    wxArrayString choices;// = new wxString[n];
    for( SizeT i=0; i<n; ++i)
        choices.Add( wxString((*val)[i].c_str(), wxConvUTF8));

    wxListBox *list = new wxListBox( panel, widgetID, wxPoint(xOffset,yOffset),
                                     wxSize( xSize, ySize),
                                     choices, style);
    this->wxWidget = list;

    wxSizer *boxSizer = gdlParent->GetSizer();
    boxSizer->Add( list, 0, wxEXPAND | wxALL, 5);

//     if ( wxParent != NULL) {
//         boxSizer->SetSizeHints( wxParent);
//     }
    gdlMutexGuiEnterLeave.Leave();
}

//GDLWidgetDropList::GDLWidgetDropList( WidgetIDT p, BaseGDL *uV, DStringGDL *value,
GDLWidgetDropList::GDLWidgetDropList( WidgetIDT p, EnvT* e, BaseGDL *value,
                                      const DString& title, DLong style)
    : GDLWidget( p, e, true, value)
{
  //  std::cout << "In DropList: " << widgetID << " " << p << std::endl;
  GUIMutexLockerT gdlMutexGuiEnterLeave;

  GDLWidget* gdlParent = GetWidget( p);
  wxWindow *wxParent = static_cast< wxWindow*>( gdlParent->GetWxWidget());

  if (gdlParent->GetMap()) {
    wxPanel *panel = gdlParent->GetPanel();

    if( vValue->Type() != GDL_STRING)
    {
      vValue = static_cast<DStringGDL*>( vValue->Convert2(GDL_STRING,BaseGDL::CONVERT));
    }
    DStringGDL* val = static_cast<DStringGDL*>( vValue);
    
    DLong n= val->N_Elements();
    wxArrayString choices;// = new wxString[n];
    for( SizeT i=0; i<n; ++i) 
      choices.Add( wxString((*val)[i].c_str(), wxConvUTF8));

    wxString val0WxString = wxString( (*val)[0].c_str(), wxConvUTF8);
    wxComboBox *combo = new wxComboBox( panel, widgetID, val0WxString,
			    wxPoint(xOffset,yOffset), wxSize(xSize,ySize), choices, style);

    wxSizer *boxSizer = gdlParent->GetSizer();
    boxSizer->Add( combo, 0, wxEXPAND | wxALL, 5);

//     if ( wxParent != NULL) {
//       //      std::cout << "SetSizeHints: " << wxParent << std::endl;
//       boxSizer->SetSizeHints( wxParent);
//     }
    
    this->wxWidget = combo;
  } // GetMap()
}


GDLWidgetText::GDLWidgetText( WidgetIDT p, EnvT* e, DStringGDL* valueStr, bool noNewLine,
	bool editable):
  GDLWidget( p, e, true, valueStr)
{
  //  std::cout << "In Text: " << widgetID << " " << p << std::endl;
  GUIMutexLockerT gdlMutexGuiEnterLeave;

  GDLWidget* gdlParent = GetWidget( p);
  wxWindow *wxParent = static_cast< wxWindow*>( gdlParent->GetWxWidget());

  wxPanel *panel = gdlParent->GetPanel();

  DString value = "";
  if( valueStr != NULL)
  {
      for( int i=0; i<valueStr->N_Elements(); ++i)
      {
	value += (*valueStr)[i];
	if( !noNewLine && (i+1) < valueStr->N_Elements())
	  value += '\n';
      }
  }
  
  // remember initial setting
  lastValue = value;
  
  wxSize fontSize = wxNORMAL_FONT->GetPixelSize();      
  wxSize widgetSize = wxDefaultSize;
  if( xSize != widgetSize.x)
    widgetSize.x = xSize * fontSize.x;
  if( ySize != widgetSize.y)
    widgetSize.y = ySize * fontSize.y;

  DLong style = wxTE_NOHIDESEL;
  if( !editable)
    style |= wxTE_READONLY;
  if( ySize > 1)
    style |= wxTE_MULTILINE;
  
  wxString valueWxString = wxString( value.c_str(), wxConvUTF8);
  wxTextCtrl* text = new wxTextCtrl( panel, widgetID, valueWxString,
    wxPoint(xOffset,yOffset), widgetSize, style );
  this->wxWidget = text;

  wxBoxSizer *boxSizer = (wxBoxSizer *) gdlParent->GetSizer();
  boxSizer->Add( text, 0, wxEXPAND | wxALL, 5);

//   if ( wxParent != NULL) {
//     boxSizer->SetSizeHints( wxParent);
//   }

  gdlMutexGuiEnterLeave.Leave();
}


void GDLWidgetText::SetTextValue( DStringGDL* valueStr, bool noNewLine)
{
  delete vValue;
  vValue = valueStr;
  DString value = "";
  for( int i=0; i<valueStr->N_Elements(); ++i)
  {
    value += (*valueStr)[ i];
    if( !noNewLine && (i+1) < valueStr->N_Elements())
      value += '\n';
  }

  GUIMutexLockerT gdlMutexGuiEnterLeave;

  wxString valueWxString = wxString( value.c_str(), wxConvUTF8);
  static_cast<wxTextCtrl*>(wxWidget)->SetValue( valueWxString);
  static_cast<wxTextCtrl*>(wxWidget)->Refresh(); 
  //wxMilliSleep(700); 
  gdlMutexGuiEnterLeave.Leave();
 }


GDLWidgetLabel::GDLWidgetLabel( WidgetIDT p, EnvT* e, DString value)
: GDLWidget( p, e)
{
  GUIMutexLockerT gdlMutexGuiEnterLeave;
  //  std::cout << "In Label: " << widgetID << " " << p << std::endl;

  GDLWidget* gdlParent = GetWidget( p);
  wxWindow *wxParent = static_cast< wxWindow*>( gdlParent->GetWxWidget());
  
  wxPanel *panel = gdlParent->GetPanel();

//     wxSize fontSize = wxNORMAL_FONT->GetPixelSize(); 
  wxString valueWxString = wxString( value.c_str(), wxConvUTF8);
  wxStaticText* label = new wxStaticText( panel, widgetID, valueWxString,
			      wxPoint(xOffset,yOffset), wxSize(xSize, ySize), wxALIGN_CENTRE);
  this->wxWidget = label;

  wxBoxSizer *boxSizer = (wxBoxSizer *) gdlParent->GetSizer();
  boxSizer->Add( label, 0, wxEXPAND | wxALL, 5);

//   if ( wxParent != NULL) {
//     boxSizer->SetSizeHints( wxParent);
//   }
  gdlMutexGuiEnterLeave.Leave();
}

void GDLWidgetLabel::SetLabelValue( DString value)
{
  GUIMutexLockerT gdlMutexGuiEnterLeave;
  wxString valueWxString = wxString( value.c_str(), wxConvUTF8);
  static_cast<wxStaticText*>(wxWidget)->SetLabel( valueWxString);
  static_cast<wxStaticText*>(wxWidget)->Refresh(); 
  gdlMutexGuiEnterLeave.Leave();
 }

// *** GDLFrame ***
GDLFrame::GDLFrame(GDLWidgetBase* gdlOwner_, wxWindow* parent, wxWindowID id, const wxString& title)
: wxFrame(parent, id, title)
, gdlOwner( gdlOwner_)
{
}

GDLFrame::~GDLFrame()
{ 
  wxMutexLocker lock( ownerMutex); // TODO
  std::cout << "~GDLFrame: " << this << std::endl;
  std::cout << "This IsMainThread: " << wxIsMainThread() << std::endl;
  if( gdlOwner != NULL)
  {
    gdlOwner->NullWxWidget();
  }
}


// *** guiThread ***
wxThread::ExitCode GDLGUIThread::Entry()
{
    // Called from PthreadStart() in threadpsx.cpp (wxWidgets)

    // gui loop

//   std::cout << "In thread Entry()" << std::endl;
    try {
        wxTheApp->OnRun();
        // Calls GDLApp::OnRun()
    }
    catch( exception& e)
    {
        cout << "GDLGUIThread::Entry(): Exception caught: " << e.what() << endl;
    }
    catch( ...)
    {
        cout << "GDLGUIThread::Entry(): Unknown exception caught." << endl;
    }

    return NULL;
}

int GDLApp::OnRun()
{
  // Called by guiThread::Entry()

//   std::cout << " In OnRun()" << std::endl;

  int exitcode = wxApp::OnRun();
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
  if (gdlGUIThread != NULL)
  {
//      delete gdlGUIThread;
     gdlGUIThread = NULL;
  }

  return 0;
}


void GDLGUIThread::OnExit()
{
  // Called by GDLApp::OnExit() in gdlwidget.cpp
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "In guiThread::OnExit()." << std::endl;
  std::cout << "IsMainThread: " << wxIsMainThread() << std::endl;
#endif
  gdlGUIThread = NULL;
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

#endif

