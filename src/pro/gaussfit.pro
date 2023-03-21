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

compile_opt idl2, hidden
  ON_ERROR,2              ;Return to caller IF error

;; Use CURVEFIT instead of mpfitpeak at the end.
;; calling poly_fit more than once to simplify this code (things are fast)
  a=fltarr(nterms)
  temp=mpfitpeak (x, y, a,nterms=nterms, /gauss) ;; populate a with good default values.
;; get a better estimate by removing linear or square baseline
  if nterms gt 3 then begin
     if n_elements(est) eq 0 then begin
        c = poly_fit(x, y, (nterms eq 4) ? 0 : 1, yfit)
        ytemp = y - yfit
        temp=mpfitpeak (x, ytemp, a,nterms=nterms, /gauss)
     endif
  endif
  if (n_elements(measure_errors) gt 0) then weights = 1/measure_errors^2
  
  yfit = CURVEFIT(x,y,weights,a,sigma,  chisq=chisq, yerror=yerror, function_name =  "gdl_gaussfunct")
  return, yfit

 end

