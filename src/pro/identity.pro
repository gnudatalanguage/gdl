;$Id: identity.pro,v 1.1.1.1 2004-12-09 15:10:19 m_schellens Exp $

function identity, n,double_keyword=double_keyword

;+
;
;
;
; NAME:
;    identity
;
; PURPOSE: 
;   returns an n x n identity matrix
;
;
; CATEGORY:
;   Mathematics: Matrices
;
; CALLING SEQUENCE:
;   i=identity(4)
;
;
; KEYWORD PARAMETERS: 
;     DOUBLE_KEYWORD : Keyword for double precision calculation
;
; OUTPUTS:
;     The identity matrix
;
;
; PROCEDURE:
;     Creates an n x n matrix, fills the diagonal elements with the
;     value 1.0
;
; EXAMPLE:
;     i = identity(4)
;     print, i
;     1.00000    0.00000   0.0000    0.00000
;     0.00000    1.00000   0.0000    0.00000
;     0.00000    0.00000   1.0000    0.00000
;     0.00000    0.00000   0.0000    1.00000
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



on_Error, 2
if(keyword_set(double_keyword)) then begin
    id=dblarr(n,n)
    for i=0, n-1 do id[i,i]=1.0
endif else begin
    id=fltarr(n,n)
    for i=0, n-1 do id[i,i]=1.0d
endelse

return, id



end
