pro test_write_csv
  a = [1,2,3,4,5]
  b = [6,7,8,9,10]
  if execute("write_csv,""issue_test.csv"", a, b") ne 1 then exit, status=1
end
