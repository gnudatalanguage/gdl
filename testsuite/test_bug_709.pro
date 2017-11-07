function test_bug_709_1
a=dblarr(1,10,3)
string="A               DOUBLE    = Array[1, 10, 3]"
help,a,out=hstring
if (string ne hstring) then return,1
return,0
end

function test_bug_709_2
a=dblarr(10,10,3)
b=a[5,*,*]
string="B               DOUBLE    = Array[1, 10, 3]"
help,b,out=hstring
if (string ne hstring) then return,1
return,0
end

function test_bug_709_3
a=dblarr(3,10,1)
string="A               DOUBLE    = Array[3, 10]"
help,a,out=hstring
if (string ne hstring) then return,1
return,0
end

function test_bug_709_4
a=dblarr(10,10,3)
b=a[*,*,1]
string="B               DOUBLE    = Array[10, 10]"
help,b,out=hstring
if (string ne hstring) then return,1
return,0
end

function test_bug_709_5
a = dblarr(10)  
a = reform(a, 10, 1, /overwrite)  
string="A               DOUBLE    = Array[10, 1]"
help,a,out=hstring
if (string ne hstring) then return,1
return,0
end

function test_bug_709_6
s = {a:dblarr(2,2)}
b = replicate(s, 13)
string = "<Expression>    DOUBLE    = Array[1, 13]"
help,b.a[0,0,*],out=hstring
if (string ne hstring) then return,1
return,0
end
function test_bug_709_7
s = {a:dblarr(2,2,2)}
b = replicate(s, 13)
string = "<Expression>    DOUBLE    = Array[1, 1, 2, 13]"
help,b.a[0,0,*],out=hstring
if (string ne hstring) then return,1
return,0
end
function test_bug_709_8
catch, err
if (err ne 0) then begin
 PRINT, 'Error in test #8'
 PRINT, 'Error message: ', !ERROR_STATE.MSG 
 catch,/cancel
 return,1
endif
s = {a:dblarr(2)}
b = replicate(s, 13)
b.a[0,*] = dblarr(1,13)
return,0
end
function test_bug_709_9
catch, err
if (err ne 0) then begin
 PRINT, 'Error in test #9'
 PRINT, 'Error message: ', !ERROR_STATE.MSG 
 catch,/cancel
 return,1
endif
structarray=REPLICATE({test, value:0.0},10)
;populate values:
structarray.value=FINDGEN(10)
return,0 
end
function test_bug_709_10
catch, err
if (err ne 0) then begin
 PRINT, 'Error in test #10'
 PRINT, 'Error message: ', !ERROR_STATE.MSG 
 return,1
 exit
endif
a = {value:reform(indgen(2*3*5),2,3,5)} ; note bug is not present when value has less dimensions than 3.
structarr = replicate(a, 7)
multipleindex=[1,4,6]
v=structarr(multipleindex).value(1,1:2,3) ; is a INT Array[1, 2, 3]
structarr(multipleindex).value(1,1:2,3)=0
w=structarr(multipleindex).value(1,1:2,3) ; is a INT Array[1, 2, 3]
structarr(multipleindex).value(1,1:2,3)=v ; this assign is in error for gdl 0.9.7
x=structarr(multipleindex).value(1,1:2,3) ; is a INT Array[1, 2, 3]
if total(x-v) ne 0 or total(w) ne 0.0 then return,1
return,0
end

pro test_bug_709,verb=verb
if (n_elements(verb) eq 0) then verb=0
; various tests around the IDL behaviour on handling degenerate
; leading and trailing dimensions of size 1.
; 1) on direct creation, keep leading
nerr=0
nerr+=test_bug_709_1()
; 2) on extraction, keep leading
nerr+=test_bug_709_2()
; 3) on direct creation, drop trailing
nerr+=test_bug_709_3()
; 4) on extraction, drop trailing
nerr+=test_bug_709_4()
; 5) REFORM can be used to add trailing degenerate dimensions:
nerr+=test_bug_709_5()
; it is not expected the previous tests fail.

; things are a bit  more touchy with structures:
; 6) 
nerr+=test_bug_709_6()
; 7) 
nerr+=test_bug_709_7()
; 8) the original #709 bug:
nerr+=test_bug_709_8()
; 9) same test as in test_structures:
nerr+=test_bug_709_9()
; 10) further bug reported by C. Hummel:
nerr+=test_bug_709_10()

; display
if (verb) or (nerr GT 0) then BANNER_FOR_TESTSUITE, 'test_bug_709', nerr
end
