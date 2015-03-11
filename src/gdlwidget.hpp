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
#include <wx/grid.h>
#ifdef HAVE_WXWIDGETS_PROPERTYGRID
#include <wx/propgrid/propgrid.h>
#endif
#include <wx/defs.h>//for timer.
#include <wx/gdicmn.h> 

#include <deque>
#include <map>

#include "typedefs.hpp"
#include "str.hpp"
#include "datatypes.hpp"
#include "widget.hpp"
#include "plotting.hpp"

#define SCROLL_WIDTH 20
#define DEFAULT_BORDER_SIZE 3

typedef DLong WidgetIDT;
static string widgetNameList[14]={"BASE","BUTTON","SLIDER","TEXT","DRAW","LABEL","LIST","MBAR","DROPLIST","TABLE","TAB","TREE","COMBOBOX","PROPERTYSHEET"};
static int    widgetTypeList[14]={0,1,2,3,4,5,6,7,8,9,10,11,12,13};

class DStructGDL;

class GDLApp;
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
//  static void RefreshWidgets();
  void RefreshWidget();
  
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
  bool         buttonState; //only for buttons
  int          exclusiveMode;
  DLong        xOffset, yOffset, xSize, ySize, scrXSize, scrYSize;
  wxSizer*     mySizer; //optional sizer forcing sizes & placement of widget.
  wxSizer*     topWidgetSizer; //the frame sizer (contains all widgets)
  wxSizer*     widgetSizer; // the sizer which governs the placement of the widget in its Panel
  wxPanel*     widgetPanel; // the Panel in which the widget is placed
  wxSizer*     scrollSizer; // the sizer for the (optional) Scroll Panel
  wxScrolledWindow*     scrollPanel; // Panel with scrollBars in which the widget may be shown
  wxSizer*     frameSizer;  // the sizer of the (optional) "frame" (its not a wxFrame its a StaticBox) drawn around a widget
  wxPanel*     framePanel;  // the corresponding panel
  DInt         widgetType;
  DString      widgetName;
  WidgetIDT    groupLeader;
  wxSize       units;
  DLong        frame;
  DString      font;
  bool         valid; //if not, is in the process of being destroyed (prevent reentrance).
  bool         updating; //widget is modified by program (avoid sending events)
  long  alignment; //alignment of the widget
  long widgetStyle; //style (alignment code + other specific codes used as option to widgetsizer) 
  vector<WidgetIDT> followers; //all the widgets that use me as group_leader

  
private:  
  DULong eventFlags; // event types widget should reply to

  DString      uName;
  DString      proValue;
  DString      funcValue;
  DString      eventPro; // event handler PRO
  DString      eventFun; // event handler FUN
  DString      notifyRealize;
  DString      killNotify;
  
  void GetCommonKeywords( EnvT* e);

  
public:
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
    , EV_KILL = 32768
    } EventTypeFlags;
 
   typedef enum WidgetTypes_
    { WIDGET_BASE = 0
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
    } WidgetTypes;
 
  DULong GetEventFlags()  const { return eventFlags;}
  bool SetEventFlags( DULong evFlags) { eventFlags = evFlags;}
  bool HasEventType( DULong evType) const { return (eventFlags & evType) != 0;}
  void AddEventType( DULong evType) { eventFlags |= evType;}
  void RemoveEventType( DULong evType) { eventFlags &= ~evType;}
  void Raise();
  void Lower();
  long buttonTextAlignment();
  long textAlignment();
  long widgetAlignment();
  long getDefautAlignment();
  void widgetUpdate(bool update);

  GDLWidget( WidgetIDT p, EnvT* e, BaseGDL* vV=NULL, DULong eventFlags_=0);

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
        std::string RIP=killNotify;
        killNotify.clear();
      CallEventPro( RIP, new DLongGDL( widgetID));
    }
  }
  
  virtual void AddToFollowers(WidgetIDT him)
  {
    followers.insert( followers.end( ), him );
  }
  
  void SetSizeHints();
  void SetSize(DLong sizex, DLong sizey);
  DLong GetXSize(){return xSize;}
  DLong GetYSize(){return ySize;}
  DLong GetXPos(){return static_cast<wxWindow*>(wxWidget)->GetPosition().x;}
  DLong GetYPos(){return static_cast<wxWindow*>(wxWidget)->GetPosition().y;}
  bool IsValid(){return valid;}
  void SetUnValid(){valid=FALSE;}
  void SetValid(){valid=TRUE;}
  
  WidgetIDT GetParentID() const { return parentID;}
  
  wxObject* GetWxWidget() const { return wxWidget;}
  void FrameWidget(long style=0);
  void ScrollWidget(DLong x_scroll_size,  DLong y_scroll_size);
  void UnFrameWidget();
  void UnScrollWidget();

  BaseGDL* GetUvalue() const { return uValue;}
  BaseGDL* GetVvalue() const { return vValue;}

  void Realize( bool);
  void SendWidgetTimerEvent(DDouble secs);

  // for query of children
  virtual bool IsContainer() const { return false;} 
  virtual bool IsBase() const { return false;} 
  virtual bool IsButton() const { return false;} 
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
  virtual bool IsMenuBar() const { return false;}
  virtual bool IsPropertySheet() const { return false;}

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
  
  bool IsUpdating(){return updating;}
  void ClearUpdating(){updating=FALSE;}
  void SetUpdating(){updating=TRUE;}
  
  wxSize computeWidgetSize(); 
  BaseGDL * getSystemColours();
};

