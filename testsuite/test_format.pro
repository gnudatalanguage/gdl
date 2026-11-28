;
; AC 2024/04/16
;
; some tricks we had in format ...
; These ones can be tested without writing files
;
; ------------------------------------
; see issue #2237
pro TEST_FORMAT_INSIDE_ARRAY, cumul_errors, test=test, verbose=verbose
errors=0
;
expected1='Sep+0001'
data1=string(['Sep', '1'], FORMAT='(A,I+05)')
if (data1 NE expected1) then ERRORS_ADD, errors, 'inside_array 1'
expected2='OCT,10/02!!!!1.5707963E+08 +00314159265:'
data2=string(FORMAT='(C(CMOA,",",CMOI02,"/",CDI02),"!!!!",G13.8,X,I+012,":")', replicate(dcomplex(!DPI,!DPI/2)*1E8,2))
if n_elements(data2) ne 2 then begin
   ERRORS_ADD, errors, 'inside_array 2'
   ERRORS_CUMUL, cumul_errors, errors
   if KEYWORD_SET(test) then STOP
   return
endif

if (data2[0] NE expected2) then ERRORS_ADD, errors, 'inside_array 3'
expected3='APR,04/29'
if (data2[1] NE expected3) then ERRORS_ADD, errors, 'inside_array 4'
BANNER_FOR_TESTSUITE, 'TEST_FORMAT_NEWLINE', errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
end
; see issue # & PR #1799
pro TEST_FORMAT_NEWLINE, cumul_errors, test=test, verbose=verbose
;
errors=0
;s1=string(format='($,I0)', 123)+string(format='(I0)', 456)
;s2=string(format='(I0,$)', 123)+string(format='(I0)', 456)

print, 'not finishied :('

; --------------
;
BANNER_FOR_TESTSUITE, 'TEST_FORMAT_NEWLINE', errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; -------------------------------------------------
; simple debug print when needed ...
pro MODE_DEBUG, exp, val, valname, debug=debug, test=test
if KEYWORD_SET(debug) then begin
   svalname=STRING(valname, format='(A30)')+': '
   print, STRING('expected', format='(A30)')+': ', '>>'+exp+'<<'
   print, svalname,  '>>'+val+'<<'
endif
if KEYWORD_SET(test) then STOP
end
; ------------------------------------
; see dicussion in PR #1799
pro TEST_FORMAT_C, cumul_errors, test=test, verbose=verbose, debug=debug
;
errors=0
;
expected1='Sat Dec 25 01:19:12 2010'
data1=STRING(2.455555555D6,format='(c())')
if (data1 NE expected1) then ERRORS_ADD, errors, 'date 1 (Sat/Sun)'
MODE_DEBUG, expected1, data1, '(c())', debug=debug, test=test
;
expected2='Tue Apr 16 12:00:00 2024'
data2=STRING(2460417.,format='(c())')
if (data2 NE expected2) then ERRORS_ADD, errors, 'date 2 (Sat/Sun)'
MODE_DEBUG, expected2, data2, '(c())', debug=debug, test=test
;
; same expected but literal format ...
format_litteral='(c(CDwA,X,CMoA,X,CDI2.2,X,CHI2.2,":",CMI2.2,":",CSI2.2, CYI5))'
data2bis=STRING(2460417.,format=format_litteral)
if (data2bis NE expected2) then ERRORS_ADD, errors, 'date 2 bis (litteral)'
MODE_DEBUG, expected2, data2bis, 'c() literal', debug=debug, test=test
;
; day/month
;
format3='(C(CMOI,"/",CDI))'
expected3=' 4/16'
data3=STRING(2460417.,format=format3)
if (data3 NE expected3) then ERRORS_ADD, errors, 'date 3 (CMOI/CDI)'
MODE_DEBUG, expected3, data3, format3, debug=debug, test=test
;
; hour/min/sec
format4='(C(CHI2.2,":",CMI2.2,":",CSF05.2))'
expected4='12:00:00.00'
data4=STRING(2460417.,format=format4)
if (data4 NE expected4) then ERRORS_ADD, errors, 'date 3 (h:m:s)'
MODE_DEBUG, expected4, data4, format4, debug=debug, test=test
;
; --------------
;
BANNER_FOR_TESTSUITE, 'TEST_FORMAT_C', errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------------------
;
pro TEST_FORMAT_LOOP_ERROR, errors, fmt, values, input=input, unit=unit
catch, err
if err ne 0 then begin
   msg = !error_state.msg
   catch, /cancel
   if strpos(msg, 'Infinite format loop detected.') lt 0 then $
      ERRORS_ADD, errors, 'Unexpected error for '+fmt+' '+msg
   return
endif
if keyword_set(input) then begin
   result = values
   reads, ['123','abc'], result, format=fmt
endif else if n_elements(unit) ne 0 then begin
   printf, unit, format=fmt, values
endif else print, format=fmt, values
ERRORS_ADD, errors, 'Expected an infinite format loop error for '+fmt
end
;
pro TEST_FORMAT_LOOP_FILE, errors
file = 'test_format_loop_output.dat'
get_lun, u
foreach compress, [0,1] do begin
   openw, u, file, compress=compress
   TEST_FORMAT_LOOP_ERROR, errors, '("abc")', 1, unit=u
   TEST_FORMAT_LOOP_ERROR, errors, '(I0,("abc"))', [1,2], unit=u
   close, u
   openr, u, file, compress=compress, /delete
   if ~eof(u) then ERRORS_ADD, errors, 'Failed PRINTF wrote data to an empty file'
   close, u

   foreach fmt, ['(I0,("abc"))', '(I0,/,("abc"))'] do begin
      openw, u, file, compress=compress
      printf, u, format='(A)', 'before'
      TEST_FORMAT_LOOP_ERROR, errors, fmt, [1,2], unit=u
      printf, u, format='(A)', 'after'
      close, u
      openr, u, file, compress=compress, /delete
      lines = strarr(2)
      readf, u, lines
      if ~array_equal(lines, ['before','after']) || ~eof(u) then $
         ERRORS_ADD, errors, 'Failed PRINTF changed surrounding records for '+fmt+' with COMPRESS='+strtrim(compress,2)
      close, u
   endforeach
endforeach
free_lun, u
end
;
pro TEST_FORMAT, help=help, verbose=verbose, test=test, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
   print, 'TEST_OBJ_FORMAT, help=help, verbose=verbose, $'
   print, '                 test=test, no_exit=no_exit'
   return
endif
;
cumul_errors=0
;
TEST_FORMAT_NEWLINE, cumul_errors, test=test, verbose=verbose
;
TEST_FORMAT_C, cumul_errors, test=test, verbose=verbose
;
TEST_FORMAT_LOOP_ERROR, cumul_errors, '("abc")', 1
; The first pass consumes a value. The repeated group contains only a literal.
TEST_FORMAT_LOOP_ERROR, cumul_errors, '(I0,("abc"))', [1,2]
TEST_FORMAT_LOOP_ERROR, cumul_errors, '("abc")', 1, /input
TEST_FORMAT_LOOP_ERROR, cumul_errors, '(I1,("abc"))', [1,2], /input
TEST_FORMAT_LOOP_FILE, cumul_errors
lines = string([1,2,3], format='("v",I0)')
if ~array_equal(lines, ['v1','v2','v3']) then ERRORS_ADD, cumul_errors, 'Wrong output on format reversion'
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_FORMAT', cumul_errors, short=short
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end

