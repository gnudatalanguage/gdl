;
;  GDL replacement to LINFIT in IDL
;  Bin Wu <bin.wu (at) edinsights.no>
;  Sep. 2018
;
;  Ref: http://www.harrisgeospatial.com/docs/LINFIT.html
;
FUNCTION LINFIT,X,Y,$
                CHISQR=CHISQR,COVAR=COVAR,DOUBLE=DOUBLE,$
                MEASURE_ERRORS=MEASURE_ERRORS,PROB=PROB,SIGMA=SIGMA,SIG_AB=SIG_AB,YFIT=YFIT,SDEV=SDEV


  COMPILE_OPT idl2
; Return the caller of a procedure in the event of an error.
  ON_ERROR, 2

;  To ensure X and Y are both vectors with the same size.
    nElX = N_ELEMENTS(X)
    nElY = N_ELEMENTS(Y)
    IF (nElX NE nElY) THEN MESSAGE, 'X and Y must be vectors of the same size'

;  To force the computation to be done in double-precision arithmetic.
    IF KEYWORD_SET(DOUBLE) THEN BEGIN
        X = DOUBLE(X)
        Y = DOUBLE(Y)
    ENDIF

;  To give a vector containing standard measurement errors for each
;  point Y[i]. SDEV is an old name for MEASURE_ERRORS.

    hasSdev = n_elements(sdev) gt 0
    hasME = n_elements(measure_errors) gt 0
    if (hasSdev and hasME) then message, 'Conflicting keywords SDEV and MEASURE_ERRORS.'
    if (~hasSdev and ~hasME) then STDDEV = Y*0 + 1 else begin
       stddev=hasSdev?sdev:measure_errors
       stddev_name=hasSdev?"SDEV":"MEASURE_ERRORS"
       nElS = N_ELEMENTS(STDDEV)
       IF nElS NE nElX THEN MESSAGE, stddev_name+' must have the same number of elements as X and Y.'
    endelse

;  Coefficients calculation.
    S = TOTAL(1/(STDDEV^2))
    Sx = TOTAL(X/(STDDEV^2))
    Sy = TOTAL(Y/(STDDEV^2))
    Ti = (X-Sx/S)/STDDEV
    Stt = TOTAL(Ti^2)

;  Solve 2x2 linear equations by CRAMER's rule
    B = TOTAL(Ti*Y/STDDEV)/Stt
    A = (Sy - Sx*B)/S

;  Variance estimation.
    Va = (1+Sx^2/S/Stt)/S
    Vb = 1/Stt
    CVab = -Sx/S/Stt
    COVAR = [[Va, CVab],[CVab,Vb]]

;  Approximated Y.
    YFIT = A + B*X

;  Calculate the unreduced chi-square goodness-of-fit statistic.
    CHISQR = TOTAL(((Y-YFIT)/STDDEV)^2)

;  The 1-sigma uncertainty estimates for the returned parameters and the probability that the computed fit.
    SIGMA = [SQRT(Va),SQRT(Vb)]
    IF (hasSdev or hasME) THEN PROB = 1 - IGAMMA((nElY-2)/2.0,CHISQR/2) ELSE BEGIN
        PROB = Y-Y+1
        fct = SQRT(CHISQR/(nElY-2))
        SIGMA = SIGMA*fct
     ENDELSE
; SIG_AB is an old name for sigma:
    SIG_AB=SIGMA

    AB = [A,B]
    RETURN, AB
END

