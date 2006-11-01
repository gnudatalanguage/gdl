;; test some language elements of GDL

pro if_test

if 1 then $
  if 0 then begin & end $
else if 1 then $ 
  if 0 then begin & end $
else a=1

if a ne 1 then print,'***IF: ERROR'

print,'IF: OK'
end


pro switch_test

a='abc'

;; empty
switch a of

    1:
    complex(1): begin & end
    'def':
    'abc':
    1.0: begin & end
    else:
endswitch

switch a of

    1: print,'***SWITCH: ERROR'
    complex(1): print,'***SWITCH: ERROR'
    'def': print,'***SWITCH: ERROR'
    'abc': begin
        abc = 1
    end
    1.0: fall = abc+1
    else: e=1
endswitch

if abc ne 1 then print,'***SWITCH: ERROR'
if fall ne abc+1 then print,'***SWITCH: ERROR'
if e ne 1 then print,'***SWITCH: ERROR'

switch 1 of
   0: print,"***SWITCH: ERROR"
   0: print,"***SWITCH: ERROR"
   1:       
endswitch

print,'SWITCH: OK'
end

pro case_test

a='abc'

case a of

    1: print,'***CASE: ERROR'
    complex(1): print,'***CASE: ERROR'
    'def': print,'***CASE: ERROR'
    'abc': begin
        c=1
    end
    1.0: print,'***CASE: ERROR'
    else: print,'***CASE: ERROR'
endcase

if c ne 1 then print,'***CASE: ERROR'

case a of

    1: print,'***CASE: ERROR'
    complex(1): print,'***CASE: ERROR'
    'def': print,'***CASE: ERROR'
    'a': begin
        print,'***CASE: ERROR'
    end
    1.0: print,'***CASE: ERROR'
    else: e=1
end

if e ne 1 then print,'***CASE: ERROR'

case 1 of
   0: print,"***CASE: ERROR"
   0: print,"***CASE: ERROR"
   1:       
endcase

print,'CASE: OK'
end

pro while_test

i=0
while i lt 100 do begin
i=i+1
endwhile

if i eq 100 then print,'WHILE: OK' else print,'***WHILE: ERROR'
end

pro for_test

c=0
for i=0,99 do c=c+1 

if c eq 100 then print,'FOR: OK' else print,'***FOR: ERROR'

c=0
for i=0,990,10 do c=c+1 

if c eq 100 then print,'FOR: Step: OK' else print,'***FOR: Step: ERROR'
end

pro repeat_test

i=0
repeat begin 
i=i+1
endrep until (i eq 100)

repeat begin 
endrep until (i eq 100)

repeat begin 
endrep until 1

if i eq 100 then print,'REPEAT: OK' else print,'***REPEAT: ERROR'
end

pro goto_test

;; simple
goto,jump11
print,'***GOTO: Error'
jump21:
goto,jump31
print,'***GOTO: Error'
jump11:
goto,jump21
print,'***GOTO: Error'
jump31:


count=0
;; within loop
for i=0,10 do begin

goto,jump1
print,'***GOTO: Error1'
jump2:
goto,jump3
print,'***GOTO: Error2'
jump1:
goto,jump2
print,'***GOTO: Error3'
jump3:
goto,jump4 ; even to jump out of the block and in again is ok
jump5:

if 0 then goto,jump5 else goto,jump7
print,'***GOTO: Error4'

jump7:

if 1 then begin

    goto,jump8
    print,'***GOTO: Error5'
jump8:

endif else begin

    print,'***GOTO: Error6'
endelse


count=count+1
endfor

goto,jump6
jump4:
goto,jump5
jump6:

if count ne 11 then print,'***GOTO: ERROR7'

print,'GOTO: OK'

end

function ret99
return,99b
end

function ret,a
return,a
end

function retEq,a
return,(a=3)
end

function retTag,a
return,(a.t=3)
end

function ret2,b
return,ret(b)
end

function ret3,b
return,ret2(b)
end

function retOverwriteLocal
a=[['m','a'],['r','c']]
return,reform(a,4,/OVERWRITE)
end
function retOverwriteGlobal,a
return,reform(a,4,/OVERWRITE)
end
function retOverwriteExpression
a=[['m','a'],['r','c']]
return,reform(a[*],4,/OVERWRITE)
end

pro ret_test

