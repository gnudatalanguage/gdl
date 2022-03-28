pro TIFF_WRITE ,X,Y,_extra=extra
  if (n_elements(y) eq 0) then write_tiff, X,_extra=extra else write_tiff, X, Y,_extra=extra
end  
