;
; initiale version by Sylvester, 30 March 2011
;
; AC : adding example from bug report 
; https://sourceforge.net/p/gnudatalanguage/bugs/714/
;
; ---------------------------------
; 
; Modifications history :
;
; - 2018-07-30 : AC. bad normalization discovered during test of Nika2 pipeline
;
; -----------------------------------
;
; Four cases should be tested :
; CORRELATE(x,y)
; CORRELATE(x, y, /covariance)
; CORRELATE(TRANSPOSE([[x],[y]]))
; CORRELATE(TRANSPOSE([[x],[y]]),/covariance)
;
function ARRAY_EQUAL_ROUND, arr1, arr2, factor
return, ARRAY_EQUAL(ROUND(arr1*factor),ROUND(arr2*factor))
end
;
pro TEST_CORRELATE_COYOTE, cumul_errors, translate=translate, $
                           verbose=verbose, test=test
;
nb_errors=0
;
x = [2.5, 0.5, 2.2, 1.9, 3.1, 2.3, 2.0, 1.0, 1.5, 1.1]
y = [2.4, 0.7, 2.9, 2.2, 3.0, 2.7, 1.6, 1.1, 1.6, 0.9]
;
; need to round the computations to be able to compare num. arrays
factor=1e4
;
; the computation should be invariable by translation !
;
if KEYWORD_SET(translate) then begin
   if translate EQ 1 then begin
      txt='Removing MEAN() values'
      x = x - MEAN(x, /DOUBLE)
      y = y - MEAN(y, /DOUBLE)
   endif
   if translate EQ 2 then begin
      txt='Removing Constant values'
      x = x - 1e4
      y = y - 1e4
      factor=1.e3
   endif
endif else begin
   txt='Raw data (no Translation)'
endelse
;
expected1=0.92592931
expected2=0.61544442
expected3=[[1.,expected1],[expected1,1.]]
expected4=[[0.61655552, 0.61544445], $
          [0.61544445, 0.71655560]]
;
res1=CORRELATE(x, y)
res2=CORRELATE(x, y, /COVARIANCE)
;
dataAdjust = TRANSPOSE([[x],[y]])
res3=CORRELATE(dataAdjust)
res4=CORRELATE(dataAdjust, /COVARIANCE, /DOUBLE)
;
if ~ARRAY_EQUAL_ROUND(expected1, res1, factor) then $
   ERRORS_ADD, nb_errors, '1 Bad Cov Matrix !'
if ~ARRAY_EQUAL_ROUND(expected2, res2, factor) then $
   ERRORS_ADD, nb_errors, '2 Bad Cov Matrix !'
if ~ARRAY_EQUAL_ROUND(expected3, res3, factor) then $
   ERRORS_ADD, nb_errors, '3 Bad Cov Matrix !'
if ~ARRAY_EQUAL_ROUND(expected4, res4, factor) then $
   ERRORS_ADD, nb_errors, '4 Bad Cov Matrix !'
;
if KEYWORD_SET(verbose) then begin
    print, 'Translate mode ? : '+txt
    print, 'expected :', expected4
    print, 'computed :', FLOAT(res4)
 endif
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_CORRELATE_COYOTE', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; -----------------------------------
;
pro TEST_CORRELATE_BASIC, cumul_errors, verbose=verbose, test=test
;
errors=0
;
; vectors strictly anti-corelated
if CORRELATE([0,1,2], [2,1,0]) ne -1 then begin
   ERRORS_ADD, '(1) Failed on anti-corelated test', errors 
endif
;  
; correlation of two constant vectors -> nan
if ~FINITE(CORRELATE([0,0,0], [1,1,1]), /nan) then begin
   ERRORS_ADD, '(2) Failed on constant vectors (NaN expected)', errors 
endif

; covariance of two constant vectors -> 0
if CORRELATE([0,0,0], [1,1,1], /cov) ne 0. then begin
   ERRORS_ADD, '(2bis) Failed on Covariance on constant vectors', errors
endif
;
; correlation of two vectors of different length
;
a = [1,2,3]
b = [2,3,4,5,6]
if CORRELATE(a,b) ne CORRELATE(b,a) then begin
   ERRORS_ADD, '(3a) two vectors of different length', errors 
endif
if CORRELATE(a,b) ne CORRELATE(a,b[0:2]) then begin
   ERRORS_ADD, '(3b) two vectors of different length', errors 
endif
;
; two-dimensional case 
; (AC 2018/07/30 : I do not remember what I wanted to test with "n" !)
;
n = !VALUES.F_NAN
if ~ARRAY_EQUAL(FINITE(CORRELATE([[1,-1],[1,2]])), [[0,0], [0, 1]]) then begin
   ERRORS_ADD, '(5a) Cov', errors
endif
;
if ~ARRAY_EQUAL(CORRELATE([[1,-1],[1,2]], /cov), [[0,0], [0, 4.5]]) then begin
   ERRORS_ADD, '(5b) Cov', errors 
endif
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_CORRELATE_BASIC', errors, /short
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_set(test) then STOP
;
end
;
; -----------------------------------
;
pro TEST_CORRELATE, no_exit=no_exit, help=help, verbose=verbose, test=test
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_CORRELATE, no_exit=no_exit, help=help, verbose=verbose, test=test'
   return
endif
;
cumul_errors=0
;
TEST_CORRELATE_BASIC, cumul_errors, verbose=verbose, test=test
;
TEST_CORRELATE_COYOTE, cumul_errors, verbose=verbose, test=test
TEST_CORRELATE_COYOTE, cumul_errors, translate=1, verbose=verbose, test=test
TEST_CORRELATE_COYOTE, cumul_errors, translate=2, verbose=verbose, test=test
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_CORRELATE', cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end


