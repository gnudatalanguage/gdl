;$Id: moment.pro,v 1.1.1.1 2004-12-09 15:10:19 m_schellens Exp $
function moment, x,double_keyword=double_keyword,mdev=mdev,sdev=sdev,nan=nan

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
;     DOUBLE_KEYWORD : Keyword for double precision calculation
;     MDEV   : Named variable which will contain the mean absolute deviation
;     SDEV   : Named variable which will contain the standard deviation
;     NAN    : Flag to ignore IEEE Floating point NaN
;
; OUTPUTS:
;    Result is a 4 element array, with
;    [mean,variance,skewness,kurtosis] as the elements
;
;
;
; OPTIONAL OUTPUTS:
;    MDEV    : The mean absolute deviation 
;    SDEV    : Standard deviation
;
;
; RESTRICTIONS:
;    The input x needs to be an array of numbers (i.e not strings,
;    struct, ptr, object)
;
; PROCEDURE:
;     mean = (1/N)sum(x),
;     variance = 1/(N-1) sum((x-mean(x))^2), 
;     skewness = 1/(N) sum((x-mean(x))^3/sqrt(variance(x))), 
;     kurtosis  = 1/(N) sum((x-mean(x))^4/sqrt(variance(x))) -3, 
;     mean absolute deviation = (1/N) sum(abs(x-mean(x))),
;     standard deviation = sqrt(variance)
;
; EXAMPLE:
;     a=findgen(100)
;     result=moment(a)
;     print, a
;     49.5000    841.667     0.0000   -1.23606
;
; MODIFICATION HISTORY:
; 	Written by:  2004-03-20 Christopher Lee.
;
;
;
;-
; LICENCE:
; Copyright (C) 2004, Christopher Lee
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;
;-


n=n_elements(x)
;if(keyword_set(nan)) then begin
;    w=where(abs(x) ne !values.f_nan,c)
;    if(c eq 0) then begin
;        if(arg_present(mdev)) then mdev=!values.f_nan
;        if(arg_present(sdev)) then sdev=!values.f_nan
;        return, [!values.f_nan,!values.f_nan,!values.f_nan,!values.f_nan]
;    endif else if(x eq n)
;    if(keyword_set(double_keyword)) $
;      then xptr=ptr_new(double(x)) $
;    else $
;      xptr=ptr_new(x)
;
;    endif else begin
;    if(keyword_set(double_keyword)) $
;      then xptr=ptr_new(double(x[w])) $
;    else $
;      xptr=ptr_new(x[w])
;
;        n=c
;    endelse
;endif else begin
    if(keyword_set(double_keyword)) $
      then xptr=ptr_new(double(x)) $
    else $
      xptr=ptr_new(x)

;endelse

;xptr contains a point to the data, with no values.f_nan
;n contains the number of elements

mean=total(*xptr)/n
variance=(1./(n-1.)) * total((*xptr-mean)^2)
skewness=(1./n) * total((*xptr-mean)^3)/(sqrt(variance))^3
kurtosis=(1./n) * total((*xptr-mean)^4)/(sqrt(variance))^4 - 3.


if(keyword_set(mdev)) then mdev=(1./n) * total(abs(*xptr-mean))
if(keyword_set(sdev)) then sdev=sqrt(variance)

return, [mean, variance, skewness, kurtosis]

end

