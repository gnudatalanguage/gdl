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

#include <wx/wx.h>

#include <deque>
#include <map>

#include "typedefs.hpp"
#include "str.hpp"
#include "datatypes.hpp"
#include "widget.hpp"

typedef DLong WidgetIDT;

class DStructGDL;

// thread save deque
class GDLEventQueue
{
private:
  std::deque<DStructGDL*> dq;
  wxMutex mutex;
public:
  GDLEventQueue()
  {}
  
  DStructGDL* Pop()
  {
    if( dq.empty()) // optimzation: aquiring of mutex not necessary at first
      return NULL;   
    wxMutexLocker lock( mutex);
    if( dq.empty()) // needed again for thread save behaviour
      return NULL;   
    DStructGDL* front = dq.front();
    dq.pop_front();
    return front;
  }
  // for all regular events
  void Push( DStructGDL* ev)
  {
    wxMutexLocker lock( mutex);
    dq.push_back( ev);
  }
  // for priority events (like delete widget)
  void PushFront( DStructGDL* ev)
  {
    wxMutexLocker lock( mutex);
    dq.push_back( ev);
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
#define GUIMutexLockerWidgetsT_OFF
class GUIMutexLockerWidgetsT
{
#ifdef GUIMutexLockerWidgetsT_OFF
public:
  GUIMutexLockerWidgetsT() {}
  ~GUIMutexLockerWidgetsT() {}
  void Leave() {}
#else
  bool left;
public:
  GUIMutexLockerWidgetsT(): left(false) { wxMutexGuiEnter();}
  ~GUIMutexLockerWidgetsT() { if(!left) wxMutexGuiLeave();}
  void Leave() { wxMutexGuiLeave(); left=true;}
#endif  
};
#define GUIMutexLockerEventHandlersT_OFF
class GUIMutexLockerEventHandlersT
{
#ifdef GUIMutexLockerEventHandlersT_OFF
public:
  GUIMutexLockerEventHandlersT() {}
  ~GUIMutexLockerEventHandlersT() {}
  void Leave() {}
#else
  bool left;
public:
  GUIMutexLockerEventHandlersT(): left(false) { wxMutexGuiEnter();}
  ~GUIMutexLockerEventHandlersT() { if(!left) wxMutexGuiLeave();}
  void Leave() { wxMutexGuiLeave(); left=true;}
#endif  
};

class GDLGUIThread : public wxThread
{
//   bool exited;
public:
  static GDLGUIThread* gdlGUIThread;

  GDLGUIThread() : wxThread(wxTHREAD_DETACHED)//wxTHREAD_JOINABLE)
//   , exited(false)
  {}
  ~GDLGUIThread();

//   bool Exited() const { return exited;}

  // thread execution starts here
  ExitCode Entry();

  // called when the thread exits - whether it terminates normally or is
  // stopped with Delete() (but not when it is Kill()ed!)
  void OnExit();
  
//   void Exit(); // end this
};

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
  wxMutex m_mutex;

public:
  WidgetListT(): map(), m_mutex() {}
  ~WidgetListT() {}
  
  void erase (iterator position) 
  { 
    wxMutexLocker lock(m_mutex);
    map.erase(position);
  }
  size_type erase (const key_type& k) 
  { 
    wxMutexLocker lock(m_mutex);
    return map.erase(k);
  }
  iterator find (const key_type& k) 
  { 
    wxMutexLocker lock(m_mutex);
    return map.find(k);
  }
  iterator begin() 
  { 
    wxMutexLocker lock(m_mutex);
    return map.begin();
  }
  iterator end() 
  { 
    wxMutexLocker lock(m_mutex);
    return map.end();
  }

  iterator insert (iterator position, value_type val) 
  { 
    wxMutexLocker lock(m_mutex);
    return map.insert( position, val);    
  }
};


// main App class
class GDLApp: public wxApp
{
public:
  int OnRun(); 
  int OnExit(); 
};

// GUI base class **********************************
class GDLWidgetBase;
class GDLWidget
{ 
  // static part is used for the abstraction
  // all widgets are refered to as IDs
private:
  // the global widget list 
  // a widget is added by the constructor and removed by the destructor
  // so no other action is necessary for list handling
  static WidgetListT widgetList;

public:
  static GDLEventQueue eventQueue;
  static GDLEventQueue readlineEventQueue;
  static void PushEvent( WidgetIDT baseWidgetID, DStructGDL* ev);

