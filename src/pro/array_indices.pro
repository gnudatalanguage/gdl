;$Id: array_indices.pro,v 1.4 2015-12-02 20:46:44 gilles-duvert Exp $

function Array_Indices,a,ix,dimensions=dimensions
  on_error, 2
;+
;
;
;
; NAME:
; ARRAY_INDICES
;
; PURPOSE: 
; translates a one-dimensional index (like given by where() function)
; into a multidimensional one (i.e. the array indices according to
; the multidimensional array)
;
; PARAMETERS:
; a   the array
; ix  the onedimensional index (or array of indices)
;
; CATEGORY:
;
;
; CALLING SEQUENCE:
;  result = array_index(a,i)
;
;
;
;
; KEYWORD PARAMETERS: 
;   dimensions: with this keyword the array "a" is a vector giving the
;   dimensions. Use this keyword if you don't want to spoil the
;   memory to generate a sample array "a" with the output dimensions.
;
;
; OUTPUTS:
;
;
;
;
; OPTIONAL OUTPUTS:
;
;
;
; COMMON BLOCKS:
;
;
; SIDE EFFECTS:
;
;
; RESTRICTIONS:
;
;
; PROCEDURE:
;
;
; EXAMPLE:
; GDL> a=intarr(23,24,27,33)
; GDL> a[13,19,2,11]=1 
; GDL> ix=where(a) 
; GDL> print,l_getdim(a,ix)
;          13          19           2          11 
;
;
;
; MODIFICATION HISTORY:
; 	Written by: Marc Schellens, 02-12-2004	
;       introduces dimensions keyword by Reto Stockli (2015/11/29)
;
;
;-
; LICENCE:
; Copyright (C) 2004-2015,
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;
;-

;; check number of parameters
if n_params() ne 2 then begin
    message,'Wrong number of parameters.'
endif

;; check index type
ty = size( ix, /TYPE)
if ty ge 6 and ty le 11 then begin
    message,'Index must be of integer type.'
endif

;; get index range
maxIx = max( ix, MIN=minIx)

;; check if the array "a" provides the dimensions
;; and evaluate dimensions
if (keyword_set(dimensions)) then begin
   ntot = product(a,/integer)
   dim = a
   ndim = n_elements(a)
endif else begin
   ntot = n_elements(a)
   dim = size(a,/dimensions)
   ndim = n_elements(dim)
endelse

if minIx lt 0 or maxIx ge ntot then begin
    message,'Index out of range.'
endif

r=lonarr( ndim>1,n_elements(ix))

;; index 1
r[0,*]=ix mod dim[0]

;; index 2..n-1
sum=1L
for i=1,ndim-2 do begin
    sum=sum*dim[i-1]
    r[i,*]=(ix / sum) mod dim[i]
endfor

;; index n
if ndim ge 2 then begin
    i=ndim-1

    sum=sum*dim[i-1]
    r[i,*]=ix / sum
endif 

return,r
end



