;
; Alain C., 2019-Jan-07, under GNU GPL v3+
;
pro MAP_GSHHG_PATH_SHOW, help=help, test=test
;
if KEYWORD_SET(help) then begin
   print, 'pro MAP_GSHHG_PATH_SHOW, help=help, test=test'
   return
endif
;
GSHHG_DATA_DIR_INIT
;
print, '!GSHHG_DATA_DIR  = ', !GSHHG_DATA_DIR
;
tmp=!GSHHG_DATA_DIR
if (STRLEN(tmp) EQ 0) OR ~FILE_TEST(tmp, /dir) then begin
   print, 'Warning : the !GSHHG_DATA_DIR value don''t correspond to a real path'
   print, 'Please run MAP_GSHHG_PATH_STATUS for more informations'
   print, 'Please run MAP_GSHHG_PATH_SET to change the value'
endif
;
if KEYWORD_SET(test) then STOP
;
end
;
