pro test_bug_3441031
  ; this caused a segfault
  res=EXECUTE("a = strjoin(strsplit((['a'])[1],'a'),'a')")
end
