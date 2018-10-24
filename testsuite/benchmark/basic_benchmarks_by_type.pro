
pro printb, str, flt, tab
vide='              '
print, format='(A20, f7.2)', str+vide, flt
tab=[tab, STRING(str), STRING(flt, format='(f7.2)')]
end
;
pro BASIC_BENCHMARKS_BY_TYPE, nbps=nbps, double=double, test=test
;
if ~KEYWORD_SET(nbps) then nbps=1e8
tab=['']
;
a=TIC()
input=RANDOMU(1, nbps, double=double)
printb, 'Time for RANDOMU : ', TOC(a), tab
input=1000.*input-500
;
GIVE_LIST_NUMERIC, list_num_types, list_num_names
;
for ii=0, N_ELEMENTS(list_num_types)-1 do begin
   mod_input=FIX(input, TYPE=list_num_types[ii])
   if ISA(mod_input,/complex) then begin
      print, 'Skip Complex'
      continue
   endif
   a=TIC()
   res=WHERE(mod_input EQ 0.5)
;;   res=(mod_input EQ 0.5)
   PRINTB, 'Time for WHERE and TYPE ='+list_num_names[ii]+' : ', TOC(a), tab
endfor
;
; save file ...
;
tab=REFORM(tab[1:*],2, N_ELEMENTS(tab)/2)
filename=BENCHMARK_GENERATE_FILENAME('bench_where_by_type')
cpuinfo=BENCHMARK_GENERATE_CPUINFO()
DEFSYSV, '!GDL', exist=exist
if exist then version=!gdl.release else version=!version.release
;
SAVE, file=filename, tab, cpuinfo, version
;
if KEYWORD_SET(test) then stop
;
end
