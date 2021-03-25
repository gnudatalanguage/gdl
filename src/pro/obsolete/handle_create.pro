; support for obsolete pro/func.
function handle_create,id,first_child=first_child,no_copy=no_copy,sibling=sibling,value=value
; note:first_child & sibling are ignored.
common gdlhandles,ids,ptrs,parentid
if n_elements(ids) eq 0 then begin 
 ids=[1]
 last=1
 if n_elements(value) eq 0 then ptrs=[ptr_new()] else ptrs=[ptr_new(value)]
 if (n_params() eq 0) then parentid=[0] else parentid=[id] 
endif else begin
 last=ids[-1]+1
 ids=[ids,last]
 if n_elements(value) eq 0 then ptrs=[ptrs,ptr_new()] else ptrs=[ptrs,ptr_new(value)]
 if (n_params() eq 0) then parentid=[parentid,0] else parentid=[parentid,id] 
end
return,last
end

