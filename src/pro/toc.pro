; AC, first version 14/01/2013 at JPL
; using the timing ... eventually no reseting it
;
; rewrote by GD 2023 (!) not accurate enough
; under GPL 2 or any later
;
function toc, listOfClocks, report=profilerReport ; report ignored.

  compile_opt idl2, hidden
  on_error, 2
  common ourtictoc, NewReferenceTime
; get time first
  tt = systime(/seconds)

  if (~isa(listOfClocks) && ~isa(NewReferenceTime)) then begin
    message, 'no tic, no toc', /informational
    return, !null
  endif else return, isa(listOfClocks) ? tt - listOfClocks.time : tt - NewReferenceTime
end


pro toc, listOfClocks, lun=lun, report=profilerReport  ; report ignored.
  
  compile_opt idl2, hidden
  on_error, 2
  common ourtictoc, NewReferenceTime

  tt = toc(listOfClocks)
  if ~isa(tt) then return
  if ~isa(lun) then lun=-1
  
  header = '% Time elapsed '
  semi=': '
  for iClock=0,N_ELEMENTS(tt)-1 do begin
    if (isa(listOfClocks) && listOfClocks[iClock].name ne '') then begin
       str = header + listOfClocks[iClock].name + semi + strtrim(tt[iClock],2) + ' seconds.'
       printf, lun, str 
    endif else begin
       str = header + semi + strtrim(tt[iClock],2) + ' seconds.'
       printf, lun, str
    endelse
  endfor

end
