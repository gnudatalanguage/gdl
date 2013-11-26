; AC, first version 14/01/2013 at JPL
; using the timing ... eventually no reseting it
;
; under GPL 2 or any later
;
pro TOC, list_of_clocks, report=report, help=help, test=test
;
ON_ERROR, 2
;
if KEYWORD_SET(help) then begin
   print, 'pro TOC, list_of_clocks, report=report, help=help, test=test'
   return
endif
;
if KEYWORD_SET(report) then begin
   MESSAGE,/continue, 'This /REPORT keyword is not available, please contribute'
endif
;
if N_PARAMS() GT 1 then MESSAGE, 'Incorrect number of arguments.'
;
t0=SYSTIME(1)
prefix='% Time elapsed '
suffix=' seconds.'
;
input_is_null=(N_ELEMENTS(list_of_clocks) EQ 0) OR (list_of_clocks EQ !null)
;
if ((N_PARAMS() EQ 0) OR (input_is_null)) then begin
   DEFSYSV, '!TIC', exist=tic_exist
   if ~tic_exist then MESSAGE, 'No tic, no toc'
   print, prefix+': ', t0-!tic.time, suffix
   return
endif
;
;if N_PARAMS() EQ 1 then stop
;if N_ELEMENTS(list_of_clocks) EQ 0 then begin
;   help, list_of_clocks 
;   stop
;endif
;
for ii=0, N_ELEMENTS(list_of_clocks)-1 do begin
   print, prefix+list_of_clocks[ii].name+': ', $
          t0-list_of_clocks[ii].TIME, suffix
endfor
;
if KEYWORD_SET(test) then STOP
;
end
;
; --------------------------------
;
function TOC, list_of_clocks, report=report, help=help, test=test
;
ON_ERROR, 2
;
if KEYWORD_SET(help) then begin
   print, 'function TOC, list_of_clocks, report=report, help=help, test=test'
   return, !null
endif
;
if KEYWORD_SET(report) then begin
   MESSAGE,/continue, 'This /REPORT keyword is not available, please contribute'
endif
;
if N_PARAMS() GT 1 then MESSAGE, 'Incorrect number of arguments.'
;
t0=SYSTIME(1)
;
input_is_null=(N_ELEMENTS(list_of_clocks) EQ 0) OR (list_of_clocks EQ !null)
;
if ((N_PARAMS() EQ 0) OR (input_is_null)) then begin
   DEFSYSV, '!TIC', exist=tic_exist
   if ~tic_exist then begin
      MESSAGE, /continue, 'No tic, no toc'
      return, !null
   endif
   return, t0-!tic.time
endif
;
list_of_times=DBLARR(N_ELEMENTS(list_of_clocks))
for ii=0, N_ELEMENTS(list_of_clocks)-1 do begin
   list_of_times[ii]=t0-list_of_clocks[ii].TIME
endfor
;
if KEYWORD_SET(test) then STOP
;
return, list_of_times
;
end
