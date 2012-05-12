;+
;
; NAME: 
;     standardize
; PURPOSE: 
;     Standardizes/normalizes an array of m variables and n observations
;     (m by n).
;
; CATEGORY:
;     Mathematics: Statistics
;
; CALLING SEQUENCE:
;     Result = STANDARIZE(array)
;
; KEYWORD PARAMETERS:
;     DOUBLE: Keyword for double precision calculation
;     HELP: showing how to use and exit
;     TEST: stop just at the end, before returning
;
; OUTPUTS:
;     Result is a standarized/normalized array with a mean of zero and a
;     variance of 1.
;
; RESTRICTIONS:
;     The input array must be two dimensional numeric array
;
; EXAMPLE: see "testsuite/test_standarize,pro" in GDL CVS
;
;     (another exemple here:
;     http://idlastro.gsfc.nasa.gov/idl_html_help/STANDARDIZE.html) 
;     Define an array with 4 variables and 20 observations:  
;     array = FLTARR(4, 20)
;     array= [...]
;     result = STANDARIZE(array)
;     print, result
;
; MODIFICATION HISTORY:
; 15-Apr-2012: Written by Josh Sixsmith
; 13-May-2012: Alain: Correcting a sigm pb; Preparing for putting in GDL CVS ...
;
; LICENCE:
; Copyright (C) 2012, Josh Sixsmith
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 3 of the License, or     
; (at your option) any later version.                                   
;
;-
 
function STANDARDIZE, array, DOUBLE=dbl, test=test, help=help
;
ON_ERROR, 2
;
if KEYWORD_SET(help) then begin
   print, 'function STANDARDIZE, array, DOUBLE=dbl, test=test, help=help'
   print, 'array must be a 2D array'
   return, -1
endif
;
if (SIZE(array))[0] ne 2 then MESSAGE, 'Array needs to be 2 dimensional'
;
dbl = KEYWORD_SET(dbl)
;
dims = SIZE(array, /dimensions)
result = dbl ? dblarr(dims) : fltarr(dims)
;                             
; Uses the form for population (x - mu)/sigma
; Unsure whether the form should be for a sample:
; (xbar - mu)/(sigma/sqrt(n))
;
for i=0, dims[0]-1 do begin
   mom = MOMENT(array[i,*], sdev=stdv, Double = dbl)
   result[i,*] = -(mom[0] - array[i,*])/stdv
endfor
;
if KEYWORD_SET(test) then STOP
;
return, result
;
end
