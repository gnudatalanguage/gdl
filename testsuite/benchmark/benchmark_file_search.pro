;
; AC, August 7, 2017, under GPL v2+
;
; ----------------------------------------------------
; Modifications history :
;
; 2018-Sep-04 : AC. 
; 1/ "filter" can be a liste
; 2/ return liste='' if nothing found
;
; ----------------------------------------------- 
;
function BENCHMARK_FILE_SEARCH, filter, message, path=path, $
                            test=test, vervose=verbose, help=help
;
ON_ERROR, 2
;
if N_PARAMS() NE 2 then MESSAGE, 'missing inputs parameters'
;
if ~ISA(filter) then MESSAGE, 'filter input must be set'
if ~ISA(message) then MESSAGE, 'message input must be set'
;
message='No '+STRUPCASE(message)+' test data found.'
;
if KEYWORD_SET(path) then filter=path+PATH_SEP()+filter
;
liste=FILE_SEARCH(filter, count=count)
if (count EQ 0) then begin
   MESSAGE, /continue, message
   print, 'used filter(s) : ', filter
   liste=''
endif
;
if KEYWORD_SET(test) then begin
   print, 'used filter(s) : ', filter
   print, 'liste : ', liste
endif
;
if KEYWORD_SET(test) then STOP
;
return, liste
;
end
