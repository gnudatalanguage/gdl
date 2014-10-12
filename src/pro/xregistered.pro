function xregistered,name,noshow=noshow

common managed_by_gdl, ids, names
forward_function tidyManagedCommon

IF ( ~keyword_set(ids)) then return,0
;update list of managed widgets
tidyManagedCommon
;which may result in a zero id:
if (ids[0] eq 0) then return, 0 

occurences=where(names eq name, count)
if ( count gt 0 ) then begin
 if ( ~keyword_set(noshow) ) then widget_control, ids[occurences[0]], /show
 return, count
endif
return, 0
end
