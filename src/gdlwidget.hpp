/***************************************************************************
                          gdlwidget  -  base class for GDL widgets
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


#ifndef GDLWIDGET_HPP
#define GDLWIDGET_HPP

#ifdef HAVE_LIBWXWIDGETS
// #define GDL_DEBUG_WIDGETS
// #define GDL_DEBUG_WIDGETS_COLORIZE

// use "plain menubars" instead of 'taskbars used as menubars'. taskbars permit to change font in bar, put pixmaps instead of text, and will work
// on OSX. So we choose normally to undefine this 
#ifndef __WXMAC__ 
//warning MAC should not have prefer_menubar=1 unless you solve the mac manubar problem.
#define PREFERS_MENUBAR 1
#endif
// For compilers that support precompilation, includes "wx/wx.h".
// HAVE_LARGEFILE_SUPPORT, SIZEOF_VOID_P, SIZEOF_SIZE_T,  may be set by Python, creates unnecessary warnings
#ifdef HAVE_LARGEFILE_SUPPORT
#undef HAVE_LARGEFILE_SUPPORT
#endif
#ifdef SIZEOF_VOID_P
#undef SIZEOF_VOID_P
#endif
#ifdef SIZEOF_SIZE_T
#undef SIZEOF_SIZE_T
#endif
#include <wx/wx.h>

#include <wx/app.h>
#include <wx/panel.h>
#include <wx/treebase.h>

#include <wx/treectrl.h>
#include <wx/dragimag.h>
#include <wx/dcbuffer.h>
#include <wx/dnd.h>

#include <wx/grid.h>
#ifdef HAVE_WXWIDGETS_PROPERTYGRID
//#include <wx/propgrid/propgrid.h>
#endif
#include <wx/defs.h>//for timer.
#include <wx/gdicmn.h> 
#include <wx/imaglist.h>
#include <wx/artprov.h>
#include <wx/popupwin.h>
#include <wx/notebook.h>
#include <wx/toolbook.h>
 #define gdlNotebook wxNotebook
//#define gdlNotebook wxToolbook
#include <wx/dcbuffer.h>
#include <wx/toolbar.h>
#include <wx/listbox.h>
#include <deque>
#include <map>

#include "typedefs.hpp"
#include "str.hpp"
#include "datatypes.hpp"
#include "widget.hpp"
#include "GDLInterpreter.hpp"
#include "basic_fun.hpp" //for GDL_TOSTRING

#define gdlSCROLL_RATE 10
#define gdlABSENT_SIZE_VALUE 15
#define gdlSCROLL_HEIGHT_X  ((sysScrollHeight < 10) ? gdlABSENT_SIZE_VALUE: sysScrollHeight) //wxSystemSettings::GetMetric(wxSYS_VSCROLL_X,xxx) //25 
#define gdlSCROLL_WIDTH_Y ((sysScrollWidth< 10) ? gdlABSENT_SIZE_VALUE: sysScrollWidth) //wxSystemSettings::GetMetric(wxSYS_HSCROLL_Y,xxx) //25
#define gdlDEFAULT_XSIZE 100
#define gdlDEFAULT_YSIZE 100
#define gdlCOMBOBOX_ARROW_WIDTH sysComboboxArrow 
#define gdlDEFAULT_SCROLL_SIZE 100 //gdlDEFAULT_XSIZE+gdlSCROLL_HEIGHT_X
#define gdlFRAME_MARGIN 0
#define gdlPAD 0 //3 //default padding
#define gdlSPACE 0
#define gdlLABEL_SPACE 2
#define gdlTEXT_SPACE 4
#define gdlBUTTON_SPACE 4
#define gdlSMALL_SPACE 1
#define gdlBORDER_SPACE 2
#define gdlBORDER_EXT  wxBORDER_SUNKEN // wxBORDER_THEME // wxBORDER_SUNKEN //wxBORDER_RAISED//wxBORDER_SIMPLE //wxBORDER_RAISED
#define gdlBORDER_INT wxBORDER_RAISED //wxBORDER_SUNKEN 
#define gdlTEXT_XMARGIN 4
#define gdlTEXT_YMARGIN 4
#define DONOTALLOWSTRETCH 0
#define ALLOWSTRETCH 0
#define FRAME_ALLOWSTRETCH 0
#define DEFAULT_TREE_IMAGE_SIZE 16

#ifdef __WXMSW__
  #define NEWLINECHARSIZE 2  //length of <cr><nl>
#else
  #define NEWLINECHARSIZE 1  //length of <nl> 
#endif
#ifdef __WXMSW__
#define gdlSIZE_EVENT_HANDLER wxSizeEventHandler(gdlwxFrame::OnSize) //Timer resize do not work on MSW
#else
#define gdlSIZE_EVENT_HANDLER wxSizeEventHandler(gdlwxFrame::OnSizeWithTimer) //filter mouse events (manual resize) to avoid too many updtes for nothing
#endif
#define gdlSIZE_IMMEDIATE_EVENT_HANDLER wxSizeEventHandler(gdlwxFrame::OnSize) 
typedef DLong WidgetIDT;
static std::string widgetNameList[]={"BASE","BUTTON","SLIDER","TEXT","DRAW","LABEL","LIST","MBAR","DROPLIST","TABLE","TAB","TREE","COMBOBOX","PROPERTYSHEET","WINDOW"};
static int    widgetTypeList[]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14};
static bool handlersInited=false; //handlers of graphic formats for bitmaps (magick).

enum { WINDOW_TIMER = -2*wxID_HIGHEST, RESIZE_TIMER, RESIZE_PLOT_TIMER }; //negative values, should not clash with our (positive) widget ids.
enum { 
  TREE_BITMAP_ITEM = 0,
  TREE_BITMAP_ITEM_SELECTED = 0,
  TREE_BITMAP_FOLDER,
  TREE_BITMAP_FOLDER_OPEN,
  TREE_BITMAP_END
};
enum { 
  gdlWxTree_UNCHECKED = 0,
  gdlWxTree_CHECKED
};
class DStructGDL;

// thread safe deque
class GDLEventQueue
{
private:
  std::deque<DStructGDL*> dq;
public:
  GDLEventQueue() //normally we should have ~GDLEventQueue removing the DStructGDLs?
  {}
  
  DStructGDL* Pop()
  {
    if( dq.empty())
      return NULL;   
//    if( dq.empty()) // needed again for thread safe behaviour
//      return NULL;   
    DStructGDL* front = dq.front();
    dq.pop_front();
    return front;
  }
  // for all regular events
  void PushBack( DStructGDL* ev)
  {
    dq.push_back( ev);
  }
  // for priority events (like delete widget)
  void PushFront( DStructGDL* ev)
  {
    dq.push_front( ev);
  }
  // Not good: between call of Empty and Pop another thread's Pop could be executed
//           -> Empty is useless (dangerous) for polling
// although: as used here (there is only one thread calling Pop) it would work
//   bool Empty() const
//   { 
//     return isEmpty;    
//   }
  void Purge();
  void Purge( WidgetIDT topID);
};

// all locker classes are identical. For control of locking separately
// class GUIMutexLockerT
// {
//   bool left;
// public:
//   GUIMutexLockerT(): left(false) { wxMutexGuiEnter();}
//   ~GUIMutexLockerT() { if(!left) wxMutexGuiLeave();}
//   void Leave() { wxMutexGuiLeave(); left=true;}
// };

class GDLWidget;

// global widget list type
// typedef DLong                       WidgetIDT;
// typedef std::map<WidgetIDT, GDLWidget*> WidgetListT;
// typedef std::deque<DStructGDL*> EventQueueT;

class WidgetListT
{
public:
  typedef std::map<WidgetIDT, GDLWidget*> mapT;
  typedef mapT::iterator iterator;
  typedef mapT::size_type size_type;
  typedef WidgetIDT key_type;
  typedef GDLWidget* mapped_type;
  typedef std::pair<const key_type,mapped_type> value_type;

private:
  mapT map;

public:
  WidgetListT(): map(){}
  ~WidgetListT() {}
  
  void erase (iterator position) 
  { 
    map.erase(position);
  }
  size_type erase (const key_type& k) 
  { 
    return map.erase(k);
  }
  iterator find (const key_type& k) 
  { 
    return map.find(k);
  }
  iterator begin() 
  { 
    return map.begin();
  }
  iterator end() 
  { 
    return map.end();
  }

  iterator insert (iterator position, value_type val) 
  { 
    return map.insert( position, val);    
  }
};

class WidgetEventInfo {
public:
 wxEventType t;
 wxObjectEventFunction f;
 wxWindow* w;

 WidgetEventInfo(wxEventType t_, wxObjectEventFunction f_, wxWindow* w_) : t(t_), f(f_), w(w_) {
 }
};
#ifndef __WXMAC__
// main App class
 #include "wx/evtloop.h"
 
class wxAppGDL: public wxApp
{
 wxGUIEventLoop loop;
public:
 int MyLoop();
};

wxDECLARE_APP(wxAppGDL); //wxAppGDL is equivalent to wxGetApp()
#endif

// GDL versions of wxWidgets controls =======================================
DECLARE_LOCAL_EVENT_TYPE(wxEVT_SHOW_REQUEST, -1)
DECLARE_LOCAL_EVENT_TYPE(wxEVT_HIDE_REQUEST, -1)


// GUI base class **********************************
class GDLWidgetBase;
class GDLWidgetTopBase;
class GDLWidgetContainer;

class gdlwxFrame : public wxFrame {

 bool mapped;
 wxSize frameSize;
 wxTimer * m_resizeTimer;
 GDLWidgetTopBase* gdlOwner;
 wxWindowDisabler *m_windowDisabler; //for modal bases
public:

 // ctor(s)
 gdlwxFrame(wxWindow* parent, GDLWidgetTopBase* gdlOwner_, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style=wxDEFAULT_FRAME_STYLE, bool modal=false);
 ~gdlwxFrame();
 // called from ~GDLWidgetBase
 void NullGDLOwner() {
  gdlOwner = NULL;
 }
 void UnblockIfModal() {
    if (m_windowDisabler != NULL) {
      delete m_windowDisabler; //destructor should reenable all windows.
      m_windowDisabler = NULL;
    }
 }
 GDLWidgetTopBase* GetGDLOwner() {
  return gdlOwner;
 }

 bool IsMapped() const {
  return mapped;
 }

  void SendShowRequestEvent() {
  wxCommandEvent* event;
  event = new wxCommandEvent(wxEVT_SHOW_REQUEST, GetId());
  event->SetEventObject(this);
  // only for wWidgets > 2.9 (takes ownership of event)
  //     this->QueueEvent( event);
//  this->AddPendingEvent(*event); // copies event
  this->OnShowRequest(*event); // JP Apr 2015: Should block the main thread until the window opens,
  //              so that the following WIDGET_INFO can properly read
  //              the window's properties.
  delete event;
  mapped = true;
 }

 void SendHideRequestEvent() {
  wxCommandEvent* event;
  event = new wxCommandEvent(wxEVT_HIDE_REQUEST, GetId());
  event->SetEventObject(this);
  // only for wWidgets > 2.9 (takes ownership of event)
  //     this->QueueEvent( event);
  this->AddPendingEvent(*event); // copies event
  delete event;
  mapped = false;
 }
 
 wxSize GetFrameSize(){return frameSize;}
 void SetFrameSize(wxSize &sz){frameSize=sz;}
 void refreshFrameSize(){frameSize=this->GetSize();}
 
 // event handlers (these functions should _not_ be virtual)
 void OnDropList(wxCommandEvent& event);
 void OnListBox(wxCommandEvent& event);
 void OnListBoxDoubleClicked(wxCommandEvent& event);
 void OnComboBox(wxCommandEvent& event);
 void OnComboBoxTextEnter(wxCommandEvent& event);
 void OnButton(wxCommandEvent& event);
 void OnRadioButton(wxCommandEvent& event);
 void OnCheckBox(wxCommandEvent& event);
 void OnPageChanged(wxNotebookEvent& event);
 void OnText(wxCommandEvent& event);
 void OnTextMouseEvents(wxMouseEvent& event);
 void OnTextPaste(wxClipboardTextEvent& event);
 void OnTextCut(wxClipboardTextEvent& event);
 void OnTextEnter( wxCommandEvent& event); //NOT USED
 void OnThumbTrack(wxScrollEvent& event);
 void OnThumbRelease(wxScrollEvent& event);
 void OnSize(wxSizeEvent& event); //unused.
 void OnIconize(wxIconizeEvent & event);
 void OnMove(wxMoveEvent & event);
 void OnCloseFrame(wxCloseEvent & event);
 void OnUnhandledCloseFrame(wxCloseEvent & event);
// void OnCloseWindow(wxCloseEvent & event);
 void OnEnterWindow(wxMouseEvent& event);
 void OnLeaveWindow(wxMouseEvent& event);
 void OnShowRequest(wxCommandEvent& event);
 void OnHideRequest(wxCommandEvent& event);
 void OnIdle(wxIdleEvent& event);
 void OnMenu(wxCommandEvent& event);
 void OnSizeWithTimer(wxSizeEvent& event);
 void OnTimerResize(wxTimerEvent& event);
 void OnContextEvent(wxContextMenuEvent& event);
 void OnTracking(wxFocusEvent& event);
 void OnWidgetTimer(wxTimerEvent & event);
 void OnKBRDFocusChange(wxFocusEvent &event);
void OnDrag(wxMouseEvent& event);
private:
 void OnListBoxDo(wxCommandEvent& event, DLong clicks);
 DECLARE_EVENT_TABLE()
};

static int sysScrollHeight=25;
static int sysScrollWidth=25;
static int sysComboboxArrow=25;
static wxUint32 sysPanelDefaultColour;  
class GDLWidget
{ 
  // static part is used for the abstraction
  // all widgets are refered to as IDs
  static int gdl_lastControlId;
  static bool wxIsOn; //tells if wx is started, permits to starts wxInit() as soon as needed but not before (speedup).
  static bool handlersOk; //tells if wx is started, permits to starts wxInit() as soon as needed but not before (speedup).
private:
  // the global widget list 
  // a widget is added by the constructor and removed by the destructor
  // so no other action is necessary for list handling
  static WidgetListT widgetList;
public:
  static wxFont defaultFont;
  static wxFont systemFont;
  static GDLEventQueue BlockingEventQueue;
  static GDLEventQueue InteractiveEventQueue;
  static void PushEvent( WidgetIDT baseWidgetID, DStructGDL* ev);
  static void InformAuthorities(const std::string& message);
  
  static void HandleUnblockedWidgetEvents();
  static const WidgetIDT NullID;
  
  GDLWidget( WidgetIDT p, EnvT* e, BaseGDL* vV=NULL, DULong eventFlags_=0);

  virtual ~GDLWidget();

  // get widget from ID
  static GDLWidget* GetWidget( WidgetIDT widID);
  static GDLWidget* GetParent( WidgetIDT widID);
  static GDLWidgetTopBase* GetTopLevelBaseWidget( WidgetIDT widID);
  //self variants
  GDLWidgetTopBase* GetMyTopLevelBaseWidget();
  GDLWidgetBase* GetMyBaseWidget();
  GDLWidgetBase* GetMyParentBaseWidget();
  GDLWidget* GetMyParent();
  gdlwxFrame* GetMyTopLevelFrame();
  // get ID of base widgets
  static WidgetIDT  GetBaseId( WidgetIDT widID);
  static WidgetIDT  GetIdOfTopLevelBase( WidgetIDT widID);
  
  //  static void RefreshWidgets();
  void RefreshDynamicWidget();
  void UpdateGui();
  
  static bool InitWx(); // global start of wxWidgets
  static void Init(); // global GUI intialization upon GDL startup
  static void UnInit(); // global GUI desinitialization in case it is useful (?)
  static void ResetWidgets(); // for widget_control,/reset and UnInit
  static bool wxIsStarted(){return (wxIsOn);}
  static void SetWxStarted(){wxIsOn=true;}
  static void UnsetWxStarted(){gdl_lastControlId=0;/* not possible: wxWidgets library does not survive wxUniitiailze() ... wxIsOn=false; handlersOk=false;*/}
  static int  GDLNewControlId(){
   gdl_lastControlId++;
   if (gdl_lastControlId >= wxID_LOWEST && gdl_lastControlId <= wxID_HIGHEST) gdl_lastControlId=wxID_HIGHEST+1;
   return gdl_lastControlId;
  }
  static void setDefaultFont(wxFont thefont){
   defaultFont=thefont;
  }
  static BaseGDL * getSystemColours();
  static void CallWXEventLoop(){
#ifdef __WXMAC__
    wxTheApp->Yield();
#else
    wxGetApp().MyLoop(); //central loop for wxEvents!
#endif
  }
