;+
; NAME: ESCAPE_SPECIAL_CHAR
;
; important Nota Bene : AC 2018 : it would be great to be able to *remove* this
; code in the future ... All "file_*" functions should be able to
; manage by them-selves the special chars.
;
; ESCAPE_SPECIAL_CHAR function escape special characteres present
; in input string argument and returns the result.
; Special characteres can be given by Keyword : list_of_special_char
; or default list is used.
;
; LICENCE:
; Copyright (C) 2010, Lea Noreskal and Alain Coulais
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.
;
; AC 2018 : we do removed "." in the list 
; but not sure if "?" or "*" should be in the list ...
;
; ------------------------
;
; Modifications history :
; - June 2010: created by Lea Noreskal
; - 28 June 2010: modified by Alain Coulais (N_PARAMS(), /help)
;
; - 2018-04-10 : AC 
;  *0* input string should be a singleton !
;  *1* manage repetition of same char ("  " --> "\ \ ")
;  *2* remove of "." in the list ...
;  *3* adding the space " " in the list (and remove bad 
;                 STRJOIN(STRSPLIT(str_out, /EXTRACT), '\ ')  )
;
;- ------------------------
;
function ESCAPE_SPECIAL_CHAR, str_in,  show_list=show_list, $
                              list_of_special_char=list_of_special_char, $
                              test=test, help=help, debug=debug, verbose=verbose
;
if N_PARAMS() NE 1 then begin
   MESSAGE, /continue, 'You must provide a input string !'
   help=1
endif
if KEYWORD_SET(help) then begin
   print, 'function ESCAPE_SPECIAL_CHAR, str_in, show_list=show_list, $'
   print, '           list_of_special_char=list_of_special_char, $'
   print, '           test=test, help=help, debug=debug, verbose=verbose'
   return, -1
endif
;
if N_ELEMENTS(str_in) GT 1 then MESSAGE, 'input string should not be an array'
;
if KEYWORD_SET(verbose) then begin
   MESSAGE, /continue, '/verbose keyword not available, please contribute'
endif
;
; list of special characteres can be modified if it is not complete
;if( not KEYWORD_SET(list_of_special_char)) then list_of_special_char ='[*^$,&\]'
;tab=STRSPLIT(str_in, list_of_special_char, /REGEX, /preserve)
;print, tab

if ~KEYWORD_SET(list_of_special_char) then list_of_special_char =' ^$,&\'
;
if KEYWORD_SET(show_list) then begin
   print, 'List of Special Chars which will be escaped :'
   print, list_of_special_char
endif
;
tab=STRSPLIT(str_in, list_of_special_char, /preserve_null)
;
str_out=''                      ;
;
for ii=0,N_ELEMENTS(tab)-1 do begin
   if (ii+1 LT N_ELEMENTS(tab)) then begin
      ;;print,ii,  ' : ', tab[ii] ,' > ', STRMID(str_in,tab[ii],tab[ii+1]-tab[ii]-1)
      if (tab[ii] GT 0) then begin
         ;;escape special char
         str_out=str_out+'\'+STRMID(str_in,tab[ii]-1,1)+STRMID(str_in,tab[ii],tab[ii+1]-tab[ii]-1)
      endif else begin
         ;;beginning of the world
         str_out=str_out+STRMID(str_in,tab[ii],tab[ii+1]-tab[ii]-1)
      endelse
   endif else begin
      ;;print, ii,  ' : ', tab[ii] ,' > ', STRMID(str_in,tab[ii])
      if(tab[ii] GT 0) then begin
         ;;escape special char
         str_out=str_out+'\'+STRMID(str_in,tab[ii]-1,1)+STRMID(str_in,tab[ii])
      endif else begin
         ;;end of  the word        
         str_out=str_out+STRMID(str_in,tab[ii])
      endelse
   endelse
endfor
;
;merger of each part and escape spaces
;str_out=STRJOIN(STRSPLIT(str_out, /EXTRACT), '\ ') 
;
if KEYWORD_SET(debug) then begin
   for ii=0, N_ELEMENTS(str_in) do begin
      print, 'Before : ', str_in[ii]
      print, 'After : ', str_out[ii]
   endfor
endif
;
if KEYWORD_SET(test) then STOP
;
return, str_out
;
end
