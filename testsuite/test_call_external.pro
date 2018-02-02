;
; under GNU GPL v3 or later
;
; Initial version "test_ce.pro" by Slayoo, 2010-OCT-27
;
; test suite for IDL/GDL call_external
; should be used together with "libtest_ce.so" compiled from "libtest_ce.cpp"
;
pro LOOKING_FOR_CE_LIB, image, path=path, name=name, test=test, verbose=verbose
;
image = './libtest_ce.' + (STRlowCase(!VERSION.OS_NAME) eq 'darwin' ? "dylib" : "so")
if ~FILE_TEST(image) then image = '.libs/' + image
if ~FILE_TEST(image) then begin
   MESSAGE, 'library found not compiled!', /conti
   EXIT, status=1
endif
MESSAGE, /info, 'using library file: ' + image
;
end
;
; --------------------------------------------------------------------------
; Byte 
pro TEST_CE_BYTE, image, cumul_errors, test=test, verbose=verbose
;
errors=0
entry = 'testce_byte'
;
p1 = 1B
p2 = [2B,3B]
p3 = 4B
;
if KEYWORD_SET(verbose) then print, 'Starting test Byte'
ret1 = call_external(image, entry, p1, p2, p3, value=[0,0,1], return_type=1)
;
if (    size(ret1, /type) ne 1  $
        or  ret1 ne 14B         $
        or  p1  ne 11B          $
        or  p2[0] ne 12B        $
        or  p2[1] ne 13B        $
        or  p3    ne 4B         $
   ) then begin
   print, 'back in IDL: Error:'
   print, 'Return value:  Got: ', ret1
   print, '          Expected: ', 14B
   print, 'First arg:     Got: ', p1
   print, '          Expected: ', 11B
   print, 'Second arg     Got: ', p2
   print, '          Expected: ', [12B, 13B]
   print, 'Third arg      Got: ', p3
   print, '          Expected: ', 4B
   errors++
endif
;
BANNER_FOR_TESTSUITE, 'TEST_CE_BYTE', errors, /short
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_set(test) then STOP
;
end
;
; --------------------------------------------------------------------------
; Int  
pro TEST_CE_INT, image, cumul_errors, test=test, verbose=verbose
;
errors=0
entry = 'testce_int'
;
p1 = 1
p2 = [2,3]
p3 = 4
;
if KEYWORD_SET(verbose) then print, 'Starting test Int'
ret2 = call_external(image, entry, p1, p2, p3, value=[0,0,1], return_type=2)
;
if (    size(ret2, /type) ne 2              $
        or  ret2 ne 14                          $
        or  p1  ne -11                          $
        or  p2[0] ne 12                        $
        or  p2[1] ne 13                        $
        or  p3    ne 4                         $
   ) then begin
   print, 'back in IDL: Error:'
   print, 'Return value:  Got: ', ret2
   print, '          Expected: ', 14
   print, 'First arg:     Got: ', p1
   print, '          Expected: ', -11
   print, 'Second arg     Got: ', p2
   print, '          Expected: ', [12, 13]
   print, 'Third arg      Got: ', p3
   print, '          Expected: ', 4
   errors++
endif
;
BANNER_FOR_TESTSUITE, 'TEST_CE_BYTE', errors, /short
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_set(test) then STOP
;
end
;
; --------------------------------------------------------------------------
; Long 
pro TEST_CE_LONG, image, cumul_errors, test=test, verbose=verbose
;
errors=0
entry = 'testce_long'
;
p1 = 1L
p2 = [2L,3L]
p3 = 4L
;
if KEYWORD_SET(verbose) then print, 'Starting test Long'
;
ret3 = call_external(image, entry, p1, p2, p3, value=[0,0,1], return_type=3)
;
if (    size(ret3, /type) ne 3              $
        or  ret3 ne 14L                         $
        or  p1  ne -11L                         $
        or  p2[0] ne 12L                       $
        or  p2[1] ne 13L                       $
        or  p3    ne 4L                        $
   ) then begin
   print, 'back in IDL: Error:'
   print, 'Return value:  Got: ', ret3
   print, '          Expected: ', 14L
   print, 'First arg:     Got: ', p1
   print, '          Expected: ', -11L
   print, 'Second arg     Got: ', p2
   print, '          Expected: ', [12L, 13L]
   print, 'Third arg      Got: ', p3
   print, '          Expected: ', 4L
   errors++
