; by Alain (moved here from src/pro/str_sep.pro)
pro test_str_sep
  resu1 = STR_SEP('../foo.txt','.')
  err = 0
  if n_elements(resu1) NE 4 then begin
    MESSAGE, 'problem 1', /conti
    err++
  endif
  if resu1[2] NE '/foo' then begin
    MESSAGE, 'problem 1bis', /conti
    err++
  endif
  if resu1[3] NE 'txt' then begin
    MESSAGE, 'problem 1ter', /conti
    err++
  endif
  if err ne 0 then exit, status=1
end
