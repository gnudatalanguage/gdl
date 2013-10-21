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

// #define GDL_DEBUG_WIDGETS


BEGIN_EVENT_TABLE(GDLFrame, wxFrame)
  EVT_MENU(wxID_ANY, GDLFrame::OnButton)
  EVT_BUTTON( wxID_ANY, GDLFrame::OnButton)
  EVT_RADIOBUTTON(wxID_ANY, GDLFrame::OnRadioButton)
  EVT_IDLE( GDLFrame::OnIdle)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(GDLWindow, wxWindow)
  EVT_PAINT(GDLWindow::OnPaint)
  EVT_SHOW(GDLWindow::OnShow)
  EVT_CLOSE(GDLWindow::OnClose)
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
// , memPlotDC( NULL)
// , memPlotDCBitmap( NULL)
{
  pstreamIx = GraphicsDevice::GetGUIDevice()->WAdd();
  if( pstreamIx == -1)
    throw GDLException("Failed to allocate GUI stream.");

//   memPlotDC = new wxMemoryDC();
// //   memPlotDC->SelectObject( wxNullBitmap );
//   memPlotDCBitmap = new wxBitmap( size.x, size.y, -1 );
//   memPlotDC->SelectObject( *memPlotDCBitmap);

//   pstream = new GDLWXStream( memPlotDC, width, height);

  drawSize = size;
  bool success = GraphicsDevice::GetGUIDevice()->GUIOpen( pstreamIx, drawSize.x, drawSize.y);  
  if( !success)
  {
//     memPlotDC->SelectObject( wxNullBitmap );
//     delete memPlotDCBitmap;
//     delete memPlotDC;
    throw GDLException("Failed to open GUI stream: "+i2s(pstreamIx));
  }
  pstreamP = static_cast<GDLWXStream*>(GraphicsDevice::GetGUIDevice()->GetStreamAt( pstreamIx));
  pstreamP->SetGDLWindow( this);
  
  m_dc = pstreamP->GetDC();
}

GDLWindow::~GDLWindow()
{ 
  std::cout << "~GDLWindow: " << this << std::endl;
//   memPlotDC->SelectObject( wxNullBitmap );
//   delete memPlotDCBitmap;
//   delete memPlotDC;
  if( pstreamP != NULL)
      pstreamP->SetValid(false);
}

// GDLWXStream* GDLWindow::PStream()
// {
//   if( pstreamIx == -1)
//   {
//     pstreamIx = Graphics::GetGUIDevice()->WAdd();
//     bool success = Graphics::GetGUIDevice()->GUIOpen( pstreamIx, memPlotDC, drawSize.x, drawSize.y);  
//     if( !success)
//       return NULL;
//     pstreamP = static_cast<GDLWXStream*>(Graphics::GetGUIDevice()->GetStreamAt( pstreamIx));
//   }
//   return pstreamP;
// }
  
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

//     if( pstreamIx == -1)
//     {
//       pstreamIx = Graphics::GetGUIDevice()->WAdd();
//     }
//     if( pstreamP == NULL)
//     {
//       bool success = Graphics::GetGUIDevice()->GUIOpen( pstreamIx, memPlotDC, drawSize.x, drawSize.y);  
//       if( !success)
// 	return;
//       pstreamP = static_cast<GDLWXStream*>(Graphics::GetGUIDevice()->GetStreamAt( pstreamIx));
//     }
//     assert( pstreamP != NULL);
    
//     int width, height;
//     GetSize( &width, &height );
// 
// //     assert( PStream() != NULL);
//     
//     // Check if we window was resized (or dc is invalid)
//     if( (drawSize.x != width) || (drawSize.y !=height)) 
//     {
// 	memPlotDC->SelectObject( wxNullBitmap );
// 
//         if( memPlotDCBitmap )
//             delete memPlotDCBitmap;
//         memPlotDCBitmap = new wxBitmap( width, height, -1 );
// 
// 	memPlotDC->SelectObject( *memPlotDCBitmap);
// 
// //         PStream()->SetSize( width, height);
// //         PStream()->replot();
// 
//         drawSize = wxSize( width, height);
//     }