  static WidgetIDT HandleEvents();
  static const WidgetIDT NullID;
  
  // get widget from ID
  static GDLWidget* GetWidget( WidgetIDT widID);
  static GDLWidget* GetParent( WidgetIDT widID);
  static GDLWidgetBase* GetTopLevelBaseWidget( WidgetIDT widID);
  static GDLWidgetBase* GetBaseWidget( WidgetIDT widID);

  // get ID of base widgets
  static WidgetIDT  GetBase( WidgetIDT widID);
  static WidgetIDT  GetTopLevelBase( WidgetIDT widID);

  static void Init(); // GUI intialization upon GDL startup

protected:
// only TLB have to care for this
// (they do by sending messgages to each other in a thread save way)
// as the rest is deleted automatically 
// Note: wxWidget is GDL name not wxWidgets (JMG)
  wxObject* wxWidget;

  WidgetIDT    widgetID;  // own index to widgetList
  WidgetIDT    parentID;  // parent ID (0 for TLBs)
  BaseGDL*     uValue;    // the UVALUE
  BaseGDL*     vValue;    // the VVALUE
  bool         scroll;
  bool         sensitive;
  bool         managed;
  bool         map;
  bool         buttonSet;
  int          exclusiveMode;
  DLong        xOffset, yOffset, xSize, ySize, scrXSize, scrYSize;
  wxSizer*     topWidgetSizer;
  wxSizer*     widgetSizer;
  wxPanel*     widgetPanel;
  DString      widgetType;
  WidgetIDT    groupLeader;
  DLong        units;
  DLong        frame;
  
private:  
  DString      uName;
  DString      proValue;
  DString      funcValue;
  DString      eventPro; // event handler PRO
  DString      eventFun; // event handler FUN
  DString      notifyRealize;
  DString      killNotify;
  
  void SetCommonKeywords( EnvT* e);

public:
  typedef enum BGroupMode_ {
    BGNORMAL=0, BGEXCLUSIVE=1, BGNONEXCLUSIVE=2, BGEXCLUSIVE1ST=3 } BGroupMode;

  GDLWidget( WidgetIDT p, EnvT* e, bool map_=true, BaseGDL* vV=NULL);

  virtual ~GDLWidget();

  void CreateWidgetPanel();
  // this is called from the GUI thread on (before) Show()
  // wxTextCtrl and maybe other controls crash when called from the
  // main thread
  virtual void OnShow() {}
  // this is called from the main thread on (before) Realize()
  // for latest initialzation (like allocating the plplot stream)
  // calls NOTIFY_REALIZE procedure
  virtual void OnRealize() 
  {
    if( notifyRealize != "")
      CallEventPro( notifyRealize, new DLongGDL( widgetID));
  }
  virtual void OnKill()
  {
    if( killNotify != "")
      CallEventPro( killNotify, new DLongGDL( widgetID));
  }

  void SetSizeHints();
  
  WidgetIDT GetParentID() const { return parentID;}
  
  wxObject* GetWxWidget() const { return wxWidget;}

  BaseGDL* GetUvalue() const { return uValue;}
  BaseGDL* GetVvalue() const { return vValue;}

  void Realize( bool);

  // for query of children
  virtual bool IsBase() const { return false;} 
  virtual bool IsButton() const { return false;} 
  virtual bool IsDropList() const { return false;} 
  virtual bool IsTab() const { return false;}
  virtual bool IsText() const { return false;} 
  virtual bool IsSlider() const { return false;}
  virtual bool IsDraw() const { return false;}

  virtual WidgetIDT GetChild( DLong) const {return NullID;}
  virtual DLong NChildren() const {return 0;}
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

  WidgetIDT WidgetID() { return widgetID;}

  wxSizer* GetSizer() { return widgetSizer;}
  wxPanel* GetPanel() { return widgetPanel;}

  bool GetManaged() const { return managed;}
  void SetManaged( bool manval){managed = manval;}

