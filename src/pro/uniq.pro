;$Id: uniq.pro,v 1.1.1.1 2004-12-09 15:10:20 m_schellens Exp $
;+
;
;
;
; NAME: 
;      UNIQ
;
;
; PURPOSE: 
;          Returns subscripts of unique elements of an array
;          To be used with SORT
;
; CATEGORY: 
;          General 
;
; CALLING SEQUENCE:
;          res = UNIQ( Array [, Index])     ;; if Array is sorted
;          res = UNIQ( Array, SORT( Array)) ;; if Array is unsorted
;
;
; KEYWORD PARAMETERS: 
;          None  
;
; OUTPUTS:
;          Indices of uniq elements of Array
;          Array must be sorted. If Array is unsorted use:
;          res = UNIQ( Array, SORT( Array))
;
; OPTIONAL OUTPUTS:
;          None.
;
;
; COMMON BLOCKS:
;          None.
;
; SIDE EFFECTS:
;          None.
;
;
; RESTRICTIONS:
;          None.
;
;
; PROCEDURE:
;
;
; EXAMPLE:
;
;
;
; MODIFICATION HISTORY:
; 	Written by: Marc Schellens	
;
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

function UNIQ, arr, index

nEl = n_elements( arr)
if nEl le 1 then return,0

if n_params() eq 1 then begin
    ix = where(arr ne shift(arr, -1))
    if ix[0] ne -1 then return, ix $
    else return, nEl-1
endif else begin
    tmp = arr[ index]
    ix = where(tmp ne shift(tmp,-1))
    if ix[0] ne -1 then return, index[ix] $
    else return, nEl-1
endelse
end
