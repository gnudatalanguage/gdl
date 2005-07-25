;$Id: moment.pro,v 1.2 2005-07-25 07:33:25 m_schellens Exp $
function moment, x, mdev=mdev, sdev=sdev, double=double, NaN=NaN

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
;     DOUBLE : Keyword for double precision calculation
;     NAN    : Flag to treat IEEE Special Floating-Point values as missing data
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
;     print, a
;     49.5000    841.667     0.0000   -1.23606
;
; MODIFICATION HISTORY:
;   20-Mar-2004 : Written by Christopher Lee
;   18-Jul-2005 : Rewritten by Pierre Chanial
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
 
 if n_elements(x) le 2 then begin
    message, 'Input Array must contain 2 OR more elements.'
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
 
 variance = total(x0^2, NaN=NaN)/(n-1)
 sdev     = sqrt(variance)
 skewness = total(x0^3, NaN=NaN)/sdev^3/n
 kurtosis = (total(x0^4, NaN=NaN)/sdev^4-3)/n

 if arg_present(mdev) then mdev = total(abs(x0), NaN=NaN)/n

 return, [mean, variance, skewness, kurtosis]

end
