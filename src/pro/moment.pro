;$Id: moment.pro,v 1.3 2009-08-11 08:53:05 slayoo Exp $
function moment, x, mdev=mdev, sdev=sdev, double=double, NaN=NaN, $
  maxmoment=maxmoment

;+
;
;
;
; NAME: 
;       moment
;
; PURPOSE: 
;     Calculates the following from the input data; mean, variance,
;     skewness, kurtosis, mean absolute deviation, 
;     standard deviation
;       
;
; CATEGORY:
;     Mathematics: Statistics
;
; CALLING SEQUENCE:
;     Result=moment(x)
;
;
; KEYWORD PARAMETERS: 
;     DOUBLE    : Keyword for double precision calculation
;     NAN       : Flag to treat IEEE Special Floating-Point values as missing data
;     MAXMOMENT : Keyword for precising which moments are to be calculated
;                 1 - calculate mean 
;                 2 - calculate mean, variance, mean absolute deviation and standard dev.
;                 3 - calculate all but kurtosis
;                 4 or 0 (keyword not present) - calculate all moments
;
; OUTPUTS:
;    Result is a 4 element array, with
;    [mean,variance,skewness,kurtosis] as the elements
;
; OPTIONAL OUTPUTS:
;     MDEV   : Named variable which will contain the mean absolute deviation
;     SDEV   : Named variable which will contain the standard deviation
;
;
; RESTRICTIONS:
;    The input x needs to be an array of numbers (i.e not strings,
;    struct, ptr, object)
;
; PROCEDURE:
;     mean = (1/N)sum(x),
;     variance = 1/(N-1) sum((x-mean(x))^2), 
;     skewness = 1/N sum((x-mean(x))^3/sqrt(variance(x))), 
;     kurtosis = 1/N sum((x-mean(x))^4/sqrt(variance(x))) - 3, 
;     mean absolute deviation = 1/N sum(abs(x-mean(x))),
;     standard deviation = sqrt(variance)
;
; EXAMPLE:
;     a=findgen(100)
;     result=moment(a)
;     print, result
;     49.5000    841.667     0.0000   1.73395
;
; MODIFICATION HISTORY:
;   20-Mar-2004 : Written by Christopher Lee
;   18-Jul-2005 : Rewritten by Pierre Chanial
;   10-Aug-2009 : MAXMOMENT keyword added by Sylwester Arabas
;
;
; LICENCE:
; Copyright (C) 2004, Christopher Lee
;               2005, Pierre Chanial
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;
;-

 on_error, 2
 
 if n_elements(x) le 1 then begin
    ;message, 'Input Array must contain 2 OR more elements.'
    return, [x[0], !VALUES.F_NAN, !VALUES.F_NAN, !VALUES.F_NAN]
 endif
 
 ; we don't reuse code in mean.pro, because we need variable n.
 if keyword_set(NaN) then begin
    n = total(finite(x), double=double)
 endif else begin
    n = n_elements(x)
 endelse
 
 ; get the mean value in the required type (FLOAT or DOUBLE)
 ; subsequent operations will rely on GDL automatic type conversion
 mean = total(x, double=double, NaN=NaN)/n
 x0   = x-mean
 
 if ~keyword_set(maxmoment) then maxmoment = 4
 variance = maxmoment ge 2 ? total(x0^2, NaN=NaN)/(n-1)        : !VALUES.F_NAN
 sdev     = maxmoment ge 2 ? sqrt(variance)                    : !VALUES.F_NAN 
 skewness = maxmoment ge 3 ? total(x0^3, NaN=NaN)/sdev^3/n     : !VALUES.F_NAN
 kurtosis = maxmoment ge 4 ? (total(x0^4, NaN=NaN)/sdev^4-3)/n : !VALUES.F_NAN

 if arg_present(mdev) then begin
   mdev = maxmoment ge 2 ? total(abs(x0), NaN=NaN)/n           : !VALUES.F_NAN         
 endif
  

 return, [mean, variance, skewness, kurtosis]

end
