; AC, 14/01/2013 at JPL
; initializing the timing ...
;
; revisiting on Nov. 26, 2013
; rewrote by GD 2023 (!) not accurate enough
; under GPL 2 or any later
;
function TIC, tagname, profiler=doProfile, default=UseAsDefault ; default undocumented and profiler not exactly supported.
;
compile_opt idl2, hidden
ON_ERROR, 2
;
common ourtictoc,  NewReferenceTime
;
if ~isa(NewReferenceTime) eq 0 then NewReferenceTime=0d
if ~isa(tagname) then tagname=''
; TOC must be alredy compiled when it will be called, so:
resolve_routine, 'toc', /no_recompile, /either

  time = systime(/seconds)
  return_value = {name: tagname, time: time}

  if (keyword_set(UseAsDefault)) then NewReferenceTime = time

  return, return_value
end

pro tic, tagname, profiler=doProfile ; profiler not exactly supported.

  compile_opt idl2, hidden
  on_error, 2
;  common ourtictoc, NewReferenceTime
  
;  !null = tic(tagname, /default, profiler=doProfile)
  z = tic(tagname, /default, profiler=doProfile)
end

