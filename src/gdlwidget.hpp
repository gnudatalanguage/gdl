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

#include <wx/wx.h>

#include <map>

#include "typedefs.hpp"

class GDLWidget;

// global widget list type
typedef DLong                       WidgetIDT;
typedef std::map<WidgetIDT, GDLWidget*> WidgetListT;

// main App class
class GDLApp: public wxApp
{
  virtual bool OnInit();
};

// GUI base class **********************************
class GDLWidget
{ 
  // static part is used for the abstraction
  // all widgets are refered to as IDs
private:
  // the global widget list and the actual index for new widgets
  // shared among all widgets
  static WidgetIDT                   widgetIx;
  static WidgetListT                 widgetList;
protected:
  // removes a widget, (called from widgets destructor -> don't delete)
  static void WidgetRemove( WidgetIDT widID);

public:
  // ID for widget (called from widgets constructor)
  static WidgetIDT NewWidget( GDLWidget* w);
  // get widget from ID
  static GDLWidget* GetWidget( WidgetIDT widID);

  static void Init(); // GUI intialization upon GDL startup

  
protected:
  wxObject* wxWidget; // deleted only from TLB as the rest is deleted 
                      // automatic

  WidgetIDT    widgetID;  // own index to widgetList
  WidgetIDT    parent;    // parent ID (0 for TLBs)
  BaseGDL*     uValue;    // the UVALUE
  bool         sensitive; 
  DLong        xOffset, yOffset, xSize, ySize;

public:
  GDLWidget( WidgetIDT p=0, BaseGDL* uV=NULL, bool s=true,
	     DLong xO=0, DLong yO=0, DLong xS=0, DLong yS=0);
  virtual ~GDLWidget();

  wxObject* WxWidget() { return wxWidget;}
  
  virtual void Realize() {} 

  WidgetIDT WidgetID() { return widgetID;}
};


// base widget **************************************************
class GDLWidgetMbar;
class GDLWidgetButton;

class GDLWidgetBase: public GDLWidget
{
protected:
  typedef std::deque<WidgetIDT>::iterator cIter;
  std::deque<WidgetIDT>                   children;
  
  bool                                    modal;
  WidgetIDT                               mbarID;

public:
  GDLWidgetBase( WidgetIDT parentID, 
		 BaseGDL* uvalue, DString uname,
		 bool sensitive, bool mapWid,
		 WidgetIDT mBarID, bool modal, 
		 WidgetIDT group_leader,
		 DLong col, DLong row,
		 long events,
		 int exclusiveMode, 
		 bool floating,
		 DString event_func, DString event_pro,
		 DString pro_set_value, DString func_get_value,
		 DString notify_realize, DString kill_notify,
		 DString resource_name, DString rname_mbar,
		 DString title,
		 DLong frame, DLong units,
		 DString display_name,
		 DLong xpad, DLong ypad,
		 DLong xoffset, DLong yoffset,
		 DLong xsize, DLong ysize,
		 DLong scr_xsize, DLong scr_ysize,
		 DLong x_scroll_size, DLong y_scroll_size);

  GDLWidgetBase( WidgetIDT p=0,           // parent
		 BaseGDL* uV=NULL,        // UVALUE
		 bool s=true,             // SENSITIVE
		 DLong xO=0, DLong yO=0,  // offset 
		 DLong xS=0, DLong yS=0); // size
  virtual ~GDLWidgetBase();

  void AddChild( WidgetIDT c) 
  { children.push_back( c);}
  void RemoveChild( WidgetIDT  c)
  { children.erase( find( children.begin(), children.end(), c));}

  void Realize();
};

class GDLWidgetMBar: public GDLWidgetBase
{
};


#endif
