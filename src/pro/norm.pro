;+
; NAME: NORM
;
; PURPOSE:
;       For vectors, returns a norm 
;       For matrix, returns the L0, L1 or L2 norm (only)
;
; CATEGORY:
;       Algebra
;
; CALLING SEQUENCE:
;       result=norm(a [, /double] [, /lnorm] )
;
; INPUTS:
;       a       Vector or Matrix (can be complex)
;
; OUTPUTS:
;       result  Norm (see Purpose)
;
; IDL DIFFERENCES:
;       scalars are treated as vectors
;       double=0 does not convert the result to float (like total)
;
; MODIFICATION HISTORY:
;  12-Jan-2006 : written by Pierre Chanial
;  12-Jul-2017 : AlainC., except the case (Lnorm=2 & matrix & Complex
;  input), try to be complete.
;
; LICENCE:
; Copyright (C) 2006, P. Chanial, 2017, A. Coulais
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;-
;
function NORM, array, double=double, lnorm=lnormOut, test=test, help=help
;
ON_ERROR, 2
;
if KEYWORD_SET(help) then begin
   print, "function NORM, array, double=double, lnorm=lnormOut, $"
   print, "               test=test, help=help"
   return, -1
endif
;
type=SIZE(array, /type)
ndim=SIZE(array, /n_dimensions)
;
if ((ndim LT 1) OR (ndim GT 2)) then begin
   MESSAGE, 'Input must be an N-element vector or an M by N array.'
endif
;
; "lnorm" value must be carefuly processed ...
; single element of numerical type but not complex
;
if (N_ELEMENTS(lnormOut) GT 0) then begin
   if (N_ELEMENTS(lnormOut) GT 1) then $
      MESSAGE, 'LNORM must be a scalar or a 1-element array.'
   lnorm=lnormOut[0]
endif else begin
   if (ndim EQ 1) then lnorm=2
   if (ndim EQ 2) then lnorm=0
endelse
; "lnorm" must be numeric : integer or float but no complex
if ~(ISA(lnorm,/integer) OR ISA(lnorm,/float)) then $
   MESSAGE,/continue, 'bad type for LNORM, converted into Double'
;
; "lnorm" must be a double
lnorm=DOUBLE(lnorm)
;
if KEYWORD_SET(test) then $
   print, "input norm value, used norm value : ", lnormout, lnorm
;
; Do we have NaN in input array ?? O if no NaN, 1 otherwise
nan=1-ARRAY_EQUAL(FINITE(array), 1)
;
; First we process the Vector case (ndim == 1),
; no peculiar problem :)
;
if (ndim EQ 1) then begin
   case lnorm of
      ;; infinity norm
      0: result=MAX(ABS(array), NAN=nan)
      ;; L(1) norm
      1: result=TOTAL(ABS(array), /DOUBLE, NAN=nan)
      ;; L(2) norm (Euclidean)
      2: result=SQRT(TOTAL(ABS(array)^2d, /DOUBLE, NAN=nan))
      ;; General L(n) norm (tested with negatives values)
      ;; print, norm(findgen(10)+1, lnorm=-10,/doubl)
      ;; 0.99990060
      else: result=(TOTAL(ABS(array)^lnorm, /DOUBLE, NAN=nan))^(1/lnorm)
   endcase     
endif
;
if (ndim EQ 2) then begin
   case lnorm of
      ;; infinity norm (maximum absolute *row* sum norm)
      0: result=MAX(TOTAL(ABS(array), 1, /DOUBLE, NAN=nan))
      ;; L(1) norm (maximum absolute *column* sum norm)
      1: result=MAX(TOTAL(ABS(array), 2, /DOUBLE, NAN=nan))
      ;; L(2) norm (spectral norm)
      2: begin
         ;; we are not ready for Complex inputs in GDL
         if ISA(array, /complex) then begin
            MESSAGE, /continue, 'Complex Input not ready in GDL !'
            MESSAGE, 'please contribute : LA_SVD missing :('
         endif else begin
            SVDC, array, W, U, V, /DOUBLE
            result = MAX(W)
         endelse
      end
      ;; no other case for Matrix
      else: MESSAGE, 'For MATRIX input, LNORM must be equal to 0, 1, or 2'
      endcase
endif
;
; Which type for the result ? Float or Double !
; Default is "float"
result_type=4
if KEYWORD_SET(double) then result_type=5
if (type EQ 5) OR (type EQ 9) then result_type=5
;
if KEYWORD_SET(test) then STOP
;
return, FIX(result, TYPE=result_type)
;
end
