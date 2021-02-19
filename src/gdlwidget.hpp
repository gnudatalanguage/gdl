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
 #define GDL_DEBUG_WIDGETS_COLORIZE

#include <wx/wx.h>
#include <wx/app.h>
#include <wx/panel.h>
#include <wx/treebase.h>
#include <wx/treectrl.h>
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
#include <wx/dcbuffer.h>
#include <deque>
#include <map>

#include "typedefs.hpp"
#include "str.hpp"
#include "datatypes.hpp"
#include "widget.hpp"

#define gdlSCROLL_RATE 20
#define gdlSCROLL_HEIGHT_X  sysScrollHeight //wxSystemSettings::GetMetric(wxSYS_VSCROLL_X,xxx) //25 
#define gdlSCROLL_WIDTH_Y sysScrollWidth //wxSystemSettings::GetMetric(wxSYS_HSCROLL_Y,xxx) //25
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
#define gdlBORDER_EXT wxBORDER_THEME // wxBORDER_SUNKEN //wxBORDER_RAISED//wxBORDER_SIMPLE //wxBORDER_RAISED
#define gdlBORDER_INT wxBORDER_NONE //wxBORDER_SUNKEN 
#define gdlTEXT_XMARGIN 4
#define gdlTEXT_YMARGIN 4
#define DONOTALLOWSTRETCH 0
#define ALLOWSTRETCH 1
#define FRAME_ALLOWSTRETCH 1
#ifdef __WXMSW__
  #define NEWLINECHARSIZE 2  //length of <cr><nl>
#else
  #define NEWLINECHARSIZE 1  //length of <nl> 
#endif
#define gdlSIZE_EVENT_HANDLER wxSizeEventHandler(gdlwxFrame::OnSizeWithTimer) //filter mouse events (manual resize) to avoid too many updtes for nothing
#define gdlSIZE_IMMEDIATE_EVENT_HANDLER wxSizeEventHandler(gdlwxFrame::OnSize) 
typedef DLong WidgetIDT;
static std::string widgetNameList[]={"BASE","BUTTON","SLIDER","TEXT","DRAW","LABEL","LIST","MBAR","DROPLIST","TABLE","TAB","TREE","COMBOBOX","PROPERTYSHEET","WINDOW"};
static int    widgetTypeList[]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14};
static bool handlersInited=false; //handlers of graphic formats for bitmaps (magick).

enum { WINDOW_TIMER = -2*wxID_HIGHEST, RESIZE_TIMER, RESIZE_PLOT_TIMER }; //negative values, should not clash with our (positive) widget ids.

class DStructGDL;

class wxAppGDL;
//static GDLApp *theGDLApp=NULL;

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
  void Push( DStructGDL* ev)
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

// main App class
class wxAppGDL: public wxApp
{
public:
//  virtual int OnRun(); 
  virtual int OnExit();
 virtual int MainLoop();
// virtual int OneLoop();
 virtual bool OnInit();
// bool Pending(); //Returns true if unprocessed events are in the window system event queue.
// int FilterEvent(wxEvent& event) //This function is called before processing any event and 
//allows the application to preempt the processing of some events. If this method returns -1
//the event is processed normally, otherwise either true or false should be returned and 
//the event processing stops immediately considering that the event had been already processed
//(for the former return value) or that it is not going to be processed at all (for the latter one).
};
wxDECLARE_APP(wxAppGDL);

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
 wxAppGDL* appOwner;
 wxTimer * m_resizeTimer;
 GDLWidgetTopBase* gdlOwner;

public:

 // ctor(s)
 gdlwxFrame(wxWindow* parent, GDLWidgetTopBase* gdlOwner_, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style=wxDEFAULT_FRAME_STYLE);
 ~gdlwxFrame();
 // called from ~GDLWidgetBase
 void NullGDLOwner() {
  gdlOwner = NULL;
 }
 wxAppGDL* GetTheApp() {
  return appOwner;
 }

 void SetTheApp(wxAppGDL* myApp) {
  appOwner = myApp;
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
  //this->AddPendingEvent( *event); // copies event
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
 //  void OnTextEnter( wxCommandEvent& event); //NOT USED
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
private:
 void OnListBoxDo(wxCommandEvent& event, DLong clicks);
 DECLARE_EVENT_TABLE()
};

