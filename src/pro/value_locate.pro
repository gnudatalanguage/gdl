;+
; NAME:
;   VALUE_LOCATE
;
; AUTHOR:
;   Richard Schwartz, richard.schwartz@gsfc.nasa.gov
;   Documentation taken from Craig Markwardt's version.
; PURPOSE:
;
;   Locate one or more values in a reference array (IDL LE 5.2 compatibility)
;
; CALLING SEQUENCE:
;
;   INDICES = VALUE_LOCATE(REF, VALUES)
;
; DESCRIPTION: 
;
;   VALUE_LOCATE locates the positions of given values within a
;   reference array.  The reference array need not be regularly
;   spaced.  This is useful for various searching, sorting and
;   interpolation algorithms.
;
;   The reference array should be a monotonically increasing or
;   decreasing list of values which partition the real numbers.  A
;   reference array of NBINS numbers partitions the real number line
;   into NBINS+1 regions, like so:
;
;
; REF:           X[0]         X[1]   X[2] X[3]     X[NBINS-1]
;      <----------|-------------|------|---|----...---|--------------->
; INDICES:  -1           0          1    2       3        NBINS-1
;
;
;   VALUE_LOCATE returns which partition each of the VALUES falls
;   into, according to the figure above.  For example, a value between
;   X[1] and X[2] would return a value of 1.  Values below X[0] return
;   -1, and above X[NBINS-1] return NBINS-1.  Thus, besides the value
;   of -1, the returned INDICES refer to the nearest reference value
;   to the left of the requested value.
;
;   If the reference array is monotonically decreasing then the
;   partitions are numbered starting at -1 from the right instead (and
;   the returned INDICES refer to the nearest reference value to the
;   *right* of the requested value).  If the reference array is
;   neither monotonically increasing or decreasing the results of
;   VALUE_LOCATE are undefined.
;
;   VALUE_LOCATE appears as a built-in funcion in IDL v5.3 and later.
;   This version of VALUE_LOCATE should work under IDL v4 and later,
;   and is intended to provide a portable solution for users who do
;   not have the latest version of IDL.  The algrorithm in this file
;   is slower but not terribly so, than the built-in version.
;
;   Users should be able to place this file in their IDL path safely:
;   under IDL 5.3 and later, the built-in function will take
;   precedence; under IDL 5.2 and earlier, this function will be used.
;
; INPUTS:
;
;   REF - the reference array of monotonically increasing or
;         decreasing values.
;
;   VALUES - a scalar value or array of values to be located in the
;            reference array.
;
;
; KEYWORDS:
;
;   L64 -  for compatibility with built-in version. 
;
;  
; RETURNS:
;
;   An array of indices between -1L and NBINS-1.  If VALUES is an
;   array then the returned array will have the same dimensions.
;
;
; EXAMPLE:
;
;   Cast random values into a histogram with bins from 1-10, 10-100,
;   100-1000, and 1000-10,000.
;
;     ;; Make bin edges - this is the ref. array
;     xbins = 10D^dindgen(5)  
;
;     ;; Make some random data that ranges from 1 to 10,000
;     x     = 10D^(randomu(seed,1000)*4)
;
;     ;; Find the bin number of each random value
;     ii    = value_locate(xbins, x)
;
;     ;; Histogram the data
;     hh    = histogram(ii)
;
;
; SEE ALSO:
;
;   VALUE_LOCATE (IDL 5.3 and later), HISTOGRAM, CMHISTOGRAM
;
;
; MODIFICATION HISTORY:
;   Written and documented, 7-aug-2006
;   Case of XBINS having only one element, CM, 29 Apr 2001
;   Handle case of VALUES exactly hitting REF points, CM, 13 Oct 2001
;   19-Oct-2006 - modified to return array with same dimensions as
;    second argument as with the IDL intrinsic
;   1-nov-2006, ras, protect against differing input dimensions
;	causing concatenation problems
; 
; AC 19 May 2014:
;   - I checked that is is OK with idl 8.2.2 change
;     (e.g.: print, VALUE_LOCATE([0], [-2,-1,0,3,4]))
;   - just a warning when array clearly not monotoneous (results are
;     not OK)
;   - few type conversions/test ...
;
;-
; Copyright (C) 2006, richard schwartz
; This software is provided as is without any warranty whatsoever.
; Permission to use, copy, modify, and distribute modified or
; unmodified copies is granted, provided this copyright and disclaimer
; are included unchanged.
;-

