;
; Testing MAKE_ARRAY : keyword, type, value type promotion ...
;
; Initial version by Rexso, 11 June 2018, under GNU GPL v3 or later
;
; This bug https://github.com/gnudatalanguage/gdl/issues/310
; was reported by Sylvain.
;
; https://github.com/gnudatalanguage/gdl/pull/316
;
; ----------------------------------------------------
; Modifications history :
;
; 2018-Jun-12 : Rexso : initial version
; 2018-Jun-13 : AC : 
;   - reformating 
;   - extension (full cross matrix ...)
;   - min2max & ADD_ERRROR/ERRORS_ADD
;
; ----------------------------------------------------
;
pro TEST_MAKE_ARRAY_KEYWORD, cumul_errors, test=test, verbose=verbose

nerr=0
;; Explicit type keywords

if TYPENAME(MAKE_ARRAY(1, /BYTE)) ne "BYTE" then ERRORS_ADD, nerr, 'MAKE_ARRAY with /BYTE keyword does not yield a BYTE array'
if TYPENAME(MAKE_ARRAY(1, /COMPLEX)) ne "COMPLEX" then ERRORS_ADD, nerr, 'MAKE_ARRAY with /COMPLEX keyword does not yield a COMPLEX array'
if TYPENAME(MAKE_ARRAY(1, /DCOMPLEX)) ne "DCOMPLEX" then ERRORS_ADD, nerr, 'MAKE_ARRAY with /DCOMPLEX keyword does not yield a DCOMPLEX array'
if TYPENAME(MAKE_ARRAY(1, /DOUBLE)) ne "DOUBLE" then ERRORS_ADD, nerr, 'MAKE_ARRAY with /DOUBLE keyword does not yield a DOUBLE array'
if TYPENAME(MAKE_ARRAY(1, /FLOAT)) ne "FLOAT" then ERRORS_ADD, nerr, 'MAKE_ARRAY with /FLOAT keyword does not yield a FLOAT array'
if TYPENAME(MAKE_ARRAY(1, /INTEGER)) ne "INT" then ERRORS_ADD, nerr, 'MAKE_ARRAY with /INTEGER keyword does not yield an INT array'
if TYPENAME(MAKE_ARRAY(1, /L64)) ne "LONG64" then ERRORS_ADD, nerr, 'MAKE_ARRAY with /L64 keyword does not yield a LONG64 array'
if TYPENAME(MAKE_ARRAY(1, /LONG)) ne "LONG" then ERRORS_ADD, nerr, 'MAKE_ARRAY with /LONG keyword does not yield a LONG array'
if TYPENAME(MAKE_ARRAY(1, /OBJ)) ne "OBJREF" then ERRORS_ADD, nerr, 'MAKE_ARRAY with /OBJ keyword does not yield an OBJREF array'
if TYPENAME(MAKE_ARRAY(1, /PTR)) ne "POINTER" then ERRORS_ADD, nerr, 'MAKE_ARRAY with /PTR keyword does not yield a POINTER array'
if TYPENAME(MAKE_ARRAY(1, /STRING)) ne "STRING" then ERRORS_ADD, nerr, 'MAKE_ARRAY with /STRING keyword does not yield a STRING array'
if TYPENAME(MAKE_ARRAY(1, /UINT)) ne "UINT" then ERRORS_ADD, nerr, 'MAKE_ARRAY with /UINT keyword does not yield an UINT array'
if TYPENAME(MAKE_ARRAY(1, /UL64)) ne "ULONG64" then ERRORS_ADD, nerr, 'MAKE_ARRAY with /UL64 keyword does not yield an ULONG64 array'
if TYPENAME(MAKE_ARRAY(1, /ULONG)) ne "ULONG" then ERRORS_ADD, nerr, 'MAKE_ARRAY with /ULONG keyword does not yield an ULONG array'
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_MAKE_ARRAY_KEYWORD', nerr, /status
ERRORS_CUMUL, cumul_errors, nerr
if KEYWORD_set(test) then STOP
;
end
;
; ----------------------------------------------------
;
pro TEST_MAKE_ARRAY_TYPE, cumul_errors, test=test, verbose=verbose
;
nerr=0
;
; Numbered TYPE keyword
if TYPENAME(MAKE_ARRAY(1, TYPE=1)) ne "BYTE" then ERRORS_ADD, nerr, 'MAKE_ARRAY with TYPE=1 does not yield a BYTE array'
if TYPENAME(MAKE_ARRAY(1, TYPE=2)) ne "INT" then ERRORS_ADD, nerr, 'MAKE_ARRAY with TYPE=2 does not yield an INT array'
if TYPENAME(MAKE_ARRAY(1, TYPE=3)) ne "LONG" then ERRORS_ADD, nerr, 'MAKE_ARRAY with TYPE=3 does not yield a LONG array'
if TYPENAME(MAKE_ARRAY(1, TYPE=4)) ne "FLOAT" then ERRORS_ADD, nerr, 'MAKE_ARRAY with TYPE=4 does not yield a FLOAT array'
if TYPENAME(MAKE_ARRAY(1, TYPE=5)) ne "DOUBLE" then ERRORS_ADD, nerr, 'MAKE_ARRAY with TYPE=5 does not yield a DOUBLE array'
if TYPENAME(MAKE_ARRAY(1, TYPE=6)) ne "COMPLEX" then ERRORS_ADD, nerr, 'MAKE_ARRAY with TYPE=6 does not yield a COMPLEX array'
if TYPENAME(MAKE_ARRAY(1, TYPE=7)) ne "STRING" then ERRORS_ADD, nerr, 'MAKE_ARRAY with TYPE=7 does not yield a STRING array'
;; TYPE=8 (STRUCT) not supported my MAKE_ARRAY, use REPLICATE instead
if TYPENAME(MAKE_ARRAY(1, TYPE=9)) ne "DCOMPLEX" then ERRORS_ADD, nerr, 'MAKE_ARRAY with TYPE=9 does not yield a DCOMPLEX array'
if TYPENAME(MAKE_ARRAY(1, TYPE=10)) ne "POINTER" then ERRORS_ADD, nerr, 'MAKE_ARRAY with TYPE=10 does not yield a POINTER array'
if TYPENAME(MAKE_ARRAY(1, TYPE=11)) ne "OBJREF" then ERRORS_ADD, nerr, 'MAKE_ARRAY with TYPE=11 does not yield an OBJREF array'
if TYPENAME(MAKE_ARRAY(1, TYPE=12)) ne "UINT" then ERRORS_ADD, nerr, 'MAKE_ARRAY with TYPE=12 does not yield an UINT array'
if TYPENAME(MAKE_ARRAY(1, TYPE=13)) ne "ULONG" then ERRORS_ADD, nerr, 'MAKE_ARRAY with TYPE=13 does not yield an ULONG array'
if TYPENAME(MAKE_ARRAY(1, TYPE=14)) ne "LONG64" then ERRORS_ADD, nerr, 'MAKE_ARRAY with TYPE=14 does not yield a LONG64 array'
if TYPENAME(MAKE_ARRAY(1, TYPE=15)) ne "ULONG64" then ERRORS_ADD, nerr, 'MAKE_ARRAY with TYPE=15 does not yield an ULONG64 array'
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_MAKE_ARRAY_TYPE', nerr, /status
ERRORS_CUMUL, cumul_errors, nerr
if KEYWORD_set(test) then STOP
;
end
;
; ----------------------------------------------------
;
pro TEST_MAKE_ARRAY_MISC, cumul_errors, test=test, verbose=verbose
;
nerr=0
;
;; TODO: BOOLEAN keyword (not yet implemented)
;
; DIMENSION keyword
if ~ARRAY_EQUAL(SIZE(MAKE_ARRAY(dimension=[ 2, 3, 4 ], /byte), /dimensions), [ 2, 3, 4 ]) then $
   ERRORS_ADD, nerr, 'MAKE_ARRAY with DIMENSION yields wrong result'
