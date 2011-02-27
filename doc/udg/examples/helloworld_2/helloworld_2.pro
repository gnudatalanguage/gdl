a = fltarr(1024) 
a[9] = 1
plot, wtn(a, 4, /inverse)
xyouts, 480, .02, 'Hello world!', charsize=2
