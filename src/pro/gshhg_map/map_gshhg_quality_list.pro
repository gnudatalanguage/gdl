;
; Alain C., 2019-Jan-07, under GNU GPL v3+
;
; Easy way to change the quality of the maps
; No check whether the *data* are around !
;
pro MAP_GSHHG_QUALITY_LIST, help=help, test=test, verbose=verbose
;
if KEYWORD_SET(help) then begin
   print, 'pro MAP_GSHHG_QUALITY_LIST, help=help, test=test, verbose=verbose'
   return
endif
;
list=['CRUDE','LOW','INTERMEDIATE','HIGH','FULL']
;
print, 'List of possible Map Quality levels : ', list
; 
print, 'We are using : ', !GDL.MAP_QUALITY
;
if KEYWORD_SET(verbose) then begin
   print, ''
   print, 'You can change the Map Quality level calling :'
   print, '1/ GDL_CONFIG, map_quality=*you choice*'
   print, '2/ MAP_GSHHG_QUALITY_SET, '+$
          '[/CRUDE, /LOW, /INTERMEDIATE, /HIGH, /FULL]'
endif
;
if KEYWORD_SET(test) then STOP
;
end
;
