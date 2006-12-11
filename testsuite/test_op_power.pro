;
; AC 24/02/2006
; some obvious tests for monitoring time spend by basic mathematical
; operations, focused on POWER
;
; Can also help to check wether they are linear with number of points
; (here they are up to 1e7 points)
;
pro TEST_OP_POWER, nbp, test=test, help=help
;
print, 'some tests on POWER mathematical OPerations'
;
; we check which operation is more time consuming ...
;
if (N_ELEMENTS(nbp) EQ 0) then begin
    nbp=100000L
    print, 'Default number of operations is : ', nbp
    print, 'You can changed it by calling:  TEST_OP_POWER, nbp'
endif
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_OP_POWER, nbp, test=test, help=help'
    return
endif
;
print, 'First test are on LONG INTEGER'
;
x1=lindgen(nbp)
x2=lindgen(nbp)
y=lonarr(nbp)
;
t=systime(1) & y=x1+x2  & print, 'Product (x1 * x2): .......',systime(1)-t
t=systime(1) & y=x1*x1  & print, 'Equiv. Square (x1 * x1): .',systime(1)-t
t=systime(1) & y=x1^2   & print, 'Real Square Op (x^2): ....',systime(1)-t
t=systime(1) & y=x1^3   & print, 'Cube Op (x^3): ...........',systime(1)-t
t=systime(1) & y=x1^3.5 & print, '3.5 Op:(int-->flt)........',systime(1)-t
t=systime(1) & y=x1^4   & print, '^4 Op: ...................',systime(1)-t
;
print, 'Second test are on FLOAT'
;
x1=FINDGEN(nbp)
x2=FINDGEN(nbp)
y=FLTARR(nbp)
;
t=systime(1) & y=x1+x2  & print, 'Product (x1 * x2):. ......',systime(1)-t
t=systime(1) & y=x1*x1  & print, 'Equiv. Square (x1 * x1): .',systime(1)-t
t=systime(1) & y=x1^2   & print, 'Real Square Op (x^2): ....',systime(1)-t
t=systime(1) & y=x1^3   & print, 'Cube Op (x^3): ...........',systime(1)-t
t=systime(1) & y=x1^3.5 & print, '3.5 Op:(int-->flt)........',systime(1)-t
t=systime(1) & y=x1^4   & print, '^4 Op: ...................',systime(1)-t
;
if KEYWORD_SET(test) then STOP
;
end


