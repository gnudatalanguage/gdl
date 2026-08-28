;
; Yanis Saglio, 30 june 2026
; under GNU GPL V3
; 
; Fix issue #2179, initial version July 2026 by YS
;
; -------------------------------------------------
; Modifications history :
;
; - 2026-Jul-19 : AC & YS : wider coverage test (1D, 2D, String)
;
; -------------------------------------------------
;
pro TEST_VALUE_LOCATE_1D, cumul_errors, test=test, verbose=verbose
;
errors=0
;
input=[1,4,7,9]
pos=INDGEN(12)
expected=[ -1L,0,0,0,1,1,1,2,2,3,3,3]
transexp=TRANSPOSE(expected)
;
txt_wd="Wrong dimension"
;
; No transpose
res=VALUE_LOCATE(input, pos)
if ~ARRAY_EQUAL(res, expected) then ERRORS_ADD, errors, "Wrong value(s)"
if ~ARRAY_EQUAL(SIZE(res),SIZE(expected)) then ERRORS_ADD, errors, txt_wd+" 1"
;
; Input transpose ("res" not tranposed)
res=VALUE_LOCATE(TRANSPOSE(input), pos)
if ~ARRAY_EQUAL(res, expected) then ERRORS_ADD, errors, "Wrong value(s)"
if ~ARRAY_EQUAL(SIZE(res),SIZE(expected)) then ERRORS_ADD, errors, txt_wd+" 2"
;
; Pos transposed then "res" tranposed
res=VALUE_LOCATE(input, TRANSPOSE(pos))
if ~ARRAY_EQUAL(res,transexp) then ERRORS_ADD, errors, "Wrong value(s)"
if ~ARRAY_EQUAL(SIZE(res),SIZE(transexp)) then ERRORS_ADD, errors, txt_wd+" 3"
;   
res=VALUE_LOCATE(TRANSPOSE(input), TRANSPOSE(pos))
if ~ARRAY_EQUAL(res,transexp) then ERRORS_ADD, errors, "Wrong value(s)"
if ~ARRAY_EQUAL(SIZE(res),SIZE(transexp)) then ERRORS_ADD, errors, txt_wd+" 4"
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_VALUE_LOCATE_1D', errors, /short
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_set(test) then STOP
;
end
;
; -------------------------------------------------
;
pro TEST_VALUE_LOCATE_2D, cumul_errors, test=test, verbose=verbose
;
errors = 0
;

