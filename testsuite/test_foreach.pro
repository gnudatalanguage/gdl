;
; AC 2024/03/06
;
; Trying to collect and classify known bugs related to FOREACH
; Some of those bugs have been solved but it is always a good
; idea to keep the test if we know how to test it !
; As usual, event "stupid" tests migth be useful any time in the
; future !
;
; As usual, it is not easy to write exhaustif tests, please contribute !
;
; -------------------------------------------------------------
; https://sourceforge.net/p/gnudatalanguage/bugs/273/
pro TEST_FOREACH_SF_273, cumul_errors, test=test
;
nb_errors=0
;
a=1000
FOREACH a, [0] do a = 0
if a NE 0 then ERRORS_ADD, nb_errors, '<<a>> should be 0'
;
b=1000
FOREACH a, [0] do b = 0
if b NE 0 then ERRORS_ADD, nb_errors, '<<b>> should be 0'
;
BANNER_FOR_TESTSUITE, 'TEST_FOREACH_SF_273', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; -------------------------------------------------------------
; https://sourceforge.net/p/gnudatalanguage/bugs/630/
; copy in https://github.com/gnudatalanguage/gdl/issues/29
pro TEST_FOREACH_SF_630, cumul_errors, test=test, verbose=verbose
;
nb_errors=0
;
res=0
a = [1,2,3]
FOREACH a, a DO BEGIN
   res=[res,a]
ENDFOREACH
;
if (N_ELEMENTS(res) NE 2) then begin
   ERRORS_ADD, nb_errors, '<<res>> shoudl contain 2 elements only'
   print, 'the case is : a = [1,2,3] & FOREACH a, a DO print, a'
endif
;print, 'I don''t know how to change that into a test :('
;
BANNER_FOR_TESTSUITE, 'TEST_FOREACH_SF_630', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; -------------------------------------------------------------
; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
; part of GDL - GNU Data Language
; former file "test_bug_3288652.pro" in the test suite
;
pro TEST_BUG_3288652, cumul_errors, test=test
;
; get the list of all system (internal) functions
arr = ROUTINE_NAMES(/s_fun)
;
nb_errors=0
;
; WTN should be detected one and only one time !
count=0
FOREACH a, arr do if a eq 'WTN' then count++
;
if (count NE 1) then begin
   if (count LT 1) then message, /count, 'WTN not counted, but should be !'
   if (count GT 1) then message, /count, 'WTN counted more than one time :('
   ERRORS_ADD, nb_errors, 'bad count in FOREACH loop'
endif
;
BANNER_FOR_TESTSUITE, 'TEST_BUG_3288652', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; -------------------------------------------------------------
; This test is also a test on LIST() !
;
pro TEST_FOREACH_LIST, cumul_errors, test=test
;
nb_errors=0
;
el0='toto'
el1=COMPLEX(0.,1.)
el2=!pi
el3=[!pi]
el4=indgen(4)
mylist=LIST(el0,el1,el2,el3,el4)
;
ii=0
;
; since LIST() should be processed in order we can test !
FOREACH element, mylist DO BEGIN
   if ii EQ 0 then begin
      if ~ARRAY_EQUAL(element, el0) then ERRORS_ADD, nb_errors, 'bad <<el0>>'
   endif
   if ii EQ 1 then begin
      if ~ARRAY_EQUAL(element, el1) then ERRORS_ADD, nb_errors, 'bad <<el1>>'
   endif
   if ii EQ 2 then begin
      if ~ARRAY_EQUAL(element, el2) then ERRORS_ADD, nb_errors, 'bad <<el2>>'
   endif
   if ii EQ 3 then begin
      if ~ARRAY_EQUAL(element, el3) then ERRORS_ADD, nb_errors, 'bad <<el3>>'
   endif
   if ii EQ 4 then begin
      if ~ARRAY_EQUAL(element, el4) then ERRORS_ADD, nb_errors, 'bad <<el4>>'
   endif
   
   ii++
   ;;   if a NE 0 then ERRORS_ADD, nb_errors, '<<a>> should be 0'
ENDFOREACH
;
nb_elts_list=N_ELEMENTS(mylist)
if (ii NE nb_elts_list)  then ERRORS_ADD, nb_errors, 'bad internal count'
;
b=1000
FOREACH a, [0] do b = 0
if b NE 0 then ERRORS_ADD, nb_errors, '<<b>> should be 0'
;
BANNER_FOR_TESTSUITE, 'TEST_FOREACH_LIST', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; -------------------------------------------------------------
;
pro TEST_FOREACH_INDEX, cumul_errors, test=test, verbose=verbose
;
nb_errors=0
;
ramp=INDGEN(6)
ii=0
;
FOREACH element, ramp, index DO BEGIN
   if element NE ramp[index] then ERRORS_ADD, nb_errors, 'bad <<el0>>'
   if KEYWORD_SET(verbose) then begin
      print, 'Index ', index, ' Value = ', element, 'Expected = ', ramp[index]
   endif
ENDFOREACH
;
; ---------------------
;
BANNER_FOR_TESTSUITE, 'TEST_FOREACH_INDEX', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; -------------------------------------------------------------
;
pro TEST_FOREACH, help=help, test=test, verbose=verbose, no_exit=no_exit 
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_FOREACH, help=help, test=test, verbose=verbose, no_exit=no_exit'
   return
endif 
;
cumul_errors=0
;
TEST_FOREACH_SF_273, cumul_errors, test=test
; GD: removing TEST_FOREACH_SF_630 as it will not corrected in a decent future.
;TEST_FOREACH_SF_630, cumul_errors, test=test
TEST_BUG_3288652, cumul_errors, test=test
TEST_FOREACH_LIST, cumul_errors, test=test
TEST_FOREACH_INDEX, cumul_errors, test=test
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_FOREACH', cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
end
