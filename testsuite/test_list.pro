;
; Alain, 28 March 2014: 
; first draft for a testsuite for LIST
; AC July: adding IsEmpty tests
;
pro MYMESS, errors, message
errors=errors+1
MESSAGE, /continue, message
end
;
pro TEST_LIST, help=help, verbose=verbose, short=short, $
               no_exit=no_exit, test=test
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_LIST, help=help, verbose=verbose, short=short, $'
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
txt0='bad counting of elements number '
txt=txt0
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
txt=txt0+'after LIST.ADD (singleton)'
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
txt=txt0+'after LIST.ADD (array)'
if (nbe1 NE known_nbe) then MYMESS, nb_errors, txt+'(N_ELEMENTS)'
if (nbe2 NE known_nbe) then MYMESS, nb_errors, txt+'(LIST.COUNT())'
if KEYWORD_SET(verbose) then print, 'counting OK after array add'
;
; Empty List ?
;
empty_list=LIST()
known_nbe=0
nbe1=N_ELEMENTS(empty_list)
;
txt=txt0+'after LIST.IsEmpty()'
if (nbe1 NE known_nbe) then MYMESS, nb_errors, txt+'(N_ELEMENTS)'
if (empty_list.IsEmpty() NE 1) then MYMESS, nb_errors, txt+'(it is LIST.IsEmpty())'
if (list.IsEmpty() NE 0) then MYMESS, nb_errors, txt+'(not LIST.IsEmpty())'
;
; ----------------- final messages ----------
;
BANNER_FOR_TESTSUITE, 'TEST_LIST', nb_errors, short=short
;
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end

