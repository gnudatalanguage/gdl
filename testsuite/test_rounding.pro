;
; Alain C, 26 Feb. 2018, under GNU GPL v3 or later
;
; Except FIX() in "test_fix.pro", we don't ready test the rounding
; functions : ROUND(), CEIL(), FLOOR(), COMPLEXROUND()
;
; We have to test :
;  -- if the output TYPE (and value) is OK or not for all integer type
;     (TEST_ROUNDING_TYPE) : it should not be changed !!!!
;  -- if the output  is OK of not (TEST_ROUNDING_REAL)
;
; We also need to check whether the /L64 is OK or not
;
; IMPORTANT NOTE : 
;
; AC 2018-Mar-17 during night shift test for MIRI (JWST)
; this code is working for IDL (8.2 & 8.4)
; this code is not OK for GDL on TEST_ROUNDING_L64: /ceil & /floor
;
; ----------------------------------------------------
; Modifications history :
;
; 2018-Feb-26 : 
;
; ----------------------------------------------------
;
; First test is not really on value BUT on type preservation 
; for all types in "INTEGER" family !
;
pro TEST_ROUNDING_TYPE, cumul_errors, test=test, verbose=verbose, $
                        round=round, ceil=ceil, floor=floor
;
flags=KEYWORD_SET(round) + KEYWORD_SET(ceil) + KEYWORD_SET(floor)
if flags EQ 0 then MESSAGE, 'Set at least one keyword in : /ROUND, /CEIL, /FLOOR'
if flags GT 1 then MESSAGE, 'Keywords /ROUND, /CEIL, /FLOOR are exclusive'
;
if KEYWORD_SET(round) then MESSAGE, /Continue, 'testing /ROUND'
if KEYWORD_SET(ceil) then MESSAGE, /Continue, 'testing /CEIL'
if KEYWORD_SET(floor) then MESSAGE, /Continue, 'testing /FLOOR'
;
nb_errors=0
;
GIVE_LIST_NUMERIC, list_numeric_types, list_numeric_names, /integer
types=list_numeric_types
names=list_numeric_names
;
seed=[123.4, -123.4]
;
;if KEYWORD_SET(round) then 
expected=[123, -123]
;
for ii=0, N_ELEMENTS(list_numeric_types)-1 do begin
   ;;
   if KEYWORD_SET(round) then res_int=ROUND(FIX(seed, type=types[ii]))
   if KEYWORD_SET(ceil) then res_int=CEIL(FIX(seed, type=types[ii]))
   if KEYWORD_SET(floor) then res_int=FLOOR(FIX(seed, type=types[ii]))
   ;;
   expected_int=FIX(expected, type=list_numeric_types[ii])
   ;;
   if KEYWORD_SET(verbose) then begin
      HELP, res_int
      HELP, expected_int
      print, res_int
      print, expected_int
   endif
   ;;
   if (TYPENAME(res_int) NE TYPENAME(expected_int)) then $
      ERRORS_ADD, nb_errors, 'bad conv. type '+list_numeric_names[ii]
   if ~ARRAY_EQUAL(expected_int, res_int, /no_typeconv) then begin
      ERRORS_ADD, nb_errors, 'bad value type '+list_numeric_names[ii]
      if (expected_int[0] NE res_int[0]) then print, 'Pos. side'
      if (expected_int[1] NE res_int[1]) then print, 'Neg. side'
   endif
endfor
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_ROUNDING_TYPE', nb_errors, /noline
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; ----------------------------------------------
;
; second test for Float, Double, Complex & Dcomplex
;
pro TEST_ROUNDING_FLOAT, cumul_errors, test=test, verbose=verbose, $
                         round=round, ceil=ceil, floor=floor
;
flags=KEYWORD_SET(round) + KEYWORD_SET(ceil) + KEYWORD_SET(floor)
if flags EQ 0 then MESSAGE, 'Set at least one keyword in : /ROUND, /CEIL, /FLOOR'
if flags GT 1 then MESSAGE, 'Keywords /ROUND, /CEIL, /FLOOR are exclusive'
;
if KEYWORD_SET(round) then MESSAGE, /Continue, 'testing /ROUND'
if KEYWORD_SET(ceil) then MESSAGE, /Continue, 'testing /CEIL'
if KEYWORD_SET(floor) then MESSAGE, /Continue, 'testing /FLOOR'
;
nb_errors=0
;
seed=[123.4, -123.4]
expected_long=[123l,-123l]
if KEYWORD_SET(ceil) then expected_long[0]++
if KEYWORD_SET(floor) then expected_long[1]--
;
GIVE_LIST_NUMERIC, list_numeric_types, list_numeric_names
types=[4,5,6,9]
;
for ii=0, N_ELEMENTS(types)-1 do begin
   factor=1.+MAKE_ARRAY(1,type=types[ii])
   val=factor[0]*seed
   ;;
   if KEYWORD_SET(round) then res_long=ROUND(val)
   if KEYWORD_SET(ceil) then res_long=CEIL(val)
   if KEYWORD_SET(floor) then res_long=FLOOR(val)
   ;;
   if KEYWORD_SET(verbose) then begin
      print, 'input val. : ', val
      print, 'expected val. : ', expected_long
      print, 'result val. : ', res_long
   endif
   ;;
   if (TYPENAME(res_long) NE TYPENAME(expected_long)) then $
      ERRORS_ADD, nb_errors, 'bad conv. type '+list_numeric_names[types[ii]]
   if ~ARRAY_EQUAL(expected_long, res_long, /no_typeconv) then begin
      ERRORS_ADD, nb_errors, 'bad value type '+list_numeric_names[types[ii]]
      if (expected_long[0] NE res_long[0]) then print, 'Pos. side'
      if (expected_long[1] NE res_long[1]) then print, 'Neg. side'
   endif   
