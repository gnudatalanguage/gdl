;+
; NAME:      REAL_PART
;
; PURPOSE:
;
; This function returns the real part of a complex number/array 
; (of any dimension), in a type consistant to the input variable 
; (either FLOAT or DOUBLE).
; If the type of the input is convertible (BYTE, INT, STRING, ...)
; it is promote to FLOAT type.
;
; CATEGORY: 
;
; CALLING SEQUENCE:
;
;                 Result = REAL_PART(Z)
;
; INPUTS:
;
;   Z:  A scalar or array. Z may be of any numeric type.
;   If Z is not complex then the result is simply converted to
;   floating-point (single-precision for all integer types,
;   double precision for type double). If Z is STRING, it is converted
;   to FLOAT when possible, despite, it is converted to "0.0"
;
; OPTIONAL INPUTS:  none
;
; KEYWORD PARAMETERS:  none
;
; OUTPUTS: A scale or an array of the same size of the input.
;
; OPTIONAL OUTPUTS:  none
;
; COMMON BLOCKS:  none
;
; SIDE EFFECTS:  none
;
; RESTRICTIONS: Behavior on Objects/Structures/Pointers not clear ! 
;
; PROCEDURE:
;
; EXAMPLE:
;
; print, REAL_PART(INDGEN(10))
; print, REAL_PART(COMPLEXARR(2)+1)
; print, REAL_PART(['s','10,'])
; 
; MODIFICATION HISTORY:
;   Written by:  Alain Coulais, 06 June 2008.
;
; LICENCE:
; Copyright (C) 2008, Alain Coulais
;
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
;-

function real_part, z
;
ON_ERROR, 2
;
if (N_PARAMS() lt 1) then MESSAGE, 'Incorrect number of arguments.'
;
type=SIZE(z, /TYPE)
;
; When input is a DOUBLE (ie DOUBLE or DCOMPLEX) we 
; When input is not of DOUBLE type, it is converted to FLOAT type ...
;
; DOUBLE: type == 5
; DCOMPLEX: type == 9
;
if ((type EQ 5L) OR (type EQ 9L)) then begin
   return, DOUBLE(z)
endif else begin
   return, FLOAT(z)
endelse
;
end
;

