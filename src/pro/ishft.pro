function ishft, p1, n, _extra=_extra 
  on_error, 2
;+ 
; 
; 
; 
; NAME: 
; ISHFT 
; 
; PURPOSE: 
; Replacement for IDL intrisic function ISHFT 
; 
; PARAMETERS: 
; p1  byte, int, long integer to shift 
; n   # of bit positions to shift 
;     (+) left-shifted 0 bits fill 
;     (-) right-shifted 0 bits fill 
; CATEGORY: 
; 
; 
; CALLING SEQUENCE: 
; 
; result = ISHFT(p1, n) 
; 
; 
; 
; KEYWORD PARAMETERS: 
; 
; 
; OUTPUTS: 
; 
; 
; 
; 
; OPTIONAL OUTPUTS: 
; 
; 
; 
; COMMON BLOCKS: 
; 
; 
; SIDE EFFECTS: 
; 
; 
; RESTRICTIONS: 
; 
; 
; PROCEDURE: 
; 
; 
; EXAMPLE: 
; GDL> a = ishft(434,-2) 
; GDL> print,a 
;     108 
; 
; GDL> a = ishft(434,+2) 
; GDL> print,a 
;    1736 
; 
; 
; 
; MODIFICATION HISTORY: 
;       Written by: Richard Schwartz, 09-28-2006 
;	Revised 12-20-2006, Richard Schwartz - Use out[*] to fill final array instead 
;	of out[0] 
; 
; 
;- 
; LICENSE: 
; Copyright (C) 2006, 
; This program is free software; you can redistribute it and/or modify 
; it under the terms of the GNU General Public License as published by 
; the Free Software Foundation; either version 2 of the License, or 
; (at your option) any later version. 
; 
; 
;- 

IF (N_PARAMS() NE 2) THEN BEGIN
   message, 'Incorrect number of arguments.'
ENDIF
 
x = p1 
sizen= size(/struct, n) 
numn   = sizen.n_elements 
sizex= size(/struct, x) 
case sizex.type of 
   1: x=x 
   2: x=uint(x) 
   3: x=ulong(x) 
   else: x=ulong64(x) 
   endcase 
 
scalar = sizex.n_dimensions eq 0 
out    = make_array(dim =sizex.dimensions>1, type=sizex.type ) 
 
h = histogram( [n]>(-1)<0, min=-1, max=.1,r=r) 
; 
; Divide them into shift left and shift right 
if h[0] gt 0 then begin ; negative n, so divide 
   if sizen.n_dimensions eq 0 then $ 
   out[*] = out + x / 2ull^((-1)*n) else begin 
   neg = r[r[0]:r[1]-1] 
   nsel= 1 ;or more 
   case 1 of 
       scalar: xx = x 
       numn le sizex.n_elements: xx = x[neg] 
       else: begin 
       sel = where( neg lt sizex.n_elements, nsel) 
       if nsel ge 1 then begin 
           neg = neg[sel] 
           xx = x[neg] 
           endif 
       end 
       endcase 
             if nsel gt 0 then out[neg] = out[neg] + xx / 2ull^((-1)*n[neg]) 
       endelse 
       endif 
      if h[1] gt 0 then begin ; positive n, so multiply 
   if sizen.n_dimensions eq 0 then $ 
   out[*] = out + x * 2ull^n else begin 
   pos = r[r[1]:r[2]-1] 
   nsel= 1 ;or more 
   case 1 of 
       scalar: xx = x 
       numn le sizex.n_elements: xx = x[pos] 
       else: begin 
       sel = where( pos lt sizex.n_elements, nsel) 
       if nsel ge 1 then begin 
           pos = pos[sel] 
           xx = x[pos] 
           endif 
       end 
       endcase 
             if nsel gt 0 then out[pos] = out[pos] + xx * 2ull^n[pos] 
       endelse 
       endif 
 
  return, scalar ? out[0] : out 
end 