endif
;
BANNER_FOR_TESTSUITE, 'TEST_CE_LONG', errors, /short
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_set(test) then STOP
;
end
;
; --------------------------------------------------------------------------
; Float
pro TEST_CE_FLOAT, image, cumul_errors, test=test, verbose=verbose
;
errors=0
entry = 'testce_float'
;
p1 = 1.11
p2 = [2.22,3.33]
p3 = 4.44
;
if KEYWORD_SET(verbose) then print, 'Starting test Float'
ret4 = call_external(image, entry, p1, p2, p3, value=[0,0,1], return_type=4)
;
EPSILON = 1e-4
;
if (    size(ret4, /type) ne 4              $
        or  abs(ret4 - 14.14) gt EPSILON        $
        or  abs(p1  - 11.11) gt EPSILON        $
        or  abs(p2[0] - 12.12) gt EPSILON      $
        or  abs(p2[1] - 13.13) gt EPSILON      $
        or  abs(p3    - 4.44) gt EPSILON        $
   ) then begin
   print, 'back in IDL: Errors:'
   print, 'Return value:  Got: ', ret4
   print, '          Expected: ', 14.14
   print, 'First arg:     Got: ', p1
   print, '          Expected: ', 11.11
   print, 'Second arg     Got: ', p2
   print, '          Expected: ', [12.12, 13.13]
   print, 'Third arg      Got: ', p3
   print, '          Expected: ', 4.44
   errors++
endif
;
BANNER_FOR_TESTSUITE, 'TEST_CE_FLOAT', errors, /short
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_set(test) then STOP
;
end
; --------------------------------------------------------------------------
; Double (no passing by value here, see later 32/64 bit)
pro TEST_CE_DOUBLE, image, cumul_errors, test=test, verbose=verbose
;
errors=0
entry = 'testce_double'
;
p1 = 1.11D
p2 = [2.22D,3.33D]
;
if KEYWORD_SET(verbose) then print, 'Starting test double'
ret5 = call_external(image, entry, p1, p2, return_type=5)

EPSILON = 1d-8

if (    size(ret5, /type) ne 5              $
        or  abs(ret5 - 14.14D) gt EPSILON       $
        or  abs(p1  - 11.11D) gt EPSILON        $
        or  abs(p2[0] - 12.12D) gt EPSILON      $
        or  abs(p2[1] - 13.13D) gt EPSILON      $
   ) then begin
   print, 'back in IDL: Errors:'
   print, 'Return value:  Got: ', ret5
   print, '          Expected: ', 14.14D
   print, 'First arg:     Got: ', p1
   print, '          Expected: ', 11.11D
   print, 'Second arg     Got: ', p2
   print, '          Expected: ', [12.12D, 13.13D]
   errors++
endif
;
BANNER_FOR_TESTSUITE, 'TEST_CE_DOUBLE', errors, /short
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_set(test) then STOP
;
end
;
; --------------------------------------------------------------------------
; UInt  
pro TEST_CE_UINT, image, cumul_errors, test=test, verbose=verbose
;
errors=0
entry = 'testce_uint'
;
p1 = 1U
p2 = [2U,3U]
p3 = 4U
;
if KEYWORD_SET(verbose) then print, 'Starting test Uint'
ret12 = call_external(image, entry, p1, p2, p3, value=[0,0,1], return_type=12)
;
if (    size(ret12, /type) ne 12              $
        or  ret12 ne 14U                          $
        or  p1  ne 65535U                          $
        or  p2[0] ne 12U                        $
        or  p2[1] ne 13U                        $
        or  p3    ne 4U                         $
   ) then begin
   print, 'back in IDL: Errors:'
   print, 'Return value:  Got: ', ret12
   print, '          Expected: ', 14U
   print, 'First arg:     Got: ', p1
   print, '          Expected: ', 65535U
   print, 'Second arg     Got: ', p2
   print, '          Expected: ', [12U, 13U]
   print, 'Third arg      Got: ', p3
   print, '          Expected: ', 4U
   errors++
endif
;
BANNER_FOR_TESTSUITE, 'TEST_CE_UINT', errors, /short
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_set(test) then STOP
;
end
;
; --------------------------------------------------------------------------
; ULong 
pro TEST_CE_ULONG, image, cumul_errors, test=test, verbose=verbose
;
errors=0
entry = 'testce_ulong'
;
p1 = 1UL
p2 = [2UL,3UL]
p3 = 4UL

if KEYWORD_SET(verbose) then print, 'Starting test Ulong'
ret13 = call_external(image, entry, p1, p2, p3, value=[0,0,1], return_type=13)

