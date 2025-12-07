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
; ---------------------------
; 
; Modification history:
;
; - 2025-Dec-04 : AC. check size of the 2 input vectors
;   Promote into Double the 2 input vectors
;
;-
; --------------------------------
; Polynomial Function
;
PRO GDL_MPFIT_POLYNOMIAL, x, p, ymod, dp, _extra=extra
;  
COMPILE_OPT strictarr,hidden
ndeg=extra.degree
ymod=x*0+p[0]
for i = 1, ndeg do ymod+=p[i]*x^i
;
end
;
; --------------------------------
;
FUNCTION POLY_FIT, x_in, y_in, ndegree, $
                   oldyfit, oldyband, yerror_old, corrm, $ ; obsolete arguments
                   CHISQ=chisq, $
                   COVAR=covar, $
                   DOUBLE=double, $
                   MEASURE_ERRORS=measure_errors, $
                   SIGMA=sigma, $
                   STATUS=status, $
                   YBAND=yband, $
                   YERROR=yerror, $
                   YFIT=yfit
;
COMPILE_OPT strictarr, hidden
ON_ERROR,2
;
txt='X and Y must have same number of elements.'
if N_ELEMENTS(x_in) NE N_ELEMENTS(y_in) then MESSAGE, txt
;
; AC 2025-12-04 : based on Zanardo data,
; it is better to ensure the type of inputs to be double ...
;
x=DOUBLE(x_in)
y=DOUBLE(y_in)
;
farg={DEGREE:ndegree}
; test if yband is wanted...
doyband=ARG_PRESENT(yband) || arg_present(oldyband)
; test if measure_errors were given
haserrors=ARG_PRESENT(measure_errors)
;
if (ndegree lt 1) then begin
   m=N_ELEMENTS(x)
   p=MEAN(y)
   sigma=(sigma(y))[0]
   dummy=MOMENT(y,sdev=yerror)  ; get yerror
   yfit=p
   measure_errors=REPLICATE(p,m)
   covar=TOTAL((y-p)*(x-MEAN(x)))/(m-1)
   chisq=TOTAL((y-p)^2/p)
   goto, near_end
endif
;
; weights w are based on measure_errors:
if (N_ELEMENTS(measure_errors) gt 0) then w=1D/measure_errors^2 else w=x*0+1.
p=DBLARR(ndegree+1)+1.0
;;
yfit=MPCURVEFIT(x, y, w, p, sigma, function_name="GDL_MPFIT_POLYNOMIAL", $
                functargs=farg, $
                chisq=chisq, $
                covar=covar, yerror=yerror, $
                STATUS=status, /noderivative, /quiet, _EXTRA=extra )
; 
; compute backprojection of correlation matrix on individual point's 
if (doyband) then begin
   xx = x*0+1
   yband = x*0+covar[0,0]
   FOR k=1L,2*ndegree DO BEGIN
      xx *= x                   ; x^1, x^2 ...                    
      sum = 0
      for j=0 > (k - ndegree), ndegree<k do sum += covar[j,k-j]
      yband += sum * xx
   ENDFOR
   ;; special case for no MEASURE_ERRORS, as per the documentation
   if (~haserrors) then yband *= yerror^2
   yband = SQRT(TEMPORARY(yband) )
endif
near_end:
; obsolete arguments?
if (n_params() ge 4) then begin
   corrm = covar*yerror^2       ; was a correlation matrix
   oldyerror = yerror
   oldyfit = yfit
   if (doyband) then oldyband = yband
ENDIF
;
return, TRANSPOSE(p)
;
END
