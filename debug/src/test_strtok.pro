pro test_strtok

print,'No ESC, only sep'
r=strtok(',,,,,,',',',L=l)
print,r
print,l
print,'No ESC, only sep, keep'
r=strtok(',,,,,,',',',/PR,L=l)
print,r
print,l

print,'No ESC, only sep, EXTRACT'
r=strtok(',,,,,,',',',/EX)
help,r
print,'No ESC, only sep, keep, EXTRACT'
r=strtok(',,,,,,',',',/PR,/EX)
help,r

print,'ESC, only sep'
r=strtok('b,,bb,,,,',',',ESC='b',L=l)
print,r
print,l
print,'ESC, only sep, keep'
r=strtok('b,,bb,,,,',',',/PR,ESC='b',L=l)
print,r
print,l

print,'ESC, only sep, EXTRACT'
r=strtok('b,,bb,,,,',',',ESC='b',/EX)
print,r
print,'ESC, only sep, keep, EXTRACT'
r=strtok('b,,bb,,,,',',',ESC='b',/PR,/EX)
print,r



print,'ESC, keep, EXTRACT'
r=strtok('ab,a,cbc,def,ghj,klm,',',',ESC='b',/PR,/EX)
print,r
print,'ESC, keep'
r=strtok('ab,a,cbc,def,ghj,klm,',',',ESC='b',/PR)
print,r

end