if (    size(ret13, /type) ne 13              $
        or  ret13 ne 14UL                         $
        or  p1  ne 4294967295UL                         $
        or  p2[0] ne 12UL                       $
        or  p2[1] ne 13UL                       $
        or  p3    ne 4UL                        $
   ) then begin
   print, 'back in IDL: Errors:'
   print, 'Return value:  Got: ', ret13
   print, '          Expected: ', 14UL
   print, 'First arg:     Got: ', p1
   print, '          Expected: ', 4294967295UL
   print, 'Second arg     Got: ', p2
   print, '          Expected: ', [12UL, 13UL]
   print, 'Third arg      Got: ', p3
   print, '          Expected: ', 4UL
   errors++
endif
;
BANNER_FOR_TESTSUITE, 'TEST_CE_ULONG', errors, /short
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_set(test) then STOP
;
end
;
; --------------------------------------------------------------------------
; Long64 
pro TEST_CE_LONG64, image, cumul_errors, test=test, verbose=verbose
;
errors=0
entry = 'testce_long64'
;
p1 = 1LL
p2 = [2LL,3LL]
;
if KEYWORD_SET(verbose) then print, 'Starting test Long64'
ret14 = call_external(image, entry, p1, p2, return_type=14)
;
if (    size(ret14, /type) ne 14            $
        or  ret14 ne 14LL                       $
        or  p1  ne -11LL                         $
        or  p2[0] ne 12LL                       $
        or  p2[1] ne 13LL                       $
   ) then begin
   print, 'back in IDL: Errors:'
   print, 'Return value:  Got: ', ret14
   print, '          Expected: ', 14LL
   print, 'First arg:     Got: ', p1
   print, '          Expected: ', -11LL
   print, 'Second arg     Got: ', p2
   print, '          Expected: ', [12LL, 13LL]
   errors++
endif
;
BANNER_FOR_TESTSUITE, 'TEST_CE_LONG64', errors, /short
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_set(test) then STOP
;
end
;
; --------------------------------------------------------------------------
; ULong64 
pro TEST_CE_ULONG64, image, cumul_errors, test=test, verbose=verbose
;
errors=0
entry = 'testce_ulong64'
;
p1 = 1ULL
p2 = [2ULL,3ULL]
;
if KEYWORD_SET(verbose) then print, 'Starting test Long64'
ret15 = call_external(image, entry, p1, p2, return_type=15)
;
if (    size(ret15, /type) ne 15            $
        or  ret15 ne 14ULL                  $
        or  p1  ne 18446744073709551615ULL  $
        or  p2[0] ne 12ULL                  $
        or  p2[1] ne 13ULL                  $
   ) then begin
   print, 'back in IDL: Errors:'
   print, 'Return value:  Got: ', ret15
   print, '          Expected: ', 14ULL
   print, 'First arg:     Got: ', p1
   print, '          Expected: ', 18446744073709551615ULL
   print, 'Second arg     Got: ', p2
   print, '          Expected: ', [12ULL, 13ULL]
   errors++
endif
;
BANNER_FOR_TESTSUITE, 'TEST_CE_ULONG64', errors, /short
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_set(test) then STOP
;
end
;
; --------------------------------------------------------------------------
; String 
pro TEST_CE_STRING, image, cumul_errors, test=test, verbose=verbose
;
errors=0
entry = 'testce_string'
;
p1 = "One  "
p2 = ["Two  ", "Three"]
p3 = "Four "
c1 = "One  "
;
if KEYWORD_SET(verbose) then print, 'Starting test String'
ret7 = call_external(image, entry, p1, p2, p3, value=[0, 0, 1],  /s_value)
;
if (    size(ret7, /type) ne 7            $
        or  ret7 ne "Fourteen"                $
        or  p1  ne "  enO"                    $
        or  p2[0] ne "  owT"                  $
        or  p2[1] ne "eerhT"                  $
   ) then begin
   print, 'back in IDL: Errors:'
   print, 'Return value:  Got: ', ret7
   print, '          Expected: ', "Fourteen"
   print, 'First arg:     Got: ', p1
   print, '          Expected: ', "  enO"
   print, 'Second arg     Got: ', p2
   print, '          Expected: ', ["  owT", "eerhT"]
   errors++
endif else begin
   print, 'back in IDL: OK' ;; keep this, special case
   if (c1 ne "One"+"  ") then begin
      print, "(But constant string changed as well: '", c1, "' instead of 'One  ')"
   endif
