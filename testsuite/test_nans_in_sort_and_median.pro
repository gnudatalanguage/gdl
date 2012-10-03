; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
; a follow-up to the bug-report by Boud Roukema (tracker item
; no. 2985552)
;
pro TEST_NANS_IN_SORT_AND_MEDIAN, no_exit=no_exit, help=help, verbose=verbose, test=test
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_NANS_IN_SORT_AND_MEDIAN, no_exit=no_exit, help=help, verbose=verbose, test=test'
    return
endif
;
error_sort=0
error_median=0
;
;; testing behaviour of SORT() with NaN, inf, -inf
;
a = [1, !VALUES.F_NAN, -1]
if ~ARRAY_EQUAL(SORT(a), [2,0,1]) then begin
    MESSAGE, '[S1] SORT() failed to sort an array containing a FLOAT NaN', /conti
    error_sort++
endif

a = DOUBLE(a)
if ~ARRAY_EQUAL(SORT(a), [2,0,1]) then begin
    MESSAGE, '[S2] SORT() failed to sort an array containing a DOUBLE NaN', /conti
    error_sort++
endif

a = [1, !VALUES.F_INFINITY, -!VALUES.F_INFINITY, -1]
if ~array_equal(SORT(a), [2,3,0,1]) then begin
    MESSAGE, '[S3] SORT() failed to sort an array containing FLOAT Infs', /conti
    error_sort++
endif

a = DOUBLE(a)
if ~ARRAY_EQUAL(SORT(a), [2,3,0,1]) then begin
    MESSAGE, '[S4] SORT() failed to sort an array containing DOUBLE Infs', /conti
    error_sort++
endif
;
; ----------------------------------------------------------
;; testing behaviour of MEDIAN() with NaN, inf, -inf
;
a = [-500, !VALUES.F_NAN, !VALUES.F_INFINITY, -!VALUES.F_INFINITY, 500, 600.0]
if MEDIAN(a) ne MEDIAN(a[where(finite(a))]) then begin
    MESSAGE, '[M1] MEDIAN() failed to compute the median for a 1D array containing FLOAT NaN/Infs', /conti
    error_median++
endif
;
a = DOUBLE(a)
if MEDIAN(a) ne MEDIAN(a[where(finite(a))]) then begin
    MESSAGE, '[M2] MEDIAN() failed to compute the median for a 1D array containing DOUBLE NaN/Infs', /conti
    error_median++
endif
;
a = [[-500, !VALUES.F_INFINITY], [-!VALUES.F_INFINITY, 600.0]]
if ~ARRAY_EQUAL(MEDIAN(a, dim=1), [!VALUES.F_INFINITY, 600]) then begin
    MESSAGE, '[M3] MEDIAN() failed to compute the median for a 2D array containing FLOAT Infs', /conti
    error_median++
endif
;
a = DOUBLE(a)
if ~ARRAY_EQUAL(MEDIAN(a, dim=1), [!VALUES.F_INFINITY, 600]) then begin
    MESSAGE, '[M4] MEDIAN() failed to compute the median for a 2D array containing DOUBLE Infs', /conti
    error_median++
endif
;
a = [[-500, !VALUES.F_NAN], [-!VALUES.F_NAN, 600.0]]
if ~ARRAY_EQUAL(MEDIAN(a, dim=1), [-500, 600]) then begin
    MESSAGE, '[M5] MEDIAN() failed to compute the median for a 2D array containing FLOAT NaNs', /conti
    error_median++
endif
;
a = DOUBLE(a)
if ~ARRAY_EQUAL(MEDIAN(a, dim=1), [-500, 600]) then begin
    MESSAGE, '[M6] MEDIAN() failed to compute the median for a 2D array containing DOUBLE NaNs', /conti
    error_median++
endif
;
a = [[-500, !VALUES.F_NAN, !VALUES.F_INFINITY], [-!VALUES.F_INFINITY, 500, 600.0]]
if ~ARRAY_EQUAL(MEDIAN(a, dim=1), [!VALUES.F_INFINITY, 500]) then begin
    MESSAGE, '[M7] MEDIAN() failed to compute the median for a 2D array containing FLOAT NaNs and Infs', /conti
    error_median++
endif
;
a = DOUBLE(a)
if ~ARRAY_EQUAL(MEDIAN(a, dim=1), [!VALUES.F_INFINITY, 500]) then begin
    MESSAGE, '[M8] MEDIAN() failed to compute the median for a 2D array containing DOUBLE NaNs and Infs', /conti
    error_median++
endif
;
if ((error_sort GT 0) OR (error_median GT 0)) then begin
    MESSAGE, /continue, 'Errors detected'
    if (error_sort GT 0) then MESSAGE, /continue, STRING(error_sort)+' in SORT() test'
    if (error_median GT 0) then MESSAGE, /continue, STRING(error_median)+' in MEDIAN() test'
    if ~KEYWORD_SET(no_exit) then EXIT, status=1
endif else begin
    MESSAGE, /continue, 'SORT() and MEDIAN() with NaN succesfuly tested !'
endelse
;
if KEYWORD_SET(test) then STOP
;
end
