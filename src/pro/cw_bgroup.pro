; part of GNU Data Language - GDL 
;-------------------------------------------------------------
;+
; NAME:
;       CW_BGROUP
; PURPOSE:
;       Create a compound checkbox, radiobox or regular button group
;
; CATEGORY:
;       GUI widgets
;
; CALLING SEQUENCE:
; widgetID = CW_BGROUP(ParentID, ButtonNames, $
;                    BUTTON_UVALUE=button_uvalue, COLUMN=column, EVENT_FUNCT=event_funct,$
;                    EXCLUSIVE=exclusive,NONEXCLUSIVE=nonexclusive,SPACE=space,$
;                    XPAD=xpad, YPAD=ypad, FONT=font, FRAME=frame, IDS=ids,$
;                    LABEL_LEFT=label_left, LABEL_TOP=label_top, MAP=map, NO_RELEASE=no_release,$
;                    RETURN_ID=return_id, RETURN_INDEX=return_index, RETURN_NAME=return_name,$
;                    ROW=row, SCROLL=scroll, SET_VALUE=sel_value, TAB_MODE=tab_mode,$
;                    X_SCROLL_SIZE=x_scroll_size, Y_SCROLL_SIZE=y_scroll_size,$
;                    SET_VALUE=set_value, UNAME=uname, UVALUE=uvalue, XOFFSET=xoffset,$
;                    XSIZE=xsize, YOFFSET=yoffset, YSIZE=ysize)
;
; INPUTS:
;
; KEYWORD PARAMETERS:
;
; OUTPUTS:
;       widgetID = widget ID of compound widget
;
; MODIFICATION HISTORY:
; 	written 2013-10-29 by Marc Schellens (Initial import)
;
;-
; LICENCE:
; Copyright (C) 2013
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;
function CW_BGROUP_EVENT,ev
  widget_control,ev.handler,GET_UVALUE=state1
  if size(state1,/TYPE) ne 8 then begin ;; no STRUCT?
     ;; if no struct, uvalue is the UVALUE holder's ID
     widget_control,state1,GET_UVALUE=state
  endif else begin
     state = state1
  endelse
  widget_control,ev.id,GET_UVALUE=index  
  if ev.select ne 0 then begin
     state.lastButton = index
  endif
  if state.exclusiveMode eq 0 then begin
  endif
  if state.exclusiveMode eq 1 then begin
  endif
  if state.exclusiveMode eq 2 then begin
     state.buttonState[index] = ev.select
  endif

  widget_control,ev.handler,SET_UVALUE=state

  if state.noRelease ne 0 and ev.select eq 0 then return,0

  retEv = { ID:state.topBaseID, $
            TOP:ev.top,$
            HANDLER:0L,$
            SELECT:ev.select, $
            VALUE:state.rArray[index] }
  return,retEv
end

function CW_BGROUP_GETV,id
  bbase = widget_info(id,/CHILD)
  widget_control,bbase,GET_UVALUE=state
  if state.exclusiveMode eq 0 then begin
     message,"CW_BGROUP: Unable to get plain button group value."
  endif
  if state.exclusiveMode eq 1 then begin
     return,state.rArray[state.lastButton]
  endif
  return,state.buttonState
end

pro CW_BGROUP_SETV,id,value
  bbase = widget_info(id,/CHILD)
  widget_control,bbase,GET_UVALUE=state

  if state.exclusiveMode eq 0 then begin
     message,"CW_BGROUP: Unable to set plain button group value."
     return
  endif

  if state.exclusiveMode eq 1 then begin
     state.lastButton = value
     widget_control,state.buttonID[value],SET_BUTTON=1
     widget_control,bbase,SET_UVALUE=state
     return
  endif

  for b=0,N_ELEMENTS(value)-1 do begin
     state.buttonState[b] = value[b]
     widget_control,state.buttonID[b],SET_BUTTON=value[b]
  endfor
  widget_control,bbase,SET_UVALUE=state
end

