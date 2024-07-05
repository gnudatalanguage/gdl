; will exercise all basic operations for the 3 cases: 1 element size, multiple elements with number < cpu.tpool_min_elts (no threads), multiple with nel > cpu.tpool_min_elts (in this case testing the result is the same with 1 or N threads)
; there are dozens of specializations for each operation, so we have to call numerous cases
; dummy nothing
PRO dummy, dummy
  compile_opt hidden, strictarr
  return
end
; helper for repetitive test with new variable created (operators like in "z = a and b" )
pro process_new,what,limit
  format='("{",a,"} ",a)'
  common test_all_basic_function_common, lun, typecodes, typenames, zero, scalar, onedim, small, big
  for i=0,n_elements(what)-1 do z=execute( " printf,lun,'------------------------------------------------------------------------'" )
intent="zero right: z=scalar"+what+'0'
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do  begin & ret=(*scalar[k]"+what+"*zero[k]) & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="zero right: z=onedim"+what+'0'
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do  begin & ret=(*onedim[k]"+what+"*zero[k]) & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="zero right: z=big"+what+'0'
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do  begin & ret=(*big[k]"+what+"*zero[k]) & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="zero left: z=0"+what+"scalar"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do  begin &ret=(*zero[k]"+what+"*scalar[k]) & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="zero left: z=0"+what+"onedim"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do  begin &ret=(*zero[k]"+what+"*onedim[k]) & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="zero left: z=0"+what+"big"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do  begin &ret=(*zero[k]"+what+"*big[k]) & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="scalar right: z=onedim"+what+"scalar"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do  begin & ret=(*onedim[k]"+what+"*scalar[k]) & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="scalar right: z=big"+what+"scalar"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do  begin & ret=(*big[k]"+what+"*scalar[k]) & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="scalar left: z=scalar"+what+"onedim"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do  begin &ret=(*scalar[k]"+what+"*onedim[k]) & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="scalar left: z=scalar"+what+"big"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do  begin &ret=(*scalar[k]"+what+"*big[k]) & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="small right: z=big"+what+"small"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do  begin &ret=(*big[k]"+what+"*small[k]) & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="small left: z=small"+what+"big"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do  begin &ret=(*small[k]"+what+"*big[k]) & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="big big: z=big"+what+"big"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do  begin ret=(*big[k]"+what+"*big[k]) & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
end
; helper for repetitive test with guarded variable (after "AdjustTypesXXX", in prognodeexpr.cpp) operators like in "z= a and temporary(b) " )
pro process_temporary_right,what,limit
  for i=0,n_elements(what)-1 do z=execute( " printf,lun,'------------------------------------------------------------------------'" )
  format='("{",a,"} ",a)'
  common test_all_basic_function_common, lun, typecodes, typenames, zero, scalar, onedim, small, big
