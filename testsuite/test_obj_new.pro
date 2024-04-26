;
; This bug was discoverd by chance on April, 29, 2014
;
; Alain C., under GNU GPL
;
; 2018 change object example from gdlffdicom object,
; which is huge, to a lighter code 'idlsysmonitorinfo'
; (Win32 has trouble gdlffdicom unrelated to this test.)
;
; Modifications history :
; -2024-04-15 AC : cleaning (details ...)
;
; -------------------------------------------------
;
pro USING_PRO, count, verbose=verbose
;
if KEYWORD_SET(verbose) then MESSAGE, /continue, 'inside USING_PRO'
tic
tmp1=OBJ_NEW('idlsysmonitorinfo')
toc
count=count+1
if KEYWORD_SET(verbose) then MESSAGE, /continue, 'end inside USING_PRO'
;
end
; -------------------------------------------------
;
pro USING_PRO_ONE_LINE, count, verbose=verbose
;
if KEYWORD_SET(verbose) then MESSAGE, /continue, 'inside USING_PRO_ONE_LINE'
txt='end of line calling  OBJ_NEW in USING_PRO_ONE_LINE'
;
tic & tmp1=OBJ_NEW('idlsysmonitorinfo') & count++ & toc & print, txt
;
if KEYWORD_SET(verbose) then MESSAGE, /continue, 'end inside USING_PRO_ONE_LINE'
;
end
; -------------------------------------------------
;
function USING_FUN, count, verbose=verbose
;
if KEYWORD_SET(verbose) then MESSAGE, /continue, 'inside USING_FUN'
tic
tmp1=OBJ_NEW('idlsysmonitorinfo')
toc
count=count+1
if KEYWORD_SET(verbose) then MESSAGE, /continue, 'end inside USING_FUN'
;
return, -1
;
end
; -------------------------------------------------
;
pro TEST_TEST_OBJ_NEW, count, skip=skip, verbose=verbose, $
                       test=test, debug=debug
;
;if count NE !NULL then print, count
if (N_PARAMS() EQ 0) then count=0
;
in='going into '
out='outside '
;
if ~KEYWORD_SET(skip) then begin
    if KEYWORD_SET(verbose) then MESSAGE, /continue, in+'USING_PRO'
    USING_PRO, count, verbose=verbose
    if KEYWORD_SET(verbose) then MESSAGE, /continue, out+'USING_PRO'
    if KEYWORD_SET(debug) then print, count
endif
;
if ~KEYWORD_SET(skip) then begin
    if KEYWORD_SET(verbose) then MESSAGE, /continue, in+'USING_PRO_ONE_LINE'
    USING_PRO_ONE_LINE, count, verbose=verbose
    if KEYWORD_SET(verbose) then MESSAGE, /continue, out+'USING_PRO_ONE_LINE'
    if KEYWORD_SET(debug) then print, count
endif
;
if KEYWORD_SET(verbose) then MESSAGE, /continue, in+'USING_FUN'
res=USING_FUN(count, verbose=verbose)
if KEYWORD_SET(verbose) then MESSAGE, /continue, out+'USING_FUN'
;
print, 'final incremental value of COUNT: ', count
;
if KEYWORD_SET(test) then STOP
;
end
; -------------------------------------------------
;
pro SUPER_CALL_OF_TEST_OBJ_NEW, count, verbose=verbose, test=test
;
;if count NE !NULL then print, count
if (N_PARAMS() EQ 0) then count=0
;
CALL_PROCEDURE, "TEST_TEST_OBJ_NEW", count, verbose=verbose, test=test
;stop
end
;
; -------------------------------------------------
;
pro TEST_OBJ_NEW, help=help, verbose=verbose, test=test, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_OBJ_NEW, help=help, verbose=verbose, $'
    print, '                  no_exit=no_exit, test=test'
    return
endif
;
cumul_errors=0
;
; first call skipping problematic calls
;
count1=0
TEST_TEST_OBJ_NEW, count1, /skip, verbose=verbose
if (count1 NE 1) then begin
   ERRORS_ADD, cumul_errors, 'Error 1'
endif else print, 'succes 1'
;
; seconf call with problematic calls
;
count2=0
TEST_TEST_OBJ_NEW, count2, verbose=verbose
if (count2 NE 3) then begin
   ERRORS_ADD, cumul_errors, 'Error 2'
endif else print, 'succes 2'
;
count3=0
SUPER_CALL_OF_TEST_OBJ_NEW, count3, verbose=verbose, test=test
if (count3 NE 3) then begin
   ERRORS_ADD, cumul_errors, 'Error 3'
endif else print, 'succes 3'
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_OBJ_NEW', cumul_errors, short=short
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end

