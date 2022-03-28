FUNCTION NR_SVBKSB, u, w, v, b, _EXTRA=extra
 RETURN, SVSOL(u, w, v, b, /COLUMN, _EXTRA=extra )
end
