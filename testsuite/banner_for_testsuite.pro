;
; Alain C., 23 April 2014: just moving this code
; from outside TEST_ARRAY_EQUAL to be common
;
; hierarchy : Wide (default) > verbose > short
;
pro BANNER_FOR_TESTSUITE, case_name, nb_pbs, help=help, $
                          short=short, verbose=verbose, wide=wide
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
   print, 'pro BANNER_FOR_TESTSUITE, case_name, nb_pbs, help=help, $'
   print, '                          short=short, verbose=verbose, wide=wide'
   return
endif
;
prefixe='% '+STRUPCASE(case_name)+': '
;
indent="  "
message=' errors encountered during '+STRUPCASE(case_name)+' tests'
if (nb_pbs GT 0) then begin
   message=STRCOMPRESS(STRING(nb_pbs),/remove_all)+message
endif else begin
   message='NO'+message
endelse
message=indent+message+indent
;
; managing Keywords hierarchy. Default is "isWide"
;
isWide=1
isVerbose=0
isShort=0
;
if ~KEYWORD_SET(wide) then begin
    if KEYWORD_SET(verbose) then begin
        isWide=0
        isVerbose=1
        isShort=0
    endif else begin
        if KEYWORD_SET(short) then begin
            isWide=0
            isVerbose=0
            isShort=1
        endif
    endelse
endif
;
if (isShort) then begin
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
if (isWide) then print, prefixe, sep+blanc+sep
print, prefixe, sep+message+sep
if (isWide) then print, prefixe, sep+blanc+sep
print, prefixe, sep+line+sep
;
end
