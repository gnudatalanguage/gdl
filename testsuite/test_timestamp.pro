;
; Very basic tests for TIMESTAMP
;
; Eloi R. de Linage, July 2021 
;
pro TEST_TIMESTAMP, test=test, quiet=quiet, help=help, no_exit=no_exit

if KEYWORD_SET(help) then begin
    print, 'pro TEST_TIMESTAMP, test=test, quiet=quiet, help=help, no_exit=no_exit'
    return
endif

errors=0

; Testing out of bounds args

success=execute('res=timestamp(DAY=90)')
if success NE 0 then ERRORS_ADD, errors, 'Error 1: timestamp() works with DAY > 31 !!!'
success=execute('res=timestamp(HOUR=-2)')
if success NE 0 then ERRORS_ADD, errors, 'Error 2: timestamp() works with HOUR < 0 !!!'

ts=timestamp(/UTC)
if (strmid(ts,strlen(ts)-1,1) NE 'Z') then ERRORS_ADD, errors, 'Error 3: invalid format'

ts = TIMESTAMP(YEAR = 2012, MONTH = 9, $
   DAY = 4, HOUR = 11, MINUTE = 25, SECOND = 15)
if ts NE '2012-09-04T11:25:15Z' then ERRORS_ADD, errors, 'Error 4'

ts = TIMESTAMP(YEAR = 2012, MONTH = 9, DAY = 4, $
   HOUR = 11, OFFSET = 4, UTC = 0)
if ts NE '2012-09-04T11:00:00+04:00' then ERRORS_ADD, errors, 'Error 5'

ts = TIMESTAMP(YEAR = 2012, MONTH = 9, DAY = 4, $
   HOUR = 13, OFFSET = -4, /UTC)
if ts NE '2012-09-04T13:00:00Z' then ERRORS_ADD, errors, 'Error 6'

n_err=0
for i=0, 3 do begin
    wait, 0.01
    tims=timestamp()
    syst=systime()
    ;compare seconds...
    s_tims=FIX(STRMID(tims, 17, 2))
    s_syst=FIX(STRMID(syst, 17, 2))
    if s_syst NE s_tims then n_err++
endfor
if n_err GT 1 then ERRORS_ADD, errors, 'Error 7 (comparison with SYSTIME())'

; ------------------- final message ------------------
BANNER_FOR_TESTSUITE, 'TEST_TIMESTAMP', errors
if (errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
if KEYWORD_SET(test) then STOP
end
