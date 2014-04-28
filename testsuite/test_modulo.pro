;
; some tests on Modulo operations
;
; Alain Coulais, April 22, 2014
;
; Under GNU GPL v2 or later
;
; any suggestion welcome to enlarge the scope of these tests
;
pro MYMESS, errors, message
errors=errors+1
MESSAGE, /continue, message
end
;
function TMP_TYPENAME, input
;
typename_exist=0
;
DEFSYSV, '!gdl', exists=is_it_gdl
;
if (is_it_gdl) then begin
   if (!gdl.EPOCH GT 1398200000) then typename_exist=1
   ;;
endif else begin
   ;;
   ;; we are in IDL, TYPENAME() was introduced in 8.0
   ;;
   if !VERSION.RELEASE GE '8.0' then typename_exist=1
endelse
;
if (typename_exist) then begin
   return, TYPENAME(input)
endif else begin
   return, STRING(SIZE(input,/tname))
endelse
;
end
;
; ----------------------------
;
pro MODULO_ON_INTEGERS, nb_errors, type=type, verbose=verbose, test=test
;
if ~KEYWORD_SET(type) then begin
   type=2
   MESSAGE,/continue, 'input TYPE set to : INT'
endif
;
if ((type EQ 0) OR ((type GT 3 ) AND (type LT 12))) then begin
   if (type EQ 10) OR (type EQ 11) then begin
      MESSAGE, /continue, 'BAD input TYPE (pointer, struct)'
   endif else begin
      MESSAGE, /continue, 'BAD numerical input TYPE (not an Integer-like) ['+TMP_TYPENAME(INDGEN(1, type=type))+']'
   endelse
   return
endif
;
errors=0
;
nbps=123456
;
; special limits for Byte, Int, Uint ...
if (type EQ 1) then begin 
   nbps=30
   half=0b
endif
if (type EQ 2) OR (type EQ 12) then nbps=4500 ; (~~ 32768 / 7)
if (type EQ 12) OR (type EQ 13) OR (type EQ 15) then half=0
;
if (type EQ 2) OR (type EQ 3) OR (type EQ 14) then half=nbps/2
;
input0=INDGEN(nbps, type=type)-half
;
if KEYWORD_SET(verbose) then begin
   print, 'requested TYPE = ', type, ', Type Name = ', $
          TMP_TYPENAME(INDGEN(1, type=type))
   print, 'effective TYPE = ', SIZE(input0,/type), ', Type Name = ', $
          TMP_TYPENAME(input0)   
endif
;
input2=input0*2
input7=input0*7
;
type_info='(for TYPE == '+TMP_TYPENAME(input0)+')'
;
txt='Errors in MODULO_ON_INTEGERS '+type_info
;
res= input2 MOD 2
mini=MIN(res)
maxi=MAX(res)
if ((mini LT 0) OR (maxi GT 0)) then MYMESS, errors, txt+'(case MOD 2)'
;
res= input7 MOD 7
mini=MIN(res)
maxi=MAX(res)
if ((mini LT 0) OR (maxi GT 0)) then MYMESS, errors, txt+'(case MOD 7)'
;
mess='errors found in MODULO_ON_INTEGERS '
;
if (errors GT 0) then begin
   MESSAGE, /cont, 'nb of '+mess+type_info+' : '+string(errors)
endif else begin
   MESSAGE, /cont, 'NO '+mess+type_info+'.'
endelse
;
if (N_PARAMS() EQ 1) then nb_errors=nb_errors+errors
;
if KEYWORD_SET(test) then STOP
;
end
;
; ----------------------------
;
pro MODULO_ON_FLOAT, nb_errors, tolerance=tolerance, nbp=nbp, scale=scale, $
                     double=double, verbose=verbose, test=test
;
errors=0
;
if ~KEYWORD_SET(tolerance) then tolerance=0.0
if ~KEYWORD_SET(nbp) then nbp=100
if ~KEYWORD_SET(scale) then scale=.25
;
if KEYWORD_SET(double) then begin
   two_pi=2.*!Dpi
   four_pi=4.*!Dpi
   input=scale*(DINDGEN(nbp)-nbp/2)
endif else begin
   two_pi=2.*!pi
   four_pi=4.*!pi
   input=scale*(FINDGEN(nbp)-nbp/2)
endelse
;
res=(((four_pi+input) mod two_pi) - (input mod two_pi)) mod two_pi
abs_res=TOTAL(ABS(res))
if KEYWORD_SET(verbose) then print, 'Tolerance, Result :', tolerance, abs_res
;
if KEYWORD_SET(double) then txt='/Double ' else txt='/Float '
txt=txt+' and NBP= '+STRING(nbp)+', scale='+STRING(scale)
txt=txt+' and Tolerance='+STRING(Tolerance)
;
if (abs_res LE tolerance) then begin
   txt='OK for '+txt
endif else begin
   txt='ERROR for '+txt
   errors++
endelse
MESSAGE,/continue, STRCOMPRESS(txt)
;
if (N_PARAMS() EQ 1) then nb_errors=nb_errors+errors
;
if KEYWORD_SET(test) then STOP
;
end
;
; ----------------------------------------------------------------
;
pro TEST_MODULO, help=help, verbose=verbose, no_exit=no_exit, test=test
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_MODULO, help=help, verbose=verbose, $'
   print, '                 no_exit=no_exit, test=test'
   return
endif
;
nb_errors=0
;
types_list=[1,2,3,12,13,14,15]
for ii=0, N_ELEMENTS(types_list)-1 do begin
   MODULO_ON_INTEGERS, nb_errors, type=types_list[ii], verbose=verbose, test=test
endfor
;
; no tolerance on default
MODULO_ON_FLOAT, nb_errors, tol=3.5e-5, verbose=verbose, test=test
MODULO_ON_FLOAT, nb_errors, /double, verbose=verbose, test=test
;
MODULO_ON_FLOAT, nb_errors, tol=6.04, scale=11.12, nb=1230, $
                 verbose=verbose, test=test
MODULO_ON_FLOAT, nb_errors, tol=2.4e-10, scale=11.12, nb=1230, /double, $
                 verbose=verbose, test=test
;
MODULO_ON_FLOAT, nb_errors, tol=6.3, scale=1.12, nb=1230, $
                 verbose=verbose, test=test
MODULO_ON_FLOAT, nb_errors, tol=2.4e-11, scale=1.12, nb=1230, /double, $
                 verbose=verbose, test=test
;
BANNER_FOR_TESTSUITE, 'TEST_MODULO', nb_errors
;
if (nb_errors EQ 0) then begin
    MESSAGE, /continue, 'No error found in TEST_MODULO'
endif else begin
    MESSAGE, /continue, STRING(nb_errors)+' nb_errors found in TEST_MODULO'
    if ~KEYWORD_SET(no_exit) then EXIT, status=1
endelse
;
if KEYWORD_SET(test) then STOP
;
end