if (SIZE(MAKE_ARRAY(dimension=[ 2, 3, 4 ], /byte), /n_dimensions) NE 3) then $
   ERRORS_ADD, nerr, 'MAKE_ARRAY with DIMENSION yields wrong result'
;
; INDEX keyword
if (TOTAL(MAKE_ARRAY(2, 2, 2, /index, /int)) ne 28) then ERRORS_ADD, nerr, 'TOTAL value of MAKE_ARRAY with /INDEX yields wrong result'
if ((MAKE_ARRAY(2, 2, 2, /index, /float))[2] ne 2.0) then ERRORS_ADD, nerr, 'Element values in MAKE_ARRAY with /INDEX yields wrong result'

; START keyword
if TOTAL(MAKE_ARRAY(5, start=2, /int)) ne 0 then $
   ERRORS_ADD, nerr, 'MAKE_ARRAY with START yields wrong result (START expected to be ignored unless /INDEX is set)'
if (MAKE_ARRAY(5, start=1.9, /index, /float))[0] ne 1.9 then $
   ERRORS_ADD, nerr, 'MAKE_ARRAY with START yields wrong result (START expected to represent first value)'
if (MAKE_ARRAY(5, start=1.9, /index, /int))[0] ne 1 then $
   ERRORS_ADD, nerr, 'MAKE_ARRAY with START yields wrong result'
