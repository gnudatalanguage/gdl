;
; various basic tests on SIMPLEX
;
; written by G. Duvert July 2017
; under GNU GPL v2 or any later
;
; ---------------------------------
; 
; Modifications history :
;
; - 2020-Mars-31 : AC.
;   * better way to locate the problems I have
;   * adding a test based on GLPK doc.        
;
; -----------------------------------------------
;
pro TEST_SIMPLEX_IDL, cumul_errors, test=test, verbose=verbose
;
errors=0
Zequation = [1,1,3,-0.5]
Constraints = [[740, -1,  0, -2,  0], $
               [  0,  0, -2,  0,  7], $
               [0.5,  0, -1,  1, -2], $
               [  9, -1, -1, -1, -1] ]
m1=2
m2=1
m3=1
;
result = SIMPLEX(Zequation, Constraints, m1, m2, m3, toto, a, b, /double)
; (in fact, toto, a and b are currently *not computed*)
;
expected=[17.025d, 0.00, 3.325, 4.725, 0.95]
if (TOTAL(expected-result) GT 1d-6) then ERRORS_ADD, errors, 'TEST_SIMPLEX_IDL'
;
; --------------
;
BANNER_FOR_TESTSUITE, 'TEST_SIMPLEX_IDL', errors, /status, verbose=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; -----------------------------------------------
; this test comes from GLPK Version 4.55 documentation, p. 12
; http://www.chiark.greenend.org.uk/doc/glpk-doc/glpk.pdf
;
pro TEST_SIMPLEX_GLPK, cumul_errors, test=test, verbose=verbose
;
errors=0
Zequation = [10,6,4]
Constraints = [[100, -1,  -1, -1], $
               [600,-10, -4,  -5], $
               [300, -2, -2,  -6]]
m1=3
m2=0
m3=0
;
result=SIMPLEX(Zequation, Constraints, m1, m2, m3, toto, a, b, /double)
;
one_third=100.D/3
expected=[700+one_third, one_third, 2.*one_third, 0.]
if (TOTAL(expected-result) GT 1d-6) then ERRORS_ADD, errors, 'TEST_SIMPLEX_GLPK'
;
; --------------
;
BANNER_FOR_TESTSUITE, 'TEST_SIMPLEX_GLPK', errors, /status, verbose=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; -----------------------------------------------
;
pro TEST_SIMPLEX, test=test, no_exit=no_exit, verbose=verbose, help=help
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_SIMPLEX [, /test] [, /no_exit] [, /help] [, /verbose]'
   return
endif
;
FORWARD_FUNCTION GLPK_EXISTS
;
if (GDL_IDL_FL() EQ 'FL') then MESSAGE, /continue, 'This code don''t work on FL now'
;
DEFSYSV, '!gdl', exists=is_it_gdl
;
if (is_it_gdl) then begin 
   if ~GLPK_EXISTS() then begin
      MESSAGE, /continue, 'GDL was compiled without GLPK support'
      EXIT, status=77
   endif
endif
;
total_errors=0
;
TEST_SIMPLEX_IDL, total_errors, test=test, verbose=verbose
TEST_SIMPLEX_GLPK, total_errors, test=test, verbose=verbose
;
; ----------------- final MESSAGE ----------
;
BANNER_FOR_TESTSUITE, 'TEST_SIMPLEX', total_errors
;
if (total_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
if KEYWORD_SET(test) then stop
;
end
