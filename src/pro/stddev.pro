;$Id: stddev.pro,v 1.1.1.1 2004-12-09 15:10:19 m_schellens Exp $

function stddev, x,double_keyword=double_keyword,nan=nan

;+
;
;
;
; NAME: 
;       stddev
;
; PURPOSE: 
;     Calculates the standard deviation of the input data
;       
;
; CATEGORY:
;     Mathematics: Statistics
;
; CALLING SEQUENCE:
;     Result=stddev(x)
;
;
; KEYWORD PARAMETERS: 
;     DOUBLE_KEYWORD : Keyword for double precision calculation
;     NAN    : Flag to ignore IEEE Floating point NaN
;
; OUTPUTS:
;    Result is the standard deviation of input data
;
;
; RESTRICTIONS:
;    The input x needs to be an array of numbers (i.e not strings,
;    struct, ptr, object)
;
; PROCEDURE:
;     standard deviation = sqrt(variance)
;     Uses the MOMENT function
;
; EXAMPLE:
;     a=findgen(100)
;     result=kurtosis(a)
;     print, result
;     29.0115
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



m=moment(x, nan=nan, double_keyword=double_keyword,sdev=sdev)

return, sdev

end
