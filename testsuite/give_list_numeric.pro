;
; AC 2017-Dec-24
;
; used in "test_save_restore.pro" and revised version of "test_ntags.pro"
;
; ----------------------------------------------------
; Modifications history :
;
; 2018-Feb-07 : AC. 
; 1/ Since now, default is Upper. 
; 2/ being able to select Integer types only
;
; -----------------------------------------------
;
pro GIVE_LIST_NUMERIC, list_numeric_types, list_numeric_names, $
                       list_numeric_size, verbose=verbose, $
                       integer=integer, lowercase=lowercase
;
; http://www.harrisgeospatial.com/docs/size.html
;
list_integer_types=[1,1,1,0,0,0,0,1,1,1,1]
list_numeric_types=[1,2,3,4,5,6,9,12,13,14,15]
list_numeric_size =[1,2,4,4,8,8,16,2,4,8,8]
list_numeric_names=['byte','int','long',$
                    'float','double','complex','dcomplex', $
                    'uint','ulong','long64','ulong64']
;
if KEYWORD_SET(integer) then begin
   ok=WHERE(list_integer_types GT 0)
   list_numeric_types=list_numeric_types[ok]
   list_numeric_size=list_numeric_size[ok]
   list_numeric_names=list_numeric_names[ok]
endif
;
; default is UpperCase
list_numeric_names=STRUPCASE(list_numeric_names)
if KEYWORD_SET(lowercase) then begin
   list_numeric_names=STRLOWCASE(list_numeric_names)
endif
;
if KEYWORD_SET(verbose) then begin
   print, format='(A11,A6, A6)', 'Type :', 'val.', 'size' 
   for ii=0, N_ELEMENTS(list_numeric_names)-1 do begin
      print, format='(A1,A8,A2,2i6)',' ',list_numeric_names[ii], ':', $
             list_numeric_types[ii], list_numeric_size[ii]
   endfor
endif
;
end
;
