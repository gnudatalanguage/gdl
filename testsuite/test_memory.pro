; GD 03/12/2022
; changed return value to insure test does not impact validity of a PR.
; since we have a warning message in the log, this can be cured at will
; the test is NOT reliable as it passes very well on a Catalina.
;
; Small changes by Alain Coulais on Nov 2026, MSwin should be OK now
;
; -------------------------------------------------
;
pro TEST_MEMORY, no_exit=no_exit, help=help
;
cumul_errors=0
;
; AC 2026-04-02 temporary short-cut for tests on MSwin
; if (!version.os_family eq 'Windows') then EXIT, status=77
;
memref = MEMORY(/curr)
a = DBLARR(9999999)
memnew = MEMORY(/curr)
;
if (memref GE memnew) then begin
   txt='reported memory consumption should increase after allocating a big array!'
   ERRORS_ADD, cumul_errors, txt
endif
;
; final message
;
BANNER_FOR_TESTSUITE, 'TEST_MEMORY', cumul_errors, short=short
;
if KEYWORD_SET(test) then STOP
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
end
