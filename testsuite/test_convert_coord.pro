;
; Alain C., 13 March 2014
;
; Under GNU GPL v2 or any later
;
; ********************************************************
; This is a very preliminary test suite for CONVERT_COORD
; the code in version before March 13 has several drawbacks
; it is better to collect known issues not to forget later !
; ********************************************************
;
; Types: 
; -- if at least one of the inputs (x,y,z)  or /double, then 
;  computation in double
; -- even if complex or Dcomplex, computation in float if no doubles
;    around
;
; Management of input dimensions if (x,y) or (x,y,z) provided :
;  -- if all elements are vectors, the smallest size is used.
;  -- if one element is a singleton, it is changed into REPLICATE
; of this singleton
; print,CONVERT_COORD([0, 5,2,2], [1, 7d,2], 4,/DATA, /TO_norm)
; --> must return 3 elements (Y size)
; print,CONVERT_COORD([0, 5,2,2], [1, 7d,2], [4],/DATA, /TO_norm)
; --> must return 1 element (Z size)
;
pro MYMESS, errors, message
errors=errors+1
MESSAGE, /continue, message
end
;
; -------------------------
;
pro TEST_CONVERT_COORD, help=help, test=test, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_CONVERT_COORD, help=help, test=test, no_exit=no_exit'
endif
;
if (!d.window GE 0) then begin
    MESSAGE, /continue, 'This test must start with no window'
    while (!d.window GE 0) do WDELETE
endif
;
nb_errors=0
;
expected=[0,5,4]
res=CONVERT_COORD([0, 1, 2, 3], [5,5,5], [4],/DATA, /TO_norm)
if ~ARRAY_EQUAL(res,expected) then MYMESS, nb_errors, 'bad dim proc 1'
;
expected=[[0,5,4],[1,5,4],[2,5,4]]
res=CONVERT_COORD([0, 1, 2, 3], [5,5,5], 4,/DATA, /TO_norm)
if ~ARRAY_EQUAL(res,expected) then MYMESS, nb_errors, 'bad dim proc 2'
;
name='TEST_CONVERT_COORD'
if (nb_errors GT 0) then begin
    MESSAGE, /continue, STRING(nb_errors)+' error(s) found in '+name
    if ~KEYWORD_SET(no_exit) then EXIT, status=1
endif else begin
    MESSAGE, /continue, 'No error found in '+name
endelse
;
if KEYWORD_SET(test) then STOP
;
end
