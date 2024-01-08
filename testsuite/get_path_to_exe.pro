;
; AC 2024-JAN-08, under GNU GPL v2+
;
; Return the absolute path of the current interpretor
; (OK with GDL, IDL & FL ...)
;
function GET_PATH_TO_EXE, verbose=verbose, test=test
;
if (!version.os NE 'linux') then begin
   MESSAGE, 'Sorry, this is working only for GNU/Linux', /cont
   MESSAGE, 'Please contribute !'
endif
;
get_pid=FILE_READLINK('/proc/self')
get_abspath_to_exe=file_readlink('/proc/'+get_pid+'/exe')
;
if KEYWORD_SET(verbose) then begin
   print, 'Running GDL : ', get_abspath_to_exe
endif
;
if KEYWORD_SET(stop) then STOP
;
return, get_abspath_to_exe
;
end
;
