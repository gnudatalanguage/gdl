pro Test_JD_OP_Tut

a={b:findgen(5)}
print,a.b[2]
print,'      2.00000 expected.'

a={b:ptr_new(findgen(5))}
print,*a.b
print,'      0.00000      1.00000      2.00000      3.00000      4.00000 expected.'
print,(*a.b)[1]
print,'      1.00000 expected.'
a=ptr_new({b:findgen(5)})
print,(*a).b[1]
print,'      1.00000 expected.'
a=replicate({b:findgen(5)},3)
print,a[0].b[1]
print,'      1.00000 expected.'
a=replicate({b:ptr_new(findgen(5))},3)
print,*a[1].b[0]
print,'      0.00000      1.00000      2.00000      3.00000      4.00000 expected.'
print,(*a[1].b)[1]
print,'      1.00000 expected.'
a=replicate({b:ptr_new([ptr_new(findgen(5,2)), $
                        ptr_new({c:replicate({d:findgen(5)},2)})])},3)
print,(*(*a[0].b)[1]).c[1].d[3]
print,'      3.00000 expected.'
print,a[0].b,(*a[0].b)[1]
print,'<PtrHeapVarX><PtrHeapVarX> expected.'
print,++(*(*a[0].b)[1]).c[1].d[3]^2
print,'      16.0000 expected.'
a={b:{c:[ptr_new(5.),ptr_new(6.)]}}
print,*a.b.c[0]
print,'      5.00000 expected.'
print,(*a.b.c[0])
print,'      5.00000 expected.'

end
