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
   CATCH, Error_status
   IF Error_status NE 0 THEN BEGIN
      CATCH, /CANCEL
      MESSAGE, /REISSUE_LAST
      RETALL
   ENDIF

  ret=size(value)
  ndim=ret[0]
  type=ret[ndim+1]

  if( n_elements(tagname) gt 0) then tmpstr='"'+tagname+'":' else tmpstr=''

  n=n_elements(value)

  ; unfortunately lists etc are seen as arrays by 'size', so:
  mytype=typename(value)
  if (mytype eq 'LIST' or mytype eq 'HASH' or mytype eq 'ORDEREDHASH' or mytype eq 'DICTIONARY' or mytype eq 'UNDEFINED' or mytype eq 'POINTER' or mytype eq 'COMPLEX' or mytype eq 'DCOMPLEX')  then n=1 ; else 'OBJREF' 

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
     ELSE: tmpstr+=strtrim(value,2)
  endcase


  endif else begin
  s=size(value)
  ndim=s[0]
  odim=8-ndim
  dims=s[1:ndim]
  if (odim gt 0) then begin & oval=intarr(odim)+1 & dims=[dims,oval] & end
  dims-- ; superfluous sizes==0
    tmpstr+="["
    for i6=0,dims[6] do begin
     if (dims[6] gt 0) then tmpstr+="["
    for i5=0,dims[5] do begin
     if (dims[5] gt 0) then tmpstr+="["
    for i4=0,dims[4] do begin
     if (dims[4] gt 0) then tmpstr+="["
    for i3=0,dims[3] do begin
     if (dims[3] gt 0) then tmpstr+="["
    for i2=0,dims[2] do begin
     if (dims[2] gt 0) then tmpstr+="["
     for i1=0,dims[1] do begin
       if (dims[1] gt 0) then tmpstr+="["
       thisval=value[*,i1,i2,i3,i4,i5,i6]
       nel=n_elements(thisval)
       for i=0,nel-1 do begin
        case type of
           8: begin
              tagn=tag_names((thisval[i]))
              tmpstr+='{'
              for j=0,n_tags((thisval[i]))-1 do begin
                 tmpstr+=json_serialize(tagname=tagn[j],(thisval[i]).(j))
                 if (j lt n_tags((thisval[i]))-1 ) then tmpstr+=","
              end
              tmpstr+='}'
           end
           
           11: begin            ; more tricky depending on single thisval of array
              mytype=typename((thisval[i]))
              if (mytype eq 'LIST') then begin
                 tmpstr+='['
                 nn=(thisval[i]).Count()
                 for j=0,nn-1 do begin
                    tmpstr+=json_serialize((thisval[i])[j])
                    if (j lt nn-1) then tmpstr+=","
                 endfor
                 tmpstr+=']'
              endif else begin
                 tmpstr+='{'
                 nn=(thisval[i]).Count()
                 keys=(thisval[i]).Keys()
                 for j=0,nn-1 do begin
                    tmpstr+=json_serialize(tagname=keys[j],(thisval[i])[j])
                       if (j lt nn-1) then tmpstr+=","
                    endfor
                 tmpstr+='}'
              endelse
           end

           7:  tmpstr+='"'+(thisval[i])+'"'
           
           10:  message,"Type POINTER not allowed with JSON."
           0:  message,"Type UNDEFINED not allowed with JSON."
           6: message,"Type COMPLEX not allowed with JSON."
           9: message,"Type DCOMPLEX not allowed with JSON."
           
           
           ELSE: tmpstr+=strtrim(thisval[i],2)
        endcase
        if (i lt nel-1) then tmpstr+=","
       endfor
       if (dims[1] gt 0) then tmpstr+="]"
       if (i1 lt dims[1]) then tmpstr+=","
     endfor
     if (dims[2] gt 0) then tmpstr+="]"
     if (i2 lt dims[2]) then tmpstr+=","
     endfor
     if (dims[3] gt 0) then tmpstr+="]"
     if (i3 lt dims[3]) then tmpstr+=","
     endfor
     if (dims[4] gt 0) then tmpstr+="]"
     if (i4 lt dims[4]) then tmpstr+=","
     endfor
     if (dims[5] gt 0) then tmpstr+="]"
     if (i5 lt dims[5]) then tmpstr+=","
     endfor
     if (dims[6] gt 0) then tmpstr+="]"
     if (i6 lt dims[6]) then tmpstr+=","
     endfor
   tmpstr+="]"
 endelse
  return,tmpstr
end
