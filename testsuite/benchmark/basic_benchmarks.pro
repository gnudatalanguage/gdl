;
; Under GNU GPL V3+
; Alain C., summer 2018
;
; Basic benchmarks, easy to extend ...
; In the future, maybe the pure numerical part (COS, SIN, SQRT, EXP,
; ...) can be separeted for
; more high level functionnalities (WHERE, SORT, FINITE, ...)
;
; Please not that from now, RANDOMU in GDL is related to the GSL
;
; ---------------------------------
; 
; Modifications history :
;
; - 2018-10-15 : AC. 
;   * forcing !cpu.TPOOL_NTHREADS to be equal to
;     physical (detected !) core number !cpu.HW_NCPU
;   * ading more informations on the machine used in the output XDR
;     (to be used by "PLOT_BASIC_BENCHMARKS"
;
; ---------------------------------
;
; just a convenience code for smart print ...
pro PRINTB, str, flt, tab_name, tab_val
vide='              '
print, format='(A20, f7.2)', str+vide, flt
;
tmp=STRMID(STRMID(str,9),STRLEN(str), STRLEN(str)-9-2, /REV)
tab_name=[tab_name, tmp]
tab_val=[tab_val, flt]
end
pro PRINT_CPUINFO, message
txt1=': nb detected CORES : '+STRING(!cpu.HW_NCPU, format='(i4)')
txt2=', nb cores/threads in use : '+STRING(!cpu.TPOOL_NTHREADS, format='(i4)')
print, message, txt1, txt2
end
;
; ---------------------------------
;
pro BASIC_BENCHMARKS, nbps=nbps, double=double, type=type, $
                      force_max_cpu=force_max_cpu, set_cpu_nb=set_cpu_nb, $
                      prefix=prefix, $
                      help=help, verbose=verbose, test=test
;
if KEYWORD_SET(help) then begin
   print, 'pro basic_benchmarks, nbps=nbps, double=double, type=type, $'
   print, '                      prefix=prefix, force_max_cpu=force_max_cpu, $'
   print, '                      help=help, verbose=verbose, test=test'
   return
endif
;
; Checking status for CPU: sometimes, in GDL, due to load balancing in
; Eigen3 as studied by Nodar, not all threads are activated by default
; Bench on a machine with other load is not a good idea !!!
PRINT_CPUINFO, 'Initial'
;
if ~KEYWORD_SET(force_max_cpu) then begin
   if (!cpu.TPOOL_NTHREADS LT !cpu.HW_NCPU) then $
      MESSAGE, /continue, 'Please consider /force_max_cpu'
endif else begin
   CPU, TPOOL_NTHREADS=!cpu.HW_NCPU
   PRINT_CPUINFO, 'Updated'
endelse
;
if KEYWORD_SET(set_cpu_nb) then begin
   CPU, TPOOL_NTHREADS=set_cpu_nb
   PRINT_CPUINFO, 'Set'
endif
;
if ~KEYWORD_SET(nbps) then nbps=1e8
tab_name=['']
tab_val=[0.]
;
; ------------ need to change the type if needed ---
;
; note by AC : we are not ready to add a input now 
; side effect in "plot_basic_benchmarks.pro"
; a=TIC()
;input=RANDOMN(seed, nbps/4, double=double)
;printb, 'Time for RANDOMN (1/4) : ', TOC(a), tab_name, tab_val
;
a=TIC()
input=RANDOMU(1, nbps, double=double)
b=TOC(a)
;
; do we want to convert the type ?
if KEYWORD_SET(type) then input=FIX(input, type=type)
used_type=TYPENAME(input)
print, 'Working on type : ', used_type
;
; ------------ starting other computation ! ----
;
printb, 'Time for RANDOMU : ', b, tab_name, tab_val
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
val=input GT 0.5
a=TIC() & res=WHERE(val)
printb, 'Time for WHERE : ', TOC(a), tab_name, tab_val
;
sub=input[0:nbps/30]
a=TIC() & res=SORT(sub)
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
filename=BENCHMARK_GENERATE_FILENAME('basic'+prefix)
;
info_cpu=BENCHMARK_INFO_CPU()
info_os=BENCHMARK_INFO_OS()
info_soft=BENCHMARK_INFO_SOFT()
;
op_name=tab_name[1:*]
op_val=tab_val[1:*]
;
SAVE, file=filename, nbps, used_type, op_name, op_val, $
      info_cpu, info_os, info_soft
print, 'Writing file : ', filename
;
if KEYWORD_SET(test) then stop
;
end
