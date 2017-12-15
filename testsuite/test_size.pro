;
; AC 2017-12-14: one time again a bug report on problem on arrays
; with one dim equal to unity. 
; In 0.9.7 Vanilla in structure, now in basic array.
;
; bug 1 reported after 0.9.7 delivery
; bug 2 reported by Marteen R on Dec 13, 2017
;
; Modifications history :
; - 2017-12-14 : AC. initial version
;
; ------------------------
; reported by Giloo in 2015-10-30
; https://sourceforge.net/p/gnudatalanguage/bugs/675/
;
pro TEST_BUG_675, cumul_errors, verbose=verbose, $
                  help=help, test=test, debug=debug
;
nb_errors=0
;
array1d=REPLICATE(0d,[9])
array2d=REPLICATE(1d,[3,3])
arraynotreally2d=REFORM(REPLICATE(1d,9),[9,1]); is equivalent to a 1-dim array
;
res=EXECUTE('array1d[0,0]=arraynotreally2d')
if ~res then ADD_ERROR, nb_errors, 'bad assigantion [0,0] not really 2D'
;
; this one should never work !
;
;res=EXECUTE('array1d[0,0]=array2d')
;if ~res then ADD_ERROR, nb_errors, 'bad assigantion [0,0]'
;
res=EXECUTE('array1d[1,0,0]=array1d[1:8]')
if ~res then ADD_ERROR, nb_errors, 'bad assigantion [1,0,0]'

res=EXECUTE('array1d[0]=arraynotreally2d')
if ~res then ADD_ERROR, nb_errors, 'bad assigantion [0]'
;
; -----------
;
BANNER_FOR_TESTSUITE, 'TEST_BUG_675', nb_errors, /short
;
ERRORS_CUMUL, cumul_errors, nb_errors
;
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------
;
pro TEST_SIZE_STRUCTURE, cumul_errors, verbose=verbose, $
                         help=help, test=test, debug=debug
;
; note by AC: yes I have a snippet somewhere (;-;)
;
end
;
; ------------------------
; Reported by Maarten R. on Dec 13, 2017
; A previous problem with other places for 1D was discovered 
; in the 0.9.7 (Vanilla) then solved in Debian .deb
;
function REMOVE_LAST_1D, input, verbose=verbose
;
while ((N_ELEMENTS(input) GT 1) AND (input[-1] EQ 1)) do begin
   if KEYWORD_SET(verbose) then print, input
   input=input[0:N_ELEMENTS(input)-2]
endwhile
;
return, input
;
end
;
pro TEST_SIZE_ARRAY, cumul_errors, dims=dims, verbose=verbose, $
                     help=help, test=test, debug=debug
;
nb_errors=0
;
if ~KEYWORD_SET(dims) then dims=[1,2,3,4]
;
MESSAGE, /Continue, 'begin of test'
print, 'Circulating (shift) on dims : ', dims
;
type=4 ; float. Should we iterate on the type ??
;
for ii=0, N_ELEMENTS(dims)-1 do begin
   current_dims=SHIFT(dims, ii)
   tab=MAKE_ARRAY(current_dims)
   size_effective=SIZE(tab)
   clean_dims=REMOVE_LAST_1D(current_dims)
   size_expected=[N_ELEMENTS(clean_dims),clean_dims,type,PRODUCT(clean_dims,/pre)]
   ;;
   if ~ARRAY_EQUAL(size_effective,size_expected) then begin
      ADD_ERROR, nb_errors, 'bad SIZE for case :'+ii
      print, 'current dims :', current_dims
      print, 'correct (clean) dims :', clean_dims
   endif
   ;;
   if KEYWORD_SET(verbose) then begin
      print, 'Expected SIZE :', size_expected
      print, 'computed SIZE :', size_effective
      
   endif
endfor
;
BANNER_FOR_TESTSUITE, 'TEST_SIZE_ARRAY', nb_errors, /short
;
ERRORS_CUMUL, cumul_errors, nb_errors
;
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------
;
pro TEST_SIZE, no_exit=no_exit, verbose=verbose, $
                     help=help, test=test, debug=debug
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_SIZE, no_exit=no_exit, verbose=verbose, $'
    print, '                     help=help, test=test, debug=debug, $'
    return
endif
;
nb_errors=0
;
TEST_BUG_675, nb_errors, verbose=verbose
;
TEST_SIZE_STRUCTURE, nb_errors, verbose=verbose
;
TEST_SIZE_ARRAY, nb_errors, verbose=verbose
TEST_SIZE_ARRAY, nb_errors, dims=[1,1,1,1,1], verbose=verbose
TEST_SIZE_ARRAY, nb_errors, dims=[1,10,1,10,1], verbose=verbose
;
; ----------------- final message ----------
;
BANNER_FOR_TESTSUITE, 'TEST_SIZE', nb_errors
;
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
