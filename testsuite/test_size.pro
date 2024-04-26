;
; AC 2017-12-14: one time again a bug report on problem on arrays
; with one dim equal to unity. 
; In 0.9.7 Vanilla in structure, now in basic array.
;
; bug 1 reported after 0.9.7 delivery
; bug 2 reported by Marteen R on Dec 13, 2017
;
; ------------------------
;
; Modifications history :
; - 2017-12-14 : AC. initial version
; - 2018-02-22 : AC : a test on self-promotion in L64 when needed is
;   in "test_l64.pro"
; - 2024-04-15 AC : cleaning (details ...) + some extra tests for LIST
;   and HASH () bugs found, see #issue 1807
;   + new tests for STRUCT
; - 2024-04-18 AC : bugs for HASH() and LIST() now solved in c++ code,
;   then new tests added !
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
if ~res then ERRORS_ADD, nb_errors, 'bad assigantion [0,0] not really 2D'
;
; this one should never work !
;
print, 'The following message is OK'
res=EXECUTE('array1d[0,0]=array2d')
if res then ERRORS_ADD, nb_errors, 'bad assignation [0,0]'
;
res=EXECUTE('array1d[1,0,0]=array1d[1:8]')
if ~res then ERRORS_ADD, nb_errors, 'bad assignation [1,0,0]'

res=EXECUTE('array1d[0]=arraynotreally2d')
if ~res then ERRORS_ADD, nb_errors, 'bad assignation [0]'
;
; -----------
;
BANNER_FOR_TESTSUITE, 'TEST_BUG_675', nb_errors, /status
ERRORS_CUMUL, cumul_errors, nb_errors
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------
;
function ARE_SIZE_STRUCTS_EQUAL, struct1, struct2, mess=mess, debug=debug
;
errors=0
if (struct1.type_name ne struct2.type_name) then ERRORS_ADD, errors, mess+'type_name'
if (struct1.structure_name ne struct2.structure_name) then ERRORS_ADD, errors, mess+'struct_name'
if (struct1.type ne struct2.type) then ERRORS_ADD, errors, mess+'type'
if (struct1.file_lun ne struct2.file_lun) then ERRORS_ADD, errors,mess+'file_lun'
if (struct1.file_offset ne struct2.file_offset) then ERRORS_ADD, errors, mess+'file_offset'
if (struct1.n_elements ne struct2.n_elements) then ERRORS_ADD, errors, mess+'n_elements'
if (struct1.n_dimensions ne struct2.n_dimensions) then ERRORS_ADD, errors, mess+'n_dimensions'
if ~ARRAY_EQUAL(struct1.dimensions, struct2.dimensions) then $
   ERRORS_ADD, errors,mess+'dimensions'

;
if KEYWORD_SET(debug) then begin
   print, 'dim 1 : ', struct1.dimensions
   print, 'dim 2 : ', struct2.dimensions
endif
;
return, errors
end
; ------------------------
;
pro TEST_SIZE_STRUCTURE, cumul_errors, verbose=verbose, $
                         help=help, test=test, debug=debug
;
errors=0
;
stemplate=SIZE(create_struct('toto',1),/structure)
stemplate.structure_name='TEST'
stemplate.n_elements=10
stemplate.n_dimensions=1
stemplate.dimensions[0]=10
;
; first case (1D)
;
structarray=REPLICATE({test, value:0.0},10)
struc1 = SIZE(structarray,/struct)
errors=errors+ARE_SIZE_STRUCTS_EQUAL(struc1, stemplate, mess='struc 1, ')
;
; second case
struc2=SIZE(structarray[0],/struct)
stemplate.n_elements=SIZE(structarray[0],/n_elements)
stemplate.dimensions=SIZE(structarray[0],/dimensions)
errors=errors+ARE_SIZE_STRUCTS_EQUAL(struc2, stemplate, mess='struc 2, ')
;
; third case (2D)
;
structarray=REPLICATE({test, value:0.0},10,2)
struc3 = SIZE(structarray,/struct)
stemplate.n_elements=SIZE(structarray,/n_elements)
stemplate.n_dimensions=SIZE(structarray,/n_dimensions)
stemplate.dimensions=LONARR(8)
stemplate.dimensions=SIZE(structarray,/dimensions)
errors=errors+ARE_SIZE_STRUCTS_EQUAL(struc3, stemplate, mess='struc 3, ')
;
; third case, cut 1
;
structarray=REPLICATE({test, value:0.0},10,2)
struc3c1 = SIZE(structarray[*,0],/struct)
stemplate.n_elements=SIZE(structarray[*,0],/n_elements)
stemplate.n_dimensions=SIZE(structarray[*,0],/n_dimensions)
stemplate.dimensions=LONARR(8)
stemplate.dimensions=SIZE(structarray[*,0],/dimensions)
errors=errors+ARE_SIZE_STRUCTS_EQUAL(struc3c1, stemplate, mess='struc 3 c1, ')
;
; third case, cut 2
;
structarray=REPLICATE({test, value:0.0},10,2)
struc3c2 = SIZE(structarray[2,*],/struct)
stemplate.n_elements=SIZE(structarray[2,*],/n_elements)
stemplate.n_dimensions=SIZE(structarray[2,*],/n_dimensions)
stemplate.dimensions=LONARR(8)
stemplate.dimensions=SIZE(structarray[2,*],/dimensions)
errors=errors+ARE_SIZE_STRUCTS_EQUAL(struc3c2, stemplate, mess='struc 3 c2, ')
;
;
; --------------
;
BANNER_FOR_TESTSUITE, "TEST_SIZE_STRUCTURE", errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
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
; ------------------------
;
pro TEST_SIZE_HASH, cumul_errors, dims=dims, verbose=verbose, $
                    help=help, test=test, debug=debug
