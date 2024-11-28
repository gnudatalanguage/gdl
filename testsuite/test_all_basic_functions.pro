; in case we substract the time of 'doing nothing' in loops, this is it.
PRO dummy, dummy
  compile_opt hidden, strictarr
  return
end

; helper for repetitive test with new variable created
pro process_new,what,limit
  common test_all_basic_function_common, scalar, small, big
; scalar right
calls="for k=0,limit do ret=(*big[k])"+what+'(*scalar[k])'
for i=0,n_elements(what)-1 do z=execute(calls[i])
; scalar left
calls="for k=0,limit do ret=(*scalar[k])"+what+"(*big[k])"
for i=0,n_elements(what)-1 do z=execute(calls[i])
; small right
calls="for k=0,limit do ret=(*big[k])"+what+'(*small[k])'
for i=0,n_elements(what)-1 do z=execute(calls[i])
; small left
calls="for k=0,limit do ret=(*small[k])"+what+"(*big[k])"
for i=0,n_elements(what)-1 do z=execute(calls[i])
; big big, register time
calls="for k=0,limit do ret=(*big[k])"+what+"(*big[k])"
for i=0,n_elements(what)-1 do begin & clock=tic(what[i])  &  z=execute(calls[i]) &  toc,clock & endfor
end
; helper for repetitive test with self variable
pro process_self,what,limit
  common test_all_basic_function_common, scalar, small, big
; need to copy first operands, they will be modified
; scalar right
  calls="for k=0,limit do begin & var=(*big[k]) & " +  "var"+what+"(*scalar[k]) & endfor"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
; scalar left
  calls="for k=0,limit do begin & var=(*scalar[k]) & " + "var"+what+"(*big[k]) & endfor"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
; small right
  calls="for k=0,limit do begin & var=(*big[k]) & " + "var"+what+"(*small[k]) & endfor"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
; small left
  calls="for k=0,limit do begin & var=(*small[k]) & " + "var"+what+"(*big[k]) & endfor"
  for i=0,n_elements(what)-1 do z=execute(calls[i])
; big big, register time
  calls="for k=0,limit do begin & var=(*big[k]) & var"+what+"var & endfor"
for i=0,n_elements(what)-1 do begin & clock=tic(what[i])  &  z=execute(calls[i]) &  toc,clock & endfor
end

pro test_all_basic_functions, size=size, section=section
  common test_all_basic_function_common, scalar, small, big
  if (n_elements(size) eq 0 ) then size=10000
  if (n_elements(section) eq 0 ) then section=0
; initialisations: floats at end, since some commands do not accpet floats/doubles/complex
  typecodes=[1,2,3,12,13,14,15,4,5,6,9]
  typenames=["		BYTE","		INT","		LONG","		UINT","		ULONG","		LONG64","		ULONG64","		FLOAT","		DOUBLE","		COMPLEX","		DCOMPLEX"]
  all_numeric=10
  integers_only=6
  not_complex=8
  seed=33
  cote=long(sqrt(size)) > 8
  a=randomn(seed,cote,cote,/double)*randomu(seed,cote,cote,/ulong)
  smallarray=a[0:4]
  big=ptrarr(11,/allo)
  k=0 & foreach i,typecodes do begin & *big[k]=fix(a,type=i) & k++ &end
  scalar=ptrarr(11,/allo)
  k=0 & foreach i,typecodes do begin & *scalar[k]=fix(1,type=i) & k++ &end
  small=ptrarr(11,/allo)
  k=0 & foreach i,typecodes do begin & *small[k]=fix(smallarray,type=i) & k++ &end
; for poly_2d:
   XO = [61, 62, 143, 133]&YO = [89, 34, 38, 105]&XI = [24, 35, 102, 92]&YI = [81, 24, 25, 92]&POLYWARP, XI, YI, XO, YO, 1, P, Q

; start master clock here --- random  does not obey pool 
   masterclock=tic("ALL TESTS")

   
; array generation
if (section eq 0 or section eq 1) then begin
  command=["BYTARR","COMPLEXARR","DBLARR","DCOMPLEXARR","FLTARR","INTARR","LON64ARR","LONARR","UINTARR","ULON64ARR","ULONARR","OBJARR","PTRARR"]
  calls="ret ="+command+"(size)"
  for i=0,n_elements(command)-1 do begin  & clock=tic(command[i]) & z=execute(calls[i]) & toc,clock & end
     print
