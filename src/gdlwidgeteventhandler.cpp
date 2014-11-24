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
  EVT_MOTION (GDLDrawPanel::OnMouseMove)
  EVT_LEFT_DOWN (GDLDrawPanel::OnMouseDown)
  EVT_LEFT_UP (GDLDrawPanel::OnMouseUp)
  EVT_LEFT_DCLICK(GDLDrawPanel::OnMouseDown)
  EVT_MIDDLE_DOWN(GDLDrawPanel::OnMouseDown)
  EVT_MIDDLE_UP(GDLDrawPanel::OnMouseUp)
  EVT_MIDDLE_DCLICK(GDLDrawPanel::OnMouseDown)
  EVT_RIGHT_DOWN(GDLDrawPanel::OnMouseDown)
  EVT_RIGHT_UP(GDLDrawPanel::OnMouseUp)
  EVT_RIGHT_DCLICK(GDLDrawPanel::OnMouseDown)
  //EVT_MOUSE_AUX1_DOWN
  //EVT_MOUSE_AUX1_UP
  //EVT_MOUSE_AUX1_DCLICK
  //EVT_MOUSE_AUX2_DOWN
  //EVT_MOUSE_AUX2_UP
  //EVT_MOUSE_AUX2_DCLICK
  EVT_MOUSEWHEEL(GDLDrawPanel::OnMouseWheel)
  EVT_KEY_DOWN(GDLDrawPanel::OnKey)
  EVT_KEY_UP(GDLDrawPanel::OnKey)
  EVT_CHAR(GDLDrawPanel::OnKey)
//  EVT_ENTER_WINDOW( GDLDrawPanel::OnEnterWindow)
//  EVT_LEAVE_WINDOW( GDLDrawPanel::OnLeaveWindow)
//   EVT_SHOW(GDLWindow::OnShow)
//   EVT_CLOSE(GDLWindow::OnClose)
END_EVENT_TABLE()

IMPLEMENT_APP_NO_MAIN( GDLApp)


