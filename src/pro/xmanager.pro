;+
; NAME:   XMANAGER
;
; PURPOSE:  Provides event management for GDL widgets.
;
; CATEGORY: GUI, widgets
;
; CALLING SEQUENCE:
;
; XMANAGER, NAME, ID, /NO_BLOCK, GROUP_LEADER=groupLeader, CLEANUP=Cleanup
;
;
; inputs:
;
; OPTIONAL INPUTS:  none
;
; KEYWORD PARAMETERS:
;
; COMMON BLOCKS:    "MANAGED"
;
; SIDE EFFECTS:     ????
;
; RESTRICTIONS:     incomplete, more functionality to be added.
;
;
; MODIFICATION HISTORY:
;  - 05/11/2013: inital version
;  - 25/08/2016: removed UNXREGISTER function (now in unxregister.pro):
;                Indeed, Toplevel widgets can be managed by user, without use of xmanager.
;                however, at Top Level widget destruction, function UNXREGISTER is called by the deletion of the widget.
;                So UNXREGISTER must exist in the path indedently from xmanager. 
;
;-
; LICENCE:
; Copyright (C) 2013 Marc Schellens
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.
;-
;

pro ValidateManagedWidgets

common managed, ids, names, modalList
catch,falseinfo
; if ids contains only unknown widget ids, remove silently the list
; after catching the error:
if falseinfo ne 0 then begin
  ids=0
  names=0
  catch,/cancel
  return
endif

if (n_elements(ids) eq 0) then begin ids=0 & names=0 & endif
if (ids[0] eq 0) then return
keep=where(widget_info(ids,/managed),count)
if ( count gt 0 ) then begin
  ids=(temporary(ids))[keep]
  names=(temporary(names))[keep]
endif else begin
  ids = 0
  names = 0
endelse
return

end

pro XMANAGER, name, id, NO_BLOCK = noBlock, GROUP_LEADER=groupLeader, EVENT_HANDLER=eventHandler, $
    CLEANUP=Cleanup, JUST_REG=just_reg, CATCH=catch, MODAL=modal

common managed, ids, names, modalList

ValidateManagedWidgets

if keyword_set(modal) then message,/informational,"The MODAL keyword to the XMANAGER procedure is obsolete."+$
" It is superseded by the MODAL keyword to the WIDGET_BASE function."
if not keyword_set(eventHandler) then begin
  eventHandler = name + '_event'
endif
;if id is not the top base, get the top base:
while (widget_info(id,/parent) ne 0) do id=widget_info(id,/parent)
 
widget_control, id, event_pro=eventHandler
widget_control, id, /managed
; add to common
if (ids[0] ne 0) then begin
 ids = [ids, id]
 names = [names, name]
endif else begin
 ids = id
 names = name
endelse

if keyword_set(groupLeader) then begin
   widget_control, id, GROUP_LEADER=groupLeader
endif
; cleanup is implemented now
if n_elements(cleanup) then begin
   widget_control, id, KILL_NOTIFY=Cleanup
endif

if keyword_set(noBlock) then begin
   widget_control, /XMANAGER_ACTIVE_COMMAND, id
endif else begin
   tmp = widget_event(/XMANAGER_BLOCK) ; will block until TLB widget is closed
endelse

end

