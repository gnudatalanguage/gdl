;+
; NAME: IDL_VALIDNAME
;
; PURPOSE:
;	quick n dirty replacement for idl_validname
;	THIS IS JUST A DRAFT (but working)
;
; MODIFICATION HISTORY:
; 	creation by Rene Preusker 10/2010
;       2011-Aug-18: modification by Alain Coulais :
;         adding fake keywords /CONVERT_ALL,
;         /CONVERT_SPACES for test with HealPix lib.
;
; TODO: 
;	1. include working keywords
;	2. include reserved words
;
; LICENCE:
; Copyright (C) 2010, R. Preusker, 2011, A. Coulais
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
;-

function IDL_VALIDNAME_REPLACE_SPACE, in
out=in
while 1 do begin
    res= STRPOS(out," ")
    if res eq -1 then return,out
    STRPUT, out,"_",res
endwhile
end
;
; --------------------------------
;
function IDL_VALIDNAME_REPLACE_STRING, in, was, womit
out=in
while 1 do begin
    res= STRPOS(out,was)
    if res eq -1 then return,out
    STRPUT,out,womit,res
endwhile
end
;
; --------------------------------
;
function IDL_VALIDNAME, in, $
                        convert_all=convert_all, convert_spaces=convert_spaces
out=in
verboten=["#",":",".","/","+","-","*","!","$",";",'"',"&",":",",","?","@","\",")","(","]","[","{","}"]
out=IDL_VALIDNAME_REPLACE_SPACE(out)
for i=0,N_ELEMENTS(verboten)-1 do begin
    out=IDL_VALIDNAME_REPLACE_STRING(out,verboten(i),"_")
endfor
first_char=STRMID(out,0,1)
;;
;;  number is not allowed as first char
if STREGEX(out,'^[0-9]') eq 0 then out="N"+out
;
return, out
;
end
