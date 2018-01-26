
pro TEST_MAKE_DLL, no_exit=no_exit, help=help, test=test, verbose=verbose
;
;
radix='test_make_dll'
c_file=radix+'.c'
so_file=GDL_IDL_FL(/upper)+'_'+radix;+'.so'
idl_so_file=radix+'.so'
;
; do we have the "c_file" around ?
;
if ~FILE_TEST(c_file) then MESSAGE, 'missing input C file !'
;
;radix_c_file=FILE_BASENAME(c_file, '.c')
tmp=FILE_BASENAME(so_file, '.so')
;
; working local !
;
srcdir=FILE_DIRNAME(FILE_EXPAND_PATH(c_file))
print, srcdir
;
bindir='bin'
file_mkdir, bindir
fulldir=srcdir+PATH_SEP()+bindir
;
; calling the code !
;
MAKE_DLL, radix+'.c', tmp, $
          input_directory=srcdir, $
          output_directory=fulldir, $
          /SHOW_ALL_OUTPUT, /verbose
;
;if (GDL_IDL_FL(/upper) EQ 'GDL') then FILE_MOVE, radix, so_file
if (GDL_IDL_FL(/upper) EQ 'IDL') then FILE_MOVE, idl_so_file, so_file
;
result=CALL_EXTERNAL(bindir+PATH_SEP()+so_file, 'add', 5, 6)
;
if (result NE 11) then errors=1 else errors=0

BANNER_FOR_TESTSUITE, 'TEST_MAKE_DLL', errors
;
if (errors GT 0) AND ~KEYWORD_SET(no_exit) then EXIT, status=1
;
if KEYWORD_SET(test) then STOP
;
end
