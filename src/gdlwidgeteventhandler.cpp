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
#include <wx/grid.h>
#include <wx/notebook.h>

#include "basegdl.hpp"
#include "dstructgdl.hpp"

#include "dinterpreter.hpp"

#include "gdlwxstream.hpp"

#include "gdlwidget.hpp"

#include "widget.hpp"

 // #define GDL_DEBUG_EVENTS

DEFINE_EVENT_TYPE(wxEVT_SHOW_REQUEST)
DEFINE_EVENT_TYPE(wxEVT_HIDE_REQUEST)

BEGIN_EVENT_TABLE(GDLFrame, wxFrame)
  EVT_COMMAND(wxID_ANY, wxEVT_SHOW_REQUEST, GDLFrame::OnShowRequest)
  EVT_COMMAND(wxID_ANY, wxEVT_HIDE_REQUEST, GDLFrame::OnHideRequest)
//  EVT_IDLE( GDLFrame::OnIdle)
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
//Added: [TLB_]SIZE_EVENTS
  EVT_SIZE(GDLFrame::OnSize) //should actually restarts a timer...
  EVT_TIMER(GDLFrame::WINDOW_TIMER, GDLFrame::OnWidgetTimer) 
//  EVT_TIMER(GDLFrame::RESIZE_TIMER, GDLFrame::OnTimerResize) //... where size event is really done here. But does not work (refresh not OK)
//  EVT_SIZE(GDLFrame::OnSizeWithTimer) //use a timer to pass only once the resize event
//CONTEXT
  EVT_RIGHT_DOWN(GDLFrame::OnRightClickAsContextEvent)
//TLB_MOVE_EVENTS
  EVT_MOVE(GDLFrame::OnMove)
//please ADD KBRD_FOCUS
//ICONIFY
  EVT_ICONIZE(GDLFrame::OnIconize)
//KILL_REQUEST
  EVT_CLOSE(GDLFrame::OnCloseFrame)
EVT_SET_FOCUS(GDLFrame::OnFocusChange)
EVT_KILL_FOCUS(GDLFrame::OnFocusChange)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(GDLGrid, wxGrid)
EVT_GRID_COL_SIZE(GDLGrid::OnTableColResizing)
EVT_GRID_ROW_SIZE(GDLGrid::OnTableRowResizing)
EVT_GRID_RANGE_SELECT(GDLGrid::OnTableRangeSelection)
//EVT_GRID_SELECT_CELL(GDLGrid::OnTableCellSelection)
//EVT_GRID_CELL_LEFT_CLICK(GDLGrid::OnTableCellSelection)
EVT_TEXT_ENTER(wxID_ANY, GDLGrid::OnTextEnter)
EVT_TEXT(wxID_ANY, GDLGrid::OnText)
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
//  EVT_TIMER(GDLDrawPanel::RESIZE_TIMER, GDLDrawPanel::OnTimerResize) //... where size event is really done here. But does not work (refresh not OK)
//  EVT_SIZE(GDLDrawPanel::OnSizeWithTimer)
  EVT_SIZE(GDLDrawPanel::OnSize)
END_EVENT_TABLE()



IMPLEMENT_APP_NO_MAIN( GDLApp)

void wxTextCtrl::OnChar(wxKeyEvent& event ) {
#ifdef GDL_DEBUG_EVENTS
  wxMessageOutputStderr().Printf(_T("in wxTextCtrl::OnChar: %d\n"),event.GetId());
#endif

  //I cannot get cw_field to work if OnChar is not overwritten -- certainly there is a better way?
  GDLWidget* txt = GDLWidget::GetWidget( event.GetId());
  if( txt == NULL)
  {
#ifdef GDL_DEBUG_EVENTS
    wxMessageOutputStderr().Printf(_T("GDLWidget == NULL: %d\n"),event.GetId());
#endif
    event.Skip();
    return;
  }
  DStructGDL* widg;
  bool report = txt->HasEventType( GDLWidget::EV_ALL );
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
      widg->InitTag( "HANDLER", DLongGDL( baseWidgetID ) );
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
        widg->InitTag( "HANDLER", DLongGDL( baseWidgetID ) );
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
          widg->InitTag( "HANDLER", DLongGDL( baseWidgetID ) );
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
          widg->InitTag( "HANDLER", DLongGDL( baseWidgetID ) );
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
        widg->InitTag( "HANDLER", DLongGDL( baseWidgetID ) );
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
    widg->InitTag( "HANDLER", DLongGDL( baseWidgetID ) );
    widg->InitTag( "TYPE", DIntGDL( 0 ) ); // single char
    widg->InitTag( "OFFSET", DLongGDL( this->GetInsertionPoint( ) ) );
    widg->InitTag( "CH", DByteGDL( event.GetKeyCode( ) ) );
    GDLWidget::PushEvent( baseWidgetID, widg );
}

 void GDLFrame::OnWidgetTimer( wxTimerEvent& event)
 {
#ifdef GDL_DEBUG_EVENTS
   wxMessageOutputStderr().Printf(_T("in OnWidgetTimer: %d\n"),event.GetId());
#endif
  GDLWidget* owner=static_cast<GDLWidget*>(gdlOwner);
    WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( owner->WidgetID());
  // create GDL event struct if widget still exist..
    DStructGDL* widgtimer = new DStructGDL( "WIDGET_TIMER" );
    WidgetIDT * originating_id=static_cast<WidgetIDT*>(this->GetEventHandler()->GetClientData());
    widgtimer->InitTag( "ID", DLongGDL(*originating_id));
    widgtimer->InitTag( "TOP", DLongGDL( baseWidgetID ) );
    widgtimer->InitTag( "HANDLER", DLongGDL( baseWidgetID ) );
    GDLWidget::PushEvent( baseWidgetID, widgtimer);
 }

void GDLFrame::OnShowRequest( wxCommandEvent& event)
{
#ifdef GDL_DEBUG_EVENTS
  wxMessageOutputStderr().Printf(_T("in GDLFrame::OnShowRequest: %d\n"),event.GetId());
#endif

  GDLWidget* widget = GDLWidget::GetWidget( event.GetId());
  if( widget == NULL)
  {
#ifdef GDL_DEBUG_EVENTS
    wxMessageOutputStderr().Printf(_T("GDLWidget == NULL: %d\n"),event.GetId());
#endif
    return;
  }
  GDLFrame *frame = static_cast<GDLFrame*>( widget->GetWxWidget());
  if( !frame->IsShown())
  {
    widget->SetSizeHints(); //fundamental, insure main frame wrapping around widgets.
    bool stat = frame->Show(true);
  }
  event.Skip();
}

void GDLFrame::OnHideRequest( wxCommandEvent& event)
{  
#ifdef GDL_DEBUG_EVENTS
  wxMessageOutputStderr().Printf(_T("in GDLFrame::OnHideRequest: %d\n"),event.GetId());
#endif

  GDLWidget* widget = GDLWidget::GetWidget( event.GetId());
  if( widget == NULL)
  {
#ifdef GDL_DEBUG_EVENTS
    wxMessageOutputStderr().Printf(_T("GDLWidget == NULL: %d\n"),event.GetId());
#endif
    return;
  }
  GDLFrame *frame = static_cast<GDLFrame*>( widget->GetWxWidget());
  if( frame->IsShown())
  {
    bool stat = frame->Show(false);
  }
  event.Skip();
}



