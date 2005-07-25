;$Id: stddev.pro,v 1.2 2005-07-25 07:33:25 m_schellens Exp $

function stddev, x, double=double, NaN=NaN

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
;     DOUBLE : Keyword for double precision calculation
;     NAN    : Flag to treat IEEE Special Floating-Point values as missing data
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
;     result=stddev(a)
;     print, result
;     29.0115
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
 
 junk = moment(x, sdev=sdev, double=double, NaN=NaN)
 return, sdev

end