protected:
  

  WidgetIDT    widgetID;  // own index to widgetList
  WidgetIDT    parentID;  // parent ID (0 for TLBs)

  //Accelerators to components inside and outside
  wxSizer* widgetSizer; // the sizer (possibly NULL) that governs the widget size & position. 
                        // Usually the widgetSizer of the parent widget, a Base.
  wxScrolled<wxPanel>* widgetPanel; // the wxPanel in which the widget is placed, i.e. the parentBase's theWxWidget, as a base is mostly a wxPanel.
  wxPanel* framePanel; // Panel with frame in which the widget may be shown

  wxObject* theWxWidget; //the active wxWidget, the one that sends and gets events and subjects to widget_control actions. Note this is mostly a wxWindow,
  //except the MENUs, that force theWxWidget to be a wxObject, not a wxWindow (which would have been simpler).
  wxObject* theWxContainer; //the external wx Container (wxFrame mostly) that contains everything wx that must be destroyed, or created, and is what is seen. 
  //theWxContainer is subject to framing (AddFrame) and scrolling (AddScroll)
  //position & size 
  wxPoint      wOffset;
  wxSize       wSize;
  wxSize       wScreenSize;
  
  BaseGDL*     uValue;    // the UVALUE
  BaseGDL*     vValue;    // the VVALUE
  bool         scrolled;
  bool         sensitive;
  bool         managed;
  DULong eventFlags; // event types widget should reply to
  int          exclusiveMode;

  DInt         widgetType;
  DString      widgetName;
  WidgetIDT    groupLeader;
  wxRealPoint  unitConversionFactor;
  DLong        frameWidth;
  wxFont       font;
  bool         valid; //if not, is in the process of being destroyed (prevent reentrance).
  long  alignment; //alignment of the widget
  int dynamicResize; //for some widgets, will enable resizing: -1: not resizable, 0/1 resizable
  std::vector<WidgetIDT> followers; //all the widgets that use me as group_leader
  std::vector<WidgetEventInfo*> desiredEventsList; //list of all the events (and handlers) this widget must obey.
  DString      notifyRealize;
  
  wxTimer * m_windowTimer;
//  bool delay_destroy;
  
private:  

  DString      uName;
  DString      proValue;
  DString      funcValue;
  DString      eventPro; // event handler PRO
  DString      eventFun; // event handler FUN
  DString      killNotify;
  
  void GetCommonKeywords( EnvT* e);
  void DefaultValuesInAbsenceofEnv();
  
public:

//  void SetDelayDestroy(bool val){delay_destroy=val;}
//  bool GetDelayDestroy(){return delay_destroy;}
 void setFont();
 void setFont(wxObject* o);

  typedef enum BGroupMode_ 
  { BGNORMAL=0
  , BGEXCLUSIVE=1
  , BGNONEXCLUSIVE=2
  , BGEXCLUSIVE1ST=3 
  } BGroupMode;

  typedef enum EventTypeFlags_ 
    { EV_NONE = 0
    , EV_ALL = 1
    , EV_CONTEXT = 2
    , EV_KBRD_FOCUS = 4
    , EV_TRACKING = 8 
    , EV_DROP = 16
    , EV_EXPOSE = 32
    , EV_MOTION = 64
    , EV_VIEWPORT = 128
    , EV_WHEEL = 256
    , EV_BUTTON = 512
    , EV_KEYBOARD = 1024 //widget_draw, normal keys in the KEY field, modifiers reported in the "MODIFIERS" field
    , EV_KEYBOARD2 = 2048 //widget_draw, normal keys and compose keys reported in the KEY field
    , EV_SIZE = 4096
    , EV_MOVE = 8192
    , EV_ICONIFY = 16384
    , EV_DRAG = 32768
    , EV_KILL = 65536
    } EventTypeFlags;
 
   typedef enum WidgetTypes_
    { WIDGET_UNKNOWN = -1 
     ,WIDGET_BASE = 0
     ,WIDGET_BUTTON 
     ,WIDGET_SLIDER
     ,WIDGET_TEXT
     ,WIDGET_DRAW
     ,WIDGET_LABEL
     ,WIDGET_LIST
     ,WIDGET_MBAR //actually this is not present in IDL, but this place is void in IDL...
     ,WIDGET_DROPLIST
     ,WIDGET_TABLE
     ,WIDGET_TAB
     ,WIDGET_TREE 
     ,WIDGET_COMBOBOX
     ,WIDGET_PROPERTYSHEET
     ,WIDGET_WINDOW
    } WidgetTypes;
    enum {
        gdlwALIGN_NOT=0,
        gdlwALIGN_LEFT=1,
        gdlwALIGN_CENTER=2,
        gdlwALIGN_RIGHT=4,
        gdlwALIGN_TOP=8,
        gdlwALIGN_BOTTOM=16
    } gdlAlignmentPossibilities;
  
 
  DULong GetEventFlags()  const { return eventFlags;}
  bool HasEventType( DULong evType) const { return (eventFlags & evType) != 0;}
  virtual void AddEventType( DULong evType) { eventFlags |= evType;}
  virtual void RemoveEventType( DULong evType) { eventFlags &= ~evType;}
  void Raise();
  void Lower();
  int buttonTextAlignment();
  int labelTextAlignment();
  virtual int widgetAlignment();
  void EnableWidgetUpdate(bool update);
  void ChangeUnitConversionFactor( EnvT* e);
  wxRealPoint GetRequestedUnitConversionFactor( EnvT* e);
  wxRealPoint GetCurrentUnitConversionFactor(){return unitConversionFactor;}
  void SetCurrentUnitConversionFactor(wxRealPoint value){unitConversionFactor = value;}
  virtual DStructGDL* GetGeometry(wxRealPoint fact=wxRealPoint(1.0,1.0));

  // this is called from the GUI thread on (before) Show()
  // wxTextCtrl and maybe other controls crash when called from the
  // main thread
