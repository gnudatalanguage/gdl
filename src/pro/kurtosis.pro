;$Id: kurtosis.pro,v 1.2 2005-07-25 07:33:25 m_schellens Exp $
function kurtosis, x, double=double, NaN=NaN

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
;     DOUBLE : Keyword for double precision calculation
;     NAN    : Flag to treat IEEE Special Floating-Point values as missing data
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
;     kurtosis = 1/N sum((x-mean(x))^4/sqrt(variance(x))) - 3,
;     Uses the MOMENT function
;
; EXAMPLE:
;     a=findgen(100)
;     result=kurtosis(a)
;     print, result
;     -1.23606
;
; MODIFICATION HISTORY:
;   20-Mar-2004 : Written by Christopher Lee
;   18-Jul-2005 : PC, moment.pro update
;
; LICENCE:
; Copyright (C) 2004, Christopher Lee
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;
;-

 on_error, 2
 
 m = moment(x, double=double, NaN=NaN)
 return, m[3]

end
