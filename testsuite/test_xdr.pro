;
; Initial work by  Gilles D., March 2015
;
; Few cleaning up by AC end of August 2015
; (better way to locate the reference file "idl.xdr")
;
; -----------------------------------------------
;
pro ADD_ERROR, nb_errors, message
;
print, 'Error on operation : '+message
nb_errors=nb_errors+1
;
end
;
; -----------------------------------------------
;
pro TEST_WRITE_XDR
common testxdr,datar,dataw
;
GET_LUN, nlun
OPENW, nlun, /XDR, 'gdl.xdr'
WRITEU, nlun, dataw
CLOSE, nlun
FREE_LUN, nlun
;
end
;
; -----------------------------------------------
;
pro TEST_WRITE_COMPRESSED_XDR
common testxdr,datar,dataw
;
GET_LUN, nlun
OPENW, nlun, /XDR, 'gdl.xdr.gz', /COMPRESS
WRITEU, nlun, dataw
CLOSE, nlun
FREE_LUN, nlun
;
end
;
; -----------------------------------------------
;
pro TEST_READ_XDR, file, errors, compress=compress, test=test
common testxdr,datar,dataw
;
GET_LUN, nlun
OPENR, nlun, /XDR, file, compress=compress
READU, nlun, datar
CLOSE, nlun
FREE_LUN, nlun
;
if ~ISA(errors) then errors=0
;
for i=0,N_TAGS(dataw)-1 do begin
    if (TOTAL(dataw.(i) eq datar.(i)) ne N_ELEMENTS(dataw.(i)) ) then begin
        MESSAGE, 'FAILED at tag #'+string(i)+" in file "+file, /continue
        errors++
    endif
endfor
;
if KEYWORD_SET(test) then STOP
;
end
;
; -----------------------------------------------
;
pro TEST_XDR, help=help, test=test, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_XDR, help=help, test=test, no_exit=no_exit'
   return
endif
;
common testxdr, datar, dataw
;
DATAR={BytesAtStart: BYTARR(13), ShortArray: INTARR(3), $
       UnitArray: UINTARR(3), longarray: LONARR(3), $
       ulongarray: ULONARR(3), long64array: LON64ARR(3), $
       stringarray: STRARR(2), ulong64array: ULON64ARR(3), $
       floatarray: FLTARR(3), doublearray: DBLARR(3), $
       complexarray: COMPLEXARR(2), doublecomplexarray: DCOMPLEXARR(2),$
       finalbytesum: 0b}
;
DATAW={ BytesAtStart: BYTE(INDGEN(13)),ShortArray: FIX([-1,-2,-3]), $
        UnitArray: UINT([1,2,3]),longarray: LONG([-1,-2,-3]), $
        ulongarray: ULONG([1,2,3]),long64array: LONG64([-1,-2,-3]), $
        stringarray: ['String1','a Longer String:2'], $
        ulong64array: ULONG64([1,2,3]),floatarray: FLOAT([1,2,3]), $
        doublearray: DOUBLE([1,2,3]), $
        complexarray: [COMPLEX(1,2),COMPLEX(3,4)], $
        doublecomplexarray: [DCOMPLEX(12.,13.),DCOMPLEX(14,15.)], $
        finalbytesum: 0b}
;
dataw.finalbytesum=N_TAGS(dataw)
;
; locating then read back the reference idl xdr:
;
filename='idl.xdr'
list_of_dirs=STRSPLIT(!PATH, PATH_SEP(/SEARCH_PATH), /EXTRACT)
file_idl_xdr=FILE_SEARCH(list_of_dirs+PATH_SEP()+filename)
;
if ~FILE_TEST(file_idl_xdr) then begin
    MESSAGE, 'file <<'+filename+'>> not found in the !PATH', /continue
    if KEYWORD_SET(no_exit) OR KEYWORD_SET(test) then STOP
    EXIT, status=1
endif
;
; counting the errors
errors=0
;
; do we agree with the input file ?
;
TEST_READ_XDR, file_idl_xdr, errors, test=test
BANNER_FOR_TESTSUITE, 'Testing the input file', errors, /short
;
; WRITE our own
TEST_WRITE_XDR
;
; counting the errors
errors=0
;
; reread it and compare
TEST_READ_XDR, 'gdl.xdr', errors, test=test
BANNER_FOR_TESTSUITE, 'Testing the generated XDR file', errors, /short
;
; WRITE our own compressed
TEST_WRITE_COMPRESSED_XDR
;
; reread it and compare
TEST_READ_XDR, 'gdl.xdr.gz', errors, test=test, /compress
BANNER_FOR_TESTSUITE, 'Testing the generated XDR (compress) file', errors, /short
;
BANNER_FOR_TESTSUITE, 'TEST_XDR', errors, short=short
;
if (errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
