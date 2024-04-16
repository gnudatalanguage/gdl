;
; various basics tests on OBJ_ISA()
; written by Alain, October 2016
; under GNU GPL v2 or any later
;
; Collecting examples from Greg Jung
; https://sourceforge.net/p/gnudatalanguage/bugs/696/
;
; Modifications history :
; -2024-04-15 AC : cleaning (details ...)
;
; -----------------------------------------------
;
pro TEST_OBJ_ISA_FOR_LIST, cumul_errors, verbose=verbose
;
if KEYWORD_SET(verbose) then verb=1 else verb=0
;
nb_errors=0
;
txt=' testing OBJ_ISA for LIST'
if (verb) then MESSAGE, /continue, 'Starting '+txt
;
k = LIST()  ; "K" in Greg
l = LIST(INDGEN(4)) ; "L" in Greg
;
if ~OBJ_ISA(k,'list') then ERRORS_ADD, nb_errors, 'list K'
if ~OBJ_ISA(l,'list') then ERRORS_ADD, nb_errors, 'list L'
;
if ~ARRAY_EQUAL(OBJ_ISA([l,l,l], 'list'),[1,1,1]) then $
   ERRORS_ADD, nb_errors, 'list [L,L,L] (dim enforced)'
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_OBJ_ISA_FOR_LIST', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_SET(test) then STOP
;
end
;
; -----------------------------------------------
;
pro TEST_OBJ_ISA_FOR_OBJARR, cumul_errors, verbose=verbose
;
if KEYWORD_SET(verbose) then verb=1 else verb=0
;
nb_errors=0
;
txt=' testing OBJ_ISA for OBJARR'
if (verb) then MESSAGE, /continue, 'Starting '+txt
;
k = LIST()  ; "K" in Greg
l = LIST(INDGEN(4)) ; "L" in Greg
;
ii=OBJARR(3)
;
if ~ARRAY_EQUAL(OBJ_ISA(ii,'list'),[0,0,0]) then $
   ERRORS_ADD, nb_errors, 'First case list L'
;
ii[0]=l
ii[1]=k
if ~ARRAY_EQUAL(OBJ_ISA(ii,'list'),[1,1,0]) then $
   ERRORS_ADD, nb_errors, 'Second case list L/K'
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_OBJ_ISA_FOR_OBJARR', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_SET(test) then STOP
;
end
;
; -----------------------------------------------
;
pro TEST_OBJ_ISA, help=help, verbose=verbose, test=test, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_OBJ_ISA, help=help, verbose=verbose, $'
    print, '                  no_exit=no_exit, test=test'
    return
endif
;
cumul_errors=0
;
TEST_OBJ_ISA_FOR_LIST, cumul_errors, verbose=verbose
;
TEST_OBJ_ISA_FOR_OBJARR, cumul_errors, verbose=verbose
;
; final message
;
BANNER_FOR_TESTSUITE, 'TEST_OBJ_ISA', cumul_errors, short=short
;
if KEYWORD_SET(test) then STOP
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
end
