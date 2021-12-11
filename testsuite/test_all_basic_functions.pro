pro test_all_basic_functions, size=size

  masterclock=tic("ALL TESTS")

  
  if (n_elements(size) eq 0 ) then size=1000000
; initialisations

  command=["BYTARR","COMPLEXARR","DBLARR","DCOMPLEXARR","FLTARR","INTARR","LON64ARR","LONARR","UINTARR","ULON64ARR","ULONARR","OBJARR","PTRARR"]
  calls="ret ="+command+"(size)"
  for i=0,n_elements(command)-1 do begin  & clock=tic(command[i]) & z=execute(calls[i]) & toc,clock & end
  print

  command=["BINDGEN","CINDGEN","DCINDGEN","DINDGEN","FINDGEN","INDGEN","L64INDGEN","LINDGEN","SINDGEN","UINDGEN","UL64INDGEN","ULINDGEN"]
  calls="ret ="+command+"(size)"
  for i=0,n_elements(command)-1 do begin &  clock=tic(command[i]) & z=execute(calls[i]) & toc,clock & end
  print

  
  typecodes=[1,2,3,4,5,6,9,12,13,14,15]
  typenames=["		BYTE","		INT","		LONG","		FLOAT","		DOUBLE","		COMPLEX","		DCOMPLEX","		UINT","		ULONG","		LONG64","		ULONG64"]
  seed=33
  cote=long(sqrt(size)) > 8
  a=randomn(seed,cote,cote,/double)*randomu(seed,cote,cote,/ulong)
  various_types=ptrarr(11,/allo)
k=0 & foreach i,typecodes do begin & *various_types[k]=fix(a,type=i) & k++ &end

   command=["BYTE", "COMPLEX", "DCOMPLEX", "DOUBLE", "FIX", "FLOAT", $
      "LONG", "LONG64", "STRING", "ULONG", "ULONG64"]
;   command=["BYTE", "FLOAT", "FLOAT", "DOUBLE", "FIX", "FLOAT", $
;      "LONG", "LONG64", "STRING", "ULONG", "ULONG64"]
   calls="for k=0,10 do ret="+command+"(*various_types[k])"
     for i=0,n_elements(command)-1 do begin & clock=tic(command[i])  &  z=execute(calls[i]) & toc,clock & end
  what=['BYTSCL','SORT','MEDIAN','MEAN','MOMENT','TRANSPOSE','WHERE','TOTAL','PRODUCT','MIN','MAX','FINITE','SHIFT','LOGICAL_AND','LOGICAL_OR','LOGICAL_TRUE','ATAN']
  calls=[$
   'print,what[i] & for k=0,10 do begin & subclock=tic(typenames[k]) & ret=BYTSCL(*various_types[k],max=10,min=1,/nan,top=100) & toc,subclock & end ',$
   'print,what[i] & for k=0,10 do begin & subclock=tic(typenames[k]) & ret=sort(*various_types[k]) & toc,subclock & end ',$
   'print,what[i] & for k=0,10 do begin & subclock=tic(typenames[k]) & ret=median(*various_types[k]) & toc,subclock & end ',$
   'print,what[i] & for k=0,10 do begin & subclock=tic(typenames[k]) & ret=mean(*various_types[k],/nan) & toc,subclock & end ',$
   'print,what[i] & for k=0,10 do begin & subclock=tic(typenames[k]) & ret=moment(*various_types[k],/nan) & toc,subclock & end ',$
   'print,what[i] & for k=0,10 do begin & subclock=tic(typenames[k]) & ret=transpose(*various_types[k]) & toc,subclock & end ',$
   'print,what[i] & for k=0,10 do begin & subclock=tic(typenames[k]) & ret=where(*various_types[k] eq 0) & toc,subclock & end ',$
   'print,what[i] & for k=0,10 do begin & subclock=tic(typenames[k]) & ret=total(*various_types[k]) & toc,subclock & end ',$
   'print,what[i] & for k=0,10 do begin & subclock=tic(typenames[k]) & ret=product(*various_types[k]) & toc,subclock & end ',$
   'print,what[i] & for k=0,10 do begin & subclock=tic(typenames[k]) & ret=min(*various_types[k],max=max) & toc,subclock & end ',$
   'print,what[i] & for k=0,10 do begin & subclock=tic(typenames[k]) & ret=max(*various_types[k],min=min) & toc,subclock & end ',$
   'print,what[i] & for k=0,10 do begin & subclock=tic(typenames[k]) & ret=finite(*various_types[k]) & toc,subclock & end ',$
   'print,what[i] & for k=0,10 do begin & subclock=tic(typenames[k]) & ret=shift(*various_types[k],[2,3]) & toc,subclock & end ',$
   'print,what[i] & for k=0,10 do begin & subclock=tic(typenames[k]) & ret=logical_and(*various_types[k],!dpi) & toc,subclock & end ',$
   'print,what[i] & for k=0,10 do begin & subclock=tic(typenames[k]) & ret=logical_or(*various_types[k],!dpi) & toc,subclock & end ',$
   'print,what[i] & for k=0,10 do begin & subclock=tic(typenames[k]) & ret=logical_true(*various_types[k]) & toc,subclock & end ',$
   'print,what[i] & for k=0,10 do begin & subclock=tic(typenames[k]) & ret=atan(*various_types[k],*various_types[k]) & toc,subclock & end ']
     

  for i=0,n_elements(calls)-1 do begin & clock=tic(what[i])  & z=execute(calls[i]) &  toc,clock & end
  print
  command=["SIN","COS","TAN","SINH","COSH","TANH","ASIN","ACOS","ALOG","ALOG2","ALOG10","SQRT","ABS","EXP","CONJ","IMAGINARY","ROUND","CEIL","FLOOR"]

     calls="for k=0,10 do ret="+command+"(*various_types[k])"
     for i=0,n_elements(command)-1 do begin & clock=tic(command[i])  &  z=execute(calls[i]) &  toc,clock & end
  print
  
  toc,masterclock
end
; ishft needs integer
;    'print,what[i] & for k=0,10 do begin & subclock=tic(typenames[k]) & ret=ishft(*various_types[k],[2,3]) & toc,subclock & end ',$
; to do :string things
;   'print,what[i] & for k=0,10 do begin & subclock=tic(typenames[k]) & ret=string(*various_types[k]) & toc,subclock & end ',$
;   'print,what[i] & for k=0,10 do begin & subclock=tic(typenames[k]) & ret=strtrim(*various_types[k],2) & toc,subclock & end ',$
