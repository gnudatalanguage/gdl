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
;   14-Apr-2010 : Alain Coulais : complete re-writing
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

function STRSPLIT, input1, input2, $
                   count=count, length=length, extract=extract, regex=regex, $
                   escape=escape, fold_case=fold_case, preserve_null=preserve_null, $
                   test=test, help=help
;
ON_ERROR, 2
;
if KEYWORD_SET(help) then begin
    print, 'function STRSPLIT, input1, input2, $'
    print, '                   count=count, length=length, extract=extract, regex=regex, $'
    print, '                   escape=escape, fold_case=fold_case, preserve_null=preserve_null, $'
    print, '                   test=test, help=help'
    return, -1
endif
;
if KEYWORD_SET(escape) then begin
    MESSAGE, /continue, 'Sorry, this keyword ESCAPE is not implemented (no effect)'
    MESSAGE, /continue, 'PLEASE CONTRIBUTE'
endif
if KEYWORD_SET(preserve_null) then begin
    MESSAGE, /continue, 'Sorry, this keyword PRESERVE_NULL is not implemented (no effect)'
    MESSAGE, /continue, 'PLEASE CONTRIBUTE'
endif
if KEYWORD_SET(fold_case) then begin
    MESSAGE, /continue, 'Sorry, this keyword FOLD_CASE is not implemented (no effect)'
    MESSAGE, /continue, 'PLEASE CONTRIBUTE'
endif
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
if KEYWORD_SET(regex) then regex_flag=1 else regex_flag=0
;
if (N_PARAMS() EQ 2) then begin
    if (N_ELEMENTS(input2) EQ 0) then begin
        MESSAGE, 'Undefined pattern string.'
    endif
    resu=STRTOK(local_input1, input2, extract=extract, REGEX=regex_flag)
endif else begin
    resu=STRTOK(local_input1, extract = extract, REGEX=regex_flag)
endelse
;
if KEYWORD_SET(count) then begin
    if ((STRLEN(input1) EQ 0) OR (STRLEN(input2) EQ 0)) then begin
        count=0
    endif else begin
        count=N_ELEMENTS(resu)
    endelse
endif
;
if KEYWORD_SET(length) then length=STRLEN(resu)
;
if KEYWORD_SET(test) then STOP
;
return, resu
;
end
