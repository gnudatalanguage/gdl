;
; Alain Coulais, 16-Nov-2011, from an idea by LLuvia
;
; do we have good statitical/numerical processing, using a know entry ?
; (was tested with success on x86 and x86_64
; after "correction" in IMAGE_STATISTICS (type conversion
; and exact way to call MEAN, STDDEV, TOTAL with /double))
;
function DIFF_BELOW_TOL, number1, number2, tolerance
if (ABS(number1-number2) LT tolerance) then return, 1 else return, 0
end
;
pro TEST_IMAGE_STATISTICS, fake_error=fake_error, $
                           help=help, test=test, verbose=verbose
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_IMAGE_STATISTICS, fake_error=fake_error, $'
   print, '                           help=help, test=test, verbose=verbose'
   return
endif
;
;  for IDL compatibility
FORWARD_FUNCTION MAGICK_EXISTS
;
DEFSYSV, '!gdl', exists=is_it_gdl
if (is_it_gdl EQ 1) then begin
    ;; Do we have access to ImageMagick functionnalities ??
    ;;
    if (MAGICK_EXISTS() EQ 0) then begin
        MESSAGE, /continue, "GDL was compiled without ImageMagick support."
        MESSAGE, /con, "You must have ImageMagick support to use this functionaly."
        EXIT, status=77
    endif
endif
;
one_file_and_path=FILE_SEARCH_FOR_TESTSUITE('Saturn.jpg')
;
queryStatus = QUERY_IMAGE(one_file_and_path, imageInfo)
if (queryStatus eq 0) then begin
    MESSAGE, /info, "Image for test (Saturn.jpg) not found, test aborted"
    return
end
;
READ_JPEG, one_file_and_path, cube
;
if KEYWORD_SET(verbose) then begin
   MESSAGE, /continue, 'Reading : '+one_file_and_path
endif
;
if SIZE(cube, /N_dim) NE 3 then begin
   MESSAGE, /continue, '3D cube expected ! Bad reading !!'
   EXIT, status=1
endif
;
; exemple of introducing fake error:
if KEYWORD_SET(fake_error) then cube[0,0,1]=234
;
; computation of various numbers ...
;
IMAGE_STATISTICS, cube, COUNT = pixelNumber, $  
                  DATA_SUM = pixelTotal, MAXIMUM = pixelMax, $  
                  MEAN = pixelMean, MINIMUM = pixelMin, $  
                  STDDEV = pixelDeviation, $  
                  SUM_OF_SQUARES = pixelSquareSum, $  
                  VARIANCE = pixelVariance
;
; we know the expected values
;
nb_errors=0
;
if pixelNumber NE 466200 then nb_errors=nb_errors+1
if ~DIFF_BELOW_TOL(pixelTotal, 2.26349e+07, 1e3) then nb_errors=nb_errors+1
if ~DIFF_BELOW_TOL(pixelMax, 255.0, 0.001) then nb_errors=nb_errors+1
if ~DIFF_BELOW_TOL(pixelMean, 48.5520, 0.0001) then nb_errors=nb_errors+1
if ~DIFF_BELOW_TOL(pixelMin,0.0, 0.0001) then nb_errors=nb_errors+1
if ~DIFF_BELOW_TOL(pixelDeviation, 65.5660, 0.0001) then nb_errors=nb_errors+1
if ~DIFF_BELOW_TOL(pixelSquareSum, 3.10312e+09, 1e4) then nb_errors=nb_errors+1
if ~DIFF_BELOW_TOL(pixelVariance, 4298.91, 0.01) then nb_errors=nb_errors+1
;
; final message
;
BANNER_FOR_TESTSUITE, 'TEST_IMAGE_STATISTICS', nb_errors, short=short
;
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
