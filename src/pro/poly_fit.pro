;+
; NAME:
;       POLY_FIT
;
; PURPOSE:
;       replaces original poly_fit.pro using mpcurvefit free by Craig
;       B. Markwardt.
;
; CALLING SEQUENCE:
; Result = POLY_FIT( X, Y, Degree [, CHISQ=variable] [,
; COVAR=variable] [, /DOUBLE] [, MEASURE_ERRORS=vector] [,
; SIGMA=variable] [, STATUS=variable] [, YBAND=variable] [,
; YERROR=variable] [, YFIT=variable] ) 
;
;
; INPUTS:
;   X:  The independent variable vector.
;
;   Y:  The dependent variable vector (same length as x).
;
;   Degree: The degree of the polynomial to fit.
;
; OUTPUTS:
;   a vector of coefficients of length = DEGREE+1.
;
;-
; Polynomial Function
PRO gdl_mpfit_polynomial, x, p, ymod, dp, _extra=extra
  
  COMPILE_OPT strictarr,hidden
  ndeg=extra.degree
  ymod=x*0+p[0]
  for i = 1, ndeg do ymod+=p[i]*x^i 
end

FUNCTION POLY_FIT, x, y, ndegree, $
    oldyfit, oldyband, yerror_old, corrm, $     ; obsolete arguments
    CHISQ=chisq, $
    COVAR=covar, $
    DOUBLE=double, $
    MEASURE_ERRORS=measure_errors, $
    SIGMA=sigma, $
    STATUS=status, $
    YBAND=yband, $
    YERROR=yerror, $
    YFIT=yfit

  COMPILE_OPT strictarr
  
  if (ndegree lt 1) then message,"polynomial degree should be > 0" 
  farg={DEGREE:ndegree}
; test if yband is wanted...
  doyband=arg_present(yband) || arg_present(oldyband)
; test if measure_errors were given
  haserrors=arg_present(measure_errors)
; weights w are based on measure_errors:
  if (n_elements(measure_errors) gt 0) then w=1D/measure_errors^2 else w=x*0+1
  p=dblarr(ndegree+1)+1.0
  yfit=mpcurvefit(x, y, w, p, sigma, function_name="GDL_MPFIT_POLYNOMIAL", $
                  functargs=farg, $
                  chisq=chisq, $
                  covar=covar, yerror=yerror, $
                  STATUS=status,/noderivative, _EXTRA=extra, /quiet )
  ; compute backprojection of correlation matrix on individual point's 
  if (doyband) then begin
     xx = x*0+1
     yband = x*0+covar[0,0]
     FOR k=1L,2*ndegree DO BEGIN
        xx *= x ; x^1, x^2 ...                    
        sum = 0
        for j=0 > (k - ndegree), ndegree<k do sum += covar[j,k-j]
        yband += sum * xx
     ENDFOR
     
  ; special case for no MEASURE_ERRORS, as per the documentation
     if (~haserrors) then yband *= yerror^2
     
     yband = sqrt( temporary(yband) )
  endif

; obsolete arguments?
    if (n_params() ge 4) then begin
        corrm = covar*yerror^2   ; was a correlation matrix
        oldyerror = yerror
        oldyfit = yfit
        if (doyband) then oldyband = yband
    ENDIF
  
  return,p
END