//void GDLFrame::OnIdle( wxIdleEvent& event)
//{
//  event.Skip();
//}

void GDLFrame::OnButton( wxCommandEvent& event)
{
#ifdef GDL_DEBUG_EVENTS
  wxMessageOutputStderr().Printf(_T("in GDLFrame::OnButton: %d\n"),event.GetId());
#endif

  WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId());

  // create GDL event struct
  DStructGDL*  widgbut = new DStructGDL( "WIDGET_BUTTON");
  widgbut->InitTag("ID", DLongGDL( event.GetId()));
  widgbut->InitTag("TOP", DLongGDL( baseWidgetID));
  widgbut->InitTag("HANDLER", DLongGDL( baseWidgetID ));
  widgbut->InitTag("SELECT", DLongGDL( 1));

  GDLWidget::PushEvent( baseWidgetID, widgbut);
}

void GDLFrame::OnRadioButton( wxCommandEvent& event)
{
#ifdef GDL_DEBUG_EVENTS
  wxMessageOutputStderr().Printf(_T("in GDLFrame::OnRadioButton: %d\n"),event.GetId());
#endif

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
    widgbut->InitTag("HANDLER", DLongGDL( baseWidgetID ));
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
  widgbut->InitTag("HANDLER", DLongGDL( baseWidgetID ));
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
#ifdef GDL_DEBUG_EVENTS
  wxMessageOutputStderr().Printf(_T("in GDLFrame::OnCheckBox: %d\n"),event.GetId());
#endif

  bool selectValue = event.IsChecked();
  
  WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId());
  GDLWidget* widget = GDLWidget::GetWidget( event.GetId());
  assert(widget->IsButton());
  static_cast<GDLWidgetButton*>(widget)->SetButton( selectValue);
  
  // create GDL event struct
  DStructGDL*  widgbut = new DStructGDL( "WIDGET_BUTTON");
  widgbut->InitTag("ID", DLongGDL( event.GetId()));
  widgbut->InitTag("TOP", DLongGDL( baseWidgetID));
  widgbut->InitTag("HANDLER", DLongGDL( baseWidgetID ));
  widgbut->InitTag("SELECT", DLongGDL( selectValue ? 1 : 0));

  GDLWidget::PushEvent( baseWidgetID, widgbut);
}

void GDLFrame::OnComboBox( wxCommandEvent& event)
{
#ifdef GDL_DEBUG_EVENTS
  wxMessageOutputStderr().Printf(_T("in GDLFrame::OnComboBox: %d\n"),event.GetId());
#endif

  WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId());
  int selectValue = event.GetSelection();
  
  // create GDL event struct
  DStructGDL*  widgcbox;
  wxString strValue = event.GetString();

  widgcbox = new DStructGDL( "WIDGET_COMBOBOX");
  widgcbox->InitTag("ID", DLongGDL( event.GetId()));
  widgcbox->InitTag("TOP", DLongGDL( baseWidgetID));
  widgcbox->InitTag("HANDLER", DLongGDL( baseWidgetID ));
  widgcbox->InitTag("INDEX", DLongGDL( selectValue));
  widgcbox->InitTag("STR", DStringGDL( string(strValue.mb_str(wxConvUTF8)) ));

  GDLWidget::PushEvent( baseWidgetID, widgcbox);
}

void GDLFrame::OnDropList( wxCommandEvent& event)
{
#ifdef GDL_DEBUG_EVENTS
  wxMessageOutputStderr().Printf(_T("in GDLFrame::OnDropList: %d\n"),event.GetId());
#endif

  WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId());
  int selectValue = event.GetSelection();
  
  DStructGDL*  widdrplst;
  widdrplst = new DStructGDL( "WIDGET_DROPLIST");
  widdrplst->InitTag("ID", DLongGDL( event.GetId()));
  widdrplst->InitTag("TOP", DLongGDL( baseWidgetID));
  widdrplst->InitTag("HANDLER", DLongGDL( baseWidgetID ));
  widdrplst->InitTag("INDEX", DLongGDL( selectValue));   

  GDLWidget::PushEvent( baseWidgetID, widdrplst);
}

void GDLFrame::OnListBoxDo( wxCommandEvent& event, DLong clicks)
{
#ifdef GDL_DEBUG_EVENTS
  wxMessageOutputStderr().Printf(_T("in GDLFrame::OnListBoxDo: %d\n"),event.GetId());
#endif

  WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId());

  int selectValue = event.GetSelection();
  
  // create GDL event struct
  DStructGDL*  widgcbox;
  widgcbox = new DStructGDL( "WIDGET_LIST");
  widgcbox->InitTag("ID", DLongGDL( event.GetId()));
  widgcbox->InitTag("TOP", DLongGDL( baseWidgetID));
  widgcbox->InitTag("HANDLER", DLongGDL( baseWidgetID ));
  widgcbox->InitTag("INDEX", DLongGDL( selectValue));
  widgcbox->InitTag("CLICKS", DLongGDL( clicks));

  GDLWidget::PushEvent( baseWidgetID, widgcbox);
}
void GDLFrame::OnListBox( wxCommandEvent& event)
{
#ifdef GDL_DEBUG_EVENTS
  wxMessageOutputStderr().Printf(_T("in GDLFrame::OnListBox: %d\n"),event.GetId());
#endif

  OnListBoxDo( event, 1);
}
void GDLFrame::OnListBoxDoubleClicked( wxCommandEvent& event)
{
#ifdef GDL_DEBUG_EVENTS
  wxMessageOutputStderr().Printf(_T("in GDLFrame::OnListBoxDoubleClicked: %d\n"),event.GetId());
#endif

// Note: IDL sends one click AND two click event on double click  
  OnListBoxDo( event, 1);
  OnListBoxDo( event, 2);
}

void GDLFrame::OnText( wxCommandEvent& event)
{
#ifdef GDL_DEBUG_EVENTS
  wxMessageOutputStderr().Printf(_T("in GDLFrame::OnText: %d\n"),event.GetId());
#endif

  WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId());

  bool isModified;
  long selStart, selEnd;
  DLong offset;
  std::string lastValue;
  std::string newValue;
  
  GDLWidget* widget = GDLWidget::GetWidget( event.GetId());
  if( widget == NULL)
  {
#ifdef GDL_DEBUG_EVENTS
    wxMessageOutputStderr().Printf(_T("GDLWidget == NULL: %d\n"),event.GetId());
#endif
    event.Skip();
    return;
  }
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
    newValue = textCtrl->GetValue().mb_str(wxConvUTF8);
    isModified = lastValue != newValue;
    static_cast<GDLWidgetText*>(widget)->SetLastValue(newValue);
