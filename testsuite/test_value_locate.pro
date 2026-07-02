;
; Yanis Saglio, 30 june 2026
; under GNU GPL V3
; 
; Fix issue #2179
;
pro test_value_locate, help=help, test=test, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
   print, 'pro test_value_locate, help=help, test=test, no_exit=no_exit'
   return
endif
;
errors = 0
;
expected = [[0,1],[2,3],[4,5]]
test = value_locate(findgen(10),findgen(2,3))
if ~(array_equal(test, expected)) then errors_add, errors, "Wrong value(s)"
dim = size(test, /dimensions)
if ~(array_equal(dim,[2,3])) then errors_add, errors, "Wrong dimension"
;
BANNER_FOR_TESTSUITE, "TEST_VALUE_LOCATE", errors
;
if (errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
