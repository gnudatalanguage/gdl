;$Id: factorial.pro,v 1.1 2005-02-22 11:18:45 m_schellens Exp $
function factorial, n, stirling=stirling, ul64=ul64
;+
;
;
;
; NAME: Factorial
;
;
; PURPOSE: Calculates N!
;
;
;
; CATEGORY: Mathematics
;
;
; CALLING SEQUENCE: result=factorial(n)
;
; KEYWORD PARAMETERS: 
;             stirling : Use the Stirling approximation
;             ul64     : Use unsigned long 64 bit, ignored if stirling
;             is set
; OUTPUTS:
;              The factorial of n
;
;
; RESTRICTIONS:
;             IEEE floating point maximum number if 120!
;
; MODIFICATION HISTORY:
; 	Written by:	Christopher Lee 2004-12-10
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
t=size(n,/type)


integer = (t eq 2) or (t eq 3) or $
          (t eq 12) or (t eq 13) or $
          (t eq 14) or (t eq 15)

if(keyword_set(stirling)) then begin
    f=sqrt(2*!pi*n)*(n/exp(1))^n
endif else if(keyword_set(ul64)) then begin

f=1ULL
for i=2, ulong64(n)-1 do f=f*i
  
endif else begin
f=1.0d
    if(integer) then for i=2, n-1 do f=f*i else f=1.0d*gamma((n+1.)*1.0D)
endelse

return, f
end
