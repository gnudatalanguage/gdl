; by Reto Stockli <reto.stockli@gmail.com>
pro test_array_indices
  ; testing the two ways of calculating array indices:

  ;; solution: tested with IDL 8.1
  s = [[0L,5L],[1L,27L]]
  i = [10,55]

  ;; full size array a
  a = bytarr(2,28)
  if ~array_equal(array_indices(a,i),s) then exit, status=1

  ;; array containing dimensions
  a = [2,28]
  if ~array_equal(array_indices(a,i,/dimensions),s) then exit, status=1
end
