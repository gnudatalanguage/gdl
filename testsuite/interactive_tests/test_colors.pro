;
; AC le 16 Mai 2006
; Debugged with GDL team 13/10/2006
; Postscript tests are move to another program
;
pro TEST_COLORS, verbose=verbose, background=background
;
print, 'This program quickly checks basic colors in DEVICE, /decomposed mode'
;
; We switch to the good color enviromment
; http://fermi.jhuapl.edu/s1r/idl/s1rlib/idl_color/
;
DEVICE, get_decomposed=old_decomposed
if NOT(old_decomposed) then begin
   print, 'We switch to DEVICE, /decomposed'
   DEVICE, decompose=1
endif
;
; begin of the plots !
;
plot,[1,0], background=background
;
xxo=0.025
;
; somes basic colors
;
xx=0.1
oplot,[xx,xx],color='ff'x
xyouts, xx+xxo, xx+xxo, 'RED expected', color='ff'x
;
xx=0.2
oplot,[xx,xx],color='ff00'x
xyouts, xx+xxo, xx+xxo, 'GREEN expected', color='ff00'x
;
xx=0.3
oplot,[xx,xx],color='ff0000'x
xyouts, xx+xxo, xx+xxo, 'BLUE expected', color='ff0000'x
;
xx=0.4
oplot,[xx,xx],color='ffff00'x
xyouts, xx+xxo, xx+xxo, 'CYAN expected', color='ffff00'x
;
xx=0.5
oplot,[xx,xx],color='ff00ff'x
xyouts, xx+xxo, xx+xxo, 'MAGENTA expected', color='ff00ff'x
;
xx=0.6
oplot,[xx,xx],color='00ffff'x
xyouts, xx+xxo, xx+xxo, 'YELLOW expected', color='00ffff'x
;
; we switch back to previous Device mode
DEVICE, decomposed=old_decomposed
; we close the window we opened
car=''
read, 'press any case to finish', car
WDELETE
;
end
