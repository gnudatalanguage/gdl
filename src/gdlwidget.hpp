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
#include <wx/treebase.h>
#include <wx/treectrl.h>
#include <wx/defs.h>

#include <deque>
#include <map>

#include "typedefs.hpp"
#include "str.hpp"
#include "datatypes.hpp"
#include "widget.hpp"
#include "plotting.hpp"

#define SCROLL_WIDTH 10
#define DEFAULT_BORDER_SIZE 3

typedef DLong WidgetIDT;

class DStructGDL;

class GDLApp;
//static GDLApp *theGDLApp=NULL;

// thread safe deque
class GDLEventQueue
{
private:
  std::deque<DStructGDL*> dq;
  wxMutex mutex;
public:
  GDLEventQueue() //normally we should have ~GDLEventQueue removing the DStructGDLs?
  {}
  
  DStructGDL* Pop()
  {
    if( dq.empty()) // optimization: acquiring of mutex not necessary at first
      return NULL;   
    wxMutexLocker lock( mutex);
    if( dq.empty()) // needed again for thread safe behaviour
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
  wxMutex m_mutex;

public:
  WidgetListT(): map(), m_mutex() {}
  ~WidgetListT() {}
  
  void erase (iterator position) 
  { 
    wxMutexLocker lock(m_mutex);
    map.erase(position);
    cerr <<"deletep,size="<<map.size()<<endl;
  }
  size_type erase (const key_type& k) 
  { 
    wxMutexLocker lock(m_mutex);
    return map.erase(k);
    cerr <<"deletek,size="<<map.size()<<endl;
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
  static void RefreshWidgets();
  
  // get ID of base widgets
  static WidgetIDT  GetBase( WidgetIDT widID);
  static WidgetIDT  GetTopLevelBase( WidgetIDT widID);

  static void Init(); // global GUI intialization upon GDL startup


protected:
  
// only TLB have to care for this
// (they do by sending messages to each other in a thread save way)
// as the rest is deleted automatically 
// Note: wxWidget is GDL name not wxWidgets (JMG)
  wxObject* wxWidget;

  WidgetIDT    widgetID;  // own index to widgetList
  WidgetIDT    parentID;  // parent ID (0 for TLBs)
  BaseGDL*     uValue;    // the UVALUE
  BaseGDL*     vValue;    // the VVALUE
  bool         scrolled;
  bool         sensitive;
  bool         managed;
  bool         map;
  bool         buttonState; //only for buttons
  int          exclusiveMode;
  DLong        xOffset, yOffset, xSize, ySize, scrXSize, scrYSize;
  wxSizer*     topWidgetSizer; //the frame sizer (contains all widgets)
  wxSizer*     widgetSizer; // the sizer which governs the placement of the widget in its Panel
  wxPanel*     widgetPanel; // the Panel in which the widget is placed
  wxSizer*     scrollSizer; // the sizer for the (optional) Scroll Panel
  wxScrolledWindow*     scrollPanel; // Panel with scrollBars in which the widget may be shown
  wxSizer*     frameSizer;  // the sizer of the (optional) "frame" (its not a wxFrame its a StaticBox) drawn around a widget
  wxPanel*     framePanel;  // the corresponding panel
  DString      widgetType;
  WidgetIDT    groupLeader;
  DLong        units;
  DLong        frame;
  DString      font;
  long  alignment; //alignment of the widget
  long widgetStyle; //style (alignment code + other specific codes used as option to widgetsizer) 

  
private:  
  DULong eventFlags; // event types widget should reply to

  DString      uName;
  DString      proValue;
  DString      funcValue;
  DString      eventPro; // event handler PRO
  DString      eventFun; // event handler FUN
  DString      notifyRealize;
  DString      killNotify;
  
  void SetCommonKeywords( EnvT* e);

  
public:
  typedef enum BGroupMode_ 
  { BGNORMAL=0
  , BGEXCLUSIVE=1
  , BGNONEXCLUSIVE=2
  , BGEXCLUSIVE1ST=3 
  } BGroupMode;

  typedef enum EventTypeFlags_ 
    { NONE = 0
    , ALL = 1
    , CONTEXT = 2
    , KBRD_FOCUS = 4
    , TRACKING = 8 
    , DROP = 16
    , EXPOSE = 32
    , MOTION = 64
    , VIEWPORT = 128
    , WHEEL = 256
    , BUTTON = 512
    , KEYBOARD = 1024 //widget_draw, normal keys in the KEY field, modifiers reported in the "MODIFIERS" field
    , KEYBOARD2 = 2048 //widget_draw, normal keys and compose keys reported in the KEY field
    , SIZE = 4096
    , MOVE = 8192
    , ICONIFY = 16384
    , KILL = 32768
    } EventTypeFlags;
 
 
  virtual void updateFlags(); //to be overloaded...
  DULong GetEventFlags()  const { return eventFlags;}
  bool SetEventFlags( DULong evFlags) { eventFlags = evFlags; updateFlags();}
  bool HasEventType( DULong evType) const { return (eventFlags & evType) != 0;}
  void AddEventType( DULong evType) { eventFlags |= evType; updateFlags();}
  void RemoveEventType( DULong evType) { eventFlags &= ~evType; updateFlags();}
  void Raise();
  void Lower();
  long textAlignment();
  long widgetAlignment();
  long getDefautAlignment();

  GDLWidget( WidgetIDT p, EnvT* e, 
	     bool map_=true, BaseGDL* vV=NULL, DULong eventFlags_=0);

  virtual ~GDLWidget();

  void CreateWidgetPanel(DLong borderWidth=DEFAULT_BORDER_SIZE, wxBorder=wxNO_BORDER );
  // this is called from the GUI thread on (before) Show()
  // wxTextCtrl and maybe other controls crash when called from the
  // main thread
//  virtual void OnShow();
  // this is called from the main thread on (before) Realize()
  // for latest initialzation (like allocating the plplot stream)
  // calls NOTIFY_REALIZE procedure
  virtual void OnRealize() 
  {
    if( notifyRealize != "") { //insure it is called once only for this.
      std::string note=notifyRealize;
      notifyRealize.clear();
      CallEventPro( note, new DLongGDL( widgetID));
    }
  }
  virtual void OnKill()
  {
    if( killNotify != ""){ //remove kill notify for this widget BEFORE calling it (avoid infinite recursal)
    cerr <<"calling procedure: \""<<killNotify<<"\" for"<<widgetID<<endl;
        std::string RIP=killNotify;
        killNotify.clear();
      CallEventPro( RIP, new DLongGDL( widgetID));
    }
  }

  void SetSizeHints();
  void SetSize(DLong sizex, DLong sizey);
  DLong GetXSize(){return xSize;}
  DLong GetYSize(){return ySize;}
  
  WidgetIDT GetParentID() const { return parentID;}
  
  wxObject* GetWxWidget() const { return wxWidget;}
  void FrameWidget(long style=0);
  void ScrollWidget(DLong x_scroll_size,  DLong y_scroll_size);
  void UnFrameWidget();
  void UnScrollWidget();

  BaseGDL* GetUvalue() const { return uValue;}
  BaseGDL* GetVvalue() const { return vValue;}

  void Realize( bool);

  // for query of children
  virtual bool IsBase() const { return false;} 
  virtual bool IsButton() const { return false;} 
  virtual bool IsDropList() const { return false;} 
  virtual bool IsComboBox() const { return false;} 
  virtual bool IsTab() const { return false;}
  virtual bool IsText() const { return false;} 
  virtual bool IsTree() const { return false;} 
  virtual bool IsSlider() const { return false;}
  virtual bool IsDraw() const { return false;}
  virtual bool IsMenuBar() const { return false;}
  virtual bool IsMenu() const { return false;}
  virtual bool IsMenuItem() const { return false;}

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
  static DLong GetNumberOfWidgets();
  static BaseGDL* GetWidgetsList();
  
  WidgetIDT WidgetID() { return widgetID;}

  wxSizer* GetSizer() { return widgetSizer;}
  wxPanel* GetPanel() { return widgetPanel;}

  bool GetManaged() const { return managed;}
  void SetManaged( bool manval){managed = manval;}
//  void SetSensitive( bool value){sensitive = value;}
  virtual void SetSensitive( bool value);
  virtual void SetFocus();

  bool GetMap() const { return map;}
  void SetMap( bool mapval){ map = mapval;}

  int  GetExclusiveMode() const { return exclusiveMode;}
  void SetExclusiveMode( int exclusiveval){exclusiveMode = exclusiveval;}

  void SetUvalue( BaseGDL *uV){uValue = uV;}
  void SetVvalue( BaseGDL *vV){vValue = vV;}

  const DString& GetWidgetType() const { return widgetType;}
  void SetWidgetType( const DString& wType){widgetType = wType;}

  virtual bool GetButtonSet() const { return 0;} //normally not a button
//   void SetButtonSet(bool onOff){buttonSet = onOff;}

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
  typedef std::deque<WidgetIDT>::reverse_iterator rcIter;
  std::deque<WidgetIDT>                   children;
  
  bool                                    xmanActCom;
  bool                                    modal;
  WidgetIDT                               mbarID;
  // for radio buttons to generate deselect event
  WidgetIDT                               lastRadioSelection;

  wxMutex*                                m_gdlFrameOwnerMutexP;
  DLong ncols;
  DLong nrows;
  bool stretchX;
  bool stretchY;
  long childrenAlignment;

public:
  GDLWidgetBase( WidgetIDT parentID, EnvT* e,
		 bool mapWid,
		 WidgetIDT& mBarIDInOut, bool modal, 
		 DLong col, DLong row,
		 int exclusiveMode, 
		 bool floating,
		 const DString& resource_name, const DString& rname_mbar,
		 const DString& title,
		 const DString& display_name,
		 DLong xpad, DLong ypad,
		 DLong x_scroll_size, DLong y_scroll_size, bool grid_layout, long children_alignment);
  
  ~GDLWidgetBase();

//perhaps a bit too simple!
  void ClearEvents()
  {
  if (!this->GetXmanagerActiveCommand( ))  eventQueue.Purge();
  else readlineEventQueue.Purge(); 
  }
  
//  void OnShow() 
//  {
//    for( cIter c=children.begin(); c!=children.end(); ++c)
//    {
//      GDLWidget* w = GetWidget( *c);
//      if( w != NULL)
//	w->OnShow();
//    }
//    GDLWidget::OnShow();
//  }
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
    for( rcIter rc=children.rbegin(); rc!=children.rend(); ++rc)
    {
      GDLWidget* w = GetWidget( *rc);
      if( w != NULL) w->OnKill();
    }
//    this->OnKill(); //removing this stops otherwise reentrant code leading to crash. But it stinks!.FIXME
  }
  
  void NullWxWidget() { this->wxWidget = NULL;}
  
  WidgetIDT GetLastRadioSelection() const { return lastRadioSelection;}                         
  void SetLastRadioSelection(WidgetIDT lastSel) { lastRadioSelection = lastSel;}                         

  // as this is called in the constructor, no type checking of c can be done
  // hence the AddChild() function should be as simple as that
  void AddChild( WidgetIDT c) { children.push_back( c);}
  void RemoveChild( WidgetIDT  c) { children.erase( find( children.begin(),
							  children.end(), c));}

//  void Realize( bool);
  
  void SelfDestroy(); // sends delete event to itself
  
  void SetXmanagerActiveCommand() 
  { 
//     wxMessageOutputStderr().Printf(_T("SetXmanagerActiveCommand: %d\n",widgetID);
    xmanActCom = true;
  }
  bool GetXmanagerActiveCommand() const 
  { 
//     wxMessageOutputStderr().Printf(_T("GetXmanagerActiveCommand: %d\n",widgetID);
    return xmanActCom;
  }

//   void SetEventPro( DString);
//   const DString& GetEventPro() { return eventHandler;}

  WidgetIDT GetChild( DLong) const;
  DLong NChildren() const;

  bool IsBase() const { return true;} 
  bool IsScrolled() { return scrolled;}
  bool IsStretchable() {return stretchX||stretchY;}
  void setStretchX(bool stretch) {stretchX=stretch;}
  void setStretchY(bool stretch) {stretchY=stretch;}
  long getChildrenAlignment(){return childrenAlignment;}
//  void FitInside();
};



// class GDLWidgetMbar;

// button widget **************************************************
class GDLWidgetButton: public GDLWidget
{
  typedef enum ButtonType_ {
  UNDEFINED=-1, NORMAL=0, RADIO=1, CHECKBOX=2, MENU=3, MBAR=3, ENTRY=4} ButtonType;

