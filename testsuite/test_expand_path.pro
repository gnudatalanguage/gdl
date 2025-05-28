;
; Alain Coulais, 18 April 2025. Under GNU GPL v2+
;
; preliminatry test suite for function EXPAND_PATH
;
; See issues : #832 (duplicate),
;              #2022 (tilde expansion)
; Missing tests for #217 (sym link)
;
; ---------------------------------------
; Modifications history :
;
; 2025-Apr-18 : creation. no solution for ~ on OSX arm64 !
;
; ----------------------------------------------------
;
; Please note IDL don't remove trailing spaces/blancks/tabs ...
; we don't have to do it. Removing space(s) before ~ ONLY
;
pro TEST_EXPAND_PATH_TILDE, cumul_errors, test=test, verbose=verbose
;
nb_errors=0
;
val1=EXPAND_PATH('~/.gdl')
val2=EXPAND_PATH(' ~/.gdl') ;; first char is a space
val3=EXPAND_PATH(STRING(9b)+'~/.gdl') ;; first char is a tab
val4=EXPAND_PATH('  '+STRING(9b)+' ~/.gdl') ;; 2 spaces + 1 tab
;
if KEYWORD_SET(verbose) then begin
   print, val1
   print, val2
   print, val3
   print, val4
endif
;
if val1 NE val2 then ERRORS_ADD, nb_errors, 'Space then Tilde test failed'
if val1 NE val3 then ERRORS_ADD, nb_errors, 'Tab + Tilde test failed'
if val1 NE val4 then ERRORS_ADD, nb_errors, '2 spaces + Tilde test failed'
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_EXPAND_PATH_TILDE', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
; ----------------------------------------------------
;
pro TEST_EXPAND_PATH_REC, cumul_errors, test=test, verbose=verbose
;
nb_errors=0
;
val1=EXPAND_PATH(GETENV('HOME'))
val2=EXPAND_PATH(GETENV('HOME')+PATH_SEP()+'..'+PATH_SEP()+GETENV('USER'))
;
if val1 NE val2 then ERRORS_ADD, nb_errors, 'recursive test failed'
;
if KEYWORD_SET(verbose) then begin
   print, val1
   print, val2
endif
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_EXPAND_PATH_REC', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
; ----------------------------------------------------
; 
pro TEST_EXPAND_PATH_DUPLICATE, cumul_errors, test=test, verbose=verbose
;
nb_errors=0
;
val1='a:b:a:b:a:c:a:d:a:d:a:e:a'
expected='a:b:c:d:e'
;
if EXPAND_PATH(val1) NE expected then $
   ERRORS_ADD, nb_errors, 'bad duplicate removal'
;
if KEYWORD_SET(verbose) then begin
   print, 'input : ', val1
   print, 'processed : ', EXPAND_PATH(val1)
   print, 'expected  : ', expected
endif
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_EXPAND_PATH_DUPLICATE', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
; ----------------------------------------------------
;
pro TEST_EXPAND_PATH, help=help, test=test, verbose=verbose, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_EXPAND_PATH, test=test, verbose=verbose, $'
   print, '                      help=help, no_exit=no_exit'
   return
end
;
cumul_errors=0
;
TEST_EXPAND_PATH_TILDE, cumul_errors, verbose=verbose
;
TEST_EXPAND_PATH_REC, cumul_errors, verbose=verbose
;
TEST_EXPAND_PATH_DUPLICATE, cumul_errors, verbose=verbose
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_EXPAND_PATH', cumul_errors, short=short
;
if (cumul_errors NE 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
