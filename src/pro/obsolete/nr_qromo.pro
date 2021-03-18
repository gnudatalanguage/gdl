FUNCTION NR_QROMO,  Func, A, B, _EXTRA=extra
  if (n_elements(b) eq 0) then RETURN, QROMO( Func, A, _EXTRA=extra ) else RETURN, QROMO( Func, A, B, _EXTRA=extra ) 
end
