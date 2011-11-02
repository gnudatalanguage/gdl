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
;
; LICENCE:
; Copyright (C) 2010, R. Preusker
; Copyright (C) 2011, Alain Coulais, Hong Xu
;
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
;-
;
function IDL_VALIDNAME, in, $
                        convert_spaces=convert_spaces, $
                        convert_all=convert_all, $
                        help=help, test=test
;
if KEYWORD_SET(help) then begin
   print, 'function IDL_VALIDNAME, in, $'
   print, '                        convert_spaces=convert_spaces, $'
   print, '                        convert_all=convert_all, $'
   print, '                        help=help, test=test'
   return, -1
endif
;
if (in EQ '') then begin
   if KEYWORD_SET(convert_all) then return, '_' else return, ''
endif
;
out=in
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
if KEYWORD_SET(convert_spaces) or KEYWORD_SET(convert_all) then begin
   while 1 do begin
      res = STRPOS(out, ' ')
      if (res EQ -1) then break
      STRPUT, out, '_', res
   endwhile
endif
;
if KEYWORD_SET(convert_all) then begin
   ;;
   ;; AC 03/11/2011
   ;; we need a working STREGEX to be able to run IDL_VALIDNAME
   ;; (please don't remove this test: if STREGEX is wrong,
   ;; infinite loop below)
   if (STREGEX('1abc','[^0-9a-z]') NE -1) then begin
      MESSAGE, 'No working STREGEX, we cannot do this test.'
   endif
   ;;
   _ = WHERE(reserved_words EQ STRUPCASE(out), n)
   if (n gt 0) then out = '_' + out
   while 1 do begin
      res = STREGEX(out, '[^0-9A-Za-z_\$!]')
      if res EQ -1 then break
      STRPUT, out, '_', res
   endwhile
   ;; numbers and $ are not allowed as first char
   first_char = STRMID(out, 0, 1)
   if STREGEX(out, '^[0-9\$]') EQ 0 then out = "_" + out
endif else begin
   _ = WHERE(reserved_words EQ STRUPCASE(out), n)
   if (STREGEX(out, '^[0-9\$]') EQ 0) or $
      STREGEX(out, '[^0-9A-Za-z_\$!]') ne -1 or $
      n gt 0 then return, ''
endelse
;
if KEYWORD_SET(test) then STOP
;
return, out
;
end
