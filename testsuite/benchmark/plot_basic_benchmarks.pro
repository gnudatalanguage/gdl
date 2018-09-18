;
; August 2018
;
pro PLOT_BASIC_BENCHMARKS, path=path, filter=filter, $
                           select=select, $
                           xrange=xrange, yrange=yrange, $
                           xlog=xlog, ylog=ylog, $
                           svg=svg, special=special, $
                           help=help, test=test
;
if KEYWORD_SET(help) then begin
    print, 'pro PLOT_BASIC_BENCHMARKS, path=path, filter=filter, $'
    print, '                           xrange=xrange, yrange=yrange, $'
    print, '                           xlog=xlog, ylog=ylog, $'
    print, '                           svg=svg, $'
    print, '                           help=help, test=test'
    return
endif
;
ON_ERROR, 2
;
CHECK_SAVE_RESTORE
;
if ~KEYWORD_SET(filter) then filter='bench_basic_*.xdr'
;
liste=BENCHMARK_FILE_SEARCH(filter, 'BASIC_BENCHMARKS', path=path)
if N_ELEMENTS(liste) EQ 0 then begin
   print, 'Used filter : ', filter
   MESSAGE, 'No file found !'
endif
;
BENCHMARK_SVG, svg=svg, /on, filename='bench_basic_bench.svg', infosvg=infosvg 
;
; not useful now, maybe later ...
if KEYWORD_SET(xrange) then MESSAGE,/info, "keyword Xrange not effective !"
if KEYWORD_SET(xlog) then MESSAGE,/info, "keyword Xrange not effective !"
;
;; Which ranges ? fake Xrange (we don't care)
BENCHMARK_COMPUTE_RANGE, liste, xrange, yrange_new, 'op_val', 'op_val', $
                         xelems=xelems, yelems=yelems
if ~KEYWORD_SET(yrange) then yrange=yrange_new
;
; in fact, we overwrite xrange ...
xval=INDGEN(xelems)+1
xrange=[0, xelems+1]
;
DEVICE, decompose=1
;
BENCHMARK_GRAPHIC_STYLE, liste, colors, mypsym, myline, flags, languages, $
                         select=select
;
PLOT, FINDGEN(10), /nodata, xstyle=4, /ystyle, ylog=ylog, $
      xrange=xrange, yrange=yrange, $
      xtitle='Operations', ytitle='time [s]', $
      title='Basic Benchmark', pos=[0.05,0.05, 0.85, 0.9]
;
for ii=0, N_ELEMENTS(liste)-1 do begin
   print, 'Restoring '+liste[ii]
   RESTORE, liste[ii]
   ;; ordering data stored in XDR files
   jj=flags[ii]
   if ii EQ 0 then $
      AXIS, xtickname=['',op_name], xticks=N_ELEMENTS(op_name)+1
   
   my_line=myline[jj]
   if KEYWORD_SET(special) and ii GT 0 then my_line=0

   if jj GE 0 then begin
      OPLOT, xval, op_val, psym=mypsym[jj], line=my_line, col=colors[jj]
      OPLOT, xval, op_val, psym=-mypsym[jj], col=colors[jj]
   endif
   print, ii, jj, mypsym[jj], myline, colors[jj]
endfor
;
; adding vertical lines separating the 4 cases
;
BENCHMARK_PLOT_CARTOUCHE, pos=[0.85,0.05,0.98,0.5], languages, /box, $
                          colors=colors, lines=lines, /all, /absolute, $
                          thick=1.5, title='Languages'

if KEYWORD_SET(special) then begin
   colors=[colors[0],colors[0]]
   info=['git_11','git_19']
   lines=[2,0]
   BENCHMARK_PLOT_CARTOUCHE, pos=[0.85,0.55,0.98,0.95], info, /box, $
                             colors=colors, lines=lines, /all, /absolute, $
                             thick=1.5, title='Git version'
endif
;
BENCHMARK_SVG, svg=svg, /off, infosvg=infosvg
;
if KEYWORD_SET(test) then STOP
;
end
;
