;
; Under GNU GPL V3 or later
;
; AC 2018-Feb-22 during a MIRI workshop at IAP
;
; Several bugs related to /L64 found,
; thanks to the use of WHERE( ..., /L64) by a colleague.
;
; ---------------------------------
; 
; Modifications history :
;
; - 2018-01-22 : AC. creation. Ideas welcome ...
;
; ---------------------------------
;
pro TEST_L64_SIZE, cumul_errors, array_2pow32, test=test
;
if ~ISA(array_2pow32) then MESSAGE, 'input array be defined before'
;
nb_errors=0
txt1=' : bad self-promotion into L64'
txt2=' : bad forced-promotion into L64'
;
; First we test the auto-promotion into L64
; which should happen because the size of the array
; is above "Max Long"
;
if (TYPENAME(SIZE(array_2pow32)) NE 'LONG64') then begin
   ERRORS_ADD, nb_errors, 'bad TYPENAME for SIZE'+txt1
endif
if (TYPENAME(SIZE(array_2pow32, /l64)) NE 'LONG64') then begin
   ERRORS_ADD, nb_errors, 'bad TYPENAME for SIZE'+txt2
endif
;
expected_size=[1LL,4294967296LL,1LL,4294967296LL]
;
if ~ARRAY_EQUAL(expected_size,SIZE(array_2pow32),/no_typeconv) then begin
   ERRORS_ADD, nb_errors, 'bad value inside SIZE'+txt1
endif
;
if ~ARRAY_EQUAL(expected_size,SIZE(array_2pow32, /l64),/no_typeconv) then begin
   ERRORS_ADD, nb_errors, 'bad value inside SIZE'+txt2
endif
;
; testing SIZE( /n_elements) ...
;
exp_nbp64=4294967296LL
;
res1=SIZE(array_2pow32, /n_elements)
res2=SIZE(array_2pow32, /n_elements, /l64)
;
if (TYPENAME(res1) NE 'LONG64') then ERRORS_ADD, nb_errors, 'bad type case 1'
if (TYPENAME(res2) NE 'LONG64') then ERRORS_ADD, nb_errors, 'bad type case 2'
if ~ARRAY_EQUAL(res1,exp_nbp64,/no_typeconv) then $
   ERRORS_ADD, nb_errors, 'bad value case 1 (SIZE(/n_elem))'
if ~ARRAY_EQUAL(res2,exp_nbp64,/no_typeconv) then $
   ERRORS_ADD, nb_errors, 'bad value case 2 (SIZE(/n_elem))'
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_L64_SIZE', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; -------------------------------
; AC : I don't have more idea now ... help welcome !
; very basic test on N_ELEMENTS()
; Should auto/self promote into L64 if the size is above Max Long
;
pro TEST_L64_N_ELEMENTS, cumul_errors, the_array, test=test
;
if ~ISA(the_array) then MESSAGE, 'input array be defined before'
;
nb_errors=0
txt1=' : bad self-promotion into L64'
;
get_nbp=N_ELEMENTS(the_array)
;
nbp=4294967296ll
;
if (TYPENAME(get_nbp) NE 'LONG64') then begin
   ERRORS_ADD, nb_errors, 'bad TYPENAME for N_ELEMENTS'+txt1
endif
if (get_nbp NE nbp) then begin
   ERRORS_ADD, nb_errors, 'bad value for N_ELEMENTS'+txt1
endif
;
; the same using "CALL_FUNCTION" which is a special case for ()N_ELEMENTS
;
get_nbp_cf=CALL_FUNCTION('N_ELEMENTS', the_array)
;
if (TYPENAME(get_nbp_cf) NE 'LONG64') then begin
   ERRORS_ADD, nb_errors, 'bad TYPENAME for CALL_FUNCTION & N_ELEMENTS'+txt1
endif
if (get_nbp_cf NE nbp) then begin
   ERRORS_ADD, nb_errors, 'bad value for CALL_FUNCTION & N_ELEMENTS'+txt1
endif
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_L64_N_ELEMENTS', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; -------------------------------
;
pro TEST_L64, help=help, test=test, no_exit=no_exit, verbose=verbose
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_L64, help=help, test=test, $'
   print, '              no_exit=no_exit, verbose=verbose'
   return
endif
;
print, 'Be patient, this code will allocate LARGE arrays (2LL^32 size)'
print, 'The code may fail or swap if not enough memory'
;
cumul_errors=0
;
; creating the array
;
array_2pow32=bytarr(2LL^32)
;
TEST_L64_SIZE, cumul_errors, array_2pow32
TEST_L64_N_ELEMENTS, cumul_errors, array_2pow32
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_L64', cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
