pro test1, x, y
help, x, y
end

function test2, x
return,x
end

function test3, x
return,x[0].p[0]
end

pro test4, x, y
  x[2] = 10.
  print, y
end

pro test_brain
a = indgen(5)
test1, a, a

a = indgen(5)
(test2(a))[0] = -2
print, a

a = {stest,tag:0}
(test2(a)).tag = 5
print, a

a = indgen(5)
print, a
test4, a, a
end



