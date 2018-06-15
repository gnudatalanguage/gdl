;
; Testing *INDGEN family with keywords
;
; ----------------------------------------------------
; Modifications history :
;
; 2018-Jun-15 : AC: some rewriting
;
; ----------------------------------------------------
;
pro TEST_INDGEN_by_name, test=test, no_exit=no_exit
;
nerr=0
;
;; Type specialised functions 
;
if TYPENAME(BINDGEN(5)) ne "BYTE" then ERRORS_ADD, nerr, 'BINDGEN does not yield a BYTE array'
if TYPENAME(CINDGEN(5)) ne "COMPLEX" then ERRORS_ADD, nerr, 'CINDGEN does not yield a COMPLEX array'
if TYPENAME(DCINDGEN(5)) ne "DCOMPLEX" then ERRORS_ADD, nerr, 'DCINDGEN does not yield a DCOMPLEX array'
if TYPENAME(DINDGEN(5)) ne "DOUBLE" then ERRORS_ADD, nerr, 'DINDGEN does not yield a DOUBLE array'
if TYPENAME(FINDGEN(5)) ne "FLOAT" then ERRORS_ADD, nerr, 'FINDGEN does not yield a FLOAT array'
if TYPENAME(INDGEN(5)) ne "INT" then ERRORS_ADD, nerr, 'INDGEN does not yield an INT array'
if TYPENAME(LINDGEN(5)) ne "LONG" then ERRORS_ADD, nerr, 'LINDGEN does not yield a LONG array'
if TYPENAME(L64INDGEN(5)) ne "LONG64" then ERRORS_ADD, nerr, 'L64INDGEN does not yield a LONG64 array'
if TYPENAME(SINDGEN(5)) ne "STRING" then ERRORS_ADD, nerr, 'SINDGEN does not yield a STRING array'
if TYPENAME(UINDGEN(5)) ne "UINT" then ERRORS_ADD, nerr, 'UINDGEN does not yield an UINT array'
if TYPENAME(ULINDGEN(5)) ne "ULONG" then ERRORS_ADD, nerr, 'ULINDGEN does not yield an ULONG array'
if TYPENAME(UL64INDGEN(5)) ne "ULONG64" then ERRORS_ADD, nerr, 'UL64INDGEN does not yield an ULONG64 array'

end
;;
pro TEST_INDGEN_by_KEYWORD, test=test, no_exit=no_exit

