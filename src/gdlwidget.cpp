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

#include "basegdl.hpp"
#include "dstructgdl.hpp"

#ifdef HAVE_LIBWXWIDGETS

#include "gdlwidget.hpp"

// instantiation
WidgetIDT                   GDLWidget::widgetIx;
WidgetListT                 GDLWidget::widgetList;

VarListT                    eventVarList;

BEGIN_EVENT_TABLE(GDLFrame, wxFrame)
  EVT_BUTTON( wxID_ANY, GDLFrame::OnButton)
END_EVENT_TABLE()


IMPLEMENT_APP_NO_MAIN( GDLApp)

// next are the abstraction to access all widgets only by their
// handle ID
// ID for widget (called from widgets constructor)
WidgetIDT GDLWidget::NewWidget( GDLWidget* w)
{
  //  std::cout << " In NewWidget()" << std::endl;
  WidgetIDT tmpIx = widgetIx;
  widgetList.insert( widgetList.end(),
		     std::pair<WidgetIDT, GDLWidget*>( widgetIx++, w));

  return tmpIx;
}

// removes a widget, (called from widgets destructor -> don't delete)
void GDLWidget::WidgetRemove( WidgetIDT widID)
{
  widgetList.erase( widID); 
}


// widget from ID
GDLWidget* GDLWidget::GetWidget( WidgetIDT widID)
{
  WidgetListT::iterator it=widgetList.find( widID);
  if( it == widgetList.end()) return NULL;
  return it->second;
}

// widget parent from ID
GDLWidget* GDLWidget::GetParent( WidgetIDT widID)
{
  GDLWidget *widget = GetWidget( widID);
  WidgetIDT parentID = widget->parent;
  GDLWidget *parent = GetWidget( parentID);
  return widget;
}

void GDLWidget::SetManaged()
{
  managed = true;
}

