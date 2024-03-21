;
; AC 2024-Feb-27
;
; more systematics tests on loops
; showing the serious/critical issue on OSX ...
;
; the procedure PLOT_BENCH_LOOPS for plot
; is fully derived from the one PLOT_BASIC_BENCHMARKS
; located in same directory
;
; on my laptop :
; BENCH_LOOPS,/save
; BENCH_LOOPS,/save, nb=1e6
; PLOT_BENCH_LOOPS,/ylo, yrang=[0.001,0.5]
;
; --------------------------------------------------------------
;
function LISTE_OF_NBP, liste, debug=debug
nbps=0L
for ii=0, N_elements(liste)-1 do begin
   RESTORE, liste[ii]
   if KEYWORD_SET(debug) then print, nbp, liste[ii]
   nbps=[nbps, nbp]
endfor
nbps=nbps[1:*]
return, nbps[UNIQ(nbps, SORT(nbps))]
end
;
pro SYMBOLS_TO_BE_USED, nbp, nbps, family_type, psymb, my_line, test=test
;
calcul=MIN(ABS(nbps-nbp), indice)
;
family_type=STRING(nbps[indice], format='(g10.5)')
case indice of
   0 : begin
      psymb=4
      my_line=0
   end
   1 : begin
      psymb=6
      my_line=2
   end
   2 : begin
      psymb=2
      my_line=1
   end
ENDCASE
;
if KEYWORD_SET(test) then STOP
;
end
;
pro PLOT_BENCH_LOOPS, path=path, filter=filter, type=type, $
                      select=select, title=title, $
                      xrange=xrange, yrange=yrange, $
                      xlog=xlog, ylog=ylog, $
                      svg=svg, special=special, $
                      help=help, test=test, debug=debug, verbose=verbose
;
if KEYWORD_SET(help) then begin
    print, 'pro PLOT_BENCH_LOOPS, path=path, filter=filter, $'
    print, '                      xrange=xrange, yrange=yrange, $'
    print, '                      xlog=xlog, ylog=ylog, $'
    print, '                      svg=svg, $'
    print, '                      help=help, test=test'
    return
endif
;
if ~KEYWORD_SET(test) then ON_ERROR, 2
;
CHECK_SAVE_RESTORE
;
if ~KEYWORD_SET(filter) then filter='bench_loops*.xdr'
;
liste=BENCHMARK_FILE_SEARCH(filter, 'bench_loops', path=path)
if N_ELEMENTS(liste) EQ 0 then begin
   print, 'Used filter : ', filter
   MESSAGE, 'No file found !'
