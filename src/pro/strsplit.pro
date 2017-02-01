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
;   01-Feb-2016 : rewrote after change in STRTOK to handle arrays.
;
; LICENCE:
; Copyright (C)
; 2004, Pierre Chanial
; 2010, Alain Coulais and Lea Noreskal; 2012 :AC
; 2013, Marc Schellens
; 2016, Gilles Duvert
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
function STRSPLIT, strin, pattern, count=count, length=length, extract=extract, _REF_EXTRA=extra
; note: _REF_EXTRA is *really* important here.
;
  compile_opt idl2, hidden
  ON_ERROR, 2
;
  if KEYWORD_SET(help) then begin
     print, 'function STRSPLIT, strin, pattern, count=count, length=length, $'
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
  n = N_ELEMENTS(strin) 
  dolength = ARG_PRESENT(length)
  if N GT 1 then begin
                                ; new: array input. result is a list,
                                ; length is a list and count is an
                                ; array
    nel=n_elements(pattern)
    if (nel gt 1 and nel ne n) then message, 'PATTERN must be a scalar or have the same number of elements as STRING.'
    if (nel eq 1 ) then pattern=replicate(pattern,n)

    out=list()
    count=lonarr(n)
    if (dolength) then length = list()

    for i=0,n-1 do begin
     if (n_params() eq 1) then tmp=strtok(strin[i], count=cnt, length=len, extract=extract, _strict_extra=extra) $
     else  tmp=strtok(strin[i], pattern[i], count=cnt, length=len, extract=extract, _strict_extra=extra)
     out.add,tmp
     count[i]=cnt
     if (dolength) then length.add,len
    endfor
    return, out
 endif else begin
; scalar
     if (n_params() eq 1) then out=strtok(strin, count=count, length=length, extract=extract, _strict_extra=extra) $
     else  out=strtok(strin, pattern, count=count, length=length, extract=extract, _strict_extra=extra)
     return,out
 endelse
end
