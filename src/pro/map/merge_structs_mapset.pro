PRO merge_structs_mapset, first, second, OVERWRITE = over
COMPILE_OPT hidden
if n_elements(second) eq 0 then return

if n_elements(first) eq 0 then begin
    first = second
    return
endif

firsttags = tag_names(first)
secondtags = tag_names(second)
for i=0, n_elements(secondtags)-1 do begin
    w = where(firsttags eq secondtags[i], count)
    if count eq 0 then first = create_struct(first, secondtags[i], second.(i)) else if keyword_set(over) then first.(w[0]) = second.(i)
endfor
end
