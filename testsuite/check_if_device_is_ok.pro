;
; AC 2018-Oct-24
;
; still pending : What is this Mac Device ?!
;
; Testing whether we can open a WINDOW for later "plots"
;
; * two cases on X11 mode (Linux & OSX) : 
; 1 : computing node without X11
; 2 : connection without X11 (ssh without -X)
;
; ----------------------------------------------------
; Modifications history :
;
; 2018-April-06 : AC : really, this code was working ?
; it return 1 when ssh without "-X" !!
; Try GET_SCREEN_SIZE() to check whether we can have a Window.
;
; ----------------------------------------------------
;
function CHECK_IF_DEVICE_IS_OK, rname, force=force, $
                                verbose=verbose, quiet=quiet, test=test
;
if (N_PARAMS() NE 1) then begin
   MESSAGE, /continue, 'Missing input routine name ...'
   rname='Unknown'
endif
;
if (!d.name EQ 'X') OR (!d.name EQ 'WIN') OR (!d.name EQ 'MAC') then begin
   status=1
endif else begin
   ;; Z, SVG, PS
   status=0 
endelse
;
; if /FORCE, just in case, we try the three differents modes with WINDOW ...
;
if (status EQ 0) AND KEYWORD_SET(force) then begin
   is_X11_ok=EXECUTE('set_plot, "X"')
   if (is_X11_ok) then status=1 else begin
      is_WIN_ok=EXECUTE('set_plot, "WIN"')
      if (is_WIN_ok) then status=1 else begin
         is_MAC_ok=EXECUTE('set_plot, "MAC"')
         if (is_MAC_ok) then status=1
      endelse
   endelse
   if status then MESSAGE, /continue, 'Force DEVICE successful !'
endif
;;
if KEYWORD_SET(verbose) then begin
   print, '(1) Detected DEVICE !d.name : ', !d.name
   HELP, /device, out=out
   print, '(2) Detected DEVICE via HELP : ', out[1]
   print, '(3) Detected SCREEN size     : ', GET_SCREEN_SIZE()
endif
;;
;; now, trying to know if we will be able to open a window
;; but without open it ;) 
;;
if status then begin
   ;; when the remote computer (or computing node)
   ;; is reached without X11 on (without -X for ssh)
   ;; but GDL was compiled with X11 on the main node 
   ;; we are in a state where !d.name = "X"
   ;; BUT the scren size is (0,0) :))
   res=GET_SCREEN_SIZE()
   ;;
   if (TOTAL(res) EQ 0) then begin
      status=0
      if (!d.name EQ 'X') then begin
         BANNER_FOR_TESTSUITE, rname, out[1], /line
         print, 'This computer can have X11 but we cannot plot'
         print, 'Did you really use << ssh -X >> to connect to it ?'
         BANNER_FOR_TESTSUITE, rname, out[1], /line
      endif
   endif
endif
;;
if (status EQ 0) then begin
   HELP, /device, out=out
   BANNER_FOR_TESTSUITE, rname, out[1], /line
   BANNER_FOR_TESTSUITE, rname, out[1], /short
   BANNER_FOR_TESTSUITE, rname, 'not working for this Graphic Mode', /short
   BANNER_FOR_TESTSUITE, rname, out[1], /line
endif
;
if KEYWORD_SET(test) then STOP
;
return, status
;
end