//  virtual void OnShow();
  // this is called from the main thread on (before) Realize()
  // for latest initialzation (like allocating the plplot stream)
  // calls NOTIFY_REALIZE procedure
  virtual void OnRealize() //virtual as redefined in gdlwidget container
  {
    //setFont() will set the font for this widget and children if FONT= is present in the WIDGET_XXX command (and is supported) 
   this->setFont();
   this->SetSensitive(sensitive);
//   if (this->GetRealized()) this->RefreshWidget();
    if (eventPro.size() > 0 ) {
#ifdef GDL_DEBUG_WIDGETS
      wxMessageOutputStderr().Printf(_T("Realize: SetEventPro: \"%s\" for %d\n"), eventPro, widgetID);
#endif
      bool found=GDLInterpreter::SearchCompilePro(eventPro, true); // true -> search for procedure
    }
    if (eventFun.size() > 0 ) {
#ifdef GDL_DEBUG_WIDGETS
      wxMessageOutputStderr().Printf(_T("Realize: SetEventFun: \"%s\" for %d\n"), eventFun, widgetID);
#endif
      bool found=GDLInterpreter::SearchCompilePro(eventFun, false); // false -> search for function
    }
   if( notifyRealize != "") { //insure it is called once only for this.
      std::string note=notifyRealize;
      notifyRealize.clear();
      CallEventPro( note, new DLongGDL( widgetID));
    }
   //define Events now.
   ConnectToDesiredEvents();
  }
  void OnKill()
  {
    std::string RIP=killNotify;
    killNotify.clear(); //remove kill notify for this widget BEFORE calling it (avoid infinite recursal)
    if( RIP != ""){ 
        try {
        CallEventPro( RIP, new DLongGDL( widgetID));
        } catch (GDLException& e) {
         GDLWidget::InformAuthorities(e.getMessage());
        }
    }
  }
  
  virtual void ReorderWidgets(){} //do Nothing, only for Base.
  void AddToFollowers(WidgetIDT him)
  {
    followers.insert( followers.end( ), him );
  }
   void RemoveIfFollower(WidgetIDT him)
  {
      std::vector<WidgetIDT>::iterator it = find(followers.begin(), followers.end(), him); // Find first,
      if (it != followers.end()) followers.erase(it);                                   // ... and remove.
  }
   
  virtual void SetWidgetSize(DLong sizex, DLong sizey);
  virtual void SetWidgetVirtualSize(DLong sizex, DLong sizey){}; //do Nothing
  virtual void SetWidgetScreenSize(DLong sizex, DLong sizey);
  void SetWidgetPosition(DLong posx, DLong posy);
  bool IsValid(){return valid;}
  void SetUnValid(){valid=false;}
  void SetValid(){valid=true;}
  bool IsDynamicResize(){return ((dynamicResize>0)|| (dynamicResize==0 && !IsRealized())); }
  void SetDynamicResize(){if (dynamicResize > -1) dynamicResize=1;}
  void UnsetDynamicResize(){if (dynamicResize > -1) dynamicResize=0;}
  void authorizeDynamicResize(){dynamicResize=1;}
  
  WidgetIDT GetParentID() const { return parentID;}
  
  wxObject* GetWxWidget() const { return theWxWidget;}
  wxObject* GetWxContainer() const { return theWxContainer;}

  BaseGDL* GetUvalue() const { return uValue;}
  BaseGDL* GetVvalue() const { return vValue;}

  virtual void Realize( bool b, bool use_default=false);

  // for query of children
  virtual bool IsContainer() const { return false;} 
  virtual bool IsBase() const { return false;} 
  virtual bool IsTopBase() const { return false;} 
  virtual bool IsNormalBase() const { return false;} 
  virtual bool IsTabbedBase() const { return false;} 
  virtual bool IsContextBase() const {  return false; }
  virtual bool IsButton() const { return false;} 
  virtual bool IsMenu() const { return false;}
  virtual bool IsEntry() const {return false;}
  virtual bool IsDropList() const { return false;} 
  virtual bool IsList() const { return false;} 
  virtual bool IsComboBox() const { return false;} 
  virtual bool IsTab() const { return false;}
  virtual bool IsTable() const { return false;}
  virtual bool IsText() const { return false;} 
  virtual bool IsLabel() const { return false;} 
  virtual bool IsTree() const { return false;} 
  virtual bool IsSlider() const { return false;}
  virtual bool IsDraw() const { return false;}
  virtual bool IsMenuBar() const {return false;}
  virtual bool IsPropertySheet() const { return false;}
  virtual bool IsModal() const { return false;}
  virtual bool IsInCharacters() const {return false;} //measurements are not in characters

  virtual WidgetIDT GetChild( DLong) const {return NullID;}
  virtual DLong NChildren() const { return 0;}
  DLong GetSibling();
  virtual DLongGDL* GetChildrenList() const {return new DLongGDL(0);}
  
  //returns a list of IDs of all the widgets starting at me and below.
  DLongGDL* GetAllHeirs();
  
  virtual void MakeInteractive() {std::cerr<<"XMANAGER ACTIVE COMMAND on a not-top widget, please report."<<std::endl;}
  
  virtual bool IsUsingInteractiveEventLoop() {/*std::cerr<<"IsEventLoopBlocked on a not-top widget, please report."<<std::endl*/;return false;} //default for a normal widget

  void SetEventPro( const DString& ePro) {
    eventPro = StrUpCase( ePro);
#ifdef GDL_DEBUG_WIDGETS
      wxMessageOutputStderr().Printf(_T("SetEventPro: \"%s\" for %d\n"), eventPro, widgetID);
#endif
    if (eventPro.size() > 0) {
      bool found = GDLInterpreter::SearchCompilePro(eventPro, true); // true -> search for procedure
    }
   }
  
  const DString& GetEventPro() const { return eventPro;};
  
  void SetEventFun( const DString& eFun) {
    eventFun = StrUpCase(eFun);
#ifdef GDL_DEBUG_WIDGETS
    wxMessageOutputStderr().Printf(_T("SetEventFun: \"%s\" for %d\n"),eventFun, widgetID);
#endif
    if (eventFun.size() > 0) {
      bool found = GDLInterpreter::SearchCompilePro(eventFun, false); // false -> search for function
    }
  }
  const DString& GetEventFun() const { return eventFun;}
  void SetNotifyRealize( const DString& eNR) { notifyRealize = StrUpCase( eNR);}
  const DString& GetNotifyRealize() const { return notifyRealize;}
  void SetKillNotify( const DString& eKN) { killNotify = StrUpCase( eKN);}
  const DString& GetKillNotify() const { return killNotify;}

  static bool IsXmanagerBlocking();
  static bool IsActive();
  static DLong GetNumberOfWidgets();
  static BaseGDL* GetWidgetsList();
  static BaseGDL* GetManagedWidgetsList();
  
  WidgetIDT GetWidgetID() { return widgetID;}

 wxSizer* GetParentSizer() {
  GDLWidget* gdlParent = GetWidget(parentID);
  return gdlParent->GetSizer();
 }
  wxSizer* GetSizer() { return widgetSizer;}

 wxScrolled<wxPanel>* GetParentPanel() {
  GDLWidget* gdlParent = GetWidget(parentID);
  return gdlParent->GetPanel();
 }
  wxScrolled<wxPanel>* GetPanel() { return widgetPanel;}

  bool GetManaged() const { return managed;}
  bool IsRealized(); 
  void SetManaged( bool manval){managed = manval;}
  virtual void SetSensitive( bool value);
  bool GetSensitive();
  virtual DLong GetTheSiblingOf(DLong myIdx){return 0;}
  virtual void SetFocus();

  int  GetExclusiveMode() const { return exclusiveMode;}
  void SetExclusiveMode( int exclusiveval){exclusiveMode = exclusiveval;}

  void SetUvalue( BaseGDL *uV){uValue = uV;}
//  void SetVvalue( BaseGDL *vV){vValue = vV;} //unused!

  const DString& GetWidgetName() const { return widgetName;}
  void SetWidgetName( const DString& wName){widgetName = wName;}
  DInt GetWidgetType() { return widgetType;}
  void SetWidgetType( DInt type){widgetType=widgetTypeList[type]; widgetName = widgetNameList[type];}

  virtual bool GetButtonSet() const { return 0;} //normally not a button
//   void SetButtonSet(bool onOff){buttonSet = onOff;}

  const DString& GetUname() const { return uName;}
  void SetUname( const DString& uname){uName = uname;}

  const DString& GetProValue() const { return proValue;}
  void SetProValue( const DString& provalue){proValue = StrUpCase(provalue);}

  const DString& GetFuncValue() const { return funcValue;}
  void SetFuncValue( const DString& funcvalue){funcValue = StrUpCase(funcvalue);}
  
  virtual wxSize computeWidgetSize(); 
  wxSize getFontSize();
  wxFont getFont(){return font;};
  wxSize calculateTextScreenSize(std::string &s, wxFont testFont=wxNullFont);
  void ConnectToDesiredEvents();

  void AddToDesiredEvents(wxEventType t, wxObjectEventFunction f, wxWindow* w) {
 //immediate: w->Connect(widgetID, t, f); 
   //delayed at Realize time
 WidgetEventInfo *wei=new WidgetEventInfo(t,f,w);
   desiredEventsList.push_back(wei);
 }
  bool DisableSizeEvents(gdlwxFrame* &tlbFrame,WidgetIDT &id);
  static void EnableSizeEvents(gdlwxFrame* &tlbFrame,WidgetIDT &id);

  void SendWidgetTimerEvent(DDouble secs);
  virtual void SetButtonWidget( bool onOff){}
  void ClearEvents();
};

class GDLWidgetContainer: public GDLWidget
{
protected:
  std::deque<WidgetIDT> children;
  bool xfree;
  bool yfree;
public:
  GDLWidgetContainer( WidgetIDT parentID, EnvT* e, ULong eventFlags_=0);

  ~GDLWidgetContainer();
  
  virtual bool IsContainer() const { return true;}
  bool xFree() {return xfree;}
  bool yFree() {return yfree;}
//Realize a Container==> realise first all children. Vertically-stored widgets in a base widget must be reordered, done in overriding GDLWidgetBase::OnRealize.
  void OnRealize();

   // as this is called in the constructor, no type checking of c can be done
  // hence the AddChild() function should be as simple as that
  void AddChildID( WidgetIDT c) { children.push_back( c);}
  void RemoveChild( WidgetIDT  c) {
      std::deque<WidgetIDT>::iterator it = find(children.begin(), children.end(), c); // Find first,
      if (it != children.end()) children.erase(it);                                   // ... and remove.
  }
  DLong NChildren() const
  {
    return children.size( );
  }
  WidgetIDT GetChild( DLong childIx) const final
  {
    assert( childIx >= 0 );
    assert( childIx < children.size( ) );
    return children[childIx];
  }
  
