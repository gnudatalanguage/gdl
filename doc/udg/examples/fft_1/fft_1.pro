$ tail stddev*.pro
x = [1.31, 2.44, 2.51, 3.01, 2.96, 2.50, 0.05, 3.24, 0.13]
print, stddevsum(x), stddevfft(x)
