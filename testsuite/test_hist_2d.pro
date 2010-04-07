; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>

pro ensure_equal, checkno, a, b
  if ~array_equal(size(a), size(b)) || ~array_equal(a, b, /no_typeconv) then begin
    message, "check " + checkno + " failed", /conti
    exit, status=1
  endif
end

pro test_hist_2d
  
  message, 'basic tests...', /conti
  ensure_equal, '01', hist_2d([1],[1]), [[0,0],[0,1l]]                     ; \
  ensure_equal, '02', hist_2d([0],[1],max1=.1), [[0],[1l]]                 ;  |- dimensions check
  ensure_equal, '03', hist_2d([1],[0],max2=.1), reform([0,1l],2,1)         ; /
  ensure_equal, '04', hist_2d([1,1],[1,1,1]), [[0,0],[0,2l]]               ; - non-equal array length
  ensure_equal, '05', hist_2d(dist(2),dist(2),max2=1), [[1,0],[0,2l]]      ; - max2 kw check
  ensure_equal, '06', hist_2d(dist(2),dist(2),max2=2), [[1,0],[0,3],[0,0l]]; - max2 kw check
  ensure_equal, '07', hist_2d(dist(2),dist(2),max1=1), [[1,0],[0,2l]]      ; - max1 kw check
  ensure_equal, '08', hist_2d(dist(2),dist(2),max1=2), [[1,0,0],[0,3,0l]]  ; - max1 kw check
  ensure_equal, '09', hist_2d(dist(2),dist(2),bin1=2), [[1],[3l]]          ; - bin1 kw check
  ensure_equal, '10', hist_2d(dist(2),dist(2),bin2=2), reform([1,3l],2,1)  ; - bin2 kw check

  ; result should always be a 2D long array
  data = [ $
    ptr_new(1b), $
    ptr_new([-3,4]), $
    ptr_new(-10 + dindgen(20)), $
    ptr_new(findgen(2,3,1,1,3)), $
    ptr_new(randomn(seed, 3, 3)), $
    ptr_new(1000*randomu(seed, 2,1,1,1,2)), $
    ptr_new([1, !VALUES.D_NAN, !VALUES.D_NAN]), $
    ptr_new([-10,-9]) $
  ]
  bins = [ptr_new(1b), ptr_new(.1)]
  mins = [ptr_new(10b), ptr_new(-.1)]
  maxs = [ptr_new(12b), ptr_new(13.)]
  for i = 0, n_elements(data) - 1 do begin; first data array
    message, 'case ' + strtrim(string(i + 1), 2) + ' / ' + strtrim(string(n_elements(data)), 2), /conti
    for j = 0, n_elements(data) - 1 do begin; second data array
      for mi1=0,1 do for mi2=0,1 do for ma1=0,1 do for ma2=0,1 do for bi1=0,1 do for bi2=0,1 do begin
        for bare = 0, 1 do begin; with optional arguments or not
          a = bare $
            ? hist_2d(*(data[i]), *(data[j])) $
            : hist_2d(*(data[i]), *(data[j]), $
              min1=*(mins[mi1]), $
              min2=*(mins[mi2]), $
              max1=*(maxs[ma1]), $
              max2=*(maxs[ma2]), $
              bin1=*(bins[bi1]), $ 
              bin2=*(bins[bi2]) $
            )
          if (size(a))[0] ne 2 || size(a,/type) ne 3 then begin
            message, "ERROR", /conti
            exit, status=1
          endif
        endfor
      endfor
    endfor
  endfor

  for i = 0, n_elements(data) - 1 do ptr_free, data[i]
  for i = 0, n_elements(mins) - 1 do ptr_free, mins[i]
  for i = 0, n_elements(maxs) - 1 do ptr_free, maxs[i]
  for i = 0, n_elements(bins) - 1 do ptr_free, bins[i]

end
