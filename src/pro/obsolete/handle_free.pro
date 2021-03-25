; support for obsolete pro/func.
pro handle_free,id
common gdlhandles
if n_elements(ids) eq 0 then return
w=where(ids eq id, count)
if count eq 1 then begin
 ptr_free,ptrs[w]
 ids[w]=-1 ; sufficient but bad practice
end
end

