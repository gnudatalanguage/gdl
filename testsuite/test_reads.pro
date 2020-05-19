;
; Alain C., 4 Oct. 2015
;
; READS should work with space or comma as separators.
;
; First Bug found thanks to the MIRIM simulator
;
; -------------------------------------
; 
; Modifications history :
;
; - 2020-April-02 : AC.
;   * as mentionned in #701, READS is in error with complex
;   * adding more tests on arrays (basic & mixing types)
;
; -------------------------------------
;
pro TEST_READS_ARRAYS, cumul_errors, verbose=verbose, test=test
;
errors=0
;
input1= '100,800,600'
input2= '100 800 600 400'
;
ok=EXECUTE('READS, input1, x,y,z')
;
if ~ok then ERRORS_ADD, errors, 'EXECUTE failed, first case !'
;
if ~ARRAY_EQUAL(x, 100) then ERRORS_ADD, errors, 'c1 bad value X !'
if ~ARRAY_EQUAL(y, 800) then ERRORS_ADD, errors, 'c1 bad value Y !'
if ~ARRAY_EQUAL(z, 600) then ERRORS_ADD, errors, 'c1 bad value Z !'
;
; we provide only 3 variables for an input with 4 values ...
;
ok=EXECUTE('READS, input2, x,y,z')
;
if ~ok then ERRORS_ADD, errors, 'EXECUTE failed, second case !'
;
if ~ARRAY_EQUAL(x, 100) then ERRORS_ADD, errors, 'c2 bad value X !'
if ~ARRAY_EQUAL(y, 800) then ERRORS_ADD, errors, 'c2 bad value Y !'
if ~ARRAY_EQUAL(z, 600) then ERRORS_ADD, errors, 'c2 bad value Z !'
;
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_READS_ARRAYS', errors, /status
ERRORS_CUMUL, cumul_errors, errors
;
if KEYWORD_SET(test) then STOP
;
end
; -------------------------------------
;
function ICI_ARRAY_EQUAL, x, y, debug=debug
if KEYWORD_SET(debug) then begin
   print, 'expected     :', x
   print, 'what we have :', y
endif
return, ARRAY_EQUAL(x,y)
end
;
pro TEST_READS_COMPLEX, cumul_errors, verbose=verbose, test=test, debug=debug
;
errors=0
;
; basic case, one value
;
input= '(1,2)'
var=COMPLEX(0,0)
ok=EXECUTE('READS, input, var')
if ~ok then ERRORS_ADD, errors, 'EXECUTE 1 failed !'
;
expected=COMPLEX(1,2)
if ~ICI_ARRAY_EQUAL(expected, var, debug=debug) then $
   ERRORS_ADD, errors, 'basic complex 1 val, bad values !'
;
;----- multiple inputs, same size  Case 1 in #701
;
input= '(1,-1) (2,-2) (3,-3)'
var=COMPLEXARR(3)
ok=EXECUTE('READS, input, var')
if ~ok then ERRORS_ADD, errors, 'EXECUTE 2 failed !'
;
tmp=INDGEN(3)+1
expected=COMPLEX(tmp, -tmp)
if ~ICI_ARRAY_EQUAL(expected, var, debug=debug) then $
 ERRORS_ADD, errors, 'basic complex 3 val, bad values !'
;
;----- multiple inputs, different size
input= '(1,-1) (2,-2) (3,-3)'
var=COMPLEXARR(2)
ok=EXECUTE('READS, input, var')
if ~ok then ERRORS_ADD, errors, 'EXECUTE 2 failed !'
;
tmp=INDGEN(2)+1
expected=COMPLEX(tmp, -tmp)
if ~ICI_ARRAY_EQUAL(expected, var, debug=debug) then $
 ERRORS_ADD, errors, 'basic complex 3 val (bis), bad values !'
;
;----- should resist to excess of commas & spaces 
input= '(1,-1) (2,-2),,,,,   (3,-3)'
var=COMPLEXARR(3)
ok=EXECUTE('READS, input, var')
;
if ~ok then ERRORS_ADD, errors, 'EXECUTE failed !'
;
tmp=INDGEN(3)+1
expected=COMPLEX(tmp, -tmp)
if ~ICI_ARRAY_EQUAL(expected, var, debug=debug) then $
 ERRORS_ADD, errors, 'bad format, extra commas, complex bad values !'
;
;----- should resist to missing braces too !!
; case 3 reported in #701 by GD
;
input= '12 , 13'
var=COMPLEXARR(2)
ok=EXECUTE('READS, input, var')
if ~ok then ERRORS_ADD, errors, 'EXECUTE failed !'
;
tmp=INDGEN(2)+12
expected=COMPLEX(tmp, 0)
if ~ICI_ARRAY_EQUAL(expected, var, debug=debug) then $
 ERRORS_ADD, errors, 'bad format, no brace, complex bad values !'
