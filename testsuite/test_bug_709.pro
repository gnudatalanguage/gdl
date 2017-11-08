function test_bug_709_1,verb=verb
a=dblarr(1,10,3)
string="A               DOUBLE    = Array[1, 10, 3]"
help,a,out=hstring
if (string ne hstring) then begin if verb then print,"error in 1" & return,1 & endif else  print,"OK for 1"
return,0
end

function test_bug_709_2,verb=verb
a=dblarr(10,10,3)
b=a[5,*,*]
string="B               DOUBLE    = Array[1, 10, 3]"
help,b,out=hstring
if (string ne hstring) then begin if verb then print,"error in 2" & return,1 & endif else  print,"OK for 2"
return,0
end

function test_bug_709_3,verb=verb
a=dblarr(3,10,1)
string="A               DOUBLE    = Array[3, 10]"
help,a,out=hstring
if (string ne hstring) then begin if verb then print,"error in 3" & return,1 & endif else  print,"OK for 3"
return,0
end

function test_bug_709_4,verb=verb
a=dblarr(10,10,3)
b=a[*,*,1]
string="B               DOUBLE    = Array[10, 10]"
help,b,out=hstring
if (string ne hstring) then begin if verb then print,"error in 4" & return,1 & endif else  print,"OK for 4"
return,0
end

function test_bug_709_5,verb=verb
a = dblarr(10)  
a = reform(a, 10, 1, /overwrite)  
string="A               DOUBLE    = Array[10, 1]"
help,a,out=hstring
if (string ne hstring) then begin if verb then print,"error in 5" & return,1 & endif else  print,"OK for 5"
return,0
end

function test_bug_709_6,verb=verb
s = {a:dblarr(2,2)}
b = replicate(s, 13)
string = "<Expression>    DOUBLE    = Array[1, 13]"
help,b.a[0,0,*],out=hstring
if (string ne hstring) then begin if verb then print,"error in 6" & return,1 & endif else  print,"OK for 6"
return,0
end

function test_bug_709_7,verb=verb
s = {a:dblarr(2,2,2)}
b = replicate(s, 13)
string = "<Expression>    DOUBLE    = Array[1, 1, 2, 13]"
help,b.a[0,0,*],out=hstring
if (string ne hstring) then begin if verb then print,"error in 7" & return,1 & endif else  print,"OK for 7"
return,0
end

function test_bug_709_8,verb=verb
catch, err
if (err ne 0) then begin
 PRINT, 'Error in test #8'
 PRINT, 'Error message: ', !ERROR_STATE.MSG 
 catch,/cancel
 if verb then print,"error in 8"
 return,1 
endif
s = {a:dblarr(2)}
b = replicate(s, 13)
b.a[0,*] = dblarr(1,13)
if verb then print,"OK for 8"
return,0
end
function test_bug_709_9,verb=verb
catch, err
if (err ne 0) then begin
 PRINT, 'Error in test #9'
 PRINT, 'Error message: ', !ERROR_STATE.MSG 
 catch,/cancel
 if verb then print,"error in 9"
 return,1
endif
structarray=REPLICATE({test, value:0.0},10)
;populate values:
structarray.value=FINDGEN(10)
if verb then print,"OK for 9"
return,0 
end
function test_bug_709_10,verb=verb
catch, err
if (err ne 0) then begin
 PRINT, 'Error in test #10'
 PRINT, 'Error message: ', !ERROR_STATE.MSG 
 if verb then print,"error in 10"
 return,1 
endif
a = {value:reform(indgen(2*3*5),2,3,5)} ; note bug is not present when value has less dimensions than 3.
structarr = replicate(a, 7)
multipleindex=[1,4,6]
v=structarr(multipleindex).value(1,1:2,3) ; is a INT Array[1, 2, 3]
structarr(multipleindex).value(1,1:2,3)=0
w=structarr(multipleindex).value(1,1:2,3) ; is a INT Array[1, 2, 3]
structarr(multipleindex).value(1,1:2,3)=v ; this assign is in error for gdl 0.9.7
x=structarr(multipleindex).value(1,1:2,3) ; is a INT Array[1, 2, 3]
if total(x-v) ne 0 or total(w) ne 0.0 then begin if verb then print,"error in X" & return,1 & endif else print,"OK for 10"
return,0
end

pro test_bug_709,verb=verb
if (n_elements(verb) eq 0) then verb=0
; various tests around the IDL behaviour on handling degenerate
; leading and trailing dimensions of size 1.
; 1) on direct creation, keep leading
nerr=0
nerr+=test_bug_709_1(verb=verb)
; 2) on extraction, keep leading
nerr+=test_bug_709_2(verb=verb)
; 3) on direct creation, drop trailing
nerr+=test_bug_709_3(verb=verb)
; 4) on extraction, drop trailing
nerr+=test_bug_709_4(verb=verb)
; 5) REFORM can be used to add trailing degenerate dimensions:
nerr+=test_bug_709_5(verb=verb)
; it is not expected the previous tests fail.

; things are a bit  more touchy with structures:
; 6) 
nerr+=test_bug_709_6(verb=verb)
; 7) 
nerr+=test_bug_709_7(verb=verb)
; 8) the original #709 bug:
nerr+=test_bug_709_8(verb=verb)
; 9) same test as in test_structures:
nerr+=test_bug_709_9(verb=verb)
; 10) further bug reported by C. Hummel:
nerr+=test_bug_709_10(verb=verb)

; display
if (verb) or (nerr GT 0) then BANNER_FOR_TESTSUITE, 'test_bug_709', nerr
end
