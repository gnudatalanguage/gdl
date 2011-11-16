;
; submitted by G. Duvert, 14-Nov-2011
;
; This is a temporary workaround for this famous procedure.
; We saw no way to implement it now within PLplot.
; Help welcome
;
pro USERSYM, myx, myy, color=mycolor, fill=myfill, thick=mythick
;
COMMON common_for_usersym, usersym_warning_done
;
if N_ELEMENTS(usersym_warning_done) eq 0 then begin
   MESSAGE, /continue, "warning, USERSYM not yet implemented (fixme)"
   MESSAGE, /continue, "circles will be plotted instead."
   usersym_warning_done=0b
endif
return
end

