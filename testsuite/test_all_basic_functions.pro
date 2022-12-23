; in case we substract the time of 'doing nothing' in loops, this is it.
PRO dummy, dummy
  compile_opt hidden, strictarr
  return
end

pro test_all_basic_functions, size=size, section=section
  
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
  various_types=ptrarr(11,/allo)
k=0 & foreach i,typecodes do begin & *various_types[k]=fix(a,type=i) & k++ &end
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
   calls="for k=0,all_numeric do ret="+command+"(*various_types[k])"
   for i=0,n_elements(command)-1 do begin & clock=tic(command[i])  &  z=execute(calls[i]) & toc,clock & end
  print
endif
if (section eq 0 or section eq 4) then begin
; operators 1
what=[' + ',' - ' ,' * ',' / ']
calls="for k=0,all_numeric do ret=(*various_types[k])"+what+'1'
for i=0,n_elements(what)-1 do begin & clock=tic(what[i])  &  z=execute(calls[i]) &  toc,clock & endfor
; operators 2
what=[' # ',' ## ']
calls="for k=0,all_numeric do ret=(*various_types[k])"+what+"(*various_types[k])"
for i=0,n_elements(what)-1 do begin & clock=tic(what[i])  &  z=execute(calls[i]) &  toc,clock & endfor
what=[' #= ',' ##= ']
calls="for k=0,all_numeric do (*various_types[k])"+what+"(*various_types[k])"
for i=0,n_elements(what)-1 do begin & clock=tic(what[i])  &  z=execute(calls[i]) &  toc,clock & endfor
; operators 3
what=[' ^' ,' < ',' > ',' AND ',' OR ',' MOD ',' EQ ',' NE ',' LE ',' LT ', ' GE ', ' GT ']
calls="z=2 & for k=0,not_complex do ret=(*various_types[k]) "+what+" z"
for i=0,n_elements(what)-1 do begin & clock=tic(what[i])  &  z=execute(calls[i]) &  toc,clock & endfor
; operators 4
what=[ '*=' , 'eq=' , 'ge=' , '>=' , 'gt=' , 'le=' , '<=' , 'lt=' ,  '-=' , 'mod=' , 'ne=' , 'or=' , '+=' , '^=' , '/= ', 'xor=']
calls="z=2 & for k=0,all_numeric do (*various_types[k]) "+what+" z"
for i=0,n_elements(what)-1 do begin & clock=tic(what[i])  &  z=execute(calls[i]) &  toc,clock & endfor
what=['and=']
calls="z=2 & for k=0,not_complex do (*various_types[k]) "+what+" z"
for i=0,n_elements(what)-1 do begin & clock=tic(what[i])  &  z=execute(calls[i]) &  toc,clock & endfor
endif
print

; get device, save & set to null
olddev=!D.NAME
set_plot,"Z"
device,set_resolution=[cote,cote]

; functions that oerate on whole array
if (section eq 0 or section eq 5) then begin
  what=['BYTSCL','SORT','TRANSPOSE','WHERE','FINITE','SHIFT','ISHIFT','LOGICAL_AND','LOGICAL_OR','LOGICAL_TRUE','ATAN','CONVOL','INTERPOLATE' , 'POLY_2D' , 'TVSCL']
  calls=[$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=BYTSCL(*various_types[k],max=10,min=1,/nan,top=100) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=sort(*various_types[k]) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=transpose(*various_types[k]) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & b=(*various_types[k] eq 0) & subclock=tic(typenames[k]) & ret=where(b) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=finite(*various_types[k]) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=shift(*various_types[k],[2,3]) & toc,subclock & end ',$
   'print,what[i] & for k=0,integers_only do begin & subclock=tic(typenames[k]) & ret=ishft(*various_types[k],[2,3]) & toc,subclock & end ',$ ; ishft needs integer
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=logical_and(*various_types[k],!dpi) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=logical_or(*various_types[k],!dpi) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=logical_true(*various_types[k]) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=atan(*various_types[k],*various_types[k]) & toc,subclock & end ',$
   'print,what[i] & kernel=[ [0,1,0],[-1,0,1],[0,-1,0] ] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=convol(*various_types[k],kernel) & toc,subclock & end ',$
   'print,what[i] & z=findgen(size) & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=interpolate(*various_types[k],z) & toc,subclock & end ',$
   'print,what[i] & for k=0,not_complex do begin & subclock=tic(typenames[k]) & ret=poly_2d(*various_types[k],p,q) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & tvscl,(*various_types[k]) & toc,subclock & end ' ]

