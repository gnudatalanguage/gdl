PRO test_file_lines, no_exit=no_exit
total_errors=0
if file_lines("./test_file_lines.pro") ne 13 then total_errors++
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
