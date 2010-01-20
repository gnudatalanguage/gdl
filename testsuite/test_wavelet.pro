; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
pro test_wavelet, plot=plot

  p = keyword_set(plot)

  n = 16     ; signal length / number of wavelets (1D case)
  nn = 8     ; dimension length (2D case)
  eps = 1e-6 ; a small number
  maxc = 20  ; GSL max: 20

  for kw_column = 0, 1 do for kw_double = 0, 1 do for kw_over = 0, 1 do $
  for dim = 1, 2 do begin

    if p then begin
      if dim eq 1 then !P.MULTI = [0, 4, n / 4] $
      else !P.MULTI = [0, nn, nn]
    endif

    for c = 4, maxc, 2 do begin
      for i = 0, (dim eq 1 ? n : nn * nn) - 1 do begin

        ; inverse-transforming a unit vector
        x1 = dim eq 1 ? fltarr(n) : fltarr(nn, nn) & x1[i] = 1
        v1 = x1
        v1 = wtn(v1, c, /inverse, double=kw_double, column=kw_column, over=kw_over)
  
        ; plotting if desired
        if p then begin
          if dim eq 1 then plot, v1 else surface, v1
        endif

        ; testing if the transform of the inverse transform equals the unit vector
        wh = where(abs(x1 - wtn(v1, c, double=kw_double, column=kw_column)) gt eps, cnt)
        if cnt gt 0 then message, 'wtn(wtn(a, /inv)) != a'

        ; testing if every wavelet (not the mother) has a zero mean
        if i gt 0 and abs(mean(v1)) gt eps then message, 'abs(mean(v1)) > eps'

        ; testing if every other member of the basis is orthogonal to this one
        for j = 0, (dim eq 1 ? n : nn * nn) - 1 do begin
          x2 = dim eq 1 ? fltarr(n) : fltarr(nn, nn) & x2[j] = 1
          v2 = x2
          v2 = wtn(v2, c, /inverse, double=kw_double, column=kw_column, over=kw_over)
          v1v2 = total(v1 * v2)
          if i eq j and v1v2 lt 1 - eps then message, 'i eq j and v1v2 < 1 - eps !!!' 
          if i ne j and v1v2 gt 0 + eps then message, 'i ne j and v1v2 > 0 + eps !!!'
        endfor ; j

      endfor ; i 

    endfor ; c

  endfor ; dim

end ; pro
