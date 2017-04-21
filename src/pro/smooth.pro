;+
; NAME:   SMOOTH
;
; PURPOSE:  The SMOOTH function returns a smoothed array of the input array.
; smoothed with a boxcar average of the specified width.
; The result has the same type and dimensions as Array.
;
; SMOOTH is just a way to call CONVOL with proper convolution kernel.
; And, (thanks to Sergey A.) since this kernel is separable, this is
; just a series of 1D convolutions made faster by transposing the array.
;
; CATEGORY: Numerical analysis.
;
; CALLING SEQUENCE:
;
; Result = SMOOTH( Array, Width [, /EDGE_TRUNCATE | /EDGE_WRAP | /EDGE_ZERO] [, /NAN], [MISSING=value], [/help]);
; 
;
; INPUTS:
; -- the input Array. Can be any dimension
; -- the filtering Width. Must be great or equal to 3 to have any
;    effect. Can be a single value or an array of integers of same rank as Array.
;
; OPTIONAL INPUTS:  none
;
; KEYWORD PARAMETERS:
; Initials KEYWORD PARAMETERS (compatibles with the IDL version of SMOOTH);
; -- NaN           : say to the program to manage NaN numbers (-Inf, +Inf also)
; -- Missing=xxx   : will replace missing values (if NaNs are present and the result of smoothing at
;                    some position would be Nan, it s replaced by xxx.)
; -- Edge_TRUNCATE : If absent, only the "smoothable" part of Array will be smoothed.
;                    The rest will be unchnaged. By "smoothable" we mean array elements that
;                    have sufficent neighbors (on all "sides") for the smoothing window "Width" to work.
;                    If present, acts as if the nearest edge value was extending the Array to infinity.
; -- Edge_Zero     : As above, but acts as if the exterior of Array was extended by zeros.
; -- Edge_Wrap     : As above, but acts as if Array was folded onto itself like in a PacMan game.
; -- Help          : prints a short help but do you know about doc_library,"smooth" ?
;
; OUTPUTS:  return an array of the same size than the input one.
;
; OPTIONAL OUTPUTS: none
;
; COMMON BLOCKS:    none
;
; SIDE EFFECTS:     none
;
; COMPATIBILITY:  
;   Should be OK
;
; EXAMPLES:     b=SMOOTH(a,3)
;               b=SMOOTH(a,3, /edge)
;               b=SMOOTH(a,3, /nan)
;               b=SMOOTH(a,3, /nan, /edge)
;               b=SMOOTH(randomu(seed,10,20,30), 5)
;               b=SMOOTH(randomu(seed,10,20,30), [3,5,7])
;
; MODIFICATION HISTORY:
;  - 26/07/2006: created by Alain Coulais (ARSC) during CalTech visit
;  - 28/03/2008: managing missing array input (0-element and 1-element)
;  - 29/04/2012: Sergey A. : Patch ID 3522374 on sf.net. Optimized version.
;        (using several 1d convolutions instead of multidimensional one).
;  - 20/04/2017: G. Duvert, following optimization of the CONVOL code.
;                Note: ancillary historical information can be retrieved
;                from the former version of this file.
;-
; LICENCE:
; Copyright (C) 2006, 2012, 2017 Alain Coulais and Sergey A. and G. Duvert
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.
;
;
function SMOOTH, input_array, width, help=help, _EXTRA=e
;
ON_ERROR, 2
COMPILE_OPT idl2, HIDDEN 
;
if KEYWORD_SET(help) then begin
   print, 'function SMOOTH, array, width, [/EDGE_TRUNCATE], [EDGE_WRAP], [EDGE_ZERO], [/NAN] , [MISSING=value], [INVALID=invalid], /help, /test, /verbose]'
   return, !NULL
endif
;
; We must have exactly an "input_array and a "width" !
if (N_PARAMS() NE 2) then message,"Incorrect number of arguments."
;
if (SIZE(input_array,/type) EQ 7) then begin
   MESSAGE, 'STRING expression not allowed in this context:'
endif
;
if (SIZE(input_array,/type) EQ 8) then begin
   MESSAGE, 'STRUCT expression not allowed in this context:'
endif
;
if (N_ELEMENTS(input_array) LE 1) then begin
   MESSAGE, 'Expression must be an array in this context:'
endif
;
n_dimensions=SIZE(input_array,/n_dimensions)
dimensions=SIZE(input_array,/dimensions)
;
; --------------------
; Managing Width (can be a number OR a array)
; Width must be >= than 2 (in fact 2 becomes 3 !)
;
; This is checked in IDL
;
if (N_ELEMENTS(width) GT 1) then begin
   if (N_ELEMENTS(width) NE n_dimensions) then begin
      MESSAGE, 'Number of Array dimensions does not match number of Width dimensions'
   endif else begin
      real_width=width
   endelse
endif else begin
   real_width=REPLICATE(width,n_dimensions)
endelse
;
; below here, no more use of "width"
;
for ii=0, N_ELEMENTS(real_width)-1 do begin
   if (real_width[ii] LT 1) then begin
      MESSAGE, 'Width must be strictly positive'
   endif
   ;;
   if (dimensions[ii] LE real_width[ii]) then begin
      MESSAGE, 'Width must be smaller than array dimensions'
   endif
   ;;
   if ((real_width[ii] MOD 2) EQ 0) then real_width[ii]=real_width[ii]+1
endfor

;; A FAST version base on calls to 1D CONVOL
;; calling of CONVOL for each dimension subsequently
;; performing several 1d convolutions instead of  multidimensional one

output_array=input_array
transpcode=shift(indgen(N_ELEMENTS(real_width)),-1)
order=indgen(N_ELEMENTS(real_width))

for i=0,N_ELEMENTS(real_width)-1 do begin

  ;;preparing dimensions for  1d kernel ( [1,1,1]->[1,3,1])

  gate=REPLICATE(1.0,real_width[i])

  ;;creating kernel (gate) for 1d convolution

  norm_of_gate=TOTAL(gate)
  output_array=CONVOL(TEMPORARY(output_array), gate, norm_of_gate, _extra=e)

  ;;convolution with 1d kernel and transposition for next dimension:

  order=shift(order,-1)
  output_array=transpose(TEMPORARY(output_array),transpcode)

endfor
; array is back to initial shape thanks to all transpositions made.
return, output_array
end
