FUNCTION rstrpos,Expression, Search_String, Pos, _extra=extra
  if (n_elements(Pos) eq 0 ) then RETURN, strpos(Expression, Search_String,  /REVERSE_SEARCH, _extra=extra ) else RETURN, strpos(Expression, Search_String, Pos,  /REVERSE_SEARCH, _extra=extra ) 
end
