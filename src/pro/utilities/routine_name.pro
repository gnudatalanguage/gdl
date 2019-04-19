;
; AC 2018-Oct-24
;
; for unclear reasons, it is not so obvious to know
; in which routine (pro or funct) we are ...
; It would be useful for various messages !
;
; This code can be use in IDL and FL too
;
; In GDL, since Feb. 2019, we also have a ROUTINE_NAME_GDLINTERNAL()
;
function ROUTINE_NAME, lowercase=lowercase, test=test, verbose=verbose
;
callStack = SCOPE_TRACEBACK()
nb_levels=N_ELEMENTS(callStack)
;
raw=callStack[nb_levels-2]
callingRoutine = (STRSPLIT(STRCOMPRESS(raw)," ", /Extract))[0]
;
if KEYWORD_SET(lowercase) then callingRoutine=STRlowCASE(callingRoutine)
;
if KEYWORD_SET(verbose) then print, 'Routine Name : ', callingRoutine
;
if KEYWORD_SET(test) then STOP
;
return, callingRoutine
;
end
