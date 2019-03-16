;
; Alain Coulais, 6 Feb. 2019. Under GNU GPL v2+
;
; preliminatry test suite for function ROUTINE_NAME
;
; ----------------------------------------------------
; testing for a procedure
pro AC_PRO1234, cumul_errors, test=test
;
FORWARD_FUNCTION ROUTINE_NAME_INTERNALGDL
;
nb_errors=0
;
ref='AC_PRO1234'
n1=ROUTINE_NAME()
;
if (ref NE n1) then ERRORS_ADD, nb_errors, 'pb with ROUTINE_NAME()'
;
if (GDL_IDL_FL() EQ 'GDL') then begin
   n2=ROUTINE_NAME_INTERNALGDL()
   if (ref NE n2) then ERRORS_ADD, nb_errors, 'pb with ROUTINE_NAME_INTERNALGDL()'
endif
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, ref, nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; ----------------------------------------------------
; testing for a function
function AC_FUNCT1234, cumul_errors, test=test
;
FORWARD_FUNCTION ROUTINE_NAME_INTERNALGDL
;
nb_errors=0
;
ref='AC_FUNCT1234'
n1=ROUTINE_NAME()
;
if (ref NE n1) then ERRORS_ADD, nb_errors, 'pb with ROUTINE_NAME()'
;
if (GDL_IDL_FL() EQ 'GDL') then begin
   n2=ROUTINE_NAME_INTERNALGDL()
   if (ref NE n2) then ERRORS_ADD, nb_errors, 'pb with ROUTINE_NAME_INTERNALGDL()'
endif
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, ref, nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
return, 1
;
end
;
; ----------------------------------------------------
;
pro TEST_ROUTINE_NAME, help=help, verbose=verbose, short=short, $
                       debug=debug, test=test, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_ROUTINE_NAME, help=help, verbose=verbose, short=short, $'
    print, '                       debug=debug, test=test, no_exit=no_exit'
    return
endif
;
cumul_errors=0
;
AC_PRO1234, cumul_errors, test=test
tmp=AC_FUNCT1234(cumul_errors, test=test)
;
BANNER_FOR_TESTSUITE, 'TEST_ROUTINE_NAME', cumul_errors, short=short
;
if (cumul_errors NE 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
