pro test_delvarrnew

;<<== insert delvarrnew string
; now that the dependent files are written to the current directory,
; execute them
  spawn,'../src/gdl -quiet ./delvarrnew.pro', result & nres=n_elements(result)
  if nres lt 2 then exit, status=1

  if(result[nres-1] eq 'FAILURE') then exit, status=1
  banner_for_testsuite,' DELVAR-RNEW',' works! (from $main)'

end
