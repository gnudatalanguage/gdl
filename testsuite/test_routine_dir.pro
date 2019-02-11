;
; Alain Coulais, 6 Feb. 2019. Under GNU GPL v2+
;
; preliminatry test suite for function ROUTINE_DIR
; (introduced in IDL 8.7)
;
; ----------------------------------------------------
; testing for a procedure
pro AC_PRO_DIR_1234, cumul_errors, test=test, verbose=verbose
;
FORWARD_FUNCTION ROUTINE_DIR
;
name=ROUTINE_NAME()
;
nb_errors=0
;
;cd, cur=cur
;cur=cur+PATH_SEP()

cur=FILE_DIRNAME(ROUTINE_FILEPATH(name))+PATH_SEP()
;
dir=ROUTINE_DIR()
;
if KEYWORD_set(verbose) then begin
   print, 'current : ', cur
   print, 'dir     : ', dir
endif
;
if (cur NE dir) then ERRORS_ADD, nb_errors, 'pb with ROUTINE_DIR()'
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, name, nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; ----------------------------------------------------
; testing for a function
function AC_FUNCT_DIR_1234, cumul_errors, test=test, verbose=verbose
;
FORWARD_FUNCTION ROUTINE_DIR
;
name=ROUTINE_NAME()
;
nb_errors=0
;
;cd, cur=cur
;cur=cur+PATH_SEP()
cur=FILE_DIRNAME(ROUTINE_FILEPATH(name,/is_function))+PATH_SEP()
;
dir=ROUTINE_DIR()
;
if KEYWORD_set(verbose) then begin
   print, 'current : ', cur
   print, 'dir     : ', dir
endif
;
if (cur NE dir) then ERRORS_ADD, nb_errors, 'pb with ROUTINE_DIR()'
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, ROUTINE_NAME(), nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
return, 1
;
end
;
; ----------------------------------------------------
;
pro TEST_ROUTINE_DIR, help=help, verbose=verbose, short=short, $
                      debug=debug, test=test, no_exit=no_exit
;
FORWARD_FUNCTION AC_PRO_DIR_1234, AC_FUNCT_DIR_1234
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_ROUTINE_DIR, help=help, verbose=verbose, short=short, $'
    print, '                      debug=debug, test=test, no_exit=no_exit'
    return
 endif
;
if ((GDL_IDL_FL() EQ 'IDL') and (GDL_VERSION() LT 80700)) then begin
   txt='This ROUTINE_DIR() function appeared in IDL 8.7'
   MESSAGE, /continue, 'IDL version too old. '+txt
   EXIT, status=77
endif
;
if (GDL_IDL_FL() EQ 'FL') then begin
   txt='This ROUTINE_DIR() function don''t exist now in FL.'
   MESSAGE, /continue, txt+' Please report when available.'
   EXIT, status=77
endif
;
cumul_errors=0
;
AC_PRO_DIR_1234, cumul_errors, test=test, verbose=verbose
tmp=AC_FUNCT_DIR_1234(cumul_errors, test=test, verbose=verbose)
;
BANNER_FOR_TESTSUITE, 'TEST_ROUTINE_DIR', cumul_errors, short=short
;
if (cumul_errors NE 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
