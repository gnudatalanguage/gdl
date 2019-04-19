;
; AC 2019-Jan-10
;
; Derived work from internal CHECK_MD5_OF_IMAGES in 
; "testsuite/test_read_standard_images.pro"
; pro CHECK_MD5_OF_IMAGES, images, test=test, verbose=verbose
;
; return 1 if and only if ALL checks are OK
;
; -----------------------------------------------------------
;
function MD5SUM_CHECK, list_files, list_md5, $
                       help=help, test=test, verbose=verbose
;
if (N_PARAMS() NE 2) then begin
   MESSAGE, /cont, 'two parameters are mandatory'
endif
;
if KEYWORD_SET(help) OR (N_PARAMS() NE 2) then begin
   print, 'function MD5SUM_CHECK, list_files, list_md5, $'
   print, '                       help=help, test=test, verbose=verbose'
   return, -1
endif
;
md5cmd=''
if STRLOWCASE(!version.os) eq 'linux' then md5cmd='md5sum '
if STRLOWCASE(!version.os) eq 'darwin' then  md5cmd='md5 -r '
;
if (STRLEN(md5cmd) EQ 0) then begin
   MESSAGE, /cont, 'MD5 command not known for your system: '+!version.os_name
   MESSAGE, /cont, 'please contribute, no test done. (!fixme!)'
   return, -1
endif
;
; We check whether the two list have same number of elements
;
if N_ELEMENTS(list_files) NE N_ELEMENTS(list_md5) then begin
   print, 'Number of elements in inputs are different ... Please check !'
   print, 'list_files : ', N_ELEMENTS(list_files)
   print, 'list_md5 : ', N_ELEMENTS(list_md5)
   return, -1
endif else begin
   nbps=N_ELEMENTS(list_md5)
endelse
;
; put all flags to failed
list_result=REPLICATE(0,nbps)
;
for ii=0, nbps-1 do begin
   ;;
   ;; We check whether the files exist !
   if ~FILE_TEST(list_files[ii]) then begin
      print, 'Missing file : '+list_files[ii]
      subresult='BAD'
   endif else begin
      SPAWN, md5cmd+list_files[ii], result, error
      ;;md5lenght=32
      subresult=STRMID(result,0, 32)
   endelse
   ;;
   if (subresult NE list_md5[ii]) then begin
      MESSAGE,/cont, 'MD5 checksum error for file : '+list_files[ii]
   endif else begin
      ;; put 1 since OK
      list_result[ii]=1
      if KEYWORD_SET(verbose) then $
         print, 'MD5 checksum OK for file : '+list_files[ii]
   endelse
endfor
;
if TOTAL(list_result) EQ nbps then begin
   print, 'MD5 checksum OK for ALL files !'
   res=1
endif else begin
   print, 'At least one bad MD5 checksum'
   if ~KEYWORD_SET(verbose) then print, '
   res=0
endelse
;
if KEYWORD_SET(test) then STOP
;
return, res
;
end