  ButtonType buttonType;
  bool addSeparatorAbove;

//  bool buttonState; //defined in base class now.
  
public:
  GDLWidgetButton( WidgetIDT parentID, EnvT* e, const DString& value, bool isMenu, bool hasSeparatorAbove);

  // for WIDGET_CONTROL
  void SetButtonWidget( bool onOff)
  {
    if( wxWidget != NULL)
    {
      switch( buttonType) {
	case RADIO: {	  
	  SetButton( onOff);
	  wxRadioButton* radioButton = static_cast<wxRadioButton*>(wxWidget);
	  radioButton->SetValue(onOff);
	  break;
	}
	case CHECKBOX: {
	  SetButton( onOff);
	  wxCheckBox* checkBox = static_cast<wxCheckBox*>(wxWidget);
	  checkBox->SetValue(onOff);
	  break;
	}
      }
    }
  }
  void SetButtonWidgetLabelText( const DString& value_ );//code in gdlwidget
  void SetButton( bool onOff)
  {
    buttonState = onOff;
  }
  bool GetButtonSet() const
  {
    return buttonState;
  }
  
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
  GDLWidgetDropList( WidgetIDT p, EnvT* e, BaseGDL *value,
		     const DString& title, DLong style);

  bool IsDropList() const { return true;} 

