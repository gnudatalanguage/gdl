;
;  GDL replacement to LINFIT in IDL
;  Bin Wu <bin.wu (at) edinsights.no>
;  Sep. 2018
;
;  Purpose:
;  The LINFIT function fits the paired data {X, Y} to the linear model, Y = A + B*X, by minimizing the chi-square error statistic. The chi-square error statistic is computed as
;  CHI^2(A,B) = sum((Y[i]-A-B*X[i])^2).
;
;  Arguments (from IDL):
;    X:
;         An n-element vector containing the independent variable values. X may be of type integer, floating point, or double-precision floating-point.
;    Y:
;         An n-element integer, single-, or double-precision floating-point vector.
;
;  Keywords (from IDL):
;    CHISQR:
;         Set this keyword to a named variable that will contain the value of the unreduced chi-square goodness-of-fit statistic.
;    COVAR:
;         Set this keyword to a named variable that will contain the covariance matrix of the coefficients. The COVAR matrix depends only upon the
;         independent variable X and (optionally) the MEASURE_ERRORS. The values do not depend upon Y.
;    DOUBLE:(a boolean keyword)
;         Set this keyword to force the computation to be done in double-precision arithmetic.
;    MEASURE_ERRORS:
;         Set this keyword to a vector containing standard measurement errors for each point Y[i]. This vector must be the same length as X and Y. When
;         MEASURE_ERRORS is set, the chi-square error statistic is computed as CHI^2(A,B) = sum(((Y[i]-A-B*X[i])/σ[i])^2), where σ represents the measurement error. For Gaussian errors (e.g.,
;         instrumental uncertainties), MEASURE_ERRORS should be set to the standard deviations of each point in Y. For Poisson or statistical weighting,
;         MEASURE_ERRORS should be set to SQRT(ABS(Y)).
;    PROB:
;         Set this keyword to a named variable that will contain the probability that the computed fit would have a value of CHISQ or greater. If PROB is
;         greater than 0.1, the model parameters are “believable”. If PROB is less than 0.1, the accuracy of the model parameters is questionable.
;    SIGMA:
;         Set this keyword to a named variable that will contain the 1-sigma uncertainty estimates for the returned parameters. If MEASURE_ERRORS is omitted,
;         then you are assuming that a straight line is the correct model for your data, and therefore, no independent goodness-of-fit test is possible. In
;         this case, the values returned in SIGMA are multiplied by SQRT(CHISQ/(N–M)), where N is the number of points in X, and M is the number of
;         coefficients.
;    YFIT:
;         Set this keyword equal to a named variable that will contain the vector of calculated Y values.
;  Return (from IDL): The result is a two-element vector containing the linear model parameters [A, B].
;
;  Syntax (from IDL):
;	 Result = LINFIT( X, Y [, CHISQR=variable] [, COVAR=variable] [, /DOUBLE] [, MEASURE_ERRORS=vector] [, PROB=variable]
;            [, SIGMA=variable] [, YFIT=variable] )
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
