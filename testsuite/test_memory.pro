pro test_memory
  mem = memory(/curr)
  a = dblarr(9999999)
  if (mem ge memory(/curr)) then begin
    message, 'reported memory consumption should increase after allocating a big array!', /conti
    exit, status=1
  endif
end
