;
; AC, October 21, 2018, GPL V3+
;
; Collecting informations on the interpreter version (GDL or IDL or FL)
; -- soft
; -- version
;
function BENCHMARK_INFO_SOFT, test=test, verbose=verbose, help=help
;
if KEYWORD_SET(help) then begin
   print, 'function BENCHMARK_INFO_SOFT, test=test, verbose=verbose, help=help'
   return, -1
endif
;
info_soft={name : '', release : ''}
info_soft.name=GDL_IDL_FL()
;
if info_soft.name EQ 'GDL' then version=!gdl.release
if info_soft.name EQ 'IDL' then version=!version.release
if info_soft.name EQ 'FL' then version='Unknown'
;
if KEYWORD_SET(verbose) then HELP,/struct, info_soft
if KEYWORD_SET(test) then STOP
;
return, info_soft
;
end
;
