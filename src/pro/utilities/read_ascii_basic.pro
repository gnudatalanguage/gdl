;
; Under GNU GPL V3+
; Alain C., October 2018
;
; Frequently, we just want to copy in a STRING Array all the lines
; of an ACSII file ...
;
function READ_ASCII_BASIC, filename, $
                           test=test, verbose=verbose, help=help
;
if (KEYWORD_SET(help) or (N_PARAMS() NE 1)) then begin
   print, 'function READ_ASCII_BASIC, filename, $'
   print, '                           test=test, verbose=verbose, help=help'
   return, ''
endif
;
ON_ERROR, 2
;
if N_ELEMENTS(filename) GT 1 then MESSAGE, 'Only one file at once !'
if ~FILE_TEST(filename) then MESSAGE, 'Missing file >>'+filename+'<<'
if FILE_TEST(filename, /zero) then $
   MESSAGE, 'File >>'+filename+'<< does exist but has zero-length'
;
txt='' 
restxt=''
;
GET_LUN, lun
OPENR, lun, filename
;
while ~EOF(lun) do begin
   READF, lun, txt
   ;;print, txt
   restxt=[restxt,txt]
endwhile
;
CLOSE, lun
FREE_LUN, lun
;
if N_ELEMENTS(restxt) GT 1 then restxt=restxt[1:*]
if KEYWORD_SET(verbose) then begin
   print, 'File : ', filename, ' read with ', N_ELEMENTS(restxt), ' lines.'
endif
;
if KEYWORD_SET(test) then STOP
;
return, restxt
;
end
;
