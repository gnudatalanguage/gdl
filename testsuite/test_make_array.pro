;
; Testing MAKE_ARRAY with keywords
;

pro test_make_array, test=test, no_exit=no_exit

  nerr=0

  ; Explicit type keywords
  if typename(make_array(1, /BYTE)) ne "BYTE" then ADD_ERROR, nerr, 'MAKE_ARRAY with /BYTE keyword does not yield a BYTE array'
  if typename(make_array(1, /COMPLEX)) ne "COMPLEX" then ADD_ERROR, nerr, 'MAKE_ARRAY with /COMPLEX keyword does not yield a COMPLEX array'
  if typename(make_array(1, /DCOMPLEX)) ne "DCOMPLEX" then ADD_ERROR, nerr, 'MAKE_ARRAY with /DCOMPLEX keyword does not yield a DCOMPLEX array'
  if typename(make_array(1, /DOUBLE)) ne "DOUBLE" then ADD_ERROR, nerr, 'MAKE_ARRAY with /DOUBLE keyword does not yield a DOUBLE array'
  if typename(make_array(1, /FLOAT)) ne "FLOAT" then ADD_ERROR, nerr, 'MAKE_ARRAY with /FLOAT keyword does not yield a FLOAT array'
  if typename(make_array(1, /INTEGER)) ne "INT" then ADD_ERROR, nerr, 'MAKE_ARRAY with /INTEGER keyword does not yield an INT array'
  if typename(make_array(1, /L64)) ne "LONG64" then ADD_ERROR, nerr, 'MAKE_ARRAY with /L64 keyword does not yield a LONG64 array'
  if typename(make_array(1, /LONG)) ne "LONG" then ADD_ERROR, nerr, 'MAKE_ARRAY with /LONG keyword does not yield a LONG array'
  if typename(make_array(1, /OBJ)) ne "OBJREF" then ADD_ERROR, nerr, 'MAKE_ARRAY with /OBJ keyword does not yield an OBJREF array'
  if typename(make_array(1, /PTR)) ne "POINTER" then ADD_ERROR, nerr, 'MAKE_ARRAY with /PTR keyword does not yield a POINTER array'
  if typename(make_array(1, /STRING)) ne "STRING" then ADD_ERROR, nerr, 'MAKE_ARRAY with /STRING keyword does not yield a STRING array'
  if typename(make_array(1, /UINT)) ne "UINT" then ADD_ERROR, nerr, 'MAKE_ARRAY with /UINT keyword does not yield an UINT array'
  if typename(make_array(1, /UL64)) ne "ULONG64" then ADD_ERROR, nerr, 'MAKE_ARRAY with /UL64 keyword does not yield an ULONG64 array'
  if typename(make_array(1, /ULONG)) ne "ULONG" then ADD_ERROR, nerr, 'MAKE_ARRAY with /ULONG keyword does not yield an ULONG array'

  ; Numbered TYPE keyword
  if typename(make_array(1, TYPE=1)) ne "BYTE" then ADD_ERROR, nerr, 'MAKE_ARRAY with TYPE=1 does not yield a BYTE array'
  if typename(make_array(1, TYPE=2)) ne "INT" then ADD_ERROR, nerr, 'MAKE_ARRAY with TYPE=2 does not yield an INT array'
  if typename(make_array(1, TYPE=3)) ne "LONG" then ADD_ERROR, nerr, 'MAKE_ARRAY with TYPE=3 does not yield a LONG array'
  if typename(make_array(1, TYPE=4)) ne "FLOAT" then ADD_ERROR, nerr, 'MAKE_ARRAY with TYPE=4 does not yield a FLOAT array'
  if typename(make_array(1, TYPE=5)) ne "DOUBLE" then ADD_ERROR, nerr, 'MAKE_ARRAY with TYPE=5 does not yield a DOUBLE array'
  if typename(make_array(1, TYPE=6)) ne "COMPLEX" then ADD_ERROR, nerr, 'MAKE_ARRAY with TYPE=6 does not yield a COMPLEX array'
  if typename(make_array(1, TYPE=7)) ne "STRING" then ADD_ERROR, nerr, 'MAKE_ARRAY with TYPE=7 does not yield a STRING array'
  ; TYPE=8 (STRUCT) not supported my MAKE_ARRAY, use REPLICATE instead
  if typename(make_array(1, TYPE=9)) ne "DCOMPLEX" then ADD_ERROR, nerr, 'MAKE_ARRAY with TYPE=9 does not yield a DCOMPLEX array'
  if typename(make_array(1, TYPE=10)) ne "POINTER" then ADD_ERROR, nerr, 'MAKE_ARRAY with TYPE=10 does not yield a POINTER array'
  if typename(make_array(1, TYPE=11)) ne "OBJREF" then ADD_ERROR, nerr, 'MAKE_ARRAY with TYPE=11 does not yield an OBJREF array'
  if typename(make_array(1, TYPE=12)) ne "UINT" then ADD_ERROR, nerr, 'MAKE_ARRAY with TYPE=12 does not yield an UINT array'
  if typename(make_array(1, TYPE=13)) ne "ULONG" then ADD_ERROR, nerr, 'MAKE_ARRAY with TYPE=13 does not yield an ULONG array'
  if typename(make_array(1, TYPE=14)) ne "LONG64" then ADD_ERROR, nerr, 'MAKE_ARRAY with TYPE=14 does not yield a LONG64 array'
  if typename(make_array(1, TYPE=15)) ne "ULONG64" then ADD_ERROR, nerr, 'MAKE_ARRAY with TYPE=15 does not yield an ULONG64 array'

  ; TODO: BOOLEAN keyword (not yet implemented)

  ; DIMENSION keyword
  if not array_equal(size(make_array(dimension=[ 2, 3, 4 ], /byte), /dimensions), [ 2, 3, 4 ]) then ADD_ERROR, nerr, 'MAKE_ARRAY with DIMENSION yields wrong result'
  if not size(make_array(dimension=[ 2, 3, 4 ], /byte), /n_dimensions) eq 3 then ADD_ERROR, nerr, 'MAKE_ARRAY with DIMENSION yields wrong result'

  ; INDEX keyword
  if total(make_array(2, 2, 2, /index, /int)) ne 28 then ADD_ERROR, nerr, 'TOTAL value of MAKE_ARRAY with /INDEX yields wrong result'
  if (make_array(2, 2, 2, /index, /float))[2] ne 2.0 then ADD_ERROR, nerr, 'Element values in MAKE_ARRAY with /INDEX yields wrong result'

  ; START keyword
  if total(make_array(5, start=2, /int)) ne 0 then ADD_ERROR, nerr, 'MAKE_ARRAY with START yields wrong result (START expected to be ignored unless /INDEX is set)'
  if (make_array(5, start=1.9, /index, /float))[0] ne 1.9 then ADD_ERROR, nerr, 'MAKE_ARRAY with START yields wrong result (START expected to represent first value)'
  if (make_array(5, start=1.9, /index, /int))[0] ne 1 then ADD_ERROR, nerr, 'MAKE_ARRAY with START yields wrong result'

  ; INCREMENT keyword
  if total(make_array(5, start=1.9, increment=0.3, /index, /int)) ne 10 then ADD_ERROR, nerr, 'MAKE_ARRAY with INCREMENT yields wrong result'
  if total(make_array(5, start=1.9, increment=0.3, /index, /float)) ne 12.5 then ADD_ERROR, nerr, 'MAKE_ARRAY with INCREMENT yields wrong result'
  if not array_equal(make_array(5, start=1.9, /index, /float), [1.9, 2.9, 3.9, 4.9, 5.9]) then ADD_ERROR, nerr, 'MAKE_ARRAY with INCREMENT yields wrong result'

  ; NOZERO keyword is difficult to test

  ; VALUE keyword
  if not array_equal(make_array(2, /string, value="ok"), [ "ok", "ok" ]) then ADD_ERROR, nerr, 'MAKE_ARRAY with VALUE yields wrong result for STRING type'
  if not array_equal(make_array(3, /int, value=4), [ 4, 4, 4 ]) then ADD_ERROR, nerr, 'MAKE_ARRAY with VALUE yields wrong result for INT type'

  BANNER_FOR_TESTSUITE, 'test_make_array', nerr

  if nerr gt 0 and ~keyword_set(no_exit) then exit, status=1
  if keyword_set(test) then stop
end

