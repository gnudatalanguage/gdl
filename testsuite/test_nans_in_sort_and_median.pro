; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
; a follow-up to the bug-report by Boud Roukema (tracker item no. 2985552)
pro test_nans_in_sort_and_median

  ; testing behaviour of SORT() with NaN, inf, -inf

  a = [1, !VALUES.F_NAN, -1]
  if ~array_equal(sort(a), [2,0,1]) then begin
    message, 'sort() failed to sort an array containing a float NaN', /conti
    exit, status=1
  endif

  a = double(a)
  if ~array_equal(sort(a), [2,0,1]) then begin
    message, 'sort() failed to sort an array containing a double NaN', /conti
    exit, status=1
  endif

  a = [1, !VALUES.F_INFINITY, -!VALUES.F_INFINITY, -1]
  if ~array_equal(sort(a), [2,3,0,1]) then begin
    message, 'sort() failed to sort an array containing float Infs', /conti
    exit, status=1
  endif

  a = double(a)
  if ~array_equal(sort(a), [2,3,0,1]) then begin
    message, 'sort() failed to sort an array containing double Infs', /conti
    exit, status=1
  endif

  ; testing behaviour of MEDIAN() with NaN, inf, -inf

  a = [-500, !VALUES.F_NAN, !VALUES.F_INFINITY, -!VALUES.F_INFINITY, 500, 600.0]
  if median(a) ne median(a[where(finite(a))]) then begin
    message, 'median() failed to compute the median for a 1D array cotaining float NaN/Infs', /conti
    exit, status=1
  endif

  a = double(a)
  if median(a) ne median(a[where(finite(a))]) then begin
    message, 'median() failed to compute the median for a 1D array cotaining double NaN/Infs', /conti
    exit, status=1
  endif

  a = [[-500, !VALUES.F_INFINITY], [-!VALUES.F_INFINITY, 600.0]]
  if ~array_equal(median(a, dim=1), [!VALUES.F_INFINITY, 600]) then begin
    message, 'median() failed to compute the median for a 2D array cotaining float Infs', /conti
    exit, status=1
  endif

  a = double(a)
  if ~array_equal(median(a, dim=1), [!VALUES.F_INFINITY, 600]) then begin
    message, 'median() failed to compute the median for a 2D array cotaining double Infs', /conti
    exit, status=1
  endif

  a = [[-500, !VALUES.F_NAN], [-!VALUES.F_NAN, 600.0]]
  if ~array_equal(median(a, dim=1), [-500, 600]) then begin
    message, 'median() failed to compute the median for a 2D array cotaining float NaNs', /conti
    exit, status=1
  endif

  a = double(a)
  if ~array_equal(median(a, dim=1), [-500, 600]) then begin
    message, 'median() failed to compute the median for a 2D array cotaining double NaNs', /conti
    exit, status=1
  endif

  a = [[-500, !VALUES.F_NAN, !VALUES.F_INFINITY], [-!VALUES.F_INFINITY, 500, 600.0]]
  if ~array_equal(median(a, dim=1), [!VALUES.F_INFINITY, 500]) then begin
    message, 'median() failed to compute the median for a 2D array cotaining float NaNs and Infs', /conti
    exit, status=1
  endif

  a = double(a)
  if ~array_equal(median(a, dim=1), [!VALUES.F_INFINITY, 500]) then begin
    message, 'median() failed to compute the median for a 2D array cotaining double NaNs and Infs', /conti
    exit, status=1
  endif

end
