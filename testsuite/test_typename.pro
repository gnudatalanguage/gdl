;
; Alain C., 2014/03/27
;
; very basic tests for TYPENAME() function
; 3 types (Obj, Struct, Pointer) are not ready at all)
;
pro MYMESS, errors, message
errors=errors+1
MESSAGE, /continue, message
end
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
if (TYPENAME(a_undef) NE "UNDEFINED") then MYMESS, nb_errors, 'bad type with UNDEFINED'
;
a=!null
if (TYPENAME(a) NE "UNDEFINED") then MYMESS, nb_errors, 'bad type with !NULL'
;
if (TYPENAME(1b) NE "BYTE") then MYMESS, nb_errors, 'bad type with BYTE'
if (TYPENAME(1) NE "INT") then MYMESS, nb_errors, 'bad type with INT'
if (TYPENAME(LONG(1)) NE "LONG") then MYMESS, nb_errors, 'bad type with LONG'
if (TYPENAME(1.) NE "FLOAT") then MYMESS, nb_errors, 'bad type with FLOAT'
if (TYPENAME(1.D) NE "DOUBLE") then MYMESS, nb_errors, 'bad type with DOUBLE'
if (TYPENAME(complex(1.,0)) NE "COMPLEX") then MYMESS, nb_errors, 'bad type with COMPLEX'
if (TYPENAME(dcomplex(1.,0)) NE "DCOMPLEX") then MYMESS, nb_errors, 'bad type with DCOMPLEX'
if (TYPENAME(1l) NE "LONG") then MYMESS, nb_errors, 'bad type with LONG'
if (TYPENAME(1ll) NE "LONG64") then MYMESS, nb_errors, 'bad type with LONG64'
if (TYPENAME(1ul) NE "ULONG") then MYMESS, nb_errors, 'bad type with ULONG'
if (TYPENAME(1ull) NE "ULONG64") then MYMESS, nb_errors, 'bad type with ULONG64'
;
if (nb_errors EQ 0) then begin
    MESSAGE, /continue, 'No error found in TEST_TYPENAME'
endif else begin
    MESSAGE, /continue, STRING(nb_errors)+' errors found in TEST_TYPENAME'
    if ~KEYWORD_SET(no_exit) then EXIT, status=1
endelse
;
if KEYWORD_SET(test) then STOP
;
end