FUNCTION IS_DEFINED, var 
; debug,'V1.0 FH 1998-01-20' 
a = SIZE(var) 
n = N_ELEMENTS(a) 
RETURN, a[n - 2] NE 0 
END
;
FUNCTION IS_SCALAR, var 
; debug,'V1.0 FH 1998-01-23' 
RETURN, ((SIZE(var))[0] EQ 0) AND is_defined(var) 
END
;
; -------------------------------------------
;
function VAL_LOC_INC, x, uu, l64=l64
;
on_error, 2
;
; AC: I don't know how to without copy of input
u=uu
if (SIZE(uu,/type) EQ 6) then u=FLOAT(uu)
if (SIZE(uu,/type) EQ 9) then u=DOUBLE(uu)
;
maxlong = 2LL^31-1

y64 = (n_elements(x) gt maxlong) or keyword_set(l64)
one = y64 ? 1LL : 1L
nx  = n_elements(x) + one
nu  = n_elements(u)*one
mm  = (last_item(x) > max(u) )*1.01
xx  = [x[*],mm[*]]
c   = [xx, u[*]]

ord = sort(c)
d   = [-1,where( ord lt nx), nx]
out = y64? lon64arr(nu) : lonarr(nu)

;for i=0,(nx-one)+1 do begin
;	if d[i+1] gt d[i] then out[ord[d[i]+1:d[i+1]-1]-nx] = i-1

j0  = d+1
j1  = d[1:*]-1
nouti = j1-j0+1
nouts = total(nouti,/cum)

for i=(one-one), (nx-one)+1 do $

	if nouti[i] ge 1 then out[ord[j0[i]:j1[i]]-nx]= i-1
;check boundaries
nlow = 1
itst = 0
while nlow ge 1 do begin
	bound = where( xx[out+1] le u and xx[out+1] eq u, nlow)
	if nlow ge 1 then out[bound]=out[bound]+1 ;< (nx-2)
	;stop
	itst = itst + 1
	if itst gt 999999L then message,'Boundary Check Failed'
	endwhile
	;print, itst
out = out < (nx-2)
if (size(u))[0] eq 0 then out = out[0]
return, out
end
;
; ---------------------------------------
;
function VALUE_LOCATE, x, u, l64=l64
;
;increasing or decreasing
;default, l64,0
;
if (KEYWORD_SET(l64) EQ 0) then l64 = 0
;
if SIZE(x,/type) EQ 0 then MESSAGE, 'First variable is undefined.'
if SIZE(u,/type) EQ 0 then MESSAGE, 'Second variable is undefined.'
;
; check on input type
;
type_x=SIZE(x,/type)
if ((type_x EQ 6) or ((type_x GE 8) and (type_x LE 11))) then $
   MESSAGE, 'First variable : '+SIZE(x,/tname)+' not allowed in this context.'
;
type_u=SIZE(u,/type)
if ((type_u EQ 8) or (type_u EQ 10) or (type_u EQ 11)) then $
   MESSAGE, 'Second variable : '+SIZE(u,/tname)+' not allowed in this context.'
;
; warning if array is not monotomeous (GDL extension)
;
if N_ELEMENTS(x) GT 1 then begin
   diff=x-SHIFT(x,1)
   maxi=MAX(diff[1:*])
   mini=MIN(diff[1:*])
   if (maxi*mini LT 0.0) then begin
      txt='Warning : input array is NOT monotonically increasing or decreasing'
      MESSAGE,/continue, txt
   endif
endif
;
out = IS_SCALAR(u) ? 0 : MAKE_ARRAY( long = 1-l64, l64 = l64, dim=SIZE(/dim, u))
temp = (last_item(x) lt x[0]) ? N_ELEMENTS(x)-2- VAL_LOC_INC(reverse(x), u, l64=l64) : $
	VAL_LOC_INC(x, u, l64=l64)

out = out + TEMPORARY( temp)
return, out
end
