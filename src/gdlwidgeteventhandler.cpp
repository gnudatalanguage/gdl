/***************************************************************************
  gdlwidgeteventhandler.cpp  -  GDL widget event handling 
                             -------------------
    begin                : Fri May 7 2004
    copyright            : (C) 2004-2013 by Marc Schellens
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

#ifdef HAVE_LIBWXWIDGETS

#include <memory> 
#include <wx/notebook.h>

#include "basegdl.hpp"
#include "dstructgdl.hpp"

#include "dinterpreter.hpp"

#include "gdlwxstream.hpp"

#include "gdlwidget.hpp"

#include "widget.hpp"

// #define GDL_DEBUG_WIDGETS

DEFINE_EVENT_TYPE(wxEVT_SHOW_REQUEST)
DEFINE_EVENT_TYPE(wxEVT_HIDE_REQUEST)

BEGIN_EVENT_TABLE(GDLFrame, wxFrame)
  EVT_COMMAND(wxID_ANY, wxEVT_SHOW_REQUEST, GDLFrame::OnShowRequest)
  EVT_COMMAND(wxID_ANY, wxEVT_HIDE_REQUEST, GDLFrame::OnHideRequest)
  EVT_IDLE( GDLFrame::OnIdle)
  EVT_MENU(wxID_ANY, GDLFrame::OnButton)
  EVT_BUTTON( wxID_ANY, GDLFrame::OnButton)
  EVT_RADIOBUTTON(wxID_ANY, GDLFrame::OnRadioButton)
  EVT_CHECKBOX(wxID_ANY, GDLFrame::OnCheckBox)
  EVT_COMBOBOX(wxID_ANY, GDLFrame::OnComboBox)
  EVT_CHOICE(wxID_ANY, GDLFrame::OnDropList)
  EVT_LISTBOX_DCLICK(wxID_ANY, GDLFrame::OnListBoxDoubleClicked)
  EVT_LISTBOX(wxID_ANY, GDLFrame::OnListBox)
  EVT_TEXT_ENTER(wxID_ANY, GDLFrame::OnTextEnter)
  EVT_TEXT(wxID_ANY, GDLFrame::OnText)
  EVT_NOTEBOOK_PAGE_CHANGED(wxID_ANY, GDLFrame::OnPageChanged)
//   EVT_SLIDER(wxID_ANY,GDLFrame::OnSlider)
  EVT_SCROLL_THUMBRELEASE(GDLFrame::OnThumbRelease)
  EVT_SCROLL(GDLFrame::OnScroll)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(GDLDrawPanel, wxPanel)
  EVT_PAINT(GDLDrawPanel::OnPaint)
//   EVT_SHOW(GDLWindow::OnShow)
//   EVT_CLOSE(GDLWindow::OnClose)
END_EVENT_TABLE()

IMPLEMENT_APP_NO_MAIN( GDLApp)



void GDLFrame::OnShowRequest( wxCommandEvent& event)
{  
#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputDebug().Printf(_T("in OnShowRequest: %d\n"),event.GetId());
#endif
  GDLWidget* widget = GDLWidget::GetWidget( event.GetId());
  if( widget == NULL)
  {
#ifdef GDL_DEBUG_WIDGETS
    wxMessageOutputDebug().Printf(_T("GDLWidget == NULL: %d\n"),event.GetId());
#endif
    return;
  }
  GDLFrame *frame = static_cast<GDLFrame*>( widget->GetWxWidget());
  if( !frame->IsShown())
  {
    widget->OnShow();
    widget->SetSizeHints();
    
    bool stat = frame->Show(true);
  }
}

void GDLFrame::OnHideRequest( wxCommandEvent& event)
{  
#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputDebug().Printf(_T("in OnHideRequest: %d\n"),event.GetId());
#endif
  GDLWidget* widget = GDLWidget::GetWidget( event.GetId());
  if( widget == NULL)
  {
#ifdef GDL_DEBUG_WIDGETS
    wxMessageOutputDebug().Printf(_T("GDLWidget == NULL: %d\n"),event.GetId());
#endif
    return;
  }
  GDLFrame *frame = static_cast<GDLFrame*>( widget->GetWxWidget());
  if( frame->IsShown())
  {
    bool stat = frame->Show(false);
  }
}



void GDLFrame::OnIdle( wxIdleEvent&)
{
}

void GDLFrame::OnButton( wxCommandEvent& event)
{
#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputDebug().Printf(_T("in OnButton: %d\n"),event.GetId());
#endif
  GUIMutexLockerEventHandlersT gdlMutexGuiEnterLeave;

  WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId());

  // create GDL event struct
  DStructGDL*  widgbut = new DStructGDL( "WIDGET_BUTTON");
  widgbut->InitTag("ID", DLongGDL( event.GetId()));
  widgbut->InitTag("TOP", DLongGDL( baseWidgetID));
  widgbut->InitTag("HANDLER", DLongGDL( 0));
  widgbut->InitTag("SELECT", DLongGDL( 1));

  GDLWidget::PushEvent( baseWidgetID, widgbut);
}

void GDLFrame::OnRadioButton( wxCommandEvent& event)
{
#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputDebug().Printf(_T("in OnRadioButton: %d\n"),event.GetId());
#endif
  GUIMutexLockerEventHandlersT gdlMutexGuiEnterLeave;

  WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId());

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

    GDLWidget* widget = GDLWidget::GetWidget( lastSelection);
    assert(widget->IsButton());
    static_cast<GDLWidgetButton*>(widget)->SetButton( false);
    GDLWidget::PushEvent( baseWidgetID, widgbut);
  }
    
  // create GDL event struct
  DStructGDL*  widgbut = new DStructGDL( "WIDGET_BUTTON");
  widgbut->InitTag("ID", DLongGDL( event.GetId()));
  widgbut->InitTag("TOP", DLongGDL( baseWidgetID));
  widgbut->InitTag("HANDLER", DLongGDL( 0));
//   widgbut->InitTag("SELECT", DLongGDL( selectValue ? 1 : 0));
  widgbut->InitTag("SELECT", DLongGDL( 1));

  gdlParentWidget->SetLastRadioSelection(event.GetId());

  GDLWidget* widget = GDLWidget::GetWidget( event.GetId());
  assert(widget->IsButton());
  static_cast<GDLWidgetButton*>(widget)->SetButton( true);
  GDLWidget::PushEvent( baseWidgetID, widgbut);
}

void GDLFrame::OnCheckBox( wxCommandEvent& event)
{
#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputDebug().Printf(_T("in OnCheckBox: %d\n"),event.GetId());
#endif
  GUIMutexLockerEventHandlersT gdlMutexGuiEnterLeave;

  bool selectValue = event.IsChecked();
  
  WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId());
  GDLWidget* widget = GDLWidget::GetWidget( event.GetId());
  assert(widget->IsButton());
  static_cast<GDLWidgetButton*>(widget)->SetButton( selectValue);
  
  // create GDL event struct
  DStructGDL*  widgbut = new DStructGDL( "WIDGET_BUTTON");
  widgbut->InitTag("ID", DLongGDL( event.GetId()));
  widgbut->InitTag("TOP", DLongGDL( baseWidgetID));
  widgbut->InitTag("HANDLER", DLongGDL( 0));
  widgbut->InitTag("SELECT", DLongGDL( selectValue ? 1 : 0));

  GDLWidget::PushEvent( baseWidgetID, widgbut);
}

void GDLFrame::OnComboBox( wxCommandEvent& event)
{
#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputDebug().Printf(_T("in OnComboBox: %d\n"),event.GetId());
#endif
  GUIMutexLockerEventHandlersT gdlMutexGuiEnterLeave;

  WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId());
  int selectValue = event.GetSelection();
  
  // create GDL event struct
  DStructGDL*  widgcbox;
  wxString strValue = event.GetString();

  widgcbox = new DStructGDL( "WIDGET_COMBOBOX");
  widgcbox->InitTag("ID", DLongGDL( event.GetId()));
  widgcbox->InitTag("TOP", DLongGDL( baseWidgetID));
  widgcbox->InitTag("HANDLER", DLongGDL( 0));
  widgcbox->InitTag("INDEX", DLongGDL( selectValue));
  widgcbox->InitTag("STR", DStringGDL( string(strValue.mb_str()) ));

  GDLWidget::PushEvent( baseWidgetID, widgcbox);
}

void GDLFrame::OnDropList( wxCommandEvent& event)
{
#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputDebug().Printf(_T("in OnDropList: %d\n"),event.GetId());
#endif
  GUIMutexLockerEventHandlersT gdlMutexGuiEnterLeave;

  WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId());
  int selectValue = event.GetSelection();
  
  DStructGDL*  widdrplst;
  widdrplst = new DStructGDL( "WIDGET_DROPLIST");
  widdrplst->InitTag("ID", DLongGDL( event.GetId()));
  widdrplst->InitTag("TOP", DLongGDL( baseWidgetID));
  widdrplst->InitTag("HANDLER", DLongGDL( 0));
  widdrplst->InitTag("INDEX", DLongGDL( selectValue));   

  GDLWidget::PushEvent( baseWidgetID, widdrplst);
}

void GDLFrame::OnListBoxDo( wxCommandEvent& event, DLong clicks)
{
#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputDebug().Printf(_T("in OnListBoxDo: %d\n"),event.GetId());
#endif
  GUIMutexLockerEventHandlersT gdlMutexGuiEnterLeave;

  WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId());

  int selectValue = event.GetSelection();
  
  // create GDL event struct
  DStructGDL*  widgcbox;
  widgcbox = new DStructGDL( "WIDGET_LIST");
  widgcbox->InitTag("ID", DLongGDL( event.GetId()));
  widgcbox->InitTag("TOP", DLongGDL( baseWidgetID));
  widgcbox->InitTag("HANDLER", DLongGDL( 0));
  widgcbox->InitTag("INDEX", DLongGDL( selectValue));
  widgcbox->InitTag("CLICKS", DLongGDL( clicks));

  GDLWidget::PushEvent( baseWidgetID, widgcbox);
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

void GDLFrame::OnText( wxCommandEvent& event)
{
#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputDebug().Printf(_T("in OnText: %d\n"),event.GetId());
#endif
  GUIMutexLockerEventHandlersT gdlMutexGuiEnterLeave;

  WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId());

  bool isModified;
  long selStart, selEnd;
  DLong offset;
  std::string lastValue;
  std::string newValue;
  
  GDLWidget* widget = GDLWidget::GetWidget( event.GetId());
  if( widget->IsText())
  {
    wxTextCtrl* textCtrl = static_cast<wxTextCtrl*>(widget->GetWxWidget());
    if( textCtrl == NULL)
    {
      event.Skip();
      return; // happens on construction
    }
    // isModified = textCtrl->IsModified();
    textCtrl->GetSelection( &selStart, &selEnd);
    offset = textCtrl->GetInsertionPoint();
    lastValue = static_cast<GDLWidgetText*>(widget)->GetLastValue();
    newValue = textCtrl->GetValue().mb_str();
    isModified = lastValue != newValue;
    static_cast<GDLWidgetText*>(widget)->SetLastValue(newValue);

    textCtrl->Refresh();
  } 
  else if ( widget->IsComboBox()) 
  {
    wxComboBox* control = static_cast<wxComboBox*>(widget->GetWxWidget());
    if( control == NULL)
    {
      event.Skip();
      return; // happens on construction
    }
    control->GetSelection( &selStart, &selEnd);
    offset = control->GetInsertionPoint();    
    lastValue = static_cast<GDLWidgetComboBox*>(widget)->GetLastValue();
    newValue = control->GetValue().mb_str();
    isModified = lastValue != newValue;
    static_cast<GDLWidgetComboBox*>(widget)->SetLastValue(newValue);

    control->Refresh();
  }
  else
  {
    assert( widget->IsDropList());
    wxChoice* control = static_cast<wxChoice*>(widget->GetWxWidget());
    if( control == NULL)
    {
      event.Skip();
      return; // happens on construction
    }
    int where = control->GetSelection();
    if (where !=  wxNOT_FOUND) {
      newValue=control->GetString(where);
      lastValue = static_cast<GDLWidgetDropList*>(widget)->GetLastValue();
      isModified = lastValue != newValue;
      static_cast<GDLWidgetDropList*>(widget)->SetLastValue(newValue);
    }

    control->Refresh();
  }

  DStructGDL*  widg;
  if( !isModified)
  {
    widg = new DStructGDL( "WIDGET_TEXT_SEL");
    widg->InitTag("ID", DLongGDL( event.GetId()));
    widg->InitTag("TOP", DLongGDL( baseWidgetID));
    widg->InitTag("HANDLER", DLongGDL( 0));
    widg->InitTag("TYPE", DIntGDL( 3)); // selection
    widg->InitTag("OFFSET", DLongGDL( offset));
    widg->InitTag("LENGTH", DLongGDL( selEnd-selStart));
  }
  else
  {
    int lengthDiff = newValue.length() - lastValue.length();
    if( lengthDiff < 0) // deleted
    {
      widg = new DStructGDL( "WIDGET_TEXT_DEL");
      widg->InitTag("ID", DLongGDL( event.GetId()));
      widg->InitTag("TOP", DLongGDL( baseWidgetID));
      widg->InitTag("HANDLER", DLongGDL( 0));
      widg->InitTag("TYPE", DIntGDL( 2)); // delete
      widg->InitTag("OFFSET", DLongGDL( offset-1));
      widg->InitTag("LENGTH", DLongGDL( -lengthDiff));
    }
    else if( lengthDiff == 0) // replace TODO: just flag the real change
    {   
      // 1st delete all
      widg = new DStructGDL( "WIDGET_TEXT_DEL");
      widg->InitTag("ID", DLongGDL( event.GetId()));
      widg->InitTag("TOP", DLongGDL( baseWidgetID));
      widg->InitTag("HANDLER", DLongGDL( 0));
      widg->InitTag("TYPE", DIntGDL( 2)); // delete
      widg->InitTag("OFFSET", DLongGDL( 0));
      widg->InitTag("LENGTH", DLongGDL( lastValue.length()));
      
      GDLWidget::PushEvent( baseWidgetID, widg);

      // 2nd insert new
      widg = new DStructGDL( "WIDGET_TEXT_STR");
      widg->InitTag("ID", DLongGDL( event.GetId()));
      widg->InitTag("TOP", DLongGDL( baseWidgetID));
      widg->InitTag("HANDLER", DLongGDL( 0));
      widg->InitTag("TYPE", DIntGDL( 1)); // multiple char
      widg->InitTag("OFFSET", DLongGDL( 0));
      widg->InitTag("STR", DStringGDL( newValue));
    }
    else if( lengthDiff == 1)
    {
      widg = new DStructGDL( "WIDGET_TEXT_CH");
      widg->InitTag("ID", DLongGDL( event.GetId()));
      widg->InitTag("TOP", DLongGDL( baseWidgetID));
      widg->InitTag("HANDLER", DLongGDL( 0));
      widg->InitTag("TYPE", DIntGDL( 0)); // single char
      widg->InitTag("OFFSET", DLongGDL( offset+1));
      widg->InitTag("CH", DByteGDL( newValue[offset<newValue.length()?offset:newValue.length()-1]));
    }
    else // > 1
    {
      int nVLenght = newValue.length();
      if( offset < lengthDiff)
	lengthDiff = offset;
      string str = "";
      if( offset <= nVLenght && lengthDiff > 0)
	str = newValue.substr(offset-lengthDiff,lengthDiff);
      
      widg = new DStructGDL( "WIDGET_TEXT_STR");
      widg->InitTag("ID", DLongGDL( event.GetId()));
      widg->InitTag("TOP", DLongGDL( baseWidgetID));
      widg->InitTag("HANDLER", DLongGDL( 0));
      widg->InitTag("TYPE", DIntGDL( 1)); // multiple char
      widg->InitTag("OFFSET", DLongGDL( offset));
      widg->InitTag("STR", DStringGDL( str));
    }
  }
  
  GDLWidget::PushEvent( baseWidgetID, widg);
}

void GDLFrame::OnTextEnter( wxCommandEvent& event)
{
#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputDebug().Printf(_T("in OnTextEnter: %d\n"),event.GetId());
#endif
  GUIMutexLockerEventHandlersT gdlMutexGuiEnterLeave;

  WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId());

  DLong offset;
  std::string newValue;

  GDLWidget* widget = GDLWidget::GetWidget( event.GetId());
  if( widget->IsText())
  {
    wxTextCtrl* textCtrl = static_cast<wxTextCtrl*>(widget->GetWxWidget());
    offset = textCtrl->GetInsertionPoint();
    newValue = textCtrl->GetValue().mb_str();
    static_cast<GDLWidgetText*>(widget)->SetLastValue(newValue);

    textCtrl->Refresh();
  }
  else if( widget->IsComboBox())
  {
    wxComboBox* control = static_cast<wxComboBox*>(widget->GetWxWidget());
    offset = control->GetInsertionPoint();    
    newValue = control->GetValue().mb_str();
    static_cast<GDLWidgetComboBox*>(widget)->SetLastValue(newValue);

    control->Refresh();
  }
  else
  {
    assert( widget->IsDropList());
    wxChoice* control = static_cast<wxChoice*>(widget->GetWxWidget());
    int where = control->GetSelection();
    newValue = control->GetString(where);
    static_cast<GDLWidgetDropList*>(widget)->SetLastValue(newValue);

    control->Refresh();
  }
  
  // create GDL event struct
  DStructGDL*  widg;
  widg = new DStructGDL( "WIDGET_TEXT_CH");
  widg->InitTag("ID", DLongGDL( event.GetId()));
  widg->InitTag("TOP", DLongGDL( baseWidgetID));
  widg->InitTag("HANDLER", DLongGDL( 0));
  widg->InitTag("TYPE", DIntGDL( 0)); // single char
  widg->InitTag("OFFSET", DLongGDL( offset));
  widg->InitTag("CH", DByteGDL( '\n'));

  GDLWidget::PushEvent( baseWidgetID, widg);
}

void GDLFrame::OnPageChanged( wxNotebookEvent& event)
{  
#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputDebug().Printf(_T("in OnPageChanged: %d\n"),event.GetId());
#endif
  GUIMutexLockerEventHandlersT gdlMutexGuiEnterLeave;

  WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId());
 
  // create GDL event struct
  DStructGDL*  widg;
  widg = new DStructGDL( "WIDGET_TAB");
  widg->InitTag("ID", DLongGDL( event.GetId()));
  widg->InitTag("TOP", DLongGDL( baseWidgetID));
  widg->InitTag("HANDLER", DLongGDL( 0));
  widg->InitTag("TAB", DLongGDL( event.GetSelection()));
  
  GDLWidget::PushEvent( baseWidgetID, widg);
}

// void GDLFrame::OnSlider( wxCommandEvent& event)
// {  
// #ifdef GDL_DEBUG_WIDGETS
//   wxMessageOutputDebug().Printf(_T("in OnSlider: %d\n"),event.GetId());
// #endif
// 
//   WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId());
// 
//   // create GDL event struct
//   DStructGDL*  widg;
//   widg = new DStructGDL( "WIDGET_SLIDER");
//   widg->InitTag("ID", DLongGDL( event.GetId()));
//   widg->InitTag("TOP", DLongGDL( baseWidgetID));
//   widg->InitTag("HANDLER", DLongGDL( 0));
//   widg->InitTag("VALUE", DLongGDL( event.GetSelection()));
//   widg->InitTag("DRAG", DIntGDL( 0)); // update event
//   
//   GDLWidget::PushEvent( baseWidgetID, widg);
// }

void GDLFrame::OnScroll( wxScrollEvent& event)
{  
#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputDebug().Printf(_T("in OnScroll: %d\n"),event.GetId());
#endif
  GDLWidget* widget = GDLWidget::GetWidget( event.GetId());
  if( widget == NULL)
  {
#ifdef GDL_DEBUG_WIDGETS
    wxMessageOutputDebug().Printf(_T("GDLWidget == NULL: %d\n"),event.GetId());
#endif
    return;
  }

  WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId());

  if( widget->IsSlider())
  {
    GDLWidgetSlider* sl = static_cast<GDLWidgetSlider*>(widget);
    DLong oldValue = sl->GetValue();
    
    DLong newSelection = event.GetSelection();
    
    if( newSelection != oldValue)
    {
      sl->SetValue( newSelection);
      
      // create GDL event struct
      DStructGDL*  widg;
      widg = new DStructGDL( "WIDGET_SLIDER");
      widg->InitTag("ID", DLongGDL( event.GetId()));
      widg->InitTag("TOP", DLongGDL( baseWidgetID));
      widg->InitTag("HANDLER", DLongGDL( 0));
      widg->InitTag("VALUE", DLongGDL( newSelection));
      widg->InitTag("DRAG", DIntGDL( 1)); // dragging events from here
      
      GDLWidget::PushEvent( baseWidgetID, widg);
    }
    return;
  }

  // other scroll events not implemented yet
  event.Skip();
}

void GDLFrame::OnThumbRelease( wxScrollEvent& event)
{  
#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputDebug().Printf(_T("in OnThumbRelease: %d\n"),event.GetId());
#endif
  GDLWidget* widget = GDLWidget::GetWidget( event.GetId());
  if( widget == NULL)
  {
#ifdef GDL_DEBUG_WIDGETS
    wxMessageOutputDebug().Printf(_T("GDLWidget == NULL: %d\n"),event.GetId());
#endif
    return;
  }

  WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId());

  if( widget->IsSlider())
  {
    GDLWidgetSlider* sl = static_cast<GDLWidgetSlider*>(widget);

    DLong newSelection = event.GetSelection();
    sl->SetValue( newSelection);
    // create GDL event struct
    DStructGDL*  widg;
    widg = new DStructGDL( "WIDGET_SLIDER");
    widg->InitTag("ID", DLongGDL( event.GetId()));
    widg->InitTag("TOP", DLongGDL( baseWidgetID));
    widg->InitTag("HANDLER", DLongGDL( 0));
    widg->InitTag("VALUE", DLongGDL( newSelection));
    widg->InitTag("DRAG", DIntGDL( 0)); // set events from here
    
    GDLWidget::PushEvent( baseWidgetID, widg);

    return;
  }

  // other scroll events not implemented yet
  event.Skip();
}


void GDLDrawPanel::OnPaint(wxPaintEvent& event)
{
//   cout <<"in OnPaint: "<< event.GetId() << endl;
#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputDebug().Printf(_T("in OnPaint: %d\n"),event.GetId());
#endif
  GUIMutexLockerEventHandlersT gdlMutexGuiEnterLeave;

  wxPaintDC dc( this);
  dc.SetDeviceClippingRegion( GetUpdateRegion());
  dc.Blit( 0, 0, drawSize.x, drawSize.y, m_dc, 0, 0 );
}

void GDLDrawPanel::OnShow(wxShowEvent& event)
{
  std::cout << "GDLWindow::OnShow: " << this << std::endl;
}
void GDLDrawPanel::OnClose(wxCloseEvent& event)
{
  std::cout << "GDLWindow::OnClose: " << this << std::endl;
}




#endif

