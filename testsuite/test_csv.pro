;
; Alain C., January 2026
;
; No systematic tests were done on CSV files.
; Several tricks & problems may arise with CSV files :
;  -- implicit header
;  -- format conversion (strin, long, float, int)
;  -- lines without content
;  -- cells without content
;
; -----------------------------------------------
;
pro GENERATE_CVS_FILES
;
nbp=10
col1=FLTARR(nbp)
col2=INDGEN(nbp)
col3=DINDGEN(nbp)
col1str=STRING(col1)+'str'
col2str='str '+STRING(col2)
col3str=STRING(col3)+' str'
;
prefix=GDL_IDL_FL(/pref)
;
WRITE_CSV, prefix+'2col.csv', col1, col2
WRITE_CSV, prefix+'3col.csv', col1, col2, col3
;
;in fact, HEADER and TABLE_HEADER are not ready on the GDL side ...
str_header=['ici',' la',' bas  ']
str_table_h=['t1,t2,t3', 'ligne', '']
;
file3=prefix+'3col_header.csv'
file3string=prefix+'3col_header_str.csv'
file3tbl=prefix+'3col_header_tbl.csv'
;
; WRITE_CSV in GDL is not ready for HEADER & TABLE_HEADER
;
if GDL_IDL_FL() EQ 'IDL' then begin
   WRITE_CSV, file3, col1, col2, col3, header=str_header
   WRITE_CSV, file3string, col1str, col2str, col3str, header=str_header
   WRITE_CSV, file3tbl, col1, col2, col3, header=str_header, table=str_table_h
endif else begin
   ;;
   tmp_str_header='ici, la, bas  '
   ;; file with header only
   OPENW, lun, file3, /get
   PRINTF, lun, tmp_str_header
   for ii=0, n_elements(col1)-1 do begin
      PRINTF, lun, col1[ii],',', col2[ii],',', col3[ii]
   endfor
   CLOSE, lun
   FREE_LUN, lun
   ;; file with header only AND with strings only
   OPENW, lun, file3string, /get
   PRINTF, lun, tmp_str_header
   for ii=0, n_elements(col1)-1 do begin
      PRINTF, lun, col1str[ii],',', col2str[ii],',', col3str[ii]
   endfor
   CLOSE, lun
   FREE_LUN, lun
   ;; file with header + table_header
   OPENW, lun, file3tbl, /get
   for ii=0, N_ELEMENTS(str_table_h)-1 do printf, lun, str_table_h[ii]
   PRINTF, lun, tmp_str_header
   for ii=0, n_elements(col1)-1 do begin
      PRINTF, lun, col1[ii],',', col2[ii],',', col3[ii]
   endfor
   CLOSE, lun
   FREE_LUN, lun
endelse
;
end
;
; ----------------------------------------------
;
pro TEST_READ_CSV, cumul_errors, test=test, prefix=prefix
;
errors=0
if ~KEYWORD_SET(prefix) then prefix=GDL_IDL_FL(/pref)
;
; expected_header
str_header=['ici',' la',' bas  ']
;
file1=prefix+'2col.csv'
if ~FILE_TEST(file1) then begin
   ERRORS_ADD, errors, 'Missing file 1 : '+file1
endif else begin
   d1=READ_CSV(file1)
   txt='pb File1 : '+file1+' : '
   if (N_TAGS(d1) NE 2) then ERRORS_ADD, errors, txt+'Bad N_TAGS()'
   if N_ELEMENTS(d1.field1) NE 10 then ERRORS_ADD, errors, txt+'Nb lines. col1'
   if (TOTAL(d1.field1) GT 0.0) then ERRORS_ADD, errors, txt+'num. col1'
   if TYPENAME(d1.field1) NE 'DOUBLE' then ERRORS_ADD, errors, txt+'type. col1'
   if N_ELEMENTS(d1.field2) NE 10 then ERRORS_ADD, errors, txt+'Nb lines. col2'
   if (TOTAL(d1.field2,/preserve) NE 45) then ERRORS_ADD, errors, txt+'total. col2'
   if TYPENAME(d1.field2) NE 'LONG' then ERRORS_ADD, errors, txt+'type. col2'
endelse
;
file2=prefix+'3col.csv'
if ~FILE_TEST(file2) then begin
   ERRORS_ADD, errors, 'Missing file 2 : '+file2
endif else begin
   d2=READ_CSV(file2)
   txt='pb File2 : '+file2+' : '
   if (N_TAGS(d2) NE 3) then ERRORS_ADD, errors, txt+'Bad N_TAGS()'
   if N_ELEMENTS(d2.field2) NE 10 then ERRORS_ADD, errors, txt+'Nb lines. col2'
   if (TOTAL(d2.field2,/preserve) NE 45) then ERRORS_ADD, errors, txt+'total. col2'
   if TYPENAME(d2.field2) NE 'LONG' then ERRORS_ADD, errors, txt+'type. col2'
   if (ABS(TOTAL(d2.field3)-45.) GT 1e-35) then ERRORS_ADD, errors, txt+'total. col3'
   if TYPENAME(d2.field3) NE 'DOUBLE' then ERRORS_ADD, errors, txt+'type. col3'
   ;;
   ;; skip 5 lines
   d2s=READ_CSV(file2, record_start=5, count=count)
   txt='pb File2, skip 5 : '+file2+' : '      
   if (count NE 5) then ERRORS_ADD, errors, txt+'bad count'
   if N_ELEMENTS(d2s.field2) NE 5 then ERRORS_ADD, errors, txt+'Nb lines. col2'
   if (TOTAL(d2s.field2,/preserve) NE 35) then ERRORS_ADD, errors, txt+'total. col2'
   if TYPENAME(d2s.field2) NE 'LONG' then ERRORS_ADD, errors, txt+'type. col2'
   if (ABS(TOTAL(d2s.field3)-35.) GT 1e-35) then ERRORS_ADD, errors, txt+'total. col3'
   if TYPENAME(d2s.field3) NE 'DOUBLE' then ERRORS_ADD, errors, txt+'type. col3'