//return without producing event struct if eventType is not set
    if (!(widget->HasEventType( GDLWidget::EV_ALL ))) return;

  } 
  else if ( widget->IsComboBox()) 
  {
    wxComboBox* control = static_cast<wxComboBox*>(widget->GetWxWidget());
    if( control == NULL)
    {
      event.Skip();
      return; // happens on construction
    }
    if (!control->IsEditable()) return; //no OnText Events shoudl be generated!
    int pos = control->GetCurrentSelection() ; //would be control->GetSelection( &selStart, &selEnd); // with GTK 
    offset = control->GetInsertionPoint();    
    lastValue = static_cast<GDLWidgetComboBox*>(widget)->GetLastValue();
    newValue = control->GetValue().mb_str(wxConvUTF8);
    isModified = lastValue != newValue;
//    if (isModified) control->SetString(pos,wxString(newValue.c_str(), wxConvUTF8 ));
    static_cast<GDLWidgetComboBox*>(widget)->SetLastValue(newValue);
    return;
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
      newValue=control->GetString(where).mb_str(wxConvUTF8);
      lastValue = static_cast<GDLWidgetDropList*>(widget)->GetLastValue();
      isModified = lastValue != newValue;
      static_cast<GDLWidgetDropList*>(widget)->SetLastValue(newValue);
    }
  }

  DStructGDL*  widg;
  if( !isModified)
  {
    widg = new DStructGDL( "WIDGET_TEXT_SEL");
    widg->InitTag("ID", DLongGDL( event.GetId()));
    widg->InitTag("TOP", DLongGDL( baseWidgetID));
    widg->InitTag("HANDLER", DLongGDL( baseWidgetID ));
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
      widg->InitTag("HANDLER", DLongGDL( baseWidgetID ));
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
      widg->InitTag("HANDLER", DLongGDL( baseWidgetID ));
      widg->InitTag("TYPE", DIntGDL( 2)); // delete
      widg->InitTag("OFFSET", DLongGDL( 0));
      widg->InitTag("LENGTH", DLongGDL( lastValue.length()));
      
      GDLWidget::PushEvent( baseWidgetID, widg);

      // 2nd insert new
      widg = new DStructGDL( "WIDGET_TEXT_STR");
      widg->InitTag("ID", DLongGDL( event.GetId()));
      widg->InitTag("TOP", DLongGDL( baseWidgetID));
      widg->InitTag("HANDLER", DLongGDL( baseWidgetID ));
      widg->InitTag("TYPE", DIntGDL( 1)); // multiple char
      widg->InitTag("OFFSET", DLongGDL( 0));
      widg->InitTag("STR", DStringGDL( newValue));
    }
    else if( lengthDiff == 1)
    {
      widg = new DStructGDL( "WIDGET_TEXT_CH");
      widg->InitTag("ID", DLongGDL( event.GetId()));
      widg->InitTag("TOP", DLongGDL( baseWidgetID));
      widg->InitTag("HANDLER", DLongGDL( baseWidgetID ));
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
      widg->InitTag("HANDLER", DLongGDL( baseWidgetID ));
      widg->InitTag("TYPE", DIntGDL( 1)); // multiple char
      widg->InitTag("OFFSET", DLongGDL( offset));
      widg->InitTag("STR", DStringGDL( str));
    }
  }
  
  GDLWidget::PushEvent( baseWidgetID, widg);
}

void GDLFrame::OnTextEnter( wxCommandEvent& event)
{
#ifdef GDL_DEBUG_EVENTS
  wxMessageOutputStderr().Printf(_T("in GDLFrame::OnTextEnter: %d\n"),event.GetId());
#endif

  WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId());

  DLong offset;
  std::string newValue;

  GDLWidget* widget = GDLWidget::GetWidget( event.GetId());
  if( widget == NULL)
  {
#ifdef GDL_DEBUG_EVENTS
    wxMessageOutputStderr().Printf(_T("GDLWidget == NULL: %d\n"),event.GetId());
#endif
    event.Skip();
    return;
  }
  if( widget->IsText())
  {
    wxTextCtrl* textCtrl = static_cast<wxTextCtrl*>(widget->GetWxWidget());
    offset = textCtrl->GetInsertionPoint();
    newValue = textCtrl->GetValue().mb_str(wxConvUTF8);
    static_cast<GDLWidgetText*>(widget)->SetLastValue(newValue);
  // create GDL event struct
  DStructGDL*  widg;
  widg = new DStructGDL( "WIDGET_TEXT_CH");
  widg->InitTag("ID", DLongGDL( event.GetId()));
  widg->InitTag("TOP", DLongGDL( baseWidgetID));
  widg->InitTag("HANDLER", DLongGDL( baseWidgetID ));
  widg->InitTag("TYPE", DIntGDL( 0)); // single char
  widg->InitTag("OFFSET", DLongGDL( offset));
  widg->InitTag("CH", DByteGDL( '\n'));

  GDLWidget::PushEvent( baseWidgetID, widg);
  }
  else if( widget->IsComboBox())
  {
  // create GDL event struct
  DStructGDL*  widgcbox;
  int selectValue = event.GetSelection();
  wxString strValue = event.GetString();

  widgcbox = new DStructGDL( "WIDGET_COMBOBOX");
  widgcbox->InitTag("ID", DLongGDL( event.GetId()));
  widgcbox->InitTag("TOP", DLongGDL( baseWidgetID));
  widgcbox->InitTag("HANDLER", DLongGDL( baseWidgetID ));
  widgcbox->InitTag("INDEX", DLongGDL( selectValue));
  widgcbox->InitTag("STR", DStringGDL( string(strValue.mb_str(wxConvUTF8)) ));

  GDLWidget::PushEvent( baseWidgetID, widgcbox);
  }
  else
  {
  assert( widget->IsDropList());
  // create GDL event struct
  DStructGDL*  widdrplst;
  int selectValue = event.GetSelection();
  widdrplst = new DStructGDL( "WIDGET_DROPLIST");
  widdrplst->InitTag("ID", DLongGDL( event.GetId()));
  widdrplst->InitTag("TOP", DLongGDL( baseWidgetID));
  widdrplst->InitTag("HANDLER", DLongGDL( baseWidgetID ));
  widdrplst->InitTag("INDEX", DLongGDL( selectValue));   

  GDLWidget::PushEvent( baseWidgetID, widdrplst);
  }
}

void GDLFrame::OnPageChanged( wxNotebookEvent& event)
{  
#ifdef GDL_DEBUG_EVENTS
  wxMessageOutputStderr().Printf(_T("in GDLFrame::OnPageChanged: %d\n"),event.GetId());
#endif

  WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId());
 
  // create GDL event struct
  DStructGDL*  widg;
  widg = new DStructGDL( "WIDGET_TAB");
  widg->InitTag("ID", DLongGDL( event.GetId()));
  widg->InitTag("TOP", DLongGDL( baseWidgetID));
  widg->InitTag("HANDLER", DLongGDL( baseWidgetID ));
  widg->InitTag("TAB", DLongGDL( event.GetSelection()));
  
  GDLWidget::PushEvent( baseWidgetID, widg);
}

