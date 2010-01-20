;$Id: poly.pro,v 1.2 2010-01-20 11:41:59 slayoo Exp $
;+
;
;
;
; NAME: POLY
;
;
; PURPOSE:  Calculates the polynomial of X where with coefficients C
; and order n_elements(c)-1
;
;
;
; CATEGORY: Algebra/ mathematics
;
;
; CALLING SEQUENCE: result= POLY(x,c)
;
;
; OUTPUTS: result : c[0] + c[1]*x + c[2]*x^2 + ....
;
; RESTRICTIONS: X and C should be numbers
;
; PROCEDURE: Starting from the highest order (N=n_max) exponent loop
; to the lowest (N=1) adding x[N] and multipying by c[N]
;
;
; EXAMPLE: x=[1,2,3]
;          c=[1,1,2]
;          print, poly(x,c)
;          4  11  22
;
;
; MODIFICATION HISTORY:
; 	Written by: Christopher Lee 2005-07-08	
;
;
;
;-
; LICENCE:
; Copyright (C) 2004,
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;
;-
function poly, x,c
  on_error, 2

  IF (N_PARAMS() NE 2) THEN BEGIN
    message, 'Incorrect number of arguments.'
  ENDIF

  nc=n_elements(c)

  p=x*0.
  for i=nc-1,1L,-1 do p=(p+c[i])*x
  p=p+c[0]
  return, p

end
