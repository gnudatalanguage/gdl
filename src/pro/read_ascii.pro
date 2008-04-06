;+
;
; NAME: READ_ASCII
;
;
; PURPOSE: Reads an ASCII file. The output is a structure whose tags contains
;          columns (1D array) from the file. Its use is flexible: the user can 
;          specify the types, and tag names of the fields and he/she also can 
;          group columns into a single tag (2D array).
;
;
; CATEGORY: IO
;
;
; CALLING SEQUENCE: 
;   structure=read_ascii(filename, count=, data_start=, delimiter=,
;                        missing_value=, comment_symbol=, record_start=
;                        num_records=, template=, header=, verbose=)
;
;
; INPUT:
;   filename       Name of the ASCII file to be read
;
;
; KEYED INPUTS:
;   data_start     Specify the number of lines that constitute the header
;                  These lines will be discarded as records, but are available
;                  with the header keyword
;
;   delimeter      If set (or non equal to ''), the records will be split 
;                  according to the supplied delimiter locations and the length
;                  of the fields is not necessarily the same for all records. 
;                  Otherwise, template.fieldlocations will be used to identify 
;                  the fields (columns). If the template is not provided, the 
;                  delimiter default is ' '
;
;   missing_value  Specify the value that will be used for missing or 
;                  non-numeric values
;
;   comment_symbol Comment symbol. The part of the line that begins with
;                  the comment symbol and ends to the end of line is discarded
;                  Default is ';'
;
;   record_start   Record number of the first record to be read (starts from 0)
;
;   num_record     Number of records to be read
;
;   template       structure that defines how the file will be processed
;                  the tags datastart, delimiter, missingvalue and commentsymbol
;                  can be overridden by the keywords data_start, delimiter,
;                  missing_value and comment_symbol.
;
;   template.VERSION           template version number (not used)
;   template.FIELDCOUNT=n      number of fields
;   template.FIELDNAMES[n]     field names
;   template.FIELDTYPES[n]     field integer types
;   template.FIELDGROUPS[n]    group ID. fields can be grouped into a single tag
;   template.FIELDLOCATIONS[n] start positions of the fields
;   template.DATASTART         see data_start keyed input
;   template.DELIMITER         see delimiter keyed input
;   template.MISSINGVALUE      see missing_value keyed input
;   template.COMMENTSYMBOL     see comment_symbol keyed input
;
;   if field of different types are grouped together, the following priority
;   if assumed to determine the tag type:
;     BYTE<UINT<INT<ULONG<LONG<ULONG64<LONG64<FLOAT<DOUBLE<COMPLEX<DCOMPLEX
;   Exception: grouping double and complex will result in a dcomplex
;   No boundary check is made, so information could be lost.
;
;
; KEYWORDS:
;   verbose        Not used.
;
;
; OUTPUT:
;   structure      Structure containing the file columns.
;
;
; KEYED OUTPUTS:
;   count          Number of records that have beeen read
;   header         First data_start lines of the file
;
;
; EXAMPLES:
; t = {version:1.0, fieldnames : strsplit('fa,fb,fc,fd,fe,ff',',', /extr), $
;      fieldtypes : [7, 4, 7, 2, 1, 5], fieldgroups : [0, 1, 2, 3, 4, 5], $
;      fieldcount: 6, fieldlocations:[0, 5, 9, 11, 14, 16], datastart:0, $
;      delimiter:'', missingvalue:-999, commentsymbol:';'}
; a = read_ascii('test-read_ascii.txt', template=t, header=header, 
;                data_start=2, count=count)
; 
;
; RESTRICTIONS:
;   no boundary check is performed if the missing value can not be represented
;   by the field type.
;
;
; MODIFICATION HISTORY:
;   13-Jan-2006 : written by Pierre Chanial
;   06-APr-2008 : m_schellens: made data_start independent of header
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
 

pro read_ascii_helper, tags, tag, structure, variable, default
 compile_opt hidden

 if n_elements(variable) ne 0 then return
 if n_elements(tags) eq 0 then begin
    variable = default
    return
 endif
 index = (where(tags eq tag, count))[0]
 if count eq 0 then begin
    variable = default
 endif else begin
    variable = structure.(index)
 endelse
