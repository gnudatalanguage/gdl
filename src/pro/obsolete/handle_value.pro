; support for obsolete pro/func.
pro handle_value,id,value,no_copy=no_copy,set=set
common gdlhandles
if n_params() ne 2 then return
w=where(ids eq id, count)
if count ne 1 then return
if keyword_set(set) then begin
    ptr_free,ptrs[w]
    ptrs[w]=ptr_new(value)
endif else begin
    value=*(ptrs[w[0]])
endelse
end
