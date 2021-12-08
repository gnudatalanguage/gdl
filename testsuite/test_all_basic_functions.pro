pro test_all_basic_functions, size=size

  tic

  
  if (n_elements(size) eq 0 ) then size=1000000
; initialisations

  command=["BYTARR","COMPLEXARR","DBLARR","DCOMPLEXARR","FLTARR","INTARR","LON64ARR","LONARR","UINTARR","ULON64ARR","ULONARR","OBJARR","PTRARR"]
  calls="ret ="+command+"(size)"
  for i=0,n_elements(command)-1 do begin  & clock=tic(command[i]) & z=execute(calls[i]) & toc,clock & end

  command=["BINDGEN","CINDGEN","DCINDGEN","DINDGEN","FINDGEN","INDGEN","L64INDGEN","LINDGEN","SINDGEN","UINDGEN","UL64INDGEN","ULINDGEN"]
  calls="ret ="+command+"(size)"
  for i=0,n_elements(command)-1 do begin &  clock=tic(command[i]) & z=execute(calls[i]) & toc,clock & end

  
  usual_typecodes=[1,2,3,4,5,6,9,12,13,14,15]
  seed=33
  cote=long(sqrt(size)) > 8
  a=randomn(seed,cote*cote,/double)*randomu(seed,cote*cote,/ulong)
  various_types=ptrarr(11,/allo)
k=0 & foreach i,usual_typecodes do begin & *various_types[k]=fix(a,type=i) & k++ &end

   command=["BYTE", "COMPLEX", "DCOMPLEX", "DOUBLE", "FIX", "FLOAT", $
      "LONG", "LONG64", "STRING", "ULONG", "ULONG64"]
     calls="for k=0,10 do ret="+command+"(*various_types[k])"
     for i=0,n_elements(command)-1 do begin & clock=tic(command[i])  &  z=execute(calls[i]) & toc,clock & end
  what=['BYTSCL','SORT','MEDIAN','MEAN','MOMENT','TRANSPOSE','STRING','STRTRIM','WHERE','TOTAL','PRODUCT','MIN','MAX','FINITE','SHIFT','ISHFT','LOGICAL_AND','LOGICAL_OR','LOGICAL_TRUE','ATAN']
  calls=['for k=0,10 do ret=BYTSCL(*various_types[k],max=10,min=1,/nan,top=100)',$
     'for k=0,10 do ret=sort(*various_types[k])',$
   'for k=0,10 do ret=median(*various_types[k])',$
   'for k=0,10 do ret=mean(*various_types[k],/nan)',$
   'for k=0,10 do ret=moment(*various_types[k],/nan)',$
   'for k=0,10 do ret=transpose(*various_types[k])',$
   'for k=0,10 do ret=string(*various_types[k])',$
     'for k=0,10 do ret=strtrim(*various_types[k],2)',$
     'for k=0,10 do ret=where(*various_types[k] eq 0)',$
     'for k=0,10 do ret=total(*various_types[k])',$
     'for k=0,10 do ret=product(*various_types[k])',$
     'for k=0,10 do ret=min(*various_types[k],max=max)',$
     'for k=0,10 do ret=max(*various_types[k],min=min)',$
     'for k=0,10 do ret=finite(*various_types[k])',$
     'for k=0,10 do ret=shift(*various_types[k],2)',$
     'for k=0,10 do ret=ishft(*various_types[k],2)',$
     'for k=0,10 do ret=logical_and(*various_types[k],!dpi)',$
     'for k=0,10 do ret=logical_or(*various_types[k],!dpi)',$
     'for k=0,10 do ret=logical_true(*various_types[k])',$
     'for k=0,10 do ret=atan(*various_types[k],*various_types[k])']
     

  for i=0,n_elements(calls)-1 do begin & clock=tic(what[i])  & z=execute(calls[i]) &  toc,clock & end

  command=["LOGICAL_TRUE","SIN","COS","TAN","SINH","COSH","TANH","ASIN","ACOS","ALOG","ALOG2","ALOG10","SQRT","ABS","EXP","CONJ","IMAGINARY","ROUND","CEIL","FLOOR"]

     calls="for k=0,10 do ret="+command+"(*various_types[k])"
     for i=0,n_elements(command)-1 do begin & clock=tic(command[i])  &  z=execute(calls[i]) &  toc,clock & end
  
  toc
end
