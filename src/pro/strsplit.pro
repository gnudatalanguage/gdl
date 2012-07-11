;+
;
; NAME:       STRSPLIT
;
; PURPOSE:    STRTOK wrapper
;
; CATEGORY:   String Manipulation
;
; CALLING SEQUENCE:  Same as STRTOK
;
;
; MODIFICATION HISTORY:
;   17-Jul-2005 : written by Pierre Chanial
;   30-Jun-2009 : Alain Coulais : will allow 1D string : 'string' and ['string']
;   14-Apr-2010 : Alain Coulais : complete re-writing
;      Jun-2010 : Lea Noreskal : /preserve_null and other improvments
;   15-Oct-2010 : Alain Coulais : bug in COUNT, 
;                 activating no exclusive keywords,
;                 better managment of (missing/void) inputs
;   11-Aug-2011 : Alain Coulais : solving conflits due to
;                 /preserve_null and /regex; curing bugs in special cases
;   11-Jul-2012 : When /extract, we must return STRARR even for 1-element
;
; LICENCE:
; Copyright (C)
; 2004, Pierre Chanial
; 2010, Alain Coulais and Lea Noreskal
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
;-
;
function STRMULTIPOS, str, single_char, test=test
ON_ERROR, 2
if (SIZE(str, /type) NE 7) OR (SIZE(single_char, /type) NE 7) then begin
   MESSAGE, 'Invalid input string.'
   return, -1
endif
if (STRLEN(single_char) NE 1) then begin
   MESSAGE, 'field2 must be a Single Char'
   return, -1
endif
;
resu=-1
;
for ii=0, STRLEN(str)-1 do begin
   sub_str=STRMID(str,ii,1)
   if (sub_str EQ single_char) then resu=[resu,ii]
endfor
;
if N_ELEMENTS(resu) GT 1 then resu=resu[1:*]
;
if KEYWORD_SET(test) then STOP
;
return, resu
;
end
;
; ----------------------------------------
;
function STRSPLIT, input1, input2, count=count, length=length, $
                   extract=extract, regex=regex, escape=escape, $
                   fold_case=fold_case, preserve_null=preserve_null, $
                   test=test, help=help
;
ON_ERROR, 2
;
if KEYWORD_SET(help) then begin
   print, 'function STRSPLIT, input1, input2, count=count, length=length, $'
   print, '                   extract=extract, regex=regex, escape=escape, $'
   print, '                   fold_case=fold_case, preserve_null=preserve_null, $'
   print, '                   test=test, help=help'
   return, -1
endif
;
if N_PARAMS() EQ 0 then begin
   MESSAGE, 'Variable is undefined: STRINGIN'
endif
;
if N_ELEMENTS(input1) EQ 0 then MESSAGE, 'Variable is undefined: STRINGIN.'
if N_ELEMENTS(input1) GT 1 then $
   MESSAGE, 'Expression must be a scalar or 1 element array in this context: STRINGIN.'
;
if (N_PARAMS() EQ 2) then begin
   if N_ELEMENTS(input2) EQ 0 then MESSAGE, 'Variable is undefined: PATTERN.'
endif
;
if KEYWORD_SET(escape) then begin
   MESSAGE, /continue, 'Sorry, this keyword /ESCAPE is not implemented (no effect)'
   MESSAGE, /continue, 'PLEASE CONTRIBUTE'
endif
;
if KEYWORD_SET(fold_case) then begin
   MESSAGE, /continue, 'Sorry, this keyword /FOLD_CASE is not implemented (no effect)'
   MESSAGE, /continue, 'PLEASE CONTRIBUTE'
endif
;
;; AC 14-Oct-2010: may be not fully OK (see below)
;if KEYWORD_SET(regex) and KEYWORD_SET(preserve_null) then begin
;    MESSAGE, /continue, 'Keywords /REGEX and /PRESERVE_NULL are incompatible, /PRESERVE_NULL is ignored' 
;endif
;
if (SIZE(input1, /type) NE 7) then begin
   local_input1=STRING(input1)
   ;;MESSAGE, 'Invalid input string.'
endif else begin
   local_input1=input1
endelse
;
; When no Pattern is provided, default pattern is white space (' ')
;
short_cut=0
;
if (STRLEN(local_input1) EQ 0) then begin
   short_cut=1
   if KEYWORD_SET(extract) then resu='' else resu=0