//Timer-filtered resizing are not clever enough for graphcis. FIXME!
// Besides moise.LeftIsDown() is not present before wxWidgets  2.8.12 , find an alternative.
//void GDLFrame::OnTimerResize( wxTimerEvent& event)
// {
//   wxMouseState mouse=wxGetMouseState();
//   if (mouse.LeftIsDown()) {
//    m_resizeTimer->Start(100, wxTIMER_ONE_SHOT);
//    return;
//   }
//#ifdef GDL_DEBUG_EVENTS
//   wxMessageOutputStderr().Printf(_T("in OnTimerResize: %d\n"),event.GetId());
//#endif
//  GDLWidget* owner=static_cast<GDLWidget*>(gdlOwner);
//  DULong flags=0;
//  if( owner ) flags=owner->GetEventFlags();
//  if (flags & GDLWidget::EV_SIZE && !owner->IsUpdating() ) {
//    WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( owner->WidgetID());
//  // create GDL event struct
//    DStructGDL* widgbase = new DStructGDL( "WIDGET_BASE" );
//    widgbase->InitTag( "ID", DLongGDL( owner->WidgetID() ) );
//    widgbase->InitTag( "TOP", DLongGDL( baseWidgetID ) );
//    widgbase->InitTag( "HANDLER", DLongGDL( baseWidgetID ) );
//    widgbase->InitTag( "X", DLongGDL( newSize.x ) );
//    widgbase->InitTag( "Y", DLongGDL( newSize.y ) );
//    GDLWidget::PushEvent( baseWidgetID, widgbase);
//   }
// }

 void GDLFrame::OnSize( wxSizeEvent& event)
 {
   if (!gdlOwner) {event.Skip(); return;} //happens for devicewx... to be changed.
 #ifdef GDL_DEBUG_EVENTS
   wxMessageOutputStderr().Printf(_T("in OnSize: %d\n"),event.GetId());
 #endif
  // current bug:
  // onsize should not be called 1) when TLB is resized with widget_control
  // and 2) only at the end of a mouse resizing (wxSizeEvent is sent multiple times
  // when resizing a TLB. But variant with timer does not work correctly...)
  GDLWidget* owner=static_cast<GDLWidget*>(gdlOwner);
  if (owner->GetParentID() != 0) {
    event.Skip();
    return; //ignore non-TLB size events.
  }
  DULong flags=0;
  if( owner ) flags=owner->GetEventFlags();
  if (flags & GDLWidget::EV_SIZE && !owner->IsUpdating() ) {
    WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( owner->WidgetID());

  // create GDL event struct
    DStructGDL* widgbase = new DStructGDL( "WIDGET_BASE" );
    widgbase->InitTag( "ID", DLongGDL( event.GetId( ) ) );
    widgbase->InitTag( "TOP", DLongGDL( baseWidgetID ) );
    widgbase->InitTag( "HANDLER", DLongGDL( baseWidgetID ) );
    widgbase->InitTag( "X", DLongGDL( event.GetSize().GetWidth() ) );
    widgbase->InitTag( "Y", DLongGDL( event.GetSize().GetHeight() ) );
    GDLWidget::PushEvent( baseWidgetID, widgbase);
   }
  event.Skip(); //important, pass to others!
 } 
 
//Timer-filtered resizing are not clever enough for graphcis. FIXME!
// void GDLFrame::OnSizeWithTimer( wxSizeEvent& event)
// {
//   if (!gdlOwner) {event.Skip(); return;} //happens for devicewx... to be changed.
//#ifdef GDL_DEBUG_EVENTS
//   wxMessageOutputStderr().Printf(_T("in OnSizeWithTimer: %d\n"),event.GetId());
//#endif
//  GDLWidget* owner=static_cast<GDLWidget*>(gdlOwner);
//  if (owner->GetParentID() != 0) {
//    event.Skip();
//    return; //ignore non-TLB size events.
//  }
//   int millisecs=100;
//   newSize=(event.GetSize());
//   m_resizeTimer->Start(millisecs, wxTIMER_ONE_SHOT);
//  event.Skip(); //important, pass to others!
//}

void GDLFrame::OnScroll( wxScrollEvent& event)
{  
#ifdef GDL_DEBUG_EVENTS
  wxMessageOutputStderr().Printf(_T("in GDLFrame::OnScroll: %d\n"),event.GetId());
#endif

  GDLWidget* widget = GDLWidget::GetWidget( event.GetId());
  if( widget == NULL)
  {
#ifdef GDL_DEBUG_EVENTS
    wxMessageOutputStderr().Printf(_T("GDLWidget == NULL: %d\n"),event.GetId());
#endif
    event.Skip();
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
      widg->InitTag("HANDLER", DLongGDL( baseWidgetID ));
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
#ifdef GDL_DEBUG_EVENTS
  wxMessageOutputStderr().Printf(_T("in GDLFrame::OnThumbRelease: %d\n"),event.GetId());
#endif

  GDLWidget* widget = GDLWidget::GetWidget( event.GetId());
  if( widget == NULL)
  {
#ifdef GDL_DEBUG_EVENTS
    wxMessageOutputStderr().Printf(_T("GDLWidget == NULL: %d\n"),event.GetId());
#endif
    event.Skip();
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
    widg->InitTag("HANDLER", DLongGDL( baseWidgetID ));
    widg->InitTag("VALUE", DLongGDL( newSelection));
    widg->InitTag("DRAG", DIntGDL( 0)); // set events from here
    
    GDLWidget::PushEvent( baseWidgetID, widg);

    return;
  }

  // other scroll events not implemented yet
  event.Skip();
}
//not OK yet.
void GDLFrame::OnFocusChange( wxFocusEvent &event ) {
#ifdef GDL_DEBUG_EVENTS
  wxMessageOutputStderr().Printf(_T("in GDLFrame::OnFocusChange: %d\n"),event.GetId());
#endif
  if (!gdlOwner) {event.Skip(); return;}
  GDLWidget* owner=static_cast<GDLWidget*>(gdlOwner);
  DULong flags=0;
  if( owner ) flags=owner->GetEventFlags();

  if ( flags & GDLWidget::EV_TRACKING ) {
    WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId( ) );
    DStructGDL* widgtracking = new DStructGDL( "WIDGET_TRACKING" );
    widgtracking->InitTag( "ID", DLongGDL( event.GetId( ) ) );
    widgtracking->InitTag( "TOP", DLongGDL( baseWidgetID ) );
    widgtracking->InitTag( "HANDLER", DLongGDL( baseWidgetID ) );
    int enter=(event.GetEventType()==wxEVT_SET_FOCUS);
    widgtracking->InitTag( "ENTER", DIntGDL( enter ) ); 
    GDLWidget::PushEvent( baseWidgetID, widgtracking );
  } //normal end of event processing!
}
void GDLFrame::OnRightClickAsContextEvent( wxMouseEvent &event ) {
#ifdef GDL_DEBUG_EVENTS
  wxMessageOutputStderr().Printf(_T("in GDLFrame::OnRichtClickAsContextEvent: %d\n"),event.GetId());
#endif
  if (!gdlOwner) {event.Skip(); return;}
  GDLWidget* owner=static_cast<GDLWidget*>(gdlOwner);
  DULong flags=0;
  if( owner ) flags=owner->GetEventFlags();

  if ( flags & GDLWidget::EV_CONTEXT ) {
    WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId( ) );
    DStructGDL* widgcontext = new DStructGDL( "WIDGET_CONTEXT" );
    widgcontext->InitTag( "ID", DLongGDL( event.GetId( ) ) );
    widgcontext->InitTag( "TOP", DLongGDL( baseWidgetID ) );
    widgcontext->InitTag( "HANDLER", DLongGDL( baseWidgetID ) );
    widgcontext->InitTag( "TYPE", DIntGDL( 0 ) ); //button Press
    widgcontext->InitTag( "X", DLongGDL( event.GetX() ) );
    widgcontext->InitTag( "Y", DLongGDL( (static_cast<wxFrame*>(owner->GetWxWidget())->GetSize()).y-event.GetY()  ) );
//LACKS: ROW AND COL, to be added for TABLE Widget
    GDLWidget::PushEvent( baseWidgetID, widgcontext );
  } //normal end of event processing!
}

