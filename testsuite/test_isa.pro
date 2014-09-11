;
; various basics tests on ISA()
; written by Levan Loria, June 2014
; under GNU GPL v2 or any later
;
; 3 cases are not ready : Obj. (+ LIST and HASH), Struct., Pointers
;
function TYPE_GEN, type
;
if type eq "BYTE" then  return,BYTE(1)
if type eq "INT" then type_fun=1
if type eq "LONG" then return,LONG(1)	
if type eq "FLOAT" then return,FLOAT(1)
if type eq "DOUBLE" then return,DOUBLE(1)
if type eq "COMPLEX" then return,COMPLEX(1)
if type eq "DCOMPLEX" then return,DCOMPLEX(1)
;;if type eq "STRING" then return,STRING('1')
if type eq "UINT" then return,UINT(1)
if type eq "ULONG" then return,ULONG(1)
if type eq "LONG64" then return,LONG64(1)
if type eq "ULONG64" then return,ULONG64(1)
;
return,type_fun
;
end
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
;  -----------------------------------------------
;
pro TEST_ISA_FOR_LIST, external_errors, verbose=verbose
;
if KEYWORD_SET(verbose) then verb=1 else verb=0
;
nb_errors=0
accumul_errors=0
;
txt=' testing ISA for LIST'
if (verb) then MESSAGE, /continue, 'Starting '+txt
;
data = LIST()
if ISA(data) eq 0 then nb_errors=nb_errors+1
if ISA(data,'LIST') eq 0 then nb_errors=nb_errors+1
if ISA(data,'OBJREF') eq 0 then nb_errors=nb_errors+1
if ISA(data,/NULL) eq 1 then nb_errors=nb_errors+1
if ISA(data,"UNDEFINED") eq 1 then nb_errors=nb_errors+1
if ISA(data,/ARRAY) eq 0 then nb_errors=nb_errors+1
if (verb) or (nb_errors GT 0) then BANNER_FOR_TESTSUITE, 'Void list() test done', nb_errors,/short
INCREMENT_ERRORS, accumul_errors, nb_errors
;
;error in LIST rank
;
data = LIST(1,2,3)
if ISA(data,/ARRAY) eq 0 then nb_errors=nb_errors+1
if ISA(data,/SCALAR) eq 1 then nb_errors=nb_errors+1
if ISA(data,/NULL) eq 1 then nb_errors=nb_errors+1
if ISA(data,/Number) eq 1 then nb_errors=nb_errors+1
if (verb) or (nb_errors GT 0) then BANNER_FOR_TESTSUITE, 'list(1,2,3) test done', nb_errors, /short
INCREMENT_ERRORS, accumul_errors, nb_errors
;
;error in LIST rank
;
type_info='list'
data = LIST(1,2,3)
if ISA(data,type_info,/ARRAY) eq 0 then nb_errors=nb_errors+1
if ISA(data,type_info,/SCALAR) eq 1 then nb_errors=nb_errors+1
if ISA(data,type_info,/NULL) eq 1 then nb_errors=nb_errors+1
if ISA(data,type_info,/Number) eq 1 then nb_errors=nb_errors+1
if (verb) or (nb_errors GT 0) then BANNER_FOR_TESTSUITE, 'list(1,2,3) test done (with type LIST)', nb_errors, /short
INCREMENT_ERRORS, accumul_errors, nb_errors
;
type_info='objref'
data = LIST(1,2,3)
if ISA(data,type_info,/ARRAY) eq 0 then nb_errors=nb_errors+1
if ISA(data,type_info,/SCALAR) eq 1 then nb_errors=nb_errors+1
if ISA(data,type_info,/NULL) eq 1 then nb_errors=nb_errors+1
if ISA(data,type_info,/Number) eq 1 then nb_errors=nb_errors+1
if (verb) or (nb_errors GT 0) then BANNER_FOR_TESTSUITE, 'list(1,2,3) test done (with type ObjRef)', nb_errors, /short
INCREMENT_ERRORS, accumul_errors, nb_errors
;
type_info='no_a_type'
data = LIST(1,2,3)
if ISA(data,type_info,/ARRAY) eq 1 then nb_errors=nb_errors+1
if ISA(data,type_info,/SCALAR) eq 1 then nb_errors=nb_errors+1
if ISA(data,type_info,/NULL) eq 1 then nb_errors=nb_errors+1
if ISA(data,type_info,/Number) eq 1 then nb_errors=nb_errors+1
if (verb) or (nb_errors GT 0) then BANNER_FOR_TESTSUITE, 'list(1,2,3) test done (with type other) ', nb_errors, /short
INCREMENT_ERRORS, accumul_errors, nb_errors
;
if accumul_errors gt 0 then MESSAGE, /cont, 'Errors in '+txt else $
  if (verb) then MESSAGE, /cont, 'Success in '+txt