//     PStream()->RenewPlot();
//     pstreamP->SetSize( width, height);
//     pstreamP->replot();

    wxPaintDC dc( this);
    dc.SetDeviceClippingRegion( GetUpdateRegion() );
//     dc.Blit( 0, 0, width, height, memPlotDC, 0, 0 );

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



void getSizer( DLong col, DLong row, DLong frameBox, 
	       wxPanel *panel, wxSizer **sizer) {
  if ( frameBox == 0) {
    if ( row == 0) {
      *sizer = new wxBoxSizer( wxVERTICAL);
    } else if ( row != 0 && col == 0) {
      *sizer = new wxBoxSizer( wxHORIZONTAL);
    } else {
      std::cout << "Shouldn't be here" << std::endl;
      exit(2);
    }
  } else {

    wxStaticBox *box = 
      new wxStaticBox( panel, wxID_ANY, _T(""));
    if ( row == 0) {
      *sizer = new wxStaticBoxSizer( box, wxVERTICAL);
    } else if ( row != 0 && col == 0) {
      *sizer = new wxStaticBoxSizer( box, wxHORIZONTAL);
    } else {
      std::cout << "Shouldn't be here" << std::endl;
      exit(2);
    }
  }
  //  std::cout << "Creating Sizer in getSizer: " << *sizer << std::endl;
}

// next are the abstraction to access all widgets only by their
// handle ID
// ID for widget (called from widgets constructor)
WidgetIDT GDLWidget::NewWidget( GDLWidget* w)
{
// //  std::cout << " In NewWidget()" << std::endl;
// widgetList.insert( widgetList.end(), std::pair<WidgetIDT, GDLWidget*>( widgetIx, w));
// return ++widgetIx;
  wxWindowID newID = wxWindow::NewControlId();
  w->widgetID = newID;
  widgetList.insert( widgetList.end(), std::pair<WidgetIDT, GDLWidget*>( newID, w));
  
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "inserted: ID: " << newID << "  parentID: " << w->parentID << "   uname: " << w->uName << std::endl;
#endif  
  return newID; // compiler shut-up
}

