;+
;
; NAME:
;      SWAP_ENDIAN_INPLACE
;
; PURPOSE: 
;      swaps endianness of data, including structs, without copying
;
; CATEGORY:
;      Utility
;
; CALLING SEQUENCE:
;      SWAP_ENDIAN,data
;
; KEYWORD PARAMETERS: 
;      SWAP_IF_BIG_ENDIAN: only perform action if machine is big endian.
;      SWAP_IF_LITTLE_ENDIAN: only perform action if machine is little endian.
;
; OUTPUTS:
;
;
; PROCEDURE:
;     - derivate from SWAP_ENDIAN
;     - uses BYTEORDER
;
; MODIFICATION HISTORY:
;   initial procedure SWAP_ENDIAN written by Marc Schellens, 2005
;   21-Jan-2006 : PC, switch statement, correct little endian detection
;   July 2010: Maxime Lenoir for PDS Library compatibility
;
;-
; LICENCE:
; Copyright (C) 2010 Maxime Lenoir
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
;-

pro SWAP_ENDIAN_INPLACE, dataIn,$
                         SWAP_IF_BIG_ENDIAN=ifBig, $
                         SWAP_IF_LITTLE_ENDIAN=ifLittle

ON_ERROR, 2

littleEndian = (BYTE(1,0,1))[0]
if littleEndian then begin
    if KEYWORD_SET( ifBig) then return
endif else begin
    if KEYWORD_SET( ifLittle) then return
endelse

type = SIZE( dataIn, /TYPE)
switch type of
    1 :                                  ;; BYTE
    7 : return		                 ;; STRING
    2 :                                  ;; INT
    12: begin                            ;; UINT
        BYTEORDER, dataIn, /SSWAP
        break
    end
    3 :                                  ;; LONG
    4 :                                  ;; FLOAT
    6 :                                  ;; COMPLEX
    13: begin                            ;; ULONG
        BYTEORDER, dataIn, /LSWAP
        break
    end
    5 :                                  ;; DOUBLE
    9 :                                  ;; DCOMPLEX
    14:                                  ;; INT64
    15: begin                            ;; UINT64
        BYTEORDER, dataIn, /L64SWAP
        break
    end
    8 : begin                            ;; STRUCT
        for i = 0, N_TAGS(dataIn)-1 do begin 
            data=dataIn.(i)
            SWAP_ENDIAN_INPLACE, data
            dataIn.(i)=data
        endfor
        break
    end
    10: MESSAGE, 'Unable to swap reference data type.'
    11: MESSAGE, 'Unable to swap object reference data type.'
    else: MESSAGE, 'Internal error: Unknown type: '+STRTRIM(type,1)
endswitch

end
