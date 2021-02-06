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

#include <wx/clipbrd.h>

#include "basegdl.hpp"
#include "dstructgdl.hpp"
#include "gdlwidget.hpp"

//  #define GDL_DEBUG_ALL_EVENTS 1
//  #define GDL_DEBUG_MOVE_EVENTS 1
//  #define GDL_DEBUG_KBRD_EVENTS 1
//  #define GDL_DEBUG_TIMER_EVENTS 1
//  #define GDL_DEBUG_VISIBILITY_EVENTS 1
//  #define GDL_DEBUG_SIZE_EVENTS 1
//  #define GDL_DEBUG_TEXT_EVENTS 1
//  #define GDL_DEBUG_BUTTON_EVENTS 1
//  #define GDL_DEBUG_PAINT_EVENTS 1
// #define GDL_DEBUG_OTHER_EVENTS 1

//TO BE DONE: CONTINUE to REPLACE ALL STATIC CONNECTS WITH DYNAMIC. SEE http://wxwidgets.blogspot.com/2007/01/in-praise-of-connect.html
// replace, e.g;
//
//BEGIN_EVENT_TABLE(MyFrame, wxFrame)
//  EVT_MENU(wxID_EXIT, MyFrame::OnQuit)
//END_EVENT_TABLE()
//instead write (in the body of some method of MyFrame and not at global scope as with the event tables)
//    MyFrame::MyFrame(...)
//    {
//      Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED,
//      wxCommandEventHandler(MyFrame::OnQuit));
//    }
// see also the example of how to connect an event of some object to a method of another object.
// and note that in this particular case one should always remember to disconnect the event handler
// in the event handler object's destructor, otherwise the application may crash if/when the handler
// is called after the event handler object is destroyed.
//
// Note that this may be tedious since all the event_table events below are events that can be set or removed
// for each type of widget.
// Besides, the misuse of event.Skip() can pose severe problems (blank widgets, etc...) since (quoting documentation) 
// "Sizers rely on size events to function correctly. Therefore, in a sizer-based layout, do not forget to call Skip
// on all size events you catch (and don't catch size events at all when you don't need to).

DEFINE_EVENT_TYPE(wxEVT_SHOW_REQUEST)
DEFINE_EVENT_TYPE(wxEVT_HIDE_REQUEST)

//general-purpose events still left to Frame:
BEGIN_EVENT_TABLE(gdlwxFrame, wxFrame)
//impossible to replace with Connect() method?
  EVT_MENU(wxID_ANY, gdlwxFrame::OnMenu) 
  //timed resize events happen only on one frame at a time, so the timer ID is fixed here.
  // If this was not the case, then remove this line and Connect() to the timer at the timer creation place.
  EVT_TIMER(RESIZE_TIMER, gdlwxFrame::OnTimerResize) //... where size event is really done here. But does not work (refresh not OK)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(gdlwxPlotFrame, wxFrame)
  EVT_TIMER(RESIZE_PLOT_TIMER, gdlwxPlotFrame::OnTimerPlotResize) 
END_EVENT_TABLE()

wxIMPLEMENT_APP_NO_MAIN( wxAppGDL);

void wxTextCtrlGDL::OnChar(wxKeyEvent& event ) {
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_KBRD_EVENTS )
  wxMessageOutputStderr().Printf(_T("in gdlTextCtrl::OnChar: %d Keycode:%d\n"),event.GetId(),event.GetKeyCode( ));
#endif

  //I cannot get cw_field to work if OnChar is not overwritten as it is here
  GDLWidgetText* txt = static_cast<GDLWidgetText*>(GDLWidget::GetWidget( event.GetId()));
  if( txt == NULL)
  {
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_KBRD_EVENTS)
    wxMessageOutputStderr().Printf(_T("GDLWidget == NULL: %d\n"),event.GetId());
#endif
    event.Skip();
    return;
  }
  DStructGDL* widg;
  bool report = txt->HasEventType( GDLWidget::EV_ALL );
  bool edit = txt->IsEditable( );
  WidgetIDT baseWidgetID = GDLWidget::GetIdOfTopLevelBase( event.GetId( ) );
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
      return;
      }
      event.Skip( );
      return;
    }
  } else { //report ALL (text) events, editable or not but skip on <TAB>
    int sign = 1;
    long from, to;
    this->GetSelection( &from, &to );
    long oldpos = this->GetInsertionPoint( ), newpos;
    switch ( event.GetKeyCode( ) ) {
      case WXK_TAB:
        event.Skip();  return;
        break;
      case WXK_END:
        newpos = this->GetLastPosition( );
        this->SetInsertionPoint( newpos );
        break;
      case WXK_HOME:
        newpos = 0;
        this->SetInsertionPoint( newpos );
        break;
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
        if (edit) event.Skip( );
        return;
        break;
      case WXK_UP: //these two, I cannot compute the next line (or preceding line) postion with the methods provided 
                   //by wxWidgets. So I Skip(). At least most of the job is done.
      case WXK_DOWN:
        event.Skip();
        return;
        break;
      case WXK_BACK:
      case WXK_DELETE: //perform identically on idl/linux.
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
      case WXK_RETURN: //important, *DL returns CH=10 instead of 13 for <CR>
        widg = new DStructGDL( "WIDGET_TEXT_CH" );
        widg->InitTag( "ID", DLongGDL( event.GetId( ) ) );
        widg->InitTag( "TOP", DLongGDL( baseWidgetID ) );
        widg->InitTag( "HANDLER", DLongGDL( baseWidgetID ) );
        widg->InitTag( "TYPE", DIntGDL( 0 ) ); // selection
        widg->InitTag( "OFFSET", DLongGDL( from ) );
        widg->InitTag( "CH", DByteGDL( 10 ) );
        GDLWidget::PushEvent( baseWidgetID, widg );
        if (edit) event.Skip( );
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
    if (edit) event.Skip( );
}

//this is necessary to reproduce IDL's behaviour
void wxTextCtrlGDL::OnMouseEvents( wxMouseEvent& event)
{
  GDLWidgetText* txt = static_cast<GDLWidgetText*>(GDLWidget::GetWidget( event.GetId()));
  if( txt == NULL)
  {
    event.Skip();
    return;
  }
  int rot=event.GetWheelRotation();
  if (rot>0) this->ScrollLines(-1); else this->ScrollLines(1);
}

 void gdlwxFrame::OnWidgetTimer( wxTimerEvent& event)
 {
//originating widget is stored in clientData
    WidgetIDT * originating_id=static_cast<WidgetIDT*>(this->GetEventHandler()->GetClientData());
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_TIMER_EVENTS)
   wxMessageOutputStderr().Printf(_T("in gdlwxFrame::OnWidgetTimer: %d issued by: %d\n"),event.GetId(),*originating_id);
#endif

//Base Widget (frame) is retrieved such:
    WidgetIDT baseWidgetID = GDLWidget::GetIdOfTopLevelBase( *originating_id );
  // create GDL event struct
    DStructGDL* widgtimer = new DStructGDL( "WIDGET_TIMER" );
    widgtimer->InitTag( "ID", DLongGDL(*originating_id));
    widgtimer->InitTag( "TOP", DLongGDL( baseWidgetID ) );
    widgtimer->InitTag( "HANDLER", DLongGDL(*originating_id) ); //will be set elsewhere
    GDLWidget::PushEvent( baseWidgetID, widgtimer);
 }

void gdlwxFrame::OnShowRequest( wxCommandEvent& event)
{
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_VISIBILITY_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlwxFrame::OnShowRequest: %d\n"),event.GetId());
#endif

  GDLWidget* widget = GDLWidget::GetWidget( event.GetId());
  if( widget == NULL)
  {
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_VISIBILITY_EVENTS)
    wxMessageOutputStderr().Printf(_T("GDLWidget == NULL: %d\n"),event.GetId());
#endif
    return;
  }
  gdlwxFrame *frame = static_cast<gdlwxFrame*>( widget->GetWxWidget());
  if( !frame->IsShown())
  {
//    if (frame->isPlotFrame()) frame->ShowWithoutActivating();
//    else 
      bool stat = frame->Show(true);
  }
  event.Skip();
}

void gdlwxFrame::OnHideRequest( wxCommandEvent& event)
{  
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_VISIBILITY_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlwxFrame::OnHideRequest: %d\n"),event.GetId());
#endif

  GDLWidget* widget = GDLWidget::GetWidget( event.GetId());
  if( widget == NULL)
  {
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_VISIBILITY_EVENTS)
    wxMessageOutputStderr().Printf(_T("GDLWidget == NULL: %d\n"),event.GetId());
#endif
    return;
  }
  gdlwxFrame *frame = static_cast<gdlwxFrame*>( widget->GetWxWidget());
  if( frame->IsShown())
  {
    bool stat = frame->Show(false);
  }
  event.Skip();
}

