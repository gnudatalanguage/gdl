;$Id: skewness.pro,v 1.2 2005-07-25 07:33:25 m_schellens Exp $
function skewness, x, double=double, NaN=NaN

;+
;
;
;
; NAME: 
;       skewness
;
; PURPOSE: 
;     Calculates the skewness of the input data
;       
;
; CATEGORY:
;     Mathematics: Statistics
;
; CALLING SEQUENCE:
;     Result=skewness(x)
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
;     skewness = 1/N sum((x-mean(x))^3/sqrt(variance(x))), 
;     Uses the MOMENT function
;
; EXAMPLE:
;     a=findgen(100)
;     result=mean(a)
;     print, result
;     0.0000
;
; MODIFICATION HISTORY:
;   20-Mar-2004 : Written by Christopher Lee
;   18-Jul-2005 : PC, moment.pro update
;
; LICENCE:
; Copyright (C) 2004,
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;
;-

 on_error, 2
 
 m = moment(x, double=double, NaN=NaN)
 return, m[2]

end
