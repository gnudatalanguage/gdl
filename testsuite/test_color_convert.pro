;
; AC 2026-Mars-26
;
; Very preliminary tests for color_convert.pro
; (in fact, no tests at all, and a basic functionnality
; was not available)
;
; -------------------------------------------
; Modifications history :
;
; * 2026-03-26 : AC : first code
; * 2026-03-26 : AC : adding TEST_CC_RGB_HSV
;
; --------------------------------------------------
;
pro TEST_CC_RGB_HSV, cumul_errors, test=test, debug=debug
;
errors=0
;
tol=3e-3
;
;---------------------------------------------
; Mini tableau de tests
;---------------------------------------------
rgb_cases = [[0,0,0], [255,255,255], [128,128,128], $
             [255,0,0], [0,255,0], [0,0,255], $
             [255,255,0], [0,255,255], [255,0,255],$
             [192,192,192], [128,255,128]]
nbps_rgb_cases=SIZE(rgb_cases,/dim)
hsv_expect=[[0,0,0],[0,0,1],[0,0,0.5],$
            [0,1,1],[120,1,1],[240,1,1],$
            [60,1,1],[180,1,1],[300,1,1],$
            [0,0,.75],[120,0.5,1]]
hsv_name=['black', 'white', 'Gris moyen', 'Red', 'Green',$
          'blue', 'Yellow', 'Cyan','Magenta', 'Green light','Grey light']
;
for ii=0, nbps_rgb_cases[1]-1 do begin
    COLOR_CONVERT, rgb_cases[0,ii], rgb_cases[1,ii], rgb_cases[2,ii],$
                   h,s,v, /rgb_hsv
    if TOTAL(ABS([h,s,v]-hsv_expect[*,ii])) GT tol then begin
        ERRORS_ADD, errors, hsv_name[ii]
    endif
    if KEYWORD_SET(debug) then begin
        print, h,s,v, hsv_expect[*,ii], reform(TOTAL([h,s,v]-hsv_expect[*,ii]))
    endif
endfor
;
; --------------
;
BANNER_FOR_TESTSUITE, "TEST_CC_RGB_HSV", errors, /status, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; --------------------------------------------------
;
pro TEST_CC_RGB_HLS, cumul_errors, test=test
;
errors=0
;
COLOR_CONVERT, 255, 0,0, a,b,c, /rgb_hls
if ~ARRAY_EQUAL([a,b,c], [0.,0.5,1.]) then ERRORS_ADD, errors, 'RGB2HLS 255,0,0'
COLOR_CONVERT, 0, 255, 0, a,b,c, /rgb_hls
if ~ARRAY_EQUAL([a,b,c], [120.,0.5,1.]) then ERRORS_ADD, errors, 'RGB2HLS 0,255,0'
COLOR_CONVERT, 0,0, 255, a,b,c, /rgb_hls
if ~ARRAY_EQUAL([a,b,c], [240.,0.5,1.]) then ERRORS_ADD, errors, 'RGB2HLS 0,0,255'
; --------------
;
BANNER_FOR_TESTSUITE, "TEST_CC_RGB_HLS", errors, /status, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; --------------------------------------------------
;
pro TEST_CC_HLS_RGB, cumul_errors, test=test
;
errors=0
;
COLOR_CONVERT, 0.,0.,0., a,b,c, /hls_rgb
if ~ARRAY_EQUAL([a,b,c], [0,0,0]) then ERRORS_ADD, errors, 'HLS_RGB black'
COLOR_CONVERT, 0.,1.,0., a,b,c, /hls_rgb
if ~ARRAY_EQUAL([a,b,c], [255,255,255]) then ERRORS_ADD, errors, 'HLS_RGB white'
;
COLOR_CONVERT, 0.,0.5, 1., a,b,c, /hls_rgb
if ~ARRAY_EQUAL([a,b,c], [255,0,0]) then ERRORS_ADD, errors, 'HLS_RGB red'
COLOR_CONVERT, 120.,0.5, 1., a,b,c, /hls_rgb
if ~ARRAY_EQUAL([a,b,c], [0,255,0]) then ERRORS_ADD, errors, 'HLS_RGB green'
COLOR_CONVERT, 240.,0.5, 1., a,b,c, /hls_rgb
if ~ARRAY_EQUAL([a,b,c], [0,0,255]) then ERRORS_ADD, errors, 'HLS_RGB bleu'
COLOR_CONVERT, 60.,0.5, 1., a,b,c, /hls_rgb
if ~ARRAY_EQUAL([a,b,c], [255,255,0]) then ERRORS_ADD, errors, 'HLS_RGB yellow'
;
; --------------
;
BANNER_FOR_TESTSUITE, "TEST_CC_HLS_RGB", errors, /status, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; --------------------------------------------------
;
pro TEST_COLOR_CONVERT, no_exit=no_exit, verbose=verbose, test=test, $
                        debug=debug, help=help
;
if KEYWORD_SET(help) then begin
   print, 'pro COLOR_CONVERT, no_exit=no_exit, verbose=verbose, test=test, $'
   print, '                   debug=debug, help=help'
   return
endif
;
cumul_errors=0
;
one_file_and_path=FILE_SEARCH_FOR_TESTSUITE('Saturn.jpg')
if ~FILE_TEST(one_file_and_path) then begin
    MESSAGE, /info, 'Missing the input file <<Saturn.jpg>>'
    if ~KEYWORD_SET(no_exit) then EXIT, status=77
endif
READ_JPEG, one_file_and_path, input
;
TEST_CC_RGB_HSV, cumul_errors, test=test
TEST_CC_RGB_HLS, cumul_errors, test=test
TEST_CC_HLS_RGB, cumul_errors, test=test
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_STRSPLIT', cumul_errors, short=short
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end

