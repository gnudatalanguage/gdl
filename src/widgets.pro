Result = WIDGET_ACTIVEX( Parent, COM_ID, , /ALIGN_BOTTOM ,$
                         /ALIGN_CENTER , /ALIGN_LEFT , /ALIGN_RIGHT , /ALIGN_TOP,$
                         EVENT_FUNC=string , EVENT_PRO=string , FUNC_GET_VALUE=string,$
                         ID_TYPE=value , KILL_NOTIFY=string , /NO_COPY,$
                         NOTIFY_REALIZE=string , PRO_SET_VALUE=string , SCR_XSIZE=width,$
                         SCR_YSIZE=height , /SENSITIVE , UNAME=string , UNITS=0,1,2,$
                         UVALUE=value , XOFFSET=value , XSIZE=value , YOFFSET=value,$
                         YSIZE=value )
Result = WIDGET_BASE( Parent , /ALIGN_BOTTOM , /ALIGN_CENTER ,$
                      /ALIGN_LEFT , /ALIGN_RIGHT , /ALIGN_TOP , /MBAR , /MODAL,$
                      /BASE_ALIGN_BOTTOM , /BASE_ALIGN_CENTER ,$
                      /BASE_ALIGN_LEFT , /BASE_ALIGN_RIGHT , /BASE_ALIGN_TOP,$
                      /COLUMN , /ROW , /CONTEXT_EVENTS , /CONTEXT_MENU,$
                      EVENT_FUNC=string , EVENT_PRO=string , /EXCLUSIVE ,$
                      /NONEXCLUSIVE , /FLOATING , FRAME=width,$
                      FUNC_GET_VALUE=string , /GRID_LAYOUT,$
                      GROUP_LEADER=widget_id,$ ;{must specify for modal dialogs}
                      /KBRD_FOCUS_EVENTS , KILL_NOTIFY=string , /MAP,$ ;{not for modal bases} 
                      /NO_COPY , NOTIFY_REALIZE=string , PRO_SET_VALUE=string,$
                      SCR_XSIZE=width , SCR_YSIZE=height , /SCROLL{not for modal bases},$
                      /SENSITIVE , SPACE=value,$ ;{ignored if exclusive or nonexclusi
                      TITLE=string , TLB_FRAME_ATTR=value,$ ;{top-level bases only
                      /TLB_ICONIFY_EVENTS,$ ;{top-level bases only},$
                      /TLB_KILL_REQUEST_EVENTS,$ ;{top-level bases only},$
                      /TLB_MOVE_EVENTS,$ ;{top-level bases only} 
                      /TLB_SIZE_EVENTS,$ ;{top-level bases only} 
                      /TOOLBAR , /TRACKING_EVENTS , UN,$
                      UNITS=0,1,2 , UVALUE=value , XOFFSET=value,$
                      XPAD=value,$ ;{ignored if exclusive or nonexclusive} 
                      XSIZE=val,$
                      X_SCROLL_SIZE=value , YOFFSET=value , YPAD=value,$ ; {ignored if exclusive or nonexclusive} 
                      YSIZE=value , Y_SCROLL_SIZE=,$
                      DISPLAY_NAME=string,$
                      RESOURCE_NAME=string , RNAME_MBAR=string)
Result = WIDGET_BUTTON( Parent , /ALIGN_CENTER , /ALIGN_LEFT ,$
                        /ALIGN_RIGHT , /BITMAP , /CHECKED_MENU , /DYNAMIC_RESIZE,$
                        EVENT_FUNC=string , EVENT_PRO=string , FONT=string,$
                        FRAME=width , FUNC_GET_VALUE=string,$
                        GROUP_LEADER=widget_id , /HELP , KILL_NOTIFY=string , /MENU,$
                        /NO_COPY , /NO_RELEASE , NOTIFY_REALIZE=string,$
                        PRO_SET_VALUE=string , /PUSHBUTTON_EVENTS , SCR_XSIZE=width,$
                        SCR_YSIZE=height , /SENSITIVE , /SEPARATOR , TOOLTIP=string,$
                        /TRACKING_EVENTS , UNAME=string , UNITS=0,1,2,$
                        UVALUE=value , VALUE=value , X_BITMAP_EXTRA=bits,$
                        XOFFSET=value , XSIZE=value , YOFFSET=value , YSIZE=value, RESOURCE_NAME=string)
Result = WIDGET_COMBOBOX( Parent , /DYNAMIC_RESIZE , /EDITABLE,$
                          EVENT_FUNC=string , EVENT_PRO=string , FONT=string,$
                          FRAME=value , FUNC_GET_VALUE=string,$
                          GROUP_LEADER=widget_id , KILL_NOTIFY=string , /NO_COPY,$
                          NOTIFY_REALIZE=string , PRO_SET_VALUE=string,$
                          RESOURCE_NAME=string , SCR_XSIZE=width , SCR_YSIZE=height,$
                          /SENSITIVE , /TRACKING_EVENTS , UNAME=string , UNITS=0,1,2,$
                          , UVALUE=value , VALUE=value , XOFFSET=value , XSIZE=value,$
                          YOFFSET=value , YSIZE=value )
Result = WIDGET_DRAW(Parent , /APP_SCROLL , /BUTTON_EVENTS,$
                     /COLOR_MODEL , COLORS=integer , EVENT_FUNC=string,$
                     EVENT_PRO=string , /EXPOSE_EVENTS , FRAME=width,$
                     FUNC_GET_VALUE=string , GRAPHICS_LEVEL=2,$
                     GROUP_LEADER=widget_id , KEYBOARD_EVENTS=1,2,$
                     KILL_NOTIFY=string , /MOTION_EVENTS , /NO_COPY,$
                     NOTIFY_REALIZE=string , PRO_SET_VALUE=string , RENDERER=0,1,$
                     , RESOURCE_NAME=string , RETAIN=0,1,2 , SCR_XSIZE=width,$
                     SCR_YSIZE=height , /SCROLL , /SENSITIVE , TOOLTIP=string,$
                     /TRACKING_EVENTS , UNAME=string , UNITS=0,1,2,$
                     UVALUE=value , VALUE=value , /VIEWPORT_EVENTS,$
                     XOFFSET=value , XSIZE=value , X_SCROLL_SIZE=width,$
                     YOFFSET=value , YSIZE=value , Y_SCROLL_SIZE=height)
Result = WIDGET_DROPLIST( Parent , /DYNAMIC_RESIZE,$
                          EVENT_FUNC=string , EVENT_PRO=string , FONT=string,$
                          FRAME=value , FUNC_GET_VALUE=string,$
                          GROUP_LEADER=widget_id , KILL_NOTIFY=string , /NO_COPY,$
                          NOTIFY_REALIZE=string , PRO_SET_VALUE=string,$
                          RESOURCE_NAME=string , SCR_XSIZE=width , SCR_YSIZE=height,$
                          /SENSITIVE , TITLE=string , /TRACKING_EVENTS , UNAME=string,$
                          UNITS=0,1,2 , UVALUE=value , VALUE=value , XOFFSET=value,$
                          XSIZE=value , YOFFSET=value , YSIZE=value )
Result = WIDGET_LABEL( Parent , /ALIGN_CENTER , /ALIGN_LEFT ,$
                       /ALIGN_RIGHT , /DYNAMIC_RESIZE , FONT=string , FRAME=width,$
                       FUNC_GET_VALUE=string , GROUP_LEADER=widget_id,$
                       KILL_NOTIFY=string , /NO_COPY , NOTIFY_REALIZE=string,$
                       PRO_SET_VALUE=string , RESOURCE_NAME=string,$
                       SCR_XSIZE=width , SCR_YSIZE=height , /SENSITIVE,$
                       /SUNKEN_FRAME , /TRACKING_EVENTS , UNAME=string , UNITS=0,1,2,$
                       UVALUE=value , VALUE=value , XOFFSET=value , XSIZE=value,$
                       YOFFSET=value , YSIZE=value )
Result = WIDGET_LIST( Parent , /CONTEXT_EVENTS,$
                      EVENT_FUNC=string , EVENT_PRO=string , FONT=string,$
                      FRAME=width , FUNC_GET_VALUE=string,$
                      GROUP_LEADER=widget_id , KILL_NOTIFY=string , /MULTIPLE,$
                      /NO_COPY , NOTIFY_REALIZE=string , PRO_SET_VALUE=string,$
                      RESOURCE_NAME=string , SCR_XSIZE=width , SCR_YSIZE=height,$
                      /SENSITIVE , /TRACKING_EVENTS , UNAME=string , UNITS=0,1,2,$
                      UVALUE=value , VALUE=value , XOFFSET=value , XSIZE=value,$
                      YOFFSET=value , YSIZE=value )
Result = WIDGET_PROPERTYSHEET(Parent , /ALIGN_BOTTOM,$
                              /ALIGN_CENTER , /ALIGN_LEFT , /ALIGN_RIGHT , /ALIGN_TOP,$
                              /CONTEXT_EVENTS , EVENT_FUNC=string , EVENT_PRO=string,$
                              FONT=string , FUNC_GET_VALUE=string , KILL_NOTIFY=string,$
                              /NO_COPY , NOTIFY_REALIZE=string , PRO_SET_VALUE=string,$
                              SCR_XSIZE=width , SCR_YSIZE=height , /SENSITIVE,$
                              /TRACKING_EVENTS , UNAME=string ,UNITS=0,1,2,$
                              UVALUE=value , VALUE=value , XOFFSET=value , XSIZE=value,$
                              YOFFSET=value , YSIZE=value)
Result = WIDGET_SLIDER( Parent , /DRAG , EVENT_FUNC=string,$
                        EVENT_PRO=string , FONT=string , FRAME=width,$
                        FUNC_GET_VALUE=string , GROUP_LEADER=widget_id,$
                        KILL_NOTIFY=string , MAXIMUM=value , MINIMUM=value,$
                        /NO_COPY , NOTIFY_REALIZE=string , PRO_SET_VALUE=string,$
                        RESOURCE_NAME=string , SCR_XSIZE=width , SCR_YSIZE=height,$
                        SCROLL=units , /SENSITIVE , /SUPPRESS_VALUE,$
                        /TRACKING_EVENTS , TITLE=string , UNAME=string , UNITS=0,1,2,$
                        , UVALUE=value , VALUE=value , /VERTICAL , XOFFSET=value,$
                        XSIZE=value , YOFFSET=value , YSIZE=value )
Result = WIDGET_TAB( Parent , /ALIGN_BOTTOM , /ALIGN_CENTER ,$
                     /ALIGN_LEFT , /ALIGN_RIGHT , /ALIGN_TOP , EVENT_FUNC=string,$
                     EVENT_PRO=string , FUNC_GET_VALUE=string,$
                     GROUP_LEADER=widget_id , KILL_NOTIFY=string,$
                     LOCATION=0,1,2,3 , MULTILINE=0,1,$ ;(Windows) or num tabs per row,$
                     /NO_COPY , NOTIFY_REALIZE=string,$
                     PRO_SET_VALUE=string , SCR_XSIZE=width , SCR_YSIZE=height,$
                     /SENSITIVE , /TRACKING_EVENTS , UNAME=string , UNITS=0,1,2,$
                     , UVALUE=value , XOFFSET=value , XSIZE=value , YOFFSET=value,$
                     YSIZE=value )
Result = WIDGET_TABLE( Parent , ALIGNMENT=0,1,2 , /ALL_EVENTS,$
                       AM_PM=string, string , COLUMN_LABELS=string_array,$
                       /COLUMN_MAJOR , /ROW_MAJOR , COLUMN_WIDTHS=array,$
                       DAYS_OF_WEEK=string_array , /DISJOINT_SELECTION,$
                       /EDITABLE , EVENT_FUNC=string , EVENT_PRO=string , FONT=string,$
                       FORMAT=value , FRAME=width , FUNC_GET_VALUE=string,$
                       GROUP_LEADER=widget_id , /KBRD_FOCUS_EVENTS,$
                       KILL_NOTIFY=string , MONTHS=string_array , /NO_COPY,$
                       /NO_HEADERS , NOTIFY_REALIZE=string , PRO_SET_VALUE=string,$
                       /RESIZEABLE_COLUMNS , /RESIZEABLE_ROWS, RESOURCE_NAME=string,$
                       ROW_HEIGHTS=x , ROW_LABELS=string_array , SCR_XSIZE=width,$
                       SCR_YSIZE=height , /SCROLL , /SENSITIVE , /TRACKING_EVENTS,$
                       UNAME=string , UNITS=0,1,2 , UVALUE=value , VALUE=value,$
                       XOFFSET=value , XSIZE=value , X_SCROLL_SIZE=width,$
                       YOFFSET=value , YSIZE=value , Y_SCROLL_SIZE=height )
Result = WIDGET_TEXT( Parent , /ALL_EVENTS , /CONTEXT_EVENTS,$
                      /EDITABLE , EVENT_FUNC=string , EVENT_PRO=string , FONT=string,$
                      FRAME=width , FUNC_GET_VALUE=string,$
                      GROUP_LEADER=widget_id , /KBRD_FOCUS_EVENTS,$
                      KILL_NOTIFY=string , /NO_COPY , /NO_NEWLINE,$
                      NOTIFY_REALIZE=string , PRO_SET_VALUE=string,$
                      RESOURCE_NAME=string , SCR_XSIZE=width , SCR_YSIZE=height,$
                      /SCROLL , /SENSITIVE , /TRACKING_EVENTS , UNAME=string,$
                      UNITS=0,1,2 , UVALUE=value , VALUE=value , /WRAP,$
                      XOFFSET=value , XSIZE=value , YOFFSET=value , YSIZE=value )
Result = WIDGET_TREE( Parent , /ALIGN_BOTTOM , /ALIGN_CENTER,$
                      /ALIGN_LEFT , /ALIGN_RIGHT , /ALIGN_TOP , BITMAP=array,$
                      /CONTEXT_EVENTS , EVENT_FUNC=string , EVENT_PRO=string,$
                      /EXPANDED , /FOLDER , FUNC_GET_VALUE=string,$
                      GROUP_LEADER=widget_id , KILL_NOTIFY=string , /MULTIPLE,$
                      /NO_COPY , NOTIFY_REALIZE=string , PRO_SET_VALUE=string,$
                      SCR_XSIZE=width , SCR_YSIZE=height , /SENSITIVE,$
                      /TRACKING_EVENTS , /TOP , UNAME=string , UNITS=0,1,2,$
                      UVALUE=value , VALUE=string , XOFFSET=value , XSIZE=value,$
                      YOFFSET=value , YSIZE=value )


Result = WIDGET_EVENT(Widget_ID, BAD_ID=variable , /NOWAIT,$
                      /SAVE_HOURGLASS,/YIELD_TO_TTY)
WIDGET_CONTROL , Widget_ID,$
  Keywords that apply to all widgets: , BAD_ID=variable , /CLEAR_EVENTS,$
  DEFAULT_FONT=string,$         ;{do not specify Widget_ID} 
  /DELAY_DESTROY,$              ; do not specify Widget_ID} 
  /DESTROY , EVENT_FUNC=string,$
  EVENT_PRO=string , FUNC_GET_VALUE=string,$
  GET_UVALUE=variable , GROUP_LEADER=widget_id , /HOURGLASS,$ ;{do not specify Widget_ID} 
  KILL_NOTIFY=string , /MAP , /NO_COPY,$
  NOTIFY_REALIZE=string , PRO_SET_VALUE=string , /REALIZE,$
  /RESET,$                      ;{do not specify Widget_ID} 
  SCR_XSIZE=width,$
  SCR_YSIZE=height , SEND_EVENT=structure , /SENSITIVE,$
  SET_UNAME=string , SET_UVALUE=value , /SHOW , TIMER=value,$
  TLB_GET_OFFSET=variable , TLB_GET_SIZE=variable,$
  /TLB_KILL_REQUEST_EVENTS , TLB_SET_TITLE=string,$
  TLB_SET_XOFFSET=value , TLB_SET_YOFFSET=value,$
  /TRACKING_EVENTS , UNITS=0,1,2 , /UPDATE , XOFFSET=value,$
  XSIZE=value , YOFFSET=value , YSIZE=value,$
  Keywords that apply to widgets created with WIDGET_ACTIVEX:,$
  GET_VALUE=value,$
  Keywords that apply to widgets created with WIDGET_BASE:,$
  BASE_SET_TITLE=string , CANCEL_BUTTON=widget_id,$ ;{for modal bases}
  /CONTEXT_EVENTS , DEFAULT_BUTTON=widget_id,$ ;{for modal bases}
  /ICONIFY , /KBRD_FOCUS_EVENTS , /TLB_ICONIFY_EVENTS,$
  /TLB_KILL_REQUEST_EVENTS , /TLB_MOVE_EVENTS,$
  /TLB_SIZE_EVENTS,$
  Keywords that apply to widgets created with WIDGET_BUTTON: , /BITMAP,$
  /DYNAMIC_RESIZE , GET_VALUE=value , /INPUT_FOCUS,$
  /PUSHBUTTON_EVENTS , /SET_BUTTON , SET_VALUE=value,$
  TOOLTIP=string , X_BITMAP_EXTRA=bits,$
  Keywords that apply to widgets created with WIDGET_COMBOBOX:,$
  COMBOBOX_ADDITEM=string , COMBOBOX_DELETEITEM=integer,$
  COMBOBOX_INDEX=integer , ./DYNAMIC_RESIZE,$
  GET_VALUE=value , SET_COMBOBOX_SELECT=integer,$
  SET_VALUE=value,$
  Keywords that apply to widgets created with WIDGET_DRAW:,$
  /DRAW_BUTTON_EVENTS , /DRAW_EXPOSE_EVENTS,$
  DRAW_KEYBOARD_EVENTS=0,1,2 , /DRAW_MOTION_EVENTS,$
  /DRAW_VIEWPORT_EVENTS , DRAW_XSIZE=integer,$
  DRAW_YSIZE=integer , GET_DRAW_VIEW=variable,$
  GET_UVALUE=variable , GET_VALUE=variable , /INPUT_FOCUS,$
  SET_DRAW_VIEW=x, y , TOOLTIP=string,$
  Keywords that apply to widgets created with WIDGET_DROPLIST:,$
  /DYNAMIC_RESIZE , GET_VALUE=variable,$
  SET_DROPLIST_SELECT=integer , SET_VALUE=value,$
  Keywords that apply to widgets created with WIDGET_LABEL:,$
  /DYNAMIC_RESIZE , GET_VALUE=value , SET_VALUE=value,$
  Keywords that apply to widgets created with WIDGET_LIST:,$
  /CONTEXT_EVENTS , SET_LIST_SELECT=value,$
  SET_LIST_TOP=integer , SET_VALUE=value,$
  Keywords that apply to widgets created with WIDGET_PROPERTYSHEET:,$
  REFRESH_PROPERTY=string, array of strings, or integer,$
  Keywords that apply to widgets created with WIDGET_SLIDER:,$
  GET_VALUE=value , SET_SLIDER_MAX=value,$
  SET_SLIDER_MIN=value , SET_VALUE=value,$
  Keywords that apply to widgets created with WIDGET_TAB:,$
  SET_TAB_CURRENT=index , SET_TAB_MULTILINE=value,$
  Keywords that apply to widgets created with WIDGET_TABLE:,$
  ALIGNMENT=0,1,2 , /ALL_TABLE_EVENTS , AM_PM=string,$
  string , COLUMN_LABELS=string_array , COLUMN_WIDTHS=array,$
  DAYS_OF_WEEK=string_array, /DELETE_COLUMNS,$ ;{not for row_major mode} 
  /DELETE_ROWS,$                ;{not for column_major mode}
  /EDITABLE , EDIT_CELL=integer, integer , FORMAT=value,$
  GET_VALUE=variable , INSERT_COLUMNS=value,$
  INSERT_ROWS=value , /KBRD_FOCUS_EVENTS,$
  MONTHS=string_array , ROW_LABELS=string_array,$
  ROW_HEIGHTS=array , SET_TABLE_SELECT=left, top, right, bottom,$
  SET_TABLE_VIEW=integer, integer , SET_TEXT_SELECT=integer,$
  integer , SET_VALUE=value , /TABLE_BLANK=cells,$
  /TABLE_DISJOINT_SELECTION , TABLE_XSIZE=columns,$
  TABLE_YSIZE=rows , /USE_TABLE_SELECT ,$
  USE_TABLE_SELECT=left, top, right, bottom , /USE_TEXT_SELECT,$
  Keywords that apply to widgets created with WIDGET_TEXT:,$
  /ALL_TEXT_EVENTS , /APPEND , /CONTEXT_EVENTS , /EDITABLE,$
  GET_VALUE=variable , /INPUT_FOCUS , /KBRD_FOCUS_EVENTS,$
  /NO_NEWLINE , SET_TEXT_SELECT=integer, integer,$
  SET_TEXT_TOP_LINE=line_number , SET_VALUE=value,$
  /USE_TEXT_SELECT,$
  Keywords that apply to widgets created with WIDGET_TREE:,$
  SET_TREE_BITMAP=array , /SET_TREE_EXPANDED,$
  SET_TREE_SELECT= 0,1,widget_ID, array_of_widget_IDs,$
  /SET_TREE_VISIBLE

WIDGET_DISPLAYCONTEXTMENU, Parent, X, Y, ContextBase_ID

Result = WIDGET_INFO( Widget_ID )
Keywords that apply to all widgets: , /ACTIVE , /CHILD , /EVENT_FUNC,$
  /EVENT_PRO , FIND_BY_UNAME=string , /FONTNAME,$
  /GEOMETRY , /KBRD_FOCUS_EVENTS , /MANAGED , /MAP,$
  /NAME , /PARENT , /REALIZED , /SENSITIVE , /SIBLING,$
  /SYSTEM_COLORS , /TRACKING_EVENTS , /TYPE , UNITS=0,1,2,$
  /UNAME , /UPDATE , /VALID_ID , /VERSION , /VISIBLE,$
  Keywords that apply to widgets created with WIDGET_BASE:,$
  /CONTEXT_EVENTS , /MODAL , /TLB_ICONIFY_EVENTS,$
  /TLB_KILL_REQUEST_EVENTS , /TLB_MOVE_EVENTS,$
  /TLB_SIZE_EVENTS,$
  Keywords that apply to widgets created with WIDGET_BUTTON:,$
  /BUTTON_SET , /DYNAMIC_RESIZE , /PUSHBUTTON_EVENTS,$
  /TOOLTIP,$
  Keywords that apply to widgets created with WIDGET_COMBOBOX:,$
  /COMBOBOX_GETTEXT , /COMBOBOX_NUMBER,$
  /DYNAMIC_RESIZE,$
  Keywords that apply to widgets created with WIDGET_DRAW:,$
  /DRAW_BUTTON_EVENTS , /DRAW_EXPOSE_EVENTS,$
  DRAW_KEYBOARD_EVENTS=0,1,2 , /DRAW_MOTION_EVENTS,$
  /DRAW_VIEWPORT_EVENTS , /TOOLTIP,$
  Keywords that apply to widgets created with WIDGET_DROPLIST:,$
  /DROPLIST_NUMBER , /DROPLIST_SELECT , /DYNAMIC_RESIZE,$
  Keywords that apply to widgets created with WIDGET_LABEL:,$
  /DYNAMIC_RESIZE,$
  Keywords that apply to widgets created with WIDGET_LIST:,$
  /CONTEXT_EVENTS , /LIST_MULTIPLE , /LIST_NUMBER,$
  /LIST_NUM_VISIBLE , /LIST_SELECT , /LIST_TOP,$
  Keywords that apply to widgets created with WIDGET_PROPERTYSHEET:,$
  COMPONENT=objref , /PROPERTY_VALID , /PROPERTY_VALUE,$
  Keywords that apply to widgets created with WIDGET_SLIDER:,$
  /SLIDER_MIN_MAX,$
  Keywords that apply to widgets created with WIDGET_TAB:,$
  /TAB_CURRENT , /TAB_MULTILINE , /TAB_NUMBER,$
  Keywords that apply to widgets created with WIDGET_TABLE:,$
  /COLUMN_WIDTHS , /ROW_HEIGHTS,$ ;{not supported in Windows},$
  /TABLE_ALL_EVENTS , /TABLE_DISJOINT_SELECTION,$
  /TABLE_EDITABLE , /TABLE_EDIT_CELL , /TABLE_SELECT,$
  /TABLE_VIEW , /USE_TABLE_SELECT,$
  Keywords that apply to widgets created with WIDGET_TEXT:,$
  /CONTEXT_EVENTS , /TEXT_ALL_EVENTS , /TEXT_EDITABLE,$
  /TEXT_NUMBER , TEXT_OFFSET_TO_XY=integer , /TEXT_SELECT,$
  /TEXT_TOP_LINE , TEXT_XY_TO_OFFSET=column, line,$
  Keywords that apply to widgets created with WIDGET_TREE:,$
  /TREE_EXPANDED , /TREE_ROOT , /TREE_SELECT