intent="*Guarded*,zero right: z=scalar"+what+"temporary(0)"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do  begin & var=*zero[k] &ret=(*scalar[k]"+what+"temporary(var)) & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="*Guarded*,zero right: z=onedim"+what+"temporary(0)"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do  begin & var=*zero[k] &ret=(*onedim[k]"+what+"temporary(var)) & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="*Guarded*,zero right: z=big"+what+"temporary(0)"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do  begin & var=*zero[k] &ret=(*big[k]"+what+"temporary(var)) & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="*Guarded*,zero left: z=0"+what+"temporary(scalar)"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do  begin & var=*scalar[k] &ret=(*zero[k]"+what+"temporary(var)) & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="*Guarded*,zero left: z=0"+what+"temporary(onedim)"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do  begin & var=*onedim[k] &ret=(*zero[k]"+what+"temporary(var)) & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="*Guarded*,zero left: z=0"+what+"temporary(big"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do  begin & var=*big[k] &ret=(*zero[k]"+what+"temporary(var)) & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="*Guarded*,scalar right: z=onedim"+what+"temporary(scalar"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do  begin & var=*scalar[k] & ret=(*onedim[k]"+what+"temporary(var)) & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="*Guarded*,scalar right: z=big"+what+"temporary(scalar"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do  begin & var=*scalar[k] & ret=(*big[k]"+what+"temporary(var)) & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="*Guarded*,scalar left: z=scalar"+what+"temporary(onedim"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do  begin & var=*onedim[k] &ret=(*scalar[k]"+what+"temporary(var)) & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="*Guarded*,scalar left: z=scalar"+what+"temporary(big"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do  begin & var=*big[k] &ret=(*scalar[k]"+what+"temporary(var)) & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="*Guarded*,small right: z=big"+what+"temporary(small"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do  begin &var=*small[k] &ret=(*big[k]"+what+"temporary(var)) & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="*Guarded*,small left: z=small"+what+"temporary(big"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do  begin & var=*big[k] &ret=(*small[k]"+what+"temporary(var)) & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="*Guarded*,big big: z=big"+what+"temporary(big"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do  begin & var=*big[k] &ret=(*big[k]"+what+"temporary(var)) & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
end
; helper for repetitive test with guarded variable (after "AdjustTypesXXX", in prognodeexpr.cpp) operators like in "z= temporary(a) and b " )
pro process_temporary_left,what,limit
  for i=0,n_elements(what)-1 do z=execute( " printf,lun,'------------------------------------------------------------------------'" )
  format='("{",a,"} ",a)'
  common test_all_basic_function_common, lun, typecodes, typenames, zero, scalar, onedim, small, big
intent="*Guarded*,zero right: z=temporary(scalar)"+what+"zero"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do  begin & var=*scalar[k] &ret=(temporary(var)"+what+"*zero[k]) & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="*Guarded*,zero right: z=temporary(onedim)"+what+"zero"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do  begin & var=*onedim[k] &ret=(temporary(var)"+what+"*zero[k]) & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="*Guarded*,zero right: z=temporary(big)"+what+"zero"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do  begin & var=*big[k] &ret=(temporary(var)"+what+"*zero[k]) & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="*Guarded*,zero left: z=temporary(zero)"+what+"scalar"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do  begin & var=*zero[k] &ret=(temporary(var)"+what+"*scalar[k]) & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="*Guarded*,zero left: z=temporary(zero)"+what+"onedim"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do  begin & var=*zero[k] &ret=(temporary(var)"+what+"*onedim[k]) & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="*Guarded*,zero left: z=temporary(zero)"+what+"big"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do  begin &  var=*zero[k] &ret=(temporary(var)"+what+"*big[k]) & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="*Guarded*,scalar right: z=temporary(onedim)"+what+"scalar"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do  begin & var=*onedim[k] & ret=(temporary(var)"+what+"*scalar[k]) & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="*Guarded*,scalar right: z=temporary(big)"+what+"scalar"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do  begin & var=*big[k] & ret=(temporary(var)"+what+"*scalar[k])  & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="*Guarded*,scalar left: z=temporary(scalar)"+what+"onedim"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do  begin & var=*scalar[k] & ret=(temporary(var)"+what+"*onedim[k]) & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="*Guarded*,scalar left: z=temporary(scalar)"+what+"big"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do  begin & var=*scalar[k] & ret=(temporary(var)"+what+"*big[k]) & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="*Guarded*,small right: z=temporary(big)"+what+"small"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do  begin &var=*big[k] & ret=(temporary(var)"+what+"*small[k]) & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="*Guarded*,small left: z=temporary(small)"+what+"big"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do  begin & var=*small[k] & ret=(temporary(var)"+what+"*big[k]) & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="*Guarded*,big big: z=temporary(big)"+what+"big"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do  begin & var=*big[k] & ret=(temporary(var)"+what+"*big[k]) & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
end

