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

#include "basegdl.hpp"
#include "dstructgdl.hpp"

#include "dinterpreter.hpp"

#ifdef HAVE_LIBWXWIDGETS

#include "gdlwxstream.hpp"

#include "devicex.hpp"

#include "gdlwidget.hpp"

#include "widget.hpp"
#include "graphicsdevice.hpp"

#define GDL_DEBUG_WIDGETS


BEGIN_EVENT_TABLE(GDLFrame, wxFrame)
  EVT_IDLE( GDLFrame::OnIdle)
  EVT_MENU(wxID_ANY, GDLFrame::OnButton)
  EVT_BUTTON( wxID_ANY, GDLFrame::OnButton)
  EVT_RADIOBUTTON(wxID_ANY, GDLFrame::OnRadioButton)
  EVT_CHECKBOX(wxID_ANY, GDLFrame::OnCheckBox)
  EVT_COMBOBOX(wxID_ANY, GDLFrame::OnComboBox)
  EVT_LISTBOX(wxID_ANY, GDLFrame::OnListBox)
  EVT_LISTBOX_DCLICK(wxID_ANY, GDLFrame::OnListBoxDoubleClicked)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(GDLWindow, wxWindow)
  EVT_PAINT(GDLWindow::OnPaint)
//   EVT_SHOW(GDLWindow::OnShow)
//   EVT_CLOSE(GDLWindow::OnClose)
END_EVENT_TABLE()

IMPLEMENT_APP_NO_MAIN( GDLApp)

//#define GDL_DEBUG_WIDGETS

const WidgetIDT GDLWidget::NullID = 0;

// instantiation
// WidgetIDT	GDLWidget::widgetIx;
WidgetListT	GDLWidget::widgetList;

// VarListT                    eventVarList;
GDLEventQueue	GDLWidget::eventQueue; // the event queue
GDLEventQueue	GDLWidget::readlineEventQueue; // for process at command line level



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
  if( pstreamP != NULL)
      pstreamP->SetValid(false);
}

void GDLWindow::Update()
{
  wxClientDC dc( this);
  dc.SetDeviceClippingRegion( GetUpdateRegion() );
  wxMutexGuiEnter();
  dc.Blit( 0, 0, drawSize.x, drawSize.y, m_dc, 0, 0 );
  wxMutexGuiLeave();
  wxWindow::Update();
}
    
void GDLWindow::OnPaint(wxPaintEvent& event)
{
    std::cout << "GDLWindow::OnPaint: " << this << std::endl;

    wxPaintDC dc( this);
    dc.SetDeviceClippingRegion( GetUpdateRegion() );
//     wxMutexGuiEnter();
    dc.Blit( 0, 0, drawSize.x, drawSize.y, m_dc, 0, 0 );
//     wxMutexGuiLeave();
}
// void GDLWindow::OnCreate(wxWindowCreateEvent& event)
// {
//   std::cout << "GDLWindow::OnCreate: " << this << std::endl;
// }
// void GDLWindow::OnDestroy(wxWindowDestroyEvent& event)
// {
//   std::cout << "GDLWindow::OnDestroy: " << this << std::endl;
// }
void GDLWindow::OnShow(wxShowEvent& event)
{
  std::cout << "GDLWindow::OnShow: " << this << std::endl;
}
void GDLWindow::OnClose(wxCloseEvent& event)
{
  std::cout << "GDLWindow::OnClose: " << this << std::endl;
}





wxSizer* GetNewSizer( DLong col, DLong row, DLong frameBox, 
	       wxPanel *panel) 
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
  while( !GDLWidget::readlineEventQueue.empty())
  {
    ++res;
    DStructGDL* ev = GDLWidget::readlineEventQueue.pop();

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
      wxMutexGuiEnter();
      static_cast<GDLFrame*>(tlw->wxWidget)->Refresh();
      wxMutexGuiLeave();
    }
  }
  return res;
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
  //  std::cout << " In GDLWidget::Init()" << std::endl;
//   widgetIx = wxID_HIGHEST; // use same wx ID and GDL ID 

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
            GDLWidgetBase* tlb = GetBaseWidget( parentID);
            if( tlb != NULL)
                tlb->AddChild( widgetID);
        }
    }

    widgetList.insert( widgetList.end(), std::pair<WidgetIDT, GDLWidget*>( widgetID, this));
