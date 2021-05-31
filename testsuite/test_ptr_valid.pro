;
; Updated by Eloi Rozier de Linage on May 31, 2021
; following a bug found in ptr_new()
;
pro TEST_PTR_VALID, test=test, quiet=quiet, help=help, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_PTR_VALID, test=test, quiet=quiet, help=help, no_exit=no_exit'
    return
endif
;
; First, look for pre-existing pointers.
; if there are any, maybe the user doesn't want to run this.
p = ptr_valid()
if size(p,/type) ne 10 then begin
	message,' ptr_valid() did not return even a pointer type '
	exit, status=1
endif
if (n_elements(p) ne 1) then message,/con,' ptr_valid() indicates multiple pre-existing pointers'

if ptr_valid(p[0]) then message,/con,' ptr_valid() indicates a pre-existing pointer'

; if(~KEYWORD_SET(quiet)) then print, ' HEAP_GC called will reset pointer indeces'

; closed bug 708: This didn't work.
ab = ptr_new(fltarr(12))
cmp = {a:ab, b:ab}
errors=0
pcmp = ptr_new(cmp)
cmp = 0

if ptr_valid(ptr_valid(10001,/cast)) then ERRORS_ADD, errors, 'Error 1' $
else if ~KEYWORD_SET(quiet) then message,/con,' NullPointer ok'

p = (ptr_valid())[0] 
pval = ptr_valid(p,/get_heap)
if ~KEYWORD_SET(quiet)  then message,/con,' ptr_valid(p,/get_heap) value=',pval

if ~ptr_valid(p) then ERRORS_ADD, errors, 'Error 2' $
else if ~KEYWORD_SET(quiet)  then message,/con,' p =ab ok'

newptr = ptr_valid(pval,/cast)
if newptr ne p then ERRORS_ADD, errors, 'Error 3' $
else if ~KEYWORD_SET(quiet)  then message,/con,' ptr=ptr_valid(lval,/cast) passed'

llist = list() & mlist = list()
pps=ptrarr(2)
pps[0] = ptr_new(llist)
pps[1] = ptr_new(mlist)
if total(ptr_valid(pps)) ne 2  then ERRORS_ADD, errors, 'Error 4' $
else if(~KEYWORD_SET(quiet)) then message, /con, ' 2 created pointers are valid'

; GD: I'm not sure about the pertinence of above tests. Issue #425 showed that PTR_VALID was perfectly invalid in most cases.
; the following is however sure:
; will crash if bug #241 is not cured as ptr_valid(on_a_not_pointer) is always 0 whatever the type.
a={un:1, deux:[0,4], trois:[0.66,68.33,222.16], quatre:'zzzzz'}
; simple tests 
x=ptr_valid(a) ; before would have crashed on a being a structure
x=ptr_valid(a.(1)) & if total(x) ne 0 then ERRORS_ADD, errors, 'Error 5'
x=ptr_valid(a.(2)) & if total(x) ne 0 then ERRORS_ADD, errors, 'Error 6'
x=ptr_valid(a.(3)) & if total(x) ne 0 then ERRORS_ADD, errors, 'Error 7'
; more complicated: valid and not valid array of pointers:
D=PTRARR(10)& c=dindgen(10) & for i=0,5 do d[i]=ptr_new(c[i])
; x should be a pointer on the double precision value "2.000", of course provided we get the value of the heap slot good for d[2]:
pos=ptr_valid(d[2],/get)
x=PTR_VALID(pos,/cast)
if isa(x,"Pointer") ne 1 then ERRORS_ADD, errors, 'Error 8'
if isa((*x),"Double") ne 1 then ERRORS_ADD, errors, 'Error 9'
if *x ne 2 then err++
res=PTR_VALID(D,/GET) & if isa(res,"Ulong") ne 1 then ERRORS_ADD, errors, 'Error 10'
; last 4 values of res must be zero as they are not initialized:
if total(res[6:9]) ne 0 then ERRORS_ADD, errors, 'Error 11'
; same with byte output
res=PTR_VALID(D) & if isa(res,"Byte") ne 1 then ERRORS_ADD, errors, 'Error 12'
; last 4 values of res must be zero as they are not initialized:
if total(res[6:9]) ne 0 then ERRORS_ADD, errors, 'Error 13'
PTR_FREE, D ; clean pointed values ---> NULL
res=PTR_VALID(D) & if total(res) ne 0 then ERRORS_ADD, errors, 'Error 14'
; x points now to <nothing>:
if ptr_valid(x) ne 0 then ERRORS_ADD, errors, 'Error 15'

; following should complain and must be trapped:
; zz=ptr_valid(a,/cast) --> struct expression not allowed in this context: A
;
; separately, test equality to !NULL for valid and invalid pointers
; the idea is , if a pointer is undefined, it is equal to !NULL. But a pointer to !NULL is not undefined:
good=[1b,0b] & p = PTR_NEW(33) & res=[ptr_valid(p),p eq !NULL] & if total(res eq good) ne 2 then ERRORS_ADD, errors, 'Error 16'
good=[0b,1b] & p = PTR_NEW() & res=[ptr_valid(p),p eq !NULL] & if total(res eq good) ne 2 then ERRORS_ADD, errors, 'Error 17'
good=[1b,0b] & p = PTR_NEW(!NULL) & res=[ptr_valid(p),p eq !NULL] & if total(res eq good) ne 2 then ERRORS_ADD, errors, 'Error 18'
;
; bug 955: ptr_new(!NULL) and ptr_new(undef_var) should point to a !NULL var
ptr_null=ptr_new(!NULL)
ptr_undef_var=ptr_new(undef_var)
if ISA(*ptr_null, /NULL) eq 0 then ERRORS_ADD, errors, 'Error: ptr_new(!NULL) does not point towards a !NULL var'
if ISA(*ptr_null, /NULL) eq 0 then ERRORS_ADD, errors, 'Error: ptr_new(undef_var) does not point towards a !NULL var'
;
; ------------------- final message ------------------
BANNER_FOR_TESTSUITE,' TEST_PTR_VALID', errors
;
if (errors gt 0) and ~keyword_set(no_exit) then exit, status = 1 
;
if keyword_set(test) then stop
;
end