  void SetLastValue( const std::string& v) {  wxMutexLocker lock(m_mutex); lastValue = v;}
  std::string GetLastValue() { wxMutexLocker lock(m_mutex); return lastValue;}
};

// combobox widget **************************************************
class GDLWidgetComboBox: public GDLWidget
{
  std::string lastValue;
  wxMutex m_mutex;
  DString title;
  DLong style;
  
public:
  GDLWidgetComboBox( WidgetIDT p, EnvT* e, BaseGDL *value,
		     const DString& title, DLong style);

//  void OnShow();
  
//   void SetSelectOff();
  bool IsComboBox() const { return true;} 

  void SetLastValue( const std::string& v) {  wxMutexLocker lock(m_mutex); lastValue = v;}
  std::string GetLastValue() { wxMutexLocker lock(m_mutex); return lastValue;}
};

// list widget **************************************************
class GDLWidgetList : public GDLWidget
{
public:
  GDLWidgetList( WidgetIDT p, EnvT* e, BaseGDL *value, DLong style);
//   void SetSelectOff();
};


// text widget **************************************************
class GDLWidgetText: public GDLWidget
{
  std::string lastValue;
  wxMutex m_mutex;
  bool noNewLine;
  bool editable;
  int maxlinelength;
  int nlines;
public:
  GDLWidgetText( WidgetIDT parentID, EnvT* e, DStringGDL* value, bool noNewLine,
		 bool editable);
//  void OnShow();