#ifdef GDL_DEBUG_WIDGETS
    std::cout << "inserted: ID: " << widgetID << "  parentID: " << parentID << "   uname: " << uName << std::endl;
#endif
  }

// GDLWidget::GDLWidget( WidgetIDT p, BaseGDL* uV, BaseGDL* vV, bool s, bool mp,
//                       DLong xO, DLong yO, DLong xS, DLong yS, const DString& uname
//                       , const DString&  proValue_
//                       , const DString&  funcValue_
//                       , const DString&  eventPro_
//                       , const DString&  eventFun_
//                       , const DString&  notifyRealize_
//                       , const DString&  killNotify_
//                     )
//     : wxWidget( NULL)
//     , parentID( p), uValue( uV), vValue( vV), sensitive( s)
//     , map( mp)
//     , buttonSet(false)
//     , exclusiveMode(0)
//     , xOffset( xO), yOffset( yO), xSize( xS), ySize( yS)
//     , topWidgetSizer(NULL)
//     , widgetSizer(NULL)
//     , widgetPanel(NULL)
//     , uName( uname)
//     , proValue( proValue_)
//     , funcValue( funcValue_)
//     , eventPro( eventPro_)
//     , eventFun( eventFun_)
//     , notifyRealize( notifyRealize_)
//     , killNotify( killNotify_)
//     , managed( false)
// {
//     // widgetID = NewWidget( this);
//     widgetID = wxWindow::NewControlId();
// 
//     if( parentID != GDLWidget::NullID)
//     {
//         GDLWidget* gdlParent = GetWidget( parentID);
//         if( gdlParent->IsBase())
//         {
//             GDLWidgetBase* base = static_cast< GDLWidgetBase*>( gdlParent);
// //       assert( base != NULL); // should be already checked elsewhere
//             base->AddChild( widgetID);
//         }
//         else
//         {
//             GDLWidgetBase* tlb = GetBaseWidget( parentID);
//             if( tlb != NULL)
//                 tlb->AddChild( widgetID);
//         }
//     }
// 
//     widgetList.insert( widgetList.end(), std::pair<WidgetIDT, GDLWidget*>( widgetID, this));
// #ifdef GDL_DEBUG_WIDGETS
//     std::cout << "inserted: ID: " << widgetID << "  parentID: " << parentID << "   uname: " << uName << std::endl;
// #endif
// }