endfor
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_ROUNDING_FLOAT', nb_errors, /noline
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; ----------------------------------------------
;
pro TEST_ROUNDING_L64, cumul_errors, test=test, verbose=verbose, $
                       round=round, ceil=ceil, floor=floor
;
flags=KEYWORD_SET(round) + KEYWORD_SET(ceil) + KEYWORD_SET(floor)
if flags EQ 0 then MESSAGE, 'Set at least one keyword in : /ROUND, /CEIL, /FLOOR'
if flags GT 1 then MESSAGE, 'Keywords /ROUND, /CEIL, /FLOOR are exclusive'
;
if KEYWORD_SET(round) then MESSAGE, /Continue, 'testing /ROUND'
if KEYWORD_SET(ceil) then MESSAGE, /Continue, 'testing /CEIL'
if KEYWORD_SET(floor) then MESSAGE, /Continue, 'testing /FLOOR'
;
nb_errors=0
;
; third test : above 2LL^31 (L64 needed)
;
large=4294967296.1d ;; (2LL^31 +0.1d)
mess=[', case Pos.',', case Neg.']
;
expected_long=2L^31  ;;-2147483648l
expected_l64=4294967296ll
;
for ii=0, 1 do begin
   if (ii EQ 1) then begin
      large=-1d*large
      expected_l64=-expected_l64
   endif
   ;;
   if KEYWORD_SET(round) then begin
      res_long=ROUND(large)
      res_l64=ROUND(large,/l64)
   endif
   if KEYWORD_SET(ceil) then begin
      res_long=CEIL(large)
      res_l64=CEIL(large,/l64)
      if (ii EQ 0) then begin
         res_long--
         res_l64--         
      endif
   endif
   if KEYWORD_SET(floor) then begin
      res_long=FLOOR(large)
      res_l64=FLOOR(large,/l64)
      if (ii EQ 1) then begin
         res_long++
         res_l64++
      endif
   endif
   ;;
   if KEYWORD_SET(verbose) then begin
      print, 'Expected :', expected_long, expected_l64
      print, 'Result   :', res_long, res_l64
   endif
   ;;
   if (TYPENAME(res_long) NE 'LONG') then $
      ERRORS_ADD, nb_errors, 'bad conv. type Long'+mess[ii]
   if (TYPENAME(res_l64) NE 'LONG64') then $
      ERRORS_ADD, nb_errors, 'bad conv. type L64'+mess[ii]
   ;;
   if ~ARRAY_EQUAL(expected_long, res_long, /no_typeconv) then $
      ERRORS_ADD, nb_errors, 'bad value type LONG'+mess[ii]
   if ~ARRAY_EQUAL(expected_l64, res_l64, /no_typeconv) then $
      ERRORS_ADD, nb_errors, 'bad value type LONG64'+mess[ii]
endfor
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_ROUNDING_L64', nb_errors, /noline ;short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; ----------------------------------------------
;
pro TEST_ROUNDING, help=help, test=test, no_exit=no_exit, verbose=verbose
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_ROUNDING, help=help, test=test, $'
   print, '                   no_exit=no_exit, verbose=verbose'
   return
endif
;
cumul_errors=0
;
BANNER_FOR_TESTSUITE, 'TEST_ROUNDING_TYPE', 0, /line
TEST_ROUNDING_TYPE, cumul_errors, verbose=verbose, /round
TEST_ROUNDING_TYPE, cumul_errors, verbose=verbose, /ceil
TEST_ROUNDING_TYPE, cumul_errors, verbose=verbose, /floor
BANNER_FOR_TESTSUITE, 'TEST_ROUNDING_TYPE', 0, /line
;
BANNER_FOR_TESTSUITE, 'TEST_ROUNDING_FLOAT', 0, /line
TEST_ROUNDING_FLOAT, cumul_errors, verbose=verbose, /round
TEST_ROUNDING_FLOAT, cumul_errors, verbose=verbose, /ceil
TEST_ROUNDING_FLOAT, cumul_errors, verbose=verbose, /floor
BANNER_FOR_TESTSUITE, 'TEST_ROUNDING_FLOAT', 0, /line
;
BANNER_FOR_TESTSUITE, 'TEST_ROUNDING_L64', 0, /line
TEST_ROUNDING_L64, cumul_errors, verbose=verbose, /round
TEST_ROUNDING_L64, cumul_errors, verbose=verbose, /ceil
TEST_ROUNDING_L64, cumul_errors, verbose=verbose, /floor
BANNER_FOR_TESTSUITE, 'TEST_ROUNDING_L64', 0, /line
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_ROUNDING', cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