if ret99() ne 99.0 then print,'***RET: ERROR1'
if retEq(u) ne 3 then print,'***RET: ERROR2'
(retEq(u2)) = 4
if u2 ne 4 then print,'***RET: ERROR4'


(ret3(u3))=7
if u3 ne 7 then print,'***RET: ERROR5'

s={t:0}
if retTag(s) ne 3 then print,'***RET: ERROR6'
;(ret(s))=2

n = retOverwriteLocal()
if n[2] ne "r" then print,'***RET: ERROR7'

a=[[1,2],[3,4]]
(retOverwriteGlobal(a))--
if a[1] ne 1 then print,'***RET: ERROR8'

n = retOverwriteExpression()
if n[2] ne "r" then print,'***RET: ERROR9'

a=1
c=ret(4*(3+reform(a,1,/OVERWRITE)+1)*2)
if a ne 1 then print,'***RET: ERROR10'
if c ne 40 then print,'***RET: ERROR11'

common ret_common,aa

aa=1
c=ret(4*(3+reform(aa,1,/OVERWRITE)+1)*2)
if aa ne 1 then print,'***RET: ERROR12'
if c ne 40 then print,'***RET: ERROR13'

print,'RET: OK';
end


pro struct_test

s={tag: indgen(5)}

if s.tag[ 3] ne 3 then print,'***STRUCT: ERROR1'

s.tag++

if s.tag[ 3] ne 4 then print,'***STRUCT: ERROR2'

s.tag = 9

if s.tag[ 3] ne 9 then print,'***STRUCT: ERROR3'

s.tag = [10,11,12]

if s.tag[ 2] ne 12 then print,'***STRUCT: ERROR4'

ss=[s,s]

ss[*].tag=indgen(5,2)

if ss[1].tag[2] ne 7 then print,'***STRUCT: ERROR5'

s={s:indgen(5)}
ss={ss:s}
sss={sss:ss}

;p=ptr_new( sss)
;((*p).sss.ss)[2] = 5
;print,((*p).ss)[2]

sss.sss.ss.s[2] = 10
if sss.sss.ss.s[2] ne 10 then print,'***STRUCT: ERROR6'

(ret(sss)).sss.ss.s[1] = 11
if sss.sss.ss.s[1] ne 11 then print,'***STRUCT: ERROR7'
if (ret(sss)).sss.ss.s[3] ne 3 then print,'***STRUCT: ERROR8'

(ret(sss)).sss.(0).s[1] = 11
if sss.(0).ss.s[1] ne 11 then print,'***STRUCT: ERROR9'

if ({a:3}).a ne 3 then  print,'***STRUCT: ERROR10'

;; array and struct
s4=[sss,sss]
s5=[[s4],[s4]]
if s5[1,1].sss.ss.s[3] ne 3 then print,'***STRUCT: ERROR11'

a = {a0, var1: 0, var2: 0.0d0}
b = {a0, '0L', ' 1.0'}

print,'STRUCT: OK'
end

pro multi,a,b,c
b=9
if a ne 9 then print,'***MULTI: ERROR1'
if c ne 9 then print,'***MULTI: ERROR2'
end

pro multi_test
multi,a,a,a
if a ne 9 then print,'***MULTI: ERROR3'
print,'MULTI: OK'
end

function o::init
self.a=9
return,1
end

function o::get
return,self.a
end

pro o::test
self.a=1
if self.a ne 1 then print,'***OBJECT: ERROR1'
self[0].a=2
if self[0].a ne 2 then print,'***OBJECT: ERROR2'
(self).a[0]=3
if (self[0]).a[[-2]] ne 3 then print,'***OBJECT: ERROR3'
if self[[0]].a ne 3 then print,'***OBJECT: ERROR4'
;; would be legal in GDL (but IDL complains)
;if (self[[0]]).a ne 3 then print,'***OBJECT: ERROR5'
end

pro o::cleanup
common object_test,o_cleanup
o_cleanup=-1
end

pro object_test

common object_test

;; object
s={o,a:0}
obj=obj_new('o')

if obj->get() ne 9 then print,'***OBJECT: ERROR6'

obj->test
(obj)->test
obj[0]->test
(obj[0])->test

if not obj_valid( obj) then print,'***OBJECT: ERROR7'

obj_destroy,obj

if obj_valid( obj) then print,'***OBJECT: ERROR8'
if o_cleanup ne -1 then print,'***OBJECT: ERROR9'

print,'OBJECT: OK'
end

pro ct1
common tc,x,y,z
if x ne 3 then print,'***COMMON: ERROR1'
y=7
end

