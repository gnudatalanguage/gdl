;
; Alain C., 4 Oct. 2015
;
; READS should work with space or comma as separators.
;
; First Bug found thanks to the MIRIM simulator
; Major problem on Complex input see #701
;
; Please notice that, yes, maybe some cases we test here may seems to be redundant,
; but by the way we process now the input, we need to carrefully check
; we process well all space, comma, braces ...
;
; Yes, all the cases here give same results with IDL 8.4
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
function ICI_ARRAY_EQUAL, x, y, debug=debug
if KEYWORD_SET(debug) then begin
   print, 'expected     :', x
   print, 'what we have :', y
endif
return, ARRAY_EQUAL(x,y)
end
; -------------------------------------
;
pro TEST_READS_BASIC, cumul_errors, decimal=decimal, complex=complex, basic=basic, $
                      verbose=verbose, test=test, debug=debug
;
errors=0
;
if KEYWORD_SET(decimal) then begin
   inputs=['12.1    13.4',' 12.1, 13.4, 5, 6'' 12.1,13.4   ',' 12.1,13.4,   ']
endif else begin
   inputs=['12 13','  12 13','12,13.', '12,13.   ',' 12,13.   '] 
   inputs=[inputs,['12 13  6   7','  12 13 6 7','12,13.,6.,7.', '12,13.   6 7',' 12,13.  ,6,7 '] ]
endelse
;
types=[1,2,3,4,5,6,9]
;
if KEYWORD_SET(complex) then begin
   MESSAGE, /Continue, 'Only Complex types tested'
   types=[6,9]
endif
if KEYWORD_SET(basic) then begin
   MESSAGE, /Continue, 'Only basic types tested (B, I, L, F, D)'
   types=[1,2,3,4,5]
endif
;
exp_B=[12b, 13b]
exp_I=[12, 13]
exp_L=[12L, 13]
;
if KEYWORD_SET(decimal) then exp_F=[12.1, 13.4] else exp_F=[12., 13.]
exp_D=DOUBLE(exp_F)
exp_C=Complexarr(2)+exp_F
exp_DC=DComplexarr(2)+exp_F
;
eps=1.e-6
;
for ii=0, N_ELEMENTS(inputs)-1 do begin
   inarray=inputs[ii]
   for jj=0, N_ELEMENTS(types)-1 do begin
      res=INDGEN(2, type=types[jj]) 
      READS, inarray, res
      if types[jj] EQ 1 then if ~ICI_ARRAY_EQUAL(exp_B, res, debug=debug) then $
         ERRORS_ADD, errors, 'basic BYTE bad values !'
      if types[jj] EQ 2 then if ~ICI_ARRAY_EQUAL(exp_I, res, debug=debug) then $
         ERRORS_ADD, errors, 'basic INT bad values !'
      if types[jj] EQ 3 then if ~ICI_ARRAY_EQUAL(exp_L, res, debug=debug) then $
         ERRORS_ADD, errors, 'basic LONG bad values !'
      if types[jj] EQ 4 then if ~ICI_ARRAY_EQUAL(exp_F, res, debug=debug) then $
         if (TOTAL(ABS(exp_F-res)) GT eps) then ERRORS_ADD, errors, 'basic FLOAT bad values !'
      if types[jj] EQ 5 then if ~ICI_ARRAY_EQUAL(exp_D, res, debug=debug) then $
         if (TOTAL(ABS(exp_F-res)) GT eps) then ERRORS_ADD, errors, 'basic DOUBLE bad values !'
      if types[jj] EQ 6 then if ~ICI_ARRAY_EQUAL(exp_C, res, debug=debug) then $
         if (TOTAL(ABS(exp_F-res)) GT eps) then ERRORS_ADD, errors, 'basic COMPLEX bad values !'
      if types[jj] EQ 9 then if ~ICI_ARRAY_EQUAL(exp_DC, res, debug=debug) then $
         if (TOTAL(ABS(exp_F-res)) GT eps) then ERRORS_ADD, errors, 'basic DOUBLE COMPLEX bad values !'
   endfor
endfor
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_READS_BASIC', errors, /status
ERRORS_CUMUL, cumul_errors, errors
;
if KEYWORD_SET(test) then STOP
;
end
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
;
; -------------------------------------
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
expected2=expected1 
expected1s=' This is the date of my file'
expected2s=',This is the date of my file'
;
day=0
month=0
year=0
todaystring1=''
todaystring2=''
;
if KEYWORD_SET(verbose) then $
   MESSAGE, /continue, 'Case 1 : separator is a space >> <<'
errors1=0
;
ok1=EXECUTE('READS, thisisheader, day, month, year, todaystring1')
res1=[day, month, year]
if ~ok1 then ERRORS_ADD, errors1, '(1) EXECUTE failed !'
if ~ARRAY_EQUAL(expected1, res1) then $
   ERRORS_ADD, errors1, '(1) bad numerical values D/M/Y !'
if ~ARRAY_EQUAL(expected1s, todaystring1) then   $
   ERRORS_ADD, errors1, '(1) bad string value !'
