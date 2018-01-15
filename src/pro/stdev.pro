;
function STDEV, input, mean, test=test, help=help
;
;+
;
; NAME: STDEV
;
; PURPOSE: 
;     Calculates the standard deviation of the input data, and
;     optionnaly the mean value
;       
; CATEGORY:
;     Mathematics: Statistics
;
; CALLING SEQUENCE: Result=STDEV(input, mean)
;
; KEYWORD PARAMETERS: specific to GDL : /help & /test
;
; OUTPUTS: Result is the standard deviation of input data
;
; RESTRICTIONS: The input needs to be an array of numbers
;               (i.e not strings, struct, ptr, object)
;
; PROCEDURE:
;     standard deviation = sqrt(variance)
;     Uses the MOMENT function
;
; EXAMPLE:
;     a=FINDGEN(100)
;     result=STDEV(a, mean)
;     print, result, mean
;     29.0115, 49.5000
;
; MODIFICATION HISTORY:
;   2018-Jan-15 : Alain Coulais : creation, because used in EXOFAST code ...
;
; LICENCE:
; Copyright (C) 2018 Alain Coulais
;
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;-
;
ON_ERROR, 2
;
if KEYWORD_SET(help) then begin
   print, 'function STDEV, input, mean, test=test, help=help'
   return, -1
endif
;
if ~ISA(input, /number) then MESSAGE, 'Input type is not number'
;
nbp=N_ELEMENTS(input)
if (nbp LT 2) then MESSAGE, 'Number of data points must be > 1'
;
mean=TOTAL(input)/nbp
stddev=SQRT(TOTAL((input-mean)^2)/(nbp-1))
;
if KEYWORD_SET(test) then STOP
;
return, stddev
;
end
