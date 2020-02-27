function SIGMA,x,npar,dim
;
;+
;
; NAME: SIGMA
;
; PURPOSE: 
;     Calculates the standard deviation of the input data x,
;     optionally giving the number of parameters npar (the degree of
;     freedom is n_elements(x)-npar). Can be done over
;     dimension 'dim', if present.
;
; WARNING: OBSOLETE, and further, there is a discrepancy of the order
; of sqrt(M/M-1) where M is the number of elements, between SIGMA and
; SDEV= option of MOMENT.
;  
;       
; CATEGORY:
;     Mathematics: Statistics
;
; CALLING SEQUENCE:
;     Result=SIGMA(x,[npar,[dim]])
;
;
; OUTPUTS:
;    Result is the standard deviation of input data
;
; RESTRICTIONS:
;    The input x needs to be an array of numbers (i.e not strings,
;    struct, ptr, object)
;
; PROCEDURE:
; given n=n_elements of x   
;     sigma = ; see above comments on how SIGMA may be false.
;     stddev(x)*sqrt(n^2/((n-1)*((n- npar) > 1.)) )
;     the sqrt(n/n-1) is to mimic the IDL obsolete function of course.
;     uses the MOMENT function
;
; EXAMPLE:
;     a=FINDGEN(100)
;     result=SIGMA(a,10)
;     print, result
;     30.4275
;
; MODIFICATION HISTORY:
;   27-Feb-2019 : Gilles Duvert
;
; LICENCE:
; Copyright (C) 2019, Gilles Duvert
;
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;-
;
COMPILE_OPT strictarr,hidden
ON_ERROR, 2
;
sz=size(x) & if sz[0] eq 0 then begin
   message,/cont,'variable X must be an array!'
   return, x
endif
if n_elements(dim) eq 0 then dim=0
if N_PARAMS(0) LE 2 or dim eq 0 then n=n_elements(x) else begin
   if (dim ge 1 and dim le sz[0]) then n=n_elements(x)/sz[dim+1] else message,"Bad value for dimension (3rd arg of OBSOLETE SIGMA procedure)."
endelse

corr=sqrt((n-1.)/n)

tmp=MOMENT(x, sdev=sdev, dimension=dim, maxmoment=2)

if N_PARAMS(0) LT 2 then return, sdev*corr
corr=sqrt( (n-1.) / ((n - npar) > 1.) )
return, sdev*corr
;
end
