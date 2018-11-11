;
; Alain C., 2014/03/27
;
; very basic tests for TYPENAME() function
; 3 types (Obj, Struct, Pointer) are not ready at all)
;
; ---------------------------------
; Modifications history :
;
; 2018-Feb-02: AC. tests should now cover the 4 cases for Structures
;
; ---------------------------------
;
pro TEST_TYPENAME, help=help, test=test, verbose=verbose, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_TYPENAME, help=help, test=test, verbose=verbose, no_exit=no_exit'
   return
endif
;
nb_errors=0
;
if (TYPENAME(a_undef) NE "UNDEFINED") then ERRORS_ADD, nb_errors, 'bad type with UNDEFINED'
;
a=!null
if (TYPENAME(a) NE "UNDEFINED") then ERRORS_ADD, nb_errors, 'bad type with !NULL'
;
if (TYPENAME(1b) NE "BYTE") then ERRORS_ADD, nb_errors, 'bad type with BYTE'
if (TYPENAME(1) NE "INT") then ERRORS_ADD, nb_errors, 'bad type with INT'
if (TYPENAME(LONG(1)) NE "LONG") then ERRORS_ADD, nb_errors, 'bad type with LONG'
if (TYPENAME(1.) NE "FLOAT") then ERRORS_ADD, nb_errors, 'bad type with FLOAT'
if (TYPENAME(1.D) NE "DOUBLE") then ERRORS_ADD, nb_errors, 'bad type with DOUBLE'
if (TYPENAME(complex(1.,0)) NE "COMPLEX") then ERRORS_ADD, nb_errors, 'bad type with COMPLEX'
if (TYPENAME(dcomplex(1.,0)) NE "DCOMPLEX") then ERRORS_ADD, nb_errors, 'bad type with DCOMPLEX'
if (TYPENAME(1l) NE "LONG") then ERRORS_ADD, nb_errors, 'bad type with LONG'
if (TYPENAME(1ll) NE "LONG64") then ERRORS_ADD, nb_errors, 'bad type with LONG64'
if (TYPENAME(1ul) NE "ULONG") then ERRORS_ADD, nb_errors, 'bad type with ULONG'
if (TYPENAME(1ull) NE "ULONG64") then ERRORS_ADD, nb_errors, 'bad type with ULONG64'
;
if (TYPENAME(HASH([1,2,3])) NE "HASH") then ERRORS_ADD, nb_errors, 'bad type with HASH'
if (TYPENAME(LIST(1,2,3)) NE "LIST") then ERRORS_ADD, nb_errors, 'bad type with LIST'
;
; this class should be in the GDL_PATH, or in src/pro/dicom
;
res=EXECUTE("tmp=OBJ_NEW('idlsysmonitorinfo')")
if res EQ 0 then begin
   MESSAGE, /continue, 'skipping test on OBJ_NEW'
   MESSAGE, /continue, 'please check whether "idlsysmonitorinfo" class is in the path ...'
endif else  begin
   if (TYPENAME(tmp) NE "IDLSYSMONITORINFO") then $
        ERRORS_ADD, nb_errors, 'bad type with "idlsysmonitorinfo" Obj'
endelse
;
; structures : 4 cases to be tested : array or not, ANONYMOUS or NAMED
;
txt='bad type with Structure : '

struct_anon={aa:1}
if (TYPENAME(struct_anon) NE "ANONYMOUS") then ERRORS_ADD, nb_errors, txt+'ANONYMOUS'
struct_name={gdltest,aaa:1}
if (TYPENAME(struct_name) NE "GDLTEST") then ERRORS_ADD, nb_errors, txt+'NAME'
;
struct_anon_arr=REPLICATE({zz1:1},10)
if (TYPENAME(struct_anon_arr) NE "STRUCT") then ERRORS_ADD, nb_errors, txt+'ANONYMOUS (array)'
struct_name_arr=REPLICATE({gdl_arr,zz2:2},10)
if (TYPENAME(struct_name_arr) NE "STRUCT") then ERRORS_ADD, nb_errors, txt+'NAME (array)'
if (TYPENAME(struct_name_arr[*]) NE "STRUCT") then ERRORS_ADD, nb_errors, txt+'NAME (array[*])'
;
expect='GDL_ARR' ; name of the fiels in the array struct.
for ii=0, n_elements(STRUCT_NAME_ARR)-1 do $
   if (TYPENAME(struct_name_arr[ii]) NE expect) then ERRORS_ADD, nb_errors, txt+'sub NAME at '+string(ii)
;
; ---- Final message ----
;
BANNER_FOR_TESTSUITE, 'TEST_TYPENAME', nb_errors
;
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end