  DLongGDL* GetChildrenList() const
  {
    DLong size=children.size( );
    if (size<1) return new DLongGDL(0);
    DLongGDL* ret=new DLongGDL(dimension(size),BaseGDL::ZERO);
    for (SizeT i=0; i< size; ++i) (*ret)[i]=children[i];
    return ret;   
  }
  //returns next in list or 0
  DLong GetTheSiblingOf(DLong myIdx)
  {
    DLong size=children.size( );
    if (size<1) return 0;
    DLong ret=0;
    for (int i=0; i< size-1; ++i) {
     if (children[i] == myIdx) {
      ret=children[i+1];
      break;
     }
    }
    return ret;   
  }
};

class gdlwxFrame;
// base for base widgets **************************************************
class GDLWidgetBase: public GDLWidgetContainer
{
public:
 
  // for radio buttons to generate deselect event
  WidgetIDT lastRadioSelection; //valid for all types of baseq
  DLong ncols;
  DLong nrows;
  bool grid;
  long childrenAlignment;
  int space;
  int xpad;
  int ypad;
  bool doMap;
  wxSize wScrollSize; //to be used everywhere

  GDLWidgetBase( WidgetIDT parentID, EnvT* e, ULong eventFlags_,
		 bool mapWid,
		 DLong col, DLong row,
		 int exclusiveMode, 
		 const DString& resource_name, const DString& rname_mbar,
		 const DString& title,
		 const DString& display_name,
		 int xpad_, int ypad_,
		 DLong x_scroll_size, DLong y_scroll_size, bool grid_layout, long children_alignment=wxALIGN_LEFT, 
     int space_=0);
  
  void CreateBase(wxWindow* parent);
  bool IsVertical() {return (ncols>0);}
  bool IsHorizontal() {return (nrows>0);}
  void SetWidgetSize(DLong sizex, DLong sizey) final;
 
  void NullWxWidget() { theWxWidget = NULL;}
  
  WidgetIDT GetLastRadioSelection() const { return lastRadioSelection;}                         
  void SetLastRadioSelection(WidgetIDT lastSel) { lastRadioSelection = lastSel;}                         

  bool IsBase() const { return true;} 
  bool IsContainer() const final { return true;} 
  bool IsScrolled() const { return scrolled;}
  long getChildrenAlignment(){return childrenAlignment;}
  long getSpace(){return space;}
  long getXPad(){return xpad;}
  long getYPad(){return ypad;}
  virtual void mapBase(bool val);
  DStructGDL* GetGeometry(wxRealPoint fact=wxRealPoint(1.0,1.0)) final;
//  wxScrolled<wxPanel>* AddBaseFrame(wxScrolled<wxPanel>* wxParent, int width=0);
  wxScrolled<wxPanel>* AddXYPad(wxScrolled<wxPanel>* wxParent, int xpad=0, int ypad=0);
//Apparently children of a base are plotted in reverse order in IDL (last first)
  void DoReorderColWidgets(int code=0,int style=0, int border=0);
  void ReorderWidgets();
  void ReorderForANewWidget(wxWindow* w, int code,int style, int border);
//  int widgetAlignment();
  bool GetMap() const { return doMap;}
  void SetMap( bool mapval){ doMap = mapval;}
  
 void DoMapAsRequested() {
  //descend all children, find all bases that need to be unmapped.
  for (std::deque<WidgetIDT>::iterator c = children.begin(); c != children.end(); ++c) {
   GDLWidget* w = GetWidget(*c);
   if (w != NULL && w->IsBase()) static_cast<GDLWidgetBase*>(w)->DoMapAsRequested();
  }
  mapBase(doMap);
 }
 virtual void SetButtonWidget( bool onOff) final;
};

class GDLWidgetTopBase : public GDLWidgetBase {
 WidgetIDT mbarID;
public:
 bool UseInteractiveEvents; //set by /XMANAGER_ACTIVE_COMMAND (GDL's) aka NO_BLOCK . indirectly used in pushEvent, selfDestroy, ~GDLWidgetContainer, ClearEvents
 gdlwxFrame* topFrame;
 bool modal;
 bool realized;
 GDLWidgetTopBase(EnvT* e, ULong eventFlags_,
   bool mapWid,
   WidgetIDT& mBarIDInOut, bool modal_, DLong frame_attr,
   DLong col, DLong row,
   int exclusiveMode,
   bool floating, //to be written: this topbase will appear over the group leader base
   const DString& resource_name, const DString& rname_mbar,
   const DString& title,
   const DString& display_name,
   int xpad_, int ypad_,
   DLong x_scroll_size, DLong y_scroll_size, bool grid_layout, long children_alignment = wxALIGN_LEFT,
   int space_ = 0);
 
 ~GDLWidgetTopBase ();
 
 int widgetAlignment();
 bool IsModal() const final {return modal;}
 void mapBase(bool val) final;

 bool IsTopBase() const final{
  return true;
 }
 bool IsTopLevelRealized() { return realized; }
 gdlwxFrame* GetTopFrame() {
  return topFrame;
 }
 void Realize(bool map, bool use_default=false) final;
 //Same as Container except that we have to reorder and map

 void OnRealize() final {
#ifdef GDL_DEBUG_WIDGETS
    wxMessageOutputStderr().Printf(_T("GDLWidgetTopBase:OnRealize: %d\n"), widgetID);
#endif
    ReorderWidgets();
    //setFont() will set the font for this widget and children if FONT= is present in the WIDGET_XXX command (and is supported) 
  this->setFont();
  this->SetSensitive(sensitive);

  for (std::deque<WidgetIDT>::reverse_iterator c = children.rbegin(); c != children.rend(); ++c) {
//  for (std::deque<WidgetIDT>::iterator c = children.begin(); c != children.end(); ++c) {
   GDLWidget* w = GetWidget(*c);
   if (w != NULL)
    w->OnRealize();
  }

    topFrame->Fit();
    topFrame->SetClientSize(topFrame->GetClientSize());
    topFrame->refreshFrameSize();

  ConnectToDesiredEvents();
  
  if (notifyRealize != "") { //insure it is called once only for this.
   std::string note = notifyRealize;
   notifyRealize.clear();
   CallEventPro(note, new DLongGDL(widgetID));
  }
  DoMapAsRequested();
 }

// void SelfDestroy(); // sends delete event to itself

 virtual void MakeInteractive() final {
#ifdef GDL_DEBUG_WIDGETS
          wxMessageOutputStderr().Printf(_T("MakeInteractive(%d)\n"),widgetID);
#endif
  UseInteractiveEvents = true; //unblocks base
 }
// returns UseInteractiveEvents
 virtual bool IsUsingInteractiveEventLoop() final {
#ifdef GDL_DEBUG_WIDGETS
          wxMessageOutputStderr().Printf(_T("IsUsingInteractiveEventLoop(%d): %d\n"),widgetID,UseInteractiveEvents);
#endif
  return UseInteractiveEvents;  //false (blocked) by default
 }
 
};

//specialized for Normal Base

class GDLWidgetNormalBase : public GDLWidgetBase {
public:
 GDLWidgetNormalBase(WidgetIDT parentID, EnvT* e, ULong eventFlags_,
   bool mapWid,
   DLong col, DLong row,
   int exclusiveMode,
   const DString& resource_name, const DString& rname_mbar,
   const DString& title,
   const DString& display_name,
   int xpad_, int ypad_,
   DLong x_scroll_size, DLong y_scroll_size, bool grid_layout, long children_alignment = wxALIGN_LEFT,
   int space_ = 0);

 bool IsNormalBase() const final {
  return true;
 }
 void SetBaseTitle(std::string &s);

 //Same as Container except that we have to reorder widgets in some cases

 void OnRealize() {
  ReorderWidgets();
  GDLWidgetContainer::OnRealize();
 }

};
//specialized for Tabbed Base

class GDLWidgetTabbedBase : public GDLWidgetBase {
 int myPage;
public:
 GDLWidgetTabbedBase(WidgetIDT parentID, EnvT* e, ULong eventFlags_,
   bool mapWid,
   DLong col, DLong row,
   int exclusiveMode,
   const DString& resource_name, const DString& rname_mbar,
   const DString& title,
   const DString& display_name,
   int xpad_, int ypad_,
   DLong x_scroll_size, DLong y_scroll_size, bool grid_layout, long children_alignment = wxALIGN_LEFT,
   int space_ = 0);
 
 ~GDLWidgetTabbedBase();
 void SetBaseTitle(std::string &s);
 bool IsTabbedBase() const final { return true;} 
 void mapBase(bool val) final;

 //Same as Container except that we have to reorder widgets in some cases

 void OnRealize() {
   ReorderWidgets();
   GDLWidgetContainer::OnRealize();
 }

};

class wxButtonGDL: public wxButton
{
  wxMenu* popupMenu;
  wxPoint position;
public: 
  wxButtonGDL(wxFont font, wxWindow *parent, 
          wxWindowID id, 
          const wxString &label=wxEmptyString,
          const wxPoint &pos=wxDefaultPosition,
          const wxSize &size=wxDefaultSize,
          long style=0,
          const wxValidator &validator=wxDefaultValidator,
          const wxString &name=wxButtonNameStr):
      wxButton(parent,id,label,pos,size,style,validator,name){
      this->SetFont(font);
      popupMenu=new wxMenu();
      position=this->GetClientRect().GetBottomLeft();
      Connect(id, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(wxButtonGDL::OnButton));
      Connect(id, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(wxButtonGDL::OnButton));
    }
  wxMenu* GetPopupMenu(){return popupMenu;}
  void SetPopupMenu(wxMenu* menu){popupMenu=menu;}
private:
  void OnButton(wxCommandEvent& event);
};

class wxBitmapButtonGDL: public wxBitmapButton
{
  wxMenu* popupMenu;
public: 
  wxBitmapButtonGDL(wxWindow *parent, 
          wxWindowID id, 
          const wxBitmap &bitmap_,
          const wxPoint &pos=wxDefaultPosition,
          const wxSize &size=wxDefaultSize,
          long style=wxBU_AUTODRAW,
          const wxValidator &validator=wxDefaultValidator,
          const wxString &name=wxButtonNameStr):
      wxBitmapButton(parent,id,bitmap_,pos,size,style,validator,name){
      popupMenu=new wxMenu();
      Connect(id, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(wxBitmapButtonGDL::OnButton));
      Connect(id, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(wxBitmapButtonGDL::OnButton));
  }
  wxMenu* GetPopupMenu(){return popupMenu;}
  void SetPopupMenu(wxMenu* menu){popupMenu=menu;}
  void OnButton(wxCommandEvent& event);
};

class GDLWidgetButton: public GDLWidget
{
public:

 typedef enum ButtonType_ {
  UNDEFINED=-1, NORMAL=0, RADIO=1, CHECKBOX=2, MENU=3, ENTRY=4, BITMAP=5, POPUP_NORMAL=6, POPUP_BITMAP=7} ButtonType;
 ButtonType buttonType;
//  wxBitmap* buttonBitmap;
  wxMenuItem* menuItem;
  bool       buttonState; //only for buttons
  wxString valueWxString;
  
