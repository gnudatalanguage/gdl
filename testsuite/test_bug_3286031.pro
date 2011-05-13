pro test_bug_3286031
  if strlen(string(str_sep('ahasadfasdf','dfa'), /print)) ne 11 then exit, status=1 
end
