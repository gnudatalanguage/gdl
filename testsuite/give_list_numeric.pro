;
; AC 2017-Dec-24
;
; used in "test_save_restore.pro" and revised version of
; "test_n_tags.pro", "test_fix.pro", ...
;
; ----------------------------------------------------
; Modifications history :
;
; 2018-Feb-07 : AC. 
; 1/ Since now, default is UpperCase.
; 2/ being able to select Integer types only
; 3/ if not 64b, no l64 & ul64 ...
;
; -----------------------------------------------
;
pro GIVE_LIST_NUMERIC, list_numeric_types, list_numeric_names, $
                       list_numeric_size, name=name, $
                       integer=integer, lowercase=lowercase, $
                       verbose=verbose, help=help, test=test
;
if KEYWORD_SET(help) then begin
   print, 'pro GIVE_LIST_NUMERIC, list_numeric_types, list_numeric_names, $'
   print, '                       list_numeric_size, name=name, $'
   print, '                       integer=integer, lowercase=lowercase, $'
   print, '                       verbose=verbose, help=help, test=test'
   return
endif
;
; internal use ...
list_integer_types=[1,1,1,0,0,0,0,1,1,1,1]
;
; http://www.harrisgeospatial.com/docs/idl_data_types.html
list_names=['b','s','l','.','d','__','__','u/us','ul','ll','ull']
;
; http://www.harrisgeospatial.com/docs/size.html
;
list_numeric_types=[1,2,3,4,5,6,9,12,13,14,15]
list_numeric_size =[1,2,4,4,8,8,16,2,4,8,8]
list_numeric_names=['byte','int','long',$
                    'float','double','complex','dcomplex', $
                    'uint','ulong','long64','ulong64']
;
if KEYWORD_SET(integer) then begin
   if KEYWORD_SET(verbose) then print, 'Only INTEGER types selected'
   ok=WHERE(list_integer_types GT 0)
   list_numeric_types=list_numeric_types[ok]
   list_numeric_size=list_numeric_size[ok]
   list_numeric_names=list_numeric_names[ok]
   list_names=list_names[ok]
endif
;
; the good way to test if we are on a 64b system is
; a test on : !VERSION.MEMORY_BITS
;
if (!version.memory_bits NE 64) then begin
   ;; removing Long64 & ULong64
   ;; we don't use [0,-3] because of the old IDL/GDL
   ok=WHERE(STRPOS(list_numeric_names,'64') LT 0)
   list_numeric_types=list_numeric_types[ok]
   list_numeric_size=list_numeric_size[ok]
   list_numeric_names=list_numeric_names[ok]
endif
if (!version.memory_bits GT 64) then MESSAGE, 'Please report  !!'
;
; default is UpperCase
list_numeric_names=STRUPCASE(list_numeric_names)
if KEYWORD_SET(lowercase) then begin
   list_numeric_names=STRLOWCASE(list_numeric_names)
endif
;
if KEYWORD_SET(verbose) or KEYWORD_SET(names) then begin
   print, format='(A11,A6, A6, A6)', 'Type :', 'val.', 'size', 'name' 
   for ii=0, N_ELEMENTS(list_numeric_names)-1 do begin
      print, format='(A1,A8,A2,2i6,A6)',' ',list_numeric_names[ii], ':', $
             list_numeric_types[ii], list_numeric_size[ii], list_names[ii]
   endfor
endif
;
if KEYWORD_SET(verbose) then STOP
;
end
