;+
; NAME:   SMOOTH
;
; PURPOSE:  The SMOOTH function returns a smoothed array of the input array.
; smoothed with a boxcar average of the specified width.
; The result has the same type and dimensions as Array.
;
; Two algorithms are used, the old slow using 2D convol, and
; a new faster using 1D convol.
; - The old slow algorithm used by SMOOTH is very simple: using the CONVOL
; function with a "flat" kernel (no more the default, must be
; triggered by keyword /SLOW_ALGO)
; - The new fast algorithm [now default !], thanks to Sergey A.
; [since 2012-Apr-29]
; "The point of the optimization is to use several subsequental
; 1d convolutions instead of 1 multidimensional convolution. 
; This approach is mathematically correct for boxcar filter i.e. smooth."
;  https://sourceforge.net/tracker/index.php?func=detail&aid=3522374&group_id=97659&atid=618685
;
; When something goes wrong (e.g. bad type or size of "width"), SMOOTH
; returns the initial input. 
;
; CATEGORY: Numerical analysis.
;
; CALLING SEQUENCE:
;
; Result = SMOOTH( Array, Width [, /EDGE_TRUNCATE] [, /NAN], [/TEST, /HELP]);
;
;
; INPUTS:
; -- the input Array. Can be 1D, 2D, ...
; -- the filtering Width. Must be great or equal to 3 to have any
;    effect. Can be a single value or the same number of elements than
;    Array'Dim.
;
; OPTIONAL INPUTS:  none
;
; KEYWORD PARAMETERS:
; Initials KEYWORD PARAMETERS (compatibles with the IDL version of SMOOTH);
; -- NaN           : say to the program to manage NaN numbers (-Inf, +Inf also)
; -- Edge_TRUNCATE : compute including the edge, but with 
;
; Extended KEYWORD PARAMETERS (specific to this version)
; -- Help : give a very simple summary of procedure then exit
; -- Test : will stop just before the return statement
;    (useful for internal checking/debugging)
; -- verbose : print some informations
; -- slow_algo : the old slow reference version ... [may be use
;    later if different algo emerge ... can be useful to test
;    future other alternatives]
;
; OUTPUTS:  return an array of the same size than the input one.
;
; OPTIONAL OUTPUTS: none
;
; COMMON BLOCKS:    none
;
; SIDE EFFECTS:     none
;
; RESTRICTIONS:     /Nan is not working now. IDL algo is not fully
; clear. See discussions in Idl PvWave newgroup during summer 2006
;
; COMPATIBILITY:  
; - this function is NOT FULLY compatible with the IDL one, because :
;   -1- The NaN managing is not finish now (works but gives different
;       value than IDL version)
;   -2- We DO have ADD some checks (width value, array type)
;        which were missing in initial procedure.
;       This will give clear warning when a problem is encountered.
;   -3- We DO have ADD some keywords (/TEST, /HELP)
;   -4- only 1D, 2D and 3D OK now when /Edge_TRUNCATE
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
;
;-
; LICENCE:
; Copyright (C) 2006, 2012 Alain Coulais and Sergey A.
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.
;-
;
function SMOOTH, input_array, width, $                 
                 EDGE_TRUNCATE=EDGE_TRUNCATE, NAN=NAN, $
                 slow_algo=slow_algo, $ ; the slow reference version
                 test=test, help=help, debug=debug, verbose=verbose
;
ON_ERROR, 2
;
; We must have exactly an "input_array and a "width" !
;
if (N_PARAMS() NE 2) then help=1
;
if KEYWORD_SET(help) then begin
   print, 'function SMOOTH, array, width, [/EDGE_TRUNCATE], [/NAN] [/help, /test, /verbose]'
   if (N_PARAMS() NE 2) then return, -1 else return, input_array
endif
;
if KEYWORD_SET(NaN) then begin
   print, '------------------------ warning --------'
   print, '-- /NaN  not fully ready now ------------'
   print, '------------------------ warning --------'
endif
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
      MESSAGE, 'Number of Array dimensions does not match number of Width dimensions', /continue
      return, input_array
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
   if (real_width[ii] LT 0) then begin
      MESSAGE, 'Width must be nonnegative',/continue
      return, input_array
   endif
   ;;
   ;; This is NOT check properly in IDL
   if (real_width[ii] LE 1) then begin
      MESSAGE, 'Width must be Great or Equal to 2',/continue
      return, input_array
   endif
   ;;
   if (dimensions[ii] LE real_width[ii]) then begin
      MESSAGE, 'Width must be smaller than array dimensions',/continue
      return, input_array
   endif
   ;;
   if ((real_width[ii] MOD 2) EQ 0) then real_width[ii]=real_width[ii]+1
endfor
;
half_width=real_width/2
;
if KEYWORD_SET(test) then print, width, real_width, half_width
;
; by default, we assume we don't need NaN processing
;
need_nan_process=0
;
; if the NaN switch ON ?
;
if KEYWORD_SET(NaN) then begin
   need_nan_process=1
   nan_location=WHERE(FINITE(input_array) EQ 0, nbp_nan)
   ;;
   ;; can we switch OFF ?
   if (nbp_nan EQ 0) then begin
      if KEYWORD_SET(verbose) then print, 'NaN flag switch ON but no NaN founded'
      need_nan_process=0
   endif else begin
      ;; we create an array of same size the input array but with Zero's 
      pb=FLTARR(SIZE(input_array,/dimensions))
      ;; Where we have NaN, we put "One" (1.)
      pb[nan_location]=1.
      ;; We put Zero's for Missing data
      input_array[nan_location]=0.        
   endelse
