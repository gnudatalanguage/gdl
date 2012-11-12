;; test some language elements of GDL

pro if_test

  if 1 then $
    if 0 then begin & end $
  else if 1 then $ 
    if 0 then begin & end $
  else a=1e

  if a ne 1e then begin
    message, '***IF: ERROR', /conti
    exit, status=1
  endif

  print, 'IF: OK'
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

    1: begin
      message, '***SWITCH: ERROR', /conti
      exit, status=1
    end
    complex(1): begin
      message, '***SWITCH: ERROR', /conti
      exit, status=1
    end
    'def': begin
      message, '***SWITCH: ERROR', /conti
      exit, status=1
    end
    'abc': begin
        abc = 1
    end
    1.0: fall = abc+1
    else: e=1
  endswitch

  if abc ne 1 then begin
    message, '***SWITCH: ERROR', /conti
    exit, status=1
  endif
  if fall ne abc+1 then begin
    message, '***SWITCH: ERROR', /conti
    exit, status=1
  endif
  if e ne 1 then begin
    message, '***SWITCH: ERROR', /conti
    exit, status=1
  endif

  switch 1 of
    0: begin
      message, "***SWITCH: ERROR", /conti
      exit, status=1
    end
    0: begin
      message, "***SWITCH: ERROR", /conti
      exit, status=1
    end
    1:       
  endswitch

  print,'SWITCH: OK'
end

pro case_test

  a='abc'

  case a of

    1: begin
      message, '***CASE: ERROR', /conti
      exit, status=1
    end
    complex(1): begin
      message, '***CASE: ERROR', /conti
      exit, status=1
    end
    'def': begin
      message, '***CASE: ERROR', /conti
      exit, status=1
    end
    'abc': begin
        c=1
    end
    1.0: begin
      message, '***CASE: ERROR', /conti
      exit, status=1
    end
    else: begin
      message, '***CASE: ERROR', /conti
      exit, status=1
    end
  endcase

  if c ne 1 then begin
    message, '***CASE: ERROR', /conti
    exit, status=1
  endif

  case a of

    1: begin
      message, '***CASE: ERROR', /conti
      exit, status=1
    end
    complex(1): begin
      message, '***CASE: ERROR', /conti
      exit, status=1
    end
    'def': begin
      message, '***CASE: ERROR', /conti
      exit, status=1
    end
    'a': begin
      message, '***CASE: ERROR', /conti
      exit, status=1
    end
    1.0d: begin
      message, '***CASE: ERROR', /conti
      exit, status=1
    end
    else: e=1
  end

  if e ne 1 then begin
    message, '***CASE: ERROR', /conti
    exit, status=1
  endif

  case 1 of
    0: begin
      message, "***CASE: ERROR", /conti
      exit, status=1
    end
    0: begin
      message, "***CASE: ERROR", /conti
      exit, status=1
    end
    1:       
  endcase

  print, 'CASE: OK'

end

pro while_test

  i=0
  while i lt 100 do begin
    i=i+1
  endwhile

  if i eq 100 then print,'WHILE: OK' else begin
    message, '***WHILE: ERROR', /conti
    exit, status=1
  endelse

end

pro for_test

  c=0
  for i=0,99 do c=c+1 

  if c ne 100 then begin
    message, '***FOR: ERROR', /conti
    exit, status=1
  endif

  c=0
  for i=0,990,10 do c=c+1 

  if c ne 100 then begin
    message, '***FOR: Step: ERROR', /conti
    exit, status=1
  endif

  a=0
  for i=0, 0 do begin
    for j=0, 1 do begin
      if i eq 0 then continue
    endfor
    a = 1
  endfor
  if a eq 0 then begin
    message, '***FOR: CONTINUE: ERROR', /conti
    exit, status=1
  endif

  print, 'FOR: OK'

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

  if i eq 100 then print,'REPEAT: OK' else begin
    message, '***REPEAT: ERROR', /conti
    exit, status=1
  endelse

end