GDLWidget::~GDLWidget()
{
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "in ~GDLWidget(): " << widgetID << std::endl;
#endif
  managed = false;

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
    // thread need to be created here (in realize it is too late)
    if (gdlGUIThread != NULL)
    {
#ifdef GDL_DEBUG_WIDGETS
      std::cout << "gdlGUIThread: " << gdlGUIThread << std::endl;
#endif    
      if( gdlGUIThread->Exited() || !gdlGUIThread->IsAlive())
      {
#ifdef GDL_DEBUG_WIDGETS
      std::cout << "gdlGUIThread->Wait(): " << gdlGUIThread << std::endl;
#endif    
	gdlGUIThread->Wait();
	delete gdlGUIThread;
	gdlGUIThread = NULL;
      }
    }
    if (gdlGUIThread == NULL)
    {
//       gdlGUIThread->Exit(); // delete itself

    // Defined in threadpsx.cpp (wxWidgets)
      gdlGUIThread = new GDLGUIThread();
#ifdef GDL_DEBUG_WIDGETS
      std::cout << "Created thread: " << gdlGUIThread << std::endl;
      std::cout << "This IsMainThread: " << wxIsMainThread() << std::endl;
#endif    
   
      gdlGUIThread->Create();
      gdlGUIThread->Run();
    }
    // GUI lock defined in threadpsx.cpp

    wxMutexGuiEnter();

    // GDLFrame is derived from wxFrame
    GDLFrame *gdlFrame = new GDLFrame( wxParent, widgetID, wxString(title_.c_str(), wxConvUTF8));
    wxWidget = gdlFrame;

    gdlFrame->SetSize( xSize, ySize);

    if( mbarID != 0)
    {
      GDLWidgetMBar* mBar = new GDLWidgetMBar( widgetID); 
      // already called in constructor:
      // mbarID = GDLWidget::NewWidget( mBar);
      mbarID = mBar->WidgetID();
      mBarIDInOut = mbarID;
      
//       wxMenuBar* m = static_cast<wxMenuBar*>(GDLWidget::GetWidget( mbarID)->WxWidget());
      gdlFrame->SetMenuBar( static_cast<wxMenuBar*>( mBar->WxWidget()));

//       GDLWidget::GetWidget( mbarID)->InitParentID( widgetID);

      gdlFrame->SetSize( xSize, ySize);
    }

    wxPanel *panel = new wxPanel( gdlFrame, wxID_ANY);
    widgetPanel = panel;
    //    std::cout << "Creating Panel: " << panel << std::endl;

    wxSizer *sizer = GetNewSizer( col, row, frame, panel);
    widgetSizer = sizer;

    topWidgetSizer = sizer;
    //std::cout << "SetSizer: " << *sizerPtr << std::endl;
    panel->SetSizer( sizer);
  } 
  else 
  {
    wxMutexGuiEnter();
    // If parent base widget exists ....
    GDLWidget* gdlParent = GetWidget( parentID);
    wxParent = static_cast< wxWindow*>( gdlParent->WxWidget());
    //    std::cout << "Getting Parent: " << parent << " " << gdlParent << " " 
    //      << wxParent << std::endl;

    wxPanel *panel = gdlParent->GetPanel();
    widgetPanel = panel;
    //    std::cout << "Getting Panel: " << panel << std::endl;

    // If map is true check for parent map value
    if( mapWid) 
      mapWid = gdlParent->GetMap();
    this->SetMap( mapWid);

    wxSizer *sizer = GetNewSizer( col, row, frame, panel);
    widgetSizer = sizer;
    //      std::cout << "Creating Sizer2: " << sizer << std::endl;
  
    wxSizer* parentSizer = gdlParent->GetSizer();
    parentSizer->Add( sizer, 0, wxEXPAND|wxALL, 5);

    if( modal)
      wxWidget = new wxDialog( wxParent, widgetID, wxString(title_.c_str(), wxConvUTF8));
  }
  wxMutexGuiLeave();
}

GDLWidgetBase::~GDLWidgetBase()
{
  std::cout << "In ~GDLWidgetBase(). wxWidget: " << this->wxWidget << std::endl;

  // Note: iterator for loop doesn't work when deleting widget
  for( SizeT i=0; i<children.size(); i++) 
  {
    delete GetWidget( children[i]);
  }
  std::cout << "in ~GDLWidgetBase(). wxWidget: " << this->wxWidget << std::endl;

  wxMutexGuiEnter();

  // Close widget frame
  if( this->parentID == 0)
  {
    // this seems to provoke: LIBDBUSMENU-GLIB-WARNING **: Trying to remove a child that doesn't believe we're it's parent.
    // on wxWidgets < 2.9.5
    { std::cout << "~GDLWidgetBase: GDLFrame::Destroy(): " << this->wxWidget << std::endl;}

    ((GDLFrame *) this->wxWidget)->Destroy();
  }
  // if TLB destroy wxWidget 
  //  if( parent == 0)
  //delete wxWidget;
  
  wxMutexGuiLeave();
}

