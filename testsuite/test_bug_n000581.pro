pro test_bug_n000581
  ppos = stregex('abcdefg', '[xyz]', length=lstr)
  if lstr ne -1 then exit, status=1
end
