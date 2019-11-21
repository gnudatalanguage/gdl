;+
; DESCRIPTION:
;returns the fields of a csv line (i.e., separated by SEPARATOR(=',' by
;default) allowing for quoted fields but also embedded quotes within quoted
;fields which must be doubled.
;
; CALLING SEQUENCE:
; struct=read_csv(filename, ...keywords)
; Reads "Filename" and, if it is a CSV file (character separated value file),
; interprets the values insides and returns them as arrays in a struct. 
;
; KEYWORD PARAMETERS:
;  They are all optional of course.
;  count=count : returns the number of lines successfully read
;  header=header : if columns are NOT ALL strings, may be the first line of values will be returned in header as an array of characters.
;  missing_value=missval : a value that will replace numerical values that were not present in the csv file.
;  n_table_header=n_table_header: skip that number of non-null lines before doing anything (a csv file may have a few lines of text before the columns.)
;  num_records=numrec: read numrec records instead of all lines
;  record_start=startrec: read strating from startrec, not the first line of readble values
;  table_header=tableHeader: tableHeader will contain the n_table_header lines read in the file.
;  types=types: types is an array of strings describing the intended type of the corresponding columns, 
;               possible values are "BYTE","INT","LONG","FLOAT","DOUBLE","UINT","ULONG","LONG64",
;                                   "ULONG64","STRING","DATE","TIME","DATETIME","". The latter meaning
;                                    that GDL is supposed to find the type itself.
;  separator=sep is a GDL-only keyword indicating a different separator than the default comma ",".
; MODIFICATION HISTORY:
;  Written by: Gilles Duvert, Nov 2019. 
;-

function split_csv_line,line,separator=separator
;useful values:
  comma=44b
  doublequote=34b
  OurSep=254b

  if n_elements(separator) eq 0 then bsp=comma else bsp=byte(separator[0])
  if bsp eq doublequote then message,"separator must not be double quote."
  

  l=strlen(line)
  if (l eq 0) then return,''
  v=byte(line)
  r=bytarr(2*l)
  i=0L
  inside=0L
  isquote = v eq doublequote
  
; the idea is to replace bsp by a special code (254b) , remove quotes
; except if they are doubled. then split on 254b
  for j=0,l-1 do begin
     if (isquote[j]) then begin   ; we found a doublequote
        if (inside) then begin  ; inside a string!
;           if ( (j+1) lt l and isquote[j+1] ) then begin ; next is quote, so this quote has to be kept:
           if ( (j+1) lt l ) then if ( isquote[j+1] ) then begin ; next is quote, so this quote has to be kept:
              r[i++] = doublequote & j++
           endif else begin ; not a real quote but end of string
              inside=0L
           endelse
        endif else begin        ; start of string
           inside=1L
        endelse
     endif else if (v[j] eq bsp) then begin ; separator: use ours.
        r[i++]= inside?bsp:OurSep
     endif else begin ; normal character
        r[i++] = v[j]
     endelse
   endfor
  
  res=string(r[0:i-1])
  return, strtok(res,string(OurSep),/extract,/preserve_null)
end

; creates a (possibly complicated) struct from a tagnames array and an array of pointers to data.
function read_csv_struct_create,tags,typePtr
  ntags=N_ELEMENTS(tags)
  ntypePtr=N_ELEMENTS(typePtr)
  if (ntypePtr ne ntags) then message,"internal error in read_csv_ascii_struct_create" 
                                ; create a struct by adding simple
                                ; structs, the recursive call trick is
                                ; handy
  if (ntypePtr eq 0) then return,0      ; wont happen
  if (ntypePtr eq 1) then return, create_struct(tags[0],temporary(*typePtr[0]))
  if (ntypePtr eq 2) then return, create_struct(tags[0],temporary(*typePtr[0]),tags[1],temporary(*typePtr[1]))
  if (ntypePtr eq 3) then return, create_struct(tags[0],temporary(*typePtr[0]),tags[1],temporary(*typePtr[1]),tags[2],temporary(*typePtr[2]))
  if (ntypePtr eq 4) then return, create_struct(tags[0],temporary(*typePtr[0]),tags[1],temporary(*typePtr[1]),tags[2],temporary(*typePtr[2]),tags[3],temporary(*typePtr[3]))
  
  bisect=ntags/2 ; by halves
  return, create_struct(read_csv_struct_create(tags[0:bisect-1],typePtr[0:bisect-1]),$
                        read_csv_struct_create(tags[bisect:*],typePtr[bisect:*]))
end
function read_csv, filename, count=count, header=header, missing_value=missval,$
                   n_table_header=nskip, num_records=numrec, $
                   record_start=startrec, table_header=tableHeader, $
                   types=types, $
                   separator=sep ; GDL only. Thanks, GDL.

  count=0L
  header=""
  tableHeader= ''

  if n_params() eq 0 then message,"Incorrect number of arguments"
  if ~file_test(filename,/read) then return,0L

  if n_elements(nskip) le 0 then nskip=0L
  if n_elements(numrec) le 0 then numrec=0L
  if n_elements(startrec) le 0 then startrec=0L
  
  nlines = FILE_LINES(filename)

  if (nlines le startrec+nskip) then return,0L
  
  openr, lun, filename, /get_lun
  line=''
  ; skip startrec
  for i=0L, startrec-1 do begin
     readf,lun,line
     nlines--
  endfor
; skip n_table_header by reading them in table_header.
; apparently we return only up to the first occurence of separator
  for i=0L, nskip-1 do begin
     readf,lun,line
     nlines--
     subline=split_csv_line(line,sep=sep)
     tableHeader = (i eq 0) ? subline[0] : [tableHeader, subline[0]] ; only the first part of subline.
  endfor

; IDL skips blank lines after this position.
  READF, lun, line & nlines--
  if nlines eq 0 then return,0L
  while (STRLEN(STRTRIM(line,2)) eq 0) do begin
    READF, lun, line & nlines--
    if nlines eq 0 then return,0L
  endwhile
; this is the first non-blank line encountered. it defines the content
; of the csv. there are nlines-counter to go
  subline=strtrim(split_csv_line(line,sep=sep),2)
  ncols=n_elements(subline)
  tmp=indgen(ncols)+1 ; field1, field2... fieldN 
  tags='FIELD'+strtrim(tmp,2)

  header=subline
  
  restalire=nlines
  if nlines eq 0 then return,0L

  ; if numrec > 0 we dimension data to numrec
  colsize=(numrec eq 0) ? restalire : numrec<restalire
  ptrCols=ptrarr(ncols) & for i=0L,ncols-1 do ptrCols[i]=ptr_new(strarr(colsize))

  count=0
  for iline=0L,restalire-1 do begin
    READF, lun, line
    if (STRLEN(STRTRIM(line,2)) eq 0) then continue
     subline=strtrim(split_csv_line(line,sep=sep),2)
     for j=0,(n_elements(subline)<ncols)-1 do (*ptrCols[j])[count]=subline[j]
; not here.     if n_elements(subline) lt ncols then for j=n_elements(subline),ncols-1 do (*ptrCols[j])[count]='0'
     count++
     if (numrec gt 0 and count ge numrec) then break 
  endfor

  ; if count is smaller than colsize (blank lines), truncate the columns
  if (count lt colsize) then for i=0,ncols-1 do (*ptrCols[i])=((*ptrCols[i]))[0:count-1]
  
  ; this fills easily a string-only table.
  ; now, determine a default type for the columns: integer if [0123456789+-],
  ; floats if decimal point and exponent ; 64 bits if integer is > 2^31+1, string if nothing else
  ; test if float, then ints as subset of floats
  ; problem is some values may be blank, which is to be ignored to
  ; determine the type.

  numbersigns='^[+-]?[0-9]*[.]?[0-9]*[DdEd]?[+-]?[0-9]*$'
  coltype=bytarr(ncols)
  for i=0,ncols-1 do begin
    w=where(strlen(*ptrCols[i]) gt 0, nbOk) 
      coltype[i]=(total(stregex((*ptrCols[i])[w],numbersigns,/BOOL)) eq nbOk)
  endfor
  
  w=where(coltype eq 1, n) & if (n gt 0) then begin
    intcoltype=bytarr(ncols)
    for i=0,n-1 do begin
      w2=where(strlen(*ptrCols[i]) gt 0, nbOk)
      intcoltype[w[i]]=(total(stregex((*ptrCols[w[i]])[w2],'^[+-]?[0-9]+$',/BOOL))  eq nbOk)
    endfor
    coltype+=intcoltype
  endif

  ;coltype=0->string; 1:float 2:int
  gdltype=[7,5,3]
  for i=0,ncols-1 do coltype[i]=gdltype[coltype[i]]
  
  if n_elements(types) gt 0 then begin
    for j=0L,(N_elements(types)<ncols)-1  do begin
      case (strtrim(strupcase(types[j]),2)) of
        "BYTE":     coltype[j]=1
        "INT":      coltype[j]=2
        "LONG":     coltype[j]=3
        "FLOAT":    coltype[j]=4
        "DOUBLE":   coltype[j]=5
        "UINT":     coltype[j]=12
        "ULONG":    coltype[j]=13
        "LONG64":   coltype[j]=14
        "ULONG64":  coltype[j]=15
        "STRING":   coltype[j]=7
        "DATE":     coltype[j]=7
        "TIME":     coltype[j]=7
        "DATETIME": coltype[j]=7
        ELSE: ; do nothing
      endcase
    endfor
    
  endif

  ; convert accordingly
  for i=0,ncols-1 do begin
    ;if missing value requested and not zero, memorize blanked values
    w=where(strlen(*ptrCols[i]) eq 0, nbNOk) 
    *ptrCols[i]=FIX(*ptrCols[i],type=coltype[i])
    if ( (n_elements(missval) ne 0 )and nbNOk gt 0 ) then (*ptrCols[i])[w]=missval[0]  
  endfor

  ;if table is only text, header line will be in table, no header
  ;returned.
  w=where(coltype eq 7, nbt) & if nbt eq ncols then begin
    for i=0,ncols-1 do begin
      (*ptrCols[i])=[header[i],(*ptrCols[i])]
      header[i]=''
    endfor
  endif
    
  finalstruct=read_csv_struct_create(tags,ptrCols)
  return,finalstruct
end
