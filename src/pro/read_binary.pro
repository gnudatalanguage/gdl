;+
;
; NAME: READ_BINARY
;
;
; PURPOSE: Reads an binary file. The output is a structure whose tags contains
;          the requested fields.
;
;
; CATEGORY: IO
;
;
; CALLING SEQUENCE: 
;   structure=read_binary(filename|unit, template=template, 
;                         data_start=datastart, data_type=datatypes, 
;                         data_dims=datadims, endian=endian)
;
;
; INPUT:
;   filename       Name of the ASCII file to be read
;   unit           Alternatively, unit of the filename
;
;
; KEYED INPUTS:
;   data_start     Specify the number of bytes to be skipped
;   data_type      Numeric type of the binary file
;   data_dims      Dimensions of the requested field. Allow to read only
;                  part of the file.
;   endian         'native', 'little' or 'big': byte ordering of the binary file
;
;   template       NOT IMPLEMENTED YET
;                  structure that defines how the binary file will be processed.
;                  the tags datastart, typecodes, dimensions and endian
;                  can be overridden by the keywords data_start, data_type,
;                  and data_dims and endian
;
; a binary template looks like:
;   VERSION          FLOAT           1.00000
;   TEMPLATENAME     STRING    'tmpl'
;   ENDIAN           STRING    'native'
;   FIELDCOUNT       LONG                 2
;   TYPECODES        INT       Array[2]
;   NAMES            STRING    Array[2]
;   OFFSETS          STRING    Array[2]
;   NUMDIMS          LONG      Array[2]
;   DIMENSIONS       STRING    Array[2, 8]
;   REVERSEFLAGS     BYTE      Array[2, 8]
;   ABSOLUTEFLAGS    BYTE      Array[2]
;   RETURNFLAGS      INT       Array[2]
;   VERIFYFLAGS      INT       Array[2]
;   DIMALLOWFORMULAS INT       = Array[2]
;   OFFSETALLOWFORMULAS
;                    INT       = Array[2]
;   VERIFYVALS       STRING    Array[2]
;
;
; OUTPUT:
;   output      If the template keyword is set, the output is a structure 
;               containing the fields of the binary file. Otherwise it is
;               an array of the requested dimension and type.
;
;
;
; EXAMPLES:
;   to read a binary file of floats:
;   a = read_binary(filename, type=4)
; 
;
; IDL DIFFERENCES:
;   template keyword not yet implemented
; 
;
; MODIFICATION HISTORY:
;   20-Jan-2006 : written by Pierre Chanial
;
;-
; LICENCE:
; Copyright (C) 2006, P. Chanial
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
;
;-
 

function read_binary_filesize, filename
 compile_opt hidden
 spawn, ['wc', '-c', filename], output, exit_status=status, /noshell
 if status ne 0 then message, 'Invalid filename.'
 return, long64(output[0])
end

function read_binary, input, template=template, data_start=datastart, $
                 data_type=datatypes, data_dims=datadims, endian=endian
 
 on_error, 2

 if keyword_set(template) then begin
    message, 'TEMPLATE keyword not implemented yet!'
 endif
 
 if n_elements(datastart) eq 0 then datastart = 0
 if n_elements(datatypes) eq 0 then datatypes = 1
 if n_elements(datadims)  eq 0 then datadims = -1
 if n_elements(endian)    eq 0 then endian = 'native'
 if n_elements(template)  eq 0 then fieldcount = 1
 
 nbytes = [0, 1, 2, 2, 4, 8, 8, 0, 0, 16, 0, 0, 2, 4, 8, 8]

 if size(input, /tname) eq 'STRING' then begin
    filename = input
    openr, unit, filename, /get_lun, $
           swap_if_big_endian=strcmp(endian, 'little', /fold_case), $
           swap_if_little_endian=strcmp(endian, 'big', /fold_case)
 endif else begin
    unit = input
    filename = ''
 endelse
 point_lun, unit, datastart
 
 ;----------------
 ; check filesize
 ;----------------
 filesize = read_binary_filesize(filename)-datastart
 
; 03/2006 m_schellens
 linesize = (datadims>1)*nbytes[datatypes]
; linesize = product(datadims>1)*nbytes[datatypes]
 linecount = filesize / linesize
 
 
 ;----------------------------
 ; optimize case fieldcount=1
 ;----------------------------
 if fieldcount eq 1 then begin
    if datadims[0] eq -1 then begin
       dimensions = linecount
    endif else begin
       dimensions = datadims > 1
    endelse
    result = make_array(dimension=dimensions, type=datatypes, /nozero)
    if datadims[0] eq 0 then begin
       result = result[0]
    endif
    readu, unit, result
    goto, closing
 endif
    
 ;-------------------------
 ; create output structure
 ;-------------------------


 ;----------
 ; clean up
 ;----------
closing:
 if size(filename, /tname) eq 'STRING' then free_lun, unit
 return, result
 
end
