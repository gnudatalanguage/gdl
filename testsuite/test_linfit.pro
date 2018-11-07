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
; ---------------------
;
PRO TEST_LINFIT, HELP = HELP, NERR_TOTAL, TEST = TEST, VERBOSE = VERBOSE

    IF KEYWORD_SET(HELP) THEN BEGIN
        PRINT, 'PRO TEST_LINFIT, HELP=HELP, TEST=TEST, NO_EXIT=NO_EXIT, VERBOSE=VERBOSE'
        RETURN
    ENDIF
    NERR = 0

; Test on EXAMPLEs from http://www.harrisgeospatial.com/docs/LINFIT.html
    X = [-3.20, 4.49, -1.66, 0.64, -2.43, -0.89, -0.12, 1.41, 2.95, 2.18, 3.72, 5.26]
    Y = [-7.14, -1.30, -4.26, -1.90, -6.19, -3.98, -2.87, -1.66, -0.78, -2.61, 0.31, 1.74]
    MEASURE_ERRORS = SQRT(ABS(Y))
    RESULT = LINFIT(X, Y, MEASURE_ERRORS=MEASURE_ERRORS)
    RESULT_IDL = [-3.16574,0.829856]
    IF ~ARRAY_EQUAL(RESULT, RESULT_IDL) THEN $
    ERRORS_ADD, NERR, 'Unexpected return value of LINFIN on given vectors X and Y'

    BANNER_FOR_TESTSUITE, 'TEST_LINFIT', NERR, /STATUS
    ERRORS_CUMUL, NERR_TOTAL, NERR
    IF KEYWORD_SET(TEST) THEN STOP
END