  GDLWidgetButton( WidgetIDT parentID, EnvT* e, DStringGDL* value, DULong eventflags, wxBitmap* bitmap=NULL);
  ~GDLWidgetButton();
  // for WIDGET_CONTROL

  virtual void SetButtonWidget( bool onOff)
  {
    if( theWxWidget != NULL)
    {
      switch( buttonType) {
        case RADIO: {	  
          SetRadioButton( onOff);
          wxRadioButton* radioButton = dynamic_cast<wxRadioButton*>(theWxWidget);
          radioButton->SetValue(onOff);
          break;
        }
        case CHECKBOX: {
          SetRadioButton( onOff);
          wxCheckBox* checkBox = dynamic_cast<wxCheckBox*>(theWxWidget);
          checkBox->SetValue(onOff);
          break;
        }
        default: break;
      }
    }
  }
  virtual void SetButtonWidgetLabelText( const DString& value_ ) {std::cerr<<"SetButtonWidgetLabelText() ID="<<widgetID <<" error, please check!"<<std::endl;} ;//code in gdlwidget
  virtual void SetButtonWidgetBitmap( wxBitmap* bitmap_ );//code in gdlwidget
  void SetRadioButton( bool onOff)
  {
    buttonState = onOff;
  }
  bool GetButtonSet() const
  {
    return buttonState;
  }
  bool IsButton() const final { return true;} 
  bool IsBitmapButton() const {return ( buttonType==POPUP_BITMAP || buttonType==BITMAP);}
 };

//A /MENU button in a Base, or a Button in a MBAR is a Container
class GDLWidgetMenu: public GDLWidgetButton
{
  std::deque<WidgetIDT>   children;  //as for Containers
public:
 GDLWidgetMenu( WidgetIDT parentID, EnvT* e, DStringGDL* value, DULong eventflags, wxBitmap* bitmap=NULL);
  ~GDLWidgetMenu();
  
  bool IsMenu() const final {return true;}
  //Realize a Container==> realise first all children. Vertically-stored widgets in a base widget must be reordered, done in overriding GDLWidgetBase::OnRealize.
  void OnRealize();

   // as this is called in the constructor, no type checking of c can be done
  // hence the AddChild() function should be as simple as that
  void AddChildID( WidgetIDT c) { children.push_back( c);}
  void RemoveChild( WidgetIDT  c) {
      std::deque<WidgetIDT>::iterator it = find(children.begin(), children.end(), c); // Find first,
      if (it != children.end()) children.erase(it);                                   // ... and remove.
  }
  DLong NChildren() const
  {
    return children.size( );
  }
  WidgetIDT GetChild( DLong childIx) const final
  {
    assert( childIx >= 0 );
    assert( childIx < children.size( ) );
    return children[childIx];
  }
  DLongGDL* GetChildrenList() const
  {
    DLong size=children.size( );
    if (size<1) return new DLongGDL(0);
    DLongGDL* ret=new DLongGDL(dimension(size),BaseGDL::ZERO);
    for (SizeT i=0; i< size; ++i) (*ret)[i]=children[i];
    return ret;   
  }
  //returns next in list or 0
  DLong GetTheSiblingOf(DLong myIdx)
  {
    DLong size=children.size( );
    if (size<1) return 0;
    DLong ret=0;
    for (int i=0; i< size-1; ++i) {
     if (children[i] == myIdx) {
      ret=children[i+1];
      break;
     }
    }
    return ret;   
  }

};

class GDLWidgetSubMenu: public GDLWidgetMenu
{
 bool addSeparatorAbove;
 wxMenuItem* the_sep;
public:
 GDLWidgetSubMenu( WidgetIDT parentID, EnvT* e, DStringGDL* value, DULong eventflags, bool hasSeparatorAbove=false, wxBitmap* bitmap=NULL);
 ~GDLWidgetSubMenu();
// bool IsSubMenu() const {return true;}
 void SetSensitive(bool value);
 void SetButtonWidgetLabelText( const DString& value_ );
 void SetButtonWidgetBitmap( wxBitmap* bitmap );
};

class GDLWidgetMenuButton: public GDLWidgetMenu
{ 
public:
 GDLWidgetMenuButton( WidgetIDT parentID, EnvT* e, DStringGDL* value, DULong eventflags,  wxBitmap* bitmap=NULL, DStringGDL* buttonTooltip=NULL);
 ~GDLWidgetMenuButton();
 void SetSensitive( bool value);
 void SetButtonWidgetLabelText( const DString& value_ );
} ;

class GDLWidgetMenuBarButton: public GDLWidgetMenu {
public:
#ifdef PREFERS_MENUBAR
 int entry;
 GDLWidgetMenuBarButton(WidgetIDT parentID, EnvT* e, DStringGDL* value, DULong eventflags, DStringGDL* buttonTooltip = NULL);
#else
 wxToolBarToolBase* entry;
 GDLWidgetMenuBarButton(WidgetIDT parentID, EnvT* e, DStringGDL* value, DULong eventflags, wxBitmap* bitmap_=NULL, DStringGDL* buttonTooltip = NULL);
#endif
 wxSize computeWidgetSize(); //not a real menubar: buttons may have a different fontsize.
 ~GDLWidgetMenuBarButton();
 void SetSensitive(bool value);
 void SetButtonWidgetLabelText( const DString& value_ );

};

class GDLWidgetNormalButton: public GDLWidgetButton
{ 
public:
 GDLWidgetNormalButton( WidgetIDT parentID, EnvT* e, DStringGDL* value, DULong eventflags, wxBitmap* bitmap=NULL, DStringGDL* buttonTooltip=NULL);
 ~GDLWidgetNormalButton();
 void SetButtonWidgetLabelText( const DString& value_ );
};

class GDLWidgetMenuEntry: public GDLWidgetButton
{ 
  bool addSeparatorAbove;
  wxMenuItem* the_sep;
  bool checkedState;
public:
 GDLWidgetMenuEntry( WidgetIDT parentID, EnvT* e, DStringGDL* value, DULong eventflags, bool hasSeparatorAbove=false, wxBitmap* bitmap=NULL, bool checked_type=false);
 ~GDLWidgetMenuEntry();
 bool IsEntry() const final {return true;}
 void SetSensitive(bool value);
 void SetButtonWidgetLabelText( const DString& value_ );
 void SetButtonWidgetBitmap( wxBitmap* bitmap );
  bool GetButtonSet() const
  {
    return checkedState;
  }
  virtual void SetButtonWidget(bool onOff) final {
    checkedState = onOff;
    wxMenuItem* mi = static_cast<wxMenuItem*> (theWxWidget);
    if (mi->GetKind() == wxITEM_CHECK || mi->GetKind() == wxITEM_RADIO) {
      mi->Check(checkedState);
    }
  }
};

//specialized for Context Menu Base

class GDLWidgetContextBase : public GDLWidgetMenu { //GDLWidgetBase {
public:
 GDLWidgetContextBase(WidgetIDT parentID, EnvT* e, ULong eventFlags_,
   bool mapWid,
   DLong col, DLong row,
   int exclusiveMode,
   const DString& resource_name, const DString& rname_mbar,
   const DString& title,
   const DString& display_name,
   int xpad_, int ypad_,
   DLong x_scroll_size, DLong y_scroll_size, bool grid_layout, long children_alignment = wxALIGN_LEFT,
   int space_ = 0);

 ~GDLWidgetContextBase();

 bool IsContextBase() const final {
  return true;
 }
};


// droplist widget **************************************************
class GDLWidgetDropList: public GDLWidget
{
  std::string lastValue;
  DString title;
  DLong style;
  
public:
  GDLWidgetDropList( WidgetIDT p, EnvT* e, BaseGDL *value, DULong eventflags,
		     const DString& title, DLong style);
  ~GDLWidgetDropList();
  bool IsDropList() const final { return true;} 

  void SetLastValue( const std::string& v) { lastValue = v;}
  std::string GetLastValue() { return lastValue;}
  
  void SetValue(BaseGDL *value);
  void SelectEntry(DLong entry_number);
  BaseGDL* GetSelectedEntry();
};

// combobox widget **************************************************
class GDLWidgetComboBox: public GDLWidget
{
  std::string lastValue;
  DLong style;
  
public:
  GDLWidgetComboBox( WidgetIDT p, EnvT* e, BaseGDL *value, DULong eventflags, DLong style);
 ~GDLWidgetComboBox();
//  void OnShow();
  
//   void SetSelectOff();
  bool IsComboBox() const final { return true;} 
  wxSize computeWidgetSize();
  void SetLastValue( const std::string& v) { lastValue = v;}
  std::string GetLastValue() { return lastValue;}
  void SetValue(BaseGDL *value);
  void SelectEntry(DLong entry_number);
  BaseGDL* GetSelectedEntry();
  void AddItem(DString value, DLong pos);
  void DeleteItem(DLong pos);
};

// list widget **************************************************
class GDLWidgetList : public GDLWidget
{
  int maxlinelength;
  int nlines;
public:
  GDLWidgetList( WidgetIDT p, EnvT* e, BaseGDL *value, DLong style, DULong eventflags);
  ~GDLWidgetList();
  bool IsList() const final { return true;} 
  bool IsInCharacters() const final {return true;} //measurements are in characters
  void SetValue(BaseGDL *value);
  void SelectEntry(DLong entry_number);
  BaseGDL* GetSelectedEntries();
  wxSize computeWidgetSize() final;
  void SetWidgetSize(DLong sizex, DLong sizey) final;
//  void OnRealize(){wxListBox* b=static_cast<wxListBox*> (theWxWidget); if(b) b->SetSelection(0,false); GDLWidget::OnRealize(); }
};

// text widget : overloading some wxTextCtrl basics fotr our purposes
//**************************************************
class wxTextCtrlGDL : public wxTextCtrl
{
public:
 wxTextCtrlGDL(wxWindow *parent,
   wxWindowID id,
   const wxString &value = wxEmptyString,
   const wxPoint &pos = wxDefaultPosition,
   const wxSize &size = wxDefaultSize,
   long textStyle = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString &name = wxTextCtrlNameStr):
   wxTextCtrl(parent, id,value,pos,size,textStyle,validator,name){
 }
 ~ wxTextCtrlGDL(){}
  void OnChar(wxKeyEvent& event );
  void OnMouseEvents( wxMouseEvent& event);
};

class GDLWidgetText: public GDLWidget
{
  std::string lastValue;
  bool noNewLine;
  bool editable;
  int maxlinelength; //the size of the longest line in the widgets current value.
  int nlines;
  bool wrapped;
  bool multiline;
  wxSize textSize; //the current size, in characters, not owing for the character sizes and spurious decorations as wxWidgets adds by itself
  wxSize initialSize; //memory of the initial (realized) textSize just after creation.
public:
  GDLWidgetText( WidgetIDT parentID, EnvT* e, DStringGDL* value, DULong eventflags, bool noNewLine,
		 bool editable);
  ~GDLWidgetText();
  
  bool IsMultiline(){return multiline;}
  bool IsEditable(){return editable;}
  void SetEditable(bool v){ editable=v;}
  void ChangeText( DStringGDL* value, bool noNewLine=false);
  void InsertText( DStringGDL* value, bool noNewLine=false, bool insertAtEnd=false);
  void SetTextSelection(DLongGDL* pos);
  DLongGDL* GetTextSelection();
  DStringGDL* GetSelectedText();
  void AppendTextValue( DStringGDL* value, bool noNewLine);
  