endelse
;
; Depending the way READ_CSV is read, HEADER may be read/processed or not 
;
; * case without TABLE_HEADER
; 
file3=prefix+'3col_header.csv'
if ~FILE_TEST(file3) then begin
   ERRORS_ADD, errors, 'Missing file 3 : '+file3
endif else begin
   d3raw=READ_CSV(file3)
   txt='pb File3 without TABLE_HEADER : '+file3+' : '
   if (N_TAGS(d3raw) NE 3) then ERRORS_ADD, errors, txt+'Bad N_TAGS()'
   if ~ARRAY_EQUAL(SIZE(d3raw), [1,1,8,1]) then ERRORS_ADD, errors, txt+'bad Struct'
   if (N_ELEMENTS(d3raw.field1) NE 10) then ERRORS_ADD, errors, txt+'bad N_Elem'
   if (TYPENAME(d3raw) NE 'ANONYMOUS') then ERRORS_ADD, errors, txt+'bad Struct name'
   if (TYPENAME(d3raw.field1) NE 'DOUBLE') then ERRORS_ADD, errors, txt+'bad Field Type'
endelse
;
; * case with TABLE_HEADER
;
file3tbl=prefix+'3col_header_tbl.csv'
if ~FILE_TEST(file3tbl) then begin
   ERRORS_ADD, errors, 'Missing file 3 table: '+file3tbl
endif else begin
   ;; no information on header/table_header
   ;; in this case, everything is transformed into STRING,
   d3t=READ_CSV(file3tbl)
   txt='pb File3 with TABLE_HEADER : '+file3tbl+' : '
   if (N_TAGS(d3t) NE 1) then ERRORS_ADD, errors, txt+'Bad N_TAGS()'
   if (N_ELEMENTS(d3t.field1) NE 13) then ERRORS_ADD, errors, txt+'bad N_Elem'
   if (TYPENAME(d3t.field1) NE 'STRING') then ERRORS_ADD, errors, txt+'bad Field 1 Type'
   ;;
   ;; in this case, TABLE_HEADER is skipped and Header not processed
   d3t2=READ_CSV(file3tbl, n_table=2)
   txt='pb File3 with TABLE_HEADER (skip) : '+file3tbl+' : '
   if (N_TAGS(d3t2) NE 3) then ERRORS_ADD, errors, txt+'Bad N_TAGS()'
   if (N_ELEMENTS(d3t2.field1) NE 10) then ERRORS_ADD, errors, txt+'bad N_Elem'
   if (TYPENAME(d3t2.field1) NE 'DOUBLE') then ERRORS_ADD, errors, txt+'bad Field 1 Type'
   if (TYPENAME(d3t2.field2) NE 'LONG') then ERRORS_ADD, errors, txt+'bad Field 2 Type'
   if (TYPENAME(d3t2.field3) NE 'DOUBLE') then ERRORS_ADD, errors, txt+'bad Field 3 Type'
   if (ABS(TOTAL(d3t2.field3)-45.) GT 1e-35) then ERRORS_ADD, errors, txt+'total. col3'
   ;;
   ;; in this case, TABLE_HEADER is skipped and Header processed
   d3t3=READ_CSV(file3tbl, n_table=2, header=header)
   txt='pb File3 with TABLE_HEADER (skip) : '+file3tbl+' : '
   if (N_TAGS(d3t3) NE 3) then ERRORS_ADD, errors, txt+'Bad N_TAGS()'
   if (N_ELEMENTS(d3t3.field1) NE 10) then ERRORS_ADD, errors, txt+'bad N_Elem'
   if ~ARRAY_EQUAL(STRTRIM(header,2),STRTRIM(str_header,2)) then ERRORS_ADD, errors, txt+'bad HEADER'
   if (TYPENAME(d3t3.field1) NE 'DOUBLE') then ERRORS_ADD, errors, txt+'bad Field 1 Type'
   if (TYPENAME(d3t3.field2) NE 'LONG') then ERRORS_ADD, errors, txt+'bad Field 2 Type'
   if (TYPENAME(d3t3.field3) NE 'DOUBLE') then ERRORS_ADD, errors, txt+'bad Field 3 Type'
   if (ABS(TOTAL(d3t3.field3)-45.) GT 1e-35) then ERRORS_ADD, errors, txt+'total. col3'
   ;;
endelse
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_READ_CSV', errors, /short
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; ----------------------------------------------
;
pro TEST_CSV, help=help, test=test, no_exit=no_exit, verbose=verbose
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_CSV, help=help, test=test, $'
   print, '              no_exit=no_exit, verbose=verbose'
   return
endif
;
cumul_errors=0
;
GENERATE_CVS_FILES
TEST_READ_CSV, cumul_errors, test=test
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_CSV', cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
