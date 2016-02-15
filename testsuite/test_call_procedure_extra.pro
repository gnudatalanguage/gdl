;
; Bug reporte by F. Galliano, mid-February 2016
; Detected using MPFIT ...
;
; contents of structures transported by _extra may change
; if the names are "too close", depending the order ...
; struct = {c:0, baaa:1, babaa:2, baba:[3,2]} <-- NOT OK
; struct = {c:1, baaa:1, baba:[3,2], babaa:2} <-- OK
;
PRO THE_PROCEDURE, x, p, y, _EXTRA=extra
;
common flag, error
;
print, 'INSIDE THE_PROCEDURE'
HELP, /STRUCT, extra
;
expected={c:0, baaa:1, babaa:2, baba:[3,2]}
;
ok=ARRAY_EQUAL(extra.baba, expected.baba)
if (ok NE 1) then begin
    print, 'FATAL : fields in structure changed !!'
    error=1
endif
;
END
;
; -----------------------------------------------------
;
pro CALL_PROCEDURE_EXTRA, change_order=change_order
;
common flag, error
;
if KEYWORD_SET(change_order) then begin
    ;; this case is OK
    extra = {c:1, baaa:1, baba:[3,2], babaa:2 }
endif else begin
    ;; this case does not work (as is on Feb 15, 2016, since years)
    extra = {c:0, baaa:1, babaa:2, baba:[3,2]}
endelse
;
PRINT, 'at level : TEST_CALL_PROCEDURE_EXTRA'
HELP, /STRUCT, extra
;
x=0 & p=0 & y=0
;
CALL_PROCEDURE, "THE_PROCEDURE", x, p, y, _EXTRA=extra
;
PRINT, 'returning back at level : TEST_CALL_PROCEDURE_EXTRA'
HELP, /STRUCT, extra
;
END
;
; -------------------------------------------------
;
pro TEST_CALL_PROCEDURE_EXTRA, help=help, test=test, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_CALL_PROCEDURE_EXTRA, help=help, test=test, no_exit=no_exit'
   return
endif
;
common flag, error
;
cumul_errors=0
error=0
;
CALL_PROCEDURE_EXTRA
cumul_errors=cumul_errors+error
print, error
;
error=0 ; reset error
CALL_PROCEDURE_EXTRA, /change_order
cumul_errors=cumul_errors+error
print, error
;
BANNER_FOR_TESTSUITE, 'TEST_CALL_PROCEDURE_EXTRA', cumul_errors, short=short
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end