if KEYWORD_SET(verbose) then $
   if (errors1 EQ 0) then MESSAGE, /continue, 'Case 1 : succesfully done'
;
if KEYWORD_SET(verbose) then $
   MESSAGE, /continue, 'Case 2 : separator is a comma >>,<<'
errors2=0
;
ok2=EXECUTE('READS, thisisheader2, day, month, year, todaystring2')
res2=[day, month, year]
if ~ok2 then ERRORS_ADD, errors2, '(2) EXECUTE failed !'
if ~ARRAY_EQUAL(expected2, res2) then $
   ERRORS_ADD, errors2, '(2) bad numerical values D/M/Y !'
if ~ARRAY_EQUAL(expected2s, todaystring2) then  $
   ERRORS_ADD, errors2, '(2) bad string value !'
;
if KEYWORD_SET(verbose) then $
   if (errors2 EQ 0) then MESSAGE, /continue, 'Case 2 : succesfully done'
;
errors=errors1+errors2
;
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
; as mentionned in https://github.com/gnudatalanguage/gdl/issues/701
;
pro TEST_READS_COMPLEX2, cumul_errors, verbose=verbose, test=test
;
errors=0
;
expected=[complex(12,13),complex(18,19)]
;
; Du to the way we process the input, we may fail due to extra space(s)
;
f=COMPLEXARR(2) & inarray=' ( 12,13) (18,19) '
READS, inarray, f
if ~ARRAY_EQUAL(expected, f) then ERRORS_ADD, errors, 'case 1 : '+inarray
;
f=COMPLEXARR(2) & inarray='( 12,13) (18,19) '
READS, inarray, f
if ~ARRAY_EQUAL(expected, f) then ERRORS_ADD, errors, 'case 1 : '+inarray
;
f=COMPLEXARR(2) & inarray='(12,13) (18,19) '
READS, inarray, f
if ~ARRAY_EQUAL(expected, f) then ERRORS_ADD, errors, 'case 1 : '+inarray
;
f=COMPLEXARR(2) & inarray=' ( 12 13) (18,19) '
READS, inarray, f
if ~ARRAY_EQUAL(expected, f) then ERRORS_ADD, errors, 'case 1 : '+inarray
;
f=COMPLEXARR(2) & inarray=' (12 13) (18,19) '
READS, inarray, f
if ~ARRAY_EQUAL(expected, f) then ERRORS_ADD, errors, 'case 1 : '+inarray
;
f=COMPLEXARR(2) & inarray='(12 13) (18,19) '
READS, inarray, f
if ~ARRAY_EQUAL(expected, f) then ERRORS_ADD, errors, 'case 1 : '+inarray
;
f=COMPLEXARR(2) & inarray='(12 13 ) (18,19) '
READS, inarray, f
if ~ARRAY_EQUAL(expected, f) then ERRORS_ADD, errors, 'case 1 : '+inarray
;
f=COMPLEXARR(2) & inarray=' (12 13 ) ( 18,19) '
READS, inarray, f
if ~ARRAY_EQUAL(expected, f) then ERRORS_ADD, errors, 'case 1 : '+inarray
;
; ------ some complex without imaginary parts ...
;
expected=[COMPLEX(12,13),COMPLEX(18,0),COMPLEX(19,0)]
;
f=COMPLEXARR(3) & inarray=' ( 12,13) 18 19'
READS,inarray,f 
if ~ARRAY_EQUAL(expected, f) then ERRORS_ADD, errors, 'case 2 : '+inarray
;
f=COMPLEXARR(3) & inarray=' ( 12,13) 18, 19'
READS,inarray,f 
if ~ARRAY_EQUAL(expected, f) then ERRORS_ADD, errors, 'case 2 : '+inarray
;
f=COMPLEXARR(3) & inarray=' ( 12,13   ) 18 19'
READS,inarray,f 
if ~ARRAY_EQUAL(expected, f) then ERRORS_ADD, errors, 'case 2 : '+inarray
;
; ------ all complex without imaginary parts ...
;
expected=[COMPLEX(12,0),COMPLEX(13,0)]
;
f=COMPLEXARR(2) & inarray='12,13'
READS,inarray,f 
if ~ARRAY_EQUAL(expected, f) then ERRORS_ADD, errors, 'case 3 : '+inarray
;
f=COMPLEXARR(2) & inarray='12,13 ,,,,'
READS,inarray,f 
if ~ARRAY_EQUAL(expected, f) then ERRORS_ADD, errors, 'case 3 : '+inarray
;
f=COMPLEXARR(2) & inarray=' 12, 13'
READS,inarray,f 
if ~ARRAY_EQUAL(expected, f) then ERRORS_ADD, errors, 'case 3 : '+inarray
;
f=COMPLEXARR(2) & inarray=' ( 12,) 13'
READS,inarray,f 
if ~ARRAY_EQUAL(expected, f) then ERRORS_ADD, errors, 'case 3 : '+inarray
;
f=COMPLEXARR(2) & inarray=' ( 12) 13'
READS,inarray,f 
if ~ARRAY_EQUAL(expected, f) then ERRORS_ADD, errors, 'case 3 : '+inarray
;
f=COMPLEXARR(2) & inarray='  12 13'
READS,inarray,f 
if ~ARRAY_EQUAL(expected, f) then ERRORS_ADD, errors, 'case 3 : '+inarray
;
f=COMPLEXARR(2) & inarray='  (12) ( 13 )'
READS,inarray,f 
if ~ARRAY_EQUAL(expected, f) then ERRORS_ADD, errors, 'case 3 : '+inarray
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_READS_COMPLEX2', errors, /status
ERRORS_CUMUL, cumul_errors, errors
;
if KEYWORD_SET(test) then STOP
;
end
;
; -------------------------------------
;
pro TEST_READS_MIXING_TYPES, cumul_errors, verbose=verbose, test=test, debug=debug
;
errors=0
;
expected_c=[COMPLEX(12,13),COMPLEX(18,19),COMPLEX(-1)]
expected_f=[6.,-5.]
;
c=COMPLEXARR(3) & f=FLTARR(2) & inarray=' ( 12,13   ) (18 19) (-1 0) 6, -5 '
READS,inarray, c, f 
if ~ICI_ARRAY_EQUAL(expected_c, c, debug=debug) then ERRORS_ADD, errors, 'case mix C : '+inarray
if ~ICI_ARRAY_EQUAL(expected_f, f, debug=debug) then ERRORS_ADD, errors, 'case mix F : '+inarray
;
c=COMPLEXARR(3) & f=FLTARR(2) & inarray=' ( 12,13   ) (18 19) -1 6, -5 '
READS,inarray, c, f 
if ~ICI_ARRAY_EQUAL(expected_c, c, debug=debug) then ERRORS_ADD, errors, 'case mix C : '+inarray
if ~ICI_ARRAY_EQUAL(expected_f, f, debug=debug) then ERRORS_ADD, errors, 'case mix F : '+inarray
;
c=COMPLEXARR(3) & f=FLTARR(2) & inarray=' (12 13) (18, 19) -1, 6, -5 '
READS,inarray, c, f 
if ~ICI_ARRAY_EQUAL(expected_c, c, debug=debug) then ERRORS_ADD, errors, 'case mix C : '+inarray
if ~ICI_ARRAY_EQUAL(expected_f, f, debug=debug) then ERRORS_ADD, errors, 'case mix F : '+inarray
;
c=COMPLEXARR(3) & f=FLTARR(2) & inarray=' (12 13) (18, 19) -1, 6  -5'
READS,inarray, c, f 
if ~ICI_ARRAY_EQUAL(expected_c, c, debug=debug) then ERRORS_ADD, errors, 'case mix C : '+inarray
if ~ICI_ARRAY_EQUAL(expected_f, f, debug=debug) then ERRORS_ADD, errors, 'case mix F : '+inarray
;
c=COMPLEXARR(3) & f=FLTARR(2) & inarray=' (12 13) (18, 19) -1, 6  -5, 56, 7'
READS,inarray, c, f 
if ~ICI_ARRAY_EQUAL(expected_c, c, debug=debug) then ERRORS_ADD, errors, 'case mix C : '+inarray
if ~ICI_ARRAY_EQUAL(expected_f, f, debug=debug) then ERRORS_ADD, errors, 'case mix F : '+inarray


