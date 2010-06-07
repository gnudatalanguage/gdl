;
; do the call to FFTw give Memory leaks ?
; (maybe we can generalyse such tests on other por/funct ?)
;
; Lea Noreskal and Alain Coulais
; Mai 2010
; under GNU GPL 2 or later
;
; testing possible memory leak in FFT 
; http://sourceforge.net/tracker/?func=detail&aid=3009319&group_id=97659&atid=618683
;
; Known problems: MEMORY() is not working on Linux OS as is on June 7, 2010
;
pro TEST_FFT_LEAK_ONE_TYPE, type=type, lenght=lenght, nb_iter_max=nb_iter_max, $
                            help=help, test=test, verbose=verbose, max_leak=max_leak
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_FFT_LEAK_ONE_TYPE, type=type, lenght=lenght, nb_iter_max=nb_iter_max, $'
   print, '                            help=help, test=test, verbose=verbose, max_leak=max_leak'
endif
;
if N_ELEMENTS(lenght) EQ 0 then lenght=1024
;
; types: 4: real, 5 double, 6 complex ...
;
if N_ELEMENTS(type) EQ 0 then type=4
;
x=MAKE_ARRAY(lenght, type=type)
y=x
;
if N_ELEMENTS(nb_iter_max) EQ 0 then nb_iter_max=1000
;
Resu_mem=FLTARR(nb_iter_max)
;
for i=0L, nb_iter_max-1 do begin
    x += FFT(y)
    resu_mem[i]=(MEMORY())[0]
endfor
;
max_leak=MAX(resu_mem-resu_mem[0])
;
if KEYWORD_SET(verbose) then begin
    print, 'type: ', type, ' , max leak size: ', max_leak
endif
;
if KEYWORD_SET(test) then STOP
;
end

pro TEST_FFT_LEAK, verbose=verbose, test=test, force=force
;
; /force will allow to skip the test on OS type ...
if NOT(KEYWORD_SET(force)) then begin
    if (STRLOWCASE(!version.OS) NE 'darwin') then begin
        MESSAGE, 'AC and LN 2010/06/07: because MEMORY() not working now on Linux', /continue
        MESSAGE, 'AC and LN 2010/06/07: we cannont do these tests ...', /continue
        EXIT, status=77
    endif
endif
;
nb_pb=0
;
for ii=0, 15 do begin
   if ii EQ 8 then CONTINUE   ;; Struc
   if ii EQ 10 then CONTINUE  ;; Pointer
   if ii EQ 11 then CONTINUE  ;; Objref
   ;;
   TEST_FFT_LEAK_ONE_TYPE, type=ii, max_leak=max_leak, verbose=verbose
   ;;
   ;; empirical value working for OSX
   if (max_leak GT 10000) then nb_pb=nb_pb+1
   ;;
endfor
;
if nb_pb GT 0 then begin
    MESSAGE, STRING(nb_pb)+' case of memory leak when calling FFT have been found',/continue
    EXIT, status=1
endif else begin
    MESSAGE, 'NO case of memory leak when calling FFT have been found',/continue
endelse
;
if KEYWORD_SET(test) then STOP
;
end
