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
; 	Written by:	Marc Schellens, 2005
;
;-
; LICENCE:
; Copyright (C) 2005,
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
;-

function SWAP_ENDIAN, dataIn,$
                      SWAP_IF_BIG_ENDIAN=ifBig, $
                      SWAP_IF_LITTLE_ENDIAN=ifLittle

dataOut = dataIn

littleEndian = (fix([1b,0b],0) eq 1)

if littleEndian then begin
    if keyword_set( ifBig) then return,dataOut
endif else begin
    if keyword_set( ifLittle) then return,dataOut
endelse

type = size( dataOut, /TYPE)

case type of
    1: return,dataOut                   ;; BYTE
    2: byteorder, dataOut, /SSWAP, $    ;; INT
    3: byteorder, dataOut, /LSWAP, $    ;; LONG
    4: byteorder, dataOut, /LSWAP, $    ;; FLOAT
    5: byteorder, dataOut, /L64SWAP, $  ;; DOUBLE
    6: byteorder, dataOut, /LSWAP, $    ;; COMPLEX
    7: return,dataOut                   ;; STRING
    8: begin                            ;; STRUCT
        for i = 0, n_tags( dataOut)-1 do begin 
            t = swap_endian(dataOut.(i))
            dataOut.(i) = t
        endfor
    end
    9: byteorder, dataOut, /L64SWAP, $  ;; DCOMPLEX
    10: message, 'Unable to swap pointer data.'
    11: message, 'Unable to swap object reference data.'
    12: byteorder, dataOut, /SSWAP, $   ;; UINT
    13: byteorder, dataOut, /LSWAP, $   ;; ULONG
    14: byteorder, dataOut, /L64SWAP, $ ;; INT64
    15: byteorder, dataOut, /L64SWAP, $ ;; UINT64
    else: message, 'Internal error: Unknown type: '+strtrim(type,1)
endcase

return,dataOut
end