static int sysScrollHeight=25;
static int sysScrollWidth=25;
static int sysComboboxArrow=25;
  
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
  static GDLEventQueue eventQueue;
  static GDLEventQueue readlineEventQueue;
  static void PushEvent( WidgetIDT baseWidgetID, DStructGDL* ev);
  static void InformAuthorities(const std::string& message);
  
  static void HandleWidgetEvents();
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
  
  static void Init(); // global GUI intialization upon GDL startup
  static void UnInit(); // global GUI desinitialization in case it is useful (?)
  static bool wxIsStarted(){return (wxIsOn);}
  static void SetWxStarted(){wxIsOn=true;}
  static bool AreWxHandlersOk(){return (handlersOk);}
  static void SetWxHandlersOk(){handlersOk=true;}
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
  int widgetStyle; //style (alignment code + other specific codes used as option to widgetsizer). Needed only because of frame/unframe  function
  int dynamicResize; //for some widgets, will enable resizing: -1: not resizable, 0/1 resizable
  std::vector<WidgetIDT> followers; //all the widgets that use me as group_leader
  std::vector<WidgetEventInfo*> desiredEventsList; //list of all the events (and handlers) this widget must obey.
  DString      notifyRealize;
  
  wxTimer * m_windowTimer;
  
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
  DLong GetXPos(){return dynamic_cast<wxWindow*>(theWxWidget)->GetPosition().x;}
  DLong GetYPos(){return dynamic_cast<wxWindow*>(theWxWidget)->GetPosition().y;}
  bool IsValid(){return valid;}
  void SetUnValid(){valid=false;}
  void SetValid(){valid=true;}
  bool IsDynamicResize(){return ((dynamicResize>0)|| (dynamicResize==0 && !GetRealized())); }
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
  virtual DLongGDL* GetChildrenList() const {return new DLongGDL(0);}
  virtual void SetXmanagerActiveCommand() {}
  virtual bool GetXmanagerActiveCommand() const { return false;}

  void SetEventPro( const DString& ePro) { eventPro = StrUpCase( ePro);}
  const DString& GetEventPro() const { return eventPro;};
  void SetEventFun( const DString& eFun) { eventFun = StrUpCase( eFun);}
  const DString& GetEventFun() const { return eventFun;}
  void SetNotifyRealize( const DString& eNR) { notifyRealize = StrUpCase( eNR);}
  const DString& GetNotifyRealize() const { return notifyRealize;}
  void SetKillNotify( const DString& eKN) { killNotify = StrUpCase( eKN);}
  const DString& GetKillNotify() const { return killNotify;}

  static bool GetXmanagerBlock();
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
  bool GetRealized(); 
  void SetManaged( bool manval){managed = manval;}
  virtual void SetSensitive( bool value);
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

  void SendWidgetTimerEvent(DDouble secs) {
  WidgetIDT* id = new WidgetIDT(widgetID);
  int millisecs = floor(secs * 1000.0);
  if (theWxWidget) { //we nee a handle on a wxWindow object...
   wxWindow* w=dynamic_cast<wxWindow*>(theWxWidget);
   assert (w!=NULL);
   w->GetEventHandler()->SetClientData(id);
   if (m_windowTimer==NULL) {
    m_windowTimer=new wxTimer(w->GetEventHandler(),widgetID);
   }
#ifdef GDL_DEBUG_WIDGETS
   std::cerr<<"sending event,"<<widgetID<<","<<m_windowTimer<<std::endl;
#endif
   m_windowTimer->StartOnce(millisecs);
  }
 }
};

class GDLWidgetContainer: public GDLWidget
{
protected:
  std::deque<WidgetIDT> children;
public:
  GDLWidgetContainer( WidgetIDT parentID, EnvT* e, ULong eventFlags_=0);

  ~GDLWidgetContainer();
  
  virtual bool IsContainer() const { return true;}
  
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
  virtual bool IsNormalBase() const { return false;} 
  virtual bool IsTabbedBase() const { return false;} 
  void SetWidgetSize(DLong sizex, DLong sizey) final;
  void ClearEvents()
  {
  if (!this->GetXmanagerActiveCommand( ))  eventQueue.Purge();
  else readlineEventQueue.Purge(); 
  }
  
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
  dynamic_cast<wxWindow*>(theWxContainer)->Show(doMap);
 }
};

class GDLWidgetTopBase : public GDLWidgetBase {
 WidgetIDT mbarID;
 wxAppGDL* myGDLApp;
public:
 bool xmanActCom; //set by /XMANAGER_ACTIVE_COMMAND (GDL's) aka NO_BLOCK . indirectly used in pushEvent, selfDestroy, ~GDLWidgetContainer, ClearEvents
 gdlwxFrame* topFrame;
 bool modal;
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
 wxAppGDL* GetTopApp(){
  return myGDLApp;
 }
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
  topFrame->refreshFrameSize();
  DoMapAsRequested();
  ConnectToDesiredEvents();
  
