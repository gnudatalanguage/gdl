;
; Testing *INDGEN family with keywords
;

pro test_indgen, test=test, no_exit=no_exit

  nerr=0

  ; Type specialised functions 
  if typename(bindgen(5)) ne "BYTE" then ADD_ERROR, nerr, 'BINDGEN does not yield a BYTE array'
  if typename(cindgen(5)) ne "COMPLEX" then ADD_ERROR, nerr, 'CINDGEN does not yield a COMPLEX array'
  if typename(dcindgen(5)) ne "DCOMPLEX" then ADD_ERROR, nerr, 'DCINDGEN does not yield a DCOMPLEX array'
  if typename(dindgen(5)) ne "DOUBLE" then ADD_ERROR, nerr, 'DINDGEN does not yield a DOUBLE array'
  if typename(findgen(5)) ne "FLOAT" then ADD_ERROR, nerr, 'FINDGEN does not yield a FLOAT array'
  if typename(indgen(5)) ne "INT" then ADD_ERROR, nerr, 'INDGEN does not yield an INT array'
  if typename(lindgen(5)) ne "LONG" then ADD_ERROR, nerr, 'LINDGEN does not yield a LONG array'
  if typename(l64indgen(5)) ne "LONG64" then ADD_ERROR, nerr, 'L64INDGEN does not yield a LONG64 array'
  if typename(sindgen(5)) ne "STRING" then ADD_ERROR, nerr, 'SINDGEN does not yield a STRING array'
  if typename(uindgen(5)) ne "UINT" then ADD_ERROR, nerr, 'UINDGEN does not yield an UINT array'
  if typename(ulindgen(5)) ne "ULONG" then ADD_ERROR, nerr, 'ULINDGEN does not yield an ULONG array'
  if typename(ul64indgen(5)) ne "ULONG64" then ADD_ERROR, nerr, 'UL64INDGEN does not yield an ULONG64 array'

  ; INDGEN with explicit type keywords
  if typename(indgen(5, /BYTE)) ne "BYTE" then ADD_ERROR, nerr, 'INDGEN(/BYTE) does not yield a BYTE array'
  if typename(indgen(5, /COMPLEX)) ne "COMPLEX" then ADD_ERROR, nerr, 'INDGEN(/COMPLEX) does not yield a COMPLEX array'
  if typename(indgen(5, /DCOMPLEX)) ne "DCOMPLEX" then ADD_ERROR, nerr, 'INDGEN(/DCOMPLEX) does not yield a DCOMPLEX array'
  if typename(indgen(5, /DOUBLE)) ne "DOUBLE" then ADD_ERROR, nerr, 'INDGEN(/DOUBLE) does not yield a DOUBLE array'
  if typename(indgen(5, /FLOAT)) ne "FLOAT" then ADD_ERROR, nerr, 'INDGEN(/FLOAT) does not yield a FLOAT array'
  if typename(indgen(5, /L64)) ne "LONG64" then ADD_ERROR, nerr, 'INDGEN(/L64) does not yield a LONG64 array'
  if typename(indgen(5, /LONG)) ne "LONG" then ADD_ERROR, nerr, 'INDGEN(/LONG) does not yield a LONG array'
  if typename(indgen(5, /STRING)) ne "STRING" then ADD_ERROR, nerr, 'INDGEN(/STRING) does not yield a STRING array'
  if typename(indgen(5, /UINT)) ne "UINT" then ADD_ERROR, nerr, 'INDGEN(/UINT) does not yield an UINT array'
  if typename(indgen(5, /UL64)) ne "ULONG64" then ADD_ERROR, nerr, 'INDGEN(/UL64) does not yield an ULONG64 array'
  if typename(indgen(5, /ULONG)) ne "ULONG" then ADD_ERROR, nerr, 'INDGEN(/ULONG) does not yield an ULONG array'

  ; Check correct number of elements in 1-dimensional arrays
  if size(bindgen(5), /N_ELEMENTS) ne 5 then ADD_ERROR, nerr, 'BINDGEN(5) does not yield a 5-element array'
  if size(cindgen(5), /N_ELEMENTS) ne 5 then ADD_ERROR, nerr, 'CINDGEN(5) does not yield a 5-element array'
  if size(dcindgen(5), /N_ELEMENTS) ne 5 then ADD_ERROR, nerr, 'DCINDGEN(5) does not yield a 5-element array'
  if size(dindgen(5), /N_ELEMENTS) ne 5 then ADD_ERROR, nerr, 'DINDGEN(5) does not yield a 5-element array'
  if size(findgen(5), /N_ELEMENTS) ne 5 then ADD_ERROR, nerr, 'FINDGEN(5) does not yield a 5-element array'
  if size(indgen(5), /N_ELEMENTS) ne 5 then ADD_ERROR, nerr, 'INDGEN(5) does not yield a 5-element array'
  if size(lindgen(5), /N_ELEMENTS) ne 5 then ADD_ERROR, nerr, 'LINDGEN(5) does not yield a 5-element array'
  if size(l64indgen(5), /N_ELEMENTS) ne 5 then ADD_ERROR, nerr, 'L64INDGEN(5) does not yield a 5-element array'
  if size(sindgen(5), /N_ELEMENTS) ne 5 then ADD_ERROR, nerr, 'SINDGEN(5) does not yield a 5-element array'
  if size(uindgen(5), /N_ELEMENTS) ne 5 then ADD_ERROR, nerr, 'UINDGEN(5) does not yield a 5-element array'
  if size(ulindgen(5), /N_ELEMENTS) ne 5 then ADD_ERROR, nerr, 'ULINDGEN(5) does not yield a 5-element array'
  if size(ul64indgen(5), /N_ELEMENTS) ne 5 then ADD_ERROR, nerr, 'UL64INDGEN(5) does not yield a 5-element array'

  ; Various IDL 8.2.1 START and IDL 8.3 INCREMENT tests
  if not array_equal(indgen(6, start=5, increment=0.5), [5,5,6,6,7,7]) then ADD_ERROR, nerr, 'INDGEN(START=5, INCREMENT=0.5) yields wrong result' 
  if not array_equal(findgen(6, start=5, increment=0.5), [5.0,5.5,6.0,6.5,7.0,7.5]) then ADD_ERROR, nerr, 'FINDGEN(START=5, INCREMENT=0.5) yields wrong result'
  if not array_equal(bindgen(6, start=5, increment=4), [5,9,13,17,21,25]) then ADD_ERROR, nerr, 'BINDGEN(START=5, INCREMENT=4) yields wrong result'
  if not array_equal(uindgen(2, start=2018), [2018, 2019]) then ADD_ERROR, nerr, 'UINDGEN(START=2018) yields wrong result'
  ; The same with new inline syntax, a few tricks
  a=[22:22:0.1] & if not array_equal(a, [22]) then ADD_ERROR, nerr, 'a=[22:22:0.1] yields wrong result' 
  a=[22:32.3:0.2] & if not array_equal(a, findgen(52,start=22,increment=0.2)) then ADD_ERROR, nerr, "a=[22:32.3:0.2] yields wrong result" 
  a=['22':32.3:0.2] & if not array_equal(a, sindgen(52,start=22,increment=0.2)) then ADD_ERROR, nerr, "a=['22':32.3:0.2] yields wrong result" 
  a=[22:32.3:complex(0.2,0)] & if not array_equal(a, cindgen(52,start=22,increment=0.2)) then ADD_ERROR, nerr, "a=[22:32.3:complex(0.2,0)] yields wrong result"
  a=[(3 gt 2) ? 10:20 :100:1] & if not array_equal(a, indgen(91,start=10,increment=1)) then ADD_ERROR, nerr, "a=[(3 gt 2) ? 10:20 :100:1] yields wrong result"
  ; Is our parallel indgen generation robust? Compare with sample.
  restore,"indgen_sample.sav" ; defines "samplesize".
  b_gdl=randomu(33,1000)*samplesize
  byt_gdl=bindgen(samplesize,start=33.122,incr=0.017)
  f_gdl=findgen(samplesize,start=33.122,incr=0.017)
  i_gdl=indgen(samplesize,start=33.122,incr=0.017)
  d_gdl=dindgen(samplesize,start=33.122,incr=0.017)
  u_gdl=uindgen(samplesize,start=33.122,incr=0.017)
  c_gdl=cindgen(samplesize,start=33.122,incr=0.017)
  dc_gdl=dcindgen(samplesize,start=33.122,incr=0.017)
  if (total(b-b_gdl) ne 0) then  ADD_ERROR, nerr, 'Internal logic error, randomu values not equal with IDL'
  if (total(byt_gdl[b]-byt) ne 0)  then  ADD_ERROR, nerr, 'parallel large bindgen yields wrong results' 
  if (total(f_gdl[b]-f) ne 0)  then  ADD_ERROR, nerr, 'parallel large findgen yields wrong results' 
  if (total(d_gdl[b]-d) ne 0)  then  ADD_ERROR, nerr, 'parallel large dindgen yields wrong results' 
  if (total(i_gdl[b]-i) ne 0)  then  ADD_ERROR, nerr, 'parallel large indgen yields wrong results' 
  if (total(u_gdl[b]-u) ne 0)  then  ADD_ERROR, nerr, 'parallel large uindgen yields wrong results' 
  if (total(c_gdl[b]-c) ne complex(0,0))  then  ADD_ERROR, nerr, 'parallel large cindgen yields wrong results' 
  if (total(dc_gdl[b]-dc) ne dcomplex(0,0))  then  ADD_ERROR, nerr, 'parallel large dcindgen yields wrong results' 

  BANNER_FOR_TESTSUITE, 'test_indgen', nerr

  if nerr gt 0 and ~keyword_set(no_exit) then exit, status=1
  if keyword_set(test) then stop

end
; for the record: how the sample is made:
samplesize=10000000LL
b=randomu(33,1000)*samplesize
b_idl=bindgen(samplesize,start=33.122,incr=0.017)
f_idl=findgen(samplesize,start=33.122,incr=0.017)
i_idl=indgen(samplesize,start=33.122,incr=0.017)
d_idl=dindgen(samplesize,start=33.122,incr=0.017)
u_idl=uindgen(samplesize,start=33.122,incr=0.017)
c_idl=cindgen(samplesize,start=33.122,incr=0.017)
dc_idl=dcindgen(samplesize,start=33.122,incr=0.017)
byt=b_idl[b]
i=i_idl[b]
d=d_idl[b]
f=f_idl[b]
u=u_idl[b]
c=c_idl[b]
dc=dc_idl[b]
save,samplesize,byt,f,d,i,u,c,dc,b,file="indgen_sample.sav"

