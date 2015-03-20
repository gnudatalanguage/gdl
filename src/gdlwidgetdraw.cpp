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
 
////why overcast inherited ~GDLWidget????
//GDLWidgetDraw::~GDLWidgetDraw()
//{
//#ifdef GDL_DEBUG_WIDGETS
//    std::cout << "~GDLWidgetDraw: " << this << std::endl;
//#endif  
//  // handled in GDLDrawPanel (which is deleted by wxWidgets)
////   GraphicsDevice::GetGUIDevice()->WDelete( pstreamIx);
//}
//
//
//GDLWidgetDraw::GDLWidgetDraw( WidgetIDT p, EnvT* e,
//			      DLong x_scroll_size_, DLong y_scroll_size_, bool app_scroll, DULong eventFlags_, DStringGDL* drawToolTip)
//  : GDLWidget( p, e, NULL, eventFlags_)
//  , pstreamIx(-1)
//  , x_scroll_size(x_scroll_size_)
//  , y_scroll_size(y_scroll_size_)
//{
//  //  std::cout << "In GDLWidgetDraw::GDLWidgetDraw: " << widgetID << std::endl
//  assert( parentID != GDLWidget::NullID);
//
//  //get immediately rid of scroll sizes in case of scroll or not... Here is the logic:
//  if (app_scroll) scrolled=TRUE;
//  if (x_scroll_size > 0) {scrolled=TRUE;x_scroll_size+=(SCROLL_WIDTH+2*DEFAULT_BORDER_SIZE);} 
//  if (y_scroll_size > 0) {scrolled=TRUE;y_scroll_size+=(SCROLL_WIDTH+2*DEFAULT_BORDER_SIZE);}
//  if (scrolled) x_scroll_size=(x_scroll_size<100)?100:x_scroll_size;
//  if (scrolled) y_scroll_size=(y_scroll_size<100)?100:y_scroll_size;
//
//  wxSize widgetSize = wxDefaultSize;
//  if ( xSize == widgetSize.x ) xSize=scrolled?120:100; //provide a default value!
//  if ( ySize == widgetSize.y ) ySize=scrolled?120:100; 
//
//  wxWindow *wxParent = NULL;
//
//  // If parent base widget exists ....
//  GDLWidget* gdlParent = GetWidget( parentID);
//  widgetPanel = gdlParent->GetPanel( );
//  widgetSizer = gdlParent->GetSizer( );
//  topWidgetSizer = this->GetTopLevelBaseWidget(parentID)->GetSizer();
//
//  wxParent = static_cast< wxWindow*>( gdlParent->GetWxWidget());
//  //    std::cout << "Getting Parent: " << parent << " " << gdlParent << " " 
//  //      << wxParent << std::endl;
//
//  long style = 0;
//  if( frame == 1)
//    style = wxBORDER_SIMPLE;
//  else if( frame > 1)
//    style = wxBORDER_DOUBLE;
//  
//  GDLDrawPanel* gdlWindow = new GDLDrawPanel( widgetPanel, widgetID, wxPoint(xOffset,yOffset), wxSize(xSize,ySize), style);
//  gdlWindow->SetCursor(wxCURSOR_CROSS);
//  if (drawToolTip) static_cast<wxWindow*>(gdlWindow)->SetToolTip( wxString((*drawToolTip)[0].c_str(),wxConvUTF8));
//  wxWidget = gdlWindow;
//  widgetSizer->Add( gdlWindow, 0, wxALL, DEFAULT_BORDER_SIZE);
//  if (frame) this->FrameWidget();
//  if (scrolled) this->ScrollWidget(x_scroll_size, y_scroll_size );
//
//  static_cast<GDLDrawPanel*>(wxWidget)->InitStream();
//  
//  pstreamIx = static_cast<GDLDrawPanel*>(wxWidget)->PStreamIx();
//  GDLDelete( vValue);
//  this->vValue = new DLongGDL(pstreamIx);  
//  this->SetSensitive(sensitive);
////here UPDATE_WINDOW is useful.  
//  gdlParent->GetSizer()->Layout();
//  if(widgetPanel->IsShownOnScreen()) 
//  {
//    GDLWidgetBase *tlb=GetTopLevelBaseWidget(this->WidgetID());
////    tlb->GetSizer()->Layout();
//    static_cast<wxFrame*>(tlb->GetWxWidget())->Show();
//  }
//}

//void GDLWidgetDraw::OnRealize()
//{ 
//  GDLWidget::OnRealize();
//}

#endif