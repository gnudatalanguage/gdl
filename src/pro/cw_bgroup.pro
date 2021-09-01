;+
; CW_BGROUP
;	Create a button group.
;
; Usage:
;	id = cw_bgroup(parent, labels)
;
; Arguments:
;	parent	long	The parent base widget for the compound.
;	labels	string	The labels for the buttons in the group.
;
; Keywords:
;	button_uvalue	string	An array of uvalues for the
;				buttons. If this is specified, then
;				the uvalue is returned in the value
;				field of the generated event
;				(/return... keywords are ignored). 
;	/return_id		If set, the the value field of the
;				event will contain the widget ID of
;				the pressed button.
;	/return_index		If set, then the value field will
;				contain the index of the button (0 based).
;	/return_name		If set, then the value field of the
;				event will contain the label of the
;				button.
;	row		int	Specify the number of rows of buttons
;				(Default is /row)
;	column		int	Specify the number of columns of
;				buttons.
;	/exclusive		If set then put the buttons in an
;				exclusive base.
;	/nonexclusive		If set, then put the buttons in a
;				non-exclusive base.
;	uvalue		string	Specify a user value for the hierarchy.
;	uname		string	Specify a user name for the hierarchy.
;	label_top	string	Specify a label to be placed above the
;				hierarchy.
;	label_left	string	Specify a label to be placed to the
;				left of the hierarchy. label_top has
;				precedence.
;	ids		long	A variable to return the list of
;				widget IDS.
;	/no_release		If set, then disable release
;				events from the buttons.
;	/map			Specify whether the widget is to be
;				initially mapped.
;	event_funct	string	Specify a handler function for events
;				from the heirarchy.
;	/sensitive		Set to zero to make the hierarchy
;				initially not sensitive.
;	set_value	byt/lon	Specify an initial "value" of the
;				hierarchy, for exclusive groups then
;				this is the index of the initially-set
;				button, for non-exclusive groups, it
;				is an array of flags indicating the
;				state of the buttons.
;	/scroll			If set, then the button base will be
;				scrollable. 
;	x_scroll_size	long	Specify the X-size of the view port
;				into the scrollable base.
;	y_scroll_size	long	Specify the Y-size of the view port
;				into the scrollable base.
;	Other keys are passed directly to the base widget that
;	contains the hierarchy base.
;
; Notes:
;	Some options (e.g. PUSHBUTTON_EVENTS) that are not supported
;	but GDL will get silently passed to the base and ignored.
;
; History:
;	First cut: 18/8/21; SJT
;-
; LICENCE:
; Copyright (C) 2018-2021: SJT
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   

function cw_bgroup_get, id
                                ; The parent base of the buttons is
                                ; always the last child of the ID

  idlist = widget_info(id, /all_children)
  bp = idlist[-1]
  widget_control, bp, get_uvalue = uv
  bids =  widget_info(bp, /all_children)
  case uv.type of
     0: return, 0               ; Regular group, get_value has no meaning
     1: begin                   ; non-exclusive group, return states
                                ; of the buttons
        rv = bytarr(uv.nbutton)
        for j = 0, uv.nbutton-1 do rv[j] = widget_info(bids[j], $
                                                       /button_set)
        return, rv
     end

     2: begin                   ; Exclusive group, return the index of
                                ; the selected button.
        for j = 0, uv.nbutton-1 do if widget_info(bids[j], $
                                                  /button_set) then $
                                                     return, j
        return, -1              ; No button set.
     end
  endcase
end

pro cw_bgroup_set, id, value
  
  idlist = widget_info(id, /all_children)
  bp = idlist[-1]
  widget_control, bp, get_uvalue = uv
  bids =  widget_info(bp, /all_children)

  case uv.type of
     0:                   ; No meaning for a regular group.
     1: begin                   ; Non exclusive group.
        jmax = uv.nbutton < n_elements(value)
        for j = 0, jmax-1 do $
           widget_control, bids[j], set_button = value[j]
     end
     2: widget_control, bids[value], set_button = 1
  endcase