class GDLWidgetContainer: public GDLWidget
{
  bool map;
protected:
  typedef std::deque<WidgetIDT>::iterator cIter;
  typedef std::deque<WidgetIDT>::reverse_iterator rcIter;
  std::deque<WidgetIDT>                   children;
public:
  GDLWidgetContainer( WidgetIDT parentID, EnvT* e, bool map=TRUE);

//  ~GDLWidgetContainer(){}
  
  bool IsContainer() const { return true;}
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
   // as this is called in the constructor, no type checking of c can be done
  // hence the AddChild() function should be as simple as that
  void AddChild( WidgetIDT c) { children.push_back( c);}
  void RemoveChild( WidgetIDT  c) { children.erase( find( children.begin(),
							  children.end(), c));}
  DLong NChildren() const
  {
    return children.size( );
  }
  WidgetIDT GetChild( DLong childIx) const
  {
    assert( childIx >= 0 );
    assert( childIx < children.size( ) );
    return children[childIx];
  }

  bool GetMap() const { return map;}
  void SetMap( bool mapval){ map = mapval;}
};

// base widget **************************************************
class GDLWidgetBase: public GDLWidgetContainer
{
  bool                                    xmanActCom;
  bool                                    modal;
  WidgetIDT                               mbarID;
  // for radio buttons to generate deselect event
  WidgetIDT                               lastRadioSelection;

  DLong ncols;
  DLong nrows;
  bool stretchX;
  bool stretchY;
  long childrenAlignment;
  long space;

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
		 DLong x_scroll_size, DLong y_scroll_size, bool grid_layout, long children_alignment, long space);
  
  ~GDLWidgetBase();

  void ClearEvents()
  {
  if (!this->GetXmanagerActiveCommand( ))  eventQueue.Purge();
  else readlineEventQueue.Purge(); 
  }
  
  void NullWxWidget() { this->wxWidget = NULL;}
  
  WidgetIDT GetLastRadioSelection() const { return lastRadioSelection;}                         
  void SetLastRadioSelection(WidgetIDT lastSel) { lastRadioSelection = lastSel;}                         

  void SelfDestroy(); // sends delete event to itself
  
  void SetXmanagerActiveCommand() 
  { 
    xmanActCom = true;
  }
  bool GetXmanagerActiveCommand() const 
  { 
    return xmanActCom;
  }

  bool IsBase() const { return true;} 
  bool IsContainer() const { return true;} 
  bool IsScrolled() { return scrolled;}
  bool IsStretchable() {return stretchX||stretchY;}
  void setStretchX(bool stretch) {stretchX=stretch;}
  void setStretchY(bool stretch) {stretchY=stretch;}
  long getChildrenAlignment(){return childrenAlignment;}
  long getSpace(){return space;}
  void mapBase(bool val);
};

class GDLWidgetButton: public GDLWidget
{
  typedef enum ButtonType_ {
  UNDEFINED=-1, NORMAL=0, RADIO=1, CHECKBOX=2, MENU=3, MBAR=3, ENTRY=4, BITMAP=5} ButtonType;

