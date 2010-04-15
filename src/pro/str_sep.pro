;+
; NAME: STR_SEP
;
; PURPOSE:
;       Serves as a wrapper around STRSPLIT
;
; MODIFICATION HISTORY:
;   01-Sep-2006 : written by Joel Gales
;   14-Apr-2010 : Alain Coulais : complete re-writing
;
; LICENCE:
; Copyright (C) 2006, J. Gales
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
;-
;
function STR_SEP, str, sep, $
                  trim=trim, remove_all=remove_all, esc=esc, $
                  test=test, help=help
;
ON_ERROR, 2
;
if KEYWORD_SET(help) then begin
    print, 'function STR_SEP, str, sep, $'
    print, '                  trim=trim, remove_all=remove_all, esc=esc, $'
    print, '                  test=test, help=help'
    return, -1
endif
;
if (N_PARAMS() NE 2) then MESSAGE, 'Wrong number of arguments. (Two Strings expected)'

if N_ELEMENTS(str) NE 1 then MESSAGE, 'No array allowed for Input String'
if N_ELEMENTS(sep) NE 1 then MESSAGE, 'No array allowed for Sep String'
if (SIZE(str, /type) NE 7) then MESSAGE, 'Invalid Input String.'
if (SIZE(sep, /type) NE 7) then MESSAGE, 'Invalid Sep String.'
;
if KEYWORD_SET(escape) then begin
    MESSAGE, /continue, 'Sorry, this keyword ESCAPE is not implemented (no effect)'
    MESSAGE, /continue, 'PLEASE CONTRIBUTE'
endif

if (STRLEN(sep) EQ 1) then res=STRSPLIT(str,sep, /extract)
if (STRLEN(sep) GT 1) then res=STRSPLIT(str,sep, /regex, /extract)
;
nb_parts=N_ELEMENTS(res)
;
if KEYWORD_SET(trim) then begin
    for i=0,nb_parts-1 do res[ii]=STRTRIM(res[ii], 2)
endif
if KEYWORD_SET(remove_all) then begin
    for i=0,nb_parts-1 do res[ii]=STRCOMPRESS(res[ii], /Remove_All)
endif
;
if KEYWORD_SET(test) then STOP
;
return, res
;
end
;
; --------- original code by Joel ------------
; seems to be OK only when : SRTLEN(sep) == 1
;
function OLD_STR_SEP, str, sep
  on_error, 2

  if n_params() ne 2 then message, 'Two parameters required.'

  d = strsplit(str, sep)
  d = [d, strlen(str)+1]
  n = n_elements(d) - 1

  res = strarr(n)

  for i=0,n-1 do begin
    res[i] = strmid(str, d[i], d[i+1]-d[i]-1)
  endfor

  return, res
end