void GDLWidgetBase::Realize( bool map)
{
  wxMutexGuiEnter();

  GDLFrame *frame = (GDLFrame *) this->wxWidget;
  bool stat = frame->Show( map);

  wxMutexGuiLeave();
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



GDLWidgetButton::GDLWidgetButton( WidgetIDT p, EnvT* e,
				  const DString& value)
: GDLWidget( p, e)
{
  wxMutexGuiEnter();

  GDLWidget* gdlParent = GetWidget( p);
  wxObject *wxParentObject = gdlParent->WxWidget();

  //  std::cout << "In Button: " << widgetID << " Parent: " << p << " xMode:" <<
  //gdlParent->GetExclusiveMode() << " " << value << std::endl;

  wxMenuBar *menuBar =  dynamic_cast< wxMenuBar*>( wxParentObject);
  if( menuBar != NULL)
  {
//     cout << "MenuBar: " << widgetID << endl;
    this->wxWidget = new wxMenu();
    menuBar->Append( static_cast<wxMenu*>(this->wxWidget), wxString(value.c_str(), wxConvUTF8));
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
	wxMenuItem* menuItem = new wxMenuItem( menu, widgetID, wxString(value.c_str(), wxConvUTF8));
	menu->Append( menuItem);
	this->wxWidget = menuItem;
// 	this->wxWidget = menu->Append( widgetID, wxString(value.c_str(), wxConvUTF8));
    }
    else if (gdlParent->GetMap()) {
//       cout << "Button: " << widgetID << endl;
      wxPanel *panel = gdlParent->GetPanel();

      wxBoxSizer *boxSizer = (wxBoxSizer *) gdlParent->GetSizer();

      if ( gdlParent->GetExclusiveMode() == BGNORMAL) 
      {
	wxButton *button = new wxButton( panel, widgetID, wxString(value.c_str(), wxConvUTF8),
	  wxPoint(xOffset,yOffset));
	boxSizer->Add( button, 0, wxEXPAND | wxALL, 5);
	this->wxWidget = button;
	cout << "wxButton: " << widgetID << endl;
      }
      else if ( gdlParent->GetExclusiveMode() == BGEXCLUSIVE1ST) 
      {
	wxRadioButton *radioButton = new wxRadioButton( panel, widgetID, wxString( value.c_str(), wxConvUTF8),
					wxPoint(xOffset,yOffset), wxDefaultSize,
					wxRB_GROUP);
	gdlParent->SetExclusiveMode( 1);
	static_cast<GDLWidgetBase*>(gdlParent)->SetLastRadioSelection( widgetID);
	boxSizer->Add( radioButton, 0, wxEXPAND | wxALL, 5);
	this->wxWidget = radioButton;
	cout << "wxRadioButton1: " << widgetID << endl;
      } 
      else if ( gdlParent->GetExclusiveMode() == BGEXCLUSIVE) 
      {
	wxRadioButton *radioButton = new wxRadioButton( panel, widgetID, wxString(value.c_str(), wxConvUTF8),
	  wxPoint(xOffset,yOffset));
	boxSizer->Add( radioButton, 0, wxEXPAND | wxALL, 5);
	this->wxWidget = radioButton;
	cout << "wxRadioButton: " << widgetID << endl;
      } 
      else if ( gdlParent->GetExclusiveMode() == BGNONEXCLUSIVE) 
      {
	wxCheckBox *checkBox = new wxCheckBox( panel, widgetID, wxString(value.c_str(), wxConvUTF8),
	  wxPoint(xOffset,yOffset));
	boxSizer->Add( checkBox, 0, wxEXPAND | wxALL, 5);
	this->wxWidget = checkBox;
	cout << "wxCheckBox: " << widgetID << endl;
      }

      wxWindow *wxParent = dynamic_cast< wxWindow*>( wxParentObject);
      if ( wxParent != NULL) {
	//      std::cout << "SetSizeHints: " << wxParent << std::endl;
	boxSizer->SetSizeHints( wxParent);
      }
    } // GetMap()
  }

  wxMutexGuiLeave();
//   // Generate event structure
//   DStructGDL*  widgbut = new DStructGDL( "WIDGET_BUTTON");
//   widgbut->InitTag("ID", DLongGDL( widgetID));
//   widgbut->InitTag("TOP", DLongGDL( GDLWidget::GetTopLevelBase( p)));
//   widgbut->InitTag("HANDLER", DLongGDL( 0));
//   widgbut->InitTag("SELECT", DLongGDL( 0));
// 
//   // Push event structure into event variable list
//   std::ostringstream varname;
//   varname << "WBUT" << this->WidgetID();
//   DVar *v = new DVar( varname.rdbuf()->str(), widgbut);
//   eventVarList.push_back(v);
}

