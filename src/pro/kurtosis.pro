;$Id: kurtosis.pro,v 1.1.1.1 2004-12-09 15:10:19 m_schellens Exp $

function kurtosis, x, double_keyword=double_keyword,nan=nan

;+
;
;
;
; NAME: 
;       kurtosis
;
; PURPOSE: 
;     Calculates the kurtosis of the input data
;       
;
; CATEGORY:
;     Mathematics: Statistics
;
; CALLING SEQUENCE:
;     Result=kurtosis(x)
;
;
; KEYWORD PARAMETERS: 
;     DOUBLE_KEYWORD : Keyword for double precision calculation
;     NAN    : Flag to ignore IEEE Floating point NaN
;
; OUTPUTS:
;    Result is the mean of input data
;
;
; RESTRICTIONS:
;    The input x needs to be an array of numbers (i.e not strings,
;    struct, ptr, object)
;
; PROCEDURE:
;     kurtosis  = 1/(N) sum((x-mean(x))^4/sqrt(variance(x))) -3, 
;     Uses the MOMENT function
;
; EXAMPLE:
;     a=findgen(100)
;     result=kurtosis(a)
;     print, result
;     -1.23606
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

m=moment(x, nan=nan, double_keyword=double_keyword)

return, m[3]

end
