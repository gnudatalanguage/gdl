pro test_delvarrnew

  spawn,'../src/gdl -quiet ./delvarrnew.pro', result & nres=n_elements(result)
  if nres lt 2 then exit, status=1
;  print,result[nres-1] 'SUCCESS'
  if(result[nres-1] eq 'FAILURE') then exit, status=1
;  for k=0,nres-1 do print,result[k]
end
