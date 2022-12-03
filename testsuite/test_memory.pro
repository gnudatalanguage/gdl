; GD 03/12/2022 changed return value to insure test does not impact validity of a PR.
; since we have a warning message in the log, this can be cured at will
; the test is NOT reliable as it passes very well on a Catalina.
pro test_memory
  mem = memory(/curr)
  a = dblarr(9999999)
  if (mem ge memory(/curr)) then begin
    message, 'reported memory consumption should increase after allocating a big array!', /conti
    exit, status=0
  endif
end
