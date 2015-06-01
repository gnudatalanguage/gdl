function ishft, vin, n
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
; GDL> print, ishft([1,2,3,4,5], bindgen(8))
;       1       4      12      32      80
; GDL> print, ishft([5,5,5,5,5], bindgen(8))
;       5      10      20      40      80
; GDL> print, ishft([5,5,5], bindgen(8))   
;       5      10      20
; GDL> print, ishft(bindgen(5), 3)
;    0   8  16  24  32
; GDL> print, ishft(bindgen(5), [3])
;    0   1  32  48   8                ; note: IDL apparently wrong on this case.
; GDL> print, ishft(bindgen(5), [3,3])
;    0   8
; but also:
; GDL> print, ishft([3], bindgen(5))
;       3
; GDL> print, ishft(3, bindgen(5))
;       3       6      12      24      48
; 
; MODIFICATION HISTORY: 
;       Written by: Richard Schwartz, 09-28-2006 
;	Revised 12-20-2006, Richard Schwartz - Use out[*] to fill final array instead 
;	of out[0] 
;       Rewritten 06-0&-2015, Bill Dieckmann + Gilles Duvert 
; 
;- 
; LICENSE: 
; Copyright (C) 2006, 2015
; This program is free software; you can redistribute it and/or modify 
; it under the terms of the GNU General Public License as published by 
; the Free Software Foundation; either version 2 of the License, or 
; (at your option) any later version. 
; 
; 
;- 
  compile_opt strictarr

  IF (N_PARAMS() NE 2) THEN BEGIN
     message, 'Incorrect number of arguments.'
  ENDIF

  v=vin
  s_v = size(/struct, v)
  s_n = size(/struct, n)
  scalarn = s_n.n_elements eq 1
  scalarv = s_v.n_elements eq 1 && s_v.n_dimensions eq 0

; lets solve the dimensionality problem:
; if a parameter is a scalar (not array), n_elements of result is the n_elements
; of the other parameter. If both parameters ARE ARRAYS and have n_elements > 1, the
; return dimension is the smallest of the two. However note that IDL becomes crazy when
; n is a 1-element array:
; IDL> print, ishft([1,2,3], [0])            
;       1       0      48
; Obviously GDL should return in this case [1,2,3], as if [0] was '0'.


  if (scalarv && ~scalarn) then begin ; if v scalar and n not, push v to the size of n
      v = reform(replicate(v, s_n.n_elements),s_n.dimensions[0:s_n.n_dimensions-1])
   endif else if ( ~scalarv && ~scalarn  ) then begin ; else truncate n or v to smallest size
        if ( s_n.n_elements gt s_v.n_elements ) then n=n[0:s_v.n_elements-1]
        if ( s_v.n_elements gt s_n.n_elements ) then v=v[0:s_n.n_elements-1]
   endif else if ( ~scalarv && scalarn  ) then begin ; 
      n = reform(replicate(n, s_v.n_elements),s_v.dimensions[0:s_v.n_dimensions-1])
   endif

; recompute aligned values
  s_v = size(/struct, v)
  s_n = size(/struct, n)
  scalarn = s_n.n_elements eq 1
  scalarv = s_v.n_elements eq 1

  switch s_v.type of
     1:
     12:
     15: begin
        v = reform(/OVERWRITE, v, s_v.n_elements)
        break
     end
     2: begin
        v = UINT(v,0,s_v.n_elements)
        break
     end
     13: begin
        v = ULONG(v,0,s_v.n_elements)
        break
     end
     14: begin
        v = ULONG64(v,0,s_v.n_elements)
        break
     end
     0:
     4:
     5:
     6:
     7:
     8:
     9:
     10: message,'ishft is for integers'
  endswitch


  if scalarn then begin
     if n ge 0 then begin
        v *= 2ULL^n
     endif else begin
        v /= 2ULL^(-n)
     endelse
  endif else begin
     w_neg = where( n lt 0 )
     w_nneg = where( n ge 0 )
     if w_neg[0] ne -1L then begin
        v[w_neg] /= 2ULL^(-n[w_neg])
     endif
     if w_nneg[0] ne -1L then begin
        v[w_nneg] *= 2ULL^(n[w_nneg])
     endif
  endelse

  if ( scalarv && scalarn) then return, (fix(v[0], TYPE = s_v.TYPE))[0] $
  else return, reform(fix(v, TYPE =s_v.TYPE), s_v.dimensions[0:s_v.n_dimensions-1])

end