  bool IsEditable(){return editable;}
  void ChangeText( DStringGDL* value, bool noNewLine=false);
  void InsertText( DStringGDL* value, bool noNewLine=false, bool insertAtEnd=false);
  void SetTextSelection(DLongGDL* pos);
  DLongGDL* GetTextSelection();
  void AppendTextValue( DStringGDL* value, bool noNewLine);
  
  bool IsText() const { return true;} 
  
  void SetLastValue( const std::string& v) { wxMutexError mtxerr=m_mutex.Lock(); lastValue = v; if (mtxerr==wxMUTEX_NO_ERROR) m_mutex.Unlock();}
  std::string GetLastValue() { wxMutexLocker lock(m_mutex); return lastValue;}
};


// label widget **************************************************
class GDLWidgetLabel: public GDLWidget
{
  DString value;
public:
  GDLWidgetLabel( WidgetIDT parentID, EnvT* e, const DString& value_, bool sunken);
//  void OnShow();
 
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
		  DLong x_scroll_size, DLong y_scroll_size, DULong eventFlags);

  ~GDLWidgetDraw();

//   void OnShow();
  void OnRealize();
  void updateFlags();
  bool IsDraw() const { return true;}
};


// menu bar widget **************************************************
class GDLWidgetMBar: public GDLWidget//Base
{
  // disable
  GDLWidgetMBar();
public:
  GDLWidgetMBar( WidgetIDT p): 
  GDLWidget( p, NULL)
  {
    this->wxWidget = new wxMenuBar();
    this->widgetType="MBAR";
  }

