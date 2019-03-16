;
; Alain C., 2019-Jan-07, under GNU GPL v3+
;
; We return the status (existence) of the !GSHHG_DATA_DIR path
;
function GSHHG_DATA_DIR_EXISTS, help=help, test=test, verbose=verbose
;
if KEYWORD_SET(help) then begin
   print, 'function GSHHG_DATA_DIR_EXISTS, help=help, test=test, verbose=verbose'
   return, -1
endif
;
status=0
;
if (STRLEN(!GSHHG_DATA_DIR) EQ 0) then begin
   status_mess='!GSHHG_DATA_DIR is not set at all'
endif else begin
   if ~FILE_TEST(!GSHHG_DATA_DIR, /dir) then begin
      status_mess='the provided path !GSHHG_DATA_DIR is not valid, please check it'
   endif else begin
      status_mess='the provided path !GSHHG_DATA_DIR is valid (it does exist)'
      status=1
   endelse
endelse
;
if KEYWORD_SET(verbose) then print, status_mess
;
if KEYWORD_SET(test) then STOP
;
return, status
;
end
;
