;
; AC 28/01/2013: I found no equivalent tests in the testsuite !
;
; bug reported by Gilles on Jan. 23, 2013
; http://sourceforge.net/tracker/?func=detail&aid=3601949&group_id=97659&atid=618683
;
pro TEST_STRUCTURES, no_exit=no_exit, verbose=verbose, $
                     help=help, test=test, debug=debug
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_STRUCTURES, no_exit=no_exit, verbose=verbose, $'
    print, '                     help=help, test=test, debug=debug, $'
    return
endif
;
nb_errors=0
;
structarray=REPLICATE({test, value:0.0},10)
;populate values:
structarray.value=FINDGEN(10)
;
HELP, structarray.value
;
;get subset:
www=WHERE(structarray.value gt 6)
HELP, www
;
if (ARRAY_EQUAL(www, [7,8,9]) NE 1) then begin
    message,/continue, '(1) unexpected results in <<www>> values !'
    nb_errors=nb_errors+1
endif
;
res1=EXECUTE('HELP, structarray[www].value')
if (res1 NE 1) then begin
    message,/continue, ' unexpected badly interpreted Struct indexing ! (case 1)'
    nb_errors=nb_errors+1
endif
;
tab=0.
res2=EXECUTE('tab=structarray[www].value')
if (res2 NE 1) then begin
    message,/continue, ' unexpected badly interpreted Struct indexing ! (case 2)'
    nb_errors=nb_errors+1
endif
;
if (ARRAY_EQUAL(tab, 1.*[7,8,9]) NE 1) then begin
    message,/continue, '(2) unexpected results in extracted values !'
    nb_errors=nb_errors+1
endif
;
if (nb_errors GT 0) then begin
    MESSAGE, STRING(nb_errors)+' Errors found', /continue
endif else begin
    MESSAGE, ' No Errors found', /continue
endelse
;
if KEYWORD_SET(test) then STOP
;
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
end