void GDLFrame::OnIconize( wxIconizeEvent & event)
{
#ifdef GDL_DEBUG_EVENTS
  wxMessageOutputStderr().Printf(_T("in GDLFrame::OnIconize: %d\n"),event.GetId());
#endif
  if (!gdlOwner) {event.Skip(); return;}
  GDLWidget* owner=static_cast<GDLWidget*>(gdlOwner);
  DULong flags=0;
  if( owner ) flags=owner->GetEventFlags();
  if ( flags & GDLWidget::EV_ICONIFY ) {
    WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId( ) );
    DStructGDL* widgtlb_iconify_events = new DStructGDL( "WIDGET_TLB_ICONIFY" );
    widgtlb_iconify_events->InitTag( "ID", DLongGDL( event.GetId( ) ) );
    widgtlb_iconify_events->InitTag( "TOP", DLongGDL( baseWidgetID ) );
    widgtlb_iconify_events->InitTag( "HANDLER", DLongGDL( baseWidgetID ) );
    widgtlb_iconify_events->InitTag( "ICONIFIED", DIntGDL( event.IsIconized() ) ); 
    GDLWidget::PushEvent( baseWidgetID, widgtlb_iconify_events );
  } //normal end of event processing!
}
void GDLFrame::OnMove( wxMoveEvent & event)
{
#ifdef GDL_DEBUG_EVENTS
  wxMessageOutputStderr().Printf(_T("in GDLFrame::OnMove: %d\n"),event.GetId());
#endif
  if (!gdlOwner) {event.Skip(); return;}
  GDLWidget* owner=static_cast<GDLWidget*>(gdlOwner);
  DULong flags=0;
  if( owner ) flags=owner->GetEventFlags();

  if ( flags & GDLWidget::EV_MOVE ) {
    WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId( ) );
    DStructGDL* widgtlb_move_events = new DStructGDL( "WIDGET_TLB_MOVE" );
    widgtlb_move_events->InitTag( "ID", DLongGDL( event.GetId( ) ) );
    widgtlb_move_events->InitTag( "TOP", DLongGDL( baseWidgetID ) );
    widgtlb_move_events->InitTag( "HANDLER", DLongGDL( baseWidgetID ) );
    widgtlb_move_events->InitTag( "X", DLongGDL( event.GetPosition().x ) );
    widgtlb_move_events->InitTag( "Y", DLongGDL( event.GetPosition().y ) );
    GDLWidget::PushEvent( baseWidgetID, widgtlb_move_events );
  } //normal end of event processing!
}
void GDLFrame::OnCloseFrame( wxCloseEvent & event)
{
#ifdef GDL_DEBUG_EVENTS
  wxMessageOutputStderr().Printf(_T("in GDLFrame::OnCloseFrame: %d\n"),event.GetId());
#endif
  if (!gdlOwner) {event.Skip(); return;}
  GDLWidget* owner=static_cast<GDLWidget*>(gdlOwner);
  DULong flags=0;
  if( owner ) flags=owner->GetEventFlags();

  if ( flags & GDLWidget::EV_KILL ) {
    WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId( ) );
    DStructGDL* widgtlb_kill_request_events = new DStructGDL( "WIDGET_KILL_REQUEST" );
    widgtlb_kill_request_events->InitTag( "ID", DLongGDL( event.GetId( ) ) );
    widgtlb_kill_request_events->InitTag( "TOP", DLongGDL( baseWidgetID ) );
    widgtlb_kill_request_events->InitTag( "HANDLER", DLongGDL( baseWidgetID ) );
    GDLWidget::PushEvent( baseWidgetID, widgtlb_kill_request_events );
  } else {
    //destroy TLB widget
    static_cast<GDLWidgetBase*>(gdlOwner)->SelfDestroy();
  }//normal end of event processing!
}

//Draw

void GDLDrawPanel::OnPaint(wxPaintEvent& event)
{
//   cout <<"in OnPaint: "<< event.GetId() << endl;
#ifdef GDL_DEBUG_EVENTS
  wxMessageOutputStderr().Printf(_T("in GDLDrawPanel::OnPaint: %d\n"),event.GetId());
#endif
  wxPaintDC dc( this);
  dc.SetDeviceClippingRegion( GetUpdateRegion());
  dc.Blit( 0, 0, drawSize.x, drawSize.y, m_dc, 0, 0 );
}

void GDLDrawPanel::OnClose(wxCloseEvent& event)
{
#ifdef GDL_DEBUG_EVENTS
  wxMessageOutputStderr().Printf(_T("in GDLDrawPanel::OnClose: %d\n"),event.GetId());
#endif
  event.Skip();
}

//Timer-filtered resizing are not clever enough. FIXME!
// Besides mouse.LeftIsDown() is not present before wxWidgets  2.8.12 , find an alternative.
// void GDLDrawPanel::OnTimerResize( wxTimerEvent& event)
// {
//#ifdef GDL_DEBUG_EVENTS
//  wxMessageOutputStderr().Printf(_T("in GDLDrawPanel::OnTimerResize: %d (%d,%d)\n"),event.GetId());
//#endif
//   wxMouseState mouse=wxGetMouseState();
//   if (mouse.LeftIsDown()) {
//    int millisecs=100;
//    m_resizeTimer->Start(millisecs, wxTIMER_ONE_SHOT);
//    return;
//   }
//  drawSize=newSize;
//  if (pstreamP != NULL)
//  {
//   pstreamP->SetSize(drawSize.x,drawSize.y);
//// not exactly what IDL does, but necessary to refresh draw widgets in some cases (plpot thread race problem?)   
////   pstreamP->replot();
//   this->Refresh();
//  }
////  event.Skip();
// }
 
////Timer-filtered resizing are not clever enough. FIXME!
//void GDLDrawPanel::OnSizeWithTimer( wxSizeEvent & event) {
//   int millisecs=100;
//   newSize=(event.GetSize());
//   m_resizeTimer->Start(millisecs, wxTIMER_ONE_SHOT);
//  event.Skip(); //important, pass to others!
//}
 
void GDLDrawPanel::OnSize( wxSizeEvent &event ) {
#ifdef GDL_DEBUG_EVENTS
  wxMessageOutputStderr().Printf(_T("in GDLDrawPanel::OnResize: %d (%d,%d)\n"),event.GetId(),event.GetSize().x,event.GetSize().y);
#endif
  drawSize=event.GetSize();
  if (pstreamP != NULL)
  {
   pstreamP->SetSize(drawSize.x,drawSize.y);
// not exactly what IDL does, but necessary to refresh draw widgets in some cases (plpot thread race problem?)   
   pstreamP->replot();
  }
  event.Skip();
}

void GDLDrawPanel::OnEnterWindow( wxMouseEvent &event ) {
#ifdef GDL_DEBUG_EVENTS
  wxMessageOutputStderr().Printf(_T("in GDLDrawPanel::OnEnterWindow: %d\n"),event.GetId());
#endif
  event.Skip();

}