// void GDLWidgetButton::SetSelectOff()
// {
//   // Form button event variable name
//   std::ostringstream varname;
//   varname << "WBUT" << this->WidgetID();
// 
//   // Find name and set SELECT tag to 0
//   DVar *var=FindInVarList( eventVarList, varname.rdbuf()->str());
//   DStructGDL* s = static_cast<DStructGDL*>( var->Data());
//   (*static_cast<DLongGDL*>
//    (s->GetTag(s->Desc()->TagIndex("SELECT"), 0)))[0] = 0;
// }

GDLWidgetBGroup::GDLWidgetBGroup(WidgetIDT p, DStringGDL* names,
                                 BaseGDL *uV, DString buttonuvalue,
                                 DLong xSize, DLong ySize,
                                 DString labeltop, DLong rows, DLong cols,
                                 BGroupMode mode, BGroupReturn ret
                                ):
    GDLWidget( p, NULL)
{
  wxMutexGuiEnter();

  GDLWidget* gdlParent = GetWidget( p);
    wxWindow *wxParent = static_cast< wxWindow*>(
                             gdlParent->WxWidget());

        wxPanel *panel = gdlParent->GetPanel();

        wxBoxSizer *boxSizer = (wxBoxSizer *) gdlParent->GetSizer();

        //DStringGDL* buttonval = static_cast<DStringGDL*>( buttonvalue);

        DLong n = names->N_Elements();
        wxString *choices = new wxString[n];
        for( SizeT i=0; i<n; ++i) choices[i] = wxString((*names)[i].c_str(),wxConvUTF8);

        wxStaticText* label = new wxStaticText( panel, widgetID,
                                                wxString(labeltop.c_str(), wxConvUTF8),
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

        if ( wxParent != NULL) {
            boxSizer->SetSizeHints( wxParent);
        }
  wxMutexGuiLeave();
//     // Generate event structure
//     // event = {ID:0L, TOP:0L, HANDLER:0L, SELECT:0, VALUE:0 }
//     DStructGDL*  widgbgroup = new DStructGDL( "WIDGET_BGROUP");
//     widgbgroup->InitTag("ID", DLongGDL( widgetID));
//     widgbgroup->InitTag("TOP", DLongGDL( p));
//     widgbgroup->InitTag("HANDLER", DLongGDL( 0));
//     widgbgroup->InitTag("SELECT", DLongGDL( 0));
//     widgbgroup->InitTag("VALUE", DLongGDL( 0));
// 
//     // Push event structure into event variable list
//     std::string varname = "WBGROUP" + i2s(this->WidgetID());
//     DVar *v = new DVar( varname, widgbgroup);
//     eventVarList.push_back(v);
}



GDLWidgetList::GDLWidgetList( WidgetIDT p, EnvT* e, BaseGDL *value, DLong style)
    : GDLWidget( p, e, true, value)
{
    wxMutexGuiEnter();

    GDLWidget* gdlParent = GetWidget( p);
    wxWindow *wxParent = static_cast< wxWindow*>(
                             gdlParent->WxWidget());

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
//                               wxSize( xSize*fontSize.GetWidth(),
//                                       ySize*fontSize.GetHeight()),
                                         choices, style);

        wxSizer *boxSizer = gdlParent->GetSizer();
        boxSizer->Add( list, 0, wxEXPAND | wxALL, 5);

        if ( wxParent != NULL) {
            boxSizer->SetSizeHints( wxParent);
        }

	this->wxWidget = list;
    wxMutexGuiLeave();
}

//GDLWidgetDropList::GDLWidgetDropList( WidgetIDT p, BaseGDL *uV, DStringGDL *value,
GDLWidgetDropList::GDLWidgetDropList( WidgetIDT p, EnvT* e, BaseGDL *value,
                                      const DString& title, DLong style)
    : GDLWidget( p, e, true, value)
{
  //  std::cout << "In DropList: " << widgetID << " " << p << std::endl;
  wxMutexGuiEnter();

  GDLWidget* gdlParent = GetWidget( p);
  wxWindow *wxParent = static_cast< wxWindow*>( gdlParent->WxWidget());

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

    wxComboBox *combo = new wxComboBox( panel, widgetID, wxString((*val)[0].c_str(), wxConvUTF8),
			    wxPoint(xOffset,yOffset), wxSize(xSize,ySize), choices, style);

    wxSizer *boxSizer = gdlParent->GetSizer();
    boxSizer->Add( combo, 0, wxEXPAND | wxALL, 5);

    if ( wxParent != NULL) {
      //      std::cout << "SetSizeHints: " << wxParent << std::endl;
      boxSizer->SetSizeHints( wxParent);
    }
    
    this->wxWidget = combo;
  } // GetMap()
  wxMutexGuiLeave();
}