void wxButtonGDL::OnButton( wxCommandEvent& event)
{
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_BUTTON_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlMenuButton::OnButton: %d\n"),event.GetId());
#endif
 this->PopupMenu(dynamic_cast<wxMenu*>(popupMenu));
 event.Skip();
}

void wxBitmapButtonGDL::OnButton( wxCommandEvent& event)
{
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_BUTTON_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlMenuButtonBitmap::OnButton: %d\n"),event.GetId());
#endif
 this->PopupMenu(dynamic_cast<wxMenu*>(popupMenu));
 event.Skip();
}

void gdlwxFrame::OnButton( wxCommandEvent& event)
{
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_BUTTON_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlwxFrame::OnButton: %d\n"),event.GetId());
#endif

  WidgetIDT baseWidgetID = GDLWidget::GetIdOfTopLevelBase( event.GetId());

  // create GDL event struct
  DStructGDL*  widgbut = new DStructGDL( "WIDGET_BUTTON");
  widgbut->InitTag("ID", DLongGDL( event.GetId()));
  widgbut->InitTag("TOP", DLongGDL( baseWidgetID));
  widgbut->InitTag("HANDLER", DLongGDL( baseWidgetID ));
  widgbut->InitTag("SELECT", DLongGDL( 1));

  GDLWidget::PushEvent( baseWidgetID, widgbut);
}

//change bitmap or parent button?
void gdlwxFrame::OnMenu( wxCommandEvent& event)
{
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_BUTTON_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlwxFrame::OnMenu: %d\n"),event.GetId());
#endif

  WidgetIDT baseWidgetID = GDLWidget::GetIdOfTopLevelBase( event.GetId());
  // create GDL event struct
  DStructGDL*  widgbut = new DStructGDL( "WIDGET_BUTTON");
  widgbut->InitTag("ID", DLongGDL( event.GetId()));
  widgbut->InitTag("TOP", DLongGDL( baseWidgetID));
  widgbut->InitTag("HANDLER", DLongGDL( baseWidgetID ));
  widgbut->InitTag("SELECT", DLongGDL( 1));

  GDLWidget::PushEvent( baseWidgetID, widgbut);
}

