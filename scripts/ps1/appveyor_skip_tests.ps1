  # powershell script called from appveyor.yml 
  # normally $env:platform is blanked out at top of appveyor.yml

   echo "Skipped tests for $env:platform  are found in scripts/appveyor_skip_tests.ps1"
    cd C:\projects\gdl\testsuite
       if ( $env:platform -Match "mingw64630x8664") 
      {
 #    memory, wordexp fail
#% TEST_MEMORY: reported memory consumption should increase after allocating a big array!
    Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_memory.pro' -NotMatch)
# The next category are failures that fail on Appveyor, but are auccessful with the downloaded artifact
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_byte_conversion.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_call_procedure.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_extra_keywords.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_la_least_squares.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_math_function_dim.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_matrix_multiply.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_readf_with_crlf.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_routine_filepath.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_scope_varfetch.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_xdr.pro' -NotMatch)
      }
      elseif ($env:platform -Match "mingw64630i686")
      {
      # routines common with mingw64630x8664
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_memory.pro' -NotMatch)
  # do not fail on a live system
 Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_call_procedure.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_extra_keywords.pro' -NotMatch)
 Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_la_least_squares.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_math_function_dim.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_matrix_multiply.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_readf_with_crlf.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_routine_filepath.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_scope_varfetch.pro' -NotMatch)
#extras
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_angles.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_base64.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_binfmt.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_bytscl.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_byte_conversion.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_common.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_congrid.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_dilate.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_erfinv.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_execute.pro' -NotMatch) 
   Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_fft_dim.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_file_copy.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_file_move.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_finite.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_fixprint.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_fx_root.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_fz_roots.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_gh00716.pro' -NotMatch)
   Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_hist_2d.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_interpol.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_l64.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_linfit.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_message.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_modulo.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_moment.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_nans_in_sort_and_median.pro' -NotMatch)
  #Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_netcdf.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_obj_valid.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_parse_url.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_poly_fit.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_product.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_ptrarr.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_ptr_valid.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_random.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_same_name.pro' -NotMatch)
   # Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_simplex.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_spl_init.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_str_sep.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_stregex.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_strmatch.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_strsplit.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_tag_names.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_tic_toc.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_trisol.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_typename.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_wavelet.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_xdr.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_xmlsax.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_zeropoly.pro' -NotMatch)
  # The following should be skipped (exit, status=77) but do not skip for i686
   Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_netcdf.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_python.pro' -NotMatch)
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_simplex.pro' -NotMatch)
     }
      elseif ($env:platform -Match "mingw64810i686")
      {
   Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_angles.pro' -NotMatch)
   echo $env:platform has been attempted before and has not worked out
      }
      elseif ($env:platform -Match "mingw64810x8664")
      {
  Set-Content -Path "LIST" -Value (get-content -Path "LIST" | Select-String -Pattern 'test_angles.pro' -NotMatch)
   echo $env:platform has been attempted before and has not worked out
    }
 
