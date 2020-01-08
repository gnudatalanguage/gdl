pro test_struct_assign
  err=0
  a={toto, deux:[0,4], trois:[0.66,68.33,222.16], quatre:'zzzzz'}
  b=CREATE_STRUCT(NAME='HasStructInside', ['un','deux','trois'], [1,5], 2b, $
                  a)
  d=b
  c=CREATE_STRUCT(NAME='Other',['un','deux'],3,4)
  struct_assign,c,d
  ; has been changed:
  if total(d.un) ne 3 then err++
  ; d must be zeroed in d.trois:
  if total(d.trois.deux) ne 0 then err++
  if strlen(d.trois.quatre) ne 0 then err++
  ; with noerase, things are different. 
  struct_assign,c,b,/nozero
  if total(b.trois.deux) ne 4 then err++ ; show also that it is "relaxed"
  ; 
  c=CREATE_STRUCT(NAME='HasStruct', ['un','deux','trois'], [1,5], 2b, findgen(32))
  struct_assign,c,b,/nozero,/verb
;  print,err
    if err ne 0 then begin
      message, 'test FAILED with '+strtrim(err,2)+' errors', /conti
      exit, status=1
    endif
end
