;
; AC 03/03/2007
; Quick time_tests for FFTw (or not !)
;
; --------------
; Modifications history :
;
; --------------------------------------------------------------
;
pro PLOT_BENCH_FFT, xrange=xrange, yrange=yrange, $
                    xmini=xmini, ymini=ymini, $
                    path=path, svg=svg, $
                    norm=norm, test=test, help=help
;
if KEYWORD_SET(help) then begin
   print, 'pro PLOT_BENCH_FFT, xrange=xrange, yrange=yrange, $'
   print, '                    xmini=xmini, ymini=ymini, $'
   print, '                    path=path, svg=svg, $'
   print, '                    norm=norm, test=test, help=help'
   return
endif
;
ON_ERROR, 2
;
CHECK_SAVE_RESTORE
;
liste=BENCHMARK_FILE_SEARCH('bench_fft*.xdr', 'FFT', path=path)
;
BENCHMARK_SVG, svg=svg, /on, filename='bench_fft.svg', infosvg=infosvg 
;
if KEYWORD_SET(xrange) then xmax=xrange[1] else xmax=0
if KEYWORD_SET(yrange) then ymax=yrange[1] else ymax=0
;
BENCHMARK_COMPUTE_RANGE, liste, xrange_data, yrange_data, 'x', 'st', $
                     xmax=xmax, ymax=ymax
;
if ~KEYWORD_SET(xrange) then xrange=xrange_data
if ~KEYWORD_SET(yrange) then yrange=yrange_data
;
; we need to adjust due to log-log plot
if ~KEYWORD_SET(xmini) then xmini=2 
if ~KEYWORD_SET(ymini) then ymini=1e-6
if (xrange[0] LT xmini) then xrange[0]=xmini
if (yrange[0] LT ymini) then yrange[0]=ymini
;
DEVICE, decompose=1
;
BENCHMARK_GRAPHIC_STYLE, liste, colors, mypsym, myline, flags, languages
;
PLOT, FINDGEN(10), /nodata, /xlog, /ylog, /xstyle, /ystyle, $
      xrange=xrange, yrange=yrange, $
      xtitle='FFT power', ytitle='FFT time [s]', title='FFT() benchmark'
;
for ii=0, N_ELEMENTS(liste)-1 do begin
   print, 'Restoring '+liste[ii]
   RESTORE, liste[ii]
   if KEYWORD_SET(norm) then st=st*1e3/resu_mhz
   ;;
   jj=flags[ii]
   OPLOT, x, st, psym=mypsym[jj], line=myline[jj], col=colors[jj]
endfor
;
BENCHMARK_PLOT_CARTOUCHE, pos=['rb'], languages, /box, colors=colors, $
                      lines=lines, thick=1.5, title='Languages'
;
BENCHMARK_SVG, svg=svg, /off, infosvg=infosvg
;
if KEYWORD_SET(test) then STOP
;
end
;
; --------------------------------------------------------------
;
pro BENCH_FFT, power_max=power_max, double=double, $
               display=display, help=help, save=save, test=test
;
if KEYWORD_SET(help) then begin
   print, 'pro BENCH_FFT, power_max=power_max, double=double, $'
   print, '                display=display, help=help, save=save, test=test'
   return
endif
;
if KEYWORD_SET(save) then CHECK_SAVE_RESTORE
;
; The following limits use 3 Gb of RAM after call
; but need up to +5 Gb during computation --> may swap
;
if (N_ELEMENTS(power_max) NE 1) then power_max=28
;
if power_max GT 31 then begin
   MESSAGE, "[Power_max=] was to large. Set back to 31", /continue
   power_max=31
endif
;
; store times ...
st=DBLARR(power_max-1)
x=DBLARR(power_max-1)
;
print, 'Using POWER_MAX=', power_max
print, '    power_max,    nbp, time'
;
for ii=2, power_max do begin
   nbp=2L^ii
   a=randomu(seed,nbp)
   t0=SYSTIME(1)
   b=FFT(a, double=double)
   t1=SYSTIME(1)-t0
   x[ii-2]=nbp
   st[ii-2]=t1
   print, ii, nbp, t1
endfor
;
if KEYWORD_SET(display) then begin
   plot, x, st, /xlog,/ylog, min=1e-6, psym=2
endif
;
if KEYWORD_SET(save) then begin
   if KEYWORD_SET(double) then radical='fft_d' else radical='fft'
   cpuinfo=BENCHMARK_GENERATE_CPUINFO()
   filename=BENCHMARK_GENERATE_FILENAME(radical)   
   ;;
   SAVE, file=filename, cpuinfo, x, st
endif
;
if KEYWORD_SET(test) then STOP
;
end