  ButtonType buttonType;
  bool addSeparatorAbove;
  wxBitmap* buttonBitmap;

//  bool buttonState; //defined in base class now.
  
public:
  GDLWidgetButton( WidgetIDT parentID, EnvT* e, const DString& value, bool isMenu, bool hasSeparatorAbove=FALSE, wxBitmap* bitmap=NULL, DStringGDL* buttonTooltip=NULL);
  ~GDLWidgetButton();
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
  void SetButtonWidgetBitmap( wxBitmap* bitmap_ );//code in gdlwidget
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
  DString title;
  DLong style;
  
public:
  GDLWidgetDropList( WidgetIDT p, EnvT* e, BaseGDL *value,
		     const DString& title, DLong style);
  ~GDLWidgetDropList();
  bool IsDropList() const { return true;} 

  void SetLastValue( const std::string& v) { lastValue = v;}
  std::string GetLastValue() { return lastValue;}
  
  void SetValue(BaseGDL *value);
  void SelectEntry(DLong entry_number);
};

// combobox widget **************************************************
class GDLWidgetComboBox: public GDLWidget
{
  std::string lastValue;
  DString title;
  DLong style;
  
public:
  GDLWidgetComboBox( WidgetIDT p, EnvT* e, BaseGDL *value,
		     const DString& title, DLong style);
 ~GDLWidgetComboBox();
//  void OnShow();
  
//   void SetSelectOff();
  bool IsComboBox() const { return true;} 

  void SetLastValue( const std::string& v) { lastValue = v;}
  std::string GetLastValue() { return lastValue;}
  void SetValue(BaseGDL *value);
  void SelectEntry(DLong entry_number);
  void AddItem(DString value, DLong pos);
  void DeleteItem(DLong pos);
};

// list widget **************************************************
class GDLWidgetList : public GDLWidget
{
public:
  GDLWidgetList( WidgetIDT p, EnvT* e, BaseGDL *value, DLong style);
  ~GDLWidgetList();
  bool IsList() const { return true;} 
  void SetValue(BaseGDL *value);
  void SelectEntry(DLong entry_number);
};


// text widget **************************************************
class GDLWidgetText: public GDLWidget
{
  std::string lastValue;
  bool noNewLine;
  bool editable;
  int maxlinelength;
  int nlines;
public:
  GDLWidgetText( WidgetIDT parentID, EnvT* e, DStringGDL* value, bool noNewLine,
		 bool editable);
  ~GDLWidgetText();
  
  bool IsEditable(){return editable;}
  void ChangeText( DStringGDL* value, bool noNewLine=false);
  void InsertText( DStringGDL* value, bool noNewLine=false, bool insertAtEnd=false);
  void SetTextSelection(DLongGDL* pos);
  DLongGDL* GetTextSelection();
  DStringGDL* GetSelectedText();
  void AppendTextValue( DStringGDL* value, bool noNewLine);
  
  bool IsText() const { return true;} 
  
  void SetLastValue( const std::string& v) { lastValue = v;}
  std::string GetLastValue() { return lastValue;}
  wxSize computeWidgetSize();
};


// label widget **************************************************
class GDLWidgetLabel: public GDLWidget
{
  DString value;
public:
  GDLWidgetLabel( WidgetIDT parentID, EnvT* e, const DString& value_, bool sunken);
 ~GDLWidgetLabel();
  void SetLabelValue( const DString& value_);
  bool IsLabel() const { return true;} 
};


// draw widget **************************************************
class GDLWidgetDraw: public GDLWidget
{
  int pstreamIx;
  DLong x_scroll_size;
  DLong y_scroll_size;
public:
  GDLWidgetDraw( WidgetIDT parentID, EnvT* e,
		  DLong x_scroll_size, DLong y_scroll_size, bool app_scroll, DULong eventFlags, DStringGDL* drawToolTip=NULL);

  ~GDLWidgetDraw();

  void OnRealize();
  bool IsDraw() const { return true;}
};


// menu bar widget **************************************************
class GDLWidgetMBar: public GDLWidgetContainer//Base
{
  // disable
  GDLWidgetMBar();
public:
  GDLWidgetMBar( WidgetIDT p): 
  GDLWidgetContainer( p, NULL)
  {
    this->wxWidget = new wxMenuBar();
    this->SetWidgetType(WIDGET_MBAR);
  }

