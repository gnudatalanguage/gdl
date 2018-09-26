;
;  GDL replacement to LINFIT in IDL
;  Bin Wu <bin.wu (at) edinsights.no>
;  Sep. 2018
;
;  Ref: http://www.harrisgeospatial.com/docs/LINFIT.html
;
FUNCTION LINFIT,X,Y,$
                CHISQR=CHISQR,COVAR=COVAR,DOUBLE=DOUBLE,$
                MEASURE_ERRORS=MEASURE_ERRORS,PROB=PROB,SIGMA=SIGMA,YFIT=YFIT
; Return the caller of a procedure in the event of an error.
  ON_ERROR, 2

;  To ensure X and Y are both vectors with the same size.
    dimsX = SIZE(X)
    dimsY = SIZE(Y)
    IF (dimsX[0] NE 1) OR (dimsY[0] NE 1) OR (dimsX[1] NE dimsY[1]) THEN MESSAGE, 'X and Y must be vectors of the same size'

;  To force the computation to be done in double-precision arithmetic.
    IF KEYWORD_SET(DOUBLE) THEN BEGIN
        X = DOUBLE(X)
        Y = DOUBLE(Y)
    ENDIF

;  To give a vector containing standard measurement errors for each point Y[i].
    IF KEYWORD_SET(MEASURE_ERRORS) THEN BEGIN
        dimsE = SIZE(MEASURE_ERRORS)
        IF (dimsE[0] NE 1) OR (dimsE[1] NE dimsY[1]) THEN MESSAGE, 'MEASURE_ERRORS must be of the same size as Y'
    ENDIF ELSE BEGIN
        MEASURE_ERRORS = Y - Y + 1
    ENDELSE

;  Coefficients calculation.
    S = TOTAL(1/(MEASURE_ERRORS^2))
    Sx = TOTAL(X/(MEASURE_ERRORS^2))
    Sy = TOTAL(Y/(MEASURE_ERRORS^2))
    Ti = (X-Sx/S)/MEASURE_ERRORS
    Stt = TOTAL(Ti^2)

;  Solve 2x2 linear equations by CRAMER's rule
    B = TOTAL(Ti*Y/MEASURE_ERRORS)/Stt
    A = (Sy - Sx*B)/S

;  Variance estimation.
    Va = (1+Sx^2/S/Stt)/S
    Vb = 1/Stt
    CVab = -Sx/S/Stt
    COVAR = [[Va, CVab],[CVab,Vb]]

;  Approximated Y.
    YFIT = A + B*X

;  Calculate the unreduced chi-square goodness-of-fit statistic.
    CHISQR = TOTAL(((Y-YFIT)/MEASURE_ERRORS)^2)

;  The 1-sigma uncertainty estimates for the returned parameters and the probability that the computed fit.
    SIGMA = [SQRT(Va),SQRT(Vb)]
    IF KEYWORD_SET(MEASURE_ERRORS) THEN PROB = 1 - IGAMMA((dimsY[1]-2)/2.0,CHISQR/2) ELSE BEGIN
        PROB = Y-Y+1
        fct = SQRT(CHISQR/(dimsY[1]-2))
        SIGMA = SIGMA*fct
    ENDELSE

    AB = [A,B]
    RETURN, AB
END