void wxTextCtrl::OnChar(wxKeyEvent& event ) {
  GUIMutexLockerEventHandlersT gdlMutexGuiEnterLeave;
  //I cannot get cw_field to work if OnChar is not overwritten -- certainly there is a better way?
  GDLWidget* txt = GDLWidget::GetWidget( event.GetId());
  DStructGDL* widg;
  bool report = txt->HasEventType( GDLWidget::ALL );
  bool edit = this->IsEditable( );
  WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId( ) );
  if ( !report ) {
    if ( !edit ) {
      event.Skip( );
      return;
    } else { //editable
      if ( event.GetKeyCode( ) == WXK_RETURN ) { //only 
      widg = new DStructGDL( "WIDGET_TEXT_CH" );
      widg->InitTag( "ID", DLongGDL( event.GetId( ) ) );
      widg->InitTag( "TOP", DLongGDL( baseWidgetID ) );
      widg->InitTag( "HANDLER", DLongGDL( 0 ) );
      widg->InitTag( "TYPE", DIntGDL( 0 ) ); // selection
      widg->InitTag( "OFFSET", DLongGDL( this->GetInsertionPoint() ) );
      widg->InitTag( "CH", DByteGDL( 10 ) );
      GDLWidget::PushEvent( baseWidgetID, widg );
      }
      event.Skip( );
      return;
    }
  } else { //report ALL (text) events, editable or not
    int sign = 1;
    long from, to;
    this->GetSelection( &from, &to );
    long oldpos = this->GetInsertionPoint( ), newpos;
    switch ( event.GetKeyCode( ) ) {
      case WXK_LEFT:
        sign = -1;
      case WXK_RIGHT:
        newpos = oldpos + sign * 1;
        if ( newpos >= 0 && newpos <= this->GetLastPosition( ) ) this->SetInsertionPoint( newpos );
        widg = new DStructGDL( "WIDGET_TEXT_SEL" );
        widg->InitTag( "ID", DLongGDL( event.GetId( ) ) );
        widg->InitTag( "TOP", DLongGDL( baseWidgetID ) );
        widg->InitTag( "HANDLER", DLongGDL( 0 ) );
        widg->InitTag( "TYPE", DIntGDL( 3 ) ); // selection
        widg->InitTag( "OFFSET", DLongGDL( this->GetInsertionPoint( ) ) );
        widg->InitTag( "LENGTH", DLongGDL( 0 ) );
        GDLWidget::PushEvent( baseWidgetID, widg );
        return;
        break;
      case WXK_BACK:
        if ( oldpos > 0 ) {
          widg = new DStructGDL( "WIDGET_TEXT_DEL" );
          widg->InitTag( "ID", DLongGDL( event.GetId( ) ) );
          widg->InitTag( "TOP", DLongGDL( baseWidgetID ) );
          widg->InitTag( "HANDLER", DLongGDL( 0 ) );
          widg->InitTag( "TYPE", DIntGDL( 2 ) ); // selection
          widg->InitTag( "OFFSET", DLongGDL( from - 1 ) );
          widg->InitTag( "LENGTH", DLongGDL( to - from + 1 ) );
          GDLWidget::PushEvent( baseWidgetID, widg );
        }
        if (edit) event.Skip( ); //do it!
        return;
        break;
      case WXK_DELETE:
        if ( oldpos <= this->GetLastPosition() ) {
          widg = new DStructGDL( "WIDGET_TEXT_DEL" );
          widg->InitTag( "ID", DLongGDL( event.GetId( ) ) );
          widg->InitTag( "TOP", DLongGDL( baseWidgetID ) );
          widg->InitTag( "HANDLER", DLongGDL( 0 ) );
          widg->InitTag( "TYPE", DIntGDL( 2 ) ); // selection
          widg->InitTag( "OFFSET", DLongGDL( from ) );
          widg->InitTag( "LENGTH", DLongGDL( to - from ) );
          GDLWidget::PushEvent( baseWidgetID, widg );
        }
        if (edit) event.Skip( ); //do it!
        return;
        break;
      case WXK_RETURN: //important, *DL returns CH=10 instead of 13 for <CR>
        widg = new DStructGDL( "WIDGET_TEXT_CH" );
        widg->InitTag( "ID", DLongGDL( event.GetId( ) ) );
        widg->InitTag( "TOP", DLongGDL( baseWidgetID ) );
        widg->InitTag( "HANDLER", DLongGDL( 0 ) );
        widg->InitTag( "TYPE", DIntGDL( 0 ) ); // selection
        widg->InitTag( "OFFSET", DLongGDL( from ) );
        widg->InitTag( "CH", DByteGDL( 10 ) );
        GDLWidget::PushEvent( baseWidgetID, widg );
        return;
        break;
    }
    if (edit) event.Skip( ); //else do it!
  }

  //else return a CHAR event for most keys (as *DL)
    widg = new DStructGDL( "WIDGET_TEXT_CH" );
    widg->InitTag( "ID", DLongGDL( event.GetId( ) ) );
    widg->InitTag( "TOP", DLongGDL( baseWidgetID ) );
    widg->InitTag( "HANDLER", DLongGDL( 0 ) );
    widg->InitTag( "TYPE", DIntGDL( 0 ) ); // single char
    widg->InitTag( "OFFSET", DLongGDL( this->GetInsertionPoint( ) ) );
    widg->InitTag( "CH", DByteGDL( event.GetKeyCode( ) ) );
    GDLWidget::PushEvent( baseWidgetID, widg );
}