  if (notifyRealize != "") { //insure it is called once only for this.
   std::string note = notifyRealize;
   notifyRealize.clear();
   CallEventPro(note, new DLongGDL(widgetID));
  }
 }

// void SelfDestroy(); // sends delete event to itself

 void SetXmanagerActiveCommand() {
  xmanActCom = true;
 }

 bool GetXmanagerActiveCommand() const {
  return xmanActCom;
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
 bool IsTabbedBase() const {  return true; }
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
  wxBitmap* buttonBitmap;
  wxMenuItem* menuItem;
  bool       buttonState; //only for buttons
  wxString valueWxString;
  
  GDLWidgetButton( WidgetIDT parentID, EnvT* e, DStringGDL* value, DULong eventflags, wxBitmap* bitmap=NULL);
  ~GDLWidgetButton();
  // for WIDGET_CONTROL

  void SetButtonWidget( bool onOff)
  {
    if( theWxWidget != NULL)
    {
      switch( buttonType) {
        case RADIO: {	  
          SetButton( onOff);
          wxRadioButton* radioButton = dynamic_cast<wxRadioButton*>(theWxWidget);
          radioButton->SetValue(onOff);
          break;
        }
        case CHECKBOX: {
          SetButton( onOff);
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
  void SetButton( bool onOff)
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

class GDLWidgetMbarButton: public GDLWidgetMenu {
 int entry;
public:
 GDLWidgetMbarButton(WidgetIDT parentID, EnvT* e, DStringGDL* value, DULong eventflags, DStringGDL* buttonTooltip = NULL);
 ~GDLWidgetMbarButton();
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
public:
 GDLWidgetMenuEntry( WidgetIDT parentID, EnvT* e, DStringGDL* value, DULong eventflags, bool hasSeparatorAbove=false, wxBitmap* bitmap=NULL);
 ~GDLWidgetMenuEntry();
 bool IsEntry() const final {return true;}
 void SetSensitive(bool value);
 void SetButtonWidgetLabelText( const DString& value_ );
 void SetButtonWidgetBitmap( wxBitmap* bitmap );
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
 //Same as Container except that we have to reorder widgets in some cases
//
// void OnRealize() {
//  GDLWidgetContainer::OnRealize();
// }

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
//  DStructGDL* GetGeometry(wxRealPoint fact=wxRealPoint(1.0,1.0));
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
  int maxlinelength;
  int nlines;
  bool wrapped;
public:
  GDLWidgetText( WidgetIDT parentID, EnvT* e, DStringGDL* value, DULong eventflags, bool noNewLine,
		 bool editable);
  ~GDLWidgetText();
  
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

//  void OnRealize();
  bool IsDraw() const final { return true;}
  void AddEventType( DULong evType) final; //specific for draw widgets
  void RemoveEventType( DULong evType) final;
  void SetWidgetSize(DLong sizex, DLong sizey) final; 
  void SetWidgetVirtualSize(DLong sizex, DLong sizey) final; 
  void SetWidgetScreenSize(DLong sizex, DLong sizey) final;
  void UnrefTheWxContainer(){theWxContainer=NULL;} 
  void UnrefTheWxWidget(){theWxWidget=NULL;} 
};


// menu bar widget **************************************************
class GDLWidgetMBar: public GDLWidget
{
protected:
  std::deque<WidgetIDT> children;
  ~GDLWidgetMBar();
public:
  GDLWidgetMBar( WidgetIDT p, EnvT* e): 
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
  DLong x_scroll_size;
  DLong y_scroll_size;
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


  int  GetMajority(){return majority;}
  bool IsTable() const final { return true;}
  void SetDOW(DStringGDL* val){GDLDelete(daysOfWeek); daysOfWeek=val->Dup();}
  void SetAmPm(DStringGDL* val){GDLDelete(amPm); amPm=val->Dup();};
  void SetMonth(DStringGDL* val){GDLDelete(month); month=val->Dup();};

  DLongGDL* GetSelection();
  
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

  bool InsertColumns(DLong count, DLongGDL* selection=NULL);
  bool InsertRows(DLong count, DLongGDL* selection=NULL);

  void SetSelection(DLongGDL* selection);
  DStringGDL* GetTableValues(DLongGDL* selection=NULL);
  BaseGDL* GetTableValuesAsStruct(DLongGDL* selection=NULL);
  void SetTableValues(DStringGDL *val, DLongGDL* selection=NULL);
  void SetValue(BaseGDL * val){GDLDelete(vValue); vValue=val->Dup();};
  
  void SetTableView(DLongGDL* pos);
  void EditCell(DLongGDL* pos);
  void SetTableNumberOfColumns( DLong ncols);
  void SetTableNumberOfRows( DLong nrows);
  
  bool IsSomethingSelected();
  
  bool IsUpdating(){return updating;}
  void ClearUpdating(){updating=false;}
  void SetUpdating(){updating=true;}
  DStructGDL* GetGeometry(wxRealPoint fact=wxRealPoint(1.0,1.0)) final;
  void setFont();
};


// tree widget **************************************************
class wxTreeCtrlGDL: public wxTreeCtrl
{  
  wxWindowID GDLWidgetTableID;
public:
  wxTreeCtrlGDL(wxWindow *parent, wxWindowID id = wxID_ANY,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxTR_DEFAULT_STYLE,
               const wxValidator &validator = wxDefaultValidator,
               const wxString& name = wxTreeCtrlNameStr)
          :wxTreeCtrl( parent, id, pos, size, style, wxDefaultValidator , name ),
          GDLWidgetTableID(id)
          {
//            Connect(GDLWidgetTableID, wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler(wxTreeCtrlGDL::OnItemActivated));
//            Connect(GDLWidgetTableID, wxEVT_COMMAND_TREE_ITEM_ACTIVATED,wxTreeEventHandler(wxTreeCtrlGDL::OnItemActivated));
//            Connect(GDLWidgetTableID, wxEVT_COMMAND_TREE_BEGIN_DRAG,wxTreeEventHandler(wxTreeCtrlGDL::OnBeginDrag));
//            Connect(GDLWidgetTableID, wxEVT_COMMAND_TREE_END_DRAG,wxTreeEventHandler(wxTreeCtrlGDL::OnItemDropped));
//            Connect(GDLWidgetTableID, wxEVT_COMMAND_TREE_ITEM_COLLAPSED,wxTreeEventHandler(wxTreeCtrlGDL::OnItemCollapsed));
//            Connect(GDLWidgetTableID, wxEVT_COMMAND_TREE_ITEM_EXPANDED,wxTreeEventHandler(wxTreeCtrlGDL::OnItemExpanded));
//            Connect(GDLWidgetTableID, wxEVT_COMMAND_TREE_SEL_CHANGED,wxTreeEventHandler(wxTreeCtrlGDL::OnItemSelected));
          }
//necessary to define the destructor otherwise compiler will try to find the bind event table for destruction event!
  ~wxTreeCtrlGDL(){}
void OnItemActivated(wxTreeEvent & event);
void OnItemCollapsed(wxTreeEvent & event);
void OnItemExpanded(wxTreeEvent & event);
void OnBeginDrag(wxTreeEvent & event);
void OnItemDropped(wxTreeEvent & event);
void OnItemSelected(wxTreeEvent & event);
};

class wxTreeItemDataGDL : public wxTreeItemData {
  public:
    WidgetIDT widgetID;

    wxTreeItemDataGDL(WidgetIDT id) : widgetID(id) {}
};

class GDLWidgetTree: public GDLWidget
{
bool droppable  ; 
bool draggable  ; 
bool expanded;
bool folder;
int buttonImageId;
int imageId;
WidgetIDT selectedID;
wxTreeItemId treeItemID;
wxTreeItemDataGDL* treeItemData;
WidgetIDT rootID;

public:
GDLWidgetTree( WidgetIDT p, EnvT* e, BaseGDL* value_, DULong eventFlags
,wxBitmap* bitmap_
,DLong dropability
,DLong dragability
,bool expanded_
,bool folder_
,DLong treeindex
);
		 
~GDLWidgetTree();

  bool IsTree() const final { return true;}
  bool IsDraggable() {return draggable;}
  bool IsDroppable() {return droppable;}
  bool IsFolder() {return folder;}
  bool IsExpanded() {return expanded;}
  void DoExpand(){
    wxTreeCtrlGDL * me = dynamic_cast<wxTreeCtrlGDL*>(theWxWidget);
    if (me) me->Expand(treeItemID);
  }
  WidgetIDT GetRootID(){ return rootID;}
  void SetSelectedID( WidgetIDT id){selectedID=id;}
  WidgetIDT GetSelectedID(){ return selectedID;}
  DInt GetTreeIndex();
  wxTreeItemId GetItemID(){ return treeItemID;}
  void SetValue(DString val);
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
  
   wxArrayInt GetSortedSelectedColsList(){
   std::vector<wxPoint> list=GetSelectedDisjointCellsList();
   wxArrayInt cols;
   if (list.empty()) return cols; 
   std::vector<wxPoint>::iterator iPoint;
   std::vector<int> allCols;
   std::vector<int>::iterator iter;
   for ( iPoint = list.begin(); iPoint !=list.end(); ++iPoint) {
       allCols.push_back((*iPoint).y);
    }
   std::sort (allCols.begin(), allCols.end());
   int theCol=-1;
   for ( iter = allCols.begin(); iter !=allCols.end(); ++iter) {
       if ((*iter)!=theCol) {theCol=(*iter);cols.Add(theCol);}
    }
   return cols;
  }
  wxArrayInt GetSortedSelectedRowsList(){
   std::vector<wxPoint> list=GetSelectedDisjointCellsList();
   wxArrayInt rows;
   if (list.empty()) return rows; 
   std::vector<wxPoint>::iterator iPoint;
   std::vector<int> allRows;
   std::vector<int>::iterator iter;
   for ( iPoint = list.begin(); iPoint !=list.end(); ++iPoint) {
       allRows.push_back((*iPoint).x);
    }
   std::sort (allRows.begin(), allRows.end());
   int theRow=-1;
   for ( iter = allRows.begin(); iter !=allRows.end(); ++iter) {
       if ((*iter)!=theRow) {theRow=(*iter);rows.Add(theRow);}
    }
   return rows;
  }

  void OnTableCellSelection(wxGridEvent & event);
  void OnTableRangeSelection(wxGridRangeSelectEvent & event);
  void OnTableColResizing(wxGridSizeEvent & event);
  void OnTableRowResizing(wxGridSizeEvent & event); 
//  void OnText( wxCommandEvent& event);
//  void OnTextEnter( wxCommandEvent& event);
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
 wxAppGDL* myGDLApp;
 gdlwxPhantomFrame();
 void Realize();
};


// basic for (completely separated from widgets) plot windows done with wxWidgets driver.
class gdlwxPlotFrame : public wxFrame {
 wxTimer * m_resizeTimer;
 wxSize frameSize;
 bool scrolled;
public:
 wxAppGDL* myGDLApp;
 // ctor(s)
 gdlwxPlotFrame(const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, bool scrolled=false);
 ~gdlwxPlotFrame();
  wxAppGDL* GetTopApp(){
  return myGDLApp;
 }
  bool IsScrolled(){return scrolled;}
 void SetTheApp(wxAppGDL* app){myGDLApp=app;} 
 void Realize();
// event handlers (these functions should _not_ be virtual)
// void OnClosePlotFrame(wxCloseEvent & event);
// void OnPlotSizeWithTimer(wxSizeEvent& event);
 void OnTimerPlotResize(wxTimerEvent& event);
 void OnUnhandledClosePlotFrame(wxCloseEvent & event);
 void OnPlotSizeWithTimer(wxSizeEvent& event);
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
 wxMemoryDC* m_dc;

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
 
 void ResizeDrawArea(const wxSize s);

 void SendPaintEvent() {
  wxPaintEvent* event;
  event = new wxPaintEvent(GetId());
  event->SetEventObject(this);
  // only for wWidgets > 2.9 (takes ownership of event)
  //     this->QueueEvent( event);
  this->AddPendingEvent(*event); // copies event
  delete event;
 }
 void DeleteUsingWindowNumber();
 void SetStream(GDLWXStream* s);

 void OnPaint(wxPaintEvent& event);
 virtual void OnPlotWindowSize(wxSizeEvent &event);
 
// virtual bool isPlot() const {return false;}


 void RepaintGraphics(bool doClear = false) {
  wxClientDC dc(this); //is a scrolled window: needed
  DoPrepareDC(dc); //you probably do not want to call wxScrolled::PrepareDC() on wxAutoBufferedPaintDC as it already does this internally for the real underlying wxPaintDC.
  if (doClear) dc.Clear();
 // dc.SetDeviceClippingRegion(GetUpdateRegion());
  dc.Blit(0, 0, drawSize.x, drawSize.y, m_dc, 0, 0);
 }
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

// virtual gdlWidgetDraw* GetGDLWidgetDraw() {
//  return NULL;
// }
 };
//variant used in widget_draw, with specifics for widgets.
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
 void OnMouseWheel(wxMouseEvent& event);
 void OnKey(wxKeyEvent& event);
 //  void OnSize(wxSizeEvent &event);
// void OnPlotWindowSize(wxSizeEvent &event);

};

#endif

#endif

