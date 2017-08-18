;
; AC August 2017. Derived work from "test_invert_matrix.pro"
; Basic benchmark on MEDIAN() function on 2D array
;
; pro BENCH_MEDIAN
; - The /Save keyword will allow you to save the results
;   and intercompare on different computers and with IDL & FL
;
; BENCH_MEDIAN, nb=10, star=1000, step=100, /save
;
; ----------
; Modification history :
;
; --------------------------------------------------------------
;
pro PLOT_BENCH_MEDIAN, xrange=xrange, yrange=yrange, $
                       path=path, svg=svg, $
                       xlog=xlog, ylog=ylog, test=test, help=help
;
if KEYWORD_SET(help) then begin
   print, 'pro PLOT_BENCH_MEDIAN, xrange=xrange, yrange=yrange, $'
   print, '                       path=path, svg=svg, $'
   print, '                       xlog=xlog, ylog=ylog, $'
   print, '                       test=test, help=help'
   return
end
;
ON_ERROR, 2
;
CHECK_SAVE_RESTORE
;
liste=BENCHMARK_FILE_SEARCH('bench_median*.xdr', 'Invert Matrix', path=path)
;
BENCHMARK_SVG, svg=svg, /on, filename='bench_median.svg', infosvg=infosvg 
;
; Which ranges ?
;
if keyword_set(xrange) then xmax=xrange[1] else xmax=0
if keyword_set(yrange) then ymax=yrange[1] else ymax=0
;
BENCHMARK_COMPUTE_RANGE, liste, xrange_data, yrange_data, $
                     'width_list', 'time_median', xmax=xmax, ymax=ymax
;
if ~KEYWORD_SET(xrange) then xrange=xrange_data
if ~KEYWORD_SET(yrange) then yrange=yrange_data
;
if KEYWORD_SET(xlog) then xrange[0]=10.
if KEYWORD_SET(ylog) then yrange[0]=1.e-4
;
DEVICE, decompose=1
;
BENCHMARK_GRAPHIC_STYLE, liste, colors, mypsym, myline, flags, languages
;
PLOT, 1.e-4*FINDGEN(10), /nodata, xrange=xrange, yrange=yrange, $
      xtitle='Window size', ytitle='time [s]', $
      xlog=xlog, ylog=ylog, title='MEDIAN() on matrix benchmark'
;
for ii=0, N_ELEMENTS(liste)-1 do begin
   print, 'Restoring '+liste[ii]
   RESTORE, liste[ii]
   jj=flags[ii]
   OPLOT, width_list, time_median, $
          psym=mypsym[jj], line=myline[jj], col=colors[jj]
endfor
;
BENCHMARK_PLOT_CARTOUCHE, pos=['lt'], languages, /box, $
            colors=colors, lines=lines, thick=1.5, title='Languages'
;
BENCHMARK_SVG, svg=svg, /off, infosvg=infosvg
;
if KEYWORD_SET(test) then STOP
;
end
;
; --------------------------------------------------------------
;
pro BENCH_MEDIAN, size=size, seed=seed, max_width=max_width, $
                  save=save, double=double, $
                  verbose=verbose, test=test, help=help
;
if KEYWORD_SET(help) then begin
   print, 'pro BENCH_MEDIAN, size=size, seed=seed, max_width=max_width, $'
   print, '                  save=save, double=double, $'
   print, '                  verbose=verbose, test=test, help=help'
   return
endif
;
if KEYWORD_SET(save) then CHECK_SAVE_RESTORE
;
if KEYWORD_SET(double) then radical='median_d' else radical='median'
;
if (N_ELEMENTS(size) EQ 0) then size=1000
if (N_ELEMENTS(seed) EQ 0) then seed=11 ;; not used now
;
if KEYWORD_SET(max_width) then nb_run=max_width else nb_run=20
;
width_list=FLTARR(nb_run)
time_median=FLTARR(nb_run)
;
input=DIST(size)
if KEYWORD_SET(double) then input=DOUBLE(input)
;
for ii=1, nb_run-1 do begin
   width_list[ii]=2*ii
   ;;
   time0=SYSTIME(1)
   b=MEDIAN(input, 2*ii)
   time1=SYSTIME(1)
   ;;
   print, 'width size, time : ', 2*ii, time1-time0
   time_median[ii]=time1-time0
endfor
;
PLOT, width_list, time_median, xtitle='Width values', ytitle='Median time [s]'
;
if KEYWORD_SET(save) then begin
   cpuinfo=BENCHMARK_GENERATE_CPUINFO()
   filename=BENCHMARK_GENERATE_FILENAME(radical)   
   SAVE, filename=filename, cpuinfo, size, width_list, time_median
endif
;
if KEYWORD_SET(test) then STOP
;
end
