;
; test label_date function
; serves also indirectly for testing CALDAT
;
; -------------------------------------------------
; simple debug print when needed ...
pro MODE_DEBUG, exp, val, valname, debug=debug, test=test
if KEYWORD_SET(debug) then begin
   svalname=STRING(valname, format='(A30)')+': '
   print, STRING('expected', format='(A30)')+': ', '>>'+exp+'<<'
   print, svalname,  '>>'+val+'<<'
endif
if KEYWORD_SET(test) then STOP
end
; -------------------------------------------------
;
pro TEST_LABEL_DATE, help=help, no_exit=no_exit, test=test, debug=debug
;
if KEYWORD_SET(help) then begin
   print, 'TEST_LABEL_DATE, help=help, debug=debug, $'
   print, '                 test=test, no_exit=no_exit'
   return
endif
;
nb_errors=0
;
z=2451634.1246527783d
;
v1=LABEL_DATE(0,0,z)
exp1='Thu Mar 30 14:59:30 2000'
if (v1 ne exp1) then ERRORS_ADD, nb_errors, '1/ basic case'
MODE_DEBUG, exp1, v1, '', debug=debug, test=test
;
v2=LABEL_DATE(0,0,z,date_f="%W, %M %D, %H %A",am=["matin","soir"])
exp2='Thu, Mar 30,  2 soir'
if (v2 ne exp2) then ERRORS_ADD, nb_errors, '2/ mat/soir case'
MODE_DEBUG, exp2, v2, '', debug=debug, test=test
;
v3=LABEL_DATE(0,0,z,date_f="%W, %M %D, %Hh")
exp3='Thu, Mar 30, 14h'
if (v3 ne exp3) then ERRORS_ADD, nb_errors, '3/ 14h case'
MODE_DEBUG, exp3, v3, '', debug=debug, test=test
;
v4=LABEL_DATE(0,0, JULDAY(10,3,1982), date_f="%W, %M %D,%Z %Hh", $
              am=["matin","soir"])
exp4='Sun, Oct 03,82 12h'
if (v4 ne exp4) then ERRORS_ADD, nb_errors, '4/ 14h case' 
MODE_DEBUG, exp4, v4, '', debug=debug, test=test
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, "TEST_LABEL_DATE", nb_errors
;
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
