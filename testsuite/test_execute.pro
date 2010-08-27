; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
pro test_execute

  if execute('print,execute([''''])') then begin
    message, 'EXECUTE should not accept array arguments', /conti
    exit, status=1
  endif

end
