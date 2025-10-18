;
; various basics tests on ISA()
; written by Levan Loria, June 2014
; under GNU GPL v2 or any later
;
; 3 cases are not ready : Obj. (+ LIST and HASH), Struct., Pointers
;
; Some tests may appear very artificial but good test coverage cannot
; avoid basic trivial tests !
;
; ---------------------------------------
; Modifications history :
;
; - 2024-JAN-11 : AC. Limited cleanup. Better error messages ...
; motivation : errors for HASH & LIST discovered ...
; removing TYPE_GEN (provided by GIVE_LIST_NUMERIC)
; removing INCREMENT_ERRORS (provided by ERRORS_RESET)
;
; -----------------------------------------------
;
pro TEST_ISA_FOR_LIST, cumul_errors, verbose=verbose, test=test
;
if KEYWORD_SET(verbose) then verb=1 else verb=0
name=ROUTINE_NAME()
;
nb_errors=0
accumul_errors=0
;
txt=' testing ISA for LIST'
if (verb) then MESSAGE, /continue, 'Starting '+txt
;
; --------------------------------------
; First basic tests
;
data = LIST()
if N_Elements(data) ne 0 then ERRORS_ADD, nb_errors, 'void LIST() N_Elements is not 0'
if ISA(data) eq 0 then ERRORS_ADD, nb_errors, 'first 1 LIST() ISA pretends not LIST'
if ISA(data,'LIST') eq 0 then ERRORS_ADD, nb_errors, '1 LIST() ISA LIST'
if OBJ_ISA(data,'LIST') eq 0 then ERRORS_ADD, nb_errors, '1 LIST() OBJ_ISA LIST'
if ISA(data,'OBJREF') eq 0 then ERRORS_ADD, nb_errors, '1 LIST() is is not OBJREF'
if ISA(data,/NULL) eq 1 then ERRORS_ADD, nb_errors, '1 LIST() is is not NULL'
if ISA(data,"UNDEFINED") eq 1 then ERRORS_ADD, nb_errors, '1 LIST() is defined ...'
if ISA(data,/ARRAY) eq 1 then ERRORS_ADD, nb_errors, '1 LIST() is NOT an array'
if ISA(data,/SCALAR) eq 0 then ERRORS_ADD, nb_errors, '1 LIST() is a Scalar'
;
if ISA(data,/Boolean) eq 1 then ERRORS_ADD, nb_errors, '1 LIST() is NOT a Boolean'
if ISA(data,/Number) eq 1 then ERRORS_ADD, nb_errors, '1 LIST() is NOT a Number'
if ISA(data,/String) eq 1 then ERRORS_ADD, nb_errors, '1 LIST() is NOT a String'
;
mess='First part : Void LIST() test done'
if (verb) or (nb_errors GT 0) then BANNER_FOR_TESTSUITE, mess, nb_errors, /short, pref=name
ERRORS_RESET, accumul_errors, nb_errors
;
; --------------------------------------
; Second : test on LIST rank (array or scalar ...)
;
data = LIST(1,2,3)
if ISA(data,/ARRAY) eq 0 then ERRORS_ADD, nb_errors, '2 LIST here is an array'
if ISA(data,/SCALAR) eq 1 then ERRORS_ADD, nb_errors, '2 LIST here is not a scalar'
if ISA(data,/NULL) eq 1 then ERRORS_ADD, nb_errors, '2 LIST here not NULL'
if ISA(data,/Number) eq 1 then ERRORS_ADD, nb_errors, '2 LIST here is not a number'
;
mess='Second part : LIST(1,2,3) test done'
if (verb) or (nb_errors GT 0) then BANNER_FOR_TESTSUITE, mess, nb_errors, /short, pref=name
ERRORS_RESET, accumul_errors, nb_errors
;
; --------------------------------------
; Third : test on LIST rank (array or scalar ...) + type
;
type_info='list'
data = LIST(1,2,3)
if ISA(data,type_info,/ARRAY) eq 0 then ERRORS_ADD, nb_errors, '3 LIST & Array'
if ISA(data,type_info,/SCALAR) eq 1 then ERRORS_ADD, nb_errors, '3 LIST not Scalar'
if ISA(data,type_info,/NULL) eq 1 then ERRORS_ADD, nb_errors, '3 LIST not Null'
if ISA(data,type_info,/Number) eq 1 then ERRORS_ADD, nb_errors, '3 LIST not number'
;
mess='Third part : LIST(1,2,3) test done (with type LIST)'
if (verb) or (nb_errors GT 0) then BANNER_FOR_TESTSUITE, mess, nb_errors, /short, pref=name
ERRORS_RESET, accumul_errors, nb_errors
;
; --------------------------------------
; Fourth : test bad type
;
type_info='objref'
data = LIST(1,2,3)
if ISA(data,type_info,/ARRAY) eq 0 then ERRORS_ADD, nb_errors, '4 LIST not Objref (array)'
if ISA(data,type_info,/SCALAR) eq 1 then ERRORS_ADD, nb_errors, '4 LIST not Objref (scalar)'
if ISA(data,type_info,/NULL) eq 1 then ERRORS_ADD, nb_errors, '4 LIST not Objref (null)'
if ISA(data,type_info,/Number) eq 1 then ERRORS_ADD, nb_errors, '4 LIST not Objref (number)'
;
mess='Fourth part : LIST(1,2,3) test done (with type ObjRef)'
if (verb) or (nb_errors GT 0) then BANNER_FOR_TESTSUITE, mess, nb_errors, /short, pref=name
ERRORS_RESET, accumul_errors, nb_errors
;
; --------------------------------------
; Last : strange type
;
type_info='no_a_type'
data = LIST(1,2,3)
if ISA(data,type_info,/ARRAY) eq 1 then ERRORS_ADD, nb_errors, '5 LIST n-a-t Array'
if ISA(data,type_info,/SCALAR) eq 1 then ERRORS_ADD, nb_errors, '5 LIST n-a-t Scalar'
if ISA(data,type_info,/NULL) eq 1 then ERRORS_ADD, nb_errors, '5 LIST n-a-t Null'
if ISA(data,type_info,/Number) eq 1 then ERRORS_ADD, nb_errors, '5 LIST n-a-t Number'
;
mess='Last part : LIST(1,2,3) test done (with type other) '
if (verb) or (nb_errors GT 0) then BANNER_FOR_TESTSUITE, mess, nb_errors, /short, pref=name
ERRORS_RESET, accumul_errors, nb_errors
;
; -------------- final message ------------
;
BANNER_FOR_TESTSUITE, ROUTINE_NAME(), accumul_errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, accumul_errors
if KEYWORD_SET(test) then STOP
;
end
;
; -----------------------------------------------
;
pro TEST_ISA_FOR_HASH, cumul_errors, verbose=verbose, test=test
;
if KEYWORD_SET(verbose) then verb=1 else verb=0
name=ROUTINE_NAME()
;
nb_errors=0
accumul_errors=0
;
txt=' testing ISA for HASH'
if (verb) then MESSAGE, /continue, 'Starting '+txt
;
data=HASH()
;
if N_Elements(data) ne 0 then ERRORS_ADD, nb_errors, 'void HASH() N_Elements is not 0'
if ISA(data) eq 0 then ERRORS_ADD, nb_errors, '1 HASH() does exist'
if ISA(data,'HASH') eq 0 then ERRORS_ADD, nb_errors, '1 HASH() ISA hash'
if OBJ_ISA(data,'HASH') eq 0 then ERRORS_ADD, nb_errors, '1 HASH() OBJ_ISA hash'
if ISA(data,'OBJREF') eq 0 then ERRORS_ADD, nb_errors, '1 HASH() is an object'
if ISA(data,/NULL) eq 1 then ERRORS_ADD, nb_errors, '1 HASH() is not null'
if ISA(data,"UNDEFINED") eq 1 then ERRORS_ADD, nb_errors, '1 HASH() in defined'
if ISA(data,/scalar) eq 0 then ERRORS_ADD, nb_errors, 'here 1 HASH() is a scalar '
if ISA(data,/array) eq 1 then ERRORS_ADD, nb_errors, 'here 1 HASH() is not an array '
;
mess='First part : void HASH() test done'
if (verb) or (nb_errors GT 0) then BANNER_FOR_TESTSUITE, mess, nb_errors, /short, pref=name
ERRORS_RESET, accumul_errors, nb_errors
;
; --------------------------------------
; Second : test on HASH rank (array or scalar ...)
;
data = HASH([5])
if ISA(data,/ARRAY) eq 0 then ERRORS_ADD, nb_errors, '2 HASH here is an array'
if ISA(data,/SCALAR) eq 1 then ERRORS_ADD, nb_errors, '2 HASH here is not a scalar'
if ISA(data,/NULL) eq 1 then ERRORS_ADD, nb_errors, '2 HASH here not NULL'
if ISA(data,/Number) eq 1 then ERRORS_ADD, nb_errors, '2 HASH here is not a number'
;
mess='Second part : HASH([5]) test done'
if (verb) or (nb_errors GT 0) then BANNER_FOR_TESTSUITE, mess, nb_errors, /short, pref=name
ERRORS_RESET, accumul_errors, nb_errors
;
; -------------- final message ------------
;
BANNER_FOR_TESTSUITE, ROUTINE_NAME(), accumul_errors, /short, verb=verb
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_SET(test) then STOP
;
end
;
; -----------------------------------------------
;
pro TEST_ISA_FOR_POINTER, cumul_errors, verbose=verbose, test=test
;
if KEYWORD_SET(verbose) then verb=1 else verb=0
;
nb_errors=0
;
txt=' testing ISA for POINTER'
if (verb) then MESSAGE, /continue, 'Starting '+txt
;
;testing NULL pointer
ptr = PTR_NEW()
;
if ISA(ptr) eq 1 then ERRORS_ADD, nb_errors, 'ptr exists !'
if ISA(ptr,/NULL) eq 1 then ERRORS_ADD, nb_errors, 'prt not Null'
if ISA(ptr,'POINTER') eq 0 then ERRORS_ADD, nb_errors, 'prt is a pointer !'
if ISA(ptr,'STRING') eq 1 then ERRORS_ADD, nb_errors, 'prt is not a string'
;
;testing not a NULL pointer
;
ptr = PTR_NEW(1)
;
if ISA(ptr) eq 0 then ERRORS_ADD, nb_errors, 'Prt is not null'
if ISA(*ptr) eq 0 then ERRORS_ADD, nb_errors, '*Ptr is not null'
;
; -------------- final message ------------
;
BANNER_FOR_TESTSUITE, ROUTINE_NAME(), nb_errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_SET(test) then STOP
;
end
;
; -----------------------------------------------
;
pro TEST_ISA_FOR_OBJECT, cumul_errors, verbose=verbose, test=test
;
if KEYWORD_SET(verbose) then verb=1 else verb=0
;
nb_errors=0
;
txt=' testing ISA for OBJECT'
if (verb) then MESSAGE, /continue, 'Starting '+txt
;
obj = OBJ_NEW()
; test in passing the case #2090
if N_elements(obj) ne 1 then ERRORS_ADD, nb_errors, 'NullObject n_elements() is not 1'
if ISA(obj) eq 1 then ERRORS_ADD, nb_errors, 'Obj does exist'
if ISA(obj,'OBJECT') eq 1 then ERRORS_ADD, nb_errors, 'Obj is not an Object'
if ISA(obj,'OBJREF') eq 0 then ERRORS_ADD, nb_errors, 'Obj is not an Obref'
if ISA(obj,/NULL) eq 1 then ERRORS_ADD, nb_errors, 'as defined, Obj is not null'
;
obj = OBJ_NEW('LIST')
if ISA(obj) eq 0 then ERRORS_ADD, nb_errors, 'Obj defined'
if ISA(obj,'OBJREF') eq 0 then ERRORS_ADD, nb_errors, 'Obj is a Objref'
if ISA(obj,'LIST') eq 0 then ERRORS_ADD, nb_errors, 'this Obj is a list'
;
; -------------- final message ------------
;
BANNER_FOR_TESTSUITE, ROUTINE_NAME(), nb_errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_SET(test) then STOP
;
end
;
; -----------------------------------------------
;
pro TEST_ISA_FOR_STRUCT, cumul_errors, verbose=verbose, test=test
;
if KEYWORD_SET(verbose) then verb=1 else verb=0
;
nb_errors=0
;
txt=' testing ISA for Structures'
if (verb) then MESSAGE, /continue, 'Starting '+txt
;
str = {MYSTRUCT, field1: 'f1'}
if ISA(str) eq 0 then ERRORS_ADD, nb_errors, 'This Struct does exist !'
if ISA(str,'STRUCT') eq 0 then ERRORS_ADD, nb_errors, 'It is a Struct !'
if ISA(str,'MYSTRUCT') eq 0 then ERRORS_ADD, nb_errors, 'It is a Struct named MYSTRUCT !'
;
; -------------- final message ------------
;
BANNER_FOR_TESTSUITE, ROUTINE_NAME(), nb_errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_SET(test) then STOP
;
end
;
; -----------------------------------------------
;
pro TEST_ISA_FOR_NUMBERS, cumul_errors, array=array, verbose=verbose, test=test
;
if KEYWORD_SET(array) then dim=1 else dim=0
if KEYWORD_SET(array) then mess=' (array)' else mess=' (scalar)'
;
if KEYWORD_SET(verbose) then verb=1 else verb=0
;
if (verb) then begin
    txt='Starting testing for Numbers types'
    MESSAGE, /continue, txt+mess
