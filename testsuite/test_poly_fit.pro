;
; Testing POLY_FIT
;
; Licensed under GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
;
; ---------------------
; Modification history:
;
; 2019-05-06 : GD 
; - Basic POLY_FIT test
;
; ---------------------
;
PRO TEST_POLY_FIT, HELP = HELP, NERR_TOTAL, TEST = TEST, VERBOSE = VERBOSE

    IF KEYWORD_SET(HELP) THEN BEGIN
        PRINT, 'PRO TEST_POLY_FIT, HELP=HELP, TEST=TEST, NO_EXIT=NO_EXIT, VERBOSE=VERBOSE'
        RETURN
    ENDIF
    NERR = 0

    X = randomn(33,256,/ran1) & err=randomn(44,256,/ran1)*3 ; /RAN1 to eventually compare with IDL (non-dSFMT random numbers). But will never be comparable.
    Y = 0.25+1.33*x-2.28*x*x+3.14*x^3+err
    RESULT = POLY_FIT(X, Y, 3, MEASURE_ERRORS=err, chisq=chisq, yfit=yfit, covar=covar, yerror=yerror, sigma=sigma, yband=yband)
; testing on default formatted results permits the use of ARRAY_EQUAL as its compares bits, and the formatting is OK for result precision.  
    GOOD = ["      0.27383122","       1.3734589","      -2.2819402","       3.1208941"]


    if ~ARRAY_EQUAL(STRING(RESULT),GOOD) then ERRORS_ADD, NERR, 'POLY_FIT test failed.'


    BANNER_FOR_TESTSUITE, 'TEST_POLY_FIT', NERR, /STATUS
    ERRORS_CUMUL, NERR_TOTAL, NERR
    IF KEYWORD_SET(TEST) THEN STOP
END