;
INCREMENT_ERRORS, external_errors, accumul_errors

end
;
;  -----------------------------------------------
;
pro TEST_ISA_FOR_HASH, external_errors, verbose=verbose
;
if KEYWORD_SET(verb) then verb=1 else verb=0
;
nb_errors=0
;
txt=' testing ISA for HASH'
if (verb) then MESSAGE, /continue, 'Starting '+txt
;
data=HASH()
;
if ISA(data) eq 0 then nb_errors=nb_errors+1
if ISA(data,'HASH') eq 0 then nb_errors=nb_errors+1
if ISA(data,'OBJREF') eq 0 then nb_errors=nb_errors+1
if ISA(data,/NULL) eq 1 then nb_errors=nb_errors+1
if ISA(data,"UNDEFINED") eq 1 then nb_errors=nb_errors+1
;
if nb_errors gt 0 then MESSAGE, /cont, 'Errors in '+txt else $
  if (verb) then MESSAGE, /cont, 'Success in '+txt
;
INCREMENT_ERRORS, external_errors, nb_errors

end

;  -----------------------------------------------

pro TEST_ISA_FOR_POINTER, external_errors, verbose=verbose
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
if ISA(ptr) eq 1 then nb_errors=nb_errors+1
if ISA(ptr,/NULL) eq 1 then nb_errors=nb_errors+1
if ISA(ptr,'POINTER') eq 0 then nb_errors=nb_errors+1
if ISA(ptr,'STRING') eq 1 then nb_errors=nb_errors+1
;
;testing not a NULL pointer
;
ptr = PTR_NEW(1)
;
if ISA(ptr) eq 0 then nb_errors=nb_errors+1
if ISA(*ptr) eq 0 then nb_errors=nb_errors+1
;
if nb_errors gt 0 then MESSAGE, /cont, 'Errors in '+txt else $
  if (verb) then MESSAGE, /cont, 'Success in '+txt
;
INCREMENT_ERRORS, external_errors, nb_errors
;
end
;
; -----------------------------------------------
;
pro TEST_ISA_FOR_OBJECT, external_errors, verbose=verbose
;
if KEYWORD_SET(verbose) then verb=1 else verb=0
;
nb_errors=0
;
txt=' testing ISA for OBJECT'
if (verb) then MESSAGE, /continue, 'Starting '+txt
;
obj = OBJ_NEW()
if ISA(obj) eq 1 then nb_errors=nb_errors+1
if ISA(obj,'OBJECT') eq 1 then nb_errors=nb_errors+1
if ISA(obj,'OBJREF') eq 0 then nb_errors=nb_errors+1
if ISA(obj,/NULL) eq 1 then nb_errors=nb_errors+1
;
obj = OBJ_NEW('LIST')
if ISA(obj) eq 0 then nb_errors=nb_errors+1
if ISA(obj,'OBJREF') eq 0 then nb_errors=nb_errors+1
if ISA(obj,'LIST') eq 0 then nb_errors=nb_errors+1
;
if nb_errors gt 0 then MESSAGE, /cont, 'Errors in '+txt else $
  if (verb) then MESSAGE, /cont, 'Success in '+txt
;
INCREMENT_ERRORS, external_errors, nb_errors
;
end
;
; -----------------------------------------------
;
pro TEST_ISA_FOR_STRUCT, external_errors, verbose=verbose
;
if KEYWORD_SET(verbose) then verb=1 else verb=0
;
nb_errors=0
;
txt=' testing ISA for Structures'
if (verb) then MESSAGE, /continue, 'Starting '+txt
;
str = {MYSTRUCT, field1: 'f1'}
if ISA(str) eq 0 then nb_errors=nb_errors+1
if ISA(str,'STRUCT') eq 0 then nb_errors=nb_errors+1
if ISA(str,'MYSTRUCT') eq 0 then nb_errors=nb_errors+1
;
if nb_errors gt 0 then MESSAGE, /cont, 'Errors in '+txt else $
  if (verb) then MESSAGE, /cont, 'Success in '+txt
