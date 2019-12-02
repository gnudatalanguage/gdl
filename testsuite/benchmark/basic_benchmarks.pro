;+
; Under GNU GPL V3+
; Alain C., summer 2018
;
; Basic benchmarks, easy to extend ...
; In the future, maybe the pure numerical part (COS, SIN, SQRT, EXP,
; ...) can be separeted for
; more high level functionnalities (WHERE, SORT, FINITE, ...)
;
; ---------------------------------
;
; ToDo : (clean) extension to other types (Complex, Int/Long, ...)
;
; ---------------------------------
; 
; Modifications history :
;
; - 2018-10-15 : AC. 
;   * forcing !cpu.TPOOL_NTHREADS to be equal to
;     physical (detected !) core number !cpu.HW_NCPU
;   * ading more informations on the machine used in the output XDR
;     (to be used by "PLOT_BASIC_BENCHMARKS" & "PRINT_BASIC_BENCHMARKS")
;
; - 2019-11-19 : AC. 
;   * we are not fully ready for types different than Float & Double
;
; ---------------------------------
;-
; just a convenience code for smart print ...
pro PRINTB, str, flt, tab_name, tab_val
vide='              '
print, format='(A23, g12.6)', str+vide, flt
;
tmp=STRMID(STRMID(str,9),STRLEN(str), STRLEN(str)-9-2, /REV)
tab_name=[tab_name, tmp]
tab_val=[tab_val, flt]
end
; ---------------------------------
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
if nbps lt 10LL then message,"nbps must be > 10."
; compute ntrials such as 1 is for 10^8 and 10^8 for one
log_ntrials=floor(6-alog10(nbps)) > 0
ntrials=10.^log_ntrials > 1
ntrials=ROUND(ntrials)
;
if KEYWORD_SET(verbose) then print, 'Nb Trials : ', ntrials
;
tab_name=['']
tab_val=[0.]
;
; do we want to convert the type ?
tmp=RANDOMN(seed, 4, double=double)
if KEYWORD_SET(type) then input=FIX(tmp, type=type) else input=tmp
used_type=TYPENAME(input)
print, 'Working on type : ', used_type
;
; -------- RANDOM are always on Float or Double (ULong ToDo) ---
;
a=TIC()
for i=1,ntrials do input=RANDOMN(seed, nbps/4, double=double)
printb, 'Time for RANDOMN (1/4) : ', TOC(a)/ntrials, tab_name, tab_val
;
a=TIC()
for i=1,ntrials do input=RANDOMU(seed, nbps, double=double)
printb, 'Time for RANDOMU : ', TOC(a)/ntrials, tab_name, tab_val
;
; ------------ need to change the type if needed ---
;
; do we want to convert the type ?
if KEYWORD_SET(type) then input=FIX(input, type=type)
;
; ------------ starting other computation ! ----
;
a=TIC() 
for i=1,ntrials do res=COS(input)
printb, 'Time for COS : ', TOC(a)/ntrials, tab_name, tab_val
;
a=TIC()
for i=1,ntrials do res=SIN(input)
printb, 'Time for SIN : ', TOC(a)/ntrials, tab_name, tab_val
;
a=TIC()
for i=1,ntrials do res=TAN(input)
printb, 'Time for TAN : ', TOC(a)/ntrials, tab_name, tab_val
;
a=TIC()
for i=1,ntrials do res=ALOG(input)
printb, 'Time for ALOG : ', TOC(a)/ntrials, tab_name, tab_val
;
a=TIC()
for i=1,ntrials do res=SQRT(input)
printb, 'Time for SQRT : ', TOC(a)/ntrials, tab_name, tab_val
;
a=TIC()
for i=1,ntrials do res=EXP(input)
printb, 'Time for EXP : ', TOC(a)/ntrials, tab_name, tab_val
;
val=input GT 0.5
a=TIC()
for i=1,ntrials do res=WHERE(val)
printb, 'Time for WHERE : ', TOC(a)/ntrials, tab_name, tab_val
;
sub=input[0:nbps/30]
a=TIC()
for i=1,ntrials do res=SORT(sub)
printb, 'Time for SORT (1/30) : ', TOC(a)/ntrials, tab_name, tab_val
;
a=TIC()
for i=1,ntrials do res=FINITE(input)
printb, 'Time for FINITE : ', TOC(a)/ntrials, tab_name, tab_val
;
a=TIC()
for i=1,ntrials do res=FIX(input)
printb, 'Time for FIX : ', TOC(a)/ntrials, tab_name, tab_val
;
; -------- computations finished, saving file now ... ----
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
