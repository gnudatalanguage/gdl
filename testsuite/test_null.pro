;
; AC 26 Nov. 2013, under GNU GPL v2 or any later
;
; Two kinds of tests:
;
; 1/ just verifying the logical operators (EQ and NE only) with !null are OK
; please remember that :
; - AND and OR not OK for !null
; - LT and GT not OK for !null
;
; 2/ when an array is created containing !null, !null is not taked
; into account 
;
; more exhaustive tests on July 2014. Ideas still welcome !!
;
; -----------------------------------------------
; this procedure adds running "nb_errors" into "total_errors"
; then reset "nb_errors" to 0 for next block of tests.
;
pro INCREMENT_ERRORS, total_errors, nb_errors, verbose=verbose
if KEYWORD_SET(verbose) then print, total_errors, nb_errors
total_errors=total_errors+nb_errors
nb_errors=0
end
;
; -----------------------------------------------
pro ADD_ERRORS, nb_errors, message
;
print, 'Error on operation : '+message
nb_errors=nb_errors+1
;
end
;
; -----------------------------------------------
;
pro TEST_NULL_LOGICAL, extern_errors, help=help, test=test, verbose=verbose
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_NULL_LOGICAL, extern_errors, help=help, test=test, verbose=verbose'
    return
endif
;
total_errors=0
nb_errors=0
;
if KEYWORD_SET(verbose) then verb=1 else verb=0
;
; first series of tests : comparing !Null with Undefined variable
;
txt='(1) Testing Undefined EQ/NE !null'
if (verb) then MESSAGE, /cont, txt
;
test1EQleft=(a EQ !null)
if (test1EQleft NE 1) then ADD_ERRORS, nb_errors, 'undefined EQ !null'
test1EQright=(!null EQ a)
if (test1EQright NE 1) then ADD_ERRORS, nb_errors, '!null EQ undefined'
test1NEleft=(a NE !null)
if (test1NEleft NE 0) then ADD_ERRORS, nb_errors, 'undefined NE !null'
test1NEright=(!null NE a)
if (test1NEright NE 0) then ADD_ERRORS, nb_errors, '!null NE undefined'
;
if (nb_errors GT 0) then MESSAGE, /cont, 'Errors in '+txt else $
if (verb) then MESSAGE, /cont, 'Success in '+txt
INCREMENT_ERRORS, total_errors, nb_errors, verbose=verbose
;
; second series of tests : comparing !Null with !Null
;
txt='(2) Testing !Null EQ/NE !null'
if (verb) then MESSAGE, /cont, txt
;
test2eq=(!null EQ !null)
if (test2eq NE 1) then ADD_ERRORS, nb_errors, '!null EQ !Null'
test2ne=(!null NE !null)
if (test2ne NE 0) then ADD_ERRORS, nb_errors, '!null NE !Null'
;
left=!null
test2eq=(left EQ !null)
if (test2eq NE 1) then ADD_ERRORS, nb_errors, 'Left EQ !Null'
test2ne=(left NE !null)
if (test2ne NE 0) then ADD_ERRORS, nb_errors, 'Left NE !Null'

;
right=!null
test2eq=(!null EQ right)
if (test2eq NE 1) then ADD_ERRORS, nb_errors, '!Null EQ right'
test2ne=(!null NE right)
if (test2ne NE 0) then ADD_ERRORS, nb_errors, '!Null NE right'
;
if (nb_errors GT 0) then MESSAGE, /cont, 'Errors in '+txt else $
if (verb) then MESSAGE, /cont, 'Success in '+txt
INCREMENT_ERRORS, total_errors, nb_errors, verbose=verbose
;
; third series of tests : comparing !Null with Defined variable
; (EQ/NE and Left/Right operations)
;
txt='(3) Testing Defined EQ/NE !null for '
if (verb) then MESSAGE, /cont, txt+' various TYPE'
;
for ii=1, 15 do begin
    if (ii EQ 8) OR (ii EQ 10) OR  (ii EQ 11) then CONTINUE
    b=MAKE_ARRAY(1, type=ii)
    b=b[0]
    if (verb) then MESSAGE, /cont, 'current type is : '+TYPENAME(b)
    ;;
    test3eqleft=(b EQ !null)
    if (test3eqleft NE 0) then ADD_ERRORS, nb_errors, 'x EQ !null'
    test3eqright=(!null EQ b)
    if (test3eqright NE 0) then ADD_ERRORS, nb_errors, '!null EQ x'
    test3neleft=(b NE !null)
    if (test3neleft NE 1) then ADD_ERRORS, nb_errors, 'x NE !null'
    test3neright=(!null NE b)
    if (test3neright NE 1) then ADD_ERRORS, nb_errors, '!null NE x'
    ;;
    if nb_errors gt 0 then MESSAGE, /cont, 'Errors in '+txt+'TYPE '+TYPENAME(b) else $
      if (verb) then MESSAGE, /cont, 'Success in '+txt+'TYPE '+TYPENAME(b)
    INCREMENT_ERRORS, total_errors, nb_errors;, verbose=verbose
