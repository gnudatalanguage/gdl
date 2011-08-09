;+
; NAME: IDL_VALIDNAME
;
; PURPOSE:
;	quick n dirty replacement for idl_validname
;	THIS IS JUST A DRAFT (but working)
;
; MODIFICATION HISTORY:
; 	Rene Preusker 10/2010
;
; TODO: 
;	1. include keywords
;	2. include reserved words
;
; LICENCE:
; Copyright (C) 2010, R. Preusker
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
;-

function idl_validname_replace_space,in
out=in
while 1 do begin
	res= STRPOS(out," ")
	if res eq -1 then return,out
	strput,out,"_",res
endwhile
end
function idl_validname_replace_string,in,was,womit
out=in
while 1 do begin
	res= STRPOS(out,was)
	if res eq -1 then return,out
	strput,out,womit,res
endwhile
end
function idl_validname,in
	out=in
	verboten=["#",":",".","/","+","-","*","!","$",";",'"',"&",":",",","?","@","\",")","(","]","[","{","}"]
	out=idl_validname_replace_space(out)
	for i=0,n_elements(verboten)-1  do out=idl_validname_replace_string(out,verboten(i),"_")
	first_char=strmid(out,0,1)
	if stregex(out,'^[0-9]') eq 0 then out="N"+out   ; number is not allowed as first char
	return,out
end