void GDLDrawPanel::OnLeaveWindow( wxMouseEvent &event ) {
#ifdef GDL_DEBUG_EVENTS
  wxMessageOutputStderr().Printf(_T("in GDLDrawPanel::OnLeaveWindow: %d\n"),event.GetId());
#endif
  event.Skip();

}

void GDLDrawPanel::OnMouseMove( wxMouseEvent &event ) {
#ifdef GDL_DEBUG_EVENTS
  wxMessageOutputStderr().Printf(_T("in GDLDrawPanel::OnMouseMove: %d\n"),event.GetId());
#endif
  GDLWidgetDraw* draw = static_cast<GDLWidgetDraw*>(GDLWidget::GetWidget(GDLWidgetDrawID));
  DULong eventFlags=draw->GetEventFlags();

  if ( eventFlags & GDLWidget::EV_MOTION ) {
    WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId( ) );
    DStructGDL* widgdraw = new DStructGDL( "WIDGET_DRAW" );
    widgdraw->InitTag( "ID", DLongGDL( GDLWidgetDrawID) );
    widgdraw->InitTag( "TOP", DLongGDL( baseWidgetID ) );
    widgdraw->InitTag( "HANDLER", DLongGDL( baseWidgetID ) );
    widgdraw->InitTag( "TYPE", DIntGDL( 2 ) ); //motion
    widgdraw->InitTag( "X", DLongGDL( event.GetX() ) );
    widgdraw->InitTag( "Y", DLongGDL( drawSize.y-event.GetY()  ) );
    GDLWidget::PushEvent( baseWidgetID, widgdraw );
  } //normal end of event processing!
}

void GDLDrawPanel::OnMouseDown( wxMouseEvent &event ) {
#ifdef GDL_DEBUG_EVENTS
  wxMessageOutputStderr().Printf(_T("in GDLDrawPanel::OnMouseDown: %d\n"),event.GetId());
#endif
  GDLWidgetDraw* draw = static_cast<GDLWidgetDraw*>(GDLWidget::GetWidget(GDLWidgetDrawID));
  DULong eventFlags=draw->GetEventFlags();

  if ( eventFlags & GDLWidget::EV_BUTTON ) {
    WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId( ) );
    DStructGDL* widgdraw = new DStructGDL( "WIDGET_DRAW" );
    widgdraw->InitTag( "ID", DLongGDL( GDLWidgetDrawID) );
    widgdraw->InitTag( "TOP", DLongGDL( baseWidgetID ) );
    widgdraw->InitTag( "HANDLER", DLongGDL( baseWidgetID ) );
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
#ifdef GDL_DEBUG_EVENTS
  wxMessageOutputStderr().Printf(_T("in GDLDrawPanel::OnMouseUp: %d\n"),event.GetId());
#endif
  GDLWidgetDraw* draw = static_cast<GDLWidgetDraw*>(GDLWidget::GetWidget(GDLWidgetDrawID));
  DULong eventFlags=draw->GetEventFlags();
  
  if ( eventFlags & GDLWidget::EV_BUTTON ) {
    WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId( ) );
    DStructGDL* widgdraw = new DStructGDL( "WIDGET_DRAW" );
    widgdraw->InitTag( "ID", DLongGDL( GDLWidgetDrawID) );
    widgdraw->InitTag( "TOP", DLongGDL( baseWidgetID ) );
    widgdraw->InitTag( "HANDLER", DLongGDL( baseWidgetID ) );
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
#ifdef GDL_DEBUG_EVENTS
  wxMessageOutputStderr().Printf(_T("in GDLDrawPanel::OnMouseWheel: %d\n"),event.GetId());
#endif
  GDLWidgetDraw* draw = static_cast<GDLWidgetDraw*>(GDLWidget::GetWidget(GDLWidgetDrawID));
  DULong eventFlags=draw->GetEventFlags();

  if ( eventFlags & GDLWidget::EV_WHEEL ) {
    WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId( ) );
    DStructGDL* widgdraw = new DStructGDL( "WIDGET_DRAW" );
    widgdraw->InitTag( "ID", DLongGDL( GDLWidgetDrawID ) );
    widgdraw->InitTag( "TOP", DLongGDL( baseWidgetID ) );
    widgdraw->InitTag( "HANDLER", DLongGDL( baseWidgetID ) );
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
#ifdef GDL_DEBUG_EVENTS
  wxMessageOutputStderr().Printf(_T("in GDLDrawPanel::OnKey: %d\n"),event.GetId());
#endif
  GDLWidgetDraw* draw = static_cast<GDLWidgetDraw*>(GDLWidget::GetWidget(GDLWidgetDrawID));
  DULong eventFlags=draw->GetEventFlags();

  int mode=1;
  if ( eventFlags & GDLWidget::EV_KEYBOARD || eventFlags & GDLWidget::EV_KEYBOARD2 ) {
    if (eventFlags & GDLWidget::EV_KEYBOARD2) mode=2;
  
    WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId( ) );
    DStructGDL* widgdraw = new DStructGDL( "WIDGET_DRAW" );
    widgdraw->InitTag( "ID", DLongGDL( GDLWidgetDrawID ) );
    widgdraw->InitTag( "TOP", DLongGDL( baseWidgetID ) );
    widgdraw->InitTag( "HANDLER", DLongGDL( baseWidgetID ) );
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

void GDLGrid::OnTableRowResizing(wxGridSizeEvent & event){
#ifdef GDL_DEBUG_EVENTS
  wxMessageOutputStderr().Printf(_T("in GDLGrid::OnTableRowResizing: %d\n"),event.GetId());
#endif
  GDLWidgetTable* table = static_cast<GDLWidgetTable*>(GDLWidget::GetWidget(GDLWidgetTableID));
  DULong eventFlags=table->GetEventFlags();
  if (eventFlags & GDLWidget::EV_ALL) {
    WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId( ) );
    DStructGDL* widgtablerowheight = new DStructGDL( "WIDGET_TABLE_ROW_HEIGHT");
    widgtablerowheight->InitTag("ID", DLongGDL( event.GetId( ) ));
    widgtablerowheight->InitTag("TOP", DLongGDL( baseWidgetID));
    widgtablerowheight->InitTag("HANDLER", DLongGDL( baseWidgetID ));
    widgtablerowheight->InitTag("TYPE", DIntGDL(6)); // 6
    widgtablerowheight->InitTag("ROW", DLongGDL( event.GetRowOrCol() ));
    widgtablerowheight->InitTag("HEIGHT",DLongGDL( this->GetRowSize(event.GetRowOrCol())));
    // insert into structList
    GDLWidget::PushEvent( baseWidgetID, widgtablerowheight );
  }
  event.Skip();
}