void GDLFrame::OnShowRequest( wxCommandEvent& event)
{  
#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputDebug().Printf(_T("in OnShowRequest: %d\n"),event.GetId());
#endif
  GUIMutexLockerEventHandlersT gdlMutexGuiEnterLeave;
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
  GUIMutexLockerEventHandlersT gdlMutexGuiEnterLeave;
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
  
  gdlMutexGuiEnterLeave.Leave();
  
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
    textCtrl->GetSelection( &selStart, &selEnd);
    offset = textCtrl->GetInsertionPoint();
    lastValue = static_cast<GDLWidgetText*>(widget)->GetLastValue();
    newValue = textCtrl->GetValue().mb_str();
    isModified = lastValue != newValue;
    static_cast<GDLWidgetText*>(widget)->SetLastValue(newValue);
//return without producing event struct if eventType is not set
    if (!(widget->HasEventType( GDLWidget::ALL ))) return;

//    textCtrl->Refresh();
  } 
  else if ( widget->IsComboBox()) 
  {
    wxComboBox* control = static_cast<wxComboBox*>(widget->GetWxWidget());
    if( control == NULL)
    {
      event.Skip();
      return; // happens on construction
    }
    //here we should differentiate EDITABLE and not-editable comboboxes!!! FIXME
    control->GetCurrentSelection() ; //would be control->GetSelection( &selStart, &selEnd); // with GTK 
    offset = control->GetInsertionPoint();    
    lastValue = static_cast<GDLWidgetComboBox*>(widget)->GetLastValue();
    newValue = control->GetValue().mb_str();
    isModified = lastValue != newValue;
    static_cast<GDLWidgetComboBox*>(widget)->SetLastValue(newValue);

//    control->Refresh();
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
      newValue=control->GetString(where).mb_str();
      lastValue = static_cast<GDLWidgetDropList*>(widget)->GetLastValue();
      isModified = lastValue != newValue;
      static_cast<GDLWidgetDropList*>(widget)->SetLastValue(newValue);
    }

//    control->Refresh();
  }

  gdlMutexGuiEnterLeave.Leave();
  
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

//    textCtrl->Refresh();
  }
  else if( widget->IsComboBox())
  {
    wxComboBox* control = static_cast<wxComboBox*>(widget->GetWxWidget());
    //here we should differentiate EDITABLE and not-editable comboboxes!!! FIXME
    offset = control->GetInsertionPoint();    
    newValue = control->GetStringSelection().mb_str();
    static_cast<GDLWidgetComboBox*>(widget)->SetLastValue(newValue);

//    control->Refresh();
  }
  else
  {
    assert( widget->IsDropList());
    wxChoice* control = static_cast<wxChoice*>(widget->GetWxWidget());
    int where = control->GetSelection();
    if (where != wxNOT_FOUND) {
      newValue = control->GetString(where).mb_str();
      static_cast<GDLWidgetDropList*>(widget)->SetLastValue(newValue);
    }

//    control->Refresh();
  }
  gdlMutexGuiEnterLeave.Leave();
  
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
  GUIMutexLockerEventHandlersT gdlMutexGuiEnterLeave;
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
  GUIMutexLockerEventHandlersT gdlMutexGuiEnterLeave;
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
//  GUIMutexLockerEventHandlersT gdlMutexGuiEnterLeave;
  std::cout << "GDLWindow::OnShow: " << this << std::endl;
}
void GDLDrawPanel::OnClose(wxCloseEvent& event)
{
//  GUIMutexLockerEventHandlersT gdlMutexGuiEnterLeave;
  std::cout << "GDLWindow::OnClose: " << this << std::endl;
}

void GDLDrawPanel::OnEnterWindow( wxMouseEvent &event ) {
//  GUIMutexLockerEventHandlersT gdlMutexGuiEnterLeave;
}

void GDLDrawPanel::OnLeaveWindow( wxMouseEvent &event ) {
//  GUIMutexLockerEventHandlersT gdlMutexGuiEnterLeave;
}

void GDLDrawPanel::OnMouseMove( wxMouseEvent &event ) {
  
  if ( this->eventFlags & GDLWidget::MOTION ) {
//    GUIMutexLockerEventHandlersT gdlMutexGuiEnterLeave;
    WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId( ) );
    DStructGDL* widgdraw = new DStructGDL( "WIDGET_DRAW" );
    widgdraw->InitTag( "ID", DLongGDL( event.GetId( ) ) );
    widgdraw->InitTag( "TOP", DLongGDL( baseWidgetID ) );
    widgdraw->InitTag( "HANDLER", DLongGDL( 0 ) );
    widgdraw->InitTag( "TYPE", DIntGDL( 2 ) ); //motion
    widgdraw->InitTag( "X", DLongGDL( event.GetX() ) );
    widgdraw->InitTag( "Y", DLongGDL( drawSize.y-event.GetY()  ) );
    GDLWidget::PushEvent( baseWidgetID, widgdraw );
  } //normal end of event processing!
}

