;$Id: mean.pro,v 1.2 2005-07-25 07:33:25 m_schellens Exp $
function mean, x, double=double, nan=nan

;+
;
;
;
; NAME: 
;       mean
;
; PURPOSE: 
;     Calculates the mean of the input data
;       
;
; CATEGORY:
;     Mathematics: Statistics
;
; CALLING SEQUENCE:
;     Result=mean(x)
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
;     mean = 1/N sum(x)
;
; EXAMPLE:
;     a=findgen(100)
;     result=mean(a)
;     print, result
;     49.5000
;
; MODIFICATION HISTORY:
;   20-Mar-2004 : Written by Christopher Lee
;   18-Jul-2005 : Rewritten by Pierre Chanial
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
 
 ; we don't call moment.pro, since it requires 2 or more elements
 if keyword_set(NaN) then begin
    n = total(finite(x), double=double)
 endif else begin
    n = n_elements(x)
 endelse
 
 mean = total(x, double=double, NaN=NaN)/n
 return, mean

end
