;
; Alain C., 23 April 2014: just moving this code
; from outside TEST_ARRAY_EQUAL to be common
;
; ----------------------------------------------------
; Modifications history :
;
; AC 2017-Feb
; hierarchy : Wide (default) > verbose > short
;
; AC 2017-03-09 : 
; - now testing if inputs are defines (ISA() != 0)
; - if type of "nb_pbs" is String, then used it as a message
;
; AC 2018-feb-07 : for old IDL and GDL, ISA() not existing or too limited
;
; ----------------------------------------------------
;
function ISA_IDL, var, string=string, number=number, complex=complex
;
FORWARD_FUNCTION ISA
;
; NB: this will be obsolete for IDL >= 10
;
majeur=STRMID(!VERSION.RELEASE,0,1)
mineur=STRMID(!VERSION.RELEASE,2,1)
;
; >= 8.4
if ((majeur GE 8) and (mineur GE 4)) then begin
   return, ISA(var, string=string, number=number, complex=complex)
endif
;
; ISA() since 8.1 provides /Number
;
if KEYWORD_SET(number) then begin
   if ((majeur GE 8) and (mineur GE 1)) then begin
      return, ISA(var, number=number)
   endif
endif
;
; old general case : we have to manage all keywords ourselfs
;
if KEYWORD_SET(number) then begin
   if SIZE(var,/type) GT 0 and SIZE(var,/type) LT 7 then return, 1
   if SIZE(var,/type) EQ 9 then return, 1
   if SIZE(var,/type) GE 12 then return, 1
   return, 0
endif
;;
if KEYWORD_SET(complex) then begin
   if SIZE(var,/type) EQ 6 or SIZE(var,/type) EQ 9 then return, 1 else return, 0
endif
;;
if KEYWORD_SET(string) then begin
   if SIZE(var,/type) EQ 7 then return, 1 else return, 0
endif
;;  last case ! 
if SIZE(var, /type) GT 0 then return, 1 else return, 0 
;
end
;
; ----------------------------------------------------
;
function ISA_INTERNAL, var, string=string, number=number, complex=complex
;
FORWARD_FUNCTION ISA
;
if (GDL_IDL_FL() EQ 'IDL') then begin
   return, ISA_IDL(var, string=string, number=number, complex=complex)
endif else begin
   ;; 'GDL' or 'FL' : 
   return, ISA(var, string=string, number=number, complex=complex)
endelse
;
end
;
;------------------------------------------------
;
pro BANNER_FOR_TESTSUITE, case_name, nb_pbs, prefix=prefix, $
                          short=short, wide=wide, line=line, noline=noline, $
                          help=help, test=test, verbose=verbose
;
;verbose=1
;
mess1='First mandatory param: procedure name (string)'
mess2='Second mandatory param: errors number (>=0 integer) or string message'
mess3=' UNDEFINED !'
;
if KEYWORD_SET(help) then help=1 else help=0
;
if (N_PARAMS() NE 2) then begin
   MESSAGE, /continue, mess1
   MESSAGE, /continue, mess2
   help=1
endif else begin
   if ~ISA_INTERNAL(case_name) then help=100
   if ~ISA_INTERNAL(case_name,/string) then help=101
   if ISA_INTERNAL(case_name,/string) then if N_ELEMENTS(case_name) GT 1 then help=102   
   ;;
   if ~ISA_INTERNAL(nb_pbs) then help=200
   if ISA_INTERNAL(nb_pbs) then if N_ELEMENTS(nb_pbs) GT 1 then help=216
   if ~ISA_INTERNAL(nb_pbs,/number) AND ~ISA_INTERNAL(nb_pbs,/string) then help=213
   if ISA_INTERNAL(nb_pbs,/number) then if (FLOOR(nb_pbs) LT 0) then help=214
   if ISA_INTERNAL(nb_pbs,/complex) then help=215
   ;;
   if help GT 0 then code=STRCOMPRESS('(code '+STRING(help)+') : ')
   ;;
   if help EQ 100 then MESSAGE, /continue, code+mess1+mess3
   if help EQ 101 then MESSAGE, /continue, code+'first param must be a String input'
   if help EQ 102 then MESSAGE, /continue, code+'first param must be a scalar-like String input'
   ;;
   if help EQ 200 then MESSAGE, /continue, code+mess2+mess3
   if help EQ 213 then MESSAGE, /continue, code+'second param must be a positive integer or a string'
   if help EQ 214 then MESSAGE, /continue, code+'Number of errrors must be >= 0 !'
   if help EQ 215 then MESSAGE, /continue, code+'second param as a complex type not OK !'
   if help EQ 216 then MESSAGE, /continue, code+'second param must be a scalar-like input'
endelse
;
if (help GT 0) then begin
   print, ''
   print, 'pro BANNER_FOR_TESTSUITE, case_name, nb_pbs, prefix=prefix, $'
   print, '                          short=short, wide=wide, line=line, noline=noline, $'
   print, '                          help=help, test=test, verbose=verbose'
   print, ''
   print, 'both parameters are mandatory, second one must be a positive integer or a string'
   return
endif
;
; to avoid any residual troubles if inputs as one-element arrays
if SIZE(case_name,/n_dim) EQ 1 then case_name=case_name[0]
if SIZE(nb_pbs,/n_dim) EQ 1 then nb_pbs=nb_pbs[0]
;
if ~KEYWORD_SET(prefix) then begin
   prefixe='% '+STRUPCASE(case_name)+': '
endif else begin
   prefixe='% '+STRUPCASE(prefix)+': '   
endelse
indent="  "
;
if ISA_INTERNAL(nb_pbs,/number) then begin
   message=' errors encountered during '+STRUPCASE(case_name)+' tests'
   if (nb_pbs GT 0) then begin
      message=STRCOMPRESS(STRING(nb_pbs),/remove_all)+message
   endif else begin
      message='NO'+message
   endelse
endif else begin
   ;; when we pass a String
   message=nb_pbs
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
ligne=""
vide=" "
blanc=""
for ii=0,lenght-1 do begin
   ligne=ligne+sep
   blanc=blanc+vide
endfor
;
if KEYWORD_SET(line) then begin
   print, prefixe, sep+ligne+sep 
endif else begin
   if ~KEYWORD_SET(noline) then print, prefixe, sep+ligne+sep
   if (isWide) then print, prefixe, sep+blanc+sep
   print, prefixe, sep+message+sep
   if (isWide) then print, prefixe, sep+blanc+sep
   if ~KEYWORD_SET(noline) then print, prefixe, sep+ligne+sep
endelse
;
if KEYWORD_SET(test) then STOP
;
end