endelse
;
BANNER_FOR_TESTSUITE, 'TEST_CE_STRING', errors, /short
ERRORS_CUMUL, cumul_errors, errors
;
end
;
; --------------------------------------------------------------------------
; Struct
pro TEST_CE_STRUCT, image, cumul_errors, test=test, verbose=verbose
;
errors=0
entry = 'testce_struct'
;
p1 = [ {outer, l1:1L, si:{inner, c:1B, d:2LL, s:["One", "Two"]}, l2:2L, c:2B}, $
       {outer, l1:3L, si:{inner, c:3B, d:4LL, s:["Thr", "Fou"]}, l2:4L, c:4B} ]
;
if KEYWORD_SET(verbose) then print, 'Starting test Stuct'
ret = call_external(image, entry, p1)

if (    p1[0].l1      ne 11                        $
        or  p1[1].si.s[1] ne "Fiv"		$
   ) then begin
   print, 'back in IDL: Errors:'
   errors++
endif
;
BANNER_FOR_TESTSUITE, 'TEST_CE_STRUCT', errors, /short
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_set(test) then STOP
;
end
;
; --------------------------------------------------------------------------
; Complex
pro TEST_CE_COMPLEX, image, cumul_errors, test=test, verbose=verbose
;
errors=0
entry = 'testce_complex'
;
p1 = complex(1.1, 2.2)
p2 = [complex(3.3,4.4), complex(5.5,6.6)]
;
if KEYWORD_SET(verbose) then print, 'Starting test Complex'
ret = call_external(image, entry, p1, p2, /f_value)
;
EPSILON = 1e-4
;
if (    size(ret, /type) ne 4              $
        or  ret ne 14.14			$
        or  abs( real_part(p1) - 1.1) gt EPSILON $
        or  abs( imaginary(p1) - 22.22) gt EPSILON $
        or  abs( real_part(p2[0]) - 3.3) gt EPSILON $
        or  abs( imaginary(p2[0]) - 4.4) gt EPSILON $
        or  abs( real_part(p2[1]) - 55.55) gt EPSILON $
        or  abs( imaginary(p2[1]) - 6.6) gt EPSILON $

   ) then begin
   print, 'back in IDL: Errors:'
   print, 'Return value:  Got: ', ret
   print, '          Expected: ', 14.14
   print, 'First arg:     Got: ', p1
   print, '          Expected: ', complex(1.1, 22.22)
   print, 'Second arg     Got: ', p2
   print, '          Expected: ', [complex(3.3,4.4), complex(55.55, 6.6)]
   errors++
endif
;
BANNER_FOR_TESTSUITE, 'TEST_CE_COMPLEX', errors, /short
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_set(test) then STOP
;
end
;
; --------------------------------------------------------------------------
; DComplex
pro TEST_CE_DCOMPLEX, image, cumul_errors, test=test, verbose=verbose
;
errors=0
entry = 'testce_dcomplex'

p1 = dcomplex(1.1d, 2.2d)
p2 = [dcomplex(3.3d,4.4d), dcomplex(5.5d,6.6d)]

if KEYWORD_SET(verbose) then print, 'Starting test DComplex'
ret = call_external(image, entry, p1, p2, /d_value)

EPSILON = 1d-8

if (    size(ret, /type) ne 5              $
        or  ret ne 14.14D			$
        or  abs( real_part(p1) - 1.1D) gt EPSILON $
        or  abs( imaginary(p1) - 22.22D) gt EPSILON $
        or  abs( real_part(p2[0]) - 3.3D) gt EPSILON $
        or  abs( imaginary(p2[0]) - 4.4D) gt EPSILON $
        or  abs( real_part(p2[1]) - 55.55D) gt EPSILON $
        or  abs( imaginary(p2[1]) - 6.6D) gt EPSILON $

   ) then begin
   print, 'back in IDL: Errors:'
   print, 'Return value:  Got: ', ret
   print, '          Expected: ', 14.14D
   print, 'First arg:     Got: ', p1
   print, '          Expected: ', dcomplex(1.1D, 22.22D)
   print, 'Second arg     Got: ', p2
   print, '          Expected: ', [dcomplex(3.3D,4.4D), dcomplex(55.55D, 6.6D)]
   errors++
endif
;
BANNER_FOR_TESTSUITE, 'TEST_CE_DCOMPLEX', errors, /short
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_set(test) then STOP
;
end
;
; --------------------------------------------------------------------------
; Long args by value
pro TEST_CE_BY_VALUE, image, cumul_errors, test=test, verbose=verbose
;
if (!version.memory_bits gt 32) then begin
   entry = 'testce_64'
   errors=0
   p2 = complex(1.1, 2.2)
   ;;
   if KEYWORD_SET(verbose) then print, 'Starting test 64bit'
   ret = call_external(image, entry, 3.3D, p2, /d_value, value=[1,1])
   ;;
   EPSILON = 1d-8
   ;;
   if ((SIZE(ret, /type) ne 5) or (ret ne 15.15D)) then begin
      print, 'back in IDL: Errors:'
      print, 'Return value:  Got: ', ret
      print, '          Expected: ', 15.15D
      errors++
   endif