; helper for repetitive test with self variable (operator like in "a and= b" )
pro process_new_self,what,limit
  for i=0,n_elements(what)-1 do z=execute( " printf,lun,'------------------------------------------------------------------------'" )
  common test_all_basic_function_common, lun, typecodes, typenames, zero, scalar, onedim, small, big
  format='("{",a,"} ",a)'
; need to copy first operands, they will be modified
intent="zero right: z=onedim"+what+'0'
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do begin & var=(*onedim[k]) & " +  "var"+what+"(*zero[k]) & printf,lun,typenames[k] & printf,lun,var & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="zero right: z=big"+what+'0'
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do begin & var=(*big[k]) & " +  "var"+what+"(*zero[k]) & printf,lun,typenames[k] & printf,lun,var & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="zero left: z=0"+what+"onedim"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do begin & var=(*zero[k]) & " + "var"+what+"(*onedim[k]) & printf,lun,typenames[k] & printf,lun,var & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="zero left: z=0"+what+"big"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do begin & var=(*zero[k]) & " + "var"+what+"(*big[k]) & printf,lun,typenames[k] & printf,lun,var & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="scalar right: z=onedim"+what+"scalar"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do begin & var=(*onedim[k]) & " +  "var"+what+"(*scalar[k]) & printf,lun,typenames[k] & printf,lun,var & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="scalar right: z=big"+what+"scalar"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do begin & var=(*big[k]) & " +  "var"+what+"(*scalar[k]) & printf,lun,typenames[k] & printf,lun,var & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="scalar left: z=scalar"+what+"onedim"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do begin & var=(*scalar[k]) & " + "var"+what+"(*onedim[k]) & printf,lun,typenames[k] & printf,lun,var & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="scalar left: z=scalar"+what+"big"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do begin & var=(*scalar[k]) & " + "var"+what+"(*big[k]) & printf,lun,typenames[k] & printf,lun,var & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="small right: z=big"+what+"small"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do begin & var=(*big[k]) & " + "var"+what+"(*small[k]) & printf,lun,typenames[k] & printf,lun,var & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="small left: z=small"+what+"big"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do begin & var=(*small[k]) & " + "var"+what+"(*big[k]) & printf,lun,typenames[k] & printf,lun,var & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="big big: z=big"+what+"big"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do begin & var=(*big[k]) & var"+what+"var & printf,lun,typenames[k] & printf,lun,var & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
end
; helper for repetitive test with self left (operator like in "a++" )
pro process_onevar_left_self,what,limit
  for i=0,n_elements(what)-1 do z=execute( " printf,lun,'------------------------------------------------------------------------'" )
  common test_all_basic_function_common, lun, typecodes, typenames, zero, scalar, onedim, small, big
  format='("{",a,"} ",a)'
; need to copy first operands, they will be modified
intent="zero left: a=0 & a"+what
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do begin & var=(*zero[k]) & " + "var"+what+" & printf,lun,typenames[k] & printf,lun,var & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="scalar left: a=scalar & a"+what
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do begin & var=(*scalar[k]) & " + "var"+what+" & printf,lun,typenames[k] & printf,lun,var & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="array left: a=array & a"+what
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do begin & var=(*small[k]) & " + "var"+what+"  & printf,lun,typenames[k] & printf,lun,var & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
end
; helper for repetitive test with self left (operator like in "a++" )
pro process_onevar_left,what,limit
  for i=0,n_elements(what)-1 do z=execute( " printf,lun,'------------------------------------------------------------------------'" )
  common test_all_basic_function_common, lun, typecodes, typenames, zero, scalar, onedim, small, big
  format='("{",a,"} ",a)'
