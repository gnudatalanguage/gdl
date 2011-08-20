;+
; NAME: IDL_VALIDNAME
;
; PURPOSE:
;	quick n dirty replacement for idl_validname
;	THIS IS JUST A DRAFT (but working)
;
; MODIFICATION HISTORY:
;   - creation by Rene Preusker 10/2010
;   - 2011-Aug-18: modification by Alain Coulais :
;         adding FAKE keywords /CONVERT_ALL,
;         /CONVERT_SPACES for test with HealPix lib.
;   - 2011-Aug-20: Alain: implement draft of CONVERT_SPACES
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
function IDL_VALIDNAME, in, convert_all=convert_all, $
                        convert_spaces=convert_spaces
;
print, "sorry, not finish"
;
out=in
;
spezial=[" ","$","!"]
verboten=[spezial,"#",":",".","/","+","-","*",";",'"',"&",":"]
verboten=[verboten,",","?","@","\",")","(","]","[","{","}"]
;
nb_verbot=N_ELEMENTS(verboten)
;
if NOT(KEYWORD_SET(convert_all) OR KEYWORD_SET(convert_spaces)) then begin
   for i=0, N_ELEMENTS(out)-1 do begin
      first_char=STRMID(out[i],0,1)
      if (STREGEX(first_char,'[0-9]') EQ 0) then begin
         out[i]=''
         break
      endif
      for j=0, nb_verbot-1 do begin
         if (STRPOS(out[i], verboten[j]) GE 0) then begin
            out[i]=''
            break
         endif
      endfor
   endfor
   return, out
endif
;
if KEYWORD_SET(convert_spaces) AND NOT(KEYWORD_SET(convert_all)) then begin
   for i=0, N_ELEMENTS(out)-1 do begin
      first_char=STRMID(out[i],0,1)
      if (STREGEX(first_char,'[0-9]') EQ 0) then begin
         out[i]=''
         break
      endif
      out[i]=IDL_VALIDNAME_REPLACE_SPACE(out[i])
   endfor
endif
;
if KEYWORD_SET(convert_all) then begin
   for i=0, N_ELEMENTS(out)-1 do begin
      first_char=STRMID(out[i],0,1)
      if (STREGEX(first_char,'[0-9]') EQ 0) then begin
         out[i]='_'+out[i]
      endif
      for i=0,N_ELEMENTS(verboten)-1 do begin
         out=IDL_VALIDNAME_REPLACE_STRING(out,verboten(i),"_")
      endfor
   endfor
endif
;
return, out
;
end
