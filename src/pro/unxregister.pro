;+
; NAME:   XUNREGISTER
;
; PURPOSE:  Silently Remove (unregister) a widget in the list of
; wmanaged widgets (if any)
;
; CATEGORY: GUI, widgets
;
; CALLING SEQUENCE:
;
; XUNREGISTER, ID
;
; inputs:
;
; OPTIONAL INPUTS:  none
;
; KEYWORD PARAMETERS:
;
; COMMON BLOCKS:    managed_by_gdl
;
; SIDE EFFECTS:     none
;
; RESTRICTIONS:     incomplete?
;
; MODIFICATION HISTORY:
;  - 05/02/2015: inital verison 
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

pro UNXREGISTER, id
COMPILE_OPT idl2, HIDDEN  
common managed_by_gdl, ids, names

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

