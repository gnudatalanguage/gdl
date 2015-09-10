;
; Alain C., waiting at LAX, Sept 2015
;
; We need to check whether the issued messages at sub-levels
; correspond to the expected ones.
;
pro MYDEBUG, txt, res, expected, result
;
print, 'testing LEVEL : ', txt
print, ' Errors ??    : ', 1-res
print, 'Expected      : ', expected
print, 'Results       : ', result
;
end
; --------------------------------------------------
;
pro LEVEL4, errors, test=test, debug=debug
;
txt='LEVEL4'
;
if KEYWORD_SET(debug) then print, 'entering '+txt
;
if ~ISA(errors) then errors=0
;
expected=['$MAIN$','LEVEL4','LEVEL3']
expected=expected+': '+txt
;
result=STRARR(3)
;
MESSAGE, /continue, txt, level=1
result[0]=!ERROR_STATE.msg
MESSAGE, /continue, txt, level=0
result[1]=!ERROR_STATE.msg
MESSAGE, /continue, txt, level=-1
result[2]=!ERROR_STATE.msg
;
res=ARRAY_EQUAL(expected, result)
;
if (res EQ 0) then begin
   BANNER_FOR_TESTSUITE, txt, 1, /SHORT
   errors++
endif
;
if KEYWORD_SET(debug) then MYDEBUG, txt, res, expected, result
;
if KEYWORD_SET(test) then STOP
;
end
;
; --------------------------------------------------
;
pro LEVEL3, errors, test=test, debug=debug
;
txt='LEVEL3'
;
if KEYWORD_SET(debug) then print, 'entering '+txt
;
if ~ISA(errors) then errors=0
;
expected=['$MAIN$','LEVEL3','LEVEL2','LEVEL1']
expected=expected+': '+txt
;
result=STRARR(4)
;
MESSAGE, /continue, txt, level=1
result[0]=!ERROR_STATE.msg
MESSAGE, /continue, txt, level=0
result[1]=!ERROR_STATE.msg
MESSAGE, /continue, txt, level=-1
result[2]=!ERROR_STATE.msg
MESSAGE, /continue, txt, level=-2
result[3]=!ERROR_STATE.msg
;
res=ARRAY_EQUAL(expected, result)
;
if (res EQ 0) then begin
   BANNER_FOR_TESTSUITE, txt, 1, /SHORT
   errors++
endif
;
if KEYWORD_SET(debug) then MYDEBUG, txt, res, expected, result
;
if KEYWORD_SET(test) then STOP
;
LEVEL4, errors, test=test, debug=debug
;
if KEYWORD_SET(test) then STOP
;
end
;
; --------------------------------------------------
;
pro LEVEL2, errors, test=test, debug=debug
;
txt='LEVEL2'
;
if KEYWORD_SET(debug) then print, 'entering '+txt
;
if ~ISA(errors) then errors=0
;
expected=['$MAIN$','LEVEL2','LEVEL1','TEST_MESSAGE']
expected=expected+': '+txt
;
result=STRARR(4)
;
message, /continue, txt, level=1
result[0]=!ERROR_STATE.msg
message, /continue, txt, level=0
result[1]=!ERROR_STATE.msg
message, /continue, txt, level=-1
result[2]=!ERROR_STATE.msg
message, /continue, txt, level=-2
result[3]=!ERROR_STATE.msg
;
res=ARRAY_EQUAL(expected, result)
;
if (res EQ 0) then begin
   BANNER_FOR_TESTSUITE, txt, 1, /SHORT
   errors++
endif
;
if KEYWORD_SET(debug) then MYDEBUG, txt, res, expected, result
;
if KEYWORD_SET(test) then STOP
;
LEVEL3, errors, test=test, debug=debug
;
end
;
; --------------------------------------------------
;
pro LEVEL1, errors, test=test, debug=debug
;
txt='LEVEL1'
;
if KEYWORD_SET(debug) then print, 'entering '+txt
;
if ~ISA(errors) then errors=0
;
expected=['$MAIN$','LEVEL1','TEST_MESSAGE','$MAIN$']
expected=expected+': '+txt
;
result=STRARR(4)
;
MESSAGE, /continue, txt, level=1
result[0]=!ERROR_STATE.msg
MESSAGE, /continue, txt, level=0
result[1]=!ERROR_STATE.msg
MESSAGE, /continue, txt, level=-1
result[2]=!ERROR_STATE.msg
MESSAGE, /continue, txt, level=-2
result[3]=!ERROR_STATE.msg
;
res=ARRAY_EQUAL(expected, result)
;
if (res EQ 0) then begin
   BANNER_FOR_TESTSUITE, txt, 1, /SHORT
   errors=errors++
endif
;
if KEYWORD_SET(debug) then MYDEBUG, txt, res, expected, result
;
if KEYWORD_SET(test) then STOP
;
LEVEL2, errors, test=test, debug=debug
;
if KEYWORD_SET(test) then STOP
;
end
;
; --------------------------------------------------
;
pro TEST_MESSAGE, help=help, test=test, debug=debug, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_MESSAGE, help=help, test=test, debug=debug, no_exit=no_exit'
   return
endif 
;
errors=0
;
LEVEL1, errors, test=test, debug=debug
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_MESSAGE', errors
;
if (errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end


