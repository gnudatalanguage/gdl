;
; Alain, 28 March 2014: 
; first draft for a testsuite for LIST
;
pro MYMESS, errors, message
errors=errors+1
MESSAGE, /continue, message
end
;
pro TEST_LIST, help=help, verbose=verbose, no_exit=no_exit, test=test
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_LIST, help=help, verbose=verbose, $'
   print, '               no_exit=no_exit, test=test'
   return
endif
;
nb_errors=0
;
list=LIST(1, 2, 3)
;
; testing basic counting
;
known_nbe=3
nbe1=N_ELEMENTS(list)
nbe2=list.count()
;
txt='bad counting of elements number '
if (nbe1 NE known_nbe) then MYMESS, nb_errors, txt+'(N_ELEMENTS)'
if (nbe2 NE known_nbe) then MYMESS, nb_errors, txt+'(LIST.COUNT())'
if KEYWORD_SET(verbose) then print, 'OK after basic countings'
;
; adding 2 elements and counting again
;
list.Add, 4
list.Add, 5
;
known_nbe=5
nbe1=N_ELEMENTS(list)
nbe2=list.count()
;
txt=txt+'after LIST.ADD (singleton)'
if (nbe1 NE known_nbe) then MYMESS, nb_errors, txt+'(N_ELEMENTS)'
if (nbe2 NE known_nbe) then MYMESS, nb_errors, txt+'(LIST.COUNT())'
if KEYWORD_SET(verbose) then print, 'counting OK after basic add'
;
; adding a array of 2 strings
;
list.Add, ["gdl1","gdl2"]
known_nbe=6
nbe1=N_ELEMENTS(list)
nbe2=list.count()
;
txt=txt+'after LIST.ADD (array)'
if (nbe1 NE known_nbe) then MYMESS, nb_errors, txt+'(N_ELEMENTS)'
if (nbe2 NE known_nbe) then MYMESS, nb_errors, txt+'(LIST.COUNT())'
if KEYWORD_SET(verbose) then print, 'counting OK after array add'
;
; ----------------- final messages ----------
;
if (nb_errors EQ 0) then begin
    MESSAGE, /continue, 'No error found in TEST_LIST'
endif else begin
    MESSAGE, /continue, STRING(nb_errors)+' errors found in TEST_LIST'
    if ~KEYWORD_SET(no_exit) then EXIT, status=1
endelse
;
if KEYWORD_SET(test) then STOP
;
end

