;
; AC, August 7, 2017, under GPL v2+
;
function BENCHMARK_FILE_SEARCH, filter, message, path=path, $
                            test=test, vervose=verbose, help=help
;
ON_ERROR, 2
;
if N_PARAMS() NE 2 then MESSAGE, 'missing inputs parameters'
;
if STRLEN(filter) EQ 0 then MESSAGE, 'filter input must be set'
if STRLEN(message) EQ 0 then MESSAGE, 'message input must be set'
message='No '+STRUPCASE(message)+' test data found.'
;
if KEYWORD_SET(path) then filter=path+PATH_SEP()+filter
;
liste=FILE_SEARCH(filter, count=count)
if (count EQ 0) then MESSAGE, message
;
if KEYWORD_SET(test) then STOP
;
return, liste
;
end
