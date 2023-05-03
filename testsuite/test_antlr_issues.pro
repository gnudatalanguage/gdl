; not a formal test. GDL should pass these commands without trouble.
pro test_antlr_issues
; issue #1577
b=1 & z=cos((b+=2))
; issue #1252
a = "123
;" //emacs IDLWAVE mode is fragile...
if a ne 83 then exit,status=1
a = "123"
if ~ISA(a,/STRING)  then exit,status=1
; issue #830
print,"a","b","c",format='(/a/a/a)'
; GDL knows 2 flavours of C-Type format
print,"a","b","c",format='(%"%s-%s-%s")'
print,"a","b","c",format="%s-%s-%s"
; issue #541
a = 'FF3A'x  ; existing notation
a = 0xFF3A   ; new notation
; issue #52: shothand notation, but will crash as this is not an interactive input. Removed.
; for i=0,2 do begin a=i
end
