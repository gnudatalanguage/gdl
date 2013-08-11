;+
;
; NAME: IDL_VALIDNAME
;
; PURPOSE:
; replacement for IDL_VALIDNAME
;
; MODIFICATION HISTORY:
;   - 2010-Oct.  : creation by Rene Preusker
;   - 2011-Aug-18: modification by Alain Coulais :
;         adding FAKE keywords /CONVERT_ALL,
;         /CONVERT_SPACES for test with HealPix lib.
;   - 2011-Aug-20: Alain: implement draft of CONVERT_SPACES
;   - 2011-Aug: Hong Xu : implement "reserved words"
;   - 2013-July: Tim : better managment of "\" and "$" (patch 66)
;                Alain : "!" at first place only OK; multi inputs loop
;
; LICENCE:
; Copyright (C) 2010, R. Preusker
; Copyright (C) 2011, Alain Coulais, Hong Xu
; Copyright (C) 2013, Tim, Alain Coulais
;
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
;-
;
function IDL_VALIDNAME, in_list, $
                        convert_spaces=convert_spaces, $
                        convert_all=convert_all, $
                        help=help, test=test
;
if KEYWORD_SET(help) then begin
    print, 'function IDL_VALIDNAME, in_list, $'
    print, '                        convert_spaces=convert_spaces, $'
    print, '                        convert_all=convert_all, $'
    print, '                        help=help, test=test'
    return, -1
endif
;
reserved_words=['AND', $
                'BEGIN', $
                'BREAK', $
                'CASE', $
                'COMMON', $
                'COMPILE_OPT', $
                'CONTINUE', $
                'DO', $
                'ELSE', $
                'END', $
                'ENDCASE', $
                'ENDELSE', $
                'ENDFOR', $
                'ENDIF', $
                'ENDREP', $
                'ENDSWITCH', $
                'ENDWHILE', $
                'EQ', $
                'FOR', $
                'FORWARD_FUNCTION', $
                'FUNCTION', $
                'GE', $
                'GOTO', $
                'GT', $
                'IF', $
                'INHERITS', $
                'LE', $
                'LT', $
                'MOD', $
                'NE', $
                'NOT', $
                'OF', $
                'ON_IOERROR', $
                'OR', $
                'PRO', $
                'REPEAT', $
                'SWITCH', $
                'THEN', $
                'UNTIL', $
                'WHILE', $
                'XOR']
;
;; AC 03/11/2011
;; we need a working STREGEX to be able to run IDL_VALIDNAME
;; (please don't remove this test: if STREGEX is wrong, infinite loop below)
 ;
if KEYWORD_SET(convert_all) then begin
    if (STREGEX('1abc','[^0-9a-z]') NE -1) then begin
        MESSAGE, 'No working STREGEX, we cannot do this test.'
    endif
endif
;
out_list=in_list
;
for iii=0, n_elements(out_list)-1 do begin
    ;;
    out=out_list[iii]
    ;;
    if (out EQ '') then begin
       if KEYWORD_SET(convert_all) then out='_'
       out_list[iii]=out
       break
    endif
    ;;
    if KEYWORD_SET(convert_spaces) or KEYWORD_SET(convert_all) then begin
        while 1 do begin
            res = STRPOS(out, ' ')
            if (res EQ -1) then break
            STRPUT, out, '_', res
        endwhile
    endif
    ;;
    if KEYWORD_SET(convert_all) then begin
        ;;
        first_char = STRMID(out, 0, 1)
        ;;
        _ = WHERE(reserved_words EQ STRUPCASE(out), n)
        if (n gt 0) then out = '_' + out
        while 1 do begin
            res = STREGEX(out, '[^0-9A-Za-z_$]')
            if res EQ -1 then break
            STRPUT, out, '_', res
        endwhile
        ;; numbers and $ are not allowed as first char
        ;;
        if STREGEX(first_char, '^[0-9$]') EQ 0 then out = "_" + out
        ;; one exception: if first char is "!", is is OK
        if (first_char EQ '!') then out='!'+STRMID(out,1)
    endif else begin
        _ = WHERE(reserved_words EQ STRUPCASE(out), n)
        if (STREGEX(out, '^[0-9$]') EQ 0) or $
          STREGEX(out, '[^0-9A-Za-z_$]') ne -1 or $
          n gt 0 then out=''
    endelse
    out_list[iii]=out
endfor
;
if KEYWORD_SET(test) then STOP
;
return, out_list
;
end
