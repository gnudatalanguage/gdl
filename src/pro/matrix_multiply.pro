;+
; NAME:  MATRIX_MULTIPLY
;
; PURPOSE:  GDL substitute to the IDL intrinsic function
;
; AUTHOR: Philippe Prugniel 2008/02/29
;
; Modifications:
; 05-Feb-2013: when GDL is compiled with Eigen Lib., we use internal
;              fast MATMUL function. It is not ready for Complex/DoubleComplex
; 01-Mar-2013: with Eigen Lib, matmul function is OK with complex values, removed
; some code
; Copyright (C) 2008, 2013.
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
; 
;-----------------------------------------------------------------------------
function MATRIX_MULTIPLY, a, b, ATRANSPOSE=atr, BTRANSPOSE=btr, $
                          help=help, debug=debug
;
ON_ERROR, 2
;
if KEYWORD_SET(help) then begin
    print, 'function MATRIX_MULTIPLY, a, b, ATRANSPOSE=atr, BTRANSPOSE=btr, $'
    print, '                          help=help, debug=debug'
    return, -1
endif
;
IF (N_PARAMS() NE 2) THEN BEGIN
    MESSAGE, 'Incorrect number of arguments.'
ENDIF
;
; "type" will be 1 if GDL compiled with Eigen, 0
; !matmul_quiet to avoid repeating internal message if no Eigen around ...
;
DEFSYSV, "!matmul_quiet", exist=quiet
if ~quiet then begin
    type=MATMUL(/available, quiet=quiet)
    DEFSYSV, "!matmul_quiet", 1, 1
endif else begin
    type=MATMUL(/available,/quiet)
endelse

if (type EQ 0) then begin
    case (1) of
        KEYWORD_SET(atr) and not KEYWORD_SET(btr): return, TRANSPOSE(a) # b
        KEYWORD_SET(btr) and not KEYWORD_SET(atr): return, a # TRANSPOSE(b)
        KEYWORD_SET(atr) and KEYWORD_SET(btr): return, TRANSPOSE(a) # TRANSPOSE(b)
        else : return, a # b
    endcase
endif else begin
    return, MATMUL(a, b, ATRANSPOSE=atr, BTRANSPOSE=btr, debug=debug)
endelse
;
end


