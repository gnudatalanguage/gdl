;
; Alain C., 2019-Jan-07, under GNU GPL v3+
;
; Easy way to change the quality of the maps
; No check whether the *data* are around !
;
; The information of Map Level
;
pro MAP_GSHHG_QUALITY_SET, crude=crude, low=low, intermediate=intermediate, $
                           high=high, full=full, $
                           verbose=verbose, help=help, test=test, debug=debug
;
if KEYWORD_SET(help) then begin
   print, 'pro MAP_GSHHG_QUALITY_SET, crude=crude, low=low, intermediate=intermediate, $'
   print, '                           high=high, full=full, $'
   print, '                           verbose=verbose, help=help, test=test, debug=debug'
   return
endif
;
nbkeys=0
;
if KEYWORD_SET(crude) then begin & nbkeys++ & quality='crude' & endif
if KEYWORD_SET(low) then begin & nbkeys++ & quality='low' & endif
if KEYWORD_SET(intermediate) then begin & nbkeys++ & quality='intermediate' & endif
if KEYWORD_SET(high) then begin & nbkeys++ & quality='high' & endif
if KEYWORD_SET(full) then begin & nbkeys++ & quality='full' & endif
;
if (nbkeys EQ 0) then begin
   print, 'No data quality selected !'
   if KEYWORD_SET(verbose) then $
      print, 'List of possible Map Quality levels : CRUDE, LOW, INTERMEDIATE, HIGH, FULL'
   print, 'We stay in : '+!GDL.MAP_QUALITY
   return
endif
;
if (nbkeys GT 1) then begin
   print, 'More than one data quality selected !'
   print, 'We stay in : '+!GDL.MAP_QUALITY
   return
endif
;
quality=STRUPCASE(quality)
;
print, 'Map Quality Level set to : '+quality
;
GDL_CONFIG, map_quality=quality
;
if KEYWORD_SET(test) then STOP
;
end
;
