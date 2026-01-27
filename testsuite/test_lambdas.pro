;
; under GNU GPL v3
PRO test_lambdas,no_exit=no_exit,test=test
  compile_opt idl2 ; necessary for lambda function to work
  errors=0
  L=lambda("x: x^2")
  a=3.0
  z=l(a)
  if z ne a^2 then ERRORS_ADD, errors, 'LAMBDA'
; this just to exert the function
  s="ZZZ"
  q=lambdap("arg: STRPUT,arg,'GDL',0")
  q,s
  if s ne 'GDL' then ERRORS_ADD, errors, 'LAMBDAP'
; tihs to check a new compilation is not needed if lambda is the same
  oldq=q
  q=lambdap("arg: STRPUT,arg,'GDL',0")
  if q ne oldq then ERRORS_ADD, errors, 'LAMBDAP: duplicate LAMBDA error'
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_LAMBDAS', errors
; 
if (errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
;


END