endif
if (N_PARAMS() EQ 2) then begin
   if (STRLEN(input2) EQ 0) then begin
      short_cut=1
      if KEYWORD_SET(extract) then resu='' else resu=0
   endif
endif
;
; When no Pattern is provided, default pattern is white space (' ')
;
if ((short_cut EQ 0) AND (N_PARAMS() EQ 1)) then begin
   resu=STRTOK(local_input1, extract=extract, preserve_null=preserve_null)
endif
;
if ((short_cut EQ 0) AND (N_PARAMS() EQ 2)) then begin
   ;;
   ;; AC 14-Oct-2010: may be not fully OK
   if KEYWORD_SET(regex) then begin
      resu=STRTOK(local_input1, input2, extract=extract,$
                  REGEX=regex, preserve_null=preserve_null)
   endif else begin
      resu=0
      beg=0
      ;;
      for ii=0, STRLEN(input2)-1 do begin
         resu=[resu, STRMULTIPOS(local_input1, STRMID(input2, ii, 1))]
      endfor
      ;;
      resu=resu[WHERE(resu GE 0)]
      tst=resu[WHERE(resu EQ 0)]
      resu=resu[UNIQ(resu,SORT(resu))]
      ;;
      if N_ELEMENTS(tst) EQ 2 then beg=1
      ;;
      if KEYWORD_SET(extract) then begin
         if (beg eq 1) then resu=[0,resu]
         if N_ELEMENTS(resu) EQ 1 then begin
            if (beg eq 0) then begin 
               resu=local_input1
            endif else begin
               resu=STRMID(local_input1, resu[0]+1)
            endelse
         endif else begin            
            sresu=STRARR(N_ELEMENTS(resu))
            if (beg eq 0) then begin 
               sresu[0]=STRMID(local_input1, 0, resu[1])
            endif else begin
               sresu[0]=STRMID(local_input1, resu[0]+1, resu[1]-resu[0]-1)
            endelse
            
            for ii=1, N_ELEMENTS(resu)-2 do begin
               ;;print, resu[ii]+1,resu[ii+1]-resu[ii]-1                    
               sresu[ii]=STRMID(local_input1, resu[ii]+1,resu[ii+1]-resu[ii]-1)
            endfor
            sresu[N_ELEMENTS(resu)-1]=STRMID(local_input1, resu[N_ELEMENTS(resu)-1]+1)
            ;;stop
            resu=sresu
         endelse
         ;;
         if NOT(KEYWORD_SET(preserve_null)) then begin
            ok=WHERE(STRLEN(resu) GT 0, nb_ok)
            if (nb_ok GT 0) then resu=resu[ok] else resu=''
         endif
         ;; going back to the case /extract not set 
      endif else begin
         if N_ELEMENTS(resu) GT 1 then resu[1:*]=resu[1:*]+1 else resu=0
         if (beg EQ 1) then resu[0]=resu[0]+1 

         if (KEYWORD_SET(preserve_null) and (N_ELEMENTS(resu) GT 1) and (resu[0] ne 0)) then resu=[0,resu]

         if NOT(KEYWORD_SET(preserve_null)) then begin
            refresu=resu
            resu=-1

            for ii=0, N_ELEMENTS(refresu)-2 do begin
               if ((refresu[ii+1]-refresu[ii]) ne 1) then resu=[resu,refresu[ii]]
            endfor


            if (refresu[N_ELEMENTS(refresu)-1] lt STRLEN(local_input1)) then resu=[resu,refresu[N_ELEMENTS(refresu)-1]]
            if (N_ELEMENTS(resu) eq 1 ) then resu=0 else resu=resu[WHERE(resu GE 0)]
            
         endif
      endelse
   endelse
endif
;
if  ARG_PRESENT(count) then begin
   if (short_cut EQ 1) then begin
      count=0
   endif else begin
      count=N_ELEMENTS(resu)
   endelse
endif
;
if ARG_PRESENT(length) then length=STRLEN(resu)
;
if KEYWORD_SET(test) then STOP
;
if (SIZE(resu,/type) NE 7) then begin
   resu=LONG(resu)
endif else begin
   ;; when we have a non null (not '') string singleton
   ;; we must return an array
   if (SIZE(resu,/n_dim) EQ 0) then begin
      if (STRLEN(resu) GT 0) then resu=REFORM(resu,1)
   endif
endelse
;
if KEYWORD_SET(test) then STOP
;
return, resu
;
end
