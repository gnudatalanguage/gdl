; not a formal test. GDL should pass these commands without trouble.
pro test_antlr_issues
; crashing issues:
; issue #812
a={z:[850.,1300.]}
b=ptr_new(a)
print,(*b).z(0)
; issue #26
struct={array7:[3,3,7,7,7,5,5],z:ptr_new(/allocate_heap)}
zval=findgen(12)
pointer=ptr_new(struct,/no_copy)
*(*pointer).z=zval
print,(*(*pointer).z)[0:(*pointer).array7[3]]
print,(*(*pointer).z)((*pointer).array7[1])
print,(*(*pointer).z)((*pointer).array7(1))
; format etc issues:
; issue #1577
b=1 & z=cos((b+=2))
; issue #1252
a = "123
;" //emacs IDLWAVE mode is fragile...
if a ne 83 then exit,status=1
a = "123"
if ~ISA(a,/STRING)  then exit,status=1
a="7ull
if a ne 7ull then exit,status=1
a="8ull & help,a
if ~ISA(a,/STRING)  then exit,status=1
; various flavors new and old
help,0xA9, 0xFA62, 0xFFFFFFuLL, 'A9'x, 'FA62'x, 'FFFFFFF'xull
help,0o377, 0o777777UL,'377'o, '777777'oUL,"12, "12ub
help,0b1010, 0b10101011111uLL,'1010'bb, '10101011111'bull
help, 3+4j, 3e-2+4.6e-5i, -3.6+0.8dj
; issue #830
print,"a","b","c",format='(/a/a/a)'
; GDL knows 2 flavours of C-Type format
print,"a","b","c",format='(%"%s-%s-%s")'
print,"a","b","c",format="%s-%s-%s"
; issue #541
a = 'FF3A'x  ; existing notation
a = 0xFF3A   ; new notation
; issue #52: shothand notation, but will crash as this is not an interactive input. Removed.
; for i=0,2 do begin print,i
; four different ways must be accepted:
for i=0,1 do print,i
for i=0,1 do begin print,i & endfor
for i=0,1 do begin $
print,i
endfor
for i=0,1 do begin
   print,i
endfor

end
