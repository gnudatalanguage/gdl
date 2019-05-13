;+
; NAME:
;       CURVEFIT
;
; PURPOSE:
;       replaces original curvefit.pro using mpcurvefit free by Craig
;       B. Markwardt.
; CALLING SEQUENCE:
;       Result =  CURVEFIT( X, Y, Weights, A [, Sigma] [,
;       CHISQ=variable] [, /DOUBLE] [, FITA=vector] [,
;       FUNCTION_NAME=string] [, ITER=variable] [, ITMAX=value] [,
;       /NODERIVATIVE] [, STATUS={0 | 1 | 2}] [, TOL=value] [,
;       YERROR=variable] )
;
;-
FUNCTION CURVEFIT, x, y, w, p, sigma, FUNCTION_NAME = fcn, FITA=fita, $
                   ITMAX=maxiter, ITER=iter, TOL=tol, $
                   NODERIVATIVE=noderivative, $
                   CHISQ=bestnorm, $
                   DOUBLE=double, YERROR=yerror, CHI2=chi2,$
                   STATUS=status

;  ON_ERROR,2              ;Return to caller IF error

  if n_elements(fcn) eq 0 then fcn="FUNCT" 
  if n_elements(w) eq 0 then w=x*0+1. 
  if (n_elements(fita) gt 0) then begin
     parinfo=replicate({FIXED:0},7) ; logic is reverse from idl...
     ww=where(fita eq 0, count)
     if (count gt 0) then parinfo.fixed[ww]=1
     yfit = mpcurvefit (x, y, w, p, sigma, function_name=fcn, $
                        iter=iter, itmax=maxiter, $
                        chisq=bestnorm, $
                        /nocovar, yerror=yerror, $
                        noderivative=noderivative, ftol=tol, $
                        STATUS=status, _EXTRA=extra )
  endif else yfit = mpcurvefit (x, y, w, p, sigma, function_name=fcn, $
                                iter=iter, itmax=maxiter, $
                                chisq=bestnorm, $
                                /nocovar, yerror=yerror, $
                                noderivative=noderivative, ftol=tol, $
                                STATUS=status, _EXTRA=extra )
  chi2 = bestnorm         ; Return chi2 obsolete
  return, yfit
END
