;
; Testing LINFIT
; Todo: Linear fitting for vectors
;
; Licensed under GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
;
; ---------------------
; Modification history:
;
; 2018-09-20 : Bin Wu <bin.wu (at) edinsights (dot) no>
; - Initial version
; - Basic LINFIT test
; 2019-05-06 : GD : algorithm changed for copyright purposes and
; too fussy test.    
; ---------------------
;
PRO TEST_LINFIT, HELP = HELP, NERR_TOTAL, TEST = TEST, VERBOSE = VERBOSE

    IF KEYWORD_SET(HELP) THEN BEGIN
        PRINT, 'PRO TEST_LINFIT, HELP=HELP, TEST=TEST, NO_EXIT=NO_EXIT, VERBOSE=VERBOSE'
        RETURN
    ENDIF
    NERR = 0
; this is a subcase of poly_fit really. However it uses another algorithm.
    X = randomn(33,256,/ran1) & err=randomn(44,256,/ran1)*3
    Y = -3.000+1.33333*x+err
    RESULT = LINFIT(X, Y, MEASURE_ERRORS=err)
; testing on default formatted results permits the use of ARRAY_EQUAL as its compares bits, and the formatting is OK for result precision.  
    GOOD = ["     -2.97982","      1.34937"]

    if ~ARRAY_EQUAL(STRING(RESULT),GOOD) then ERRORS_ADD, NERR, 'LINFIT test failed.'

    BANNER_FOR_TESTSUITE, 'TEST_LINFIT', NERR, /STATUS
    ERRORS_CUMUL, NERR_TOTAL, NERR
    IF KEYWORD_SET(TEST) THEN STOP
END
