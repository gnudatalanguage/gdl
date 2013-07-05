;
; testing Z buffer and X11
;
pro test_bug_3275334
;
if GETENV('DISPLAY') eq '' then begin
   MESSAGE, /continue, 'no X11 display found'
   EXIT, status=77
endif
;
SET_PLOT, 'Z' 
TV, DIST(10)
SET_PLOT, 'X' 
TV, TVRD()
;
end