pro goto_test

  ;; simple
  goto,jump11
  message, '***GOTO: Error', /conti
  exit, status=1
  jump21:
  goto,jump31
  message, '***GOTO: Error', /conti
  exit, status=1
  jump11:
  goto,jump21
  message, '***GOTO: Error', /conti
  exit, status=1
  jump31:


  count=0
  ;; within loop
  for i=0,10 do begin

  goto,jump1
  message, '***GOTO: Error1', /conti
  exit, status=1
  jump2:
  goto,jump3
  message, '***GOTO: Error2', /conti
  exit, status=1
  jump1:
  goto,jump2
  message, '***GOTO: Error3', /conti
  exit, status=1
  jump3:
  goto,jump4 ; even to jump out of the block and in again is ok
  jump5:

  if 0 then goto,jump5 else goto,jump7
  message, '***GOTO: Error4', /conti
  exit, status=1

  jump7:

  if 1 then begin

    goto,jump8
    message, '***GOTO: Error5', /conti
    exit, status=1
    jump8:

  endif else begin

    message, '***GOTO: Error6', /conti
    exit, status=1
  endelse


  count=count+1
  endfor

  goto,jump6
  jump4:
  goto,jump5
  jump6:
  
  if count ne 11 then begin
    message, '***GOTO: ERROR7', /conti
    exit, status=1
  endif

  IF (1)then begin
   
    IF (1)then begin
      ;print,"Going to Label1"
      goto, Label1
    end else begin
      message, '***GOTO: ERROR8', /conti
      exit, status=1
    end
      
    Label1:
  end

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

  if ret99() ne 99.0 then begin
    message, '***RET: ERROR1', /conti
    exit, status=1
  endif
  if retEq(u) ne 3 then begin
    message, '***RET: ERROR2', /conti
    exit, status=1
  endif
  (retEq(u2)) = 4
  if u2 ne 4 then begin
    message, '***RET: ERROR4', /conti
    exit, status=1
  endif


  (ret3(u3))=7
  if u3 ne 7 then begin
    message, '***RET: ERROR5', /conti
    exit, status=1
  endif

  s={t:0}
  if retTag(s) ne 3 then begin
    message, '***RET: ERROR6', /conti
    exit, status=1
  endif
  ;(ret(s))=2

  n = retOverwriteLocal()
  if n[2] ne "r" then begin
    message, '***RET: ERROR7', /conti
    exit, status=1
  endif

  a=[[1,2],[3,4]]
  (retOverwriteGlobal(a))--
  if a[1] ne 1 then begin
    message, '***RET: ERROR8', /conti
    exit, status=1
  endif

  n = retOverwriteExpression()
  if n[2] ne "r" then begin
    message, '***RET: ERROR9', /conti
    exit, status=1
  endif

  a=1
  c=ret(4*(3+reform(a,1,/OVERWRITE)+1)*2)
  if a ne 1 then begin
    message, '***RET: ERROR10', /conti
    exit, status=1
  endif
  if c ne 40 then begin
    message, '***RET: ERROR11', /conti
    exit, status=1
  endif

  common ret_common,aa

  aa=1
  c=ret(4*(3+reform(aa,1,/OVERWRITE)+1)*2)
  if aa ne 1 then begin
    message, '***RET: ERROR12', /conti
    exit, status=1
  endif
  if c ne 40 then begin
    message, '***RET: ERROR13', /conti
    exit, status=1
  endif

  print,'RET: OK';

end


