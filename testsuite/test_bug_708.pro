pro test_bug_708
; example given for bug 708: Unhandled exception
ab = ptr_new(fltarr(12))
cmp = {a:ab, b:ab}
heap_free, cmp
end
