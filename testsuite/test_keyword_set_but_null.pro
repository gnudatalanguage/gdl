function test_kw,kw=kw
if keyword_set(kw) then return, 1 else return, 0
end

PRO test_keyword_set_but_null, no_exit=no_exit
total_errors=0
if test_kw(kw=[]) ne 0 then total_errors++
;
; final message
;
BANNER_FOR_TESTSUITE, 'TEST_FILE_LINES', total_errors, short=short
;
if KEYWORD_SET(test) then STOP
;
if (total_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
end