intent="zero left: ret=0"+what
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do begin & var=(*zero[k]) & " + "ret=var"+what+" & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="scalar left: ret=scalar"+what
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do begin & var=(*scalar[k]) & " + "ret=var"+what+" & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="array left: ret=array"+what
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do begin & var=(*small[k]) & " + "ret=var"+what+"  & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
end
; helper for repetitive test with self right (operator like in "a = NOT a" )
pro process_onevar_right,what,limit
  for i=0,n_elements(what)-1 do z=execute( " printf,lun,'------------------------------------------------------------------------'" )
  common test_all_basic_function_common, lun, typecodes, typenames, zero, scalar, onedim, small, big
  format='("{",a,"} ",a)'
; need to copy first operands, they will be modified
intent="z="+what+"zero"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do begin & var=(*zero[k]) & " + "ret="+what+"var & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="z="+what+"scalar"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do begin & var=(*scalar[k]) & " + "ret="+what+"var & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
intent="z="+what+"array"
  calls="printf,lun,what[i],intent[i],format=format & for k=0,limit do begin & var=(*small[k]) & " + "ret="+what+"var  & printf,lun,typenames[k] & printf,lun,ret & end"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
end

pro test_indepth_operators_sub, size, given_lun, float=float, complex=complex, include_complex=include_complex
  if (n_elements(include_complex) eq 0 ) then include_complex=0
  common test_all_basic_function_common, lun, typecodes, typenames, zero, scalar, onedim, small, big
  lun=given_lun
; initialisations: floats at end, since some commands do not accept floats/doubles/complex
  typecodes_all=[1,2,3,12,13,14,15,4,5,6,9]
  typenames_all=["BYTE","INT","LONG","UINT","ULONG","LONG64","ULONG64","FLOAT","DOUBLE","COMPLEX","DCOMPLEX"]
  typecodes_float=[4,5]
  typenames_float=["FLOAT","DOUBLE"]
  typecodes_complex=[6,9]
  typenames_complex=["COMPLEX","DCOMPLEX"]
  typecodes=typecodes_all & typenames=typenames_all ; default
  all_numeric=(include_complex gt 0)?10:8 ; 10 but complex values can valuably return NaNs and Infs because this is what teh C++ runtime library returns for complex. We do not use the same algorithm as IDL.
  integers_only=6
  not_complex=8

  if (keyword_set(float)) then begin
     typecodes=typecodes_float
     typenames=typenames_float
     include_complex=0
     all_numeric=1
     integers_only=1
     not_complex=1
  endif
  

  a=dindgen(size)+1.0d & a[2]=0d ; insures test on some AND or OR codes 
  onedimarray=[777.0d]
  smallarray=dindgen(4)+1.0d & smallarray[2]=0d;
  big=ptrarr(11,/allo)
  forbig=a+3333 & forbig[3]=0 
  k=0 & foreach i,typecodes do begin & *big[k]=fix(forbig,type=i) & k++ &end
  zero=ptrarr(11,/allo)
  k=0 & foreach i,typecodes do begin & *zero[k]=fix(0,type=i) & k++ &end
  scalar=ptrarr(11,/allo)
  k=0 & foreach i,typecodes do begin & *scalar[k]=fix(99999,type=i) & k++ &end
  onedim=ptrarr(11,/allo)
  k=0 & foreach i,typecodes do begin & *onedim[k]=fix(onedimarray,type=i) & k++ &end
  small=ptrarr(11,/allo)
  k=0 & foreach i,typecodes do begin & *small[k]=fix(smallarray,type=i) & k++ &end