GDLWidgetText::GDLWidgetText( WidgetIDT p, EnvT* e, DString value,
	bool editable):
  GDLWidget( p, e)
{
  //  std::cout << "In Text: " << widgetID << " " << p << std::endl;
  wxMutexGuiEnter();

  GDLWidget* gdlParent = GetWidget( p);
  wxWindow *wxParent = static_cast< wxWindow*>( gdlParent->WxWidget());

  if (gdlParent->GetMap()) {
    wxPanel *panel = gdlParent->GetPanel();

    wxSize fontSize = wxNORMAL_FONT->GetPixelSize();      
    wxSize widgetSize = wxDefaultSize;
    if( xSize != widgetSize.x)
      widgetSize.x = xSize * fontSize.x;
    if( ySize != widgetSize.y)
      widgetSize.y = ySize * fontSize.y;
    DLong style = editable ? 0 : wxTE_READONLY;
    wxTextCtrl* text = new wxTextCtrl( panel, widgetID, wxString( value.c_str(), wxConvUTF8),
      wxPoint(xOffset,yOffset), widgetSize, style );

    wxBoxSizer *boxSizer = (wxBoxSizer *) gdlParent->GetSizer();
    boxSizer->Add( text, 0, wxEXPAND | wxALL, 5);

    if ( wxParent != NULL) {
      //      std::cout << "SetSizeHints: " << wxParent << std::endl;
      boxSizer->SetSizeHints( wxParent);
    }
    this->wxWidget = text;
  } // GetMap()

  wxMutexGuiLeave();
}


void GDLWidgetText::SetTextValue( DString value)
{
  wxMutexGuiEnter();
  static_cast<wxTextCtrl*>(wxWidget)->SetValue( wxString( value.c_str(), wxConvUTF8));
  //  text->Refresh(); 
  //wxMilliSleep(700); 
  wxMutexGuiLeave();
 }


GDLWidgetLabel::GDLWidgetLabel( WidgetIDT p, EnvT* e, DString value)
: GDLWidget( p, e)
{
  wxMutexGuiEnter();
  //  std::cout << "In Label: " << widgetID << " " << p << std::endl;

  GDLWidget* gdlParent = GetWidget( p);
  wxWindow *wxParent = static_cast< wxWindow*>( gdlParent->WxWidget());
  
  if ( gdlParent->GetMap()) {
    wxPanel *panel = gdlParent->GetPanel();

//     wxSize fontSize = wxNORMAL_FONT->GetPixelSize();      
    wxStaticText* label = new wxStaticText( panel, widgetID, wxString( value.c_str(), wxConvUTF8),
			       wxPoint(xOffset,yOffset), wxSize(xSize, ySize), wxALIGN_CENTRE);

    wxBoxSizer *boxSizer = (wxBoxSizer *) gdlParent->GetSizer();
    boxSizer->Add( label, 0, wxEXPAND | wxALL, 5);

    if ( wxParent != NULL) {
      boxSizer->SetSizeHints( wxParent);
    }
    this->wxWidget = label;
  } // GetMap()
  wxMutexGuiLeave();

}

void GDLWidgetLabel::SetLabelValue( DString value)
{
  wxMutexGuiEnter();
  static_cast<wxStaticText*>(wxWidget)->SetLabel( wxString( value.c_str(), wxConvUTF8));
  wxMutexGuiLeave();
 }

// *** GDLFrame ***
GDLFrame::GDLFrame(wxWindow* parent, wxWindowID id, const wxString& title)
       : wxFrame(parent, id, title)
{
  //  std::cout << "in GDLFrame Constructor" << std::endl;
}