input=FINDGEN(10)
pos=INDGEN(2,3)
expected=LINDGEN(2,3)
transexp=TRANSPOSE(expected)
;
txt_wd="Wrong dimension"
;
; No transpose
res=VALUE_LOCATE(input, pos)
if ~ARRAY_EQUAL(res, expected) then ERRORS_ADD, errors, "Wrong value(s)"
if ~ARRAY_EQUAL(SIZE(res),SIZE(expected)) then ERRORS_ADD, errors, txt_wd+" 1"
;
; Input transpose ("res" not tranposed)
res=VALUE_LOCATE(TRANSPOSE(input), pos)
if ~ARRAY_EQUAL(res, expected) then ERRORS_ADD, errors, "Wrong value(s)"
if ~ARRAY_EQUAL(SIZE(res),SIZE(expected)) then ERRORS_ADD, errors, txt_wd+" 2"
;
; Pos transposed then "res" tranposed
res=VALUE_LOCATE(input, TRANSPOSE(pos))
if ~ARRAY_EQUAL(res,transexp) then ERRORS_ADD, errors, "Wrong value(s)"
if ~ARRAY_EQUAL(SIZE(res),SIZE(transexp)) then ERRORS_ADD, errors, txt_wd+" 3"
;   
res=VALUE_LOCATE(TRANSPOSE(input), TRANSPOSE(pos))
if ~ARRAY_EQUAL(res,transexp) then ERRORS_ADD, errors, "Wrong value(s)"
if ~ARRAY_EQUAL(SIZE(res),SIZE(transexp)) then ERRORS_ADD, errors, txt_wd+" 4"
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_VALUE_LOCATE_2D', errors, /short
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_set(test) then STOP
;
end
;
; -------------------------------------------------
; https://github.com/gnudatalanguage/gdl/issues/1994
;
pro TEST_VALUE_LOCATE_STRING, cumul_errors, test=test, verbose=verbose
;
errors = 0
;
vec=STRING([2,5,8,10])
expected=[ -1L, 0, 1, 1, 3]
lloc = VALUE_LOCATE(vec, [0,3,5,6,12])
;
if (TYPENAME(lloc) NE "LONG") then ERRORS_ADD, errors, "Bad output type (Long expected)"
if ~ARRAY_EQUAL(lloc, expected) then ERRORS_ADD, errors, "Wrong value(s)"
;
dim1 = SIZE(lloc)
dim2 = SIZE(expected)
if ~ARRAY_EQUAL(dim1, dim2) then ERRORS_ADD, errors, "Wrong dimension"
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_VALUE_LOCATE_STRING', errors, /short
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_set(test) then STOP
;
end
;
; -------------------------------------------------
; https://github.com/gnudatalanguage/gdl/issues/2211
;
pro TEST_VALUE_LOCATE_L64, cumul_errors, test=test, verbose=verbose
;
errors = 0
;
vec=[2,5,8,10]
expected=[ -1L, 0, 1, 1, 3]
lloc = VALUE_LOCATE(vec, [0,3,5,6,12])
;
txt_badtype="Bad output type (Long expected)"
txt_badval="Wrong value(s)"
;
if (TYPENAME(lloc) NE "LONG") then ERRORS_ADD, errors, txt_badtype+" 1"
if ~ARRAY_EQUAL(lloc, expected) then ERRORS_ADD, errors, txt_badval+" 1"
;
dim1 = SIZE(lloc)
dim2 = SIZE(expected)
if ~ARRAY_EQUAL(dim1, dim2) then ERRORS_ADD, errors, "Wrong dimension"
;
; with /l64
;
lloc64 = VALUE_LOCATE(vec, [0,3,5,6,12], /l64)
if (TYPENAME(lloc64) NE "LONG64") then ERRORS_ADD, errors, txt_badtype+" l64"
if ~ARRAY_EQUAL(lloc64, expected) then ERRORS_ADD, errors, txt_badval+" l64"
;
l64non = VALUE_LOCATE(vec, [0,3,5,6,12], l64=0)
if (TYPENAME(l64non) NE "LONG") then ERRORS_ADD, errors, txt_badtype+" 2"
if ~ARRAY_EQUAL(l64non, expected) then ERRORS_ADD, errors, txt_badval+" 2"
;
l64oui=VALUE_LOCATE(vec, [0,3,5,6,12], l64=12)
if (TYPENAME(l64oui) NE "LONG64") then ERRORS_ADD, errors, txt_badtype+" l64 2"
if ~ARRAY_EQUAL(l64oui, expected) then ERRORS_ADD, errors, txt_badval+" l64 2"
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_VALUE_LOCATE_L64', errors, /short
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_set(test) then STOP
;
end
;
; -------------------------------------------------
;
pro TEST_VALUE_LOCATE, help=help, test=test, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
   print, 'pro test_value_locate, help=help, test=test, no_exit=no_exit'
   return
endif
;
cumul_errors=0
;
TEST_VALUE_LOCATE_L64, cumul_errors, test=test, verbose=verbose
TEST_VALUE_LOCATE_1D, cumul_errors, test=test, verbose=verbose
TEST_VALUE_LOCATE_2D, cumul_errors, test=test, verbose=verbose
TEST_VALUE_LOCATE_STRING, cumul_errors, test=test, verbose=verbose
;
BANNER_FOR_TESTSUITE, "TEST_VALUE_LOCATE", cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
