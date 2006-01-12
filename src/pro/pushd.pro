;+
; NAME: PUSHD
;
; PURPOSE:
;       Store the current working directory on top of the directory stack 
;       handled by PUSHD and POPD and change the current working directory to
;       a user supplied directory.
;
;
; CATEGORY:
;       Directory management
;
;
; CALLING SEQUENCE:
;       pushd, directory
;
;
; INPUTS:
;       directory    Scalar string of the requested working directory
;
;
; MODIFICATION HISTORY:
;   12-Jan-2006 : written by Pierre Chanial
;
; LICENCE:
; Copyright (C) 2006, P. Chanial
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;-

pro pushd, directory

 common dir_stack, nstack, stack
 
 on_error, 2

 if n_elements(nstack) eq 0 then nstack = (stack = 0)
 cd, directory, current=current

 if not keyword_set(nstack++) then begin
    stack = [current]
 endif else begin
    stack = [current, stack]
 endelse
 
end
