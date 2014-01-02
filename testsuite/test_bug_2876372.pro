pro test_bug_2876372
  ; all should fail but not crash GDL
  if execute("a = HISTOGRAM([0], nbins=1000L*1000L*1000L, loc=b)") eq 1 then exit, status=1
  if execute("la_trired, 1, 1, 1")                                 eq 1 then exit, status=1
end
