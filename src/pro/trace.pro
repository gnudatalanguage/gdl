;$Id: trace.pro,v 1.2 2011-02-27 19:05:38 slayoo Exp $
function trace, matrix,double=double_keyword

;+
;
;
;
; NAME:
;     trace
;
; PURPOSE: 
;    Calculates the trace of the input matrix 'matrix'
;
;
; CATEGORY:
;    Mathematics: Matrices
;
; CALLING SEQUENCE:
;    result = trace(matrix)
;
;
;
;
; KEYWORD PARAMETERS: 
;    DOUBLE_KEYWORD : Keyword for double precision calculation    
;
; OUTPUTS: 
;    Result is the trace of the matrix
;
;
;
; RESTRICTIONS:
;    The input matrix must be n x n
;
;
; PROCEDURE:
;   Sum over the diagonal elements of the input matrix, return the result
;
; EXAMPLE:
;   i=identity(4) ; 4 x 4 identity matrix
;   r=trace(i)
;   print, r
;      4.0000
;
; MODIFICATION HISTORY:
; 	Written by:  2004-03-20 Christopher Lee.
;
;
;-
; LICENCE:
; Copyright (C) 2004,
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;
;-

On_error, 2

s=size(matrix)
if(s[0] ne 2 and s[1] eq s[2]) then begin
    printf, "ERROR: need 2 dimensions"
    stop
endif


n=s[1]
type=s[s[0]+1]

if(type eq 4 or type eq 5) then begin
    ;real matrix

    tr=keyword_set(double_keyword) or type eq 5? 0.0d: 0.0
    if(keyword_set(double_keyword) and type ne 5) then $
        for i=0L, n-1 do tr=tr+double(matrix[i,i]) $
    else $
        for i=0L, n-1 do tr=tr+matrix[i,i]

endif else if(type eq 6 or type eq 9) then begin
;complex matrix
    tr=keyword_set(double_keyword) or type eq 9? dcomplex(0.0,0.0): complex(0.0,0.0)
    
    if(keyword_set(double_keyword) and type ne 9) then $
       for i=0L, n-1 do tr=tr+dcomplex(matrix[i,i]) $
    else $
      for i=0L, n-1 do tr=tr+matrix[i,i]

endif



return, tr

end


