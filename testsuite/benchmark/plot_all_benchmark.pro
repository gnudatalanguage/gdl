;
; AC, August 18th 2017
;
; Design to be run in any interpretors (Default X11 plots)
; but GDL only if /SVG, due to SVG output
;
pro PLOT_ALL_BENCHMARK, svg=svg, test=test, path=path
;
ON_ERROR, 2
;
CHECK_SAVE_RESTORE
;
if KEYWORD_SET(svg) then begin
   if GDL_IDL_FL(/up) NE 'GDL' then MESSAGE, "SVG output only for GDL, sorry"
endif else begin
   ;; do we have the X11 (or Win) mode ON ??
   if ~CHECK_IF_DEVICE_IS_OK('PLOT_ALL_BENCHMARK', /force) then $
      MESSAGE, "X11 not ready/operational on this computer (see /SVG mode ;)"
endelse
;
; we do these "EXECUTE()" below to compile the plotting functions 
; which are inside the various associated benchmark codes ... 
;
a=EXECUTE('BENCH_FFT, /help')
a=EXECUTE('BENCH_MATRIX_INVERT, /help')
a=EXECUTE('BENCH_MATRIX_MULTIPLY, /help')
a=EXECUTE('BENCH_MEDIAN, /help')
;
if ~KEYWORD_set(svg) then WINDOW, 0
PLOT_BENCH_FFT, svg=svg, path=path
;
if ~KEYWORD_set(svg) then WINDOW, 1
PLOT_BENCH_MATRIX_INVERT, svg=svg, path=path
;
if ~KEYWORD_set(svg) then WINDOW, 2
PLOT_BENCH_MATRIX_MULTIPLY, svg=svg, path=path
;
if ~KEYWORD_set(svg) then WINDOW, 3
PLOT_BENCH_MEDIAN, svg=svg, path=path
;
if KEYWORD_SET(test) then STOP
;
end
