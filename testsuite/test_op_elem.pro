;
; AC 20/02/2006
; some obvious tests for monitoring time spend by basic mathematical
; operations (add, mult, ^2, SQRT)
;
; Can help to check wether they are linear with number of points
; (here they are up to 1e7 points)
;
pro TEST_OP_ELEM, nbp, test=test, help=help
;
print, 'some tests on ELEMentary mathematical OPerations'
;
; we check which operation is more time consuming ...
;
if (N_ELEMENTS(nbp) EQ 0) then begin
    nbp=100000L
    print, 'Default number of operations is : ', nbp
    print, 'You can changed it by calling:  TEST_OP_ELEM, nbp'
endif
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_OP_ELEM, nbp, test=test, help=help'
    return
endif
;
print, 'First test are on LONG INTEGER'
;
x1=lindgen(nbp)
x2=lindgen(nbp)
y=lonarr(nbp)
;
t=systime(1) & y=x1+x2   & print, 'Addition: .',systime(1)-t
t=systime(1) & y=x1*x2   & print, 'product: ..',systime(1)-t
t=systime(1) & y=x1^2     & print, 'Square: ...',systime(1)-t
t=systime(1) & y=SQRT(x1) & print, 'Sqrt: .....',systime(1)-t
;
print, 'Second test are on FLOAT'
;
x1=FINDGEN(nbp)
x2=FINDGEN(nbp)
y=FLTARR(nbp)
;
t=systime(1) & y=x1+x2   & print, 'Addition: .',systime(1)-t
t=systime(1) & y=x1*x2   & print, 'product: ..',systime(1)-t
t=systime(1) & y=x1^2     & print, 'Square: ...',systime(1)-t
t=systime(1) & y=SQRT(x1) & print, 'Sqrt: .....',systime(1)-t
;
if KEYWORD_SET(test) then STOP
;
end


