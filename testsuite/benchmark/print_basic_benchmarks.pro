;
; August 2018
;
pro PRINT_BASIC_BENCHMARKS, path=path, filter=filter, $
                            xrange=xrange, yrange=yrange, $
                            xlog=xlog, ylog=ylog, $
                            svg=svg, $
                            help=help, test=test
;
if KEYWORD_SET(help) then begin
    print, 'pro PRINT_BASIC_BENCHMARKS, path=path, filter=filter, $'
    print, '                            xrange=xrange, yrange=yrange, $'
    print, '                            xlog=xlog, ylog=ylog, $'
    print, '                            svg=svg, $'
    print, '                            help=help, test=test'
    return
endif
;
;ON_ERROR, 2
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
for ii=0, N_ELEMENTS(liste)-1 do begin
   print, 'Restoring '+liste[ii]
   RESTORE, liste[ii]
   if ii EQ 0 then begin
      tab=REPLICATE('',N_ELEMENTS(op_name)+1, N_ELEMENTS(liste)+1)
      tab[*,0]=STRING(format='(A9)',['',op_name])
   endif
   tab[0,ii+1]=STRING(format='(A9)',info_soft)
   tab[1:*,ii+1]=STRING(format='(f9.2)', op_val)
endfor
;
print, 'host :', (GET_LOGIN_INFO()).MACHINE_NAME
print, 'nbps :', NBPS
print, TRANSPOSE(tab)

if KEYWORD_SET(test) then STOP
;
end
;