pro struct_test

  s={s:"a string",tag: indgen(5),c:complex(1,2)}

  if s.tag[ 3] ne 3 then begin
    message, '***STRUCT: ERROR1', /conti
    exit, status=1
  endif

  s={s1,tag: indgen(5),s:s}

  if s.tag[ 3] ne 3 then begin
    message, '***STRUCT: ERROR1b', /conti
    exit, status=1
  endif

  s.tag++

  if s.tag[ 3] ne 4 then begin
    message, '***STRUCT: ERROR2', /conti
    exit, status=1
  endif

  s.tag = 9

  if s.tag[ 3] ne 9 then begin
    message, '***STRUCT: ERROR3', /conti
    exit, status=1
  endif

  s.tag = [10,11,12]

  if s.tag[ 2] ne 12 then begin
    message, '***STRUCT: ERROR4', /conti
    exit, status=1
  endif

  ss=[s,s]

  ss[*].tag=indgen(5,2)

  if ss[1].tag[2] ne 7 then begin
    message, '***STRUCT: ERROR5', /conti
    exit, status=1
  endif

  s={s:indgen(5),$
    t:"string",$
    u:complexarr(3,5),$
    v:dcomplexarr(99,2),$
    x:sindgen(100)$
  }
  ss={ss:s,s2:s}
  sss={sss:ss,ss:[s,s]}

  sa = s
  for i=0,2 do begin
    sa={s1:sa,s2:[sa,sa]}
  endfor
  sa = {named,t:s}
  for i=0,3 do begin
    sa={s1:sa,s2:[sa,sa]}
  endfor
  sa=0

  ;p=ptr_new( sss)
  ;((*p).sss.ss)[2] = 5
  ;print,((*p).ss)[2]

  sss.sss.ss.s[2] = 10
  if sss.sss.ss.s[2] ne 10 then begin
    message, '***STRUCT: ERROR6', /conti
    exit, status=1
  endif

  (ret(sss)).sss.ss.s[1] = 11
  if sss.sss.ss.s[1] ne 11 then begin
    message, '***STRUCT: ERROR7', /conti
    exit, status=1
  endif
  if (ret(sss)).sss.ss.s[3] ne 3 then begin
    message, '***STRUCT: ERROR8', /conti
    exit, status=1
  endif

  (ret(sss)).sss.(0).s[1] = 11
  if sss.(0).ss.s[1] ne 11 then begin
    message, '***STRUCT: ERROR9', /conti
    exit, status=1
  endif

  if ({a:3}).a ne 3 then begin
    print, '***STRUCT: ERROR10', /conti
    exit, status=1
  endif

  ;; array and struct
  s4=[sss,sss]
  s5=[[s4],[s4]]
  if s5[1,1].sss.ss.s[3] ne 3 then begin
    print, '***STRUCT: ERROR11', /conti
    exit, status=1
  endif

  a = {a0, var1: 0, var2: 0.0d0}
  b = {a0, '0L', ' 1.0'}

  print,'STRUCT: OK'

end

pro multi,a,b,c
  b=9
  if a ne 9 then begin
    message, '***MULTI: ERROR1', /conti
    exit, status=1
  endif
  if c ne 9 then begin
    message, '***MULTI: ERROR2', /conti
    exit, status=1
  endif
end

pro multi_test
  multi,a,a,a
  if a ne 9 then begin
    message, '***MULTI: ERROR3', /conti
    exit, status=1
  endif
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
  if self.a ne 1 then begin
    message, '***OBJECT: ERROR1', /conti
    exit, status=1
  endif
  self[0].a=2
  if self[0].a ne 2 then begin
    message, '***OBJECT: ERROR2', /conti
    exit, status=1
  endif
  (self).a[0]=3
  if (self[0]).a[[-2]] ne 3 then begin
    message, '***OBJECT: ERROR3', /conti
    exit, status=1
  endif
  if self[[0]].a ne 3 then begin
    message, '***OBJECT: ERROR4', /conti
    exit, status=1
  endif
  ;; would be legal in GDL (but IDL complains)
  ;if (self[[0]]).a ne 3 then begin
  ;  message, '***OBJECT: ERROR5', /conti
  ;  exit, status=1
  ;endif
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

  if obj->get() ne 9 then begin
    message, '***OBJECT: ERROR6', /conti
    exit, status=1
  endif

  obj->test
  (obj)->test
  obj[0]->test
  (obj[0])->test

  if not obj_valid( obj) then begin
    message, '***OBJECT: ERROR7', /conti
    exit, status=1
  endif

  obj_destroy,obj

  if obj_valid( obj) then begin
    message, '***OBJECT: ERROR8', /conti
    exit, status=1
  endif
  if o_cleanup ne -1 then begin
    message, '***OBJECT: ERROR9', /conti
    exit, status=1
  endif

  print,'OBJECT: OK'
