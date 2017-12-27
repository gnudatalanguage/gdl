;
; various basic tests on FILE_MOVE()
; written by G. Duvert July 2017
; under GNU GPL v2 or any later
;
; -----------------------------------------------
;
pro TEST_SIMPLEX, test=test, no_exit=no_exit, help=help
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_SIMPLEX [, /test] [, /no_exit] [, /help]'
   return
endif
;
;Errors count is 0 at the beginning
total_errors = 0
;
Zequation = [1,1,3,-0.5]
Constraints = [[740, -1, 0, -2, 0],[ 0, 0, -2, 0, 7],[0.5, 0, -1, 1, -2],[ 9, -1, -1, -1, -1] ]
m1=2
m2=1 
m3=1
exist=execute("result = SIMPLEX(Zequation, Constraints, m1, m2, m3,toto,a,b,/doub)")

if exist eq 0 then return ; SIMPLEX does not exist, this is not a test!"

; toto, a and b are currently *not computed*
template=[17.025d, 0.00, 3.325, 4.725, 0.95]
if (total(template-result gt 1d-6)) then total_errors++
;
; final message
;
BANNER_FOR_TESTSUITE, 'TEST_SIMPLEX', total_errors
;
if KEYWORD_SET(test) then STOP
;
if (total_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
end