endif
;
;for numerical types, expect "PTR","OBJ","STR","STRING"
;list_type = ["BYTE","INT","LONG","FLOAT","DOUBLE","COMPLEX",$
;             "DCOMPLEX","UINT","ULONG","LONG64","ULONG64"]
;list_keywords = ["/NULL","/ARRAY","/SCALAR","/NUMBER"]
;
GIVE_LIST_NUMERIC, list_type, list_type_name
n = N_ELEMENTS(list_type)
;
; this is for internal errors only
nb_errors=0
;
for i=0,n-1 do begin
    ;;
   ;;tmp_var = TYPE_GEN(list_type[i])
    tmp_var = FIX(1, type=list_type[i])
    ;;
    if (dim EQ 1) then tmp_var =REPLICATE(tmp_var, 4)
    ;;
    if (verb) then HELP, tmp_var
    ;;
    if ISA(tmp_var) eq 0 then  ERRORS_ADD, nb_errors, 'it exists !'
    if ISA(tmp_var,list_type_name[i]) eq 0 then  ERRORS_ADD, nb_errors, 'sefl type test'
    if ISA(tmp_var,"OTHER") eq 1 then  ERRORS_ADD, nb_errors, 'other type ...'
    ;;
    for j=1,n-2 do begin
        ind = i+j
        if ind lt n then ind=ind else  ind = ind - n
        if ISA(tmp_var,list_type_name[ind]) eq 1 then  ERRORS_ADD, nb_errors, 'diff type'
        if ISA(tmp_var,list_type_name[ind],/SCALAR) eq 1 then  ERRORS_ADD, nb_errors, 'diff type + /scale'
        if ISA(tmp_var,list_type_name[ind],/ARRAY) eq 1 then  ERRORS_ADD, nb_errors, 'diff type + /array'
        if ISA(tmp_var,list_type_name[ind],/NUMBER) eq 1 then  ERRORS_ADD, nb_errors, 'diff type + /number'
    endfor	
    ;;
    ;;if ISA gives 1 for different types, 
    ;; ( for example: a = 2   &&   ISA(a,"STRING")-->1)
    ;;testing string
    ;;
    if ISA(str_var,list_type_name[i]) eq 1 then ERRORS_ADD, nb_errors, ''
    for j=i+1,n-1 do begin
        if ISA(tmp_var,list_type_name[j]) eq 1 then ERRORS_ADD, nb_errors, 'not the same type'
        if ISA(tmp_var,list_type_name[j],/SCALAR) eq 1 then ERRORS_ADD, nb_errors, 'not a scalar'
        if ISA(tmp_var,list_type_name[j],/ARRAY) eq 1 then ERRORS_ADD, nb_errors, 'not an array'
        if ISA(tmp_var,list_type_name[j],/NUMBER) eq 1 then ERRORS_ADD, nb_errors, 'is a number'
    endfor
    ;;
    ;;      testing keywords
    ;;NULL
    if ISA(tmp_var,/NULL) eq 1 then  ERRORS_ADD, nb_errors, 'not NULL'
    ;;NUMBER
    if ISA(tmp_var,/NUMBER) eq 0 then  ERRORS_ADD, nb_errors, 'it is a Number !'
    ;;
    ;; cross test Scalar or Array
    if (dim EQ 1) then begin
        if ISA(tmp_var,/ARRAY) eq 0 then  ERRORS_ADD, nb_errors, 'it is an array'
        if ISA(tmp_var,/SCALAR) eq 1 then  ERRORS_ADD, nb_errors, 'it is not a scalar'
    endif else begin
        if ISA(tmp_var,/ARRAY) eq 1 then  ERRORS_ADD, nb_errors, 'it is not an array'
        if ISA(tmp_var,/SCALAR) eq 0 then  ERRORS_ADD, nb_errors, 'it is a scalar'
    endelse
    ;;
    if (nb_errors GT 0) then begin
        MESSAGE, /continue, 'Errors found within type : '+list_type_name[i]
    endif
    ERRORS_RESET, accumul_errors, nb_errors