  bool IsText() const final { return true;} 
  bool IsInCharacters() const final {return true;} //measurements are in characters  
  void SetLastValue( const std::string& v) { lastValue = v;}
  std::string GetLastValue() { return lastValue;}
  wxSize computeWidgetSize() final;
  void SetWidgetSize(DLong sizex, DLong sizey) final;
};


// label widget **************************************************
class GDLWidgetLabel: public GDLWidget
{
  DString value;
  bool sunken;
public:
  GDLWidgetLabel( WidgetIDT parentID, EnvT* e, const DString& value_, DULong eventflags, bool sunken);
 ~GDLWidgetLabel();
  void SetLabelValue( const DString& value_);
  bool IsLabel() const final { return true;} 
  wxSize computeWidgetSize() final;
  wxSize updateDynamicWidgetSize();
  int widgetAlignment();
  void SetWidgetSize(DLong sizex, DLong sizey) final; 
};


// draw widget **************************************************
class GDLWidgetDraw: public GDLWidget
{
  int pstreamIx;
  wxSize wScrollSize;
public:
  GDLWidgetDraw( WidgetIDT parentID, EnvT* e, int windowIndex, DLong x_scroll_size, DLong y_scroll_size, bool app_scroll, DULong eventFlags, DStringGDL* drawToolTip=NULL);

  ~GDLWidgetDraw();

  bool IsDraw() const final { return true;}
  void AddEventType( DULong evType) final; //specific for draw widgets
  void RemoveEventType( DULong evType) final;
  void SetWidgetSize(DLong sizex, DLong sizey) final; 
  void SetWidgetVirtualSize(DLong sizex, DLong sizey) final; 
  void SetWidgetScreenSize(DLong sizex, DLong sizey) final;
  void UnrefTheWxContainer(){theWxContainer=NULL;} 
  void UnrefTheWxWidget(){theWxWidget=NULL;} 
  wxPoint GetPos();
  void SetPos(int x, int y);
};

// menubar is best done with a toolbar at the moment, see below
#ifdef PREFERS_MENUBAR
// menu bar widget **************************************************
class GDLWidgetMenuBar: public GDLWidget
{
protected:
  std::deque<WidgetIDT> children;
  ~GDLWidgetMenuBar();
public:
  GDLWidgetMenuBar( WidgetIDT p, EnvT* e): 
  GDLWidget( p, NULL) //NULL because MBar must not re-read env Values of e
  {
    theWxWidget = theWxContainer = new wxMenuBar(wxMB_DOCKABLE);
    if (this->GetWidgetType()==GDLWidget::WIDGET_UNKNOWN ) this->SetWidgetType(WIDGET_MBAR);
  }
  //same as containers
  void AddChildID( WidgetIDT c) { children.push_back( c);}
  void RemoveChild( WidgetIDT  c) {
      std::deque<WidgetIDT>::iterator it = find(children.begin(), children.end(), c); // Find first,
      if (it != children.end()) children.erase(it);                                   // ... and remove.
      }
  DLong NChildren() const
  {
    return children.size( );
  }
  WidgetIDT GetChild( DLong childIx) const final
  {
    assert( childIx >= 0 );
    assert( childIx < children.size( ) );
    return children[childIx];
  }
 
  DLongGDL* GetChildrenList() const
  {
    DLong size=children.size( );
    if (size<1) return new DLongGDL(0);
    DLongGDL* ret=new DLongGDL(dimension(size),BaseGDL::ZERO);
    for (SizeT i=0; i< size; ++i) (*ret)[i]=children[i];
    return ret;   
  } 
  int GetChildrenPos(WidgetIDT c)
  {
    DLong size=children.size( );
    if (size<1) return -1;
    for (SizeT i=0; i< size; ++i) if (children[i]==c) return i;
    return -1;   
  }
  bool IsMenuBar() const final { return true;}
};
#else
// tool bar widget **************************************************
//this widget is preferred to GDLWidgetMenuBar since 1) it can have any kind of buttons and 2) the menuBar is a problem on Apple OSX (too far from IDL rendering on Window
class GDLWidgetMenuBar: public GDLWidget
{
protected:
  std::deque<WidgetIDT> children;
  ~GDLWidgetMenuBar();
public:
  GDLWidgetMenuBar( wxFrame* frame, WidgetIDT p, EnvT* e): 
  GDLWidget( p, NULL) //NULL because MBar must not re-read env Values of e
  { 
   long style=wxTB_HORIZONTAL|wxTB_DOCKABLE|wxTB_FLAT;
   wxToolBar* t= frame->CreateToolBar(style, wxID_ANY);
    theWxWidget = theWxContainer = t;
//    widgetSizer=new wxBoxSizer(wxHORIZONTAL);
//    t->SetSizer(widgetSizer);
    this->SetWidgetType(WIDGET_MBAR);
  }
  //same as containers
  void AddChildID( WidgetIDT c) { children.push_back( c);}
  void RemoveChild( WidgetIDT  c) {
      std::deque<WidgetIDT>::iterator it = find(children.begin(), children.end(), c); // Find first,
      if (it != children.end()) children.erase(it);                                   // ... and remove.
      }
  DLong NChildren() const
  {
    return children.size( );
  }
  WidgetIDT GetChild( DLong childIx) const final
  {
    assert( childIx >= 0 );
    assert( childIx < children.size( ) );
    return children[childIx];
  }
 
  DLongGDL* GetChildrenList() const
  {
    DLong size=children.size( );
    if (size<1) return new DLongGDL(0);
    DLongGDL* ret=new DLongGDL(dimension(size),BaseGDL::ZERO);
    for (SizeT i=0; i< size; ++i) (*ret)[i]=children[i];
    return ret;   
  } 
  //returns next in list or 0
  DLong GetTheSiblingOf(DLong myIdx)
  {
    DLong size=children.size( );
    if (size<1) return 0;
    DLong ret=0;
    for (int i=0; i< size-1; ++i) {
     if (children[i] == myIdx) {
      ret=children[i+1];
      break;
     }
    }
    return ret;   
  }
  int GetChildrenPos(WidgetIDT c)
  {
    DLong size=children.size( );
    if (size<1) return -1;
    for (SizeT i=0; i< size; ++i) if (children[i]==c) return i;
    return -1;   
  }
  bool IsMenuBar() const final { return true;}
};
#endif
// tab widget **************************************************
class GDLWidgetTab: public GDLWidgetContainer
{
public:
    GDLWidgetTab( WidgetIDT parentID, EnvT* e, ULong eventFlags, DLong location, DLong multiline);

  ~GDLWidgetTab();
  
  bool IsTab() const final { return true;}
  bool IsContainer() const final { return true;}
  BaseGDL* GetTabNumber();
  BaseGDL* GetTabCurrent();
  void SetTabCurrent(int val);
  BaseGDL* GetTabMultiline(); //not exactly what expected, fixme.
  void OnRealize();
};


// table widget **************************************************
class GDLWidgetTable: public GDLWidget
{
  DByteGDL* table_alignment;
  DStringGDL* amPm;
  DByteGDL* backgroundColor;
  DByteGDL* foregroundColor;
  DStringGDL* columnLabels;
  int majority;
  DLongGDL* columnWidth;
  DStringGDL* daysOfWeek;
  bool disjointSelection;
  DByteGDL* editable;
  DStringGDL* format;
//  bool ignoreAccelerators;
  DStringGDL* month;
  bool noColumnHeaders;
  bool noRowHeaders;
  bool resizeableColumns;
  bool resizeableRows;
  DLongGDL* rowHeights;
  DStringGDL* rowLabels;
//  DLong tabMode;
  DLong x_scroll_size_columns;
  DLong y_scroll_size_rows;
  DStringGDL * valueAsStrings;
  bool         updating; //widget is modified by program (avoid sending events)

public:
 typedef enum TableMajority_ {NONE_MAJOR = 0, ROW_MAJOR, COLUMN_MAJOR} TableMajority;

 GDLWidgetTable( WidgetIDT p, EnvT* e, 
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
// 		  bool ignoreAccelerators_,
		  DStringGDL* month_,
		  bool noColumnHeaders_,
		  bool noRowHeaders_,
		  bool resizeableColumns_,
		  bool resizeableRows_,
		  DLongGDL* rowHeights_,
		  DStringGDL* rowLabels_,
//		  DLong tabMode_,
		  BaseGDL* value_,
		  DLong xScrollSize_,
		  DLong yScrollSize_,
                  DStringGDL* valueAsStrings_,
                  DULong eventFlags_
         );

~GDLWidgetTable();
  std::vector<int> GetSortedSelectedRowsOrColsList(DLongGDL* selection, bool doCol);
  DStringGDL* GetCurrentFormat(){return format;}
  void SetCurrentFormat(DStringGDL* f){format=f;}
  int  GetMajority(){return majority;}
  bool IsTable() const final { return true;}
  void SetDOW(DStringGDL* val){GDLDelete(daysOfWeek); daysOfWeek=val->Dup();}
  void SetAmPm(DStringGDL* val){GDLDelete(amPm); amPm=val->Dup();};
  void SetMonth(DStringGDL* val){GDLDelete(month); month=val->Dup();};

  DLongGDL* GetSelection(bool dothrow=false);
  
  void SetAlignment(DByteGDL* val){GDLDelete(table_alignment); table_alignment=val->Dup();};
  void DoAlign();
  void DoAlign(DLongGDL* selection);
  
  void SetBackgroundColor(DByteGDL* val){GDLDelete(backgroundColor); backgroundColor=val->Dup();};
  void DoBackgroundColor();
  void DoBackgroundColor(DLongGDL* selection);
  
  void SetForegroundColor(DByteGDL* val){GDLDelete(foregroundColor); foregroundColor=val->Dup();};
  void DoForegroundColor();
  void DoForegroundColor(DLongGDL* selection);
  
  void SetColumnLabels(DStringGDL* val){GDLDelete(columnLabels); columnLabels=val->Dup();};
  void DoColumnLabels();
  
  void SetRowLabels(DStringGDL* val){GDLDelete(rowLabels); rowLabels=val->Dup();};
  void DoRowLabels();
  
  void SetColumnWidth(DLongGDL* val){GDLDelete(columnWidth); columnWidth=val->Dup();};
  void DoColumnWidth();
  void DoColumnWidth(DLongGDL* selection);
  DFloatGDL* GetColumnWidth(DLongGDL* selection=NULL);
  
  void SetRowHeights(DLongGDL* val){GDLDelete(rowHeights); rowHeights=val->Dup();};
  void DoRowHeights();
  void DoRowHeights(DLongGDL* selection);
  DFloatGDL* GetRowHeight(DLongGDL* selection=NULL);

  bool GetDisjointSelection(){return disjointSelection;}
  void SetDisjointSelection(bool b){disjointSelection = b;}
  void ClearSelection();
  
  void DeleteColumns(DLongGDL* selection=NULL);
  void DeleteRows(DLongGDL* selection=NULL);

