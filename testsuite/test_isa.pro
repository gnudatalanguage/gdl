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
pro INCREMENT_ERRORS, total_errors, nb_errors, verbose=verbose
if KEYWORD_SET(verbose) then print, total_errors, nb_errors
total_errors=total_errors+nb_errors
nb_errors=0
end
; -----------------------------------------------
;
pro TEST_ISA, nb_errors=nb_errors, tmp_var=tmp_var, $
              help=help, verbose=verbose, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_ISA, nb_errors=nb_errors, tmp_var=tmp_var, $'
    print, '              help=help, verbose=verbose, no_exit=no_exit'
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
;for numerical types, expect "PTR","OBJ","STR","STRING"
list_type = ["BYTE","INT","LONG","FLOAT","DOUBLE","COMPLEX",$
             "DCOMPLEX","UINT","ULONG","LONG64","ULONG64"]
list_keywords = ["/NULL","/ARRAY","/SCALAR","/NUMBER"]

n = N_ELEMENTS(list_type)

for i=0,n-1 do begin
    ;;
    tmp_var = TYPE_GEN(list_type[i])
    ;;
    if ISA(tmp_var) eq 0 then nb_errors=nb_errors+1
    if ISA(tmp_var,list_type[i]) eq 0 then nb_errors=nb_errors+1
    if ISA(tmp_var,"OTHER") eq 1 then nb_errors=nb_errors+1
    ;;
    for j=1,n-2 do begin
        ind = i+j
        if ind lt n then ind=ind else  ind = ind - n
        if ISA(tmp_var,list_type[ind]) eq 1 then nb_errors=nb_errors+1 	
    endfor	
    ;;
    ;;if ISA gives 1 for different types, 
    ;; ( for example: a = 2   &&   ISA(a,"STRING")-->1)
    ;;testing string
    ;;
    if ISA(str_var,list_type[i]) eq 1 then nb_errors=nb_errors+1
    for j=i+1,n-1 do begin
        if ISA(tmp_var,list_type[j]) eq 1 then nb_errors=nb_errors+1
    endfor
    ;;
    ;;      testing keywords
    ;;NULL
    if ISA(tmp_var,/NULL) eq 1 then nb_errors=nb_errors+1
    ;;ARRAY	
    if ISA(tmp_var,/ARRAY) eq 1 then nb_errors=nb_errors+1
    ;;SCALAR
    if ISA(tmp_var,/SCALAR) eq 0 then nb_errors=nb_errors+1
    ;;
    if (nb_errors GT 0) then begin
        MESSAGE, /continue, 'Errors found within type : '+list_type[i]
    endif
    INCREMENT_ERRORS, total_errors, nb_errors, verbose=verbose
endfor
;
BANNER_FOR_TESTSUITE, 'TEST_ISA', total_errors, short=short
;
if (total_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