bool GDLWidget::GetXmanagerBlock() {
  bool xmanBlock = false;
  WidgetListT::iterator it;
  // (*it).first is widgetID
  // (*it).second is pointer to widget

  bool managed;
  bool xmanActCom;

  for( it = widgetList.begin(); it != widgetList.end(); ++it) {
    // Only consider base widgets
    if ( (*it).second->parent == 0) {
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

// PollEvents
bool GDLWidget::PollEvents( DLong *id, DLong *top, 
			    DLong *handler, DLong *select) {
  bool eventFound = false;
  WidgetListT::iterator it;
  // (*it).first is widgetID
  // (*it).second is pointer to widget

  bool managed;

  for( it = widgetList.begin(); it != widgetList.end(); ++it) {
    // Only consider base widgets
    if ( (*it).second->parent == 0) {
      managed = (*it).second->GetManaged();
      if ( managed) {
	// Get Parent Widget
	GDLWidget *parent = GDLWidget::GetParent( (*it).first);
	DLong nChildren = parent->GetChild( -1);
	if ( nChildren != 0) {
	  for( SizeT j=0; j<nChildren; j++) {
	    WidgetIDT childID = parent->GetChild( j);
	    // Check for button push

	    // Form button event variable name
	    std::ostringstream varname;
	    varname << "WBUT" << childID ;
	    //	    DString vnamestring = varname.rdbuf()->str();
	    //*vname = vnamestring;

	    // Find name and get SELECT
	    DVar *var =
	      FindInVarList( eventVarList, varname.rdbuf()->str().c_str());
	    DStructGDL* s = static_cast<DStructGDL*>( var->Data());
	    *select = (*static_cast<DLongGDL*>
		       (s->GetTag(s->Desc()->TagIndex("SELECT"), 0)))[0];
	    if ( *select) {
	      eventFound = true;
	      *id = (*static_cast<DLongGDL*>
		     (s->GetTag(s->Desc()->TagIndex("ID"), 0)))[0];
	      *top = (*static_cast<DLongGDL*>
		      (s->GetTag(s->Desc()->TagIndex("TOP"), 0)))[0];
	      *handler = (*static_cast<DLongGDL*>
			  (s->GetTag(s->Desc()->TagIndex("HANDLER"), 0)))[0];
	      break;
	    }
	  }
	}
      }
    }
  }
  return eventFound;
}

// Init
void GDLWidget::Init()
{
  // Called by InitObjects() in object.cpp
  //  std::cout << " In GDLWidget::Init()" << std::endl;
  widgetIx = wxID_HIGHEST; // use same wx ID and GDL ID 

  wxInitialize();
}


GDLWidget::GDLWidget( WidgetIDT p, BaseGDL* uV, bool s,
		      DLong xO, DLong yO, DLong xS, DLong yS): 
  wxWidget( NULL),
  parent( p), uValue( uV), sensitive( s),
  xOffset( xO), yOffset( yO), xSize( xS), ySize( yS)
{
  managed = false;
  widgetID = NewWidget( this);
  if( parent != 0)
    {
      GDLWidgetBase* base = dynamic_cast< GDLWidgetBase*>( GetWidget( parent));
      assert( base != NULL); // should be already checked elsewhere
      base->AddChild( widgetID);
    }
}

GDLWidget::~GDLWidget()
{
  // std::cout << "in ~GDLWidget(): " << std::endl;
  managed = false;

  if( parent != 0) 
    {
      GDLWidgetBase* base = dynamic_cast< GDLWidgetBase*>( GetWidget( parent));
      assert( base != NULL);
      base->RemoveChild( widgetID);
    }
  delete uValue;
  WidgetRemove( widgetID);
}

GDLWidgetBase::GDLWidgetBase( WidgetIDT p, BaseGDL* uV, bool s,
			      DLong xO, DLong yO, DLong xS, DLong yS): 
  GDLWidget( p, uV, s, xO, yO, xS, yS)
{}

GDLWidgetBase::GDLWidgetBase( WidgetIDT parentID, 
			      BaseGDL* uvalue, DString uname,
			      bool sensitive, bool mapWid,
			      WidgetIDT mBarID, bool modal_, 
			      WidgetIDT group_leader,
			      DLong col, DLong row,
			      long events,
			      int exclusiveMode, 
			      bool floating,
			      DString event_func, DString event_pro,
			      DString pro_set_value, DString func_get_value,
			      DString notify_realize, DString kill_notify,
			      DString resource_name, DString rname_mbar,
			      DString title_,
			      DLong frame, DLong units,
			      DString display_name,
			      DLong xpad, DLong ypad,
			      DLong xoffset, DLong yoffset,
			      DLong xsize, DLong ysize,
			      DLong scr_xsize, DLong scr_ysize,
			      DLong x_scroll_size, DLong y_scroll_size):
  GDLWidget( parentID, uvalue, sensitive, xoffset, yoffset, 0, 0),
  modal( modal_), mbarID( mBarID)
{
  //  std::cout << "In GDLWidgetBase::GDLWidgetBase" << std::endl;

  static bool first=true;
  if (first) {
    thread = new guiThread();

    // Defined in threadpsx.cpp (wxWidgets)
    thread->Create();
    thread->Run();

    //  wxMutexGuiEnter();
    first = false;
  }

  xmanActCom = false;

  // GUI lock defined in threadpsx.cpp
  //  std::cout << "wxMutexGuiEnter()" << std::endl;
  wxMutexGuiEnter();

  wxWindow *wxParent = NULL;
  if( parent != 0)
    {
      GDLWidget* gdlParent = GetWidget( parent);
      wxParent = static_cast< wxWindow*>( gdlParent->WxWidget());
    }

  if( modal)
    wxWidget = new wxDialog( wxParent, widgetID, title_);
  else {

    GDLFrame *frame = new GDLFrame( wxParent, widgetID, title_);
    ((wxFrame *) frame)->SetSize( xsize, ysize);
    wxWidget = frame;
    //    wxWidget = new wxFrame( wxParent, widgetID, title_);
  }
}

GDLWidgetBase::~GDLWidgetBase()
{
  // Close widget frame
  //  std::cout << "In ~GDLWidgetBase(): " << this->wxWidget << std::endl;
  ((wxFrame *) this->wxWidget)->Close( true);

  // Note: iterator for loop doesn't work when deleting widget
  cIter cI = children.begin();
  for( SizeT i=0; i<children.size(); i++) {
    delete GetWidget( *cI++);
  }

  // if TLB destroy wxWidget 
  //  if( parent == 0)
  //delete wxWidget;
}

void GDLWidgetBase::Realize()
{
  wxFrame *frame = (wxFrame *) this->wxWidget;
  bool stat = frame->Show(true);
  //  wxString nme = frame->GetName();
  //std::cout << frame->IsShown() << std::endl;

  // GUI unlock defined in threadpsx.cpp
  //  std::cout << "wxMutexGuiLeave()" << std::endl;
  wxMutexGuiLeave();
}


DLong GDLWidgetBase::GetChild( DLong childIx)
{
  if ( childIx == -1)
    return ((DLong) children.size());
  else
    return ((DLong) children[childIx]);
}


void GDLWidgetBase::SetXmanagerActiveCommand()
{
  xmanActCom = true;
}


GDLWidgetButton::GDLWidgetButton( WidgetIDT p, DString value):
  GDLWidget( p, NULL, 0, 0, 0, 0, 0)
{
  //  std::cout << "In Button: " << widgetID << " " << p << std::endl;

  GDLWidget* gdlParent = GetWidget( p);
  wxWindow *wxParent = static_cast< wxWindow*>( gdlParent->WxWidget());

  wxFrame *frame = (wxFrame *) wxParent;
  wxButton *button = new wxButton( frame, widgetID, _T( value.c_str()));

  // Generate event structure
  DStructGDL*  widgbut = new DStructGDL( "WIDGET_BUTTON");
  widgbut->InitTag("ID", DLongGDL( widgetID));
  widgbut->InitTag("TOP", DLongGDL( p));
  widgbut->InitTag("HANDLER", DLongGDL( 0));
  widgbut->InitTag("SELECT", DLongGDL( 0));

  // Push event structure into event variable list
  std::ostringstream varname;
  varname << "WBUT" << this->WidgetID();
  DVar *v = new DVar( varname.rdbuf()->str().c_str(), widgbut);
  eventVarList.push_back(v);
}


// *** GDLFrame ***
GDLFrame::GDLFrame(wxWindow* parent, wxWindowID id, const wxString& title)
       : wxFrame(parent, id, title)
{
  //  std::cout << "in GDLFrame Constructor" << std::endl;
}

void GDLFrame::OnButton(wxCommandEvent& event)
{
  // Called by EVT_BUTTON in EVENT_TABLE in gdlwidget.cpp

  //  std::cout << "in OnButton: " << event.GetId() << std::endl;

  // Form button event variable name
  std::ostringstream varname;
  varname << "WBUT" << event.GetId();

  // Find name and set SELECT tag to 1
  DVar *var=FindInVarList( eventVarList, varname.rdbuf()->str().c_str());
  DStructGDL* s = static_cast<DStructGDL*>( var->Data());
  (*static_cast<DLongGDL*>
   (s->GetTag(s->Desc()->TagIndex("SELECT"), 0)))[0] = 1;
}


// *** guiThread ***
void *guiThread::Entry()
{
  // Called from PthreadStart() in threadpsx.cpp (wxWidgets)

  // gui loop

  //  std::cout << "In thread Entry()" << std::endl;

  wxTheApp->OnRun();
  // Calls GDLApp::OnRun()

  return NULL;
}

int GDLApp::OnRun()
{
  // Called by guiThread::Entry()

  //  std::cout << " In OnRun()" << std::endl;

  int exitcode = wxApp::OnRun();
  // Note: Calls wxAppBase::OnRun() in appcmn.cpp (wxWidgets)
  // MainLoop() etc

  if (exitcode!=0)
    return exitcode;
}

int GDLApp::OnExit()
{
  // Called by exitgdl() in basic_pro.cpp

  //  std::cout << " In GDLApp::OnExit()" << std::endl;
  
  //bool running = thread->IsRunning();
  //std::cout << "running: " << running << std::endl;

  //  wxMutexGuiLeave();

  // Defined in guiThread::OnExit() in gdlwidget.cpp
  if (thread != NULL)
      thread->OnExit( thread);

  return 0;
}


void guiThread::OnExit( guiThread *thread)
{
  // Called by GDLApp::OnExit() in gdlwidget.cpp

  //  std::cout << "In guiThread::OnExit()" << std::endl;

  delete thread;
}

#endif