for i=0,n_elements(calls)-1 do begin & clock=tic(what[i])  & z=execute(calls[i]) &  toc,clock & end
     print
  endif
set_plot,olddev

; functions that operate also on particular dimension
if (section eq 0 or section eq 6) then begin
   what=['MEDIAN (all dims)','MEDIAN (dim=2)','MEAN (all dims)','MEAN (dim=2)','MOMENT (all dims)','MOMENT (dim=2)','TOTAL (all dims)','TOTAL (dim=2)','PRODUCT (all dims)','PRODUCT (dim=2)','MIN (all dims)','MIN (dim=2)','MAX (all dims)','MAX (dim=2)','FFT (all dims)','FFT (dim=2)']
  calls=[$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=median(*various_types[k]) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=median(*various_types[k],dim=2) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=mean(*various_types[k]) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=mean(*various_types[k], dim=2) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=moment(*various_types[k],/nan) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=moment(*various_types[k],dim=2) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=total(*various_types[k]) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=total(*various_types[k],2) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=product(*various_types[k]) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=product(*various_types[k],2) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=min(*various_types[k]) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=min(*various_types[k],dim=2) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=max(*various_types[k]) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=max(*various_types[k],dim=2) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=fft(*various_types[k]) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=fft(*various_types[k],dim=2) & toc,subclock & end ']

for i=0,n_elements(calls)-1 do begin & clock=tic(what[i])  & z=execute(calls[i]) &  toc,clock & end
     print
  endif

; FunDirect functions
if (section eq 0 or section eq 7) then begin
  command=["SIN","COS","TAN","SINH","COSH","TANH","ASIN","ACOS","ALOG","ALOG2","ALOG10","SQRT","ABS","EXP","CONJ","IMAGINARY","ROUND","CEIL","FLOOR"]
     calls="for k=0,all_numeric do ret="+command+"(*various_types[k])"
     for i=0,n_elements(command)-1 do begin & clock=tic(command[i])  &  z=execute(calls[i]) &  toc,clock & end
  print
endif
; non-threaded functions
if (section eq 0 or section eq 8) then begin
  what=['ROTATE','REVERSE','REFORM','ROT','BYTEORDER','INTERPOL']
  calls=[$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=ROTATE(*various_types[k],1) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=REVERSE(*various_types[k],2) & toc,subclock & end ',$
   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=REFORM(*various_types[k],cote*cote) & toc,subclock & end ',$
     'print,what[i] & for k=0,not_complex do begin & subclock=tic(typenames[k]) & ret=ROT(*various_types[k],33,0.6,/INTERP) & toc,subclock & end ',$
     'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & BYTEORDER,*various_types[k],/LSWAP & toc,subclock & end ',$
     'print,what[i] & x = FINDGEN(100)*0.02 & y=sin(x) & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & res=interpol(y,x,*various_types[k]) & toc,subclock & end ']

   for i=0,n_elements(calls)-1 do begin & clock=tic(what[i])  & z=execute(calls[i]) &  toc,clock & end
  print
endif

  toc,masterclock
end

; to do :string things, that are 2 times slower than idl (up to several seconds)
; ERF , ERFC , ERFCX , EXP , EXPINT, GAMMA , GAUSSINT, LNGAMMA , MATRIX_MULTIPLY, VOIGT, REPLICATE_INPLACE
;   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=string(*various_types[k]) & toc,subclock & end ',$
;   'print,what[i] & for k=0,all_numeric do begin & subclock=tic(typenames[k]) & ret=strtrim(*various_types[k],2) & toc,subclock & end ',$
