pro t

a=indgen(80,80,80,80)

s=systime(1)
while 1 do begin
    ss=systime(1)
    if ss ne s then break
endwhile
res=shift( a, 3,7,11,99)
print,systime(1)-ss

help,res
end

pro tt
for i=128LL, 138 do begin

a=dblarr(i,i)
a[*]=1
b=dblarr(10,10)
b[*]=1

;a=dindgen(i,i)
;b=dindgen(10,10)

print,i
r=convol(a,b,/EDGE_W)
print,ULong64(total(r))
r=convol(a,b,/EDGE_T)
print,ULong64(total(r))
print,i^2*100
print
r=convol(a,b)
print,ULong64(total(r))
r=convol(a,b,C=0)
print,ULong64(total(r))
print,(i-9)^2*100
print
endfor
end

pro ttt
a=lindgen(7,5,3)
b=lindgen(2,3,2)


print,'0'
print,convol(a,b,4)
print,'1'
print,convol(a,b,4,/EDGE_W)
print,'2'
print,convol(a,b,4,/EDGE_T)
print,'3'
print,convol(a,b,4,C=0)
print,'4'
print,convol(a,b,4,/EDGE_W,C=0)
print,'5'
print,convol(a,b,4,/EDGE_T,C=0)


H=lindgen(n_elements(a))
 print,'0'
 r=convol(a,b,4)
print,total(r),total(r*H)
 print,'1'
 r=convol(a,b,4,/EDGE_W)
print,total(r),total(r*H)
 print,'2'
 r=convol(a,b,4,/EDGE_T)
print,total(r),total(r*H)
 print,'3'
 r=convol(a,b,4,C=0)
print,total(r),total(r*H)
 print,'4'
 r=convol(a,b,4,/EDGE_W,C=0)
print,total(r),total(r*H)
 print,'5'
 r=convol(a,b,4,/EDGE_T,C=0)
print,total(r),total(r*H)


a=dindgen( 801, 801)
b=dindgen( 50, 50)

;a=lindgen( 500, 500)
;b=lindgen( 10, 10)

print,'...'
print,'center'
s=systime(1)
while 1 do begin
    ss=systime(1)
    if ss ne s then break
endwhile
c=convol(a,b)
print,systime(1)-ss
print,total(c)
print,'center=0'
s=systime(1)
while 1 do begin
    ss=systime(1)
    if ss ne s then break
endwhile
c=convol(a,b,C=0)
print,systime(1)-ss
print,total(c)

print,'center,wrap'
s=systime(1)
while 1 do begin
    ss=systime(1)
    if ss ne s then break
endwhile
c=convol(a,b,/EDGE_W)
print,systime(1)-ss
print,total(c)

print,'center=0,wrap'
s=systime(1)
while 1 do begin
    ss=systime(1)
    if ss ne s then break
endwhile
c=convol(a,b,C=0,/EDGE_W)
print,systime(1)-ss
print,total(c)

print,'center,trunc'
s=systime(1)
while 1 do begin
    ss=systime(1)
    if ss ne s then break
endwhile
c=convol(a,b,/EDGE_T)
print,systime(1)-ss
print,total(c)

print,'center=0,trunc'
s=systime(1)
while 1 do begin
    ss=systime(1)
    if ss ne s then break
endwhile
c=convol(a,b,C=0,/EDGE_T)
print,systime(1)-ss
print,total(c)
print,'*** end ***'
end
