;
; (c) Sylwester Arabas and Alain Coulais, August 2011
; under GNU GPL v2 or later
;
; We had some trouble in STR_SEP() due to bugs in STRSPLIT
; It was trigged by this bug report
;
pro TEST_BUG_3286031, test=test
;
resultat=STR_SEP('ahasadfasdf','dfa')
nb_pbs=0
if (resultat[0] NE 'ahasa') then nb_pbs=nb_pbs+1
if (resultat[1] NE 'sdf') then nb_pbs=nb_pbs+1

s_value=SIZE(STRLEN(resultat))
s_expected=[1,2,3,2]
if (ARRAY_EQUAL(s_value,s_expected) NE 1) then nb_pbs=nb_pbs+1
;
if (nb_pbs GT 0) then begin
    MESSAGE, /Continue, "Errors found, due to STR_SEP or STRSPLIT"
    if ~KEYWORD_SET(test) then EXIT, status=1 
endif
;
if KEYWORD_SET(test) then STOP
;
end
