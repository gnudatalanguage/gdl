;
; Alain C., March 2015
;
; Most tests in the testsuite are running well
; in GDL, IDL and FL. But we have few differences ...
;
function GDL_IDL_FL, verbose=verbose, test=test
;
DEFSYSV, '!gdl', exists=isGDL
;
if isGDL then suffix='gdl' else begin
   ;; AC 15 May 2015
   ;; it seems that, if not GDL, testing !Slave is simplest way to
   ;; check if we are in FL or IDL ! (better test welcome !)
   DEFSYSV, '!slave', exists=isFL
   if isFL then suffix='fl' else suffix='idl'
endelse
;
if KEYWORD_SET(verbose) then MESSAGE,/cont, 'Detected Software : '+suffix
;
if KEYWORD_SET(test) then STOP
;
return, suffix
;
end