endif else begin
   print, 'Skipping test on 32 bit system'
endelse
;
BANNER_FOR_TESTSUITE, 'TEST_CE_BY_VALUE', errors, /short
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_set(test) then STOP
;
end
;
; --------------------------------------------------------------------------
; Some tests that should fail
pro TEST_CE_MUST_FAILED, image, cumul_errors, test=test, verbose=verbose
;
errors=0
;
print, '=================================================================='
print, 'The following tests SHOULD FAIL. The result will be caught,'
print, 'but errors messages may be written to the console'
print
;
; --------------------------------------------------------------------------
; Non-existing image or entry
;
print, 'Non-existing image:'
image_nonsense = 'reallyNonsense'
r = execute( 'ret = call_external(image_nonsense, entry)' )
if (r eq 0) then begin
   print, 'Test failed as expected. OK!'
   print
endif else begin
   print, 'ERRORS: Test did not fail. (but it should !)'
   errors++
endelse
;
print, 'Non-existing entry:'
entry_nonsense = 'reallyNonsense'
r = execute( 'ret = call_external(image, entry_nonsense)' )
if (r eq 0) then begin
   print, 'Test failed as expected. OK!'
   print
endif else begin
   print, 'ERRORS: Test did not fail (but it should !)'
   errors++
endelse

; --------------------------------------------------------------------------
; Illegal object for passing by value

entry = 'doNothing'
array_by_ref = [1,2,3]
array_by_val = [4,5,6]

print, 'Trying to pass non-scalar by value:'
r = execute( 'ret = call_external(image, entry, array_by_ref, array_by_val, value=[0,1])' )
if (r eq 0) then begin
   print, 'Test failed as expected. OK!'
   print
endif else begin
   print, 'ERRORS: Test did not fail.'
   errors = errors + 1
;stop
endelse


; --------------------------------------------------------------------------
; Too large object for passing by value

if (!version.memory_bits le 64) then begin
   entry = 'doNothing'
   dc = dcomplex(1D, 1D)

   print, 'Trying to pass  too large scalar by value:'
   r = execute( 'ret = call_external(image, entry, dc, /all_value )' )
   if (r eq 0) then begin
      print, 'Test failed as expected. OK!'
      print
   endif else begin
      print, 'ERRORS: Test did not fail.'
      errors = errors + 1
;stop
   endelse
endif else begin
   print, 'Test skipped , !version.memory_bits > 64'
endelse
;
BANNER_FOR_TESTSUITE, 'TEST_CE_BY_VALUE', errors, /short
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_set(test) then STOP
;
end
;
; --------------------------------------------------------------------------
;
pro TEST_CALL_EXTERNAL, help=help, test=test, no_exit=no_exit, verbose=verbose
;
cumul_errors=0
;
LOOKING_FOR_CE_LIB, image, path=path, name=name, test=test
;
TEST_CE_BYTE, image, cumul_errors, test=test, verbose=verbose
TEST_CE_INT, image, cumul_errors, test=test, verbose=verbose
TEST_CE_LONG, image, cumul_errors, test=test, verbose=verbose
TEST_CE_FLOAT, image, cumul_errors, test=test, verbose=verbose
TEST_CE_DOUBLE, image, cumul_errors, test=test, verbose=verbose
TEST_CE_UINT, image, cumul_errors, test=test, verbose=verbose
TEST_CE_ULONG, image, cumul_errors, test=test, verbose=verbose
TEST_CE_LONG64, image, cumul_errors, test=test, verbose=verbose
TEST_CE_ULONG64, image, cumul_errors, test=test, verbose=verbose
TEST_CE_STRING, image, cumul_errors, test=test, verbose=verbose
TEST_CE_STRUCT, image, cumul_errors, test=test, verbose=verbose
TEST_CE_COMPLEX, image, cumul_errors, test=test, verbose=verbose
TEST_CE_DCOMPLEX, image, cumul_errors, test=test, verbose=verbose
TEST_CE_BY_VALUE, image, cumul_errors, test=test, verbose=verbose
TEST_CE_MUST_FAILED, image, cumul_errors, test=test, verbose=verbose
;
BANNER_FOR_TESTSUITE, 'TEST_CALL_EXTERNAL', cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end

