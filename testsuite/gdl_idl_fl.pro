;
; Alain C., March 2015
;
; * AC 2017-JUL-27 adding /uppercase
; * AC 2017-JUL-27 adding /prefix
;
; Easy way to generate prefix/suffix for outputs files
; (e.g. running test suite)
;
; Most tests in the testsuite are running well
; in GDL, IDL and FL. But we have few differences ...
;
; ----------------------------------------------------
; Modifications history :
;
; 2018-Feb-05 : AC. Default return now UpperCase
;
; ----------------------------------------------------
;
function GDL_IDL_FL, uppercase=uppercase, prefix=prefix, $
                     lowercase=lowercase, $
                     verbose=verbose, test=test
;
DEFSYSV, '!gdl', exists=isGDL
;
if isGDL then suffix='gdl' else begin
   ;;
   ;; AC 15 May 2015
   ;; (the only way we know to distinguish between IDL and FL)
   ;; it seems that, if not GDL, testing !Slave is simplest way to
   ;; check if we are in FL or IDL ! (better test welcome !)
   ;; This test is still OK in FL fl_0.79.41
   ;;
   DEFSYSV, '!slave', exists=isFL
   if isFL then suffix='fl' else suffix='idl'
endelse
;
; AC 2018-02-07 : we decided the default is now UpperCase
; We even don't test the /upperCase keyword (compatility)
suffix=STRUPCASE(suffix)
;
if KEYWORD_SET(prefix) then suffix=suffix+'_'
;
if KEYWORD_SET(lowercase) then suffix=STRLOWCASE(suffix)
;
if KEYWORD_SET(verbose) then MESSAGE, /cont, 'Detected Software : '+suffix
;
if KEYWORD_SET(test) then STOP
;
return, suffix
;
end
