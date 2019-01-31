;
; Alain C., 2019-Jan-07, under GNU GPL v3+
;
; Easy way to show the current name of map quality level
; Obvioulsy, this is just an interface to the internal variable,
; now in !GDL.map_quality
; (may change in the future, depending on feedback)
;
; No check whether the *data* are around !
;
pro MAP_GSHHG_QUALITY_SHOW, help=help, test=test
;
if KEYWORD_SET(help) then begin
   print, 'pro MAP_GSHHG_QUALITY_SHOW, verbose=verbose, help=help, test=test'
   return
endif
;
print, 'We are using Map Quality level : '+!GDL.MAP_QUALITY
;
if KEYWORD_SET(test) then STOP
;
end
;