;; INDGEN with explicit type keywords
;;
if TYPENAME(INDGEN(5, /BYTE)) ne "BYTE" then ERRORS_ADD, nerr, 'INDGEN(/BYTE) does not yield a BYTE array'
if TYPENAME(INDGEN(5, /COMPLEX)) ne "COMPLEX" then ERRORS_ADD, nerr, 'INDGEN(/COMPLEX) does not yield a COMPLEX array'
if TYPENAME(INDGEN(5, /DCOMPLEX)) ne "DCOMPLEX" then ERRORS_ADD, nerr, 'INDGEN(/DCOMPLEX) does not yield a DCOMPLEX array'
if TYPENAME(INDGEN(5, /DOUBLE)) ne "DOUBLE" then ERRORS_ADD, nerr, 'INDGEN(/DOUBLE) does not yield a DOUBLE array'
if TYPENAME(INDGEN(5, /FLOAT)) ne "FLOAT" then ERRORS_ADD, nerr, 'INDGEN(/FLOAT) does not yield a FLOAT array'
if TYPENAME(INDGEN(5, /L64)) ne "LONG64" then ERRORS_ADD, nerr, 'INDGEN(/L64) does not yield a LONG64 array'
if TYPENAME(INDGEN(5, /LONG)) ne "LONG" then ERRORS_ADD, nerr, 'INDGEN(/LONG) does not yield a LONG array'
if TYPENAME(INDGEN(5, /STRING)) ne "STRING" then ERRORS_ADD, nerr, 'INDGEN(/STRING) does not yield a STRING array'
if TYPENAME(INDGEN(5, /UINT)) ne "UINT" then ERRORS_ADD, nerr, 'INDGEN(/UINT) does not yield an UINT array'
if TYPENAME(INDGEN(5, /UL64)) ne "ULONG64" then ERRORS_ADD, nerr, 'INDGEN(/UL64) does not yield an ULONG64 array'
if TYPENAME(INDGEN(5, /ULONG)) ne "ULONG" then ERRORS_ADD, nerr, 'INDGEN(/ULONG) does not yield an ULONG array'
;;
;; Check correct number of elements in 1-dimensional arrays
;;
if SIZE(BINDGEN(5), /N_ELEMENTS) ne 5 then ERRORS_ADD, nerr, 'BINDGEN(5) does not yield a 5-element array'
if SIZE(CINDGEN(5), /N_ELEMENTS) ne 5 then ERRORS_ADD, nerr, 'CINDGEN(5) does not yield a 5-element array'
if SIZE(DCINDGEN(5), /N_ELEMENTS) ne 5 then ERRORS_ADD, nerr, 'DCINDGEN(5) does not yield a 5-element array'
if SIZE(DINDGEN(5), /N_ELEMENTS) ne 5 then ERRORS_ADD, nerr, 'DINDGEN(5) does not yield a 5-element array'
if SIZE(FINDGEN(5), /N_ELEMENTS) ne 5 then ERRORS_ADD, nerr, 'FINDGEN(5) does not yield a 5-element array'
if SIZE(INDGEN(5), /N_ELEMENTS) ne 5 then ERRORS_ADD, nerr, 'INDGEN(5) does not yield a 5-element array'
if SIZE(LINDGEN(5), /N_ELEMENTS) ne 5 then ERRORS_ADD, nerr, 'LINDGEN(5) does not yield a 5-element array'
if SIZE(L64INDGEN(5), /N_ELEMENTS) ne 5 then ERRORS_ADD, nerr, 'L64INDGEN(5) does not yield a 5-element array'
if SIZE(SINDGEN(5), /N_ELEMENTS) ne 5 then ERRORS_ADD, nerr, 'SINDGEN(5) does not yield a 5-element array'
if SIZE(UINDGEN(5), /N_ELEMENTS) ne 5 then ERRORS_ADD, nerr, 'UINDGEN(5) does not yield a 5-element array'
if SIZE(ULINDGEN(5), /N_ELEMENTS) ne 5 then ERRORS_ADD, nerr, 'ULINDGEN(5) does not yield a 5-element array'
if SIZE(UL64INDGEN(5), /N_ELEMENTS) ne 5 then ERRORS_ADD, nerr, 'UL64INDGEN(5) does not yield a 5-element array'
;;
;; Various IDL 8.2.1 START and IDL 8.3 INCREMENT tests
;;
if not ARRAY_EQUAL(INDGEN(6, start=5, increment=0.5), [5,5,6,6,7,7]) then ERRORS_ADD, nerr, 'INDGEN(START=5, INCREMENT=0.5) yields wrong result' 
if not ARRAY_EQUAL(FINDGEN(6, start=5, increment=0.5), [5.0,5.5,6.0,6.5,7.0,7.5]) then ERRORS_ADD, nerr, 'FINDGEN(START=5, INCREMENT=0.5) yields wrong result'
if not ARRAY_EQUAL(BINDGEN(6, start=5, increment=4), [5,9,13,17,21,25]) then ERRORS_ADD, nerr, 'BINDGEN(START=5, INCREMENT=4) yields wrong result'
if not ARRAY_EQUAL(UINDGEN(2, start=2018), [2018, 2019]) then ERRORS_ADD, nerr, 'UINDGEN(START=2018) yields wrong result'
;;
;; The same with new inline syntax, a few tricks
a=[22:22:0.1] & if not ARRAY_EQUAL(a, [22]) then ERRORS_ADD, nerr, 'a=[22:22:0.1] yields wrong result' 
a=[22:32.3:0.2] & if not ARRAY_EQUAL(a, FINDGEN(52,start=22,increment=0.2)) then ERRORS_ADD, nerr, "a=[22:32.3:0.2] yields wrong result" 
a=['22':32.3:0.2] & if not ARRAY_EQUAL(a, SINDGEN(52,start=22,increment=0.2)) then ERRORS_ADD, nerr, "a=['22':32.3:0.2] yields wrong result" 
a=[22:32.3:complex(0.2,0)] & if not ARRAY_EQUAL(a, CINDGEN(52,start=22,increment=0.2)) then ERRORS_ADD, nerr, "a=[22:32.3:complex(0.2,0)] yields wrong result"
a=[(3 gt 2) ? 10:20 :100:1] & if not ARRAY_EQUAL(a, INDGEN(91,start=10,increment=1)) then ERRORS_ADD, nerr, "a=[(3 gt 2) ? 10:20 :100:1] yields wrong result"
;
;; Is our parallel indgen generation robust? Compare with sample.
RESTORE, "indgen_sample.sav"   ; defines "sampleSIZE".
b_gdl=RANDOMU(33,1000)*sampleSIZE
byt_gdl=BINDGEN(sampleSIZE,start=33.122,incr=0.017)
f_gdl=FINDGEN(sampleSIZE,start=33.122,incr=0.017)
i_gdl=INDGEN(sampleSIZE,start=33.122,incr=0.017)
d_gdl=DINDGEN(sampleSIZE,start=33.122,incr=0.017)
u_gdl=UINDGEN(sampleSIZE,start=33.122,incr=0.017)
c_gdl=CINDGEN(sampleSIZE,start=33.122,incr=0.017)
dc_gdl=DCINDGEN(sampleSIZE,start=33.122,incr=0.017)
;
if (TOTAL(b-b_gdl) ne 0) then ERRORS_ADD, nerr, 'Internal logic error, RANDOMU values not equal with IDL'
if (TOTAL(byt_gdl[b]-byt) ne 0) then ERRORS_ADD, nerr, 'parallel large BINDGEN yields wrong results' 
if (TOTAL(f_gdl[b]-f) ne 0) then ERRORS_ADD, nerr, 'parallel large FINDGEN yields wrong results' 
if (TOTAL(d_gdl[b]-d) ne 0) then ERRORS_ADD, nerr, 'parallel large dindgen yields wrong results' 
if (TOTAL(i_gdl[b]-i) ne 0) then ERRORS_ADD, nerr, 'parallel large indgen yields wrong results' 
if (TOTAL(u_gdl[b]-u) ne 0) then ERRORS_ADD, nerr, 'parallel large uindgen yields wrong results' 
if (TOTAL(c_gdl[b]-c) ne COMPLEX(0,0)) then ERRORS_ADD, nerr, 'parallel large CINDGEN yields wrong results' 
if (TOTAL(dc_gdl[b]-dc) ne DCOMPLEX(0,0)) then ERRORS_ADD, nerr, 'parallel large dCINDGEN yields wrong results' 

