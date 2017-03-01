;
; Sylwester: initial version for PostScript outputs
; Alain, 18 March 2014 : extention to basic tests around X11
;
; Alain, 28 Feb 2017 : basic tests around Fonts (GET_FONTNAME,
; GET_FONTNUM, SET_FONTNAME, ...) [not finish do to problem in C++ code]
;
; -------------------------------------------------------------
;
pro TEST_DEVICE_X, nb_errors, test=test, verbose=verbose, window=window
;
errors=0
;
mode_stored=!D.name
;
if ~KEYWORD_SET(window) then begin
   test_x11=EXECUTE("SET_PLOT, 'X'")
   if (test_x11 NE 1) then begin
      MESSAGE, /continue, 'Unable to switch to X display mode'
      return
   endif
endif else begin
   test_win=EXECUTE("SET_PLOT, 'WIN'")
   if (test_win NE 1) then begin
      MESSAGE, /continue, 'Unable to switch to WIN display mode'
      return
   endif
endelse  
;
xsize=223
ysize=245
xpos=67+200
ypos=89+300
;
; because window positions are accurate to +-1 pixel ...
tolerance=3 
;
; first of all, we need to get the absolute Zero offset
; (generate by Window Manager and boxes around windows)
;
WINDOW, ypos=0, xpos=0
DEVICE, get_window_position=gwp_zero
;
; the window we check
;
WINDOW, xsize=xsize, ysize=ysize, xpos=xpos, ypos=ypos
DEVICE, get_window_position=gwp, get_screen_size=gss, $
  get_visual_depth=gvd, get_visual_name=gvn
;
; other way to get Screen Size
alt_ss=GET_SCREEN_SIZE()
; 
if ((ROUND(alt_ss[0]) NE gss[0]) OR (ROUND(alt_ss[1]) NE gss[1])) then begin
    errors++
    txt='GetScreenSize values are different from DEVICE or GET_SCREEN_SIZE()'
    MESSAGE, /continue, txt
endif
;
if KEYWORD_SET(verbose) then begin
    MESSAGE, /continue, 'Visual Depth : '+STRING(gvd)
    MESSAGE, /continue, 'Visual Name  : '+gvn
    print, 'x/y absolute offsets : ', gwp_zero
    print, format='(A,4i5)',  'IN xsize, ysize, xpos, ypos: ', $
      xsize, ysize, xpos, ypos
    print, format='(A,4i5)', 'OUT xsize, ysize, xpos, ypos: ', $
      !D.x_size, !D.y_size, gwp[0]-gwp_zero[0], gwp[1]+gwp_zero[1]-gwp_zero[0]
endif
;
if (xsize NE !D.x_size) then begin
    errors++
    MESSAGE, /continue, 'problem with XSIZE'
endif
;
if (ysize NE !D.y_size) then begin
    errors++
    MESSAGE, /continue, 'problem with YSIZE'
endif
; test changed as warning since even IDL does not pass it!!!
if (ABS(xpos-(gwp[0]-gwp_zero[0])) GT tolerance) then begin
    MESSAGE, /continue, 'problem with XPOS'
endif
if (ABS(ypos-(gwp[1]+gwp_zero[1]-gwp_zero[0])) GT tolerance) then begin
    MESSAGE, /continue, 'problem with YPOS'
endif
;
; AC 19 March 2014: very preliminaly tests, no more idea
;
if (~((gvd EQ 24) OR (gvd EQ 8))) then begin
    MESSAGE, /continue, 'Warning: unexpected Visual Depth (please contribute)'
endif
if (strlowcase(gvn) NE 'truecolor') then begin
    MESSAGE, /continue, 'Warning: unexpected Visual Name (please contribute)'
endif
;
BANNER_FOR_TESTSUITE, 'TEST_DEVICE_X', errors, /short, verb=verbose
;
ERRORS_CUMUL, nb_errors, errors
;
; restore initial mode
SET_PLOT, mode_stored
while (!D.window GT 0) do WDELETE
;
if KEYWORD_SET(test) then STOP
;
end
;
; -------------------------------------------------------------
;
pro TEST_DEVICE_PS, nb_errors, test=test, verbose=verbose
;
errors=0
;
mode_stored=!D.name
;
SET_PLOT, 'ps'
;
DEVICE, xsize=10
if !D.X_SIZE ne 10000 then begin
    MESSAGE, '!D.X_SIZE ne 10000', /conti
    errors++
endif
;
DEVICE, ysize=20
if !D.Y_SIZE ne 20000 then begin
    MESSAGE, '!D.Y_SIZE ne 20000', /conti
    errors++
endif
;
DEVICE, xsize=10, /inches
if !D.X_SIZE ne 25400 then begin
    MESSAGE, '!D.X_SIZE ne 25400', /conti
    errors++
endif
;
DEVICE, ysize=20, /inches
if !D.Y_SIZE ne 50800 then begin
    MESSAGE, '!D.Y_SIZE ne 50800', /conti
    errors++
endif
;
; we check also if !D is updated ...
;
print, '!D in PS mode ToDo'
;
BANNER_FOR_TESTSUITE, 'TEST_DEVICE_PS', errors, /short, verb=verbose
;
ERRORS_CUMUL, nb_errors, errors
;
; restore initial mode
SET_PLOT, mode_stored
;
if KEYWORD_SET(test) then STOP
;
end
;
; -------------------------------------------------------------
;
pro TEST_DEVICE_FONTS, cumul_errors, help=help, test=test, verbose=verbose
;
errors=0
;
DEVICE, GET_FONTNUMBER=fnum
if ISA(fnum) print, 'error 1'
;
BANNER_FOR_TESTSUITE, 'TEST_DEVICE_FONTS', errors, /short, verb=verbose
;
ERRORS_CUMUL, nb_errors, errors
;
if KEYWORD_SET(test) then STOP
;
end
;
; -------------------------------------------------------------
;
pro TEST_DEVICE, help=help, test=test, verbose=verbose, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_DEVICE, help=help, test=test, verbose=verbose, no_exit=no_exit'
   return
endif
;
nb_errors=0
;
mode_stored=!D.name
;
print, 'Calling TEST_DEVICE_X with X11 on'
TEST_DEVICE_X, nb_errors, test=test, verbose=verbose
;
print, 'Calling TEST_DEVICE_X with WIN on'
TEST_DEVICE_X, nb_errors, test=test, verbose=verbose,/window
;
print, 'Calling TEST_DEVICE_PS'
TEST_DEVICE_PS, nb_errors, test=test, verbose=verbose
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_DEVICE', nb_errors
;
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
; restore initial mode
SET_PLOT, mode_stored
;
end
