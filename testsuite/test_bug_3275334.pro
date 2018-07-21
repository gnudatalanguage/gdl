;
; testing Z buffer and X11
;
; question by AC on 15/06/02 : it is OK too on WIN mode ??
;
pro TEST_BUG_3275334, test=test, no_exit=no_exit
;
; AC 15/06/02 : this test was not working on a VM without X11
; but returning a DISPLAY set as : localhost:10.0 ...
;if GETENV('DISPLAY') eq '' then begin
;
device_mode=!D.name
;
if ~(device_mode EQ 'X' OR device_mode EQ 'WIN') then begin
   exit77=0
   if (!version.OS_FAMILY EQ "Windows") then begin
      test_win=EXECUTE("SET_PLOT, 'WIN'")
      if (test_win EQ 0) then begin
         MESSAGE, /continue, 'Unable to switch to WIN display mode'
         exit77=1
      endif      
   endif else begin
      ;; we are on a unix OS: linux, BSD, OSX ...
      test_x11=EXECUTE("SET_PLOT, 'X'")
      if (test_x11 EQ 0) then begin
         MESSAGE, /continue, 'Unable to switch to X11 display mode'
         exit77=1
      endif      
   endelse
   ;;
   if exit77 EQ 1 then begin
      if KEYWORD_SET(no_exit) then stop
      EXIT, status=77
   endif
endif
;
; if we are here, !D.name == X or WIN only
device_mode=!D.name
;
SET_PLOT, 'Z' 
TV, DIST(10)
;
; switch back to display mode (X or WIN)
;
SET_PLOT, device_mode
    if !version.os_family eq 'Windows' then $
        message,/continue,' TVRD not working for windows. fake success'
    if !version.os_family eq 'Windows' then return

TV, TVRD()
;
end
