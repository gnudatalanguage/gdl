;
; AC 2017-Dec-24
;
; used in "test_save_restore.pro" and revised version of "test_ntags.pro"
;
; -----------------------------------------------
;
pro GIVE_LIST_NUMERIC, list_numeric_types, list_numeric_names, list_numeric_size, verbose=verbose
;
; http://www.harrisgeospatial.com/docs/size.html
;
list_numeric_types=[1,2,3,4,5,6,9,12,13,14,15]
list_numeric_size =[1,2,4,4,8,8,16,2,4,8,8]
list_numeric_names=['byte','int','long',$
                    'float','double','complex','dcomplex', $
                    'uint','ulong','long64','ulong64']
;
if KEYWORD_SET(verbose) then begin
   for ii=0, N_ELEMENTS(list_numeric_names)-1 do begin
      print, format='(A8,A3,2i3)',list_numeric_names[ii], ':', list_numeric_types[ii], list_numeric_size[ii]
   endfor
endif
;
end
;
