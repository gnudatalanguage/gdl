;
; AC, July 2017, GPL V3+
;
; Generating the radical of file name based on :
; -- machine name (hostname -a)
; -- exact date (up to second level) [OK on Linux and OSX]
;
function BENCHMARK_GENERATE_FILENAME, radical, short=short, $
                                  test=test, verbose=verbose, help=help
;
if KEYWORD_SET(help) then begin
   print, 'function BENCHMARK_GENERATE_FILENAME, radical, short=short, $'
   print, '                             test=test, verbose=verbose, help=help'
   return, ''
endif
;
name=''
;
date=timestamp()
if KEYWORD_set(short) then begin
   result='y'+strmid(date,0,4)+'m'+strmid(date,5,2)+'d'+strmid(date,8,2)
endif else begin
    result='y'+strmid(date,0,4)+'m'+strmid(date,5,2)+'d'+strmid(date,8,2)+'_'+strjoin( strsplit( strmid(date,11,8) , /extract, ':'), path_sep(/search))
endelse
;
machine=GET_LOGIN_INFO()
;
name=machine.machine_name+'_'+result
;
; do we have a "radical"
;
if N_PARAMS() EQ 0 then radical=''
;
; prefixe + radical + GDL/IDL/FL
;
prefix='bench_'+STRCOMPRESS(radical,/remove_all)
res=EXECUTE("interpreter=GDL_IDL_FL(/uppercase)")
if (res NE 1) then begin
   DEFSYSV, '!gdl', exist=exist
   if (exist EQ 1) then interpreter='GDL' else interpreter='IDL'
endif
;
prefix=prefix+'_'+interpreter+'_'
name=prefix+name+'.xdr'
;
if KEYWORD_SET(verbose) then print, name
if KEYWORD_SET(test) then STOP
;
return, name
;
end
;
