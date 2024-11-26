;
; Seem to be related to a bug discovered long time ago in SF !
; closed bug 708: This didn't work.
;
; This test not only test "HEAP_REFCOUNT" but also "PTRARR" !
;
; Modifications history :
; -2024-05-27 AC : cleaning (details ...)
;
pro TEST_HEAP_REFCOUNT, test=test, no_exit=no_exit
;
err=0
;
ab = PTR_NEW(FLTARR(12))
cmp = {a:ab, b:ab}
if (HEAP_REFCOUNT(ab) ne 3) then ERRORS_ADD, err, 'First error'
;
jptr = PTRARR(4)
for k=1,3 do jptr[k] = PTR_NEW(FINDGEN(100+k))
jptr[0] = ab
if (TOTAL(HEAP_REFCOUNT(jptr)) ne 4+1+1+1) then ERRORS_ADD, err, 'second error'
;
kptr = PTRARR(4)
for k=0,3 do kptr[k] = jptr[k]
if (TOTAL(HEAP_REFCOUNT(jptr)) ne 5+2+2+2) then ERRORS_ADD, err, 'third error'
;
cumul_errors=err
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_HEAP_REFCOUNT', cumul_errors, short=short
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
