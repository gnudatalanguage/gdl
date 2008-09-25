;+
; NAME:  MATRIX_MULTIPLY
;
; PURPOSE:  GDL substitute to the IDL intrinsic function
;
; AUTHOR: Philippe Prugniel 2008/02/29
;
; Copyright (C) 2008, 
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
; 
;-----------------------------------------------------------------------------
function matrix_multiply, a, b, ATRANSPOSE=atr, BTRANSPOSE=btr
;
case (1) of
    keyword_set(atr) and not keyword_set(btr): return, transpose(a) # b
    keyword_set(btr) and not keyword_set(atr): return, a # transpose(b)
    keyword_set(atr) and keyword_set(btr): return, transpose(a) # transpose(b)
    else : return, a # b
endcase
;
end


