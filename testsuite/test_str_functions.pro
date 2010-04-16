;
; Alain Coulais, 16 Avril 2010
;
; Few basic tests on functions working on Strings
;
pro TEST_STRMID, exit_on_error=exit_on_error, test=test
;
a='azerty'
flag_pb=0
;
if NOT(STRCMP(a,STRMID(a,0))) then flag_pb=1
if NOT(STRCMP(a,STRMID(a,0,100))) then flag_pb=flag_pb+1
;
res=STRMID(a,3)
if NOT(STRCMP(res,'rty')) then flag_pb=flag_pb+1
;
if flag_pb GT 0 then begin
    MESSAGE, /continue, STRING(flag_pb)+' ERROR(s) found in STRMID'
    if KEYWORD_SET(exit_on_error) then  EXIT, status=1
endif else begin
    MESSAGE, /continue, 'No  ERROR found in STRMID'
endelse
;
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
    if NOT(STRCMP(str1[ii],str2[ii])) then return, 0
endfor
return, 1
end
;
; -----------------------
;
pro TEST_STRSPLIT, exit_on_error=exit_on_error, test=test, debug=debug
;
str = 'rouge&&bleu&&jaune&&pair&impair'
expected_res1=['rouge','bleu','jaune','pair','impair']
expected_res2=['rouge','bleu','jaune','pair&impair']
;
flag_pb=0
res1=STRSPLIT(str,'&&',/EXTRACT)
if NOT(STRCMP_MULTI(res1,expected_res1,debug=debug)) then flag_pb=flag_pb+1
;
res2=STRSPLIT(str,'&&',/EXTRACT,/REGEX)
if NOT(STRCMP_MULTI(res2,expected_res2,debug=debug)) then flag_pb=flag_pb+1
;
str2 = '<4>What<1>a<7>tangled<3>web<2>we<6>weave.'
expected_res=['What','a','tangled','web','we','weave.']
res = STRSPLIT(str2,'<[0-9]+>',/EXTRACT,/REGEX)
if NOT(STRCMP_MULTI(res,expected_res,debug=debug)) then flag_pb=flag_pb+1
;
if flag_pb GT 0 then begin
    MESSAGE, /continue, STRING(flag_pb)+' ERROR(s) found in STRSPLIT'
    if KEYWORD_SET(exit_on_error) then  EXIT, status=1
endif else begin
    MESSAGE, /continue, 'No  ERROR found in STRSPLIT'
endelse
;
if KEYWORD_SET(test) then STOP
;
end
;
; ---------------
;
pro TEST_STR_FUNCTIONS
;
TEST_STRMID, /exit_on_error
TEST_STRSPLIT, /exit_on_error
;
end
