pro test_bug_3298378
  if (size(expint(3,2)))[0] ne 0 then exit, status=1
  if (size(igamma(3,2)))[0] ne 0 then exit, status=1
  if (size(beta(3,2)))[0] ne 0 then exit, status=1
end
