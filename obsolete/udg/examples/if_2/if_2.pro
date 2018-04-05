$ cat replace_with_nans.pro
x = [1.1, 2.1, -3.3, 4.1, -999, 6]
replace_with_nans, x, -999
print, x
