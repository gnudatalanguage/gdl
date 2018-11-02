;
; Under GNU GPL V3+
; Alain C., August 2018
;
; Nota Bene : we don't check all the XDR provided come from the
; same machine ...
;
; ---------------------------------
; 
; Modifications history :
;
; - 2018-10-15 : AC. 
;   * adding on the plot more informations on the machine used
;   * better display of Float & Double
;   * filtering on used type (eg float of dlb ...)
;
; ---------------------------------
;
pro PLOT_BASIC_BENCHMARKS, path=path, filter=filter, type=type, $
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
; selection on type
;
if KEYWORD_SET(type) then keep_type=type else keep_type=''
if ISA(keep_type,/string) then begin
   keep_type=STRUPCASE(keep_type)
endif else begin
   keep_type=TYPENAME(FIX(1, type=keep_type))
endelse
;
; name for the output, if needed (+ type name if selected)
;
RESTORE, liste[0]
fig_name='bench_basic_'
if STRLEN(keep_type) GT 0 then fig_name=fig_name+keep_type+'_'
fig_name=fig_name+info_os.generic_osname+'_'+info_os.hostname+'.svg'
BENCHMARK_SVG, svg=svg, /on, filename=fig_name, infosvg=infosvg 
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
; reading a first time all the XDR in liste to get infos
BENCHMARK_GRAPHIC_STYLE, liste, colors, mypsym, myline, flags, $
                         languages, select=select
;
PLOT, FINDGEN(10), /nodata, xstyle=4, /ystyle, ylog=ylog, $
      xrange=xrange, yrange=yrange, $
      xtitle='Operations', ytitle='time [s]', $
      title='Basic Benchmark', pos=[0.05,0.05, 0.75, 0.9]
;
XYOUTS, 0.78, 0.95, SYSTIME(), /normal
XYOUTS, 0.78, 0.90, info_os.hostname, /normal
XYOUTS, 0.78, 0.85, info_os.generic_osname,/normal
XYOUTS, 0.78, 0.80, info_os.family_osname+' '+info_os.accurate_osname,/normal
XYOUTS, 0.78, 0.75, info_cpu.model,/normal
XYOUTS, 0.78, 0.70, 'nb cores : '+STRING(info_cpu.nb_cores), /normal
;
for ii=0, N_ELEMENTS(liste)-1 do begin
   if KEYWORD_SET(verbose) then print, 'Restoring '+liste[ii]
   RESTORE, liste[ii]
   ;;
   ;; at the end, plotting missing Xaxis !
   if ii EQ 0 then $
      AXIS, xtickname=['',op_name], xticks=N_ELEMENTS(op_name)+1
   ;;
   ;; type ?
   if (used_type EQ 'FLOAT') then psymb=4
   if (used_type EQ 'FLOAT') then my_line=0
   if (used_type EQ 'DOUBLE') then psymb=6
   if (used_type EQ 'DOUBLE') then my_line=2
   ;;
   ;; skipped unwanted inputs if Type= provided
   if KEYWORD_SET(type) then if (keep_type NE used_type) then continue
   ;;
   jj=flags[ii]
   if jj GE 0 then begin
      OPLOT, xval, op_val, line=my_line, col=colors[jj], thick=1.5
      OPLOT, xval, op_val, psym=psymb, col=colors[jj], thick=1.5
   endif
   ;;   print, ii, jj, mypsym[jj], myline, colors[jj]
endfor
;
; adding vertical lines separating the 4 cases
;
BENCHMARK_PLOT_CARTOUCHE, pos=[0.75,0.05,0.98,0.4], languages, /box, $
                          colors=colors, lines=lines, /all, /absolute, $
                          thick=1.5, title='Languages'
;
BENCHMARK_PLOT_CARTOUCHE, pos=[0.75,0.4,0.98,0.6], ['Float','Double'], /box, $
;                          colors=color2color(!color.white), $
                          linestyle=[0,2], psym=[-4,-6], /all, /absolute, $
                          thick=1.5, title='Types'
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