void GDLFrame::OnButton( wxCommandEvent& event)
{
  // Called by EVT_BUTTON in EVENT_TABLE in gdlwidget.cpp

  std::cout << "in OnButton: " << event.GetId() << std::endl;

  WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId());

  // Get XmanagerActiveCommand status
  GDLWidget *baseWidget = GDLWidget::GetWidget( baseWidgetID);
  bool xmanActCom = baseWidget->GetXmanagerActiveCommand();
  //std::cout << "xmanActCom: " << xmanActCom << std::endl;

  // create GDL event struct
  DStructGDL*  widgbut = new DStructGDL( "");
  widgbut->InitTag("ID", DLongGDL( event.GetId()));
  widgbut->InitTag("TOP", DLongGDL( baseWidgetID));
  widgbut->InitTag("HANDLER", DLongGDL( 0));
  widgbut->InitTag("SELECT", DLongGDL( 1));

  if( xmanActCom == false)
  {
    GDLWidget::eventQueue.push(widgbut);
  }
  else
  {
    GDLWidget::readlineEventQueue.push( widgbut);
  }
}

void GDLFrame::OnRadioButton( wxCommandEvent& event)
{
  std::cout << "in OnRadioButton: " << event.GetId() << std::endl;

  WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId());

  // Get XmanagerActiveCommand status
  GDLWidget *baseWidget = GDLWidget::GetWidget( baseWidgetID);
  bool xmanActCom = baseWidget->GetXmanagerActiveCommand();
  //std::cout << "xmanActCom: " << xmanActCom << std::endl;

//   bool selectValue = event.IsSelection();
//   int selectValue = event.GetSelection();

  GDLWidgetBase* gdlParentWidget = static_cast<GDLWidgetBase*>(GDLWidget::GetParent( event.GetId()));
  WidgetIDT lastSelection = gdlParentWidget->GetLastRadioSelection();
  if( lastSelection != GDLWidget::NullID)
  {
    if( lastSelection == event.GetId())      
      return;
    // create GDL event struct
    DStructGDL*  widgbut = new DStructGDL( "WIDGET_BUTTON");
    widgbut->InitTag("ID", DLongGDL( lastSelection));
    widgbut->InitTag("TOP", DLongGDL( baseWidgetID));
    widgbut->InitTag("HANDLER", DLongGDL( 0));
    widgbut->InitTag("SELECT", DLongGDL( 0));
    if( xmanActCom == false)
    {
      GDLWidget::eventQueue.push(widgbut);
    }
    else
    {
      GDLWidget::readlineEventQueue.push( widgbut);
    }
  }
    
  // create GDL event struct
  DStructGDL*  widgbut = new DStructGDL( "WIDGET_BUTTON");
  widgbut->InitTag("ID", DLongGDL( event.GetId()));
  widgbut->InitTag("TOP", DLongGDL( baseWidgetID));
  widgbut->InitTag("HANDLER", DLongGDL( 0));
//   widgbut->InitTag("SELECT", DLongGDL( selectValue ? 1 : 0));
  widgbut->InitTag("SELECT", DLongGDL( 1));

  gdlParentWidget->SetLastRadioSelection(event.GetId());

  if( xmanActCom == false)
  {
    GDLWidget::eventQueue.push(widgbut);
  }
  else
  {
    GDLWidget::readlineEventQueue.push( widgbut);
  }
}

void GDLFrame::OnCheckBox( wxCommandEvent& event)
{
  std::cout << "in OnCheckBox: " << event.GetId() << std::endl;

  WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId());

  // Get XmanagerActiveCommand status
  GDLWidget *baseWidget = GDLWidget::GetWidget( baseWidgetID);
  bool xmanActCom = baseWidget->GetXmanagerActiveCommand();
  //std::cout << "xmanActCom: " << xmanActCom << std::endl;

  bool selectValue = event.IsChecked();
  
  // create GDL event struct
  DStructGDL*  widgbut = new DStructGDL( "WIDGET_BUTTON");
  widgbut->InitTag("ID", DLongGDL( event.GetId()));
  widgbut->InitTag("TOP", DLongGDL( baseWidgetID));
  widgbut->InitTag("HANDLER", DLongGDL( 0));
  widgbut->InitTag("SELECT", DLongGDL( selectValue ? 1 : 0));

  if( xmanActCom == false)
  {
    GDLWidget::eventQueue.push(widgbut);
  }
  else
  {
    GDLWidget::readlineEventQueue.push( widgbut);
  }
}

