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
;   14-Aug-2012 : Now GDL enforces scalar type in FOR loop ... take care
;   of STRLEN ! We ensure to work on pure STRING = '', not STRING = Array[1]
;   25-Jul-2013 : After fixing STRTOK this simplified version should
;   do.
;   01-Aug-2013 : Patch for bug #554, insure LENGTH and EXTRACT are
;   not present simultaneously, return correct length in cases such as
;   strsplit('aaaaaaaaaaaaaaaaaaaaaa','b',leng=leng) and 
;   strsplit('aaaaaaaaaaaaaaaaaaaaaa','a',leng=leng).
;
; LICENCE:
; Copyright (C)
; 2004, Pierre Chanial
; 2010, Alain Coulais and Lea Noreskal; 2012 :AC
; 2013, Marc Schellens
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
;-
;
;
; ----------------------------------------
;
function STRSPLIT, input1, pattern, count=count, length=length, $
                   extract=extract, regex=regex, escape=escape, $
                   fold_case=fold_case, preserve_null=preserve_null, $
                   test=test, help=help
;
ON_ERROR, 2
;
if KEYWORD_SET(help) then begin
   print, 'function STRSPLIT, input1, pattern, count=count, length=length, $'
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
   if N_ELEMENTS(pattern) EQ 0 then MESSAGE, 'Variable is undefined: PATTERN.'
endif
;
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
if ARG_PRESENT(length) and KEYWORD_SET(extract) then MESSAGE, "Conflicting keywords."
; we explicitely change String [1] array into pure String.
local_input1=local_input1[0]
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
   if (STRLEN(pattern) EQ 0) then begin
      short_cut=1
      if KEYWORD_SET(extract) then resu='' else resu=0
   endif else begin
      local_pattern=pattern[0]
   endelse
endif
;
; When no Pattern is provided, default pattern is white space (' ' and
; tab)
;
if ((short_cut EQ 0) AND (N_PARAMS() EQ 1)) then begin
   resu=STRTOK(local_input1, extract=extract,$
               REGEX=regex, preserve_null=preserve_null, escape=escape, fold_case=fold_case)
; 2nd pass for length
if ARG_PRESENT(length) then  temp=STRTOK(local_input1, LENGTH=length, REGEX=regex, preserve_null=preserve_null, escape=escape, fold_case=fold_case)
endif
;
if ((short_cut EQ 0) AND (N_PARAMS() EQ 2)) then begin
   resu=STRTOK(local_input1, local_pattern, extract=extract,$
               REGEX=regex, preserve_null=preserve_null, escape=escape, fold_case=fold_case)
; 2nd pass for length.
 if ARG_PRESENT(length) then temp=STRTOK(local_input1, local_pattern, LENGTH=length, REGEX=regex, preserve_null=preserve_null, escape=escape, fold_case=fold_case)
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
