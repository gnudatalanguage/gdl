;+
; NAME:
;       GAUSSFIT
;
; PURPOSE:
;       replaces original gaussfit.pro using mpfitpeak free by Craig
;       B. Markwardt.
; CALLING SEQUENCE:
; Result = GAUSSFIT( X, Y [, A] [, CHISQ=variable] [,
; ESTIMATES=array]$ [, MEASURE_ERRORS=vector] [, NTERMS=integer{3 to
; 6}] [, SIGMA=variable] [, YERROR=variable]) 
;
; INPUTS:
;   X - Array of independent variable values, whose values should
;       monotonically increase.
;
;   Y - Array of "measured" dependent variable values.  Y should have
;       the same data type and dimension as X.
;         NOTE: the following special cases apply:
;                * if Y is NaN or Infinite, and the NAN keyword is
;                  set, then the corresponding data point is ignored
;
; OUTPUTS:
;   A - Upon return, an array of NTERMS best fit parameter values.
;       See the table above for the meanings of each parameter
;       element.
;-
FUNCTION GAUSSFIT, x, y, a, $
    CHISQ=chisq, $
    ESTIMATES = est, $
    MEASURE_ERRORS=MEASURE_ERRORS, $
    NTERMS=nterms, $
    SIGMA=sigma, $
    YERROR=yerror

  ON_ERROR,2              ;Return to caller IF error

  return,mpfitpeak (x, y, a, estimates=est, nterms=nterms, /gauss, $
                      chisq=chisq, sigma=sigma, yerror=yerror,$
                      measure_errors=measure_errors, /nan)
END
