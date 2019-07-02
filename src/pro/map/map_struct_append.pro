; appends a struct element to a struct avoiding dupes
;------------------------------------------------------------------------
PRO map_struct_append, in, Tag, Val, supercede=supercede
  COMPILE_OPT hidden
  
  ntags=N_TAGS(in)
  
  if ntags eq 0 then begin
     in = CREATE_STRUCT(tag, val)
     return
  endif
  
  ttags=tag_names(IN)

  index=where(ttags eq TAG, count)
  if count eq 0 then begin 
     In = CREATE_STRUCT(in, tag, val) 
     return
  endif
  if keyword_set(supercede) then in.(index[0]) = val
end