  bool InsertColumns(DLong count, bool insertAtEnd, DLongGDL* selection=NULL);
  bool InsertRows(DLong count, bool insertAtEnd, DLongGDL* selection=NULL);

  void SetSelection(DLongGDL* selection);
  DStringGDL* GetTableValues(DLongGDL* selection=NULL);
  BaseGDL* GetTableValuesAsStruct(DLongGDL* selection=NULL);
  void SetTableValues(BaseGDL* value, DStringGDL *stringval, DLongGDL* selection=NULL);
  void SetValue(BaseGDL * val){GDLDelete(vValue); vValue=val->Dup();};
  
  void SetTableView(DLongGDL* pos);
  void MakeCellEditable(DLongGDL* pos);
  void SetTableNumberOfColumns( DLong ncols);
  void SetTableNumberOfRows( DLong nrows);
  
  bool IsSomethingSelected();
  bool GetValidTableSelection(DLongGDL* &selection);
  
  bool IsUpdating(){return updating;}
  void ClearUpdating(){updating=false;}
  void SetUpdating(){updating=true;}
  DStructGDL* GetGeometry(wxRealPoint fact=wxRealPoint(1.0,1.0)) final;
  void setFont();
};

static unsigned char tree_up_selection_mask_uc[] = {
  0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x18, 0x00, 0xfe, 0x01, 0x18, 0x01,
  0x10, 0x01, 0x00, 0x01, 0x00, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static unsigned char tree_up_selection_bits_uc[] = {
  0xff, 0xff, 0xff, 0xff, 0xef, 0xff, 0xe7, 0xff, 0x01, 0xfe, 0xe7, 0xfe,
  0xef, 0xfe, 0xff, 0xfe, 0xff, 0x80, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};
static unsigned char tree_down_selection_mask_uc[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x7f, 0x00, 0x01, 0x10, 0x01, 0x18, 0x01, 0xfe, 0x01,
   0x18, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00 };
static unsigned char tree_down_selection_bits_uc[] = {
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0x80, 0xff, 0xfe, 0xef, 0xfe, 0xe7, 0xfe, 0x01, 0xfe,
   0xe7, 0xff, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff };
static const char* tree_up_selection_bits = (const char*) tree_up_selection_bits_uc;
static const char* tree_up_selection_mask = (const char*) tree_up_selection_mask_uc;
static const char* tree_down_selection_bits = (const char*) tree_down_selection_bits_uc;
static const char* tree_down_selection_mask = (const char*) tree_down_selection_mask_uc;
// tree widget **************************************************
class GDLWidgetTree;
class wxTreeItemDataGDL;
class wxTreeCtrlGDL: public wxTreeCtrl {
  wxWindowID GDLWidgetTreeID;
  int KeyModifier; //Shift, Control... set during a drag
private:
	// the item being dragged at the moment
	wxTreeItemId itemDragging = nullptr;
  wxCursor gdlTREE_SELECT_ABOVE;
  wxCursor gdlTREE_SELECT_BELOW;
  int posCode;
  wxPoint position;

public:
//	void onLeftDown(wxMouseEvent &evt);
	void onLeftUp(wxMouseEvent &evt);
	void onLeaveWindow(wxMouseEvent &evt);
	void onEnterWindow(wxMouseEvent &evt);
	// finish the drag action 
	void endDragging();
	void onMouseMotion(wxMouseEvent &evt);
public:
  wxTreeCtrlGDL(wxWindow *parent, wxWindowID id = wxID_ANY,
    const wxPoint& pos = wxDefaultPosition,
    const wxSize& size = wxDefaultSize,
    long style = wxTR_DEFAULT_STYLE,
    const wxValidator &validator = wxDefaultValidator,
    const wxString& name = wxTreeCtrlNameStr)
          :wxTreeCtrl( parent, id, pos, size, style, wxDefaultValidator , name ),
          GDLWidgetTreeID(id),
          posCode(-1)
          , position(wxDefaultPosition)
          {

#ifdef __WXMSW__
    wxBitmap tree_up_selection_bitmap(tree_up_selection_bits, 16, 16);
    wxBitmap tree_up_selection_mask_bitmap(tree_up_selection_mask, 16, 16);
    tree_up_selection_bitmap.SetMask(new wxMask(tree_up_selection_mask_bitmap));
    wxImage tree_up_selection_image = tree_up_selection_bitmap.ConvertToImage();
    tree_up_selection_image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 14);
    tree_up_selection_image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 7);
    gdlTREE_SELECT_ABOVE = wxCursor(tree_up_selection_image);
    wxBitmap tree_down_selection_bitmap(tree_down_selection_bits, 16, 16);
    wxBitmap tree_down_selection_mask_bitmap(tree_down_selection_mask, 16, 16);
    tree_down_selection_bitmap.SetMask(new wxMask(tree_down_selection_mask_bitmap));
    wxImage tree_down_selection_image = tree_down_selection_bitmap.ConvertToImage();
    tree_down_selection_image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 14);
    tree_down_selection_image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 7);
    gdlTREE_SELECT_BELOW = wxCursor(tree_down_selection_image);

#elif defined(__WXGTK__) or defined(__WXMOTIF__)
    gdlTREE_SELECT_ABOVE = wxCursor(tree_up_selection_bits, 16, 16, 14, 7, tree_up_selection_mask, wxWHITE, wxBLACK);
    gdlTREE_SELECT_BELOW = wxCursor(tree_down_selection_bits, 16, 16, 14, 7, tree_down_selection_mask, wxWHITE, wxBLACK);
#endif
          }
  //necessary to define the destructor otherwise compiler will try to find the bind event table for destruction event!
  ~wxTreeCtrlGDL(){}
  int GetCurrentModifier(){return KeyModifier;}
  GDLWidgetTree* GetItemTreeWidget(wxTreeItemId id);
  
  void OnItemActivated(wxTreeEvent & event);
  void OnItemCollapsed(wxTreeEvent & event);
  void OnItemStateClick(wxTreeEvent & event);
  void OnItemExpanded(wxTreeEvent & event);
  void OnDrag(wxTreeEvent & event);
  void OnTreeCtrlDrop(wxTreeEvent & event);
  void OnForeignDrop(WidgetIDT selected, WidgetIDT drag_id);
  void OnItemSelected(wxTreeEvent & event);
  DECLARE_EVENT_TABLE()
};

class wxTreeItemDataGDL : public wxTreeItemData {
  public:
    WidgetIDT widgetID;
    wxTreeCtrlGDL* myTree;
    wxTreeItemId treeItemID;
  wxTreeItemDataGDL(WidgetIDT id, wxTreeCtrlGDL* myTree_) : widgetID(id), myTree(myTree_) {}
  WidgetIDT GetWidgetID(){return widgetID;}
  wxTreeCtrlGDL* GetTree(){return myTree;}
  void SetItemId(wxTreeItemId id){treeItemID=id;}
};

class GDLWidgetTree: public GDLWidget
{
int droppable  ; 
int draggable  ;
bool has_checkbox;
bool expanded;
bool folder;
bool noBitmaps;
bool mask;
bool multiple; //can select multiple
wxTreeItemId treeItemID;
wxTreeItemDataGDL* treeItemData;
GDLWidgetTree* myRoot;
DString dragNotify;

public:
GDLWidgetTree( WidgetIDT p, EnvT* e, BaseGDL* value_, DULong eventFlags
,wxBitmap* bitmap_
,DLong dropability
,DLong dragability
,bool expanded_
,bool folder_
,DLong treeindex
,DString &dragNotify_
);
		 
~GDLWidgetTree();

  bool IsTree() const final { return true;}
  //DRAG
  int GetDraggableValue() {return draggable;}
  bool GetDragability();
  void SetDragability(DLong val){draggable=val;}
  //DRAGNOTIFY
  DString GetDragNotifyValue();
  int GetDragNotifyReturn(DString &cf, WidgetIDT sourceID, int modifiers, int defaultval);
  void SetDragNotify(DString &s){dragNotify=s;}
  //DROP
  int  GetDroppableValue(){return droppable;}
  bool GetDropability();
  void SetDropability(DLong val){droppable=val;}
  bool IsExpanded() {
    return expanded;
  }
  void SetExpanded(bool b){expanded=b;}

  void DoExpand(bool what);
  void Select(bool select);
  void SetTreeIndex(DLong where);
  GDLWidgetTree* GetMyRootGDLWidgetTree(){ return myRoot;}
  WidgetIDT IsSelectedID();
  WidgetIDT IsDragSelectedID();
  DLongGDL* GetAllSelectedID();
  DLongGDL* GetAllDragSelectedID();
  DInt GetTreeIndex();
  wxTreeItemId GetItemID(){ return treeItemID;}
  void SetItemID( wxTreeItemId id){treeItemID=id;}
  void SetValue(DString val);
  void OnRealize();
  void SetFolder(){folder=true;}
  bool IsFolder(){return folder;}
  void SetMask(bool b){mask=b;}
  bool HasMask(){return mask;}
  DByteGDL* ReturnBitmapAsBytes();
  bool HasCheckBox(){return has_checkbox;}
  void CheckItem(bool b){treeItemData->myTree->SetItemState(treeItemID,b);treeItemData->myTree->Refresh();}
  bool IsUsingBitmaps(){return noBitmaps;}
  void SetBitmap(wxBitmap* bitmap);
  void SetVisible() {treeItemData->myTree->EnsureVisible(treeItemID);}
  bool IsChecked(){if (has_checkbox) return treeItemData->myTree->GetItemState(treeItemID); else return false;
  }

  DLong NChildren() const;
  WidgetIDT GetChild(DLong childIx) const final;
  DLongGDL* GetChildrenList() const;
  DLong GetTheSiblingOf(DLong myIx) final;
  DLong Sibling();
};



// slider widget **************************************************
class GDLWidgetSlider: public GDLWidget
{
  DLong value; 
  DLong minimum; 
  DLong maximum;
  DString title;
public:
  GDLWidgetSlider( WidgetIDT parentID, EnvT* e, DLong value_,
       DULong eventFlags_ ,
		   DLong minimum_, DLong maximum_,
		   bool vertical,
		   bool suppressValue,
		   DString &title
 		);

  ~GDLWidgetSlider();

  void SetValue( DLong v) { value = v;}
  void ControlSetValue ( DLong v );
  void ControlSetMinValue ( DLong v );
  void ControlSetMaxValue ( DLong v );
  DLong GetValue() const { return value;}
  
  bool IsSlider() const final { return true;}
};




class wxNotebookEvent;
class wxGridEvent;
class wxGridSizeEvent;
class wxGridRangeSelectEvent;
class wxGridGDL : public wxGrid
{
  wxWindowID GDLWidgetTableID;
public:
  wxGridGDL(wxWindow* container, wxWindowID id, 
	    const wxPoint& pos = wxDefaultPosition, 
	    const wxSize& size = wxDefaultSize,
	    long style = 0, 
	    const wxString& name = wxPanelNameStr):
  wxGrid( container, id, pos, size, style, name )
  , GDLWidgetTableID(id)
{
   //replaced by addtodesiredevents.
//    Connect(id,wxEVT_GRID_COL_SIZE,wxGridSizeEventHandler(wxGridGDL::OnTableColResizing));
//    Connect(id,wxEVT_GRID_ROW_SIZE,wxGridSizeEventHandler(wxGridGDL::OnTableRowResizing));
//    Connect(id,wxEVT_GRID_RANGE_SELECT,wxGridRangeSelectEventHandler(wxGridGDL::OnTableRangeSelection));
//    Connect(id,wxEVT_GRID_SELECT_CELL,wxGridEventHandler(wxGridGDL::OnTableCellSelection));
////    Connect(id,wxEVT_GRID_CELL_LEFT_CLICK,wxGridEventHandler(gdlGrid::OnTableCellSelection));
  }
  ~wxGridGDL(){
#ifdef GDL_DEBUG_WIDGETS
    std::cout << "~wxGridGDL: " << this << std::endl;
#endif 
  }
  
