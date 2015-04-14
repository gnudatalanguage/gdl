; support for obsolete pro/func.
function  handle_info, id,parent=parent,first_child=first_child,num_children=num_children,sibling=sibling,valid_id=valid_id
common gdlhandles
res=replicate(0,n_elements(id))
null=res
parid=res
if n_params ne 1 then return,res
for i=0L,n_elements(id) do begin
 w=where(ids eq id[i], count)
 if count eq 1 then res[i]=1
 if count eq 1 then parid[i]=parentid[w]
endfor
if keyword_set(valid_id) then return,res
if total(res) eq 0 then return,res
if keyword_set(parent) then return, parid
if keyword_set(sibling) then return, null 
if keyword_set(first_child) then return, null
if keyword_set(num_children) then return, null
return, res
end
