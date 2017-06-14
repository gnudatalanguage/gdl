;$Id: meanabsdev.pro,v 1.3 2017-06-14 16:09:04 gilles-duvert Exp $

function meanabsdev, x, double=double, NaN=NaN

;+
;
;
;
; NAME: 
;       meanabsdev
;
; PURPOSE: 
;     Calculates the mean absolute deviation of the input data
;       
;
; CATEGORY:
;     Mathematics: Statistics
;
; CALLING SEQUENCE:
;     Result=meanabsdev(x)
;
;
; KEYWORD PARAMETERS: 
;     DOUBLE : Keyword for double precision calculation
;     NAN    : Flag to ignore IEEE Floating point NaN
;
; OUTPUTS:
;    Result is the mean absolute deviation of input data
;
;
; RESTRICTIONS:
;    The input x needs to be an array of numbers (i.e not strings,
;    struct, ptr, object)
;
; PROCEDURE:
;     mean absolute deviation = (1/N) sum(abs(x-mean(x))),
;     Uses the MOMENT function
;
; EXAMPLE:
;     a=findgen(100)
;     result=meanabsdev(a)
;     print, result
;     25.0000
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
 
 junk = moment(x, maxmoment=2, mdev=mdev, double=double, NaN=NaN)
 return, mdev

end
