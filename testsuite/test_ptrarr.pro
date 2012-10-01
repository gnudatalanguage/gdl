;
; bug 3573202 reported by Cokhavim
;
pro TEST_PTRARR
;
p=PTRARR(2,2,/allocate_heap)
i=1
j=0
*p[i,j] = [3,2,5]
;
pp=PTRARR(2,2,/allocate_heap)
*pp[1,0] = [3,2,5]
;
ppp=PTRARR(2,2,/allocate_heap)
*ppp[1,0] = DIST(128)
;
end
