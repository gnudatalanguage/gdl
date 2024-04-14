pro TEST_ELMHES, test=test, no_exit=no_exit, help=help
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_ELMHES, test=test, no_exit=no_exit, help=help'
   return
endif
;
nb_errors=0
a=findgen(4,4)+0.5
b=elmhes(a)
t=total(b) & p=product(b)
st=string(t,format='(f012.8)')
pt=string(p,format='(f012.8)')
if st ne '101.32080078' or pt ne '-00.00000029' then ERRORS_ADD, nb_errors, 'bad result elmhes'
b=elmhes(a,/no)
t=total(b) & p=product(b)
st=string(t,format='(f012.8)')
pt=string(p,format='(f012.8)')
if st ne '096.58079529' or pt ne '-00.00000007' then ERRORS_ADD, nb_errors, 'bad result elmhes,/no_balance'
b=elmhes(a,/column)
t=total(b) & p=product(b)
st=string(t,format='(f012.8)')
pt=string(p,format='(f012.8)')
if st ne '099.94132233' or pt ne '000.00000553' then ERRORS_ADD, nb_errors, 'bad result elmhes,/column'

;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_ELMHES', nb_errors
;
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end

