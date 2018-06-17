;
; testing ARRAY_INDICES (.pro)
;
; initial version by Reto Stockli <reto.stockli@gmail.com>
; https://sourceforge.net/p/gnudatalanguage/patches/87/
;
; AC 2017-03-16
; putting it in the testsuite Makefile.am
;
pro TEST_ARRAY_INDICES_REKO, cumuls_errors, verbose=verbose, test=test
;
; testing the two ways of calculating array indices:
;
nb_errors=0
;
;; solution: tested with IDL 8.1
s = [[0L,5L],[1L,27L]]
i = [10,55]
;
;; full size array a
;
a = BYTARR(2,28)
if ~ARRAY_EQUAL(ARRAY_INDICES(a,i),s) then begin
      ERRORS_ADD, nb_errors, 'pb in case 1 (no /dim) '
endif
;
;; array containing dimensions
a = [2,28]
if ~array_equal(array_indices(a,i,/dimensions),s) then begin
      ERRORS_ADD, nb_errors, 'pb in case 2 (with /dim) '
endif
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_ARRAY_INDICES_REKO', nb_errors, /status
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_SET(test) then STOP
;
end
;
; ---------------------------
;
;
pro TEST_ARRAY_INDICES_DIST, cumuls_errors, $
                             debug=debug, verbose=verbose, test=test
;
; testing the two ways of calculating array indices using DIST
;
nb_errors=0
;
dims=[5,6,10,11,12, 50,51, 121,122]
;
; easy square cases
;
for ii=0, N_ELEMENTS(dims)-1 do begin
   data=DIST(dims[ii])
   mx = MAX(data, location)
   expected=[dims[ii]/2,dims[ii]/2]
   ;;
   ;; without /Dim
   indices_max = ARRAY_INDICES(data, location)
   ;;
   if KEYWORD_SET(debug) then print, dims[ii], indices_max
   ;;
   if ~ARRAY_EQUAL(indices_max,expected) then begin
      ERRORS_ADD, nb_errors, 'pb in case 1 (square), no /dim'
   endif
   ;;
   ;; with /Dim
   cur_dims=SIZE(data, /DIMENSIONS)
   indices_max_dims=ARRAY_INDICES(cur_dims, location,/dimensions)
   ;;
   if ~ARRAY_EQUAL(indices_max_dims,expected) then begin
      ERRORS_ADD, nb_errors, 'pb in case 1 (square) with /Dimensions'
   endif   
endfor
;
; rectangular cases
;
extra=10
;
for ii=0, N_ELEMENTS(dims)-1 do begin
   data=DIST(dims[ii],dims[ii]+extra)
   mx = MAX(data, location)
   expected=[dims[ii]/2,(dims[ii]+extra)/2]
   indices_max = ARRAY_INDICES(data, location)
   ;;
   if KEYWORD_SET(debug) then print, dims[ii], indices_max
   ;;
   if ~ARRAY_EQUAL(indices_max,expected) then begin
      ERRORS_ADD, nb_errors, 'pb in case 2 (rectangular), no /Dim'
   endif
   ;;
   ;; with /Dim
   cur_dims=SIZE(data, /DIMENSIONS)
   indices_max_dims=ARRAY_INDICES(cur_dims, location,/dimensions)
   ;;
   if ~ARRAY_EQUAL(indices_max_dims,expected) then begin
      ERRORS_ADD, nb_errors, 'pb in case 2 (rectangular) with /Dimensions'
   endif   
endfor
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_ARRAY_INDICES_DIST', nb_errors, /status
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_SET(test) then STOP
;
end
;
; -------------------------------------
;
pro TEST_ARRAY_INDICES, help=help, verbose=verbose, no_exit=no_exit, test=test
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_ARRAY_INDICES, help=help, verbose=verbose, $'
   print, '                  no_exit=no_exit, test=test'
   return
endif
;
nb_errors=0
;
TEST_ARRAY_INDICES_REKO, nb_errors, verbose=verbose, test=test
;
TEST_ARRAY_INDICES_DIST, nb_errors, verbose=verbose, test=test
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_ARRAY_INDICES', nb_errors
;
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
