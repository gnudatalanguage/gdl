; test label_date function
; serves also indirectly for testing caldat
pro test_label_date, no_exit=no_exit, test=test
  nb_errors=0
  z=2451634.1246527783d
  v=LABEL_DATE(0,0,z)
  if v ne 'Thu Mar 30 14:59:30 2000' then nb_errors++
  v=LABEL_DATE(0,0,z,date_f="%W, %M %D, %H %A",am=["matin","soir"])
  if v ne 'Thu, Mar 30,  2 soir' then nb_errors++
  v=LABEL_DATE(0,0,z,date_f="%W, %M %D, %Hh")
  if v ne 'Thu, Mar 30, 14h' then nb_errors++
  v=LABEL_DATE(0,0,julday(10,3,1982),date_f="%W, %M %D,%Z %Hh",am=["matin","soir"])
  if v ne 'Mon, Oct 03,82 12h' then nb_errors++
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
