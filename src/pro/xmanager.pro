;+
; NAME:   XMANAGER
;
; PURPOSE:  Provides event management for GDL widgets.
;
; CATEGORY: GUI, widgets
;
; CALLING SEQUENCE:
;
; XMANAGER, NAME, ID, /NO_BLOCK, GROUP_LEADER=groupLeader
;
;
; INPUTS:
;
; OPTIONAL INPUTS:  none
;
; KEYWORD PARAMETERS:
;
; COMMON BLOCKS:    none
;
; SIDE EFFECTS:     none
;
; RESTRICTIONS:     incomplete, more functionality to be added.
;
;
; MODIFICATION HISTORY:
;  - 05/11/2013: inital verison 
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

pro XMANAGER, name, id, NO_BLOCK = noBlock, GROUP_LEADER=groupLeader, EVENT_HANDLER=eventHandler

if not keyword_set(eventHandler) then begin
  eventHandler = name + '_event'
endif
 
widget_control, id, event_pro=eventHandler


if keyword_set(groupLeader) then begin
   widget_control, id, GROUP_LEADER=groupLeader
endif

if keyword_set(noBlock) then begin

   widget_control, /XMANAGER_ACTIVE_COMMAND, id
endif else begin

   tmp = widget_event(/XMANAGER_BLOCK)
endelse

end

