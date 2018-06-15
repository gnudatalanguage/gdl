;
; AC 12 Mars 2008 (initial version not used in the regression tests)
; testing the BYTE conversion for different types
;
; AC 2017-03-10 at JPL. This bug still not solved ?!
; Time to move this code into the regression tests list !!!
;
; As written in IDL documentation, BYTE is a 256 modulo operation !
; https://www.harrisgeospatial.com/docs/type_conversion_function.html
; BYTE(-1) --> 255b (Bytes are modulo 256)
;
; ------------------------
; AC: I suspect this code might be removed : what does it provide
; we do not test below ? TBC ! (when main bug will be solved !)
;
pro TEST_BYTE_CONV_POWER, negative=negative, help=help
;
if KEYWORD_SET(help) then begin
  print, 'pro TEST_BYTE_CONV_POWER, negative=negative, help=help'
  return
endif

array=[255,256., 257, 258]

if KEYWORD_SET(negative) then sign=-1 else sign=1

print, 'imput data : ', array

for ii=1,4 do begin
   print, 'power ', ii
   print, '(integer --> byte) ', BYTE(sign*(FIX(array)^ii))
   print, '(  float --> byte) ', BYTE(sign*(array^ii))
   print, '( double --> byte) ', BYTE(sign*(DOUBLE(array)^ii))
   print, '(complex --> byte) ', BYTE(sign*(COMPLEX(array)^ii))
   print, '(   long --> byte) ', BYTE(sign*(LONG(array)^4))
   print, '(  ulong --> byte) ', BYTE(sign*(ULONG(array)^4))
endfor
;
end
; ------------------------
; https://sourceforge.net/p/gnudatalanguage/bugs/586/
; just to prevent any future regression
;
pro TEST_BYTE_BUG_586, Cumul_errors, test=test, verbose=verbose
;
nb_errors=0
;
nb_fields=5
;
; putting a BYTE into a STRING array
;
expected_lenghts=REPLICATE(0,5)
expected_lenghts[1]=4
expected_strings=REPLICATE('',5)
expected_strings[1]='  32'
;
v=32b
vs=replicate('',5)
vs[1]=v
;
if ARRAY_EQUAL(expected_lenghts, STRLEN(vs)) NE 1 then begin
   ERRORS_ADD, nb_errors, 'bad lenght of converted output (BYTE)'
endif
;
if ARRAY_EQUAL(expected_strings, vs) NE 1 then begin
   ERRORS_ADD, nb_errors, 'bad content of converted output (BYTE)'
endif
;
; the same but for LONG ...
;
expected_lenghts[1]=12
expected_strings[1]='          22'
;
v=22L
vs[1]=v
;
if ~ARRAY_EQUAL(expected_lenghts, STRLEN(vs)) then begin
   ERRORS_ADD, nb_errors, 'bad lenght of converted output (LONG)'
endif
;
; ----- final ----
;
if ~ARRAY_EQUAL(expected_strings, vs) then begin
   ERRORS_ADD, nb_errors, 'bad content of converted output (LONG)'
endif
;
BANNER_FOR_TESTSUITE, 'TEST_BYTE_BUG_586', nb_errors, /status
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------
;
pro TEST_BYTE_BASIC_32768, cumul_errors, debug=debug, negative=negative, $
                           test=test, verbose=verbose
;
nb_errors=0
;
if KEYWORD_SET(debug) then print, 'loop, expected, res. as Long, res. as Float'
;
for jj=32765L, 32770 do begin
   ;;
   input=jj
   if KEYWORD_SET(negative) then input=-input
   ;;
   expected= input mod 256
   ;;
   if (KEYWORD_SET(negative) AND (expected LT 0)) then expected=expected+256
   ;;
   ;; this one does not fail in GDL now
   input_as_long=LONG(input)
   result_as_long=BYTE(input_as_long)
   if ARRAY_EQUAL(expected, result_as_long) NE 1 then begin
      ERRORS_ADD, nb_errors, 'input as Long for : '+STRING(input)
   endif
   ;;
   ;; this one does fail in GDL now
   input_as_float=FLOAT(input)
   result_as_float=BYTE(input_as_float)
   if ARRAY_EQUAL(expected, result_as_float) NE 1 then begin
      ERRORS_ADD, nb_errors, 'input as Long for : '+STRING(input)
   endif
   ;;
   if KEYWORD_SET(debug) then begin
      print, format='(A7, 4i7)', 'loop : ', input, expected, result_as_long, result_as_float
   endif
