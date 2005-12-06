;+
; NAME:
;   CMSV_PTRSUM
;
; AUTHOR:
;   Craig B. Markwardt, NASA/GSFC Code 662, Greenbelt, MD 20770
;   craigm@lheamail.gsfc.nasa.gov
;
; PURPOSE:
;   Construct an inventory of heap data accessible to a variable
;
; CALLING SEQUENCE:
;   CMSV_PTRSUM, VAR, LIST
;   
; DESCRIPTION: 
;
;   This procedure constructs an inventory of heap data that is
;   accessible to a single variable.  It searches all array elements,
;   recursively through structure tags, and by dereferencing pointers.
;   Users can use this procedure to determine all heap variables that
;   need to be saved to disk.
;
;   This procedure is part of the CMSVLIB SAVE library for IDL by
;   Craig Markwardt. 
;
; ==================================================================
;   Research Systems, Inc. has issued a separate license intended
;   to resolve any potential conflict between this software and the
;   IDL End User License Agreement. The text of that license
;   can be found in the file LICENSE.RSI, included with this
;   software library.
; ==================================================================
;
; INPUTS:
;
;   VAR - the variable to be examined
;
;   LIST - upon output, an array of pointers, each of which points to
;          a heap variable accessible to VAR.  If there are no heap
;          data pointed to by VAR, then LIST returns a NULL value.
;
; KEYWORDS:
;
;   NULL - if set, return the null value in LIST instead of the
;          pointer list.  VAR is ignored.
;
;   HAS_OBJECTS - upon return, the value is 1 if VAR contains or
;                 points to an object reference, and 0 if not.
;
;
; EXAMPLE:
;
;
; SEE ALSO:
;
;   CMRESTORE, SAVE, RESTORE, CMSVLIB
;
; MODIFICATION HISTORY:
;   Written, 2000
;   Documented, 24 Jan 2001
;   Make version checks with correct precision, 19 Jul 2001, CM
;   Added notification about RSI License, 13 May 2002, CM
;
; $Id: cmsv_ptrsum.pro,v 1.1 2005-12-06 13:58:47 jomoga Exp $
;
;-
; Copyright (C) 2000-2001, Craig Markwardt
; This software is provided as is without any warranty whatsoever.
; Permission to use, copy, modify, and distribute modified or
; unmodified copies is granted, provided this copyright and disclaimer
; are included unchanged.
;-
pro cmsv_ptrsum, data, result, null=null0, has_objects=hobj

  forward_function ptr_new
  common cmsv_ptrsum_data, version, null

  if n_elements(version) EQ 0 then version = double(!version.release)
  if version LT 5D then begin
      result = 0L
      return
  endif
  if n_elements(null) EQ 0 then null = ptr_new()
  if keyword_set(null0) then begin
      data = null
      return
  endif

  pd = null
  sz = size(data)
  tp = sz(sz(0)+1)
  if tp EQ 8 then begin
      for i = 0L, n_tags(data(0))-1 do begin
          cmsv_ptrsum, data.(i), x, has_objects=hobj
          sz = size(data.(i))
          if sz(sz(0)+1) EQ 11 then hobj = 1
          if n_elements(x) GT 1 then pd = [pd, x] $
          else if x(0) NE null then pd = [pd, x]
      endfor
  endif else if tp EQ 10 then begin
      pd = [pd, data(*)]
      for i = 0, n_elements(data)-1 do begin
          wh = where(data(i) EQ pd, ct)
          if ct EQ 0 then begin   ;; Prevent cycles!!!
              cmsv_ptrsum, *(data(i)), x
              sz = size(*(data(i)))
              if sz(sz(0)+1) EQ 11 then hobj = 1
              if n_elements(x) GT 1 then pd = [pd, x] $
              else if x(0) NE null then pd = [pd, x]
          endif
      endfor
  endif
  if n_elements(pd) GT 1 then pd = pd(uniq(pd, sort(pd)))

  result = pd
  return
end

