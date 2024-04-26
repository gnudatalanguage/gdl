;
; Unknown origin ...
;
; Modifications history :
; -2024-04-15 AC : cleaning (details ...)
;
; -------------------------------------------------
;
pro TEST_OBJ_HASMETHOD_INTERNAL, input, cumul_errors, debug=debug, $
                                 test=test, verbose=verbose
;
errors=0
;
if ~OBJ_VALID(input) then ERRORS_ADD, errors, 'Not an valid OBJ'
;
hasadd = OBJ_HASMETHOD(input,'ADD')
if ~hasadd then ERRORS_ADD, errors, 'Method ADD not available'
;
hasmove= OBJ_HASMETHOD(input,'MOVE')
if ~hasmove then ERRORS_ADD, errors, 'Method MOVE not available'
;
hasremove= OBJ_HASMETHOD(input,'REMOVE')
if ~hasremove then ERRORS_ADD, errors, 'Method REMOVE not available'
;
hastoarray=OBJ_HASMETHOD(input,'TOARRAY')
if ~hastoarray then ERRORS_ADD, errors, 'Method TOARRAY not available'
;
; --------------
;
BANNER_FOR_TESTSUITE, 'TEST_OBJ_HASMETHOD_INTERNAL', errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; ----------------------------
;
pro TEST_OBJ_HASMETHOD, help=help, verbose=verbose, test=test, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
   print, 'TEST_OBJ_HASMETHOD, help=help, verbose=verbose, $'
   print, '                    test=test, no_exit=no_exit'
   return
endif
;
cumul_errors=0
;
llist = LIST(FLTARR(4), "hello", 2.)
TEST_OBJ_HASMETHOD_INTERNAL, llist, cumul_errors

mlist = LIST(!pi, "goodbye", FINDGEN(3,4))
TEST_OBJ_HASMETHOD_INTERNAL, mlist, cumul_errors;
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_OBJ_HASMETHOD', cumul_errors, short=short
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end