  bool IsMenuBar() const { return true;}
};

// tab widget **************************************************
class GDLWidgetTab: public GDLWidget
{
public:
  GDLWidgetTab( WidgetIDT parentID, EnvT* e, DLong location, DLong multiline);

  ~GDLWidgetTab();
  
  bool IsTab() const { return true;}
};


// table widget **************************************************
class GDLWidgetTable: public GDLWidget
{
  DLongGDL* alignment;
  DStringGDL* amPm;
  DByteGDL* backgroundColor;
  DByteGDL* foregroundColor;
  DStringGDL* columnLabels;
  bool columnMajor;
  DLongGDL* columnWidth;
  DStringGDL* daysOfWeek;
  bool disjointSelection;
  bool editable;
  DStringGDL* format;
  DLong groupLeader;
  bool ignoreAccelerators;
  DStringGDL* month;
  bool noColumnHeaders;
  bool noRowHeaders;
  bool resizeableColumns;
  bool resizeableRows;
  DLongGDL* rowHeights;
  DStringGDL* rowLabels;
  bool rowMajor;
  DLong tabMode;
  DLong x_scroll_size;
  DLong y_scroll_size;

public:
  GDLWidgetTable( WidgetIDT p, EnvT* e, 
		  DLongGDL* alignment_,
		  DStringGDL* amPm_,
		  DByteGDL* backgroundColor_,
		  DByteGDL* foregroundColor_,
		  DStringGDL* columnLabels_,
		  bool columnMajor_,
		  DLongGDL* columnWidth_,
		  DStringGDL* daysOfWeek_,
		  bool disjointSelection_,
		  bool editable_,
		  DStringGDL* format_,
		  DLong groupLeader_,
 		  bool ignoreAccelerators_,
		  DStringGDL* month_,
		  bool noColumnHeaders_,
		  bool noRowHeaders_,
		  bool resizeableColumns_,
		  bool resizeableRows_,
		  DLongGDL* rowHeights_,
		  DStringGDL* rowLabels_,
		  bool rowMajor_,
		  DLong tabMode_,
		  BaseGDL* value_,
		  DLong xScrollSize_,
		  DLong yScrollSize_
		);

~GDLWidgetTable()
{
  GDLDelete( alignment );
  GDLDelete( amPm );
  GDLDelete( backgroundColor );
  GDLDelete( foregroundColor );
  GDLDelete( columnLabels );
  GDLDelete( columnWidth );
  GDLDelete( daysOfWeek );
  GDLDelete( format );
  GDLDelete( month );
  GDLDelete( rowHeights );
  GDLDelete( rowLabels );
}
  
//  void OnShow();

  bool IsTable() const { return true;}
};


// tree widget **************************************************
class GDLWidgetTree: public GDLWidget
{
bool alignBottom; 
bool alignCenter; 
bool alignLeft  ; 
bool alignRight ; 
bool alignTop   ; 
BaseGDL* bitmap ; 
bool checkbox   ; 
DLong checked   ; 
DString dragNotify ; 
bool draggable  ; 
bool expanded   ; 
bool folder     ; 
DLong groupLeader; 
DLong index     ; 
bool mask       ; 
bool multiple   ; 
bool noBitmaps  ; 
DLong tabMode   ; 
DString toolTip ; 
DString value;  
  
wxTreeItemId treeItemID;

public:
  GDLWidgetTree( WidgetIDT parentID, EnvT* e, DString value_,
                   bool alignBottom_,
                   bool alignCenter_,
                   bool alignLeft_,
                   bool alignRight_,
                   bool alignTop_,
                   BaseGDL* bitmap_,
                   bool checkbox_,
                   DLong checked_,
                   DString dragNotify_,
                   bool draggable_,
                   bool expanded_,
                   bool folder_,
                   DLong groupLeader_,
                   DLong index_,
                   bool mask_,
                   bool multiple_,
                   bool noBitmaps_,
                   DLong tabMode_,
                   DString toolTip_);
		 
~GDLWidgetTree()
{
  GDLDelete( bitmap );
}

//  void OnShow();

  bool IsTree() const { return true;}
};



