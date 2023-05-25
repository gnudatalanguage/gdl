;
; Very basic tests for TIMESTAMP
;
; Eloi R. de Linage, July 2021 
;
; ---------------------------------
;
; Modifications history :
; - 2023-02-20 : AC. in fact, case 6 was wrong ...
;   adding other cases ...
;
; ---------------------------------
; Testing out of bounds args
;
pro TEST_TIMESTAMP_BADRANGE, cumul_errors, test=test
;
nb_errors=0
;
success=EXECUTE('res=TIMESTAMP(DAY=90)')
if success EQ 0 then ERRORS_ADD, nb_errors, 'Error 1: timestamp() works with DAY > 31 !!!'

success=EXECUTE('res=TIMESTAMP(DAY=-8)')
if success EQ 0 then ERRORS_ADD, nb_errors, 'Error 2: timestamp() works with DAY < 1 !!!'

success=EXECUTE('res=TIMESTAMP(MONTH=-1)')
if success EQ 0 then ERRORS_ADD, nb_errors, 'Error 3: timestamp() works with MONTH < 1 !!!'

success=EXECUTE('res=TIMESTAMP(MONTH=13)')
if success EQ 0 then ERRORS_ADD, nb_errors, 'Error 4: timestamp() works with MONTH > 12 !!!'

success=EXECUTE('res=timestamp(HOUR=-2)')
if success EQ 0 then ERRORS_ADD, nb_errors, 'Error 5: timestamp() works with HOUR < 0 !!!'

success=EXECUTE('res=timestamp(HOUR=24)')
if success EQ 0 then ERRORS_ADD, nb_errors, 'Error 6: timestamps() works with HOUR > 23 !!!'


res=timestamp(HOUR=0)
if (STRMID(res, 11, STRLEN(res)-1) NE '00:00:00Z') then ERRORS_ADD, nb_errors, 'Error 7: midnight not accepted !!!'


;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_TIMESTAMP_BADRANGE', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_SET(test) then STOP
;
end
;
; ---------------------------------
; Testing various basic inputs (1D)
;
pro TEST_TIMESTAMP_BASIC, cumul_errors, test=test

nb_errors=0

ts=TIMESTAMP(/UTC)
if (STRMID(ts,STRLEN(ts)-1,1) NE 'Z') then ERRORS_ADD, nb_errors, 'Error 7: invalid format'


ts=TIMESTAMP(YEAR=2023, MONTH=9, DAY=4, HOUR=11, MINUTE=25, SECOND=15)
if ts NE '2023-09-04T11:25:15Z' then ERRORS_ADD, nb_errors, 'Error 8 basic sec/Z'


ts=TIMESTAMP(YEAR=2022, MONTH=9, DAY=4, HOUR=11, OFFSET=4, UTC=0)
if ts NE '2022-09-04T11:00:00+04:00' then ERRORS_ADD, nb_errors, 'Error 9 (offset + no UTC)'


ts=TIMESTAMP(YEAR=2012, MONTH=9, DAY=4, HOUR=13, OFFSET=-4, /UTC)
if ts NE '2012-09-04T17:00:00Z' then ERRORS_ADD, nb_errors, 'Error 10 (offset + /UTC)'


ts=TIMESTAMP(YEAR=2022, MONTH=8, DAY=5, HOUR=23, OFFSET=-3, /UTC)
if ts NE '2022-08-06T02:00:00Z' then ERRORS_ADD, nb_errors, 'Error 11 not new day'

ts=TIMESTAMP(YEAR=2023, MONTH=8, DAY=6, HOUR=1, OFFSET=3, /UTC)
if ts NE '2023-08-05T22:00:00Z' then ERRORS_ADD, nb_errors, 'Error 12 not new day'


ts=TIMESTAMP(year=2023,month=04,day=21,hour=13,offset=3.5,utc=0)
if ts NE '2023-04-21T13:00:00+03:30' then ERRORS_ADD, nb_errors, 'Error 13 (offset=float + UTC=0)'


ts=TIMESTAMP(year=2023,month=04,day=21,hour=13,offset=3.5,/utc)
if ts NE '2023-04-21T09:30:00Z' then ERRORS_ADD, nb_errors, 'Error 14 (offset=float + /UTC)'


ts=TIMESTAMP(YEAR=2023,MONTH=5+5,DAY=8)
if ts NE '2023-10-08' then ERRORS_ADD, nb_errors, 'Error 15: operation not done'


ts=TIMESTAMP(YEAR=2023,MONTH=8,DAY=2*3)
if ts NE '2023-08-06' then ERRORS_ADD, nb_errors, 'Error 16: operation not done'


ts=TIMESTAMP(YEAR=2023,MONTH=5,DAY=7,SECOND=3)
if ts NE '2023-05-07T00:00:03Z' then ERRORS_ADD, nb_errors, 'Error 17: wrong second format'


ts=TIMESTAMP(YEAR=2023,MONTH=5,DAY=7,SECOND=3.5)
if ts NE '2023-05-07T00:00:03.5Z' then ERRORS_ADD, nb_errors, 'Error 18: wrong second format'


