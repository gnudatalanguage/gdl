;
; AC 2024-mars-13
;
; Regression in the GET_TICK as reported in issue #1778
; https://github.com/gnudatalanguage/gdl/issues/1778
;
; As usual in tests, errors flags can be triggered more that one time
; but only one bug is detected ! We have a special situation here
; than some calls don't return values ... 
;
; These tests are also tests on the ranges computed for PLOT & SURFACE ...
;
; Modifications history :
; -2024-03-29 AC : adding more cases for PLOT since #1785 don't work well
;
; -------------------------------------------------
; simple debug print when needed ...
pro MODE_DEBUG, exp, val, valname, debug=debug, test=test
if KEYWORD_SET(debug) then begin
   svalname=STRING(valname, format='(A9)')+':'
   print, 'expected :', STRING(exp, form='(g10.5)')
   print, svalname,     STRING(val, form='(g10.5)')
endif
if KEYWORD_SET(test) then STOP
end
; -------------------------------------------------
;
pro TEST_TICK_GET_PLOT, cumul_errors, debug=debug, test=test, verbose=verbose
;
errors=0
;
; testing only XTICK_GET
;
command='plot, FINDGEN(10), xtick_get=xt'
res=EXECUTE(command)
if KEYWORD_SET(verbose) or KEYWORD_SET(debug) then print, command 
;
if (res EQ 0) then ERRORS_ADD, errors, 'bad execution, X fields only'
;
if ISA(xt) then begin
   expected=2.*DINDGEN(6)
   if ~ARRAY_EQUAL(expected, xt) then ERRORS_ADD, errors, 'bad X-axis (1)'
   MODE_DEBUG, expected, xt, 'X axis', debug=debug
endif else ERRORS_ADD, errors, 'undefined xtick_get (X-axis (1))'
;
; testing only YTICK_GET
;
command='plot, -FINDGEN(10), ytick_get=yt'
res=EXECUTE(command)
if KEYWORD_SET(verbose) or KEYWORD_SET(debug) then print, command 
;
if (res EQ 0) then ERRORS_ADD, errors, 'bad execution, Y fields only'
;
if ISA(yt) then begin
   expected=-10.+2.*DINDGEN(6)
   if ~ARRAY_EQUAL(expected, yt) then ERRORS_ADD, errors, 'bad Y-axis (1)'
   MODE_DEBUG, expected, yt, 'Y axis', debug=debug
endif else ERRORS_ADD, errors, 'undefined ytick_get (Y-axis (1))'
;
; ----- testing both
;
command='plot, FINDGEN(10), xtick_get=xt, ytick_get=yt'
res=EXECUTE(command)
if KEYWORD_SET(verbose) or KEYWORD_SET(debug) then print, command 
;
if (res EQ 0) then ERRORS_ADD, errors, 'bad execution, two fields'
;
if ISA(xt) AND ISA(yt) then begin
   expected=2.*DINDGEN(6)
   if ~ARRAY_EQUAL(expected, xt) then ERRORS_ADD, errors, 'bad X-axis'
   if ~ARRAY_EQUAL(expected, yt) then ERRORS_ADD, errors, 'bad Y-axis'
   MODE_DEBUG, expected, xt, 'X axis', debug=debug
   MODE_DEBUG, expected, yt, 'Y axis', debug=debug
endif else ERRORS_ADD, errors, 'undefined xtick_get OR ytick_get'
;
; ----- another ranges v2 ...
;
command='plot, FINDGEN(63)-500, xtick_get=xt, ytick_get=yt'
res=EXECUTE(command)
if KEYWORD_SET(verbose) or KEYWORD_SET(debug) then print, command 
;
if (res EQ 0) then ERRORS_ADD, errors, 'bad execution, two fields, v2'
;
if ISA(xt) AND ISA(yt) then begin
   exp_x=20.*DINDGEN(5)
   exp_y=-500+20.*DINDGEN(5)
   if ~ARRAY_EQUAL(exp_x, xt) then ERRORS_ADD, errors, 'bad X-axis'
   if ~ARRAY_EQUAL(exp_y, yt) then ERRORS_ADD, errors, 'bad Y-axis'
   MODE_DEBUG, exp_x, xt, 'X axis', debug=debug
   MODE_DEBUG, exp_y, yt, 'Y axis', debug=debug
