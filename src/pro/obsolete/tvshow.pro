pro tvshow,i,j, _EXTRA=extra
  case (n_params()) of
  0: WSHOW, _extra=extra
  1: WSHOW, i, _extra=extra
  2: WSHOW, i, j, _extra=extra
endcase
end  