;
; INCREMENT keyword
if TOTAL(MAKE_ARRAY(5, start=1.9, increment=0.3, /index, /int)) ne 10 then $
   ERRORS_ADD, nerr, 'MAKE_ARRAY with INCREMENT yields wrong result /int)'
if TOTAL(MAKE_ARRAY(5, start=1.9, increment=0.3, /index, /float)) ne 12.5 then $
   ERRORS_ADD, nerr, 'MAKE_ARRAY with INCREMENT yields wrong result (/float)'
if ~ARRAY_EQUAL(MAKE_ARRAY(5, start=1.9, /index, /float), [1.9, 2.9, 3.9, 4.9, 5.9]) then $
   ERRORS_ADD, nerr, 'MAKE_ARRAY with INCREMENT yields wrong result (values)'
;
; NOZERO keyword is difficult to test
;
; VALUE keyword
if ~ARRAY_EQUAL(make_array(2, /string, value="ok"), [ "ok", "ok" ]) then $
   ERRORS_ADD, nerr, 'MAKE_ARRAY with VALUE yields wrong result for STRING type'
if ~ARRAY_EQUAL(make_array(3, /int, value=4), [ 4, 4, 4 ]) then $
   ERRORS_ADD, nerr, 'MAKE_ARRAY with VALUE yields wrong result for INT type'
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_MAKE_ARRAY_MISC', nerr, /status
ERRORS_CUMUL, cumul_errors, nerr
if KEYWORD_set(test) then STOP
;
end
;
; ----------------------------------------------------
;
; the type of "value" is overpassed by the type provided as Keyword
;
pro TEST_MAKE_ARRAY_VALUE, cumul_errors, test=test, verbose=verbose
;
GIVE_LIST_NUMERIC,  list_num_types, list_num_names, lns, list_num_keywords 
;
nb_errors=0
;
for ii=0, N_ELEMENTS(list_num_types)-1 do begin
   for jj=0, N_ELEMENTS(list_num_types)-1 do begin
      ;;
      value=FIX(1, type=list_num_types[jj])
      expected=FIX(1, type=list_num_types[ii])
      ;;
      cmd='res=MAKE_ARRAY(10, /'+list_num_keywords[ii]+', value=value)'
      status=EXECUTE(cmd)
      if (status EQ 1) then begin
         exp_res=MAKE_ARRAY(10, value=expected)
         ;;help, res, exp_res
         if ~ARRAY_EQUAL(exp_res, res, /no_typeconv) then $
            ERRORS_ADD, nb_errors, 'bad value inside '+list_num_names[ii]
         ;;
         if (TYPENAME(res) NE TYPENAME(expected)) then $
            ERRORS_ADD, nb_errors, 'bad conv. type '+list_num_names[ii]
      endif else begin
         txt=STRCOMPRESS('('+string(ii)+','+string(jj)+')')
         ERRORS_ADD, nb_errors, 'bad status for case '+txt
      endelse
   endfor
endfor
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_MAKE_ARRAY_VALUE', nb_errors, /status
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; ----------------------------------------------
;
pro  TEST_MAKE_ARRAY, help=help, test=test, no_exit=no_exit, verbose=verbose
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_MAKE_ARRAY, help=help, test=test, $'
   print, '                     no_exit=no_exit, verbose=verbose'
   return
endif
;
; Rexso
TEST_MAKE_ARRAY_KEYWORD, cumul_errors, verbose=verbose
TEST_MAKE_ARRAY_TYPE, cumul_errors, verbose=verbose
TEST_MAKE_ARRAY_MISC, cumul_errors, verbose=verbose
;
; AC
TEST_MAKE_ARRAY_VALUE, cumul_errors, verbose=verbose
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_MAKE_ARRAY', cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end


