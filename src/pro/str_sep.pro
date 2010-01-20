;+
; NAME: STR_SEP
;
; PURPOSE:
;       Serves as a wrapper around STRSPLIT
;
; MODIFICATION HISTORY:
;   01-Sep-2006 : written by Joel Gales
;
; LICENCE:
; Copyright (C) 2006, J. Gales
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
;-

function str_sep, str, sep
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
