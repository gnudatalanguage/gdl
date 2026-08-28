;
; Original version was "test_issues1474.pro"
; 
; Main goal of this test, derived from bug report #1474
; is to expand the coverage of tests for FSTAT()
; Bug reports #2220 & #1394 
;
; Comments by AC 2026-08-26 :
; This code was tested in IDL 8.8 without problem
; This code is OK on Linux (x86_64) and also on OSX on M2 (arm)
; We create a file in each procedure and we removed then at the end of
; the procedures. Use /test to keep them !
;
; ---------------------------------------
; Modifications history :
;
; - 2026-08-26 : AC. rewriting and adding new tests from #2220 & #1394
;  Let's be clear : some tests are maybe redundant, but they exist ...
;
; ---------------------------------------
;
; This test is not so simple because we open 3 times simultaneously
; the same file (lun 1, 2, 3). Please be very cautious if changing
; this code (AC 26/08/27)
;
pro TEST_FSTAT_ISSUES1474, cumul_errors, debug=debug, test=test, verbose=verbose
;
nb_errors=0
if KEYWORD_SET(debug) then debug=1 else debug=0
;
CLOSE, 1, 2, 3
filename='test_fstat_ISSUES1474'
FILE_DELETE, filename, /allow_nonexistent
;
OPENW,1,filename
line1='01234567890123456789'
for i=0,10 do WRITEU,1,line1
FLUSH,1 ; very necessary as IDL's i/o are buffered
;
OPENR,2,filename
line2=BYTARR(20)
for i=0,10 do READU,2,line2
;
if debug then print,"before adding several bytes at end:"
f1=FSTAT(1)
expect1=[1,220,220]
res1=[f1.unit, f1.cur_ptr, f1.size]
if ~ARRAY_EQUAL(expect1, res1) then ERRORS_ADD, errors, 'before adding 1'
if debug then print, 'unit 1 (write) :', f1.unit, f1.cur_ptr, f1.size
before_w=f1.cur_ptr
;
f2=FSTAT(2)
expect2=[2,220,220]
res2=[f2.unit, f2.cur_ptr, f2.size]
if ~ARRAY_EQUAL(expect2, res2) then ERRORS_ADD, errors, 'before adding 2'
if debug then print, 'unit 2 (read)  :', f2.unit, f2.cur_ptr, f2.size
before_r=f2.cur_ptr
;
; -----------------------------------------
; append to file separately:
;
OPENU,3,filename
POINT_LUN,3,before_r
WRITEU,3,"ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ"
FLUSH,3
CLOSE,3
;
if debug then print,"after adding several bytes at end:"
expect1=[1,220,280]
f1=FSTAT(1)
res1=[f1.unit, f1.cur_ptr, f1.size]
if ~ARRAY_EQUAL(expect1, res1) then ERRORS_ADD, errors, 'after adding 1'
if debug then print, 'unit 1 (write) :', f1.unit, f1.cur_ptr, f1.size
after_w=f1.cur_ptr
;
f2=FSTAT(2)
expect2=[2,220,280]
res2=[f2.unit, f2.cur_ptr, f2.size]
if ~ARRAY_EQUAL(expect2, res2) then ERRORS_ADD, errors, 'after adding 2'
if debug then print, 'unit 2 (read)  :', f2.unit, f2.cur_ptr, f2.size
after_r=f2.cur_ptr
;
if (before_r ne after_r) then ERRORS_ADD, errors, '(before_R ne after_R)'
if (before_w ne after_w) then ERRORS_ADD, errors, '(before_W ne after_W)'
;
; other tests
;
if debug then print,"writing something at current offset "+STRTRIM(after_w)
line3=BYTARR(12)+65b ; a serie of 'A'
WRITEU,1,line3
FLUSH,1                         ; necessary...
f1 = FSTAT(1)
PRINT, f1.CUR_PTR, f1.SIZE
;
; reading independantly to check whether the content
; was changed as expected !
;
OPENR, 3, filename
POINT_LUN, 3, 220
a3 = 0b
READU, 3, a3
if (a3 ne 65) then ERRORS_ADD, nb_errors, 'Bad value for independent read :(' 
CLOSE, 3
;
a=0b
READU,2,a
if (a ne 65) then begin
   ERRORS_ADD, nb_errors, 'Known problem with sync()'
   MESSAGE, /info, "Known problem with sync() in defaut_io.cpp, FIXME"