void GDLGrid::OnTableColResizing(wxGridSizeEvent & event){
#ifdef GDL_DEBUG_EVENTS
  wxMessageOutputStderr().Printf(_T("in GDLGrid::OnTableColResizing: %d\n"),event.GetId());
#endif
  GDLWidgetTable* table = static_cast<GDLWidgetTable*>(GDLWidget::GetWidget(GDLWidgetTableID));
  DULong eventFlags=table->GetEventFlags();
  if (eventFlags & GDLWidget::EV_ALL) {
    WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId( ) );
    DStructGDL* widgtablerowheight = new DStructGDL( "WIDGET_TABLE_COL_WIDTH");
    widgtablerowheight->InitTag("ID", DLongGDL( event.GetId( ) ));
    widgtablerowheight->InitTag("TOP", DLongGDL( baseWidgetID));
    widgtablerowheight->InitTag("HANDLER", DLongGDL( baseWidgetID ));
    widgtablerowheight->InitTag("TYPE", DIntGDL(7)); // 7
    widgtablerowheight->InitTag("COL", DLongGDL( event.GetRowOrCol() ));
    widgtablerowheight->InitTag("WIDTH",DLongGDL( this->GetColSize(event.GetRowOrCol())));
    // insert into structList
    GDLWidget::PushEvent( baseWidgetID, widgtablerowheight );
  }
  event.Skip();
}
void  GDLGrid::OnTableRangeSelection(wxGridRangeSelectEvent & event){
#ifdef GDL_DEBUG_EVENTS
  wxMessageOutputStderr().Printf(_T("in GDLGrid::OnTableRangeSelection: %d\n"),event.GetId());
#endif
  //this event is called when a selection is added or changed (control-click, etc).
  //If we are not in disjoint mode, clear previous selection to mimick idl's when the user control-clicked.
  GDLWidgetTable* table = static_cast<GDLWidgetTable*>(GDLWidget::GetWidget(GDLWidgetTableID));
  DULong eventFlags=table->GetEventFlags();
  if (eventFlags & GDLWidget::EV_ALL && !table->IsUpdating() ) {
//    //Removing the deselection event when nothing is selected makes is more compatible with idl's behaviour.
//    SizeT count=0;
//    count+=this->GetSelectedCells().GetCount();
//    count+=this->GetSelectedRows().GetCount();
//    count+=this->GetSelectedCols().GetCount();
//    count+=this->GetSelectionBlockTopLeft().GetCount();
//     if (!event.Selecting() && count <=0 ) {event.Skip(); return;}
//
    if (event.Selecting()) 
    {
      WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId( ) );
      DStructGDL* widgtablecelsel = new DStructGDL( "WIDGET_TABLE_CELL_SEL");
      widgtablecelsel->InitTag("ID",  DLongGDL( event.GetId( ) ));
      widgtablecelsel->InitTag("TOP", DLongGDL( baseWidgetID));
      widgtablecelsel->InitTag("HANDLER", DLongGDL( baseWidgetID ));
      widgtablecelsel->InitTag("TYPE",DIntGDL(4)); // 4 or 9
      widgtablecelsel->InitTag("SEL_LEFT",  DLongGDL( event.GetLeftCol()));
      widgtablecelsel->InitTag("SEL_TOP",  DLongGDL( event.GetTopRow()));
      widgtablecelsel->InitTag("SEL_RIGHT",  DLongGDL( event.GetRightCol()));
      widgtablecelsel->InitTag("SEL_BOTTOM",  DLongGDL( event.GetBottomRow()));
      GDLWidget::PushEvent( baseWidgetID, widgtablecelsel );
    } else {
      int ncols = static_cast<wxGrid*>(event.GetEventObject())->GetNumberCols();
      int nrows = static_cast<wxGrid*>(event.GetEventObject())->GetNumberRows();
      if (event.GetLeftCol()==0 && event.GetRightCol()==ncols-1 && event.GetTopRow()==0 && event.GetBottomRow()==nrows-1 ) {
        WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId( ) );
        DStructGDL* widgtablecelsel = new DStructGDL( "WIDGET_TABLE_CELL_SEL");
        widgtablecelsel->InitTag("ID",  DLongGDL( event.GetId( ) ));
        widgtablecelsel->InitTag("TOP", DLongGDL( baseWidgetID ));
        widgtablecelsel->InitTag("HANDLER", DLongGDL( baseWidgetID ) );
        widgtablecelsel->InitTag("TYPE",DIntGDL( 4 )); // 
        widgtablecelsel->InitTag("SEL_LEFT",  DLongGDL( -1 ));
        widgtablecelsel->InitTag("SEL_TOP",  DLongGDL( -1 ));
        widgtablecelsel->InitTag("SEL_RIGHT",  DLongGDL( -1 ));
        widgtablecelsel->InitTag("SEL_BOTTOM",  DLongGDL( -1 ));
        GDLWidget::PushEvent( baseWidgetID, widgtablecelsel );
      } else {
        if (!table->GetDisjointSelection()  && event.ControlDown() ) this->ClearSelection(); else {
        WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId( ) );
        DStructGDL* widgtablecelsel = new DStructGDL( "WIDGET_TABLE_CELL_DESEL");
        widgtablecelsel->InitTag("ID",  DLongGDL( event.GetId( ) ));
        widgtablecelsel->InitTag("TOP", DLongGDL( baseWidgetID ));
        widgtablecelsel->InitTag("HANDLER", DLongGDL( baseWidgetID ));
        widgtablecelsel->InitTag("TYPE",DIntGDL( 9 )); // 9
        widgtablecelsel->InitTag("SEL_LEFT",  DLongGDL( event.GetLeftCol() ));
        widgtablecelsel->InitTag("SEL_TOP",  DLongGDL( event.GetTopRow() ));
        widgtablecelsel->InitTag("SEL_RIGHT",  DLongGDL( event.GetRightCol() ));
        widgtablecelsel->InitTag("SEL_BOTTOM",  DLongGDL( event.GetBottomRow()));
        GDLWidget::PushEvent( baseWidgetID, widgtablecelsel );
        }
      }
    }
}
  event.Skip();
}

 void  GDLGrid::OnTableCellSelection(wxGridEvent & event){
//#ifdef GDL_DEBUG_EVENTS
//  wxMessageOutputStderr().Printf(_T("in GDLGrid::OnTableCellSelection: %d\n"),event.GetId());
//#endif
////This event is called only when the user left-clicks somewhere, thus deleting all previous selection.
//
//  GDLWidgetTable* table = static_cast<GDLWidgetTable*>(GDLWidget::GetWidget(GDLWidgetTableID));
//  if (!table->GetDisjointSelection()  && event.ControlDown() ) {
//    this->ClearSelection();
//  }
  event.Skip();
}


//Foget this function for the time being!

//  void  GDLGrid::OnTableCellSelection(wxGridEvent & event){
//#ifdef GDL_DEBUG_EVENTS
//  wxMessageOutputStderr().Printf(_T("in GDLGrid::OnTableCellSelection: %d\n"),event.GetId());
//#endif
////This event is called only when the user left-clicks somewhere, thus deleting all previous selection.
//
//  GDLWidgetTable* table = static_cast<GDLWidgetTable*>(GDLWidget::GetWidget(GDLWidgetTableID));
//  DULong eventFlags=table->GetEventFlags();
//
//  if (eventFlags & GDLWidget::EV_ALL && !table->IsUpdating()) {
//    WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase( event.GetId( ) );
//    DStructGDL* widgtablecelsel = new DStructGDL( "WIDGET_TABLE_CELL_SEL");
//    widgtablecelsel->InitTag("ID",  DLongGDL( event.GetId( ) ));
//    widgtablecelsel->InitTag("TOP", DLongGDL( baseWidgetID));
//    widgtablecelsel->InitTag("HANDLER", DLongGDL( baseWidgetID ));
//    widgtablecelsel->InitTag("TYPE",DIntGDL(4)); // 4
//    widgtablecelsel->InitTag("SEL_LEFT",  DLongGDL(event.Selecting()?event.GetCol():-1));
//    widgtablecelsel->InitTag("SEL_TOP",  DLongGDL(event.Selecting()?event.GetRow(): -1 ));
//    widgtablecelsel->InitTag("SEL_RIGHT",  DLongGDL( event.Selecting()?event.GetCol():-1 ));
//    widgtablecelsel->InitTag("SEL_BOTTOM",  DLongGDL( event.Selecting()?event.GetCol():-1 ));
//    GDLWidget::PushEvent( baseWidgetID, widgtablecelsel );
//  }
//  event.Skip();
////For compatibility with idl, we should force to select the current table entry.
////  this->SelectBlock(event.GetRow(),event.GetCol(),event.GetRow(),event.GetCol(),FALSE);
//  if (table->IsUpdating()) {cerr<<"cleared"<<endl; table->ClearUpdating();}
//  }
  
