/***************************************************************************
             gdlwidgetdraw.cpp  - draw widget basics  
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

#include "gdlwidget.hpp"

#include "widget.hpp"

// #define GDL_DEBUG_WIDGETS

GDLWidgetDraw::~GDLWidgetDraw()
{}


GDLWidgetDraw::GDLWidgetDraw( WidgetIDT parentID, 
			      BaseGDL* uvalue, const DString& uname,
			      bool sensitive, WidgetIDT group_leader,
			      const DString& event_func, const DString& event_pro,
			      const DString& pro_set_value, const DString& func_get_value,
			      const DString& notify_realize, const DString& kill_notify,
			      DLong frame, DLong units,
			      DLong xoffset, DLong yoffset,
			      DLong xsize, DLong ysize,
			      DLong scr_xsize, DLong scr_ysize,
			      DLong x_scroll_size, DLong y_scroll_size)
  : GDLWidget( parentID, uvalue, NULL, sensitive, map, xoffset, yoffset, xsize, ysize, uname)
{
  //  std::cout << "In GDLWidgetDraw::GDLWidgetDraw: " << widgetID << std::endl
  assert( parentID != GDLWidget::NullID);
  
  wxWindow *wxParent = NULL;

  wxMutexGuiEnter();
  // If parent base widget exists ....
  GDLWidget* gdlParent = GetWidget( parentID);
  wxParent = static_cast< wxWindow*>( gdlParent->WxWidget());
  //    std::cout << "Getting Parent: " << parent << " " << gdlParent << " " 
  //      << wxParent << std::endl;

  wxPanel *panel = gdlParent->GetPanel();
  widgetPanel = panel;
  
  if ( frame == 0) {
    wxBoxSizer *parentSizer = (wxBoxSizer *) gdlParent->GetSizer();
    //    std::cout << "Getting Parent Sizer:" << parentSizer << std::endl;
//     parentSizer->Add( sizer, 0, wxEXPAND|wxALL, 5);
  } else {
    wxStaticBoxSizer *parentSizer = (wxStaticBoxSizer *) gdlParent->GetSizer();
    //    std::cout << "Getting Parent Sizer:" << parentSizer << std::endl;
//     parentSizer->Add( sizer, 0, wxEXPAND|wxALL, 5);
  }
  
  // TODO
// In the wxWidgets application a wxMemoryDC must be created (e.g. in the constructor of a wxWindow) and made known to the driver, e.g.
  
// memPlotDC = new wxMemoryDC;
// memPlotDCBitmap = new wxBitmap( 640, 400, -1 );
// memPlotDC->SelectObject( *MemPlotDCBitmap );
// my_stream = new GDLWXStream( (wxDC*)MemPlotDC, MemPlotDC_width, MemPlotDC_height );

  wxMutexGuiLeave();
}

#endif