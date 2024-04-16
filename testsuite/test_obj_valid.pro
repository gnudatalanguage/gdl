;
; Unknown origin ...
;
; Modifications history :
; -2024-04-15 AC : cleaning (details ...)
;
; -------------------------------------------------

pro TEST_OBJ_VALID, help=help, verbose=verbose, test=test, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
   print, 'TEST_OBJ_VALID, help=help, verbose=verbose, $'
   print, '                test=test, no_exit=no_exit'
   return
endif
;
errors=0
;
llist = LIST(FLTARR(4),"hello",2.)
mlist = LIST("!pi", "goodbye",FINDGEN(3,4),COMPLEX(0))
;
if ~OBJ_VALID(llist) then ERRORS_ADD, errors, 'Not an valid OBJ (1)'
if ~OBJ_VALID(mlist) then ERRORS_ADD, errors, 'Not an valid OBJ (2)'
;
; Particular case for ptr_valid and
; comparison with !NULL that should be the inverse
;
good=[1b,0b]
p=OBJ_NEW('idl_container')
res=[OBJ_VALID(p), p eq !NULL]
if ~ARRAY_EQUAL(res,good, /no_typeconv) then $
   ERRORS_ADD, errors, 'Problem 1 : OBJ_NEW & !null'
;
good=[0b,1b]
OBJ_DESTROY, p
res=[OBJ_VALID(p),!NULL eq p]
if ~ARRAY_EQUAL(res,good,/no_typeconv) then $
   ERRORS_ADD, errors, 'Problem 2 : OBJ_DESTROY & !null'
;
; to prepare any further/future evolution, if any 
;
cumul_errors=errors
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_OBJ_VALID', cumul_errors, short=short
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
