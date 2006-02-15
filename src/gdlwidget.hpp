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
  static WidgetIDT                       widgetIx;
  static WidgetListT                 widgetList;
protected:
  // ID for widget (called from widgets constructor)
  static WidgetIDT WidgetID( GDLWidget* w);
  // removes a widget, (called from widgets destructor -> don't delete)
  static void WidgetRemove( WidgetIDT widID);

  // get widget from ID
  static GDLWidget* GetWidget( WidgetIDT widID);
public:
  static void Init(); // GUI intialization upon GDL startup

  
protected:
  wxObject* wxWidget; // deleted only from TLB as the rest is deleted 
                      // automatic

  WidgetIDT    widgetID;  // own index to widgetList
  WidgetIDT    parent;    // parent ID (0 for TLBs)
  BaseGDL* uValue;    // the UVALUE
  bool     sensitive; 
  DLong    xOffset, yOffset, xSize, ySize;

  
public:
  GDLWidget( WidgetIDT p=0, BaseGDL* uV=NULL, bool s=true,
	     DLong xO=0, DLong yO=0, DLong xS=0, DLong yS=0);
  virtual ~GDLWidget();
};


// base widget **************************************************
class GDLWidgetBase: public GDLWidget
{
private:
  typedef std::deque<WidgetIDT>::iterator cIter;
  std::deque<WidgetIDT>                   children;

public:
  GDLWidgetBase( WidgetIDT p=0,               // parent
		 BaseGDL* uV=NULL,        // UVALUE
		 bool s=true,             // SENSITIVE
		 DLong xO=0, DLong yO=0,  // offset 
		 DLong xS=0, DLong yS=0); // size
  virtual ~GDLWidgetBase();

  void AddChild( WidgetIDT c) 
  { children.push_back( c);}
  void RemoveChild( WidgetIDT  c)
  { children.erase( find( children.begin(), children.end(), c));}
};

#endif
