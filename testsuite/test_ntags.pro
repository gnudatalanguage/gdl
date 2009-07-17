;
; T. Mermet
; under GPL v2. or later 
;
; Purpose: quick check of computation of structure (data) length
;
; Expected sizes:
; BYTE  : 1
; INT   : 2 
; LONG  : 4
; FLOAT : 4
; DOUBLE: 8
; (check with success on x86 and x86_64)
;
pro TEST_NTAGS
;
tmp = {data_n2, ydh:0l, num:0l, t97:0.d0, f:0., dt:0., df:0., $
        autoX:0., autoZ:0., crossR:0., crossI:0., ant:0b, a: 0}
;
test={data_n2}
help,test,/str
print,"data_length :", N_TAGS(test,/data_length)
;
; here we have: 1*B+1*I+2*L+7*F+1*D ==> 47
;
if (N_TAGS(test,/data_length) eq 47) then begin
	print, "test ok"
endif else begin
	print, "test failed"
endelse
;
end