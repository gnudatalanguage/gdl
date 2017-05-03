;+
; NAME:  JSON_SERIALIZE
; 
; PURPOSE:
; 
; This function returns the JSON description of a LIST, HASH or STRUCTURE (or arrays thereof).
; JSON is a light exchange format for data, easy to read by humand and computers alike.
;
; SYNTAX:
; 
; string = JSON_SERIALIZE(something, /LOWERCASE)
;
; RETURN VALUE:
;
; a string
;
; ARGUMENTS:
;
; something:  A valid STRUCUTRE, HASH or LIST, or array of them
;
; KEYWORDS:
;
; - LOWERCASE : currently unsupported.
;
; EXAMPLES:
;
; GDL> struct={ Planet: "Saturn", image: "testsuite:Saturn.jpg", weight:95.159 , units:"Earth Mass" } 
; GDL> print, json_serialize(struct)
; {"PLANET":"Saturn","IMAGE":"testsuite:Saturn.jpg","WEIGHT":      95.1590,"UNITS":"Earth Mass"}
;
; MODIFICATION HISTORY:
;   03-May-2017 : written by G. Duvert
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
function json_serialize,value,tagname=tagname,lowercase=lowercase

COMPILE_OPT idl2, HIDDEN
ON_ERROR, 2

  ret=size(value)
  ndim=ret[0]
  type=ret[ndim+1]

  if( n_elements(tagname) gt 0) then tmpstr='"'+tagname+'":' else tmpstr=''

  n=n_elements(value)

  ; unfortunately lists etc are seen as arrays by 'size', so:
  mytype=typename(value)
  if (mytype eq 'LIST' or mytype eq 'HASH' or mytype eq 'ORDEREDHASH' or mytype eq 'DICTIONARY')  then n=1 ; else 'OBJREF' 

  if (n eq 1 ) then begin
      case type of

        8: begin
           tagn=tag_names(value)
        tmpstr+='{'
        for j=0,n_tags(value)-1 do begin
           tmpstr+=json_serialize(tagname=tagn[j],value.(j))
           if (j lt n_tags(value)-1 ) then tmpstr+=","
        end
        tmpstr+='}'
     end
        
     11: begin                  ; more tricky depending on single value of array
        mytype=typename(value)
        if (mytype eq 'LIST') then begin
           tmpstr+='['
           nn=value.Count()
           for j=0,nn-1 do begin
              tmpstr+=json_serialize(value[j])
              if (j lt nn-1) then tmpstr+=","
           endfor
           tmpstr+=']'
        endif else begin
           tmpstr+='{'
           nn=value.Count()
           keys=value.Keys()
           for j=0,nn-1 do begin
              tmpstr+=json_serialize(tagname=keys[j],value[j])
              if (j lt nn-1) then tmpstr+=","
           endfor
           tmpstr+='}'
        endelse
     end

     7:  tmpstr+='"'+value+'"'
     
     10:  message,"Type POINTER not allowed with JSON."
     0:  message,"Type UNDEFINED not allowed with JSON."
     6: message,"Type COMPLEX not allowed with JSON."
     9: message,"Type DCOMPLEX not allowed with JSON."     
     ELSE: tmpstr+=string(value)
  endcase


  endif else begin
     tmpstr+="["
     for i=0,n-1 do begin
        case type of
           8: begin
              tagn=tag_names((value[i]))
              tmpstr+='{'
              for j=0,n_tags((value[i]))-1 do begin
                 tmpstr+=json_serialize(tagname=tagn[j],(value[i]).(j))
                 if (j lt n_tags((value[i]))-1 ) then tmpstr+=","
              end
              tmpstr+='}'
           end
           
           11: begin            ; more tricky depending on single value of array
              mytype=typename((value[i]))
              if (mytype eq 'LIST') then begin
                 tmpstr+='['
                 nn=(value[i]).Count()
                 for j=0,nn-1 do begin
                    tmpstr+=json_serialize((value[i])[j])
                    if (j lt nn-1) then tmpstr+=","
                 endfor
                 tmpstr+=']'
              endif else begin
                 tmpstr+='{'
                 nn=(value[i]).Count()
                 keys=(value[i]).Keys()
                 for j=0,nn-1 do begin
                    tmpstr+=json_serialize(tagname=keys[j],(value[i])[j])
                       if (j lt nn-1) then tmpstr+=","
                    endfor
                 tmpstr+='}'
              endelse
           end

           7:  tmpstr+='"'+(value[i])+'"'
           
           10:  message,"Type POINTER not allowed with JSON."
           0:  message,"Type UNDEFINED not allowed with JSON."
           6: message,"Type COMPLEX not allowed with JSON."
           9: message,"Type DCOMPLEX not allowed with JSON."
           
           
           ELSE: tmpstr+=string((value[i])[i])
        endcase
        if (i lt n-1) then tmpstr+=","
     endfor
     tmpstr+="]"
  endelse
  return,tmpstr
end

;function json_serialize_sub_list,value
;  print,"LIST"
;  return,'!'
;end
;
;
;function json_serialize,value,lowercase=lowercase
;
;  ret=size(value)
;  ndim=ret[0]
;  if (ndim gt 1) then message,'aaaaaaaaa pas booooooonn'
;  type=ret[ndim+1]
;  mytype=typename(value)
;  if (type ne 11 and type ne 8) then message,'aaaaaaaaa pas booooooonn'
;  if (type eq 11 and (mytype ne 'LIST' and mytype ne 'HASH' and mytype NE 'ORDEREDHASH' and mytype ne 'DICTIONARY'))  then message,'aaaaaaaaa pas booooooonn'
;  if type eq 8 then begin
;    tagn=tag_names(value)
;    tmpstr="{"
;    for i=0,n_tags(value)-1 do begin
;       tmpstr+=json_serialize_sub_struct_or_hash(tagn[i],value.(i))
;       if (i lt n_tags(value)-1) then tmpstr+=","
;    end
;    tmpstr+="}"
;    return, tmpstr
;  endif
;;  if mytype eq 
;;  if mytype eq LIST then begin
;;  endif else if mytype eq  then begin
;  return, 0
;end
