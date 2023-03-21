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

compile_opt hidden, idl2

ON_ERROR, 2

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

pro XUNREGISTER, id

compile_opt hidden, idl2

ON_ERROR, 2

common managed, ids, names, modalList

if (n_elements(id) eq 0) then return
if (n_elements(ids) eq 0) then return
if (ids[0] eq 0) then return
occurences=where(ids eq id, count, complement=complement, ncomplement=ncomp)
if (count le 0) then return
if ( ncomp gt 0 ) then begin ; there are others
 names=names[complement]
 ids=ids[complement]
 return
endif else begin ; there are no others, clear lists
 names = 0
 ids = 0
endelse
end


pro XMANAGER, name, id, NO_BLOCK = noBlock, GROUP_LEADER=groupLeader, EVENT_HANDLER=eventHandler, $
   CLEANUP=Cleanup, JUST_REG=just_reg, CATCH=catch, MODAL=modal, BACKGROUND=background

  compile_opt hidden, idl2
  
  ON_ERROR, 2

  common managed, ids, names, modalList

  common gdl_xmanager_private,docatch,nx
  if n_elements(nx) eq 0 then nx=0
  if n_elements(docatch) eq 0 then docatch=1 ; catch by default
  nx++;
  if (n_elements(catch) ne 0) THEN BEGIN
    docatch = keyword_set(catch)
    message, /INFO, 'Error handling is now ' + (['off', 'on'])[docatch]
    return
  ENDIF

  ;; Debug: uncomment the ;; below
  ;; print,"**************************************Entering Xmanager #"+strtrim(nx,2)+"**************************************"

  if keyword_set(modal) then message,/informational,"The MODAL keyword to the XMANAGER procedure is obsolete."+$
     " It is superseded by the MODAL keyword to the WIDGET_BASE function. This will *not* work. Please modify your code."
  if keyword_set(background) then message,/informational,"The BACKGROUND keyword to the XMANAGER procedure is obsolete."+$
     " It is superseded by the TIMER keyword to the WIDGET_CONTROL procedure. Please modify your code."

  ValidateManagedWidgets

  if (n_params() eq 0) then begin
     if ~ids[0] then message,/informational, 'No widgets are currently being managed.'
     return
  endif else if (n_params() ne 2) then message, 'Wrong number of arguments, usage: XMANAGER [, name, id]'

  if (n_elements(just_reg) eq 0) then just_reg = 0

  if n_params() eq 2 then begin
     if not keyword_set(eventHandler) then begin
        eventHandler = name + '_event'
     endif
;if id is not the top base, get the top base:
     while (widget_info(id,/parent) ne 0) do id=widget_info(id,/parent)
     
; add to common
     if (ids[0] ne 0) then begin
        ids = [ids, id]
        names = [names, name]
     endif else begin
        ids = id
        names = name
     endelse
  AlreadyBlocked = widget_info(/XMANAGER_BLOCK)
  ;; print,'before widget registration, AlreadyBlocked is=',AlreadyBlocked

; We can now define the widget as managed 
     widget_control, id, /managed
; define handler
     widget_control, id, event_pro=eventHandler
; define group leader     
     if keyword_set(groupLeader) then begin
        widget_control, id, GROUP_LEADER=groupLeader
     endif
; cleanup is implemented now
     if n_elements(cleanup) then begin
        widget_control, id, KILL_NOTIFY=Cleanup
     endif
; return if just registering (use of registering only still not clear for me -- probably a relic from the past)
     if (just_reg) then return
     
; XMANAGER must block, i.e. call widget_event until the widget that blocked is destroyed.
; besides, ony one blocking widget is permitted, so if xmanager is blocking, a subsequent call to xmanager must not block.

; all TopWidgets are blocking by default in GDL (makes no difference as long as XMANAGER is not called).
; of course we consider only managed widgets.
; if there was a blocking widget before registering this one, or if noBlocks is asked for, we 'unBlock' this widget:


     if AlreadyBlocked or keyword_set(noBlock) then begin
        widget_control, /XMANAGER_ACTIVE_COMMAND, id ; mark as non-blocking
        return                                       ; passthrough
     endif
     
; eventloop
; are we blocked now, with the newly registered widget?     
  Blocked = widget_info(/XMANAGER_BLOCK)
  ;; print,'after last tweaks, active is=',Blocked
  WHILE (Blocked NE 0) DO BEGIN
     error = 0
     IF (docatch) then catch, error
     IF (error EQ 0) THEN BEGIN
        unused = widget_event(/XMANAGER_BLOCK)
     endif else begin
        Message,/informational, !ERROR_STATE.MSG_PREFIX+'Caught unexpected error from client application. Message follows...'
        help,/last_message
     endelse
    IF (docatch) THEN catch, /cancel
    Blocked = widget_info(/XMANAGER_BLOCK)
    ;; print,'active=',Blocked
  ENDWHILE

 ; end of eventloop, cleanup       
     ValidateManagedWidgets
  endif
  ;; print,"**************************************Exiting Xmanager #"+strtrim(nx,2)+"**************************************"
end