;
errors=0
;
myhash=HASH('a',1,'b',2,'c',3)
;
count = myhash.count()
nel = SIZE(myhash, /N_elements)
szchk = [1, nel, 11, nel]
if (nel ne count) then ERRORS_ADD, errors,'(1) size(<hash>) wrong'
if ~ARRAY_EQUAL(szchk, SIZE(myhash)) then ERRORS_ADD, errors,'(2) size(<hash>) wrong'
;
; detailed comparison of each field in SIZE(/structure) !
;
strucobj = SIZE(myhash,/struct)
;
; create a template and put the expected values ...
expected=SIZE(HASH(),/struct)
expected.n_elements=count
expected.n_dimensions=SIZE(myhash, /N_dim)
expected.dimensions=LONARR(8)
expected.dimensions[0]=nel
errors=errors+ARE_SIZE_STRUCTS_EQUAL(strucobj, expected, mess='<<hash>> ')
;
; --------------
;
BANNER_FOR_TESTSUITE, "TEST_SIZE_HASH", errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------
;
pro TEST_SIZE_LIST, cumul_errors, dims=dims, verbose=verbose, $
                    help=help, test=test, debug=debug
;
errors=0
;
mylist=LIST('a',1,'b',2,'c',3)
;
count = mylist.count()
nel = SIZE(mylist, /N_elements)
szchk = [1, nel, 11, nel]
if (nel ne count) then ERRORS_ADD, errors,'(1) size(<list>) wrong'
if ~ARRAY_EQUAL(szchk, SIZE(mylist)) then ERRORS_ADD, errors,'(2) size(<list>) wrong'
;
ndims=SIZE(mylist, /N_dim)
;
; detailed comparison of each field in SIZE(/structure) !
;
strucobj = SIZE(mylist,/struct)
;
; create a template and put the expected values ...
expected=SIZE(LIST(),/struct)
expected.n_elements=count
expected.n_dimensions=SIZE(mylist, /N_dim)
expected.dimensions=LONARR(8)
expected.dimensions[0]=nel
errors=errors+ARE_SIZE_STRUCTS_EQUAL(strucobj, expected, mess='<<list>> ')
;
; test on LIST() with internal dims ...
;
mylist=LIST(DIST(4),RANDOMU(seed,2,3,4))
strucobj0=SIZE(mylist[0],/struct)
strucobj1=SIZE(mylist[1],/struct)
;
expected=SIZE(0.,/struct)
exp0=expected
exp0.n_elements=SIZE(mylist[0], /N_elem)
exp0.n_dimensions=SIZE(mylist[0], /N_dim)
exp0.dimensions=LONARR(8)
exp0.dimensions[0:1]=[4,4]
errors=errors+ARE_SIZE_STRUCTS_EQUAL(strucobj0, exp0, mess='<<list>> 0 ')

exp1=expected
exp1.n_elements=SIZE(mylist[1], /N_elem)
exp1.n_dimensions=SIZE(mylist[1], /N_dim)
exp1.dimensions=LONARR(8)
exp1.dimensions[0:2]=[2,3,4]
errors=errors+ARE_SIZE_STRUCTS_EQUAL(strucobj1, exp1, mess='<<list>> 1 ')

; --------------
;
BANNER_FOR_TESTSUITE, "TEST_SIZE_LIST", errors, /short, verb=verbose
ERRORS_CUMUL, cumul_errors, errors
if KEYWORD_SET(test) then STOP
;
end
;
; ------------------------
;
pro TEST_SIZE_ARRAY, cumul_errors, dims=dims, verbose=verbose, $
                     help=help, test=test, debug=debug
;
nb_errors=0
;
if ~KEYWORD_SET(dims) then dims=[1,2,3,4]
;
if KEYWORD_SET(verbose) or KEYWORD_SET(debug) then begin 
   MESSAGE, /Continue, 'begin of test'
   print, 'Circulating (shift) on dims : ', dims
endif
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
      ERRORS_ADD, nb_errors, 'bad SIZE for case :'+ii
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
if KEYWORD_SET(verbose) or KEYWORD_SET(debug) then $
   MESSAGE, /Continue, 'end of test'
;
; --------------------------
;
BANNER_FOR_TESTSUITE, 'TEST_SIZE_ARRAY', nb_errors, /status
ERRORS_CUMUL, cumul_errors, nb_errors
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
    print, '               help=help, test=test, debug=debug, $'
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
TEST_SIZE_HASH, nb_errors, verbose=verbose, $
                help=help, test=test, debug=debug
TEST_SIZE_LIST, nb_errors,  verbose=verbose, $
                help=help, test=test, debug=debug
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
