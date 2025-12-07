;
; by Alain C. (moved here from src/pro/str_sep.pro)
;
; Important 1 : the function STR_SEP() is tagged as obsolete in IDL
; but many old codes do used it.
;
; Important 2 : AC 2025 : I don't want to add path to src/pro/obsolete
; in testsuite/CMakeLists.txt then we will look for the code inside
; this test
;
; Modification on October, 18, 2012 to include new cases.
; While testing PSM soft (Planck Sky Model: 
; http://www.apc.univ-paris7.fr/~delabrou/PSM/psm.html )
; we discovered a bug: print, STR_SEP('2**10 uK/J', '**')
;
; do not hesitate to submit problematic cases !
;
; -------------------------------------------
; Modifications history :

; - 2012-Oct-18, include new cases.
;     While testing PSM soft (Planck Sky Model: 
;     http://www.apc.univ-paris7.fr/~delabrou/PSM/psm.html )
;     we discovered a bug: print, STR_SEP('2**10 uK/J', '**')
;
; - 2018-Mar-21 : AC. rewriting and expand of initial version
;
; - 2025-Dec-06 : AC.
;   0 use "moderm" infra
;   1 move a small test code from "str_sep.pro" here
;   2 find a way to locate the code in "obsolete"
;
; -------------------------------------------
;
pro TEST_STR_SEP_BASIC, cumul_errors, test=test, verbose=verbose
;
errors=0
;
resu1=STR_SEP('../foo.txt','.')
;
txt='problem 1 (null string removed ?)'
if N_ELEMENTS(resu1) NE 4 then ERRORS_ADD, errors, txt
if resu1[2] NE '/foo' then ERRORS_ADD, errors, 'problem 1bis'
if resu1[3] NE 'txt' then ERRORS_ADD, errors, 'problem 1ter'
;
; --------------
;
BANNER_FOR_TESTSUITE, "TEST_STR_SEP_BASIC", errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; -------------------------------------------
;
pro TEST_STR_SEP_PSM, cumul_errors, test=test, verbose=verbose
;
; inpired from PSM tests: will not survive to a call to STRSPLIT
; due to REGEX issues ...
;
errors=0
;
units=STR_SEP('arcsec**2','**')
;
if (units[0] NE 'arcsec') then ERRORS_ADD, 'PSM problem 2a'
if (units[1] NE '2') then ERRORS_ADD, 'PSM problem 2ab'
;
; --------------
;
BANNER_FOR_TESTSUITE, "TEST_STR_SEP_PSM", errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; -------------------------------------------
;
pro TEST_STR_SEP_TRICKS, cumul_errors, test=test, verbose=verbose
;
errors=0
;
; conversions of no-string input: when Separator is a simple char,
; we do not convert into STRING before comparisons ...
;
res_single=STR_SEP(2e3, '0')
res_nosingle=STR_SEP(2e3, '00')
res_nosingle_str=STR_SEP(STRING(2e3), '00')
;
; first case
;
if (N_ELEMENTS(res_single) NE 1) then $
   ERRORS_ADD, errors, 'problem 1a: output should be converted'
if (STRLEN(res_single) NE 13) then $
   ERRORS_ADD, errors, 'problem 1b: bad legnth of string'
if (res_single NE STRING(2e3)) then $
   ERRORS_ADD, errors, 'problem 1c: output should be converted into string'
;
; second case
if N_ELEMENTS(res_nosingle) NE 3 then $
   ERRORS_ADD, errors, 'problem 2a: bad Elements in output'
;
; third one
if (ARRAY_EQUAL(res_nosingle,res_nosingle_str) eq 0) then begin
    ERRORS_ADD, errors, 'problem 3a: result differs from reference'
endif
;
; final
;
BANNER_FOR_TESTSUITE, "TEST_STR_SEP_TRICKS", errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; -------------------------------------------
;
pro TEST_STR_SEP, help=help, test=test, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_STR_SEP, help=help, test=test, no_exit=no_exit'
    return
endif
;
; Only for GDL we need to locate str_pos.pro which is
; in the src/pro/obsolete path
; when running "make test", we don't have the src/pro/obsolete path
; but we do have src/pro/utilities which provide PATH_ADD ...
;
if (GDL_IDL_FL() eq 'GDL') then begin
   PATH_ADD, '.'
   root_of_pro=ROUTINE_FILEPATH('path_add')
   root_of_pro=FILE_DIRNAME(FILE_DIRNAME(root_of_pro))
   root_of_pro=root_of_pro+PATH_SEP()+'obsolete'
   PATH_ADD, root_of_pro
endif
;
; now we do have the src/pro/obsolete path in !path !
;
TEST_STR_SEP_BASIC, cumul_errors, test=test, verbose=verbose
TEST_STR_SEP_PSM, cumul_errors, test=test, verbose=verbose
TEST_STR_SEP_TRICKS, cumul_errors, test=test, verbose=verbose
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_STR_SEP', cumul_errors, short=short
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
