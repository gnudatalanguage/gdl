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

#ifdef HAVE_LIBWXWIDGETS

#include "gdlwidget.hpp"

#include <memory> 
#include <wx/grid.h>
#include <wx/gbsizer.h>
#include <wx/wrapsizer.h>
#include <wx/textctrl.h>
#include <wx/menuitem.h>
#include <wx/display.h>

#include "gdlwxstream.hpp"
#include "graphicsdevice.hpp"

#undef MIN
#define MIN(a,b) ((a) > (b) ? (b) : (a))
#undef MAX
#define MAX(a,b) ((a) < (b) ? (b) : (a))
#undef ABS
#define ABS(a) (((a) < 0) ? -(a) : (a))

//must arrive after "gdlwidget.hpp"
#ifdef GDL_DEBUG_WIDGETS_COLORIZE
#include <random>
#include <chrono>
#endif

//abbrevs to avoid sending size events when changing sizes
#define START_CHANGESIZE_NOEVENT \
WidgetIDT id;\
gdlwxFrame* local_topFrame;\
bool reconnect=this->DisableSizeEvents(local_topFrame,id);

#define END_CHANGESIZE_NOEVENT if (reconnect) EnableSizeEvents(local_topFrame,id);

#define START_ADD_EVENTUAL_FRAME \
wxScrolled<wxPanel>* saveWidgetPanel = widgetPanel;\
wxSizer* frameSizer;\
if (frameWidth > 0) {\
  framePanel = new wxPanel(widgetPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, gdlBORDER_EXT);\
  frameSizer = new wxBoxSizer(wxVERTICAL);\
  framePanel->SetSizer(frameSizer);\
  widgetPanel = static_cast<wxScrolled<wxPanel>*>(framePanel);\
}

#define END_ADD_EVENTUAL_FRAME \
  if (frameWidth > 0) {\
    frameSizer->Add(static_cast<wxWindow*> (theWxWidget), DONOTALLOWSTRETCH, wxALL|wxEXPAND , frameWidth);\
    frameSizer->Fit(static_cast<wxWindow*> (theWxWidget));\
    framePanel->Fit();\
    theWxContainer = framePanel;\
    widgetPanel = saveWidgetPanel;\
    widgetSizer=saveWidgetSizer;\
    }

#define TIDY_WIDGET(xxx) \
  this->setFont();\
  if (widgetSizer) {\
    /*if we create a widget after the topwidget has been realized, insure the insertion is IDL-compliant for multicolumns, using the following; */\
    /* 1) recompute the base sizer taking into account the additional widget to be inserted */\
    /* 2) recreate the adequate sizer */\
    /* 3) re-add all child windows (including this one) */\
    if (this->IsRealized()) {\
      GDLWidgetBase* b = static_cast<GDLWidgetBase*> (gdlParent);\
      b->ReorderForANewWidget(static_cast<wxWindow*> (theWxContainer), DONOTALLOWSTRETCH, widgetAlignment() | wxALL, b->getSpace());\
    } else widgetSizer->Add(static_cast<wxWindow*> (theWxContainer), DONOTALLOWSTRETCH, widgetAlignment()  | wxALL, xxx);\
  } else {\
    static_cast<wxWindow*> (theWxContainer)->SetPosition(wOffset);\
  }\
  widgetPanel->FitInside();

#define REALIZE_IF_NEEDED { if (this->IsRealized()) {this->OnRealize(); UpdateGui();} }

//a few useful defaut pixmaps:
static const char * pixmap_unchecked[] = {
"13 13 14 1",
" 	c None",
".	c #CCCED3",
"+	c #838793",
"@	c #C4C7CF",
"#	c #F5F5F5",
"$	c #F6F6F6",
"%	c #F7F7F7",
"&	c #F9F9F9",
"*	c #FAFAFA",
"=	c #FBFBFB",
"-	c #FCFCFC",
";	c #FDFDFD",
">	c #FEFEFE",
",	c #FFFFFF",
".+++++++++++.",
"+@@@@@@@@@@@+",
"+@##########+",
"+@$$$$$$$$$$+",
"+@%%%%%%%%%%+",
"+@&&&&&&&&&&+",
"+@**********+",
"+@==========+",
"+@----------+",
"+@;;;;;;;;;;+",
"+@>>>>>>>>>>+",
"+@,,,,,,,,,,+",
".+++++++++++."};

static const char * pixmap_checked[] = {
"13 13 38 1",
" 	c None",
".	c #CCCED3",
"+	c #838793",
"@	c #C4C7CF",
"#	c #F5F5F5",
"$	c #BBBBBB",
"%	c #131313",
"&	c #0C0C0C",
"*	c #F6F6F6",
"=	c #BEBEBE",
"-	c #030303",
";	c #060606",
">	c #B4B4B4",
",	c #F7F7F7",
"'	c #C6C6C6",
")	c #080808",
"!	c #000000",
"~	c #C0C0C0",
"{	c #020202",
"]	c #363636",
"^	c #F9F9F9",
"/	c #505050",
"(	c #2C2C2C",
"_	c #545454",
":	c #606060",
"<	c #010101",
"[	c #FAFAFA",
"}	c #FBFBFB",
"|	c #333333",
"1	c #1D1D1D",
"2	c #FCFCFC",
"3	c #C4C4C4",
"4	c #FDFDFD",
"5	c #B9B9B9",
"6	c #FEFEFE",
"7	c #5E5E5E",
"8	c #777777",
"9	c #FFFFFF",
".+++++++++++.",
"+@@@@@@@@@@@+",
"+@######$%&#+",
"+@*****=-;**+",
"+@$>,,')!~,,+",
"+@{!]^/!(^^^+",
"+@_!!:<![[[[+",
"+@}|!!!1}}}}+",
"+@22)!!32222+",
"+@445!)44444+",
"+@6667866666+",
"+@9999999999+",
".+++++++++++."};

#include "../resource/gdlicon.xpm"
wxIcon wxgdlicon;


class GDLWidgetTable;

//class wxGridGDLCellStringRenderer : public wxGridCellStringRenderer {
//public:
//
//  wxGridGDLCellStringRenderer() : wxGridCellStringRenderer() { }
//
//  virtual void Draw(wxGrid &grid, wxGridCellAttr &attr, wxDC &dc,
//	const wxRect &rect, int row, int col, bool isSelected) {
////	dc.SetBackground(attr.GetBackgroundColour());
////	dc.Clear();
//    // Get text
////	GDLWidgetTable* table = static_cast<GDLWidgetTable*>(GDLWidget::GetWidget(grid.GetWidgetTableID()));
////    std::cerr<<table->GetVvalue()->N_Elements()<<std::endl;
//	
//	// Draw the text.
//
//	wxGridCellStringRenderer::Draw(grid, attr, dc, rect,
//	  row, col, isSelected);
//  }
//};

class wxGridGDLCellTextEditor : public wxGridCellEditor {
GDLWidgetTable* table;
public:
  explicit wxGridGDLCellTextEditor(size_t maxChars = 0);

  virtual void Create(wxWindow* parent,
	wxWindowID id,
	wxEvtHandler* evtHandler);
//  virtual void SetSize(const wxRect& rect);

  virtual void PaintBackground(wxDC& dc,
	const wxRect& rectCell,
	const wxGridCellAttr& attr);

//  virtual bool IsAcceptedKey(wxKeyEvent& event);
  virtual void BeginEdit(int row, int col, wxGrid* grid);
  virtual bool EndEdit(int row, int col, const wxGrid* grid,
	const wxString& oldval, wxString *newval);
  virtual void ApplyEdit(int row, int col, wxGrid* grid);

  virtual void Reset();
//  virtual void StartingKey(wxKeyEvent& event);
//  virtual void HandleReturn(wxKeyEvent& event);
  wxString GetEditedValue(int row, int col, wxGrid* grid);
  wxString SetEditedValue(wxString s, int row, int col);
  virtual void SetParameters(const wxString& params);
#if wxUSE_VALIDATORS
  virtual void SetValidator(const wxValidator& validator);
#endif

  virtual wxGridCellEditor *Clone() const;

  // added GetValue so we can get the value which is in the control
  virtual wxString GetValue() const;

protected:
  // parts of our virtual functions reused by the derived classes
  void DoCreate(wxWindow* parent, wxWindowID id, wxEvtHandler* evtHandler,
	long style = 0);
  void DoBeginEdit(const wxString& startValue);
  void DoReset(const wxString& startValue);

  wxTextCtrl* Text() {
	return m_text;
  }
private:
  size_t m_maxChars; // max number of chars allowed
#if wxUSE_VALIDATORS
  wxScopedPtr<wxValidator> m_validator;
#endif
  wxString m_value;

  wxDECLARE_NO_COPY_CLASS(wxGridGDLCellTextEditor);

  wxEvtHandler* m_handler;
  wxTextCtrl* m_text;
};

wxGridGDLCellTextEditor::wxGridGDLCellTextEditor(size_t maxChars) {
  m_maxChars = maxChars;
}

void wxGridGDLCellTextEditor::Create(wxWindow* parent,
  wxWindowID id,
  wxEvtHandler* evtHandler) {
  DoCreate(parent, id, evtHandler);
}

void wxGridGDLCellTextEditor::DoCreate(wxWindow* parent,
  wxWindowID id,
  wxEvtHandler* evtHandler,
  long style) {
  wxControl* control = new wxControl(parent, id, wxDefaultPosition,
	wxDefaultSize, wxNO_BORDER);
  m_handler = evtHandler;

  style |= wxTE_PROCESS_ENTER | wxTE_PROCESS_TAB | wxNO_BORDER;
  m_text = new wxTextCtrl(control, wxID_ANY, wxEmptyString,
	wxDefaultPosition, wxDefaultSize,
	style);
  m_text->SetMargins(0, 0);

  m_control = control;

  wxBoxSizer* szr = new wxBoxSizer(wxHORIZONTAL);
  szr->Add(m_text, wxSizerFlags(1));

  control->SetSizer(szr);

//#ifdef __WXOSX__
//  wxWidgetImpl* impl = m_text->GetPeer();
//  impl->SetNeedsFocusRect(false);
//#endif
  // set max length allowed in the textctrl, if the parameter was set
  if (m_maxChars != 0) {
	m_text->SetMaxLength(m_maxChars);
  }
#if wxUSE_VALIDATORS
  // validate text in textctrl, if validator is set
  if (m_validator) {
	m_text->SetValidator(*m_validator);
  }
#endif

  wxGridCellEditor::Create(parent, id, evtHandler);
}

void wxGridGDLCellTextEditor::PaintBackground(wxDC& dc,
  const wxRect& WXUNUSED(rectCell),
  const wxGridCellAttr& WXUNUSED(attr)) {
  // as we fill the entire client area,
  // don't do anything here to minimize flicker
}

//void wxGridGDLCellTextEditor::SetSize(const wxRect& rectOrig) {
//  wxRect rect = AdjustRectForPlatform(rectOrig);
//
//  wxGridCellEditor::SetSize(rect);
//}
wxString wxGridGDLCellTextEditor::GetEditedValue(int row, int col, wxGrid* grid){
  wxGridGDL* mygrid=static_cast<wxGridGDL*>(grid);
  table = static_cast<GDLWidgetTable*>(GDLWidget::GetWidget(mygrid->GetWidgetTableID()));
  BaseGDL* value = table->GetVvalue();
  if (value==NULL) return "";
  SizeT dim0 = value->Dim(0);
  SizeT nEl = value->N_Elements();
  int offset=row * dim0 + col;
  DType editableType = value->Type();
  //special struct case: point to the correct BaseGDL
  if (editableType == GDL_STRUCT) {
	DStructGDL* s = static_cast<DStructGDL*> (value);
	dim0 = s->Dim(0);
	SizeT ntags = s->NTags();
	SizeT iTag = 0;
	if (table->GetMajority() == GDLWidgetTable::ROW_MAJOR) {
	  iTag = col;
	  offset = row;
	} else {
	  iTag = row;
	  offset = col;
	}
	if ( (iTag > s->NTags() - 1) || (offset > dim0 - 1) ) return ""; //unexistent table entry
	value = s->GetTag(iTag, offset);
	nEl = 1;
	offset = 0;
	editableType=value->Type();
  }
    switch (editableType) {
  case GDL_STRING:
	return table->GetRawEditingValue <DStringGDL, DString> (static_cast<DStringGDL*> (value), nEl, offset);
	break;
  case GDL_BYTE:
	return table->GetRawEditingValue <DByteGDL, DByte> (static_cast<DByteGDL*> (value), nEl, offset);
	break;
  case GDL_INT:
	return table->GetRawEditingValue <DIntGDL, DInt> (static_cast<DIntGDL*> (value), nEl, offset);
	break;
  case GDL_LONG:
	return table->GetRawEditingValue <DLongGDL, DLong> (static_cast<DLongGDL*> (value), nEl, offset);
	break;
  case GDL_FLOAT:
	return table->GetRawEditingValue <DFloatGDL, DFloat> (static_cast<DFloatGDL*> (value), nEl, offset);
	break;
  case GDL_DOUBLE:
	return table->GetRawEditingValue <DDoubleGDL, DDouble> (static_cast<DDoubleGDL*> (value), nEl, offset);
	break;
  case GDL_COMPLEX:
	return table->GetRawEditingValue <DComplexGDL, DComplex> (static_cast<DComplexGDL*> (value), nEl, offset);
	break;
  case GDL_COMPLEXDBL:
	return table->GetRawEditingValue <DComplexDblGDL, DComplexDbl> (static_cast<DComplexDblGDL*> (value), nEl, offset);
	break;
  case GDL_UINT:
	return table->GetRawEditingValue <DUIntGDL, DUInt> (static_cast<DUIntGDL*> (value), nEl, offset);
	break;
  case GDL_ULONG:
	return table->GetRawEditingValue <DULongGDL, DULong> (static_cast<DULongGDL*> (value), nEl, offset);
	break;
  case GDL_LONG64:
	return table->GetRawEditingValue <DLong64GDL, DLong64> (static_cast<DLong64GDL*> (value), nEl, offset);
	break;
  case GDL_ULONG64:
	return table->GetRawEditingValue <DULong64GDL, DULong64> (static_cast<DULong64GDL*> (value), nEl, offset);
	break;
  default:
	cerr << "Unhandled Table Type, please report!" << endl;
  }
  return "";
}
wxString wxGridGDLCellTextEditor::SetEditedValue(wxString sval, int row, int col){
  BaseGDL* value = table->GetVvalue();  
  if (value==NULL) return "";

  DStringGDL* format=table->GetCurrentFormat();
  SizeT dim0 = value->Dim(0);
  SizeT nEl = value->N_Elements();
  DType editableType=value->Type();
  int offset=row * dim0 + col;
  //special struct case: point to the correct BaseGDL
  if (editableType == GDL_STRUCT)
{
	DStructGDL* s = static_cast<DStructGDL*> (value);
	dim0=s->Dim(0);
	SizeT ntags=s->NTags();
	SizeT iTag=0;
	if (table->GetMajority() == GDLWidgetTable::ROW_MAJOR) {
	  iTag=col;
	  offset=row;
	} else {
	  iTag=row;
	  offset=col;
	}
	if ( (iTag > s->NTags()-1) || (offset > dim0-1) )  return ""; //unexistent table entry
	value=s->GetTag(iTag,offset);
	nEl=1;
	offset=0;
	editableType=value->Type();
  }
  switch (editableType) {
  case GDL_STRING:
	return table->SetEditedValue <DStringGDL, DString> (sval, static_cast<DStringGDL*> (value), nEl, offset, format);
	break;
  case GDL_BYTE:
	return table->SetEditedValue <DByteGDL, DByte> (sval, static_cast<DByteGDL*> (value), nEl, offset, format);
	break;
  case GDL_INT:
	return table->SetEditedValue <DIntGDL, DInt> (sval, static_cast<DIntGDL*> (value), nEl, offset, format);
	break;
  case GDL_LONG:
	return table->SetEditedValue <DLongGDL, DLong> (sval, static_cast<DLongGDL*> (value), nEl, offset, format);
	break;
  case GDL_FLOAT:
	return table->SetEditedValue <DFloatGDL, DFloat> (sval, static_cast<DFloatGDL*> (value), nEl, offset, format);
	break;
  case GDL_DOUBLE:
	return table->SetEditedValue <DDoubleGDL, DDouble> (sval, static_cast<DDoubleGDL*> (value), nEl, offset, format);
	break;
  case GDL_COMPLEX:
	return table->SetEditedValue <DComplexGDL, DComplex> (sval, static_cast<DComplexGDL*> (value), nEl, offset, format);
	break;
  case GDL_COMPLEXDBL:
	return table->SetEditedValue <DComplexDblGDL, DComplexDbl> (sval, static_cast<DComplexDblGDL*> (value), nEl, offset, format);
	break;
  case GDL_UINT:
	return table->SetEditedValue <DUIntGDL, DUInt> (sval, static_cast<DUIntGDL*> (value), nEl, offset, format);
	break;
  case GDL_ULONG:
	return table->SetEditedValue <DULongGDL, DULong> (sval, static_cast<DULongGDL*> (value), nEl, offset, format);
	break;
  case GDL_LONG64:
	return table->SetEditedValue <DLong64GDL, DLong64> (sval, static_cast<DLong64GDL*> (value), nEl, offset, format);
	break;
  case GDL_ULONG64:
	return table->SetEditedValue <DULong64GDL, DULong64> (sval, static_cast<DULong64GDL*> (value), nEl, offset, format);
	break;
  default:
	cerr << "Unhandled Table Type, please report!" << endl;
  }
  return ""; 
}

void wxGridGDLCellTextEditor::BeginEdit(int row, int col, wxGrid* grid) {
  wxASSERT_MSG(m_control, wxT("The wxGridCellEditor must be created first!"));
  wxGridGDL* mygrid=static_cast<wxGridGDL*>(grid);
  GDLWidgetTable* table = static_cast<GDLWidgetTable*>(GDLWidget::GetWidget(mygrid->GetWidgetTableID()));
  BaseGDL* value = table->GetVvalue();
  if (value==NULL) return;

  int majority=table->GetMajority();
  SizeT dim0 = value->Dim(0);
  SizeT nEl = value->N_Elements();
  m_value=GetEditedValue(row, col, grid);
  DoBeginEdit(m_value);
}

void wxGridGDLCellTextEditor::DoBeginEdit(const wxString& startValue) {
  m_text->SetValue(startValue);
  m_text->SetInsertionPointEnd();
  m_text->SelectAll();
  m_text->SetFocus();

  m_control->Layout();
}

bool wxGridGDLCellTextEditor::EndEdit(int row,
  int col,
  const wxGrid* grid,
  const wxString& WXUNUSED(oldval),
  wxString *newval) {
  wxCHECK_MSG(m_control, false,
	"wxGridCellTextAndButtonEditor must be created first!");

  const wxString value = m_text->GetValue();
  if (value == m_value)	return false; else m_value=value;
  m_value = SetEditedValue(m_value, row, col); //value;

  if (newval)
	*newval = m_value;

  return true;
}

void wxGridGDLCellTextEditor::ApplyEdit(int row, int col, wxGrid* grid) {
  grid->GetTable()->SetValue(row, col, m_value);
  m_value.clear();
}

void wxGridGDLCellTextEditor::Reset() {
  wxASSERT_MSG(m_control, "wxGridCellTextAndButtonEditor must be created first!");

  DoReset(m_value);
}

void wxGridGDLCellTextEditor::DoReset(const wxString& startValue) {
  m_text->SetValue(startValue);
  m_text->SetInsertionPointEnd();
}

//bool wxGridGDLCellTextEditor::IsAcceptedKey(wxKeyEvent& event) {
//  switch (event.GetKeyCode()) {
//  case WXK_DELETE:
//  case WXK_BACK:
//	return true;
//
//  default:
//	return wxGridCellEditor::IsAcceptedKey(event);
//  }
//}

//void wxGridGDLCellTextEditor::StartingKey(wxKeyEvent& event) {
//  // Since this is now happening in the EVT_CHAR event EmulateKeyPress is no
//  // longer an appropriate way to get the character into the text control.
//  // Do it ourselves instead.  We know that if we get this far that we have
//  // a valid character, so not a whole lot of testing needs to be done.
//
//  int ch;
//
//  bool isPrintable;
//
//#if wxUSE_UNICODE
//  ch = event.GetUnicodeKey();
//  if (ch != WXK_NONE)
//	isPrintable = true;
//  else
//#endif // wxUSE_UNICODE
//  {
//	ch = event.GetKeyCode();
//	isPrintable = ch >= WXK_SPACE && ch < WXK_START;
//  }
//
//  switch (ch) {
//  case WXK_DELETE:
//	// Delete the initial character when starting to edit with DELETE.
//	m_text->Remove(0, 1);
//	break;
//
//  case WXK_BACK:
//	// Delete the last character when starting to edit with BACKSPACE.
//  {
//	const long pos = m_text->GetLastPosition();
//	m_text->Remove(pos - 1, pos);
//  }
//	break;
//
//  default:
//	if (isPrintable)
//	  m_text->WriteText(static_cast<wxChar> (ch));
//	break;
//  }
//}

//void wxGridGDLCellTextEditor::HandleReturn(wxKeyEvent& event) {
//#if defined(__WXMOTIF__) || defined(__WXGTK__)
//  // wxMotif needs a little extra help...
//  size_t pos = (size_t) (Text()->GetInsertionPoint());
//  wxString s(Text()->GetValue());
//  s = s.Left(pos) + wxT("\n") + s.Mid(pos);
//  Text()->SetValue(s);
//  Text()->SetInsertionPoint(pos);
//#else
//  // the other ports can handle a Return key press
//  //
//  event.Skip();
//#endif
//}

void wxGridGDLCellTextEditor::SetParameters(const wxString& params) {
  if (!params) {
	// reset to default
	m_maxChars = 0;
  } else {
	long tmp;
	if (params.ToLong(&tmp)) {
	  m_maxChars = (size_t) tmp;
	} else {
	  wxLogDebug(wxT("Invalid wxGridCellTextAndButtonEditor parameter string '%s' ignored"), params.c_str());
	}
  }
}

#if wxUSE_VALIDATORS

void wxGridGDLCellTextEditor::SetValidator(const wxValidator& validator) {
  m_validator.reset(static_cast<wxValidator*> (validator.Clone()));
}
#endif

wxGridCellEditor *wxGridGDLCellTextEditor::Clone() const {
  wxGridGDLCellTextEditor* editor = new wxGridGDLCellTextEditor(m_maxChars);
#if wxUSE_VALIDATORS
  if (m_validator) {
	editor->SetValidator(*m_validator);
  }
#endif
  return editor;
}

// return the value in the text control

wxString wxGridGDLCellTextEditor::GetValue() const {
  return m_text->GetValue();
}


const WidgetIDT GDLWidget::NullID = 0;

// instantiation
WidgetListT GDLWidget::widgetList;
wxImageList *gdlDefaultTreeStateImages;
wxImageList *gdlDefaultTreeImages;

GDLEventQueue GDLWidget::BlockingEventQueue; // the event queue in which all widget events are versed in case of blocking (XMANAGER)
GDLEventQueue GDLWidget::InteractiveEventQueue; // event queue used when no blocking is made -- part of the main GDLEventHandler() 
bool GDLWidget::wxIsOn=false;
bool GDLWidget::handlersOk=false;
wxFont GDLWidget::defaultFont=wxNullFont; //the font defined by widget_control,default_font.
wxFont GDLWidget::systemFont=wxNullFont;  //the initial system font. This to behave as IDL

#ifdef __WXMAC__
        #include <Carbon/Carbon.h>
extern "C" { void CPSEnableForegroundOperation( ProcessSerialNumber* psn ); }
#endif

  //initialize wxWidgets system:  create an instance of wxAppGDL here, not at Main (
//#ifndef __WXMAC__ 
//    wxAppGDL& wxGetApp() { return *static_cast<wxAppGDL*>(wxApp::GetInstance()); }   
//    wxAppConsole *wxCreateApp() 
//    { 
//        wxAppConsole::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE,"GDL");
//        return new wxAppGDL;
//    }
//    wxAppInitializer  wxTheAppInitializer((wxAppInitializerFunction) wxCreateApp);
//#else
    wxApp& wxGetApp() { return *static_cast<wxApp*>(wxApp::GetInstance()); }   
    wxAppConsole *wxCreateApp() 
    { 
        wxAppConsole::CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE,"GDL");
        return new wxApp;
    }
    wxAppInitializer  wxTheAppInitializer((wxAppInitializerFunction) wxCreateApp);
//#endif

void GDLEventQueue::Purge()
{
  for ( SizeT i = 0; i < dq.size( ); ++i )
    delete dq[i];
  dq.clear( );
  //   isEmpty = true;
}

// removes all events for hierarchy staring at 'parentID' and below
void GDLEventQueue::Purge( WidgetIDT parentID) {
  //establish a list of children:
  GDLWidget* w = GDLWidget::GetWidget(parentID);
  DLongGDL* list = w->GetAllHeirs();
  for (long i = dq.size() - 1; i >= 0; --i) {
    DStructGDL* ev = dq[i];
    static int idIx = 0; // ev->Desc( )->TagIndex( "ID" ); //always 0
    DLong id = (*static_cast<DLongGDL*> (ev->GetTag(idIx, 0)))[0];
    //all events pertaining to any heirs of 'parentID' including 'parentID' are removed:
    for (DLong testid = 0; testid < list->N_Elements(); ++testid) {
      if (id == (*list)[testid]) {
//        std::cerr<<"event "<<id<<" purged."<<std::endl;
        delete ev;
        dq.erase(dq.begin() + i);
      }
    }
  }
  GDLDelete(list);
  //   isEmpty = true;
}
#ifdef GDL_DEBUG_WIDGETS_COLORIZE
inline wxColour RandomWxColour() {
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine generator (seed);

  std::uniform_int_distribution<int> distribution(0,255);
  int r = distribution(generator);
  int g = distribution(generator);
  int b = distribution(generator);
  return wxColour(r,g,b);
}
#endif

//this seem to respect the order of preference if variuos align_xxx are passed to the widget.
inline int GDLWidget::labelTextAlignment()
{//this concerns only how the thext is written in the label. 
  // Top and bottom are not allowed in IDL.
   if ( alignment & gdlwALIGN_RIGHT ) return (wxALIGN_RIGHT|wxST_NO_AUTORESIZE);
   if ( alignment & gdlwALIGN_CENTER ) return (wxALIGN_CENTRE_HORIZONTAL|wxST_NO_AUTORESIZE);
   if ( alignment & gdlwALIGN_LEFT ) return (wxALIGN_LEFT|wxST_NO_AUTORESIZE); 
   return wxALIGN_CENTRE_HORIZONTAL|wxST_NO_AUTORESIZE;
}

inline int GDLWidget::buttonTextAlignment()
{ //this concerns only how the thext is written in the button. Apparently wxBU_RIGHT does not work, and center is absent.
  // Top and bottom are not allowed in IDL.
      if ( alignment & gdlwALIGN_RIGHT ) return wxBU_EXACTFIT|wxBU_RIGHT; //right does not work.
      if ( alignment & gdlwALIGN_CENTER ) return wxBU_EXACTFIT; //center does not exist
      if ( alignment & gdlwALIGN_LEFT ) return wxBU_EXACTFIT|wxBU_LEFT; //default, works
      return wxBU_EXACTFIT;
}

inline int GDLWidgetLabel::widgetAlignment()
{
  long myAlign = alignment;
  if (myAlign == gdlwALIGN_NOT) {
    myAlign = this->GetMyParentBaseWidget()->getChildrenAlignment();
}
  //wxALIGN_LEFT and TOP is in fact wxALIGN_NOT as this is the default alignment
  if (myAlign == gdlwALIGN_NOT) return wxALIGN_CENTER;
  //left is top by default and right is bottom. So define left as top and remove top if bottom, etc.
  //ignore sets that do not concern the current layout (vertical or horizontal). Only FlexGridSizer (col >1 or row >1 ) support alignments.
  if (this->GetMyParentBaseWidget()->IsVertical()) { //col>0 = FlexGridSizer, alignment possible: left, center, right
    if (myAlign & gdlwALIGN_CENTER) return wxALIGN_CENTER_HORIZONTAL;
    if (myAlign & gdlwALIGN_LEFT) return wxALIGN_LEFT;
    if (myAlign & gdlwALIGN_RIGHT) return wxALIGN_RIGHT;
    if (myAlign & gdlwALIGN_TOP) return wxALIGN_LEFT;
    if (myAlign & gdlwALIGN_BOTTOM) return wxALIGN_RIGHT;
  } else if (this->GetMyParentBaseWidget()->IsHorizontal()) { //row>0
    if (myAlign & gdlwALIGN_CENTER) return wxALIGN_CENTER_VERTICAL;
    if (myAlign & gdlwALIGN_TOP) return wxALIGN_TOP;
    if (myAlign & gdlwALIGN_BOTTOM) return wxALIGN_BOTTOM;
    if (myAlign & gdlwALIGN_LEFT) return wxALIGN_TOP;
    if (myAlign & gdlwALIGN_RIGHT) return wxALIGN_BOTTOM;
  } else {
    if (myAlign & gdlwALIGN_CENTER) return wxALIGN_CENTER_HORIZONTAL;
    if (myAlign & gdlwALIGN_LEFT) return wxALIGN_LEFT;
    if (myAlign & gdlwALIGN_RIGHT) return wxALIGN_RIGHT;
    if (myAlign & gdlwALIGN_TOP) return wxALIGN_LEFT;
    if (myAlign & gdlwALIGN_BOTTOM) return wxALIGN_RIGHT;
  }
  return 0;
}

inline int GDLWidget::widgetAlignment()
{
  long myAlign = alignment;
  if (myAlign == gdlwALIGN_NOT) {
    myAlign = this->GetMyParentBaseWidget()->getChildrenAlignment();
}
  //wxALIGN_LEFT and TOP is in fact wxALIGN_NOT as this is the default alignment
  long expand=wxEXPAND;
  if (this->IsLabel()) std::cerr<<"GDLWidget::widgetAlignment() called instead of GDLWidgetLabel::widgetAlignment()!"<<std::endl; //expand=0; //labels are not expanded 
  if (this->IsDraw()) expand=0; //draw are not expanded 
  if (this->IsDropList()) expand=0; //droplists are not expanded 
  if (myAlign == gdlwALIGN_NOT) return expand; //|wxALIGN_LEFT|wxALIGN_TOP;
  //left is top by default and right is bottom. So define left as top and remove top if bottom, etc.
  //ignore sets that do not concern the current layout (vertical or horizontal), otherwise complain!
  if (this->GetMyParentBaseWidget()->IsVertical()) { 
    if (myAlign & gdlwALIGN_CENTER) return wxALIGN_CENTER_HORIZONTAL;
    if (myAlign & gdlwALIGN_LEFT) return wxALIGN_LEFT;
    if (myAlign & gdlwALIGN_RIGHT) return wxALIGN_RIGHT;
    if (myAlign & gdlwALIGN_TOP) return wxALIGN_LEFT;
    if (myAlign & gdlwALIGN_BOTTOM) return wxALIGN_RIGHT;
  } else if (this->GetMyParentBaseWidget()->IsHorizontal()) {
    if (myAlign & gdlwALIGN_CENTER) return wxALIGN_CENTER_VERTICAL;
    if (myAlign & gdlwALIGN_TOP) return wxALIGN_TOP;
    if (myAlign & gdlwALIGN_BOTTOM) return wxALIGN_BOTTOM;
    if (myAlign & gdlwALIGN_LEFT) return wxALIGN_TOP;
    if (myAlign & gdlwALIGN_RIGHT) return wxALIGN_BOTTOM;
  } else {
    if (myAlign & gdlwALIGN_CENTER) return wxALIGN_CENTER_HORIZONTAL;
    if (myAlign & gdlwALIGN_LEFT) return wxALIGN_LEFT;
    if (myAlign & gdlwALIGN_RIGHT) return wxALIGN_RIGHT;
    if (myAlign & gdlwALIGN_TOP) return wxALIGN_LEFT;
    if (myAlign & gdlwALIGN_BOTTOM) return wxALIGN_RIGHT;
  }
  return 0;
}

 void GDLWidget::setFont() {
   wxWindow* w = dynamic_cast<wxWindow*> (this->GetWxWidget());
   if (w != NULL) w->SetFont(font); //may be menu: this is ok
 }
 void GDLWidget::setFont(wxObject* o) {
   wxWindow* w = dynamic_cast<wxWindow*> (o);
   if (w != NULL) w->SetFont(font);
}

inline wxSizer* AddABaseSizer(DLong col, DLong row, bool grid, long space)
{
  if (row <= 0 && col <= 0) return NULL;
  //This to mimic the disposal of children in IDL's Bases, where the 'packing' is more clever
  //than in wxWidgets, we will ALWAYS use a ROW gridding, where the number of rows is
  //deduced from the number of cols and of children when this number is known...
  //..which is not the case for us until the wigdet is realized, and ReorderWidgets() is called.
  //So the col >1 case below is a bit irrelevant as it will be recomputed.
  //In this case (col >1) we do not create a base Sizer, it will be created in ReorderWidgets().
  //We also need to use a BoxSizer for col==1 or row==1 as only BoxSizer is able to strectch correctly.
  if (row==1 && space < 1) {
    wxBoxSizer* s=new wxBoxSizer(wxHORIZONTAL);
    return s;
}
  if (col==1 && space < 1) {
    wxBoxSizer* s=new wxBoxSizer(wxVERTICAL);
    return s;
  }
  // grid behaviour of Motif (IDL) is not as perfect as wxWidget's. We cannot use wxGridSizer here,
  // the trick is to set only the non-flexible grow mode

  if (row>0) {
    wxFlexGridSizer* s=new wxFlexGridSizer(row,0,space,space);
    if (grid) {
      s->SetFlexibleDirection(wxVERTICAL);
      s->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_ALL);
    }
    return s;
  }
//  if (col>0) { //will be treated only at realize time, since default order is not good.
//    return NULL;
//  }
   if (col>0) {
    wxFlexGridSizer* s=new wxFlexGridSizer(0,col,space,space);
    if (grid) {
      s->SetFlexibleDirection(wxHORIZONTAL);
      s->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_ALL);
    }
    return s;
  }
  return NULL;
}

//returns the current (widget or default) fontSize, taking care a typical width is returned instead of 0 as it is the case for variable pitch fonts, at least under windows
inline wxSize GDLWidget::getFontSize() {
  wxSize fontSize = defaultFont.GetPixelSize();
  if (!font.IsSameAs(wxNullFont)) fontSize = font.GetPixelSize();
  // under Windows, as of today: problem getting sizes
  if (fontSize.x < 1 || fontSize.y < 1 ) { //do it ourselves
    wxScreenDC dc;
    dc.SetFont(font);
    fontSize=dc.GetTextExtent(wxString('M'));
  }
  return fontSize;
}

// return the size of text (pixels) as it will take if displayed with current or given font
wxSize GDLWidget::calculateTextScreenSize(std::string &s, wxFont testFont) {
  wxFont f=font; //current font
//  std::cerr<<font.GetFamilyString()<<","<<font.GetFaceName()<<","<<font.GetNativeFontInfoUserDesc()<<std::endl;
  if (testFont!=wxNullFont) f = testFont;
  wxScreenDC dc;
  dc.SetFont(f);
  wxSize ret=dc.GetTextExtent(wxString(s.c_str( ), wxConvUTF8));
//  std::cerr<<ret.x<<","<<ret.y<<std::endl;
  return ret;
}

inline wxSize GDLWidgetText::computeWidgetSize()
{
  wxWindow* me = dynamic_cast<wxWindow*> (this->GetWxWidget());
  //widget text size is in LINES in Y and CHARACTERS in X. But overridden by scr_xsize et if present
  wxRealPoint widgetSize = wxRealPoint(-1,-1);
  wxSize fontSize = getFontSize();
  static std::string testExtent("M");
  wxSize otherSize=calculateTextScreenSize(testExtent); //use text extent of an EM for realistic fontSize. 
  int lineHeight=otherSize.y;
  if (textSize.x > 0) {
    widgetSize.x = (textSize.x) * fontSize.x;
  } else {
      widgetSize.x = maxlinelength*fontSize.x;
      textSize.x=maxlinelength;
      if (textSize.x < 20) {
        textSize.x=20;
        widgetSize.x = (textSize.x) * fontSize.x;
      }
  }

  if (textSize.y > 1) {
    widgetSize.y = textSize.y * lineHeight;
  } else {
    widgetSize.y = lineHeight;
    textSize.y = 1;
  }
  if (textSize.y ==1) widgetSize.y+=2*gdlTEXT_SPACE; //for margin
  if (scrolled && textSize.y >1) widgetSize.x+=gdlSCROLL_WIDTH_Y;

  //if multiline and no hscroll, a x-axis scrollbar will be adde by wxWidgets if longestLineSize cannot be shown, and we cannot do anything about it.
  if (!scrolled) {if (textSize.y > 1 && (textSize.x < maxlinelength) )  widgetSize.y += gdlSCROLL_HEIGHT_X; }
  else if (textSize.y > 1 && widgetSize.x < (maxlinelength*fontSize.x+gdlSCROLL_WIDTH_Y) ) {
    widgetSize.y += gdlSCROLL_HEIGHT_X; 
  }
  widgetSize.x+=2*gdlTEXT_SPACE; //for margin
   
  //but..
  if (wScreenSize.x > 0) widgetSize.x = wScreenSize.x;
  if (wScreenSize.y > 0) widgetSize.y = wScreenSize.y;

  int x = ceil(widgetSize.x);
  int y = ceil(widgetSize.y);
  //memorize the current textSize, in characters.
  initialSize=textSize;
  return wxSize(x, y);
}

inline wxSize GDLWidgetList::computeWidgetSize()
{ 
  //default wxChoice sizing is 2 lines, lines are well-separated. default IDL is only 1 line. Thus we need to fix sizes 
  
  //widget text size is in LINES in Y and CHARACTERS in X. But overridden by scr_xsize et if present
  wxRealPoint widgetSize = wxRealPoint(-1,-1);
  wxSize fontSize = getFontSize();
  //based on experience, actual line height is 1.2 times font y size for fonts > 20 but 1.5 for smaller fonts
  int lineHeight=(fontSize.y<20)?fontSize.y*1.5:fontSize.y*1.2;
  if (wSize.x > 0) {
    widgetSize.x = (wSize.x+1) * fontSize.x;
  } else {
    widgetSize.x = ( maxlinelength+1 ) * fontSize.x;
  } 

  if (wSize.y > 0) {
    widgetSize.y = wSize.y * lineHeight;
  } else {
    widgetSize.y = lineHeight;
  }

  if (wSize.x > 0 && maxlinelength > wSize.x) widgetSize.y += gdlSCROLL_HEIGHT_X;
  if (nlines > wSize.y) widgetSize.x += gdlSCROLL_WIDTH_Y;
  widgetSize.y += 10;
  //but..
  if (wScreenSize.x > 0 ) widgetSize.x = wScreenSize.x;  //we need an integer value
  if (wScreenSize.y > 0) widgetSize.y = wScreenSize.y;

  int x=ceil(widgetSize.x); int y=ceil(widgetSize.y);
  return wxSize(x,y);
}

inline wxSize GDLWidgetLabel::computeWidgetSize()
{
  if (wSize.x > 0 || wSize.y > 0 || wScreenSize.x > 0 || wScreenSize.y > 0) dynamicResize=-1;
  //widget label size is in pixels.
  wxSize widgetSize = wSize; //start with wanted values.
  wxSize fontSize = getFontSize();
  //based on experience, actual line height is 1.2 times font y size for fonts > 20 but 1.5 for smaller fonts
  int lineHeight = fontSize.y+2*gdlLABEL_SPACE ; //(fontSize.y < 20) ? fontSize.y * 1.2 : fontSize.y * 1.2;  
  wxSize s=calculateTextScreenSize(value);
  if (wSize.x < 0) widgetSize.x =  s.x+2*fontSize.x;//add 2 char wide for border. //fontSize.x*(value.size());
  if (wSize.y < 0) widgetSize.y = s.y;//lineHeight;

  if (wScreenSize.x > 0) widgetSize.x = wScreenSize.x;
  if (wScreenSize.y > 0) widgetSize.y = wScreenSize.y;

  if (sunken) {widgetSize.y+=1; widgetSize.x+=1;}
 
  return widgetSize;
}
inline wxSize GDLWidgetLabel::updateDynamicWidgetSize()
{
  //widget label size is in pixels.
  wxSize widgetSize = wxSize(-1, -1); //start unknown sizes.
  wxSize fontSize = getFontSize();
  //based on experience, actual line height is 1.2 times font y size for fonts > 20 but 1.5 for smaller fonts
  int lineHeight = fontSize.y+2*gdlLABEL_SPACE ; //(fontSize.y < 20) ? fontSize.y * 1.2 : fontSize.y * 1.2;  
  
  widgetSize.x =  calculateTextScreenSize(value).x+2*fontSize.x;//add 2 char wide for border. //fontSize.x*(value.size());
  widgetSize.y = lineHeight;

  if (sunken) {widgetSize.y+=1; widgetSize.x+=1;}

  return widgetSize;
}
inline wxSize GDLWidgetMenuBarButton::computeWidgetSize()
{
  if (wSize.x > 0 || wSize.y > 0 || wScreenSize.x > 0 || wScreenSize.y > 0) dynamicResize=-1;
  //widget label size is in pixels.
  wxSize widgetSize = wSize; //start with wanted values.
  wxSize fontSize = getFontSize();
  //based on experience, actual line height is 1.2 times font y size for fonts > 20 but 1.5 for smaller fonts
  int lineHeight = 2*fontSize.y; 
  std::string s=valueWxString.ToStdString();
  if (wSize.x < 0) widgetSize.x =  calculateTextScreenSize(s).x+2*fontSize.x;//add 2 char wide for border. //fontSize.x*(value.size());
  if (wSize.y < 0) widgetSize.y = lineHeight;

  if (wScreenSize.x > 0) widgetSize.x = wScreenSize.x;
  if (wScreenSize.y > 0) widgetSize.y = wScreenSize.y;

  return widgetSize;
}
inline wxSize GDLWidget::computeWidgetSize()
{
//here is a good place to make dynamic widgets static, since dynamic_resize is permitted only if size is not given.
  if (wSize.x > 0 || wSize.y > 0 || wScreenSize.x > 0 || wScreenSize.y > 0) dynamicResize=-1;
  wxSize widgetSize;
  if ( wSize.x > 0 ) widgetSize.x = wSize.x*unitConversionFactor.x;
  else widgetSize.x = wxDefaultSize.x;
//but..
  if (wScreenSize.x > 0) widgetSize.x=wScreenSize.x;
  
  if ( wSize.y > 0 )  widgetSize.y = wSize.y * unitConversionFactor.y; 
  else widgetSize.y = wxDefaultSize.y;
//but..
   if (wScreenSize.y > 0) widgetSize.y=wScreenSize.y;  
  return widgetSize;
}

//void GDLWidget::UpdateGui()
//{
//  //needed to recompute sizes in case of a change of realized widgets or any size change.
//  START_CHANGESIZE_NOEVENT 
//  
//  WidgetIDT actID = parentID;
//  while ( actID != GDLWidget::NullID ) {
//    GDLWidget *widget = GetWidget( actID );
//      wxPanel* p=widget->GetPanel();
//    if (p) {
//      wxSizer* s = p->GetSizer();
//      if (s) s->Fit(p);
//      else { //force widget having the panel to adopt correct sizes
//        p->Fit();
//        GDLWidgetContainer* myParentContainer = static_cast<GDLWidgetContainer*> (widget->GetMyParentBaseWidget());
//        if (myParentContainer != NULL) {
//          wxWindow* w = static_cast<wxWindow*> (widget->GetParentPanel());
//          assert(w != NULL);
//          wxSize wSize = w->GetSize();
//          wxSize mySize = p->GetSize();
//          // if w.x or w.y was SET BY USER it is a requested size, that we should not change
//          wxSize desiredSize;
//          if (myParentContainer->xFree()) desiredSize.x = max(mySize.x, wSize.x);
//          else {
//            desiredSize.x = wSize.x;
//          }
//          if (myParentContainer->yFree()) desiredSize.y = max(mySize.y, wSize.y);
//          else {
//            desiredSize.y = wSize.y;
//          }
//          w->SetSize(desiredSize);
//          w->SetMinSize(desiredSize);
//        }
//      }
//    }
//    actID = widget->parentID;
//  }
//  this->GetMyTopLevelFrame()->Fit();
//  this->GetMyTopLevelFrame()->Refresh();
////  this->GetMyTopLevelFrame()->Update();
//  END_CHANGESIZE_NOEVENT
//  //really show by letting the loop do its magic. Necessary ?.
////  CallWXEventLoop();
//}

//Alternate version if there were sizing problems with the one above.
void GDLWidget::UpdateGui()
{
  START_CHANGESIZE_NOEVENT 
  
  // go back to top parent widget while resizing each BASE containers since there must be some update needed.
  WidgetIDT actID = widgetID;
  while ( actID != GDLWidget::NullID ) {
    GDLWidget *widget = GetWidget( actID ); //Not Necessarily a Base
    if (widget->IsBase()) {
      wxPanel* p=widget->GetPanel();
      wxWindow* thisBaseOuterPanel=dynamic_cast<wxWindow*>(widget->GetWxContainer());
      wxWindow* thisBaseInnerPanel=dynamic_cast<wxWindow*>(widget->GetWxWidget());
      assert((p==thisBaseOuterPanel)||(p==thisBaseInnerPanel));
      GDLWidgetBase* b=static_cast<GDLWidgetBase*>(widget);
      if (b->IsScrolled()) thisBaseInnerPanel->FitInside(); else thisBaseInnerPanel->Fit();
      while (thisBaseOuterPanel!=thisBaseInnerPanel) {
        wxSizer* s=thisBaseInnerPanel->GetContainingSizer();
        assert (s); //must exist as there are always sizers between theWxContainer and theWxWidget
        s->FitInside(thisBaseInnerPanel);
        thisBaseInnerPanel=thisBaseInnerPanel->GetParent();
      }
    }
    actID = widget->parentID;
  }
  this->GetMyTopLevelFrame()->Fit();
  END_CHANGESIZE_NOEVENT 
}

// widget from ID
GDLWidget* GDLWidget::GetWidget( WidgetIDT widID)
{
  if ( widID == GDLWidget::NullID ) return NULL;
  WidgetListT::iterator it = widgetList.find( widID );
  if ( it == widgetList.end( ) ) return NULL;
  return it->second;
}

// widget parent from ID
GDLWidget* GDLWidget::GetParent( WidgetIDT widID)
{
  GDLWidget *widget = GetWidget( widID );
  if ( widget == NULL )
    return NULL;
  WidgetIDT parentID = widget->parentID;
  GDLWidget *parent = GetWidget( parentID );
  return parent;
}
// widget parent
GDLWidget* GDLWidget::GetMyParent( ){return GetWidget( parentID ); }

void  GDLWidget::EnableWidgetUpdate(bool update){
   wxWindow * me = dynamic_cast<wxWindow*>(theWxWidget);
   if (me) {if (update) {if ( me->IsFrozen()) me->Thaw(); else me->Refresh(); } else me->Freeze();} else cerr<<"freezing unknown widget\n";
}
  
GDLWidgetBase* GDLWidget::GetMyBaseWidget()
{
  WidgetIDT actID = widgetID;
  while ( 1 ) {
    GDLWidget* widget = GetWidget( actID );
    if ( widget == NULL )
      return NULL;
    if ( widget->IsBase( ) ) 
      return static_cast<GDLWidgetBase*> (widget);
    assert( widget->parentID != GDLWidget::NullID );
    actID = widget->parentID;
  }
}

// this widget's first parent base widget
GDLWidgetBase* GDLWidget::GetMyParentBaseWidget( )
{
  WidgetIDT actID =  parentID;
  while ( 1 ) {
    GDLWidget* widget = GetWidget( actID );
    if ( widget == NULL )
      return NULL;
    if ( widget->IsBase( ) ) 
      return static_cast<GDLWidgetBase*> (widget);
    assert( widget->parentID != GDLWidget::NullID );
    actID = widget->parentID;
  }
}

WidgetIDT GDLWidget::GetBaseId( WidgetIDT widID)
{
  GDLWidget *widget;
  WidgetIDT actID = widID;
  while ( 1 ) {
    GDLWidget* widget = GetWidget( actID );
    if ( widget == NULL )
      return GDLWidget::NullID;
    if ( widget->IsBase( ) )
      return actID;
    assert( widget->parentID != GDLWidget::NullID );
    actID = widget->parentID;
  }
}

GDLWidgetTopBase* GDLWidget::GetTopLevelBaseWidget( WidgetIDT widID)
{
  WidgetIDT actID = widID;
  while ( 1 ) {
    GDLWidget *widget = GetWidget( actID );
    if ( widget == NULL )
      return (GDLWidgetTopBase*)GDLWidget::NullID;
    if ( widget->parentID == GDLWidget::NullID )
      return static_cast<GDLWidgetTopBase*> (widget);
    else
      actID = widget->parentID;
  }
}

GDLWidgetTopBase* GDLWidget::GetMyTopLevelBaseWidget()
{
  WidgetIDT actID = this->GetWidgetID();
  while ( 1 ) {
    GDLWidget *widget = GetWidget( actID );
    if ( widget == NULL )
      return (GDLWidgetTopBase*)GDLWidget::NullID;
    if ( widget->parentID == GDLWidget::NullID )
      return static_cast<GDLWidgetTopBase*> (widget);
    else
      actID = widget->parentID;
  }
}

gdlwxFrame* GDLWidget::GetMyTopLevelFrame()
{
  WidgetIDT actID = this->GetWidgetID();
  while ( 1 ) {
    GDLWidget *widget = GetWidget( actID );
    if ( widget == NULL )
      return (gdlwxFrame*) NULL;
    if ( widget->parentID == GDLWidget::NullID )
      return static_cast<GDLWidgetTopBase*>(widget)->GetTopFrame();
    else
      actID = widget->parentID;
  }
}
WidgetIDT GDLWidget::GetIdOfTopLevelBase( WidgetIDT widID)
{
  WidgetIDT actID = widID;
  while ( 1 ) {
    GDLWidget *widget = GetWidget( actID );
    if ( widget == NULL )
      return GDLWidget::NullID;
    if ( widget->parentID == GDLWidget::NullID )
      return actID;
    else
      actID = widget->parentID;
  }
}

/**
  @brief        content of theWxWidget has changed, make size changes appear if Dynamic property Set
 */
void GDLWidget::RefreshDynamicWidget() {
    if (this->IsDynamicResize()) {
      if (theWxContainer && theWxContainer!=theWxWidget) {
        wxPanel* p=static_cast<wxPanel*>(theWxContainer);
        wxWindow* w=static_cast<wxWindow*>(theWxWidget);
        if (p) {
          wxSizer* s=p->GetSizer();
          if (s) s->Fit(w);  else p->Fit();
        }
      }
      UpdateGui();
    }
}

void GDLWidget::SendWidgetTimerEvent(int millisecs) {
  if (millisecs < 1) millisecs=1;  //otherwise 0 hangs on OSX
  WidgetIDT* id = new WidgetIDT(widgetID);
  if (theWxWidget) { //we nee a handle on a wxWindow object...
    wxWindow* w = dynamic_cast<wxWindow*> (theWxWidget);
    assert(w != NULL);
    w->GetEventHandler()->SetClientData(id);
    if (m_windowTimer == NULL) {
      m_windowTimer = new wxTimer(w->GetEventHandler(), widgetID);
    }
#ifdef GDL_DEBUG_WIDGETS
    std::cerr << "sending event," << widgetID << "," << m_windowTimer << std::endl;
#endif
    m_windowTimer->StartOnce(millisecs);
  }
}

void GDLWidget::ClearEvents() {
    InteractiveEventQueue.Purge(this->GetWidgetID());
    BlockingEventQueue.Purge(this->GetWidgetID());
}

void GDLWidget::HandleUnblockedWidgetEvents()
{
  //make one loop for wxWidgets Events. Forcibly, as HandleUnblockedWidgetEvents() is called by the readline eventLoop, we are in a non-blocked case.
  CallWXEventLoop();
  //treat our GDL events...
    DStructGDL* ev = NULL;
    while( (ev = GDLWidget::InteractiveEventQueue.Pop()) != NULL)
    {
      ev = CallEventHandler( ev );

      if( ev != NULL)
      {
        GDLDelete( ev );
        ev = NULL;
      }
	  CallWXEventLoop(); // eneble results of above in the widgets
    }
    if (wxIsBusy()) wxEndBusyCursor( );
  }

void GDLWidget::PushEvent( WidgetIDT baseWidgetID, DStructGDL* ev) {
  // Get XmanagerActiveCommand status
  GDLWidget *baseWidget = GDLWidget::GetWidget( baseWidgetID );
  if ( baseWidget != NULL ) {
    bool interactive = baseWidget->IsUsingInteractiveEventLoop( );
    if ( interactive ) { //non-Blocking: events in InteractiveEventQueue.
#ifdef GDL_DEBUG_WIDGETS
           wxMessageOutputStderr().Printf(_T("InteractiveEventQueue.PushEvent: %d\n"),baseWidgetID);
#endif
      InteractiveEventQueue.PushBack( ev );
    } else { //blocking: events in BlockingEventQueue.
#ifdef GDL_DEBUG_WIDGETS
           wxMessageOutputStderr().Printf(_T("BlockingEventQueue.PushEvent: %d\n"),baseWidgetID);
#endif
      BlockingEventQueue.PushBack( ev );
    }
  } else cerr << "NULL baseWidget (possibly Destroyed?) found in GDLWidget::PushEvent( WidgetIDT baseWidgetID=" << baseWidgetID << ", DStructGDL* ev=" << ev << "), please report!\n";
}

void GDLWidget::InformAuthorities(const std::string& message){
        // create GDL event struct
        DStructGDL* ev = new DStructGDL( "*WIDGET_RUNTIME_ERROR*" );
        ev->InitTag( "ID", DLongGDL( 0) );
        ev->InitTag( "TOP", DLongGDL( 0 ) );
        ev->InitTag( "HANDLER", DLongGDL( 0 ) );
        ev->InitTag( "MESSAGE", DStringGDL(message) );
          InteractiveEventQueue.PushFront( ev ); // push front (will be handled next)
    }
//return false if already blocked by XManager (one managed realized top Widget is not marked as interactive).
bool GDLWidget::IsXmanagerBlocking() 
{
  WidgetListT::iterator it;
  // (*it).first is widgetID
  // (*it).second is pointer to widget

  for ( it = widgetList.begin( ); it != widgetList.end( ); ++it ) {
    // Only consider managed top base widgets
    if ( (*it).second->parentID == GDLWidget::NullID ) {
      bool managed = (*it).second->GetManaged( );
      bool realized = (*it).second->IsRealized( );
      if (managed & realized) {
        bool IsBlocked = ((*it).second->IsUsingInteractiveEventLoop( )==false);
        if (IsBlocked) {
          //std::cerr<<"Found Blocked by "<<(*it).second->GetWidgetID()<<std::endl;
          return true;
        }
      }
    }
  }
  //std::cerr<<"Found UnBlocked"<<std::endl;
  return false;
}
//return true if at least one Managed Realized Top Widget is present in the hierarchy
bool GDLWidget::IsActive() 
{
  WidgetListT::iterator it;
  // (*it).first is widgetID
  // (*it).second is pointer to widget

  for ( it = widgetList.begin( ); it != widgetList.end( ); ++it ) {
    // Only consider managed realized top base widgets
    if ( (*it).second->parentID == GDLWidget::NullID ) {
      bool managed = (*it).second->GetManaged( );
      bool realized = (*it).second->IsRealized( );
      if (managed && realized) return true;
    }
  }
  return false;
}

DLong GDLWidget::GetNumberOfWidgets() {
  WidgetListT::iterator it;
  DLong result=0;
  for ( it = widgetList.begin( ); it != widgetList.end( ); ++it ) result++;
  return result;
}
BaseGDL* GDLWidget::GetWidgetsList() {
  DLong nw=GetNumberOfWidgets();
  if (nw<=0) return new DLongGDL(0);
  WidgetListT::iterator it;
  SizeT index=0;
  DLongGDL* result=new DLongGDL(nw,BaseGDL::NOZERO);
  for ( it = widgetList.begin( ); it != widgetList.end( ); ++it ) {
    (*result)[index]=(*it).second->widgetID; index++;
  }
  return result;
}
BaseGDL* GDLWidget::GetManagedWidgetsList() {
  DLong nw=GetNumberOfWidgets();
  if (nw<=0) return new DLongGDL(0);
  WidgetListT::iterator it;
  SizeT index;
  for (index=0, it = widgetList.begin( ); it != widgetList.end( ); ++it ) {
    if ((*it).second->GetManaged() == true) index++;
  }
  if (index<=0) return new DLongGDL(0);
  DLongGDL* result=new DLongGDL(index,BaseGDL::NOZERO);
  for (index=0, it = widgetList.begin( ); it != widgetList.end( ); ++it ) {
    if ((*it).second->GetManaged() == true) (*result)[index++]=(*it).second->widgetID;
  }
  return result;
}

DLongGDL* GDLWidget::GetAllHeirs(){
  std::vector<WidgetIDT> widgetIDList;
  std::vector<bool> has_children;
  widgetIDList.push_back(this->widgetID);
  if (this->NChildren() > 0) has_children.push_back(true);
  else has_children.push_back(false);
  //loop on this list, and add recursively all children when widget is a container.
  SizeT currentVectorSize = widgetIDList.size();
  while (1) {
    for (SizeT i = 0; i < currentVectorSize; i++) {
      if (has_children.at(i)) {
        has_children.at(i) = false;
        GDLWidget *widget = GDLWidget::GetWidget(widgetIDList.at(i));
        DLongGDL* list = static_cast<GDLWidgetContainer*> (widget)->GetChildrenList();
        for (SizeT j = 0; j < list->N_Elements(); j++) {
          widgetIDList.push_back((*list)[j]);
          if (GDLWidget::GetWidget((*list)[j])->NChildren() > 0) has_children.push_back(true);
          else has_children.push_back(false);
        }
      }
    }
    if (widgetIDList.size() == currentVectorSize) break; //no changes
    currentVectorSize = widgetIDList.size();
  }
  DLongGDL* result = new DLongGDL(currentVectorSize, BaseGDL::NOZERO);
  for (SizeT i = 0; i < currentVectorSize ; ++i) (*result)[i] = widgetIDList[i];
  return result;
}

//
bool GDLWidget::InitWx() {
  // this hack enables to have a GUI on Mac OSX even if the
  // program was called from the command line (and isn't a bundle)
  try{
     wxInitialize();
  } catch (...) {return false;}
 //avoid using if no Display is present!
  wxDisplay *d= new wxDisplay();
  if(d->GetCount()<1) return false;
  wxInitAllImageHandlers(); //do it here once for all
  
#if wxCHECK_VERSION(3,1,6)
  //See #1470 and https://groups.google.com/g/wx-dev/c/fY8WeIDD35I
#if defined ( __WXGTK3__) 
//  std::cerr<<"Suppressing annoying GTK3 Diagnostics.\n";
#ifndef __WXMAC__
  wxGetApp().GTKSuppressDiagnostics();
#endif
#endif
#endif
  return true;
}
// Init
void GDLWidget::Init()
{
 //set system font to something sensible now that wx is ON:
  if (tryToMimicOriginalWidgets)
    systemFont = wxFont(8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL) ;//  identical for me to GDLWidget::setDefaultFont(wxFont("Monospace 8"));
#ifdef __WXMSW__ //update for windows:
    bool ok=systemFont.SetNativeFontInfoUserDesc(wxString("consolas 8"));  //consolas 8 is apparently the one most identical to linux courier 8 and IDL X11 default font.
    if (!ok) systemFont = wxFont(8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL) ; 
#endif
    else systemFont = *wxSMALL_FONT; // close to IDL in size, but nicer (not fixed).

  //initially defaultFont and systemFont are THE SAME.
  defaultFont=systemFont;
  SetWxStarted();
  //initialize default image lists for trees:
  // Make an image list containing small icons
  wxSize ImagesSize(DEFAULT_TREE_IMAGE_SIZE,DEFAULT_TREE_IMAGE_SIZE);
  gdlDefaultTreeImages = new wxImageList(ImagesSize.x, ImagesSize.y, true);
  //order must be same as enum definition! 
  gdlDefaultTreeImages->Add(wxArtProvider::GetBitmap(wxART_NORMAL_FILE, wxART_OTHER, ImagesSize)); //gdlWxTree_ITEM,/gdlWxTree_ITEM_SELECTED (IDL give the same image)
  gdlDefaultTreeImages->Add(wxArtProvider::GetBitmap(wxART_FOLDER, wxART_OTHER, ImagesSize)); //gdlWxTree_FOLDER
  gdlDefaultTreeImages->Add(wxArtProvider::GetBitmap(wxART_FOLDER_OPEN, wxART_OTHER, ImagesSize)); //gdlWxTree_FOLDER_OPEN

  wxSize StateImageSize=wxIcon(pixmap_unchecked).GetSize(); //
  gdlDefaultTreeStateImages = new wxImageList(StateImageSize.x, StateImageSize.y, true);
  gdlDefaultTreeStateImages->Add(wxIcon(pixmap_unchecked)); //gdlWxTree_UNCHECKED
  gdlDefaultTreeStateImages->Add(wxIcon(pixmap_checked)); //gdlWxTree_UNCHECKED
  //create wxIcon HERE and not before wxWidgets is started!
  wxgdlicon = wxIcon(gdlicon_xpm);
  //use a phantom window to retrieve the exact size of scrollBars (wxWidget give wrong values).
  //Also get normal panel color
  gdlwxPhantomFrame* test = new gdlwxPhantomFrame();
   test->Hide();
   test->Realize();
   test->Destroy();
  }
//ResetWidgets
void GDLWidget::ResetWidgets() {
  //Delete current widgets --- complicated, use utility procedure
  std::string callP = "GDL_RESET_WIDGETS";
  StackGuard<EnvStackT> guard(BaseGDL::interpreter->CallStack());
  int proIx = LibProIx(callP);
  if (proIx == -1) proIx = DInterpreter::GetProIx(callP);
  if (proIx == -1) return;
  ProgNodeP callingNode = NULL;
  EnvUDT* newEnv = new EnvUDT(callingNode, proList[ proIx], NULL);
  BaseGDL::interpreter->CallStack().push_back(newEnv);
  BaseGDL::interpreter->call_pro(static_cast<DSubUD*> (newEnv->GetPro())->GetTree());
  //really remove widgets from screen
  CallWXEventLoop();
}
// UnInit
void GDLWidget::UnInit() {
  if (wxIsStarted()) {
    ResetWidgets();
    //clear all events --- otherwise baoum!)
    InteractiveEventQueue.Purge();
    BlockingEventQueue.Purge();
    // the following cannot be done: once unitialized, the wxWidgets library cannot be safely initilized again.:  wxUninitialize( );
    UnsetWxStarted(); //reset handlersOk too.
  }
}

void GDLWidget::ConnectToDesiredEvents(){
  wxWindow* w=dynamic_cast<wxWindow*>(theWxWidget);
  assert (w!=NULL);
  //connect to general events
  w->Connect(widgetID, wxEVT_TIMER, wxTimerEventHandler(gdlwxFrame::OnWidgetTimer)); //always.
  if ( eventFlags & GDLWidget::EV_TRACKING ) {
    w->Connect(widgetID,wxEVT_ENTER_WINDOW, wxMouseEventHandler(gdlwxFrame::OnEnterWindow));
    w->Connect(widgetID,wxEVT_LEAVE_WINDOW, wxMouseEventHandler(gdlwxFrame::OnLeaveWindow));
  }
  if ( eventFlags & GDLWidget::EV_CONTEXT ) w->Connect(widgetID,wxEVT_CONTEXT_MENU, wxContextMenuEventHandler(gdlwxFrame::OnContextEvent));
  if ( eventFlags & GDLWidget::EV_KBRD_FOCUS ) {
      w->Connect(widgetID,wxEVT_SET_FOCUS, wxFocusEventHandler(gdlwxFrame::OnKBRDFocusChange));
      w->Connect(widgetID,wxEVT_KILL_FOCUS, wxFocusEventHandler(gdlwxFrame::OnKBRDFocusChange));
  }
  for (int i = 0; i < desiredEventsList.size(); ++i) {
      desiredEventsList[i]->w->Connect(widgetID, desiredEventsList[i]->t, desiredEventsList[i]->f);
  }
// useful to have no event yet produced at this stage?
//  w->DeletePendingEvents();
}

 bool GDLWidget::DisableSizeEvents(gdlwxFrame* &tlbFrame,WidgetIDT &id) {
  GDLWidgetBase *tlb = this->GetMyTopLevelBaseWidget();
  tlbFrame = tlb->GetMyTopLevelFrame();
  id = tlb->GetWidgetID();
  bool disconnect = ((tlb->GetEventFlags() & GDLWidget::EV_SIZE) == GDLWidget::EV_SIZE);
  if (disconnect) tlbFrame->Disconnect(id, wxEVT_SIZE, gdlSIZE_EVENT_HANDLER);
  return disconnect;
}

 void GDLWidget::EnableSizeEvents(gdlwxFrame* &tlbFrame,WidgetIDT &id) {
  if (tlbFrame) tlbFrame->Connect(id, wxEVT_SIZE, gdlSIZE_EVENT_HANDLER);
}
//initialize static member
int GDLWidget::gdl_lastControlId=0;

GDLWidget::GDLWidget( WidgetIDT p, EnvT* e, BaseGDL* vV, DULong eventFlags_)
: theWxWidget( NULL )
, theWxContainer(NULL)
, widgetID (0)
, parentID( p )
, uValue( NULL )
, vValue( vV )
, scrolled(false)
, sensitive(true)
, managed( false )
, eventFlags( eventFlags_ )
, exclusiveMode( 0 )
, wOffset(wxDefaultPosition)
, wSize(wxDefaultSize)
, wScreenSize(wxDefaultSize)
, widgetSizer( NULL )
, widgetPanel( NULL )
, framePanel( NULL )
, widgetType(GDLWidget::WIDGET_UNKNOWN)
, widgetName("")
, groupLeader(GDLWidget::NullID)
, unitConversionFactor(wxRealPoint(1.0,1.0)) //no conversion at start.
, frameWidth(0)
, font(defaultFont)
, valid(true)
, alignment(gdlwALIGN_NOT)
, dynamicResize(0) //unset
, eventFun("")
, eventPro("")
, killNotify("")
, notifyRealize("")
, proValue("")
, funcValue("")
, uName("")
//, delay_destroy(false)
{
  m_windowTimer = NULL;
  
  if ( e != NULL ) GetCommonKeywords( e ); else DefaultValuesInAbsenceofEnv();

// was   widgetID =  wxWindow::NewControlId( ); // but some scripts use the fact that widget ids are positive (graffer.pro)
  widgetID = GDLNewControlId( );
  
  if ( parentID != GDLWidget::NullID ) {

    GDLWidget* gdlParent = GetWidget( parentID );
    assert( gdlParent != NULL);
    
    // a col or row sizer means no offset
    if (GetParentSizer()) wOffset=wxDefaultPosition;
    //there is a ParentID, so widgetPanel:
    widgetPanel=GetParentPanel();
      
    if ( gdlParent->IsContainer( ) ) { //only case, include some buttons
      GDLWidgetContainer* base = dynamic_cast<GDLWidgetContainer*> (gdlParent);
      base->AddChildID( widgetID );
    }
    else if ( gdlParent->IsMenu( ) )
    { 
      GDLWidgetMenu* menu = dynamic_cast<GDLWidgetMenu*> (gdlParent);
      if (menu) menu->AddChildID( widgetID );
    }
    else if ( gdlParent->IsMenuBar( ) )
    { 
       GDLWidgetMenuBar* mb = dynamic_cast<GDLWidgetMenuBar*> (gdlParent);
       mb->AddChildID( widgetID );
    }
//    else if ( gdlParent->IsTree( ) )
//    { 
//      // do nothing 
//    }//    else {    assert(false);} //other widgets are parent of popups, assert should be removed.
  }
  
  widgetList.insert( widgetList.end( ), std::pair<WidgetIDT, GDLWidget*>(widgetID, this) );
#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputStderr( ).Printf( _T( "inserted: ID: %d  parentID: %d\n" ), widgetID, parentID );
#endif
  if (groupLeader != GDLWidget::NullID) {
    GDLWidget* leader=this->GetWidget(groupLeader);
    if (leader) leader->AddToFollowers(widgetID);
  }
}

void GDLWidget::SetSensitive(bool value)
{
    sensitive = value;
    wxWindow *me=dynamic_cast<wxWindow*>(theWxWidget); 
    if (me) {if (value) me->Enable(); else me->Disable();}
}

bool GDLWidget::GetSensitive()
{
  return sensitive;
}

  
DLong GDLWidget::GetSibling()
{
  if ( parentID == GDLWidget::NullID ) {return 0;}
  GDLWidget * parent=GetWidget(parentID);
  if (parent->IsContainer() || parent->IsMenuBar() || parent->IsMenu() || parent->IsTree()) {
    return parent->GetTheSiblingOf(widgetID);
  }
  return 0;
}
void GDLWidget::SetFocus() //gives focus to the CHILD of the panel.
{
  wxWindow *me=dynamic_cast<wxWindow*>(this->GetWxWidget()); if (me!=NULL) me->SetFocus(); else cerr<<"Setting Focus for unknown widget!\n";
}
  void GDLWidget::SetWidgetPosition(DLong posx, DLong posy){
  // -1: not set ---> keep the same
    wxWindow* me=static_cast<wxWindow*>(theWxContainer);
    if (me) {
      wxPoint where=me->GetPosition();
      if (posx >= 0) where.x=posx;
      if (posy >= 0) where.y=posy;
      me->Move(where);
    } else cerr<<"set offset on non-existent widget!"<<endl;

    UpdateGui();

  }

void GDLWidget::SetWidgetSize(DLong sizex, DLong sizey) //in pixels. Always.
{
  START_CHANGESIZE_NOEVENT
  
  //although 2 values are passed, one only may be set by the user.
  //the code is:
  // -1: not set --> keep the same
  // 0 : set, and means: do your best.
  // > 0 : imposed size.

  //Sizes are in pixels. Units must be converted before calling this function.
  wxWindow* me=dynamic_cast<wxWindow*>(theWxWidget);
  if (me==NULL) {
#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputStderr( ).Printf( _T( "GDLWidget::SetSize of non wxWindow %s\n"),this->GetWidgetName().c_str());
#endif
  return;
  }
  //a graphic window cannot be larger than the base widget it is in, if the base widget size has been fixed
  wxSize currentSize=me->GetSize();
  wxSize currentBestSize=me->GetBestSize();
#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputStderr( ).Printf( _T( "GDLWidget::SetSize currentSize=%d,%d (%s)\n"),currentSize.x,currentSize.y,this->GetWidgetName().c_str());
#endif
  //unprecised sizes do not change
  if (sizex == -1) sizex = currentSize.x;
  if (sizey == -1) sizey = currentSize.y;
  
  //passed zeroes = your best is to keep best size (in cache, value defined at creation)
  if (sizex == 0) sizex = currentBestSize.x;
  if (sizey == 0) sizey = currentBestSize.y;
  wSize.x = sizex;
  wSize.y = sizey;
  me->SetSize(wSize);
  //the sizer takes care of the eventual frame
  me->SetMinSize(wSize);
  
  UpdateGui();

  END_CHANGESIZE_NOEVENT
  

#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputStderr( ).Printf( _T( "GDLWidget::SetSize %d,%d (%s)\n"),sizex,sizey,this->GetWidgetName().c_str());
#endif
}

void GDLWidget::SetWidgetScreenSize(DLong sizex, DLong sizey) //in pixels. Always.
{
  START_CHANGESIZE_NOEVENT

  //although 2 values are passed, one only may be set by the user.
  //the code is:
  // -1: not set --> keep the same
  // 0 : set, and means: do your best.
  // > 0 : imposed size.

  //Sizes are in pixels. Units must be converted before calling this function.
  wxWindow* me = dynamic_cast<wxWindow*> (theWxWidget);
  if (me == NULL) {
#ifdef GDL_DEBUG_WIDGETS
    wxMessageOutputStderr().Printf(_T("GDLWidget::SetSize of non wxWindow %s\n"), this->GetWidgetName().c_str());
#endif
    return;
  }
  //a graphic window cannot be larger than the base widget it is in, if the base widget size has been fixed
  wxSize currentSize = me->GetSize();
  wxSize currentBestSize = me->GetBestSize();
#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputStderr().Printf(_T("GDLWidget::SetSize currentSize=%d,%d (%s)\n"), currentSize.x, currentSize.y, this->GetWidgetName().c_str());
#endif
  //unprecised sizes do not change
  if (sizex == -1) sizex = currentSize.x;
  if (sizey == -1) sizey = currentSize.y;

  if (widgetSizer) {
    //passed zeroes = mr sizer, do your best..
    if (sizex == 0) sizex = -1;
    if (sizey == 0) sizey = -1;
  } else {
    //passed zeroes = your best is to keep best size (in cache, value defined at creation)
    if (sizex == 0) sizex = currentBestSize.x;
    if (sizey == 0) sizey = currentBestSize.y;
  }
  wSize.x = sizex;
  wSize.y = sizey;
  me->SetSize(wSize); //includes scrollbars.
  //the sizer takes care of the eventual frame
  me->SetMinSize(wSize); //includes scrollbars.
  //framed bases may have a complicated frame around, to be resized:
  if (theWxContainer != theWxWidget) {
    wxWindow* container = dynamic_cast<wxWindow*> (theWxContainer);
    wxSizer* s = me->GetSizer();
    if (s) s->Fit(me);
    else container->Fit();
  }
  UpdateGui();

  END_CHANGESIZE_NOEVENT


#ifdef GDL_DEBUG_WIDGETS
    wxMessageOutputStderr().Printf(_T("GDLWidget::SetSize %d,%d (%s)\n"), sizex, sizey, this->GetWidgetName().c_str());
#endif
}

void GDLWidgetTabbedBase::SetBaseTitle(std::string &title_)
{
  GDLWidgetTab* parentTab=dynamic_cast<GDLWidgetTab*>(this->GetMyParent());
  if (parentTab) {
    gdlNotebook* wxParent = dynamic_cast<gdlNotebook*> (parentTab->GetWxWidget( ));
    wxParent->SetPageText(wxParent->FindPage(static_cast<wxWindow*>(theWxWidget)),wxString(title_.c_str(), wxConvUTF8));
  }
#ifdef GDL_DEBUG_WIDGETS
  else wxMessageOutputStderr( ).Printf( _T( "GDLWidgetTabbedBase::SetBaseTitle() no parent tab exists? %d\n"));
#endif
}

GDLWidgetTabbedBase::~GDLWidgetTabbedBase()
{
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetTabbedBase(" << widgetID << ")" << std::endl;
#endif
  GDLWidgetTab* parentTab=static_cast<GDLWidgetTab*>(this->GetMyParent());
  if (parentTab) { //may be already destroyed.
    gdlNotebook* wxParent = static_cast<gdlNotebook*> (parentTab->GetWxWidget( ));
    //whereAmI?
    myPage=wxParent->FindPage(static_cast<wxWindow*>(theWxContainer));
    if (wxParent) wxParent->RemovePage(myPage); //do not delete the page, GDL will delete the contents itself, widget per widget.
  }
#ifdef GDL_DEBUG_WIDGETS
  else wxMessageOutputStderr( ).Printf( _T( "~GDLWidgetTabbedBase() no parent tab exists? %d\n"));
#endif
}

void GDLWidget::Realize( bool map, bool use_default) {  GetMyTopLevelBaseWidget()->Realize(map, use_default); }

void GDLWidgetTopBase::Realize(bool map, bool use_default) {
#ifdef GDL_DEBUG_WIDGETS
    wxMessageOutputStderr().Printf(_T("GDLWidgetTopBase:Realize: %d\n"), widgetID);
#endif
  if (use_default) map = GetMap();

  OnRealize();

  if (map) topFrame->Show() ; //endShowRequestEvent();
  else topFrame->Hide(); //SendHideRequestEvent();
  realized = true;
}

bool GDLWidget::IsRealized() {
    GDLWidgetTopBase *tlb = GetMyTopLevelBaseWidget();
    gdlwxFrame* topFrame = tlb->GetTopFrame();
    return (tlb->IsTopLevelRealized());
  }
  void GDLWidgetContainer::OnRealize() {
#ifdef GDL_DEBUG_WIDGETS
    wxMessageOutputStderr().Printf(_T("GDLWidgetContainer::OnRealize: %d\n"), widgetID);
#endif
    //setFont() will set the font for this widget and children if FONT= is present in the WIDGET_XXX command (and is supported) 
  this->setFont();
  this->SetSensitive(sensitive);

  for (std::deque<WidgetIDT>::reverse_iterator c = children.rbegin(); c != children.rend(); ++c) {
//  for (std::deque<WidgetIDT>::iterator c = children.begin(); c != children.end(); ++c) {
   GDLWidget* w = GetWidget(*c);
   if (w != NULL)
    w->OnRealize();
  }
  
  wxPanel* p=static_cast<wxPanel*> (theWxContainer);
  if (p) {
    wxSizer* s = p->GetSizer();
    if (s) s->Fit(p);
    else {
      p->Fit();
      if (this->GetParentSizer() == NULL) { //force size of parent since we cannot leave it to a nonexistent parent sizer.
        GDLWidgetContainer* myParentContainer = static_cast<GDLWidgetContainer*> (this->GetMyParentBaseWidget());
        if (myParentContainer != NULL) {
          wxWindow* w = static_cast<wxWindow*> (this->GetParentPanel());
          assert(w != NULL);
          wxSize wSize = w->GetSize();
          wxSize mySize = p->GetSize();
          // if w.x or w.y was SET BY USER it is a requested size, that we should not change
          wxSize desiredSize;
          if (myParentContainer->xFree()) desiredSize.x = max(mySize.x, wSize.x);
          else {
            desiredSize.x = wSize.x;
          }
          if (myParentContainer->yFree()) desiredSize.y = max(mySize.y, wSize.y);
          else {
            desiredSize.y = wSize.y;
          }
          w->SetSize(desiredSize);
          w->SetMinSize(desiredSize);
        }
      }
    }
  }
#ifdef GDL_DEBUG_WIDGETS
    else wxMessageOutputStderr().Printf(_T("Unknown Container for (%s) widget ID %d\n"), widgetName, widgetID);
#endif

  if (notifyRealize != "") { //insure it is called once only for this.
   std::string note = notifyRealize;
   notifyRealize.clear();
   CallEventPro(note, new DLongGDL(widgetID));
  }
  //define Events Last (TBC).
  ConnectToDesiredEvents();
 }
  
void GDLWidgetMenu::OnRealize() {
  //setFont() will set the font for this widget and children if FONT= is present in the WIDGET_XXX command (and is supported) 
  this->setFont();
  this->SetSensitive(sensitive);

  //Menu children are menus or entries, *NOT* any widget.
  for (std::deque<WidgetIDT>::reverse_iterator c = children.rbegin(); c != children.rend(); ++c) {
//  for (std::deque<WidgetIDT>::iterator c = children.begin(); c != children.end(); ++c) {
   GDLWidgetMenu* w = dynamic_cast<GDLWidgetMenu*>(GetWidget(*c));
   if (w != NULL)
    w->OnRealize();
  }

  if (notifyRealize != "") { //insure it is called once only for this.
   std::string note = notifyRealize;
   notifyRealize.clear();
   CallEventPro(note, new DLongGDL(widgetID));
  }
 }

#define GetSysC(x)   { col=wxSystemSettings::GetColour(x); r=col.Red();g=col.Green();b=col.Blue(); (*val)[0]=r;(*val)[1]=g;(*val)[2]=b; }
BaseGDL * GDLWidget::getSystemColours()
{
  DStructGDL* colo = new DStructGDL( "WIDGET_SYSTEM_COLORS");
  DIntGDL*  val=new DIntGDL( dimension(3), BaseGDL::NOZERO);
  int  r,g,b;
  wxColour col;
  GetSysC(wxSYS_COLOUR_3DDKSHADOW );
  colo->InitTag("DARK_SHADOW_3D",(*val));
  colo->InitTag("SHADOW_3D", (*val));
  GetSysC(wxSYS_COLOUR_3DLIGHT );
  colo->InitTag("FACE_3D", (*val));
  colo->InitTag("LIGHT_EDGE_3D", (*val));
  colo->InitTag("LIGHT_3D", (*val));
  GetSysC(wxSYS_COLOUR_ACTIVEBORDER  );
  colo->InitTag("ACTIVE_BORDER", (*val));
  GetSysC(wxSYS_COLOUR_ACTIVECAPTION   );
  colo->InitTag("ACTIVE_CAPTION", (*val));
  GetSysC(wxSYS_COLOUR_APPWORKSPACE    );
  colo->InitTag("APP_WORKSPACE", (*val));
  GetSysC(wxSYS_COLOUR_DESKTOP     );
  colo->InitTag("DESKTOP", (*val));
  GetSysC(wxSYS_COLOUR_BTNTEXT     );
  colo->InitTag("BUTTON_TEXT", (*val));
  GetSysC(wxSYS_COLOUR_CAPTIONTEXT     );
  colo->InitTag("CAPTION_TEXT", (*val));
  GetSysC(wxSYS_COLOUR_GRAYTEXT     );
  colo->InitTag("GRAY_TEXT", (*val));
  GetSysC(wxSYS_COLOUR_HIGHLIGHT     );
  colo->InitTag("HIGHLIGHT", (*val));
  GetSysC(wxSYS_COLOUR_HIGHLIGHTTEXT     );
  colo->InitTag("HIGHLIGHT_TEXT", (*val));
  GetSysC(wxSYS_COLOUR_INACTIVEBORDER     );
  colo->InitTag("INACTIVE_BORDER", (*val));
  GetSysC(wxSYS_COLOUR_INACTIVECAPTION     );
  colo->InitTag("INACTIVE_CAPTION", (*val));
  GetSysC(wxSYS_COLOUR_INACTIVECAPTIONTEXT     );
  colo->InitTag("INACTIVE_CAPTION_TEXT", (*val));
  GetSysC(wxSYS_COLOUR_INFOBK     );
  colo->InitTag("TOOLTIP_BK", (*val));
  GetSysC(wxSYS_COLOUR_INFOTEXT     );
  colo->InitTag("TOOLTIP_TEXT", (*val));
  GetSysC(wxSYS_COLOUR_MENU     );
  colo->InitTag("MENU", (*val));
  GetSysC(wxSYS_COLOUR_MENUTEXT     );
  colo->InitTag("MENU_TEXT", (*val));
  GetSysC(wxSYS_COLOUR_SCROLLBAR     );
  colo->InitTag("SCROLLBAR", (*val));
  GetSysC(wxSYS_COLOUR_WINDOW     );
  colo->InitTag("WINDOW_BK", (*val));
  GetSysC(wxSYS_COLOUR_WINDOWFRAME     );
  colo->InitTag("WINDOW_FRAME", (*val));
  GetSysC(wxSYS_COLOUR_WINDOWTEXT     );
  colo->InitTag("WINDOW_TEXT", (*val));
  return colo;
}

GDLWidget::~GDLWidget()
{
#ifdef GDL_DEBUG_WIDGETS
      std::string state=(this->IsValid())?" ":"(Already destroyed) "; 
      std::cout << "~GDLWidget "+state+"("<< widgetID <<")"<< std::endl;
#endif
        
  if (!this->IsValid()) return; //to avoid crashing if asserts are not honored
  //unvalidate widget to prevent some further actions
  this->SetUnValid();
  if (m_windowTimer) {if (m_windowTimer->IsRunning()) m_windowTimer->Stop();}

  // kill followers (here?)
  // delete all followers (in reverse order ?)
  while (!followers.empty()) {
#ifdef GDL_DEBUG_WIDGETS
      std::cout << "~GDLWidget("<< widgetID <<"): destroy follower "<< followers.back()<< endl;
#endif
      GDLWidget* follower=GetWidget(followers.back()); followers.pop_back();
    if (follower) delete follower;
  }

  GDLWidget* gdlParent = GetMyParent();
  if (gdlParent) { //not the TLB
    gdlParent->RemoveIfFollower(widgetID);

//    UpdateGui();  //way too long! use a more subtle command!

  } else {
    static_cast<GDLWidgetTopBase*>(this)->GetTopFrame()->Hide();
    static_cast<GDLWidgetTopBase*>(this)->GetTopFrame()->Destroy(); //delete topBase
  }
  
  GDLDelete(uValue);
  GDLDelete(vValue);
  uName.clear();
  proValue.clear();
  funcValue.clear();
  eventPro.clear(); // event handler PRO
  eventFun.clear(); // event handler FUN
  notifyRealize.clear();
  killNotify.clear();
  valid=false; //if not, is in the process of being destroyed (prevent reentrance).

  widgetList.erase(widgetID);
  desiredEventsList.clear();
  followers.clear();
  widgetName.clear();
  
  theWxContainer=NULL;
  theWxWidget=NULL;
  widgetSizer=NULL;
  widgetPanel=NULL;
  framePanel=NULL;
}

void GDLWidget::Raise()
{
  this->GetMyTopLevelFrame()->Raise();
}

void GDLWidget::Lower()
{
  this->GetMyTopLevelFrame()->Lower();
}

DStructGDL* GDLWidget::GetGeometry(wxRealPoint fact)
{
  if (!this->IsRealized()) this->OnRealize();//necessary if a geometry request is done previous to the command widget_control,xxx,,/Realize !
  GDLWidgetBase* container = static_cast<GDLWidgetBase*> (this->GetMyParent());
  assert(container != NULL);
  int ixsize = 0, iysize = 0, iscr_xsize = 0, iscr_ysize = 0;
  DFloat xsize = 0;
  DFloat ysize = 0;
  DFloat scr_xsize = 0;
  DFloat scr_ysize = 0;
  DFloat xoffset = 0;
  DFloat yoffset = 0;
  DFloat margin = 0;

  DStructGDL* ex = new DStructGDL("WIDGET_GEOMETRY");

  wxWindow* test = dynamic_cast<wxWindow*> (theWxContainer);
  if (test==NULL) return ex;
  wxRect r=test->GetRect();
//  std::cerr<<"CONTAINER: x: "<<r.x<<", y: "<<r.y<<", width="<<r.width<<", height="<<r.height<<std::endl;
  xoffset = r.x ;
  yoffset = r.y ;
  wxWindow* w = dynamic_cast<wxWindow*> (theWxWidget);
  if (w==NULL) return ex;
  wxRect z=w->GetRect();
//  std::cerr<<"WIDGET: x: "<<z.x<<", y: "<<z.y<<", width="<<z.width<<", height="<<z.height<<std::endl;

  iscr_xsize = z.width;
  iscr_ysize = z.height;
  wxSize s=w->GetClientSize();
  ixsize = s.x; if (ixsize < 1) ixsize=iscr_xsize; //Patch for OSX where s.x is 0
  iysize = s.y; if (iysize < 1) iysize=iscr_ysize; //Patch for OSX where s.y is 0
  
  if (theWxWidget == theWxContainer) margin=0; else {
    margin=(r.width-z.width)/2; //Rect is about the container, which is the widgetPanel and not the frame depending on case.
  }
  
  if (this->IsInCharacters()) {
  wxSize fontSize = getFontSize();
  xsize = ixsize / fontSize.x;
  ysize = iysize / fontSize.y;
  } else {
  //size is in pixels, pass in requested units (1.0 default)
  xsize = ixsize / fact.x;
  ysize = iysize / fact.y;
  }
  scr_xsize = iscr_xsize / fact.x;
  scr_ysize = iscr_ysize / fact.y;
  xoffset /= fact.x;
  yoffset /= fact.y;
  margin /= fact.x;

  ex->InitTag("XOFFSET", DFloatGDL(xoffset));
  ex->InitTag("YOFFSET", DFloatGDL(yoffset));
  ex->InitTag("XSIZE", DFloatGDL(xsize));
  ex->InitTag("YSIZE", DFloatGDL(ysize));
  ex->InitTag("SCR_XSIZE", DFloatGDL(scr_xsize));
  ex->InitTag("SCR_YSIZE", DFloatGDL(scr_ysize));
  ex->InitTag("MARGIN", DFloatGDL(margin));
//special DRAW:
  if (this->IsDraw()) {
    wxSize d=w->GetVirtualSize();
    DFloat dxsize=d.x;
    DFloat dysize=d.y;
    ex->InitTag("DRAW_XSIZE", DFloatGDL(dxsize/fact.x));
    ex->InitTag("DRAW_YSIZE", DFloatGDL(dysize/fact.y));
  }

  return ex;
}

GDLWidgetContainer::GDLWidgetContainer( WidgetIDT parentID, EnvT* e, ULong eventFlags_)
: GDLWidget( parentID, e, NULL, eventFlags_),xfree(true),yfree(true)
{
}
  
  GDLWidgetContainer::~GDLWidgetContainer()
{
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetContainer(" << widgetID <<")"<< std::endl;
#endif

  // delete all children (in reverse order ?)
  while (!children.empty()) {
    GDLWidget* child = GetWidget(children.back());
    children.pop_back();

     if (child) {
      WidgetIDT childID = child->GetWidgetID();
#ifdef GDL_DEBUG_WIDGETS
      std::cout << "~GDLWidgetContainer, deleting child ID #" << childID << " of container  #" << widgetID << std::endl;
#endif
      // call KILL_NOTIFY procedures
      child->OnKill();
      // widget may have been killed by above OnKill:
      child = GDLWidget::GetWidget(childID);
      if (child != NULL) {
      //special case for WIDGET_DRAW: delete from 'wdelete' command-like:
      if (child->IsDraw()) {
        gdlwxGraphicsPanel* draw=static_cast<gdlwxGraphicsPanel*>(child->GetWxWidget());
        draw->DeleteUsingWindowNumber(); //just emit quivalent to "wdelete,winNum".
      } else delete child;
     }
    }
  }
  if (theWxContainer) static_cast<wxWindow*>(theWxContainer)->Destroy(); //which is the panel.
}

#ifdef PREFERS_MENUBAR  
   GDLWidgetMenuBar::~GDLWidgetMenuBar() {
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetMenuBar(" << widgetID << ")" << std::endl;
#endif

  // delete all children (in reverse order ?)
  while (!children.empty()) {
    GDLWidget* child=GetWidget(children.back()); children.pop_back();
    if (child) {
#ifdef GDL_DEBUG_WIDGETS
      std::cout << "~GDLWidgetMenuBar, deleting child ID #" << child->GetWidgetID() << " of container  #" << widgetID << std::endl;
#endif
      delete child;
    }
  }
  //remove wxWidgets MBAR from wxFrame container otherwise wxFrame will attempt to destroy it again and segfault
  dynamic_cast<wxMenuBar*> (theWxWidget)->GetFrame()->SetMenuBar(NULL);
 }
#else
   GDLWidgetMenuBar::~GDLWidgetMenuBar() {
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetMenuBar(" << widgetID << ")" << std::endl;
#endif

  // delete all children (in reverse order ?)
  while (!children.empty()) {
    GDLWidget* child=GetWidget(children.back()); children.pop_back();
    if (child) {
#ifdef GDL_DEBUG_WIDGETS
      std::cout << "~GDLWidgetMenuBar, deleting child ID #" << child->GetWidgetID() << " of container  #" << widgetID << std::endl;
#endif
      delete child;
    }
  }
  //toolbar can be removed
  dynamic_cast<wxToolBar*> (theWxWidget)->Destroy();
 }
#endif  
 /*********************************************************/
// for WIDGET_BASE
/*********************************************************/
GDLWidgetBase::GDLWidgetBase(WidgetIDT parentID, EnvT* e, ULong eventFlags_,
  bool mapWid,
  DLong col, DLong row,
  int exclusiveMode_,
  const DString& resource_name, const DString& rname_mbar,
  const DString& title_,
  const DString& display_name,
  int xpad_, int ypad_,
  DLong x_scroll_size, DLong y_scroll_size, bool grid_layout, long children_alignment, int space_)
: GDLWidgetContainer(parentID, e, eventFlags_)
, lastRadioSelection(NullID)
, nrows(row)
, ncols(col)
, grid(grid_layout)
, childrenAlignment(children_alignment)
, space(space_)
, xpad(xpad_)
, ypad(ypad_)
, doMap(mapWid) {
  // All bases can receive events: EV_CONTEXT, EV_KBRD_FOCUS, EV_TRACKING

  wSize = computeWidgetSize();
//get immediately rid of scroll sizes in case of scroll or not... Here is the logic:

  if (x_scroll_size > 0) {scrolled=true;x_scroll_size*=unitConversionFactor.x;x_scroll_size+=gdlSCROLL_WIDTH_Y;} 
  if (y_scroll_size > 0) {scrolled=true;y_scroll_size*=unitConversionFactor.y;y_scroll_size+=gdlSCROLL_HEIGHT_X;}
  if (scrolled) {
    if (x_scroll_size < 1) x_scroll_size = gdlDEFAULT_XSIZE+gdlSCROLL_WIDTH_Y;
    if (y_scroll_size < 1) y_scroll_size = gdlDEFAULT_YSIZE+gdlSCROLL_HEIGHT_X;
  }
  wScrollSize = scrolled ? wxSize(x_scroll_size , y_scroll_size ) : wSize; //y_scroll_size + gdlSCROLL_HEIGHT_X);
  xfree=(wScrollSize.x <= 0);
  yfree=(wScrollSize.y <= 0);  
  // Set exclusiveMode
  // If exclusive then set to -1 to signal first radiobutton
  if ( exclusiveMode_ == BGEXCLUSIVE )
  this->SetExclusiveMode( BGEXCLUSIVE1ST );
  else
    this->SetExclusiveMode( exclusiveMode_ );

  // COLUMN is default if col=0 and row=0 for exclusive etc bases. 
  if ( this->GetExclusiveMode() != BGNORMAL ) {
    if (ncols < 1 && nrows <1 ) ncols=1;
  }

}

// generic creation of a base in which widgets will be put.
//
// defines widgetPanel which is the (scrolled) panel in which children will be Added. 
// widgetPanel may be a panel inside a (scrolled) panel used to mimic padding if necessary. In ths case it is the padding panel that is scrolled.
// Further, all this casn be inside a frame.
//
// define the widgetSizer, the sizer that children will 'obey' (that will govern overall placement and size of children)
// the widgetSizer may be NULL, in which case the placement of children inside the panel will be governed by their respective size and position,
// but if widgetSizer exist, then everytime a new child is created, it must be invoked to resize/reposition this child.
//
// NOTE: a=widget_base(/base_align_center, /scroll,xsize=300,ysize=300) will NOT produce a blank 300x300 panel inside a 100x100 scolled window, as we use only ONE
// panel, and the internal size (the VirtualSize) will be determined by what WILL BE created as childrens (there exist no 
// This is completely different from IDL's Motif widgets, and there is no SetMinVirtualSize()
void GDLWidgetBase::CreateBase(wxWindow* parent){
//the container is a ScrollPanel
//  bool doFrame=true; //!(this->IsTopBase()); //IDL Prevents topBases to be framed (?).
  if (frameWidth > 0) {
    wxPanel* frame = new wxPanel(parent, wxID_ANY, wOffset, wxDefaultSize, gdlBORDER_EXT); 
    theWxContainer=frame;
    wxBoxSizer* panelsz = new wxBoxSizer(wxVERTICAL);
    frame->SetSizer(panelsz);

    wxBoxSizer* sz_inside=panelsz;
    wxPanel* frame_inside=frame;
    DLong newframewidth=frameWidth;
// Fancy variant:
    if (frameWidth > 1 /*&&  tryToMimicOriginalWidgets*/ ) {
      newframewidth=frameWidth/2;
//      frame->SetBackgroundColour(*wxBLACK); //will show a strong frame as does IDL
      frame->SetBackgroundColour(wxColour(127,127,127)); //will show a strong frame as does IDL
      int mode = wxBORDER_NONE;
      frame_inside = new wxPanel(frame, wxID_ANY, wxDefaultPosition, wxDefaultSize, mode);
      frame_inside->SetBackgroundColour(*wxLIGHT_GREY);
      panelsz->Add(frame_inside, FRAME_ALLOWSTRETCH, wxALL|wxEXPAND, newframewidth);

      sz_inside = new wxBoxSizer(wxVERTICAL);
      frame_inside->SetSizer(sz_inside);
    } 
    if (xpad > 0 || ypad > 0) {
      wxScrolled<wxPanel>* padxpady = new wxScrolled<wxPanel>(frame_inside);
#ifdef GDL_DEBUG_WIDGETS_COLORIZE
      padxpady->SetBackgroundColour(wxColour(0xa7, 0x3d, 0x0f)); //orange fonce
#else
      if (frameWidth > 1 /*&&  tryToMimicOriginalWidgets*/) padxpady->SetBackgroundColour(wxColour(sysPanelDefaultColour));
#endif
      wxGridBagSizer* sz = new wxGridBagSizer(ypad, xpad);
      padxpady->SetSizer(sz);
      sz->SetEmptyCellSize(wxSize(0, 0));
      if (xpad > 1) sz->Add(0, 0, wxGBPosition(1, 2));
      if (ypad > 1) sz->Add(0, 0, wxGBPosition(2, 1));

      widgetPanel = new wxScrolled<wxPanel>(padxpady, widgetID, wOffset, wxDefaultSize); 
      sz->Add(widgetPanel, wxGBPosition(1, 1));
#ifdef GDL_DEBUG_WIDGETS_COLORIZE
      widgetPanel->SetBackgroundColour(RandomWxColour());
#endif
      //    widgetPanel->SetVirtualSize(wSize);
      padxpady->SetSize(wScrollSize);
      padxpady->SetMinSize(wScrollSize);
      //Just Enable scrollBars if scrolling is necessary
      if (scrolled) {
        padxpady->SetScrollbars(gdlSCROLL_RATE, gdlSCROLL_RATE, wSize.x / gdlSCROLL_RATE, wSize.y / gdlSCROLL_RATE);
        padxpady->ShowScrollbars(wxSHOW_SB_ALWAYS, wxSHOW_SB_ALWAYS);
      }
      sz_inside->Add(padxpady, FRAME_ALLOWSTRETCH, wxALL | wxEXPAND, newframewidth);//gdlFRAME_MARGIN);
      sz_inside->Fit(padxpady);
    } else {
      widgetPanel = new wxScrolled<wxPanel>(frame_inside, widgetID, wOffset, wxDefaultSize);
#ifdef GDL_DEBUG_WIDGETS_COLORIZE
      widgetPanel->SetBackgroundColour(RandomWxColour());
#else
      if (frameWidth > 1 /*&&  tryToMimicOriginalWidgets*/) widgetPanel->SetBackgroundColour(wxColour(sysPanelDefaultColour));
#endif
      //    widgetPanel->SetVirtualSize(wSize);
      widgetPanel->SetSize(wScrollSize);
      widgetPanel->SetMinSize(wScrollSize);
      //Just Enable scrollBars if scrolling is necessary
      if (scrolled) {
        widgetPanel->SetScrollbars(gdlSCROLL_RATE, gdlSCROLL_RATE, wSize.x / gdlSCROLL_RATE, wSize.y / gdlSCROLL_RATE);
        widgetPanel->ShowScrollbars(wxSHOW_SB_ALWAYS, wxSHOW_SB_ALWAYS);
      }
      sz_inside->Add(widgetPanel, FRAME_ALLOWSTRETCH, wxALL | wxEXPAND, newframewidth);//gdlFRAME_MARGIN);
      sz_inside->Fit(widgetPanel);
    }
    theWxWidget = widgetPanel;
  } else {
    if (xpad > 0 || ypad > 0 ) {
      wxScrolled<wxPanel>* padxpady = new wxScrolled<wxPanel>(parent);
      theWxContainer = padxpady;
  #ifdef GDL_DEBUG_WIDGETS_COLORIZE
      padxpady->SetBackgroundColour(wxColour(0xa7, 0x3d, 0x0f)); //orange fonce
  #endif
      wxGridBagSizer* sz = new wxGridBagSizer(ypad, xpad);
      padxpady->SetSizer(sz);
      sz->SetEmptyCellSize(wxSize(0, 0));
      if (xpad > 1) sz->Add(0, 0, wxGBPosition(1, 2));
      if (ypad > 1) sz->Add(0, 0, wxGBPosition(2, 1));

      widgetPanel = new wxScrolled<wxPanel>(padxpady, widgetID, wOffset, wxDefaultSize); 
      sz->Add(widgetPanel, wxGBPosition(1, 1));     
#ifdef GDL_DEBUG_WIDGETS_COLORIZE
      widgetPanel->SetBackgroundColour(RandomWxColour());
#endif
      //    widgetPanel->SetVirtualSize(wSize);
      padxpady->SetSize(wScrollSize);
      padxpady->SetMinSize(wScrollSize);
      //Just Enable scrollBars if scrolling is necessary
      if (scrolled) {
        padxpady->SetScrollbars(gdlSCROLL_RATE, gdlSCROLL_RATE, wSize.x / gdlSCROLL_RATE, wSize.y / gdlSCROLL_RATE);
        padxpady->ShowScrollbars(wxSHOW_SB_ALWAYS, wxSHOW_SB_ALWAYS);
      }
    } else {
      widgetPanel = new wxScrolled<wxPanel>(parent, widgetID, wOffset, wxDefaultSize); 
      theWxContainer = widgetPanel;
#ifdef GDL_DEBUG_WIDGETS_COLORIZE
      widgetPanel->SetBackgroundColour(RandomWxColour());
#endif
//        widgetPanel->SetVirtualSize(wSize);
      widgetPanel->SetSize(wScrollSize);
      widgetPanel->SetMinSize(wScrollSize);
      //Just Enable scrollBars if scrolling is necessary
      if (scrolled) {
        widgetPanel->SetScrollbars(gdlSCROLL_RATE, gdlSCROLL_RATE, wSize.x / gdlSCROLL_RATE, wSize.y / gdlSCROLL_RATE);
        widgetPanel->ShowScrollbars(wxSHOW_SB_ALWAYS, wxSHOW_SB_ALWAYS);
      }
    }
    
    theWxWidget = widgetPanel;
  }


//define inside sizer.  
  widgetSizer = AddABaseSizer(ncols, nrows, grid, space);
  if (widgetSizer) widgetPanel->SetSizer(widgetSizer); 
  else if (scrolled) { //no col or row but scroll: need a sizer otherwise the content size will not be propagated to the scrolled window.
    widgetSizer=new wxBoxSizer(wxHORIZONTAL);
    widgetPanel->SetSizer(widgetSizer);
    nrows=1; //do not forget as there would be problems with alignment checks afterwards
  }

  wxSizer* parentSizer = parent->GetSizer();
  if (parentSizer) parentSizer->Add(static_cast<wxWindow*>(theWxContainer), ALLOWSTRETCH, wxALL | widgetAlignment(), gdlSPACE);
  }

 void GDLWidgetBase::SetWidgetSize(DLong sizex, DLong sizey) 
{
  START_CHANGESIZE_NOEVENT
  
  wxWindow* me=dynamic_cast<wxWindow*>(theWxWidget);
  if (me==NULL) {
#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputStderr( ).Printf( _T( "GDLWidget::SetSize of non wxWindow %s\n"),this->GetWidgetName().c_str());
#endif
  return;
  }
  //a graphic window cannot be larger than the base widget it is in, if the base widget size has been fixed
  wxSize currentVirtualSize=me->GetVirtualSize();
  wxSize currentSize=me->GetSize();
#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputStderr( ).Printf( _T( "GDLWidgetBase::SetSize currentSize=%d,%d\n"),currentSize.x,currentSize.y);
#endif
  
  //passed zeroes = your best is to keep best size (in cache, value defined at creation)
  if (sizex < 1) sizex = scrolled?currentSize.x:currentVirtualSize.x;
  if (sizey < 1) sizey = scrolled?currentSize.y:currentVirtualSize.y;
  wxSize theSize=wxSize(sizex,sizey);
  if (scrolled) theSize += wxSize(gdlSCROLL_HEIGHT_X, gdlSCROLL_WIDTH_Y);
  me->SetSize( theSize );
  me->SetMinSize( theSize );
  
  UpdateGui();

  END_CHANGESIZE_NOEVENT

#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputStderr( ).Printf( _T( "GDLWidgetBase::SetSize %d,%d (%s)\n"),sizex,sizey,this->GetWidgetName().c_str());
#endif
}
/*********************************************************/
// special TOP WIDGET_BASE
/*********************************************************/

GDLWidgetTopBase::GDLWidgetTopBase( EnvT* e, ULong eventFlags_,
bool mapWid,
WidgetIDT& mBarIDInOut, bool modal_, DLong frame_attr,
DLong col, DLong row,
int exclusiveMode_,
bool floating,
const DString& resource_name, const DString& rname_mbar,
const DString& title_,
const DString& display_name,
int xpad_, int ypad_,
DLong x_scroll_size, DLong y_scroll_size, bool grid_layout, long children_alignment, int space_)
: GDLWidgetBase( GDLWidget::NullID, e, eventFlags_, mapWid, col, row, exclusiveMode_, resource_name, rname_mbar, title_, display_name, xpad_, ypad_, x_scroll_size, y_scroll_size, grid_layout, children_alignment, space_)
, mbarID(mBarIDInOut)
, UseInteractiveEvents(false) //TopBases are blocking by default
, modal(modal_)
, realized(false)
{
  
  // All bases can receive events: EV_CONTEXT, EV_KBRD_FOCUS, EV_TRACKING

//on wxMAC, frame will not appear if style is not exactly this (!!!)
#ifdef __WXMAC__
long style = (wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxRESIZE_BORDER | wxCAPTION | wxCLOSE_BOX);
#else
  long style=wxDEFAULT_FRAME_STYLE; //|wxFRAME_TOOL_WINDOW would be to NOT get focus, but IDL gives focus to widgets. See behaviour of 'P' (photometry) while using ATV (atv.pro). 
  if (frame_attr) {
    style=0;
    if (!(frame_attr & 1)) style |= (wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxRESIZE_BORDER| wxCAPTION);
    if (!(frame_attr & 2)) style |=wxSYSTEM_MENU;
    if (!(frame_attr & 4)) style |=wxCAPTION;
    if (!(frame_attr & 8)) style |=wxCLOSE_BOX;
    //style 16 not supported.
  }
  if (floating) {
    if (this->GetWidget(groupLeader)==NULL) e->Throw("FLOATING top level bases must have a group leader specified.");
    style |= wxFRAME_TOOL_WINDOW|wxSTAY_ON_TOP ; //wxFRAME_FLOAT_ON_PARENT will destroy the parent widget!!
  }
#endif
  // Top Level Base Widget: can receive special events: tlb_size, tlb_move, tlb_icon and tlb_kill. cannot be framed.
  wxString titleWxString;
  if (title_.size() < 1) titleWxString = wxString("GDL");
  else titleWxString = wxString(title_.c_str(), wxConvUTF8);
  if (floating) {
    wxWindow* parent=static_cast<wxWindow*>(this->GetWidget(groupLeader)->GetWxWidget());
    //make it really above parent:
    wxRect x=parent->GetScreenRect();
    if (wOffset.x < 0) wOffset.x =x.x+x.width/2;
    if (wOffset.y < 0) wOffset.y =x.y+x.height/2;
  }
  topFrame = new gdlwxFrame(NULL, this, widgetID, titleWxString, wOffset, wxDefaultSize, style, modal);

#ifdef __WXMAC__
//does not work.
  ProcessSerialNumber psn = {0, kCurrentProcess};
  ProcessApplicationTransformState state = kProcessTransformToForegroundApplication;
  OSStatus osxErr = TransformProcessType(&psn, state);
#endif

#ifdef GDL_DEBUG_WIDGETS_COLORIZE
  topFrame->SetBackgroundColour(wxColour(0x81, 0x46, 0xf1)); //violet
#endif

  //Base Frame inherits default font -- then each widget will possibly get its own font when /FONT is possible    
  topFrame->SetFont(defaultFont);
  //add icon
  topFrame->SetIcon(wxgdlicon);

  wxSizer* tfSizer=new wxBoxSizer(wxVERTICAL);
  topFrame->SetSizer(tfSizer);
  if (mbarID != 0) {
#if PREFERS_MENUBAR
    GDLWidgetMenuBar* mBar = new GDLWidgetMenuBar(widgetID, e);
    mbarID = mBar->GetWidgetID();
    mBarIDInOut = mbarID;
    wxMenuBar* me = dynamic_cast<wxMenuBar*> (mBar->GetWxWidget());
#ifdef __WXMAC__
//  me->Append(me->OSXGetAppleMenu(), "Apple");
//    if (!osxErr) me->Append(new wxMenu("does not work\n"));
#endif
    if (me) topFrame->SetMenuBar(me);
    else cerr << "Warning: GDLWidgetBase::GDLWidgetBase: Non-existent menubar widget!\n";
#else    

#ifdef __WXMAC__
    wxPanel* p = new wxPanel(topFrame, wxID_ANY);
    GDLWidgetMenuBar* mBar = new GDLWidgetMenuBar(p, widgetID, e);
	wxBoxSizer* mbs=new wxBoxSizer(wxHORIZONTAL);
	tfSizer->Add(p);
#else
    GDLWidgetMenuBar* mBar = new GDLWidgetMenuBar(topFrame, widgetID, e);
#endif
    mbarID = mBar->GetWidgetID();
    mBarIDInOut = mbarID;
#endif
  }
  CreateBase(topFrame); //define widgetPanel, widgetSizer, theWxWidget and theWxContainer.
  //it is the FRAME that manage all events. Here we dedicate particularly the tlb_* events:
  // note that we have the choice for Size Event Handler for Frames, but need to change also is widgets.cpp
  if (eventFlags & GDLWidget::EV_SIZE) this->AddToDesiredEvents(wxEVT_SIZE, gdlSIZE_EVENT_HANDLER, topFrame); //this->AddToDesiredEvents(wxEVT_SIZE, wxSizeEventHandler(GDLFrame::OnSizeWithTimer),topFrame); 
  if (eventFlags & GDLWidget::EV_MOVE) this->AddToDesiredEvents(wxEVT_MOVE, wxMoveEventHandler(gdlwxFrame::OnMove), topFrame);
  if (eventFlags & GDLWidget::EV_ICONIFY) this->AddToDesiredEvents(wxEVT_ICONIZE, wxIconizeEventHandler(gdlwxFrame::OnIconize), topFrame);
  if (eventFlags & GDLWidget::EV_KILL) {
    this->AddToDesiredEvents(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(gdlwxFrame::OnCloseFrame), topFrame);
  } else {
    this->AddToDesiredEvents(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(gdlwxFrame::OnUnhandledCloseFrame), topFrame);
  }
 this->AddToDesiredEvents(wxEVT_SHOW_REQUEST, wxCommandEventHandler(gdlwxFrame::OnShowRequest), topFrame);
 this->AddToDesiredEvents(wxEVT_HIDE_REQUEST, wxCommandEventHandler(gdlwxFrame::OnShowRequest), topFrame);
 //UpdateGui(); REALIZE_IF_NEEDED //no use here
 //this->Realize(false);
}

inline int GDLWidgetTopBase::widgetAlignment() {
    long myAlign = alignment;
    //there is no parent for topBase...
//    if (myAlign == gdlwALIGN_NOT) {
//      myAlign = this->GetMyParentBaseWidget()->getChildrenAlignment();
//    }
    //wxALIGN_LEFT and TOP is in fact wxALIGN_NOT as this is the default alignment
    if (myAlign == gdlwALIGN_NOT) return wxALIGN_CENTER;
    //left is top by default and right is bottom. So define left as top and remove top if bottom, etc.
    //ignore sets that do not concern the current layout (vetrtical or horizontal)
    if (this->IsVertical()) { //col=1 left, center, right
      if (myAlign & gdlwALIGN_CENTER) return wxALIGN_CENTER_HORIZONTAL;
      if (myAlign & gdlwALIGN_LEFT) return wxALIGN_LEFT;
      if (myAlign & gdlwALIGN_RIGHT) return wxALIGN_RIGHT;
      if (myAlign & gdlwALIGN_TOP) return wxALIGN_LEFT;
      if (myAlign & gdlwALIGN_BOTTOM) return wxALIGN_RIGHT;
    } else if (this->IsHorizontal()) { //row=1
      if (myAlign & gdlwALIGN_CENTER) return wxALIGN_CENTER_VERTICAL;
      if (myAlign & gdlwALIGN_TOP) return wxALIGN_TOP;
      if (myAlign & gdlwALIGN_BOTTOM) return wxALIGN_BOTTOM;
      if (myAlign & gdlwALIGN_LEFT) return wxALIGN_TOP;
      if (myAlign & gdlwALIGN_RIGHT) return wxALIGN_BOTTOM;
    }
    return 0;
  }

GDLWidgetTopBase::~GDLWidgetTopBase() {
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetTopBase(" << widgetID << ")" << std::endl;
#endif
  topFrame->UnblockIfModal();
  topFrame->NullGDLOwner();
  //what if delay_destroy ?
  //IMPORTANT: xunregister TLB if was managed 
  if (this->GetManaged()) CallEventPro("XUNREGISTER", new DLongGDL(widgetID)); //XUNREGISTER defined in XMANAGER.PRO

  //send RIP 
  // create GDL event struct
  DStructGDL* ev = new DStructGDL("*TOPLEVEL_DESTROYED*");
  ev->InitTag("ID", DLongGDL(widgetID));
  ev->InitTag("TOP", DLongGDL(widgetID));
  ev->InitTag("HANDLER", DLongGDL(0));
  if (this->IsUsingInteractiveEventLoop()) {
#ifdef GDL_DEBUG_WIDGETS
            wxMessageOutputStderr().Printf(_T("~GDLWidgetTopBase InteractiveEventQueue.Push: %d\n"),widgetID);
#endif
   InteractiveEventQueue.PushFront(ev); // push front (will be handled next)
   } else {
#ifdef GDL_DEBUG_WIDGETS
           wxMessageOutputStderr().Printf(_T("~GDLWidgetTopBase BlockingEventQueue.Push: %d\n"),widgetID);
#endif           
    BlockingEventQueue.PushFront(ev); // push front (will be handled next)
 }
}
/*********************************************************/
// Context Menu pseudo-base
/*********************************************************/
GDLWidgetContextBase::GDLWidgetContextBase( WidgetIDT parentID, EnvT* e, ULong eventFlags_,
bool mapWid,
DLong col, DLong row,
int exclusiveMode_,
const DString& resource_name, const DString& rname_mbar,
const DString& title_,
const DString& display_name,
int xpad_, int ypad_,
DLong x_scroll_size, DLong y_scroll_size, bool grid_layout, long children_alignment, int space_)
: GDLWidgetMenu(parentID, e, new DStringGDL(title_), eventFlags_, NULL)
{
GDLWidget* gdlParent = GetWidget( parentID );
    assert( gdlParent != NULL);
    gdlParent->AddToFollowers(widgetID); //very important, the transient must be deleted if the associated widget is deleted!
    wxWindow* parentWindow=dynamic_cast<wxWindow*>(gdlParent->GetWxWidget());
    assert( parentWindow != NULL);
  //get default value: a menu. May be NULL here
    wxMenu *menu = new wxMenu();
  theWxContainer = menu; //is not a window
  theWxWidget = menu;
  buttonType = MENU;

//  UpdateGui(); REALIZE_IF_NEEDED; //no use (TBC)
}

GDLWidgetContextBase::~GDLWidgetContextBase() {
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetContextBase(" << widgetID << ")" << std::endl;
#endif
}
/*********************************************************/
// Normal WIDGET_BASE but Tab hosted
/*********************************************************/
GDLWidgetTabbedBase::GDLWidgetTabbedBase(WidgetIDT parentID, EnvT* e, ULong eventFlags_,
  bool mapWid,
  DLong col, DLong row,
  int exclusiveMode_,
  const DString& resource_name, const DString& rname_mbar,
  const DString& title_,
  const DString& display_name,
  int xpad_, int ypad_,
  DLong x_scroll_size, DLong y_scroll_size, bool grid_layout, long children_alignment, int space_)
: GDLWidgetBase(parentID, e, eventFlags_, mapWid, col, row, exclusiveMode_, resource_name, rname_mbar, title_, display_name, xpad_, ypad_, x_scroll_size, y_scroll_size, grid_layout, children_alignment, space_)
, myPage(-1)
{
  //for historical logic reasons, there must be a container panel child of the tab, that will contain the actual widget, a panel.
  //the container is a scrolledwindow OR just a panel.
  //it would have been simpler to use just a wxScrolled<> for each container, and have it scroll or not depending on GDLWidget::scrolled.
  //this would accessorily eliminate the use of 'scrollSizer' and 'scrollPanel'.
  //but this has consequences in all the creation/deletion chain, to be done in time.
  GDLWidgetTab* parent = static_cast<GDLWidgetTab*> (GetWidget(parentID));
  assert(parent != NULL);

  gdlNotebook* parentTab = dynamic_cast<gdlNotebook*> (parent->GetWxWidget());
  assert(parentTab != NULL);
  wxString titleWxString = wxString(title_.c_str(), wxConvUTF8);
  if (nrows < 1 && ncols < 1 && frameWidth < 1) frameWidth=1; //set framewidth (temporary) in this case to get good result
  CreateBase(parentTab);
  wxWindow* w=static_cast<wxWindow*>(theWxContainer); //defined in CreateBase.
  myPage=parentTab->GetPageCount();
  parentTab->InsertPage(myPage, w, titleWxString);
 
//  UpdateGui();
  REALIZE_IF_NEEDED
}
/*********************************************************/
// Normal WIDGET_BASE

/*********************************************************/
GDLWidgetNormalBase::GDLWidgetNormalBase(WidgetIDT parentID, EnvT* e, ULong eventFlags_,
  bool mapWid,
  DLong col, DLong row,
  int exclusiveMode_,
  const DString& resource_name, const DString& rname_mbar,
  const DString& title_,
  const DString& display_name,
  int xpad_, int ypad_,
  DLong x_scroll_size, DLong y_scroll_size, bool grid_layout, long children_alignment, int space_)
: GDLWidgetBase(parentID, e, eventFlags_, mapWid, col, row, exclusiveMode_, resource_name, rname_mbar, title_, display_name, xpad_, ypad_, x_scroll_size, y_scroll_size, grid_layout, children_alignment, space_) {
  // If parent base widget exists ....
  GDLWidget* gdlParent = GetWidget(parentID);
  assert(gdlParent != NULL);
  wxWindow* wxParent = dynamic_cast<wxWindow*> (GetParentPanel());
  assert(wxParent != NULL);
  CreateBase(wxParent);
  
//  UpdateGui();
  REALIZE_IF_NEEDED
}

DStructGDL* GDLWidgetBase::GetGeometry(wxRealPoint fact) {
  if (!this->IsRealized()) this->OnRealize();
  int panel_xoff = 0;
  int panel_yoff = 0;

  int ixsize = 0, iysize = 0, iscr_xsize = 0, iscr_ysize = 0;
  DFloat xsize = 0;
  DFloat ysize = 0;
  DFloat scr_xsize = 0;
  DFloat scr_ysize = 0;
  DFloat xoffset = 0;
  DFloat yoffset = 0;
  DFloat margin = 0;
  //reported size includes xpad and ypad.
  int ixpad=this->getXPad();
  int iypad=this->getYPad();
  DStructGDL* ex = new DStructGDL("WIDGET_GEOMETRY");
 
  wxWindow* w = dynamic_cast<wxWindow*> (theWxContainer);
  if (w == NULL) return ex;
  if (frameWidth > 0 && (ixpad>0 || iypad >0)) w=w->GetChildren()[0]; //padxpady is the first child of frame
  wxRect z = w->GetRect();

  margin=frameWidth;
  GDLWidgetBase* container = static_cast<GDLWidgetBase*> (this->GetMyParent());
  if (container == NULL) { //TLB
//    w->GetScreenPosition(&panel_xoff, &panel_yoff);
    xoffset = panel_xoff+margin;
    yoffset = panel_yoff+margin;
  } else {
    xoffset = z.x+margin;
    yoffset = z.y+margin;
  }
  iscr_xsize = z.width-2*margin;
  iscr_ysize = z.height-2*margin;
  wxSize s = w->GetClientSize();
  ixsize = s.x-2*margin;
  iysize = s.y-2*margin;
  //size is in pixels, pass in requested units (1.0 default)
  xsize = ixsize / fact.x;
  ysize = iysize / fact.y;
  scr_xsize = iscr_xsize / fact.x;
  scr_ysize = iscr_ysize / fact.y;
  xoffset /= fact.x;
  yoffset /= fact.y;
  margin /= fact.x;

  ex->InitTag("XOFFSET", DFloatGDL(xoffset));
  ex->InitTag("YOFFSET", DFloatGDL(yoffset));
  ex->InitTag("XSIZE", DFloatGDL(xsize));
  ex->InitTag("YSIZE", DFloatGDL(ysize));
  ex->InitTag("SCR_XSIZE", DFloatGDL(scr_xsize));
  ex->InitTag("SCR_YSIZE", DFloatGDL(scr_ysize));
  ex->InitTag("MARGIN", DFloatGDL(margin));

  //reported size includes xpad and ypad.
  DFloat xpad=ixpad/ fact.x;
  DFloat ypad=iypad/ fact.y;
  DFloat space=this->getSpace()/ fact.x;
  ex->InitTag( "XPAD", DFloatGDL( xpad ) );
  ex->InitTag( "YPAD", DFloatGDL( ypad ) );
  ex->InitTag( "SPACE", DFloatGDL( space) );
  return ex;
}

//DStructGDL* GDLWidgetBase::GetGeometry(wxRealPoint fact)
//{
//  if (!this->IsRealized()) this->Realize(true,false);//necessary if a geometry request is done previous to the command widget_control,xxx,,/Realize !
//  //the only way to get accurate positions seems to get the screen position of the Panel
//  //in which the window is (the Panel is the useful part of the Base) and substract them from the
//  //screen position of the siwget itself. And get the margin (=frame) size if any.
//  //for Bases, one has also to retrieve xpad,ypad and space;
//  int panel_xoff = 0;
//  int panel_yoff = 0;
//
//  GDLWidgetBase* container = static_cast<GDLWidgetBase*> (this->GetMyParent());
//  if (container != NULL) { //not TLB
//    wxPanel* p = dynamic_cast<wxPanel*> (container->GetPanel());
//    p->GetScreenPosition(&panel_xoff, &panel_yoff);
//  }
//  int ixsize = 0, iysize = 0, iscr_xsize = 0, iscr_ysize = 0;
//  int widget_xoff = 0;
//  int widget_yoff = 0;
//  DFloat xsize = 0;
//  DFloat ysize = 0;
//  DFloat scr_xsize = 0;
//  DFloat scr_ysize = 0;
//  DFloat xoffset = 0;
//  DFloat yoffset = 0;
//  DFloat margin = frameWidth;
//  
//  wxWindow* test = dynamic_cast<wxWindow*> (theWxWidget);
//
//  if (test != NULL) {
//    //offsets. easy.
//    test->GetScreenPosition(&widget_xoff, &widget_yoff);
//    xoffset = widget_xoff - panel_xoff;
//    yoffset = widget_yoff - panel_yoff;
//
//    test->GetSize(&iscr_xsize, &iscr_ysize); //contains frame size if framed
//
//    ixsize = iscr_xsize; //with scrollbars, if any, and frames
//    iysize = iscr_ysize;
//
//    //reported size includes xpad and ypad.
//    DFloat xpad=this->getXPad()/ fact.x;
//    DFloat ypad=this->getYPad()/ fact.y;
//    DFloat space=this->getSpace()/ fact.x;
//    //offset is including xpad,ypad
//    xoffset -= xpad;
//    yoffset -= ypad;
//    //size is in pixels, pass in requested units (1.0 default)
//    xsize = ixsize / fact.x + 2*margin;
//    ysize = iysize / fact.y + 2*margin;
//    scr_xsize = iscr_xsize / fact.x;
//    scr_ysize = iscr_ysize / fact.y;
//    xoffset /= fact.x;
//    yoffset /= fact.y;
//    margin /= fact.x;
//  }
//
//  DStructGDL* ex = new DStructGDL("WIDGET_GEOMETRY");
//  ex->InitTag("XOFFSET", DFloatGDL(xoffset));
//  ex->InitTag("YOFFSET", DFloatGDL(yoffset));
//  ex->InitTag("XSIZE", DFloatGDL(xsize));
//  ex->InitTag("YSIZE", DFloatGDL(ysize));
//  ex->InitTag("SCR_XSIZE", DFloatGDL(scr_xsize));
//  ex->InitTag("SCR_YSIZE", DFloatGDL(scr_ysize));
//  ex->InitTag("MARGIN", DFloatGDL(margin));
//  ex->InitTag( "XPAD", DFloatGDL( xpad ) );
//  ex->InitTag( "YPAD", DFloatGDL( ypad ) );
//  ex->InitTag( "SPACE", DFloatGDL( space) );
//  return ex;
//}

//// called from event handling thread
//// sends a destroy event for itself
//void GDLWidgetTopBase::SelfDestroy()
//{
//  assert( parentID == NullID );
//  // create GDL event struct
//  DStructGDL* ev = new DStructGDL( "*WIDGET_DESTROY*" );
//  ev->InitTag( "ID", DLongGDL( widgetID ) );
//  ev->InitTag( "TOP", DLongGDL( widgetID ) );
//  ev->InitTag( "HANDLER", DLongGDL( 0 ) );
//  if ( this->GetXmanagerActiveCommand( ) || !this->GetManaged() ){
//    readlineEventQueue.PushFront( ev ); // push front (will be handled next)
//  } else {
//    eventQueue.PushFront( ev ); // push front (will be handled next)
//  }
//}

  void GDLWidgetBase::mapBase(bool val){
    wxWindow* me=dynamic_cast<wxWindow*>(theWxContainer);
    if (me) me->Show(val); else {cerr<<"Warning: GDLWidgetBase::mapBase(): Non-existent widget!\n"; return;}
  }
  void GDLWidgetTopBase::mapBase(bool val){
    if (this->IsRealized()) topFrame->Show(val);
  }  
  void GDLWidgetTabbedBase::mapBase(bool val){
    //Note: it is impossible to reproduce the IDL behaviour (tab is present but void) with wxWidgets unless doing ridiculously complicated things.
    wxWindow* me=dynamic_cast<wxWindow*>(theWxContainer);
    if (me) me->Show(val); else {cerr<<"Warning: GDLWidgetTabbedBase::mapBase(): Non-existent widget!\n"; return;}
  }
  // Order in which widgets are plotted is different btw wxWindow and IDL if one does not do the following tricks:
  void GDLWidgetBase::DoReorderColWidgets(int code,int style, int border) {
  int nchild = this->NChildren();
  //get children list...
  DLongGDL* gdlwList= GetChildrenList();
  // set up window list, get existing code, style and border for each of the children
  std::vector<int> proportionlist;
  std::vector<wxWindow*> windowlist;
  std::vector<int> flaglist;
  std::vector<int> borderlist;
  for (int i = 0; i < nchild; ++i) {
    GDLWidget* g=GetWidget((*gdlwList)[i]);
    wxWindow* w=static_cast<wxWindow*>(g->GetWxContainer());
    windowlist.push_back(w);
    wxSizerItem* s=widgetSizer->GetItem(w);
    if (s==NULL) { //this is a new window that has not yet been added (the widget is Realized) : use passed values
      proportionlist.push_back(code);
      flaglist.push_back(style);
      borderlist.push_back(border);
    } else { //use existing values
      proportionlist.push_back(s->GetProportion());
      flaglist.push_back(s->GetFlag());
      borderlist.push_back(s->GetBorder());
    }
  }
  GDLDelete(gdlwList);
  
  //compute new positions (align items along column 1, then 2 etc... when wxWidget will insert them col1,col2,col1,col2, etc..)
  int local_nchild=nchild;
  // method for COL=n , row is growing
  //This should be called only for /COL => nrows is 0 
  int local_ncols = ncols; 
  int local_nrows = local_nchild / local_ncols;
  if (local_nrows * local_ncols < local_nchild) local_nrows++;
  //how many filled cols?
  int reste = (local_ncols * local_nrows) - local_nchild;
  int filledcols = local_ncols - reste;

  //we create the corresponding flexigridsizer, destroy the previous one, assign back to widgetSizer
  wxFlexGridSizer* newWidgetSizer = new wxFlexGridSizer(local_nrows, local_ncols, border, border);
  widgetPanel->SetSizer(newWidgetSizer, true);
  widgetPanel->SetAutoLayout(true);
  if (grid) {
    newWidgetSizer->SetFlexibleDirection(wxHORIZONTAL);
    newWidgetSizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_ALL);
  }
  widgetSizer = newWidgetSizer;

  //so the first 'filledcols' will be filled, the rest will have (apparently) one row less.
  //we have to insert a place holder in the list of childrens at each index corresponding to the last row, starting at the first incomplete column. 
  //widgets indexes are k=icol*local_nrows+jrow, compute a new indexlist where empty indexes are those jumped


  int list[local_ncols * local_nrows];
  int i = 0;
  int k = 0;
  int avoidrow = (reste == 0) ? local_nrows : local_nrows - 1;
  int lastfilledcol = (reste == 0) ? local_ncols : filledcols - 1;
  //  list of indexes as displayed in colums (idl) with -1 when there should be a void.
  for (int icol = 0; icol < local_ncols; icol++) {
    for (int jrow = 0; jrow < local_nrows; jrow++) {
      if ((icol > lastfilledcol) && (jrow == avoidrow)) {
        list[k++] = -1;
      } else {
        list[k++] = i++;
      }
    }
  }

  //find transposed values: read list in transposed order, use a local_nrows stride.
  int stride = local_nrows;
  int outer_stride = local_ncols*local_nrows;
  for (k = 0, i = 0; i < nchild; ++i) {
    widgetSizer->Add(windowlist[list[k]], proportionlist[list[k]],flaglist[list[k]],borderlist[list[k]]);
    k += stride;
    if (k >= outer_stride) {
      k -= outer_stride;
      k++;
    }
  }
}

void GDLWidgetBase::ReorderWidgets()
{
  // NULL widget Sizer means 1) no row no col was asked for (so, no sizer) or, if col>1, we have to create the sizer here and add children in specific order.
  // do *not* forget to give back the sizer pointer instead of the previous NULL to the base widget!
  if (widgetSizer == NULL) return;
  if (ncols > 1) DoReorderColWidgets(0,0,space); //need to reorder widget for /COL only
}

void GDLWidgetBase::ReorderForANewWidget(wxWindow* w, int code,int style, int border)
{
  if (widgetSizer == NULL) return;
  if (ncols > 1 ) DoReorderColWidgets(code, style, border); //need to reorder widget for /COL only
  else widgetSizer->Add(w, code, style, border ); // just add widget in sizer for /ROW
}

void GDLWidgetBase::SetButtonWidget( bool onOff)
{
  int nchild = this->NChildren();
  if ( exclusiveMode == BGEXCLUSIVE && nchild>1 ) throw GDLException("Exclusive base must have exactly one set button.");
  if ( exclusiveMode == BGNORMAL) return;
  DLongGDL* gdlwList = GetChildrenList();
  for (int i = 0; i < nchild; ++i) {
    GDLWidget* g = GetWidget((*gdlwList)[i]);
    if (g->IsButton()) g->SetButtonWidget(onOff);
  }
  GDLDelete(gdlwList);
}
/*********************************************************/
// for WIDGET_TAB
/*********************************************************/
GDLWidgetTab::GDLWidgetTab(WidgetIDT p, EnvT* e, ULong eventFlags_, DLong location, DLong multiline)
: GDLWidgetContainer(p, e, eventFlags_) {

  scrolled = false; //TAB has no Scrolled.
  this->setFont();
  GDLWidget* gdlParent = GetWidget(parentID);
  widgetPanel = GetParentPanel();
  widgetSizer = GetParentSizer();

  long style = wxNB_TOP;
  if (location == 1) style = wxNB_BOTTOM;
  if (location == 2) style = wxNB_LEFT;
  if (location == 3) style = wxNB_RIGHT;

  if (multiline != 0)
    style |= wxNB_MULTILINE; //works only for WINDOWS.

#include "start_eventual_frame.incpp"
  gdlNotebook * notebook;
  notebook = new gdlNotebook(widgetPanel, widgetID, wOffset, computeWidgetSize(), style);
  theWxContainer = theWxWidget = notebook;
//  notebook->SetPadding(wxSize(0, 0));
  END_ADD_EVENTUAL_FRAME
  TIDY_WIDGET(gdlBORDER_SPACE)
  //  UpdateGui();
  REALIZE_IF_NEEDED
  this->AddToDesiredEvents(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler(gdlwxFrame::OnPageChanged), notebook);
}
GDLWidgetTab::~GDLWidgetTab() {
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetTab(" << widgetID << ")" << std::endl;
#endif
}

BaseGDL* GDLWidgetTab::GetTabNumber(){
  gdlNotebook * notebook=dynamic_cast<gdlNotebook*>(theWxWidget);
  assert( notebook != NULL);
  return new DIntGDL(notebook->GetPageCount());
}

BaseGDL* GDLWidgetTab::GetTabCurrent(){
  gdlNotebook * notebook=dynamic_cast<gdlNotebook*>(theWxWidget);
  assert( notebook != NULL);
  return new DIntGDL(notebook->GetSelection());
}

void GDLWidgetTab::SetTabCurrent(int val){
  gdlNotebook * notebook=dynamic_cast<gdlNotebook*>(theWxWidget);
  assert( notebook != NULL);
  if (val<notebook->GetPageCount()){
//   notebook->GetPage(val)->Raise();    
   notebook->ChangeSelection(val);
  }
}

BaseGDL* GDLWidgetTab::GetTabMultiline(){
  gdlNotebook * notebook=dynamic_cast<gdlNotebook*>(theWxWidget);
  assert( notebook != NULL);
  return new DIntGDL(notebook->GetExtraStyle()&wxNB_MULTILINE);
}
//special as gdlNotebook DOES NOT RECOMPILE ITS SIZE BEFORE REALIZATION.
void GDLWidgetTab::OnRealize(){
  GDLWidgetContainer::OnRealize();
}


/*********************************************************/
// for WIDGET_TABLE
/*********************************************************/
////overrides method to label the columns & lines
//  wxString wxGridTableBase::GetRowLabelValue( int row )
//{
//    wxString s;
//
//    // RD: Starting the rows at zero confuses users,
//    // no matter how much it makes sense to us geeks.
//    // GD: So IDL and GDL are for geeks.
//    s << row ;
//
//    return s;
//}
//  wxString wxGridTableBase::GetColLabelValue( int col )
//{
//    wxString s;
//    s << col ;
//
//    return s;
//}

GDLWidgetTable::GDLWidgetTable( WidgetIDT p, EnvT* e,
DByteGDL* alignment_,
DStringGDL* amPm_,
DByteGDL* backgroundColor_,
DByteGDL* foregroundColor_,
DStringGDL* columnLabels_,
int majority_,
DLongGDL* columnWidth_,
DStringGDL* daysOfWeek_,
bool disjointSelection_,
DByteGDL* editable_,
DStringGDL* format_,
//bool ignoreAccelerators_,
DStringGDL* month_,
bool noColumnHeaders_,
bool noRowHeaders_,
bool resizeableColumns_,
bool resizeableRows_,
DLongGDL* rowHeights_,
DStringGDL* rowLabels_,
//DLong tabMode_,
BaseGDL* value_,
DLong xScrollSize_,
DLong yScrollSize_,
DStringGDL* valueAsStrings,
DULong eventFlags_
)
: GDLWidget( p, e, value_, eventFlags_ )
, table_alignment( alignment_ )
, amPm( amPm_ )
, backgroundColor( backgroundColor_ )
, foregroundColor( foregroundColor_ )
, columnLabels( columnLabels_ )
, majority ( majority_ )
, columnWidth( columnWidth_ )
, daysOfWeek( daysOfWeek_ )
, disjointSelection( disjointSelection_ )
, editable( editable_)
, format( format_ )
//, ignoreAccelerators( ignoreAccelerators_ )
, month( month_ )
, noColumnHeaders( noColumnHeaders_ )
, noRowHeaders( noRowHeaders_ )
, resizeableColumns( resizeableColumns_ )
, resizeableRows( resizeableRows_ )
, rowHeights( rowHeights_ )
, rowLabels( rowLabels_ )
//, tabMode( tabMode_ )
, x_scroll_size_columns( xScrollSize_ )
, y_scroll_size_rows( yScrollSize_)
, updating(false) {
  GDLWidget* gdlParent = GetWidget(parentID);
  widgetPanel = GetParentPanel();
  widgetSizer = GetParentSizer();

#include "start_eventual_frame.incpp"   //Widget_TABLE

  //at this stage, valueAsStrings is OK dim 1 or 2 BUT vVALUE MAY BE NULL!
  SizeT numRows, numCols;
  if (valueAsStrings->Rank() == 1) {
	numRows = 1;
	numCols = valueAsStrings->Dim(0); //lines
  } else {
	numRows = valueAsStrings->Dim(1);
	numCols = valueAsStrings->Dim(0);
  }

  //if wSize is not explicit, it must now be explicit:
  SizeT grid_nrows = numRows;
  if (wSize.y <= 0) wSize.y = numRows;
  else grid_nrows = wSize.y;
  SizeT grid_ncols = numCols;
  if (wSize.x <= 0) wSize.x = numCols;
  else grid_ncols = wSize.x;

  wxGridGDL *grid = new wxGridGDL(widgetPanel, widgetID, wxDefaultPosition, wxDefaultSize);
  //important:set wxWidget here. (fonts)
  theWxContainer = theWxWidget = grid;
  // important: use adapted font for further sizes & shapes. Define font for labels AND  cells.
  this->setFont();
  // editor & renderer:
  grid->SetDefaultEditor(new wxGridGDLCellTextEditor(10));
  //	grid->SetDefaultRenderer(new wxGridGDLCellStringRenderer);
  //Alignment
  bool hasAlignment = (table_alignment != NULL);
  if (hasAlignment) {
	if (table_alignment->N_Elements() == 1) { //singleton case
	  switch ((*table_alignment)[0]) {
	  case 0:
		grid->SetDefaultCellAlignment(wxALIGN_LEFT, wxALIGN_CENTRE);
		break;
	  case 1:
		grid->SetDefaultCellAlignment(wxALIGN_CENTRE, wxALIGN_CENTRE);
		break;
	  case 2:
		grid->SetDefaultCellAlignment(wxALIGN_RIGHT, wxALIGN_CENTRE);
	  }
	  hasAlignment = false; //otherwise will be treated cell by cell below!
	}
  }

  //Single Background Colour
  bool isBackgroundColored = (backgroundColor != NULL);
  if (isBackgroundColored) { //one value set for all?
	if (backgroundColor->N_Elements() == 3) {
	  grid->SetDefaultCellBackgroundColour(wxColour((*backgroundColor)[0], (*backgroundColor)[1], (*backgroundColor)[2]));
	  isBackgroundColored = false;
	}
  }
  //Single Text Colour
  bool isForegroundColored = (foregroundColor != NULL);
  if (isForegroundColored) { //one value set for all?
	if (foregroundColor->N_Elements() == 3) {
	  grid->SetDefaultCellTextColour(wxColour((*foregroundColor)[0], (*foregroundColor)[1], (*foregroundColor)[2]));
	  isForegroundColored = false;
	}
  }
  //No column Headers
  if (noColumnHeaders) grid->SetColLabelSize(0);
  //No row Headers
  if (noRowHeaders) grid->SetRowLabelSize(0);
  //end General Setup
  int selmode = wxGrid::wxGridSelectCells; //wxWidgets's modes do not reflect IDL's. We trick the selections according to modes in eventhandler.
  if (!resizeableColumns) grid->DisableDragColSize();
  if (!resizeableRows) grid->DisableDragRowSize();
  grid->CreateGrid(grid_nrows, grid_ncols, static_cast<wxGrid::wxGridSelectionModes> (selmode));
  // Set grid cell contents as strings. Note that there may be less or more cells than valueAsStrings, due to possibly different wSize.x,wSize.y :

  for (int ival = 0, i = 0, k = 0; i < numRows; ++i, ++ival) for (int jval = 0, j = 0; j < numCols; ++j, ++jval) {
	  if (i < grid_nrows && j < grid_ncols) grid->SetCellValue(i, j, wxString(((*valueAsStrings)[k]).c_str(), wxConvUTF8));
	  k++;
	}
  for (SizeT j = numRows; j < grid_nrows; ++j) for (SizeT i = 0; i < grid_ncols; ++i) grid->SetCellBackgroundColour(j, i, *wxLIGHT_GREY);
  for (SizeT j = numRows; j < grid_nrows; ++j) for (SizeT i = 0; i < grid_ncols; ++i) grid->SetReadOnly(j, i);
  for (SizeT i = numCols; i < grid_ncols; ++i) for (SizeT j = 0; j < grid_nrows; ++j) grid->SetCellBackgroundColour(j, i, *wxLIGHT_GREY);
  for (SizeT i = numCols; i < grid_ncols; ++i) for (SizeT j = 0; j < grid_nrows; ++j) grid->SetReadOnly(j, i);
  //General Editability
  bool isEditable = (editable != NULL);
  bool hasEditableList = false;
  if (isEditable) {
	if (editable->N_Elements() == 1) { //singleton case
	  if ((*editable)[0] == 0) {
		grid->EnableEditing(false);
		isEditable = false;
	  } else {
		grid->EnableEditing(true);
	  } // fully editable
	} else {
	  hasEditableList = true;
	  //Editability cell by cell
	  //take too long as soon as table has a more than a few elements. Fixme!
	  {
		SizeT k = 0;
		for (SizeT irow = 0; irow < grid_nrows; ++irow) for (SizeT icol = 0; icol < grid_ncols; ++icol) {
			if ((*editable)[k] == 0) grid->SetReadOnly(irow, icol);
			else grid->SetReadOnly(irow, icol, false);
			++k;
			if (k == editable->N_Elements()) k = 0;
		  }
	  }
	}
  } else grid->EnableEditing(false);

  //colors per element
  if (isBackgroundColored) this->DoBackgroundColor();
  if (isForegroundColored) this->DoForegroundColor();
  if (columnWidth != NULL) this->DoColumnWidth();
  if (rowHeights != NULL) this->DoRowHeights();
  //treat other alignment cases.
  if (hasAlignment) this->DoAlign();
  // default values for row and col labels in IDL are 0...n so we always redefine column and row as it is not the same default as wxWidgets.
  this->DoColumnLabels();
  this->DoRowLabels();

  //get back on sizes. Do we enforce some size or scroll_size, in columns/rows:
  int currentColLabelHeight = grid->GetColLabelSize();
  int currentRowLabelWidth = grid->GetRowLabelSize();
  ////ClientSize
  //int clientsizex=currentRowLabelWidth;
  //int clientsizey=currentColLabelHeight; 
  //for (SizeT i=0; i< numCols ; ++i) clientsizex+=(i<grid_ncols)?grid->GetColSize(i):grid->GetDefaultColSize(); 
  //for (SizeT j=0; j< numRows ; ++j) clientsizey+=(j<grid_nrows)?grid->GetRowHeight(j):grid->GetDefaultRowSize();
  //wxSize wClientSize=wxSize(clientsizex,clientsizey);
  //
  ////Not Useful?
  //grid->SetInitialSize(wClientSize);

  //Size will be WindowSize, as wSize is in columns.
  wxSize windowSize(100, 100); //default value, avoids warnings in GTK
  //wxSize in columns, transform to units
  int xsize_columns = wSize.x;
  int ysize_rows = wSize.y;
  if (xsize_columns > 0) { //defined
	int windowsizex = currentRowLabelWidth;
	for (SizeT i = 0; i < xsize_columns; ++i) windowsizex += (i < grid_ncols) ? grid->GetColSize(i) : grid->GetDefaultColSize();
	windowSize = wxSize(windowsizex, windowSize.y);
  }
  if (ysize_rows > 0) { //defined
	int windowsizey = currentColLabelHeight;
	for (SizeT j = 0; j < ysize_rows; ++j) windowsizey += (j < grid_nrows) ? grid->GetRowHeight(j) : grid->GetDefaultRowSize();
	windowSize = wxSize(windowSize.x, windowsizey);
  }

  //Scrolled?
  x_scroll_size_columns = (x_scroll_size_columns > grid_ncols) ? grid_ncols : x_scroll_size_columns;
  y_scroll_size_rows = (y_scroll_size_rows > grid_nrows) ? grid_nrows : y_scroll_size_rows;
  if (x_scroll_size_columns > 0) { //scroll size is in columns
	scrolled = true;
  }
  if (y_scroll_size_rows > 0) { //rows
	scrolled = true;
  }
  if (scrolled) { //size to pass is given by scrolled 
	if (x_scroll_size_columns > 0) { //scroll size is in columns
	  int scrollsizex = currentRowLabelWidth + gdlSCROLL_WIDTH_Y;
	  for (SizeT i = 0; i < x_scroll_size_columns; ++i) scrollsizex += (i < grid_ncols) ? grid->GetColSize(i) : grid->GetDefaultColSize();
	  windowSize = wxSize(scrollsizex, windowSize.y);
	}
	if (y_scroll_size_rows > 0) { //rows
	  int scrollsizey = currentColLabelHeight + gdlSCROLL_HEIGHT_X;
	  for (SizeT j = 0; j < y_scroll_size_rows; ++j) scrollsizey += (j < grid_nrows) ? grid->GetRowHeight(j) : grid->GetDefaultRowSize();
	  windowSize = wxSize(windowSize.x, scrollsizey);
	}
  }

  //scr_xsize or ysize: replaces value in windowSize:
  if (wScreenSize.x > 0) windowSize = wxSize(wScreenSize.x, windowSize.y);
  if (wScreenSize.y > 0) windowSize = wxSize(windowSize.x, wScreenSize.y);

  //wxGrid IS a scrolled window
  if (scrolled) {
	//  int visibx=x_scroll_size>0?wxSHOW_SB_ALWAYS:wxSHOW_SB_NEVER;
	//  int visiby=y_scroll_size>0?wxSHOW_SB_ALWAYS:wxSHOW_SB_NEVER;
	grid->ShowScrollbars(wxSHOW_SB_ALWAYS, wxSHOW_SB_ALWAYS);
  }
  grid->SetScrollLineX(grid->GetColSize(0));
  grid->SetScrollLineY(grid->GetRowHeight(0));
  grid->SetSize(windowSize);
  grid->SetMinSize(windowSize);
#if wxCHECK_VERSION(3,1,4)
  grid->SetDefaultCellFitMode(wxGridFitMode::Clip());
#endif
  END_ADD_EVENTUAL_FRAME
  TIDY_WIDGET(gdlBORDER_SPACE);
  this->AddToDesiredEvents(wxEVT_GRID_COL_SIZE, wxGridSizeEventHandler(wxGridGDL::OnTableColResizing), grid);
  this->AddToDesiredEvents(wxEVT_GRID_ROW_SIZE, wxGridSizeEventHandler(wxGridGDL::OnTableRowResizing), grid);
  this->AddToDesiredEvents(wxEVT_GRID_RANGE_SELECT, wxGridRangeSelectEventHandler(wxGridGDL::OnTableRangeSelection), grid);
  this->AddToDesiredEvents(wxEVT_GRID_CELL_LEFT_CLICK, wxGridEventHandler(wxGridGDL::OnTableCellSelection), grid);
  //      this->AddToDesiredEvents( wxEVT_GRID_CELL_CHANGING,wxGridEventHandler(wxGridGDL::OnTextChanging),grid);
  this->AddToDesiredEvents(wxEVT_GRID_CELL_CHANGED, wxGridEventHandler(wxGridGDL::OnTextChanged), grid);
  // UpdateGui();
  REALIZE_IF_NEEDED
  grid->SelectBlock(0, 0, 0, 0, FALSE); //hyperimportant
}

std::vector<int> GDLWidgetTable::GetSortedSelectedRowsOrColsList(DLongGDL* selection, bool doCol = true) {
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert(grid != NULL);
  std::vector<int> list;
  if (selection == NULL) {
	std::vector<wxPoint> wxList = grid->GetSelectedDisjointCellsList();
	if (wxList.empty()) return list;
	std::vector<wxPoint>::iterator iPoint;
	std::vector<int> allRowsOrCols;
	std::vector<int>::iterator iter;
	for (iPoint = wxList.begin(); iPoint != wxList.end(); ++iPoint) {
	  if (doCol) allRowsOrCols.push_back((*iPoint).y);
	  else allRowsOrCols.push_back((*iPoint).x);
	}
	std::sort(allRowsOrCols.begin(), allRowsOrCols.end());
	int theRowOrCol = -1;
	for (iter = allRowsOrCols.begin(); iter != allRowsOrCols.end(); ++iter) {
	  if ((*iter) != theRowOrCol) {
		theRowOrCol = (*iter);
		list.push_back(theRowOrCol);
	  }
	}
  } else { //use the passed selection, mode-dependent:
	//check selection is OK otherwise will throw);
	bool ok = GetValidTableSelection(selection);
	if (disjointSelection) { //pairs lists
	  std::vector<int> allRowsOrCols;
	  std::vector<int>::reverse_iterator riter;
	  //find concerned rows or cols
	  int index = 0;
	  for (SizeT n = 0, l = 0; n < MAX(selection->Dim(1), 1); ++n) {
		index = (*selection)[l++];
		if (doCol) allRowsOrCols.push_back(index);
		index = (*selection)[l++];
		if (!doCol) allRowsOrCols.push_back(index);
	  }
	  std::sort(allRowsOrCols.begin(), allRowsOrCols.end());
	  int theRowOrCol = -1;
	  for (riter = allRowsOrCols.rbegin(); riter != allRowsOrCols.rend(); ++riter) {
		if ((*riter) != theRowOrCol) {
		  theRowOrCol = (*riter);
		  list.push_back(theRowOrCol);
		}
	  }
	} else { //4 values, cols are contiguous, easy.
	  int colTL = (*selection)[0];
	  int colBR = (*selection)[2];
	  int count = colBR - colTL + 1;
	  for (auto i = colTL; i < colBR + 1; ++i) list.push_back(i);
	}
  }
  return list;
}

DStringGDL* CallStringFunction(BaseGDL* val, BaseGDL* format) {
  int stringIx = LibFunIx("GDL_TOSTRING");
  EnvT *newEnv = new EnvT(NULL, libFunList[stringIx]);
  Guard<EnvT> guard(newEnv);
  newEnv->SetNextPar(val); // pass as local
  if (format != NULL) newEnv->SetKeyword("FORMAT", format);
  DStringGDL* s = static_cast<DStringGDL*> (lib::gdl_tostring_fun(newEnv));
  guard.release();
  for (auto i = 0; i < s->N_Elements(); ++i) StrTrim((*s)[i]);
  s->SetDim(val->Dim()); //necessary
  return s;
}

DStringGDL* ConvertValueToStringArray(BaseGDL* &value, DStringGDL* format, const int majority) {
  DStringGDL* valueAsStrings;
  if (value->Type() == GDL_STRUCT) {
	//convert to STRING
	DStructGDL *input = static_cast<DStructGDL*> (value);
	SizeT nTags = input->NTags();
	SizeT nEl = input->N_Elements();
	stringstream os;
	input->ToStreamRaw(os);
	SizeT dims[2];
	dims[0] = nTags;
	dims[1] = nEl;
	dimension dim(dims, 2);
	valueAsStrings = new DStringGDL(dim, BaseGDL::NOZERO);
	valueAsStrings->FromStream(os); //simple as that if we manage the dimensions and transpose accordingly....
	if (majority == GDLWidgetTable::COLUMN_MAJOR) valueAsStrings = static_cast<DStringGDL*> (valueAsStrings->Transpose(NULL));
  } else if (value->Type() == GDL_STRING) {
	//no conversion and besides, prevent problem with NULL strings
	 valueAsStrings = static_cast<DStringGDL*>(value)->Dup();
  } else {
	//convert to STRING using FORMAT.
	valueAsStrings = CallStringFunction(value, format);
  }
  return valueAsStrings;
}

template <typename T>
void RemoveGDLCols(BaseGDL* outvarGDL, BaseGDL* invarGDL, std::vector<int> &list) {
  T* invar = static_cast<T*> (invarGDL);
  T* outvar = static_cast<T*> (outvarGDL);
  for (auto j = 0, l = 0, k = 0; j < invar->Dim(1); ++j) {
	for (auto i = 0; i < invar->Dim(0); ++i, ++l) {
	  bool go = true;
	  for (auto n = 0; n < list.size(); ++n) if (i == list[n]) {
		  go = false;
		  break;
		}
	  if (go) (*outvar)[k++] = (*invar)[l];
	}
  }
}

template <typename T>
void AddGDLCols(BaseGDL* outvarGDL, BaseGDL* invarGDL, int iCol, int num, bool before) {
  T* invar = static_cast<T*> (invarGDL);
  T* outvar = static_cast<T*> (outvarGDL);
  for (auto j = 0, l = 0, k = 0; j < invar->Dim(1); ++j) {
	for (auto i = 0; i < invar->Dim(0); ++i, ++l) {
	  bool dojump = (i == iCol);
	  if (dojump && before) {
		k += num;
		dojump = false;
	  }
	  (*outvar)[k++] = (*invar)[l];
	  if (dojump) k += num;
	}
  }
}

template <typename T>
void RemoveGDLRows(BaseGDL* outvarGDL, BaseGDL* invarGDL, std::vector<int> &list) {
  T* invar = static_cast<T*> (invarGDL);
  T* outvar = static_cast<T*> (outvarGDL);
  for (auto j = 0, l = 0, k = 0; j < invar->Dim(1); ++j) {
	bool go = true;
	for (auto n = 0; n < list.size(); ++n) if (j == list[n]) {
		go = false;
		break;
	  }
	if (go) {
	  for (auto i = 0; i < invar->Dim(0); ++i, ++l) {
		(*outvar)[k++] = (*invar)[l];
	  }
	} else l += invar->Dim(0);
  }
}

void RemoveStructGDLColOrRows(DStructGDL* outvar, const DStructGDL* invar, std::vector<int> &list) {
  SizeT nTags = invar->NTags();
  SizeT nEl = invar->N_Elements();
  for (SizeT right = 0, left = 0; right < nEl; ++right) {
	bool go = true;
	for (auto n = 0; n < list.size(); ++n) if (right == list[n]) {
		go = false;
		break;
	  }
	if (go) {
	  for (SizeT t = 0; t < nTags; ++t) {
		const BaseGDL* from = invar->GetTag(t, right);
		outvar->GetTag(t, left)->InitFrom((*from));
	  }
	  left++;
	}
  }
}

void AddStructGDLColOrRows(DStructGDL* outvar, DStructGDL* invar, int jRow, int num, bool before) {
  SizeT nTags = invar->NTags();
  SizeT nElIn = invar->N_Elements();
  SizeT nElOut = outvar->N_Elements();
  for (auto j = 0, l = 0; j < nElIn; ++j) {
	bool dojump = (j == jRow);
	if (dojump && before) {
	  l++;
	  dojump = false;
	}
	for (SizeT t = 0; t < nTags; ++t) {
	  const BaseGDL* from = invar->GetTag(t, j);
	  outvar->GetTag(t, l)->InitFrom((*from));
	}
	l++;
	if (dojump) l++;
  }
}

template <typename T>
void AddGDLRows(BaseGDL* outvarGDL, BaseGDL* invarGDL, int jRow, int num, bool before) {
  T* invar = static_cast<T*> (invarGDL);
  T* outvar = static_cast<T*> (outvarGDL);
  for (auto j = 0, l = 0, k = 0; j < invar->Dim(1); ++j) {
	bool dojump = (j == jRow);
	if (dojump && before) {
	  k += (num * outvar->Dim(0));
	  dojump = false;
	}
	for (auto i = 0; i < invar->Dim(0); ++i, ++l) (*outvar)[k++] = (*invar)[l];
	if (dojump) k += (num * outvar->Dim(0));
  }
}

// Forces value of any GDL type at offset 'inew' to replace any other GDL type at offset 'iold', whatever the type of both operands.

void GDLWidgetTable::UpdatevValues(BaseGDL* oldPassedValue, SizeT iold, BaseGDL* newPassedValue, SizeT inew) {
  BaseGDL* oldValue = oldPassedValue; //intermediary pointer
  BaseGDL* newValue = newPassedValue; //intermediary pointer
  DType editableType = oldValue->Type();
  if (editableType == GDL_STRUCT) {
	DStructGDL* s = static_cast<DStructGDL*> (oldValue);
	SizeT dim0 = s->Dim(0);
	SizeT ntags = s->NTags();
	SizeT iTag = 0;
	SizeT iElem = 0;
	if (this->GetMajority() == GDLWidgetTable::COLUMN_MAJOR) {
	  iElem = iold / ntags;
	  iTag = iold - iElem*ntags;
	} else {
	  iElem = iold / dim0;
	  iTag = iold - iElem*dim0;
	}
	if ((iTag > s->NTags() - 1) || (iold > dim0 - 1)) return; //unexistent table entry
	oldValue = s->GetTag(iTag, iElem);
	iold = 0;
	editableType = oldValue->Type();
	//convert newValue to this tag type (absolutely not optimized!)
	newValue = newPassedValue->Dup()->Convert2(editableType);
  }
  switch (editableType) {
  case GDL_STRING:
	UPDATE_VVALUE_HELPER(DStringGDL, DString)
	break;
  case GDL_BYTE:
	UPDATE_VVALUE_HELPER(DByteGDL, DByte)
	break;
  case GDL_INT:
	UPDATE_VVALUE_HELPER(DIntGDL, DInt)
	break;
  case GDL_LONG:
	UPDATE_VVALUE_HELPER(DLongGDL, DLong)
	break;
  case GDL_FLOAT:
	UPDATE_VVALUE_HELPER(DFloatGDL, DFloat)
	break;
  case GDL_DOUBLE:
	UPDATE_VVALUE_HELPER(DDoubleGDL, DDouble)
	break;
  case GDL_COMPLEX:
	UPDATE_VVALUE_HELPER(DComplexGDL, DComplex)
	break;
  case GDL_COMPLEXDBL:
	UPDATE_VVALUE_HELPER(DComplexDblGDL, DComplexDbl)
	break;
  case GDL_UINT:
	UPDATE_VVALUE_HELPER(DUIntGDL, DUInt)
	break;
  case GDL_ULONG:
	UPDATE_VVALUE_HELPER(DULongGDL, DULong)
	break;
  case GDL_LONG64:
	UPDATE_VVALUE_HELPER(DLong64GDL, DLong64)
	break;
  case GDL_ULONG64:
	UPDATE_VVALUE_HELPER(DULong64GDL, DULong64)
	break;
  default:
	cerr << "Unhandled Table Type, please report!" << endl;
  }
}

BaseGDL* GetNewTypedBaseGDLColRemoved(BaseGDL* var, std::vector<int> &list) {
  int n_remove = list.size();
  SizeT newdims[2];
  newdims[0] = var->Dim(0) - n_remove;
  newdims[1] = var->Dim(1);
  dimension dim(newdims, 2);
  BaseGDL* res = NULL;
  switch (var->Type()) {
  case GDL_STRING:
	res = new DStringGDL(dim, BaseGDL::NOZERO);
	RemoveGDLCols<DStringGDL>(res, var, list);
	break;
  case GDL_BYTE:
	res = new DByteGDL(dim, BaseGDL::NOZERO);
	RemoveGDLCols<DByteGDL>(res, var, list);
	break;
  case GDL_INT:
	res = new DIntGDL(dim, BaseGDL::NOZERO);
	RemoveGDLCols<DIntGDL>(res, var, list);
	break;
  case GDL_LONG:
	res = new DLongGDL(dim, BaseGDL::NOZERO);
	RemoveGDLCols<DLongGDL>(res, var, list);
	break;
  case GDL_FLOAT:
	res = new DFloatGDL(dim, BaseGDL::NOZERO);
	RemoveGDLCols<DFloatGDL>(res, var, list);
	break;
  case GDL_DOUBLE:
	res = new DDoubleGDL(dim, BaseGDL::NOZERO);
	RemoveGDLCols<DDoubleGDL>(res, var, list);
	break;
  case GDL_COMPLEX:
	res = new DComplexGDL(dim, BaseGDL::NOZERO);
	RemoveGDLCols<DComplexGDL>(res, var, list);
	break;
  case GDL_COMPLEXDBL:
	res = new DComplexDblGDL(dim, BaseGDL::NOZERO);
	RemoveGDLCols<DComplexDblGDL>(res, var, list);
	break;
  case GDL_UINT:
	res = new DUIntGDL(dim, BaseGDL::NOZERO);
	RemoveGDLCols<DUIntGDL>(res, var, list);
	break;
  case GDL_ULONG:
	res = new DULongGDL(dim, BaseGDL::NOZERO);
	RemoveGDLCols<DULongGDL>(res, var, list);
	break;
  case GDL_LONG64:
	res = new DLong64GDL(dim, BaseGDL::NOZERO);
	RemoveGDLCols<DLong64GDL>(res, var, list);
	break;
  case GDL_ULONG64:
	res = new DULong64GDL(dim, BaseGDL::NOZERO);
	RemoveGDLCols<DULong64GDL>(res, var, list);
	break;
  case GDL_STRUCT:
  {
	DStructGDL* s = static_cast<DStructGDL*> (var);
	DStructDesc* desc = s->Desc();
	DStructGDL* struct_res = new DStructGDL(desc, dimension(s->Dim(0) - n_remove));
	RemoveStructGDLColOrRows(struct_res, s, list);
	return struct_res;
	break;
  }
  default:
	cerr << "Unhandled Table Type, please report!" << endl;
	return NULL; //signal error
  }
  return res;
}

BaseGDL* GetNewTypedBaseGDLColAdded(BaseGDL* var, int iCol, int n_add, bool before) {
  SizeT newdims[2];
  newdims[0] = var->Dim(0) + n_add;
  newdims[1] = var->Dim(1);
  dimension dim(newdims, 2);
  BaseGDL* res = NULL;
  switch (var->Type()) {
  case GDL_STRING:
	res = new DStringGDL(dim, BaseGDL::ZERO);
	AddGDLCols<DStringGDL>(res, var, iCol, n_add, before);
	break;
  case GDL_BYTE:
	res = new DByteGDL(dim, BaseGDL::ZERO);
	AddGDLCols<DByteGDL>(res, var, iCol, n_add, before);
	break;
  case GDL_INT:
	res = new DIntGDL(dim, BaseGDL::ZERO);
	AddGDLCols<DIntGDL>(res, var, iCol, n_add, before);
	break;
  case GDL_LONG:
	res = new DLongGDL(dim, BaseGDL::ZERO);
	AddGDLCols<DLongGDL>(res, var, iCol, n_add, before);
	break;
  case GDL_FLOAT:
	res = new DFloatGDL(dim, BaseGDL::ZERO);
	AddGDLCols<DFloatGDL>(res, var, iCol, n_add, before);
	break;
  case GDL_DOUBLE:
	res = new DDoubleGDL(dim, BaseGDL::ZERO);
	AddGDLCols<DDoubleGDL>(res, var, iCol, n_add, before);
	break;
  case GDL_COMPLEX:
	res = new DComplexGDL(dim, BaseGDL::ZERO);
	AddGDLCols<DComplexGDL>(res, var, iCol, n_add, before);
	break;
  case GDL_COMPLEXDBL:
	res = new DComplexDblGDL(dim, BaseGDL::ZERO);
	AddGDLCols<DComplexDblGDL>(res, var, iCol, n_add, before);
	break;
  case GDL_UINT:
	res = new DUIntGDL(dim, BaseGDL::ZERO);
	AddGDLCols<DUIntGDL>(res, var, iCol, n_add, before);
	break;
  case GDL_ULONG:
	res = new DULongGDL(dim, BaseGDL::ZERO);
	AddGDLCols<DULongGDL>(res, var, iCol, n_add, before);
	break;
  case GDL_LONG64:
	res = new DLong64GDL(dim, BaseGDL::ZERO);
	AddGDLCols<DLong64GDL>(res, var, iCol, n_add, before);
	break;
  case GDL_ULONG64:
	res = new DULong64GDL(dim, BaseGDL::ZERO);
	AddGDLCols<DULong64GDL>(res, var, iCol, n_add, before);
	break;
  case GDL_STRUCT:
  {
	DStructGDL* s = static_cast<DStructGDL*> (var);
	DStructDesc* desc = s->Desc();
	DStructGDL* struct_res = new DStructGDL(desc, dimension(s->Dim(0) + n_add));
	AddStructGDLColOrRows(struct_res, s, iCol, n_add, before);
	return struct_res;
	break;
  }
  default:
	cerr << "Unhandled Table Type, please report!" << endl;
	return NULL; //signal error
  }
  return res;
}

BaseGDL* GetNewTypedBaseGDLRowRemoved(BaseGDL* var, std::vector<int> &list) {
  int n_remove = list.size();
  SizeT newdims[2];
  newdims[0] = var->Dim(0);
  newdims[1] = var->Dim(1) - n_remove;
  dimension dim(newdims, 2);
  BaseGDL* res = NULL;
  switch (var->Type()) {
  case GDL_STRING:
	res = new DStringGDL(dim, BaseGDL::NOZERO);
	RemoveGDLRows<DStringGDL>(res, var, list);
	break;
  case GDL_BYTE:
	res = new DByteGDL(dim, BaseGDL::NOZERO);
	RemoveGDLRows<DByteGDL>(res, var, list);
	break;
  case GDL_INT:
	res = new DIntGDL(dim, BaseGDL::NOZERO);
	RemoveGDLRows<DIntGDL>(res, var, list);
	break;
  case GDL_LONG:
	res = new DLongGDL(dim, BaseGDL::NOZERO);
	RemoveGDLRows<DLongGDL>(res, var, list);
	break;
  case GDL_FLOAT:
	res = new DFloatGDL(dim, BaseGDL::NOZERO);
	RemoveGDLRows<DFloatGDL>(res, var, list);
	break;
  case GDL_DOUBLE:
	res = new DDoubleGDL(dim, BaseGDL::NOZERO);
	RemoveGDLRows<DDoubleGDL>(res, var, list);
	break;
  case GDL_COMPLEX:
	res = new DComplexGDL(dim, BaseGDL::NOZERO);
	RemoveGDLRows<DComplexGDL>(res, var, list);
	break;
  case GDL_COMPLEXDBL:
	res = new DComplexDblGDL(dim, BaseGDL::NOZERO);
	RemoveGDLRows<DComplexDblGDL>(res, var, list);
	break;
  case GDL_UINT:
	res = new DUIntGDL(dim, BaseGDL::NOZERO);
	RemoveGDLRows<DUIntGDL>(res, var, list);
	break;
  case GDL_ULONG:
	res = new DULongGDL(dim, BaseGDL::NOZERO);
	RemoveGDLRows<DULongGDL>(res, var, list);
	break;
  case GDL_LONG64:
	res = new DLong64GDL(dim, BaseGDL::NOZERO);
	RemoveGDLRows<DLong64GDL>(res, var, list);
	break;
  case GDL_ULONG64:
	res = new DULong64GDL(dim, BaseGDL::NOZERO);
	RemoveGDLRows<DULong64GDL>(res, var, list);
	break;
  case GDL_STRUCT:
  {
	DStructGDL* s = static_cast<DStructGDL*> (var);
	DStructDesc* desc = s->Desc();
	DStructGDL* struct_res = new DStructGDL(desc, dimension(s->Dim(0) - n_remove));
	RemoveStructGDLColOrRows(struct_res, s, list);
	return struct_res;
	break;
  }
  default:
	cerr << "Unhandled Table Type, please report!" << endl;
	return NULL; //signal error
  }
  return res;
}

BaseGDL* GetNewTypedBaseGDLRowAdded(BaseGDL* var, int iRow, int n_add, bool before) {
  SizeT newdims[2];
  newdims[0] = var->Dim(0);
  newdims[1] = var->Dim(1) + n_add;
  dimension dim(newdims, 2);
  BaseGDL* res = NULL;
  switch (var->Type()) {
  case GDL_STRING:
	res = new DStringGDL(dim, BaseGDL::ZERO);
	AddGDLRows<DStringGDL>(res, var, iRow, n_add, before);
	break;
  case GDL_BYTE:
	res = new DByteGDL(dim, BaseGDL::ZERO);
	AddGDLRows<DByteGDL>(res, var, iRow, n_add, before);
	break;
  case GDL_INT:
	res = new DIntGDL(dim, BaseGDL::ZERO);
	AddGDLRows<DIntGDL>(res, var, iRow, n_add, before);
	break;
  case GDL_LONG:
	res = new DLongGDL(dim, BaseGDL::ZERO);
	AddGDLRows<DLongGDL>(res, var, iRow, n_add, before);
	break;
  case GDL_FLOAT:
	res = new DFloatGDL(dim, BaseGDL::ZERO);
	AddGDLRows<DFloatGDL>(res, var, iRow, n_add, before);
	break;
  case GDL_DOUBLE:
	res = new DDoubleGDL(dim, BaseGDL::ZERO);
	AddGDLRows<DDoubleGDL>(res, var, iRow, n_add, before);
	break;
  case GDL_COMPLEX:
	res = new DComplexGDL(dim, BaseGDL::ZERO);
	AddGDLRows<DComplexGDL>(res, var, iRow, n_add, before);
	break;
  case GDL_COMPLEXDBL:
	res = new DComplexDblGDL(dim, BaseGDL::ZERO);
	AddGDLRows<DComplexDblGDL>(res, var, iRow, n_add, before);
	break;
  case GDL_UINT:
	res = new DUIntGDL(dim, BaseGDL::ZERO);
	AddGDLRows<DUIntGDL>(res, var, iRow, n_add, before);
	break;
  case GDL_ULONG:
	res = new DULongGDL(dim, BaseGDL::ZERO);
	AddGDLRows<DULongGDL>(res, var, iRow, n_add, before);
	break;
  case GDL_LONG64:
	res = new DLong64GDL(dim, BaseGDL::ZERO);
	AddGDLRows<DLong64GDL>(res, var, iRow, n_add, before);
	break;
  case GDL_ULONG64:
	res = new DULong64GDL(dim, BaseGDL::ZERO);
	AddGDLRows<DULong64GDL>(res, var, iRow, n_add, before);
	break;
  case GDL_STRUCT:
  {
	DStructGDL* s = static_cast<DStructGDL*> (var);
	DStructDesc* desc = s->Desc();
	DStructGDL* struct_res = new DStructGDL(desc, dimension(s->Dim(0) + n_add));
	AddStructGDLColOrRows(struct_res, s, iRow, n_add, before);
	return struct_res;
	break;
  }
  default:
	cerr << "Unhandled Table Type, please report!" << endl;
	return NULL; //signal error
  }
  return res;
}

//returns true if selection is invalid, but it will have throwed before returning anyway.

bool GDLWidgetTable::GetValidTableSelection(DLongGDL* &selection) {
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert(grid != NULL);

  if (selection == NULL) return false;
  else if (selection->Rank() == 0) {
	if ((*selection)[0] == 0) return false;
	else { //create selection from current selected areas
	  selection = GetSelection(true); //will throw if selection is bad.
	  return true;
	}
  }

  //there is a passed selection. Check and throw if necessary.
  if (disjointSelection) {
	if (selection->Dim(0) != 2) ThrowGDLException("USE_TABLE_SELECT (disjoint mode) Array must have dimensions of (2, N) ");
  } else {
	if (selection->Rank() != 1 || selection->Dim(0) != 4) ThrowGDLException("USE_TABLE_SELECT (continuous mode) Array must have dimensions of (4) ");
  }
  int ncolsmax = this->GetDataNCols() - 1;
  int nrowsmax = this->GetDataNRows() - 1;
  if (disjointSelection) { //pairs lists
	for (auto j = 0; j < MAX(selection->Dim(1), 1); ++j) {
	  DLong col = (*selection)[j * 2 + 0];
	  DLong row = (*selection)[j * 2 + 1];
	  if (col < 0 || col > ncolsmax) ThrowGDLException("USE_TABLE_SELECT value out of range.");
	  if (row < 0 || row > nrowsmax) ThrowGDLException("USE_TABLE_SELECT value out of range.");
	}
  } else { //4 values
	DLong val = (*selection)[0];
	if (val < 0 || val > ncolsmax) ThrowGDLException("USE_TABLE_SELECT value out of range.");
	val = (*selection)[1];
	if (val < 0 || val > nrowsmax) ThrowGDLException("USE_TABLE_SELECT value out of range.");
	val = (*selection)[2];
	if (val < 0 || val > ncolsmax) ThrowGDLException("USE_TABLE_SELECT value out of range.");
	val = (*selection)[3];
	if (val < 0 || val > nrowsmax) ThrowGDLException("USE_TABLE_SELECT value out of range.");
  }
  return true;
}

bool GDLWidgetTable::IsSomethingSelected() {
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert(grid != NULL);
  return grid->IsSomethingSelected();
}

DLongGDL* GDLWidgetTable::GetSelection(bool dothrow) {
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert(grid != NULL);
  SizeT k = 0;
  DLongGDL * sel;
  Guard<BaseGDL> guard;
  int ncolsmax = this->GetDataNCols() - 1;
  int nrowsmax = this->GetDataNRows() - 1;
  if (disjointSelection) { //pairs lists
	std::vector<wxPoint> list = grid->GetSelectedDisjointCellsList();
	if (list.size() < 1) {
	  sel = new DLongGDL(2, BaseGDL::ZERO);
	  sel->Dec();
	} else {
	  SizeT dims[2];
	  dims[0] = 2;
	  dims[1] = list.size();
	  dimension dim(dims, 2);
	  sel = new DLongGDL(dim, BaseGDL::NOZERO);
	  guard.Reset(sel); //remove sel if we throw below
	  for (std::vector<wxPoint>::iterator it = list.begin(); it != list.end(); ++it) {
		if (dothrow) {
		  if ((*it).y < 0 || (*it).y > ncolsmax) ThrowGDLException("USE_TABLE_SELECT value out of range.");
		  if ((*it).x < 0 || (*it).x > nrowsmax) ThrowGDLException("USE_TABLE_SELECT value out of range.");
		}
		(*sel)[k++] = (*it).y;
		(*sel)[k++] = (*it).x;
	  }
	}
  } else { //4 values
	wxGridCellCoordsArray selectionTL = grid->GetSelectionBlockTopLeft();
	wxGridCellCoordsArray selectionBR = grid->GetSelectionBlockBottomRight();
	sel = new DLongGDL(4, BaseGDL::ZERO);
	sel->Dec(); //will return [-1,-1,-1,-1] if nothing selected
	guard.Reset(sel); //remove sel if we throw  below
	if (!selectionTL.IsEmpty() && !selectionBR.IsEmpty()) { //ok with a block...
	  //LEFT TOP BOTTOM RIGHT
	  (*sel)[0] = selectionTL[0].GetCol();
	  (*sel)[1] = selectionTL[0].GetRow();
	  (*sel)[2] = selectionBR[0].GetCol();
	  (*sel)[3] = selectionBR[0].GetRow();
	} else {
	  //try columns, rows, and singletons
	  wxArrayInt selectionRow = grid->GetSelectedRows();
	  wxArrayInt selectionCol = grid->GetSelectedCols();
	  if (selectionRow.GetCount() > 0) {
		(*sel)[0] = 0;
		(*sel)[1] = selectionRow[0];
		(*sel)[2] = grid->GetNumberCols() - 1;
		(*sel)[3] = selectionRow[selectionRow.GetCount() - 1];
	  } else if (selectionCol.GetCount() > 0) {
		(*sel)[0] = selectionCol[0];
		(*sel)[1] = 0;
		(*sel)[2] = selectionCol[selectionCol.GetCount() - 1];
		(*sel)[3] = grid->GetNumberRows() - 1;
	  } else {
		wxGridCellCoordsArray cellSelection = grid->GetSelectedCells();
		if (cellSelection.size() > 0) {
		  int row = cellSelection[0].GetRow();
		  int col = cellSelection[0].GetCol();
		  (*sel)[0] = col;
		  (*sel)[1] = row;
		  (*sel)[2] = col;
		  (*sel)[3] = row;
		}
	  }
	}
	if (dothrow) {
	  DLong val = (*sel)[0];
	  if (val < 0 || val > ncolsmax) ThrowGDLException("USE_TABLE_SELECT value out of range.");
	  val = (*sel)[1];
	  if (val < 0 || val > nrowsmax) ThrowGDLException("USE_TABLE_SELECT value out of range.");
	  val = (*sel)[2];
	  if (val < 0 || val > ncolsmax) ThrowGDLException("USE_TABLE_SELECT value out of range.");
	  val = (*sel)[3];
	  if (val < 0 || val > nrowsmax) ThrowGDLException("USE_TABLE_SELECT value out of range.");
	}
  }
  guard.Release();
  return sel;
}

void GDLWidgetTable::ClearSelection() {
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert(grid != NULL);
  updating = true;
  grid->ClearSelection();
  updating = false;
}

void GDLWidgetTable::DoAlign() {
  if (table_alignment->N_Elements() == 0) {
	return;
  }
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert(grid != NULL);
  int nRows = grid->GetNumberRows();
  int nCols = grid->GetNumberCols();
  SizeT k = 0;
  grid->BeginBatch();
  for (SizeT i = 0; i < nRows; ++i) {
	for (SizeT j = 0; j < nCols; ++j) {
	  switch ((*table_alignment)[k % table_alignment->N_Elements()]) {
	  case 0:
		grid->SetCellAlignment(i, j, wxALIGN_LEFT, wxALIGN_CENTRE);
		break;
	  case 1:
		grid->SetCellAlignment(i, j, wxALIGN_CENTRE, wxALIGN_CENTRE);
		break;
	  case 2:
		grid->SetCellAlignment(i, j, wxALIGN_RIGHT, wxALIGN_CENTRE);
	  }
	  k++;
	  if (table_alignment->N_Elements() > 1) if (k == table_alignment->N_Elements()) break;
	}
	if (table_alignment->N_Elements() > 1) if (k == table_alignment->N_Elements()) break;
  }
  grid->EndBatch();
}

void GDLWidgetTable::DoAlign(DLongGDL* selection) {
  if (table_alignment->N_Elements() == 0) {
	return;
  }
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert(grid != NULL);
  SizeT k = 0;
  grid->BeginBatch();
  if (selection->Rank() == 0) { //use current wxWidgets selection
	std::vector<wxPoint> list = grid->GetSelectedDisjointCellsList();
	for (std::vector<wxPoint>::iterator it = list.begin(); it != list.end(); ++it) {
	  int ali;
	  switch ((*table_alignment)[k % table_alignment->N_Elements()]) {
	  case 0:
		ali = wxALIGN_LEFT;
		break;
	  case 1:
		ali = wxALIGN_CENTRE;
		break;
	  case 2:
		ali = wxALIGN_RIGHT;
	  }
	  grid->SetCellAlignment((*it).x, (*it).y, ali, wxALIGN_CENTRE);
	  k++;
	}
  } else { //use the passed selection, mode-dependent:
	if (disjointSelection) { //pairs lists
	  for (SizeT n = 0, l = 0; n < MAX(selection->Dim(1), 1); ++n) {
		int col = (*selection)[l++];
		int row = (*selection)[l++];
		int ali;
		switch ((*table_alignment)[k % table_alignment->N_Elements()]) {
		case 0:
		  ali = wxALIGN_LEFT;
		  break;
		case 1:
		  ali = wxALIGN_CENTRE;
		  break;
		case 2:
		  ali = wxALIGN_RIGHT;
		}
		grid->SetCellAlignment(row, col, ali, wxALIGN_CENTRE);
		k++;
	  }
	} else { //4 values
	  int colTL = (*selection)[0];
	  int rowTL = (*selection)[1];
	  int colBR = (*selection)[2];
	  int rowBR = (*selection)[3];
	  for (int i = rowTL; i <= rowBR; ++i) for (int j = colTL; j <= colBR; ++j) {
		  int ali;
		  switch ((*table_alignment)[k % table_alignment->N_Elements()]) {
		  case 0:
			ali = wxALIGN_LEFT;
			break;
		  case 1:
			ali = wxALIGN_CENTRE;
			break;
		  case 2:
			ali = wxALIGN_RIGHT;
		  }
		  grid->SetCellAlignment(i, j, ali, wxALIGN_CENTRE);
		  k++;
		}
	}
  }
  grid->EndBatch();

}

void GDLWidgetTable::DoBackgroundColor() {
  SizeT nbColors = backgroundColor->N_Elements();
  if (nbColors == 0) {
	return;
  }
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert(grid != NULL);
  int nRows = grid->GetNumberRows();
  int nCols = grid->GetNumberCols();
  SizeT k = 0;
  grid->BeginBatch();
  for (SizeT i = 0; i < nRows; ++i) for (SizeT j = 0; j < nCols; ++j) //Row by Row, from top.
	{
	  grid->SetCellBackgroundColour(i, j, wxColour((*backgroundColor)[k % nbColors], (*backgroundColor)[k % nbColors + 1], (*backgroundColor)[k % nbColors + 2]));
	  k += 3;
	}
  grid->EndBatch();

}

void GDLWidgetTable::DoBackgroundColor(DLongGDL* selection) {
  SizeT nbColors = backgroundColor->N_Elements();

  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert(grid != NULL);
  grid->BeginBatch();

  SizeT k = 0;

  if (selection->Rank() == 0) { //use current wxWidgets selection
	std::vector<wxPoint> list = grid->GetSelectedDisjointCellsList();
	for (std::vector<wxPoint>::iterator it = list.begin(); it != list.end(); ++it) {
	  grid->SetCellBackgroundColour((*it).x, (*it).y, wxColour((*backgroundColor)[k % nbColors], (*backgroundColor)[k % nbColors + 1], (*backgroundColor)[k % nbColors + 2]));
	  k += 3;
	}
  } else { //use the passed selection, mode-dependent:
	if (disjointSelection) { //pairs lists
	  for (SizeT n = 0, l = 0; n < MAX(selection->Dim(1), 1); ++n) {
		int col = (*selection)[l++];
		int row = (*selection)[l++];
		grid->SetCellBackgroundColour(row, col, wxColour((*backgroundColor)[k % nbColors], (*backgroundColor)[k % nbColors + 1], (*backgroundColor)[k % nbColors + 2]));
		k += 3;
	  }
	} else { //4 values
	  int colTL = (*selection)[0];
	  int rowTL = (*selection)[1];
	  int colBR = (*selection)[2];
	  int rowBR = (*selection)[3];
	  for (int i = rowTL; i <= rowBR; ++i) for (int j = colTL; j <= colBR; ++j) {
		  grid->SetCellBackgroundColour(i, j, wxColour((*backgroundColor)[k % nbColors], (*backgroundColor)[k % nbColors + 1], (*backgroundColor)[k % nbColors + 2]));
		  k += 3;
		}
	}
  }

  grid->EndBatch();

}

void GDLWidgetTable::DoForegroundColor() {
  SizeT nbColors = foregroundColor->N_Elements();
  if (nbColors == 0) {
	return;
  }
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert(grid != NULL);
  int nRows = grid->GetNumberRows();
  int nCols = grid->GetNumberCols();
  SizeT k = 0;
  grid->BeginBatch();
  for (SizeT i = 0; i < nRows; ++i) for (SizeT j = 0; j < nCols; ++j) //Row by Row, from top.
	{
	  grid->SetCellTextColour(i, j, wxColour((*foregroundColor)[k % nbColors], (*foregroundColor)[k % nbColors + 1], (*foregroundColor)[k % nbColors + 2]));
	  k += 3;
	}
  grid->EndBatch();

}

void GDLWidgetTable::DoForegroundColor(DLongGDL* selection) {
  SizeT nbColors = foregroundColor->N_Elements();
  if (nbColors == 0) {
	return;
  }
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert(grid != NULL);
  grid->BeginBatch();

  SizeT k = 0;

  if (selection->Rank() == 0) { //use current wxWidgets selection
	std::vector<wxPoint> list = grid->GetSelectedDisjointCellsList();
	for (std::vector<wxPoint>::iterator it = list.begin(); it != list.end(); ++it) {
	  grid->SetCellTextColour((*it).x, (*it).y, wxColour((*foregroundColor)[k % nbColors], (*foregroundColor)[k % nbColors + 1], (*foregroundColor)[k % nbColors + 2]));
	  k += 3;
	}
  } else { //use the passed selection, mode-dependent:
	if (disjointSelection) { //pairs lists
	  for (SizeT n = 0, l = 0; n < MAX(selection->Dim(1), 1); ++n) {
		int col = (*selection)[l++];
		int row = (*selection)[l++];
		grid->SetCellTextColour(row, col, wxColour((*foregroundColor)[k % nbColors], (*foregroundColor)[k % nbColors + 1], (*foregroundColor)[k % nbColors + 2]));
		k += 3;
	  }
	} else { //4 values
	  int colTL = (*selection)[0];
	  int rowTL = (*selection)[1];
	  int colBR = (*selection)[2];
	  int rowBR = (*selection)[3];
	  for (int i = rowTL; i <= rowBR; ++i) for (int j = colTL; j <= colBR; ++j) {
		  grid->SetCellTextColour(i, j, wxColour((*foregroundColor)[k % nbColors], (*foregroundColor)[k % nbColors + 1], (*foregroundColor)[k % nbColors + 2]));
		  k += 3;
		}
	}
  }

  grid->EndBatch();

}

void GDLWidgetTable::DoColumnLabels() {

  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert(grid != NULL);
  int nCols = grid->GetNumberCols();
  grid->BeginBatch();
  if (columnLabels == NULL || columnLabels->N_Elements() == 0) {
	for (SizeT j = 0; j < nCols; ++j) grid->SetColLabelValue(j, wxString(i2s(j)));
	grid->EndBatch();
	return;
  }
  if (columnLabels->N_Elements() == 1) { //singleton case
	if (static_cast<DString> ((*columnLabels)[0]).length() == 0) {
	  for (SizeT j = 0; j < nCols; ++j) grid->SetColLabelValue(j, wxEmptyString);
	} else {
	  for (SizeT j = 0; j < nCols; ++j) {
		if (j > (columnLabels->N_Elements() - 1)) break;
		grid->SetColLabelValue(j, wxString(static_cast<DString> ((*columnLabels)[j]).c_str(), wxConvUTF8));
	  }
	}
  } else {
	for (SizeT j = 0; j < nCols; ++j) {
	  if (j > (columnLabels->N_Elements() - 1)) break;
	  grid->SetColLabelValue(j, wxString(static_cast<DString> ((*columnLabels)[j]).c_str(), wxConvUTF8));
	}
  }
  grid->EndBatch();

}

void GDLWidgetTable::DoColumnWidth() {
  if (columnWidth->N_Elements() == 0) {
	return;
  }
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert(grid != NULL);
  int nCols = grid->GetNumberCols();
  grid->BeginBatch();
  if (columnWidth->N_Elements() == 1) for (SizeT j = 0; j < nCols; ++j) grid->SetColSize(j, (*columnWidth)[0] * unitConversionFactor.x);
  else {
	for (SizeT j = 0; j < nCols; ++j) {
	  if (j > (columnWidth->N_Elements() - 1)) break;
	  grid->SetColSize(j, (*columnWidth)[j] * unitConversionFactor.x);
	}
  }
  grid->EndBatch();
}

void GDLWidgetTable::DoColumnWidth(DLongGDL* selection) {
  SizeT nbCols = columnWidth->N_Elements();
  if (nbCols == 0) {
	return;
  }
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert(grid != NULL);
  grid->BeginBatch();

  SizeT k = 0;

  if (selection->Rank() == 0) { //use current wxWidgets selection
	std::vector<int> list = GetSortedSelectedRowsOrColsList(selection, true);
	//find concerned cols
	for (int it = 0; it < list.size(); ++it) {
	  grid->SetColSize(list[it], (*columnWidth)[it % nbCols] * unitConversionFactor.x);
	}
  } else { //use the passed selection, mode-dependent:
	if (disjointSelection) { //pairs lists
	  std::vector<int> allCols;
	  std::vector<int>::iterator iter;
	  //find concerned cols
	  for (SizeT n = 0, l = 0; n < MAX(selection->Dim(1), 1); ++n) {
		int col = (*selection)[l++];
		l++;
		allCols.push_back(col);
	  }
	  std::sort(allCols.begin(), allCols.end());
	  int theCol = -1;
	  for (iter = allCols.begin(); iter != allCols.end(); ++iter) {
		if ((*iter) != theCol) {
		  theCol = (*iter);
		  //if index is -1, we want to size the label column instead of the grid columns.
		  if (theCol == -1) grid->SetRowLabelSize((*columnWidth)[k % nbCols] * unitConversionFactor.x);
		  else if (theCol >= 0 && theCol < grid->GetNumberCols()) grid->SetColSize(theCol, (*columnWidth)[k % nbCols] * unitConversionFactor.x);
		  k++;
		}
	  }
	} else { //4 values
	  int colTL = (*selection)[0];
	  int colBR = (*selection)[2];
	  for (int j = colTL; j <= colBR; ++j) {
		//if index is -1, we want to size the label column instead of the grid columns.
		if (j == -1) grid->SetRowLabelSize((*columnWidth)[k % nbCols] * unitConversionFactor.x);
		else if (j >= 0 && j < grid->GetNumberCols()) grid->SetColSize(j, (*columnWidth)[k % nbCols] * unitConversionFactor.x);
		k++;
	  }
	}
  }

  grid->EndBatch();
}

DFloatGDL* GDLWidgetTable::GetColumnWidth(DLongGDL* selection) {
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert(grid != NULL);
  SizeT k = 0;
  int nCols = grid->GetNumberCols();

  if (selection == NULL) {
	DFloatGDL* res = new DFloatGDL(dimension(nCols), BaseGDL::NOZERO);
	for (SizeT j = 0; j < nCols; ++j) (*res)[j] = grid->GetColSize(j);
	return res;
  } else if (selection->Rank() == 0) { //use current wxWidgets selection
	std::vector<int> list = GetSortedSelectedRowsOrColsList(selection, true);
	//find concerned cols
	if (list.size() == 0) return NULL;
	DFloatGDL* res = new DFloatGDL(dimension(list.size()), BaseGDL::NOZERO);
	for (int it = 0; it < list.size(); ++it) {
	  (*res)[it] = grid->GetColSize(list[it]);
	}
	return res;
  } else { //use the passed selection, mode-dependent:
	if (disjointSelection) { //pairs lists
	  std::vector<int> allCols;
	  std::vector<int>::iterator iter;
	  std::vector<int> theCols;
	  //find concerned cols
	  for (SizeT n = 0, l = 0; n < MAX(selection->Dim(1), 1); ++n) {
		int col = (*selection)[l++];
		l++;
		allCols.push_back(col);
	  }
	  std::sort(allCols.begin(), allCols.end());
	  int theCol = -1;
	  for (iter = allCols.begin(); iter != allCols.end(); ++iter) {
		if ((*iter) != theCol) {
		  theCol = (*iter);
		  k++;
		  theCols.push_back(theCol);
		}
	  }
	  //final list:
	  if (theCols.size() == 0) return NULL;
	  DFloatGDL* res = new DFloatGDL(dimension(theCols.size()), BaseGDL::NOZERO);
	  for (iter = theCols.begin(); iter != theCols.end(); ++iter) {
		(*res)[k++] = grid->GetColSize((*iter));
	  }
	  return res;
	} else { //4 values
	  int colTL = (*selection)[0];
	  int colBR = (*selection)[2];
	  int count = colBR - colTL + 1;
	  if (count == 0) return NULL;
	  DFloatGDL* res = new DFloatGDL(dimension(count), BaseGDL::NOZERO);
	  for (int j = colTL; j <= colBR; ++j) {
		(*res)[k++] = grid->GetColSize(j);
	  }
	  return res;
	}
  }
  return new DFloatGDL(0); //to keep compiler happy
}

DFloatGDL* GDLWidgetTable::GetRowHeight(DLongGDL* selection) {
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert(grid != NULL);
  SizeT k = 0;
  int nRows = grid->GetNumberRows();

  if (selection == NULL) {
	DFloatGDL* res = new DFloatGDL(dimension(nRows), BaseGDL::NOZERO);
	for (SizeT i = 0; i < nRows; ++i) (*res)[i] = grid->GetRowSize(i);
	return res;
  } else if (selection->Rank() == 0) { //use current wxWidgets selection
	std::vector<int> list = GetSortedSelectedRowsOrColsList(selection, false);
	//find concerned rows
	if (list.size() == 0) return NULL;
	DFloatGDL* res = new DFloatGDL(dimension(list.size()), BaseGDL::NOZERO);
	for (int it = 0; it < list.size(); ++it) {
	  (*res)[it] = grid->GetRowSize(list[it]);
	}
	return res;
  } else { //use the passed selection, mode-dependent:
	if (disjointSelection) { //pairs lists
	  std::vector<int> allRows;
	  std::vector<int>::iterator iter;
	  std::vector<int> theRows;
	  //find concerned rows
	  for (SizeT n = 0, l = 0; n < MAX(selection->Dim(1), 1); ++n) {
		int row = (*selection)[l++];
		l++;
		allRows.push_back(row);
	  }
	  std::sort(allRows.begin(), allRows.end());
	  int theRow = -1;
	  for (iter = allRows.begin(); iter != allRows.end(); ++iter) {
		if ((*iter) != theRow) {
		  theRow = (*iter);
		  k++;
		  theRows.push_back(theRow);
		}
	  }
	  //final list:
	  if (theRows.size() == 0) return NULL;
	  DFloatGDL* res = new DFloatGDL(dimension(theRows.size()), BaseGDL::NOZERO);
	  for (iter = theRows.begin(); iter != theRows.end(); ++iter) {
		(*res)[k++] = grid->GetRowSize((*iter));
	  }
	  return res;
	} else { //4 values
	  int rowTL = (*selection)[1];
	  int rowBR = (*selection)[3];
	  int count = rowBR - rowTL + 1;
	  if (count == 0) return NULL;
	  DFloatGDL* res = new DFloatGDL(dimension(count), BaseGDL::NOZERO);
	  for (int j = rowTL; j <= rowBR; ++j) {
		(*res)[k++] = grid->GetRowSize(j);
	  }
	}
  }
  return new DFloatGDL(0); //to keep compiler happy
}

void GDLWidgetTable::DoRowHeights() {
  if (rowHeights->N_Elements() == 0) {
	return;
  }
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert(grid != NULL);
  int nRows = grid->GetNumberRows();
  grid->BeginBatch();
  if (rowHeights->N_Elements() == 1) for (SizeT i = 0; i < nRows; ++i) grid->SetRowSize(i, (*rowHeights)[0] * unitConversionFactor.y);
  else {
	for (SizeT i = 0; i < nRows; ++i) {
	  if (i > (rowHeights->N_Elements() - 1)) break;
	  grid->SetRowSize(i, (*rowHeights)[i] * unitConversionFactor.y);
	}
  }
  grid->EndBatch();
}

void GDLWidgetTable::DoRowHeights(DLongGDL* selection) {
  SizeT nbRows = rowHeights->N_Elements();
  if (nbRows == 0) {
	return;
  }
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert(grid != NULL);
  grid->BeginBatch();

  SizeT k = 0;

  if (selection->Rank() == 0) { //use current wxWidgets selection
	std::vector<int> list = GetSortedSelectedRowsOrColsList(selection, false);
	for (int it = 0; it < list.size(); ++it) {
	  if (list[it] < grid->GetNumberRows()) grid->SetRowSize(list[it], (*rowHeights)[it % nbRows] * unitConversionFactor.y);
	}
  } else { //use the passed selection, mode-dependent:
	if (disjointSelection) { //pairs lists
	  std::vector<int> allRows;
	  std::vector<int>::iterator iter;
	  for (SizeT n = 0, l = 0; n < MAX(selection->Dim(1), 1); ++n) {
		l++;
		int row = (*selection)[l++];
		allRows.push_back(row);
	  }
	  std::sort(allRows.begin(), allRows.end());
	  int theRow = -1;
	  for (iter = allRows.begin(); iter != allRows.end(); ++iter) {
		if ((*iter) != theRow) {
		  theRow = (*iter);
		  //if index is -1, we want to size the label column instead of the grid columns.
		  if (theRow == -1) grid->SetColLabelSize((*columnWidth)[k % nbRows] * unitConversionFactor.x);
		  else if (theRow >= 0 && theRow < grid->GetNumberRows()) grid->SetRowSize(theRow, (*rowHeights)[k % nbRows] * unitConversionFactor.y);
		  k++;
		}
	  }
	} else { //4 values
	  int rowTL = (*selection)[1];
	  int rowBR = (*selection)[3];
	  for (int i = rowTL; i <= rowBR; ++i) {
		//if index is -1, we want to size the label column instead of the grid columns.
		if (i == -1) grid->SetColLabelSize((*columnWidth)[k % nbRows] * unitConversionFactor.x);
		else if (i >= 0 && i < grid->GetNumberRows()) grid->SetRowSize(i, (*rowHeights)[k % nbRows] * unitConversionFactor.y);
		k++;
	  }
	}
  }

  grid->EndBatch();
}

void GDLWidgetTable::DoRowLabels() {
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert(grid != NULL);
  int nRows = grid->GetNumberRows();
  grid->BeginBatch();
  if (rowLabels == NULL || rowLabels->N_Elements() == 0) {
	for (SizeT i = 0; i < nRows; ++i) grid->SetRowLabelValue(i, wxString(i2s(i)));
	grid->EndBatch();
	return;
  }
  if (rowLabels->N_Elements() == 1) { //singleton case
	if (static_cast<DString> ((*rowLabels)[0]).length() == 0) {
	  for (SizeT i = 0; i < nRows; ++i) grid->SetRowLabelValue(i, wxEmptyString);
	} else {
	  for (SizeT i = 0; i < nRows; ++i) {
		if (i > (rowLabels->N_Elements() - 1)) break;
		grid->SetRowLabelValue(i, wxString(static_cast<DString> ((*rowLabels)[i]).c_str(), wxConvUTF8));
	  }
	}
  } else {
	for (SizeT i = 0; i < nRows; ++i) {
	  if (i > (rowLabels->N_Elements() - 1)) break;
	  grid->SetRowLabelValue(i, wxString(static_cast<DString> ((*rowLabels)[i]).c_str(), wxConvUTF8));
	}
  }
  grid->EndBatch();
}

//Row and Column insertion and deletion cannot be done using wxWidgets wxGrid functions, as IDL perform them on a different representation (private array).
//It is necessary to delete all the rows and columns of the widget and recreate a new table

void GDLWidgetTable::DeleteColumns(DLongGDL* selection) {
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert(grid != NULL);
  std::vector<int> list = GetSortedSelectedRowsOrColsList(selection, true);
  //check "memory" selection is good:
  int ncolsmax = this->GetDataNCols();
  for (int it = list.size() - 1; it > -1; --it) if (list[it] < 0 || list[it] > ncolsmax) ThrowGDLException("USE_TABLE_SELECT value out of range.");
  // if possible, unset background of end rows:
  int count = list.size();
  int grid_ncols = grid->GetNumberCols();
  int data_ncols = this->GetDataNCols() - count;
  int ncols = MIN(data_ncols, grid_ncols);
  int data_nrows = this->GetDataNRows();
  for (SizeT j = 0; j < data_nrows; ++j) for (SizeT i = data_ncols; i < grid_ncols; ++i) grid->SetCellBackgroundColour(j, i, *wxLIGHT_GREY);
  //OK, resize vValue:
  BaseGDL* newVal = GetNewTypedBaseGDLColRemoved(vValue, list);
  //... then create the String equivalent
  int majority = GetMajority();
  DStringGDL* format = GetCurrentFormat();
  DStringGDL* newValueAsStrings = ConvertValueToStringArray(newVal, format, majority);
  SetTableValues(newVal, newValueAsStrings, NULL);
}

void GDLWidgetTable::DeleteRows(DLongGDL* selection) {
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert(grid != NULL);
  std::vector<int> list = GetSortedSelectedRowsOrColsList(selection, false);
  //check "memory" selection is good:
  int nrowsmax = this->GetDataNRows();
  for (int it = list.size() - 1; it > -1; --it) if (list[it] < 0 || list[it] > nrowsmax) ThrowGDLException("USE_TABLE_SELECT value out of range.");
  // if possible, set grayed background of count end rows:
  int count = list.size();
  int grid_nrows = grid->GetNumberRows();
  int data_ncols = this->GetDataNCols();
  int data_nrows = nrowsmax - count;
  int nrows = MIN(data_nrows, grid_nrows);
  for (SizeT j = data_nrows; j < grid_nrows; ++j) for (SizeT i = 0; i < data_ncols; ++i) grid->SetCellBackgroundColour(j, i, *wxLIGHT_GREY);
  //OK, resize vValue:
  BaseGDL* newVal = GetNewTypedBaseGDLRowRemoved(vValue, list);
  //... then create the String equivalent
  int majority = GetMajority();
  DStringGDL* format = GetCurrentFormat();
  DStringGDL* newValueAsStrings = ConvertValueToStringArray(newVal, format, majority);
  SetTableValues(newVal, newValueAsStrings, NULL);
}

bool GDLWidgetTable::InsertColumns(DLong count, bool insertAtEnd, DLongGDL* selection) {
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert(grid != NULL);
  std::vector<int> list = GetSortedSelectedRowsOrColsList(selection, true);
  //check "memory" selection is good:
  int ncolsmax = this->GetDataNCols();
  for (int it = list.size() - 1; it > -1; --it) if (list[it] < 0 || list[it] > ncolsmax) ThrowGDLException("USE_TABLE_SELECT value out of range.");
  int iCol = list[0];
  if (insertAtEnd) iCol = ncolsmax;
  // if possible, unset background of count end rows:
  int grid_ncols = grid->GetNumberCols();
  int data_ncols = this->GetDataNCols();
  int ncols = MIN(data_ncols + count, grid_ncols);
  int data_nrows = this->GetDataNRows();
  for (SizeT j = 0; j < data_nrows; ++j) for (SizeT i = data_ncols; i < ncols; ++i) grid->SetCellBackgroundColour(j, i, *wxWHITE);
  //OK, resize vValue:
  BaseGDL* newVal = GetNewTypedBaseGDLColAdded(vValue, iCol, count, !insertAtEnd);
  //... then create the String equivalent
  int majority = GetMajority();
  DStringGDL* format = GetCurrentFormat();
  DStringGDL* newValueAsStrings = ConvertValueToStringArray(newVal, format, majority);
  SetTableValues(newVal, newValueAsStrings, NULL);
  return true;
}

bool GDLWidgetTable::InsertRows(DLong count, bool insertAtEnd, DLongGDL* selection) {
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert(grid != NULL);
  std::vector<int> list = GetSortedSelectedRowsOrColsList(selection, true);
  //check "memory" selection is good:
  int nrowsmax = this->GetDataNRows();
  for (int it = list.size() - 1; it > -1; --it) if (list[it] < 0 || list[it] > nrowsmax) ThrowGDLException("USE_TABLE_SELECT value out of range.");
  int iRow = list[0];
  if (insertAtEnd) iRow = nrowsmax;
  // if possible, unset background of count end rows:
  int grid_nrows = grid->GetNumberRows();
  int data_ncols = this->GetDataNCols();
  int data_nrows = this->GetDataNRows();
  int nrows = MIN(data_nrows + count, grid_nrows);
  for (SizeT j = data_nrows; j < nrows; ++j) for (SizeT i = 0; i < data_ncols; ++i) grid->SetCellBackgroundColour(j, i, *wxWHITE);
  //OK, resize vValue:
  BaseGDL* newVal = GetNewTypedBaseGDLRowAdded(vValue, iRow, count, !insertAtEnd);
  //... then create the String equivalent
  int majority = GetMajority();
  DStringGDL* format = GetCurrentFormat();
  DStringGDL* newValueAsStrings = ConvertValueToStringArray(newVal, format, majority);
  SetTableValues(newVal, newValueAsStrings, NULL);
  return true;
}

void GDLWidgetTable::SetTableValues(BaseGDL* value, DStringGDL* newValueAsStrings, DLongGDL* selection) {
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert(grid != NULL);

  grid->BeginBatch();
  int curr_rows = grid->GetNumberRows();
  int curr_cols = grid->GetNumberCols();
	
  if (selection == NULL) {
	//reset table to everything. val replaces newValueAsStrings.
	// replace all vValue
	SetValue(value);
	SizeT numRows, numCols;
	if (newValueAsStrings->Rank() == 1) {
	  numRows = 1;
	  numCols = newValueAsStrings->Dim(0); //lines
	} else {
	  numRows = newValueAsStrings->Dim(1);
	  numCols = newValueAsStrings->Dim(0);
	}
	grid->ClearGrid();
	//adjust rows and cols:
	if (numRows > curr_rows) grid->AppendRows(numRows - curr_rows);
	if (numCols > curr_cols) grid->AppendCols(numCols - curr_cols);
	// Set grid cell contents as strings
	{
	  SizeT k = 0;
	  for (SizeT i = 0; i < numRows; ++i) for (SizeT j = 0; j < numCols; ++j) {
		  grid->SetCellValue(i, j, wxString(((*newValueAsStrings)[k]).c_str(), wxConvUTF8));
		  ++k;
		}
	}
  } else {
	//use the wxWidget selection or the passed selection, mode-dependent:
	//	assert(value->Type() == vValue->Type());
	SizeT nmaxVal = newValueAsStrings->N_Elements();
	SizeT data_ncols = this->GetDataNCols();
	if (disjointSelection) { //pairs lists
	  if (selection->Rank() == 0) {
		std::vector<wxPoint> list = grid->GetSelectedDisjointCellsList();
		SizeT k = 0;
		for (std::vector<wxPoint>::iterator it = list.begin(); it != list.end(); ++it) {
		  //update vValue
		  UpdatevValues(vValue, it->y * data_ncols + it->x, value, k);
		  //Show in widget
		  grid->SetCellValue((*it).x, (*it).y, wxString(((*newValueAsStrings)[k]).c_str(), wxConvUTF8)); // NO non-visible cells here. 
		  k++; //next one
		  if (k == nmaxVal) break;
		}
	  } else {
		for (SizeT k = 0, n = 0, l = 0; n < MAX(selection->Dim(1), 1); ++n) {
		  int i = (*selection)[l++];
		  int j = (*selection)[l++];
		  //update vValue
		  UpdatevValues(vValue, j * data_ncols + i, value, k);
		  //Show in widgetavoid updating 
		  if (j<curr_rows && i< curr_cols)  grid->SetCellValue(j, i, wxString(((*newValueAsStrings)[k]).c_str(), wxConvUTF8)); // avoid updating non-visible cells xConvUTF8)); 
		  k++; //next one
		  if (k == nmaxVal) break;
		}
	  }
	} else { //IDL maintains the 2D-structure of val!
	  SizeT newValueNumRows, NewValueNumCols;
	  if (newValueAsStrings->Rank() == 1) {
		NewValueNumCols = newValueAsStrings->Dim(0); //lines
		newValueNumRows = 1;
	  } else {
		NewValueNumCols = newValueAsStrings->Dim(0);
		newValueNumRows = newValueAsStrings->Dim(1);
	  }
	  int colTL, colBR, rowTL, rowBR;
	  if (selection->Rank() == 0) {
		wxArrayInt block = grid->GetSelectedBlockOfCells();
		//normally only ONE block is available.
		colTL = block[0];
		rowTL = block[1];
		colBR = block[2];
		rowBR = block[3];
	  } else {
		colTL = (*selection)[0];
		rowTL = (*selection)[1];
		colBR = (*selection)[2];
		rowBR = (*selection)[3];
	  }
	  // insert block of dim [ 0:NewValueNumCols-1 , 0: newValueNumRows-1 ] in block [ rowTL:rowBR , colTL:colBR ] taking account of who is smaller,
	  // BUT also avoid updating non-visible cells 
	  for (int jVal = 0; jVal < newValueNumRows; ++jVal) {
		int j = rowTL + jVal;
		if (j > rowBR) break;
		for (int iVal = 0; iVal < NewValueNumCols; ++iVal) {
		  int i = colTL + iVal;
		  if (i > colBR) break;
		  int k = jVal * NewValueNumCols + iVal;
		  //update vValue
		  UpdatevValues(vValue, j * data_ncols + i, value, k);
		  //Show in widget
		  if (j<curr_rows && i< curr_cols) grid->SetCellValue(j, i, wxString(((*newValueAsStrings)[k]).c_str(), wxConvUTF8)); // avoid updating non-visible cells 
		}
	  }
	}
  }
  grid->EndBatch();
}

template <typename T1, typename T2>
void GDLWidgetTable::PopulateWithDisjointSelection(T1* res, DLongGDL* selection) {
  //selection will have throwed if bad.
  int data_numberCols = this->GetDataNCols();
  for (SizeT k = 0, n = 0, l = 0; n < MAX(selection->Dim(1), 1); ++n) {
	int col = (*selection)[l++];
	int row = (*selection)[l++];
	(*res)[k++] = static_cast<T2*> (vValue->DataAddr())[col + row * data_numberCols];
  }
}

BaseGDL* GDLWidgetTable::GetDisjointSelectionValues(DLongGDL* selection) {
  dimension dim(MAX(selection->Dim(1), 1));
  switch (vValue->Type()) {
  case GDL_STRING:
  {
	DStringGDL* res = new DStringGDL(dim, BaseGDL::NOZERO);
	PopulateWithDisjointSelection<DStringGDL, DString>(res, selection);
	return res;
	break;
  }
  case GDL_BYTE:
  {
	DByteGDL* res = new DByteGDL(dim, BaseGDL::NOZERO);
	PopulateWithDisjointSelection<DByteGDL, DByte>(res, selection);
	return res;
	break;
  }
  case GDL_INT:
  {
	DIntGDL* res = new DIntGDL(dim, BaseGDL::NOZERO);
	PopulateWithDisjointSelection<DIntGDL, DInt>(res, selection);
	return res;
	break;
  }
  case GDL_LONG:
  {
	DLongGDL* res = new DLongGDL(dim, BaseGDL::NOZERO);
	PopulateWithDisjointSelection<DLongGDL, DLong>(res, selection);
	return res;
	break;
  }
  case GDL_FLOAT:
  {
	DFloatGDL* res = new DFloatGDL(dim, BaseGDL::NOZERO);
	PopulateWithDisjointSelection<DFloatGDL, DFloat>(res, selection);
	return res;
	break;
  }
  case GDL_DOUBLE:
  {
	DDoubleGDL* res = new DDoubleGDL(dim, BaseGDL::NOZERO);
	PopulateWithDisjointSelection<DDoubleGDL, DDouble>(res, selection);
	return res;
	break;
  }
  case GDL_COMPLEX:
  {
	DComplexGDL* res = new DComplexGDL(dim, BaseGDL::NOZERO);
	PopulateWithDisjointSelection<DComplexGDL, DComplex>(res, selection);
	return res;
	break;
  }
  case GDL_COMPLEXDBL:
  {
	DComplexDblGDL* res = new DComplexDblGDL(dim, BaseGDL::NOZERO);
	PopulateWithDisjointSelection<DComplexDblGDL, DComplexDbl>(res, selection);
	return res;
	break;
  }
  case GDL_UINT:
  {
	DUIntGDL* res = new DUIntGDL(dim, BaseGDL::NOZERO);
	PopulateWithDisjointSelection<DUIntGDL, DUInt>(res, selection);
	return res;
	break;
  }
  case GDL_ULONG:
  {
	DULongGDL* res = new DULongGDL(dim, BaseGDL::NOZERO);
	PopulateWithDisjointSelection<DULongGDL, DULong>(res, selection);
	return res;
	break;
  }
  case GDL_LONG64:
  {
	DLong64GDL* res = new DLong64GDL(dim, BaseGDL::NOZERO);
	PopulateWithDisjointSelection<DLong64GDL, DLong64>(res, selection);
	return res;
	break;
  }
  case GDL_ULONG64:
  {
	DULong64GDL* res = new DULong64GDL(dim, BaseGDL::NOZERO);
	PopulateWithDisjointSelection<DULong64GDL, DULong64>(res, selection);
	return res;
	break;
  }
  default:
	cerr << "Unhandled Table Type, please report!" << endl;
	return NULL; //signal error
  }
  return NULL;
}

template <typename T1, typename T2>
void GDLWidgetTable::PopulateWithSelection(T1* res, int colTL, int colBR, int rowTL, int rowBR) {
  int data_numberCols = vValue->Dim(0);
  for (SizeT k = 0, j = rowTL; j <= rowBR; ++j) for (SizeT i = colTL; i <= colBR; ++i) {
	  (*res)[k++] = static_cast<T2*> (vValue->DataAddr())[j * data_numberCols + i];
	}
}

BaseGDL* GDLWidgetTable::GetDisjointSelectionValuesForStructs(DLongGDL* selection) {
  char tagbuf[13];
  DStructGDL* structIn = static_cast<DStructGDL*> (vValue);
  // define out struct as a collection of unnamed entries of different types
  DStructDesc* inStructDesc = structIn->Desc();
  //create tag structure
  DStructDesc* outStructDesc = new DStructDesc("$truct");
  for (int outTag = 0, n = 0, l = 0; n < MAX(selection->Dim(1), 1); ++n) {
	SizeT t = 0;
	SizeT ix = 0;	
    if (majority == GDLWidgetTable::ROW_MAJOR) {
	  t = (*selection)[l++];
	  ix = (*selection)[l++];
    } else {
 	  ix = (*selection)[l++];
	  t = (*selection)[l++];
	}
	snprintf(tagbuf, 12, "%12d", outTag);
	//convert ' ' to '_'
	for (auto z = 0; z < 12; ++z) if (tagbuf[z] == 32) tagbuf[z] = 95;
	std::string outTagName(const_cast<char *>(tagbuf));
	
	switch (structIn->GetTag(t,ix)->Type()) {
	case GDL_BYTE: //	Byte
	{
	  SpDByte entry;
	  outStructDesc->AddTag(outTagName, &entry);
	}
	  break;

	case GDL_INT: //	16-bit Integer 
	{
	  SpDInt entry;
	  outStructDesc->AddTag(outTagName, &entry);
	}
	  break;

	case GDL_LONG: //	32-bit Long Integer 
	{
	  SpDLong entry;
	  outStructDesc->AddTag(outTagName, &entry);
	}
	  break;

	case GDL_FLOAT: //	32-bit Floating Point Number 
	{
	  SpDFloat entry;
	  outStructDesc->AddTag(outTagName, &entry);
	}
	  break;

	case GDL_DOUBLE: //	64-bit Floating Point Number 
	{
	  SpDDouble entry;
	  outStructDesc->AddTag(outTagName, &entry);
	}
	  break;

	case GDL_COMPLEX: //	Complex Floating Point Number (32-bits each) 
	{
	  SpDComplex entry;
	  outStructDesc->AddTag(outTagName, &entry);
	}
	  break;
	case GDL_STRING: //	String
	{
	  SpDString entry;
	  outStructDesc->AddTag(outTagName, &entry);
	}
	  break;

	case GDL_COMPLEXDBL: //	Complex Floating Point 
	{
	  SpDComplexDbl entry;
	  outStructDesc->AddTag(outTagName, &entry);
	}
	  break;

	case GDL_UINT: //	16-bit Unsigned Integer 
	{
	  SpDUInt entry;
	  outStructDesc->AddTag(outTagName, &entry);
	}
	  break;

	case GDL_ULONG: //	32-bit Unsigned Integer 
	{
	  SpDULong entry;
	  outStructDesc->AddTag(outTagName, &entry);
	}
	  break;

	case GDL_LONG64: //	64-bit Integer 
	{
	  SpDLong64 entry;
	  outStructDesc->AddTag(outTagName, &entry);
	}
	  break;

	case GDL_ULONG64: //	64-bit Unsigned Integer 
	{
	  SpDULong64 entry;
	  outStructDesc->AddTag(outTagName, &entry);
	}
	  break;

	default: //	0 ? Undefined (not allowed) 
	  assert(false);
	}
	outTag++; //increment tag number
  }
  //create the returned struct
  DStructGDL* res = new DStructGDL(outStructDesc,dimension(1));
  //populate
  for (SizeT outTag = 0, n = 0, l = 0; n < MAX(selection->Dim(1), 1); ++n) {
	SizeT t = 0;
	SizeT ix = 0;
	if (majority == GDLWidgetTable::ROW_MAJOR) {
	  t = (*selection)[l++];
	  ix = (*selection)[l++];
	} else {
	  ix = (*selection)[l++];
	  t = (*selection)[l++];
	}
	*(res->GetTag(outTag)) = *(structIn->GetTag(t, ix));
	outTag++; //increment tag number
  }
  return res;
}

BaseGDL* GDLWidgetTable::GetSelectionValues(int colTL, int colBR, int rowTL, int rowBR) {
  SizeT dims[2];
  dims[1] = (rowBR - rowTL + 1);
  dims[0] = (colBR - colTL + 1);
  dimension dim(dims, 2);
  switch (vValue->Type()) {
  case GDL_STRING:
  {
	DStringGDL* res = new DStringGDL(dim, BaseGDL::NOZERO);
	PopulateWithSelection<DStringGDL, DString>(res, colTL, colBR, rowTL, rowBR);
	return res;
	break;
  }
  case GDL_BYTE:
  {
	DByteGDL* res = new DByteGDL(dim, BaseGDL::NOZERO);
	PopulateWithSelection<DByteGDL, DByte>(res, colTL, colBR, rowTL, rowBR);
	return res;
	break;
  }
  case GDL_INT:
  {
	DIntGDL* res = new DIntGDL(dim, BaseGDL::NOZERO);
	PopulateWithSelection<DIntGDL, DInt>(res, colTL, colBR, rowTL, rowBR);
	return res;
	break;
  }
  case GDL_LONG:
  {
	DLongGDL* res = new DLongGDL(dim, BaseGDL::NOZERO);
	PopulateWithSelection<DLongGDL, DLong>(res, colTL, colBR, rowTL, rowBR);
	return res;
	break;
  }
  case GDL_FLOAT:
  {
	DFloatGDL* res = new DFloatGDL(dim, BaseGDL::NOZERO);
	PopulateWithSelection<DFloatGDL, DFloat>(res, colTL, colBR, rowTL, rowBR);
	return res;
	break;
  }
  case GDL_DOUBLE:
  {
	DDoubleGDL* res = new DDoubleGDL(dim, BaseGDL::NOZERO);
	PopulateWithSelection<DDoubleGDL, DDouble>(res, colTL, colBR, rowTL, rowBR);
	return res;
	break;
  }
  case GDL_COMPLEX:
  {
	DComplexGDL* res = new DComplexGDL(dim, BaseGDL::NOZERO);
	PopulateWithSelection<DComplexGDL, DComplex>(res, colTL, colBR, rowTL, rowBR);
	return res;
	break;
  }
  case GDL_COMPLEXDBL:
  {
	DComplexDblGDL* res = new DComplexDblGDL(dim, BaseGDL::NOZERO);
	PopulateWithSelection<DComplexDblGDL, DComplexDbl>(res, colTL, colBR, rowTL, rowBR);
	return res;
	break;
  }
  case GDL_UINT:
  {
	DUIntGDL* res = new DUIntGDL(dim, BaseGDL::NOZERO);
	PopulateWithSelection<DUIntGDL, DUInt>(res, colTL, colBR, rowTL, rowBR);
	return res;
	break;
  }
  case GDL_ULONG:
  {
	DULongGDL* res = new DULongGDL(dim, BaseGDL::NOZERO);
	PopulateWithSelection<DULongGDL, DULong>(res, colTL, colBR, rowTL, rowBR);
	return res;
	break;
  }
  case GDL_LONG64:
  {
	DLong64GDL* res = new DLong64GDL(dim, BaseGDL::NOZERO);
	PopulateWithSelection<DLong64GDL, DLong64>(res, colTL, colBR, rowTL, rowBR);
	return res;
	break;
  }
  case GDL_ULONG64:
  {
	DULong64GDL* res = new DULong64GDL(dim, BaseGDL::NOZERO);
	PopulateWithSelection<DULong64GDL, DULong64>(res, colTL, colBR, rowTL, rowBR);
	return res;
	break;
  }
  default:
	cerr << "Unhandled Table Type, please report!" << endl;
	return NULL; //signal error
  }
  return NULL;
}

BaseGDL* GDLWidgetTable::GetSelectionValuesForStructs(int colTL, int colBR, int rowTL, int rowBR) {
  DStructGDL* structIn = static_cast<DStructGDL*> (vValue);
  SizeT structIn_dim0 = structIn->Dim(0);
  SizeT structIn_ntags = structIn->NTags();
  // return a new struct created from existing struct, in block mode this is a multidim array of 'line' or 'col' structs
  SizeT structOut_ntags = 0;
  SizeT start = 0;
  SizeT end = 0;
  SizeT structOut_nEl = 0;
  SizeT structIn_startelem = 0;
  if (majority == GDLWidgetTable::ROW_MAJOR) {
	structOut_ntags = colBR - colTL + 1;
	end = colBR;
	start = colTL;
	structOut_nEl = rowBR - rowTL + 1;
	structIn_startelem = rowTL;
  } else {
	structOut_ntags = rowBR - rowTL + 1;
	end = rowBR;
	start = rowTL;
	structOut_nEl = colBR - colTL + 1;
	structIn_startelem = colTL;
  }
  DStructDesc* inStructDesc = structIn->Desc();
  //create tag structure
  DStructDesc* outStructDesc = new DStructDesc("$truct");
  for (SizeT j = start; j <= end; ++j) {
	switch (structIn->GetTag(j)->Type()) {
	case GDL_BYTE: //	Byte
	{
	  SpDByte entry;
	  outStructDesc->AddTag(inStructDesc->TagName(j), &entry);
	}
	  break;

	case GDL_INT: //	16-bit Integer 
	{
	  SpDInt entry;
	  outStructDesc->AddTag(inStructDesc->TagName(j), &entry);
	}
	  break;

	case GDL_LONG: //	32-bit Long Integer 
	{
	  SpDLong entry;
	  outStructDesc->AddTag(inStructDesc->TagName(j), &entry);
	}
	  break;

	case GDL_FLOAT: //	32-bit Floating Point Number 
	{
	  SpDFloat entry;
	  outStructDesc->AddTag(inStructDesc->TagName(j), &entry);
	}
	  break;

	case GDL_DOUBLE: //	64-bit Floating Point Number 
	{
	  SpDDouble entry;
	  outStructDesc->AddTag(inStructDesc->TagName(j), &entry);
	}
	  break;

	case GDL_COMPLEX: //	Complex Floating Point Number (32-bits each) 
	{
	  SpDComplex entry;
	  outStructDesc->AddTag(inStructDesc->TagName(j), &entry);
	}
	  break;
	case GDL_STRING: //	String
	{
	  SpDString entry;
	  outStructDesc->AddTag(inStructDesc->TagName(j), &entry);
	}
	  break;

	case GDL_COMPLEXDBL: //	Complex Floating Point 
	{
	  SpDComplexDbl entry;
	  outStructDesc->AddTag(inStructDesc->TagName(j), &entry);
	}
	  break;

	case GDL_UINT: //	16-bit Unsigned Integer 
	{
	  SpDUInt entry;
	  outStructDesc->AddTag(inStructDesc->TagName(j), &entry);
	}
	  break;

	case GDL_ULONG: //	32-bit Unsigned Integer 
	{
	  SpDULong entry;
	  outStructDesc->AddTag(inStructDesc->TagName(j), &entry);
	}
	  break;

	case GDL_LONG64: //	64-bit Integer 
	{
	  SpDLong64 entry;
	  outStructDesc->AddTag(inStructDesc->TagName(j), &entry);
	}
	  break;

	case GDL_ULONG64: //	64-bit Unsigned Integer 
	{
	  SpDULong64 entry;
	  outStructDesc->AddTag(inStructDesc->TagName(j), &entry);
	}
	  break;

	default: //	0 ? Undefined (not allowed) 
	  assert(false);
	}
  }
  //create the returned struct
  DStructGDL* res = new DStructGDL(outStructDesc, dimension(structOut_nEl));
  //populate
  for (SizeT ix = 0; ix < structOut_nEl; ++ix) for (SizeT t = 0; t < structOut_ntags; ++t) {
	  SizeT tt = start + t;
	  SizeT i = structIn_startelem + ix;
	  *(res->GetTag(t, ix)) = *(structIn->GetTag(tt, i));
	}
  return res;
}

BaseGDL* GDLWidgetTable::GetTableValues(DLongGDL* selection) {
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert(grid != NULL);
  if (selection == NULL) return vValue->Dup();

  BaseGDL * res;
  int data_numberCols = this->GetDataNCols();
  int data_numberRows = this->GetDataNRows();
  int grid_ncols = grid->GetNumberCols();
  int grid_nrows = grid->GetNumberRows();
  //use the wxWidget selection or the passed selection, mode-dependent:
  if (disjointSelection) {
	if (majority == GDLWidgetTable::NONE_MAJOR) return GetDisjointSelectionValues(selection);
	else return GetDisjointSelectionValuesForStructs(selection);
  } else { //IDL maintains the 2D-structure of val!
	int colTL, colBR, rowTL, rowBR;
	if (selection->Rank() == 0) {
	  wxArrayInt block = grid->GetSelectedBlockOfCells();
	  //normally only ONE block is available.
	  colTL = block[0];
	  if (colTL < 0 || colTL > data_numberCols - 1) ThrowGDLException("USE_TABLE_SELECT value out of range.");
	  rowTL = block[1];
	  if (rowTL < 0 || rowTL > data_numberRows - 1) ThrowGDLException("USE_TABLE_SELECT value out of range.");
	  colBR = block[2];
	  if (colBR < 0 || colBR > data_numberCols - 1) ThrowGDLException("USE_TABLE_SELECT value out of range.");
	  rowBR = block[3];
	  if (rowBR < 0 || rowBR > data_numberRows - 1) ThrowGDLException("USE_TABLE_SELECT value out of range.");
	} else {
	  colTL = (*selection)[0];
	  if (colTL < 0 || colTL > data_numberCols - 1) ThrowGDLException("USE_TABLE_SELECT value out of range.");
	  rowTL = (*selection)[1];
	  if (rowTL < 0 || rowTL > data_numberRows - 1) ThrowGDLException("USE_TABLE_SELECT value out of range.");
	  colBR = (*selection)[2];
	  if (colBR < 0 || colBR > data_numberCols - 1) ThrowGDLException("USE_TABLE_SELECT value out of range.");
	  rowBR = (*selection)[3];
	  if (rowBR < 0 || rowBR > data_numberRows - 1) ThrowGDLException("USE_TABLE_SELECT value out of range.");
	}
	if (majority == GDLWidgetTable::NONE_MAJOR) return GetSelectionValues(colTL, colBR, rowTL, rowBR);
	else return GetSelectionValuesForStructs(colTL, colBR, rowTL, rowBR);
  }
  return NULL;
}

void GDLWidgetTable::SetSelection(DLongGDL* selection) {
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert(grid != NULL);
  grid->BeginBatch();
  updating = true; //prevent sending unwanted events
  grid->ClearSelection();
  wxPoint firstVisible = wxPoint(0, 0);
  if (disjointSelection) { //pairs lists
	SizeT k = 0;
	for (SizeT i = 0; i < MAX(selection->Dim(1), 1); ++i) {
	  int col = (*selection)[k++];
	  int row = (*selection)[k++];
	  grid->SelectBlock(row, col, row, col, true);
	  if (k == 2) {
		firstVisible.x = row;
		firstVisible.y = col;
	  }
	}
  } else {
	int colTL = (*selection)[0];
	int rowTL = (*selection)[1];
	int colBR = (*selection)[2];
	int rowBR = (*selection)[3];
	grid->SelectBlock(rowTL, colTL, rowBR, colBR, false);
	firstVisible.x = rowTL;
	firstVisible.y = colTL;
  }
  grid->EndBatch();
  grid->MakeCellVisible(firstVisible.x, firstVisible.y);
  updating = false; //allow events
}

void GDLWidgetTable::SetTableView(DLongGDL* pos) {
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert(grid != NULL);
  grid->MakeCellVisible((*pos)[1], (*pos)[0]);
}

void GDLWidgetTable::MakeCellEditable(DLongGDL* pos) {
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert(grid != NULL);
  grid->SetReadOnly((*pos)[0], (*pos)[1], false);
}

void GDLWidgetTable::SetTableXsizeAsNumberOfColumns(DLong ncols) {
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert(grid != NULL);
  grid->BeginBatch();
  int old_ncols = grid->GetNumberCols();
  int numRows = this->GetDataNRows();
  int numCols = this->GetDataNCols();
  if (ncols > old_ncols) {
	DStringGDL* format = GetCurrentFormat();
	DStringGDL* newValueAsStrings = ConvertValueToStringArray(vValue, format, majority);
	grid->AppendCols(ncols - old_ncols);
	for (SizeT i = old_ncols; i < ncols; ++i) {
	  grid->SetColLabelValue(i, wxString(i2s(i)));
	  for (SizeT j = 0; j < grid->GetNumberRows(); ++j) grid->SetCellBackgroundColour(j, i, *wxLIGHT_GREY);
	}
	if (numCols > old_ncols) {
	  int colTL, colBR, rowTL, rowBR;
	  colTL = old_ncols - 1;
	  colBR = ncols - 1;
	  rowTL = 0;
	  rowBR = grid->GetNumberRows() - 1;
	  for (int j = rowTL; j <= rowBR; ++j) for (int i = colTL; i <= colBR; ++i) {
		  if (i < numCols && j < numRows) {
			grid->SetCellBackgroundColour(j, i, *wxWHITE);
			grid->SetCellValue(j, i, wxString(((*newValueAsStrings)[j * numCols + i]).c_str(), wxConvUTF8));
		  }
		}
	}
  } else if (ncols < old_ncols) grid->DeleteCols(ncols, old_ncols - ncols);
  grid->EndBatch();
}

void GDLWidgetTable::SetTableYsizeAsNumberOfRows(DLong nrows) {
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert(grid != NULL);
  grid->BeginBatch();
  SizeT old_nrows = grid->GetNumberRows();
  int numRows = this->GetDataNRows();
  int numCols = this->GetDataNCols();
  if (nrows > old_nrows) {
	int majority = GetMajority();
	DStringGDL* format = GetCurrentFormat();
	DStringGDL* newValueAsStrings = ConvertValueToStringArray(vValue, format, majority);
	grid->AppendRows(nrows - old_nrows);
	for (SizeT j = old_nrows; j < nrows; ++j) {
	  for (SizeT i = 0; i < grid->GetNumberCols(); ++i) grid->SetCellBackgroundColour(j, i, *wxLIGHT_GREY);
	}
	if (numRows > old_nrows) {
	  int colTL, colBR, rowTL, rowBR;
	  colTL = 0;
	  colBR = grid->GetNumberCols() - 1;
	  rowTL = old_nrows - 1;
	  rowBR = nrows - 1;
	  for (int j = rowTL; j <= rowBR; ++j) for (int i = colTL; i <= colBR; ++i) {
		  if (i < numCols && j < numRows) {
			grid->SetCellBackgroundColour(j, i, *wxWHITE);
			grid->SetCellValue(j, i, wxString(((*newValueAsStrings)[j * numCols + i]).c_str(), wxConvUTF8));
		  }
		}
	}
  } else if (nrows < old_nrows) grid->DeleteRows(nrows, old_nrows - nrows);
  grid->EndBatch();
}

DStructGDL* GDLWidgetTable::GetGeometry(wxRealPoint fact) {
  if (!this->IsRealized()) this->OnRealize();
  GDLWidgetBase* container = static_cast<GDLWidgetBase*> (this->GetMyParent());
  assert(container != NULL);
  int ixsize = 0, iysize = 0, iscr_xsize = 0, iscr_ysize = 0;
  DFloat xsize = 0;
  DFloat ysize = 0;
  DFloat scr_xsize = 0;
  DFloat scr_ysize = 0;
  DFloat xoffset = 0;
  DFloat yoffset = 0;
  DFloat margin = 0;

  DStructGDL* ex = new DStructGDL("WIDGET_GEOMETRY");

  wxWindow* test = dynamic_cast<wxWindow*> (theWxContainer);
  if (test == NULL) return ex;
  wxRect r = test->GetRect();
  xoffset = r.x;
  yoffset = r.y;
  wxGrid* w = dynamic_cast<wxGrid*> (theWxWidget);
  if (w == NULL) return ex;
  wxRect z = w->GetRect();

  iscr_xsize = z.width;
  iscr_ysize = z.height;
  wxSize s = w->GetClientSize();
  ixsize = s.x;
  iysize = s.y;

  if (theWxWidget == theWxContainer) margin = 0;
  else {
	margin = (r.width - z.width) / 2; //Rect is about the container, which is the widgetPanel and not the frame depending on case.
  }
  //size is in columns
  int rowsize = w->GetRowSize(0);
  int rowlabelsize = w->GetRowLabelSize();
  int colsize = w->GetColSize(0);
  int collabelsize = w->GetColLabelSize();
  xsize = (ixsize - rowlabelsize) / colsize;
  ysize = (iysize - collabelsize) / rowsize;

  scr_xsize = iscr_xsize / fact.x;
  scr_ysize = iscr_ysize / fact.y;
  xoffset /= fact.x;
  yoffset /= fact.y;
  margin /= fact.x;

  ex->InitTag("XOFFSET", DFloatGDL(xoffset));
  ex->InitTag("YOFFSET", DFloatGDL(yoffset));
  ex->InitTag("XSIZE", DFloatGDL(xsize));
  ex->InitTag("YSIZE", DFloatGDL(ysize));
  ex->InitTag("SCR_XSIZE", DFloatGDL(scr_xsize));
  ex->InitTag("SCR_YSIZE", DFloatGDL(scr_ysize));
  ex->InitTag("MARGIN", DFloatGDL(margin));

  return ex;
}

void GDLWidgetTable::setFont() {
  wxGrid* g = dynamic_cast<wxGrid*> (this->GetWxWidget());
  if (g != NULL) {
	if (!font.IsSameAs(wxNullFont)) {
	  g->SetLabelFont(font);
	  g->SetDefaultCellFont(font);
	} else {
	  g->SetLabelFont(defaultFont);
	  g->SetDefaultCellFont(defaultFont);
	}
  }
}

GDLWidgetTable::~GDLWidgetTable() {
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetTable(" << widgetID << ")" << std::endl;
#endif  
  if (theWxContainer) static_cast<wxWindow*> (theWxContainer)->Destroy();
}

/*********************************************************/
// for WIDGET_TREE
/*********************************************************/

GDLWidgetTree::GDLWidgetTree( WidgetIDT p, EnvT* e, BaseGDL* value_, DULong eventFlags_
,wxBitmap* bitmap
,DLong dragability
,DLong dropability
,bool expanded_
,bool folder_
,DLong treeindex
,DString &dragNotify_
)
: GDLWidget( p, e, value_, eventFlags_ )
,droppable(dropability ) //inherited
,draggable(dragability ) //inherited
,expanded(expanded_)
,myRoot(NULL)
,treeItemData(NULL)
,has_checkbox(false)
,folder(folder_)
,mask(false)
,dragNotify( dragNotify_) 
{

  //checkbox is inherited
  
  static int CHECKBOX = e->KeywordIx("CHECKBOX");
  static int CHECKED = e->KeywordIx("CHECKED");
  bool checkbox_asked = false;
  if (e->KeywordPresent(CHECKBOX)){
    checkbox_asked = true;
    DLong value=0;
    e->AssureLongScalarKWIfPresent(CHECKBOX,value);
    has_checkbox = (value>0);
  }
  bool checked = (has_checkbox && e->KeywordSet(CHECKED));

  GDLWidget* gdlParent = GetWidget( parentID );
  widgetPanel = GetParentPanel( );
  widgetSizer = GetParentSizer( );
  DStringGDL* value=static_cast<DStringGDL*>(vValue);
  
  //define the base tree widget globally here
  wxTreeCtrlGDL* myTreeRoot;
  if ( gdlParent->IsBase( ) ) {
    folder=true; //IS A FOLDER!
    static int NO_BITMAPS = e->KeywordIx("NO_BITMAPS");
    noBitmaps = e->KeywordSet(NO_BITMAPS);
    static int MULTIPLE = e->KeywordIx("MULTIPLE");
    multiple = e->KeywordSet(MULTIPLE);

    wxImageList *stateImages = gdlDefaultTreeStateImages;
    wxImageList *images = gdlDefaultTreeImages; 
    #include "start_eventual_frame.incpp" //Widget_TREE
  
    // a tree widget is always inside a scrolled window, whose ScrollSize is 200 pixels by default
    if ( wSize.x <= 0 ) wSize.x = 200; //yes, has a default value!
    if ( wSize.y <= 0 ) wSize.y = 200;

    wSize=computeWidgetSize( ); //this is a SetClientSize
    
    long style = wxTR_HAS_BUTTONS|wxTR_LINES_AT_ROOT|wxTR_HIDE_ROOT; //OK
    if (multiple) style |= wxTR_MULTIPLE; //widget is insensitive, FIXME
    //we have no root, create one.
    myTreeRoot = new wxTreeCtrlGDL(widgetPanel, widgetID, wxDefaultPosition, wxDefaultSize, style );
    theWxContainer = theWxWidget = myTreeRoot;
    //All lists are set always. Checkboxes will be hidden if checkbox is not present
    myTreeRoot->SetImageList(images);
    myTreeRoot->SetStateImageList(stateImages);
    
    myRoot=this;
    treeItemData=new wxTreeItemDataGDL(widgetID, myTreeRoot);
//    if (bitmap) {
//      int index=images->Add(*bitmap);
//      treeItemID = tree->AddRoot(wxString( (*value)[0].c_str( ), wxConvUTF8 ),  index ,-1, treeItemData);
//    } else { //use open and closed folder icons
      treeItemID = myTreeRoot->AddRoot(wxString( (*value)[0].c_str( ), wxConvUTF8 ),  0 ,1, treeItemData);
//    }
    treeItemData->SetItemId(treeItemID);
//    tree->SetItemImage(treeItemID,(folder)?(expanded?gdlWxTree_FOLDER_OPEN:gdlWxTree_FOLDER):gdlWxTree_ITEM);
// checkbox is not visible for root//    if (has_checkbox) tree->SetItemState(treeItemID,(checked==true)); else tree->SetItemState(treeItemID,wxTREE_ITEMSTATE_NONE); //CHECKED,UNCHECKE,NOT_VISIBLE
    if (dropability == -1) droppable=0; //this for root only
    if (dragability == -1) draggable=0; //this for root only
//do not expand root if hidden: will assert() in wxWidgets! //    if (expanded) tree->Expand(treeItemID); 
    myTreeRoot->SetClientSize(wSize);
    myTreeRoot->SetMinClientSize(wSize);
//    tree->ShowScrollbars(wxSHOW_SB_ALWAYS,wxSHOW_SB_ALWAYS); //possibly useful.
    END_ADD_EVENTUAL_FRAME
    TIDY_WIDGET(gdlBORDER_SPACE)
      
      //does not work, fixme. Replaced by global setting in gdlwidgeteventhandler 
      //    this->AddToDesiredEvents(wxEVT_COMMAND_TREE_ITEM_ACTIVATED,wxTreeEventHandler(wxTreeCtrlGDL::OnItemActivated),myTreeRoot);
      //    this->AddToDesiredEvents(wxEVT_COMMAND_TREE_BEGIN_DRAG,wxTreeEventHandler(wxTreeCtrlGDL::OnBeginDrag),myTreeRoot);
      //    this->AddToDesiredEvents(wxEVT_COMMAND_TREE_END_DRAG,wxTreeEventHandler(wxTreeCtrlGDL::OnItemDropped),myTreeRoot);
      //    this->AddToDesiredEvents(wxEVT_COMMAND_TREE_ITEM_COLLAPSED,wxTreeEventHandler(wxTreeCtrlGDL::OnItemCollapsed),myTreeRoot);
      //    this->AddToDesiredEvents(wxEVT_COMMAND_TREE_ITEM_EXPANDED,wxTreeEventHandler(wxTreeCtrlGDL::OnItemExpanded),myTreeRoot);
      //    this->AddToDesiredEvents(wxEVT_COMMAND_TREE_SEL_CHANGED,wxTreeEventHandler(wxTreeCtrlGDL::OnItemSelected),myTreeRoot);

  } else {
//    static int TOOLTIP = e->KeywordIx( "TOOLTIP" );
//  DString toolTip;
//  e->AssureStringScalarKWIfPresent( TOOLTIP, toolTip );

    GDLWidgetTree* parentTree = static_cast<GDLWidgetTree*> (gdlParent);
    assert( parentTree != NULL);
    theWxWidget = parentTree->GetWxWidget( );
    myRoot =  parentTree->GetMyRootGDLWidgetTree();
    bool nobitmaps=myRoot->IsUsingBitmaps();

    myTreeRoot = dynamic_cast<wxTreeCtrlGDL*> (theWxWidget);
    assert( myTreeRoot != NULL);
    treeItemData=new wxTreeItemDataGDL(widgetID,myTreeRoot);
    theWxContainer=NULL; //this is not a widget

    //if parent has checkbox, I have a checkbox too unless has_checkbox is false
    bool parent_has_checkbox=parentTree->HasCheckBox(); 
    if (!checkbox_asked) has_checkbox=(parent_has_checkbox);
    //if treeindex is present, it must be < the number of children (otherwise baoum, thank you wxWidgets)
    if (treeindex > -1 ) {
      //possible?
      if (parentTree->IsFolder()) {
        unsigned int count = myTreeRoot->GetChildrenCount(parentTree->treeItemID, false);
        if (treeindex >= count) treeindex = count;
      } else e->Throw("Parent tree widget is not a folder.");
    }
    if (nobitmaps) {
        if (treeindex > -1 ) treeItemID = myTreeRoot->InsertItem( parentTree->treeItemID, treeindex, wxString( (*value)[0].c_str( ), wxConvUTF8 ) ,-1,-1, treeItemData);
        else treeItemID = myTreeRoot->AppendItem( parentTree->treeItemID, wxString( (*value)[0].c_str( ), wxConvUTF8 ) ,-1,-1, treeItemData);
    }
    else if (bitmap) {
   //images (default vs. custom: problem of sizes: wxWidgets ENFORCES an uniform scaling)
      int imindex=myTreeRoot->GetImageList()->Add(wxBitmap((*bitmap).ConvertToImage().Rescale(DEFAULT_TREE_IMAGE_SIZE,DEFAULT_TREE_IMAGE_SIZE)));
      if (treeindex > -1 ) treeItemID = myTreeRoot->InsertItem( parentTree->treeItemID, treeindex, wxString( (*value)[0].c_str( ), wxConvUTF8 ) ,imindex,imindex, treeItemData);
      else treeItemID = myTreeRoot->AppendItem( parentTree->treeItemID, wxString( (*value)[0].c_str( ), wxConvUTF8 ) ,imindex,imindex, treeItemData);
    } else {     //since no image is a bit disappointing use our internal wxWigdets icons
      if (folder) { //use open and closed folder icons 
        if (treeindex > -1 ) treeItemID = myTreeRoot->InsertItem( parentTree->treeItemID, treeindex, wxString( (*value)[0].c_str( ), wxConvUTF8 ) ,TREE_BITMAP_FOLDER,TREE_BITMAP_FOLDER_OPEN, treeItemData);
        else treeItemID = myTreeRoot->AppendItem( parentTree->treeItemID, wxString( (*value)[0].c_str( ), wxConvUTF8 ) ,TREE_BITMAP_FOLDER,TREE_BITMAP_FOLDER_OPEN, treeItemData);
      } //or normal file
      else if (treeindex > -1) treeItemID = myTreeRoot->InsertItem( parentTree->treeItemID, treeindex, wxString( (*value)[0].c_str( ), wxConvUTF8 ) ,TREE_BITMAP_ITEM,TREE_BITMAP_ITEM_SELECTED, treeItemData);
      else  treeItemID = myTreeRoot->AppendItem( parentTree->treeItemID, wxString( (*value)[0].c_str( ), wxConvUTF8 ) ,TREE_BITMAP_ITEM,TREE_BITMAP_ITEM_SELECTED, treeItemData);
    }
    treeItemData->SetItemId(treeItemID);
    if (has_checkbox) myTreeRoot->SetItemState(treeItemID,(checked==true)); else myTreeRoot->SetItemState(treeItemID,wxTREE_ITEMSTATE_NONE); //CHECKED,UNCHECKE,NOT_VISIBLE
    //expand if requested:
    if (expanded)  {
      myTreeRoot->SetItemHasChildren( treeItemID, true); //TRICK! to enable folder opened or closed BY CONSTRUCTION.
     DoExpand(true);
    }
//    if (tooltip) DO SOMETHING! FIXME.
    if (this->IsRealized()){
      myTreeRoot->Refresh();
    }
  }
    //    UpdateGui();
    REALIZE_IF_NEEDED
    if ( eventFlags & GDLWidget::EV_CONTEXT ) myTreeRoot->Connect(widgetID,wxEVT_CONTEXT_MENU, wxContextMenuEventHandler(gdlwxFrame::OnContextEvent));
}
  bool GDLWidgetTree::GetDropability() {
    if (droppable > -1) return droppable;
    int enabledrop=droppable;
    GDLWidgetTree* root=myRoot;
    GDLWidgetTree* w=this;
    while (enabledrop < 0 && w!=root) {
      w=static_cast<GDLWidgetTree*>(w->GetMyParent());
      enabledrop=w->GetDroppableValue();
    }
    return (enabledrop == 1);
  }
  bool GDLWidgetTree::GetDragability() {
    if (draggable > -1) return draggable;
    int enabledrag=draggable;
    GDLWidgetTree* root=myRoot;
    GDLWidgetTree* w=this;
    while (enabledrag < 0 && w!=root) {
      w=static_cast<GDLWidgetTree*>(w->GetMyParent());
      enabledrag=w->GetDraggableValue();
    }
    return (enabledrag == 1);
  }
  void GDLWidgetTree::DoExpand(bool what){
    expanded=what;
    if (what) treeItemData->myTree->Expand(treeItemID); else treeItemData->myTree->Collapse(treeItemID);
    treeItemData->myTree->Refresh();
  }
  void GDLWidgetTree::Select(bool select){
    if (this->myRoot->GetWidgetID() != widgetID ) treeItemData->myTree->SelectItem(treeItemID,select); //root is not selectable
    treeItemData->myTree->Refresh();
  }
  void GDLWidgetTree::SetTreeIndex(DLong where) {
    GDLWidgetTree* parentTree = static_cast<GDLWidgetTree*> (GetWidget( parentID ));
    if (parentTree->IsFolder()){ //is Folder
      wxTreeCtrlGDL* myTreeRoot=dynamic_cast<wxTreeCtrlGDL*> (theWxWidget);
      assert( myTreeRoot != NULL);
      wxTreeItemId currentId = this->treeItemID;
      //where should i put it?
      unsigned int count = myTreeRoot->GetChildrenCount(parentTree->treeItemID, false); 
      DLong treeindex=count;
      if (where > -1 && where <= treeindex) treeindex =where; //will start putting at 'treeindex'
      wxString s=myTreeRoot->GetItemText(currentId);
      int imindex=myTreeRoot->GetItemImage(currentId);
      wxTreeItemId newId=myTreeRoot->InsertItem( parentTree->treeItemID, treeindex, s ,imindex,imindex, treeItemData);
      //we heve to suppress treeItemData from where it was previously attached otherwise it will be destroyed and bang!
      myTreeRoot->SetItemData(currentId,NULL);
      if (this->HasCheckBox()) myTreeRoot->SetItemState(newId,this->IsChecked()); // else myTreeRoot->SetItemState(treeItemID,wxTREE_ITEMSTATE_NONE); //CHECKED,UNCHECKE,NOT_VISIBLE
      if (folder)  {
         myTreeRoot->SetItemHasChildren( newId, true); //TRICK! to enable folder opened or closed BY CONSTRUCTION.
         if (expanded) myTreeRoot->Expand(newId);
       }
      // give back treeItemID to transferred GDLTreeWidget:
      this->SetItemID(newId);
      //Children?
      count = myTreeRoot->GetChildrenCount(currentId,false);
      if (count == 0) {
        myTreeRoot->Delete(currentId);
        return;
      }
      //build full list of children, call this on each GDLWidgetTree:
      wxArrayTreeItemIds list;
      wxTreeItemIdValue cookie;
      wxTreeItemId id = myTreeRoot->GetFirstChild(currentId, cookie);
      do {
        list.Add(id);
        id = myTreeRoot->GetNextSibling(id); 
      } while (id.IsOk());
      int nb=list.Count();
      for (int i=0; i< count ; ++i) {
        id=list[i];
        wxTreeItemDataGDL* d=static_cast<wxTreeItemDataGDL*>(myTreeRoot->GetItemData(id));
        static_cast<GDLWidgetTree*>(GDLWidget::GetWidget(d->widgetID))->SetTreeIndex(-1); //added at the end
      }
      myTreeRoot->Delete(currentId);
      myTreeRoot->Refresh();
    } //else throw GDLException("Parent tree widget is not a folder."); //IDL just forgets.
  }
void GDLWidgetTree::OnRealize(){
   GDLWidgetTree* root=this->GetMyRootGDLWidgetTree();
   if (this==root) {
     wxTreeCtrlGDL* ctrl=static_cast<wxTreeCtrlGDL*>(this->GetWxWidget());
     wxTreeItemId id=ctrl->GetFirstVisibleItem 	( 		) 	;
     if (id) ctrl->SetFocusedItem(id);
   }
}
DInt GDLWidgetTree::GetTreeIndex()
{
  DInt count=0;
  wxTreeCtrlGDL* tree=dynamic_cast<wxTreeCtrlGDL*>(theWxWidget);
  assert( tree != NULL);
  wxTreeItemId  id=this->treeItemID;
  wxTreeItemId prev_id=tree->GetPrevSibling(id);
  while (prev_id.IsOk()){
    count++; 
    id=prev_id; 
    prev_id=tree->GetPrevSibling(id);
  }
  return count;
}

GDLWidgetTree::~GDLWidgetTree()
{
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetTree: (" << widgetID <<")"<< std::endl;
#endif  
  //the wxWidget points to  the parent branch. A leaf has wxContainer=NULL. //If we are on a leaf, set thewxWidget to NULL at the end, as it would be doubly destroyed in ~GDLWidget otherwise.
  
  wxTreeCtrlGDL* tree = dynamic_cast<wxTreeCtrlGDL*> (theWxWidget);
  if (tree) { // container-type behaviour: kill gdl childrens 
    wxTreeItemId id = this->treeItemID;
    if (id.IsOk()) {
      wxTreeItemIdValue cookie;
      wxTreeItemId child = tree->GetFirstChild(id, cookie);
      while (child.IsOk()) {
        WidgetIDT childID = dynamic_cast<wxTreeItemDataGDL*> (tree->GetItemData(child))->widgetID;
        GDLWidgetTree* GDLchild = static_cast<GDLWidgetTree*> (GDLWidget::GetWidget(childID));
        child = tree->GetNextSibling(child); //nextChild did not find the last (?) nextSibling does the job.
#ifdef GDL_DEBUG_WIDGETS
        std::cout << "~GDLWidgetTree, deleting item ID #" << childID << " of tree widget #" << widgetID << std::endl;
#endif
        delete GDLchild;
      }
      treeItemID.Unset();
      tree->Delete(id);
    }
    tree->Refresh();
  }
  //after all children destroyed, destroy my wx counterpart
  if (theWxContainer) static_cast<wxWindow*>(theWxContainer)->Destroy();
}

void GDLWidgetTree::SetValue(DString val)
{
  GDLDelete(vValue); 
  vValue=new DStringGDL(val);
  wxTreeCtrlGDL* tree=dynamic_cast<wxTreeCtrlGDL*>(theWxWidget);
  assert( tree != NULL);
  tree->SetItemText(treeItemID, wxString( val.c_str( ), wxConvUTF8 ));
  tree->Refresh();
}

void GDLWidgetTree::SetBitmap(wxBitmap* bitmap) {
  //images (default vs. custom: problem of sizes: wxWidgets ENFORCES an uniform scaling)
  wxTreeCtrlGDL* myTreeRoot=treeItemData->myTree;
  wxBitmap b=wxBitmap((*bitmap).ConvertToImage().Rescale(DEFAULT_TREE_IMAGE_SIZE, DEFAULT_TREE_IMAGE_SIZE));
  int myimindex = myTreeRoot->GetItemImage(treeItemID, wxTreeItemIcon_Normal);
  if (myimindex < TREE_BITMAP_END) { //this is a 'default' image, do not overwrite, add a new one
    int imindex=myTreeRoot->GetImageList()->Add(b);
    for (wxTreeItemIcon i=wxTreeItemIcon_Normal; i< wxTreeItemIcon_Max; i=wxTreeItemIcon(i+1)) myTreeRoot->SetItemImage(treeItemID,imindex,i);
  } else {
    myTreeRoot->GetImageList()->Replace(myimindex,b);
  }
  myTreeRoot->Refresh();
}

DByteGDL* GDLWidgetTree::ReturnBitmapAsBytes() {
  wxTreeCtrlGDL* myTreeRoot = treeItemData->myTree;
  int myimindex = myTreeRoot->GetItemImage(treeItemID, wxTreeItemIcon_Normal);
  if (myimindex < TREE_BITMAP_END) { //this is a 'default' image,return 0
    return new DByteGDL(0);
  } else {
    wxImage image=myTreeRoot->GetImageList()->GetBitmap(myimindex).ConvertToImage().Mirror(false); //Mirror=FlIP necessary!!!
    unsigned char* pixels=image.GetData(); 
    wxSize sz=image.GetSize();
    DByteGDL* res=new DByteGDL(dimension(sz.x,sz.y,3), BaseGDL::NOZERO); //in fact [3,N,M] RGBRGB...
    SizeT k=0;
    SizeT jump=sz.x*sz.y;
    for (SizeT i=0; i< sz.x*sz.y; ++i) {(*res)[i]=pixels[k++];(*res)[i+jump]=pixels[k++]; (*res)[i+2*jump]=pixels[k++];}//[N,M,3]
    return res;
  }
}

WidgetIDT GDLWidgetTree::IsSelectedID() {
  return treeItemData->myTree->IsSelected(treeItemID);
}

WidgetIDT GDLWidgetTree::IsDragSelectedID() { //must return 
  wxTreeItemId test = treeItemID;
  wxTreeCtrlGDL* myTreeRoot = treeItemData->myTree;
  if (!myTreeRoot->IsSelected(test)) return 0;
  do {
    test = myTreeRoot->GetItemParent(test);
  } while (test.IsOk() && !myTreeRoot->IsSelected(test));
  if (!test.IsOk()) {
    return 1; //no parent was selected
  }
  return 0;
}

DString GDLWidgetTree::GetDragNotifyValue(){
  DString s=dragNotify;
  if (s!="<inherit>") return s;
  
  wxTreeItemId test = treeItemID;
  wxTreeCtrlGDL* myTreeRoot = treeItemData->myTree;
  do {
    test = myTreeRoot->GetItemParent(test);
    GDLWidgetTree* parent=myTreeRoot->GetItemTreeWidget(test);
    s=parent->GetDragNotifyValue();
  } while (test.IsOk() && s=="<inherit>");
  if (!test.IsOk()) {
    s="<default>";
    return s; //no parent was selected
  }
  if (s.size()) return s; else return "<default>";
}

DLongGDL* GDLWidgetTree::GetAllSelectedID() {
  //tree must be root
  GDLWidgetTree* myGdlTreeRoot = this->GetMyRootGDLWidgetTree();
  assert(myGdlTreeRoot == this);
  wxTreeCtrlGDL* myTreeRoot = treeItemData->myTree;
  wxArrayTreeItemIds list;
  int nb = myTreeRoot->GetSelections(list);
  if (nb == 0) return new DLongGDL(-1);
  DLongGDL* res = new DLongGDL(dimension(nb), BaseGDL::NOZERO);
  for (int i = 0; i < nb; ++i) {
    wxTreeItemDataGDL* data = static_cast<wxTreeItemDataGDL*> (myTreeRoot->GetItemData(list[i]));
    (*res)[i] = data->GetWidgetID();
  }
  return res;
}
//called with root tree only. As soon as a folder is selected, childrens must not be returned as they are supposed to be 'included' in whatever action
//is done with this list.
  DLongGDL* GDLWidgetTree::GetAllDragSelectedID(){
    //tree must be root
  GDLWidgetTree* myGdlTreeRoot = this->GetMyRootGDLWidgetTree();
  assert(myGdlTreeRoot==this);
  wxTreeCtrlGDL* myTreeRoot = treeItemData->myTree;
  wxArrayTreeItemIds list1;
  int nb = myTreeRoot->GetSelections(list1);
  if (nb == 0) return new DLongGDL(-1);
  wxArrayTreeItemIds list2;
  for (int i = 0; i < nb; ++i) { //test whether itemid's list of parent is selected. if one parent is lecetd, drop it else add in list2.
    wxTreeItemId test=list1[i];
    do {
      test=myTreeRoot->GetItemParent(test);
    } while (test.IsOk() && !myTreeRoot->IsSelected(test));
      if (!test.IsOk()) {
      list2.Add(list1[i]); //no parent was selected
      }
  }
  nb=list2.Count();
  if (nb == 0) return new DLongGDL(-1);
  DLongGDL* res = new DLongGDL(dimension(nb), BaseGDL::NOZERO);
  for (int i = 0; i < nb; ++i) {
    wxTreeItemDataGDL* data = static_cast<wxTreeItemDataGDL*> (myTreeRoot->GetItemData(list2[i]));
    (*res)[i] = data->GetWidgetID();
  }
  return res;
}
  

DLong GDLWidgetTree::NChildren() const {  
  wxTreeCtrlGDL* myTreeRoot=treeItemData->myTree;
  return myTreeRoot->GetChildrenCount(treeItemID,false);
}
WidgetIDT GDLWidgetTree::GetChild(DLong childIx) const { //childIx is not used here
  wxTreeCtrlGDL* myTreeRoot=treeItemData->myTree;
  wxTreeItemIdValue cookie;
  wxTreeItemId id=myTreeRoot->GetFirstChild(treeItemID,cookie);
  if (!id.IsOk()) return 0;
  return static_cast<wxTreeItemDataGDL*>(myTreeRoot->GetItemData(id))->widgetID;
}
DLongGDL* GDLWidgetTree::GetChildrenList() const {
  wxTreeCtrlGDL* myTreeRoot=treeItemData->myTree;
  int n=myTreeRoot->GetChildrenCount(treeItemID,false);
  if (n<1) return new DLongGDL(0);
  wxTreeItemIdValue cookie;
  DLongGDL* ret=new DLongGDL(dimension(n),BaseGDL::NOZERO);
  wxTreeItemId id=myTreeRoot->GetFirstChild(treeItemID,cookie);
  (*ret)[0]=static_cast<wxTreeItemDataGDL*>(myTreeRoot->GetItemData(id))->widgetID;
  for (int i=1; i<n; ++i) {
    id=myTreeRoot->GetNextChild(treeItemID,cookie); 
    assert (id.IsOk());
    (*ret)[i]=static_cast<wxTreeItemDataGDL*>(myTreeRoot->GetItemData(id))->widgetID;
  }
  return ret;
}
DLong GDLWidgetTree::GetTheSiblingOf(DLong myId) {  //GetTheSibling is called by widget_info() using the parent widget (a container). This is not our case here.
  // retrieve My infos, not parent's:
  GDLWidgetTree* me=static_cast<GDLWidgetTree*>(GDLWidget::GetWidget(myId));
  if (me==NULL) return 0; else return me->Sibling(); //call Sibling with good 'this'
}
DLong GDLWidgetTree::Sibling() { //uses NextSibling, which may be 0.
  wxTreeCtrlGDL* myTreeRoot=treeItemData->myTree;
  wxTreeItemId id=myTreeRoot->GetNextSibling(treeItemID);
  if (!id.IsOk()) return 0;
  return static_cast<wxTreeItemDataGDL*>(myTreeRoot->GetItemData(id))->widgetID;
}
//The Following does not work due to a wxWidgets inner loop problem I cannot fathom.
  int GDLWidgetTree::GetDragNotifyReturn(DString &getFuncName, WidgetIDT sourceID, int modifiers, int defaultval) {
    try{
      SizeT funIx = GDLInterpreter::GetFunIx( StrUpCase( getFuncName)  );
      if (funIx < 0) {
        Warning("Drag Notify Function "+getFuncName+" not found.");
        return 0;
      }
      EnvT* newEnv = new EnvT(NULL, libFunList[ funIx]);
      newEnv->SetNextPar( new DLongGDL( widgetID ) ); // i am destination
      newEnv->SetNextPar( new DLongGDL( sourceID ) ); 
      newEnv->SetNextPar( new DLongGDL( modifiers ) ); 
      newEnv->SetNextPar( new DLongGDL( defaultval ) ); 
      DLongGDL* res =static_cast<DLongGDL*>( static_cast<DLibFun*>(newEnv->GetPro())->Fun()(static_cast<EnvT*>(newEnv)));
      return (*res)[0];
    } catch(...) {Warning("problem using "+getFuncName+"."); return -1;}
    return -1;
  }

  GDLWidgetTree* wxTreeCtrlGDL::GetItemTreeWidget(wxTreeItemId itemid){
    GDLWidget* wid=GDLWidget::GetWidget(static_cast<wxTreeItemDataGDL*>(GetItemData(itemid))->widgetID);
    return static_cast<GDLWidgetTree*>(wid);
  }
/*********************************************************/
// for WIDGET_SLIDER
/*********************************************************/
GDLWidgetSlider::GDLWidgetSlider( WidgetIDT p, EnvT* e, DLong value_
, DULong eventFlags_
, DLong minimum_
, DLong maximum_
, bool vertical
, bool suppressValue
, DString &title_ )
: GDLWidget( p, e, NULL, eventFlags_ )
, value( value_ ) //should disappear , duplicates vValue!
, minimum( minimum_ )
, maximum( maximum_ )
, title( title_)
{
  GDLWidget* gdlParent = GetWidget( parentID );
  widgetPanel = GetParentPanel( );
  widgetSizer = GetParentSizer( );

  #include "start_eventual_frame.incpp" //Widget_SLIDER

  long style = wxSL_HORIZONTAL;
  if ( vertical ) style = wxSL_VERTICAL|wxSL_INVERSE;
  if ( !suppressValue ) {
#if wxCHECK_VERSION(2,9,1)
    style |= wxSL_VALUE_LABEL;
#else
    style |= wxSL_LABELS;
#endif
  }

  const wxString s=wxString( title.c_str( ), wxConvUTF8 );
  wxSlider* slider;

//get defined sizes if any
  wSize=computeWidgetSize( );
// this is the size in absence of text:  
  bool hastitle=(s.size()>0);
//// A gdlSlider is a comboPanel containing a wxStaticText (eventually) plus a slider. 
  //if text or frame, we need a panel containing the wxVERTICAL association. frame is sunken if /FRAME, or NONE
  if (hastitle) {
    wxPanel* comboPanel=new wxPanel(widgetPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    if (frameWidth > 1 /*&&  tryToMimicOriginalWidgets*/) comboPanel->SetBackgroundColour(wxColour(sysPanelDefaultColour));
    wxSizer* combosz = (vertical)?new wxBoxSizer(wxHORIZONTAL):new wxBoxSizer(wxVERTICAL);
    comboPanel->SetSizer(combosz);
    theWxContainer = comboPanel; //else stays as panel_if_framed.
    comboPanel->SetFont(font); //enable fancy font
    slider= new wxSlider( comboPanel, widgetID, value, minimum, maximum, wxDefaultPosition, wxDefaultSize, style);
    combosz->Add(slider,DONOTALLOWSTRETCH,wxEXPAND|wxALL,gdlSPACE);
    int w=0;
    int h=0;
    int d=0;
    int lead=0;
    comboPanel->GetTextExtent(s,&w,&h,&d,&lead,&font);
    wxSize TextSize=wxSize(w,h);
    if (!vertical && wSize.x < 1) wSize.x = TextSize.x;
    wxStaticText *theTitle=new wxStaticText(comboPanel,wxID_ANY,s, wxDefaultPosition, TextSize, wxST_ELLIPSIZE_END);
    combosz->Add(theTitle,DONOTALLOWSTRETCH,0,gdlSPACE);
//    combosz->AddSpacer(5);
    comboPanel->SetSize(wSize);
    comboPanel->SetMinSize(wSize);
    theWxWidget=comboPanel; //to trick ADD EVENTUAL FRAME 
  } else {
    slider= new wxSlider( widgetPanel, widgetID, value, minimum, maximum, wxDefaultPosition, wxDefaultSize, style);
    slider->SetSize(wSize);
    slider->SetMinSize(wSize);
    theWxContainer = theWxWidget=slider;
  }
  widgetPanel->Fit();
 
  END_ADD_EVENTUAL_FRAME
  TIDY_WIDGET(gdlBORDER_SPACE)
//  UpdateGui();
  REALIZE_IF_NEEDED 
  this->AddToDesiredEvents( wxEVT_SCROLL_CHANGED,wxScrollEventHandler(gdlwxFrame::OnThumbRelease),slider);
  //dynamically select drag, saves resources! (note: there is no widget_control,/drag for sliders)
  if ( eventFlags & GDLWidget::EV_DRAG ) this->AddToDesiredEvents( wxEVT_SCROLL_THUMBTRACK,wxScrollEventHandler(gdlwxFrame::OnThumbTrack),slider);
}
wxSlider* GDLWidgetSlider::GetSliderWidget(){
  wxSlider* s;
  if (title.length() > 0) {
      wxPanel* p=dynamic_cast<wxPanel*>(theWxWidget);
      const wxWindowList& l = p->GetChildren();
      for ( wxWindowList::compatibility_iterator it = l.GetFirst(); it; it = it->GetNext() ) {
        wxWindow *current = (wxWindow *)it->GetData();
        if (current->IsKindOf(CLASSINFO(wxSlider))) {
            s = dynamic_cast<wxSlider*>(current);
        }
      }
  } else {
      s = dynamic_cast<wxSlider*>(theWxWidget);
  }
  return(s);
}  
 
void GDLWidgetSlider::ControlSetValue(DLong v){
  value=v;
  wxSlider* s = GDLWidgetSlider::GetSliderWidget();
  assert( s != NULL);
  s->SetValue(v);
}

void GDLWidgetSlider::ControlSetMinValue(DLong v) {
  value = v;
  wxSlider* s = GDLWidgetSlider::GetSliderWidget();
  assert(s != NULL);
  s->SetRange(v, s->GetMax());
}

void GDLWidgetSlider::ControlSetMaxValue(DLong v) {
  value = v;
  wxSlider* s = GDLWidgetSlider::GetSliderWidget();
  assert(s != NULL);
  s->SetRange(s->GetMin(),v);
}

GDLWidgetSlider::~GDLWidgetSlider(){
  title.clear();
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetSlider(" << widgetID <<")"<< std::endl;
#endif
  if (theWxContainer) static_cast<wxWindow*>(theWxContainer)->Destroy();
}

GDLWidgetButton::GDLWidgetButton( WidgetIDT p, EnvT* e,
DStringGDL* value , DULong eventflags, wxBitmap* bitmap_)
: GDLWidget( p, e, value, eventflags )
, buttonType( UNDEFINED )
//, buttonBitmap(bitmap_)
, buttonState(false)
, menuItem(NULL)
, valueWxString( wxString((*value)[0].c_str(), wxConvUTF8) )
{
  if (valueWxString.Length() < 1) valueWxString=wxT(" ");
}


GDLWidgetButton::~GDLWidgetButton() {
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetButton(" << widgetID << ")" << std::endl;
#endif
//  if (buttonBitmap) delete buttonBitmap;
}

//a normal button.
GDLWidgetNormalButton::GDLWidgetNormalButton(WidgetIDT p, EnvT* e,
  DStringGDL* value, DULong eventflags,  wxBitmap* bitmap_, DStringGDL* buttonToolTip)
: GDLWidgetButton(p, e, value, eventflags, bitmap_)
{
  GDLWidget* gdlParent = GetWidget(parentID);
  widgetPanel = GetParentPanel();
  widgetSizer = GetParentSizer();
  
  #include "start_eventual_frame.incpp" //Widget NORMAL BUTTON

  wSize = computeWidgetSize();
  //we deliberately prevent exclusive buttons when bitmap are present (exclusive buttons w/ pixmap do not exist in wxWidgets.
  if (gdlParent->GetExclusiveMode() == BGNORMAL || bitmap_) {
    if (bitmap_) {
      wxBitmapButton *button = new wxBitmapButton(widgetPanel, widgetID, *bitmap_, wxDefaultPosition,  wxDefaultSize);
      theWxContainer = theWxWidget = button;
      buttonType = BITMAP;
      this->AddToDesiredEvents(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(gdlwxFrame::OnButton), button);
    } else {
      wxButton *button = new wxButton(widgetPanel, widgetID, valueWxString,wxDefaultPosition, wxDefaultSize, buttonTextAlignment()); 
      theWxContainer = theWxWidget = button;
      buttonType = NORMAL;
      this->AddToDesiredEvents(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(gdlwxFrame::OnButton), button);
    }
  } else if (gdlParent->GetExclusiveMode() == BGEXCLUSIVE1ST) {
    wxRadioButton *radioButton = new wxRadioButton(widgetPanel, widgetID, valueWxString,  wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
    gdlParent->SetExclusiveMode(1);
    GDLWidgetBase* b = static_cast<GDLWidgetBase*> (gdlParent);
    if (b) b->SetLastRadioSelection(widgetID);
    else cerr << "Warning GDLWidgetNormalButton::GDLWidgetNormalButton(): widget type confusion(1)\n";

    theWxContainer = theWxWidget = radioButton;
    this->AddToDesiredEvents(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(gdlwxFrame::OnRadioButton), radioButton);
    buttonType = RADIO;
  } else if (gdlParent->GetExclusiveMode() == BGEXCLUSIVE) {
    wxRadioButton *radioButton = new wxRadioButton(widgetPanel, widgetID, valueWxString, wxDefaultPosition, wxDefaultSize);
    theWxContainer = theWxWidget = radioButton;
    this->AddToDesiredEvents(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(gdlwxFrame::OnRadioButton), radioButton);
    buttonType = RADIO;
  } else if (gdlParent->GetExclusiveMode() == BGNONEXCLUSIVE) {
    wxCheckBox *checkBox = new wxCheckBox(widgetPanel, widgetID, valueWxString,  wxDefaultPosition, wxDefaultSize);
    theWxContainer = theWxWidget = checkBox;
    this->AddToDesiredEvents(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(gdlwxFrame::OnCheckBox), checkBox);
    buttonType = CHECKBOX;
  } else cerr << "Warning GDLWidgetNormalButton::GDLWidgetNormalButton(): widget type confusion(2)\n";
  wxWindow *win = static_cast<wxWindow*> (theWxWidget);
  if (win && buttonToolTip) win->SetToolTip(wxString((*buttonToolTip)[0].c_str(), wxConvUTF8));


  win->SetSize(wSize);
  win->SetMinSize(wSize);
  
  END_ADD_EVENTUAL_FRAME
  TIDY_WIDGET(gdlBORDER_SPACE)
//  UpdateGui();
  REALIZE_IF_NEEDED

}

void GDLWidgetNormalButton::SetButtonWidgetLabelText(const DString& value) {
  delete(vValue);
  vValue = new DStringGDL(value);
  if (theWxWidget != NULL) {
    wxAnyButton *b = dynamic_cast<wxAnyButton*> (theWxWidget);
    b->SetLabelText(wxString(value.c_str(), wxConvUTF8));
    // Should switch to version > 2.9 now!
#if wxCHECK_VERSION(2,9,1)
    b->SetBitmap(wxBitmap(1, 1));
#endif 
    if (this->IsDynamicResize()) {
      this->SetWidgetSize(0,0);
//     this->RefreshDynamicWidget();
    }
  }
}

GDLWidgetNormalButton::~GDLWidgetNormalButton() {
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetNormalButton(" << widgetID << ")" << std::endl;
#endif
  if (theWxContainer) static_cast<wxWindow*>(theWxContainer)->Destroy();
}

//this type of buttons use a container
GDLWidgetMenu::GDLWidgetMenu(WidgetIDT p, EnvT* e,
  DStringGDL* value, DULong eventflags, wxBitmap* bitmap_)
: GDLWidgetButton(p, e, value, eventflags, bitmap_)
{
}

GDLWidgetMenu::~GDLWidgetMenu() {
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetMenu(" << widgetID << ")" << std::endl;
#endif
  // delete all children (in reverse order ?)
  while (!children.empty()) {
    GDLWidget* child = GetWidget(children.back());
    children.pop_back();
    if (child) {
#ifdef GDL_DEBUG_WIDGETS
      std::cout << "~GDLWidgetMenu(" << widgetID << "): removing child: ID=" << child->GetWidgetID() << std::endl;
#endif
      delete child;
    } else {
#ifdef GDL_DEBUG_WIDGETS
      std::cout << "~GDLWidgetMenu(" << widgetID << "): ignoring unexisting child: ID=" << child->GetWidgetID() << std::endl;
#endif
    }
  }
}

//this type of buttons use a container
GDLWidgetSubMenu::GDLWidgetSubMenu(WidgetIDT p, EnvT* e,
  DStringGDL* value, DULong eventflags, bool hasSeparatorAbove, wxBitmap* bitmap_)
: GDLWidgetMenu(p, e, value, eventflags, bitmap_)
, addSeparatorAbove( hasSeparatorAbove)
, the_sep(NULL)
{
  GDLWidget* gdlParent = GetWidget(parentID);

  //get default value: a menu. May be NULL here
  wxMenu *menu = dynamic_cast<wxMenu*> (gdlParent->GetWxWidget());
  wxMenu* submenu = new wxMenu();
  if (addSeparatorAbove) the_sep=menu->AppendSeparator();
  menuItem = new wxMenuItem(menu, widgetID, valueWxString, wxEmptyString, wxITEM_NORMAL, submenu );
  if (bitmap_) menuItem->SetBitmap(*bitmap_);
  menu->Append(menuItem);
  menu->Enable(menuItem->GetId(), sensitive);
  theWxContainer = menu; //is not a window: special treatment
  theWxWidget = submenu; 
  buttonType = MENU;
  //No UpdateGui(); see menu entry.

}

void GDLWidgetSubMenu::SetSensitive(bool value) {
  sensitive = value;
  if (menuItem) menuItem->Enable(value);
}

void GDLWidgetSubMenu::SetButtonWidgetLabelText( const DString& value ) {
  delete(vValue);
  vValue = new DStringGDL(value);
  if (menuItem && value.length() > 0 ) menuItem->SetItemLabel( wxString(value.c_str( ), wxConvUTF8 ) ); //avoid null strings asserts!
}

void GDLWidgetSubMenu::SetButtonWidgetBitmap( wxBitmap* bitmap_ ) {
    if ( menuItem ) {
      menuItem->SetBitmap( *bitmap_ ); //probably only MsW
    }
}

GDLWidgetSubMenu::~GDLWidgetSubMenu() {
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetSubMenu(" << widgetID << ")" << std::endl;
#endif
  GDLWidgetMenu* gdlParent = dynamic_cast<GDLWidgetMenu*> (GetWidget(parentID));
  if (gdlParent) gdlParent->RemoveChild(widgetID);
  
  if (menuItem) {
#ifdef GDL_DEBUG_WIDGETS
    std::cout << "~GDLWidgetSubMenu(" << widgetID << "): attempting to remove submenu #" << menuItem->GetId() << " from menu " << parentID << std::endl;
#endif
    wxMenu* menu = dynamic_cast<wxMenu*> (theWxContainer);
    menu->Remove(menuItem);
    if (addSeparatorAbove) menu->Remove(the_sep);
  }
}

//this type of buttons use a container
GDLWidgetMenuEntry::GDLWidgetMenuEntry(WidgetIDT p, EnvT* e,
  DStringGDL* value, DULong eventflags, bool hasSeparatorAbove, wxBitmap* bitmap_, bool checked_type)
: GDLWidgetButton(p, e, value, eventflags, bitmap_)
, addSeparatorAbove( hasSeparatorAbove)
, checkedState(false) //unchecked at start
, the_sep(NULL)
{
  GDLWidget* gdlParent = GetWidget(parentID);
  if (bitmap_) checked_type=false; //wxWidgets does not like checked bitmaps 
  //get default value: a menu. May be NULL here
  wxMenu *menu = dynamic_cast<wxMenu*> (gdlParent->GetWxWidget());
  if (addSeparatorAbove) the_sep=menu->AppendSeparator();
  menuItem = new wxMenuItem(menu, widgetID, valueWxString,wxEmptyString, checked_type?wxITEM_CHECK:wxITEM_NORMAL);
  if (bitmap_) menuItem->SetBitmap(*bitmap_);
  menu->Append(menuItem);
  menu->Enable(menuItem->GetId(), sensitive);
  theWxContainer = menu; //is not a window
  theWxWidget = menuItem;
  buttonType = ENTRY;
  //NO UPDATE WINDOW as menu are dynamical on stack.
}

void GDLWidgetMenuEntry::SetSensitive(bool value) {
  sensitive = value;
  wxMenuItem* item = dynamic_cast<wxMenuItem*> (theWxWidget);
  if (item) item->Enable(value);
}

void GDLWidgetMenuEntry::SetButtonWidgetLabelText( const DString& value ) {
  delete(vValue);
  vValue = new DStringGDL(value);
  wxMenuItem* item = dynamic_cast<wxMenuItem*> (theWxWidget);
  if (item)  item->SetItemLabel(wxString( value.c_str( ), wxConvUTF8 ));
}
void GDLWidgetMenuEntry::SetButtonWidgetBitmap( wxBitmap* bitmap_ ) {
wxMenuItem* item = dynamic_cast<wxMenuItem*> (theWxWidget);
  if (item) {
    item->SetBitmap( *bitmap_); //probably only on MSW
  }
}

GDLWidgetMenuEntry::~GDLWidgetMenuEntry() {
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetMenuEntry(" << widgetID << ")" << std::endl;
#endif
   GDLWidgetMenu* gdlParent = dynamic_cast<GDLWidgetMenu*>(GetWidget(parentID));
   if (gdlParent) gdlParent->RemoveChild(widgetID);
  if (menuItem) {
#ifdef GDL_DEBUG_WIDGETS
    std::cout << "~GDLWidgetMenuEntry(" << widgetID << "): attempting to remove menuitem #" << menuItem->GetId() << " of menu " << parentID << std::endl;
#endif
    wxMenu* menu = dynamic_cast<wxMenu*> (theWxContainer);
    menu->Remove(menuItem);
    if (addSeparatorAbove) menu->Remove(the_sep);
  }
}

//Mbar buttons are pulldown MENUS, so container

#ifdef PREFERS_MENUBAR
GDLWidgetMenuBarButton::GDLWidgetMenuBarButton(WidgetIDT p, EnvT* e,
  DStringGDL* value, DULong eventflags, DStringGDL* buttonToolTip)
: GDLWidgetMenu(p, e, value, eventflags, NULL)
, entry(-1) {
  GDLWidgetMenuBar* gdlParent = dynamic_cast<GDLWidgetMenuBar*> (GetWidget(parentID));
  if (gdlParent) {
    wxMenuBar *menuBar = dynamic_cast<wxMenuBar*> (gdlParent->GetWxWidget());
    assert(menuBar != NULL);
    wxMenu* menu = new wxMenu();
    theWxContainer = menuBar; //not a WwxWindow
    theWxWidget = menu;
    entry = menuBar->GetMenuCount();
    menuBar->Insert(entry, menu, valueWxString);
    menuBar->EnableTop(entry, sensitive);
    buttonType = MENU;

    //MBAR menus cannot have a tooltip due to 

//    UpdateGui(); REALIZE_IF_NEEDED; //made on the fly, non need.
  }
}

#else
GDLWidgetMenuBarButton::GDLWidgetMenuBarButton(WidgetIDT p, EnvT* e,
  DStringGDL* value, DULong eventflags, wxBitmap* bitmap_,  DStringGDL* buttonToolTip)
: GDLWidgetMenu(p, e, value, eventflags, NULL)
, entry(NULL) {


  GDLWidgetMenuBar* gdlParent = dynamic_cast<GDLWidgetMenuBar*> (GetWidget(parentID));
  assert(gdlParent->IsMenuBar());
  
  wxToolBar *toolBar = dynamic_cast<wxToolBar*> (gdlParent->GetWxWidget());
    assert(toolBar != NULL);
    if (bitmap_ == NULL) {
    wSize=computeWidgetSize();
    wxButtonGDL *button = new wxButtonGDL(font, toolBar, widgetID, valueWxString,
      wOffset, wSize,  wxBORDER_NONE);
    buttonType = POPUP_NORMAL; //gdlMenuButton is a wxButton --> normal. Bitmaps will be supported starting from 2.9.1 
    theWxContainer = button;
    theWxWidget = button->GetPopupMenu(); //a menu
    button->Enable(sensitive);
    entry=toolBar->AddControl(button);
    wxSize tbSize=toolBar->GetSize();
    if (tbSize.y < wSize.y) toolBar->SetSize(wxSize(-1,wSize.y));
    toolBar->Realize();
    } else {
    wxBitmapButtonGDL *button = new wxBitmapButtonGDL(toolBar, widgetID, *bitmap_,
      wOffset, wSize,  wxBU_EXACTFIT|wxBORDER_NONE);
    buttonType = POPUP_BITMAP; //
    theWxContainer = button;
    theWxWidget = button->GetPopupMenu(); //a menu
    button->Enable(sensitive);
    entry=toolBar->AddControl(button);
    wxSize tbSize=toolBar->GetSize();
    if (tbSize.y < wSize.y) toolBar->SetSize(wxSize(-1,wSize.y));
    toolBar->Realize();
    }
    
  wxWindow *win = dynamic_cast<wxWindow*> (theWxContainer);
  if (win) {
    if (buttonToolTip) win->SetToolTip(wxString((*buttonToolTip)[0].c_str(), wxConvUTF8));
    if (widgetSizer) widgetSizer->Add(win, DONOTALLOWSTRETCH, widgetAlignment()|wxALL, gdlSPACE); //|wxALL, gdlSPACE_BUTTON);
  } else cerr << "Warning GDLWidgetMenuButton::GDLWidgetMenuButton(): widget type confusion.\n";

//    UpdateGui(); REALIZE_IF_NEEDED;
}
#endif

void GDLWidgetMenuBarButton::SetSensitive(bool value) {
#ifdef PREFERS_MENUBAR
  sensitive = value;
  wxMenuBar *menuBar = dynamic_cast<wxMenuBar*> (theWxContainer);
  menuBar->EnableTop(entry, value);
#else
  GDLWidget::SetSensitive(value)  ;
#endif
}

void GDLWidgetMenuBarButton::SetButtonWidgetLabelText( const DString& value ) {
  delete(vValue);
  vValue = new DStringGDL(value);
#ifdef PREFERS_MENUBAR
  wxMenuBar *menuBar = dynamic_cast<wxMenuBar*> (theWxContainer);
  menuBar->SetMenuLabel(entry, wxString( value.c_str( ), wxConvUTF8 ));
#else
  wxButton* m=dynamic_cast<wxButton*>(theWxWidget);
  if (m) m->SetLabelText(wxString( value.c_str( ), wxConvUTF8 ));
  this->RefreshDynamicWidget();
#endif
}

GDLWidgetMenuBarButton::~GDLWidgetMenuBarButton() {
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetMenuBarButton(" << widgetID << ")" << std::endl;
#endif
#ifdef PREFERS_MENUBAR
  GDLWidgetMenuBar* gdlParent = dynamic_cast<GDLWidgetMenuBar*>(GetWidget(parentID));
  //which pos?
  if (gdlParent) {
    int pos=gdlParent->GetChildrenPos(widgetID);
    gdlParent->RemoveChild(widgetID);
    if (pos >= 0) {
      wxMenuBar *menuBar = dynamic_cast<wxMenuBar*> (theWxContainer);
      menuBar->Remove(pos);
    }
  }
#else
 GDLWidgetMenuBar* gdlParent = dynamic_cast<GDLWidgetMenuBar*> (GetWidget(parentID));
  assert(gdlParent->IsMenuBar());
  wxToolBar *toolBar = dynamic_cast<wxToolBar*> (gdlParent->GetWxWidget());
  assert(toolBar != NULL);
  toolBar->RemoveTool(widgetID);
#endif
}

//a MenuButton Widget is an otherwise normal button in a Base that activates a pulldown menu and is a container
GDLWidgetMenuButton::GDLWidgetMenuButton(WidgetIDT p, EnvT* e,
  DStringGDL* value, DULong eventflags, wxBitmap* bitmap_, DStringGDL* buttonToolTip)
: GDLWidgetMenu(p, e, value, eventflags, bitmap_) {
  GDLWidget* gdlParent = GetWidget(parentID);
  widgetPanel = GetParentPanel();
  widgetSizer = GetParentSizer();
  wSize = computeWidgetSize();

  assert(gdlParent->IsBase());

  //A menu button in a base is a button starting a popup menu. it is a container also.
  //2 different buttons, if bitmap or not, waiting for version > 2.9.1 to have bitmaps handled for ALL buttontypes:
  if (bitmap_ == NULL) {
    wxButtonGDL *button = new wxButtonGDL(font, widgetPanel, widgetID, valueWxString,
      wOffset, wxDefaultSize, buttonTextAlignment());
    button->SetSize(wSize);
    button->SetMinSize(wSize);
    buttonType = POPUP_NORMAL; //gdlMenuButton is a wxButton --> normal. Bitmaps will be supported starting from 2.9.1 
    theWxContainer = button;
    theWxWidget = button->GetPopupMenu(); //a menu
    button->Enable(sensitive);
  } else {
    wxBitmapButtonGDL *button = new wxBitmapButtonGDL(widgetPanel, widgetID, *bitmap_,
      wOffset, wxDefaultSize, buttonTextAlignment());
    button->SetSize(wSize);
    button->SetMinSize(wSize);
    buttonType = POPUP_BITMAP; //
    theWxContainer = button;
    theWxWidget = button->GetPopupMenu(); //a menu
    button->Enable(sensitive);
  }
  wxWindow *win = dynamic_cast<wxWindow*> (theWxContainer);
  if (win) {
    if (buttonToolTip) win->SetToolTip(wxString((*buttonToolTip)[0].c_str(), wxConvUTF8));
    if (widgetSizer) widgetSizer->Add(win, DONOTALLOWSTRETCH,  widgetAlignment()|wxALL, gdlSPACE); //|wxALL, gdlSPACE_BUTTON);
  } else cerr << "Warning GDLWidgetMenuButton::GDLWidgetMenuButton(): widget type confusion.\n";

//  UpdateGui();
  REALIZE_IF_NEEDED
}

void GDLWidgetMenuButton::SetSensitive(bool value) {
  sensitive = value;
  wxButton* m = dynamic_cast<wxButton*> (theWxContainer);
  if (m) m->Enable(value);
}

void GDLWidgetMenuButton::SetButtonWidgetLabelText( const DString& value ) {
  delete(vValue);
  vValue = new DStringGDL(value);
  wxButton* m=dynamic_cast<wxButton*>(theWxContainer);
  if (m) m->SetLabelText(wxString( value.c_str( ), wxConvUTF8 ));
  this->RefreshDynamicWidget();
}

GDLWidgetMenuButton::~GDLWidgetMenuButton() {
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetMenuButton(" << widgetID << ")" << std::endl;
#endif
    GDLWidgetBase* b=dynamic_cast<GDLWidgetBase*>(GetWidget(parentID));
    if (b && !b->IsContextBase()) if (theWxContainer) static_cast<wxWindow*>(theWxContainer)->Destroy();
  }

void GDLWidgetButton::SetButtonWidgetBitmap( wxBitmap* bitmap_ ) {
  if ( buttonType == BITMAP) {
    wxBitmapButton *b = dynamic_cast<wxBitmapButton*> (theWxWidget);
    if ( b ) {
      b->SetBitmapLabel( *bitmap_ );
      b->SetLabelText(wxEmptyString);
      }
  } else if ( buttonType == POPUP_BITMAP ) {
    wxBitmapButton *b = dynamic_cast<wxBitmapButton*> (theWxContainer); //not the wxWidget since the widget is the popup menu itself.
    if ( b ) {
      b->SetBitmapLabel( *bitmap_ );
      b->SetLabelText(wxEmptyString);
      }
  }  else if ( buttonType == MENU || buttonType ==ENTRY) {
    if ( menuItem != NULL ) {
      menuItem->SetBitmap( *bitmap_ );
      menuItem->SetItemLabel(wxT(" ")); //setting null strings to menus is not permitted !!!!
    }
  }
// Should switch to version > 2.9 now!
#if wxCHECK_VERSION(2,9,1)
  else if ( buttonType == NORMAL || buttonType == POPUP_NORMAL) {
      wxButton *b = dynamic_cast<wxButton*> (theWxWidget);
      if ( b ) {
        b->SetBitmapLabel( *bitmap_ );
        b->SetLabelText(wxEmptyString);
      }
     }
#endif 
  this->RefreshDynamicWidget();
}
  
GDLWidgetList::GDLWidgetList( WidgetIDT p, EnvT* e, BaseGDL *value, DLong style, DULong eventflags )
    : GDLWidget( p, e, value, eventflags)
, maxlinelength(0)
, nlines(0)
{
  GDLWidget* gdlParent = GetWidget( parentID );
  widgetPanel = GetParentPanel( );
  widgetSizer = GetParentSizer( );
  
  #include "start_eventual_frame.incpp" //Widget_LIST
    
  if( vValue->Type() != GDL_STRING)
  {
    vValue = static_cast<DStringGDL*> (vValue->Convert2( GDL_STRING, BaseGDL::CONVERT ));
  }
  DStringGDL* val = static_cast<DStringGDL*> (vValue);

  nlines = val->N_Elements( );
  wxArrayString choices; // = new wxString[n];
  for ( SizeT i = 0; i < nlines; ++i ){
    int length=(*val)[i].length();
    if (length>maxlinelength){
      maxlinelength=length;
    }
    choices.Add( wxString( (*val)[i].c_str( ), wxConvUTF8 ) );
  }
  wxListBox * list=new wxListBox();
  theWxContainer = theWxWidget = list;
  //ok now size can be computed
  list->Create(widgetPanel, widgetID, wxDefaultPosition, wxDefaultSize , choices, style|wxLB_NEEDED_SB|wxLB_SINGLE); //|wxLB_MULTIPLE );
  this->setFont(); //set fancy font before computing sizes!
  wSize=computeWidgetSize();
  list->SetClientSize(wSize);
  list->SetMinClientSize(wSize);
  list->SetSelection(wxNOT_FOUND);//necessary to avoid spurious event on first click.
  this->AddToDesiredEvents( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED,wxCommandEventHandler(gdlwxFrame::OnListBoxDoubleClicked),list);
  this->AddToDesiredEvents( wxEVT_COMMAND_LISTBOX_SELECTED,wxCommandEventHandler(gdlwxFrame::OnListBox),list);
  
  END_ADD_EVENTUAL_FRAME
  TIDY_WIDGET(gdlBORDER_SPACE)
  
//  UpdateGui();
  REALIZE_IF_NEEDED
}
void GDLWidgetList::SetWidgetSize(DLong sizex, DLong sizey)
{ 

  START_CHANGESIZE_NOEVENT
  
  wxWindow* me = dynamic_cast<wxWindow*> (this->GetWxWidget());
  if (me == NULL) {
#ifdef GDL_DEBUG_WIDGETS
    wxMessageOutputStderr().Printf(_T("GDLWidgetList::SetSize of non wxWindow %s\n"), this->GetWidgetName().c_str());
#endif
    return;
  }
  //a graphic window cannot be larger than the base widget it is in, if the base widget size has been fixed
  wxSize currentSize=me->GetSize();
  wxSize currentBestSize=me->GetBestSize();
#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputStderr( ).Printf( _T( "GDLWidgetList::SetSize currentSize=%d,%d \n"),currentSize.x,currentSize.y);
#endif

  //widget text size is in LINES in Y and CHARACTERS in X.
  wxRealPoint widgetSize = wxRealPoint(-1,-1);
  wxSize fontSize = getFontSize();
  //based on experience, actual line height is 1.2 times font y size for fonts > 20 but 1.5 for smaller fonts
  int lineHeight=(fontSize.y<20)?fontSize.y*1.5:fontSize.y*1.2;
  if (sizex > 0) {
    widgetSize.x = (sizex+0.5) * fontSize.x;
  } else {
    widgetSize.x = currentSize.x;
  } 

  if (sizey > 0) {
    widgetSize.y =sizey * lineHeight;
  } else {
    widgetSize.y = currentSize.y;
  }

  if (sizex > 0 && maxlinelength > sizex) widgetSize.y += gdlSCROLL_HEIGHT_X;
  if (nlines > sizey) widgetSize.x += gdlSCROLL_WIDTH_Y;
  widgetSize.y += 10;

  sizex=ceil(widgetSize.x);
  sizey=ceil(widgetSize.y);
  wSize.x = sizex;
  wSize.y = sizey;
  me->SetSize(wSize);
  me->SetMinSize(wSize);
  
  if (!widgetSizer) { if (framePanel) framePanel->Fit();}
  
  UpdateGui();

  END_CHANGESIZE_NOEVENT

#ifdef GDL_DEBUG_WIDGETS
    wxMessageOutputStderr().Printf(_T("GDLWidgetList::SetSize %d,%d (%s)\n"), sizex, sizey, this->GetWidgetName().c_str());
#endif
}

void GDLWidgetList::SetValue(BaseGDL *value){
  GDLDelete(vValue);
  vValue=value;
 if( vValue->Type() != GDL_STRING)
  {
    vValue = static_cast<DStringGDL*> (vValue->Convert2( GDL_STRING, BaseGDL::CONVERT ));
  }
  DStringGDL* val = static_cast<DStringGDL*> (vValue);

  wxArrayString newchoices; // = new wxString[n];
  nlines = val->N_Elements( );
  for ( SizeT i = 0; i < nlines; ++i ){
    int length=(*val)[i].length();
    if (length>maxlinelength){
      maxlinelength=length;
    }
   newchoices.Add( wxString( (*val)[i].c_str( ), wxConvUTF8 ) );
  }

  wxListBox * list = dynamic_cast<wxListBox*>(theWxWidget);
  assert( list != NULL);
  
  list->Clear();
  list->InsertItems(newchoices,0);
  list->SetSelection(wxNOT_FOUND);
}

void GDLWidgetList::SelectEntry(DLong entry_number){
  if (entry_number < 0 ) return;
 wxListBox * list=dynamic_cast<wxListBox*>(theWxWidget);
  assert( list != NULL);
  if (entry_number > list->GetCount()-1) return;
 list->Select(entry_number); 
}

BaseGDL* GDLWidgetList::GetSelectedEntries(){
 wxListBox * list=dynamic_cast<wxListBox*>(theWxWidget);
  assert( list != NULL);
 wxArrayInt selections;
 list->GetSelections(selections);
 DIntGDL* liste;
 if  (selections.Count()<1) {
   liste=new DIntGDL(-1);
   return liste;
 }
 liste=new DIntGDL(dimension(selections.Count()),BaseGDL::NOZERO);
 for (SizeT i=0; i< selections.Count(); ++i) (*liste)[i]=selections[i];
 return liste;
}

GDLWidgetList::~GDLWidgetList(){
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetList(" << widgetID << ")" << std::endl;
#endif
  if (theWxContainer) static_cast<wxWindow*>(theWxContainer)->Destroy();
}

GDLWidgetDropList::GDLWidgetDropList( WidgetIDT p, EnvT* e, BaseGDL *value, DULong eventflags ,
const DString& title_, DLong style_ )
: GDLWidget( p, e, static_cast<DStringGDL*> (value->Convert2( GDL_STRING, BaseGDL::CONVERT )), eventflags)
, title( title_ )
, style( style_)
{
  GDLWidget* gdlParent = GetWidget(parentID);
  widgetPanel = GetParentPanel();
  widgetSizer = GetParentSizer();

  DStringGDL* val = static_cast<DStringGDL*> (vValue);
  DLong n = val->N_Elements();
  wxArrayString choices; // = new wxString[n];
  for (SizeT i = 0; i < n; ++i) {
    choices.Add(wxString((*val)[i].c_str(), wxConvUTF8));
  }
   
///  static int flatIx=e->KeywordIx("FLAT"); ignored.
  bool hastitle=(title.size()>0);

  //get defined sizes if any
  wSize=computeWidgetSize( );
  
  wxChoice * droplist;
  theWxContainer=NULL;
  wxSizer* theSizer=NULL;
#include "start_eventual_frame.incpp"
  if (hastitle) {
    wxPanel* comboPanel=new wxPanel(widgetPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    wxSizer* combosz = new wxBoxSizer(wxHORIZONTAL);
    comboPanel->SetSizer(combosz);
    this->setFont(comboPanel); //enable fancy font
    wxString t= wxString(title.c_str(), wxConvUTF8);
    int w=0;
    int h=0;
    int d=0;
    int lead=0;
    comboPanel->GetTextExtent(t,&w,&h,&d,&lead,&font);
    wxSize TextSize=wxSize(w,h);
    wxStaticText *theTitle=new wxStaticText(comboPanel,wxID_ANY,t, wxDefaultPosition, TextSize, wxST_ELLIPSIZE_END);
    combosz->Add(theTitle,DONOTALLOWSTRETCH,wxALIGN_CENTER|wxALL,gdlSPACE);
    combosz->AddSpacer(5);
    droplist = new wxChoice( comboPanel, widgetID, wxDefaultPosition, wxDefaultSize, choices, style );
    combosz->Add(droplist,DONOTALLOWSTRETCH,wxALIGN_CENTER|wxALL,gdlSPACE);
    comboPanel->Fit();
    theWxWidget = theWxContainer = comboPanel;
    } else {
    droplist = new wxChoice( widgetPanel, widgetID, wxDefaultPosition, wxDefaultSize, choices, style );
    droplist->SetSize(wSize);
    droplist->SetMinSize(wSize);
    theWxWidget = theWxContainer = droplist;
  }
  
    END_ADD_EVENTUAL_FRAME
TIDY_WIDGET(gdlBORDER_SPACE);
    theWxWidget = droplist;
  droplist->SetSelection(0);
  this->AddToDesiredEvents(  wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(gdlwxFrame::OnDropList),droplist);

//  UpdateGui();
  REALIZE_IF_NEEDED
}
  
void GDLWidgetDropList::SetValue(BaseGDL *value){
   GDLDelete(vValue);
  vValue=value;
 if( vValue->Type() != GDL_STRING)
  {
    vValue = static_cast<DStringGDL*> (vValue->Convert2( GDL_STRING, BaseGDL::CONVERT ));
  }
  DStringGDL* val = static_cast<DStringGDL*> (vValue);
  wxArrayString newchoices; // = new wxString[n];
  for ( SizeT i = 0; i < val->N_Elements( ); ++i )  newchoices.Add( wxString( (*val)[i].c_str( ), wxConvUTF8 ) );

  wxChoice* droplist=dynamic_cast<wxChoice*>(theWxWidget);
  assert( droplist != NULL);
  droplist->Clear();
  droplist->Append(newchoices);
  droplist->SetSelection(0);
  //IDL behaviour  with DynamicResize is identical
  if (this->IsDynamicResize()) {
    wSize=droplist->GetSizeFromTextSize(droplist->GetTextExtent(newchoices[0]).GetWidth());
    droplist->SetMinSize(wSize);
    droplist->SetSize(wSize);
   }
  RefreshDynamicWidget();
}

void GDLWidgetDropList::SelectEntry(DLong entry_number){
 wxChoice * droplist=dynamic_cast<wxChoice*>(theWxWidget);
 assert( droplist != NULL);
 droplist->Select(entry_number); 
}

BaseGDL* GDLWidgetDropList::GetSelectedEntry(){
  wxChoice * droplist=dynamic_cast<wxChoice*>(theWxWidget);
  assert( droplist != NULL);
  return new DIntGDL(droplist->GetSelection());
}

GDLWidgetDropList::~GDLWidgetDropList(){
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetDropList(" << widgetID << ")" << std::endl;
#endif
    if (theWxContainer) static_cast<wxWindow*>(theWxContainer)->Destroy();
}
  
GDLWidgetComboBox::GDLWidgetComboBox( WidgetIDT p, EnvT* e, BaseGDL *value, DULong eventflags, DLong style_ )
: GDLWidget( p, e, static_cast<DStringGDL*> (value->Convert2( GDL_STRING, BaseGDL::CONVERT )) , eventflags)
, style( style_)
{
  GDLWidget* gdlParent = GetWidget( parentID );
  widgetPanel = GetParentPanel( );
  widgetSizer = GetParentSizer( );
  
 #include "start_eventual_frame.incpp" //Widget_COMBOBOX
    
  DStringGDL* val = static_cast<DStringGDL*> (vValue);
  DLong n = val->N_Elements( );
  wxArrayString choices; // = new wxString[n];
  for ( SizeT i = 0; i < n; ++i )
  choices.Add( wxString( (*val)[i].c_str( ), wxConvUTF8 ) );

  wxString val0WxString = wxString( (*val)[0].c_str( ), wxConvUTF8 );

  wxComboBox * combobox = new wxComboBox();
  combobox->Create(widgetPanel, widgetID, val0WxString, wxDefaultPosition, wxDefaultSize, choices, style );  

  
  theWxContainer = theWxWidget = combobox;
  this->AddToDesiredEvents( wxEVT_COMMAND_COMBOBOX_SELECTED,wxCommandEventHandler(gdlwxFrame::OnComboBox),combobox);
  if (style & wxTE_PROCESS_ENTER) this->AddToDesiredEvents( wxEVT_COMMAND_TEXT_ENTER,wxCommandEventHandler(gdlwxFrame::OnComboBoxTextEnter),combobox);
  wSize=computeWidgetSize();
  if (widgetSizer==NULL) {
    if (wSize.x < 1) wSize.x=gdlDEFAULT_XSIZE+gdlCOMBOBOX_ARROW_WIDTH;
  }
  combobox->SetSize(wSize);
  combobox->SetMinSize(wSize);
  
  END_ADD_EVENTUAL_FRAME
  TIDY_WIDGET(gdlBORDER_SPACE)
//  UpdateGui();
  REALIZE_IF_NEEDED
}

inline wxSize GDLWidgetComboBox::computeWidgetSize() {
  //here is a good place to make dynamic widgets static, since dynamic_resize is permitted only if size is not given.
  if (wSize.x > 0 || wSize.y > 0 || wScreenSize.x > 0 || wScreenSize.y > 0) dynamicResize = -1;
  wxSize widgetSize;
  if (wSize.x > 0) widgetSize.x = wSize.x * unitConversionFactor.x;
  else widgetSize.x = wxDefaultSize.x;
  //but..
  if (wScreenSize.x > 0) widgetSize.x = wScreenSize.x;

  if (wSize.y > 0) widgetSize.y = wSize.y * unitConversionFactor.y;
  else widgetSize.y = wxDefaultSize.y;
  //but..
  if (wScreenSize.y > 0) widgetSize.y = wScreenSize.y;
//Specific to macOSX: combobox must be small.
#ifdef __WXMAC__
  //MAC OSX complains about large Y comboboxes.
  if (widgetSize.y > 12) widgetSize.y = -1;
#endif  
  return widgetSize;
}
void GDLWidgetComboBox::SetValue(BaseGDL *value){
  GDLDelete(vValue);
  vValue=value;
 if( vValue->Type() != GDL_STRING)
  {
    vValue = static_cast<DStringGDL*> (vValue->Convert2( GDL_STRING, BaseGDL::CONVERT ));
  }
  DStringGDL* val = static_cast<DStringGDL*> (vValue);
  wxArrayString newchoices; // = new wxString[n];
  for ( SizeT i = 0; i < val->N_Elements( ); ++i )  newchoices.Add( wxString( (*val)[i].c_str( ), wxConvUTF8 ) );

  wxComboBox * combo = dynamic_cast<wxComboBox*>(theWxWidget);
  assert( combo != NULL);
  combo->Clear();
  combo->Append(newchoices);
  combo->SetSelection(0,0);
  combo->SetValue(newchoices[0]);
  //IDL behaviour  with DynamicResize is different as Combobox's size never decreases with this method, it cannot be smaller than the initial size.
  //this would plead to have a new wInitialSize for which wSize cannot be smaller her, but is reset to new values with SetSize. Oh well.
  if (this->IsDynamicResize()) {
    wSize=combo->GetSizeFromTextSize(combo->GetTextExtent(newchoices[0]).GetWidth());
    combo->SetMinSize(wSize);
    combo->SetSize(wSize);
  }
  RefreshDynamicWidget();
  }

void GDLWidgetComboBox::AddItem(DString value, DLong pos) {
  wxComboBox * combo = dynamic_cast<wxComboBox*>(theWxWidget);
  assert( combo != NULL);
  int nvalues=combo->GetCount();
  if (pos == -1) combo->Append( wxString( value.c_str( ), wxConvUTF8 ) );
  else if (pos > -1  && pos < nvalues) combo->Insert( wxString( value.c_str( ), wxConvUTF8 ), pos);
}

void GDLWidgetComboBox::DeleteItem(DLong pos) {
  wxComboBox * combo = dynamic_cast<wxComboBox*>(theWxWidget);
  assert( combo != NULL);
  int nvalues=combo->GetCount();
  int selected=combo->GetSelection();
  if (pos > -1  && pos < nvalues) combo->Delete( pos );
  if (pos==selected && (nvalues-1) > 0 ) combo->Select((pos+1)%(nvalues-1)); else if (selected==-1 && pos==0) combo->Select(0);
}

void GDLWidgetComboBox::SelectEntry(DLong entry_number){
 wxComboBox * combo=dynamic_cast<wxComboBox*>(theWxWidget);
  assert( combo != NULL);
 combo->Select(entry_number); 
}

BaseGDL* GDLWidgetComboBox::GetSelectedEntry(){
  wxComboBox * combo=dynamic_cast<wxComboBox*>(theWxWidget);
  assert( combo != NULL);
  DStringGDL* stringres=new DStringGDL(dimension(1));
  (*stringres)[0]=combo->GetStringSelection().mb_str(wxConvUTF8); //return null string if no selection
  return stringres;
}

GDLWidgetComboBox::~GDLWidgetComboBox(){
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetComboBox(" << widgetID <<")"<< std::endl;
#endif
  if (theWxContainer) static_cast<wxWindow*>(theWxContainer)->Destroy();
}

GDLWidgetText::GDLWidgetText( WidgetIDT p, EnvT* e, DStringGDL* valueStr, DULong eventflags, bool noNewLine_,
bool editable_ )
: GDLWidget( p, e, valueStr, eventflags )
, noNewLine( noNewLine_ )
, editable(editable_)
, multiline(false)
, maxlinelength(0)
{
  static int wrapIx=e->KeywordIx("WRAP");
  wrapped=(e->KeywordSet(wrapIx));
  DString value = "";
  nlines=1;
  //textSize is the current size (fixed or computed) in characters.
  textSize=wSize;
  if (textSize.y > 1) multiline=true; //multiline is a property implied by the widget Ysize, nothing else
  //reform entries into one string, with eventual \n . If noNewLines, do not insert a \n . If ysize=1 and no scroll, idem.
  bool doNotAddNl=(noNewLine || (!multiline) );
  
  if( vValue != NULL)
  {
      for( int i=0; i<vValue->N_Elements(); ++i)
      {
        int length=((*static_cast<DStringGDL*> (vValue))[i]).length();
        value += (*static_cast<DStringGDL*> (vValue))[i]; 
        if ( !doNotAddNl) maxlinelength=(length>maxlinelength)?length:maxlinelength; else maxlinelength+=length;
        if ( !doNotAddNl )
#ifdef _WIN32
        {value += "\r\n"; nlines++;}
#else
        {value += '\n'; nlines++;}
#endif
      }
  }

  lastValue = value;
  
  //now the string is formatted as the widget will see it. If the string contains \n  the widget will be multiline anyway.
  //recompute number of \n as some could have been embedded in each of the strings
  //recompute nlines, maxlinelength from start to be sure
  nlines=1;
  maxlinelength=0;
  const char* s=lastValue.c_str();
  int length=0;
  for (int i=0; i<lastValue.length(); ++i, ++length)
  {
    if (s[i]==10) {
      maxlinelength=(length>maxlinelength)?length:maxlinelength;
      nlines++;
      length=0;
    }
  }
  if (length>maxlinelength) maxlinelength=length; //if no last return.

  GDLWidget* gdlParent = GetWidget( parentID );
  widgetPanel = GetParentPanel( );
  widgetSizer = GetParentSizer( );
  
  #include "start_eventual_frame.incpp" //Widget_TEXT
  
  bool report=((eventFlags & GDLWidget::EV_ALL)==1);
  
 //for text, apparently, if   wxTE_MULTILINE is in effect, the font handler is probably RichText 
 //and the text SIZE is OK only if imposed by a wxTextAttr. (go figure).
 //note that computeWidgetSize() above returns sizes "compatible" with the font size, without any trouble.
  wxTextAttr attr= wxTextAttr ();
  attr.SetFont(font, 	wxTEXT_ATTR_FONT);
  wxString valueWxString = wxString( lastValue.c_str( ), wxConvUTF8 );
  long textStyle = wxTE_PROCESS_ENTER|wxTE_RICH2|wxTE_NOHIDESEL; //PROCESS_ENTER is trapped and ignored --- check if this is OK for all platforms.

  if (multiline) textStyle |= wxTE_MULTILINE; 
  if (!scrolled) {textStyle |= wxTE_NO_VSCROLL;} else {textStyle |= wxHSCROLL;}
  if (wrapped) textStyle |= wxTE_WORDWRAP; else textStyle |= wxTE_DONTWRAP;
  if (!editable && !report ) textStyle |= wxTE_READONLY;

  wxTextCtrlGDL* text = new wxTextCtrlGDL(widgetPanel, widgetID, wxEmptyString/*valueWxString*/, wxDefaultPosition, wxDefaultSize, textStyle);
  // above, permits to a procedure (cw_field.pro for example) to filter the editing of text fields.
  // so wxTextCtrl::OnChar is overwritten in gdlwidgeteventhandler.cpp just for this reason.
  theWxContainer = theWxWidget = text;
  this->setFont();
  if ((textStyle & wxTE_MULTILINE)==0) text->SetMargins(wxPoint(gdlTEXT_XMARGIN,gdlTEXT_YMARGIN)); //without the if clause, provokes: GLib-GObject-WARNING **: 23:04:21.080: invalid cast from 'GtkTextView' to 'GtkEntry' (and does not work)
  text->SetDefaultStyle(attr);
  text->SetValue(valueWxString);

  wSize = computeWidgetSize(); //updates textSize.
  text->SetClientSize(wSize);
  text->SetMinClientSize(wSize);
  
  text->SetSelection(0,0);
  text->SetInsertionPoint(0);
  text->ShowPosition(0);
  
  END_ADD_EVENTUAL_FRAME
  TIDY_WIDGET(gdlBORDER_SPACE)

  this->AddToDesiredEvents( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(gdlwxFrame::OnTextEnter),text); //NOT USED
  if( editable || report) this->AddToDesiredEvents(wxEVT_CHAR, wxKeyEventHandler(wxTextCtrlGDL::OnChar),text);
  //add scrolling even if text is not scrollable since scroll is not permitted (IDL widgets are not at all the same as GTK...)
  if (!scrolled) this->AddToDesiredEvents(wxEVT_MOUSEWHEEL, wxMouseEventHandler(wxTextCtrlGDL::OnMouseEvents),text);
  this->AddToDesiredEvents( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(gdlwxFrame::OnText),text);
  if (report) this->AddToDesiredEvents( wxEVT_COMMAND_TEXT_PASTE, wxClipboardTextEventHandler(gdlwxFrame::OnTextPaste),text);
  if (report) this->AddToDesiredEvents( wxEVT_COMMAND_TEXT_CUT, wxClipboardTextEventHandler(gdlwxFrame::OnTextCut),text);
  if (report) this->AddToDesiredEvents( wxEVT_LEFT_DOWN, wxMouseEventHandler(gdlwxFrame::OnTextMouseEvents),text); 
  if (report) this->AddToDesiredEvents( wxEVT_LEFT_UP, wxMouseEventHandler(gdlwxFrame::OnTextMouseEvents),text); 
  if (report) this->AddToDesiredEvents( wxEVT_MIDDLE_DOWN, wxMouseEventHandler(gdlwxFrame::OnTextMouseEvents),text); 

//  UpdateGui();
  REALIZE_IF_NEEDED
}
void GDLWidgetText::SetWidgetSize(DLong sizex, DLong sizey)
{ 

  START_CHANGESIZE_NOEVENT

  wxWindow* me = dynamic_cast<wxWindow*> (this->GetWxWidget());
  if (me == NULL) {
#ifdef GDL_DEBUG_WIDGETS
    wxMessageOutputStderr().Printf(_T("GDLWidgetText::SetSize of non wxWindow %s\n"), this->GetWidgetName().c_str());
#endif
    return;
  }
  //widget text size is in LINES in Y and CHARACTERS in X. But overridden by scr_xsize et if present
  wxRealPoint widgetSize = wxRealPoint(-1,-1);
  static std::string testExtent("M");
  wxSize fontSize=calculateTextScreenSize(testExtent); //use text extent of an EM for realistic fontSize. 
  int lineHeight=fontSize.y;
  wxSize currentSize=me->GetClientSize();

  //for each x or y, if >0 take the new value in textSize, and correct wSize to take into account possible wxWidgets-induced scrollbars.
  // if ==0 take the initial textSize value
  // if < 0 use current textSize.
  // all pixels values are recomputed as we need to take care of spurious scrollbars.
  if (sizex > 0) textSize.x=sizex; else if (sizex == 0) textSize.x=initialSize.x;

  if (sizey > 0) textSize.y=sizey; else if (sizey == 0) textSize.y=initialSize.y;
  
  //proceed as in computeWidgetSize() , same code, except that textSize is never -1.
  widgetSize.x = (textSize.x) * fontSize.x;
  if (textSize.y > 1)  widgetSize.y = textSize.y * lineHeight; else widgetSize.y = lineHeight;

  if (textSize.y == 1) widgetSize.y+=2*gdlTEXT_SPACE; //for margin
  if (scrolled && textSize.y > 1) widgetSize.x+=gdlSCROLL_WIDTH_Y;

  //if multiline and no hscroll, a x-axis scrollbar will be added by wxWidgets if longestLineSize cannot be shown, and we cannot do anything about it.
  //multiline here is "if (multiline) textStyle |= wxTE_MULTILINE;"
  if (!scrolled) {if (textSize.y > 1 && (textSize.x < maxlinelength) )  widgetSize.y += gdlSCROLL_HEIGHT_X; }
  else if (textSize.y > 1 && widgetSize.x < (maxlinelength*fontSize.x+gdlSCROLL_WIDTH_Y) ) {
    widgetSize.y += gdlSCROLL_HEIGHT_X; 
  }
  widgetSize.x+=2*gdlTEXT_SPACE; //for margin
  
  int x = ceil(widgetSize.x);
  int y = ceil(widgetSize.y);
  wSize = wxSize(x,y); //update current widgetSize.
  me->SetClientSize(wSize);
  me->SetMinClientSize(wSize);
  if (!widgetSizer) if (framePanel) framePanel->Fit();

  UpdateGui();

  END_CHANGESIZE_NOEVENT
  
#ifdef GDL_DEBUG_WIDGETS
    wxMessageOutputStderr().Printf(_T("GDLWidgetList::SetSize %d,%d (%s)\n"), sizex, sizey, this->GetWidgetName().c_str());
#endif
}

GDLWidgetText::~GDLWidgetText(){
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetText(" << widgetID <<")"<< std::endl;
#endif
  if (theWxContainer) static_cast<wxWindow*>(theWxContainer)->Destroy();
}

void GDLWidgetText::ChangeText( DStringGDL* valueStr, bool noNewLine_)
{
  delete vValue;
  vValue = valueStr;
  DString value = "";

  bool doNotAddNl=(noNewLine_ || (!multiline) );

  nlines=0; 
    for( int i=0; i<valueStr->N_Elements(); ++i)
    {
    value += (*valueStr)[ i];
    if ( !doNotAddNl )
#ifdef _WIN32
    {value += "\r\n"; nlines++;}
#else
    {value += '\n'; nlines++;}
#endif
    }
  lastValue = value;
  //recompute nlines, maxlinelength from start to be sure
  nlines=1;
  maxlinelength=0;
  const char* s=lastValue.c_str();
  int length=0;
  for (int i=0; i<lastValue.length(); ++i, ++length)
  {
    if (s[i]==10) {
      maxlinelength=(length>maxlinelength)?length:maxlinelength;
      nlines++;
      length=0;
    }
  }
  if (length>maxlinelength) maxlinelength=length; //if no last return.
  
  wxString valueWxString = wxString( lastValue.c_str( ), wxConvUTF8 );
  if ( theWxWidget != NULL ) {
    wxTextCtrl* txt=dynamic_cast<wxTextCtrl*> (theWxWidget);
    assert( txt != NULL);
    txt->ChangeValue( valueWxString ); //by contrast with SetValue, does not generate an EVENT -- IDL does not either.
  }  else std::cerr << "Null widget in GDLWidgetText::SetTextValue(), please report!" << std::endl;
//update widgetSize. Important to deal with 'added' scrollbars.
  this->SetWidgetSize(-1,-1);
}

void GDLWidgetText::InsertText( DStringGDL* valueStr, bool noNewLine_, bool append) {
  wxTextCtrl* txt = dynamic_cast<wxTextCtrl*> (theWxWidget);
  assert(txt != NULL);
  long from, to;
  
  //wxWidgets bug?
  wxTextPos pos=txt->GetLastPosition();
  if (pos<1) {
    GDLWidgetText::ChangeText(valueStr,noNewLine_);
    return;
  }
  
  //really append/replace
  if (append) { //see discussion wxTextEntry::GetInsertionPoint() at https://docs.wxwidgets.org/trunk/classwx_text_entry.html
	//changes suggested by @klimpel (on windows)
	if (multiline) {
	  from = pos - 1;
	  to = pos;
	} else {
	  from = pos;
	  to = pos;
	}
  }
  bool doNotAddNl=(noNewLine_ || (!multiline) );

  DString value = (doNotAddNl)?"":(!append)?"":"\n";
  for ( int i = 0; i < valueStr->N_Elements( ); ++i ) {
    int length=(*valueStr)[ i].length();
    value += (*valueStr)[ i];
    if ( !doNotAddNl ) {
#ifdef _WIN32
      value += "\r\n"; nlines++;
#else
      value += '\n'; nlines++;
#endif
    }
  }
  //note value.size it will be used
  int insertedLength=value.size();
  lastValue.replace(std::min(size_t(from),lastValue.size()),to-from,value); //avoid the exception even if lastValue would still be wrong
  //recompute nlines, maxlinelength from start to be sure
  nlines=1;
  maxlinelength=0;
  const char* s=lastValue.c_str();
  int length=0;
  for (int i=0; i<lastValue.length(); ++i, ++length)
  {
    if (s[i]==10) {
      maxlinelength=(length>maxlinelength)?length:maxlinelength;
      nlines++;
      length=0;
    }
  }
  if (length>maxlinelength) maxlinelength=length; //if no last return.
  delete vValue;
  vValue = new DStringGDL(lastValue);
  wxString valueWxString = wxString( lastValue.c_str( ), wxConvUTF8 );
  if ( theWxWidget != NULL ) {
    txt->ChangeValue( valueWxString ); //by contrast with SetValue, does not generate an EVENT (neither does *DL).    
    txt->SetSelection(from+insertedLength,from+insertedLength);
    txt->ShowPosition(from+insertedLength);
  }  else std::cerr << "Null widget in GDLWidgetText::SetTextValue(), please report!" << std::endl;
//update widgetSize
  this->SetWidgetSize(-1,-1);
}

void GDLWidgetText::SetTextSelection(DLongGDL* pos)
{ //*DL selection are [start,length] [3,20] -> pos (3,23)
  //numerous cases to handle with invalid values (negative, too large).
  //Hope it works for all cases:
  long from,to,len;
  long oldSelStart, oldSelEnd;
  wxTextCtrl* txt=dynamic_cast<wxTextCtrl*> (theWxWidget);
  assert( txt != NULL);
  
  txt->GetSelection(&oldSelStart,&oldSelEnd);
  len = txt->GetLastPosition()+1;

  if ((*pos)[0] < 0 ) from=0;
  else if ((*pos)[0] > len ) from=len;
  else from=(*pos)[0];

  
  if (pos->N_Elements()<2) {
    //selection = insertion
    txt->SetSelection(from,from);
    return;
  }
  //2 elements, define selection
  to=(*pos)[1];

  if ( from+to >= len ) {
    //set insertion position at end of last selection (?)
    txt->SetSelection(oldSelEnd,oldSelEnd);
    return;
  }
  if ( to < 0 ) {
    //set insertion position at from
    txt->SetSelection(from,from);
    return;
  }
  txt->SetSelection(from,from+to);
}

DLongGDL* GDLWidgetText::GetTextSelection()
{
  DLongGDL* pos=new DLongGDL(dimension(2),BaseGDL::ZERO);
  long from,to;
  wxTextCtrl* txt=dynamic_cast<wxTextCtrl*> (theWxWidget);
  assert( txt != NULL);
  
  txt->GetSelection(&from,&to);
  (*pos)[0]=from; (*pos)[1]=to-from;
  return pos;
}

DStringGDL* GDLWidgetText::GetSelectedText()
{
  wxTextCtrl* txt=dynamic_cast<wxTextCtrl*>(theWxWidget);
  assert( txt != NULL);
  
return new DStringGDL(txt->GetStringSelection().mb_str(wxConvUTF8).data());
}

GDLWidgetLabel::GDLWidgetLabel( WidgetIDT p, EnvT* e, const DString& value_ , DULong eventflags, bool sunken_)
: GDLWidget( p, e , new DStringGDL(value_), eventflags )
, value(value_)
, sunken(sunken_)
{
  GDLWidget* gdlParent = GetWidget( parentID );
  widgetPanel = GetParentPanel( );
  widgetSizer = GetParentSizer( );

  if (sunken) frameWidth=0;

  const wxString valueWxString = wxString( value.c_str( ), wxConvUTF8 );

  bool simplelabel=(wSize.x < 0 && frameWidth <1 && !sunken); //before recomputing wSize!
  wSize=computeWidgetSize();
  //to comply with the positioning observed with IDL, a label with a defined size is in fact a label inside a (possibly invisible) frame of such size.
  //the centering of the label is 'center' by default and does not inherit from the parent base default centering (it is the frame that inherits), but if
  //a centering is required, it is done.
  //labels with no particular size and frames are made idem.
  //label with no particular size and no frame are aligned normally, i.e., they inherit if the alignment is not precised.
  // the simple case is
  if (simplelabel) {
    wxStaticText* label = new wxStaticText( widgetPanel, widgetID, wxT(""), wOffset, wxDefaultSize, wxST_NO_AUTORESIZE);
    label->SetLabelText(valueWxString);
    label->SetSize(wSize);
    label->SetMinSize(wSize);
    label->Wrap(-1);
    theWxContainer = theWxWidget = label;
    if (widgetSizer) widgetSizer->Add(label, ALLOWSTRETCH, widgetAlignment()|wxALL, gdlSPACE);
    if (widgetSizer) widgetSizer->Fit(label); else widgetPanel->Fit();
//    UpdateGui();
    REALIZE_IF_NEEDED
    return;
  } 
  //Now more complicated. Framed is not Sunken. Framed implies adding 2 panels, one 'extruded'.
  //the inside panel is of (eventually fixed) size wSize. It contains the text, aligned.
  //sizers should link all those sizes
#include "start_eventual_frame.incpp"
  wxStaticText* label;

  wxPanel* hiddenPanel = new wxPanel(widgetPanel, wxID_ANY, wOffset, wxDefaultSize, sunken?wxBORDER_SUNKEN:wxBORDER_NONE);
    wxSize fsize=wSize+wxSize(2,0);
    hiddenPanel->SetSize(fsize);
    hiddenPanel->SetMinSize(fsize);
    wxSizer* sz = new wxBoxSizer(wxVERTICAL);
    hiddenPanel->SetSizer(sz);
    //the label, with special style inside
    //create and position label in panel
    label = new wxStaticText(hiddenPanel, widgetID, valueWxString, wOffset, wxDefaultSize, labelTextAlignment());
    label->SetLabelText(valueWxString);
    label->SetSize(wSize);
    label->SetMinSize(wSize);
    label->Wrap(-1);
    sz->Add(label, DONOTALLOWSTRETCH, wxALL, 0);
    sz->Fit(label);
    theWxContainer=theWxWidget=hiddenPanel; //redefine theWxWidget at end -- this is only for END_ADD_EVENTUAL_FRAME 

    END_ADD_EVENTUAL_FRAME
TIDY_WIDGET(gdlBORDER_SPACE);
theWxWidget=label; //good value for theWxWidget

//  UpdateGui();
  REALIZE_IF_NEEDED
}

GDLWidgetLabel::~GDLWidgetLabel(){
#ifdef GDL_DEBUG_WIDGETS
    std::cout << "~GDLWidgetLabel(" << widgetID << ")" << std::endl;
#endif 
    if (theWxContainer) static_cast<wxWindow*>(theWxContainer)->Destroy();
}
void GDLWidgetLabel::SetWidgetSize(DLong sizex, DLong sizey) //in pixels. Always.
{
  START_CHANGESIZE_NOEVENT
  //Sizes are in pixels. Units must be converted before calling this function.
  wxWindow* me=static_cast<wxWindow*>(theWxContainer);
  //a graphic window cannot be larger than the base widget it is in, if the base widget size has been fixed
  wxSize currentSize=me->GetSize();
  wxSize currentBestSize=me->GetBestSize();
#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputStderr( ).Printf( _T( "GDLWidgetLabel::SetSize currentSize=%d,%d\n"),currentSize.x,currentSize.y);
#endif
  //unprecised sizes do not change
  if (sizex == -1) sizex = currentSize.x;
  if (sizey == -1) sizey = currentSize.y;
  
  if (widgetSizer) {
    //passed zeroes = mr sizer, do your best..
    if (sizex == 0) sizex = -1;
    if (sizey == 0) sizey = -1;
  } else {
    //passed zeroes = your best is to keep best size (in cache, value defined at creation)
    if (sizex == 0) sizex = currentBestSize.x;
    if (sizey == 0) sizey = currentBestSize.y;
  }
  wSize.x = sizex;
  wSize.y = sizey;
  me->SetSize(wSize);
  //the sizer takes care of the eventual frame
  me->SetMinSize(wSize);

  UpdateGui();

  END_CHANGESIZE_NOEVENT
  

#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputStderr( ).Printf( _T( "GDLWidget::SetSize %d,%d (%s)\n"),sizex,sizey,this->GetWidgetName().c_str());
#endif
}

void GDLWidgetLabel::SetLabelValue(const DString& value_) {
  value = value_;
  //update vValue
  delete(vValue);
  vValue = new DStringGDL(value);

  wxString valueWxString = wxString(value_.c_str(), wxConvUTF8);
  if (theWxWidget != NULL) {
    wxStaticText *label = dynamic_cast<wxStaticText*> (theWxWidget);
    if (label) {  
      label->SetLabelText(valueWxString);
      if (this->IsDynamicResize()) {
        wSize = updateDynamicWidgetSize();
        label->SetMinSize(wSize);
        label->SetSize(wSize);
        RefreshDynamicWidget();
      }
    }
    else cerr << "Warning GDLWidgetLabel::SetLabelValue(): widget type confusion\n";
    //SetLabel, unless SetLabelText, interprets markup (<b></b> etc)
  } else std::cerr << "Null widget in GDLWidgetLabel::SetLabelValue(), please report!" << std::endl;
}

//propertysheet. Unsupported as need to be completely rewritten as the other widgets are (new code) and not able to test.
#ifdef HAVE_WXWIDGETS_PROPERTYGRID
//
// GDLWidgetPropertySheet::GDLWidgetPropertySheet( WidgetIDT parentID, EnvT* e)
// : GDLWidget( p, e , value, eventflag)
//{
//  GDLWidget* gdlParent = GetWidget( parentID );
//  widgetPanel = GetParentPanel( );
//  widgetSizer = GetParentSizer( );
//  topWidgetSizer = this->GetTopLevelBaseWidget(parentID)->GetSizer();
// // Construct wxPropertyGrid control
//  wxPropertyGrid* pg = new wxPropertyGrid(gdlParent,widgetID,wxDefaultPosition,wxDefaultSize,
//  // Here are just some of the supported window styles
//  wxPG_AUTO_SORT | // Automatic sorting after items added
//  wxPG_SPLITTER_AUTO_CENTER | // Automatically center splitter until user manually adjusts it
//  // Default style
//  wxPG_DEFAULT_STYLE );
//// Window style flags are at premium, so some less often needed ones are
//// available as extra window styles (wxPG_EX_xxx) which must be set using
//// SetExtraStyle member function. wxPG_EX_HELP_AS_TOOLTIPS, for instance,
//// allows displaying help strings as tool tips.
//  pg->SetExtraStyle( wxPG_EX_HELP_AS_TOOLTIPS );
//  
//  theWxWidget=pg;
//  
//  // Add int property
//pg->Append( new wxIntProperty("IntProperty", wxPG_LABEL, 12345678) );
//// Add float property (value type is actually double)
//pg->Append( new wxFloatProperty("FloatProperty", wxPG_LABEL, 12345.678) );
//// Add a bool property
//pg->Append( new wxBoolProperty("BoolProperty", wxPG_LABEL, false) );
//// A string property that can be edited in a separate editor dialog.
//pg->Append( new wxLongStringProperty("LongStringProperty",
//wxPG_LABEL,
//"This is much longer string than the "
//"first one. Edit it by clicking the button."));
//// String editor with dir selector button.
//pg->Append( new wxDirProperty("DirProperty", wxPG_LABEL, ::wxGetUserHome()) );
//// wxArrayStringProperty embeds a wxArrayString.
//pg->Append( new wxArrayStringProperty("Label of ArrayStringProperty",
//"NameOfArrayStringProp"));
//// A file selector property.
//pg->Append( new wxFileProperty("FileProperty", wxPG_LABEL, wxEmptyString) );
//
//UpdateGui(); REALIZE_IF_NEEDED;
//}
#endif
// GDL widgets =====================================================
// GDLFrame ========================================================
gdlwxFrame::gdlwxFrame( wxWindow* parent, GDLWidgetTopBase* gdlOwner_, wxWindowID id, const wxString& title , const wxPoint& pos, const wxSize& size, long style, bool modal)
: wxFrame()
, mapped( false )
, frameSize(size)
, gdlOwner( gdlOwner_),
  m_windowDisabler(NULL)
{
  Create ( parent, id, title, pos, size, style );
  m_resizeTimer = new wxTimer(this,RESIZE_TIMER);
  if (modal) m_windowDisabler = new wxWindowDisabler(this);
}

gdlwxFrame::~gdlwxFrame()
{ 
    if (m_resizeTimer->IsRunning()) m_resizeTimer->Stop(); 
#ifdef GDL_DEBUG_WIDGETS
    std::cout << "~GDLFrame: " << this << std::endl;
#endif  
 //frame is part of a TLB. if frame is destroyed, destroy TLB if still existing. 
 if( gdlOwner != NULL)
  {
#ifdef GDL_DEBUG_WIDGETS
    std::cout << "~GDLFrame: Destroying Base Container" << gdlOwner->GetWidgetID() << std::endl;
#endif
//    gdlOwner->NullWxWidget( ); //remove one's reference from container
//    gdlOwner->SelfDestroy( ); // send delete request to GDL owner = container.
  }
 }
// Phantom frame 
gdlwxPhantomFrame::gdlwxPhantomFrame()
: wxFrame(NULL,wxID_ANY,wxString("phantom"),wxDefaultPosition,wxDefaultSize,wxFRAME_TOOL_WINDOW)
{
  wxScrolled<wxPanel>* test = new wxScrolled<wxPanel>(this); //to measure scrollbars
  test->ShowScrollbars(wxSHOW_SB_ALWAYS, wxSHOW_SB_ALWAYS);
}

void gdlwxPhantomFrame::Realize() {
  wxWindow* c=this->GetChildren()[0];
  //retrieve toolkit's width and height of scrollbars (useful to calculate sizes for widgets)
  wxSize s=c->GetSize();
  wxSize e=c->GetClientSize();
  wxSize w=s-e;
//  std::cerr<<w.x<<","<<w.y<<std::endl;
  if (w.x <= 0) w.x=gdlABSENT_SIZE_VALUE;
  if (w.y <= 0) w.y=gdlABSENT_SIZE_VALUE;
  sysScrollHeight=w.y;
  sysScrollWidth=w.x;
  wxColour color=c->GetBackgroundColour();
  sysPanelDefaultColour=color.GetRGB();
//  std::cerr<<sysPanelDefaultColour<<std::endl;
}

// Frame for Plots ========================================================
gdlwxPlotFrame::gdlwxPlotFrame( const wxString& title , const wxPoint& pos, const wxSize& size, long style, bool scrolled_)
: wxFrame( NULL, wxID_ANY, title, pos, size, style )
, scrolled(scrolled_)
{
  m_resizeTimer = new wxTimer(this,RESIZE_PLOT_TIMER);
  this->SetIcon(wxgdlicon);
}

gdlwxPlotFrame::~gdlwxPlotFrame() {
  //We are here outside gdl widget hierarchy. If we Destroy the frame, everything goes.
  // However, we MUST insure this is equivalent to "WDELETE" because the complicated interplay of multidevices, plplot streams, and widgets.
  // So it is better to pass and wait for the deletion of the contained window id (by marking it unValid) and let TidyWindowsList clear everything.
  // this widget will be actually destroyed when the inside GraphicsPanel will be destroyed.
  if (m_resizeTimer->IsRunning()) m_resizeTimer->Stop();
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~gdlwxPlotFrame: " << this << std::endl;
#endif
}

void gdlwxPlotFrame::Realize() {
 this->SetClientSize(this->GetClientSize());
#ifdef GDL_DEBUG_WIDGETS
    wxMessageOutputStderr().Printf(_T("gdlwxPlotFrame:Realize\n"));
#endif
}

//version using wxBG_STYLE_PAINT and blit to an AutoBufferedPaintDC, will this improve speed?
gdlwxGraphicsPanel::gdlwxGraphicsPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
: wxScrolled<wxPanel>() // Use default ctor here!
, pstreamIx( -1 )
, pstreamP( NULL )
, wx_dc( NULL)
, drawSize(size)
{ 
        // Do this first:
        SetBackgroundStyle(wxBG_STYLE_PAINT);
  Create( parent, id, pos, size, style, name );
}
//gdlwxGraphicsPanel::gdlwxGraphicsPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
//: wxScrolled<wxPanel>( parent, id, pos, size, style, name )
//, pstreamIx( -1 )
//, pstreamP( NULL )
//, m_dc( NULL)
//, drawSize(size)
//{ }
GDLWXStream* gdlwxGraphicsPanel::GetStream(){return pstreamP;};
void gdlwxGraphicsPanel::DeleteUsingWindowNumber(){
  pstreamP->SetValid(false);
  GraphicsDevice::GetGUIDevice()->TidyWindowsList(); //tidy Window List will delete widget by itself
}
void gdlwxGraphicsPanel::SetUndecomposed(){
  GraphicsDevice::GetGUIDevice()->Decomposed(0); //indexed
}
void gdlwxGraphicsPanel::SetStream(GDLWXStream* s) {
  pstreamP = s;
  wx_dc = pstreamP->GetStreamDC();
}
gdlwxPlotPanel::gdlwxPlotPanel( gdlwxPlotFrame* parent) //, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
: gdlwxGraphicsPanel::gdlwxGraphicsPanel(parent)
, myFrame(parent)
{
#ifdef GDL_DEBUG_WIDGETS
    std::cout << "gdlwxPlotPanel::gdlwxPlotPanel(" << this << ") called."<< std::endl;
#endif
//    this->SetBackgroundColour(*wxBLACK);
}


////Stem for generalization of Drag'n'Drop, a WIDGET_DRAW can receive drop events from something else than a tree widget...
//bool DnDText::OnDropText(wxCoord x, wxCoord y, const wxString& text)
//{
//  //... BUT THIS IS NOT GOING TO WORK!
//  gdlwxDrawPanel* draw = static_cast<gdlwxDrawPanel*> (wxFindWindowAtPoint(wxGetMousePosition()));
//  if (draw) {
//    wxString* s=new wxString(text);
//    wxDropFilesEvent* e = new wxDropFilesEvent(0,draw->GetMyWidget()->GetWidgetID(), s); //event handling will destroy content, make it new object.
//    draw->OnFakeDropFileEvent(*e);
//    return true;
//  } return false;
//}

gdlwxDrawPanel::gdlwxDrawPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
: gdlwxGraphicsPanel::gdlwxGraphicsPanel(parent, id, pos, size, style, name )
, myWidgetDraw(static_cast<GDLWidgetDraw*>(GDLWidget::GetWidget(id)))
{
#ifdef GDL_DEBUG_WIDGETS
    std::cout << "gdlwxDrawPanel::gdlwxDrawPanel(" << this << ") called."<< std::endl;
#endif
//    this->SetDropTarget(new DnDText()); //will generate a DND event
//    this->SetDropTarget(new DnDFile()); just to give an example
}

void gdlwxDrawPanel::InitStream(int wIx)
{
  if (wIx < 0)  pstreamIx = GraphicsDevice::GetGUIDevice( )->WAddFree( );
  else  pstreamIx = wIx;
  if ( pstreamIx == -1 ) throw GDLException( "Failed to allocate GUI stream." );
  pstreamP = static_cast<GDLWXStream*>(GraphicsDevice::GetGUIDevice( )->GUIOpen( pstreamIx, drawSize.x, drawSize.y, this ));
  if( pstreamP == NULL)
  {
    throw GDLException( "Failed to open GUI stream: " + i2s( pstreamIx ) );
  }
  wx_dc = pstreamP->GetStreamDC( );
}

void gdlwxGraphicsPanel::ResizeDrawArea(wxSize s)
{
  if (drawSize.x == s.x & drawSize.y == s.y) return; //VERY important , was one problem in #1471
  bool doClear=false; 
  if (drawSize.x > s.x || drawSize.y > s.y ) doClear=true; 
  drawSize=s;
  this->SetVirtualSize(drawSize);
  pstreamP->SetSize(drawSize);
  Refresh(); //--> will call PAINT EVENT, this one will blit and work.
} 

gdlwxPlotPanel::~gdlwxPlotPanel()
{ 
#ifdef GDL_DEBUG_WIDGETS
    std::cout << "~gdlwxPlotPanel() :" << this << "pstreamP="<<GetStream()<< " is now invalid."<<std::endl;
#endif
    this->GetMyFrame()->Show(false);
    this->GetMyFrame()->Destroy(); //will be really destroyed after next call to wxApp (remember, wxApp is not running, only GDL is running).
}

gdlwxDrawPanel::~gdlwxDrawPanel() 
{ 
 //widget_control,this,/dest actually calls wdelete. So the only case where we are not 'deleted' with a wdelete or widget_control command is during the detetion of a container.
 // in which case, we avoid reentrance because on the first pass the stream has been invalidated.
 // if associated stream is invalid, do nothing : 'this' has already been called. Normally, should not happen...
 if (pstreamP->GetValid()) return;
 
  //invalidate stream in any case, but do not have TidyWindowsList() re-deleting it, as whatever call us, gdlwxDrawPanel is going to die.
  pstreamP->SetValid(false);
#ifdef GDL_DEBUG_WIDGETS
    std::cout << "~gdlwxDrawPanel() :" << this << "pstreamP="<<GetStream()<< " is now invalid."<<std::endl;
#endif
    
    GraphicsDevice::GetGUIDevice()->TidyWindowsList(false);
  // get WIDGET_DRAW
  GDLWidgetDraw* d = GetMyWidget();
  // at exit from here, widget_draw is either destroyed because I ask for it or because it was already being destroyed. To prevent reentrance, NULL the reference to it.
  myWidgetDraw = NULL;
  //either widget_draw is valid and we must destroy it properly. Else it is not anymore (see ~GDLWidgetDraw() ) so we have nothing to do.
  if (d && d->IsValid()) { //we come from wdelete
    //this=thewxWidget will be destroyed no matter what, since this is part of its destruction process. To destroy the gdlwidget, it would need to remove 'this' from the
    //gdlWidget structure and the underlying wxwidgets parent-child lists. Difficult. Simpler at the moment:
    // unref the container (so destruction call of the wxcontainer will not be effected at widget destruction)
    // delete the wigdet to remove it from widget hierarchy.
    wxWindow* delayed_destroy_frame = static_cast<wxWindow*>(d->GetWxContainer()); //container can be theWxWidget=this
    d->UnrefTheWxContainer(); //in any cases
    //remove the container from its parent (panel)
    delayed_destroy_frame->GetParent()->RemoveChild(delayed_destroy_frame); //should hide the wxContainer.
    delayed_destroy_frame->Hide();
    //no link anymore to 'this' in gdlwidget destruction
    delete d;
  } 
}
GDLWidgetDraw::GDLWidgetDraw( WidgetIDT p, EnvT* e, int windowIndex,
                  DLong x_scroll_size, DLong y_scroll_size, bool app_scroll, DULong eventFlags_, DStringGDL* drawToolTip)
  : GDLWidget( p, e, NULL, eventFlags_)
  , pstreamIx(windowIndex)
{
  // xsize,ysize (or, scr_xsize, scr_ysize) define the plot size.
  assert( parentID != GDLWidget::NullID);
  wSize = computeWidgetSize();
  if (widgetSizer==NULL) {
    if (wSize.x < 1) wSize.x=gdlDEFAULT_XSIZE;
    if (wSize.y < 1) wSize.y=gdlDEFAULT_YSIZE;
  }
  //get immediately rid of scroll sizes in case of scroll or not... Here is the logic:
  // Note that IDL reports wrong dimensions when size and scr_xsize are used simultaneously for at least widget_draw. 

  if (x_scroll_size > 0) {scrolled=true;x_scroll_size*=unitConversionFactor.x;x_scroll_size+=gdlSCROLL_WIDTH_Y;} 
  if (y_scroll_size > 0) {scrolled=true;y_scroll_size*=unitConversionFactor.y;y_scroll_size+=gdlSCROLL_HEIGHT_X;}
  if (scrolled) {
    if (x_scroll_size < 1) x_scroll_size = gdlDEFAULT_XSIZE+gdlSCROLL_WIDTH_Y;
    if (y_scroll_size < 1) y_scroll_size = gdlDEFAULT_YSIZE+gdlSCROLL_HEIGHT_X;
  }  
  wScrollSize = scrolled ? wxSize(x_scroll_size , y_scroll_size ) : wSize; 
 
  GDLWidget* gdlParent = GetWidget( parentID);
  widgetPanel = GetParentPanel( );
  widgetSizer = GetParentSizer( );
  
  #include "start_eventual_frame.incpp" //Widget_DRAW
    
  long style = 0;

  gdlwxDrawPanel* draw = new gdlwxDrawPanel( widgetPanel, widgetID, wxDefaultPosition, wxDefaultSize, style);
#ifdef GDL_DEBUG_WIDGETS_COLORIZE
  draw->SetBackgroundColour(wxColour(0,255,255));
#endif
  draw->SetMinClientSize(wScrollSize);
  draw->SetClientSize(wScrollSize);
  draw->SetVirtualSize(wSize);
  draw->InitDrawSize(wSize);
  
  if (scrolled) {
    draw->SetScrollbars(gdlSCROLL_RATE, gdlSCROLL_RATE, wSize.x / gdlSCROLL_RATE, wSize.y / gdlSCROLL_RATE);
    draw->ShowScrollbars(wxSHOW_SB_ALWAYS, wxSHOW_SB_ALWAYS);
  }

 
  draw->SetCursor(wxCURSOR_CROSS);
  if (drawToolTip) dynamic_cast<wxWindow*>(draw)->SetToolTip( wxString((*drawToolTip)[0].c_str(),wxConvUTF8));
  theWxContainer = theWxWidget = draw;
 
  END_ADD_EVENTUAL_FRAME
  TIDY_WIDGET(gdlBORDER_SPACE)
    
  draw->InitStream(windowIndex);
  
  pstreamIx = draw->PStreamIx();
  GDLDelete( vValue);
  vValue = new DLongGDL(pstreamIx);  

  //these widget specific events are always set:
   this->AddToDesiredEvents( wxEVT_PAINT, wxPaintEventHandler(gdlwxDrawPanel::OnPaint),draw);
//   //disable flicker see https://wiki.wxwidgets.org/Flicker-Free_Drawing
//   this->AddToDesiredEvents( wxEVT_ERASE_BACKGROUND, wxEraseEventHandler(gdlwxDrawPanel::OnErase),draw);

  //other set event handling according to flags
  if (eventFlags & GDLWidget::EV_MOTION) this->AddToDesiredEvents( wxEVT_MOTION, wxMouseEventHandler(gdlwxDrawPanel::OnMouseMove),draw);
//  if ( eventFlags & GDLWidget::EV_DROP) nothing to do yet, fixme!;
  if ( eventFlags & GDLWidget::EV_EXPOSE) this->AddToDesiredEvents(wxEVT_ERASE_BACKGROUND, wxEraseEventHandler(gdlwxDrawPanel::OnErase),draw);
//  if ( eventFlags &  GDLWidget::EV_VIEWPORT) idem;
  if ( eventFlags &  GDLWidget::EV_WHEEL) this->AddToDesiredEvents( wxEVT_MOUSEWHEEL, wxMouseEventHandler(gdlwxDrawPanel::OnMouseWheel),draw); 
  if ( eventFlags &  GDLWidget::EV_BUTTON) {
      this->AddToDesiredEvents(wxEVT_LEFT_DOWN, wxMouseEventHandler(gdlwxDrawPanel::OnMouseDown),draw); 
      this->AddToDesiredEvents(wxEVT_LEFT_UP, wxMouseEventHandler(gdlwxDrawPanel::OnMouseUp),draw); 
      this->AddToDesiredEvents(wxEVT_LEFT_DCLICK, wxMouseEventHandler(gdlwxDrawPanel::OnMouseDownDble),draw); 
      this->AddToDesiredEvents(wxEVT_MIDDLE_DOWN, wxMouseEventHandler(gdlwxDrawPanel::OnMouseDown),draw); 
      this->AddToDesiredEvents(wxEVT_MIDDLE_DCLICK, wxMouseEventHandler(gdlwxDrawPanel::OnMouseDownDble),draw); 
      this->AddToDesiredEvents(wxEVT_MIDDLE_UP, wxMouseEventHandler(gdlwxDrawPanel::OnMouseUp),draw); 
      this->AddToDesiredEvents(wxEVT_RIGHT_DOWN, wxMouseEventHandler(gdlwxDrawPanel::OnMouseDown),draw); 
      this->AddToDesiredEvents(wxEVT_RIGHT_DCLICK, wxMouseEventHandler(gdlwxDrawPanel::OnMouseDownDble),draw); 
      this->AddToDesiredEvents(wxEVT_RIGHT_UP, wxMouseEventHandler(gdlwxDrawPanel::OnMouseUp),draw); 
  }
  if (eventFlags &  GDLWidget::EV_KEYBOARD2 || eventFlags & GDLWidget::EV_KEYBOARD){
       this->AddToDesiredEvents(wxEVT_KEY_DOWN, wxKeyEventHandler(gdlwxDrawPanel::OnKey),draw); 
       this->AddToDesiredEvents(wxEVT_KEY_UP, wxKeyEventHandler(gdlwxDrawPanel::OnKey),draw); 
  }

//   UpdateGui(); 
   REALIZE_IF_NEEDED 
}

GDLWidgetDraw::~GDLWidgetDraw() {
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetDraw(" << widgetID << ")" << std::endl;
#endif
 if (theWxContainer) static_cast<wxWindow*>(theWxContainer)->Destroy();
}

void GDLWidgetDraw::AddEventType( DULong evType){
  eventFlags |= evType;
  gdlwxDrawPanel* draw=(gdlwxDrawPanel*)theWxWidget;
//this one is for the moment defined globally:
//   if ( evType == GDLWidget::EV_TRACKING) { 
//    draw->Connect(widgetID, wxEVT_ENTER_WINDOW, wxMouseEventHandler(GDLFrame::OnEnterWindow));
//    draw->Connect(widgetID, wxEVT_LEAVE_WINDOW, wxMouseEventHandler(GDLFrame::OnLeaveWindow));
//  } else
  if ( evType == GDLWidget::EV_MOTION) draw->Connect(widgetID, wxEVT_MOTION, wxMouseEventHandler(gdlwxDrawPanel::OnMouseMove));
//else  if ( evType == GDLWidget::EV_DROP) nothing to do yet, fixme!;
//else  if ( evType == GDLWidget::EV_EXPOSE) nothing to do yet, fixme!;
//else  if ( evType == GDLWidget::EV_VIEWPORT) idem;
  else if ( evType == GDLWidget::EV_WHEEL) draw->Connect(widgetID, wxEVT_MOUSEWHEEL, wxMouseEventHandler(gdlwxDrawPanel::OnMouseWheel)); 
  else if ( evType == GDLWidget::EV_BUTTON) {
      draw->Connect(widgetID, wxEVT_LEFT_DOWN, wxMouseEventHandler(gdlwxDrawPanel::OnMouseDown)); 
      draw->Connect(widgetID, wxEVT_LEFT_UP, wxMouseEventHandler(gdlwxDrawPanel::OnMouseUp)); 
      draw->Connect(widgetID, wxEVT_LEFT_DCLICK, wxMouseEventHandler(gdlwxDrawPanel::OnMouseDown)); 
      draw->Connect(widgetID, wxEVT_MIDDLE_DOWN, wxMouseEventHandler(gdlwxDrawPanel::OnMouseDown)); 
      draw->Connect(widgetID, wxEVT_MIDDLE_DCLICK, wxMouseEventHandler(gdlwxDrawPanel::OnMouseDown)); 
      draw->Connect(widgetID, wxEVT_MIDDLE_UP, wxMouseEventHandler(gdlwxDrawPanel::OnMouseUp)); 
      draw->Connect(widgetID, wxEVT_RIGHT_DOWN, wxMouseEventHandler(gdlwxDrawPanel::OnMouseDown)); 
      draw->Connect(widgetID, wxEVT_RIGHT_DCLICK, wxMouseEventHandler(gdlwxDrawPanel::OnMouseDown)); 
      draw->Connect(widgetID, wxEVT_RIGHT_UP, wxMouseEventHandler(gdlwxDrawPanel::OnMouseUp)); 
  }
  else if (evType == GDLWidget::EV_KEYBOARD2 || evType == GDLWidget::EV_KEYBOARD){
       draw->Connect(widgetID, wxEVT_KEY_DOWN, wxKeyEventHandler(gdlwxDrawPanel::OnKey)); 
       draw->Connect(widgetID, wxEVT_KEY_UP, wxKeyEventHandler(gdlwxDrawPanel::OnKey)); 
  } 
}

void GDLWidgetDraw::RemoveEventType( DULong evType){
  gdlwxDrawPanel* draw=(gdlwxDrawPanel*)theWxWidget;
//this one is for the moment defined globally:
//   if ( evType == GDLWidget::EV_TRACKING) { 
//    draw->Disconnect(widgetID, wxEVT_ENTER_WINDOW, wxMouseEventHandler(GDLFrame::OnEnterWindow));
//    draw->Disconnect(widgetID, wxEVT_LEAVE_WINDOW, wxMouseEventHandler(GDLFrame::OnLeaveWindow));
//  } else
  if ( evType == GDLWidget::EV_MOTION) draw->Disconnect(widgetID, wxEVT_MOTION, wxMouseEventHandler(gdlwxDrawPanel::OnMouseMove));
//else  if ( evType == GDLWidget::EV_DROP) nothing to do yet, fixme!;
//else  if ( evType == GDLWidget::EV_EXPOSE) nothing to do yet, fixme!;
//else  if ( evType == GDLWidget::EV_VIEWPORT) idem;
  else if ( evType == GDLWidget::EV_WHEEL) draw->Disconnect(widgetID, wxEVT_MOUSEWHEEL, wxMouseEventHandler(gdlwxDrawPanel::OnMouseWheel)); 
  else if ( evType == GDLWidget::EV_BUTTON) {
      draw->Disconnect(widgetID, wxEVT_LEFT_DOWN, wxMouseEventHandler(gdlwxDrawPanel::OnMouseDown)); 
      draw->Disconnect(widgetID, wxEVT_LEFT_UP, wxMouseEventHandler(gdlwxDrawPanel::OnMouseUp)); 
      draw->Disconnect(widgetID, wxEVT_LEFT_DCLICK, wxMouseEventHandler(gdlwxDrawPanel::OnMouseDown)); 
      draw->Disconnect(widgetID, wxEVT_MIDDLE_DOWN, wxMouseEventHandler(gdlwxDrawPanel::OnMouseDown)); 
      draw->Disconnect(widgetID, wxEVT_MIDDLE_DCLICK, wxMouseEventHandler(gdlwxDrawPanel::OnMouseDown)); 
      draw->Disconnect(widgetID, wxEVT_MIDDLE_UP, wxMouseEventHandler(gdlwxDrawPanel::OnMouseUp)); 
      draw->Disconnect(widgetID, wxEVT_RIGHT_DOWN, wxMouseEventHandler(gdlwxDrawPanel::OnMouseDown)); 
      draw->Disconnect(widgetID, wxEVT_RIGHT_DCLICK, wxMouseEventHandler(gdlwxDrawPanel::OnMouseDown)); 
      draw->Disconnect(widgetID, wxEVT_RIGHT_UP, wxMouseEventHandler(gdlwxDrawPanel::OnMouseUp)); 
  }
  else if (evType == GDLWidget::EV_KEYBOARD2 || evType == GDLWidget::EV_KEYBOARD){
       draw->Disconnect(widgetID, wxEVT_KEY_DOWN, wxKeyEventHandler(gdlwxDrawPanel::OnKey)); 
       draw->Disconnect(widgetID, wxEVT_KEY_UP, wxKeyEventHandler(gdlwxDrawPanel::OnKey)); 
  }
  eventFlags &= ~evType;
}

 void GDLWidgetDraw::SetWidgetVirtualSize(DLong sizex, DLong sizey) {
  START_CHANGESIZE_NOEVENT
    gdlwxDrawPanel* me = dynamic_cast<gdlwxDrawPanel*> (theWxWidget);
    wxSize drawSize=me->GetDrawSize();
    if (sizex < 1) sizex=drawSize.x;
    if (sizey < 1) sizey=drawSize.y;
#ifdef GDL_DEBUG_WIDGETS
    wxMessageOutputStderr().Printf(_T("GDLWidgetDraw::SetWidgetVirtualSize %d,%d\n"), sizex, sizey);
#endif
     me->ResizeDrawArea(wxSize(sizex, sizey)); //changes draw area
    if (!scrolled) {
      wSize=wxSize(sizex, sizey);
      me->SetMinClientSize(wSize);
      me->SetClientSize(wSize);
    }
     UpdateGui();
  END_CHANGESIZE_NOEVENT
}

 //Draw widgets as Base Widgets have a Size that can be SIZE(without scrollbars) or SCR_SIZE(with scrollbars)
void GDLWidgetDraw::SetWidgetSize(DLong sizex, DLong sizey) {
  START_CHANGESIZE_NOEVENT
  gdlwxDrawPanel* me = dynamic_cast<gdlwxDrawPanel*> (theWxWidget);
  if (!scrolled) { //size is same as clientSize
    if (sizex < 1) sizex = wSize.x;
    if (sizey < 1) sizey = wSize.y;
    wSize=wxSize(sizex,sizey);
    me->SetSize(wSize);
    me->SetMinSize(wSize);
    me->ResizeDrawArea(wSize); //changes draw area
  } else { //value asked is for CLIENT size
    if (sizex < 1) sizex = wScrollSize.x;
    if (sizey < 1) sizey = wScrollSize.y;
    wScrollSize=wxSize(sizex,sizey);
    me->SetClientSize(wScrollSize);
    me->SetMinClientSize(wScrollSize);
  }

#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputStderr().Printf(_T("GDLWidgetDraw::SetWidgetSize %d,%d\n"), sizex, sizey);
#endif
  UpdateGui();
  END_CHANGESIZE_NOEVENT
}

//Draw widgets as Base Widgets have a Size that can be SIZE(without scrollbars) or SCR_SIZE(with scrollbars)
void GDLWidgetDraw::SetWidgetScreenSize(DLong sizex, DLong sizey) {
  START_CHANGESIZE_NOEVENT
  gdlwxDrawPanel* me = dynamic_cast<gdlwxDrawPanel*> (theWxWidget);
  if (!scrolled) { //size is same as clientSize
    if (sizex < 1) sizex = wSize.x;
    if (sizey < 1) sizey = wSize.y;
    wSize=wxSize(sizex,sizey);
    me->SetClientSize(wSize);
    me->SetMinClientSize(wSize);
    me->ResizeDrawArea(wSize); //changes draw area
  } else { //value asked is for SIZE
    if (sizex < 1) sizex = wScrollSize.x;
    if (sizey < 1) sizey = wScrollSize.y;
    wScrollSize=wxSize(sizex,sizey);
    me->SetSize(wScrollSize);
    me->SetMinSize(wScrollSize);
  }

#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputStderr().Printf(_T("GDLWidgetDraw::SetWidgetSize %d,%d\n"), sizex, sizey);
#endif
  UpdateGui();
  END_CHANGESIZE_NOEVENT
}
  wxPoint GDLWidgetDraw::GetPos(){
    gdlwxDrawPanel* dp=static_cast<gdlwxDrawPanel*>(theWxWidget);
    int yvs=dp->GetVirtualSize().y;
    int ycs=dp->GetClientSize().y;
    //the reference position is not at the top but at the bottom of this client window
    wxPoint zero(0,yvs-ycs);
    wxPoint np=dp->CalcScrolledPosition(zero);
    return wxPoint(-np.x,np.y);
  }
  void GDLWidgetDraw::SetPos(int x, int y){
    gdlwxDrawPanel* dp=static_cast<gdlwxDrawPanel*>(theWxWidget);
    int yvs=dp->GetVirtualSize().y;
    int ycs=dp->GetClientSize().y;
    int scx,scy;
    dp->GetScrollPixelsPerUnit(&scx,&scy);
    if (scx) x=int(float(x)/float(scx));
    if (scy) y=int(float(yvs-ycs-y)/float(scy));
    dp->Scroll(x, y); //in scroll units
    dp->Refresh();
  }
  
// for MacOS /COCOA port, the following code does not work and the application is hung (!) Help!
// So we rely only on doing nothing and call Yield() , that works, fingers crossed.
// Really strange but wxWidgets is not well documented (who is?)
//#ifndef __WXMAC__ 
//int wxAppGDL::MyLoop() {
//    if (loop.IsOk()) {
////      std::cerr<<&loop<<std::endl;
//      loop.SetActive(&loop);
//      if (loop.IsRunning()) {
//        while (loop.Pending()) // Unprocessed events in queue
//        {
//          loop.Dispatch(); // Dispatch next event in queue
//        }
//      }
//    }
//  return 0;
//}
//#endif

#endif