  bool IsSomethingSelected(){
      wxGridCellCoordsArray cellSelection=this->GetSelectedCells();
      if ( cellSelection.Count() > 0 ) return true;
      wxGridCellCoordsArray selectionBR=this->GetSelectionBlockBottomRight();
      if ( selectionBR.Count() > 0 ) return true;
      wxArrayInt selectionRow=this->GetSelectedRows();
      if ( selectionRow.GetCount() > 0 ) return true;
      wxArrayInt selectionCol=this->GetSelectedCols();
      if ( selectionCol.GetCount() > 0 ) return true;
      return false;
  }
  std::vector<wxPoint> GetSelectedDisjointCellsList(){
      std::vector<wxPoint> list;
      wxGridCellCoordsArray cellSelection=this->GetSelectedCells();
      for( int i=0; i<cellSelection.Count(); i++ ) {
       int row = cellSelection[i].GetRow();
       int col = cellSelection[i].GetCol();
       list.push_back(wxPoint(row,col));
      }

      wxGridCellCoordsArray selectionTL=this->GetSelectionBlockTopLeft();
      wxGridCellCoordsArray selectionBR=this->GetSelectionBlockBottomRight();
      for( int k=0; k<selectionBR.Count(); k++ ) {
       int rowTL = selectionTL[k].GetRow();
       int colTL = selectionTL[k].GetCol();
       int rowBR = selectionBR[k].GetRow();
       int colBR = selectionBR[k].GetCol();
       int nrows=rowBR-rowTL+1;
       int ncols=colBR-colTL+1;
       for ( int i=0; i< nrows; ++i) for (int j=0; j<ncols; ++j) list.push_back(wxPoint(rowTL+i,colTL+j));
      }
      wxArrayInt selectionRow=this->GetSelectedRows();
      for( int k=0; k<selectionRow.GetCount(); k++ ) {
       int row = selectionRow[k];
       for ( int i=0; i< this->GetNumberCols(); ++i) list.push_back(wxPoint(row,i));
      }
      wxArrayInt selectionCol=this->GetSelectedCols();
      for( int k=0; k<selectionCol.GetCount(); k++ ) {
       int col = selectionCol[k];
       for ( int i=0; i< this->GetNumberRows(); ++i) list.push_back(wxPoint(i,col));
      }      
      return list;
  }

  wxArrayInt GetSelectedBlockOfCells() {
    wxArrayInt block;
    wxGridCellCoordsArray selectionTL = this->GetSelectionBlockTopLeft();
    wxGridCellCoordsArray selectionBR = this->GetSelectionBlockBottomRight();
    if (selectionBR.Count() > 0) {
      for (int k = 0; k < selectionBR.Count(); k++) {
        int colTL = selectionTL[k].GetCol();
        block.push_back(colTL);
        int rowTL = selectionTL[k].GetRow();
        block.push_back(rowTL);
        int colBR = selectionBR[k].GetCol();
        block.push_back(colBR);
        int rowBR = selectionBR[k].GetRow();
        block.push_back(rowBR);
      }
      return block;
    }
    
    wxArrayInt selectionRow=this->GetSelectedRows();
    if (selectionRow.GetCount() > 0) {
      block.push_back(0);
      block.push_back(selectionRow[0]);
      block.push_back(this->GetNumberCols()-1);
      block.push_back(selectionRow[selectionRow.GetCount()-1]);
      return block;
    }
    
    wxArrayInt selectionCol=this->GetSelectedCols();
    if (selectionCol.GetCount() > 0) {
      block.push_back(selectionCol[0]);
      block.push_back(0);
      block.push_back(selectionCol[selectionCol.GetCount()-1]);
      block.push_back(this->GetNumberRows()-1);
      return block;
    } 

    wxGridCellCoordsArray cellSelection=this->GetSelectedCells(); //last chance for block selection, return only first if exist!
    if (cellSelection.Count()==0) return block; //should produce error...
    int row = cellSelection[0].GetRow();
    int col = cellSelection[0].GetCol();
    block.push_back(col);
    block.push_back(row);
    block.push_back(col);
    block.push_back(row);
    return block;
  }

  void OnTableCellSelection(wxGridEvent & event);
  void OnTableRangeSelection(wxGridRangeSelectEvent & event);
  void OnTableColResizing(wxGridSizeEvent & event);
  void OnTableRowResizing(wxGridSizeEvent & event); 
//  void OnText( wxCommandEvent& event);
  void OnTextChanging( wxGridEvent & event);
};
#ifdef HAVE_WXWIDGETS_PROPERTYGRID
//
//class GDLWidgetPropertySheet : public GDLWidget
//{
//public:
//  GDLWidgetPropertySheet( WidgetIDT parentID, EnvT* e);
//
//  ~GDLWidgetPropertySheet();
//
//  bool IsPropertySheet() const { return true;}
//};
#endif
//phantom window used to find exactly the size of scrollbars, as wxWidgets does not coorectly report them
class gdlwxPhantomFrame : public wxFrame {
 public:
 gdlwxPhantomFrame();
 void Realize();
};


// basic for (completely separated from widgets) plot windows done with wxWidgets driver.
class gdlwxPlotFrame : public wxFrame {
 wxTimer * m_resizeTimer;
 bool scrolled;
public:
 // ctor(s)
 gdlwxPlotFrame(const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, bool scrolled=false);
 ~gdlwxPlotFrame();
  bool IsScrolled(){return scrolled;}
 void Realize();
// event handlers (these functions should _not_ be virtual)
 void OnTimerPlotResize(wxTimerEvent& event);
 void OnUnhandledClosePlotFrame(wxCloseEvent & event);
 void OnPlotSizeWithTimer(wxSizeEvent& event);
 void OnPlotWindowSize(wxSizeEvent& event); //unused
 DECLARE_EVENT_TABLE()
};
class GDLWXStream;
#include "graphicsdevice.hpp"
// graphic panels. A basic one, conected with WXStream, used for plots. does not pertain to the widgets hierarchy.
class gdlwxGraphicsPanel : public wxScrolled<wxPanel> {
protected:
 GDLWXStream* pstreamP;
public:
 int pstreamIx;
 wxSize drawSize;
 wxMemoryDC* wx_dc; //pointer on plplot's stream DC

 gdlwxGraphicsPanel(wxWindow* parent, wxWindowID id = wxID_ANY,
   const wxPoint& pos = wxDefaultPosition,
   const wxSize& size = wxDefaultSize,
   long style = 0,
   const wxString& name = wxPanelNameStr);

 int PStreamIx() {return pstreamIx;}
 void SetPStreamIx( int ix){pstreamIx=ix;} 
 GDLWXStream* GetStream();

 void InitDrawSize(wxSize s) {drawSize = s;}
 wxSize GetDrawSize() {return drawSize;}
 wxPoint WhereIsMouse(wxMouseEvent &e);
 wxPoint WhereIsMouse(wxKeyEvent &e);
 void ResizeDrawArea(const wxSize s);
 void DeleteUsingWindowNumber();
 void SetUndecomposed();
 void SetStream(GDLWXStream* s);

 void OnErase(wxEraseEvent& event);
 void OnPaint(wxPaintEvent& event);
 virtual void OnPlotWindowSize(wxSizeEvent &event);
 
// virtual bool isPlot() const {return false;}


 virtual void RaisePanel()  {}
 virtual void LowerPanel()  {}
 virtual void IconicPanel()    {}
 virtual void DeIconicPanel()    {}
 virtual bool UnsetFocusPanel()  {return false;}
 virtual void SetFocusPanel()   {}
};
//for (non-widget) plot panels
class gdlwxPlotPanel : public gdlwxGraphicsPanel
{
 gdlwxPlotFrame* myFrame;
public:
 gdlwxPlotPanel(gdlwxPlotFrame* parent);
 ~gdlwxPlotPanel();
 
// bool isPlot() const final {return true;}
 void OnPlotWindowSize(wxSizeEvent &event) final;
 gdlwxPlotFrame* GetMyFrame(){return myFrame;}
//works for basic panels, void for Draw widgets:

 void RaisePanel() final {
  this->GetParent()->Raise();
 }

 void LowerPanel() final {
  this->GetParent()->Lower();
 }

 void IconicPanel() final {
  static_cast<wxFrame*> (this->GetParent())->Iconize(true);
 }

 void DeIconicPanel() final {
  static_cast<wxFrame*> (this->GetParent())->Iconize(false);
 }

 bool UnsetFocusPanel() final {  //focus in not given 
  this->Disable(); //no buttons will not work!
  return true;
 }

 };
// 
////Stem for generalization of Drag'n'Drop, a WIDGET_DRAW can receive drop events from something else than a tree widget (but will probably misunderstand)
//class DnDText : public wxTextDropTarget {
//public:
//
//  DnDText() {
//  }
//
//  virtual bool OnDropText(wxCoord x, wxCoord y, const wxString& text);
//
//};
//
//class DnDFile : public wxFileDropTarget
//{
//public:
//    DnDFile() {  }
//    virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames) {
//      std::cerr<<filenames[0]<<std::endl;
//      return true;
//    }
//
//};
class gdlwxDrawPanel : public gdlwxGraphicsPanel
{
 GDLWidgetDraw* myWidgetDraw;

public:
 // ctor(s)
 gdlwxDrawPanel(wxWindow* parent, wxWindowID id,
   const wxPoint& pos = wxDefaultPosition,
   const wxSize& size = wxDefaultSize,
   long style = 0,
   const wxString& name = wxPanelNameStr);

 ~gdlwxDrawPanel();

 GDLWidgetDraw* GetMyWidget()  {return myWidgetDraw;}
 void InitStream(int windowIndex = -1);
 //works for basic panels, void for Draw widgets:

 
 // event handlers (these functions should _not_ be virtual)
 void OnErase(wxEraseEvent& event);
 //  void OnClose(wxCloseEvent& event);
 void OnMouseMove(wxMouseEvent& event);
 void OnMouseDown(wxMouseEvent& event);
 void OnMouseUp(wxMouseEvent& event);
 void OnMouseDownDble(wxMouseEvent& event);
 void OnMouseWheel(wxMouseEvent& event);
 void OnKey(wxKeyEvent& event);
 void OnFakeDropFileEvent(wxDropFilesEvent& event);
 //  void OnSize(wxSizeEvent &event);
// void OnPlotWindowSize(wxSizeEvent &event);

};
DStringGDL* CallStringFunction(BaseGDL* val, BaseGDL* format);
#endif

#endif

