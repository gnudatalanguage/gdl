;
; AC July 2017. Derived work from "test_invert_matrix.pro"
; Basic benchmark on matrix inversion.
;
; pro BENCH_INVERT_MATRIX
; - inversion of RANDOMU matrix of different sizes. The /Save keyword
;   will allow you to save the results and intercompare on different
;   computers and with IDL
;
; BENCH_MATRIX_INVERT, nb=10, star=1000, step=100, /save
;
; ------------
; Modifications history :
;
; --------------------------------------------------------------
;
pro PLOT_BENCH_MATRIX_INVERT, xrange=xrange, yrange=yrange, $
                              path=path, svg=svg, $
                              xlog=xlog, ylog=ylog, test=test, help=help
;
if KEYWORD_SET(help) then begin
   print, 'pro PLOT_BENCH_MATRIX_INVERT, xrange=xrange, yrange=yrange, $'
   print, '                              path=path, svg=svg, $'
   print, '                              xlog=xlog, ylog=ylog, $'
   print, '                              test=test, help=help'
   return
end
;
ON_ERROR, 2
;
CHECK_SAVE_RESTORE
;
liste=BENCHMARK_FILE_SEARCH('bench_invert*.xdr', 'Invert Matrix', path=path)
;
BENCHMARK_SVG, svg=svg, /on, filename='bench_invert.svg', infosvg=infosvg 
;
; Which ranges ?
;
if keyword_set(xrange) then xmax=xrange[1] else xmax=0
if keyword_set(yrange) then ymax=yrange[1] else ymax=0
;
BENCHMARK_COMPUTE_RANGE, liste, xrange_data, yrange_data, $
                     'size_index', 'resu_inv', xmax=xmax, ymax=ymax
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
      xtitle='Matrix size', ytitle='Inversion time [s]', $
      xlog=xlog, ylog=ylog, title='INVERT() matrix benchmark'
;
for ii=0, N_ELEMENTS(liste)-1 do begin
   print, 'Restoring '+liste[ii]
   RESTORE, liste[ii]
   jj=flags[ii]
   OPLOT, size_index, resu_inv, $
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
pro BENCH_MATRIX_INVERT, nb_matrix=nb_matrix, seed=seed, $
                         start_size=start_size, step=step, $
                         save=save, double=double, $
                         verbose=verbose, test=test, help=help
;
if KEYWORD_SET(help) then begin
   print, 'pro BENCH_MATRIX_INVERT, nb_matrix=nb_matrix, seed=seed, $'
   print, '                         start_size=start_size, step=step, $'
   print, '                         save=save, double=double, $'
   print, '                         verbose=verbose, test=test, help=help'
   return
endif
;
if KEYWORD_SET(save) then CHECK_SAVE_RESTORE
;
if KEYWORD_SET(double) then radical='invert_d' else radical='invert'
;
if (N_ELEMENTS(nb_matrix) EQ 0) then nb_matrix=5
if (N_ELEMENTS(start_size) EQ 0) then start_size=100
if (N_ELEMENTS(step) EQ 0) then step=100
if (N_ELEMENTS(seed) EQ 0) then seed=11
;
resu_gen=FLTARR(nb_matrix)
resu_inv=FLTARR(nb_matrix)
size_index=INTARR(nb_matrix)
;
for ii=0, nb_matrix-1 do begin
   ;; size of the square array (matrix)
   nbp=start_size+ii*step
   size_index[ii]=nbp
   print, '---------------------------------'
   print, 'Matrix size : ', nbp
   time0=SYSTIME(1)
   ;; generating the input array
   a=RANDOMU(seed, nbp, nbp, double=double)
   time1=SYSTIME(1)
   print, 'Generation time: ', time1-time0
   b=INVERT(a, status)
   print, 'status :', status
   time2=SYSTIME(1)
   print, 'Inversion time: ', time2-time1
   resu_gen[ii]=time1-time0
   resu_inv[ii]=time2-time1
endfor
;
PLOT, size_index, resu_inv, xtitle='Matrix size', ytitle='Inversion time [s]'
OPLOT, size_index, resu_gen
;
if KEYWORD_SET(save) then begin
   cpuinfo=BENCHMARK_GENERATE_CPUINFO()
   filename=BENCHMARK_GENERATE_FILENAME(radical)   
   SAVE, filename=filename, cpuinfo, size_index, resu_gen, resu_inv
endif
;
if KEYWORD_SET(test) then STOP
;
end