end

pro ct1
  common tc,x,y,z
  if x ne 3 then begin
    message, '***COMMON: ERROR1', /conti
    exit, status=1
  endif
  y=7
end

pro ct2
  common tc
  if x ne 3 then begin
    message, '***COMMON: ERROR2', /conti
    exit, status=1
  endif
  if y ne 7 then begin
    message, '***COMMON: ERROR3', /conti
    exit, status=1
  endif
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

  if a ne 3 then begin
    message, '***COMMON: ERROR4', /conti
    exit, status=1
  endif
  if b ne 7 then begin
    message, '***COMMON: ERROR5', /conti
    exit, status=1
  endif
  if c ne 9 then begin
    message, '***COMMON: ERROR6', /conti
    exit, status=1
  endif
  if d ne 3 then begin
    message, '***COMMON: ERROR7', /conti
    exit, status=1
  endif
  if e ne 7 then begin
    message, '***COMMON: ERROR8', /conti
    exit, status=1
  endif
  if f ne 9 then begin
    message, '***COMMON: ERROR9', /conti
    exit, status=1
  endif
  if x ne 3 then begin
    message, '***COMMON: ERROR10', /conti
    exit, status=1
  endif
  if y ne 7 then begin
    message, '***COMMON: ERROR11', /conti
    exit, status=1
  endif
  if z ne 9 then begin
    message, '***COMMON: ERROR12', /conti
    exit, status=1
  endif

  (ct3()) = 2
  if ct3() ne 2 then begin
    message, '***COMMON: ERROR13', /conti
    exit, status=1
  endif

  print,'COMMON: OK'

end

pro set22,a,X=x
  a=2
  x=2
end

pro ref_test,MEMCHECK=mCheck

  set22,b
  if b ne 2 then begin
    message, "***REF: ERROR1", /conti
    exit, status=1
  endif

  if not keyword_set( mCheck) then begin

    p=ptr_new(/alloc)
    set22,*p
    if *p ne 2 then begin
      message, "***REF: ERROR2", /conti
      exit, status=1
    endif
    ptr_free,p

    p=ptr_new(/alloc)
    pp=ptr_new(p)
    set22,**pp
    if **pp ne 2 then begin
      message, "***REF: ERROR3", /conti
      exit, status=1
    endif
    ptr_free,p,pp

  endif

  a=indgen(3)
  set22,reform(a,1,3,/OVERWRITE)
  if a ne 2 then begin
    message, "***REF: ERROR4", /conti
    exit, statis=1
  endif

  a=4 & x=4
  set22,a++,X=x++
  if a ne 5 or x ne 5 then begin
    message, "***REF: ERROR5", /conti
    exit, status=1
  endif

  a=1 & x=1
  set22,++a,X=++x
  if a ne 2 or x ne 2 then begin
    message, "***REF: ERROR6", /conti
    exit, status=1
  endif

  a=1 & x=1
  set22,(a=4),X=(x=4)
  if a ne 2 or x ne 2 then begin
    message, "***REF: ERROR7", /conti
    exit, status=1
  endif

  a=4 & x=4
  set22,++(a=1),X=++(x=1)
  if a ne 2 or x ne 2 then begin
    message, "***REF: ERROR8", /conti
    exit, status=1
  endif

  set22,++(a[0]=4),X=++(x[0]=4)
  if a ne 5 or x ne 5 then begin
    message, "***REF: ERROR9", /conti
    exit, status=1
  endif

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
  if baseInit ne 1 then begin
    message, '***INHERITANCE: ERROR1', /conti
    exit, status=1
  endif
  if derivInit ne 1 then begin
    message, '***INHERITANCE: ERROR2', /conti
    exit, status=1
  endif

  o->setbase
  if o->get() ne 'base' then begin
    message, '***INHERITANCE: ERROR3', /conti
    exit, status=1
  endif
  o->set
  if o->get() ne 'deriv' then begin
    message, '***INHERITANCE: ERROR4', /conti
    exit, status=1
  endif
  o->base::set
  if o->get() ne 'base' then begin
    message, '***INHERITANCE: ERROR5', /conti
    exit, status=1
  endif

  obj_destroy, o
  if baseInit ne 1 then begin
    message, '***INHERITANCE: ERROR6', /conti
    exit, status=1
  endif
  if derivInit ne 1 then begin
    message, '***INHERITANCE: ERROR7', /conti
    exit, status=1
  endif

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
  if b ne 2 then begin
    message, '***SYNTAX_TEST: ERROR1', /conti
    exit, status=1
  endif

  a=(byte(1,0,1))[0]
  a=(syntax_test(1,0,1))[0]

  ;((a=4)) syntax error
  ;(a=2)=3  forbidden in GDL (as it hardly makes sense)

