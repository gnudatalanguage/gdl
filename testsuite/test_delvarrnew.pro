pro test_delvarrnew

  spawn,'../src/gdl -quiet ./delvarrnew.pro', result & nres=n_elements(result)
  if nres lt 2 then exit, status=1

  if(result[nres-1] eq 'FAILURE') then exit, status=1
  banner_for_testsuite,' DELVAR-RNEW',' works! (from $main)'

end
