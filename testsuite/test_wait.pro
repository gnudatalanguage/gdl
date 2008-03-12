;
; AC 12 Mars 2008
;
; Because I saw WAIT is not very accurate !
;
pro test_wait, delay=delay, inner_loop=inner_loop, help=help
;
if KEYWORD_SET(help) then begin
   print, 'pro test_wait, delay=delay, inner_loop=inner_loop, help=help'
   print, ''
   print, 'Delay can be an array. '
   return
endif
;
if N_ELEMENTS(delay) EQ 0 then delay=[0.1, 0.5, 1.0]
if N_ELEMENTS(inner_loop) EQ 0 then inner_loop=3

for ii=0, N_ELEMENTS(delay)-1 do begin
   cur_delay=delay[ii]
   print, 'Currently tested DELAY : ',  cur_delay
   for jj=0, inner_loop-1 do begin
      a=SYSTIME(1)
      WAIT, cur_delay
      print, SYSTIME(1)-a
   endfor
endfor
end

