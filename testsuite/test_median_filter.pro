; Regression coverage for https://github.com/gnudatalanguage/gdl/issues/2245.

pro test_median_filter_equal, actual, expected, context
  a = double(actual)
  b = double(expected)
  nan_a = finite(a, /nan)
  nan_b = finite(b, /nan)
  bad = ~array_equal(size(actual, /dimensions), size(expected, /dimensions))
  bad = bad or ~array_equal(nan_a, nan_b)
  indices = where(nan_a, count)
  if count gt 0 then a[indices] = 0
  indices = where(nan_b, count)
  if count gt 0 then b[indices] = 0
  if bad or ~array_equal(a, b) then begin
    message, 'MEDIAN filter mismatch: ' + context, /continue
    print, 'Actual: ', actual
    print, 'Expected: ', expected
    exit, status=1
  endif
end

function test_median_filter_value, window, even=even
  sorted = double(window)
  valid = where(~finite(sorted, /nan), count)
  if count eq 0 then return, !values.d_nan
  sorted = sorted[valid]
  sorted = sorted[sort(sorted)]
  count = n_elements(sorted)
  middle = count / 2
  if keyword_set(even) and (count mod 2 eq 0) then $
    return, (sorted[middle-1] + sorted[middle]) / 2
  return, sorted[middle]
end

; Sort each complete window as an independent reference.
; IDL leaves WIDTH/2 elements unchanged at both ends of each dimension,
; including for even widths.
function test_median_filter_reference, data, width, even=even
  result = double(data)
  dims = size(data, /dimensions)
  wx = width[0]
  hx = wx / 2
  if n_elements(dims) eq 1 then begin
    for x = hx, dims[0] - hx - 1 do $
      result[x] = test_median_filter_value(data[x-hx:x-hx+wx-1], even=even)
  endif else begin
    wy = width[n_elements(width)-1]
    hy = wy / 2
    for y = hy, dims[1] - hy - 1 do begin
      for x = hx, dims[0] - hx - 1 do $
        result[x,y] = test_median_filter_value(data[x-hx:x-hx+wx-1, y-hy:y-hy+wy-1], even=even)
    endfor
  endelse
  return, result
end

