;
; Alain Coulais, 16 Avril 2010
;
; Few basic tests on functions working on Strings
;
; Adding few new tests for STRMID, /reverse was broken but not tested !
; We really need as exhaustive as possible tests to avoid
; any unwanted regression, including improbables (e.g. typo in commit !)
;
; ---------------------------------
; Modifications history :
;
; 2025-Apr-18: AC. adding TEST_STRTRIM
; Some rewritting to be complient with current system (cummul_errors)
;
; ---------------------------------
;
; 4 cases : no key, 0, 1, 2
pro TEST_STRTRIM, cumul_errors, test=test
;
nb_errors=0
;
; basic test
a=STRING(56)
;
; tests without STRTRIM
if ~STRCMP(a,'      56') then ERRORS_ADD, nb_errors, 'case 0 basic'
if (STRLEN(a) NE 8) then ERRORS_ADD, nb_errors, 'case 0 basic len'
;
; test STRTRIM with no key
if ~STRCMP(STRTRIM(a),'      56') then ERRORS_ADD, nb_errors, 'case 0'
if STRLEN(STRTRIM(a)) NE 8 then ERRORS_ADD, nb_errors, 'case 0 len'
;
; test STRTRIM with key =0 (trail) (equivaent to previous without key)
if ~STRCMP(STRTRIM(a,0),'      56') then ERRORS_ADD, nb_errors, 'case 0, 0'
if STRLEN(STRTRIM(a,0)) NE 8 then ERRORS_ADD, nb_errors, 'case 0, 0 len'
;
; test STRTRIM with key =1 (trim)
if ~STRCMP(STRTRIM(a,1),'56') then ERRORS_ADD, nb_errors, 'case 0, 1'
if STRLEN(STRTRIM(a,1)) NE 2 then ERRORS_ADD, nb_errors, 'case 0, 1 len'
;
; test STRTRIM with all trimming (key =2)
if ~STRCMP(STRTRIM(a,2),'56') then ERRORS_ADD, nb_errors, 'case 0, 2'
if STRLEN(STRTRIM(a,2)) NE 2 then ERRORS_ADD, nb_errors, 'case 0, 2 len'
;
; -----
; same with a space and a tab *after*
a=STRING(56)+' '+STRING(9b)
;
; tests without STRTRIM
if ~STRCMP(a,'      56 '+STRING(9b)) then ERRORS_ADD, nb_errors, 'case 1 basic'
if (STRLEN(a) NE 10) then ERRORS_ADD, nb_errors, 'case 1 basic len'
;
; test STRTRIM with no key
if ~STRCMP(STRTRIM(a),'      56') then $
   ERRORS_ADD, nb_errors, 'case 1'
if STRLEN(STRTRIM(a)) NE 8 then ERRORS_ADD, nb_errors, 'case 1 len'
;
; test STRTRIM with key =0 (trail) (equivaent to previous without key)
if ~STRCMP(STRTRIM(a,0),'      56') then ERRORS_ADD, nb_errors, 'case 1, 0'
if STRLEN(STRTRIM(a,0)) NE 8 then ERRORS_ADD, nb_errors, 'case 1, 0, len'
;
; test STRTRIM with key =1 (trim)
if ~STRCMP(STRTRIM(a,1),'56 '+STRING(9b)) then ERRORS_ADD, nb_errors, 'case 1, 1'
if STRLEN(STRTRIM(a,1)) NE 4 then ERRORS_ADD, nb_errors, 'case 1, 1, len '
;
; test STRTRIM with all trimming (key =2)
if ~STRCMP(STRTRIM(a,2),'56') then ERRORS_ADD, nb_errors, 'case 1, 2'
if STRLEN(STRTRIM(a,2)) NE 2 then ERRORS_ADD, nb_errors, 'case 1, 2, len'
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_STRTRIM', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_SET(test) then STOP
;
end
; ---------------------------------
;
pro TEST_STRMID, cumul_errors, test=test
;
a='azerty'
nb_errors=0
;
if ~STRCMP(a,STRMID(a,0)) then ERRORS_ADD, nb_errors, 'case 0'
if ~STRCMP(a,STRMID(a,0,100)) then ERRORS_ADD, nb_errors, 'case 100'
;
res=STRMID(a,3)
if ~STRCMP(res,'rty') then ERRORS_ADD, nb_errors, 'case 3 rty'
;
res=STRMID(a,3,1)
if ~STRCMP(res,'r') then ERRORS_ADD, nb_errors, 'case 3 1 r'
;
res=STRMID(a,2,1,/reverse)
if ~STRCMP(res,'r') then ERRORS_ADD, nb_errors, 'case 2 r /reverse'
;
res=STRMID(a,2,/reverse)
if ~STRCMP(res,'rty') then ERRORS_ADD, nb_errors, 'case rty /reverse'
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_STRMID', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_SET(test) then STOP
;
end
;
; ---------------
;
; return "0" when the arrays of substrings are different, else return "1"
function STRCMP_MULTI, str1, str2, debug=debug
;
if N_ELEMENTS(str1) NE N_ELEMENTS(str2) then return, 0
;
for ii=0, N_ELEMENTS(str1)-1 do begin
    if KEYWORD_SET(debug) then print, ii, str1[ii],str2[ii]
    if ~(STRCMP(str1[ii],str2[ii])) then return, 0
endfor
return, 1
end
;
; -----------------------
;
pro TEST_STRSPLIT, cumul_errors, test=test, debug=debug
;
str = 'rouge&&bleu&&jaune&&pair&impair'
expected_res1=['rouge','bleu','jaune','pair','impair']
expected_res2=['rouge','bleu','jaune','pair&impair']
;
nb_errors=0
res1=STRSPLIT(str,'&&',/EXTRACT)
if ~STRCMP_MULTI(res1,expected_res1,debug=debug) then $
   ERRORS_ADD, nb_errors, 'case &&'
;
res2=STRSPLIT(str,'&&',/EXTRACT,/REGEX)
if ~STRCMP_MULTI(res2,expected_res2,debug=debug) then $
     ERRORS_ADD, nb_errors, 'case && + regex' 
;
str2 = '<4>What<1>a<7>tangled<3>web<2>we<6>weave.'
expected_res=['What','a','tangled','web','we','weave.']
res = STRSPLIT(str2,'<[0-9]+>',/EXTRACT,/REGEX)
if ~STRCMP_MULTI(res,expected_res,debug=debug) then $
     ERRORS_ADD, nb_errors, 'case <[0-9]+> + regex' 
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_STRSPLIT', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_SET(test) then STOP
;
end
;
; ---------------
pro TEST_STR_FUNCTIONS, help=help, test=test, no_exit=no_exit, verbose=verbose
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_STR_FUNCTIONS, help=help, test=test, $'
   print, '                        no_exit=no_exit, verbose=verbose'
   return
endif
;
cumul_errors=0
;
TEST_STRTRIM, cumul_errors
TEST_STRMID, cumul_errors
TEST_STRSPLIT, cumul_errors
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_STR_FUNCTIONS', cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
;

