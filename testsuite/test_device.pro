;
; Sylwester: initial version for PostScript outputs
; Alain, 18 March 2014: extention to basic tests around X11
;
pro INTERNAL_MESSAGES, message, errors
;
if (errors EQ 0) then begin
    print, 'no errors in '+message
endif else begin
    print, STRING(errors)+' errors in '+message
endelse
;
end
;
; -------------------------------------------------------------
;
pro TEST_DEVICE_X, nb_errors, test=test, verbose=verbose
;
errors=0
;
SET_PLOT, 'X'
if (strlowcase(!d.name) NE 'x') then begin
    MESSAGE, /continue, 'Unable to switch to X display mode'
    return
endif
;
xsize=223
ysize=245
xpos=67+100
ypos=89+200
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
    message,/continue, 'problem with XSIZE'
endif
;
if (ysize NE !D.y_size) then begin
    errors++
    message,/continue, 'problem with YSIZE'
endif
;
if (ABS(xpos-(gwp[0]-gwp_zero[0])) GT tolerance) then begin
    errors++
    message,/continue, 'problem with XPOS'
endif
if (ABS(ypos-(gwp[1]+gwp_zero[1]-gwp_zero[0])) GT tolerance) then begin
    errors++
    message,/continue, 'problem with YPOS'
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
INTERNAL_MESSAGES, 'TEST_DEVICE_X', errors
;
if KEYWORD_SET(test) then STOP
;
if N_PARAMS() GT 0 then nb_errors=nb_errors+errors
;
end
;
; -------------------------------------------------------------
;

pro TEST_DEVICE_PS, nb_errors, test=test, verbose=verbose
;
errors=0
;
SET_PLOT, 'ps'
;
device, xsize=10
if !D.X_SIZE ne 10000 then begin
    message, '!D.X_SIZE ne 10000', /conti
    errors++
endif
;
device, ysize=20
if !D.Y_SIZE ne 20000 then begin
    message, '!D.Y_SIZE ne 20000', /conti
    errors++
endif
;
device, xsize=10, /inches
if !D.X_SIZE ne 25400 then begin
    message, '!D.X_SIZE ne 25400', /conti
    errors++
endif
;
device, ysize=20, /inches
if !D.Y_SIZE ne 50800 then begin
    message, '!D.Y_SIZE ne 50800', /conti
    errors++
endif
;
; we check also if !D is updated ...
;
print, '!D in PS mode ToDo'
;
INTERNAL_MESSAGES, 'TEST_DEVICE_X', errors
;
if N_PARAMS() GT 0 then nb_errors=nb_errors+errors
;
SET_PLOT, 'X'
if (strlowcase(!d.name) NE 'x') then begin
    MESSAGE, /continue, 'Unable to switch to X display mode'
    return
endif
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
print, 'Calling TEST_DEVICE_X'
TEST_DEVICE_X, nb_errors, test=test, verbose=verbose
;
print, 'Calling TEST_DEVICE_PS'
TEST_DEVICE_PS, nb_errors, test=test, verbose=verbose
;
if (nb_errors EQ 0) then begin
    MESSAGE, /continue, 'No error found in TEST_DEVICE'
endif else begin
    MESSAGE, /continue, STRING(nb_errors)+' errors found in TEST_DEVICE'
    if ~KEYWORD_SET(no_exit) then EXIT, status=1
endelse
;
if KEYWORD_SET(test) then STOP
;
end
