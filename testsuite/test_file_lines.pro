;
; revised by AC on 2025-Apr-18 because it was not OK on IDL
; (previously used file "swap_endian.pro" is different in IDL & GDL ...)
;
pro TEST_FILE_LINES, test=test, verbose=verbose, $
                     help=help, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_FILE_LINES, test=test, verbose=verbose, $'
   print, '                     help=help, no_exit=no_exit'
   return
end
;
total_errors=0
reference_file="banner_for_testsuite.pro"
filesw = FILE_WHICH(reference_file)
;
if (STRLEN(filesw) EQ 0) then begin
   ERRORS_ADD, total_errors, "file >>"+reference_file+"<< not found."
endif else begin
   if FILE_LINES(filesw) ne 210 then $
      ERRORS_ADD, total_errors, 'bad lenght'
   if FILE_LINES(filesw,/compress) ne 210 then $
      ERRORS_ADD, total_errors, 'bad lenght /compress'
endelse
;
; final message
;
BANNER_FOR_TESTSUITE, 'TEST_FILE_LINES', total_errors, short=short
;
if KEYWORD_SET(test) then STOP
;
if (total_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
end
