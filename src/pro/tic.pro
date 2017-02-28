; AC, 14/01/2013 at JPL
; initializing the timing ...
;
; revisiting on Nov. 26, 2013
;
; under GPL 2 or any later
; 
pro TIC, vide, profiler=profiler, help=help, test=test
;
compile_opt idl2, hidden
ON_ERROR, 2
;
if KEYWORD_SET(help) then begin
   print, 'pro TIC, name, profiler=profiler, help=help, test=test'
   return
endif
;
if KEYWORD_SET(profiler) then begin
   MESSAGE, /continue, 'This /PROFILER keyword is not available, please contribute'
endif
;
if N_PARAMS() GT 1 then MESSAGE, 'Incorrect number of arguments.'
;
if N_PARAMS() EQ 1 then vide=''
;
t0=SYSTIME(1)
;
DEFSYSV, '!TIC', {NAME: '', TIME : t0}
;
if KEYWORD_SET(test) then STOP
;
end
;
; --------------------
;
function TIC, name, profiler=profiler, help=help, test=test
;
compile_opt idl2, hidden
ON_ERROR, 2
;
if KEYWORD_SET(help) then begin
   print, 'function TIC, name, profiler=profiler, help=help, test=test'
   return, !null
endif
;
if KEYWORD_SET(profiler) then begin
   MESSAGE, /continue, 'This /PROFILER keyword is not available, please contribute'
endif
;
if N_PARAMS() GT 1 then MESSAGE, 'Incorrect number of arguments.'
;
if N_PARAMS() EQ 0 then name=''
;
t0=SYSTIME(1)
;
; since a function call to TIC then TOC is OK, !tic must be set up
; (the value is wrong but works as expected !)
;
DEFSYSV, '!TIC', exist=tic_exist
if ~tic_exist then DEFSYSV, '!TIC', {NAME: '', TIME : 0.0D}
;
if KEYWORD_SET(test) then STOP
;
return, {NAME: name, TIME : t0}
;
end

