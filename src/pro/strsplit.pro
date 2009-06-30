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
;   30-Jun-2009 : Alain Coulais : will allow 1D string : 'string' and ['string']
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

function STRSPLIT, input1, input2, _ref_extra = extra, REGEX=regex

on_error, 2
 
;if keyword_set( regex) then begin
;    message,'REGEX keyword not implemented yet.'
;endif

if (SIZE(input1, /type) NE 7) then begin
   MESSAGE, 'Invalid input string.'
endif
;
if (SIZE(input1, /n_dimensions) GT 1) then begin
   MESSAGE, 'Invalid input string.'
endif
;
local_input1=input1
;
if (SIZE(input1, /n_dimensions) EQ 1) then begin
   local_input1=local_input1[0]
endif
;
if (N_PARAMS() EQ 2) then begin
   if (N_ELEMENTS(input2) EQ 0) then begin
      MESSAGE, 'Undefined pattern string.'
   endif
   if KEYWORD_SET(regex) then begin
      return, STRTOK(local_input1, input2, _extra = extra, REGEX=regex)
   endif else begin
      return, STRTOK(local_input1, input2, _extra = extra)
   endelse
endif
;
if KEYWORD_SET(regex) then begin
   return, STRTOK(local_input1, _extra = extra, REGEX=regex)
endif else begin
   return, STRTOK(local_input1, _extra = extra)
endelse
;
end