void GDLFrame::OnComboBox( wxCommandEvent& event)
{
  std::cout << "in OnComboBox: " << event.GetId() << std::endl;

  WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId());

  // Get XmanagerActiveCommand status
  GDLWidget *baseWidget = GDLWidget::GetWidget( baseWidgetID);
  bool xmanActCom = baseWidget->GetXmanagerActiveCommand();
  //std::cout << "xmanActCom: " << xmanActCom << std::endl;

  int selectValue = event.GetSelection();
  
  bool dropList = false;
  
  // create GDL event struct
  DStructGDL*  widgcbox;
  if( !dropList)
  {
    wxString strValue = event.GetString();
    
    widgcbox = new DStructGDL( "WIDGET_COMBOBOX");
    widgcbox->InitTag("ID", DLongGDL( event.GetId()));
    widgcbox->InitTag("TOP", DLongGDL( baseWidgetID));
    widgcbox->InitTag("HANDLER", DLongGDL( 0));
    widgcbox->InitTag("INDEX", DLongGDL( selectValue));
    widgcbox->InitTag("STR", DStringGDL( strValue.c_str().AsChar()));
  }
  else
  {
    widgcbox = new DStructGDL( "WIDGET_DROPLIST");
    widgcbox->InitTag("ID", DLongGDL( event.GetId()));
    widgcbox->InitTag("TOP", DLongGDL( baseWidgetID));
    widgcbox->InitTag("HANDLER", DLongGDL( 0));
    widgcbox->InitTag("INDEX", DLongGDL( selectValue));   
  }

  if( xmanActCom == false)
  {
    GDLWidget::eventQueue.push(widgcbox);
  }
  else
  {
    GDLWidget::readlineEventQueue.push( widgcbox);
  }
}

void GDLFrame::OnListBoxDo( wxCommandEvent& event, DLong clicks)
{
  std::cout << "in OnListBoxDo: " << event.GetId() << "  " << clicks << std::endl;

  WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId());

  // Get XmanagerActiveCommand status
  GDLWidget *baseWidget = GDLWidget::GetWidget( baseWidgetID);
  bool xmanActCom = baseWidget->GetXmanagerActiveCommand();
  //std::cout << "xmanActCom: " << xmanActCom << std::endl;

  int selectValue = event.GetSelection();
  
  // create GDL event struct
  DStructGDL*  widgcbox;
  widgcbox = new DStructGDL( "WIDGET_LIST");
  widgcbox->InitTag("ID", DLongGDL( event.GetId()));
  widgcbox->InitTag("TOP", DLongGDL( baseWidgetID));
  widgcbox->InitTag("HANDLER", DLongGDL( 0));
  widgcbox->InitTag("INDEX", DLongGDL( selectValue));
  widgcbox->InitTag("CLICKS", DLongGDL( clicks));

  if( xmanActCom == false)
  {
    GDLWidget::eventQueue.push(widgcbox);
  }
  else
  {
    GDLWidget::readlineEventQueue.push( widgcbox);
  }
}
void GDLFrame::OnListBox( wxCommandEvent& event)
{
  OnListBoxDo( event, 1);
}
void GDLFrame::OnListBoxDoubleClicked( wxCommandEvent& event)
{
// Note: IDL sends one click AND two click event on double click  
  OnListBoxDo( event, 1);
  OnListBoxDo( event, 2);
}

void GDLFrame::OnIdle( wxIdleEvent&)
{
  //std::cout << "In OnIdle" << std::endl;
  // Refresh();
}


// *** guiThread ***
wxThread::ExitCode GDLGUIThread::Entry()
{
  // Called from PthreadStart() in threadpsx.cpp (wxWidgets)

  // gui loop

//   std::cout << "In thread Entry()" << std::endl;

  wxTheApp->OnRun();
  // Calls GDLApp::OnRun()

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
     delete gdlGUIThread;
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
  exited = true;
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
    std::cout << "In ~GDLGUIThread(). exited: " << exited << std::endl;
#endif
}

#endif