endif
;
; The SLOW initial version.
; We keep it just in case, and also because we use it in time tests
;
if KEYWORD_SET(slow_algo) then begin
   if KEYWORD_SET(debug) then MESSAGE, /continue, 'Reference Algo (slowest)'
   ;;
   ;; calling of 2D CONVOL with "constant" gate
   ;;
   gate=REPLICATE(1., real_width)
   norm_of_gate=TOTAL(gate)
   output_array=CONVOL(input_array, gate, norm_of_gate, $
                       EDGE_TRUNCATE=EDGE_TRUNCATE)
endif else begin
   if KEYWORD_SET(debug) then MESSAGE, /continue, 'Revisited Algo (fast)'
   ;;
   ;; A FAST version base on calls to 1D CONVOL
   ;; calling of CONVOL for each dimension subsequently
   ;; performing several 1d convolutions instead of  multidimensional one
   ;;
   temp_width=real_width*0.+1.
   ;;
   ;; this will be dimensions for 1d kernel ([1,1,1])
   ;;
   output_array=input_array
   for i=0,N_ELEMENTS(real_width)-1 do begin
      temp_width[i]=real_width[i]
      ;;
      ;;preparing dimensions for  1d kernel ( [1,1,1]->[1,3,1])
      ;;
      gate=REFORM(REPLICATE(1.0,real_width[i]),temp_width)
      ;;
      ;;creating kernel (gate) for 1d convolution
      ;;
      norm_of_gate=TOTAL(gate)
      output_array=CONVOL(TEMPORARY(output_array), gate, norm_of_gate, $
                          EDGE_TRUNCATE=EDGE_TRUNCATE)
      ;;
      ;;convolution with 1d kernel
      ;;
      temp_width[i]=1.
      ;;
      ;; returning kernel dimensions to the initial state ( [1,3,1]->[1,1,1])
      ;;
   endfor
endelse
;
; Please remember that Convol return 0 on borders when EDGE_TRUNCATE is not set 
;
if NOT(KEYWORD_SET(EDGE_TRUNCATE)) then begin
   if (n_dimensions GT 3) then begin 
      MESSAGE, '/EDGE_TRUNCATE not available in SMOOTH when N_dim > 3 ...', /continue
      return, input_array
   endif
   ;; not very efficient but mandatory for the edges,
   ;; and I never used SMOOTH in higher dimensions
   ;;
   if (n_dimensions EQ 1) then begin
      output_array[0:half_width-1]=input_array[0:half_width-1]
      output_array[dimensions[0]-half_width:dimensions[0]-1]=input_array[dimensions[0]-half_width:dimensions[0]-1]
   endif
   ;;
   if (n_dimensions EQ 2) then begin
      output_array[0:half_width[0]-1,*]=input_array[0:half_width[0]-1,*]
      output_array[dimensions[0]-half_width[0]:dimensions[0]-1,*]=input_array[dimensions[0]-half_width[0]:dimensions[0]-1,*]
      ;;
      output_array[*,0:half_width[1]-1]=input_array[*,0:half_width[1]-1]
      output_array[*,dimensions[1]-half_width[1]:dimensions[1]-1]=input_array[*,dimensions[1]-half_width[1]:dimensions[1]-1]
   endif
   ;;
   if (n_dimensions EQ 3) then begin
      output_array[0:half_width[0]-1,*,*]=input_array[0:half_width[0]-1,*,*]
      output_array[dimensions[0]-half_width[0]:dimensions[0]-1,*,*]=input_array[dimensions[0]-half_width[0]:dimensions[0]-1,*,*]
      ;;
      output_array[*,0:half_width[1]-1,*]=input_array[*,0:half_width[1]-1,*]
      output_array[*,dimensions[1]-half_width[1]:dimensions[1]-1,*]=input_array[*,dimensions[1]-half_width[1]:dimensions[1]-1,*]
      ;;
      output_array[*,*,0:half_width[2]-1]=input_array[*,*,0:half_width[2]-1]
      output_array[*,*,dimensions[2]-half_width[2]:dimensions[2]-1]=input_array[*,*,dimensions[2]-half_width[2]:dimensions[2]-1]
   endif
endif
;
; 
if (need_nan_process EQ 1) then begin
   pb=CONVOL(pb, gate, norm_of_gate)
   pb_remain=WHERE(pb GT 0.999999, nbp_pb_remain)
   if (nbp_pb_remain GT 0) then begin
      if KEYWORD_SET(verbose) then print, 'It remain''s some NaN points !'
      output_array[pb_remain]=!values.f_NaN
   endif else begin
      if KEYWORD_SET(verbose) then print, 'No more NaN points !'
   endelse
   ;;
   ;; we write back the NaN in the input array
   input_array[nan_location]=!values.f_NaN
endif
;
if KEYWORD_SET(test) then STOP
;
return, output_array
;
end
