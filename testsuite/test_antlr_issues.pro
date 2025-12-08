;
; GD. not a formal test. GDL should pass these commands without trouble.
;
; ---------------------------------------
; Modifications history :
; 
;
; AC 2025-12-08
; - Few lines are not OK in IDL 8.8.x and below, fully ok since IDL 8.9
;   in fact since IDL "compiles" before run the code, no way to fail
;   before 8.9 ... (syntax error messagess)
; - partially transform into a more formal test
;
; ---------------------------------------
;
pro TEST_ANTLR_ISSUES, help=help, test=test, verbose=verbose, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
   print, 'pro ANTLR_ISSUES, help=help, test=test, verbose=verbose, no_exit=no_exit'
   return
endif 
;
cumul_errors=0
errors=0
;
; ---------------------
; crashing issues:
; issue #812
a={z:[850.,1300.]}
b=ptr_new(a)
if KEYWORD_SET(verbose) then print,(*b).z(0)
if ((*b).z(0) NE 850.000) then ERRORS_ADD, errors, 'Case (*b).z(0)'
;
; ---------------------
; issue #26
struct={array7:[3,3,7,7,7,5,5],z:ptr_new(/allocate_heap)}
zval=findgen(12)
pointer=ptr_new(struct,/no_copy)
*(*pointer).z=zval
res1=(*(*pointer).z)[0:(*pointer).array7[3]]
res2=(*(*pointer).z)((*pointer).array7[1])
res3=(*(*pointer).z)((*pointer).array7(1))
if KEYWORD_SET(verbose) then print, res1, res2, res3
if ~ARRAY_EQUAL(res1, FINDGEN(8)) then ERRORS_ADD, errors, 'Case res1'
if (res2 NE 3.0) then ERRORS_ADD, errors, 'Case res2'
if (res3 NE 3.0) then ERRORS_ADD, errors, 'Case res3'
;
; ---------------------
; format etc issues:
; issue #1577
b=1 & z=cos((b+=2))
; issue #1252
a = "123
;" //emacs IDLWAVE mode is fragile...
if a ne 83 then exit,status=1
a = "123"
if ~ISA(a,/STRING)  then ERRORS_ADD, errors, 'Case Bad String 1'
a="7ull  
;;" //emacs IDLWAVE mode is fragile...
if (a ne 7ull) then ERRORS_ADD, errors, 'Case Bad value 7ull'
a="8ull & help,a
;;" //emacs IDLWAVE mode is fragile...
if ~ISA(a,/STRING)  then ERRORS_ADD, errors, 'Case Bad String 2'
;
; ---------------------
; various flavors new and old
help,0xA9, 0xFA62, 0xFFFFFFuLL, 'A9'x, 'FA62'x, 'FFFFFFF'xull
;
; this is OK in GDL and IDL only since 8.9. I don't know in FL
;
modern=1
if (GDL_IDL_FL() EQ 'IDL') then if GDL_VERSION() LT 80900 then modern=0
if (modern) then begin
   res=EXECUTE("help,0o377, 0o777777UL,'377'o, '777777'oUL,""12, ""12ub")
   res=EXECUTE("help,0b1010, 0b10101011111uLL,'1010'bb, '10101011111'bull")
   res=EXECUTE("help, 3+4j, 3e-2+4.6e-5i, -3.6+0.8dj")
endif
;
; ---------------------
; issue #830
print,"a","b","c",format='(/a/a/a)'
; GDL knows 2 flavours of C-Type format
print,"a","b","c",format='(%"%s-%s-%s")'
formatres2=STRING("a","b","c",format='(%"%s-%s-%s")')
if (formatres2 NE 'a-b-c') then ERRORS_ADD, errors, 'Case Bad Format 2'
print,"a","b","c",format="%s-%s-%s"
formatres3=STRING("a","b","c",format="%s-%s-%s")
if (formatres3 NE 'a-b-c') then ERRORS_ADD, errors, 'Case Bad Format 3'
;
; ---------------------
; issue #541
a1 = 'FF3A'x  ; existing notation
a2 = 0xFF3A   ; new notation
;
if ~ARRAY_EQUAL(a1,a2,/no_type) then ERRORS_ADD, errors, 'Case New notation'
; ---------------------
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
;
; ----------- back to normal convention ...
;
cumul_errors=errors
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_ANTLR_ISSUES', cumul_errors, short=short
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