  bool GetMap() const { return map;}
  void SetMap( bool mapval){ map = mapval;}

  int  GetExclusiveMode() const { return exclusiveMode;}
  void SetExclusiveMode( int exclusiveval){exclusiveMode = exclusiveval;}

  void SetUvalue( BaseGDL *uV){uValue = uV;}
  void SetVvalue( BaseGDL *vV){vValue = vV;}

  const DString& GetWidgetType() const { return widgetType;}
  void SetWidgetType( const DString& wType){widgetType = wType;}

  bool GetButtonSet() const { return buttonSet;}
  void SetButtonSet(bool onOff){buttonSet = onOff;}

  const DString& GetUname() const { return uName;}
  void SetUname( const DString& uname){uName = uname;}

  const DString& GetProValue() const { return proValue;}
  void SetProValue( const DString& provalue){proValue = StrUpCase(provalue);}

  const DString& GetFuncValue() const { return funcValue;}
  void SetFuncValue( const DString& funcvalue){funcValue = StrUpCase(funcvalue);}
};


// base widget **************************************************
class GDLWidgetBase: public GDLWidget
{
protected:
  typedef std::deque<WidgetIDT>::iterator cIter;
  std::deque<WidgetIDT>                   children;
  
  bool                                    xmanActCom;
  bool                                    modal;
  WidgetIDT                               mbarID;
  // for radio buttons to generate deselect event
  WidgetIDT                               lastRadioSelection;

  wxMutex*                                m_gdlFrameOwnerMutexP;

public:
  GDLWidgetBase( WidgetIDT parentID, EnvT* e,
		 bool mapWid,
		 WidgetIDT& mBarIDInOut, bool modal, 
		 DLong col, DLong row,
		 long events,
		 int exclusiveMode, 
		 bool floating,
		 const DString& resource_name, const DString& rname_mbar,
		 const DString& title,
		 const DString& display_name,
		 DLong xpad, DLong ypad,
		 DLong x_scroll_size, DLong y_scroll_size);
  
  ~GDLWidgetBase();

  void OnShow() 
  {
    for( cIter c=children.begin(); c!=children.end(); ++c)
    {
      GDLWidget* w = GetWidget( *c);
      if( w != NULL)
	w->OnShow();
    }
  }
  void OnRealize() 
  {
    for( cIter c=children.begin(); c!=children.end(); ++c)
    {
      GDLWidget* w = GetWidget( *c);
      if( w != NULL)
	w->OnRealize();
    }
    GDLWidget::OnRealize();
  }
  void OnKill()
  {
    for( cIter c=children.begin(); c!=children.end(); ++c)
    {
      GDLWidget* w = GetWidget( *c);
      if( w != NULL)
	w->OnKill();
    }
    GDLWidget::OnKill();
  }

  void NullWxWidget() { this->wxWidget = NULL;}
  
  WidgetIDT GetLastRadioSelection() const { return lastRadioSelection;}                         
  void SetLastRadioSelection(WidgetIDT lastSel) { lastRadioSelection = lastSel;}                         

  // as this is called in the constructor, no type checking of c can be done
  // hence the AddChild() function should be as simple as that
  void AddChild( WidgetIDT c) { children.push_back( c);}
  void RemoveChild( WidgetIDT  c) { children.erase( find( children.begin(),
							  children.end(), c));}

  void Realize( bool);
  
  void Destroy(); // sends delete event to itself
  
  void SetXmanagerActiveCommand() 
  { 
//     wxMessageOutputDebug().Printf(_T("SetXmanagerActiveCommand: %d\n",widgetID);
    xmanActCom = true;
  }
  bool GetXmanagerActiveCommand() const 
  { 
//     wxMessageOutputDebug().Printf(_T("GetXmanagerActiveCommand: %d\n",widgetID);
    return xmanActCom;
  }

//   void SetEventPro( DString);
//   const DString& GetEventPro() { return eventHandler;}

  WidgetIDT GetChild( DLong) const;
  DLong NChildren() const;

