;+
; NAME: STR_SEP
;
; PURPOSE:
;       Serves as a wrapper around STRSPLIT
;
; MODIFICATION HISTORY:
;  01-Sep-2006 : written by Joel Gales
;  14-Apr-2010 : Alain Coulais (AC) : complete re-writing
;  18-Jun-2010 : AC and Lea N.: mandatory usage of /preserve_null
;  18-Oct-2012 : AC: new problematic cases found thanks to
;  tests done with PSM soft test suite
;  (e.g. :   print, STR_SEP('2**10 uK/J','**') )
;  --> revisited algorithm without using STRSPLIT
;
; LICENCE:
; Copyright (C) 2006, J. Gales, 2010, Lea N. and AC, 2012, A. Coulais
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
;-
;
function STR_SEP, str_input, str_separator, $
                  trim=trim, remove_all=remove_all, esc=esc, $
                  test=test, help=help
;
ON_ERROR, 2
;
if KEYWORD_SET(help) then begin
    print, 'function STR_SEP, str_input, str_separator, $'
    print, '                  trim=trim, remove_all=remove_all, esc=esc, $'
    print, '                  test=test, help=help'
    return, -1
endif
;
if (N_PARAMS() NE 2) then MESSAGE, 'Wrong number of arguments. (Two Strings expected)'

if N_ELEMENTS(str_input) NE 1 then MESSAGE, 'No array allowed for Input String'
if N_ELEMENTS(str_separator) NE 1 then MESSAGE, 'No array allowed for Sep String'
;
if KEYWORD_SET(escape) then begin
    MESSAGE, /continue, 'Sorry, this keyword ESCAPE is not implemented (no effect)'
    MESSAGE, /continue, 'PLEASE CONTRIBUTE'
endif
;
; old version, not surviving to:
;   print, STR_SEP('2**10 uK/J','**')
;if (STRLEN(str_separator) EQ 1) then res=STRSPLIT(str,str_separator, /extract, /preserve_null)
;if (STRLEN(str_separator) GT 1) then res=STRSPLIT(str,str_separator, /regex, /extract, /preserve_null)
;
; debug=1
;
if STRLEN(str_separator) EQ 0 then resu=STRING(str_input)
;
if STRLEN(str_separator) EQ 1 then begin
    resu=STRSPLIT(str_input, str_separator, /extract, /preserve_null)
endif

if STRLEN(str_separator) GT 1 then begin
    ;; AC 2012-OCT-18: we try to convert ... but only if Separator is not
    ;; single char
    ;;
    if (SIZE(str_input, /type) NE 7) then begin
        flag=EXECUTE('str_input=STRING(str_input)')
        if (flag EQ 0) then MESSAGE, 'Invalid Input String.'
    endif
    ;;
    if (SIZE(str_separator, /type) NE 7) then begin
        flag=EXECUTE('str_separator=STRING(str_separator)')
        if (flag EQ 0) then MESSAGE, 'Invalid Sep String.'
    endif   
    ;; debug=1
    resu=''
    if SIZE(str_input, /type) NE 7 then begin
       str_input = byte( str_input)
    end
    residual=STRING(str_input)
    while (STRLEN(residual) GT 0) do begin
        pos=STRPOS(residual, str_separator)
        if pos GE 0 then begin
            resu=[resu,STRMID(residual,0,pos)]
            residual=STRMID(residual,pos+STRLEN(str_separator))
            if STRLEN(residual) EQ 0 then resu=[resu,'']
            ;; if debug then print, 'pos : ', pos, '>'+residual+'<'
        endif else begin
            resu=[resu,residual]
            residual=''
        endelse
    endwhile
    ;;
    if STRLEN(str_input) GT 0 then resu=resu[1:*]
endif
;
if KEYWORD_SET(trim) then begin
    resu=STRTRIM(resu, 2)
endif else begin
    if KEYWORD_SET(remove_all) then begin
        resu=STRCOMPRESS(resu, /Remove_All)
    endif
endelse
;
if KEYWORD_SET(test) then STOP
;
return, resu
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