void GDLGrid::OnTextEnter( wxCommandEvent& event)
{
#ifdef GDL_DEBUG_EVENTS
  wxMessageOutputStderr().Printf(_T("in GDLGrid::OnTextEnter: %d\n"),event.GetId());
#endif
}  
void GDLGrid::OnText( wxCommandEvent& event)
{
#ifdef GDL_DEBUG_EVENTS
  wxMessageOutputStderr().Printf(_T("in GDLGrid::OnText: %d\n"),event.GetId());
#endif
  GDLWidgetTable* table = static_cast<GDLWidgetTable*>(GDLWidget::GetWidget(GDLWidgetTableID));
  DULong eventFlags=table->GetEventFlags();

  WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase(GDLWidgetTableID);

  bool isModified;
  long selStart, selEnd;
  DLong offset;
  std::string lastValue;
  std::string newValue;
  
  wxTextCtrl* textCtrl = static_cast<wxTextCtrl*>(event.GetEventObject());
  if( textCtrl == NULL)
  {
    event.Skip();
    return; // happens on construction
  }
  textCtrl->GetSelection( &selStart, &selEnd);
  offset = textCtrl->GetInsertionPoint();
  lastValue = textCtrl->GetLabelText().mb_str(wxConvUTF8);
  newValue = textCtrl->GetValue().mb_str(wxConvUTF8);
  isModified = lastValue != newValue;
//  textCtrl->SetValue(wxString(newValue, wxConvUTF8));
//return without producing event struct if eventType is not set
  if (!(eventFlags & GDLWidget::EV_ALL )) return;

  DStructGDL*  widg;
  if( !isModified)
  {
    widg = new DStructGDL( "WIDGET_TABLE_TEXT_SEL");
    widg->InitTag("ID", DLongGDL( GDLWidgetTableID));
    widg->InitTag("TOP", DLongGDL( baseWidgetID));
    widg->InitTag("HANDLER", DLongGDL( baseWidgetID ));
    widg->InitTag("TYPE", DIntGDL( 3)); // selection
    widg->InitTag("OFFSET", DLongGDL( offset));
    widg->InitTag("LENGTH", DLongGDL( selEnd-selStart));
    widg->InitTag("X", DLongGDL( 0 ));
    widg->InitTag("Y", DLongGDL( 0 ));
  }
  else
  {
    int lengthDiff = newValue.length() - lastValue.length();
    if( lengthDiff < 0) // deleted
    {
      widg = new DStructGDL( "WIDGET_TABLE_DEL");
      widg->InitTag("ID", DLongGDL(  GDLWidgetTableID));
      widg->InitTag("TOP", DLongGDL( baseWidgetID));
      widg->InitTag("HANDLER", DLongGDL( baseWidgetID ));
      widg->InitTag("TYPE", DIntGDL( 2)); // delete
      widg->InitTag("OFFSET", DLongGDL( offset-1));
      widg->InitTag("LENGTH", DLongGDL( -lengthDiff));
      widg->InitTag("X", DLongGDL( 0 ));
      widg->InitTag("Y", DLongGDL( 0 ));
    }
    else if( lengthDiff == 0) // replace TODO: just flag the real change
    {   
      // 1st delete all
      widg = new DStructGDL( "WIDGET_TABLE_DEL");
      widg->InitTag("ID", DLongGDL(  GDLWidgetTableID));
      widg->InitTag("TOP", DLongGDL( baseWidgetID));
      widg->InitTag("HANDLER", DLongGDL( baseWidgetID ));
      widg->InitTag("TYPE", DIntGDL( 2)); // delete
      widg->InitTag("OFFSET", DLongGDL( 0));
      widg->InitTag("LENGTH", DLongGDL( lastValue.length()));
      widg->InitTag("X", DLongGDL( 0 ));
      widg->InitTag("Y", DLongGDL( 0 ));
    
      GDLWidget::PushEvent( baseWidgetID, widg);

      // 2nd insert new
      widg = new DStructGDL( "WIDGET_TABLE_STR");
      widg->InitTag("ID", DLongGDL(  GDLWidgetTableID));
      widg->InitTag("TOP", DLongGDL( baseWidgetID));
      widg->InitTag("HANDLER", DLongGDL( baseWidgetID ));
      widg->InitTag("TYPE", DIntGDL( 1)); // multiple char
      widg->InitTag("OFFSET", DLongGDL( 0));
      widg->InitTag("STR", DStringGDL( newValue));
      widg->InitTag("X", DLongGDL( 0 ));
      widg->InitTag("Y", DLongGDL( 0 ));
    }
    else if( lengthDiff == 1)
    {
      widg = new DStructGDL( "WIDGET_TABLE_CH");
      widg->InitTag("ID", DLongGDL(  GDLWidgetTableID));
      widg->InitTag("TOP", DLongGDL( baseWidgetID));
      widg->InitTag("HANDLER", DLongGDL( baseWidgetID ));
      widg->InitTag("TYPE", DIntGDL( 0)); // single char
      widg->InitTag("OFFSET", DLongGDL( offset+1));
      widg->InitTag("CH", DByteGDL( newValue[offset<newValue.length()?offset:newValue.length()-1]));
      widg->InitTag("X", DLongGDL( 0 ));
      widg->InitTag("Y", DLongGDL( 0 ));
    }
    else // lengthDiff > 1
    {
      int nVLenght = newValue.length();
      if( offset < lengthDiff)  lengthDiff = offset;
      string str = "";
      if( offset <= nVLenght && lengthDiff > 0)	 str = newValue.substr(offset-lengthDiff,lengthDiff+1);
      
      widg = new DStructGDL( "WIDGET_TABLE_STR");
      widg->InitTag("ID", DLongGDL( GDLWidgetTableID));
      widg->InitTag("TOP", DLongGDL( baseWidgetID));
      widg->InitTag("HANDLER", DLongGDL( baseWidgetID ));
      widg->InitTag("TYPE", DIntGDL( 1)); // multiple char
      widg->InitTag("OFFSET", DLongGDL( offset));
      widg->InitTag("STR", DStringGDL( str));
      widg->InitTag("X", DLongGDL( 0 ));
      widg->InitTag("Y", DLongGDL( 0 ));
    }
  }
  GDLWidget::PushEvent( baseWidgetID, widg);
}
  
#endif

