; Scalar and dimensional MEDIAN, including /EVEN after NaN removal.
pro test_median
  resolve_routine, 'test_median_filter'
  test_median_filter_equal, median([56,93,29,66],/even), 61., 'lower middle value'

  seed = 2245L
  for n = 1, 40 do begin
    values = fix(randomu(seed,n) * 17) - 8
    foreach type, [1,2,3,4,5,6,9,12,13,14,15] do begin
      data = fix(values,type=type)
      original = data
      for dbl = 0, 1 do begin
        expected_type = 4
        if dbl or (type eq 5) or (type eq 9) then expected_type = 5
        for even = 0, 1 do begin
          expected = fix(test_median_filter_value(fix(data,type=expected_type),even=even),type=expected_type)
          result = median(data,double=dbl,even=even)
          test_median_filter_equal, result, expected, 'scalar' + string(n,type,dbl,even)
          test_median_filter_equal, size(result,/type), expected_type, 'scalar return type'
        endfor
      endfor
      test_median_filter_equal, data, original, 'scalar input unchanged'
    endforeach
  endfor

  foreach type, [4,5,6,9] do begin
    for n = 1, 12 do begin
      values = double((indgen(n) * 37 + 19) mod 101)
      for missing = 0, n do begin
        data = values
        if missing gt 0 then data[0:missing-1] = !values.d_nan
        data = fix(data,type=type)
        for even = 0, 1 do begin
          expected = test_median_filter_value(data,even=even)
          test_median_filter_equal, median(data,even=even), expected, 'scalar NaN parity'
        endfor
      endfor
    endfor

    ; Exercise each axis, transposition, output shape, and per-slice parity.
    data = double((indgen(3,4,5) * 37 + 19) mod 101)
    data[0,*,*] = !values.d_nan
    data[1,0,0] = !values.d_infinity
    data[2,0,0] = -!values.d_infinity
    data[*,3,4] = !values.d_nan
    data = fix(data,type=type)
    original = data
    for even = 0, 1 do begin
      expected = dblarr(4,5)
      for z = 0, 4 do for y = 0, 3 do $
        expected[y,z] = test_median_filter_value(data[*,y,z],even=even)
      test_median_filter_equal, median(data,dimension=1,even=even), expected, 'DIMENSION=1'
      expected = dblarr(3,5)
      for z = 0, 4 do for x = 0, 2 do $
        expected[x,z] = test_median_filter_value(data[x,*,z],even=even)
      test_median_filter_equal, median(data,dimension=2,even=even), expected, 'DIMENSION=2'
      expected = dblarr(3,4)
      for y = 0, 3 do for x = 0, 2 do $
        expected[x,y] = test_median_filter_value(data[x,y,*],even=even)
      test_median_filter_equal, median(data,dimension=3,even=even), expected, 'DIMENSION=3'
      test_median_filter_equal, median(data,dimension=0,even=even), $
        test_median_filter_value(data,even=even), 'DIMENSION=0'
    endfor
    test_median_filter_equal, data, original, 'dimensional input unchanged'
  endforeach

  a = reform([1.,9.,100.,!values.f_nan,1.,9.,100.,!values.f_nan],4,2)
  test_median_filter_equal, median(a,dimension=1,/even), [9.,9.], 'four elements, three valid'
  a = reform([1.,!values.f_nan,9.,1.,!values.f_nan,9.],3,2)
  test_median_filter_equal, median(a,dimension=1,/even), [5.,5.], 'three elements, two valid'

  ; A supplied width ignores DIMENSION, including an otherwise invalid value.
  test_median_filter_equal, median(findgen(7),3,dimension=99), findgen(7), 'DIMENSION ignored for filters'
  foreach expression, ['median(findgen(5),0)', $
                        'median(findgen(5),6)', $
                        'median(findgen(5),!values.d_nan)', $
                        'median(findgen(5),!values.d_infinity)', $
                        'median(findgen(5),[3,3])', $
                        'median(findgen(5,5),[3,3,3])', $
                        'median(findgen(5,5),[3,!values.d_nan])', $
                        'median(findgen(5),dimension=-1)', $
                        'median(findgen(5),dimension=2)', $
                        'median(findgen(5),dimension=[0,0])'] do begin
    if execute('ignored = ' + expression) then begin
      message, 'MEDIAN accepted invalid argument: ' + expression, /continue
      exit, status=1
    endif
  endforeach
end
