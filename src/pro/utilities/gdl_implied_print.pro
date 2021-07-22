;+
; NAME:  GDL_IMPLIED_PRINT
; 
; PURPOSE:
; print something like the 'implied_print' option of 'print' 
; 
; MODIFICATION HISTORY:
;   29-Jun-2021 : written by G. Duvert
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
function pretty_serialize,value,tagname=tagname,arrayIdentifier=arrayIdentifier
common json_serialize_gdl_level, level, types

COMPILE_OPT idl2, HIDDEN
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
braceright=string(10B)+space+'}'
XXright=string(10B)+space+']'
braceleft='{'+string(10B)
XXleft='['+string(10B)
comma=','+string(10B)
arrayIdentifier=keyword_set(arrayIdentifier)
  ret=size(value)
  ndim=ret[0]
  type=ret[ndim+1]

  if( n_elements(tagname) gt 0) then tmpstr=space+'"'+tagname+'": ' else tmpstr=''
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
           tmpstr+=pretty_serialize(tagname=tagn[j],value.(j),/arrayIdentifier)
           if (j lt n_tags(value)-1 ) then tmpstr+=comma
        end
        tmpstr+=braceright & level--
     end
        
     11: begin                  ; more tricky depending on single value of array
        mytype=typename(value)
        if (mytype eq 'LIST') then begin
           tmpstr+=XXleft & level++
           nn=value.Count()
           for j=0,nn-1 do begin
              tmpstr+=pretty_serialize(value[j])
              if (j lt nn-1) then tmpstr+=comma
           endfor
           tmpstr+=XXright & level --
        endif else begin
           tmpstr+=braceleft & level++
           nn=value.Count()
           keys=value.Keys()
           for j=0,nn-1 do begin
              tmpstr+=pretty_serialize(tagname=keys[j],value[j])
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
   if (arrayIdentifier) then  tmpstr+="["
       nel=n_elements(value)
       for i=0,nel-1 do begin
        case type of
           8: begin
              tagn=tag_names((value[i]))
              tmpstr+='{'
              for j=0,n_tags((value[i]))-1 do begin
                 tmpstr+=pretty_serialize(tagname=tagn[j],(value[i]).(j),/arrayIdentifier)
                 if (j lt n_tags((value[i]))-1 ) then tmpstr+=comma
              end
              tmpstr+='}'
           end
           
           11: begin            ; more tricky depending on single value of array
              mytype=typename((value[i]))
              if (mytype eq 'LIST') then begin
                 tmpstr+='['
                 nn=(value[i]).Count()
                 for j=0,nn-1 do begin
                    tmpstr+=pretty_serialize((value[i])[j])
                    if (j lt nn-1) then tmpstr+=comma
                 endfor
                 tmpstr+=']'
              endif else begin
                 tmpstr+=braceleft & level++
                 nn=(value[i]).Count()
                 keys=(value[i]).Keys()
                 for j=0,nn-1 do begin
                    tmpstr+=pretty_serialize(tagname=keys[j],(value[i])[j])
                    if (j lt nn-1) then tmpstr+=comma
                 endfor
                 tmpstr+=braceright & level--
              endelse
           end
           7:  tmpstr+='"'+(value[i])+'"'
           10: BEGIN
              ind=PTR_VALID(value[i],/get)
              tmpstr+="<PtrHeapVar"+strtrim(ind,2)+">"
           END 
           1:  tmpstr+=strtrim(string(value[i],/print),2)
           0:  tmpstr+="!NULL"
           ELSE: tmpstr+=strtrim(string(value[i]),2)
        endcase
        if (i lt nel-1) then tmpstr+=', '
       endfor
   if (arrayIdentifier) then tmpstr+="]"
 endelse
  return,tmpstr
end

pro gdl_implied_print,value
COMPILE_OPT idl2, HIDDEN
ON_ERROR, 2
types=[1,2,3,4,5,6,7,9,12,13,14,15]
type=size(value,/type)
if (type eq 0) then return
w=where(type eq types, count)
if (count gt 0) then begin
  ret=size(value)
  ndim=ret[0]
  type=ret[ndim+1]
  n=n_elements(value)
  case type of
  7: BEGIN
    for i=0,n-1 do print,value[i]
  END
  4: print,value,format='(G16.8)'
  5: print,value,format='(G25.17)'
  6: print,value,format='("(",G16.8,",",G16.8,")")'
  9: print,value,format='("(",G25.17,",",G25.17,")")'
  ELSE: print,value
  endcase
  return
endif

text=pretty_serialize(value)
print,text
end

