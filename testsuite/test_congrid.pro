;
; This basic test for CONGRID, using the Saturn.jpg image in the
; testsuite is under GNU GPL V2 or later
;
; G. Duvert, 17-Dec-2011
;
; Note by Alain: it would also be great to have some "numerical null
; tests"
;
; ---------------------------------
;
pro TEST_CONGRID_ON_IMAGES, image=image, filename=filename
;
if (N_ELEMENTS(image) EQ 0) then begin
    if (N_ELEMENTS(filename) EQ 0) then begin
        ;; we know we have "Saturn" in testsuite/, which should be in !PATH
        filename='Saturn.jpg'
        list_of_dirs=STRSPLIT(!PATH, PATH_SEP(/SEARCH_PATH), /EXTRACT)
        file=FILE_SEARCH(list_of_dirs+PATH_SEP()+filename)
    endif else begin
        file=FILE_SEARCH(filename)
        if STRLEN(file) EQ 0 then begin
            list_of_dirs=STRSPLIT(!PATH, PATH_SEP(/SEARCH_PATH), /EXTRACT)
            file=FILE_SEARCH(list_of_dirs+PATH_SEP()+filename)
        endif
    endelse
    ;;
    if STRLEN(file) EQ 0 then MESSAGE, 'File not found :'+filename
    ;;   
    status=QUERY_IMAGE(file, info)
    CASE info.type OF
        'JPEG' : READ_JPEG, file, image, /gray
        'GIF'  : READ_GIF, file, image, /gray
        'BMP'  : image=READ_BMP(file)
    endcase
endif
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
stop

LOADCT, 28  
DEVICE, DECOMPOSED = 0
WINDOW, 0, XSIZE = 600, YSIZE = 260
TV, gray_image
;
nbpX=ndims[0]*2
nbpY=ROUND(ndims[1]*1.5)
big_image=CONGRID(gray_image, nbpX, nbpY, /INTERP, /CENTER, /CUB)
;
WINDOW, 1, XSIZE=nbpX, YSIZE=nbpY
TV, big_image
;
nbpX=ndims[0]/2
nbpY=ndims[1]/2
small_image=CONGRID(gray_image, nbpX, nbpY, /INTERP, /CENTER, /CUB)
;
WINDOW, 2, XSIZE=nbpX, YSIZE=nbpY
TV, small_image
;
nbpX=ndims[0]/4
nbpY=ndims[1]/2
small_image=CONGRID(gray_image, nbpX, nbpY, /INTERP, /CENTER, /CUB)
;
WINDOW, 3, XSIZE=nbpX, YSIZE=nbpY
TV, small_image
;
end
;
; ---------------------------------
;
pro TEST_CONGRID
;
TEST_CONGRID_ON_IMAGES
;
end
