;+
;
; NAME:
;     strsplit
;
; PURPOSE:
;     strtok wrapper
;
; CATEGORY:
;     String Manipulation
;
; CALLING SEQUENCE:
;     Same as strtok
;
;
; MODIFICATION HISTORY:
;   17-Jul-2005 : written by Pierre Chanial
;
; LICENCE:
; Copyright (C) 2004, Pierre Chanial
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
;
;-

function strsplit, input1, input2, _ref_extra = extra, REGEX=regex

 on_error, 2
 
;if keyword_set( regex) then begin
;    message,'REGEX keyword not implemented yet.'
;endif

 if size(input1, /type) ne 7 or size(input1, /n_dimensions) ne 0 then begin
    message, 'Invalid input string.'
 endif
 if n_params() eq 2 then begin
    if n_elements(input2) eq 0 then begin
       message, 'Undefined pattern string.'
   endif
   if keyword_set( regex) then $ 
     return, strtok(input1, input2, _extra = extra, REGEX=regex) $
   else $
     return, strtok(input1, input2, _extra = extra)
 endif
 
 if keyword_set( regex) then $ 
   return, strtok(input1, _extra = extra, REGEX=regex) $
 else $
   return, strtok(input1, _extra = extra)
end