endif else ERRORS_ADD, errors, 'undefined xtick_get OR ytick_get'
;
; ----- another ranges v2 ...
;
command='plot, FINDGEN(71)-500, xtick_get=xt, ytick_get=yt'
res=EXECUTE(command)
if KEYWORD_SET(verbose) or KEYWORD_SET(debug) then print, command 
;
if (res EQ 0) then ERRORS_ADD, errors, 'bad execution, two fields, v3'
;
if ISA(xt) AND ISA(yt) then begin
   exp_x=20.*DINDGEN(5)
   exp_y=-500+20.*DINDGEN(5)
   if ~ARRAY_EQUAL(exp_x, xt) then ERRORS_ADD, errors, 'bad X-axis'
   if ~ARRAY_EQUAL(exp_y, yt) then ERRORS_ADD, errors, 'bad Y-axis'
   MODE_DEBUG, exp_x, xt, 'X axis', debug=debug
   MODE_DEBUG, exp_y, yt, 'Y axis', debug=debug
endif else ERRORS_ADD, errors, 'undefined xtick_get OR ytick_get'
;
; --------------
;
BANNER_FOR_TESTSUITE, "TEST_TICK_GET_PLOT", errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; ----------------------------
;
pro TEST_TICK_GET_SURFACE, cumul_errors, test=test, debug=debug, verbose=verbose
;
errors=0
;
; expected values
data1=DIST(53,63)
exp_x1=10.*DINDGEN(7)
exp_y1=20.*DINDGEN(5)
exp_z1=10.*DINDGEN(6)
;
; one field
; (we use xti1/yti1/zti1 to avoid mixing with next vars, in case of
; undefined ...)
;
if KEYWORD_SET(debug) then print, "Testing ONE in the THREE fields"
;
res1=EXECUTE('SURFACE, data1, xtick_get=xti1')
res2=EXECUTE('SURFACE, data1, ytick_get=yti1')
res3=EXECUTE('SURFACE, data1, ztick_get=zti1')
if (res1 EQ 0) then ERRORS_ADD, errors, 'bad execution, one field, X case'
if (res2 EQ 0) then ERRORS_ADD, errors, 'bad execution, one field, Y case'
if (res3 EQ 0) then ERRORS_ADD, errors, 'bad execution, one field, Z case'
if ISA(xti1) then begin
   if ~ARRAY_EQUAL(xti1, exp_x1) then ERRORS_ADD, errors, 'bad X-axis value'
   MODE_DEBUG, exp_x1, xti1, 'X axis', debug=debug
endif else ERRORS_ADD, errors, 'undefined XTICK_GET, one field, X case'
if ISA(yti1) then begin
   if ~ARRAY_EQUAL(yti1, exp_y1) then ERRORS_ADD, errors, 'bad Y-axis value'
   MODE_DEBUG, exp_y1, yti1, 'Y axis', debug=debug
endif else ERRORS_ADD, errors, 'undefined YTICK_GET, one field, Y case'
if ISA(zti1) then begin
   if ~ARRAY_EQUAL(zti1, exp_z1) then ERRORS_ADD, errors, 'bad Z-axis value'
   MODE_DEBUG, exp_z1, zti1, 'Z axis', debug=debug
endif else ERRORS_ADD, errors, 'undefined ZTICK_GET, one field, Z case'
;
; two fields
;
if KEYWORD_SET(debug) then print, "Testing TWO in the THREE fields"
; expected values
data2=DIST(23,73)
exp_x2=5.*DINDGEN(6)
exp_y2=20.*DINDGEN(5)
exp_z2=10.*DINDGEN(5)
;
res1=EXECUTE('SURFACE, data2, xtick_get=xt1, ytick_get=yt1')
res2=EXECUTE('SURFACE, data2, ytick_get=yt2, ztick_get=zt2')
res3=EXECUTE('SURFACE, data2, xtick_get=xt3, ztick_get=zt3')
if (res1 EQ 0) then ERRORS_ADD, errors, 'bad execution, two fields, X&Y case'
if (res2 EQ 0) then ERRORS_ADD, errors, 'bad execution, two fields, Y&Z case'
if (res3 EQ 0) then ERRORS_ADD, errors, 'bad execution, two fields, X&Z case'
;
messcase=' in two fields, X&Y case'
if ISA(xt1) then begin
   if ~ARRAY_EQUAL(exp_x2, xt1) then ERRORS_ADD, errors, 'bad X value'+messcase
   MODE_DEBUG, exp_x2 , xt1, 'X axis', debug=debug
