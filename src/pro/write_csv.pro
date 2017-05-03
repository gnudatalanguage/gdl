;+
; NAME:
;    WRITE_CSV
;
; PURPOSE:
;    Writes a comma-separated-variables (CSV) file from a structure, array or arrays.
;    
;
; CATEGORY:
;    I/O
;
; CALLING SEQUENCE:
;    WRITE_CSV, Filename, struct | 2d_array | [col1, col2, ... col8],
;    Header=header, table_header=table_header
;
; INPUTS:
;    Filename:  Name of CSV file to write to.
;
;    struct: Structure containing data. If there are N fields, then
;               the first N tags in the structure must be the data fields
;               followed by 2 longs containing NROWS and NCOLS.
;  OR:
;    2d_array: each column will become a separate comumn in the csv file
;  OR:
;     up to 8 1-D arrays (can be of different types), each become a column
;
; OUTPUTS:
;    Writes out a comma-separated-variables (CSV) file with header
;    (1st line) taken from the TAG Names of structs, or, if present,
;    by the HEADER parameter (which should be a string array of the
;    right size). 
;
; EXAMPLE:
; GDL> struct={ Planet: "Saturn", image: "testsuite:Saturn.jpg", weight:95.159 , units:"Earth Mass" }
; GDL> write_csv,"test.csv",struct
; GDL> $cat test.csv
; "PLANET","IMAGE","WEIGHT","UNITS"
; "Saturn","testsuite:Saturn.jpg",95.1590,"EarthMass"
;
; BUGS:
; HEADER and TABLE_HEADER not supported, fixme, it's easy.
;
; MODIFICATION HISTORY:
;    Written by:  Gilles Duvert
;    03 May 2017: first version
;-
; LICENCE:
; Copyright (C) 2017
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;-
pro write_csv, file, input1, input2, input3, input4, input5, input6, input7, input8,  header=header, table_header=tableheader

  compile_opt idl2

  ON_ERROR, 2                   ;Return on error

  ON_IOERROR, error_io

; check a few errors:
; - number of parameters
  if (n_params() le 1) then message,"Incorrect number of arguments."
;-filename defined string:
  if (size(file,/type) ne 7) then message, 'Filename must be a string.'

  typestruct=isa(input1,"STRUCT") 
  typearray=Size(input1, /N_DIM) eq 2

  if ((typestruct or typearray) and n_params() gt 2) then message, 'Too many parameters.'

  if (typestruct) then begin

     header_names = tag_names(input1)
     ncols=n_elements(header_names)
     nrows=n_elements(input1.(0))
     openw, lun, file, /get_lun
     strline=''
     for j=0L,ncols-1 do begin ; must quote strings but protect quotes inside
        a = strsplit(header_names[j],'"',/extract,/preserve_null)
        b = a[0]
        for k=1,n_elements(a)-1 do b+='""'+a[k] 
        strline += '"'+b+'"'
        if j ne ncols-1 then strline +=','
     endfor
     printf, lun, strcompress(strline,/remove_all)

     for i=0L,nrows-1 do begin
        strline=''
        for j=0L,ncols-1 do begin
           switch size(input1.(j), /type) of
              7 : begin         ; must quote strings but protect quotes inside
                 a = strsplit(input1.(j)[i],'"',/extract,/preserve_null)
                 b = a[0]
                 for k=1,n_elements(a)-1 do b+='""'+a[k] 
                 strline += '"'+b+'"'
                 break
              end

              6:                ; passthru
              9: begin
                 strline += '"'+string(input1.(j)[i])+'"'
                 break
              end

              ELSE: strline += string(input1.(j)[i])
           endswitch
           if j ne ncols-1 then strline +=','
        endfor
        printf, lun, strcompress(strline,/remove_all)
     endfor
     
     close, lun
     free_lun, lun
     return
  endif else if (typearray) then begin
     z=size(input1)
     ncols=z[1]
     nrows=z[2]
     openw, lun, file, /get_lun
     switch z[3] of
        7 : begin
           for i=0L,nrows-1 do begin
              strline=''
              for j=0L,ncols-1 do begin
                 a = strsplit(input1[j,i],'"',/extract,/preserve_null)
                 b = a[0]
                 for k=1,n_elements(a)-1 do b+='""'+a[k] 
                 strline += '"'+b+'"'
                 if j ne ncols-1 then strline +=','
              endfor
              printf, lun, strcompress(strline,/remove_all)
           endfor
           break
        end

        6: ; passthru
        9: begin
           for i=0L,nrows-1 do begin
              strline=''
              for j=0L,ncols-1 do begin
                 strline += '"'+string(input1[j,i])+'"'
                 if j ne ncols-1 then strline +=','
              endfor
              printf, lun, strcompress(strline,/remove_all)
           endfor
           break
        end

        ELSE: for i=0L,nrows-1 do begin
           strline=''
           for j=0L,ncols-1 do begin
              strline += string(input1[j,i])
              if j ne ncols-1 then strline +=','
           endfor
           printf, lun, strcompress(strline,/remove_all)
       endfor
     endswitch
     close, lun
     free_lun, lun
     return
  endif else begin
     ncols=n_params()-1
     nrows=n_elements(input1)
     openw, lun, file, /get_lun
     ptr=ptrarr(ncols,/allocate_heap)
     mess='Data fields must all have the same number of elements.'
     switch (ncols) of
        8: if (n_elements(input8) ne nrows) then message,mess else ptr[7]=ptr_new(input8)
        7: if (n_elements(input7) ne nrows) then message,mess else ptr[6]=ptr_new(input7)
        6: if (n_elements(input6) ne nrows) then message,mess else ptr[5]=ptr_new(input6)
        5: if (n_elements(input5) ne nrows) then message,mess else ptr[4]=ptr_new(input5)
        4: if (n_elements(input4) ne nrows) then message,mess else ptr[3]=ptr_new(input4)
        3: if (n_elements(input3) ne nrows) then message,mess else ptr[2]=ptr_new(input3)
        2: if (n_elements(input3) ne nrows) then message,mess else ptr[1]=ptr_new(input2)
        1: ptr[0]=ptr_new(input1)
     endswitch

     for i=0,nrows-1 do begin
        strline=''
        for j=0,ncols-1 do begin
           z=size(*ptr[j])
           switch z[2] of
              7 : begin         ; must quote strings but protect quotes inside
                 a = strsplit((*ptr[j])[i],'"',/extract,/preserve_null)
                 b = a[0]
                 for k=1,n_elements(a)-1 do b+='""'+a[k] 
                 strline += '"'+b+'"'
                 break
              end

              6:
              9: begin
                 strline += '"'+string((*ptr[j])[i])+'"'
                 break
              end

              ELSE: strline += string((*ptr[j])[i])
           endswitch
           if j ne ncols-1 then strline +=','
        endfor
        printf, lun, strcompress(strline,/remove_all)
     endfor
     close, lun
     free_lun, lun
     return
  endelse

error_io:
  ON_IOERROR, null
  if (n_elements(lun) gt 0) then free_lun, lun
  message, !error_state.msg
end

