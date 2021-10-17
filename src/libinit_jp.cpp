/***************************************************************************
          libinit_jp.cpp  -  initialization of GDL widget library routines
                             -------------------
    begin                : 2015
    copyright            : (C) 2015 by Jeongbin Park, G. Duvert
    email                : pjb7687@gmail.com
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
#include "dialog.hpp"


#include "widget.hpp"

using namespace std;

void LibInit_jp()
{
#ifdef HAVE_LIBWXWIDGETS
  const char KLISTEND[] = "";

  // WIDGET_ functions
  new DLibPro(lib::widget_displaycontextmenu, string("WIDGET_DISPLAYCONTEXTMENU"), 4);

#define WIDGET_COMMON_KEYWORDS "EVENT_FUNC", "EVENT_PRO", "FUNC_GET_VALUE", "KILL_NOTIFY" \
    , "NOTIFY_REALIZE" , "PRO_SET_VALUE" , "NO_COPY" , "GROUP_LEADER"\
    , "SCR_XSIZE" , "SCR_YSIZE" , "SCROLL", "SENSITIVE" \
    , "UNAME" , "UNITS" , "UVALUE" , "XOFFSET" \
    , "XSIZE" , "YOFFSET" , "YSIZE" , "FRAME" \
    , "ALIGN_LEFT", "ALIGN_RIGHT", "ALIGN_CENTER", "ALIGN_BOTTOM", "ALIGN_TOP", "FONT","RESOURCE_NAME"

  //ACTIVEX
  //BASE  
  const string widget_baseKey[] = {WIDGET_COMMON_KEYWORDS, "MBAR", "APP_MBAR", "MODAL", "COLUMN", "ROW",
    "EXCLUSIVE", "NONEXCLUSIVE", "FLOATING", "MAP", "TITLE", "XPAD", "X_SCROLL_SIZE", "YPAD", "Y_SCROLL_SIZE", "DISPLAY_NAME",
    "RNAME_MBAR", "CONTEXT_EVENTS", "KBRD_FOCUS_EVENTS", "TLB_ICONIFY_EVENTS", "TLB_KILL_REQUEST_EVENTS",
    "TLB_MOVE_EVENTS", "TLB_SIZE_EVENTS", "TRACKING_EVENTS", "GRID_LAYOUT",
    "BASE_ALIGN_CENTER", "BASE_ALIGN_LEFT", "BASE_ALIGN_RIGHT", "BASE_ALIGN_BOTTOM", "BASE_ALIGN_TOP", "SPACE", "CONTEXT_MENU",
    "TOOLBAR", "TLB_FRAME_ATTR", "TLB_RESIZE_NODRAW", "BITMAP", 
  //unsupported but warning is a pain.
    "TAB_MODE",  
    KLISTEND};
  new DLibFunRetNew(lib::widget_base, string("WIDGET_BASE"), 1, widget_baseKey);
  //BUTTON
  const string widget_buttonKey[] = {WIDGET_COMMON_KEYWORDS, "MENU", "VALUE", "HELP", "SEPARATOR", "INPUT_FOCUS", "BITMAP", "IMAGE", "TOOLTIP",
    "TRACKING_EVENTS", "DYNAMIC_RESIZE", "X_BITMAP_EXTRA", "FLAT", "NO_RELEASE", "CHECKED_MENU","PUSHBUTTON_EVENTS","RESOURCE_NAME",
    "ACCELERATOR","TAB_MODE", 
    KLISTEND};
  new DLibFunRetNew(lib::widget_button, string("WIDGET_BUTTON"), 1, widget_buttonKey);
  //COMBOBOX
  const string widget_comboboxKey[] = {WIDGET_COMMON_KEYWORDS, "EDITABLE", "VALUE", "TRACKING_EVENTS", "DYNAMIC_RESIZE", "FLAT",
  //unsupported but warning is a pain.
    "IGNORE_ACCELERATORS",
    "TAB_MODE",    
    KLISTEND};
  new DLibFunRetNew(lib::widget_combobox, string("WIDGET_COMBOBOX"), 1, widget_comboboxKey); 
  //CONTROL
  const string widget_ControlKey[] = {"REALIZE", "MANAGED", "EVENT_FUNC", "EVENT_PRO",
    "XMANAGER_ACTIVE_COMMAND", "DESTROY",
    "GET_UVALUE", "SET_UVALUE", "SET_VALUE",
    "MAP", "FUNC_GET_VALUE", "PRO_SET_VALUE",
    "SET_UNAME", "NO_COPY", "SET_BUTTON",
    "SET_DROPLIST_SELECT", "SET_LIST_SELECT", "SET_COMBOBOX_SELECT",
    "SENSITIVE", "XOFFSET", "YOFFSET",
    "GET_VALUE", "NO_NEWLINE", "TLB_GET_SIZE",
    "HOURGLASS", "TLB_SET_TITLE", "INPUT_FOCUS",
    "CLEAR_EVENTS", "TLB_SET_XOFFSET", "TLB_SET_YOFFSET", "TLB_GET_OFFSET",
    "DRAW_BUTTON_EVENTS", "DRAW_EXPOSE_EVENTS", "DRAW_KEYBOARD_EVENTS",
    "DRAW_MOTION_EVENTS", "DRAW_WHEEL_EVENTS", "TRACKING_EVENTS", "DRAW_VIEWPORT_EVENTS",
    "SET_DROP_EVENTS", "KILL_NOTIFY", "SHOW", "APPEND", "USE_TEXT_SELECT", "SET_TEXT_SELECT",
    "XSIZE", "YSIZE", "SCR_XSIZE", "SCR_YSIZE", "DRAW_XSIZE", "DRAW_YSIZE", "FRAME", "SCROLL", //not IDL!
    "BITMAP", "IMAGE", "TIMER", "NOTIFY_REALIZE", "ALL_TABLE_EVENTS", "ALL_TEXT_EVENTS",
    "TLB_KILL_REQUEST_EVENTS", "TLB_MOVE_EVENTS", "TLB_SIZE_EVENTS", "TLB_ICONIFY_EVENTS",
    "TLB_RESIZE_NODRAW", //This keyword will be silently ignored, as the wxWidgets redrawing is always done as if "TLB_RESIZE_NODRAW" was set.
    "CONTEXT_EVENTS", "KBRD_FOCUS_EVENTS", "ALIGNMENT",
    "BACKGROUND_COLOR", "FOREGROUND_COLOR", "ROW_LABELS", "COLUMN_LABELS",
    "COLUMN_WIDTHS", "ROW_HEIGHTS", "TABLE_DISJOINT_SELECTION", "USE_TABLE_SELECT",
    "INSERT_COLUMNS", "INSERT_ROWS", "DELETE_COLUMNS", "DELETE_ROWS",
    "AM_PM", "DAYS_OF_WEEK", "MONTHS", "SET_TABLE_SELECT", "SET_TABLE_VIEW",
    "UPDATE", "FORMAT", "EDIT_CELL", "TABLE_XSIZE", "TABLE_YSIZE", "SEND_EVENT", "BAD_ID",
    "GROUP_LEADER", "COMBOBOX_ADDITEM", "COMBOBOX_DELETEITEM", "COMBOBOX_INDEX",
    "GET_DRAW_VIEW", "SET_DRAW_VIEW","SET_TAB_CURRENT", "UNITS", "DYNAMIC_RESIZE", "SET_SLIDER_MIN", "SET_SLIDER_MAX",
    "X_BITMAP_EXTRA", "DEFAULT_FONT", "FONT", "EDITABLE", "BASE_SET_TITLE", "SET_TREE_EXPANDED", 
    "SET_TREE_SELECT","SET_TREE_INDEX","SET_DRAG_NOTIFY","SET_DRAGGABLE","SET_TREE_CHECKED",
    "SET_TREE_BITMAP","SET_MASK","SET_TREE_VISIBLE",
    //unsupported but warning is a pain.
    "TAB_MODE", 
    KLISTEND};
  const string widget_WarnControlKey[] = { "DELAY_DESTROY",
    "PUSHBUTTON_EVENTS", "TABLE_BLANK", "SET_TAB_MULTILINE", "ICONIFY"
    , "CANCEL_BUTTON" //obsoleted in 6.2
    , "DEFAULT_BUTTON" //obsoleted in 6.2
    , KLISTEND}; //LIST NOT CLOSE!!!  
  //IMPORTANT :   
  new DLibPro(lib::widget_control, string("WIDGET_CONTROL"), 1,
    widget_ControlKey, widget_WarnControlKey);
  //DISPLAYCONTEXTMENU
  //DRAW
  const string widget_drawKey[] = {WIDGET_COMMON_KEYWORDS, "X_SCROLL_SIZE", "Y_SCROLL_SIZE"
    , "MOTION_EVENTS"
    , "BUTTON_EVENTS"
    , "DROP_EVENTS"
    , "EXPOSE_EVENTS"
    , "KEYBOARD_EVENTS"
    , "TRACKING_EVENTS"
    , "WHEEL_EVENTS"
    , "VIEWPORT_EVENTS"
    , "APP_SCROLL" //not taken into account, but not useful, too.
    , "TOOLTIP"
 // unsupported yet but warning about it may be a pain
    , "RETAIN" //not taken into account, but not useful, too.
    , "CLASSNAME"
    , "COLOR_MODEL"
    , "COLORS"
    , "GRAPHICS_LEVEL"
    , "IGNORE_ACCELERATORS"
    , "RENDERER"
    , KLISTEND};
  const string widget_drawWarnKey[] = {
    "DRAG_NOTIFY" //should be implemented 
    , KLISTEND
  };
  new DLibFunRetNew(lib::widget_draw, string("WIDGET_DRAW"), 1, widget_drawKey, widget_drawWarnKey);
  //DROPLIST 
  const string widget_droplistKey[] = {WIDGET_COMMON_KEYWORDS, "TITLE", "VALUE", "TRACKING_EVENTS", "DYNAMIC_RESIZE", "FLAT",
    //unsupported but warning is a pain.
    "TAB_MODE", 
    KLISTEND};
  new DLibFunRetNew(lib::widget_droplist, string("WIDGET_DROPLIST"), 1, widget_droplistKey); //,widget_droplistWarnKey);
  //EVENT  
  const string widget_eventKey[] = {"XMANAGER_BLOCK", "DESTROY", "SAVE_HOURGLASS", "NOWAIT", "BAD_ID", KLISTEND};
  const string widget_eventWarnKey[] = {"YIELD_TO_TTY", "EVENT_BREAK", "BREAK_ON_EXPOSE", KLISTEND};
  new DLibFunRetNew(lib::widget_event, string("WIDGET_EVENT"), 1, widget_eventKey, widget_eventWarnKey);
  //INFO
  const string widget_infoKey[] = {"DEBUG", "ACTIVE", "VALID_ID", "MODAL", "MANAGED",
    "XMANAGER_BLOCK", //only GDL, used in xmanager.pro , may even not be useful now.
    "CHILD", "VERSION", "GEOMETRY", "UNAME", "DISPLAY",
    "FONTNAME", "STRING_SIZE",
    "BUTTON_SET", "PARENT", "TEXT_SELECT", "FIND_BY_UNAME", "TYPE", "NAME",
    "TABLE_DISJOINT_SELECTION", "TABLE_SELECT", "COLUMN_WIDTHS", "ROW_HEIGHTS", "USE_TABLE_SELECT", "SYSTEM_COLORS",
    "TREE_ROOT", "TREE_SELECT", "TREE_DRAG_SELECT", "TREE_EXPANDED", "TREE_FOLDER", "TREE_INDEX", "TREE_BITMAP",
  "DROP_EVENTS", "DRAGGABLE", "DRAG_NOTIFY", "MASK","TREE_CHECKBOX", "TREE_CHECKED",
    "LIST_SELECT", "DROPLIST_SELECT", "COMBOBOX_GETTEXT",
    "TAB_NUMBER", "TAB_MULTILINE", "TAB_CURRENT",
    "TLB_KILL_REQUEST_EVENTS", "TLB_MOVE_EVENTS", "TLB_SIZE_EVENTS", "TLB_ICONIFY_EVENTS",
    "EVENT_FUNC", "EVENT_PRO", "N_CHILDREN", "ALL_CHILDREN", "SIBLING", "REALIZED", "UNITS", "MAP",
    "DRAW_MOTION_EVENTS", "DRAW_BUTTON_EVENTS", "DRAW_EXPOSE_EVENTS", "DRAW_KEYBOARD_EVENTS",
    "DRAW_VIEWPORT_EVENTS", "DRAW_WHEEL_EVENTS", "TRACKING_EVENTS", "SENSITIVE",
  //Fake answer
    "UPDATE","TAB_MODE","TOOLTIP",
    KLISTEND};
  const string widget_infoWarnKey[] = {
    //  "COLUMN_WIDTHS",
    //  "COMBOBOX_NUMBER","COMPONENT","CONTEXT_EVENTS",
    // 
    //  "DROPLIST_NUMBER","DROPLIST_SELECT",
    //  "DYNAMIC_RESIZE","EVENT_FUNC","EVENT_PRO","FIND_BY_UNAME","KBRD_FOCUS_EVENTS",
    //  "LIST_MULTIPLE","LIST_NUMBER","LIST_NUM_VISIBLE","LIST_TOP",
    //  "MULTIPLE_PROPERTIES","PROPERTYSHEET_NSELECTED","PROPERTYSHEET_SELECTED",
    //  "PROPERTY_VALID","PROPERTY_VALUE","PUSHBUTTON_EVENTS","ROW_HEIGHTS",
    //  "SLIDER_MIN_MAX","TABLE_ALL_EVENTS","TABLE_BACKGROUND_COLOR",
    //  "TABLE_EDITABLE","TABLE_EDIT_CELL","TABLE_FONT","TABLE_FOREGROUND_COLOR","TABLE_VIEW",
    //  "TEXT_ALL_EVENTS","TEXT_EDITABLE","TEXT_NUMBER","TEXT_OFFSET_TO_XY",
    //  "TEXT_TOP_LINE","TEXT_XY_TO_OFFSET",
    //  "USE_TABLE_SELECT","VISIBLE",
    KLISTEND
  };
  new DLibFunRetNew(lib::widget_info, string("WIDGET_INFO"), 1, widget_infoKey, widget_infoWarnKey);
  //LABEL
  const string widget_labelKey[] = {WIDGET_COMMON_KEYWORDS, "VALUE", "SUNKEN_FRAME", "TRACKING_EVENTS", "DYNAMIC_RESIZE", KLISTEND};
  new DLibFunRetNew(lib::widget_label, string("WIDGET_LABEL"), 1, widget_labelKey);
  //LIST
  const string widget_listKey[] = {WIDGET_COMMON_KEYWORDS, "MULTIPLE", "VALUE", "CONTEXT_EVENTS", "TRACKING_EVENTS",
  //unsupported but warning is a pain.
    "TAB_MODE", 
    KLISTEND};
  new DLibFunRetNew(lib::widget_list, string("WIDGET_LIST"), 1, widget_listKey);
  //MESSAGE
  //PROPERTYSHEET (2-columns TABLE). UNSUPPORTED !!!
#ifdef HAVE_WXWIDGETS_PROPERTYGRID
  const string widget_propertyKey[] = {WIDGET_COMMON_KEYWORDS, KLISTEND} //to be finished...
  new DLibFunRetNew(lib::widget_property, string("WIDGET_PROPERTYSHEET"), 1, widget_propertyKey);
#endif
  //SLIDER
  const string widget_sliderKey[] = {WIDGET_COMMON_KEYWORDS, "TITLE", "DRAG", "VALUE", "MINIMUM", "MAXIMUM", "VERTICAL",
    "SUPPRESS_VALUE", "TRACKING_EVENTS",
  //unsupported but warning is a pain.
    "TAB_MODE",
    KLISTEND};
  new DLibFunRetNew(lib::widget_slider, string("WIDGET_SLIDER"), 1, widget_sliderKey);
  //STUB
  //TAB
  const string widget_tabKey[] = {WIDGET_COMMON_KEYWORDS, "MULTILINE", "LOCATION", "TRACKING_EVENTS",
  //unsupported but warning is a pain.
  "TAB_MODE",
   KLISTEND};
  new DLibFunRetNew(lib::widget_tab, string("WIDGET_TAB"), 1, widget_tabKey);
  //TABLE
  const string widget_tableKey[] = {WIDGET_COMMON_KEYWORDS
    , "ALIGNMENT"
    , "ALL_EVENTS"
    , "BACKGROUND_COLOR"
    , "COLUMN_LABELS"
    , "COLUMN_WIDTHS"
    , "CONTEXT_EVENTS"
    , "DISJOINT_SELECTION"
    , "EDITABLE"
    , "FOREGROUND_COLOR"
    , "FORMAT"
    , "KBRD_FOCUS_EVENTS"
    , "RESIZEABLE_COLUMNS"
    , "RESIZEABLE_ROWS"
    , "ROW_HEIGHTS"
    , "ROW_LABELS"
    , "TRACKING_EVENTS"
    , "VALUE"
    , "X_SCROLL_SIZE"
    , "Y_SCROLL_SIZE"
    , "NO_COLUMN_HEADERS"
    , "NO_HEADERS"
    , "NO_ROW_HEADERS"
    , "COLUMN_MAJOR"
    , "ROW_MAJOR"
    , "AM_PM"
    , "DAYS_OF_WEEK"
    , "MONTHS"
// unsupported yet but warning about it may be a pain
    , "IGNORE_ACCELERATORS"
    , "TAB_MODE"   
    , KLISTEND};
  new DLibFunRetNew(lib::widget_table, string("WIDGET_TABLE"), 1, widget_tableKey);
  //TEXT
  const string widget_textKey[] = {WIDGET_COMMON_KEYWORDS, "EDITABLE", "NO_NEWLINE", "VALUE",
    "INPUT_FOCUS", "ALL_EVENTS", "CONTEXT_EVENTS", "KBRD_FOCUS_EVENTS", "TRACKING_EVENTS", "WRAP",
// unsupported yet but warning about it may be a pain
  "IGNORE_ACCELERATORS", "TAB_MODE",
  KLISTEND};
  new DLibFunRetNew(lib::widget_text, string("WIDGET_TEXT"), 1, widget_textKey);
  //TREE
  const string widget_treeWarnKey[] = { "DRAG_NOTIFY"

    , KLISTEND};
  const string widget_treeKey[] = {WIDGET_COMMON_KEYWORDS
    , "BITMAP"
    , "MASK"
    , "FOLDER"
    , "EXPANDED"
    , "VALUE"
    , "CONTEXT_EVENTS"
    , "TRACKING_EVENTS"
    , "DROP_EVENTS"
    , "DRAGGABLE"
    , "INDEX"
    , "TOP" //obsolete in 6.4, use INDEX=0 instead
    , "TAB_MODE"
    , "CHECKBOX"
    , "CHECKED"
    , "NO_BITMAPS"
    , "MULTIPLE"
    , "TOOLTIP"
    , "DRAG_NOTIFY"
    , KLISTEND};
  new DLibFunRetNew(lib::widget_tree, string("WIDGET_TREE"), 1, widget_treeKey, widget_treeWarnKey);
  //TREE_MOVE  
  // 	const string widget_bgroupKey[] =
  // 	{"BUTTON_UVALUE","COLUMN","EVENT_FUNC","EXCLUSIVE","NONEXCLUSIVE","SPACE","XPAD","YPAD","FRAME","IDS","LABEL_LEFT","LABEL_TOP","MAP","NO_RELEASE","RETURN_ID","RETURN_INDEX","RETURN_NAME","ROW","SCROLL","SET_VALUE","TAB_MODE","X_SCROLL_SIZE","Y_SCROLL_SIZE","SET_VALUE","UNAME","UVALUE","XOFFSET","XSIZE","YOFFSET","YSIZE",KLISTEND};
  // 	new DLibFunRetNew(lib::widget_bgroup,string("CW_BGROUP"),2,widget_bgroupKey);

  const string dialog_pickFile_wxwidgetsKey[] = { "DEFAULT_EXTENSION", "DIRECTORY", "DIALOG_PARENT",
						  "DISPLAY_NAME", "FILE", "FILTER", "FIX_FILTER", "GET_PATH", "GROUP", "MULTIPLE_FILES",
						  "MUST_EXIST", "OVERWRITE_PROMPT", "PATH", "READ", "WRITE", "RESOURCE_NAME", "TITLE", KLISTEND };
  new DLibFunRetNew(lib::dialog_pickfile_wxwidgets, string("DIALOG_PICKFILE"), 0, dialog_pickFile_wxwidgetsKey);
  new DLibFunRetNew(lib::dialog_pickfile_wxwidgets, string("PICKFILE"), 0, dialog_pickFile_wxwidgetsKey); //old name
  
  const string dialog_message_wxwidgetsKey[] = { "CANCEL", "CENTER", "DEFAULT_CANCEL", "DEFAULT_NO",
						 "DIALOG_PARENT", "DISPLAY_NAME", "ERROR", "INFORMATION", "QUESTION", "RESOURCE_NAME", "TITLE", KLISTEND };
  new DLibFunRetNew(lib::dialog_message_wxwidgets, string("DIALOG_MESSAGE"), 1, dialog_message_wxwidgetsKey);
  new DLibFunRetNew(lib::dialog_message_wxwidgets, string("WIDGET_MESSAGE"), 1, dialog_message_wxwidgetsKey); //old name
#endif

}