end

function read_ascii_read, filename
 compile_opt hidden
 on_error, 2
 
 ;XXX should use nline=file_lines(filename) when implemented
 spawn, ['wc', '-l', filename], output, exit_status=status, /noshell
 if status ne 0 then message, 'Invalid filename'
 nline = long(output[0])
 text = strarr(nline)
 openr, unit, filename, /get_lun
 readf, unit, text
 free_lun, unit
 return, text
 
end

function read_ascii_gettype, types
 compile_opt hidden
 on_error, 2
 
 priority = [ 0,  $; undefined
              1,  $; byte
              3,  $; int
              5,  $; long
              8,  $; float
              9,  $; double
              10, $; complex
              99, $; string
              0,  $; struct
              11, $; dcomplex
              0,  $; pointer
              0,  $; object
              2,  $; uint
              4,  $; ulong
              7,  $; long64
              6   $; ulong64
 ]
 
 p = max(priority[types], itype, min=status)
 if status eq 0 then message, 'Invalid field type.'
 type = types[itype]

 ; special case: double+complex -> dcomplex
 if max(types eq 5) and max(types eq 6) and (type ne 7) then type = 11
 
 return, type
 
end

function read_ascii, filename, count=linecount, $
                     data_start=data_start, delimiter=delimiter,               $
                     missing_value=missing_value,comment_symbol=comment_symbol,$
                     num_records=num_records, record_start=record_start,   $
                     template=template, header=header, verbose=verbose

 on_error, 2
  
 if size(template, /tname) eq 'STRUCT' then begin
    tags = tag_names(template)
    deldefault = ''
 endif else begin
    deldefault = ' '
 endelse
 read_ascii_helper, tags, 'DATASTART',     template, data_start,     0
 read_ascii_helper, tags, 'DELIMITER',     template, delimiter,      deldefault
 read_ascii_helper, tags, 'MISSINGVALUE',  template, missing_value,  'NaN'
 read_ascii_helper, tags, 'COMMENTSYMBOL', template, comment_symbol, ';'
 
 text = read_ascii_read(filename)

 
 ;----------------
 ; get the header
 ;----------------

 if arg_present(header) then begin
    if data_start eq 0 then begin
       if n_elements(header) ne 0 then junk = temporary(header)
    endif else begin
       header = text[0:data_start-1]
       ;; text = text[data_start:*]
    endelse
endif

if data_start ne 0 then begin
    text = text[data_start:*]