pro ct2
common tc
if x ne 3 then print,'***COMMON: ERROR2'
if y ne 7 then print,'***COMMON: ERROR3'
z=9
end

function ct3
common tc3,a
return,a
end

pro common_test
common tc,a,b,c
common tc,d,e,f
common tc

a=3
ct1
ct2

if a ne 3 then print,'***COMMON: ERROR4'
if b ne 7 then print,'***COMMON: ERROR5'
if c ne 9 then print,'***COMMON: ERROR6'
if d ne 3 then print,'***COMMON: ERROR7'
if e ne 7 then print,'***COMMON: ERROR8'
if f ne 9 then print,'***COMMON: ERROR9'
if x ne 3 then print,'***COMMON: ERROR10'
if y ne 7 then print,'***COMMON: ERROR11'
if z ne 9 then print,'***COMMON: ERROR12'

(ct3()) = 2
if ct3() ne 2 then print,'***COMMON: ERROR13'

print,'COMMON: OK'
end

pro set22,a,X=x
a=2
x=2
end

pro ref_test,MEMCHECK=mCheck

set22,b
if b ne 2 then print,"***REF: ERROR1"

if not keyword_set( mCheck) then begin

    p=ptr_new(/alloc)
    set22,*p
    if *p ne 2 then print,"***REF: ERROR2"
    ptr_free,p

    p=ptr_new(/alloc)
    pp=ptr_new(p)
    set22,**pp
    if **pp ne 2 then print,"***REF: ERROR3"
    ptr_free,p,pp

endif

a=indgen(3)
set22,reform(a,1,3,/OVERWRITE)
if a ne 2 then print,"***REF: ERROR4"

a=4 & x=4
set22,a++,X=x++
if a ne 5 or x ne 5 then print,"***REF: ERROR5"

a=1 & x=1
set22,++a,X=++x
if a ne 2 or x ne 2 then print,"***REF: ERROR6"

a=1 & x=1
set22,(a=4),X=(x=4)
if a ne 2 or x ne 2 then print,"***REF: ERROR7"

a=4 & x=4
set22,++(a=1),X=++(x=1)
if a ne 2 or x ne 2 then print,"***REF: ERROR8"

set22,++(a[0]=4),X=++(x[0]=4)
if a ne 5 or x ne 5 then print,"***REF: ERROR9"

print,'REF: OK'
end

function base::init
common inheritance,baseInit,baseCleanup,derivInit,derivCleanup
baseInit=1
return,1
end
pro base::cleanup
common inheritance,baseInit,baseCleanup,derivInit,derivCleanup
baseCleanup=1
end
pro base::set
self.b='base'
end
pro base::setbase
self.b='base'
end
function base::get
return,self.b
end

function deriv::init
common inheritance,baseInit,baseCleanup,derivInit,derivCleanup
r=self->base::init()
derivInit=1
return,1
end
pro deriv::cleanup
common inheritance,baseInit,baseCleanup,derivInit,derivCleanup
self->base::cleanup
derivCleanup=1
end
pro deriv::set
self.b='deriv'
end

pro inheritance_test

base={ base, b: ''}
deriv={ deriv, INHERITS base, d: ''}

common inheritance

o=obj_new( 'deriv')
if baseInit ne 1 then print,'***INHERITANCE: ERROR1'
if derivInit ne 1 then print,'***INHERITANCE: ERROR2'

o->setbase
if o->get() ne 'base' then print,'***INHERITANCE: ERROR3'
o->set
if o->get() ne 'deriv' then print,'***INHERITANCE: ERROR4'
o->base::set
if o->get() ne 'base' then print,'***INHERITANCE: ERROR5'

obj_destroy, o
if baseInit ne 1 then print,'***INHERITANCE: ERROR6'
if derivInit ne 1 then print,'***INHERITANCE: ERROR7'

print,'INHERITANCE: OK'
end

function syntax_test,a,b,c
return,byte(a,b,c)
end

pro syntax_test

s={a:0}

t=s[0].a
t=s[[0]].a
t=(s[0]).a
t=(s[[0]]).a

b=(a=2)
if b ne 2 then print,'***SYNTAX_TEST: ERROR1'

a=(byte(1,0,1))[0]
a=(syntax_test(1,0,1))[0]

;((a=4)) syntax error
;(a=2)=3  forbidden in GDL (as it hardly makes sense)

end