endfor
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_BYTE_BASIC_32768', nb_errors, /status
;
if ~KEYWORD_SET(debug) AND (nb_errors GT 0) then begin
   message='For details, run it again with /DEBUG !!'
   BANNER_FOR_TESTSUITE, 'TEST_BYTE_BASIC_32768', message
endif
;
ERRORS_CUMUL, cumul_errors, nb_errors
;
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------
;
; testing all the numerical types (ISA(/number)) from 1 to 15 ...
;
pro TEST_BYTE_WITH_LOOP, cumul_errors, max_loop=max_loop, $
                         test=test, verbose=verbose
;
nb_errors=0
stop_report=0 ; in GDL, too much errors now ...
;
; it is enough today in GDL to have trouble with 25 ...
if ~KEYWORD_SET(max_loop) then max_loop=25
max_loop=ROUND(max_loop)
max_loop=max_loop[0]
if max_loop LT 0 then MESSAGE, 'max_loop must be positive ...'
;
expected=45b
factor=256.*14 ;; (can fail in range 25 for GDL now)
;
for iloop=0, max_loop do begin
   for itype=1, 15 do begin   
      if (itype NE 8) then begin
         if ISA(MAKE_ARRAY(1, type=itype),/number) then begin
            base=INDGEN(1, type=itype)
            input=(factor+base[0])*iloop+expected[0]
            result=BYTE(input)
            ;;  print, base[0], expected, result, input
            if ARRAY_EQUAL(expected, result) NE 1 then begin
               if (nb_errors GT 15) then begin
                  ERRORS_ADD, nb_errors, 'loop :'+STRING(iloop)+', TYPE : '+STRING(itype)
               endif else begin
                  if (stop_report EQ 0) then begin
                     BANNER_FOR_TESTSUITE, 'TEST_BYTE_WITH_LOOP', /wide, $
                                           'We stop reporting errors (too much)'
                     stop_report=1
                  endif
               endelse
            endif
         endif
      endif
   endfor
endfor
;
BANNER_FOR_TESTSUITE, 'TEST_BYTE_WITH_LOOP', nb_errors, /short, verb=verbose
;
ERRORS_CUMUL, cumul_errors, nb_errors
;
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------
;
pro TEST_BYTE_STRING2BYTE, cumul_errors, test=test, verbose=verbose
;
nb_errors=0
;
input='0123ABCDabcd'
expected=[48b+BINDGEN(4),65b+BINDGEN(4), 97b+BINDGEN(4)]
result=BYTE(input)
;
if ARRAY_EQUAL(expected, result) NE 1 then begin
   ERRORS_ADD, nb_errors, 'conversion from a STRING to BYTE failed'
endif
;
BANNER_FOR_TESTSUITE, 'TEST_BYTE_STRING2BYTE', nb_errors, /short, verb=verbose
;
ERRORS_CUMUL, cumul_errors, nb_errors
;
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------
;
pro TEST_BYTE_CONVERSION, help=help, verbose=verbose, no_exit=no_exit, test=test
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_BYTE, help=help, verbose=verbose, $'
   print, '               no_exit=no_exit, test=test'
   return
endif
;
; old test, to avoid any regression
TEST_BYTE_BUG_586, Cumul_errors, test=test, verbose=verbose
;
; very clear test on current problem (bug in GDL)
;
TEST_BYTE_BASIC_32768, nb_errors, test=test, verbose=verbose
; the same but with negative values ...
TEST_BYTE_BASIC_32768, nb_errors, /NEGATIVE, test=test, verbose=verbose
;
;
TEST_BYTE_STRING2BYTE, nb_errors, test=test, verbose=verbose
;
; GDL now OK when loop below 10, equivalent to below 32768 ...
print, '2017-03-11 : First run should be OK in GDL'
TEST_BYTE_WITH_LOOP, nb_errors, max_loop=8, test=test, verbose=verbose
;
print, '2017-03-11 : Second run should be BAD in GDL'
TEST_BYTE_WITH_LOOP, nb_errors, max_loop=50, test=test, verbose=verbose
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_BYTE_CONVERSION', nb_errors
;
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
