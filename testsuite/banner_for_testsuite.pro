;
; Alain C., 23 April 2014: just moving this code
; from outside TEST_ARRAY_EQUAL to be common
;
pro BANNER_FOR_TESTSUITE, case_name, nb_pbs, short=short, help=help
;
if (N_PARAMS() NE 2) then begin
   MESSAGE, /continue, 'First mandatory param: procedure name (string)'
   MESSAGE, /continue, 'Second mandatory param: errors number (>=0 integer)'
   help=1
endif else begin
   if (nb_pbs LT 0) then begin
      MESSAGE, /continue, 'Number of errrors must be >= 0 !'
      help=1
   endif
endelse
;
if KEYWORD_SET(help) then begin
   print, 'pro BANNER_FOR_TESTSUITE, case_name, nb_pbs, short=short, help=help'
   return
endif
;
prefixe='% '+STRUPCASE(case_name)+': '
;
indent="  "
message=' errors encoutered during '+STRUPCASE(case_name)+' tests'
if (nb_pbs GT 0) then begin
   message=STRCOMPRESS(STRING(nb_pbs),/remove_all)+message
endif else begin
   message='NO'+message
endelse
message=indent+message+indent
;
if KEYWORD_SET(short) then begin
   print, prefixe, message
   return
endif
;
lenght=STRLEN(message)
sep ="="
line=""
vide=" "
blanc=""
for ii=0,lenght-1 do begin
   line=line+sep
   blanc=blanc+vide
endfor
;
print, prefixe, sep+line+sep
print, prefixe, sep+blanc+sep
print, prefixe, sep+message+sep
print, prefixe, sep+blanc+sep
print, prefixe, sep+line+sep
;
end
