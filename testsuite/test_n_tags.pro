;
; T. Mermet & Alain C. 2009-JUL-17
; under GPL v2. or later 
;
; Purpose: quick check of computation of structure (data) length
;
; Expected sizes:
; BYTE  : 1
; INT   : 2 
; LONG  : 4
; FLOAT : 4
; DOUBLE: 8
; (check with success on x86 and x86_64)
;
; This code was working OK in gdl-0.9.1cvs110818
; .... and not working since gdl-0.9.1cvs110829
;
; ---------------------------------------
; Modifications history :
;
; - 2009-JUL-17 : initial version by T. Mermet & AC
; - 2018-JAN-31 : AC. large rewriting, trying to be more general
;   Unfortunatelly, GDL now totally wrong here.
;
; -----------------------------------------------------------------
;
pro TEST_NTAGS_BY_TYPE, cumul_errors, factor=factor, test=test, verbose=verbose
;
errors=0
;
if ~KEYWORD_SET(factor) then factor=3
;
GIVE_LIST_NUMERIC, list_num_types, list_num_names, list_num_size
;
for ii=0,N_ELEMENTS(list_num_names)-1 do begin
   ;;
   type_value=list_num_types[ii]
   type_name=list_num_names[ii]
   ;;if KEYWORD_SET(test) then print, 'test on type : ', type_name
   ;;
   name='tmp_'+type_name
   vname='var_'+type_name
   field='field_'+type_name+STRCOMPRESS(string(factor),/remove)
   command=name+"={"+field+", "+vname+": INDGEN(factor, type=type_value)}"
   res=EXECUTE(command)
   res=EXECUTE('res_l=N_TAGS('+name+',/length)')
   res=EXECUTE('res_dl=N_TAGS('+name+',/data_length)')
   ;;
   if (res_l NE factor*list_num_size[ii]) then begin
      ADD_ERROR, errors, '/length case <<'+type_name+'>>'
      if KEYWORD_SET(verbose) then $
         print, 'expected : ', factor*list_num_size[ii], '; effective : ', res_l
   endif
   if (res_dl NE factor*list_num_size[ii]) then begin
      ADD_ERROR, errors, '/dat_length case <<'+type_name+'>>'
      if KEYWORD_SET(verbose) then $
         print, 'expected : ', factor*list_num_size[ii], '; effective : ', res_dl
   endif
   ;;
   ;;print, format='(A,A8, 2I4)', "type, length, data_length :", $
   ;;       type_name, v1, v2 ;N_TAGS(name,/length), N_TAGS(name,/data_length)
   ;;if KEYWORD_SET(test) then help, name, /struct
   ;;stop
endfor
;
; --------------
;
BANNER_FOR_TESTSUITE, "TEST_NTAGS_BY_TYPE", errors, /short, verb=verbose
;
ERRORS_CUMUL, cumul_errors, errors
;
if KEYWORD_SET(test) then STOP
;
end
;
; -----------------------------------------------------------------
;
pro TEST_NTAGS_MIXING_TYPE, cumul_errors, test=test, verbose=verbose
;
errors=0
;
; First (historical) test: here we have: 1*B+1*I+2*L+7*F+1*D ==> 47
;
tmp47 = {data, ydh:0l, num:0l, t97:0.d0, f:0., dt:0., df:0., $
         autoX:0., autoZ:0., crossR:0., crossI:0., ant:0b, a: 0}
exp47=[47,48]
;
if KEYWORD_SET(verbose) then begin
   help, tmp47, /struct
   print,"length, data_length :", N_TAGS(tmp47,/length), N_TAGS(tmp47,/data_length)
endif
;
if N_TAGS(tmp47,/data_length) NE exp47[0] then ADD_ERROR, errors, '/data_length case 47'
if N_TAGS(tmp47,/length) NE exp47[1] then ADD_ERROR, errors, '/length case 47'
;
; --------------
;
BANNER_FOR_TESTSUITE, "TEST_NTAGS_MIXING_TYPE,", errors, /short, verb=verbose
;
ERRORS_CUMUL, cumul_errors, errors
;
if KEYWORD_SET(test) then STOP
;
end
;
; -----------------------------------------------------------------
;
pro TEST_N_TAGS, help=help, test=test, verbose=verbose, no_exit=no_exit
;
if KEYWORD_SET(test) then begin
   print, 'pro TEST_NTAGS, help=help, test=test, verbose=verbose, no_exit=no_exit'
   return
endif 
;
cumul_errors=0
;
TEST_NTAGS_BY_TYPE, cumul_errors, test=test, verbose=verbose
TEST_NTAGS_BY_TYPE, cumul_errors, test=test, verbose=verbose, factor=4
TEST_NTAGS_BY_TYPE, cumul_errors, test=test, verbose=verbose, factor=7
;
TEST_NTAGS_MIXING_TYPE, cumul_errors, test=test, verbose=verbose
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_NTAGS', cumul_errors, short=short
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