  bool IsBase() const { return true;} 
};



// class GDLWidgetMbar;

// button widget **************************************************
class GDLWidgetButton: public GDLWidget
{
public:
  GDLWidgetButton( WidgetIDT parentID, EnvT* e, const DString& value);

  bool IsButton() const { return true;} 

//   void SetSelectOff();
};


// droplist widget **************************************************
class GDLWidgetDropList: public GDLWidget
{
  std::string lastValue;
  wxMutex m_mutex;
  DString title;
  DLong style;
  
public:
  //  GDLWidgetDropList( WidgetIDT p, BaseGDL *uV, DStringGDL *value,
  //	     DString title, DLong xSize, DLong style);
  GDLWidgetDropList( WidgetIDT p, EnvT* e, BaseGDL *value,
		     const DString& title, DLong style);

  void OnShow();
  
//   void SetSelectOff();
  bool IsDropList() const { return true;} 

  void SetLastValue( const std::string& v) { m_mutex.Lock(); lastValue = v; m_mutex.Unlock();}
  std::string GetLastValue() { wxMutexLocker lock(m_mutex); return lastValue;}
};

// list widget **************************************************
class GDLWidgetList : public GDLWidget
{
public:
  GDLWidgetList( WidgetIDT p, EnvT* e, BaseGDL *value, DLong style);
//   void SetSelectOff();
};

// bgroup widget **************************************************
class GDLWidgetBGroup: public GDLWidget
{
public:
	typedef enum e_BGRoupReturn {RETURN_ID, RETURN_INDEX, RETURN_NAME}
	BGroupReturn;

	GDLWidgetBGroup(WidgetIDT p, DStringGDL* names,
			BaseGDL *uV, DString buttonuvalue,
			DLong xSize, DLong ySize,
			DString labeltop, DLong rows, DLong cols,
			BGroupMode mode, BGroupReturn ret
			);

};

// text widget **************************************************
class GDLWidgetText: public GDLWidget
{
  std::string lastValue;
  wxMutex m_mutex;
  bool noNewLine;
  bool editable;
public:
  GDLWidgetText( WidgetIDT parentID, EnvT* e, DStringGDL* value, bool noNewLine,
		 bool editable);
  void OnShow();

  void SetTextValue( DStringGDL* value, bool noNewLine);
  
  bool IsText() const { return true;} 
  
  void SetLastValue( const std::string& v) { m_mutex.Lock(); lastValue = v; m_mutex.Unlock();}
  std::string GetLastValue() { wxMutexLocker lock(m_mutex); return lastValue;}
};


// label widget **************************************************
class GDLWidgetLabel: public GDLWidget
{
  DString value;
public:
  GDLWidgetLabel( WidgetIDT parentID, EnvT* e, const DString& value_);
  void OnShow();
 
  void SetLabelValue( const DString& value_);
};


// draw widget **************************************************
class GDLWidgetDraw: public GDLWidget
{
  int pstreamIx;
  DLong x_scroll_size;
  DLong y_scroll_size;
public:
  GDLWidgetDraw( WidgetIDT parentID, EnvT* e,
		  DLong x_scroll_size, DLong y_scroll_size);

  ~GDLWidgetDraw();

//   void OnShow();
  void OnRealize();
  
  bool IsDraw() const { return true;}
};

class GDLWidgetMBar: public GDLWidget//Base
{
  // disable
  GDLWidgetMBar();
public:
  GDLWidgetMBar( WidgetIDT p): 
  GDLWidget( p, NULL)
  {
    this->wxWidget = new wxMenuBar();
  }
};

// tab widget **************************************************
class GDLWidgetTab: public GDLWidget
{
public:
  GDLWidgetTab( WidgetIDT parentID, EnvT* e, DLong location, DLong multiline);

  ~GDLWidgetTab();
  
  bool IsTab() const { return true;}
};

// slider widget **************************************************
class GDLWidgetSlider: public GDLWidget
{
  DLong value; 
  DLong minimum; 
  DLong maximum;
public:
  GDLWidgetSlider( WidgetIDT parentID, EnvT* e,
		   DLong value_, DLong minimum_, DLong maximum_,
		   bool vertical,
		   bool suppressValue);

  ~GDLWidgetSlider();

  void SetValue( DLong v) { value = v;}
  DLong GetValue() const { return value;}
  
