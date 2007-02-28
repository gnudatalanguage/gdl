;
; AC, le 27-fev-2007
;
; very simple tests, expecially on unamed variable
;
pro TEST_SAVE
;
a='Hello'
line='----------------------'
;
save, a, /verbose
print, 'Zero-th test performed --> shoudl work fine !'
print, line
;
; first serie of test: must return because
; unnamed variables
;
save, a, '12'
print, 'first test performed (no unamed variable)'
print, line
;
save, a, 12
print, 'second test performed (no unamed variable)'
print, line
;
; the array [a,a] is a "unamed variable"
save, a, [a,a]
print, 'third test performed (no unamed variable)'
print, line
;
save, a, /all
print, '4a th test performed (warning for /all keyword)'
print, line
;
save, /all
print, '4b th test performed (Nothing to do higher priority than 
print, 'warning for /all keyword)'
print, line
;
; no check if same variable ...
;
save, a, a
print, '5th test performed (no warning if duplicate variable)'
print, line
;
end
