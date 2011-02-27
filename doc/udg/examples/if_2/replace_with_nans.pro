pro replace_with_nans, x, val
  whr = where(x eq val, cnt)
  if cnt gt 0 then begin
    x[whr] = !VALUES.F_NAN
    message, 'nan count: ' + strtrim(cnt, 2), /conti
  endif 
end
