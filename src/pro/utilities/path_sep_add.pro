;
; Alain C., 2018-JAN-29
;
; adding a PATH_SEP() at the end of a path (directory) if needed
;
function PATH_SEP_ADD, directories, verbose=verbose
;
pathsep=PATH_SEP()
;
if !version.os_family eq 'windows' then MESSAGE, 'We are not ready, please help'
;
for ii=0, N_ELEMENTS(directories)-1 do begin
   if STRLEN(directories) GT 0 then begin
      last = STRMID(directories[ii], STRLEN(directories[ii])-1, 1)
      if last NE pathsep then directories[ii]=directories[ii]+pathsep
   endif
endfor
;
if KEYWORD_SET(verbose) then print, 'after : '+TRANPOSE5(directories)
if KEYWORD_SET(test) then STOP
return, directories
end
