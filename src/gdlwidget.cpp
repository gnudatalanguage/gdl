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

#ifdef HAVE_LIBWXWIDGETS

#include "gdlwidget.hpp"

// instantiation
WidgetIDT                       GDLWidget::widgetIx;
WidgetListT                 GDLWidget::widgetList;

IMPLEMENT_APP_NO_MAIN( GDLApp)

bool GDLApp::OnInit()
{
  SetExitOnFrameDelete( FALSE);
  return TRUE;
}

// next are the abstraction to access all widgets only by their
// handle ID
// ID for widget (called from widgets constructor)
WidgetIDT GDLWidget::NewWidget( GDLWidget* w)
{
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
void GDLWidget::Init()
{
  widgetIx = wxID_HIGHEST; // use same wx ID and GDL ID 
  //  wxApp::SetExitOnFrameDelete( FALSE);
}




GDLWidget::GDLWidget( WidgetIDT p, BaseGDL* uV, bool s,
		      DLong xO, DLong yO, DLong xS, DLong yS): 
  wxWidget( NULL),
  parent( p), uValue( uV), sensitive( s),
  xOffset( xO), yOffset( yO), xSize( xS), ySize( yS)
{
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
  wxWindow *wxParent = NULL;
  if( parent != 0)
    {
      GDLWidget* gdlParent = GetWidget( parent);
      wxParent = static_cast< wxWindow*>( gdlParent->WxWidget());
    }

  if( modal)
    wxWidget = new wxDialog( wxParent, widgetID, title_);
  else
    wxWidget = new wxFrame( wxParent, widgetID, title_);

}

GDLWidgetBase::~GDLWidgetBase()
{
  // delete all children
  for( cIter cI = children.begin(); cI != children.end(); ++cI)
    {
      delete GetWidget( *cI);
    }
  
   // if TLB destroy wxWidget 
  if( parent == 0)
    delete wxWidget;
}

void GDLWidgetBase::Realize()
{
  
}

#endif
