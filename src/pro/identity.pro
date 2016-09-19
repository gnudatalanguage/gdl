;$Id: identity.pro,v 1.4 2016-09-19 21:29:57 gilles-duvert Exp $
function IDENTITY, n, double=double_keyword, help=help
;+
;
; NAME:
;    identity
;
; PURPOSE: 
;   returns an n x n identity matrix
;
; CATEGORY:
;   Mathematics: Matrices
;
; CALLING SEQUENCE:
;   i=IDENTITY(4)
;
; KEYWORD PARAMETERS: 
;     DOUBLE_KEYWORD : Keyword for double precision calculation
;
; OUTPUTS:
;     The identity matrix
;
; PROCEDURE:
;     Creates an n x n matrix, fills the diagonal elements with the
;     value 1.0
;
; EXAMPLE:
;     i = IDENTITY(4)
;     print, i
;     1.00000    0.00000   0.0000    0.00000
;     0.00000    1.00000   0.0000    0.00000
;     0.00000    0.00000   1.0000    0.00000
;     0.00000    0.00000   0.0000    1.00000
;
; MODIFICATION HISTORY:
; 	Written by:  2004-03-20 Christopher Lee.
;                    2011-12-17 G. Duvert (if n==1 !)
;
;-
; LICENCE:
; Copyright (C) 2004, 2011
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;-
;
ON_ERROR, 2
;
if KEYWORD_SET(help) then begin
    print, "function IDENTITY, n, double=double_keyword, help=help"
    return, -1
endif
;
diag=INDGEN(n)
;
if (KEYWORD_SET(double_keyword)) then begin
    id=DBLARR(n,n)
endif else begin
    id=FLTARR(n,n)
endelse
;
id[diag,diag]=1
;
return, id
;
end
