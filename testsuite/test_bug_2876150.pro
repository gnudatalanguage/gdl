pro test_bug_2876150

  if string(1,"()") ne "       1()" then exit, status=1
  if string(1,"(I)") ne "      1" then exit, status=1
  if string(1,"$(I)") ne "      1" then exit, status=1
  if string(1,"$(G)") ne "        1.000000000000000" then exit, status=1
  if string(1,".$(G)") ne "       1.$(G)" then exit, status=1

  if execute('print, "$()", 1') eq 1 then exit, status=1
  if execute('print, string(1,"$()")') eq 1 then exit, status=1

end
