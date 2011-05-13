pro test_bug_3152899
  a = ptr_new(complexarr(10)) & help, fft(*a, /over)
  a = ptr_new(complexarr(10)) & help, fft(*a, /over)
  a = ptr_new(1) & *a= reform(*a, /over) & *a= reform(*a, /over)
  a = ptr_new(1) & *a= reform(*a, /over) & *a= reform(*a, /over)
end
