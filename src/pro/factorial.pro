function FACTORIAL, input, stirling=stirling, ul64=ul64
;+
;
; NAME: Factorial
;
; PURPOSE: Calculates N!
;
; CATEGORY: Mathematics
;
; CALLING SEQUENCE: result=factorial(n)
;
; KEYWORD PARAMETERS: 
;       /stirling : Use the Stirling approximation
;       /ul64     : Use unsigned long 64 bit, ignored if stirling is set
;
; INPUTS: "input": can be a singleton or an array ...
;
; OUTPUTS:  The factorial of the "input"
;    - output types are:
;       -- UL64   if /ul64 set AND /Stirling not set
;       -- DOUBLE in all other cases
;
; EXAMPLES:
;      res=FACTORIAL(20)             --> Double
;      res=FACTORIAL(20,/ul64)       --> ul64
;      res=FACTORIAL(20.)            --> Double
;      res=FACTORIAL(20, /striling)  --> Double
;  res_vec=FACTORIAL([20,21,22])     --> Double Array
;  res_vec=FACTORIAL([20,21,22],/ul64)  --> Ulong64 Array
;
; RESTRICTIONS:
;             IEEE floating point maximum number if 120!
;
; MODIFICATION HISTORY:
; 	Written by:	Christopher Lee 2004-12-10
;       Corrections by : Alain Coulais 2008-04-17
;          -- see testsuite/test_factorial.pro
;          -- bad computation for INTEGER and /UL64 !!
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
;
ON_ERROR, 2
;
if (MIN(input) LT 0) then begin
   MESSAGE, 'Values in input array must be non-negative.'
endif
;
t=SIZE(input,/type)
;
integer = (t eq 2) or (t eq 3) or $
          (t eq 12) or (t eq 13) or $
          (t eq 14) or (t eq 15)
;
if KEYWORD_SET(stirling) then begin
   dbl_in=input+0.0D
   result=SQRT(2.0D*!dpi*dbl_in)*(dbl_in/EXP(1.0D))^dbl_in
endif else begin
   if (integer) then begin
      if (N_ELEMENTS(input) EQ 1) then begin
         result=1.0D
         for i=2, input[0] do result=result*i
      endif else begin
         dims=SIZE(input,/dim)
         result=DBLARR(dims,/nozero)
         for jj=0, N_ELEMENTS(input)-1 do begin
            current=input[jj]
            tmp=1.0D
            for i=2, current do tmp=tmp*i
            result[jj]=tmp
         endfor
      endelse
   endif else begin
      result=GAMMA(input+1.0D)
   endelse
   if KEYWORD_SET(ul64) then begin
      if (MAX(input) GT 20) then begin
         txt='% Program caused arithmetic error: Floating illegal operand'
         MESSAGE, txt, /continue
      endif
      result=ULONG64(result)
   endif   
endelse
;
return, result
;
end