endif

 
 ;-----------------
 ; remove comments
 ;-----------------

 if keyword_set(comment_symbol) then begin
    pos = strpos(text, comment_symbol)
    index = where(pos ne -1, count)
    for i=0, count-1 do begin
       j = index[i]
       text[j] = strmid(text[j], 0, pos[j])
    endfor
 endif
 
 
 ;--------------------
 ; remove blank lines
 ;--------------------

 text = strtrim(text, 2)
 index = where(strlen(text) ne 0, linecount)
 if linecount eq 0 then return, 0
 text = text[index]

 
 ;---------------------------
 ; get the requested records
 ;---------------------------

 if keyword_set(record_start) then begin
    if record_start ge linecount then begin
       message, 'Invalid record start ('+strtrim(record_start,1)+$
                '): the file only has '+strtrim(linecount,1)+' lines.'
    endif
    text = text[record_start:*]
    linecount = n_elements(text)
 endif
 
 if keyword_set(num_records) then begin
    if num_records gt linecount then begin
       message, 'Excessive number of requested records.'
    endif
    text = text[0:num_records-1]
    linecount = num_records
 endif
 
 rnumber = '^[+-]?([0-9]*\.?[0-9]*[ed]?[+-]?[0-9]+\.?[0-9]*|NaN|Inf|Infinity)$'

 
 ;------------------
 ; no-template case
 ;------------------
 
 if n_elements(template) eq 0 then begin
    ncolumns = lonarr(linecount)
    for line=0l, linecount-1 do begin
       ncolumns[line] = n_elements(strsplit(text[line], delimiter))
    endfor
    ncolumn = max(ncolumns)
    result = make_array(ncolumn, linecount, /float, value=float(missing_value))
    for line=0l, linecount-1 do begin
       row = strsplit(text[line], delimiter, /extract)
       index = where(stregex(row, rnumber, /fold_case, /boolean), count)
       if count gt 0 then result[index, line] = float(row[index])
    endfor
    return, {field1:temporary(result)}
 endif
 
 ; should take into account the field keyword, when RSI implements it.
 fieldcount  = template.fieldcount
 fieldtypes  = template.fieldtypes
 fieldnames  = template.fieldnames
 fieldlocs   = template.fieldlocations
 fieldgroups = template.fieldgroups

 strresult = strarr(fieldcount, linecount)

 
 ;-------------------------------------
 ; slice the file content into columns
 ;-------------------------------------
 
 if keyword_set(delimiter) then begin
    for line=0l, linecount-1 do begin
       row = strsplit(text[line], string(delimiter), /extract)
       strresult[0, line] = row
    endfor
 endif else begin
    for i=0l, fieldcount-2 do begin
       strresult[i,*] = strmid(text, fieldlocs[i], fieldlocs[i+1]-fieldlocs[i])
    endfor
    strresult[i,*] = strmid(text, fieldlocs[i])
 endelse
 strresult = strtrim(strresult,2)

 
 ;---------------------------
 ; get output structure info
 ;---------------------------
 
 tagcount  = n_elements(uniq(fieldgroups, sort(fieldgroups)))
 tagncols  = lonarr(tagcount)
 tagnames  = strarr(tagcount)
 tagtypes  = intarr(tagcount)
 taggroups = lonarr(tagcount)

 ; get the group IDs of the tags, which are the unique elements of |fieldgroups|
 ; with preserved order (we can not use uniq)
 taggroups[0] = fieldgroups[0]
 itag = 1l
 for i=1l, fieldcount-1 do begin
    if max(fieldgroups[i] eq taggroups[0:itag-1]) then continue
    taggroups[itag++] = fieldgroups[i]
 endfor

 for i=0l, tagcount-1 do begin
    index = where(fieldgroups eq taggroups[i], count)
    tagncols[i] = count
    tagnames[i] = fieldnames[index[0]]
    tagtypes[i] = read_ascii_gettype(fieldtypes[index])
 endfor
 
 
 ;-----------------------------
 ; create the output structure
 ;-----------------------------

 ; deal with columns that will be grouped as a 2D array into a single tag
 dims = replicate(strtrim(linecount,1), tagcount)
 index = where(tagncols gt 1, count)
 if count gt 0 then dims[index] = strtrim(tagncols[index],1)+','+dims[index]
 
 ; deal with the missing value. If it is not finite, use 0 for integers
 values = strarr(tagcount)
 index = where(tagtypes ne 7, count)
 if count gt 0 then values[index] = ', value=missing_value'
 
 ; construct the statement
 arrays = 'make_array(dim=['+dims+'], type='+strtrim(tagtypes,1)+values+')'
 str = 'result={'+strjoin(tagnames+':'+arrays, ',')+'}'
 ok = execute(str)

 
 ;---------------------------
 ; fill the output structure
 ;---------------------------
 
 ; loop over the output structure tags
 for i=0l, tagcount-1 do begin
    icol = where(fieldgroups eq taggroups[i], ncol)
    ; loop over the columns in the same group
    for j=0l, ncol-1 do begin
       row = reform(strresult[icol[j],*])
       if fieldtypes[i] eq 7 then begin
          if ncol eq 1 then begin
             result.(i) = row
          endif else begin
             result.(i)[j,*] = row
          endelse
       endif else begin
          index = where(stregex(row, rnumber, /fold_case, /boolean), count)
          if count eq 0 then continue
          if ncol eq 1 then begin
             result.(i)[index] = row[index]
          endif else begin
             result.(i)[j,index] = row[index]
          endelse
       endelse
    endfor
 endfor
 
 return, result
    
end
