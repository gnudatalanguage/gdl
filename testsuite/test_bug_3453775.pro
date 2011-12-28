pro test_bug_3453775
  a=[1,2,3]
  aa=shift(a,0) ; this caused a segfault
end
