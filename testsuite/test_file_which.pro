pro test_file_which 
  if file_which('../..', 'gdl') ne '../../gdl' then exit, status=1
end