;
INCREMENT_ERRORS, external_errors, nb_errors
;
end
;
; -----------------------------------------------
;
pro TEST_ISA_FOR_NUMBERS, external_errors, array=array, verbose=verbose
;
if KEYWORD_SET(array) then dim=1 else dim=0
;
if KEYWORD_SET(verbose) then verb=1 else verb=0
;
if (verb) then begin
    txt='Starting testing for Numbers types '
    if (dim EQ 0) then MESSAGE, /continue, txt+'(scalar)'
    if (dim EQ 1) then MESSAGE, /continue, txt+'(array)'
endif
;
;for numerical types, expect "PTR","OBJ","STR","STRING"
list_type = ["BYTE","INT","LONG","FLOAT","DOUBLE","COMPLEX",$
             "DCOMPLEX","UINT","ULONG","LONG64","ULONG64"]
;list_keywords = ["/NULL","/ARRAY","/SCALAR","/NUMBER"]
;
n = N_ELEMENTS(list_type)
;
; this is for internal errors only
nb_errors=0
;
for i=0,n-1 do begin
    ;;
    tmp_var = TYPE_GEN(list_type[i])
    ;;
    if (dim EQ 1) then tmp_var =REPLICATE(tmp_var, 4)
    ;;
    if (verb) then HELP, tmp_var
    ;;
    if ISA(tmp_var) eq 0 then nb_errors=nb_errors+1
    if ISA(tmp_var,list_type[i]) eq 0 then nb_errors=nb_errors+1
    if ISA(tmp_var,"OTHER") eq 1 then nb_errors=nb_errors+1
    ;;
    for j=1,n-2 do begin
        ind = i+j
        if ind lt n then ind=ind else  ind = ind - n
        if ISA(tmp_var,list_type[ind]) eq 1 then nb_errors=nb_errors+1 	
        if ISA(tmp_var,list_type[ind],/SCALAR) eq 1 then nb_errors=nb_errors+1
        if ISA(tmp_var,list_type[ind],/ARRAY) eq 1 then nb_errors=nb_errors+1
        if ISA(tmp_var,list_type[ind],/NUMBER) eq 1 then nb_errors=nb_errors+1
    endfor	
    ;;
    ;;if ISA gives 1 for different types, 
    ;; ( for example: a = 2   &&   ISA(a,"STRING")-->1)
    ;;testing string
    ;;
    if ISA(str_var,list_type[i]) eq 1 then nb_errors=nb_errors+1
    for j=i+1,n-1 do begin
        if ISA(tmp_var,list_type[j]) eq 1 then nb_errors=nb_errors+1
        if ISA(tmp_var,list_type[j],/SCALAR) eq 1 then nb_errors=nb_errors+1
        if ISA(tmp_var,list_type[j],/ARRAY) eq 1 then nb_errors=nb_errors+1
        if ISA(tmp_var,list_type[j],/NUMBER) eq 1 then nb_errors=nb_errors+1
    endfor
    ;;
    ;;      testing keywords
    ;;NULL
    if ISA(tmp_var,/NULL) eq 1 then nb_errors=nb_errors+1
    ;;NUMBER
    if ISA(tmp_var,/NUMBER) eq 0 then nb_errors=nb_errors+1
    ;;
    ;; cross test Scalar or Array
    if (dim EQ 1) then begin
        if ISA(tmp_var,/ARRAY) eq 0 then nb_errors=nb_errors+1
        if ISA(tmp_var,/SCALAR) eq 1 then nb_errors=nb_errors+1
    endif else begin
        if ISA(tmp_var,/ARRAY) eq 1 then nb_errors=nb_errors+1
        if ISA(tmp_var,/SCALAR) eq 0 then nb_errors=nb_errors+1
    endelse
    ;;
    if (nb_errors GT 0) then begin
        MESSAGE, /continue, 'Errors found within type : '+list_type[i]
    endif
    INCREMENT_ERRORS, external_errors, nb_errors, verbose=verbose
endfor
;
end
;
; -----------------------------------------------
;
pro TEST_ISA, nb_errors=nb_errors, tmp_var=tmp_var, $
              help=help, verbose=verbose, no_exit=no_exit, test=test
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_ISA, nb_errors=nb_errors, tmp_var=tmp_var, $'
    print, '              help=help, verbose=verbose, no_exit=no_exit, test=test'
    return
