;
; various basics tests on OBJ_ISA()
; written by Alain, October 2016
; under GNU GPL v2 or any later
;
; Collecting examples from Greg Jung
; https://sourceforge.net/p/gnudatalanguage/bugs/696/
;
; -----------------------------------------------
; this procedure adds running "nb_errors" into "total_errors"
; then reset "nb_errors" to 0 for next block of tests.
;
pro INCREMENT_ERRORS, total_errors, nb_errors, verbose=verbose
if KEYWORD_SET(verbose) then print, total_errors, nb_errors
total_errors=total_errors+nb_errors
nb_errors=0
end
;
;  -----------------------------------------------
;
pro TEST_OBJ_ISA_FOR_LIST, external_errors, verbose=verbose
;
if KEYWORD_SET(verbose) then verb=1 else verb=0
;
nb_errors=0
accumul_errors=0
;
txt=' testing OBJ_ISA for LIST'
if (verb) then MESSAGE, /continue, 'Starting '+txt
;
k = LIST()  ; "K" in Greg
l = LIST(INDGEN(4)) ; "L" in Greg
;
if OBJ_ISA(k,'list') NE 1 then nb_errors++
if OBJ_ISA(l,'list') NE 1 then nb_errors++
;
if ARRAY_EQUAL(OBJ_ISA([l,l,l], 'list'),[1,1,1]) NE 1 then nb_errors++
;
INCREMENT_ERRORS, external_errors, nb_errors
;
end
;
pro TEST_OBJ_ISA_FOR_OBJARR, external_errors, verbose=verbose
;
if KEYWORD_SET(verbose) then verb=1 else verb=0
;
nb_errors=0
accumul_errors=0
;
txt=' testing OBJ_ISA for OBJARR'
if (verb) then MESSAGE, /continue, 'Starting '+txt
;
k = LIST()  ; "K" in Greg
l = LIST(INDGEN(4)) ; "L" in Greg
;
ii=OBJARR(3)
;
if ARRAY_EQUAL(OBJ_ISA(ii,'list'),[0,0,0]) NE 1 then nb_errors++
;
ii[0]=l
ii[1]=k
if ARRAY_EQUAL(OBJ_ISA(ii,'list'),[1,1,0]) NE 1 then nb_errors++
;
INCREMENT_ERRORS, external_errors, nb_errors
;
end
;
; -----------------------------------------------
;
pro TEST_OBJ_ISA, help=help, verbose=verbose, no_exit=no_exit, test=test
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_OBJ_ISA, help=help, verbose=verbose, $'
    print, '                  no_exit=no_exit, test=test'
    return
endif
;
total_errors=0
nb_errors=0
;
if KEYWORD_SET(verbose) then verb=1 else verb=0
;
TEST_OBJ_ISA_FOR_LIST, nb_errors, verbose=verbose
if nb_errors gt 0 then MESSAGE, /cont, 'Errors in Testing LIST'
INCREMENT_ERRORS, total_errors, nb_errors, verbose=verbose
;
TEST_OBJ_ISA_FOR_OBJARR, nb_errors, verbose=verbose
if nb_errors gt 0 then MESSAGE, /cont, 'Errors in Testing OBJARR'
INCREMENT_ERRORS, total_errors, nb_errors, verbose=verbose
;
; final message
;
BANNER_FOR_TESTSUITE, 'TEST_OBJ_ISA', total_errors, short=short
;
if KEYWORD_SET(test) then STOP
;
if (total_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
end