endfor
;
; -------------- final message ------------
;
BANNER_FOR_TESTSUITE, ROUTINE_NAME()+mess, accumul_errors, /short, pref=ROUTINE_NAME()
ERRORS_CUMUL, cumul_errors, accumul_errors
if KEYWORD_SET(test) then STOP
;
end
;
; -----------------------------------------------
;
pro TEST_ISA_FOR_STRING, cumul_errors, verbose=verbose, test=test
;
if KEYWORD_SET(verbose) then verb=1 else verb=0
;
nb_errors=0
;
txt=' testing ISA for STRING'
if (verb) then MESSAGE, /continue, 'Starting '+txt
;
; testing string
str_var = STRING(1)
if ISA(str_var) eq 0 then  ERRORS_ADD, nb_errors, 'the var *is* defined (as a sting'
if ISA(str_var,"STRING") eq 0 then  ERRORS_ADD, nb_errors, 'the var *is* a String'
if ISA(str_var,"OTHER") eq 1 then  ERRORS_ADD, nb_errors, 'the var is not OTHER'
if ISA(str_var,/NULL) eq 1 then  ERRORS_ADD, nb_errors, 'the var is not !NULL'
if ISA(str_var,"undefined",/NULL) eq 1 then  ERRORS_ADD, nb_errors, 'the var is defined and not Null'
if ISA(str_var,"undefined") eq 1 then  ERRORS_ADD, nb_errors, 'the var is defined'
if ISA(str_var, /number) eq 1 then  ERRORS_ADD, nb_errors, 'not a number'
if ISA(str_var, /array) eq 1 then  ERRORS_ADD, nb_errors, 'not a array'
if ISA(str_var, /boolean) eq 1 then  ERRORS_ADD, nb_errors, 'not a Bool'
if ISA(str_var, /integer) eq 1 then  ERRORS_ADD, nb_errors, 'not a Int'
if ISA(str_var, /float) eq 1 then  ERRORS_ADD, nb_errors, 'not a Float'
if ISA(str_var, /complex) eq 1 then  ERRORS_ADD, nb_errors, 'not a Complex'
;
; -------------- final message ------------
;
BANNER_FOR_TESTSUITE, ROUTINE_NAME(), nb_errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_SET(test) then STOP
;
end
;
; -----------------------------------------------
;
pro TEST_ISA_FOR_UNDEFINED, cumul_errors, verbose=verbose, test=test
;
if KEYWORD_SET(verbose) then verb=1 else verb=0
;
nb_errors=0
accumul_errors=0
;
txt=' testing ISA for UNDEFINED'
if (verb) then MESSAGE, /continue, 'Starting '+txt
;
;; testing type = undefined
if (verb) then MESSAGE, /cont, 'Testing Undefined.'
if ISA(a) eq 1 then  ERRORS_ADD, nb_errors, ''
if ISA(a,"undefined") eq 0 then  ERRORS_ADD, nb_errors, ''
if nb_errors gt 0 then MESSAGE, /cont, 'Errors in Testing Undefined (1).'
ERRORS_RESET, accumul_errors, nb_errors, verbose=verbose
;
;NULL keyword for "UNDEFINED" type
if (verb) then MESSAGE, /cont, 'Testing Undefined and !Null.'
a = !NULL
if ISA(a,/NULL) eq 0 then  ERRORS_ADD, nb_errors, '!null is null'
if ISA(a,"undefined",/NULL) eq 0 then  ERRORS_ADD, nb_errors, 'yes, the variable is null + undefined'
if ISA(a,"undefined") eq 0 then  ERRORS_ADD, nb_errors, 'yes, the variable is undefined'
if nb_errors gt 0 then MESSAGE, /cont, 'Errors in Testing Undefined (2).'
ERRORS_RESET, accumul_errors, nb_errors, verbose=verbose
;
b=b
if ISA(b,/NULL) eq 1 then  ERRORS_ADD, nb_errors, ''
if ISA(b,"undefined",/NULL) eq 1 then  ERRORS_ADD, nb_errors, ''
if ISA(b,"undefined") eq 0 then  ERRORS_ADD, nb_errors, ''
if ISA(b, /number) eq 1 then  ERRORS_ADD, nb_errors, 'not a number'
if ISA(b, /string) eq 1 then  ERRORS_ADD, nb_errors, 'not a String'
if ISA(b, /array) eq 1 then  ERRORS_ADD, nb_errors, 'not a array'
if ISA(b, /boolean) eq 1 then  ERRORS_ADD, nb_errors, 'not a Bool'
if ISA(b, /integer) eq 1 then  ERRORS_ADD, nb_errors, 'not a Int'
if ISA(b, /float) eq 1 then  ERRORS_ADD, nb_errors, 'not a Float'
if ISA(b, /complex) eq 1 then  ERRORS_ADD, nb_errors, 'not a Complex'
;
if nb_errors gt 0 then MESSAGE, /cont, 'Errors in Testing Undefined (3).'
ERRORS_RESET, accumul_errors, nb_errors, verbose=verbose
;
; -------------- final message ------------
;
BANNER_FOR_TESTSUITE, ROUTINE_NAME(), accumul_errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, accumul_errors
if KEYWORD_SET(test) then STOP
;
end
;
; -----------------------------------------------
;
; If fact /file is not ready in ISA()
;
pro TEST_ISA_FOR_FILE, cumul_errors, verbose=verbose, test=test
;
if KEYWORD_SET(verbose) then verb=1 else verb=0
;
nb_errors=0
accumul_errors=0
;
txt=' testing ISA for /FILE'
if (verb) then MESSAGE, /continue, 'Starting '+txt
;
res=EXECUTE('pb=ISA("toto",/file)')
if res EQ 0 then BANNER_FOR_TESTSUITE, ROUTINE_NAME(), 'keyword /FILE not ready in ISA()'
return
;
; -------------- final message ------------
;
BANNER_FOR_TESTSUITE, ROUTINE_NAME(), accumul_errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, accumul_errors
if KEYWORD_SET(test) then STOP
;
end
;;
; -----------------------------------------------
;
pro TEST_ISA, help=help, verbose=verbose, no_exit=no_exit, test=test
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_ISA, help=help, verbose=verbose, no_exit=no_exit, test=test'
    return
endif
;
cumul_errors=0
;
TEST_ISA_FOR_UNDEFINED, cumul_errors, verbose=verbose
;
TEST_ISA_FOR_STRING, cumul_errors, verbose=verbose
;
; testing numerical types, one time for Scalar, one time for Array
;
TEST_ISA_FOR_NUMBERS, cumul_errors, verbose=verbose
TEST_ISA_FOR_NUMBERS, cumul_errors, /array, verbose=verbose
;
; testing Objects, List, Hash
;
TEST_ISA_FOR_OBJECT, cumul_errors, verbose=verbose
;
TEST_ISA_FOR_POINTER, cumul_errors, verbose=verbose
;
TEST_ISA_FOR_STRUCT, cumul_errors, verbose=verbose
;
TEST_ISA_FOR_LIST, cumul_errors, verbose=verbose
;
TEST_ISA_FOR_HASH, cumul_errors, verbose=verbose
;
TEST_ISA_FOR_FILE, cumul_errors, verbose=verbose
;
; -------------------------
; final message
;
BANNER_FOR_TESTSUITE, 'TEST_ISA', cumul_errors, short=short
;
if KEYWORD_SET(test) then STOP
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
end

