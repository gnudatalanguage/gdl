;+
; NAME: STR_SEP
;
; PURPOSE:
;  Obsolete routine for string separation
;  (was: Wrapper for STRSPLIT (but STRSPLIT is no longer used))
;
; MODIFICATION HISTORY:
;   01-Sep-2006 : written by Joel Gales
;   14-Apr-2010 : Alain Coulais : complete re-writing
;   18-Jun-2010 : Alain Coulais and Lea N.: mandatory usage of /preserve_null
;   24-Sep-2013 : Marc Schellens TEST_STR_SEP compliant version 
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
;if (SIZE(str, /type) NE 7) then MESSAGE, 'Invalid Input String.'
;if (SIZE(sep, /type) NE 7) then MESSAGE, 'Invalid Sep String.'
if KEYWORD_SET(escape) then begin
    MESSAGE, /continue, 'Sorry, this keyword ESCAPE is not implemented (no effect)'
    MESSAGE, /continue, 'PLEASE CONTRIBUTE'
endif

;if (STRLEN(sep) EQ 1) then res=STRSPLIT(str,sep, /extract, /preserve_null)
;if (STRLEN(sep) GT 1) then res=STRSPLIT(str,sep, /regex, /extract, /preserve_null)

sPos = 0L
if strlen(sep) EQ 0 OR strlen(str) EQ 0 then begin

   res = [str]

endif else if strlen(sep) EQ 1 then begin	

    sepPos = where(byte(str) eq (byte(sep))[0], nPos)
    res = strarr(nPos+1)
    for i=0, nPos-1 do begin
	res[i] = strmid(str, sPos, sepPos[i]-sPos)
	sPos = sepPos[i] + 1
    endfor
    res[nPos] = strmid(str, sPos, strlen(str)) 

endif else begin		

    ;; make array large enough
    nMax = strlen(str) / strlen(sep) + 1
    res = strarr( nMax)
    actIx = 0
    repeat begin

      pos = strpos (str, sep, sPos)
      if pos ge 0 then begin
         res[actIx] = strmid (str, sPos, pos-sPos) 
      endif else begin
         res[actIx] = strmid(str, sPos, strlen(str))
         break
      endelse

      sPos = pos + strlen(sep)
      ++actIx
      if actIx GE nMax then begin
         MESSAGE, 'Internal error: Endless loop detected'
      endif
    endrep until 0 ;; forever
    ;; cut to actual size
    res = res[0:actIx]
endelse
;
;;nb_parts=N_ELEMENTS(res)
;
if KEYWORD_SET(trim) then begin
;;    for i=0,nb_parts-1 do res[ii]=STRTRIM(res[ii], 2)
    res = STRTRIM(res,2)
endif
if KEYWORD_SET(remove_all) then begin
;;    for i=0,nb_parts-1 do res[ii]=STRCOMPRESS(res[ii], /Remove_All)
    res = STRCOMPRESS(res, /Remove_All)
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
;
; --------- please add tests here ------------
;
pro TEST_STR_SEP, test=test
;
resu1=STR_SEP('../foo.txt','.')
if n_elements(resu1) NE 4 then MESSAGE, 'problem 1'
if resu1[3] NE '/foo' then MESSAGE, 'problem 1bis'
if resu1[4] NE 'txt' then MESSAGE, 'problem 1ter'
MESSAGE, /continue, 'First test OK'
;
if KEYWORD_SET(test) then STOP
;
end
