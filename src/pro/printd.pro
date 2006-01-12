;+
; NAME: PRINTD
;
; PURPOSE:
;       Print the content of the directory stack handled by PUSHD and POPD.
;
;
; CATEGORY:
;       Directory management
;
;
; CALLING SEQUENCE:
;       printd
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

pro printd

 common dir_stack, nstack, stack
 
 on_error, 2

 if n_elements(nstack) eq 0 then nstack = (stack = 0)
 
 if nstack eq 0 then message, 'Directory stack is empty.'
 
 cd, curr=curr
 print, 'Current Directory: ', curr
 print, 'Directory Stack Contents:'
 
 for i=0l, nstack-1 do begin
    print, format=('(i3,") ",a)'), i, stack[i]
 endfor
 
end
