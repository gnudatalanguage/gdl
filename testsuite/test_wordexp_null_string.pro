;
; This test exists because once upon a time WordExp()
; crashed when the input is a string without elements
;
; WordExp() crashed when called with a null string ...
; as a result, openr was broken ...
; Authored by: alaingdl 2018-04-25
; https://sourceforge.net/p/gnudatalanguage/svn/4121/
;
; Sylwerter wrote this test
; https://github.com/gnudatalanguage/gdl/pull/273
;
; ------------------------
;
; Modifications history :
; - 2024-04-15 AC : cleaning (details ...)
;    I hope to merge that into TEST_WORDEXP
; 
;
pro TEST_WORDEXP_NULL_STRING, help=help, test=test, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_WORDEXP_NULL_STRING, no_exit=no_exit, $'
    print, '                              help=help, test=test'
    return
endif
;
nb_errors=0
;
; The test it-self !
;
OPENR, unit, '', ERROR=error,/get_lun
;
if (error GE 0) then ERRORS_ADD, nb_errors, 'This ERROR should be negative'
;
; unit should not be defined ...
if ISA(unit) then ERRORS_ADD, nb_errors, 'This UNIT should not be define'
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_WORDEXP_NULL_STRING', nb_errors
;
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
