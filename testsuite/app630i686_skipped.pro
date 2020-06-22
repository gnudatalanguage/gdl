; if ($env:platform -Match "mingw64630i686")
;      # routines common with mingw64630x8664
;  test_memory
;  test_wordexp
;  # do not fail on a live system
 test_call_procedure
  test_extra_keywords
 test_la_least_squares
  test_math_function_dim
  test_matrix_multiply
  test_readf_with_crlf
  test_routine_filepath
  test_scope_varfetch
;#extras (# do not fail on a live system)
  test_angles
  test_base64
  test_binfmt
  test_bytscl
  test_byte_conversion
  test_common
  test_congrid
  test_dilate
  test_erfinv
  test_execute 
   test_fft_dim
  test_file_copy
  test_file_move
  test_finite
  test_fixprint
  test_fx_root
  test_fz_roots
  test_gh00716
   test_hist_2d
  test_interpol
  test_l64
  test_linfit
  test_message
  test_modulo
  test_moment
  test_nans_in_sort_and_median
  test_netcdf
  test_obj_valid
  test_parse_url
  test_poly_fit
  test_product
  test_ptrarr
  test_ptr_valid
  test_random
  test_same_name
  test_simplex
  test_spl_init
  test_str_sep
  test_stregex
  test_strmatch
  test_strsplit
  test_tag_names
  test_tic_toc
  test_trisol
  test_typename
  test_wavelet
;  test_xmlsax
;% Compiled module: OB_XML_TO_STRUCT__DEFINE.
;% IDLFFXMLSAX::PARSEFILE: file obxml.xml does not exist.
;% Execution halted at: TEST_XMLSAX          8 D:/gdlgit/gdl-master/testsuite/test_xmlsax.pro
  test_zeropoly
  test_xdr
