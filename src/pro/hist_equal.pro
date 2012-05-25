;+
;
; NAME:
;     HIST_EQUAL
;
; PURPOSE:
;       Scales an input image by equalising its histogram. By setting
;       a percent it can an also scale an image via a linear contrast.
;
; CATEGORY: Image processing.
;
; CALLING SEQUENCE: 
;        Result = HIST_EQUAL(image, BinSIZE=value,
;          Fcn=column_vector, Histogram_Only=value, Maxv=value,
;          Minv=value, Omax=variable, Omin=variable, Percent=value,
;          Top=value)
;
; KEYWORD PARAMETERS:
;
;       BINSIZE: The SIZE of the bin in which to construct the
;        histogram. Defaulted to 1 for an image of type BYTE. The
;        default for other data types is (maxv-minv)/5000double.
;
;       FCN: The cumulative probability distribution function. (Not
;         currently implemented).
;
;       HISTOGRAM_ONLY: Returns the cumulative histogram of type Long.
;
;       MAXV: The maximum value of the original image to
;        consider. Defaulted to 255 if image is of type BYTE,
;        otherwise the image is searched for the maximum value.
;
;       MINV: The minimum value of the original image to
;        consider. Default is 0 if image is of type BYTE, otherwise
;        the image is searched for the minimum value.
;
;       PERCENT: If set to a value between 0 and 100, the histogram of
;        the image will be linearly stretched between the upper and
;        lower values found by the specified percent of the cumulative
;        histogram.
;
;       TOP: The maximum value of the scaled result. Defaulted to 255.
;
; OUTPUTS: An image that is has been modified by its histogram. If the
;       Histogram_Only keyword has been set, then the cumulative
;       histogram (of type Long) will be output.
;
;
; OPTIONAL OUTPUTS:
;
;   OMAX: Will contain the maximum value used for constructing the histogram.
;   OMIN: Will contain the minimum value used for constructing the histogram.
;
; COMMON BLOCKS:
;
; SIDE EFFECTS:
;
; RESTRICTIONS:
;
;     The FCN keyword is currently not implemented.
;
; PROCEDURE:
;     Sources:
;     http://www.cacs.louisiana.edu/~cice/cacs508/fa11/lecture%2005%200908.pdf
;     http://www.cis.rit.edu/class/simg782/lectures/lecture_05/lec782_05_05.pdf
;
; EXAMPLE:
;       http://idlastro.gsfc.nasa.gov/idl_html_help/HIST_EQUAL.html
;       image = dist(100)
;       tv, image
;       scl_img = hist_equal(image, minv=10, maxv=200, top=220)
;       tv, scl_img
;  
;       window, 1, xSIZE=900, ySIZE=550
;       !p.multi=[0,2,1]
;       plot, histogram(image), title='Histogram'
;       oplot, histogram(scl_img), color=255
;       plot, total(histogram(image), /cumulative), title='Cumulative Histogram'
;       oplot, total(histogram(scl_img), /cumulative), color=255
;       xyouts, 0.2324805, 0.0200364, 'White = Original', /normal, charSIZE=2
;       xyouts, 0.5795328, 0.0200364, 'Red = Equalised', /normal, charSIZE=2
;
; MODIFICATION HISTORY:
;   01-May-2012 Written by:     Josh Sixsmith
;   07-May-2012 Fixed percent keyword argument
;   22-May-2012 Alain Coulais: Details (/help,/test), initial import in GDL
;   25-May-2012 Alain: typo in LINEAR_PERCENT :( , formatting
;
;-
; LICENCE:
; Copyright (C) 2010, Josh Sixsmith
; This program is free software; you can redistribute it and/or modify  
; it under the terms of the GNU General Public License as published by  
; the Free Software Foundation; either version 3 of the License, or     
; (at your option) any later version.                                   
;
;-
;
pro LINEAR_PERCENT, cumuhist, percent, min, binSIZE, maxDN, minDN
;
n = cumuhist[N_ELEMENTS(cumuhist)-1]
low = (percent/100.) 
high = (1 - (percent/100.)) 
;
x1 = VALUE_LOCATE(cumuhist, n * low)
if x1 eq -1 then x1 = 0
while cumuhist[x1] eq cumuhist[x1 + 1] do begin
    x1 = x1 + 1
endwhile
;
close1 = abs(low - cumuhist[x1]/n)
close2 = abs(low - cumuhist[x1 + 1]/n)
x1 = (close1 le close2) ? x1 : x1 + 1
minDN = x1 * binSIZE + min
;
x2 = VALUE_LOCATE(cumuhist, n * high)
while cumuhist[x2] eq cumuhist[x2 - 1] do begin
    x2 = x2 - 1
endwhile
;
close1 = ABS(high - cumuhist[x2]/n)
close2 = ABS(high - cumuhist[x2 + 1]/n)
x2 = (close1 le close2) ? x2 : x2 + 1
maxDN = x2 * binSIZE + min
;
end
;
; ------------------------------------
;
function HIST_EQUAL, img, binSIZE=bs, fcn=fcn, Histogram_Only=ho, $
                     maxv=maxv, minv=minv, omax=omax_, omin=omin_, $
                     percent=percent, Top=top_, $
                     help=help, test=test
;
ON_ERROR, 2
;
if KEYWORD_SET(help) then begin
    print, 'function HIST_EQUAL, img, binSIZE=bs, fcn=fcn, Histogram_Only=ho, $'
    print, '                     maxv=maxv, minv=minv, omax=omax_, omin=omin_, $'
    print, '                     percent=percent, Top=top_, $'
    print, '                     help=help, test=test'
    return, -1
endif
;
txt='Cumulative Probability Distribution Function not currently supported'
if KEYWORD_SET(fcn) then MESSAGE, txt
;
;    if (percent le 0) or (percent ge 100) then MESSAGE, 'Percent must be between 0 and 100'
;
if N_ELEMENTS(maxv) eq 0 then maxv = max(img, min=min_)
if N_ELEMENTS(minv) eq 0 then minv = min_
;
if N_ELEMENTS(top_) eq 1 then top = top_[0] else top = 255
;
type = SIZE(img, /type)
;
if N_ELEMENTS(bs) eq 0 then begin
    if type eq 1 then begin
        bs = 1
    endif else begin
        ;; Calculate as double. Precision needed for very small values. 
        bs = (maxv - minv)/5000d 
    endelse
endif
;
hist = HISTOGRAM(img, binSIZE=bs, omax=omax_, omin=omin_, max=maxv, min=minv)
;; Zeroing the first element of the histogram
hist[0] = 0
;
cumu = TOTAL(hist, /cumulative, /double)
if KEYWORD_SET(ho) then return, long(cumu)
;
if N_ELEMENTS(percent) gt 0 then begin
    if (percent le 0) or (percent ge 100) then MESSAGE, 'Percent must be between 0 and 100'
    LINEAR_PERCENT, cumu, percent, minv, bs, maxDN, minDN
    scl = BYTSCL(img, max=maxDN, min=minDN, top=top)
    ;;
    if KEYWORD_SET(test) then STOP
    return, scl
endif
;
bytsc = BYTSCL(cumu, top=top)
;
if SIZE(img, /type) eq 1 then begin
    ;; Check for minv. Clip to zero if non zero
    scl = (minv eq 0) ? bytsc[img] : bytsc[(img > minv) - minv]
endif else begin
    ;; Scale by the binSIZE to in order to get the bin position
    ;; Check for minv. Clip to zero if non zero
    scl = (minv eq 0) ? bytsc[img/bs] : bytsc[((img > minv) - minv)/bs]    
endelse
;
if KEYWORD_SET(test) then STOP
;
return, scl
;
end
