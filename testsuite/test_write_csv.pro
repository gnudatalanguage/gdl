pro test_write_csv

if execute(a = [1,2,3,4,5] $
b = [6,7,8,9,10] $
write_csv,"issue_test.csv", a, b) ne 1 then exit, status=1


end
