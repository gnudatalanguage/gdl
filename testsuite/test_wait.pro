;
; AC 12 Mars 2008
;
; Because I saw WAIT is not very accurate !
;
; You can check whether the CPU is loaded or not 
; (e.g. "GDL >  WAIT, 60" should not overloaded a CPU).
; At the beggining we used a inadequate primitive
; which overloaded a CPU just by waiting.
;
pro TEST_WAIT, delay=delay, inner_loop=inner_loop, tol_error=tol_error, $
               help=help, test=test, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_WAIT, delay=delay, inner_loop=inner_loop, tol_error=tol_error, $'
   print, '               help=help, test=test, no_exit=no_exit'
   print, ''
   print, 'Delay can be an array.'
   return
endif
;
; on CentOS, tol_error below 1.2e-3 is not OK
; on Suse, tol_error below 5e-3 is not OK
;
if ~KEYWORD_SET(tol_error) then tol_error=5e-3
nb_errors=0
;
if ~KEYWORD_SET(delay) then delay=[0.1, 0.25, 0.5, 1.0]
if ~KEYWORD_SET(inner_loop) then inner_loop=3
;
for ii=0, N_ELEMENTS(delay)-1 do begin
   cur_delay=delay[ii]
   print, 'Currently tested DELAY : ',  cur_delay
   for jj=0, inner_loop-1 do begin
      t0=SYSTIME(1)
      WAIT, cur_delay
      diff=SYSTIME(1)-t0
      if (ABS(diff-cur_delay) GT tol_error) then nb_errors=nb_errors+1
      print, diff
   endfor
endfor
;
if (nb_errors GT 0) then begin
   MESSAGE,/continue, 'Tolerance Time error was exceed '+ $
           STRCOMPRESS(STRING(nb_errors))+' times.'
   if ~KEYWORD_SET(no_exit) then EXIT, status=1
endif else begin
   MESSAGE,/continue, 'Success: Tolerance Time Error was never exceed by WAIT'
endelse
;
if KEYWORD_SET(test) then STOP
;
end
;