// removes a widget, (called from widgets destructor -> don't delete)
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
WidgetIDT GDLWidget::GetTopLevelBase( WidgetIDT widID)
{
  GDLWidget *widget;
  WidgetIDT actID = widID;
  while ( 1) {
    widget = GetWidget( actID);
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


GDLWidget::GDLWidget( WidgetIDT p, BaseGDL* uV, BaseGDL* vV, bool s, bool mp,
		      DLong xO, DLong yO, DLong xS, DLong yS, const DString& uname): 
  wxWidget( NULL),
  parentID( p), uValue( uV), vValue( vV), sensitive( s), map( mp)
  , buttonSet(false)
  , exclusiveMode(0)
  , xOffset( xO), yOffset( yO), xSize( xS), ySize( yS)
  , topWidgetSizer(NULL)
  , widgetSizer(NULL)
  , widgetPanel(NULL)
  , uName( uname)
{
  managed = false;
  // TODO exception savety
  widgetID = NewWidget( this);
  if( parentID != GDLWidget::NullID)
    {
      GDLWidget* gdlParent = GetWidget( parentID);
      GDLWidgetBase* base = dynamic_cast< GDLWidgetBase*>( gdlParent);
//       assert( base != NULL); // should be already checked elsewhere
      if( base != NULL)
	base->AddChild( widgetID);
      else
      {
	WidgetIDT topID = GetTopLevelBase( widgetID);
	GDLWidget* top = GetWidget( topID);
	GDLWidgetBase* tlb = dynamic_cast< GDLWidgetBase*>(top);
	if( tlb != NULL)
	  tlb->AddChild( widgetID);
      }
    }
}

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

GDLWidgetBase::GDLWidgetBase( WidgetIDT p, BaseGDL* uV, BaseGDL* vV, 
			      bool s, bool mp,
			      DLong xO, DLong yO, DLong xS, DLong yS): 
  GDLWidget( p, uV, vV, s, mp, xO, yO, xS, yS)
{}

GDLWidgetBase::GDLWidgetBase( WidgetIDT parentID, 
			      BaseGDL* uvalue, const DString& uname,
			      bool sensitive, bool mapWid,
			      WidgetIDT& mBarIDInOut, bool modal_, 
			      WidgetIDT group_leader,
			      DLong col, DLong row,
			      long events,
			      int exclusiveMode_, 
			      bool floating_,
			      const DString& event_func, const DString& event_pro,
			      const DString& pro_set_value, const DString& func_get_value,
			      const DString& notify_realize, const DString& kill_notify,
			      const DString& resource_name, const DString& rname_mbar,
			      const DString& title_,
			      DLong frameBox, DLong units,
			      const DString& display_name,
			      DLong xpad, DLong ypad,
			      DLong xoffset, DLong yoffset,
			      DLong xsize, DLong ysize,
			      DLong scr_xsize, DLong scr_ysize,
			      DLong x_scroll_size, DLong y_scroll_size)
  : GDLWidget( parentID, uvalue, NULL, sensitive, map, xoffset, yoffset, xsize, ysize, uname)
  , modal( modal_)
  , mbarID( mBarIDInOut)
{
  //  std::cout << "In GDLWidgetBase::GDLWidgetBase: " << widgetID << std::endl
//   this->SetExclusiveMode( exclusiveMode_);

  xmanActCom = false;
  wxWindow *wxParent = NULL;

  wxSizer *sizer;
//   wxSizer **sizerPtr;
//   sizerPtr = &sizer;

  // If first base widget ...
  if ( parentID == 0) 
  {
    // thread need to be created here (in realize it is too late)
    if (gdlGUIThread != NULL)
    {
      if( gdlGUIThread->Exited() || !gdlGUIThread->IsAlive())
      {
	gdlGUIThread->Wait();
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
    GDLFrame *frame = new GDLFrame( wxParent, widgetID, wxString(title_.c_str(), wxConvUTF8));
    wxWidget = frame;

    frame->SetSize( xsize, ysize);

    if( mbarID != 0)
    {
      GDLWidgetMBar* mBar = new GDLWidgetMBar( widgetID); 
      // already called in constructor:
      // mbarID = GDLWidget::NewWidget( mBar);
      mbarID = mBar->WidgetID();
      mBarIDInOut = mbarID;
      
//       wxMenuBar* m = static_cast<wxMenuBar*>(GDLWidget::GetWidget( mbarID)->WxWidget());
      frame->SetMenuBar( static_cast<wxMenuBar*>( mBar->WxWidget()));

//       GDLWidget::GetWidget( mbarID)->InitParentID( widgetID);

      frame->SetSize( xsize, ysize);
    }

    wxPanel *panel = new wxPanel( frame, wxID_ANY);
    widgetPanel = panel;
    //    std::cout << "Creating Panel: " << panel << std::endl;

    getSizer( col, row, frameBox, panel, &sizer);
    widgetSizer = sizer;

    topWidgetSizer = sizer;
    //std::cout << "SetSizer: " << *sizerPtr << std::endl;
    panel->SetSizer( sizer);

    this->SetMap( mapWid);

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
    if ( mapWid) mapWid = gdlParent->GetMap();
    this->SetMap( mapWid);

    // Set exclusiveMode
    // If exclusive then set to -1 to signal first radiobutton
    if ( exclusiveMode == 1)
      this->SetExclusiveMode( -exclusiveMode);
    else
      this->SetExclusiveMode(  exclusiveMode);

    if ( mapWid) {
      if ( frameBox == 0) {
	if ( row == 0) {
	  sizer = new wxBoxSizer( wxVERTICAL);
	} else if ( row != 0 && col == 0) {
	  sizer = new wxBoxSizer( wxHORIZONTAL);
	} else {
	  std::cout << "Shouldn't be here" << std::endl;
	  exit(2);
	}
      } else {
	wxStaticBox *box = new wxStaticBox( panel, wxID_ANY, _T(""));
	if ( row == 0) {
	  sizer = new wxStaticBoxSizer( box, wxVERTICAL);
	} else if ( row != 0 && col == 0) {
	  sizer = new wxStaticBoxSizer( box, wxHORIZONTAL);
	} else {
	  std::cout << "Shouldn't be here" << std::endl;
	  exit(2);
	}
      }

      widgetSizer = sizer;
      //      std::cout << "Creating Sizer2: " << sizer << std::endl;
    
      if ( frameBox == 0) {
	wxBoxSizer *parentSizer = (wxBoxSizer *) gdlParent->GetSizer();
	//    std::cout << "Getting Parent Sizer:" << parentSizer << std::endl;
	parentSizer->Add( sizer, 0, wxEXPAND|wxALL, 5);
      } else {
	wxStaticBoxSizer *parentSizer = 
	  (wxStaticBoxSizer *) gdlParent->GetSizer();
	//    std::cout << "Getting Parent Sizer:" << parentSizer << std::endl;
	parentSizer->Add( sizer, 0, wxEXPAND|wxALL, 5);
      }

      if( modal)
	wxWidget = new wxDialog( wxParent, widgetID, wxString(title_.c_str(), wxConvUTF8));

    } // if (mapWid == true)
  }
  wxMutexGuiLeave();
}

GDLWidgetBase::~GDLWidgetBase()
{
  std::cout << "In ~GDLWidgetBase(). wxWidget: " << this->wxWidget << std::endl;
  wxMutexGuiEnter();

  // Note: iterator for loop doesn't work when deleting widget
  for( SizeT i=0; i<children.size(); i++) 
  {
    delete GetWidget( children[i]);
  }
  std::cout << "in ~GDLWidgetBase(). wxWidget: " << this->wxWidget << std::endl;

  // Close widget frame
  if( this->parentID == 0)
  {
    // this seems to provoke: LIBDBUSMENU-GLIB-WARNING **: Trying to remove a child that doesn't believe we're it's parent.
    // on wxWidgets < 2.9.5
    { std::cout << "GDLFrame::Destroy(): " << this->wxWidget << std::endl;}

    ((GDLFrame *) this->wxWidget)->Destroy();
  }
  // if TLB destroy wxWidget 
  //  if( parent == 0)
  //delete wxWidget;
  
  wxMutexGuiLeave();
}

void GDLWidgetBase::Realize( bool map)
{
//   if (gdlGUIThread != NULL)
//   {
//     if( gdlGUIThread->Exited() || !gdlGUIThread->IsAlive())
//     {
//       gdlGUIThread->Wait();
//       gdlGUIThread = NULL;
//     }
//   }
//   if (gdlGUIThread == NULL)
//   {
// //       gdlGUIThread->Exit(); // delete itself
// 
//   // Defined in threadpsx.cpp (wxWidgets)
//     gdlGUIThread = new GDLGUIThread();
// #ifdef GDL_DEBUG_WIDGETS
//     std::cout << "Created thread: " << gdlGUIThread << std::endl;
// #endif    
// 
//     gdlGUIThread->Create();
//     gdlGUIThread->Run();
//   }

  wxMutexGuiEnter();

  GDLFrame *frame = (GDLFrame *) this->wxWidget;
  bool stat = frame->Show( map);
  // std::cout << frame->IsShown() << std::endl;
  // std::cout << "wxMutexGuiLeave()" << std::endl;
  wxMutexGuiLeave();
}


DLong GDLWidgetBase::GetChild( DLong childIx)
{
  if ( childIx == -1)
    return ((DLong) children.size());
  else
    return ((DLong) children[childIx]);
}


// void  GDLWidgetBase::SetEventPro( DString eventPro)
// {
//   std::cout << "Setting up event handler: " << eventPro.c_str() << std::endl;
//   eventHandler = eventPro;
// }



GDLWidgetButton::GDLWidgetButton( WidgetIDT p, BaseGDL *uV, const DString& value, const DString& uname):
  GDLWidget( p, uV, NULL, false, false, 0, 0, 0, 0, uname)
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

      wxButton *button;
      wxRadioButton *radioButton;
      wxCheckBox *checkBox;
      wxBoxSizer *boxSizer = (wxBoxSizer *) gdlParent->GetSizer();

      if ( gdlParent->GetExclusiveMode() == 0) {
	button = new wxButton( panel, widgetID, wxString(value.c_str(), wxConvUTF8));
	boxSizer->Add( button, 0, wxEXPAND | wxALL, 5);
	this->wxWidget = button;
      } else if ( gdlParent->GetExclusiveMode() == -1) {
	radioButton = new wxRadioButton( panel, widgetID, wxString( value.c_str(), wxConvUTF8),
					wxDefaultPosition, wxDefaultSize,
					wxRB_GROUP);
	gdlParent->SetExclusiveMode( 1);
	boxSizer->Add( radioButton, 0, wxEXPAND | wxALL, 5);
	this->wxWidget = radioButton;
      } else if ( gdlParent->GetExclusiveMode() == 1) {
	radioButton = new wxRadioButton( panel, widgetID, wxString(value.c_str(), wxConvUTF8));
	boxSizer->Add( radioButton, 0, wxEXPAND | wxALL, 5);
	this->wxWidget = radioButton;
      } else if ( gdlParent->GetExclusiveMode() == 2) {
	checkBox = new wxCheckBox( panel, wxID_ANY, wxString(value.c_str(), wxConvUTF8));
	boxSizer->Add( checkBox, 0, wxEXPAND | wxALL, 5);
	this->wxWidget = checkBox;
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
    GDLWidget( p, uV, NULL, 0, 0, 0, 0, 0)
{
  wxMutexGuiEnter();

  GDLWidget* gdlParent = GetWidget( p);
    wxWindow *wxParent = static_cast< wxWindow*>(
                             gdlParent->WxWidget());

    if (gdlParent->GetMap()) {
        wxPanel *panel = gdlParent->GetPanel();

        wxBoxSizer *boxSizer = (wxBoxSizer *) gdlParent->GetSizer();

        //DStringGDL* buttonval = static_cast<DStringGDL*>( buttonvalue);

        DLong n = names->N_Elements();
        wxString *choices = new wxString[n];
        for( SizeT i=0; i<n; ++i) choices[i] = wxString((*names)[i].c_str(),wxConvUTF8);

        wxStaticText* label = new wxStaticText( panel, wxID_ANY,
                                                wxString(labeltop.c_str(), wxConvUTF8),
                                                wxPoint(10, 10),
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
        case NORMAL:
        {
            for( SizeT i=0; i<n; ++i) {
                wxButton* button = new wxButton(panel, widgetID, choices[i]);
                buttonSizer->Add( button, 0, wxEXPAND | wxALL, 5);
            }
        }
        break;
        case EXCLUSIVE:
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
        case NONEXCLUSIVE:
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
    } // get map
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



GDLWidgetList::GDLWidgetList( WidgetIDT p, BaseGDL *uV, BaseGDL *value,
                              DLong xSize, DLong ySize, DLong style):
    GDLWidget( p, uV, NULL, 0, 0, 0, 0, 0)
{
  wxMutexGuiEnter();

  GDLWidget* gdlParent = GetWidget( p);
    wxWindow *wxParent = static_cast< wxWindow*>(
                             gdlParent->WxWidget());
    wxListBox *list;
    if (gdlParent->GetMap()) {
        wxPanel *panel = gdlParent->GetPanel();

        DStringGDL* val = static_cast<DStringGDL*>( value);

        DLong n= val->N_Elements();
        wxString *choices = new wxString[n];
        for( SizeT i=0; i<n; ++i) choices[i] = wxString((*val)[i].c_str(), wxConvUTF8);

        wxSize fontSize = wxNORMAL_FONT->GetPixelSize();

        list = new wxListBox( panel, widgetID, wxDefaultPosition,
                              wxSize( xSize*fontSize.GetWidth(),
                                      ySize*fontSize.GetHeight()),
                              n, choices, style
                            );
        wxBoxSizer *boxSizer = (wxBoxSizer *) gdlParent->GetSizer();
        boxSizer->Add( list, 0, wxEXPAND | wxALL, 5);

        if ( wxParent != NULL) {
            boxSizer->SetSizeHints( wxParent);
        }
    } // get map
  wxMutexGuiLeave();
//     // Generate event structure
//     DStructGDL*  widglist = new DStructGDL( "WIDGET_LIST");
//     widglist->InitTag("ID", DLongGDL( widgetID));
//     widglist->InitTag("TOP", DLongGDL( p));
//     widglist->InitTag("HANDLER", DLongGDL( 0));
//     widglist->InitTag("SELECT", DLongGDL( 0));
// 
//     // Push event structure into event variable list
//     std::string varname = "WLIST" + i2s(this->WidgetID());
//     DVar *v = new DVar( varname, widglist);
//     eventVarList.push_back(v);
}

//GDLWidgetDropList::GDLWidgetDropList( WidgetIDT p, BaseGDL *uV, DStringGDL *value,
GDLWidgetDropList::GDLWidgetDropList( WidgetIDT p, BaseGDL *uV, BaseGDL *value,
				      DString title, DLong xSize, DLong style):
  GDLWidget( p, uV, NULL, 0, 0, 0, 0, 0)
{
  //  std::cout << "In DropList: " << widgetID << " " << p << std::endl;
  wxMutexGuiEnter();

  GDLWidget* gdlParent = GetWidget( p);
  wxWindow *wxParent = static_cast< wxWindow*>( gdlParent->WxWidget());

  wxComboBox *combo;

  if (gdlParent->GetMap()) {
    wxPanel *panel = gdlParent->GetPanel();

    DStringGDL* val = static_cast<DStringGDL*>( value);

    DLong n= val->N_Elements();
    wxString *choices = new wxString[n];
    for( SizeT i=0; i<n; ++i) choices[i] = wxString((*val)[i].c_str(), wxConvUTF8);

    combo = new wxComboBox( panel, widgetID, wxString( (const wchar_t*) choices[0].c_str(), wxConvUTF8),
			    wxDefaultPosition, wxDefaultSize,
			    n, choices, style);

    wxBoxSizer *boxSizer = (wxBoxSizer *) gdlParent->GetSizer();
    boxSizer->Add( combo, 0, wxEXPAND | wxALL, 5);

    if ( wxParent != NULL) {
      //      std::cout << "SetSizeHints: " << wxParent << std::endl;
      boxSizer->SetSizeHints( wxParent);
    }
  } // GetMap()

  wxMutexGuiLeave();
//   // Generate event structure
//   DStructGDL*  widgdlist = new DStructGDL( "WIDGET_DROPLIST");
//   widgdlist->InitTag("ID", DLongGDL( widgetID));
//   widgdlist->InitTag("TOP", DLongGDL( p));
//   widgdlist->InitTag("HANDLER", DLongGDL( 0));
//   widgdlist->InitTag("SELECT", DLongGDL( 0));
// 
//   // Push event structure into event variable list
//   std::string varname = "WDLIST" + i2s(this->WidgetID());
//   DVar *v = new DVar( varname, widgdlist);
//   eventVarList.push_back(v);
}


GDLWidgetText::GDLWidgetText( WidgetIDT p, BaseGDL *uV, DString value,
															DLong xSize, bool editable):
  GDLWidget( p, uV, NULL, 0, 0, 0, 0, 0)
{
  //  std::cout << "In Text: " << widgetID << " " << p << std::endl;
  wxMutexGuiEnter();

  GDLWidget* gdlParent = GetWidget( p);
  wxWindow *wxParent = static_cast< wxWindow*>( gdlParent->WxWidget());


  if (gdlParent->GetMap()) {
    wxPanel *panel = gdlParent->GetPanel();
		DLong style = editable ? 0 : wxTE_READONLY;
    text = new wxTextCtrl( panel, widgetID, wxString( value.c_str(), wxConvUTF8),
													 wxDefaultPosition, wxSize( xSize*5,
																											wxDefaultCoord), style );

    wxBoxSizer *boxSizer = (wxBoxSizer *) gdlParent->GetSizer();
    boxSizer->Add( text, 0, wxEXPAND | wxALL, 5);

    if ( wxParent != NULL) {
      //      std::cout << "SetSizeHints: " << wxParent << std::endl;
      boxSizer->SetSizeHints( wxParent);
    }
  } // GetMap()

  wxMutexGuiLeave();
//   // Generate event structure
//   DStructGDL*  widgtxt = new DStructGDL( "WIDGET_TEXT");
//   widgtxt->InitTag("ID", DLongGDL( widgetID));
//   widgtxt->InitTag("TOP", DLongGDL( p));
//   widgtxt->InitTag("HANDLER", DLongGDL( 0));
//   widgtxt->InitTag("SELECT", DLongGDL( 0));
// 
//   // Push event structure into event variable list
//   std::string varname = "WTXT" + i2s(this->WidgetID());
//   DVar *v = new DVar( varname, widgtxt);
//   eventVarList.push_back(v);
}


void GDLWidgetText::SetTextValue( DString value)
{
  wxMutexGuiEnter();
  text->SetValue( wxString( value.c_str(), wxConvUTF8));
  //  text->Refresh(); 
  //wxMilliSleep(700); 
  wxMutexGuiLeave();
 }


GDLWidgetLabel::GDLWidgetLabel( WidgetIDT p, BaseGDL *uV, DString value,
				DLong xSize):
  GDLWidget( p, uV, NULL, 0, 0, 0, 0, 0)
{
  wxMutexGuiEnter();
  //  std::cout << "In Label: " << widgetID << " " << p << std::endl;

  GDLWidget* gdlParent = GetWidget( p);
  wxWindow *wxParent = static_cast< wxWindow*>( gdlParent->WxWidget());
  
  if ( gdlParent->GetMap()) {
    wxPanel *panel = gdlParent->GetPanel();
    label = new wxStaticText( panel, wxID_ANY, wxString( value.c_str(), wxConvUTF8),
			      wxPoint(10, 10), wxDefaultSize, wxALIGN_CENTRE);

    wxBoxSizer *boxSizer = (wxBoxSizer *) gdlParent->GetSizer();
    boxSizer->Add( label, 0, wxEXPAND | wxALL, 5);

    if ( wxParent != NULL) {
      //      std::cout << "SetSizeHints: " << wxParent << std::endl;
      boxSizer->SetSizeHints( wxParent);
    }
  } // GetMap()
  wxMutexGuiLeave();

}

void GDLWidgetLabel::SetLabelValue( DString value)
{
  wxMutexGuiEnter();
  label->SetLabel( wxString( value.c_str(), wxConvUTF8));
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

  // Get XmanagerActiveCommand status
  WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId());
  GDLWidget *baseWidget = GDLWidget::GetWidget( baseWidgetID);
  bool xmanActCom = baseWidget->GetXmanagerActiveCommand();
  //std::cout << "xmanActCom: " << xmanActCom << std::endl;

  // create GDL event struct
  DStructGDL*  widgbut = new DStructGDL( "WIDGET_BUTTON");
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

// int GDLApp::Exit()
// {
//   // Called by exitgdl() in basic_pro.cpp
// 
//   std::cout << " In GDLApp::Exit()" << std::endl;
//   
//   //bool running = thread->IsRunning();
//   //std::cout << "running: " << running << std::endl;
// 
//   // Defined in guiThread::OnExit() in gdlwidget.cpp
//   //  std::cout << "Exiting thread (GDLApp::OnExit): " << thread << std::endl;
//   if (gdlGUIThread != NULL)
//   {
//      gdlGUIThread->Exit();
//      gdlGUIThread = NULL;
//   }
// 
//   return 0;
// }


void GDLGUIThread::OnExit()
{
  // Called by GDLApp::OnExit() in gdlwidget.cpp
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "In guiThread::OnExit()." << std::endl;
  std::cout << "IsMainThread: " << wxIsMainThread() << std::endl;
#endif
  exited = true;
}

void GDLGUIThread::Exit()
{
  // Called by GDLApp::OnExit() in gdlwidget.cpp
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "In GDLGUIThread::Exit()." << std::endl;
#endif
  delete this;
}

#endif

