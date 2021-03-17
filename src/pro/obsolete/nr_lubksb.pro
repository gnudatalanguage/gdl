FUNCTION NR_LUBKSB, A, Index, B, _EXTRA=extra
  RETURN, LUSOL( A, Index, B, /COLUMN, _EXTRA=extra ) 
end