; basic operators. They are "optimized" inside GDL by calling
; different sections of code, depending on the size of the operand
; (scalar or not, size of left operand greater than size of the
; right operand, (or reverse), is the operation creating a new
; variable or not, is one operand zero, is multithreading enabled.
; we try to cover most of these case.

; operators 1
; Some of these operators have 4 flavors, depending on size, ex AndOP, AndOpS, AndOpInv, AndOpInvS, and 3 cases: new, new but one var is Guarded, or operating on same operand
what=[" + "," - " ," * "," / "," ^ "]
process_new, what, all_numeric
process_temporary_right, what, all_numeric
process_temporary_left, what, all_numeric
; operators 2
;; what=[" # "," ## "]
;; calls="for k=0,all_numeric do ret=(*big[k])"+what+"(*big[k])"
;; for i=0,n_elements(what)-1 do z=execute(calls[i]) 
;; what=[" #= "," ##= "]
;; calls="for k=0,all_numeric do (*big[k])"+what+"(*big[k])"
;; for i=0,n_elements(what)-1 do z=execute(calls[i])
; operators 3
what=[" ++ "," -- "]
process_onevar_left,what, all_numeric
process_onevar_left_self,what, all_numeric
what=[" ~ "," NOT "]
process_onevar_right,what, all_numeric
; 
what=[" AND "," OR "," EQ "," NE "," LE "," LT ", " GE ", " GT "]
process_new, what, all_numeric
process_temporary_right, what, all_numeric
process_temporary_left, what, all_numeric
; just integer types
what=[" XOR "]
process_new, what, integers_only
process_temporary_right, what, integers_only
process_temporary_left, what, integers_only
; just integer types
what=[" XOR= "]
process_new_self, what, integers_only
; operators 5: complex not supported (GDL error)
what=[" < "," > "," MOD "]
process_new, what, not_complex
process_temporary_right, what, not_complex
process_temporary_left, what, not_complex
; after this, no need to use process_temporary_xxxx, already done.
; operators 6
what=[" ^= " , " *= " , " EQ= " , " GE= " ,  " GT= " , " LE= " ,  " LT= " ,  " -= " ,  " NE= " , " OR= " , " += " , " /= ", " AND= ", " MOD= ", " >= ", " <= "]
process_new_self,what, all_numeric
; operators 7:  complex not supported (GDL error)
what=[" MOD= " , " >= " ," <= " ]
process_new_self,what, not_complex
end
;; trace_routine can only be used and useful if GDL is compiled with option TRACE_OPCALLS.
;; it gives on the terminal the nama and file of the exact function used
pro test_indepth_basic_functions, size=size, trace_routine=trace_routine, test_cpu=test_cpu, include_complex=include_complex, float=float, complex=complex
  if (n_elements(size) eq 0 ) then size=10
  if keyword_set(trace_routine) then begin
     test_indepth_operators_sub, size, -1, include_complex=include_complex, float=float, complex=complex
     return
  endif
  DEFSYSV,"!GDL",exists=isgdl
  ; first pass: 1 cpu
  if isgdl then outfile1='GDL_oneCPU_test_operators.txt' else outfile1='IDL_oneCPU_test_operators.txt'
  ; open result file
  openw,lun,outfile1,/get_lun
  ; test with 1 cpu
  old_ncpu=!cpu.tpool_nthreads
  old_nmin=!cpu.tpool_min_elts
  cpu,tpool_nthreads=1
  test_indepth_operators_sub, size, lun, include_complex=include_complex, float=float, complex=complex
  print,"test done for 1 cpu. If possible, compare '"+outfile1+"' and 'IDL_oneCPU_test_operators.txt'."
  if keyword_set(test_cpu) and isgdl then begin
     ; enable test for multiple cpu if old_ncpu is > 4
     if old_ncpu gt 4 and old_nmin gt 1000 and size ge 8 then begin
        ; 2nd pass: multiple cpu -- compare results with 1 cpu and many cpu
        outfile2='GDL_manyCPU_test_operators.txt'
        close, lun
        openw,lun,outfile2
        cpu,tpool_nthreads=4
        cpu,tpool_min_elts=2
        test_indepth_operators_sub, size, lun, include_complex=include_complex, float=float, complex=complex
        cpu,tpool_nthreads=old_ncpu
        cpu,tpool_min_elts=old_nmin
        print,"test done for multiple cpu. Compare '"+outfile2+"' and '"+outfile1+"'."
     endif
  endif
  
  close, lun
  free_lun,lun
end