endfor
;
; forth series of tests : comparing !Null with exotic types: Object and Pointer
;
txt='(4) Testing  EQ/NE !null for '
if (verb) then MESSAGE, /cont, txt+' Object and Pointers'
;
for ii=10, 11 do begin
    b=MAKE_ARRAY(1, type=ii)
    ;; when we do that, we create  <NullPointer> or <NullObj>
    b=b[0]
    if (verb) then MESSAGE, /cont, 'current type is : '+TYPENAME(b)
    ;;
    test4eqleft=(b EQ !null)
    if (test4eqleft NE 1) then ADD_ERRORS, nb_errors, 'x EQ !null'
    test4eqright=(!null EQ b)
    if (test4eqright NE 1) then ADD_ERRORS, nb_errors, '!null EQ x'
    test4neleft=(b NE !null)
    if (test4neleft NE 0) then ADD_ERRORS, nb_errors, 'x NE !null'
    test4neright=(!null NE b)
    if (test4neright NE 0) then ADD_ERRORS, nb_errors, '!null NE x'
    ;;
    if nb_errors gt 0 then MESSAGE, /cont, 'Errors in '+txt+'TYPE '+TYPENAME(b) else $
      if (verb) then MESSAGE, /cont, 'Success in '+txt+'TYPE '+TYPENAME(b)
    INCREMENT_ERRORS, total_errors, nb_errors;, verbose=verbose
endfor
;
; final message
;
if ~KEYWORD_SET(verbose) then short=1 else short=0
BANNER_FOR_TESTSUITE, 'TEST_NULL_LOGICAL', total_errors, short=short
;
if ISA(extern_errors) then extern_errors=total_errors+extern_errors else extern_errors=total_errors
;
if KEYWORD_SET(test) then STOP
;
end
;
; --------------------------------------------------
;
pro TEST_NULL_ARRAY, extern_errors, help=help, test=test, verbose=verbose
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_NULL_ARRAY, extern_errors, help=help, test=test, verbose=verbose'
    return
endif
;
total_errors=0
nb_errors=0
;
if KEYWORD_SET(verbose) then verb=1 else verb=0
;
; creating array containing !Null ...
;
txt='(1) creating array containing !Null'
if (verb) then MESSAGE, /cont, txt
;
expected1=[1L,2,4,2]
res0=SIZE([!null,1,!values.f_nan])
res1=SIZE([1,!null,!values.f_nan])
expected2=[2L,2,2,2,4]
res2=SIZE([[0,1],[0,!null,3]])
;
if ~ARRAY_EQUAL(expected1,res0) then ADD_ERRORS, nb_errors, '[!null,1,!values.f_nan]'
if ~ARRAY_EQUAL(expected1,res1) then ADD_ERRORS, nb_errors, '[1,!null,!values.f_nan]'
if ~ARRAY_EQUAL(expected2,res2) then ADD_ERRORS, nb_errors, '2d case'
;
if nb_errors gt 0 then MESSAGE, /cont, 'Errors in '+txt else $
  if (verb) then MESSAGE, /cont, 'Success in '+txt
INCREMENT_ERRORS, total_errors, nb_errors ;, verbose=verbose
;
; final message
;
if ~KEYWORD_SET(verbose) then short=1 else short=0
BANNER_FOR_TESTSUITE, 'TEST_NULL_ARRAY', total_errors, short=short
;
if ISA(extern_errors) then extern_errors=total_errors+extern_errors else extern_errors=total_errors
;
if KEYWORD_SET(test) then STOP
;
end
;
; -----------------------------------------------
;
pro TEST_NULL, help=help, test=test, verbose=verbose, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_NULL, help=help, test=test, verbose=verbose, no_exit=no_exit'
    return
endif
;
total_errors=0
;
TEST_NULL_LOGICAL, total_errors, help=help, test=test, verbose=verbose
;
TEST_NULL_ARRAY, total_errors, help=help, test=test, verbose=verbose
;
; final message
;
BANNER_FOR_TESTSUITE, 'TEST_NULL', total_errors, short=short
;
if (total_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
