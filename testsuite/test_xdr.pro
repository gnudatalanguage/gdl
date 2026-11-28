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
function TEST_XDR_READ_ERROR, u, dest, expected
catch, err
if err ne 0 then begin
   msg=!error_state.msg
   catch, /cancel
   return, strpos(msg, expected) ge 0
endif
readu, u, dest
return, 0
end
;
pro TEST_XDR_COUNTS, file, cumul_errors, compress=compress
catch, err
if err ne 0 then begin
   msg=!error_state.msg
   catch, /cancel
   ERRORS_ADD, cumul_errors, msg
   if n_elements(u) ne 0 then free_lun, u
   return
endif
get_lun, u
marker='12345678'xl
DEFSYSV, '!GDL', exists=is_gdl
foreach n, [32767L, 32768L, 32769L, 65535L, 65536L, 65537L] do begin
   data=bindgen(n)
   openw, u, file, /xdr, compress=compress
   writeu, u, data, marker
   close, u
   ;
   openr, u, file, /swap_if_little_endian, compress=compress
   count=0ul & back=bytarr(n) & following=0L
   readu, u, count, back
   pad=(4 - n mod 4) mod 4
   if pad gt 0 then begin
      padding=bytarr(pad)
      readu, u, padding
      if max(padding) ne 0 then ERRORS_ADD, cumul_errors, 'Nonzero XDR byte padding'
   endif
   readu, u, following
   close, u
   if count ne n or following ne marker or ~array_equal(back, data) then $
      ERRORS_ADD, cumul_errors, 'Wrong XDR byte encoding for length '+strtrim(n,2)
   ;
   openr, u, file, /xdr, compress=compress
   back=bytarr(n) & following=0L
   readu, u, back, following
   close, u
   if following ne marker or ~array_equal(back, data) then $
      ERRORS_ADD, cumul_errors, 'Wrong XDR byte decoding for length '+strtrim(n,2)
endforeach
;
; IDL 9.1 consumes one count word plus the destination size rounded up to four.
openw, u, file, /xdr, compress=compress
writeu, u, bindgen(6), -1L
close, u
openr, u, file, /xdr, compress=compress
dest=replicate(77b,3) & following=0L
readu, u, dest
point_lun, -u, pos
readu, u, following
if pos ne 8 or ~array_equal(dest, [0b,1b,2b]) or following ne '04050000'xl then $
   ERRORS_ADD, cumul_errors, 'Wrong position, bytes, or following LONG for a 3-byte destination'
close, u
openr, u, file, /xdr, compress=compress
dest=replicate(77b,9)
readu, u, dest
point_lun, -u, pos
; The ninth byte is the first byte of the following LONG, after two padding bytes.
if pos ne 16 or ~array_equal(dest, [bindgen(6),0b,0b,255b]) then $
   ERRORS_ADD, cumul_errors, 'Wrong position or bytes for a 9-byte destination'
if ~TEST_XDR_READ_ERROR(u, following, 'End of file') then $
   ERRORS_ADD, cumul_errors, 'Expected EOF reading the LONG after a 9-byte destination'
close, u
;
foreach n, [32767L,32768L,70000L] do begin
   value=string(replicate(97b,n))
   openw, u, file, /xdr, compress=compress
   writeu, u, value, 7L
   close, u
   ; Decode the wire format independently of the XDR string reader.
   openr, u, file, /swap_if_little_endian, compress=compress
   outer=0L & inner=0ul & bytes=bytarr(n) & following=0L
   readu, u, outer, inner, bytes
   pad=(4 - n mod 4) mod 4
   if pad gt 0 then begin
      padding=bytarr(pad)
      readu, u, padding
      if max(padding) ne 0 then ERRORS_ADD, cumul_errors, 'Nonzero XDR string padding'
   endif
   readu, u, following
   if outer ne n or inner ne n or min(bytes) ne 97b or max(bytes) ne 97b or following ne 7 then $
      ERRORS_ADD, cumul_errors, 'Wrong XDR string encoding for length '+strtrim(n,2)
   close, u
   openr, u, file, /xdr, compress=compress
   back_string='' & following=0L
   readu, u, back_string, following
   point_lun, -u, pos
   if back_string ne value or following ne 7 or pos ne 12L+n+pad then $
      ERRORS_ADD, cumul_errors, 'Wrong XDR string contents, marker, or position for length '+strtrim(n,2)
   close, u
endforeach
;
if is_gdl then begin
   ; One data byte is present but its three XDR alignment bytes are missing.
   openw, u, file, compress=compress
   writeu, u, [0b,0b,0b,1b,9b]
   close, u
   openr, u, file, /xdr, compress=compress
   dest=77b
   if ~TEST_XDR_READ_ERROR(u, dest, 'Problem reading XDR file') then $
      ERRORS_ADD, cumul_errors, 'Expected an error reading a truncated XDR byte record'
   if dest ne 77b then $
      ERRORS_ADD, cumul_errors, 'Truncated XDR byte record changed the destination'
   close, u
   ; The count is valid but there is no string payload.
   openw, u, file, /swap_if_little_endian, compress=compress
   writeu, u, '7fffffff'xl, '7fffffff'xl
   close, u
   openr, u, file, /xdr, compress=compress
   back_string='unchanged'
   if ~TEST_XDR_READ_ERROR(u, back_string, 'Problem reading XDR file') then $
      ERRORS_ADD, cumul_errors, 'Expected an error reading a truncated XDR string'
   if back_string ne 'unchanged' then ERRORS_ADD, cumul_errors, 'Truncated XDR string changed the destination'
   close, u
endif
free_lun, u
file_delete, file
end
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
; An empty counted string occupies one XDR length word.
strings = ['first', '', 'last']
openw, u, file_out1+'.strings', /get_lun, /xdr, /delete
writeu, u, strings
point_lun, u, 0
back = strarr(3)
readu, u, back
free_lun, u
if ~array_equal(back, strings) then ERRORS_ADD, cumul_errors, 'XDR string alignment after an empty string'
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
byte_file=FILEPATH('test_xdr_bytes_'+STRTRIM(RANDOMU(seed,/ulong),2), /tmp)
TEST_XDR_COUNTS, byte_file, cumul_errors
TEST_XDR_COUNTS, byte_file, cumul_errors, /compress
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
