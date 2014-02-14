function MOMENT, x, mdev=mdev, sdev=sdev, $
                 double=double, NaN=NaN, $
                 maxmoment=maxmoment, dimension=dimension, $
                 kurtosis=kurtosis, mean=mean, $
                 skewness=skewness, variance=variance
;+
;
; NAME: MOMENT
;
; PURPOSE: 
;     Calculates the following from the input data; mean, variance,
;     skewness, kurtosis, mean absolute deviation, standard deviation
;       
; CATEGORY:
;     Mathematics: Statistics
;
; CALLING SEQUENCE: Result=MOMENT(x)
;
; KEYWORD PARAMETERS: 
;     DOUBLE    : Keyword for double precision calculation
;     NAN       : Flag to treat IEEE Special Floating-Point values as missing data
;     MAXMOMENT : Keyword for precising which moments are to be calculated
;                 1 - calculate mean 
;                 2 - calculate mean, variance, mean absolute deviation and standard dev.
;                 3 - calculate all but kurtosis
;                 4 or 0 (keyword not present) - calculate all moments
;     DIMENSION : if absent or equal to zero, compute the values
;                 (moment, stddev, variance, skewness and kurtosis)
;                 over the whole data. otherwise, compute along the related
;                 dimension.
;
; OUTPUTS:
;    Result is a 4 element array, with
;    [mean,variance,skewness,kurtosis] as the elements
;
; OPTIONAL OUTPUTS:
;     MDEV   : Named variable which will contain the mean absolute deviation
;     SDEV   : Named variable which will contain the standard deviation
;     KURTOSIS, MEAN, SKEWNESS, VARIANCE: as named !
;
; RESTRICTIONS:
;    The input x needs to be an array of numbers (i.e not strings,
;    struct, ptr, object)
;
;    some cases with NaN not ready
;       print, MOMENT([!values.f_nan,1])   
;       print, MOMENT([!values.f_nan,1],/nan)
;       print, MOMENT([!values.f_nan,!values.f_nan]) ...   
;       print, MOMENT([!values.f_nan,!values.f_nan,1])   
;       print, MOMENT([!values.f_nan,!values.f_nan,1],/nan)
;
; PROCEDURE:
;     mean = (1/N)sum(x),
;     variance = 1/(N-1) sum((x-mean(x))^2), 
;     skewness = 1/N sum((x-mean(x))^3/sqrt(variance(x))), 
;     kurtosis = 1/N sum((x-mean(x))^4/sqrt(variance(x))) - 3, 
;     mean absolute deviation = 1/N sum(abs(x-mean(x))),
;     standard deviation = sqrt(variance)
;
; EXAMPLE:
;     a=FINDGEN(100)
;     result=MOMENT(a)
;     print, result
;     49.5000    841.667     0.0000   1.73395
;
;     A preliminary test has been added in testsuite, file
;     "test_moment.pro". This test should later also test /NaN ...
;
; MODIFICATION HISTORY:
;   20-Mar-2004 : Written by Christopher Lee
;   18-Jul-2005 : Rewritten by Pierre Chanial
;   10-Aug-2009 : MAXMOMENT keyword added by Sylwester Arabas
;   14-Oct-2010 : Correcting Bug in Kurtosis (by Alain C.)
;   16-Jun-2012 : Dimension Keyword (by Mathieu Pinter)
;   14-Jul-2012 : check ASAP whether Dimension value is OK ... (by Alain C.)
;   14-Feb-2014 : GD: removed protection against zero-dim x since even zero-dim x
;                 have a mean (x) and moments (Nan).
;
; LICENCE:
; Copyright (C) 2004, Christopher Lee
;               2005, Pierre Chanial
;               2009, Sylwester Arabas
;               2010, Alain Coulais
;               2012, Mathieu Pinter and Alain Coulais
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 2 of the License, or     
; (at your option) any later version.                                   
;
;-
ON_ERROR, 2
;
;
; we don't reuse code in mean.pro, because we need variable n.
; The following seems to be able to handle zero-size arrays (singletons) correctly,
; with or without the /dimension keyword used.

if KEYWORD_SET(dimension) then begin
   ;; we check asap whether "dimension" in the good range
   if ((dimension GT SIZE(x, /N_DIMENSION)) || (dimension LT 0)) then begin
      MESSAGE, "Illegal keyword value for DIMENSION."
   endif
   dim = SIZE(x, /DIMENSION)
   if KEYWORD_SET(NaN) then begin 
      n = TOTAL(FINITE(x), dimension)
   endif else begin
      n = dim(dimension-1)
   endelse
endif else begin
   dimension = 0
   if KEYWORD_SET(NaN) THEN BEGIN
      n = TOTAL(FINITE(x), DOUBLE=double)
   endif ELSE BEGIN
      n = N_ELEMENTS(x)
   endelse
endelse
;
; if input is : print, MOMENT([1,!values.f_nan],/na)
;
; 
if ~KEYWORD_SET(maxmoment) THEN maxmoment = 4
;
if dimension EQ 0 THEN BEGIN
   ;;
   ;; get the mean value in the required type (FLOAT or DOUBLE)
   ;; subsequent operations will rely on GDL automatic type conversion
   mean = TOTAL(x, DOUBLE=double, NaN=NaN)/n
   x0   = x-mean
   ;;
   variance = maxmoment GE 2 ? TOTAL(x0^2, NaN=NaN)/(n-1)        : !VALUES.F_NAN
   sdev     = maxmoment GE 2 ? SQRT(variance)                    : !VALUES.F_NAN 
   skewness = maxmoment GE 3 ? TOTAL(x0^3, NaN=NaN)/sdev^3/n     : !VALUES.F_NAN
   kurtosis = maxmoment GE 4 ? (TOTAL(x0^4, NaN=NaN)/sdev^4)/n-3 : !VALUES.F_NAN
   result = [mean, variance, skewness, kurtosis]
endif ELSE BEGIN
   mean = TOTAL(x, dimension, DOUBLE=double, NaN=NaN)/n
   dim2 = dim
   dim2(dimension-1) = 1
   if KEYWORD_SET(double) then nan1 = DBLARR(SIZE(mean, /DIMENSION)) 
   if ~KEYWORD_SET(double) then nan1= FLTARR(SIZE(mean, /DIMENSION))
   nan1(*) = !VALUES.F_NAN
   x0 = (x-REBIN(REFORM(mean, dim2), dim))
   variance = maxmoment GE 2 ? TOTAL((x0)^2, dimension, NaN=NaN)/(n-1)        : nan1
   sdev     = maxmoment GE 2 ? SQRT(variance)                                 : nan1
   skewness = maxmoment GE 3 ? TOTAL((x0)^3, dimension, NaN=NaN)/(n*sdev^3)   : nan1
   kurtosis = maxmoment GE 4 ? TOTAL((x0)^4, dimension, NaN=NaN)/(n*sdev^4)-3 : nan1
   result = [[[mean]], [[variance]], [[skewness]], [[kurtosis]]]
endelse
;
if ARG_PRESENT(mdev) THEN BEGIN
   mdev = maxmoment GE 2 ? TOTAL(ABS(x0), dimension, NaN=NaN)/n : !VALUES.F_NAN
endif
;
return, result
; 
end
