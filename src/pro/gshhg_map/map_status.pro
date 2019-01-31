;
; Alain C., 2019-Jan-07, under GNU GPL v3+
;
; Because using Mapping capabilities in GDL is not obvious,
; we add few commodities ...
;
; This code now is suppose to work only for GDL (not IDL, not FL)
;
pro MAP_STATUS, help=help, test=test
;
FORWARD_FUNCTION GSHHG_EXISTS, PROJ4_EXISTS, PROJ4NEW_EXISTS
;
if KEYWORD_SET(help) then begin
   print, 'pro MAP_STATUS, help=help, test=test'
   return
endif
;
DEFSYSV, '!GDL', exists=exists_gdl
if ~exists_gdl then begin
   print, 'We are in IDL or FL. This code is not effective in IDL or FL.'
   return
endif
;
; 1/ Are the internal set as expected ??
;
print, 'GSHHG ?  : ', GSHHG_EXISTS()
print, 'Proj4 ?  : ', PROJ4_EXISTS()
print, 'Proj4 new: ', PROJ4NEW_EXISTS()
;
if ~GSHHG_EXISTS() then begin
   print, 'Very strange that GDL was compiled without GSHHG/S linking/header'
   print, 'please check you GDL version & status & compilation option(s)'
endif
;
if ~(PROJ4_EXISTS() OR PROJ4NEW_EXISTS()) then begin
   print, 'Missing Projections at compilation time ...'
   print, 'please check you GDL version & status & compilation option(s)'
endif
print, ''
;
; 2/ Do we have a recent GDL version (0.9.9 since January 2019 ?!)
; with the !gdl.map_quality' tag ?
;
ok=EXECUTE('map_quality=!gdl.map_quality')
if ~ok then begin
   print, 'Old GDL version for mapping ... please consider upgrading'
   print, '(several convenience pro/functions have been added in January 2019)'
endif else begin
   MAP_GSHHG_QUALITY_LIST, /verbose
endelse
print, ''
;
; 3/ Do we have a !GSHHG_DATA_DIR set ?
;
DEFSYSV, '!GSHHG_DATA_DIR', exist=exist
;
if ~exist then begin
   print, 'Missing internal !GSHHG_DATA_DIR variable ... we define it'
   DEFSYSV, '!GSHHG_DATA_DIR', ''
endif
;
gshhg_dir_status=0
;
if (STRLEN(!GSHHG_DATA_DIR) EQ 0) then begin
   print, 'the !GSHHG_DATA_DIR is not set'
endif else begin
   if ~FILE_TEST(!GSHHG_DATA_DIR,/directory) then begin
      print, 'the !GSHHG_DATA_DIR is set but the directory it-self don''t exist !'
   endif else begin
     gshhg_dir_status=1 
   endelse
endelse
print, ''
;
if ~gshhg_dir_status then begin
   print, 'We cannnot check whether some map data are available'
   print, 'since the !GSHHG_DATA_DIR is not OK'
endif else begin
   MAP_GSHHG_DATA_CHECK, /verbose
endelse
;
if KEYWORD_SET(test) then STOP
;
end
;
