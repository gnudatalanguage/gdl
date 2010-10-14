pro test_bug_3054361
  if n_elements(size(string(make_array(7,/string,/index),/print))) ne 4 then begin
    exit, status=1
  endif
end
