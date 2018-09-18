
pro printb, str, flt, tab_name, tab_val
vide='              '
print, format='(A20, f7.2)', str+vide, flt
;
tmp=STRMID(STRMID(str,9),STRLEN(str), STRLEN(str)-9-2, /REV)
tab_name=[tab_name, tmp]
tab_val=[tab_val, flt]
end

pro basic_benchmarks, nbps=nbps, double=double, test=test, prefix=prefix
;
if ~KEYWORD_SET(nbps) then nbps=1e8
tab_name=['']
tab_val=[0.]
;
a=TIC()
input=RANDOMU(1, nbps, double=double)
printb, 'Time for RANDOMU : ', TOC(a), tab_name, tab_val
;
a=TIC() & res=COS(input)
printb, 'Time for COS : ', TOC(a), tab_name, tab_val
;
a=TIC() & res=SIN(input)
printb, 'Time for SIN : ', TOC(a), tab_name, tab_val
;
a=TIC() & res=TAN(input)
printb, 'Time for TAN : ', TOC(a), tab_name, tab_val
;
a=TIC() & res=ALOG(input)
printb, 'Time for ALOG : ', TOC(a), tab_name, tab_val
;
a=TIC() & res=SQRT(input)
printb, 'Time for SQRT : ', TOC(a), tab_name, tab_val
;
a=TIC() & res=EXP(input)
printb, 'Time for EXP : ', TOC(a), tab_name, tab_val
;
a=TIC() & res=WHERE(input GT 0.5)
printb, 'Time for WHERE : ', TOC(a), tab_name, tab_val
;
a=TIC() & res=SORT(input[0:nbps/30])
printb, 'Time for SORT (1/30) : ', TOC(a), tab_name, tab_val
;
a=TIC() & res=FINITE(input)
printb, 'Time for FINITE : ', TOC(a), tab_name, tab_val
;
a=TIC() & res=FIX(input)
printb, 'Time for FIX : ', TOC(a), tab_name, tab_val
;
; save file ...
;
if ~KEYWORD_SET(prefix) then prefix=''
filename=BENCHMARK_GENERATE_FILENAME('basic_bench'+prefix)
cpuinfo=BENCHMARK_GENERATE_CPUINFO()
DEFSYSV, '!GDL', exist=exist
if exist then version=!gdl.release else version=!version.release
;
op_name=tab_name[1:*]
op_val=tab_val[1:*]
;
SAVE, file=filename, nbps, op_name, op_val, cpuinfo, version
print, 'writing file : ', filename
;
if KEYWORD_SET(test) then stop
;
end
