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
#include "graphicsdevice.hpp"

// #define GDL_DEBUG_WIDGETS

GDLWidgetDraw::~GDLWidgetDraw()
{
  GraphicsDevice::GetGUIDevice()->WDelete( pstreamIx);
}


GDLWidgetDraw::GDLWidgetDraw( WidgetIDT parentID, EnvT* e,
			      DLong x_scroll_size, DLong y_scroll_size)
  : GDLWidget( parentID, e)
  , pstreamIx(-1)
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
  
  long style = 0;
  if( frame == 1)
    style = wxBORDER_SIMPLE;
  else if( frame > 1)
    style = wxBORDER_DOUBLE;
        
  GDLWindow* gdlWindow = new GDLWindow( panel, widgetID, wxDefaultPosition, wxSize(xSize,ySize), style);
  wxWidget = gdlWindow;

  wxBoxSizer *parentSizer = (wxBoxSizer *) gdlParent->GetSizer();
  parentSizer->Add( gdlWindow, 0, wxEXPAND|wxALL, 5);
//   parentSizer->SetSizeHints( wxParent); 
 
  wxMutexGuiLeave();

  pstreamIx = gdlWindow->PStreamIx();
  if( pstreamIx != -1)
    {
      this->vValue = new DLongGDL(pstreamIx);
    }
  else
    {
      throw GDLException("GDLWindow::PStream: Failed to open stream.");
    }
}

#endif