pro inc_test

a=1

(ret(a))++
++(ret(a))

if a ne 3 then print,"***INC: ERROR1"

b = intarr(3)

b[1]++
++b[1]
(b[1])++
++(b[1])

if b[1] ne 4 then print,"***INC: ERROR2"

b[[1,2]]+=1
if b[1] ne 5 then print,"***INC: ERROR3"

(a=2)++
if a ne 3 then print,'***INC_TEST: ERROR4'

((a=2))++
if a ne 3 then print,'***INC_TEST: ERROR5'

++(a=2)
if a ne 3 then print,'***INC_TEST: ERROR6'

++((a=2))
if a ne 3 then print,'***INC_TEST: ERROR7'

print,'INC: OK'
end



pro continuebreak_test

repeat begin

    goto,start

    print,'***CONTINUE/BREAK: ERROR2'

start:

    for i=0,40,2 do begin

        if i lt 6 then continue

        if i lt 6 then print,'***CONTINUE/BREAK: ERROR1'

        if i gt 10 then break

        if i gt 10 then print,'***CONTINUE/BREAK: ERROR2'

        goto,in1
        print,'***CONTINUE/BREAK: ERROR2'
        goto,out1
in1:

    endfor

goto,out2
out1:
out2:

endrep until 1

print,'CONTINUE/BREAK: OK'
end


pro e2,A=a,B=b
if a ne 'a' then print,'EXTRA: ERROR1'
if b ne 'b' then print,'EXTRA: ERROR2'
end

pro eref,_REF_EXTRA=ex
e2,_EXTRA=ex
end

pro eval,_EXTRA=ex
e2,_EXTRA=ex
end

pro e1,R2=a,R1=b
a=1
b=2
end

pro eret,_REF_EXTRA=ex
e1,_EXTRA=ex
end


pro extra_test,_REF_EXTRA=ex

eval,b='b',a='a',c=u ;;
eref,b='b',a='a',c=u

eret,r1=r1,r2=r2

if r1 ne 2 then print,'***EXTRA: ERROR3'
if r2 ne 1 then print,'***EXTRA: ERROR4'

print,'EXTRA: OK'
end

pro expr_test

a=0
a++
a *= 2
a ^= 2
b = a+2*3-(9*2)+9
if b ne 1 then print,'***EXPR: ERROR1'

a=indgen(6,6,6)
a[3,3,3]=-1
if a[3,3,3] ne -1 then print,'***EXPR: ERROR2'

a=a+100
if a[3,3,3] ne 99 then print,'***EXPR: ERROR2a'

a[*]=a[*]-100
if a[3,3,3] ne -1 then print,'***EXPR: ERROR2b'


sdef={s,a:intarr(3),s:strarr(3)}
s1={s,indgen(3),s:sindgen(3)}
if s1.a[1] ne 1 then print,'***EXPR: ERROR3'
s2={s}
sArr=[s2,s2]
sArr[1].s[1]='test'
if sArr[1].s[1] ne 'test' then print,'***EXPR: ERROR4'

a=1
c=4*(3+reform(a,1,/OVERWRITE)+1)*2
if c ne 40 then print,'***EXPR: ERROR5'
if a ne 1 then print,'***EXPR: ERROR6'

if ~1 and ~2 && ~3 and 4 || ~ 5 ne 0 then print,'***EXPR: ERROR7'

print,'EXPR: OK'
end

function fac,n

if n le 1 then return,1

return,fac(n-1)*n

end

pro recursion_test

if fac( 5) ne 120 then print,"***RECURSION: ERROR"

print,'RECURSION: OK'
end

pro index_test

b=byte( "abcdefg")
d=bytarr(20)
d[10]=b

if string( d[10:16]) ne "abcdefg" then  print,"***INDEX: ERROR1"

i=indgen(3,3)
s=[[-1],[-2]]

i[1,1]=s

if i[1,2] ne -2 then  print,"***INDEX: ERROR2"

c=intarr(3)
(((c[1])))=1
if c[1] ne 1 then print,'***INDEX: ERROR3'

print,'ARRAY INDEXING: OK'
end

pro operator_test

a=indgen(4)
b=indgen(4)

c=a#b

if c[2,3] ne 6 then print,"***OPERATOR: ERROR1"

b=indgen(4,2)

c=a#b

if c[0,1] ne 38 then print,"***OPERATOR: ERROR2"

a=indgen(2,4)
b=indgen(4)