end

pro inc_test

  a=1

  (ret(a))++
  ++(ret(a))

  if a ne 3 then begin
    message, "***INC: ERROR1", /conti
    exit, status=1
  endif

  b = intarr(3)

  b[1]++
  ++b[1]
  (b[1])++
  ++(b[1])

  if b[1] ne 4 then begin
    message, "***INC: ERROR2", /conti
    exit, status=1
  endif

  b[[1,2]]+=1
  if b[1] ne 5 then begin
    message, "***INC: ERROR3", /conti
    exit, status=1
  endif

  (a=2)++
  if a ne 3 then begin
    message, '***INC_TEST: ERROR4', /conti
    exit, status=1
  endif

  ((a=2))++
  if a ne 3 then begin
    message, '***INC_TEST: ERROR5', /conti
    exit, status=1
  endif

  ++(a=2)
  if a ne 3 then begin
    message, '***INC_TEST: ERROR6', /conti
    exit, status=1
  endif

  ++((a=2))
  if a ne 3 then begin
    message, '***INC_TEST: ERROR7', /conti
    exit, status=1
  endif

  print,'INC: OK'

end


pro continuebreak_test

  repeat begin

    goto,start

    message, '***CONTINUE/BREAK: ERROR2', /conti
    exit, status=1

    start:

    for i=0,40,2 do begin

      if i lt 6 then continue

      if i lt 6 then begin
        message, '***CONTINUE/BREAK: ERROR1', /conti
        exit, status=1
      endif

      if i gt 10 then break

      if i gt 10 then begin
        message, '***CONTINUE/BREAK: ERROR2', /conti
        exit, status=1
      endif

      goto,in1
      message, '***CONTINUE/BREAK: ERROR2', /conti
      exit, status=1
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
  if a ne 'a' then begin
    message, 'EXTRA: ERROR1', /conti
    exit, status=1
  endif
  if b ne 'b' then begin
    message, 'EXTRA: ERROR2', /conti
    exit, status=1
  endif
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

  if r1 ne 2 then begin
    message, '***EXTRA: ERROR3', /conti
    exit, status=1
  endif
  if r2 ne 1 then begin
    message, '***EXTRA: ERROR4', /conti
    exit, status=1
  endif

  print,'EXTRA: OK'

end

pro expr_test

