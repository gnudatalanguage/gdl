;
; under GNU GPL v3
; Alain Coulais, 31-Oct-2017
;
; very preliminary version, 
; problem close to zero needed to be solved/clarified ...
;
; -----------------------------------------------------------------
;
pro TEST_CHISQR_CVF, help=help, verbose=verbose, no_exit=no_exit, test=test
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST__CHISQR_CVF, help=help, verbose=verbose, $'
   print, '                      no_exit=no_exit, test=test'
   return
endif
;
; tolerance
eps = 1e-5
;
nb_errors=0
;
expected_01=[3.40788e-29, 2.70554, 4.60517, 6.25139, 7.77944]
expected_05=[1.89327e-29, 0.454937, 1.38629, 2.36597, 3.35669]
expected_09=[3.78653e-30, 0.0157908, 0.210721, 0.584375, 1.06362]
;
res_01=FLTARR(5)
;
; unsolved problems within 0. ...
;
start=1
for ii=start,4 do res_01[ii]=CHISQR_CVF(0.1,ii)
if (MAX(ABS(expected_01-res_01)) GT eps) then ADD_ERROR, nb_errors, 'prob 0.1'
;
res_05=FLTARR(5)
for ii=start,4 do res_05[ii]=CHISQR_CVF(0.5,ii)
if (MAX(ABS(expected_05-res_05)) GT eps) then ADD_ERROR, nb_errors, 'prob 0.5'
;
res_09=FLTARR(5)
for ii=start,4 do res_09[ii]=CHISQR_CVF(0.9,ii)
if (MAX(ABS(expected_09-res_09)) GT eps) then ADD_ERROR, nb_errors, 'prob 0.9'
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_CHISQR_CVF', nb_errors
; 
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
