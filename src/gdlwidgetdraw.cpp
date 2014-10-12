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
  // handled in GDLDrawPanel (which is deleted by wxWidgets)
//   GraphicsDevice::GetGUIDevice()->WDelete( pstreamIx);
}


GDLWidgetDraw::GDLWidgetDraw( WidgetIDT p, EnvT* e,
			      DLong x_scroll_size_, DLong y_scroll_size_)
  : GDLWidget( p, e)
  , pstreamIx(-1)
  , x_scroll_size(x_scroll_size_)
  , y_scroll_size(y_scroll_size_)
{
  //  std::cout << "In GDLWidgetDraw::GDLWidgetDraw: " << widgetID << std::endl
  assert( parentID != GDLWidget::NullID);

  //get immediately rid of scroll sizes in case of scroll or not... Here is the logic:
  if (scrolled) scroll=TRUE;
  if (x_scroll_size > 0) {scroll=TRUE;x_scroll_size+=(SCROLL_WIDTH+2*DEFAULT_BORDER_SIZE);} 
  if (y_scroll_size > 0) {scroll=TRUE;y_scroll_size+=(SCROLL_WIDTH+2*DEFAULT_BORDER_SIZE);}
  if (scroll) x_scroll_size=(x_scroll_size<100)?100:x_scroll_size;
  if (scroll) y_scroll_size=(y_scroll_size<100)?100:y_scroll_size;
  scrolled=scroll;

  
  wxWindow *wxParent = NULL;

  GUIMutexLockerWidgetsT gdlMutexGuiEnterLeave;
  // If parent base widget exists ....
  GDLWidget* gdlParent = GetWidget( parentID);
  wxParent = static_cast< wxWindow*>( gdlParent->GetWxWidget());
  //    std::cout << "Getting Parent: " << parent << " " << gdlParent << " " 
  //      << wxParent << std::endl;

  wxPanel *parentPanel = gdlParent->GetPanel();
//   widgetPanel = panel;

  long style = 0;
  if( frame == 1)
    style = wxBORDER_SIMPLE;
  else if( frame > 1)
    style = wxBORDER_DOUBLE;
  
  wxBoxSizer *parentSizer = (wxBoxSizer *) gdlParent->GetSizer();

  if ( scroll ) { //the right way to do the job!!!
    wxScrolledWindow *scrw;
    scrw = new wxScrolledWindow( wxParent, wxID_ANY ,  wxPoint(xOffset,yOffset), wxSize(x_scroll_size, y_scroll_size ), wxBORDER_SUNKEN);
    scrw->SetScrollbars(1, 1, 1, 1); //needed to have scrollbars appear
    wxSizer * scrwsizer = new wxBoxSizer( wxVERTICAL );
    scrw->SetSizer( scrwsizer );

    GDLDrawPanel* gdlWindow = new GDLDrawPanel( scrw, widgetID, wxDefaultPosition, wxSize(xSize,ySize), style);
    scrwsizer->Add( gdlWindow, 0, wxEXPAND|wxALL); //since internal size is given just above, respect it!!
    wxWidget = gdlWindow;
    parentSizer->Add(scrw, 0, wxFIXED_MINSIZE|wxALL, DEFAULT_BORDER_SIZE);    //important to fix sizes 
  } else {
    GDLDrawPanel* gdlWindow = new GDLDrawPanel( parentPanel, widgetID, wxPoint(xOffset,yOffset), wxSize(xSize,ySize), style);
    wxWidget = gdlWindow;
    parentSizer->Add( gdlWindow, 0, wxFIXED_MINSIZE|wxALL, DEFAULT_BORDER_SIZE);
  }
  this->vValue = new DLongGDL(pstreamIx);
}

void GDLWidgetDraw::OnRealize()
{
  cout << "in GDLWidgetDraw::OnRealize()" << endl;
  static_cast<GDLDrawPanel*>(wxWidget)->InitStream();
  
  pstreamIx = static_cast<GDLDrawPanel*>(wxWidget)->PStreamIx();
  GDLDelete( vValue);
  this->vValue = new DLongGDL(pstreamIx);
  
  GDLWidget::OnRealize();
}

#endif