; BUG tracker ID: 3579499
str_template={value:1.0}
str_array=replicate(str_template,10)
str={data:str_array}
tmp=cos(str_template.value) 
tmp=cos(str_array(0).value) 
tmp=cos(str.data(0).value) 

  a=0
  a++
  a *= 2
  a ^= 2
  b = a+2*3-(9*2)+9
  if b ne 1 then begin
    message, '***EXPR: ERROR1', /conti
    exit, status=1
  endif

  a=indgen(6,6,6)
  a[3,3,3]=-1
  if a[3,3,3] ne -1 then begin
    message, '***EXPR: ERROR2', /conti
    exit, status=1
  endif

  a=a+100
  if a[3,3,3] ne 99 then begin
    message, '***EXPR: ERROR2a', /conti
    exit, status=1
  endif

  a[*]=a[*]-100
  if a[3,3,3] ne -1 then begin
    message, '***EXPR: ERROR2b', /conti
    exit, status=1
  endif

  sdef={s,a:intarr(3),s:strarr(3)}
  s1={s,indgen(3),s:sindgen(3)}
  if s1.a[1] ne 1 then begin
    message, '***EXPR: ERROR3', /conti
    exit, status=1
  endif
  s2={s}
  sArr=[s2,s2]
  sArr[1].s[1]='test'
  if sArr[1].s[1] ne 'test' then begin
    message, '***EXPR: ERROR4', /conti
    exit, status=1
  endif

  a=1
  c=4*(3+reform(a,1,/OVERWRITE)+1)*2
  if c ne 40 then begin
    message, '***EXPR: ERROR5', /conti
    exit, status=1
  endif
  if a ne 1 then begin
    message, '***EXPR: ERROR6', /conti
    exit, status=1
  endif

  if ~1 and ~2 && ~3 and 4 || ~ 5 ne 0 then begin
    message, '***EXPR: ERROR7', /conti
    exit, status=1
  endif

  print,'EXPR: OK'

end

function fac,n

  if n le 1 then return,1

  return,fac(n-1)*n

end

pro recursion_test

  if fac( 5) ne 120 then begin
    message, "***RECURSION: ERROR", /conti
    exit, status=1
  endif

  print,'RECURSION: OK'
end

pro index_test

  b=byte( "abcdefg")
  d=bytarr(20)
  d[10]=b

  if string( d[10:16]) ne "abcdefg" then begin
    message, "***INDEX: ERROR1", /conti
    exit, status=1
  endif

  i=indgen(3,3)
  s=[[-1],[-2]]

  i[1,1]=s

  if i[1,2] ne -2 then begin
    message, "***INDEX: ERROR2", /conti
    exit, status=1
  endif

  c=intarr(3)
  (((c[1])))=1
  if c[1] ne 1 then begin
    message, '***INDEX: ERROR3', /conti
    exit, status=1
  endif

  print,'ARRAY INDEXING: OK'

end

pro operator_test

  a=indgen(4)
  b=indgen(4)

  c=a#b

  if c[2,3] ne 6 then begin
    message, "***OPERATOR: ERROR1", /conti
    exit, status=1
  endif

  b=indgen(4,2)

  c=a#b

  if c[0,1] ne 38 then begin
    message, "***OPERATOR: ERROR2", /conti
    exit, status=1
  endif

  a=indgen(2,4)
  b=indgen(4)

  c=a#b

  if c[1] ne 34 then begin
    message, "***OPERATOR: ERROR3", /conti
    exit, status=1
  endif

  b=indgen(4,2)

  c=a#b

  if c[0,1] ne 76 then begin
    message, "***OPERATOR: ERROR4", /conti
    exit, status=1
  endif

  x=indgen(10) # transpose(intarr(10)+1)

  if x[9,9] ne 9 then begin
    message, "***OPERATOR: ERROR5", /conti
    exit, status=1
  endif

  ;; with scalar
  r= 4 # indgen(1,4)
  if r[0,2] ne 8 then begin
    message, "***OPERATOR: ERROR6", /conti
    exit, status=1
  endif

  r= indgen(4) # 4
  if r[1] ne 4 then begin
    message, "***OPERATOR: ERROR7", /conti
    exit, status=1
  endif

  print,'OPERATORS: OK'

end

