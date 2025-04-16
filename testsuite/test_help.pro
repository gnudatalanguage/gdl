;
; test_help: a prototype everything goes collection of compiled routines
; and common blocks, and help calls.
;
; Revised by AC 2025-apr-15
;  - Should work with IDL (no HELP,  /lib in IDL ...)
;  - unclear messages ...
;  - sould run without stopping
;  - TEST_HELP_COMMON was not OK before april 2025 !
;
; ----------------------------------------------------
;
pro HOLDACOMMON, a,b, outhelp=outhelp, test=test
; 
common acommon, acom_a,acom_b
HELP, acom_a, acom_b, out=outstrb
acom_a=a
acom_b=b
HELP, acom_a, acom_b, out=outstre
if KEYWORD_SET(test) then stop,' test KW: holdacommon'
if (N_ELEMENTS(outstrb) ne 0) then outhelp=[outstrb," ... then (holda) ...",outstre]
;
end
;
; ----------------------------------------------------
;
pro HOLDBCOMMON, a,b,outhelp=outhelp, test=test
;  
common bcommon, bcom_a,bcom_b
;
HELP, bcom_a,bcom_b,out=outstrb
bcom_a=a
bcom_b=b
HELP, bcom_a,bcom_b,out=outstre
if KEYWORD_SET(test) then stop,' test KW: holdbcommon'
if (N_ELEMENTS(outstrb) ne 0) then outhelp=[outstrb," ... then (holdb) ...",outstre]
;
end
;
; ----------------------------------------------------
;
pro TEST_HELP_COMMON, cumul_errors, verbose=verbose, test=test
;
nb_errors=0
;
HOLDACOMMON, FINDGEN(2,3), FLTARR(4), outhelp=stra;, test=test
HOLDBCOMMON, FINDGEN(4,5), FLTARR(6), outhelp=strb;, test=test
na = N_ELEMENTS(stra)
nb = N_ELEMENTS(strb)
;
if KEYWORD_SET(verbose) then print,' test KW: $MAIN check na, nb'
;
if (na ne nb) or (na eq 0) then begin
   ERRORS_ADD, nb_errors, ' na, nb check fails '
   if KEYWORD_SET(verbose) then print,' na, nb check fails: ',na, nb
endif
;  
if KEYWORD_SET(verbose) then for k=0,na-1 do print,stra[k]
;
common acommon, acom_a, acom_b
common bcommon, bcom_a, bcom_b
;
HELP, names='*com_*', out=comstr
ncs=N_ELEMENTS(comstr)
;
; expected_ncs is 8
;
expected_ncs=8
if (ncs ne expected_ncs) then ERRORS_ADD, nb_errors, 'ncs check fails !'
if KEYWORD_SET(verbose) then begin
   print, 'Expected NCS : ', expected_ncs
   print, 'Given NCS    : ', ncs
   for k=0,ncs-1 do print,comstr[k]
endif
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_HELP_COMMON', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; ----------------------------------------------------
; in GDL only we have a way to list all the internal pro/func
; We count here the number of functions and pro with three first
; chararacters as "str"
;
pro TEST_HELP_LIB, cumul_errors, verbose=verbose, test=test
;
; extension /LIB is not working for FL or IDL
;
if (GDL_IDL_FL() NE 'GDL') then return
;
nb_errors=0
;
HELP, /fun, /lib, name='str*', output=strfun
;
; AC as is on April 15, 2025, we do have 15 STR* pro/func
;
expected=15
nf=N_ELEMENTS(strfun)
if ((nf-3) NE expected) then begin
   ERRORS_ADD, nb_errors, 'bad count for STR* pro/func'
endif
;
if KEYWORD_SET(verbose) then begin
   print,' There are ',nf-3,' library functions that begin with "STR"'
   print, TRANSPOSE(strfun)
endif
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_HELP_LIB', nb_errors, /short
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_set(test) then STOP
;
end
;
; ----------------------------------------------------
; rudimentary beginnings of a test program
;
pro TEST_HELP, test=test, no_exit=no_exit, verbose=verbose, help=help
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_HELP, test=test, no_exit=no_exit, verbose=verbose, help=help'
   return
endif
;
cumul_errors=0
;
TEST_HELP_LIB, cumul_errors, verbose=verbose
TEST_HELP_COMMON, cumul_errors, verbose=verbose
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_HELP', cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then stop
;
end
