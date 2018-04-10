pro test_heap_refcount
err=0
; closed bug 708: This didn't work.
ab = ptr_new(fltarr(12))
cmp = {a:ab, b:ab}

if(heap_refcount(ab) ne 3) then err++

jptr = ptrarr(4)
for k=1,3 do jptr[k] = ptr_new(findgen(100+k))
jptr[0] = ab
if(total(heap_refcount(jptr)) ne 3+3) then err++
kptr = ptrarr(4)
for k=0,3 do kptr[k] = jptr[k]
if(total(heap_refcount(jptr)) ne 3+3+4) then err++

if err ne 0 then exit 1

end
