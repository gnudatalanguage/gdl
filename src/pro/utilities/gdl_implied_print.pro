;+
; NAME:  GDL_IMPLIED_PRINT
; 
; PURPOSE:
; print something like the 'implied_print' option of 'print' 
; 
; MODIFICATION HISTORY:
;   29-Jun-2021 : written by G. Duvert
;   03-Jan-2022 : print to 'out' as implied_print is possible with "printf'
;
; LICENCE:
; Copyright (C) 2017, G. Duvert
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
;-
;
function pretty_serialize,value,tagname=tagname,flat=flat
common json_serialize_gdl_level, level
COMPILE_OPT idl2, HIDDEN
ON_ERROR,2
   ;; CATCH, Error_status
   ;; IF Error_status NE 0 THEN BEGIN
   ;;    CATCH, /CANCEL
   ;;    MESSAGE, /REISSUE_LAST
   ;;    RETALL
   ;; ENDIF
if (n_elements(level) eq 0) then begin
   level=-1
endif
space=''
for i=0,level do space+='    '
if keyword_set(flat) then begin
   braceright='}'
   SquareBraceright=']'
   braceleft='{'
   SquareBraceleft='['
   comma=','
endif else begin
   braceright=string(10B)+space+'}'
   SquareBraceright=string(10B)+space+']'
   braceleft='{'+string(10B)
   SquareBraceleft='['+string(10B)
   comma=','+string(10B)
endelse


  ret=size(value)
  ndim=ret[0]
  type=ret[ndim+1]

if( n_elements(tagname) gt 0) then begin
   if (typename(tagname) eq "STRING") then tmpstr=space+'"'+tagname+'": ' else tmpstr=space+strtrim(tagname,2)+': '
endif else tmpstr=''

 n=n_elements(value)

 ;; unfortunately lists etc are seen as arrays by 'size', so:
  mytype=typename(value)
  if (mytype eq 'LIST' or mytype eq 'HASH' or mytype eq 'ORDEREDHASH' or mytype eq 'DICTIONARY')  then n=1
 
  if (n eq 1 ) then begin
      case type of
        8: begin
           tagn=tag_names(value)
           tmpstr+=braceleft & level++
           for j=0,n_tags(value)-1 do begin
              tmpstr+=pretty_serialize(tagname=tagn[j],value.(j), /flat)
              if j lt n_tags(value)-1  then tmpstr+=comma
           endfor
           tmpstr+=braceright & level--
        end
        
     11: begin                  ; more tricky depending on single value of array
        mytype=typename(value)
        if (mytype eq 'LIST') then begin
           tmpstr+=SquareBraceleft & level++
           nn=value.Count()
           for j=0,nn-1 do begin
              tmpstr+=pretty_serialize(value[j])
              if (j lt nn-1) then tmpstr+=comma
           endfor
           tmpstr+=SquareBraceright & level --
        endif else begin
           tmpstr+=braceleft & level++
           nn=value.Count()
           keys=value.Keys()
           for j=0,nn-1 do begin
              tmpstr+=pretty_serialize(tagname=keys[j],value[keys[j]],/flat)
              if (j lt nn-1) then tmpstr+=comma
           endfor
           tmpstr+=braceright & level--
        endelse
     end

     7:  tmpstr+='"'+value+'"'
     10: BEGIN
        ind=PTR_VALID(value,/get)
        tmpstr+="<PtrHeapVar"+strtrim(ind,2)+">"
     END
     1:  tmpstr+=strtrim(string(value,/print),2)
     0:  tmpstr+="!NULL"
     ELSE: tmpstr+=strtrim(string(value),2)
  endcase


  endif else begin
     nel=n_elements(value)
     if nel gt 1 then tmpstr+='['
       for i=0,nel-1 do begin
        case type of
           8: begin
              tagn=tag_names(value[i])
              tmpstr+=braceleft & level++
              for j=0,n_tags(value[i])-1 do begin
                 tmpstr+=pretty_serialize(tagname=tagn[j],value[i].(j),/flat)
                 if (j lt n_tags(value[i])-1 ) then tmpstr+=comma
              end
              tmpstr+=braceright & level--
           end
           
           11: begin            ; more tricky depending on single value of array
              mytype=typename(value[i])
              if (mytype eq 'LIST') then begin
                 tmpstr+=SquareBraceleft & level++
                 nn=value[i].Count()
                 for j=0,nn-1 do begin
                    tmpstr+=pretty_serialize((value[i])[j])
                    if (j lt nn-1) then tmpstr+=comma
                 endfor
                 tmpstr+=SquareBraceright & level--
              endif else begin
                 tmpstr+=braceleft & level++
                 nn=value[i].Count()
                 keys=value[i].Keys()
                 for j=0,nn-1 do begin
                    tmpstr+=pretty_serialize(tagname=keys[j],(value[i])[keys[j]],/flat)
                    if (j lt nn-1) then tmpstr+=comma
                 endfor
                 tmpstr+=braceright & level--
              endelse
           end
           7:  tmpstr+='"'+value[i]+'"'
           10: BEGIN
              ind=PTR_VALID(value[i],/get)
              tmpstr+="<PtrHeapVar"+strtrim(ind,2)+">"
           END 
           1:  tmpstr+=strtrim(string(value[i],/print),2)
           0:  tmpstr+="!NULL"
           ELSE: tmpstr+=strtrim(string(value[i]),2)
        endcase
        if (i lt nel-1) then tmpstr+=(comma+space)
       endfor
       if nel gt 1 then tmpstr+=']'
 endelse
  return,tmpstr
end

pro gdl_implied_print,out,value
COMPILE_OPT idl2, HIDDEN
ON_ERROR, 2
; get info on out
info=fstat(out)
if info.isatty eq 1 then begin
   width=(TERMINAL_SIZE( ))[0]
endif else width=132

types=[1,2,3,4,5,6,7,9,12,13,14,15]
start='('
fmtflt='(G16.8))'
fmtdbl='(G25.17))'
fmtcplx='("(",G16.8,",",G16.8,")"))'
fmtcplxdbl='("(",G25.17,",",G25.17,")"))'
type=size(value,/type)
if (type eq 0) then begin
 printf,out,"!NULL"
 return
endif

w=where(type eq types, count)
if (count gt 0) then begin
  ret=size(value)
  ndim=ret[0]
  type=ret[ndim+1]
  n=n_elements(value)
  n_firstdim=ret[1]
  case type of  ; define special formats
    4: begin & w=16 & n=fix(width/w,/print) & if n_firstdim lt n then n=n_firstdim & fmt=start+strtrim(n,2)+fmtflt & break &end
    5: begin & w=25 & n=fix(width/w,/print) & if n_firstdim lt n then n=n_firstdim & fmt=start+strtrim(n,2)+fmtdbl & break &end
    6: begin & w=35 & n=fix(width/w,/print) & if n_firstdim lt n then n=n_firstdim & fmt=start+strtrim(n,2)+fmtcplx & break &end
    9: begin & w=53 & n=fix(width/w,/print) & if n_firstdim lt n then n=n_firstdim & fmt=start+strtrim(n,2)+fmtcplxdbl & break &end
    ELSE:  begin &  printf,out,value & return & end ; just print and return
    endcase
  ; only special formats left
  switch type of
    4: 
    5: 
    6: 
    9: printf,out,value,format=fmt
 endswitch
 return
endif

text=pretty_serialize(value)

printf,out,text

end