endif
;
; name for the output, if needed (+ type name if selected)
;
RESTORE, liste[0]
fig_name='bench_loops_'
;if STRLEN(keep_type) GT 0 then fig_name=fig_name+keep_type+'_'
fig_name=fig_name+info_os.generic_osname+'_'+info_os.hostname+'.svg'
BENCHMARK_SVG, svg=svg, /on, filename=fig_name, infosvg=infosvg 
;
; not useful now, maybe later ...
if KEYWORD_SET(xrange) then MESSAGE,/info, "keyword Xrange not effective !"
if KEYWORD_SET(xlog) then MESSAGE,/info, "keyword Xrange not effective !"
;
;; Which ranges ? fake Xrange (we don't care)
BENCHMARK_COMPUTE_RANGE, liste, xrange, yrange_new, 'nbp', 'op_val', $
                         xelems=xelems, yelems=yelems
if ~KEYWORD_SET(yrange) then yrange=yrange_new
;
xelems=N_ELEMENTS(op_name)
;
; in fact, we overwrite xrange ...
xval=INDGEN(xelems)+1
xrange=[0, xelems+1]
;
DEVICE, decompose=1
;
; reading a first time all the XDR in liste to get infos
BENCHMARK_GRAPHIC_STYLE, liste, colors, mypsym, myline, flags, $
                         languages, select=select
;
bb_title='Basic Benchmark on Loops'
if KEYWORD_SET(title) then bb_title=bb_title+' : '+title
;
PLOT, FINDGEN(xelems), /nodata, xstyle=5, /ystyle, ylog=ylog, $
      xrange=xrange, yrange=yrange, $
      xtitle='Operations', ytitle='time [s]', $
      title=bb_title, pos=[0.07,0.05, 0.75, 0.9]
;
XYOUTS, 0.78, 0.95, SYSTIME(), /normal
XYOUTS, 0.78, 0.90, info_os.hostname, /normal
XYOUTS, 0.78, 0.85, info_os.generic_osname,/normal
XYOUTS, 0.78, 0.80, info_os.family_osname+' '+info_os.accurate_osname,/normal
XYOUTS, 0.78, 0.75, info_cpu.model,/normal
XYOUTS, 0.78, 0.70, 'nb cores : '+STRING(info_cpu.nb_cores), /normal
;
liste_nbps=LISTE_OF_NBP(liste)
;
for ii=0, N_ELEMENTS(liste)-1 do begin
   if KEYWORD_SET(verbose) then print, 'Restoring '+liste[ii]
   RESTORE, liste[ii]
   ;;
   ;; at the end, plotting missing Xaxis !
   if ii EQ 0 then $
      AXIS, xtickname=['',op_name,''], xticks=N_ELEMENTS(op_name)+1
   ;;
   ;; type ?
   ;;
   SYMBOLS_TO_BE_USED, nbp, liste_nbps, family_type, psymb, my_line, test=test
   ;;
   ;; skipped unwanted inputs if Type= provided
   if KEYWORD_SET(type) then if (keep_type NE family_type) then continue
   ;;
   jj=flags[ii]
   if jj GE 0 then begin
      OPLOT, xval, op_val, line=my_line, col=colors[jj], thick=2
      OPLOT, xval, op_val, psym=psymb, col=colors[jj], thick=2
   endif
   ;;   print, ii, jj, mypsym[jj], myline, colors[jj]
endfor
;
; adding vertical lines separating the 4 cases
;
BENCHMARK_PLOT_CARTOUCHE, pos=[0.75,0.05,0.98,0.4], languages, /box, $
                          colors=colors, lines=lines, /all, /absolute, $
                          thick=2, title='Languages'
;
; populate symbols lists
nbs=N_ELEMENTS(liste_nbps)
ls=INTARR(nbs)
ss=STRARR(nbs)
psym=INTARR(nbs)
for ii=0, nbs-1 do begin
   SYMBOLS_TO_BE_USED, liste_nbps[ii], liste_nbps, family_type, psymb, my_line
   ss[ii]=family_type
   ls[ii]=my_line
   psym[ii]=-psymb
endfor
;
BENCHMARK_PLOT_CARTOUCHE, pos=[0.75,0.4,0.98,0.7], ss, /box, $
    ;                      colors=color2color(!color.white), $
                          linestyle=ls, psym=psym, /all, /absolute, $
                          thick=2, title='Types'
;
if KEYWORD_SET(special) then begin
   colors=[colors[0],colors[0]]
   info=['git_11','git_19']
   lines=[2,0]
   BENCHMARK_PLOT_CARTOUCHE, pos=[0.75,0.55,0.98,0.95], info, /box, $
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


; --------------------------------------------------------------
; begin og the core code for benchmarking
; ----------------- REPEAT ------
;
pro TEST_LOOP_REPEAT, dt, nbp=nbp, quiet=quiet
;
if ~KEYWORD_SET(nbp) then nbp=100000L
;
a=0L
TIC
;
repeat begin
   a=a+1L
endrep until A GT nbp
;
dt=TOC()
if ~KEYWORD_SET(quiet) then print, 'time loop REPEAT :', dt
;
end
;
; ----------------- WHILE ------
;
pro TEST_LOOP_WHILE, dt, nbp=nbp, quiet=quiet
;
if ~KEYWORD_SET(nbp) then nbp=100000L
;
a=0L
;
TIC
;
While (a LT nbp) do begin
   a=a+1L
endwhile
;
dt=TOC()
if ~KEYWORD_SET(quiet) then print, 'time loop WHILE :', dt
;
end
;
; ----------------- FOR ------
;
pro TEST_LOOP_FOR, dt, nbp=nbp, quiet=quiet
;
if ~KEYWORD_SET(nbp) then nbp=100000L
;
a=0L
;
TIC
;
for ii=0, nbp do begin
   a=a+1L
endfor
;
dt=TOC()
if ~KEYWORD_SET(quiet) then print, 'time loop FOR :', dt
;
end
;
; ----------------- FOREACH ------
;
pro TEST_LOOP_FOREACH, dt, nbp=nbp, quiet=quiet
;
if ~KEYWORD_SET(nbp) then nbp=100000L
;
a=LINDGEN(nbp)
;
TIC
;
foreach elem, a do begin
   a[elem]=0
endforeach
;
if TOTAL(a) GT 0 then MESSAGE, 'pb here'
;
dt=TOC()
if ~KEYWORD_SET(quiet) then print, 'time loop FOREACH :', dt
;
end
;
; ----------------- call all of them ------
;
pro BENCH_LOOPS, nbp=nbp, quiet=quiet, display=display, $
                 help=help, save=save, test=test
;
if KEYWORD_SET(help) then begin
   print, 'pro BENCH_LOOPS, nbp=nbp, quiet=quiet, display=display, $'
   print, '                 help=help, save=save, test=test'
   return
endif
;
if KEYWORD_SET(save) then CHECK_SAVE_RESTORE

if ~KEYWORD_SET(nbp) then begin
   nbp=100000L
   print, 'Running tests for default value : ', nbp
endif else print, 'Running tests for value : ', nbp
nbp=LONG(nbp)
;
TEST_LOOP_REPEAT, dt1, nbp=nbp, quiet=quiet
TEST_LOOP_WHILE, dt2, nbp=nbp, quiet=quiet
TEST_LOOP_FOR, dt3, nbp=nbp, quiet=quiet
TEST_LOOP_FOREACH, dt4, nbp=nbp, quiet=quiet
;
op_name=['REPEAT','WHILE', 'FOR', 'FOREACH']
op_val=[dt1,dt2,dt3,dt4]
;
if KEYWORD_SET(display) then begin
   plot, times, title='nbp='+string(nbp)
endif
;
if KEYWORD_SET(save) then begin
   radical='loops'
   filename=BENCHMARK_GENERATE_FILENAME(radical)
   ;;
   info_cpu=BENCHMARK_INFO_CPU()
   info_os=BENCHMARK_INFO_OS()
   info_soft=BENCHMARK_INFO_SOFT()
   ;;
   SAVE, file=filename, nbp, op_name, op_val, $
      info_cpu, info_os, info_soft
endif
;
if KEYWORD_SET(test) then STOP
;
end

