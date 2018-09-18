; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
; part of GNU Data Language - GDL 
;
; Revised by AC, 21-Nov-2016 (case N_PARAMS() == 1 and /cov)
; https://sourceforge.net/p/gnudatalanguage/bugs/714/
; from example in Coyote Web Site
; http://www.idlcoyote.com/code_tips/pca.html
;
; Note by AC: this code is clearly not finished ...
; Complex cannot be used as input
;
function CORRELATE, x, y, covariance=covariance, double=double
;
ON_ERROR, 2
;
if N_PARAMS() eq 2 then begin
    ;;
    if ISA(x,/complex) OR ISA(y,/complex) then begin
        MESSAGE, /continue, 'Complex type for input not ready, please contribute.'
    endif
    ;;
    l = N_ELEMENTS(x) < N_ELEMENTS(y)
    ;;
    mx = MEAN(x[0:l-1], double=double)
    my = MEAN(y[0:l-1], double=double)
    cov = TOTAL((x - mx) * (y - my)) / (l - 1.)
    ;;
    if KEYWORD_SET(covariance) then return, cov
    ;;
    sx = SQRT(TOTAL((x[0:l-1] - mx)^2, double=double) / (l - 1.))
    sy = SQRT(TOTAL((y[0:l-1] - my)^2, double=double) / (l - 1.))
    return, cov / sx / sy
    ;;
endif else if N_PARAMS() eq 1 then begin
    ;;
    if ISA(x,/complex) OR ISA(y,/complex) then begin
        MESSAGE, /continue, 'Complex type for input not ready, please contribute.'
    endif
    ;;
    if (SIZE(x))[0] ne 2 then MESSAGE, 'Expecting two-dimensional array'
    ;;
    dbl = KEYWORD_SET(double) || SIZE(x, /type) eq 5
    nx = (SIZE(x))[1]
    cov = dbl ? dblarr(nx, nx, /nozero) : fltarr(nx, nx, /nozero)
    ;;
    ;; one option would be to call CORRELATE a couple of time...
    ;; but the code below should be faster
    ;;for i = 0, nx - 1 do for j = 0, nx - 1 do $
    ;;  cov[i, j] = CORRELATE(x[i, *], x[j, *], double=double, covariance=covariance)
    ;;
    tmp = dbl ? dblarr(nx, /nozero) : fltarr(nx, /nozero)
    ;;
    ;; AC : obsolete form
    ;; for i = 0, nx - 1 do tmp[i] = mean(x[i, *], double=double)
    dims = SIZE(x, /DIMENSIONS)
    tmp = TOTAL(x, 2)/dims[1]
    ;;
    for i = 0, nx - 1 do begin
        for j = 0, nx - 1 do begin
            cov[i, j] = total((x[i, *] - tmp[i]) * (x[j, *] - tmp[j])) 
        endfor
    endfor
    ;;
    if KEYWORD_SET(covariance) then return, cov / (dims[1]-1)
    ;;
    for i = 0, nx - 1 do tmp[i, *] = SQRT(TOTAL((x[i, *] - tmp[i])^2, double=double) / (nx - 1.))
    for i = 0, nx - 1 do begin
        cov[i, *] /= tmp[i]
        cov[*, i] /= tmp[i]
    endfor
    return, cov    
endif 
;
MESSAGE, 'Incorrect number of arguments.'
;
end
