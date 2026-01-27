;+
; DESCRIPTION:
; returns the fields of a csv line (i.e., separated by SEPARATOR(=',' by
; default) allowing for quoted fields but also embedded quotes within quoted
; fields which must be doubled.
;
; CALLING SEQUENCE:
; struct=read_csv(filename, ...keywords)
; Reads "Filename" and, if it is a CSV file (character separated value file),
; interprets the values insides and returns them as arrays in a struct. 
;
; KEYWORD PARAMETERS:
;  They are all optional of course.
;  count=count : returns the number of lines successfully read
;  header=header : if columns are NOT ALL STRINGs, may be the first line of values will be returned in header as an array of characters.
;  missing_value=missval : a value that will replace numerical values that were not present in the csv file.
;  n_table_header=n_table_header: skip that number of non-null lines before doing anything (a csv file may have a few lines of text before the columns.)
;  num_records=numrec: read numrec records instead of all lines
;  record_start=startrec: read strating from startrec, not the first line of readble values
;  table_header=tableHeader: tableHeader will contain the n_table_header lines read in the file.
;  types=types: types is an array of STRINGs describing the intended type of the corresponding columns, 
;      possible values are "BYTE","INT","LONG","FLOAT","DOUBLE","UINT","ULONG","LONG64",
;                          "ULONG64","STRING","DATE","TIME","DATETIME","". The latter meaning
;                                    that GDL is supposed to find the type itself.
;  separator=sep is a GDL-only keyword indicating a different
;  separator than the default comma ",".
;
; ----------------------------
;
; MODIFICATION HISTORY:
;  Written by: Gilles Duvert, Nov 2019.
;  Revision by Alain C., Jan 2026, see bug report #2164, adding a
;  test_csv.pro  in testsuite/
;-
;
function SPLIT_CSV_LINE, line, separator=separator

compile_opt idl2, hidden
on_error,2
;useful values:
comma=44b
doublequote=34b
OurSep=254b
;
if N_ELEMENTS(separator) eq 0 then bsp=comma else bsp=BYTE(separator[0])
if bsp eq doublequote then MESSAGE,"separator must not be double quote."
;
l=STRLEN(line)
if (l eq 0) then return,''
v=BYTE(line)
r=BYTARR(2*l)
i=0L
inside=0L
isquote = v eq doublequote
;
; the idea is to replace bsp by a special code (254b) , remove quotes
; except if they are doubled. then split on 254b
for j=0,l-1 do begin
   if (isquote[j]) then begin   ; we found a doublequote
      if (inside) then begin    ; inside a STRING!
         ;; if ( (j+1) lt l and isquote[j+1] ) then begin ; next is quote, so this quote has to be kept:
         if ( (j+1) lt l ) then if ( isquote[j+1] ) then begin ; next is quote, so this quote has to be kept:
            r[i++] = doublequote & j++
         endif else begin       ; not a real quote but end of STRING
            inside=0L
         endelse
      endif else begin          ; start of STRING
         inside=1L
      endelse
   endif else if (v[j] eq bsp) then begin ; separator: use ours.
      r[i++]= inside?bsp:OurSep
   endif else begin             ; normal character
      r[i++] = v[j]
   endelse
endfor
;  
res=STRING(r[0:i-1])
return, strtok(res,STRING(OurSep),/extract,/preserve_null)
end
;
; ------------------------------------------------
; creates a (possibly complicated) struct from a
; tagnames array and an array of pointers to data.
;
function READ_CSV_STRUCT_CREATE, tags, typePtr
;
ntags=N_ELEMENTS(tags)
ntypePtr=N_ELEMENTS(typePtr)
if (ntypePtr ne ntags) then MESSAGE,"internal error in read_csv_struct_create" 
;; create a struct by adding simple
;; structs, the recursive call trick is
;; handy
if (ntypePtr eq 0) then return,0 ; wont happen
if (ntypePtr eq 1) then return, create_struct(tags[0],TEMPORARY(*typePtr[0]))
if (ntypePtr eq 2) then return, create_struct(tags[0],TEMPORARY(*typePtr[0]),tags[1],TEMPORARY(*typePtr[1]))
if (ntypePtr eq 3) then return, create_struct(tags[0],TEMPORARY(*typePtr[0]),tags[1],TEMPORARY(*typePtr[1]),tags[2],TEMPORARY(*typePtr[2]))
if (ntypePtr eq 4) then return, create_struct(tags[0],TEMPORARY(*typePtr[0]),tags[1],TEMPORARY(*typePtr[1]),tags[2],TEMPORARY(*typePtr[2]),tags[3],TEMPORARY(*typePtr[3]))
;
bisect=ntags/2                  ; by halves
return, CREATE_STRUCT(READ_CSV_STRUCT_CREATE(tags[0:bisect-1],typePtr[0:bisect-1]),$
                      READ_CSV_STRUCT_CREATE(tags[bisect:*],typePtr[bisect:*]))
end
;
; --------------------------------------------------------
;
function READ_CSV, filename, count=count, header=header, $
                   missing_value=missval,$
                   n_table_header=nskip, num_records=numrec, $
                   record_start=startrec, table_header=tableHeader, $
                   types=types, test=test, debug=debug, $
                   separator=sep ; GDL only. Thanks, GDL.
;
compile_opt hidden, idl2
if ~(KEYWORD_SET(debug) OR KEYWORD_SET(test)) then ON_ERROR, 2
;
count=0L
header=""
tableHeader= ''
;
if N_PARAMS() eq 0 then MESSAGE,"Incorrect number of arguments"
if ~file_test(filename,/read) then return,0L
;
if N_ELEMENTS(nskip) le 0 then nskip=0L
if N_ELEMENTS(numrec) le 0 then numrec=0L
if N_ELEMENTS(startrec) le 0 then startrec=0L
;
nlines = FILE_LINES(filename)
if KEYWORD_SET(debug) then print, 'nlines :', nlines
;
if (nlines le startrec+nskip) then return,0L
;
OPENR, lun, filename, /get_lun
line=''
;
; Do we have to skip "startrec" lines ?
for i=0L, startrec-1 do begin
   READF, lun, line
   nlines--
endfor
;
; AC : TABLE_HEADER is *above* HEADER
;
; skip n_table_header by reading them in table_header.
; apparently we return only up to the first occurence of separator
;
for i=0L, nskip-1 do begin
   READF,lun,line
   nlines--
   subline=SPLIT_CSV_LINE(line,sep=sep)
   tableHeader = (i eq 0) ? [subline[0]] : [tableHeader, subline[0]] ; only the first part of subline.
endfor
;
; When header= is asked, we have to read it and it will be returned as header
;
; AC: but when no header= is requested, we will need to check later
; whether the first line is a true header containing only strings, or
; a normal data line (flag implicit_header=1)
;
;READF, lun, line
;nlines--
implicit_header=1
;subline=SPLIT_CSV_LINE(line,sep=sep)
;header=subline
;
if ARG_PRESENT(header) then implicit_header=0
;
; since here, we should have process HEADER and TableHeader
; (but exception exist, if all the data are String ...)
;
; IDL skips blank lines after this position.
READF, lun, line
nlines--
if nlines eq 0 then return,0L
while (STRLEN(STRTRIM(line,2)) eq 0) do begin
   READF, lun, line
   nlines--
   if nlines eq 0 then return,0L
endwhile
subline=SPLIT_CSV_LINE(line,sep=sep)
header=subline
;
; since here, we have finish to read & process the header
;
READF, lun, line
nlines--
if nlines eq 0 then return,0L
;
;if KEYWORD_SET(debug) then STOP
;
; this is the first non-blank line encountered. it defines the content
; of the csv. there are nlines-counter to go
;
subline=STRTRIM(SPLIT_CSV_LINE(line,sep=sep),2)
ncols=N_ELEMENTS(subline)
tmp=INDGEN(ncols)+1             ; field1, field2... fieldN 
tags='FIELD'+strtrim(tmp,2)
;
restalire=nlines+1
; is it useful ? if nlines eq 0 then return,0L
;
;; if numrec > 0 we dimension data to numrec
colsize=(numrec eq 0) ? restalire : numrec<restalire
ptrCols=PTRARR(ncols)
for i=0L,ncols-1 do ptrCols[i]=PTR_NEW(STRARR(colsize))
;
;; don't forget to copy into "ptrCols" the first line !
count=0
for j=0,(N_ELEMENTS(subline)<ncols)-1 do (*ptrCols[j])[count]=subline[j]
count++
restalire--
;
;print, line
for iline=0L,restalire-1 do begin
   READF, lun, line
   ;; AC debug print, iline, line
   if KEYWORD_SET(debug) then print, count, ' , ', iline
   if KEYWORD_SET(debug) then print, iline, ' : ', line
   if (STRLEN(STRTRIM(line,2)) eq 0) then continue
   subline=STRTRIM(SPLIT_CSV_LINE(line,sep=sep),2)
   for j=0,(N_ELEMENTS(subline)<ncols)-1 do (*ptrCols[j])[count]=subline[j]
;; not here.     if N_ELEMENTS(subline) lt ncols then for j=N_ELEMENTS(subline),ncols-1 do (*ptrCols[j])[count]='0'
   count++
   if (numrec gt 0 and count ge numrec) then break 
endfor
;
;if KEYWORD_SET(debug) then stop
;
CLOSE, lun
FREE_LUN, lun
;
;if KEYWORD_SET(debug) then
;print, count, colsize , restalire, nlines
;
;; if count is smaller than colsize (blank lines), truncate the columns
if (count lt colsize) then begin
   for i=0,ncols-1 do (*ptrCols[i])=((*ptrCols[i]))[0:count-1]
endif
;
;; this fills easily a STRING-only table.
;; now, determine a default type for the columns:
;; -- integer if [0123456789+-],
;; -- floats if decimal point and exponent,
;; -- 64 bits if integer is > 2^31+1,
;; -- STRING if nothing else
;; test if float, then ints as subset of floats
;; problem is some values may be blank, which is to be ignored to
;; determine the type.
;;
numbersigns='^[+-]?[0-9]*[.]?[0-9]*[DdEd]?[+-]?[0-9]*$'
coltype=BYTARR(ncols)
for i=0,ncols-1 do begin
   w=WHERE(STRLEN(*ptrCols[i]) gt 0, nbOk)
   coltype[i]=(TOTAL(STREGEX((*ptrCols[i])[w],numbersigns,/BOOL)) eq nbOk)
endfor

w=WHERE(coltype eq 1, n)
if (n gt 0) then begin
   intcoltype=BYTARR(ncols)
   for i=0,n-1 do begin
      w2=WHERE(STRLEN(*ptrCols[w[i]]) gt 0, nbOk)
      intcoltype[w[i]]=(TOTAL(STREGEX((*ptrCols[w[i]])[w2],'^[+-]?[0-9]+$',/BOOL))  eq nbOk)
   endfor
   coltype+=intcoltype
endif
;
;;coltype=0->STRING; 1:float 2:int
;
gdltype=[7,5,3]
for i=0,ncols-1 do coltype[i]=gdltype[coltype[i]]
;
if N_ELEMENTS(types) gt 0 then begin
   for j=0L,(N_ELEMENTS(types)<ncols)-1  do begin
      case (STRTRIM(STRUPCASE(types[j]),2)) of
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
         ELSE:                  ; do nothing
      endcase
   endfor
endif
;
if KEYWORD_SET(debug) then print, 'coltype :', coltype
;
;; convert accordingly
for i=0,ncols-1 do begin
   ;; if missing value requested and not zero, memorize blanked values
   w=WHERE(STRLEN(*ptrCols[i]) eq 0, nbNOk) 
   *ptrCols[i]=FIX(*ptrCols[i],type=coltype[i])
   if ((N_ELEMENTS(missval) ne 0) and (nbNOk gt 0)) then begin
      (*ptrCols[i])[w]=missval[0]
   endif
endfor
;
; AC 25/01/14 
; When header= is not activated, if header line is only string
; and the follwing lines do contains not only string (ie some columns
; with numbers), the first line is skipped.
; But 1/ if all the lines are string, then we add the first line !
;     2/ if numbers and first line similar to next lines, we add it !
;
;
if KEYWORD_SET(debug) then print, 'coltype :', coltype
;
if implicit_header then begin
   ;;
   ;; do we have non-string data in the data ?
   ;; If yes, Can we convert in the format the
   ;; related cell in the header line ?!
   ;;
   w=WHERE(coltype eq 7, nbt)
   conv_is_ok=REPLICATE(1,ncols)
   ;;
   if (nbt LT ncols) then begin
      for iii=0, ncols-1 do begin
         if (coltype[iii] NE 7) then begin
            ;; ok, this is noisy and not elegant  ... help welcome
            ;;res=EXECUTE("value=FIX(header[iii], type=coltype[iii])")
            ;; let's try a quiet one
            res=CAN_CONVERT_TO_TYPE(header[iii], coltype[iii])
            if ~res then conv_is_ok[iii]=0
         endif
      endfor
      if KEYWORD_SET(debug) then print, "conv_is_ok :", conv_is_ok
   endif
   ;; Si toutes les données sont STRING ou si la conversion
   if (nbt eq ncols) OR (TOTAL(conv_is_ok) EQ ncols) then begin
      ;; in that case we need to add the first line we elude at the beginning   
      for i=0,ncols-1 do begin
         (*ptrCols[i])=[header[i],(*ptrCols[i])]
      endfor
      count++
      if ARG_PRESENT(header) then header=''
   endif 
endif
;
finalstruct=READ_CSV_STRUCT_CREATE(tags,ptrCols)
;
if KEYWORD_SET(test) then STOP
;
return, finalstruct
;
end