  bool IsSlider() const { return true;}
};



// GDL versions of wxWidgets controls =======================================
DECLARE_EVENT_TYPE(wxEVT_SHOW_REQUEST, -1)
DECLARE_EVENT_TYPE(wxEVT_HIDE_REQUEST, -1)

class wxNotebookEvent;
class GDLFrame : public wxFrame
{
  bool lastShowRequest;
  GDLWidgetBase* gdlOwner;
  void OnListBoxDo( wxCommandEvent& event, DLong clicks);

  // called from ~GDLWidgetBase
  void NullGDLOnwer() { gdlOwner = NULL;}
  wxMutex* m_gdlFrameOwnerMutexP;
  friend class GDLWidgetBase;
public:
  // ctor(s)
  GDLFrame(GDLWidgetBase* gdlOwner_, wxWindow* parent, wxWindowID id, const wxString& title);
  ~GDLFrame();

  
  // event handlers (these functions should _not_ be virtual)
  void OnIdle( wxIdleEvent& event);
  void OnButton( wxCommandEvent& event);
  void OnRadioButton( wxCommandEvent& event);
  void OnCheckBox( wxCommandEvent& event);
  void OnComboBox( wxCommandEvent& event);
  void OnListBox( wxCommandEvent& event);
  void OnListBoxDoubleClicked( wxCommandEvent& event);
  void OnText( wxCommandEvent& event);
  void OnTextEnter( wxCommandEvent& event);
  void OnPageChanged( wxNotebookEvent& event);
//   void OnSlider( wxCommandEvent& event);
  void OnScroll( wxScrollEvent& event);
  void OnThumbRelease( wxScrollEvent& event);

  bool LastShowRequest() const { return lastShowRequest;}
  
  void SendShowRequestEvent( bool show)
  {
    wxCommandEvent* event;
    if( show)
    {
    event = new wxCommandEvent( wxEVT_SHOW_REQUEST, GetId() );
    }
    else
    {
    event = new wxCommandEvent( wxEVT_HIDE_REQUEST, GetId() );
    }
    event->SetEventObject( this );
    // only for wWidgets > 2.9 (takes ownership of event)
//     this->QueueEvent( event);
    
    this->AddPendingEvent( *event); // copies event
    delete event;

    lastShowRequest = show;
  }
  void OnShowRequest( wxCommandEvent& event);
  void OnHideRequest( wxCommandEvent& event);
  

  // any class wishing to process wxWidgets events must use this macro
  DECLARE_EVENT_TABLE()
};


class GDLWXStream;

class GDLDrawPanel : public wxPanel
{
  int		pstreamIx;
  GDLWXStream*	pstreamP;

  wxSize 	drawSize;

  wxDC*  	m_dc;
//   wxBitmap*    	memPlotDCBitmap;
//   GDLWXStream*	PStream();
  
public:
  // ctor(s)
  GDLDrawPanel(wxWindow* parent, wxWindowID id, 
	    const wxPoint& pos = wxDefaultPosition, 
	    const wxSize& size = wxDefaultSize,
	    long style = 0, 
	    const wxString& name = wxPanelNameStr);
  ~GDLDrawPanel();
  
  void Update();
  
//   void SetPStreamIx( int ix) { pstreamIx = ix;}
  int PStreamIx() { return pstreamIx;}

  void InitStream();
  
  // event handlers (these functions should _not_ be virtual)
  void OnPaint(wxPaintEvent& event);
  void OnShow(wxShowEvent& event);
  void OnClose(wxCloseEvent& event);

//   void OnCreate(wxWindowCreateEvent& event);
//   void OnDestroy(wxWindowDestroyEvent& event);
  void SendPaintEvent()
  {
    wxPaintEvent* event;
    event = new wxPaintEvent( GetId());
    event->SetEventObject( this);
    // only for wWidgets > 2.9 (takes ownership of event)
//     this->QueueEvent( event);
    
    this->AddPendingEvent( *event); // copies event
    delete event;
  }

  
// private:
  // any class wishing to process wxWidgets events must use this macro
  DECLARE_EVENT_TABLE()
};

#endif

#endif

