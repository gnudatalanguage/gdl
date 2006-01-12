;+
; NAME: POPD
;
; PURPOSE:
;       Change the current working directory to the top entry of the
;       directory stack handled by PUSHD and POPD and remove it from the stack.
;
;
; CATEGORY:
;       Directory management
;
;
; CALLING SEQUENCE:
;       popd
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

pro popd

 common dir_stack, nstack, stack 
 on_error, 2

 if n_elements(nstack) eq 0 then nstack = (stack = 0)
 if nstack eq 0 then begin
    message, 'Directory stack is empty.'
 endif
 
 cd, stack[0]
 if --nstack eq 0 then begin
    stack = 0
 endif else begin
    stack = stack[1:*]
 endelse
 
end
