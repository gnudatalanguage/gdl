; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
; part of GDL - GNU Data Language
function scope_varfetch, name, level=level ; TODO: COMMON, ENTER, REF_EXTRA
  return, routine_names(name, fetch=level-1)
end
