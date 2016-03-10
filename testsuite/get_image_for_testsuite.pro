;
; AC March 2016
;
; just in order to simply some redundant codes
; used in various tests ...
;
function GET_IMAGE_FOR_TESTSUITE, image, filename=filename, $
                                  test=test, verbose=verbose, help=help
;
if KEYWORD_SET(help) then begin
    print, 'function FILE_SEARCH_FOR_TESTSUITE, filename, help=help, test=test, $'
    print, '                                    quiet=quiet, verbose=verbose'
    return, ''
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
status=1
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
    endelse
endif
;
if KEYWORD_SET(test) then STOP
;
return, status
;
end