void gdlwxFrame::OnRadioButton( wxCommandEvent& event)
{
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_BUTTON_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlwxFrame::OnRadioButton: %d\n"),event.GetId());
#endif

  WidgetIDT baseWidgetID = GDLWidget::GetIdOfTopLevelBase( event.GetId());

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

    GDLWidgetButton* widget = static_cast<GDLWidgetButton*>(GDLWidget::GetWidget( lastSelection));
    assert(widget!=NULL);
    widget->SetButton( false);
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

void gdlwxFrame::OnCheckBox( wxCommandEvent& event)
{
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_BUTTON_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlwxFrame::OnCheckBox: %d\n"),event.GetId());
#endif

  bool selectValue = event.IsChecked();
  
  WidgetIDT baseWidgetID = GDLWidget::GetIdOfTopLevelBase( event.GetId());
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

void gdlwxFrame::OnComboBox( wxCommandEvent& event)
{
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_OTHER_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlwxFrame::OnComboBox: %d\n"),event.GetId());
#endif

  WidgetIDT baseWidgetID = GDLWidget::GetIdOfTopLevelBase( event.GetId());
  int selectValue = event.GetSelection();
  
  // create GDL event struct
  DStructGDL*  widgcbox;
  wxString strValue = event.GetString();

  widgcbox = new DStructGDL( "WIDGET_COMBOBOX");
  widgcbox->InitTag("ID", DLongGDL( event.GetId()));
  widgcbox->InitTag("TOP", DLongGDL( baseWidgetID));
  widgcbox->InitTag("HANDLER", DLongGDL( baseWidgetID ));
  widgcbox->InitTag("INDEX", DLongGDL( selectValue));
  widgcbox->InitTag("STR", DStringGDL( std::string(strValue.mb_str(wxConvUTF8)) ));

  GDLWidget::PushEvent( baseWidgetID, widgcbox);
}
void gdlwxFrame::OnComboBoxTextEnter( wxCommandEvent& event)
{
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_TEXT_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlwxFrame::OnComboBoxTextEvent: %d\n"),event.GetId());
#endif

  WidgetIDT baseWidgetID = GDLWidget::GetIdOfTopLevelBase( event.GetId());
  int selectValue = event.GetSelection();
  
  // create GDL event struct
  DStructGDL*  widgcbox;
  wxString strValue = event.GetString();

  widgcbox = new DStructGDL( "WIDGET_COMBOBOX");
  widgcbox->InitTag("ID", DLongGDL( event.GetId()));
  widgcbox->InitTag("TOP", DLongGDL( baseWidgetID));
  widgcbox->InitTag("HANDLER", DLongGDL( baseWidgetID ));
  widgcbox->InitTag("INDEX", DLongGDL( selectValue));
  widgcbox->InitTag("STR", DStringGDL( std::string(strValue.mb_str(wxConvUTF8)) ));

  GDLWidget::PushEvent( baseWidgetID, widgcbox);
}
void gdlwxFrame::OnDropList( wxCommandEvent& event)
{
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_OTHER_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlwxFrame::OnDropList: %d\n"),event.GetId());
#endif

  WidgetIDT baseWidgetID = GDLWidget::GetIdOfTopLevelBase( event.GetId());
  int selectValue = event.GetSelection();
  
  DStructGDL*  widdrplst;
  widdrplst = new DStructGDL( "WIDGET_DROPLIST");
  widdrplst->InitTag("ID", DLongGDL( event.GetId()));
  widdrplst->InitTag("TOP", DLongGDL( baseWidgetID));
  widdrplst->InitTag("HANDLER", DLongGDL( baseWidgetID ));
  widdrplst->InitTag("INDEX", DLongGDL( selectValue));   

  GDLWidget::PushEvent( baseWidgetID, widdrplst);
}

void gdlwxFrame::OnListBoxDo( wxCommandEvent& event, DLong clicks)
{
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_OTHER_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlwxFrame::OnListBoxDo: %d\n"),event.GetId());
#endif

  WidgetIDT baseWidgetID = GDLWidget::GetIdOfTopLevelBase( event.GetId());

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
void gdlwxFrame::OnListBox( wxCommandEvent& event)
{
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_OTHER_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlwxFrame::OnListBox: %d\n"),event.GetId());
#endif

  OnListBoxDo( event, 1);
}
void gdlwxFrame::OnListBoxDoubleClicked( wxCommandEvent& event)
{
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_OTHER_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlwxFrame::OnListBoxDoubleClicked: %d\n"),event.GetId());
#endif

// Note: IDL sends one click AND two click event on double click  
  OnListBoxDo( event, 1);
  OnListBoxDo( event, 2);
}

//this is necessary to reproduce IDL's behaviour
void gdlwxFrame::OnTextMouseEvents( wxMouseEvent& event)
{
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_TEXT_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlwxFrame::OnTextMouseEvents: %d\n"),event.GetId());
#endif
  GDLWidgetText* txt = static_cast<GDLWidgetText*>(GDLWidget::GetWidget( event.GetId()));
  if( txt == NULL)
  {
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_KBRD_EVENTS)
    wxMessageOutputStderr().Printf(_T("GDLWidget == NULL: %d\n"),event.GetId());
#endif
    event.Skip();
    return;
  }
  DStructGDL* widg;
  bool report = txt->HasEventType( GDLWidget::EV_ALL );
  bool edit = txt->IsEditable( );
  WidgetIDT baseWidgetID = GDLWidget::GetIdOfTopLevelBase( event.GetId( ) );
  if ( report ) {
    wxTextCtrl* textCtrl = dynamic_cast<wxTextCtrl*>(txt->GetWxWidget());
    if( textCtrl == NULL)
    {
      event.Skip();
      std::cerr<<"No wxWidget!"<<std::endl; return; // happens on construction 
    }
    //signal entry (even if offset is bad)
    if ( event.ButtonDown(wxMOUSE_BTN_LEFT) ) {
      widg = new DStructGDL( "WIDGET_TEXT_SEL");
      widg->InitTag("ID", DLongGDL( event.GetId()));
      widg->InitTag("TOP", DLongGDL( baseWidgetID));
      widg->InitTag("HANDLER", DLongGDL( baseWidgetID ));
      widg->InitTag("TYPE", DIntGDL( 3)); // selection
      widg->InitTag("OFFSET", DLongGDL( 0 ));
      widg->InitTag("LENGTH", DLongGDL( 0 ));
      GDLWidget::PushEvent( baseWidgetID, widg);
      event.Skip( );
    }
    return;
    //middle button (paste) should be filtered. Here we just avoid the paste pollutes a not-edit widget (without creating a GDL event).
    // So we do not do anything when report is true (as it will not be properly reported)
  }
  event.Skip( );
}

void gdlwxFrame::OnTextPaste( wxClipboardTextEvent& event)
{
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_TEXT_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlwxFrame::OnTextPaste: %d\n"),event.GetId());
#endif
  WidgetIDT baseWidgetID = GDLWidget::GetIdOfTopLevelBase( event.GetId());
  
  GDLWidgetText* widget = static_cast<GDLWidgetText*>(GDLWidget::GetWidget( event.GetId()));
  if( widget == NULL)  {    event.Skip();    return;  }
  bool report = widget->HasEventType( GDLWidget::EV_ALL );
  bool edit = widget->IsEditable( );
  if (report) {
    wxTextCtrl* textCtrl = dynamic_cast<wxTextCtrl*>(widget->GetWxWidget());
    if( textCtrl == NULL)
    {
      event.Skip();
      std::cerr<<"No wxWidget!"<<std::endl; return; // happens on construction 
    }
    wxString s;
    if (wxTheClipboard->Open())
    {
      if (wxTheClipboard->IsSupported( wxDF_TEXT ))
      {
          wxTextDataObject data;
          wxTheClipboard->GetData( data );
          s=data.GetText();
      }
      wxTheClipboard->Close();
    }
    long pos=textCtrl->GetInsertionPoint();
    DStructGDL*  widg;
    widg = new DStructGDL( "WIDGET_TEXT_STR");
    widg->InitTag("ID", DLongGDL( event.GetId()));
    widg->InitTag("TOP", DLongGDL( baseWidgetID));
    widg->InitTag("HANDLER", DLongGDL( baseWidgetID ));
    widg->InitTag("TYPE", DIntGDL( 1)); // multiple char
    widg->InitTag("OFFSET", DLongGDL( pos+s.length() ));
    widg->InitTag("STR", DStringGDL(std::string(s.c_str())));
    GDLWidget::PushEvent( baseWidgetID, widg);
    if (edit) event.Skip();
    return;
  }
  event.Skip();
}

void gdlwxFrame::OnTextCut( wxClipboardTextEvent& event)
{
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_TEXT_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlwxFrame::OnTextCut: %d\n"),event.GetId());
#endif
  
  GDLWidgetText* widget = static_cast<GDLWidgetText*>(GDLWidget::GetWidget( event.GetId()));
  if( widget == NULL)
  {
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_TEXT_EVENTS)
    wxMessageOutputStderr().Printf(_T("GDLWidget == NULL: %d\n"),event.GetId());
#endif
    event.Skip();
    return;
  }
  bool report = widget->HasEventType( GDLWidget::EV_ALL );
  bool edit = widget->IsEditable( );
  if (report) {
  //Do nothing, better than anything.
    return;
  }
  event.Skip();
}

void gdlwxFrame::OnText( wxCommandEvent& event)
{
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_TEXT_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlwxFrame::OnText: %d\n"),event.GetId());
#endif

  WidgetIDT baseWidgetID = GDLWidget::GetIdOfTopLevelBase( event.GetId());

  bool isModified=false;
  long selStart, selEnd;
  DLong offset;
  std::string lastValue;
  std::string newValue;
  
  GDLWidget* widget = GDLWidget::GetWidget( event.GetId());
  if( widget == NULL)
  {
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_TEXT_EVENTS)
    wxMessageOutputStderr().Printf(_T("GDLWidget == NULL: %d\n"),event.GetId());
#endif
    event.Skip();
    return;
  }
  if( widget->IsText())
  {
    wxTextCtrl* textCtrl = dynamic_cast<wxTextCtrl*>(widget->GetWxWidget());
    if( textCtrl == NULL)
    {
      event.Skip();
      std::cerr<<"No wxWidget!"<<std::endl; return; // happens on construction 
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
    wxComboBox* control = dynamic_cast<wxComboBox*>(widget->GetWxWidget());
    if( control == NULL)
    {
      event.Skip();
      std::cerr<<"No wxWidget!"<<std::endl; return; // happens on construction 
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
    wxChoice* control = dynamic_cast<wxChoice*>(widget->GetWxWidget());
    if( control == NULL)
    {
      event.Skip();
      std::cerr<<"No wxWidget!"<<std::endl; return; // happens on construction 
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
      std::string str = "";
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


//NOT USED
//void gdlwxFrame::OnTextEnter( wxCommandEvent& event)
//{
//#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_TEXT_EVENTS)
//  wxMessageOutputStderr().Printf(_T("in gdlwxFrame::OnTextEnter: %d\n"),event.GetId());
//#endif
//
//  WidgetIDT baseWidgetID = GDLWidget::GetIdOfTopLevelBase( event.GetId());
//
//  DLong offset;
//  std::string newValue;
//
//  GDLWidget* widget = GDLWidget::GetWidget( event.GetId());
//  if( widget == NULL)
//  {
//#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_TEXT_EVENTS)
//    wxMessageOutputStderr().Printf(_T("GDLWidget == NULL: %d\n"),event.GetId());
//#endif
//    event.Skip();
//    return;
//  }
//  if( widget->IsText())
//  {
//    wxTextCtrl* textCtrl = dynamic_cast<wxTextCtrl*>(widget->GetWxWidget());
//    offset = textCtrl->GetInsertionPoint();
//    newValue = textCtrl->GetValue().mb_str(wxConvUTF8);
//    static_cast<GDLWidgetText*>(widget)->SetLastValue(newValue);
//  // create GDL event struct
//  DStructGDL*  widg;
//  widg = new DStructGDL( "WIDGET_TEXT_CH");
//  widg->InitTag("ID", DLongGDL( event.GetId()));
//  widg->InitTag("TOP", DLongGDL( baseWidgetID));
//  widg->InitTag("HANDLER", DLongGDL( baseWidgetID ));
//  widg->InitTag("TYPE", DIntGDL( 0)); // single char
//  widg->InitTag("OFFSET", DLongGDL( offset));
//  widg->InitTag("CH", DByteGDL( '\n'));
//
//  GDLWidget::PushEvent( baseWidgetID, widg);
//  }
//  else if( widget->IsComboBox())
//  {
//  // create GDL event struct
//  DStructGDL*  widgcbox;
//  int selectValue = event.GetSelection();
//  wxString strValue = event.GetString();
//
//  widgcbox = new DStructGDL( "WIDGET_COMBOBOX");
//  widgcbox->InitTag("ID", DLongGDL( event.GetId()));
//  widgcbox->InitTag("TOP", DLongGDL( baseWidgetID));
//  widgcbox->InitTag("HANDLER", DLongGDL( baseWidgetID ));
//  widgcbox->InitTag("INDEX", DLongGDL( selectValue));
//  widgcbox->InitTag("STR", DStringGDL( std::string(strValue.mb_str(wxConvUTF8)) ));
//
//  GDLWidget::PushEvent( baseWidgetID, widgcbox);
//  }
//  else
//  {
//  assert( widget->IsDropList());
//  // create GDL event struct
//  DStructGDL*  widdrplst;
//  int selectValue = event.GetSelection();
//  widdrplst = new DStructGDL( "WIDGET_DROPLIST");
//  widdrplst->InitTag("ID", DLongGDL( event.GetId()));
//  widdrplst->InitTag("TOP", DLongGDL( baseWidgetID));
//  widdrplst->InitTag("HANDLER", DLongGDL( baseWidgetID ));
//  widdrplst->InitTag("INDEX", DLongGDL( selectValue));   
//
//  GDLWidget::PushEvent( baseWidgetID, widdrplst);
//  }
//}

void gdlwxFrame::OnPageChanged( wxNotebookEvent& event)
{  
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_OTHER_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlwxFrame::OnPageChanged: %d\n"),event.GetId());
#endif

  WidgetIDT baseWidgetID = GDLWidget::GetIdOfTopLevelBase( event.GetId());
 
  // create GDL event struct
  DStructGDL*  widg;
  widg = new DStructGDL( "WIDGET_TAB");
  widg->InitTag("ID", DLongGDL( event.GetId()));
  widg->InitTag("TOP", DLongGDL( baseWidgetID));
  widg->InitTag("HANDLER", DLongGDL( baseWidgetID ));
  widg->InitTag("TAB", DLongGDL( event.GetSelection()));
  
  GDLWidget::PushEvent( baseWidgetID, widg);
}

//Timer-filtered resizing for graphics.

// mouse.LeftIsDown() is not present before wxWidgets  2.8.12 , find an alternative.
void gdlwxPlotFrame::OnTimerPlotResize(wxTimerEvent& event)
{
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_TIMER_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlwxFrame::OnTimerPlotResize: %d (%d,%d).."), event.GetId(), frameSize.x, frameSize.y);
#endif
  wxMouseState mouse = wxGetMouseState();
#if wxCHECK_VERSION(3,0,0)
  if (mouse.LeftIsDown()) {
#else
  if (mouse.LeftDown()) {
#endif
    m_resizeTimer->Start(50, wxTIMER_ONE_SHOT);
    return;
  }
  //there may have been size events since the start of the timer. Process them.
//   frameSize=this->GetSize();
   gdlwxGraphicsPanel* w = dynamic_cast<gdlwxGraphicsPanel*> (this->GetChildren().GetFirst()->GetData());
   if (w==NULL)     {
      event.Skip();
      std::cerr<<"No wxWidget!"<<std::endl; return; // happens on construction 
    }
   wxSizeEvent sizeEvent(frameSize, w->GetId());
   w->OnPlotWindowSize(sizeEvent);
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_TIMER_EVENTS)
   wxMessageOutputStderr().Printf(_T("gdlwxFrame::OnTimerPlotResize() has been Processed.\n"));
#endif
}

void gdlwxPlotFrame::OnPlotSizeWithTimer(wxSizeEvent& event) {
  wxSize newSize = event.GetSize();
  if (frameSize == newSize) {
    event.Skip();
    return;
  }
  frameSize = newSize; //no cost
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_SIZE_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlwxPlotWindow::OnPlotSizeWithTimer: new framesize is %d %d ; mouseState %d & Processed.\n"), frameSize.x, frameSize.y, wxGetMouseState().LeftIsDown());
#endif
  //is it a resize of frame due to a manual intervention?
  wxMouseState mouse = wxGetMouseState();
#if wxCHECK_VERSION(3,0,0)
  if (mouse.LeftIsDown()) {
#else
  if (mouse.LeftDown()) {
#endif
    m_resizeTimer->Start(50, wxTIMER_ONE_SHOT);
    return;
  } else { //take it immediately.
    m_resizeTimer->StartOnce(1);
  }
}


//same for widget_draw
// mouse.LeftIsDown() is not present before wxWidgets  2.8.12 , find an alternative.
void gdlwxFrame::OnTimerResize(wxTimerEvent& event)
{
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_TIMER_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlwxFrame::OnTimerResize: %d (%d,%d).."), event.GetId(), frameSize.x, frameSize.y);
#endif
  wxMouseState mouse = wxGetMouseState();
#if wxCHECK_VERSION(3,0,0)
  if (mouse.LeftIsDown()) {
#else
  if (mouse.LeftDown()) {
#endif
    m_resizeTimer->Start(50, wxTIMER_ONE_SHOT);
    return;
  }
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_TIMER_EVENTS)
  wxMessageOutputStderr().Printf(_T("Processed.\n"));
#endif
    if (!gdlOwner) {event.Skip(); return;}
  GDLWidget* owner = static_cast<GDLWidget*> (gdlOwner);
  if (!owner) return;
  DULong flags = 0;
  if (owner) flags = owner->GetEventFlags();
  if (flags & GDLWidget::EV_SIZE) {
    WidgetIDT baseWidgetID = GDLWidget::GetIdOfTopLevelBase(owner->GetWidgetID());
    // create GDL event struct
    DStructGDL* widgbase = new DStructGDL("WIDGET_BASE");
    widgbase->InitTag("ID", DLongGDL(owner->GetWidgetID()));
    widgbase->InitTag("TOP", DLongGDL(baseWidgetID));
    widgbase->InitTag("HANDLER", DLongGDL(baseWidgetID));
    widgbase->InitTag("X", DLongGDL(frameSize.x));
    widgbase->InitTag("Y", DLongGDL(frameSize.y));
    GDLWidget::PushEvent(baseWidgetID, widgbase);
  }
  event.Skip();
}
 
//The idea is to avoid redraw as long as the user has not released the mouse (doing a resize of the window).
// if the event happens and the mouse is pressed, we register the new size and start a 50 ms timer that will, if not canceled, resize the window.
// The timer will be canceled and the actual size processed if the event shows that the mouse is not pressed anymore.
void gdlwxFrame::OnSizeWithTimer(wxSizeEvent& event)
{
  wxSize newSize = event.GetSize();
  if (frameSize == newSize) {event.Skip();return;}
  frameSize = newSize; //no cost
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_SIZE_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlwxFrame::OnSizeWithTimer: new framesize is %d %d ; mouseState %d & Processed.\n"), frameSize.x, frameSize.y, wxGetMouseState().LeftIsDown());
#endif
  //is it a resize of frame due to a manual intervention?
  wxMouseState mouse = wxGetMouseState();
#if wxCHECK_VERSION(3,0,0)
  if (mouse.LeftIsDown()) {
#else
  if (mouse.LeftDown()) {
#endif
    m_resizeTimer->Start(50, wxTIMER_ONE_SHOT);
    return;
  } else { //take it immediately.
    m_resizeTimer->StartOnce(1);
  }
}
     
  //must override wxWidget's OnSize method
 void gdlwxFrame::OnSize( wxSizeEvent& event)
 {
    if (!gdlOwner) {event.Skip(); return;}
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_SIZE_EVENTS)
   wxMessageOutputStderr().Printf(_T("in gdlwxFrame::OnSize: %d.."),event.GetId());
#endif
  // current bug ? Still OK?
  // onsize should not be called 1) when TLB is resized with widget_control
  // and 2) only at the end of a mouse resizing (wxSizeEvent is sent multiple times
  // when resizing a TLB. But variant with timer does not work correctly...)
    if (!gdlOwner) {event.Skip(); return;}
  GDLWidget* owner=static_cast<GDLWidget*>(gdlOwner);
  if (owner->GetParentID() != 0) {
    event.Skip();
    return; //ignore non-TLB size events.
  }
  wxSize newSize = event.GetSize();
  if (frameSize == newSize) {event.Skip();return;}
  frameSize=newSize;
  DULong flags=0;
  if( owner ) flags=owner->GetEventFlags();
  if (flags & GDLWidget::EV_SIZE) {
    WidgetIDT baseWidgetID = GDLWidget::GetIdOfTopLevelBase(owner->GetWidgetID());

    // create GDL event struct
    DStructGDL* widgbase = new DStructGDL("WIDGET_BASE");
    widgbase->InitTag("ID", DLongGDL(event.GetId()));
    widgbase->InitTag("TOP", DLongGDL(baseWidgetID));
    widgbase->InitTag("HANDLER", DLongGDL(baseWidgetID));
    widgbase->InitTag("X", DLongGDL(frameSize.x));
    widgbase->InitTag("Y", DLongGDL(frameSize.y));
    GDLWidget::PushEvent(baseWidgetID, widgbase);
  }
  event.Skip(); //important, pass to others!
 #if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_SIZE_EVENTS)
   wxMessageOutputStderr().Printf(_T("Processed.\n"));
 #endif
 } 

void gdlwxFrame::OnThumbTrack( wxScrollEvent& event)
{  
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_OTHER_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlwxFrame::OnScroll: %d\n"),event.GetId());
#endif

  GDLWidget* widget = GDLWidget::GetWidget( event.GetId());  
  WidgetIDT baseWidgetID = GDLWidget::GetIdOfTopLevelBase( event.GetId());

//only slider for now:  if( widget->IsSlider())
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
//  }
}

void gdlwxFrame::OnThumbRelease( wxScrollEvent& event)
{  
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_OTHER_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlwxFrame::OnThumbRelease: %d\n"),event.GetId());
#endif

  GDLWidget* widget = GDLWidget::GetWidget( event.GetId());

  WidgetIDT baseWidgetID = GDLWidget::GetIdOfTopLevelBase( event.GetId());

//only slider for now: if( widget->IsSlider())
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

//TRACKING.
void gdlwxFrame::OnEnterWindow( wxMouseEvent &event ) {
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_VISIBILITY_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlwxFrame::OnEnterWindow: %d\n"),event.GetId());
#endif
 GDLWidget* widget = GDLWidget::GetWidget( event.GetId());
  if( widget == NULL) {
     event.Skip();
     return;
  }
  if ( widget->GetEventFlags() & GDLWidget::EV_TRACKING ) {
    WidgetIDT baseWidgetID = GDLWidget::GetIdOfTopLevelBase( event.GetId( ) );
    DStructGDL* widgtracking = new DStructGDL( "WIDGET_TRACKING" );
    widgtracking->InitTag( "ID", DLongGDL( event.GetId( ) ) );
    widgtracking->InitTag( "TOP", DLongGDL( baseWidgetID ) );
    widgtracking->InitTag( "HANDLER", DLongGDL( baseWidgetID ) );
    widgtracking->InitTag( "ENTER", DIntGDL( 1 ) ); 
    GDLWidget::PushEvent( baseWidgetID, widgtracking );
  } //normal end of event processing!
  event.Skip();
}

void gdlwxFrame::OnLeaveWindow( wxMouseEvent &event ) {
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_VISIBILITY_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlwxFrame::OnLeaveWindow: %d\n"),event.GetId());
#endif
 GDLWidget* widget = GDLWidget::GetWidget( event.GetId());
  if( widget == NULL) {
     event.Skip();
     return;
  }
  if ( widget->GetEventFlags() & GDLWidget::EV_TRACKING ) {
    WidgetIDT baseWidgetID = GDLWidget::GetIdOfTopLevelBase( event.GetId( ) );
    DStructGDL* widgtracking = new DStructGDL( "WIDGET_TRACKING" );
    widgtracking->InitTag( "ID", DLongGDL( event.GetId( ) ) );
    widgtracking->InitTag( "TOP", DLongGDL( baseWidgetID ) );
    widgtracking->InitTag( "HANDLER", DLongGDL( baseWidgetID ) );
    widgtracking->InitTag( "ENTER", DIntGDL( 0 ) ); 
    GDLWidget::PushEvent( baseWidgetID, widgtracking );
  } //normal end of event processing!
  event.Skip();
}

void gdlwxFrame::OnKBRDFocusChange( wxFocusEvent &event ) {
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_KBRD_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlwxFrame::OnFocusChange: %d\n"),event.GetId());
#endif
  GDLWidget* widget = GDLWidget::GetWidget( event.GetId());
  if( widget == NULL) {event.Skip(); return;}  
  if ( widget->GetEventFlags() & GDLWidget::EV_KBRD_FOCUS ) {
    WidgetIDT baseWidgetID = GDLWidget::GetIdOfTopLevelBase( event.GetId( ) );
    DStructGDL* widgkbrdfocus = new DStructGDL( "WIDGET_KBRD_FOCUS" );
    widgkbrdfocus->InitTag( "ID", DLongGDL( event.GetId( ) ) );
    widgkbrdfocus->InitTag( "TOP", DLongGDL( baseWidgetID ) );
    widgkbrdfocus->InitTag( "HANDLER", DLongGDL( baseWidgetID ) );
    int enter=(event.GetEventType()==wxEVT_SET_FOCUS);
    widgkbrdfocus->InitTag( "ENTER", DIntGDL( enter ) ); 
    GDLWidget::PushEvent( baseWidgetID, widgkbrdfocus );
  } //normal end of event processing!
  event.Skip(); //"The focus event handlers should almost invariably call wxEvent::Skip() on their event argument to allow the default handling to take place. Failure to do this may result in incorrect behaviour of the native controls."
}

void gdlwxFrame::OnContextEvent( wxContextMenuEvent& event) {
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_OTHER_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlwxFrame::OnContextEvent: %d\n"),event.GetId());
#endif
 GDLWidget* widget = GDLWidget::GetWidget( event.GetId());
 WidgetIDT baseWidgetID;
 WidgetIDT eventID;
 if( widget == NULL) {
    if (!gdlOwner) {event.Skip(); return;}
    GDLWidget* owner=static_cast<GDLWidgetBase*>(gdlOwner);
    if( owner == NULL)
   {
     event.Skip();
     return;
  } else {
      widget=owner;
      baseWidgetID = owner->GetWidgetID();
      eventID = owner->GetWidgetID();
   }
  } else {
    baseWidgetID = GDLWidget::GetIdOfTopLevelBase( event.GetId( ) );
    eventID = event.GetId( );
  }
  if ( widget->GetEventFlags() & GDLWidget::EV_CONTEXT ) {
    DStructGDL* widgcontext = new DStructGDL( "WIDGET_CONTEXT" );
    widgcontext->InitTag( "ID", DLongGDL( eventID ) );
    widgcontext->InitTag( "TOP", DLongGDL( baseWidgetID ) );
    widgcontext->InitTag( "HANDLER", DLongGDL( baseWidgetID ) );
    wxPoint position=event.GetPosition( );
    if (position==wxDefaultPosition) { //issued from keyboard
      position=wxGetMousePosition();
    }
      widgcontext->InitTag( "X", DLongGDL( position.x ) );
      widgcontext->InitTag( "Y", DLongGDL( position.y ) );
//ROW AND COL, to be added for TABLE Widget
    if (widget->IsTable()){
      wxGridGDL * grid = dynamic_cast<wxGridGDL*> (widget->GetWxWidget());
      int col = grid->XToCol(position.x);
      int row = grid->YToRow(position.y);
      widgcontext->InitTag( "ROW", DLongGDL( row ) );
      widgcontext->InitTag( "ROW", DLongGDL( col ) );
    }
    GDLWidget::PushEvent( baseWidgetID, widgcontext );
  } //normal end of event processing!
}

void gdlwxFrame::OnIconize( wxIconizeEvent & event)
{
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_VISIBILITY_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlwxFrame::OnIconize: %d\n"),event.GetId());
#endif
  GDLWidget* widget = GDLWidget::GetWidget( event.GetId());
  if( widget == NULL) {event.Skip(); return;}  
  if (!gdlOwner) {event.Skip(); return;}
  GDLWidget* owner=static_cast<GDLWidget*>(gdlOwner);
  DULong flags=0;
  if( owner ) flags=owner->GetEventFlags();
  if ( flags & GDLWidget::EV_ICONIFY ) {
    WidgetIDT baseWidgetID = GDLWidget::GetIdOfTopLevelBase( event.GetId( ) );
    DStructGDL* widgtlb_iconify_events = new DStructGDL( "WIDGET_TLB_ICONIFY" );
    widgtlb_iconify_events->InitTag( "ID", DLongGDL( event.GetId( ) ) );
    widgtlb_iconify_events->InitTag( "TOP", DLongGDL( baseWidgetID ) );
    widgtlb_iconify_events->InitTag( "HANDLER", DLongGDL( baseWidgetID ) );
    widgtlb_iconify_events->InitTag( "ICONIFIED", DIntGDL( event.IsIconized() ) ); 
    GDLWidget::PushEvent( baseWidgetID, widgtlb_iconify_events );
  } //normal end of event processing!
}

void gdlwxFrame::OnMove( wxMoveEvent & event)
{
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_MOVE_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlwxFrame::OnMove: %d\n"),event.GetId());
#endif
  GDLWidget* widget = GDLWidget::GetWidget( event.GetId());
  if( widget == NULL) {event.Skip(); return;} 
  if (!gdlOwner) {event.Skip(); return;}
  GDLWidget* owner=static_cast<GDLWidget*>(gdlOwner);
  DULong flags=0;
  if( owner ) flags=owner->GetEventFlags();

  if ( flags & GDLWidget::EV_MOVE ) {
    WidgetIDT baseWidgetID = GDLWidget::GetIdOfTopLevelBase( event.GetId( ) );
    DStructGDL* widgtlb_move_events = new DStructGDL( "WIDGET_TLB_MOVE" );
    widgtlb_move_events->InitTag( "ID", DLongGDL( event.GetId( ) ) );
    widgtlb_move_events->InitTag( "TOP", DLongGDL( baseWidgetID ) );
    widgtlb_move_events->InitTag( "HANDLER", DLongGDL( baseWidgetID ) );
    widgtlb_move_events->InitTag( "X", DLongGDL( event.GetPosition().x ) );
    widgtlb_move_events->InitTag( "Y", DLongGDL( event.GetPosition().y ) );
    GDLWidget::PushEvent( baseWidgetID, widgtlb_move_events );
  } //normal end of event processing!
}

void gdlwxFrame::OnCloseFrame( wxCloseEvent & event)
{
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_VISIBILITY_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlwxFrame::OnCloseFrame: %d\n"),event.GetId());
#endif
  GDLWidget* widget = GDLWidget::GetWidget( event.GetId());
  if( widget == NULL) {event.Skip(); return;} 
  if (!gdlOwner) {event.Skip(); return;}
  GDLWidget* owner=static_cast<GDLWidget*>(gdlOwner);
    WidgetIDT baseWidgetID = GDLWidget::GetIdOfTopLevelBase( event.GetId( ) );
    DStructGDL* widgtlb_kill_request_events = new DStructGDL( "WIDGET_KILL_REQUEST" );
    widgtlb_kill_request_events->InitTag( "ID", DLongGDL( event.GetId( ) ) );
    widgtlb_kill_request_events->InitTag( "TOP", DLongGDL( baseWidgetID ) );
    widgtlb_kill_request_events->InitTag( "HANDLER", DLongGDL( baseWidgetID ) );
    GDLWidget::PushEvent( baseWidgetID, widgtlb_kill_request_events );
}

void gdlwxFrame::OnUnhandledCloseFrame( wxCloseEvent & event)
{
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_VISIBILITY_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlwxFrame::OnUnhandledCloseFrame: %d\n"),event.GetId());
#endif
  GDLWidget* widget = GDLWidget::GetWidget( event.GetId());
  if( widget == NULL) {event.Skip(); return;} 
  if (!gdlOwner) {event.Skip(); return;}
    //destroy TLB widget
   delete gdlOwner;
}

void gdlwxPlotFrame::OnUnhandledClosePlotFrame(wxCloseEvent & event) {
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_VISIBILITY_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlwxPlotFrame::OnUnhandledClosePlotFrame: %d\n"), event.GetId());
#endif
  // just destroy the window, things should follow their course.
  gdlwxPlotPanel* w = static_cast<gdlwxPlotPanel*> (this->GetChildren().GetFirst()->GetData());
  if (w) {
    GraphicsDevice::GetDevice()->WDelete(w->pstreamIx);
  }
}

void gdlwxDrawPanel::OnErase(wxEraseEvent& event)
{
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_VISIBILITY_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlwxDrawPanel::OnErase: %d\n"),event.GetId());
#endif
  event.Skip();
}
//Draw

void gdlwxGraphicsPanel::OnPaint(wxPaintEvent& event)
{
  if (drawSize.x<1||drawSize.y<1) return;
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_PAINT_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlwxGraphicsPanel::OnPaint: %d (%d,%d)\n"),event.GetId(),drawSize.x, drawSize.y);
#endif
  this->RepaintGraphics();
  event.Skip();
}

void gdlwxPlotPanel::OnPlotWindowSize(wxSizeEvent &event) {

  wxSize newSize = event.GetSize(); //size returned by the external frame
  gdlwxPlotFrame* p = this->GetMyFrame();
  p->SetSize(newSize);
  newSize = p->GetClientSize();
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_SIZE_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlwxPlotPanel::OnPlotWindowSize: %d (%d,%d), (%d,%d)\n"), event.GetId(), event.GetSize().x, event.GetSize().y,
    newSize.x, newSize.y);
#endif
  if (p->IsScrolled()) {
    bool enlarge = false;
    wxSize oldVirtualSize = this->GetVirtualSize();
    std::cerr << oldVirtualSize.y << std::endl;
    if (oldVirtualSize.x < newSize.x) {
      enlarge = true;
      oldVirtualSize.x = newSize.x;
    }
    if (oldVirtualSize.y < newSize.y) {
      enlarge = true;
      oldVirtualSize.y = newSize.y;
    }
    if (enlarge) this->ResizeDrawArea(oldVirtualSize);
    this->SetMinSize(newSize);
    this->SetSize(newSize);
  } else {
    this->ResizeDrawArea(newSize);
    this->SetMinClientSize(newSize);
    this->SetClientSize(newSize);
  }
}
void gdlwxGraphicsPanel::OnPlotWindowSize(wxSizeEvent &event)
{
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_SIZE_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlwxGraphicsPanel::OnPlotWindowSize: %d (%d,%d)\n"), event.GetId(), event.GetSize().x, event.GetSize().y);
#endif
  wxSize newSize = event.GetSize();
  if (newSize.x < 1 || newSize.y < 1) {
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_SIZE_EVENTS)
    wxMessageOutputStderr().Printf(_T("in gdlwxGraphicsPanel::OnPlotWindowSize: negative size rejected !\n"));
#endif
    return;
  }
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_SIZE_EVENTS)
    wxMessageOutputStderr().Printf(_T("in gdlwxGraphicsPanel::OnPlotWindowSize: (%d,%d)\n"),newSize.x,newSize.y );
#endif
  this->ResizeDrawArea(newSize);
}

void gdlwxDrawPanel::OnMouseMove( wxMouseEvent &event ) {
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_MOVE_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlwxDrawPanel::OnMouseMove: %d\n"),event.GetId());
#endif
  DULong eventFlags=myWidgetDraw->GetEventFlags();

  if ( eventFlags & GDLWidget::EV_MOTION ) {
    WidgetIDT baseWidgetID = GDLWidget::GetIdOfTopLevelBase( event.GetId( ) );
    DStructGDL* widgdraw = new DStructGDL( "WIDGET_DRAW" );
    widgdraw->InitTag( "ID", DLongGDL( myWidgetDraw->GetWidgetID() ) );
    widgdraw->InitTag( "TOP", DLongGDL( baseWidgetID ) );
    widgdraw->InitTag( "HANDLER", DLongGDL( baseWidgetID ) );
    widgdraw->InitTag( "TYPE", DIntGDL( 2 ) ); //motion
    widgdraw->InitTag( "X", DLongGDL( event.GetX() ) );
    widgdraw->InitTag( "Y", DLongGDL( drawSize.y-event.GetY()  ) );
    GDLWidget::PushEvent( baseWidgetID, widgdraw );
  } //normal end of event processing!
}

void gdlwxDrawPanel::OnMouseDown( wxMouseEvent &event ) {
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_KBRD_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlwxDrawPanel::OnMouseDown: %d\n"),event.GetId());
#endif
  DULong eventFlags=myWidgetDraw->GetEventFlags();

  if ( eventFlags & GDLWidget::EV_BUTTON ) {
    WidgetIDT baseWidgetID = GDLWidget::GetIdOfTopLevelBase( event.GetId( ) );
    DStructGDL* widgdraw = new DStructGDL( "WIDGET_DRAW" );
    widgdraw->InitTag( "ID", DLongGDL( myWidgetDraw->GetWidgetID() ) );
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

void gdlwxDrawPanel::OnMouseUp( wxMouseEvent &event ) {
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_KBRD_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlwxDrawPanel::OnMouseUp: %d\n"),event.GetId());
#endif
  DULong eventFlags=myWidgetDraw->GetEventFlags();
  
  if ( eventFlags & GDLWidget::EV_BUTTON ) {
    WidgetIDT baseWidgetID = GDLWidget::GetIdOfTopLevelBase( event.GetId( ) );
    DStructGDL* widgdraw = new DStructGDL( "WIDGET_DRAW" );
    widgdraw->InitTag( "ID", DLongGDL( myWidgetDraw->GetWidgetID() ) );
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

void gdlwxDrawPanel::OnMouseWheel( wxMouseEvent &event ) {
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_KBRD_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlwxDrawPanel::OnMouseWheel: %d\n"),event.GetId());
#endif
  DULong eventFlags=myWidgetDraw->GetEventFlags();

  if ( eventFlags & GDLWidget::EV_WHEEL ) {
    WidgetIDT baseWidgetID = GDLWidget::GetIdOfTopLevelBase( event.GetId( ) );
    DStructGDL* widgdraw = new DStructGDL( "WIDGET_DRAW" );
    widgdraw->InitTag( "ID", DLongGDL( myWidgetDraw->GetWidgetID()  ) );
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

void gdlwxDrawPanel::OnKey( wxKeyEvent &event ) {
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_KBRD_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlwxDrawPanel::OnKey: %d\n"),event.GetId());
#endif
  DULong eventFlags=myWidgetDraw->GetEventFlags();

  int mode=1;
  if ( eventFlags & GDLWidget::EV_KEYBOARD || eventFlags & GDLWidget::EV_KEYBOARD2 ) {
    if (eventFlags & GDLWidget::EV_KEYBOARD2) mode=2;
  
    WidgetIDT baseWidgetID = GDLWidget::GetIdOfTopLevelBase( event.GetId( ) );
    DStructGDL* widgdraw = new DStructGDL( "WIDGET_DRAW" );
    widgdraw->InitTag( "ID", DLongGDL( myWidgetDraw->GetWidgetID() ) );
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

void wxGridGDL::OnTableRowResizing(wxGridSizeEvent & event){
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_OTHER_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlGrid::OnTableRowResizing: %d\n"),event.GetId());
#endif
  GDLWidgetTable* table = static_cast<GDLWidgetTable*>(GDLWidget::GetWidget(GDLWidgetTableID));
  DULong eventFlags=table->GetEventFlags();
  if (eventFlags & GDLWidget::EV_ALL) {
    WidgetIDT baseWidgetID = GDLWidget::GetIdOfTopLevelBase( event.GetId( ) );
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

void wxGridGDL::OnTableColResizing(wxGridSizeEvent & event){
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_OTHER_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlGrid::OnTableColResizing: %d\n"),event.GetId());
#endif
  GDLWidgetTable* table = static_cast<GDLWidgetTable*>(GDLWidget::GetWidget(GDLWidgetTableID));
  DULong eventFlags=table->GetEventFlags();
  if (eventFlags & GDLWidget::EV_ALL) {
    WidgetIDT baseWidgetID = GDLWidget::GetIdOfTopLevelBase( event.GetId( ) );
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
void  wxGridGDL::OnTableRangeSelection(wxGridRangeSelectEvent & event){
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_OTHER_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlGrid::OnTableRangeSelection: %d\n"),event.GetId());
#endif
  //this event is called when a selection is added or changed (control-click, etc).
  //If we are not in disjoint mode, clear previous selection to mimick idl's when the user control-clicked.
  GDLWidgetTable* table = static_cast<GDLWidgetTable*>(GDLWidget::GetWidget(GDLWidgetTableID));

  DULong eventFlags=table->GetEventFlags();
  if (eventFlags & GDLWidget::EV_ALL  && !table->IsUpdating() ) {
    if (event.Selecting()) 
    {
      WidgetIDT baseWidgetID = GDLWidget::GetIdOfTopLevelBase( event.GetId( ) );
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
      int ncols = dynamic_cast<wxGrid*>(event.GetEventObject())->GetNumberCols();
      int nrows = dynamic_cast<wxGrid*>(event.GetEventObject())->GetNumberRows();
      if (event.GetLeftCol()==0 && event.GetRightCol()==ncols-1 && event.GetTopRow()==0 && event.GetBottomRow()==nrows-1 ) {
        WidgetIDT baseWidgetID = GDLWidget::GetIdOfTopLevelBase( event.GetId( ) );
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
        WidgetIDT baseWidgetID = GDLWidget::GetIdOfTopLevelBase( event.GetId( ) );
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

 void  wxGridGDL::OnTableCellSelection(wxGridEvent & event){
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_OTHER_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlGrid::OnTableCellSelection: %d\n"),event.GetId());
#endif
//This event is called only when the user left-clicks somewhere, thus deleting all previous selection.
  GDLWidgetTable* table = static_cast<GDLWidgetTable*>(GDLWidget::GetWidget(GDLWidgetTableID));
  if (!table->GetDisjointSelection()  && event.ControlDown() ) {
    table->ClearSelection();
  }
//For compatibility with idl, we should force to select the current table entry.
  this->SelectBlock(event.GetRow(),event.GetCol(),event.GetRow(),event.GetCol(),FALSE);
  event.Skip();
}


//Forget this function for the time being!

//  void  gdlGrid::OnTableCellSelection(wxGridEvent & event){
//#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_ALL_EVENTS)
//  wxMessageOutputStderr().Printf(_T("in gdlGrid::OnTableCellSelection: %d\n"),event.GetId());
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
  
//void gdlGrid::OnTextEnter( wxCommandEvent& event)
//{
//#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_ALL_EVENTS)
//  wxMessageOutputStderr().Printf(_T("in gdlGrid::OnTextEnter: %d\n"),event.GetId());
//#endif
//}  
//void gdlGrid::OnText( wxCommandEvent& event)
//{
//#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_ALL_EVENTS)
//  wxMessageOutputStderr().Printf(_T("in gdlGrid::OnText: %d\n"),event.GetId());
//#endif
//  GDLWidgetTable* table = static_cast<GDLWidgetTable*>(GDLWidget::GetWidget(GDLWidgetTableID));
//  DULong eventFlags=table->GetEventFlags();
//
//  WidgetIDT baseWidgetID = GDLWidget::GetTopLevelBase(GDLWidgetTableID);
//
//  bool isModified;
//  long selStart, selEnd;
//  DLong offset;
//  std::string lastValue;
//  std::string newValue;
//  
//  wxTextCtrl* textCtrl = dynamic_cast<wxTextCtrl*>(event.GetEventObject());
//  if( textCtrl == NULL)
//  {
//    event.Skip();
//    std::cerr<<"No wxWidget!"<<std::endl; return; // happens on construction 
//  }
//  textCtrl->GetSelection( &selStart, &selEnd);
//  offset = textCtrl->GetInsertionPoint();
//  lastValue = textCtrl->GetLabelText().mb_str(wxConvUTF8);
//  newValue = textCtrl->GetValue().mb_str(wxConvUTF8);
//  isModified = lastValue != newValue;
////  textCtrl->SetValue(wxString(newValue, wxConvUTF8));
////return without producing event struct if eventType is not set
//  if (!(eventFlags & GDLWidget::EV_ALL )) return;
//
//  DStructGDL*  widg;
//  if( !isModified)
//  {
//    widg = new DStructGDL( "WIDGET_TABLE_TEXT_SEL");
//    widg->InitTag("ID", DLongGDL( GDLWidgetTableID));
//    widg->InitTag("TOP", DLongGDL( baseWidgetID));
//    widg->InitTag("HANDLER", DLongGDL( baseWidgetID ));
//    widg->InitTag("TYPE", DIntGDL( 3)); // selection
//    widg->InitTag("OFFSET", DLongGDL( offset));
//    widg->InitTag("LENGTH", DLongGDL( selEnd-selStart));
//    widg->InitTag("X", DLongGDL( 0 ));
//    widg->InitTag("Y", DLongGDL( 0 ));
//  }
//  else
//  {
//    int lengthDiff = newValue.length() - lastValue.length();
//    if( lengthDiff < 0) // deleted
//    {
//      widg = new DStructGDL( "WIDGET_TABLE_DEL");
//      widg->InitTag("ID", DLongGDL(  GDLWidgetTableID));
//      widg->InitTag("TOP", DLongGDL( baseWidgetID));
//      widg->InitTag("HANDLER", DLongGDL( baseWidgetID ));
//      widg->InitTag("TYPE", DIntGDL( 2)); // delete
//      widg->InitTag("OFFSET", DLongGDL( offset-1));
//      widg->InitTag("LENGTH", DLongGDL( -lengthDiff));
//      widg->InitTag("X", DLongGDL( 0 ));
//      widg->InitTag("Y", DLongGDL( 0 ));
//    }
//    else if( lengthDiff == 0) // replace TODO: just flag the real change
//    {   
//      // 1st delete all
//      widg = new DStructGDL( "WIDGET_TABLE_DEL");
//      widg->InitTag("ID", DLongGDL(  GDLWidgetTableID));
//      widg->InitTag("TOP", DLongGDL( baseWidgetID));
//      widg->InitTag("HANDLER", DLongGDL( baseWidgetID ));
//      widg->InitTag("TYPE", DIntGDL( 2)); // delete
//      widg->InitTag("OFFSET", DLongGDL( 0));
//      widg->InitTag("LENGTH", DLongGDL( lastValue.length()));
//      widg->InitTag("X", DLongGDL( 0 ));
//      widg->InitTag("Y", DLongGDL( 0 ));
//    
//      GDLWidget::PushEvent( baseWidgetID, widg);
//
//      // 2nd insert new
//      widg = new DStructGDL( "WIDGET_TABLE_STR");
//      widg->InitTag("ID", DLongGDL(  GDLWidgetTableID));
//      widg->InitTag("TOP", DLongGDL( baseWidgetID));
//      widg->InitTag("HANDLER", DLongGDL( baseWidgetID ));
//      widg->InitTag("TYPE", DIntGDL( 1)); // multiple char
//      widg->InitTag("OFFSET", DLongGDL( 0));
//      widg->InitTag("STR", DStringGDL( newValue));
//      widg->InitTag("X", DLongGDL( 0 ));
//      widg->InitTag("Y", DLongGDL( 0 ));
//    }
//    else if( lengthDiff == 1)
//    {
//      widg = new DStructGDL( "WIDGET_TABLE_CH");
//      widg->InitTag("ID", DLongGDL(  GDLWidgetTableID));
//      widg->InitTag("TOP", DLongGDL( baseWidgetID));
//      widg->InitTag("HANDLER", DLongGDL( baseWidgetID ));
//      widg->InitTag("TYPE", DIntGDL( 0)); // single char
//      widg->InitTag("OFFSET", DLongGDL( offset+1));
//      widg->InitTag("CH", DByteGDL( newValue[offset<newValue.length()?offset:newValue.length()-1]));
//      widg->InitTag("X", DLongGDL( 0 ));
//      widg->InitTag("Y", DLongGDL( 0 ));
//    }
//    else // lengthDiff > 1
//    {
//      int nVLenght = newValue.length();
//      if( offset < lengthDiff)  lengthDiff = offset;
//      std::string str = "";
//      if( offset <= nVLenght && lengthDiff > 0)	 str = newValue.substr(offset-lengthDiff,lengthDiff+1);
//      
//      widg = new DStructGDL( "WIDGET_TABLE_STR");
//      widg->InitTag("ID", DLongGDL( GDLWidgetTableID));
//      widg->InitTag("TOP", DLongGDL( baseWidgetID));
//      widg->InitTag("HANDLER", DLongGDL( baseWidgetID ));
//      widg->InitTag("TYPE", DIntGDL( 1)); // multiple char
//      widg->InitTag("OFFSET", DLongGDL( offset));
//      widg->InitTag("STR", DStringGDL( str));
//      widg->InitTag("X", DLongGDL( 0 ));
//      widg->InitTag("Y", DLongGDL( 0 ));
//    }
//  }
//  GDLWidget::PushEvent( baseWidgetID, widg);
//}
//


void wxTreeCtrlGDL::OnItemActivated(wxTreeEvent & event){
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_OTHER_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlTreeCtrl::OnItemActivated: %d\n"),event.GetId());
#endif
  WidgetIDT baseWidgetID = GDLWidget::GetIdOfTopLevelBase( event.GetId( ) );
//get GDLWidgetTree ID which was passed as wxTreeItemData at creation to identify
//the GDL widget that received the event
    wxTreeCtrlGDL* me=dynamic_cast<wxTreeCtrlGDL*>(event.GetEventObject());
    
    DStructGDL* treeselect = new DStructGDL( "WIDGET_TREE_SEL");
    treeselect->InitTag("ID", DLongGDL( dynamic_cast<wxTreeItemDataGDL*>(me->GetItemData(event.GetItem()))->widgetID ));
    treeselect->InitTag("TOP", DLongGDL( baseWidgetID));
    treeselect->InitTag("HANDLER", DLongGDL( GDLWidgetTableID ));
    treeselect->InitTag("TYPE", DIntGDL(0)); // 0
    treeselect->InitTag("CLICKS",DLongGDL(2));
    // insert into structList
    GDLWidget::PushEvent( baseWidgetID, treeselect );
}


void wxTreeCtrlGDL::OnItemSelected(wxTreeEvent & event){
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_OTHER_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlTreeCtrl::OnItemActivated: %d\n"),event.GetId());
#endif
  WidgetIDT baseWidgetID = GDLWidget::GetIdOfTopLevelBase( event.GetId( ) );
//get GDLWidgetTree ID which was passed as wxTreeItemData at creation to identify
//the GDL widget that received the event
    wxTreeCtrlGDL* me=dynamic_cast<wxTreeCtrlGDL*>(event.GetEventObject());
    WidgetIDT selected=dynamic_cast<wxTreeItemDataGDL*>(me->GetItemData(event.GetItem()))->widgetID;
    GDLWidgetTree* tree= static_cast<GDLWidgetTree*>(GDLWidget::GetWidget(dynamic_cast<wxTreeItemDataGDL*>(me->GetItemData(event.GetItem()))->widgetID));
    //inform root widget it is selected
    GDLWidgetTree* root=static_cast<GDLWidgetTree*>(GDLWidget::GetWidget(tree->GetRootID()));
    root->SetSelectedID(selected);
    
    DStructGDL* treeselect = new DStructGDL( "WIDGET_TREE_SEL");
    treeselect->InitTag("ID", DLongGDL( selected ));
    treeselect->InitTag("TOP", DLongGDL( baseWidgetID));
    treeselect->InitTag("HANDLER", DLongGDL( GDLWidgetTableID ));
    treeselect->InitTag("TYPE", DIntGDL(0)); // 0
    treeselect->InitTag("CLICKS",DLongGDL(1));
    // insert into structList
    GDLWidget::PushEvent( baseWidgetID, treeselect );
}

void wxTreeCtrlGDL::OnBeginDrag(wxTreeEvent & event){
  //largely useful protection!!!
  if (!event.GetItem().IsOk()) return;

#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_OTHER_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlTreeCtrl::OnBeginDrag: %d\n"),event.GetId());
#endif

//needed to explicitly authorize dragging.
//get GDLWidgetTree ID which was passed as wxTreeItemData at creation to identify
//the GDL widget that received the event
    wxTreeCtrlGDL* me=dynamic_cast<wxTreeCtrlGDL*>(event.GetEventObject());
    GDLWidgetTree* item = static_cast<GDLWidgetTree*>(GDLWidget::GetWidget(dynamic_cast<wxTreeItemDataGDL*>(me->GetItemData(event.GetItem()))->widgetID));
    if (item->IsDraggable()) event.Allow();
}

void wxTreeCtrlGDL::OnItemDropped(wxTreeEvent & event){
  if (!event.GetItem().IsOk()) return;

#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_OTHER_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlTreeCtrl::OnItemDropped: %d\n"),event.GetId());
#endif
    WidgetIDT baseWidgetID = GDLWidget::GetIdOfTopLevelBase( event.GetId( ) );
    wxTreeCtrlGDL* me=dynamic_cast<wxTreeCtrlGDL*>(event.GetEventObject());
    GDLWidgetTree* item = static_cast<GDLWidgetTree*>(GDLWidget::GetWidget(dynamic_cast<wxTreeItemDataGDL*>(me->GetItemData(event.GetItem()))->widgetID));

    if (item->IsDroppable()) {
 //get GDLWidgetTree ID which was passed as wxTreeItemData at creation to identify
//the GDL widget that received the event
      DStructGDL* treedrop = new DStructGDL( "WIDGET_DROP");
      treedrop->InitTag("ID", DLongGDL( item->GetWidgetID()  )); //ID of the destination
      treedrop->InitTag("TOP", DLongGDL( baseWidgetID));
      treedrop->InitTag("HANDLER", DLongGDL( GDLWidgetTableID ));
      treedrop->InitTag("DRAG_ID", DLongGDL( item->GetRootID())); // ID of the source TREE
      treedrop->InitTag("POSITION",DIntGDL(2)); //ALWAYS 2 IT SEEMS THAT wxW cannot do better? !   1 above 2 on 3 below destination widget
      treedrop->InitTag("X",DLongGDL(event.GetPoint().x)); //x and Y coord of position wrt lower left corner of destination tree widget
      treedrop->InitTag("Y",DLongGDL(event.GetPoint().y));
      treedrop->InitTag("MODIFIERS",DIntGDL(0)); //mask with 1 shift 2 control 4 caps lock 8 alt
      // insert into structList
      GDLWidget::PushEvent( baseWidgetID, treedrop );
  }
}
void wxTreeCtrlGDL::OnItemExpanded(wxTreeEvent & event){
  if (!event.GetItem().IsOk()) return;
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_OTHER_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlTreeCtrl::OnItemExpanded: %d\n"),event.GetId());
#endif
    WidgetIDT baseWidgetID = GDLWidget::GetIdOfTopLevelBase( event.GetId( ) );
//get GDLWidgetTree ID which was passed as wxTreeItemData at creation to identify
//the GDL widget that received the event
    wxTreeCtrlGDL* me=dynamic_cast<wxTreeCtrlGDL*>(event.GetEventObject());
    
    DStructGDL* treeexpand = new DStructGDL( "WIDGET_TREE_EXPAND");
    treeexpand->InitTag("ID", DLongGDL( dynamic_cast<wxTreeItemDataGDL*>(me->GetItemData(event.GetItem()))->widgetID ));
    treeexpand->InitTag("TOP", DLongGDL( baseWidgetID));
    treeexpand->InitTag("HANDLER", DLongGDL( GDLWidgetTableID ));
    treeexpand->InitTag("TYPE", DIntGDL(1)); // 1
    treeexpand->InitTag("EXPAND",DLongGDL(1)); //1 expand
    // insert into structList
    GDLWidget::PushEvent( baseWidgetID, treeexpand );
}
void wxTreeCtrlGDL::OnItemCollapsed(wxTreeEvent & event){
  if (!event.GetItem().IsOk()) return;
#if (GDL_DEBUG_ALL_EVENTS || GDL_DEBUG_OTHER_EVENTS)
  wxMessageOutputStderr().Printf(_T("in gdlTreeCtrl::OnItemCollapsed: %d\n"),event.GetId());
#endif
    WidgetIDT baseWidgetID = GDLWidget::GetIdOfTopLevelBase( event.GetId( ) );
//get GDLWidgetTree ID which was passed as wxTreeItemData at creation to identify
//the GDL widget that received the event
    wxTreeCtrlGDL* me=dynamic_cast<wxTreeCtrlGDL*>(event.GetEventObject());
    
    DStructGDL* treeexpand = new DStructGDL( "WIDGET_TREE_EXPAND");
    treeexpand->InitTag("ID", DLongGDL( dynamic_cast<wxTreeItemDataGDL*>(me->GetItemData(event.GetItem()))->widgetID ));
    treeexpand->InitTag("TOP", DLongGDL( baseWidgetID));
    treeexpand->InitTag("HANDLER", DLongGDL( GDLWidgetTableID ));
    treeexpand->InitTag("TYPE", DIntGDL(1)); // 1
    treeexpand->InitTag("EXPAND",DLongGDL(0)); //0 collapse
    // insert into structList
    GDLWidget::PushEvent( baseWidgetID, treeexpand );
}
  
#endif