BANNER_FOR_TESTSUITE, 'test_indgen', nerr, /status

if nerr gt 0 and ~KEYWORD_SET(no_exit) then exit, status=1
if KEYWORD_SET(test) then stop

end

pro TEST_INDGEN_SAMPLE_GENERATE, test=test
;
; for the record: how the sample is made:
;
sampleSIZE=10000000LL
b=RANDOMU(33,1000)*sampleSIZE
b_idl=BINDGEN(sampleSIZE,start=33.122,incr=0.017)
f_idl=FINDGEN(sampleSIZE,start=33.122,incr=0.017)
i_idl=INDGEN(sampleSIZE,start=33.122,incr=0.017)
d_idl=DINDGEN(sampleSIZE,start=33.122,incr=0.017)
u_idl=UINDGEN(sampleSIZE,start=33.122,incr=0.017)
c_idl=CINDGEN(sampleSIZE,start=33.122,incr=0.017)
dc_idl=DCINDGEN(sampleSIZE,start=33.122,incr=0.017)
;
byt=b_idl[b]
i=i_idl[b]
d=d_idl[b]
f=f_idl[b]
u=u_idl[b]
c=c_idl[b]
dc=dc_idl[b]
;
SAVE, sampleSIZE, byt,f,d,i,u,c,dc,b, file="indgen_sample.sav"
;
if KEYWORD_SET(test) then stop
;
end


pro TEST_INDGEN, help=help, test=test, no_exit=no_exit
;
;
end
