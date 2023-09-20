;+
; NAME:
;       GAUSSFIT_MPFIT
;
; PURPOSE:
;       replaces original gaussfit.pro using mpfitpeak free by Craig
;       B. Markwardt.
;
; COMMENT: (by AC, 2023/09/18) GAUSSFIT is an important feature,
;          we tried hard to have an efficient version using the
;          primitives provided by the GSL, which are commonly 10 time
;          faster than this version using MPFIT writtent in IDL/GDL
;          syntax. Nevertheless in some cases we saw that the MPFIT
;          version is more accurate or less sensible in tricks in the
;          data, then we consider this code should remain around.
;
; HISTORY:
; - AC 2023/09/18. Renamed from GAUSSFIT into GAUSSFIT_MPFIT
;
; CALLING SEQUENCE:
;
; Result = GAUSSFIT_MPFIT( X, Y [, A] [, CHISQ=variable] [,
;   [, ESTIMATES=array]$ [, MEASURE_ERRORS=vector]
;   [, NTERMS=integer{3 to 6}] [, SIGMA=variable] [, YERROR=variable]) 
;
; INPUTS:
;   X - Array of independent variable values, whose values should
;       monotonically increase.
;
;   Y - Array of "measured" dependent variable values.  Y should have
;       the same data type and dimension as X.
;
; NOTE: the following special cases apply:
;      * if Y is NaN or Infinite, and the NAN keyword is
;        set, then the corresponding data point is ignored
;
; OUTPUTS:
;   A - Upon return, an array of NTERMS best fit parameter values.
;       See the table above for the meanings of each parameter
;       element.
;-

FUNCTION GAUSSFIT_MPFIT, x, y, a, $
                         CHISQ=chisq, $
                         ESTIMATES = est, $
                         MEASURE_ERRORS=MEASURE_ERRORS, $
                         NTERMS=nterms, $
                         SIGMA=sigma, $
                         YERROR=yerror

compile_opt idl2, hidden
ON_ERROR,2                      ;Return to caller IF error

;; Use CURVEFIT instead of mpfitpeak at the end.
;; calling poly_fit more than once to simplify this code (things are fast)
;;
a=FLTARR(nterms)
;
; populate a with good default values.
;
temp=MPFITPEAK (x, y, a, nterms=nterms, /gauss)
;
;; get a better estimate by removing linear or square baseline
if nterms gt 3 then begin
   if N_ELEMENTS(est) eq 0 then begin
      c = POLY_FIT(x, y, (nterms eq 4) ? 0 : 1, yfit)
      ytemp = y - yfit
      temp=MPFITPEAK (x, ytemp, a, nterms=nterms, /gauss)
   endif
endif
;
if (N_ELEMENTS(measure_errors) gt 0) then weights = 1/measure_errors^2
  
yfit = CURVEFIT(x, y, weights, a, sigma, chisq=chisq, yerror=yerror, $
                function_name =  "gdl_gaussfunct")
return, yfit

end

