; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
; part of GDL - GNU Data Language
;
; this is also important feature since used in recent versions
; of READCOL in the widely used AtroLib
; see also bug report 3573558
;
; this is not a clean version of this test ...
;
function TEST_SCOPE_CARFETCH_SUB
  return, SCOPE_VARFETCH('a', level=-1)
end
;
; this code is used in recent versions of READCOL in AstroLib
;
pro ASTRON_READCOL1, a,b,c
MESSAGE,/continue, 'inside'
list_of_name=['a','b','c']
(SCOPE_VARFETCH(list_of_name[0], LEVEL=0)) = FINDGEN(10)
end

pro ASTRON_READCOL1BIS, a,b,c
MESSAGE,/continue, 'inside'
list_of_name=['a','b','c']
for ii=0, 2 do begin
   (SCOPE_VARFETCH(list_of_name[ii], LEVEL=0)) = FINDGEN(10)
endfor
end

pro ASTRON_READCOL2, a,b,c
MESSAGE,/continue, 'inside'
list_of_name=['a','b','c']
(SCOPE_VARFETCH('a', LEVEL=0)) = FINDGEN(10)
end
;
; ---------------------------------
;
pro TEST_SCOPE_VARFETCH_NO_KEYWORD
;
a = 1234
print, a
if a ne test_scope_carfetch_sub() then exit, status=1
;
debug=1
if KEYWORD_SET(debug) then mydata1=0
ASTRON_READCOL1, mydata1,b1,c1
print, mydata1
;
if KEYWORD_SET(debug) then mydata1bis=0
ASTRON_READCOL1BIS, mydata1bis,b1bis,c1bis
print, mydata1bis

if KEYWORD_SET(debug) then mydata2=0
ASTRON_READCOL2, mydata2,b2,c2
print, mydata2
;
if KEYWORD_SET(test) then STOP
;
end
;
; ---------------------------------
;
pro TEST_SCOPE_VARFETCH_WITH_KEY, help=help, debug=deug, test=test
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_SCOPE_VARFETCH_WITH_KEY, help=help, debug=deug, test=test'
   return
endif
;
a = 1234
print, a
if a ne TEST_SCOPE_CARFETCH_SUB() then exit, status=1
;
if KEYWORD_SET(debug) then mydata1=0
ASTRON_READCOL1, mydata1,b1,c1
print, mydata1
;
if KEYWORD_SET(debug) then mydata1bis=0
ASTRON_READCOL1BIS, mydata1bis,b1bis,c1bis
print, mydata1bis

if KEYWORD_SET(debug) then mydata2=0
ASTRON_READCOL2, mydata2,b2,c2
print, mydata2
;
if KEYWORD_SET(test) then STOP
;
end

pro  TEST_SCOPE_VARFETCH_REF_EXTRA_SUB,_REF_EXTRA=ex
 Message,/inf,"testing SCOPE_VARFETCH (/REF_EXTRA) ..."
  (scope_varfetch(ex[0],/REF_EXTRA))='zzz'
  (scope_varfetch(ex[1],/REF_EXTRA))=66
end

pro TEST_SCOPE_VARFETCH_REF_EXTRA
  a=33
  TEST_SCOPE_VARFETCH_REF_EXTRA_SUB,a=a,b=b
  if a ne 'zzz' then Message,/inf,"Error 1 in TEST_SCOPE_VARFETCH_REF_EXTRA"
  if n_elements(b) eq 0 then Message,/inf,"Error 2 in TEST_SCOPE_VARFETCH_REF_EXTRA"
  if b ne 66 then Message,/inf,"Error 3 in TEST_SCOPE_VARFETCH_REF_EXTRA"
end

pro TEST_SCOPE_VARFETCH_COMMON_SUB_VALUE
; 0: a (=33) 1:b (undef)

 Message,/inf,"testing SCOPE_VARFETCH (COMMON) by value ..."
  u=scope_varfetch(0,common='test_scope_varfetch_common')
  if u ne 33 then  Message,"Error 1 in TEST_SCOPE_VARFETCH_COMMON"

  return
  
  n_err=1
  catch,error
  if error ne 0 then begin
     n_err--;
     catch,/cancel
  endif else u=scope_varfetch(1,common='test_scope_varfetch_common') ; should be catched as 'b' has no value so 'u' cannot be created.

  if n_err ne 0 then Message,/inf,"Error 1 in TEST_SCOPE_VARFETCH_COMMON_SUB_VALUE"
  
  n_err=1
  catch,error
  if error ne 0 then begin
     n_err--;
     catch,/cancel
  endif else   u=scope_varfetch(10,common='test_scope_varfetch_common') ; should be catched as common has only 3 entries.

  if n_err ne 0 then Message,/inf,"Error 2 in TEST_SCOPE_VARFETCH_COMMON_SUB_VALUE"


end

pro TEST_SCOPE_VARFETCH_COMMON_SUB_REFERENCE
; 0: a (=33) 1:b (undef)
 Message,/inf,"testing SCOPE_VARFETCH (COMMON) by reference ..."
  (scope_varfetch('a',common='test_scope_varfetch_common')) = 66 ; set A of Common to 66 --- checked by caller routine.
  n_err=1
  catch,error
  if error ne 0 then begin
     n_err--;
     catch,/cancel
  endif else (scope_varfetch('z',common='test_scope_varfetch_common')) = 66 ; should be catched: 'z' does not exist

  if n_err ne 0 then Message,/inf,"Error 1 in TEST_SCOPE_VARFETCH_COMMON_SUB_REFERENCE"
  
  (scope_varfetch('b',common='test_scope_varfetch_common')) = 'zzz' ; set B of Common to 'zzz' --- checked by caller routine. 

end


pro TEST_SCOPE_VARFETCH_COMMON
  common TEST_SCOPE_VARFETCH_COMMON,a,b,c
  a=33
  TEST_SCOPE_VARFETCH_COMMON_SUB_VALUE
  TEST_SCOPE_VARFETCH_COMMON_SUB_REFERENCE
  if a ne 66 then Message,/inf,"Error 1 in TEST_SCOPE_VARFETCH_COMMON"
  if n_elements(b) eq 0 then Message,/inf,"Error 2 in TEST_SCOPE_VARFETCH_COMMON"
  if b ne 'zzz' then Message,/inf,"Error 3 in TEST_SCOPE_VARFETCH_COMMON"
  
end

pro TEST_SCOPE_VARFETCH, help=help, debug=deug, test=test
TEST_SCOPE_VARFETCH_WITH_KEY, help=help, debug=deug, test=test
TEST_SCOPE_VARFETCH_NO_KEYWORD
TEST_SCOPE_VARFETCH_REF_EXTRA
TEST_SCOPE_VARFETCH_COMMON
end
