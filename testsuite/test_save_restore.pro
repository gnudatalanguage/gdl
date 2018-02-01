;
; AC 2017-Dec-24
;
; basic tests on SAVE/RESTORE on numeric types
; (with various dims)
;
; NB 2: cross-tests are possible
; (writing in Lang 1 & reading in Lang 2 ... see prefix)
;
; -----------------------------------------------
;
pro TEST_RESTORE_NUMERIC, cumul_errors, file=file, dim1ref=dim1ref, dim2ref=dim2ref, $
                          help=help, verbose=verbose, test=test
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_RESTORE_NUMERIC, dim1, dim2, cumul_errrors, file=file, $'
   print, '                          help=help, verbose=verbose, test=test'
   return
endif
;
errors=0
;
RESTORE, file
MESSAGE, /continue, 'SAVE file <<'+file+'>>for numeric tests READ BACK'
;
GIVE_LIST_NUMERIC, list_numeric_types, list_numeric_names
;
; check expected dimensions ... (dim1 & dim2 stored in SAV file)
;
if KEYWORD_SET(dim1ref) then begin
   if ~ARRAY_EQUAL(dim1ref, dim1, /no_typeconv) then $
      ADD_ERROR, errors, 'bad dim for DIM1'
endif
if KEYWORD_SET(dim2ref) then begin
   if ~ARRAY_EQUAL(dim2ref, dim2, /no_typeconv) then $
      ADD_ERROR, errors, 'bad dim for DIM2'
endif
;
for ii=0,N_ELEMENTS(list_numeric_names)-1 do begin
   ;;
   type_value=list_numeric_types[ii]
   type_name=list_numeric_names[ii]
   if KEYWORD_SET(test) then print, 'test on type : ', type_name
   ;;
   ;; generating the arrays to be compared with good type and values
   ;;
   tmp0d=FIX(1, type=type_value)
   tmp1d=INDGEN(dim1, type=type_value)
   tmp2d=INDGEN(dim2, type=type_value)
   ;;
   ;; associating the restored values
   ;;
   ok0=EXECUTE('d0d='+type_name+'_s')
   if ~ok0 then ADD_ERROR, errors, 'data "'+type_name+'_s" not found in SAVE file'
   ok1=EXECUTE('d1d='+type_name+'_s_a1d')
   if ~ok1 then ADD_ERROR, errors, 'data "'+type_name+'_s_a1d" not found in SAVE file'
   ok2=EXECUTE('d2d='+type_name+'_s_a2d')
   if ~ok2 then ADD_ERROR, errors, 'data "'+type_name+'_s_a2d" not found in SAVE file'
   ;;
   if ~ARRAY_EQUAL(tmp0d, d0d, /no_typeconv) then $
      ADD_ERROR, errors, 'type '+STRUPCASE(type_name)+' Singleton'
   if ~ARRAY_EQUAL(tmp1d, d1d, /no_typeconv) then $
      ADD_ERROR, errors, 'type '+STRUPCASE(type_name)+' Array 1D'
   if ~ARRAY_EQUAL(tmp2d, d2d, /no_typeconv) then $
      ADD_ERROR, errors, 'type '+STRUPCASE(type_name)+' Array 2D'
endfor
;
BANNER_FOR_TESTSUITE, "TEST_RESTORE_NUMERIC", errors, /short, verb=verbose
;
ERRORS_CUMUL, cumul_errors, errors
;
if KEYWORD_SET(test) then STOP
;
end
;
; -----------------------------------------------
;
pro TEST_SAVE_NUMERIC, dim1, dim2, file=file, $
                       help=help, verbose=verbose, test=test
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_SAVE_NUMERIC, dim1, dim2, file=file, $'
   print, '                       help=help, verbose=verbose, test=test'
   return
endif
;
GIVE_LIST_NUMERIC, list_numeric_types, list_numeric_names
;
for ii=0,N_ELEMENTS(list_numeric_names)-1 do begin
   ;;
   type_value=list_numeric_types[ii]
   type_name=list_numeric_names[ii]
   if KEYWORD_SET(test) then print, 'test on type : ', type_name
   ;;
   ;; generating on the fly the variables (type_name + '_s' + dim info)
   ;;
   ok0=EXECUTE(type_name+'_s=FIX(1, type=type_value)')
   ok1=EXECUTE(type_name+'_s_a1d=INDGEN(dim1, type=type_value)')
   ok2=EXECUTE(type_name+'_s_a2d=INDGEN(dim2, type=type_value)')
   if KEYWORD_SET(test) then print, type_name, ok0, ok1, ok2
endfor
;
SAVE, file=file, dim1, dim2, $
      byte_s, byte_s_a1d, byte_s_a2d, $
      int_s, int_s_a1d, int_s_a2d, $
      long_s, long_s_a1d, long_s_a2d, $
      float_s, float_s_a1d, float_s_a2d, $
      double_s, double_s_a1d, double_s_a2d, $
      complex_s, complex_s_a1d, complex_s_a2d, $
      dcomplex_s, dcomplex_s_a1d, dcomplex_s_a2d, $
      uint_s, uint_s_a1d, uint_s_a2d, $
      ulong_s, ulong_s_a1d, ulong_s_a2d, $
      long64_s, long64_s_a1d, long64_s_a2d, $
      ulong64_s, ulong64_s_a1d, ulong64_s_a2d
;
MESSAGE, /continue, 'SAVE file <<'+file+'>>for numeric tests WRITTEN'
;
if KEYWORD_SET(test) then STOP
;
end
;
; -----------------------------------------------
;
pro TEST_SAVE_RESTORE, help=help, test=test, verbose=verbose
;
if KEYWORD_SET(help) then begin
   print, 'pro TEST_SAVE_RESTORE, help=help, verbose=verbose, test=test'
   return
endif
;
total_errors=0
;
; first test : 0, 1 & 2 D
;
prefix=GDL_IDL_FL()
path=GETENV('IDL_TMPDIR')
;
radical='_save_test_numeric1.sav'
fullfile=path+prefix+radical
;
dim1=10
dim2=[2,5]
;
TEST_SAVE_NUMERIC, dim1, dim2, file=fullfile, test=test, verbose=verbose
TEST_RESTORE_NUMERIC, total_errors, file=fullfile, dim1ref=dim1, dim2ref=dim2, $
                      test=test, verbose=verbose
;
; second test : 0, 3 & 4 D
;
radical='_save_test_numeric2.sav'
fullfile=path+prefix+radical
;
dim1=[2,1,5]
dim2=[5,3,4,5]
;
TEST_SAVE_NUMERIC, dim1, dim2, file=fullfile, test=test, verbose=verbose
TEST_RESTORE_NUMERIC, total_errors, file=fullfile, dim1ref=dim1, dim2ref=dim2, $
                      test=test, verbose=verbose
;
; final message
;
BANNER_FOR_TESTSUITE, 'TEST_SAVE_RESTORE', total_errors, short=short
;
if KEYWORD_SET(test) then STOP
;
if (total_errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1

end

