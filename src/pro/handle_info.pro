; GD, May 2015
; support for obsolete pro/func.
;
function HANDLE_INFO, id, parent=parent, first_child=first_child, $
                      num_children=num_children, sibling=sibling, $
                      valid_id=valid_id
;
common gdlhandles
;
res=REPLICATE(0,n_elements(id))
null=res
parid=res
if (N_PARAMS() ne 1) then return,res
;
for i=0L, N_ELEMENTS(id)-1 do begin
    w=WHERE(ids eq id[i], count)
    if count eq 1 then res[i]=1
    if count eq 1 then parid[i]=parentid[w]
endfor
;
if KEYWORD_SET(valid_id) then return,res
if TOTAL(res) eq 0 then return,res
if KEYWORD_SET(parent) then return, parid
if KEYWORD_SET(sibling) then return, null 
if KEYWORD_SET(first_child) then return, null
if KEYWORD_SET(num_children) then return, null
;
return, res
;
end
