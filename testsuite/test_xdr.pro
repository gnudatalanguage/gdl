;
; Initial work by Gilles D., March 2015
;
; ---------------------------------
; 
; Modifications history :
;
; - 2015-August : AC. Few cleaning
;   (better way to locate the reference file "idl.xdr")
;
; - 2018-02-05 : AC. cleaning ... and working in TMPDIR
;
; -----------------------------------------------
;
pro TEST_XDR_WRITE, file, compress=compress
common testxdr,datar,dataw
;
GET_LUN, nlun
OPENW, nlun, /XDR, file, compress=compress 
WRITEU, nlun, dataw
CLOSE, nlun
FREE_LUN, nlun
;
if FILE_TEST(file) then MESSAGE, /continue, 'Succesfully write of : '+file
;
end
;
; -----------------------------------------------
;
pro TEST_XDR_READ, file, cumul_errors, compress=compress, txt=txt, test=test
common testxdr,datar,dataw
;
GET_LUN, nlun
OPENR, nlun, /XDR, file, compress=compress
READU, nlun, datar
CLOSE, nlun
FREE_LUN, nlun
;
; being able to read does not means the data inside are OK !
if FILE_TEST(file) then MESSAGE, /continue, 'Succesfully read of : '+file
;
errors=0
;
for i=0,N_TAGS(dataw)-1 do begin
   if (TOTAL(dataw.(i) eq datar.(i)) ne N_ELEMENTS(dataw.(i)) ) then begin
      mess='FAILED at tag #'+string(i)+" in file "+file
      ERRORS_ADD, errors, mess
   endif
endfor
;
BANNER_FOR_TESTSUITE, txt, errors, /short, prefix="TEST_XDR_READ"
ERRORS_RESET, cumul_errors, errors
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
   ;; just in case, testing the current dir.
   file_idl_xdr=FILE_SEARCH(GETENV('PWD')+PATH_SEP()+filename)
endif
;
if ~FILE_TEST(file_idl_xdr) then begin
   MESSAGE, 'file <<'+filename+'>> not found in the !PATH', /continue
   if KEYWORD_SET(no_exit) OR KEYWORD_SET(test) then STOP
   EXIT, status=1
endif else begin
   if (N_ELEMENTS(file_idl_xdr) GT 1) then begin
      file_idl_xdr=file_idl_xdr[0]
      print, 'more than one file found, read the first one ...'
   endif
   print, 'Reading back : '+file_idl_xdr
endelse
;
; counting the errors
cumul_errors=0
;
; test 1 : reading back the reference file, checking content
; do we agree with the content of the input file ?
;
txt='Testing reading back the input file'
TEST_XDR_READ, file_idl_xdr, cumul_errors, test=test, txt=txt
;
; tmpdir
tmpdir=GETENV('IDL_TMPDIR')
if STRLEN(tmpdir) GT 0 then begin
   last=STRMID(tmpdir, STRLEN(tmpdir)-1, 1)
   if (last NE PATH_SEP()) then tmpdir=tmpdir+PATH_SEP()
endif
;
radix=GDL_IDL_FL(/lower)
;
file_out1=tmpdir+radix+'.xdr'
file_out2=tmpdir+radix+'.xdr.gz'
;
; test 2 : writing and reading back a normal one, checking content
;
; WRITE our own XDR files in TMPDIR
TEST_XDR_WRITE, file_out1
;
; reread it and compare
txt='Testing the generated XDR file'
TEST_XDR_READ, file_out1, cumul_errors, test=test, txt=txt
;
; test 3 : writing and reading back a compressed one, checking content
;
; WRITE our own compressed
TEST_XDR_WRITE, file_out2, /compress
;
; reread it and compare
txt='Testing the generated XDR (compress) file'
TEST_XDR_READ, file_out2, cumul_errors, test=test, txt=txt, /compress
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_XDR', cumul_errors, short=short
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