pro assignment_test

  c = indgen( 4, 4)

  t = intarr( 5, 5)
  t[0,0] = c
  if t[3,3] ne 15 then begin
    message, "***ASSIGNMENT: ERROR1", /conti
    exit, status=1
  endif

  t = intarr( 5, 5)
  t[0,1] = c
  if t[3,3] ne 11 then begin
    message, "***ASSIGNMENT: ERROR2", /conti
    exit, status=1
  endif

  t = intarr( 5, 5)
  t[1,0] = c
  if t[3,3] ne 14 then begin
    message, "***ASSIGNMENT: ERROR3", /conti
    exit, status=1
  endif

  t = intarr( 5, 5)
  t[1,1] = c
  if t[3,3] ne 10 then begin
    message, "***ASSIGNMENT: ERROR4", /conti
    exit, status=1
  endif

  t = intarr( 5, 5)
  a=indgen(3,3)
  t[1,1]=a
  if t[2,2] ne 4 then begin
    message, "***ASSIGNMENT: ERROR5", /conti
    exit, status=1
  endif

  t = intarr( 5, 5)
  a=indgen(3,3)
  t[1]=a
  if t[3,0] ne 2 then begin
    message, "***ASSIGNMENT: ERROR6", /conti
    exit, status=1
  endif
 
  a=(1?0:9)
  b=0
  (1?a:b) = 2
  if a ne 2 then begin
    message, "***ASSIGNMENT: ERROR7", /conti
    exit, status=1
  endif


  ((a=3))=2
  if a ne 2 then begin
    message, "***ASSIGNMENT: ERROR8", /conti
    exit, status=1
  endif

  A=create_struct("boo",dblarr(10),"foo",dblarr(10))
  B=replicate(A,30)
  data=findgen(30)
  B[0:29].boo[3] = data[0:29]
  if B[29].boo[3] ne 29.0 then begin
    message, "***ASSIGNMENT: ERROR9", /conti
    exit, status=1
  endif

  print,"ASSIGNMENT: OK"

end

pro left_libfunction_test

  u=indgen(4)

  (reform(u,4,/OVERWRITE))++

  if u[2] ne 3 then begin
    message, "***LEFT_LIBFUNCTION: ERROR1", /conti
    exit, status=1
  endif

  print,"LEFT_LIBFUNCTION: OK"
end

pro stride_index_test

  a=lindgen( 11, 20, 31)

  ;print,total( a[1:*:3])

  if total( a[1:10:4, 2:16:2, 5:29:5]) ne 408480. then begin
    message, "***STRIDE_INDEX: ERROR 1", /conti
    exit, status=1
  endif
  if total( a[*, 2:14:2, 20:29:5]) ne 776622. then begin
    message, "***STRIDE_INDEX: ERROR 2", /conti
    exit, status=1
  endif
  if total( a[1:6:2, *, 17:29:5]) ne 890550. then begin
    message, "***STRIDE_INDEX: ERROR 3", /conti
    exit, status=1
  endif
  if total( a[[7,1], 2:16:2, 5:29:5]) ne 272240. then begin
    message, "***STRIDE_INDEX: ERROR 4", /conti
    exit, status=1
  endif
  if total( a[3:9:2, [7,1], 2:*:4]) ne 228480. then begin
    message, "***STRIDE_INDEX: ERROR ", /conti
    exit, status=1
  endif
  if total( a[*, 5:*:5, [7,1]]) ne 65670. then begin
    message, "***STRIDE_INDEX: ERROR 5", /conti
    exit, status=1
  endif
  if total( a[5:7, 2:16:2, 5:29:5]) ne 408600. then begin
    message, "***STRIDE_INDEX: ERROR 6", /conti
    exit, status=1
  endif
  if total( a[3:9:2, 5:7, 2:*:4]) ne 344832. then begin
    message, "***STRIDE_INDEX: ERROR 7", /conti
    exit, status=1
  endif
  if total( a[*, 5:*:5, 5:7]) ne 142065. then begin
    message, "***STRIDE_INDEX: ERROR 8", /conti
    exit, status=1
  endif

  b=lindgen(50)
  if total( b[1:32:7]) ne 75. then begin
    message, "***STRIDE_INDEX: ERROR 9", /conti
    exit, status=1
  endif
  if total( b[1:*:3]) ne 425. then begin
    message, "***STRIDE_INDEX: ERROR 10", /conti
    exit, status=1
  endif

  print,"STRIDE_INDEX: OK"

end

;  'TEST_SUITE main program called.'
;
;; set MEMCHECK to perform a memory leak check
;; as the heap (number of allocated cells) always grows so does
;; the memory consumption
;; with MEMCHECK set, no ptr and object allocations are made

pro TEST_SUITE, MEMCHECK=mCheck

  s=systime(1)

  if_test
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
