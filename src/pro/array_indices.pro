;$Id: array_indices.pro,v 1.1.1.1 2004-12-09 15:10:20 m_schellens Exp $

function Array_Indices,a,ix

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
;
;
;
;
;
; KEYWORD PARAMETERS: 
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
;
;
;-
; LICENCE:
; Copyright (C) 2004,
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;
;-

;; check number of parameters
if n_params() ne 2 then begin
    message,'ARRAY_INDICES: Wrong number of parameters.'
endif

;; check index type
ty = size( ix, /TYPE)
if ty ge 6 and ty le 11 then begin
    message,'ARRAY_INDICES: Index must be of integer type.'
endif

;; check index range
maxIx = max( ix, MIN=minIx)
if minIx lt 0 or maxIx ge n_elements(a) then begin
    message,'ARRAY_INDICES: Index out of range.'
endif

sz=size(a)

r=lonarr( sz[0]>1,n_elements(ix))

;; index 1
r[0,*]=ix mod sz[1]

;; index 2..n-1
sum=1L
for i=1,sz[0]-2 do begin
    sum=sum*sz[i]
    r[i,*]=(ix / sum) mod sz[i+1]
endfor

;; index n
if sz[0] ge 2 then begin
    i=sz[0]-1

    sum=sum*sz[i]
    r[i,*]=ix / sum
endif 

return,r
end