c=a#b

if c[1] ne 34 then print,"***OPERATOR: ERROR3"

b=indgen(4,2)

c=a#b

if c[0,1] ne 76 then print,"***OPERATOR: ERROR4"

x=indgen(10) # transpose(intarr(10)+1)

if x[9,9] ne 9 then print,"***OPERATOR: ERROR5"

;; with scalar
r= 4 # indgen(1,4)
if r[0,2] ne 8 then print,"***OPERATOR: ERROR6"

r= indgen(4) # 4
if r[1] ne 4 then print,"***OPERATOR: ERROR7"


print,'OPERATORS: OK'
end

pro assignment_test

c = indgen( 4, 4)

t = intarr( 5, 5)
t[0,0] = c
if t[3,3] ne 15 then print,"***ASSIGNMENT: ERROR1"

t = intarr( 5, 5)
t[0,1] = c
if t[3,3] ne 11 then print,"***ASSIGNMENT: ERROR2"

t = intarr( 5, 5)
t[1,0] = c
if t[3,3] ne 14 then print,"***ASSIGNMENT: ERROR3"

t = intarr( 5, 5)
t[1,1] = c
if t[3,3] ne 10 then print,"***ASSIGNMENT: ERROR4"

t = intarr( 5, 5)
a=indgen(3,3)
t[1,1]=a
if t[2,2] ne 4 then print,"***ASSIGNMENT: ERROR5"

t = intarr( 5, 5)
a=indgen(3,3)
t[1]=a
if t[3,0] ne 2 then print,"***ASSIGNMENT: ERROR6"

a=(1?0:9)
b=0
(1?a:b) = 2
if a ne 2 then print,"***ASSIGNMENT: ERROR7"


print,"ASSIGNMENT: OK"
end

pro left_libfunction_test

u=indgen(4)

(reform(u,4,/OVERWRITE))++

if u[2] ne 3 then print,"***LEFT_LIBFUNCTION: ERROR1"

print,"LEFT_LIBFUNCTION: OK"
end

pro stride_index_test

a=lindgen( 11, 20, 31)

;print,total( a[1:*:3])

if total( a[1:10:4, 2:16:2, 5:29:5]) ne 408480. then print,"***STRIDE_INDEX: ERROR 1"
if total( a[*, 2:14:2, 20:29:5]) ne 776622. then print,"***STRIDE_INDEX: ERROR 2"
if total( a[1:6:2, *, 17:29:5]) ne 890550. then print,"***STRIDE_INDEX: ERROR 3"
if total( a[[7,1], 2:16:2, 5:29:5]) ne 272240. then print,"***STRIDE_INDEX: ERROR 4"
if total( a[3:9:2, [7,1], 2:*:4]) ne 228480. then print,"***STRIDE_INDEX: ERROR "
if total( a[*, 5:*:5, [7,1]]) ne 65670. then print,"***STRIDE_INDEX: ERROR 5"
if total( a[5:7, 2:16:2, 5:29:5]) ne 408600. then print,"***STRIDE_INDEX: ERROR 6"
if total( a[3:9:2, 5:7, 2:*:4]) ne 344832. then print,"***STRIDE_INDEX: ERROR 7"
if total( a[*, 5:*:5, 5:7]) ne 142065. then print,"***STRIDE_INDEX: ERROR 8"

b=lindgen(50)
if total( b[1:32:7]) ne 75. then print,"***STRIDE_INDEX: ERROR 9"
if total( b[1:*:3]) ne 425. then print,"***STRIDE_INDEX: ERROR 10"

print,"STRIDE_INDEX: OK"
end

;; set MEMCHECK to perform a memory leak check
;; as the heap (number of allocated cells) always grows so does
;; the memory consumption
;; with MEMCHECK set, no ptr and object allocations are made
pro test_suite,MEMCHECK=mCheck

s=systime(1)

switch_test
case_test
while_test
for_test
repeat_test
goto_test
ret_test
struct_test
multi_test
if not keyword_set( mCheck) then object_test
common_test
ref_test,MEMCHECK=mCheck
syntax_test
inc_test
if not keyword_set( mCheck) then inheritance_test
continuebreak_test
extra_test
expr_test
recursion_test
index_test
operator_test
assignment_test
left_libfunction_test
stride_index_test

print,"TEST_SUITE finished. Time: ",systime(1)-s
end


;; main program
print,'TEST_SUITE main program called.'
test_suite

end