function CW_BGROUP,parentID, buttonNames, $
                   BUTTON_UVALUE=button_uvalue, COLUMN=column, EVENT_FUNCT=event_funct,$
                   EXCLUSIVE=exclusive,NONEXCLUSIVE=nonexclusive,SPACE=space,$
                   XPAD=xpad, YPAD=ypad, FONT=font, FRAME=frame, IDS=ids,$
                   LABEL_LEFT=label_left, LABEL_TOP=label_top, MAP=map, NO_RELEASE=no_release,$
                   RETURN_ID=return_id, RETURN_INDEX=return_index, RETURN_NAME=return_name,$
                   ROW=row, SCROLL=scroll, SET_VALUE=set_value, TAB_MODE=tab_mode,$
                   X_SCROLL_SIZE=x_scroll_size, Y_SCROLL_SIZE=y_scroll_size,$
                   UNAME=uname, UVALUE=uvalue, XOFFSET=xoffset,$
                   XSIZE=xsize, YOFFSET=yoffset, YSIZE=ysize

  nButtons = N_ELEMENTS(buttonNames)
  if nButtons eq 0 then begin
     MESSAGE,"CW_BGROUP: 2nd parameter (button names) not valid."
     return,0
  endif

  if N_ELEMENTS( button_uvalue) gt 0 then begin
     if N_ELEMENTS( button_uvalue) ne nButtons then begin
        MESSAGE,"CW_BGROUP: BUTTON_UVALUE mut contain one element for each button."
        return,0
     endif
  endif

  exclusiveMode = 0
  if keyword_set(exclusive) then begin
     if keyword_set(nonexclusive) then begin
        MESSAGE,"CW_BGROUP: Conficting keywords: [NON]EXCLUSIVE."
        return,0
     endif
     exclusiveMode = 1
  endif
  if keyword_set(nonexclusive) then begin
     exclusiveMode = 2
  endif

  widgetID = widget_base(parentID,/COL,MAP=map,UNAME=uname,UVALUE=uvalue,$
                         FUNC_GET_VALUE='CW_BGROUP_GETV', PRO_SET_VALUE='CW_BGROUP_SETV', $
                         EVENT_FUNC=event_funct,$
                         XSIZE=xsize,YSIZE=ysize)
  uValueHolder = 0
  if N_ELEMENTS(label_top) ne 0 then begin
     label = widget_label( widgetID, VALUE=label_top)
     uValueHolder = label
  endif
  
  if uValueHolder eq 0 then begin
     baseID = widget_base(widgetID,EXCLUSIVE=exclusive,NONEXCLUSIVE=nonexclusive,$
                          COLUMN=column,ROW=row, EVENT_FUNC="CW_BGROUP_EVENT")
     uValueHolder = baseID
  endif else begin
     baseID = widget_base(widgetID,EXCLUSIVE=exclusive,NONEXCLUSIVE=nonexclusive,$
                          COLUMN=column,ROW=row, EVENT_FUNC="CW_BGROUP_EVENT",UVALUE=label)
  endelse

  ids = lonarr(nButtons)

;; if N_ELEMENTS( button_uvalue) gt 0 then begin
  for b=0,nButtons-1 do begin

     ids[b]=widget_button(baseID,VALUE=buttonNames[b],UVALUE=b)
  endfor
;; endif else begin
;;    for b=0,nButtons-1 do begin

;;       ids[b]=widget_button(baseID,VALUE=buttonNames[b])
;;    endfor
;; endelse

  rMode = -1 
  if N_ELEMENTS( button_uvalue) gt 0 then begin
     rMode = 1                  ; button_uvalue (overrides)
     rArray = button_uvalue
;     help,1,rArray
  endif else begin
     if keyword_set( return_name) then begin
        rMode = 3
        rArray = buttonNames
;        help,3,rArray
     endif else if keyword_set( return_id) then begin
        rMode = 4
        rArray = ids
;        help,4,rArray
     endif else begin
        ;; default -> return index
        rMode = 2
        rArray = lindgen( nButtons)
;        help,2,rArray
     endelse
  endelse

  internalUValue = {rArray:rArray,$
                    exclusiveMode: exclusiveMode,$
                    buttonID: ids,$
                    buttonState: intarr(nButtons),$
                    lastButton: 0,$
                    noRelease: keyword_set(no_release),$
                    buttonBase: baseID,$
                    topBaseID: widgetID }
  widget_control,uValueHolder,SET_UVALUE=internalUValue

  if N_ELEMENTS(set_value) ne 0 then CW_BGROUP_SETV, widgetID, set_value

  return,widgetID
end
