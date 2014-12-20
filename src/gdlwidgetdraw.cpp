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
#define UPDATE_WINDOW  \
  GetWidget( parentID )->GetSizer()->Layout(); \
  if(widgetPanel->IsShownOnScreen()) \
  {\
    GDLWidgetBase *tlb=GetTopLevelBaseWidget(this->WidgetID()); \
    tlb->GetSizer()->Layout(); \
    static_cast<wxFrame*>(tlb->GetWxWidget())->Show(); \
  }   // or : static_cast<wxFrame*>(tlb->GetWxWidget())->Fit();
 
//why overcast inherited ~GDLWidget????
GDLWidgetDraw::~GDLWidgetDraw()
{
  // handled in GDLDrawPanel (which is deleted by wxWidgets)
//   GraphicsDevice::GetGUIDevice()->WDelete( pstreamIx);
}


GDLWidgetDraw::GDLWidgetDraw( WidgetIDT p, EnvT* e,
			      DLong x_scroll_size_, DLong y_scroll_size_, DULong eventFlags_)
  : GDLWidget( p, e, true, NULL, eventFlags_)
  , pstreamIx(-1)
  , x_scroll_size(x_scroll_size_)
  , y_scroll_size(y_scroll_size_)
{
  //  std::cout << "In GDLWidgetDraw::GDLWidgetDraw: " << widgetID << std::endl
  assert( parentID != GDLWidget::NullID);

  //get immediately rid of scroll sizes in case of scroll or not... Here is the logic:
  if (x_scroll_size > 0) {scrolled=TRUE;x_scroll_size+=(SCROLL_WIDTH+2*DEFAULT_BORDER_SIZE);} 
  if (y_scroll_size > 0) {scrolled=TRUE;y_scroll_size+=(SCROLL_WIDTH+2*DEFAULT_BORDER_SIZE);}
  if (scrolled) x_scroll_size=(x_scroll_size<100)?100:x_scroll_size;
  if (scrolled) y_scroll_size=(y_scroll_size<100)?100:y_scroll_size;

  wxWindow *wxParent = NULL;

  // If parent base widget exists ....
  GDLWidget* gdlParent = GetWidget( parentID);
  widgetPanel = gdlParent->GetPanel( );
  widgetSizer = gdlParent->GetSizer( );
  topWidgetSizer = this->GetTopLevelBaseWidget(parentID)->GetSizer();

  wxParent = static_cast< wxWindow*>( gdlParent->GetWxWidget());
  //    std::cout << "Getting Parent: " << parent << " " << gdlParent << " " 
  //      << wxParent << std::endl;

  long style = 0;
  if( frame == 1)
    style = wxBORDER_SIMPLE;
  else if( frame > 1)
    style = wxBORDER_DOUBLE;
  
  DULong eventFlags=this->GetEventFlags();
  
  GDLDrawPanel* gdlWindow = new GDLDrawPanel( widgetPanel, widgetID, wxPoint(xOffset,yOffset), wxSize(xSize,ySize), style, eventFlags);
  gdlWindow->SetCursor(wxCURSOR_CROSS);
  wxWidget = gdlWindow;
  if (scrolled) this->ScrollWidget(x_scroll_size, y_scroll_size );
  if (frame) this->FrameWidget();
  if (!scrolled && !frame) widgetSizer->Add( gdlWindow, 0, wxFIXED_MINSIZE|wxALL, DEFAULT_BORDER_SIZE);

  UPDATE_WINDOW
  this->vValue = new DLongGDL(pstreamIx);
}
void GDLWidgetDraw::updateFlags()
{
//  cout << "in GDLWidgetDraw::updateFlags()" << endl;
  static_cast<GDLDrawPanel*>(this->wxWidget)->SetEventFlags(this->GetEventFlags());
}
void GDLWidgetDraw::OnRealize()
{
//  cout << "in GDLWidgetDraw::OnRealize()" << endl;
  static_cast<GDLDrawPanel*>(wxWidget)->InitStream();
  
  pstreamIx = static_cast<GDLDrawPanel*>(wxWidget)->PStreamIx();
  GDLDelete( vValue);
  this->vValue = new DLongGDL(pstreamIx);
  
  GDLWidget::OnRealize();
}

#endif