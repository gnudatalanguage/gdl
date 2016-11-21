;
; initiale version by Sylvester, 30 March 2011
;
; AC : adding example from bug report 
; https://sourceforge.net/p/gnudatalanguage/bugs/714/
;
;
; -----------------------------------


pro TEST_CORRELATE_COYOTE, errors, verbose=verbose, test=test
;
if ~ISA(errors) then errors=0
;
expected=[[0.61655552, 0.61544445],$
          [0.61544445, 0.71655560]]
;
x = [2.5, 0.5, 2.2, 1.9, 3.1, 2.3, 2.0, 1.0, 1.5, 1.1]
y = [2.4, 0.7, 2.9, 2.2, 3.0, 2.7, 1.6, 1.1, 1.6, 0.9]
xmean = x - MEAN(x, /DOUBLE)
ymean = y - MEAN(y, /DOUBLE)
dataAdjust = TRANSPOSE([ [xmean], [ymean] ])
covMatrix = CORRELATE(dataAdjust, /COVARIANCE, /DOUBLE)
;
; need to round the computations to be able to compare num. arrays
factor=1e4
if ~ARRAY_EQUAL(ROUND(expected*factor),ROUND(covMatrix*factor)) then errors++
;
if KEYWORD_SET(verbose) then begin
    print, 'expected :', expected
    print, 'computed :', covMatrix
endif
;
if KEYWORD_SET(test) then STOP
;
end
;
; -----------------------------------
;
pro TEST_CORRELATE_BASIC, errors, verbose=verbose, test=test
;
if ~ISA(errors) then errors=0
err = 0
;
; vectors strictly anti-corelated
;
if CORRELATE([0,1,2], [2,1,0]) ne -1 then begin
    MESSAGE, '1', /conti
    errors++
endif
;  
; correlation of two constant vectors -> nan
if ~FINITE(CORRELATE([0,0,0], [1,1,1]), /nan) then begin
    MESSAGE, '2', /conti
    errors++
endif

; covariance of two constant vectors -> 0
if CORRELATE([0,0,0], [1,1,1], /cov) ne 0. then begin
    MESSAGE, '3', /conti
    errors++
endif
;
; correlation of two vectors of different length
;
a = [1,2,3]
b = [2,3,4,5,6]
if CORRELATE(a,b) ne CORRELATE(b,a) then begin
    MESSAGE, '4a', /conti
    errors++
endif
if CORRELATE(a,b) ne CORRELATE(a,b[0:2]) then begin
    MESSAGE, '4b', /conti
    errors++
endif
;
; two-dimensional case
;
n = !VALUES.F_NAN
if ~ARRAY_EQUAL(FINITE(CORRELATE([[1,-1],[1,2]])), [[0,0], [0, 1]]) then begin
    MESSAGE, '5', /conti
    errors++
endif
;
if ~ARRAY_EQUAL(CORRELATE([[1,-1],[1,2]], /cov), [[0,0], [0, 4.5]]) then begin
    MESSAGE, '5', /conti
    errors++
endif
;

if KEYWORD_SET(test) then STOP
;
end
;
; -----------------------------------
;
pro TEST_CORRELATE, help=help, verbose=verbose, debug=debug, test=test
;
TEST_CORRELATE_BASIC, errors1, verbose=verbose, test=test
BANNER_FOR_TESTSUITE, 'Testing TEST_CORRELATE_BASIC', errors1, /short
;
TEST_CORRELATE_COYOTE, errors2, verbose=verbose, test=test
BANNER_FOR_TESTSUITE, 'Testing TEST_CORRELATE_COYOTE', errors2, /short
;
errors=errors1+errors2
;
BANNER_FOR_TESTSUITE, 'TEST_CORRELATE', errors
;
if (errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end

