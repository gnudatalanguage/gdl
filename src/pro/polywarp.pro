;
;
;
; NAME: POLYWARP
;
;
; PURPOSE: Performs polynomial spatial warping with given points.
;
;
; CATEGORY: Image Processing/ mathematics
;
;
; CALLING SEQUENCE: polywarp, Xi, Yi, Xo, Yo, Degree, Kx, Ky [, /DOUBLE] [, STATUS=variable]
;
;
; OUTPUTS: Estimated coefficients Kx[i,j] and Ky[i,j] of the polynomial functions
;                     Xi = Sum[i,j](Kx[i,j] * Xo^j * Yo^i)
;                     Yi = Sum[i,j](Ky[i,j] * Xo^j * Yo^i)
;
;
; RESTRICTIONS: Xi, Yi, Xo, and Yo should be number array with the same lengths.
;               The number of Xi and Yi should be greater or equal than (degree+1)^2.
;
;
; PROCEDURE: Estimates Kx[i,j] and Ky[i,j] using least eqares estimation.
;
;
; EXAMPLE:  Xi = [33, 53, 53, 56]
;           Yi = [34, 12, 123, 125]
;           Xo = [75, 21, 82, 67]
;           Yo = [89, 37, 89, 17]
;           POLYWARP, Xi, Yi, Xo, Yo, 1, Kx, Ky
;           print, Kx
;            227.312     -4.59099
;           -2.39530    0.0590162
;           print, Ky
;            628.383     -17.3927
;           -6.83405     0.219644
;
;
; MODIFICATION HISTORY:
; 	Written by: Jeongbin Park, 2015-Apr-05
;
;
;-
; LICENCE:
; Copyright (C) 2015, Jeongbin Park
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;-
pro polywarp, Xi, Yi, Xo, Yo, Degree, Kx, Ky, DOUBLE=isdouble, STATUS=status
  on_error, 2

  IF (N_PARAMS() LT 5) THEN BEGIN
    message, 'Incorrect number of arguments.'
  ENDIF

  n = n_elements(Xi)
  IF ((n NE n_elements(Yi)) OR (n NE n_elements(Xo)) OR (n NE n_elements(Yo))) THEN BEGIN
    message, 'Inconsistent number of elements.'
  ENDIF
  
  k = (Degree+1)^2
  IF (n LT k) THEN BEGIN
    message, '# of points must be ge (degree+1)^2.'
  ENDIF

  _X = DBLARR(n, k, /NOZERO)
  _Xi = DOUBLE(Xi)
  _Yi = DOUBLE(Yi)
  _Xo = DOUBLE(Xo)
  _Yo = DOUBLE(Yo)

  FOR i = 0, n-1 do begin
    FOR jj = 0, Degree do begin
      FOR ii = 0, Degree do begin
        _X[i, jj*(Degree+1)+ii] = _Xo[i]^jj*_Yo[i]^ii
      ENDFOR
    ENDFOR
  ENDFOR

  _trX = TRANSPOSE(_X)
  _Sol = INVERT(_trX#_X, STATUS, /DOUBLE)#_trX

  CASE STATUS OF
    1: MESSAGE, "Singular matrix detected.", /INFOMATIONAL
    2: MESSAGE, "Warning: Invert detected a small pivot element.", /INFOMATIONAL
    ELSE:
  ENDCASE

  IF (keyword_set(DOUBLE) OR (SIZE(Xi, /TN) EQ "DOUBLE") OR (SIZE(Yi, /TN) EQ "DOUBLE") OR $
                             (SIZE(Xo, /TN) EQ "DOUBLE") OR (SIZE(Yo, /TN) EQ "DOUBLE")) THEN BEGIN
    kX = DOUBLE(_Sol#_Xi)
    kY = DOUBLE(_Sol#_Yi)
  ENDIF ELSE BEGIN
    kX = FLOAT(_Sol#_Xi)
    kY = FLOAT(_Sol#_Yi)
  ENDELSE

  kX = REFORM(kX, Degree+1, Degree+1, /OVERWRITE)
  kY = REFORM(kY, Degree+1, Degree+1, /OVERWRITE)
end
