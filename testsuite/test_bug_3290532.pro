pro test_bug_3290532
  if n_elements(strlen(string('abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabc',/print))) ne 1 then exit, status=1
end
