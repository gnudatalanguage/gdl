;+
;
;
;
; NAME:
;      SWAP_ENDIAN
;
; PURPOSE: 
;      swaps endianness of data, including structs
;
;
; CATEGORY:
;      Utility
;
; CALLING SEQUENCE:
;      result = SWAP_ENDIAN( data)
;
;
; KEYWORD PARAMETERS: 
;      SWAP_IF_BIG_ENDIAN: only perform action if machine is big endian.
;      SWAP_IF_LITTLE_ENDIAN: only perform action if machine is little endian.
;
; OUTPUTS:
;      result: same datatype and structure as input with reversed byte order
;
;
; PROCEDURE:
;      uses BYTEORDER
;
;
; MODIFICATION HISTORY:
;     Written by: Marc Schellens, 2005
;   21-Jan-2006 : PC, switch statement, correct little endian detection
;
;-
; LICENCE:
; Copyright (C) 2005, M. Schellens, 2006, P. Chanial
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
;-

function SWAP_ENDIAN, dataIn,$
                      SWAP_IF_BIG_ENDIAN=ifBig, $
                      SWAP_IF_LITTLE_ENDIAN=ifLittle

 on_error, 2

 dataOut = dataIn

 littleEndian = (byte(1,0,1))[0]
 if littleEndian then begin
    if keyword_set( ifBig) then return, dataOut
 endif else begin
    if keyword_set( ifLittle) then return, dataOut
 endelse

 type = size( dataIn, /TYPE)
 switch type of
    1 :                                  ;; BYTE
    7 : return, dataOut                  ;; STRING
    2 :                                  ;; INT
    12: begin                            ;; UINT
           byteorder, dataOut, /SSWAP
           break
        end
    3 :                                  ;; LONG
    4 :                                  ;; FLOAT
    6 :                                  ;; COMPLEX
    13: begin                            ;; ULONG
           byteorder, dataOut, /LSWAP
           break
        end
    5 :                                  ;; DOUBLE
    9 :                                  ;; DCOMPLEX
    14:                                  ;; INT64
    15: begin                            ;; UINT64
           byteorder, dataOut, /L64SWAP
           break
        end
    8 : begin                            ;; STRUCT
           for i = 0, n_tags( dataOut)-1 do begin 
              dataOut.(i) = swap_endian(dataOut.(i))
           endfor
           break
        end
    10: message, 'Unable to swap reference data type.'
    11: message, 'Unable to swap object reference data type.'
    else: message, 'Internal error: Unknown type: '+strtrim(type,1)
 endswitch

 return, dataOut
 
end