;f=complexarr(3) & a=intarr(2) & inarray=' (  12,),,, 14,13,5,7' & reads,inarray,f,a & print, f, a



; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_READS_MIXING_TYPES', errors, /status
ERRORS_CUMUL, cumul_errors, errors
;
if KEYWORD_SET(test) then STOP
;
end
;
; -------------------------------------
;
pro TEST_READS_STRUCT, cumul_errors, help=help, verbose=verbose, no_exit=no_exit, test=test
errors=0
;
  rcd = {index: 0l, $
         catalogue: 0l, $
         date: 0.d}

expected={index: 9l, $
         catalogue: 145l, $
         date: 97843.973284d}

;
data='9 145 97843.973284'
READS,data, rcd
; problem #1970 was that even the first structure read was not filled correctly.
if  ~ICI_ARRAY_EQUAL(expected.date, rcd.date, debug=debug) then ERRORS_ADD, errors, 'case read structure'
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_READS_STRUCT', errors, /status
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
TEST_READS_BASIC, errors, verbose=verbose, test=test
TEST_READS_BASIC, errors, /decimal, verbose=verbose, test=test
;
TEST_READS_ARRAYS, errors, verbose=verbose, test=test
;
TEST_READS_COMPLEX, errors, verbose=verbose, test=test
TEST_READS_COMPLEX2, errors, verbose=verbose, test=test
;
TEST_READS_STRING, errors, verbose=verbose, test=test
;
TEST_READS_MIXED, errors, verbose=verbose, test=test
;
TEST_READS_MIXING_TYPES, errors, verbose=verbose, test=test
TEST_READS_STRUCT, errors, verbose=verbose, test=test
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
