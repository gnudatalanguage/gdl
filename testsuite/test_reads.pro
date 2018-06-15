;
; Alain C., 4 Oct. 2015
;
; READS should work with space or comma as separators.
;
; First Bug found thanks to the MIRIM simulator
;
; -------------------------------------
;
pro TEST_READS_STRING, cumul_errors, verbose=verbose, test=test
;
functionname='TEST_READS_STRING'
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
BANNER_FOR_TESTSUITE, functionname, errors, /status
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
functionname='TEST_READS_MIXED'
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
; ----- final ----
;
BANNER_FOR_TESTSUITE, functionname, errors, /status
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
functionname='TEST_READS'
;
if KEYWORD_SET(help) then begin
    print, 'pro '+functionname+', help=help, verbose=verbose, $'
    print, '                no_exit=no_exit, test=test'
    return
endif
;
errors=0
;
TEST_READS_STRING, errors, verbose=verbose, test=test
;
TEST_READS_MIXED, errors, verbose=verbose, test=test
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, functionname, errors
;
if (errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
