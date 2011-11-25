pro test_bug_3441031
  ; this caused a segfault
  a = strjoin(strsplit((['a'])[1],'a'),'a')
end