  bool IsMenuBar() const { return true;}
};

// tab widget **************************************************
class GDLWidgetTab: public GDLWidgetContainer
{
public:
    GDLWidgetTab( WidgetIDT parentID, EnvT* e, DLong location, DLong multiline);

  ~GDLWidgetTab();
  
  bool IsTab() const { return true;}
  bool IsContainer() const { return true;}
};


// table widget **************************************************
class GDLWidgetTable: public GDLWidget
{
  DByteGDL* alignment;
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
  bool IsTable() const { return true;}
  void SetDOW(DStringGDL* val){GDLDelete(daysOfWeek); daysOfWeek=val->Dup();}
  void SetAmPm(DStringGDL* val){GDLDelete(amPm); amPm=val->Dup();};
  void SetMonth(DStringGDL* val){GDLDelete(month); month=val->Dup();};

  DLongGDL* GetSelection();
  
  void SetAlignment(DByteGDL* val){GDLDelete(alignment); alignment=val->Dup();};
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
  DLongGDL* GetColumnWidth(DLongGDL* selection=NULL);
  
  void SetRowHeights(DLongGDL* val){GDLDelete(rowHeights); rowHeights=val->Dup();};
  void DoRowHeights();
  void DoRowHeights(DLongGDL* selection);
  DLongGDL* GetRowHeight(DLongGDL* selection=NULL);

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
                   DLong index_,
                   bool mask_,
                   bool multiple_,
                   bool noBitmaps_,
                   DLong tabMode_,
                   DString toolTip_);
		 
~GDLWidgetTree();

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
  void ControlSetValue ( DLong v );
  DLong GetValue() const { return value;}
  
  bool IsSlider() const { return true;}
};



// GDL versions of wxWidgets controls =======================================
DECLARE_LOCAL_EVENT_TYPE(wxEVT_SHOW_REQUEST, -1)
DECLARE_LOCAL_EVENT_TYPE(wxEVT_HIDE_REQUEST, -1)

class wxNotebookEvent;
class wxGridEvent;
class wxGridSizeEvent;
class wxGridRangeSelectEvent;

class GDLGrid : public wxGrid
{
  wxWindowID GDLWidgetTableID;
public:
  GDLGrid(wxWindow* container, wxWindowID id, 
	    const wxPoint& pos = wxDefaultPosition, 
	    const wxSize& size = wxDefaultSize,
	    long style = 0, 
	    const wxString& name = wxPanelNameStr);
  ~GDLGrid();
  void OnTableCellSelection(wxGridEvent & event);
  void OnTableRangeSelection(wxGridRangeSelectEvent & event);
  void OnTableColResizing(wxGridSizeEvent & event);
  void OnTableRowResizing(wxGridSizeEvent & event); 
  void OnText( wxCommandEvent& event);
  void OnTextEnter( wxCommandEvent& event);
  
  bool IsSomethingSelected(){
      wxGridCellCoordsArray cellSelection=this->GetSelectedCells();
      if ( cellSelection.Count() > 0 ) return TRUE;
      wxGridCellCoordsArray selectionBR=this->GetSelectionBlockBottomRight();
      if ( selectionBR.Count() > 0 ) return TRUE;
      wxArrayInt selectionRow=this->GetSelectedRows();
      if ( selectionRow.GetCount() > 0 ) return TRUE;
      wxArrayInt selectionCol=this->GetSelectedCols();
      if ( selectionCol.GetCount() > 0 ) return TRUE;
      return FALSE;
  }
  vector<wxPoint> GetSelectedDisjointCellsList(){
      vector<wxPoint> list;
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

protected:
  DECLARE_EVENT_TABLE()
};
#ifdef HAVE_WXWIDGETS_PROPERTYGRID

class GDLWidgetPropertySheet : public GDLWidget
{
public:
  GDLWidgetPropertySheet( WidgetIDT parentID, EnvT* e);

  ~GDLWidgetPropertySheet();

  bool IsPropertySheet() const { return true;}
};
#endif

class GDLFrame : public wxFrame
{
  enum {WINDOW_TIMER = wxID_HIGHEST, RESIZE_TIMER};
  bool lastShowRequest;
  wxSize newSize;
  GDLApp* appOwner;
  GDLWidgetBase* gdlOwner;
  wxTimer * m_resizeTimer;
  wxTimer * m_windowTimer;
  void OnListBoxDo( wxCommandEvent& event, DLong clicks);

