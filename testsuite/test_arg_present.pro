;
; under GNU GPL v3
; Alain Coulais, 17 july 2017
;
; remember that ARG_PRESENT() can return TRUE (1)
; if and only if the key can be modified outside
; (If the value is fixed, will return FALSE)
;
; -----------------------------------------------------------------
;
pro MYPROC, value, key=key
;
if ARG_PRESENT(key) then begin
   value=1
endif else begin
   value=0
endelse
;
end
;
; -----------------------------------------------------------------
;
function MYFUNCTION, key=key
;
if ARG_PRESENT(key) then begin
   return, 1
endif else begin
   return, 0
endelse
;
end
;
; -----------------------------------------------------------------
;
pro TEST_ARG_PRESENT, help=help, verbose=verbose, no_exit=no_exit, test=test
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_ARG_PRESENT, help=help, verbose=verbose, $'
   print, '                      no_exit=no_exit, test=test'
   return
endif
;
errors=0
value=-1
key_can_change=123
;
; Testing for PROCEDURE the 4 cases (0,0,0,1)
;
MYPROC, value
if (value NE 0) then ADD_ERROR, errors, 'first PRO case'
;
MYPROC, value, /key
if (value NE 0) then ADD_ERROR, errors, 'second PRO case'
;
MYPROC, value, key=123
if (value NE 0) then ADD_ERROR, errors, 'third PRO case'

MYPROC, value, key=key_can_change
if (value NE 1) then ADD_ERROR, errors, 'fourth PRO case (the ONE !)'
;
; Now testing for FUNCTION the 4 cases (0,0,0,1)
;
value=MYFUNCTION()
if (value NE 0) then ADD_ERROR, errors, 'first FUN case'
;
value=MYFUNCTION(/key)
if (value NE 0) then ADD_ERROR, errors, 'second FUN case'
;
value=MYFUNCTION( key=123)
if (value NE 0) then ADD_ERROR, errors, 'third FUN case'
;
value=MYFUNCTION(key=key_can_change)
if (value NE 1) then ADD_ERROR, errors, 'fourth FUN case (the ONE !)'
;
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_ARG_PRESENT', errors
; 
if (errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
;
