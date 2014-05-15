;
; Alain Coulais, 6 May 2014
;
; preliminatry test suite for function ROUTINE_NAMES,
; currently focused on the data 
;
; Thanks to CMSV lib., a bug related to ROUTINE_NAMES("var_name", fetch= )
; was discorvered.
;

; this  is a derivated work (strong simplification) from CMSV_TEST_SET,
; a procedure inside CMVSlib (Craig Markwark), in the file "cmsv_test.pro"
;
pro AC_CMSV_TEST_SET, a, b, c, d, e
;
a = 1
b = FINDGEN(10)
c = (5-DINDGEN(20))*!dpi
d = {tag1: 1, tag2: 2, tag3: 'tag3', tag4: [4], tag5: FINDGEN(10), $
     tag6: {tag6a: 'a', tag6b: 'DEADBEEF'XL} }
e = DCOMPLEXARR(2,2) + 3.1415
;
end
;
;
;pro basic_internal, names
;for ii=0, N_ELEMENTS(names)-1 do begin
;    print, names[ii], routine_names(names[ii], fetch=0)
;endfor
;end

pro INTERNAL_VARIABLES, names, size_of_var, nb_errors, verbose=verbose, debug=debug, test=test
;
errors=0
;
vide=[0L,0,0]
;
cote='"'
;
for ii=0, N_ELEMENTS(names)-1 do begin
   name=names[ii]
   fname="routine_names("+cote+name+cote
   ;;
   for level=0, 4 do begin
      fname_lev=fname+",fetch="+STRING(level)+")"
      commande='sz=SIZE('+fname_lev+')'
      if KEYWORD_SET(debug) then print, ii, level, commande
      res=EXECUTE(commande)
      if KEYWORD_SET(verbose) then print, 'execute status:', res, ', fetch level=', level, ' : sz : ', sz
      ;;
      if (res NE 1) then errors++
      if (level EQ 2) then begin
          expected_sz=*size_of_var[ii]
          if ~ARRAY_EQUAL(sz, expected_sz) then begin
              MESSAGE,/continue, 'Error in DATA expected result (please check shift)'
              errors++
          endif
      endif else begin
          if ~ARRAY_EQUAL(sz, vide) then begin
              MESSAGE,/continue, 'Error in void expected result'
              errors++
          endif
      endelse
   endfor
endfor
;
if (errors GT 0) then begin
    print, 'Errors found '
endif
;
if KEYWORD_SET(test) then STOP
;
nb_errors=nb_errors+errors
;
end
;

;
pro INTERNAL_KEYWORDS, NAMES=names, info_size=info_size, nb_errors, verbose=verbose, debug=debug, test=test
;
MESSAGE, /continue, 'Begin of INTERNAL_KEYWORDS procedure'
;
size_of_var=info_size
;
errors=0
;
;vide=LONG([0L,0,0])
vide=[0L,0,0]
;
cote='"'
;
for ii=0, N_ELEMENTS(names)-1 do begin
   name=names[ii]
   fname="routine_names("+cote+name+cote
   ;;
   for level=0, 4 do begin
      fname_lev=fname+",fetch="+STRING(level)+")"
      commande='sz=SIZE('+fname_lev+')'
      if KEYWORD_SET(debug) then print, ii, level, commande
      res=EXECUTE(commande)
      if KEYWORD_SET(verbose) then print, 'execute status:', res, ', fetch level=', level, ' : sz : ', sz
      ;;
      if (res NE 1) then errors++
      if (level EQ 2) then begin
          expected_sz=*size_of_var[ii]
          if ~ARRAY_EQUAL(sz, expected_sz) then begin
              MESSAGE,/continue, 'Error in DATA expected result (please check shift)'
              errors++
          endif
      endif else begin
          if ~ARRAY_EQUAL(sz, vide) then begin
              MESSAGE,/continue, 'Error in void expected result'
              errors++
          endif
      endelse
   endfor
endfor
;
if (errors GT 0) then begin
    MESSAGE, /continue, 'End of INTERNAL_KEYWORDS procedure : Errors found'
endif else begin
    MESSAGE, /continue, 'End of INTERNAL_KEYWORDS procedure : NO Errors found'
endelse
;
if KEYWORD_SET(test) then STOP
;
nb_errors=nb_errors+errors
;
end
;
; ----------------------------------------------------
;
pro TEST_ROUTINE_NAMES, verbose=verbose, short=short, debug=debug, test=test, no_exit=no_exit
;
if KEYWORD_SET(help) then begin
    print, 'pro TEST_ROUTINE_NAMES, verbose=verbose, short=short, debug=debug, test=test, no_exit=no_exit'
    return
endif
;
nb_errors=0
;
AC_CMSV_TEST_SET, a, b, c, d, e
;
;pheap = [ptr_new(a), ptr_new(b), ptr_new(c), ptr_new(d), ptr_new(e)]
;
info_size=[PTR_NEW(SIZE(a)),PTR_NEW(SIZE(b)),PTR_NEW(SIZE(c)),PTR_NEW(SIZE(d)),PTR_NEW(SIZE(e))]
;
;
INTERNAL_VARIABLES, ['A','B','C','D','E'], info_size, nb_errors, $
  verbose=verbose, debug=debug, test=test
;
;
INTERNAL_KEYWORDS, names=['A','B','C','D','E'], info_size=info_size, nb_errors, $
  verbose=verbose, debug=debug, test=test
;
BANNER_FOR_TESTSUITE, 'TEST_ROUTINE_NAMES', nb_errors, short=short
;
if (nb_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end

