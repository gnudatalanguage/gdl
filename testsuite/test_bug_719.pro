;
; Alain C. 2017-03-16 during MIRI test at JPL ! 
;
; this bug was found by chance when trying to finish TEST_DIAG_MATRIX
;
; as long as this bug is not solved, TEST_DIAG_MATRIX will be wrong.
;
; Two situations : 
;   - on Ubuntu (14.04, 15.10, 16.04), always crash
;   - On OSX 11, CentOS 5.11, Debian, sporadically crash 
;
pro TEST_BUG_719_FINDGEN, cumul_errors, test=test, verbose=verbose
;
nb_errors=0
;
m3x4=INDGEN(3,4)
;
ii=[2]
jj=[0]
;
expected=2
;
for loop=0, 9 do begin
   result=m3x4(ii, jj)
   if ~ARRAY_EQUAL(result,expected) then $
      ADD_ERROR, nb_errors, 'case 3x4 Findgen + loop : '+STRING(loop)
endfor
;
; the same, using EXECUTE !
;
exe=EXECUTE('result_exe=m3x4(ii, jj)')
if ~ARRAY_EQUAL(result_exe,expected) then $
   ADD_ERROR, nb_errors, 'case 3x4 Findgen EXECUTE'
;
BANNER_FOR_TESTSUITE, 'TEST_BUG_719_FINDGEN', nb_errors, /short, verb=verbose
;
ERRORS_CUMUL, cumul_errors, nb_errors
;
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------
;
pro TEST_BUG_719_REFORM, cumul_errors, test=test, verbose=verbose
;
nb_errors=0
;
input=REFORM(INDGEN(12),3,4)
xx=[2]
yy=[0]
resu=REFORM(input[xx,yy])  
;
expected=2
;
if ~ARRAY_EQUAL(resu,expected) then ADD_ERROR, nb_errors, 'case 3x4 Reform'
;
; the same, using EXECUTE !
;
exe=EXECUTE('result_exe=REFORM(input[xx,yy])')
if ~ARRAY_EQUAL(result_exe,expected) then $
   ADD_ERROR, nb_errors, 'case 3x4 Findgen EXECUTE'
;
BANNER_FOR_TESTSUITE, 'TEST_BUG_719_REFORM', nb_errors, /short, verb=verbose
;
ERRORS_CUMUL, cumul_errors, nb_errors
;
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------
;
pro TEST_BUG_719, help=help, verbose=verbose, no_exit=no_exit, test=test
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_DIAG_MATRIX, help=help, verbose=verbose, $'
   print, '                      no_exit=no_exit, test=test'
   return
endif
;
TEST_BUG_719_FINDGEN, nb_errors, test=test, verbose=verbose
;
TEST_BUG_719_REFORM, nb_errors, test=test, verbose=verbose
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_BUG_719', nb_errors
;
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end

