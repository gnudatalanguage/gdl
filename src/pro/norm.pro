;+
; NAME: NORM
;
; PURPOSE:
;       For vectors, returns euclidean norm.
;       For matrix, returns the infinity norm
;
;
; CATEGORY:
;       Algebra
;
;
; CALLING SEQUENCE:
;       result=norm(a)
;
;
; INPUTS:
;       a       Vector or Matrix (can be complex)
;
;
; OUTPUTS:
;       result  Norm (see Purpose)
;
;
; IDL DIFFERENCES:
;       scalars are treated as vectors
;       double=0 does not convert the result to float (like total)
;
;
; MODIFICATION HISTORY:
;   12-Jan-2006 : written by Pierre Chanial
;
; LICENCE:
; Copyright (C) 2006, P. Chanial
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;-


function norm, array, double=double
 on_error, 2
 
 dim = size(array, /n_dimensions)
 
 switch dim of
    0 :
    1 : return, sqrt(total(abs(array)^2, double=double, /NaN))
    2 : return, max(total(abs(array), 1, double=double, /NaN))
    else : message, 'Input must be a vector or a matrix.'
 endswitch
 
end
