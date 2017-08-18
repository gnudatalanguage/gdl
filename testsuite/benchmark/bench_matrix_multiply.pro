;
; AC 25 February 2013
;
; First computation is using "#" operator.
; the 4 computations after are using MATRIX_MULTIPLY() function,
; eventualy calling Eigen3 code (a message will be issued if not)
;
; MODIFICATION HISTORY:
;
; * AC 2017-July: 
; -- Adding /Save to be used in benchmark framework
; -- changing /Dblecomplex into /Dcomplex
; -- renaming be more consistant in naming usage :
;      BENCH_MATRIX_MULTIPLY into BENCH_MATRIX_MULTIPLY_ONE
;      BENCH_MATRIX_MULTIPLY_ALL into BENCH_MATRIX_MULTIPLY
; -- for (D)Complex types, no nul matrix (because FL ultra fast on 0 arrays)
; -- Adding PLOT_BENCH_MATRIX_MULTIPLY
;
; ------------------------------------------------------------
;
pro PLOT_BENCH_MATRIX_MULTIPLY, xrange=xrange, yrange=yrange, $
                                xlog=xlog, ylog=ylog, $
                                path=path, svg=svg, $
                                help=help, test=test
;
if KEYWORD_SET(help) then begin
    print, 'pro PLOT_BENCH_MATRIX_MULTIPLY, xrange=xrange, yrange=yrange, $'
    print, '                                xlog=xlog, ylog=ylog, $'
    print, '                                path=path, svg=svg, $'
    print, '                                help=help, test=test'
    return