endif
    
;
if (section eq 0 or section eq 2) then begin
  command=["BINDGEN","CINDGEN","DCINDGEN","DINDGEN","FINDGEN","INDGEN","L64INDGEN","LINDGEN","SINDGEN","UINDGEN","UL64INDGEN","ULINDGEN"]
  calls="ret ="+command+"(size)"
  for i=0,n_elements(command)-1 do begin &  clock=tic(command[i]) & z=execute(calls[i]) & toc,clock & end
  print
endif
; conversion
if (section eq 0 or section eq 3) then begin
command=["BYTE", "COMPLEX", "DCOMPLEX", "DOUBLE", "FIX", "FLOAT", $
      "LONG", "LONG64", "ULONG", "ULONG64"]
   calls="for k=0,all_numeric do ret="+command+"(*big[k])"
   for i=0,n_elements(command)-1 do begin & clock=tic(command[i])  &  z=execute(calls[i]) & toc,clock & end
  print
endif
; basic operators. They are 'optimized' inside GDL by calling
; different sections of code, depending on the size of the operand
; (scalar or not, size of left operand greater than size of the
; right operand, (or reverse), is the operation creating a new
; variable or not, is one operand zero, is multithreading enabled.
; we try to cover most of these cases, with clock used only on the
; large-array case.
if (section eq 0 or section eq 4) then begin
; operators 1
what=[' + ',' - ' ,' * ',' / ']
process_new,what, n_elements(what)-1

; operators 2
what=[' # ',' ## ']
calls="for k=0,all_numeric do  ret=(*big[k])"+what+"(*big[k])"
for i=0,n_elements(what)-1 do begin & clock=tic(what[i])  &  z=execute(calls[i]) &  toc,clock & endfor

what=[' #= ',' ##= ']
; need to use another variable not to overwrite and change it big[] !
calls="for k=0,all_numeric do begin & var=(*big[k]) & var"+what+"var & end"
for i=0,n_elements(what)-1 do begin & clock=tic(what[i])  &  z=execute(calls[i]) &  toc,clock & endfor

; operators 3
what=[' ++ ',' -- ']
; need to use another variable not to overwrite and change it big[] !
calls="for k=0,all_numeric do begin & var=(*big[k]) & var"+what+" & end"
for i=0,n_elements(what)-1 do begin & clock=tic(what[i])  &  z=execute(calls[i]) &  toc,clock & endfor

; operators 4
what=[' AND ',' OR ',' EQ ',' NE ',' LE ',' LT ', ' GE ', ' GT ',' ^']
process_new, what, all_numeric

; operators 5: complex not supported (GDL error)
what=[' < ',' > ',' MOD ']
process_new, what, not_complex

; operators 6
what=['^=' , '*=' , 'eq=' , 'ge=' ,  'gt=' , 'le=' ,  'lt=' ,  '-=' ,  'ne=' , 'or=' , '+=' , '/= ', 'xor=', 'and=']
process_self,what, all_numeric

; operators 7:  complex not supported (GDL error)
what=['mod=' , '>=' ,'<=' ]
process_self,what, not_complex

endif
print

; get device, save & set to null
olddev=!D.NAME
set_plot,"Z"
device,set_resolution=[cote,cote]

; functions that operate on whole array
if (section eq 0 or section eq 5) then begin
  what=['BYTSCL','SORT','TRANSPOSE','WHERE','FINITE','SHIFT','ISHIFT','LOGICAL_AND','LOGICAL_OR','LOGICAL_TRUE','ATAN','CONVOL','INTERPOLATE' , 'POLY_2D' , 'TVSCL']
  calls=[$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=BYTSCL(*big[k],max=10,min=1,/nan,top=100) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=sort(*big[k]) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=transpose(*big[k]) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & b=(*big[k] eq 0) & subclock=tic(typenames[k]) & ret=where(b) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=finite(*big[k]) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=shift(*big[k],[2,3]) & toc,subclock & end ',$
   'print,what[i] & for k=0,integers_only do begin & subclock=tic(typenames[k]) & ret=ishft(*big[k],[2,3]) & toc,subclock & end ',$ ; ishft needs integer
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=logical_and(*big[k],!dpi) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=logical_or(*big[k],!dpi) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=logical_true(*big[k]) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=atan(*big[k],*big[k]) & toc,subclock & end ',$
   'print,what[i] & kernel=[ [0,1,0],[-1,0,1],[0,-1,0] ] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=convol(*big[k],kernel) & toc,subclock & end ',$
   'print,what[i] & z=findgen(size) & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=interpolate(*big[k],z) & toc,subclock & end ',$
   'print,what[i] & for k=0,not_complex do begin & subclock=tic(typenames[k]) & ret=poly_2d(*big[k],p,q) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & tvscl,(*big[k]) & toc,subclock & end ' ]

for i=0,n_elements(calls)-1 do begin & clock=tic(what[i])  & z=execute(calls[i]) &  toc,clock & end
     print
  endif
set_plot,olddev

; functions that operate also on particular dimension
if (section eq 0 or section eq 6) then begin
   what=['MEDIAN (all dims)','MEDIAN (dim=2)','MEAN (all dims)','MEAN (dim=2)','MOMENT (all dims)','MOMENT (dim=2)','TOTAL (all dims)','TOTAL (dim=2)','PRODUCT (all dims)','PRODUCT (dim=2)','MIN (all dims)','MIN (dim=2)','MAX (all dims)','MAX (dim=2)','FFT (all dims)','FFT (dim=2)']
  calls=[$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=median(*big[k]) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=median(*big[k],dim=2) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=mean(*big[k]) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=mean(*big[k], dim=2) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=moment(*big[k],/nan) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=moment(*big[k],dim=2) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=total(*big[k]) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=total(*big[k],2) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=product(*big[k]) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=product(*big[k],2) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=min(*big[k]) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=min(*big[k],dim=2) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=max(*big[k]) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=max(*big[k],dim=2) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=fft(*big[k]) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=fft(*big[k],dim=2) & toc,subclock & end ']

for i=0,n_elements(calls)-1 do begin & clock=tic(what[i])  & z=execute(calls[i]) &  toc,clock & end
     print
  endif

; FunDirect functions
if (section eq 0 or section eq 7) then begin
  command=["SIN","COS","TAN","SINH","COSH","TANH","ASIN","ACOS","ALOG","ALOG2","ALOG10","SQRT","ABS","EXP","CONJ","IMAGINARY","ROUND","CEIL","FLOOR"]
     calls="for k=0,all_numeric do ret="+command+"(*big[k])"
     for i=0,n_elements(command)-1 do begin & clock=tic(command[i])  &  z=execute(calls[i]) &  toc,clock & end
  print
endif
; non-threaded functions
if (section eq 0 or section eq 8) then begin
  what=['ROTATE','REVERSE','REFORM',$
;'ROT', 
'BYTEORDER','INTERPOL']
  calls=[$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=ROTATE(*big[k],1) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=REVERSE(*big[k],2) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=REFORM(*big[k],cote*cote) & toc,subclock & end ',$
;     'print,what[i] & for k=0,not_complex do begin & subclock=tic(typenames[k]) & ret=ROT(*big[k],33,0.6,/INTERP) & toc,subclock & end ',$
     'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & BYTEORDER,*big[k],/LSWAP & toc,subclock & end ',$
     'print,what[i] & x = FINDGEN(100)*0.02 & y=sin(x) & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & res=interpol(y,x,*big[k]) & toc,subclock & end ']

   for i=0,n_elements(calls)-1 do begin & clock=tic(what[i])  & z=execute(calls[i]) &  toc,clock & end
  print
endif

  toc,masterclock
; test if *big type has been changed in all those processes --- it should not, but only if special measures were taken at all places self operaors are used.
  if ~ISA(*big[0],'Byte') then exit,status=1
end

; to do :string things, that are 2 times slower than idl (up to several seconds)
; ERF , ERFC , ERFCX , EXP , EXPINT, GAMMA , GAUSSINT, LNGAMMA , MATRIX_MULTIPLY, VOIGT, REPLICATE_INPLACE
;   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=string(*big[k]) & toc,subclock & end ',$
;   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=strtrim(*big[k],2) & toc,subclock & end ',$