ts=TIMESTAMP(YEAR=2023,MONTH=5,DAY=7,HOUR=4,SECOND=3.5)
if ts NE '2023-05-07T04:00:03.5Z' then ERRORS_ADD, nb_errors, 'Error 19: wrong second format'


ts=TIMESTAMP(YEAR=2023,MONTH=5,DAY=7,SECOND=3.05)
if ts NE '2023-05-07T00:00:03.05Z' then ERRORS_ADD, nb_errors, 'Error 20: wrong second format'


ts=TIMESTAMP(YEAR=2023,MONTH=5,DAY=7,SECOND=3.50)
if ts NE '2023-05-07T00:00:03.5Z' then ERRORS_ADD, nb_errors, 'Error 21: wrong second format'


ts=TIMESTAMP(YEAR=2023,MONTH=5,DAY=7,SECOND=3.999999)
if ts NE '2023-05-07T00:00:04Z' then ERRORS_ADD, nb_errors, 'Error 22: No round done'


ts=TIMESTAMP(YEAR=2023,MONTH=5,DAY=7,SECOND=3.555557)
if ts NE '2023-05-07T00:00:03.55556Z' then ERRORS_ADD, nb_errors, 'Error 23: No round done'


ts=TIMESTAMP(YEAR=2023,MONTH=5,DAY=7,SECOND=33.999999)
if ts NE '2023-05-07T00:00:34Z' then ERRORS_ADD, nb_errors, 'Error 24: No round done'

;FIXME, SEE DETAILS IN SOURCE CODE TODO
;ts=TIMESTAMP(YEAR=2023,MONTH=5,DAY=7,SECOND=33.99999)
;if ts NE '2023-05-07T00:00:33.99999Z' then ERRORS_ADD, nb_errors, 'Error 25: wrong second format'



; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_TIMESTAMP_BASIC', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_SET(test) then STOP
;
end
;
; ---------------------------------
; Testing various array inputs
;
pro TEST_TIMESTAMP_ARRAY, cumul_errors, test=test
;
nb_errors=0
;
expected=['2021-02-20','2022-02-20','2023-02-20']
ts=TIMESTAMP(year=[2021,2022,2023],day=20, month=2,/UTC)
if ~ARRAY_EQUAL(expected,ts) then ERRORS_ADD, nb_errors, 'Error 1 invalid array format'
;
expected='2023-0'+['1','2','3']+'-04T11:25:15Z'
ts=TIMESTAMP(YEAR=2023, MONTH=[1,2,3], DAY=4, HOUR=11, MINUTE=25, SECOND=15)
if  ~ARRAY_EQUAL(expected,ts) then ERRORS_ADD, nb_errors, 'Error 2 basic sec/Z'
;
expected=['2020-05-06','2021-06-06','2022-07-06']
ts=TIMESTAMP(YEAR=[2020,2021,2022],MONTH=[5,6,7],DAY=6)
if ~ARRAY_EQUAL(expected,ts) then ERRORS_ADD, nb_errors, 'Error 3 invalid array format'
;



ts=TIMESTAMP(YEAR=[[2020,2021,2022],[2023,2024,2025]])
if (size(ts))(1) NE 6 then ERRORS_ADD, nb_errors, 'Error 4 wrong array size'



ts=TIMESTAMP(YEAR=[[[2020,2021],[2022,2023],[2024,2025]],[[2026,2027],[2028,2029],[2030,2031]],[[2032,2033],[2034,2035],[2036,2037]]])
if (size(ts))(1) NE 18 then ERRORS_ADD, nb_errors, 'Error 4 wrong array size'


; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_TIMESTAMP_ARRAY', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_SET(test) then STOP
;
end
;
; ---------------------------------
; TIMESTAMP vs SYSTIME
;
pro TEST_TIMESTAMP_SYSTIME, cumul_errors, test=test
;
nb_errors=0
;
n_err=0
for i=0, 10 do begin
   wait, 0.01
   tims=TIMESTAMP()
   syst=SYSTIME()
   ;;compare seconds...
   s_tims=FIX(STRMID(tims, 17, 2))
   s_syst=FIX(STRMID(syst, 17, 2))
   if s_syst NE s_tims then begin
      n_err++
      if KEYWORD_SET(test) then print, s_syst, s_tims
   endif
endfor
; we expect at least one change during the loop ...
if n_err GT 1 then ERRORS_ADD, nb_errors, 'Error 7 (comparison with SYSTIME())'
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_TIMESTAMP_SYSTIME', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_SET(test) then STOP
;
end
;
; ---------------------------------
;
pro TEST_TIMESTAMP, test=test, quiet=quiet, help=help, no_exit=no_exit

if KEYWORD_SET(help) then begin
    print, 'pro TEST_TIMESTAMP, test=test, quiet=quiet, help=help, no_exit=no_exit'
    return
endif
;
cumul_errors=0
;
TEST_TIMESTAMP_BADRANGE, cumul_errors
TEST_TIMESTAMP_BASIC, cumul_errors
TEST_TIMESTAMP_ARRAY, cumul_errors
;
; ------------------- final message ------------------
BANNER_FOR_TESTSUITE, 'TEST_TIMESTAMP', cumul_errors
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
if KEYWORD_SET(test) then STOP
;
end
