;+
; NAME:
;       GAUSS2DFIT
;
; PURPOSE:
;       replaces original gauss2dfit.pro using mpfit2dpeak free by Craig
;       B. Markwardt.
; CALLING SEQUENCE:
; Result = GAUSS2DFIT( Z, A [, X, Y] [, FITA=vector] [, MASK=array] [,
; /NEGATIVE] [, /TILT] ) 
;
; INPUTS:
;
;   Z - Two dimensional array of "measured" dependent variable values.
;       Z should be of the same type and dimension as (X # Y).
;
;   X - Optional vector of x positions for a single row of Z.
;
;          X[i] should provide the x position of Z[i,*]
;
;       Default: X values are integer increments from 0 to NX-1
;
;   Y - Optional vector of y positions for a single column of Z.
;
;          Y[j] should provide the y position of Z[*,j]
;
;       Default: Y values are integer increments from 0 to NY-1
;
; OUTPUTS:
;   A - Upon return, an array of best fit parameter values.  See the
;       table above for the meanings of each parameter element.
;
;
; RETURNS:
;
;   Returns the best fitting model function as a 2D array.
;-
Function GAUSS2DFIT, Z, A, X, Y, FITA=FITA, MASK=MASK, NEGATIVE = NEG, TILT=TILT

  ON_ERROR,2              ;Return to caller IF error
  if (n_elements(fita) gt 0) then begin
     parinfo=replicate({FIXED:0},7) ; logic is reverse from idl...
     ww=where(fita eq 0, count) & if (count gt 0) then parinfo.fixed[ww]=1
     return, mpfit2dpeak( /gauss, z, a, x, y, NEGATIVE = NEG, TILT=TILT,  parinfo=parinfo) 

  endif else return, mpfit2dpeak( /gauss, z, a, x, y, NEGATIVE = NEG, TILT=TILT)

END
