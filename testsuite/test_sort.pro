;
; under GNU GPL v2 or later
; Alain C., Oct. 31 2019 @ JPL
;
; Basic test on SORT() after bug report #659
; https://github.com/gnudatalanguage/gdl/issues/659
; ---------------------------------
; 
; Modifications history :
;
; - 2019-10-31 : AC. Creation, from a suggestion of @maynardGK in #659
;                but the original WHERE() is remplace by a TOTAL()
;                (less side-effect expected TBC)
;
; ---------------------------------
;
pro TEST_SORT_NELEMENTS, cumul_errors, nbps, test=test
;
nb_errors=0
;
array=RANDOMU(seed, nbps)
sort_array=array[SORT(array)]
;
nb_errors=TOTAL(sort_array[1:*] LT sort_array)
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_SORT_NELEMENTS', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; -------------------------------------------------
;
pro TEST_SORT, no_exit=no_exit, test=test
;
TEST_SORT_NELEMENTS, cumul_errors, 50
TEST_SORT_NELEMENTS, cumul_errors, 90
TEST_SORT_NELEMENTS, cumul_errors, 110
TEST_SORT_NELEMENTS, cumul_errors, 180
TEST_SORT_NELEMENTS, cumul_errors, 280
TEST_SORT_NELEMENTS, cumul_errors, 990
TEST_SORT_NELEMENTS, cumul_errors, 1190
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_SORT', cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