void GDLDrawPanel::OnMouseDown( wxMouseEvent &event ) {
  if ( this->eventFlags & GDLWidget::BUTTON ) {
//    GUIMutexLockerEventHandlersT gdlMutexGuiEnterLeave;
    WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId( ) );
    DStructGDL* widgdraw = new DStructGDL( "WIDGET_DRAW" );
    widgdraw->InitTag( "ID", DLongGDL( event.GetId( ) ) );
    widgdraw->InitTag( "TOP", DLongGDL( baseWidgetID ) );
    widgdraw->InitTag( "HANDLER", DLongGDL( 0 ) );
    widgdraw->InitTag( "TYPE", DIntGDL( 0 ) ); //button Press
    widgdraw->InitTag( "X", DLongGDL( event.GetX() ) );
    widgdraw->InitTag( "Y", DLongGDL( drawSize.y-event.GetY()  ) );
    unsigned long btn=1<<(event.GetButton()-1);
    widgdraw->InitTag( "PRESS", DByteGDL( btn ));
    widgdraw->InitTag( "RELEASE", DByteGDL( 0 ) );
    widgdraw->InitTag( "CLICKS", DLongGDL( 1 ) );
    widgdraw->InitTag( "MODIFIERS", DLongGDL( 0 ) );
    widgdraw->InitTag( "CH", DByteGDL( 0 ) );
    widgdraw->InitTag( "KEY", DLongGDL( 0 ) );
    GDLWidget::PushEvent( baseWidgetID, widgdraw );
  } //normal end of event processing!
}
void GDLDrawPanel::OnMouseUp( wxMouseEvent &event ) {
  if ( this->eventFlags & GDLWidget::BUTTON ) {
//    GUIMutexLockerEventHandlersT gdlMutexGuiEnterLeave;
    WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId( ) );
    DStructGDL* widgdraw = new DStructGDL( "WIDGET_DRAW" );
    widgdraw->InitTag( "ID", DLongGDL( event.GetId( ) ) );
    widgdraw->InitTag( "TOP", DLongGDL( baseWidgetID ) );
    widgdraw->InitTag( "HANDLER", DLongGDL( 0 ) );
    widgdraw->InitTag( "TYPE", DIntGDL( 1 ) ); //button Release
    widgdraw->InitTag( "X", DLongGDL( event.GetX() ) );
    widgdraw->InitTag( "Y", DLongGDL( drawSize.y-event.GetY()  ) );
    unsigned long btn=1<<(event.GetButton()-1);
    widgdraw->InitTag( "PRESS", DByteGDL( 0 ) );
    widgdraw->InitTag( "RELEASE", DByteGDL( btn ) );
    widgdraw->InitTag( "CLICKS", DLongGDL( 1 ) );
    widgdraw->InitTag( "MODIFIERS", DLongGDL( 0 ) );
    widgdraw->InitTag( "CH", DByteGDL( 0 ) );
    widgdraw->InitTag( "KEY", DLongGDL( 0 ) );
    GDLWidget::PushEvent( baseWidgetID, widgdraw );
  } //normal end of event processing!
}

