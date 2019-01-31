;
; Alain C., 2019-Jan-07, under GNU GPL v3+
;
; We return the status (existence) of the !GSHHG_DATA_DIR path
;
pro MAP_GSHHG_PATH_STATUS, help=help, test=test
;
if KEYWORD_SET(help) then begin
   print, 'pro MAP_GSHHG_PATH_STATUS, help=help, test=test'
   return
endif
;
if (STRLEN(!GSHHG_DATA_DIR) EQ 0) then begin
   print, '!GSHHG_DATA_DIR is not set at all'
endif else begin
   if ~FILE_TEST(!GSHHG_DATA_DIR, /dir) then begin
      print, 'the provided path !GSHHG_DATA_DIR is not valid, please check it'
   endif else begin
      print, 'the provided path !GSHHG_DATA_DIR is valid (it does exist)'
   endelse
endelse
;
if KEYWORD_SET(test) then STOP
;
end
;
