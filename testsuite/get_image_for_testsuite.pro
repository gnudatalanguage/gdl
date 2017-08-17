;
; AC March 2016
;
; just in order to simply some redundant codes
; used in various tests ...
;
; We keep to use the "status" to be able to debug special cases 
; we encoutered ...
;
; Modifications history :
; AC 2017-08-16 : cleaner exit when GDL compiled without IM/GM
;
function GET_IMAGE_FOR_TESTSUITE, image, filename=filename, verbose=verbose, $
                                  test=test, debug=debug, help=help
;
if KEYWORD_SET(help) then begin
    print, 'function FILE_SEARCH_FOR_TESTSUITE, image, filename=filename, $'
    print, '                                    verbose=verbose, debug=debug, $'
    print, '                                    help=help, test=test'
    return, -1
endif 
;
status=1
;
; Do we have access to ImageMagick functionnalities ??
;
if (MAGICK_EXISTS() EQ 0) then begin
    MESSAGE, /continue, "GDL was compiled without ImageMagick support."
    MESSAGE, "You must have ImageMagick support to use this functionaly."
    status=0
    if ~KEYWORD_SET(debug) then return, 0
endif
;
; The default filename is 'Saturn.jpg',
; which should be in testsuite/ directory
;
if ~KEYWORD_SET(filename) then filename='Saturn.jpg'
;
; current path (first) + !path will be checked by this function,
; only one file if many
;
file=FILE_SEARCH_FOR_TESTSUITE(filename, /quiet)
;
if (STRLEN(file) eq 0) then begin
    MESSAGE, level=-1, /info, 'Image file : <<'+filename+'>> not found, test aborted'
    status=0
end
;
if (status EQ 1) then begin
    ;; is the file containing an image ??
    queryStatus = QUERY_IMAGE(file, imageInfo)
    ;;
    if (queryStatus eq 0) then begin
        MESSAGE, level=-1, /info, '<<'+filename+'>> does not contain an image !'
        status=0
    end else begin
        ;; reading the image ...
        image = READ_IMAGE(file)
        dims=size(image,/d)
        sdims='['+STRING(dims[0])
        for ii=1, n_elements(dims)-1 do sdims=sdims+','+STRING(dims[ii])
        sdims=STRCOMPRESS(/remove_all, sdims+']')
        MESSAGE, level=-1, /info, 'image in <<'+filename+'>> read, dims are : '+sdims
    endelse
endif
;
if KEYWORD_SET(test) then STOP
;
return, status
;
end
