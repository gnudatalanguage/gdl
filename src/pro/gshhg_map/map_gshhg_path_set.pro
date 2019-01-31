;
; Alain C., 2019-Jan-07, under GNU GPL v3+
;
; Set !GSHHG_DATA__DIR within a local path where are supposed to be
; store the map files (*.b)
;
; If the value don't correspond to a real valid directory, we do nothing
;
pro MAP_GSHHG_PATH_SET, a_path, current=current, $
                        verbose=verbose, help=help, test=test
;
if KEYWORD_SET(help) then begin
   print, 'pro MAP_GSHHG_PATH_SET, a_path_to_gsshg_data, current=current, $ '
   print, '                        verbose=verbose, help=help, test=test'
   return
endif
;
GSHHG_DATA_DIR_INIT
;
if (N_PARAMS() EQ 0) AND ~KEYWORD_SET(current) then begin
   print, 'You should provide a parameter or set keyword CURRENT= as a path'
   print, 'No change in !GSHHG_DATA_DIR'
   MAP_GSHHG_PATH_SHOW
   return
endif
;
if (N_PARAMS() EQ 1) AND KEYWORD_SET(current) then begin
   print, 'Exclusive options  : parameter or keyword CURRENT= as a path !'
   print, 'You should provide only ONE value in these TWO options'
   print, 'No change in !GSHHG_DATA_DIR'
   MAP_GSHHG_PATH_SHOW
   return
endif
;
if KEYWORD_SET(current) then begin
   cd, current=the_current
   !GSHHG_DATA_DIR=the_current
endif
;
if (N_PARAMS() EQ 1) then begin
   if ~FILE_TEST(a_path, /dir) then begin
      print, 'the provided path is not valid, please check it'
      print, 'value : ', a_path
      print, 'No change in !GSHHG_DATA_DIR'
      MAP_GSHHG_PATH_SHOW
      return
   endif else begin
      CD, a_path, current=ref_path
      CD, current=full_gshhg_data_dir
      CD, ref_path
      !GSHHG_DATA_DIR=full_gshhg_data_dir
   endelse
endif
;
MAP_GSHHG_PATH_SHOW
;
if KEYWORD_SET(test) then STOP
;
end
;