endif
;
CLOSE, 1, 2
FREE_LUN, 1, 2
;
if KEYWORD_SET(test) then STOP
FILE_DELETE, filename
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, ROUTINE_NAME(), nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_SET(test) then STOP
;
end
;
; ----------------------------------
; this is related to bug report #1981 PR #1982
;
function PRINTSIZE, fname, test=test, debug=debug
OPENR,dev,fname,/get_lun
s=FSTAT(dev)
size=s.size
if KEYWORD_SET(debug) then HELP,size
if KEYWORD_SET(test) then STOP
CLOSE, dev
FREE_LUN, dev
return,size
end
;
pro TEST_FSTAT_PRINTSIZE, cumul_errors, test=test, verbose=verbose
;
nb_errors=0 
;
filename = 'test_fstat_PRINTSIZE.tmp'
FILE_DELETE, filename, /allow_nonexistent
OPENW, lun, filename, /get_lun
d=1.0
WRITEU, lun, d
CLOSE, lun
FREE_LUN, lun
;
if PRINTSIZE(filename) LT 0 then ERRORS_ADD, nb_errors, 'Bad SIZE !'
;
if KEYWORD_SET(test) then STOP
FILE_DELETE, filename
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, ROUTINE_NAME(), nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_SET(test) then STOP
;
end
;
; ----------------------------------
;
pro TEST_FSTAT_OPENW, cumul_errors, test=test, verbose=verbose
;
nb_errors=0 
;
filename = 'test_fstat_OPENW.tmp'
FILE_DELETE, filename, /allow_nonexistent
;  
OPENW, unit, filename, /get_lun
PRINTF, unit, 'one'
;
s = FSTAT(unit)
;
if s.cur_ptr ne 4 then ERRORS_ADD, nb_errors, 'FAIL: CUR_PTR <> 4 !'
;
PRINTF, unit, 'two'
CLOSE, unit
FREE_LUN, unit
;
; Reading back, is the content OK ?
;
OPENR, unit, filename, /get_lun
text = ''
READF, unit, text
if text ne 'one' then ERRORS_ADD, nb_errors, 'Bad line 1 (one)'
READF, unit, text
if text ne 'two' then ERRORS_ADD, nb_errors, 'Bad line 2 (two)'
CLOSE, unit
FREE_LUN, unit
;
; to be able to look at the file, if needed ...
if KEYWORD_SET(test) then STOP
FILE_DELETE, filename
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, ROUTINE_NAME(), nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_SET(test) then STOP

end
;
; ----------------------------------
;
pro TEST_FSTAT_EOF_POSITION, cumul_errors, test=test, verbose=verbose
;
nb_errors=0
;
; write a file, and close it
;
filename = 'test_fstat_EOF_POSITION.tmp'
FILE_DELETE, filename, /allow_nonexistent
;
OPENW, unit, filename, /get_lun
data = LONARR(100)
WRITEU, unit, data
FREE_LUN, unit
;
; read back
;
OPENR, unit, filename, /get_lun
data = LONARR(100)
READU, unit, data
;
s = FSTAT(unit)
if s.cur_ptr ne 400 then ERRORS_ADD, nb_errors, 'FAIL: CUR_PTR <> 400 !'
;
FREE_LUN, unit
; to be able to look at the file, if needed ...
if KEYWORD_SET(test) then STOP
FILE_DELETE, filename
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, ROUTINE_NAME(), nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_SET(test) then STOP
;
end
;
; ----------------------------------
;
pro TEST_FSTAT_PRESERVES_OUTPUT, cumul_errors, test=test, verbose=verbose
;
nb_errors=0 
;
filename = 'test_fstat_PRESERVES_OUTPUT.tmp'
FILE_DELETE, filename, /ALLOW_NONEXISTENT
;
line1='Print output'
line2='No output anymore!'
;
OPENW, unit, filename, /get_lun
PRINTF, unit, line1
s = FSTAT(unit)
if s.cur_ptr ne 13 then begin
   ERRORS_ADD, nb_errors, 'FAIL: CUR_PTR <> 12 !'
   MESSAGE, /cont, 'FSTAT changed CUR_PTR'
endif
;
; adding a new line
PRINTF, unit, line2
FREE_LUN, unit
;
; read back
;
OPENR, unit, filename, /get_lun
line = ''
READF, unit, line
if line ne line1 then ERRORS_ADD, nb_errors, 'First line is missing'
;
; see bug #2220 : a call to FSTAT() after writing first line
; close the file before the end
; the second line 
;
line = ''
READF, unit, line
if line ne line2 then ERRORS_ADD, nb_errors, 'FSTAT broke subsequent output'
;
FREE_LUN, unit
; to be able to look at the file, if needed ...
if KEYWORD_SET(test) then STOP
FILE_DELETE, filename
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, ROUTINE_NAME(), nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_SET(test) then STOP
;
end
;
; ----------------------------------
;
pro TEST_FSTAT, help=help, test=test, verbose=verbose, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_FSTAT, help=help, test=test, verbose=verbose, no_exit=no_exit'
   return
endif 
;
if !version.os_family eq 'Windows' then begin
   txt='We don''t know the status of FSTAT() on MSwin'
   MESSAGE, /continue, txt+' Please report any feedback/bug report'
   EXIT, status=77
endif
;
cumul_errors=0
;
TEST_FSTAT_ISSUES1474, cumul_errors, test=test
TEST_FSTAT_OPENW, cumul_errors, test=test
TEST_FSTAT_PRINTSIZE, cumul_errors, test=test
TEST_FSTAT_EOF_POSITION, cumul_errors, test=test
TEST_FSTAT_PRESERVES_OUTPUT, cumul_errors, test=test
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_FSTAT', cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