endif
;
total_errors=0
nb_errors=0
;
if KEYWORD_SET(verbose) then verb=1 else verb=0
;
;; testing type = undefined
if (verb) then MESSAGE, /cont, 'Testing Undefined.'
if ISA(a) eq 1 then nb_errors=nb_errors+1
if ISA(a,"undefined") eq 0 then nb_errors=nb_errors+1
if nb_errors gt 0 then MESSAGE, /cont, 'Errors in Testing Undefined (1).'
INCREMENT_ERRORS, total_errors, nb_errors, verbose=verbose
;
;NULL keyword for "UNDEFINED" type
if (verb) then MESSAGE, /cont, 'Testing Undefined and !Null.'
a = !NULL
if ISA(a,/NULL) eq 0 then nb_errors=nb_errors+1
if ISA(a,"undefined",/NULL) eq 0 then nb_errors=nb_errors+1
if ISA(a,"undefined") eq 0 then nb_errors=nb_errors+1
if nb_errors gt 0 then MESSAGE, /cont, 'Errors in Testing Undefined (2).'
INCREMENT_ERRORS, total_errors, nb_errors, verbose=verbose
;
b=b
if ISA(b,/NULL) eq 1 then nb_errors=nb_errors+1
if ISA(b,"undefined",/NULL) eq 1 then nb_errors=nb_errors+1
if ISA(b,"undefined") eq 0 then nb_errors=nb_errors+1
if nb_errors gt 0 then MESSAGE, /cont, 'Errors in Testing Undefined (3).'
INCREMENT_ERRORS, total_errors, nb_errors, verbose=verbose
;
; testing string
str_var = STRING(1)
if ISA(str_var) eq 0 then nb_errors=nb_errors+1
if ISA(str_var,"STRING") eq 0 then nb_errors=nb_errors+1
if ISA(str_var,"OTHER") eq 1 then nb_errors=nb_errors+1
if nb_errors gt 0 then MESSAGE, /cont, 'Errors in Testing STRING.'
INCREMENT_ERRORS, total_errors, nb_errors, verbose=verbose
;
; testing numerical types, one time for Scalar, one time for Array
;
TEST_ISA_FOR_NUMBERS, nb_errors, verbose=verbose
if nb_errors gt 0 then MESSAGE, /cont, 'Errors in Testing NUMBERS (scalar).'
INCREMENT_ERRORS, total_errors, nb_errors, verbose=verbose
;
TEST_ISA_FOR_NUMBERS, nb_errors, /array, verbose=verbose
if nb_errors gt 0 then MESSAGE, /cont, 'Errors in Testing NUMBERS (array).'
INCREMENT_ERRORS, total_errors, nb_errors, verbose=verbose
;
; testing Objects, List, Hash
;
TEST_ISA_FOR_OBJECT, nb_errors, verbose=verbose
if nb_errors gt 0 then MESSAGE, /cont, 'Errors in Testing OBJECT'
INCREMENT_ERRORS, total_errors, nb_errors, verbose=verbose
;
TEST_ISA_FOR_POINTER, nb_errors, verbose=verbose
if nb_errors gt 0 then MESSAGE, /cont, 'Errors in Testing POINTER'
INCREMENT_ERRORS, total_errors, nb_errors, verbose=verbose

TEST_ISA_FOR_STRUCT, nb_errors, verbose=verbose
if nb_errors gt 0 then MESSAGE, /cont, 'Errors in Testing STRUCT'
INCREMENT_ERRORS, total_errors, nb_errors, verbose=verbose

TEST_ISA_FOR_LIST, nb_errors, verbose=verbose
if nb_errors gt 0 then MESSAGE, /cont, 'Errors in Testing LIST'
INCREMENT_ERRORS, total_errors, nb_errors, verbose=verbose

TEST_ISA_FOR_HASH, nb_errors, verbose=verbose
if nb_errors gt 0 then MESSAGE, /cont, 'Errors in Testing HASH'
INCREMENT_ERRORS, total_errors, nb_errors, verbose=verbose
;
; final message
;
BANNER_FOR_TESTSUITE, 'TEST_ISA', total_errors, short=short
;
if KEYWORD_SET(test) then STOP
;
if (total_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
end