;
;----- should resist to mixed format : case 2 reported in #701 by GD
;
input='  ( 12,13) 18 19'
var=COMPLEXARR(3)
ok=EXECUTE('READS, input, var')
if ~ok then ERRORS_ADD, errors, 'EXECUTE failed !'
;
expected=COMPLEXARR(3)
expected[0]=COMPLEX(12,13)
expected[1]=18
expected[2]=19
if ~ICI_ARRAY_EQUAL(expected, var, debug=debug) then $
 ERRORS_ADD, errors, 'mixed format, complex bad values !'
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_READS_COMPLEX', errors, /status
ERRORS_CUMUL, cumul_errors, errors
;
if KEYWORD_SET(test) then STOP
;
end
;
; -------------------------------------
;
pro TEST_READS_STRING, cumul_errors, verbose=verbose, test=test
;
errors=0
;
input= '1000,800,600'
var=FLTARR(3)
ok=EXECUTE('READS, input, var')
;
if ~ok then ERRORS_ADD, errors, 'EXECUTE failed !'
;
expected=[1000,800,600]
if ~ARRAY_EQUAL(expected, var) then $
   ERRORS_ADD, errors, 'bad values !'
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_READS_STRING', errors, /status
ERRORS_CUMUL, cumul_errors, errors
;
if KEYWORD_SET(test) then STOP
;
end
;
; -------------------------------------
;
; following
; http://www.physics.nyu.edu/grierlab/idl_html_help/files11.html
; the rules for "free format input" are: [...]
; 4. Input data must be separated by commas or white space (tabs, spaces, or new lines).
;
pro TEST_READS_MIXED, cumul_errors, verbose=verbose, test=test
;
errors=0
;
; derived from http://www.iac.es/sieinvens/SINFIN/CursoIDL/idlpp3.php
;
; separated by spaces
thisisheader='10 12 2001 This is the date of my file'
; separeted by comma
thisisheader2='10,12,2001,This is the date of my file'
;
; expected results
;
expected1=[10,12,2001]
expected2a=' This is the date of my file'
expected2b=',This is the date of my file'
;
day=0
month=0
year=0
todaystring1=''
todaystring2=''
;
MESSAGE, /continue, 'Case 1 : separator is a space >> <<'
errors1=0
;
ok1=EXECUTE('READS, thisisheader, day, month, year, todaystring1')
res1=[day, month, year]
if ~ok1 then ERRORS_ADD, errors1, '(1) EXECUTE failed !'
if ~ARRAY_EQUAL(expected1, res1) then $
   ERRORS_ADD, errors1, '(1) bad numerical values D/M/Y !'
if ~ARRAY_EQUAL(expected2a, todaystring1) then   $
   ERRORS_ADD, errors1, '(1) bad string value !'
if (errors1 EQ 0) then MESSAGE, /continue, 'Case 1 : succesfully done'
;
MESSAGE, /continue, 'Case 2 : separator is a comma >>,<<'
errors2=0
;
ok2=EXECUTE('READS, thisisheader2, day, month, year, todaystring2')
res2=[day, month, year]
if ~ok2 then ERRORS_ADD, errors2, '(2) EXECUTE failed !'
if ~ARRAY_EQUAL(expected1, res2) then $
   ERRORS_ADD, errors2, '(2) bad numerical values D/M/Y !'
if ~ARRAY_EQUAL(expected2b, todaystring2) then  $
   ERRORS_ADD, errors2, '(2) bad string value !'
if (errors2 EQ 0) then MESSAGE, /continue, 'Case 2 : succesfully done'
;
errors=errors1+errors2
;
; special case for complex. Not yet ready for GDL
;; f1=complex(12,13) & f2=complex(18,19)
;; f=complexarr(2) & inarray=' ( 12,13) (18,19) ' & reads,inarray,f ; 2 complex written correctly: real and imaginary part readable
;; if total(atan(f-[f1,f2],/phas)) ne 0.0 then errors++
;; ;
;; ; special case for complex:
;; f1=complex(12,13) & f2=complex(18,0) & f3=complex(19,0)
;; f=complexarr(3) & inarray=' ( 12,13) 18 19' & reads,inarray,f ; 3 complex mixed writing: 1st real and imaginary part, rest is real only
;; if total(atan(f-[f1,f2,f3],/phas)) ne 0.0 then errors++
;; ;
;; f1=complex(12,0) & f2=complex(13,0)
;; f=complexarr(2) & inarray=' 12,13 ' & reads,inarray,f
;; if total(atan(f-[f1,f2],/phas)) ne 0.0 then errors++
;; 
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_READS_MIXED', errors, /status
ERRORS_CUMUL, cumul_errors, errors
;
if KEYWORD_SET(test) then STOP
;
end
;
; -------------------------------------
;
pro TEST_READS, help=help, verbose=verbose, no_exit=no_exit, test=test
;
if KEYWORD_SET(help) then begin
    print, 'pro '+'TEST_READS_'+', help=help, verbose=verbose, $'
    print, '                no_exit=no_exit, test=test'
    return
endif
;
errors=0
;
TEST_READS_ARRAYS, errors, verbose=verbose, test=test
;
print, 'AC 2020 may 18 not ready in read.cpp'
TEST_READS_COMPLEX, errors, verbose=verbose, test=test
errors=0
;
TEST_READS_STRING, errors, verbose=verbose, test=test
;
TEST_READS_MIXED, errors, verbose=verbose, test=test
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_READS', errors, /status
;
if (errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
