pro test_bug_2846561

  x=indgen(50)

  err = 0  

  if ~array_equal([1l,1], histogram(x, min=15, max=16)) then err++
  if ~array_equal([1l,1,1,1], histogram(x,min=30,max=33)) then err++
  a = histogram(x, min=15, max=16, reverse_indices=ir)

end
