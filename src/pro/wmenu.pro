;+
; NAME:     WMENU
;
; PURPOSE: This a emulation of the famous X11 WMENU (obsolete routine).
;
; Licence: GNU/GPL (v2 or later)
;
; Contain: WMENU only
;
; CATEGORY: user interface
;
; CALLING SEQUENCE:  answer_index=WMENU(list, title=, init=)
;
; INPUTS:       a list (string or float or integer)
;
; OPTIONAL INPUTS:   none
;
; KEYWORD PARAMETERS:
;        title= index number of title (usually =0)
;        init=  index number of default (usually > 0)
;
; Extended Keywords: 
;        help= display a short help and abort
;        test= will stop inside the function
;
; OUTPUTS:
;        the index of the selected answer in the input list
;
; OPTIONAL OUTPUTS:   none
;
; COMMON BLOCKS:      none
;
; SIDE EFFECTS:       none
;
; RESTRICTIONS:       obsoleted in IDL 4.0 or before 
;
; EXAMPLE:            see the test procedure TEST_WMENU
;
; Behavior: 
;
; let NbReps the number of Elements in list
; if title < 0 or title > NbReps, no title managed, any answer OK, rep from 0 to NbReps-1
; if init < 0 or init > NbReps, no init managed, any answer OK
; if title in {0..NbReps-1} then this filed is moved ontop and is not selectable
; Must be noticed too that possible index answer is from 0 to NbReps-2
; if init = title then no init
;
; MODIFICATION HISTORY:
;  - 25-JAN-2006 : created by Alain Coulais
;  - 09-FEB-2006 : various debugging (WHERE does not work with STRINGs!)
;  - 13-FEB-2006 : title is a index, not a string !
;  - 19-NOV-2012 : Zenity-based version, pushed in the public CVS
;  - 04-FEB-2021 : GD: Rewrote using widgets. Forget Zenity.
;-
; LICENCE:
; Copyright (C) 2006-2021, Alain Coulais, Gilles Duvert
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.
;-
;

function WMENU, name, title=tt, init=init, group=group

  compile_opt idl2, hidden
  ON_ERROR, 2
  if (N_PARAMS() ne 1) then message,"Incorrect number of arguments."
  if ~( Isa(name,/STRING) ) then begin
   text=scope_varname(name,level=-1)
   if (strlen(text) eq 0) then help,out=text,name
   message,"String expression required in this context: "+text
endif 
if (N_ELEMENTS(name) LE 1) then begin
   text=scope_varname(name,level=-1)
   if (strlen(text) eq 0) then help,out=text,name
   message,"Expression must be an array in this context: "+text
endif

; test capability
flag = (fix(!D.flags /2.^8 MOD 2) ) ; Set if a window device
if (~flag) then  message,"Routine is not defined for current graphics device."

refpos=0
IF N_ELEMENTS(tt) EQ 0 THEN begin
   title='Choose...' ; courtesy of GDL.
   refpos=N_ELEMENTS(name)+1    ; no refpos
   list=name
endif else begin
   title= name[tt]
   list = name[where(name NE title)]
   refpos=tt
endelse


IF N_ELEMENTS(init) EQ 0 THEN init = -1
  n=n_elements(list)
  a=widget_base(/col,group=group)
  l=widget_label(a,value=title,/align_center)
  widget_control,a,/real
; to have item 0 unselected, b must be created AFTER a has been
; realized. This seems to be a problem (event emitted or not) with wxWidgets.
  b=widget_list(a, value=list, ysize=(n<30))
  widget_control,b,set_list_select=init
z=widget_event(b)
val=z.index
if val ge refpos then val++ ;title was removed from name
widget_control,a,/dest
return, val
;
end