void GDLDrawPanel::OnMouseWheel( wxMouseEvent &event ) {
  if ( this->eventFlags & GDLWidget::WHEEL ) {
//    GUIMutexLockerEventHandlersT gdlMutexGuiEnterLeave;
    WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId( ) );
    DStructGDL* widgdraw = new DStructGDL( "WIDGET_DRAW" );
    widgdraw->InitTag( "ID", DLongGDL( event.GetId( ) ) );
    widgdraw->InitTag( "TOP", DLongGDL( baseWidgetID ) );
    widgdraw->InitTag( "HANDLER", DLongGDL( 0 ) );
    widgdraw->InitTag( "TYPE", DIntGDL( 7 ) ); //wheel event
    widgdraw->InitTag( "X", DLongGDL( event.GetX() ) );
    widgdraw->InitTag( "Y", DLongGDL( drawSize.y-event.GetY()  ) );
    widgdraw->InitTag( "PRESS", DByteGDL( 0 ) );
    widgdraw->InitTag( "RELEASE", DByteGDL( 0 ) );
    widgdraw->InitTag( "CLICKS", DLongGDL( event.GetWheelRotation() ) );
    widgdraw->InitTag( "MODIFIERS", DLongGDL( 0 ) );
    widgdraw->InitTag( "CH", DByteGDL( 0 ) );
    widgdraw->InitTag( "KEY", DLongGDL( 0 ) );
    GDLWidget::PushEvent( baseWidgetID, widgdraw );
  }
}

void GDLDrawPanel::OnKey( wxKeyEvent &event ) {
  int mode=1;
  if ( this->eventFlags & GDLWidget::KEYBOARD || this->eventFlags & GDLWidget::KEYBOARD2 ) {
    if (this->eventFlags & GDLWidget::KEYBOARD2) mode=2;
//    GUIMutexLockerEventHandlersT gdlMutexGuiEnterLeave;
    WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId( ) );
    DStructGDL* widgdraw = new DStructGDL( "WIDGET_DRAW" );
    widgdraw->InitTag( "ID", DLongGDL( event.GetId( ) ) );
    widgdraw->InitTag( "TOP", DLongGDL( baseWidgetID ) );
    widgdraw->InitTag( "HANDLER", DLongGDL( 0 ) );
    widgdraw->InitTag( "X", DLongGDL( event.GetX() ) );
    widgdraw->InitTag( "Y", DLongGDL( drawSize.y-event.GetY()  ) );
    widgdraw->InitTag( "CLICKS", DLongGDL( 0 ) );
    widgdraw->InitTag( "PRESS", DByteGDL( (event.GetEventType() == wxEVT_KEY_DOWN) ) );
    widgdraw->InitTag( "RELEASE", DByteGDL( (event.GetEventType() == wxEVT_KEY_UP) ) );
    
    int start=0;
    int keyc=event.GetKeyCode( );
    //get key
    switch ( keyc ) {
      case WXK_NUMPAD_END: case WXK_END:
        start++;
      case WXK_NUMPAD_HOME: case WXK_HOME:
        start++;
      case WXK_NUMPAD_PAGEDOWN: case WXK_PAGEDOWN:
        start++;
      case WXK_NUMPAD_PAGEUP: case WXK_PAGEUP:
        start++;
      case WXK_NUMPAD_DOWN:case WXK_DOWN:
        start++;
      case WXK_NUMPAD_UP:case WXK_UP:
        start++;
      case WXK_NUMPAD_RIGHT:case WXK_RIGHT:
        start++;
      case WXK_NUMPAD_LEFT:case WXK_LEFT:
        start++;
      case WXK_ALT:
        start++;
      case WXK_CAPITAL: //not sure OK!
        start++;
      case WXK_CONTROL:
        start++;
      case WXK_SHIFT:
        start++;
        if ( mode==2 || start > 4 ) {
          widgdraw->InitTag( "TYPE", DIntGDL( 6 ) ); //special key
          widgdraw->InitTag( "KEY", DLongGDL( start ) );
          widgdraw->InitTag( "CH", DByteGDL( 0 ) );
          widgdraw->InitTag( "MODIFIERS", DLongGDL( event.GetModifiers()) );
          GDLWidget::PushEvent( baseWidgetID, widgdraw );
        }
        break;
      default:
        widgdraw->InitTag( "TYPE", DIntGDL( 5 ) ); //normal key
        widgdraw->InitTag( "KEY", DLongGDL( 0 ) );
        widgdraw->InitTag( "CH", DByteGDL( event.GetRawKeyCode() & 0xFF ) );
        widgdraw->InitTag( "MODIFIERS", DLongGDL( event.GetModifiers() ) );
        GDLWidget::PushEvent( baseWidgetID, widgdraw );
    }
  }
}

#endif