  // called from ~GDLWidgetBase
  void NullGDLOwner() { gdlOwner = NULL;}
  friend class GDLWidgetBase;
public:
  // ctor(s)
  GDLFrame(GDLWidgetBase* gdlOwner_, wxWindowID id, const wxString& title, const wxPoint& pos=wxDefaultPosition);
  ~GDLFrame();

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
//  void OnSizeWithTimer( wxSizeEvent& event); //not yet ready
//  void OnTimerResize(wxTimerEvent& event);
  void OnScroll( wxScrollEvent& event);
  void OnThumbRelease( wxScrollEvent& event);
  void OnRightClickAsContextEvent( wxMouseEvent &event );
  void OnFocusChange( wxFocusEvent &event);
  void OnIconize( wxIconizeEvent & event);
  void OnMove( wxMoveEvent & event);
  void OnCloseFrame( wxCloseEvent & event);
  void OnWidgetTimer( wxTimerEvent & event);

  bool LastShowRequest() const { return lastShowRequest;}
  
  void SendWidgetTimerEvent(DDouble secs, WidgetIDT winId)
  {
      WidgetIDT* id=new WidgetIDT(winId);
      int millisecs=secs*1000;
      this->GetEventHandler()->SetClientData(id);
      m_windowTimer->SetOwner(this->GetEventHandler(),WINDOW_TIMER);
      m_windowTimer->Start(millisecs, wxTIMER_ONE_SHOT);
  }
  
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
  
protected:
  DECLARE_EVENT_TABLE()
};


class GDLWXStream;

class GDLDrawPanel : public wxPanel
{
  enum {WINDOW_TIMER = wxID_HIGHEST, RESIZE_TIMER};
  int		pstreamIx;
  GDLWXStream*	pstreamP;

  wxSize 	drawSize;

  wxDC*  	m_dc;
  wxWindowID GDLWidgetDrawID;
  wxSize   newSize;
  wxTimer * m_resizeTimer;
  
public:
  // ctor(s)
  GDLDrawPanel(wxWindow* parent, wxWindowID id, 
	    const wxPoint& pos = wxDefaultPosition, 
	    const wxSize& size = wxDefaultSize,
	    long style = 0, 
	    const wxString& name = wxPanelNameStr);
 ~GDLDrawPanel();
  
  void Update()
  {
      this->Refresh();
  }
  
//example for multithreading?
//void Update()
//{
//  //   cout << "in GDLDrawPanel::Update()" << endl;
//  SendPaintEvent( );
//  //   wxClientDC dc( this);
//  //   dc.SetDeviceClippingRegion( GetUpdateRegion() );
//  //   GUIMutexLockerT gdlMutexGuiEnterLeave;
//  //   dc.Blit( 0, 0, drawSize.x, drawSize.y, m_dc, 0, 0 );
//  //   wxPanel::Update();
//  //   gdlMutexGuiEnterLeave.Leave();
//}
  
//  void GetEventFlags(DULong eventFlags);
  int PStreamIx() { return pstreamIx;}

  void InitStream();
  
  // event handlers (these functions should _not_ be virtual)
  void OnPaint(wxPaintEvent& event);
  void OnClose(wxCloseEvent& event);
  void OnMouseMove( wxMouseEvent& event);
  void OnMouseDown( wxMouseEvent& event);
  void OnMouseUp( wxMouseEvent& event);
  void OnMouseWheel( wxMouseEvent& event);
  void OnKey( wxKeyEvent& event);
  void OnEnterWindow(wxMouseEvent &event);
  void OnLeaveWindow(wxMouseEvent &event);
  void OnSize(wxSizeEvent &event);
//  void OnSizeWithTimer(wxSizeEvent &event); //not yet ready
//  void OnTimerResize( wxTimerEvent& event);
  void SendPaintEvent()
  {
    wxPaintEvent* event;
    event = new wxPaintEvent( GetId());
    event->SetEventObject( this );
    // only for wWidgets > 2.9 (takes ownership of event)
//     this->QueueEvent( event);
    this->AddPendingEvent( *event); // copies event
    delete event;
  }

  
 protected:
  DECLARE_EVENT_TABLE()
};

#endif

#endif

