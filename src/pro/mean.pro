;$Id: mean.pro,v 1.3 2012-07-13 22:28:02 alaingdl Exp $
;
function MEAN, x, double=double, NaN=NaN, dimension=dimension
;
;+
;
; NAME: MEAN
;
; PURPOSE: 
;     Calculates the mean of the input data
;       
; CATEGORY:
;     Mathematics: Statistics
;
; CALLING SEQUENCE:
;     Result=mean(x)
;
; KEYWORD PARAMETERS: 
;     DOUBLE : Keyword for double precision calculation
;     NAN    : Flag to treat IEEE Special Floating-Point values as missing data
;     DIMENSION : if absent or equal to zero, compute the variance over the
;                 whole data. otherwise, compute along the related dimension.
;
; OUTPUTS:
;    Result is the mean of input data
;
; RESTRICTIONS:
;    The input x needs to be an array of numbers (i.e not strings,
;    struct, ptr, object)
;
; PROCEDURE:
;     mean = 1/N sum(x)
;
; EXAMPLE:
;     a=FINDGEN(100)
;     result=MEAN(a)
;     print, result
;     49.5000
;
; MODIFICATION HISTORY:
;   20-Mar-2004 : Written by Christopher Lee
;   18-Jul-2005 : Rewritten by Pierre Chanial
;   13-Jul-2012 : Alain Coulais : adding DIMENSION keyword, using MOMENT()
;
; LICENCE:
; Copyright (C) 2004, Christopher Lee, 2005 P. Chanial, 2012 Alain Coulais
;
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;-
;
ON_ERROR, 2
;
; old version by PC, without Dimension
; ; we don't call moment.pro, since it requires 2 or more elements
; if keyword_set(NaN) then begin
;    n = total(finite(x), double=double)
; endif else begin
;    n = n_elements(x)
; endelse
; 
; mean = total(x, double=double, NaN=NaN)/n
; return, mean
;
tmp=MOMENT(x, mean=mean, double=double, NaN=NaN, $
           dimension=dimension, maxmoment=2)
;
return, mean
;
end