endif else ERRORS_ADD, errors, 'undefined XTICK_GET'+messcase
if ISA(yt1) then begin
   if ~ARRAY_EQUAL(exp_y2, yt1) then ERRORS_ADD, errors, 'bad Y value'+messcase
   MODE_DEBUG, exp_y2, yt1, 'Y axis', debug=debug
endif else ERRORS_ADD, errors, 'undefined YTICK_GET'+messcase
;
messcase=' in two fields, X&Y case'
if ISA(yt2) then begin
   if ~ARRAY_EQUAL(exp_y2, yt2) then ERRORS_ADD, errors, 'bad Y value'+messcase
   MODE_DEBUG, exp_y2, yt2, 'Y axis', debug=debug
endif else ERRORS_ADD, errors, 'undefined YTICK_GET'+messcase
if ISA(zt2) then begin
   if ~ARRAY_EQUAL(exp_z2, zt2) then ERRORS_ADD, errors, 'bad Z value '+messcase
   MODE_DEBUG, exp_z2, zt2, 'Z axis', debug=debug
endif else ERRORS_ADD, errors, 'undefined ZTICK_GET'+messcase
;
messcase=' in two fields, X&Z case'
if ISA(xt3) then begin
   if ~ARRAY_EQUAL(xt3, exp_x2) then ERRORS_ADD, errors, 'bad X value'+messcase
   MODE_DEBUG, xt3, exp_x2, 'X axis', debug=debug
endif else ERRORS_ADD, errors, 'undefined XTICK_GET'+messcase
if ISA(zt3) then begin
   if ~ARRAY_EQUAL(zt3, exp_z2) then ERRORS_ADD, errors, 'bad Z value'+messcase
   MODE_DEBUG, zt2, exp_z2, 'Z axis', debug=debug
endif else ERRORS_ADD, errors, 'undefined ZTICK_GET'+messcase
;
; three field
;
if KEYWORD_SET(debug) then print, "Testing THREE in the THREE fields"
; expected values
exp_x3=20.*DINDGEN(7)
exp_y3=50.*DINDGEN(6)
messcase=' value in three fields'
;
for ii=-1, 1, 1 do begin
   data3=DIST(120,240)+ii*500
   exp_z3=50*DINDGEN(4)+ii*500.
   ;;
   res=EXECUTE('SURFACE, data3, xtick_get=xt, ytick_get=yt, ztick_get=zt')
   ;;
   if (res EQ 0) then ERRORS_ADD, errors, 'bad execution, 3 fields'
   ;;
   if ISA(xt) then begin
      if ~ARRAY_EQUAL(exp_x3, xt) then ERRORS_ADD, errors, 'bad X'+messcase
      MODE_DEBUG, exp_x3, xt, 'X axis', debug=debug
   endif else ERRORS_ADD, errors, 'undefined XTICK_GET'+messcase
   if ISA(yt) then begin
      if ~ARRAY_EQUAL(exp_y3, yt) then ERRORS_ADD, errors, 'bad Y'+messcase
      MODE_DEBUG, exp_y3, yt, 'Y axis', debug=debug
   endif else ERRORS_ADD, errors, 'undefined YTICK_GET'+messcase
   if ISA(zt) then begin
      if ~ARRAY_EQUAL(exp_z3, zt) then ERRORS_ADD, errors, 'bad Z'+messcase
      MODE_DEBUG, exp_z3, zt, 'Z axis', debug=debug
   endif else ERRORS_ADD, errors, 'undefined ZTICK_GET, one field, Z case'
endfor
;
; --------------
;
BANNER_FOR_TESTSUITE, "TEST_TICK_GET_SURFACE", errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; --------------------------------------
;
pro TEST_XYZTICK_GET, devicename=devicename, help=help, test=test, $
                        verbose=verbose, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_PLOT_TICK_GET, devicename=devicename, help=help, $'
   print, '                        test=test, verbose=verbose, no_exit=no_exit'
   return
endif
;
init_devicename=!d.name
;
if ~KEYWORD_SET(devicename) then SET_PLOT, 'Z' else  SET_PLOT, devicename
;
TEST_TICK_GET_PLOT, cumul_errors, test=test, verbose=verbose
TEST_TICK_GET_SURFACE, cumul_errors, test=test, verbose=verbose
;
SET_PLOT, init_devicename
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_XYZTICK_GET', cumul_errors, short=short
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
