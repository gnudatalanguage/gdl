pro test_bug_709

; various tests around the IDL behaviour on handling degenerate
; leading and trailing dimensions of size 1.
; 1) on direct creation, keep leading
a=dblarr(1,10,3)
string="A               DOUBLE    = Array[1, 10, 3]"
help,a,out=hstring
if (string ne hstring) then message,"ko 1"

; 2) on extraction, keep leading
a=dblarr(10,10,3)
b=a[5,*,*]
string="B               DOUBLE    = Array[1, 10, 3]"
help,b,out=hstring
if (string ne hstring) then message,"ko 2"

; 3) on direct creation, drop trailing
a=dblarr(3,10,1)
string="A               DOUBLE    = Array[3, 10]"
help,a,out=hstring
if (string ne hstring) then message,"ko 3"

; 4) on extraction, drop trailing
a=dblarr(10,10,3)
b=a[*,*,1]
string="B               DOUBLE    = Array[10, 10]"
help,b,out=hstring
if (string ne hstring) then message,"ko 4"

; 5) REFORM can be used to add trailing degenerate dimensions:
a = dblarr(10)  
a = reform(a, 10, 1, /overwrite)  
string="A               DOUBLE    = Array[10, 1]"
help,a,out=hstring
if (string ne hstring) then message,"ko 5"

; it is not expected the previous tests fail.
; things are a bit  more touchy with structures:

; 6) 
s = {a:dblarr(2,2)}
b = replicate(s, 13)
string = "<Expression>    DOUBLE    = Array[1, 13]"
help,b.a[0,0,*],out=hstring
if (string ne hstring) then message,"ko 6"
; 7) 
s = {a:dblarr(2,2,2)}
b = replicate(s, 13)
string = "<Expression>    DOUBLE    = Array[1, 1, 2, 13]"
help,b.a[0,0,*],out=hstring
if (string ne hstring) then message,"ko 7"
; 8) the original #709 bug:
catch, err
if (err ne 0) then begin
 PRINT, 'Error in test #8'
 PRINT, 'Error message: ', !ERROR_STATE.MSG 
 catch,/cancel
 exit
endif
s = {a:dblarr(2)}
b = replicate(s, 13)
b.a[0,*] = dblarr(1,13)
; 9) same test as in test_structures:
catch, err
if (err ne 0) then begin
 PRINT, 'Error in test #9'
 PRINT, 'Error message: ', !ERROR_STATE.MSG 
 catch,/cancel
 exit
endif
structarray=REPLICATE({test, value:0.0},10)
;populate values:
structarray.value=FINDGEN(10)
; 10) further bug reported by C. Hummel:
catch, err
if (err ne 0) then begin
 PRINT, 'Error in test #10'
 PRINT, 'Error message: ', !ERROR_STATE.MSG 
 catch,/cancel
 exit
endif
a = {value:reform(indgen(2*16*3),2,16,3)} ; note bug is not present when value has less dimensions than 3.
structarr = replicate(a, 223)
multipleindex=[132,140]
v=structarr(multipleindex).value(0,0:15,0) ; is a INT Array[1, 16, 2]
structarr(multipleindex).value(0,0:15,0)=0
w=structarr(multipleindex).value(0,0:15,0) ; is a INT Array[1, 16, 2]
structarr(multipleindex).value(0,0:15,0)=v ; this assign is in error for gdl 0.9.7
x=structarr(multipleindex).value(0,0:15,0) ; is a INT Array[1, 16, 2]
if total(x-v) ne 0 or total(w) ne 0.0 then begin
 PRINT, 'Error in test #10, 2nd flavor'
endif
end