end

function cw_bgroup_event, event
  if (event.id eq 0l) then return, 0l

  widget_control, event.handler, get_uvalue = uv
  if uv.norelease && event.select eq 0 then return, 0l
  
  widget_control, event.id, get_uvalue = buv
  
  ev = {id: widget_info(event.handler, /parent), $
        top: event.top, $
        handler: 0l, $
        select: event.select, $
        value: buv}

  new_event = widget_event(event.handler, /nowait)

  return, ev
end

function cw_bgroup, parent, labels, $
                    button_uvalue = button_uvalue, $
                    return_id = return_id, $
                    return_index = return_index, $
                    return_name = return_name, $
                    row = row, column = column, $
                    exclusive = exclusive, $
                    nonexclusive = nonexclusive, $
                    uvalue = uvalue, uname = uname, $
                    label_top = label_top, $
                    label_left = label_left, $
                    ids = ids, no_release = no_release, $
                    map = map, event_funct = event_funct, $
                    sensitive = sensitive, $
                    set_value = set_value, scroll = scroll, $
                    x_scroll_size = x_scroll_size, $
                    y_scroll_size = y_scroll_size, $
                    font = font, $
                    _extra = _extra

; GDL doesn't handle undefined keys very well.
  
  if n_elements(map) ne 0 then ismap = keyword_set(map) $
  else ismap = 1
  if n_elements(sensitive) ne 0 then issens = keyword_set(sensitive) $
  else issens = 1
  
  b0 = widget_base(parent, $
                   column = keyword_set(label_top), $
                   row = keyword_set(label_left) && $
                   ~keyword_set(label_top), $
                   map = ismap, $
                   uvalue = uvalue, $
                   uname = uname, $
                   event_func = event_funct, $
                   sensitive = issens, $
                   func_get_value = 'cw_bgroup_get', $
                   pro_set_value = 'cw_bgroup_set', $
                   _extra = _extra)

  if keyword_set(label_top) then $
     junk = widget_label(b0, $
                         value = label_top) $
  else if keyword_set(label_left) then $
     junk = widget_label(b0, $
                         value = label_left) 

  base = widget_base(b0, $
                     column = column, $
                     row = row, $
                     exclusive = exclusive, $
                     event_func = 'cw_bgroup_event', $
                     nonexclusive = nonexclusive, $
                     scroll = keyword_set(scroll), $
                     x_scroll_size = x_scroll_size, $
                     y_scroll_size = y_scroll_size)
  
  nbuts = n_elements(labels)

  ids = lonarr(nbuts)
  for j = 0, nbuts-1 do $
     ids[j] = widget_button(base, $
                            value = labels[j], $
                            font = font)

  if keyword_set(button_uvalue) then $
     for j = 0, nbuts-1 do widget_control, ids[j], $
                                           set_uvalue = $
                                           button_uvalue[j] $
  else if keyword_set(return_id) then $
     for j = 0, nbuts-1 do widget_control, ids[j], $
                                           set_uvalue = ids[j] $
  else if keyword_set(return_name) then $
     for j = 0, nbuts-1 do widget_control, ids[j], $
                                           set_uvalue = labels[j] $
  else $
     for j = 0, nbuts-1 do widget_control, ids[j], $
                                           set_uvalue = j

  if keyword_set(exclusive) then type = 2 $ 
  else if keyword_set(nonexclusive) then type = 1 $
  else type = 0

  if type ne 0 then norel = keyword_set(no_release) $
  else norel = 0
  
  uv = {type: type, $
        nbutton: nbuts, $
        norelease: norel}

  widget_control, base, set_uvalue = uv
  
  if keyword_set(set_value) &&  type ne 0 then $
     widget_control, b0, set_value = set_value

  return, b0
  
end
