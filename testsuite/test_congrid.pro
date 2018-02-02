;
; This basic test for CONGRID, using the Saturn.jpg image in the
; testsuite is under GNU GPL V2 or later
;
; G. Duvert, 17-Dec-2011
;
; Note by Alain: it would also be great to have some "numerical null
; tests"
;
; Modifications history :
;
; * 2012-Feb-10: AC. Adding a true self-consistant test (TEST_CONGRID_BASIC)
; * 2018-Feb-01: AC. Mixing PLOT & CONGRID
;
; ---------------------------------
;
pro TEST_CONGRID_BASIC, cumul_errors, nbp=nbp, test=test, byte=byte
;default is double, byte errors come from gdl rounding, not congrid.
if N_ELEMENTS(nbp) EQ 0 then nbp=9
;
isbyte=KEYWORD_SET(byte)
errors=0
tolerance=1e-6
;
if (isbyte) then in=BINDGEN(3,3) else in=DINDGEN(3,3)
;
; test /Sample
;
out_rs=REBIN(in,9,9, /sample)
out_c=CONGRID(in,9,9)
resu=SQRT(TOTAL((out_rs-out_c)^2))
if (resu GT tolerance) then ADD_ERROR, errors, 'case /sample'
;
; test /interpol
;
out_r=REBIN(in,9,9)
out_ci=CONGRID(in,9,9,/interp)
resu=SQRT(TOTAL((out_r-out_ci)^2))
if (resu GT tolerance) then ADD_ERROR, errors, 'case /interp'
;
; ----- final ----
;
BANNER_FOR_TESTSUITE, 'TEST_CONGRID_BASIC', errors, /short
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_set(test) then STOP
;
end
;
; -----------------------------
;
pro TEST_CONGRID_ON_IMAGES, cumul_errors, test=test
;
; Read "Saturn.jpg" and return content in "image"
status=GET_IMAGE_FOR_TESTSUITE(image)
if (status eq 0) then return
;
if (SIZE(image, /n_dim) LT 2) OR (SIZE(image, /n_dim) GT 3)  then begin
    MESSAGE, 'Image must be 2D or 3D'
endif
if (SIZE(image, /n_dim) EQ 3) then begin
    ;; conversion into GRAY using Coyote formula
    ;; http://www.idlcoyote.com/ip_tips/color2gray.html
    gray_image=0.3*REFORM(image[0,*,*]) + 0.59*REFORM(image[1,*,*]) + 0.11*REFORM(image[2,*,*])
endif
if (SIZE(image, /n_dim) EQ 2) then gray_image=image
;
;screensize=GET_SCREEN_SIZE()
ndims=SIZE(gray_image,/dim)
;
LOADCT, 28  
DEVICE, DECOMPOSED = 0
WINDOW, 0, XSIZE = 600, YSIZE = 260
TV, gray_image
;
nbpX=ndims[0]*2
nbpY=ROUND(ndims[1]*1.5)
big_image=CONGRID(gray_image, nbpX, nbpY, /INTERP, /CENTER, CUB=0.5)
;
WINDOW, 1, XSIZE=nbpX, YSIZE=nbpY
TV, big_image
;
nbpX=ndims[0]/2
nbpY=ndims[1]/2
small_image=CONGRID(gray_image, nbpX, nbpY, /INTERP, /CENTER, CUB=0.5)
;
WINDOW, 2, XSIZE=nbpX, YSIZE=nbpY
TV, small_image
;
nbpX=ndims[0]/4
nbpY=ndims[1]/2
small_image=CONGRID(gray_image, nbpX, nbpY, /INTERP, /CENTER, CUB=0.5)
;
WINDOW, 3, XSIZE=nbpX, YSIZE=nbpY
TV, small_image
;
; ----- final ----
;
errors=0
BANNER_FOR_TESTSUITE, 'TEST_CONGRID_ON_IMAGES', errors, /short
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_set(test) then STOP
;
WAIT, 0.25
WDELETE, 0,1,2,3
;
end
;
; ---------------------------------
;
pro TEST_CONGRID_ON_PLOT, cumul_errors, test=test
;
; Read "Saturn.jpg" and return content in "image"
status=GET_IMAGE_FOR_TESTSUITE(image)
if (status eq 0) then return
;
info=SIZE(image,/dim)
d1=info[1] & d2=info[1] & d3=info[2]
plot, [0], xrange=[0,d2], yrange=[0,d3], /xstyle, /ystyle,/iso
cv=CONVERT_COORD([0,d2-1],[0,d3-1],/TO_DEV)
TV, CONGRID(image,3,cv[0,1]-cv[0,0],cv[1,1]-cv[1,0]),1,1,/DATA,/true
;
; ----- final ----
;
errors=0
BANNER_FOR_TESTSUITE, 'TEST_CONGRID_ON_PLOT', errors, /short
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_set(test) then STOP
;
WAIT, 0.5
WDELETE, 0
;
end
;
; ---------------------------------
;
pro TEST_CONGRID, help=help, noexit=noexit, test=test
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_CONGRID, help=help, noexit=noexit, test=test'
    return
endif;
;
TEST_CONGRID_BASIC, cumul_errors, nbp=9
TEST_CONGRID_BASIC, cumul_errors, nbp=21
TEST_CONGRID_BASIC, cumul_errors, nbp=121
;
TEST_CONGRID_BASIC, cumul_errors, nbp=9, /byte
TEST_CONGRID_BASIC, cumul_errors, nbp=21, /byte
TEST_CONGRID_BASIC, cumul_errors, nbp=121, /byte
;
TEST_CONGRID_ON_IMAGES, cumul_errors
;
TEST_CONGRID_ON_PLOT, cumul_errors
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_CONGRID', cumul_errors
;
if (cumul_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
