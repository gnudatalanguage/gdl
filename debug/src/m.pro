pro m

s=600

a=dindgen(500,500)
b=shift(a,30,30)

s=systime(1)
while 1 do begin
    ss=systime(1)
    if ss ne s then break
endwhile
;for i=1,30 do begin
res=a # b
res=a ## b
;endfor
print,"<",systime(1)-ss+1

help,res,total(res)
end