pro test_median_filter
  test_median_filter_equal, median(indgen(5), 5), findgen(5), 'issue 2245, odd width'
  test_median_filter_equal, median(indgen(6), 6), findgen(6), 'issue 2245, even width'

  ; Expected values measured in IDL 8.3.
  d = [0, 9, 1, 8, 2, 7, 3, 6]
  test_median_filter_equal, median(d, 4), [0., 9., 8., 8., 7., 7., 3., 6.], 'IDL 8.3, 1D width 4'
  test_median_filter_equal, median(d, 4, /even), [0., 9., 4.5, 5., 4.5, 5., 3., 6.], 'IDL 8.3, 1D width 4 /EVEN'
  result = median(byte(d), 4, /even)
  test_median_filter_equal, result, byte([0, 9, 4, 5, 4, 5, 3, 6]), 'IDL 8.3, byte width 4 /EVEN'
  test_median_filter_equal, size(result, /type), 1, 'IDL 8.3, byte result type'

  a = reform((indgen(36) * 7) mod 11, 6, 6)
  result = median(a, 4)
  test_median_filter_equal, result[2:3, 2:3], reform([6., 5., 6., 5.], 2, 2), 'IDL 8.3, 2D width 4 interior'
  test_median_filter_equal, total(result ne a), 4., 'IDL 8.3, 2D width 4 changed pixels'
  result = median(a, 4, /even)
  test_median_filter_equal, result[2:3, 2:3], reform([5.5, 4.5, 5.5, 4.5], 2, 2), 'IDL 8.3, 2D width 4 /EVEN interior'

  ; Nonmonotonic data also catches edge corruption affecting interior medians.
  ; The long vector exercises transitions between the fast filter's blocks.
  foreach n, [2, 5, 6, 13, 101] do begin
    values = (lindgen(n) * 37 + 19) mod 101
    foreach type, [1, 2, 3, 4, 5, 6, 9, 12, 13, 14, 15] do begin
      data = fix(values, type=type)
      original = data
      for width = 2, n < 12 do begin
        for even = 0, 1 do begin
          expected = test_median_filter_reference(data, width, even=even)
          context = '1D' + string(n, type, width, even)
          typed_expected = expected
          if type eq 1 then typed_expected = byte(expected)
          test_median_filter_equal, median(data, width, even=even), typed_expected, context
          test_median_filter_equal, median(data, width, even=even, /double), expected, context + ' /DOUBLE'
          test_median_filter_equal, data, original, context + ' input unchanged'
        endfor
      endfor
    endforeach
  endforeach

  ; Cover the floating-point and byte image filters, including multiple blocks
  ; and histogram stripes. Test exact even windows and repeated values, too.
  foreach nx, [9, 101] do begin
    values = reform((lindgen(nx * 9) * 37 + 19) mod 101, nx, 9)
    foreach type, [1, 4, 5] do begin
      data = fix(values, type=type)
      for width = 3, 9 do begin
        for even = 0, 1 do begin
          expected = test_median_filter_reference(data, width, even=even)
          if type eq 1 then expected = byte(expected)
          context = '2D' + string(nx, type, width, even)
          test_median_filter_equal, median(data, width, even=even), expected, context
        endfor
      endfor
    endforeach
  endforeach

  ; An even window spanning a dimension leaves no interior to filter.
  ; Width 12 also exercises the fast path used for large even 2D windows.
  foreach width, [6, 12] do begin
    values = reform((lindgen(width * 14) * 37 + 19) mod 101, width, 14)
    foreach type, [1, 4, 5] do begin
      data = fix(values, type=type)
      test_median_filter_equal, median(data, width), data, '2D full-width window'
      test_median_filter_equal, median(data, width, /even), data, '2D full-width window /EVEN'
    endforeach
  endforeach

  ; Missing and infinite edge values must be copied, too.
  foreach type, [4, 5] do begin
    data = fix([!values.d_nan, 3, 8, 1, 7, 2, -!values.d_infinity], type=type)
    foreach width, [3, 4] do begin
      result = median(data, width)
      test_median_filter_equal, finite(result[0], /nan), 1, 'NaN at vector edge'
      test_median_filter_equal, result[6], data[6], 'infinity at vector edge'
    endforeach
    data = reform(dindgen(25), 5, 5)
    data[0] = !values.d_nan
    data[24] = -!values.d_infinity
    data = fix(data, type=type)
    result = median(data, 3)
    test_median_filter_equal, finite(result[0], /nan), 1, 'NaN at image edge'
    test_median_filter_equal, result[24], data[24], 'infinity at image edge'
  endforeach

  ; GDL's rectangular-window extension supports either parity on each axis.
  if gdl_idl_fl() eq 'GDL' then begin
    values = reform((lindgen(49 * 41) * 37 + 19) mod 101, 49, 41)
    for wx = 3, 6 do begin
      for wy = 3, 6 do begin
        kernel = [wx, wy]
        foreach type, [1, 4, 5] do begin
          data = fix(values, type=type)
          for even = 0, 1 do begin
            expected = test_median_filter_reference(data, kernel, even=even)
            if type eq 1 then expected = byte(expected)
            test_median_filter_equal, median(data, kernel, even=even), expected, 'rectangular window' + string(kernel, type, even)
          endfor
        endforeach
      endfor
    endfor
  endif

  ; Widths that used to crash or overflow, with real interior windows.
  foreach width, [59, 61, 63, 255, 257] do begin
    n = long(width) + 2
    data = reform(byte((lindgen(n*n) * 37 + 19) mod 251), n, n)
    expected = byte(test_median_filter_reference(data, width))
    test_median_filter_equal, median(data, width), expected, 'large byte window' + string(width)
  endforeach
  data = bytarr(259,259)
  test_median_filter_equal, median(data,257), data, 'histogram counter overflow'
  foreach width, [32768L, 32769L] do begin
    data = float((lindgen(width+3) * 37 + 19) mod 101)
    for even = 0, 1 do begin
      expected = test_median_filter_reference(data, width, even=even)
      test_median_filter_equal, median(data, width, even=even), expected, '32-bit width'
    endfor
  endforeach

  ; All numeric result types, independently of the selected filter algorithm.
  foreach rank, [1, 2] do begin
    if rank eq 1 then values = indgen(17) else values = indgen(17,17)
    foreach type, [1,2,3,4,5,6,9,12,13,14,15] do begin
      data = fix(values, type=type)
      foreach width, [3,4,12] do begin
        for dbl = 0, 1 do begin
          for even = 0, 1 do begin
            expected_type = 4
            if type eq 1 then expected_type = 1
            if dbl or (type eq 5) or (type eq 9) then expected_type = 5
            expected = fix(test_median_filter_reference(data, width, even=even), type=expected_type)
            result = median(data, width, double=dbl, even=even)
            test_median_filter_equal, size(result,/type), expected_type, 'return type' + string(rank,type,width,dbl)
            test_median_filter_equal, result, expected, 'typed filter values'
          endfor
        endfor
      endforeach
    endforeach
  endforeach

  ; Wide integer ranges must not allocate a histogram proportional to values.
  data = lonarr(7,7)
  data[2:4,2:4] = 1000000000L
  test_median_filter_equal, median(data,4,/even), test_median_filter_reference(data,4,/even), 'wide integer range'
  data = replicate(10L,5,5)
  data[0:3,0] = 0
  data[0,1] = 0
  test_median_filter_equal, median(data,4,/even), test_median_filter_reference(data,4,/even), 'repeated integer median'

  ; NaNs change the population's parity; infinities remain valid samples.
  foreach type, [4,5,6,9] do begin
    foreach rank, [1,2] do begin
      if rank eq 1 then data = dindgen(61) else data = dindgen(45,41)
      data = (data * 37 + 19) mod 101
      holes = where((lindgen(n_elements(data)) mod 5) eq 0)
      data[holes] = !values.d_nan
      data[1] = !values.d_infinity
      data[2] = -!values.d_infinity
      data = fix(data, type=type)
      original = data
      foreach width, [2,3,4,5,12,13] do begin
        for even = 0, 1 do begin
          expected = test_median_filter_reference(data, width, even=even)
          test_median_filter_equal, median(data,width,even=even), expected, 'NaN window' + string(type,rank,width,even)
          test_median_filter_equal, data, original, 'NaN input unchanged'
        endfor
      endforeach
      data[*] = !values.d_nan
      test_median_filter_equal, median(data,4,/even), data, 'all NaN even window'
      test_median_filter_equal, median(data,3), data, 'all NaN odd window'
    endforeach
  endforeach
end
