; this simple stub just make the function boolean() do something OK.
; it is not a correct implementation of the BOOOLEAN type (a variant of the BYTE type).
; from a suggestion by J. Tappin see https://github.com/gnudatalanguage/gdl/issues/1960
function boolean, values

  sz = size(values)
  if sz[0] eq 0 then begin
     r=fix(values,type=1)
     return, r[0] ne 0
  endif
  

  sz[sz[0]+1] = 1
  bv = make_array(size = sz)

  for j = 0, n_elements(bv)-1 do bv[j] = keyword_set(values[j])

  return, bv
end