// slider widget **************************************************
class GDLWidgetSlider: public GDLWidget
{
  DLong value; 
  DLong minimum; 
  DLong maximum;
  DString title;
public:
  GDLWidgetSlider( WidgetIDT parentID, EnvT* e,
		   DLong value_, DLong minimum_, DLong maximum_,
		   bool vertical,
		   bool suppressValue,
		   DString title
 		);

  ~GDLWidgetSlider();

  void SetValue( DLong v) { value = v;}
  DLong GetValue() const { return value;}
  
  bool IsSlider() const { return true;}
};



// GDL versions of wxWidgets controls =======================================
DECLARE_LOCAL_EVENT_TYPE(wxEVT_SHOW_REQUEST, -1)
DECLARE_LOCAL_EVENT_TYPE(wxEVT_HIDE_REQUEST, -1)

class wxNotebookEvent;
class GDLFrame : public wxFrame
{
//  enum {TIMER_RESIZE = wxID_HIGHEST};
  bool lastShowRequest;
  GDLApp* appOwner;
  GDLWidgetBase* gdlOwner;
  void OnListBoxDo( wxCommandEvent& event, DLong clicks);

  // called from ~GDLWidgetBase
  void NullGDLOwner() { gdlOwner = NULL;}
  friend class GDLWidgetBase;
public:
  // ctor(s)
  GDLFrame(GDLWidgetBase* gdlOwner_, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos=wxDefaultPosition);
  ~GDLFrame();
//  wxTimer *m_timer;
//  wxSize desired_size;

  GDLApp* GetTheApp(){return appOwner;}
  void SetTheApp(GDLApp* myApp){appOwner=myApp;}
  
  // event handlers (these functions should _not_ be virtual)
  void OnIdle( wxIdleEvent& event);
  void OnButton( wxCommandEvent& event);
  void OnRadioButton( wxCommandEvent& event);
  void OnCheckBox( wxCommandEvent& event);
  void OnComboBox( wxCommandEvent& event);
  void OnDropList( wxCommandEvent& event);
  void OnListBox( wxCommandEvent& event);
  void OnListBoxDoubleClicked( wxCommandEvent& event);
  void OnText( wxCommandEvent& event);
  void OnTextEnter( wxCommandEvent& event);
  void OnPageChanged( wxNotebookEvent& event);
  void OnSize( wxSizeEvent& event);
  void OnTimerResize(wxTimerEvent& event);
  void OnScroll( wxScrollEvent& event);
  void OnThumbRelease( wxScrollEvent& event);
  void OnRightClickAsContextEvent( wxMouseEvent &event );
  void OnFocusChange( wxFocusEvent &event);
  
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
  
private:
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
  DULong        eventFlags;
//   wxBitmap*    	memPlotDCBitmap;
//   GDLWXStream*	PStream();
  
public:
  // ctor(s)
  GDLDrawPanel(wxWindow* parent, wxWindowID id, 
	    const wxPoint& pos = wxDefaultPosition, 
	    const wxSize& size = wxDefaultSize,
	    long style = 0, 
            DULong eventFlags_ = 0,
	    const wxString& name = wxPanelNameStr);
 ~GDLDrawPanel();
  
 // void Update();
  void GetEventFlags(DULong eventFlags);
//   void SetPStreamIx( int ix) { pstreamIx = ix;}
  int PStreamIx() { return pstreamIx;}

  void InitStream();
  
  // event handlers (these functions should _not_ be virtual)
  void OnPaint(wxPaintEvent& event);
//  void OnShow(wxShowEvent& event);
  void OnClose(wxCloseEvent& event);
  void OnMouseMove( wxMouseEvent& event);
  void OnMouseDown( wxMouseEvent& event);
  void OnMouseUp( wxMouseEvent& event);
  void OnMouseWheel( wxMouseEvent& event);
  void OnKey( wxKeyEvent& event);
  void OnEnterWindow(wxMouseEvent &event);
  void OnLeaveWindow(wxMouseEvent &event);
  void OnSize(wxSizeEvent &event);
  void SetEventFlags(DULong eventFlag_);
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

  
 private:
  // any class wishing to process wxWidgets events must use this macro
  DECLARE_EVENT_TABLE()
};

#endif

#endif