endif
;
ON_ERROR, 2
;
CHECK_SAVE_RESTORE
;
liste=BENCHMARK_FILE_SEARCH('bench_matmul*.xdr', 'Matrix Multiply', path=path)
;
BENCHMARK_SVG, svg=svg, /on, filename='bench_matmul.svg', infosvg=infosvg 
;
; not useful now, maybe later ...
if KEYWORD_SET(xrange) then MESSAGE,/info, "keyword Xrange not effective !"
if KEYWORD_SET(xlog) then MESSAGE,/info, "keyword Xrange not effective !"
;
if ~KEYWORD_SET(yrange) then begin
   ;; Which ranges ? fake Xrange (we don't care)
   BENCHMARK_COMPUTE_RANGE, liste, xrange, yrange, 'time_f', 'time_dc'
endif
; in fact, we overwrite xrange ...
; 4 type (f,d,c,dc) & 5 points per type + 5 NaN !
xval=INDGEN(4*5+5)
xrange=[0,MAX(xval)]
;
; extending yrange to add info
;
if ~KEYWORD_SET(ylog) then yrange=[-0.5,yrange[1]] else yrange=[1e-3,yrange[1]]
;
; we need to adjust due to log-log plot
;if ~KEYWORD_SET(xmini) then xmini=2 
;if ~KEYWORD_SET(ymini) then ymini=1e-6
;if (xrange[0] LT xmini) then xrange[0]=xmini
;if (yrange[0] LT ymini) then yrange[0]=ymini
;
DEVICE, decompose=1
;
BENCHMARK_GRAPHIC_STYLE, liste, colors, mypsym, myline, flags, languages
;
PLOT, FINDGEN(10), /nodata, /xstyle, /ystyle, ylog=ylog, $
      xrange=xrange, yrange=yrange, $
      xtitle='f, d, c, dc types', ytitle='Mat. Mul. time [s]', $
      title='Matrix Multiply benchmark'
;
for ii=0, N_ELEMENTS(liste)-1 do begin
   print, 'Restoring '+liste[ii]
   RESTORE, liste[ii]
   ;; ordering data stored in XDR files
   nan=!values.f_nan
   data=[nan,time_f,nan,time_d,nan,time_c,nan,time_dc,nan]
   ;;
   jj=flags[ii]
   OPLOT, xval, data, psym=mypsym[jj], line=myline[jj], col=colors[jj]
endfor
;
; adding vertical lines separating the 4 cases
;
verticals=WHERE(1-FINITE(data), nbv)
xx=xval[verticals]
if KEYWORD_SET(ylog) then yy=10^(!y.crange) else yy=!y.crange
for ii=1, nbv-2 do PLOTS, [xx[ii],xx[ii]], yy, line=2
;
liste_info=['Float','Double','Complex','D Complex']
xpos=3+6*indgen(4)
if KEYWORD_SET(ylog) then ypos=2e-3 else ypos=-0.25
;
for ii=0,3 do XYOUTS, xpos[ii], ypos, liste_info[ii], ali=.5
;
if KEYWORD_SET(ylog) then cpos=[20,23,1e-3,1e-2] else cpos=['lt'] 
BENCHMARK_PLOT_CARTOUCHE, pos=cpos, languages, /box, $
                      colors=colors, lines=lines, thick=1.5, title='Languages'
;
BENCHMARK_SVG, svg=svg, /off, infosvg=infosvg
;
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------------------------------------------
;
pro BENCH_MATRIX_MULTIPLY_ONE, n1, n2, n3, small=small, medium=medium, $
                               matrix_size=matrix_size, time_res=time_res, $
                               double=double, complex=complex, dcomplex=dcomplex, $
                               help=help, test=test
;
if KEYWORD_SET(help) then begin
    print, 'pro BENCH_MATRIX_MULTPLY_ONE, n1, n2, n3, small=small, medium=medium, $'
    print, '                              matrix_size=matrix_size, time_res=time_res, $'
    print, '                              double=double, complex=complex, dcomplex=dcomplex, $'
    print, '                              help=help, test=test'
    return
endif
;
; Note by AC 28 March 2013
; obsoleting code, we don't remove if we have to go back to test
; related to Eigen3 internal use ...
;
;DEFSYSV, '!gdl', exist=it_is_GDL
;
; if (it_is_GDL) then begin
;     having_eigen3=EXECUTE("type=MATMUL(/available, quiet=quiet)")
;     if (having_eigen3 EQ 0) then begin
;         print, 'You are trying testing new capabilities (Eigen3 usage)'
;         print, 'on a too OLD GDL version ! Please make tests on CVS version !'
;         return
;     endif
; endif
;
if N_PARAMS() EQ 0 then begin
    colA=1000
    rowA=3000
    rowB=751
endif
;
if N_PARAMS() EQ 1 then begin
    colA=n1
    rowA=n1
    rowB=n1
endif
;
if N_PARAMS() EQ 2 then begin
    colA=n1
    rowA=n2
    rowB=n2
endif
if N_PARAMS() EQ 3 then begin
   colA=n1
   rowA=n2
   rowB=n3
endif
;
if KEYWORD_SET(medium) then begin
   colA=colA/2
   rowA=rowA/2
   rowB=rowB/2
endif
;
if KEYWORD_SET(small) then begin
   colA=colA/4
   rowA=rowA/4
   rowB=rowB/4
endif
;
colB=rowA
;
matrix_size=[colA,rowA,colB ,rowB]
;
do_double=0
if KEYWORD_SET(dcomplex) OR KEYWORD_SET(double) then do_double=1
;
a=RANDOMU(seed, colA, rowA, DOUBLE=do_double)
b=RANDOMU(seed, colB, rowB, DOUBLE=do_double)
;
if KEYWORD_SET(dcomplex) then begin
   a=DCOMPLEX(1.,3.)*a
   b=DCOMPLEX(1.,3.)*b
endif
if KEYWORD_SET(complex) then begin
   a=COMPLEX(1.,3.)*a
   b=COMPLEX(1.,3.)*b
endif
;
b_t=TRANSPOSE(b)
a_t=TRANSPOSE(a)
;
HELP, a, b
;
txt='Matrix size are : [' +STRING(colA)+','+STRING(rowA)
txt=txt+'] # ['+STRING(colB)+','+STRING(rowB)+']'
print, STRCOMPRESS(txt)
;
txt_ref='Classic operator #, '
txt=    'Matrix_Multiply() , '
;
times=FLTARR(5)
;
t0=SYSTIME(1) & z=a # b & times[0]=SYSTIME(1)-t0 & print, txt_ref+'a#b  :', times[0]
t0=SYSTIME(1) & z=MATRIX_MULTIPLY(a,b) & times[1]=SYSTIME(1)-t0  & print, txt+'a#b  :', times[1]
t0=SYSTIME(1) & z=MATRIX_MULTIPLY(a_t,b,/at) & times[2]=SYSTIME(1)-t0  & print, txt+'aT#b :', times[2]
t0=SYSTIME(1) & z=MATRIX_MULTIPLY(a,b_t,/bt) & times[3]=SYSTIME(1)-t0  & print, txt+'a#bT :', times[3]
t0=SYSTIME(1) & z=MATRIX_MULTIPLY(a_t,b_t,/at,/bt) & times[4]=SYSTIME(1)-t0 & print, txt+'aT#bT:', times[4]
;
time_res=times
;
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------------
;
pro BENCH_MATRIX_MULTIPLY, save=save, help=help, test=test
;
if KEYWORD_SET(help) then begin
    print, 'pro BENCH_MATRIX_MULTPLY, save=save, help=help, test=test'
    return
endif
;
ON_ERROR, 2
;
if KEYWORD_SET(save) then CHECK_SAVE_RESTORE
;
BENCH_MATRIX_MULTIPLY_ONE, time_res=time_f, matrix_size=matrix_size
BENCH_MATRIX_MULTIPLY_ONE, time_res=time_d, /double
BENCH_MATRIX_MULTIPLY_ONE, time_res=time_c, /complex
BENCH_MATRIX_MULTIPLY_ONE, time_res=time_dc, /dcomplex
;
if KEYWORD_SET(save) then begin
   cpuinfo=BENCHMARK_GENERATE_CPUINFO()
   filename=BENCHMARK_GENERATE_FILENAME('matmul')
   ;;
   SAVE, file=filename, cpuinfo, matrix_size, $
         time_f, time_d, time_c, time_dc
endif
;
if KEYWORD_SET(test) then STOP
;
end
;
