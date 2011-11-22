; part of GDL - GNU Data Language (GPL)
; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>
function assert, str, xpr, bool, fold_case=fold_case
  if ~keyword_set(fold_case) then fold_case=0
  if strmatch(str, xpr, fold_case=fold_case) ne bool then begin
    message, 'strmatch(''' + str + ''', ''' + xpr + ''', fold=' + strtrim(fold_case,2) + ') ne ' + strtrim(bool,2)
    return, 1
  endif
  return, 0
end

pro test_strmatch
  err = 0
  err += assert('gdl', 'gdl', 1)
  err += assert('gpl', 'gdl', 0)
  err += assert('gdl', 'gd', 0)
  err += assert('GDL', 'gdl', 0)
  err += assert('GDL', 'gdl', 1, /fold)
  err += assert('GDL', 'gd*', 0)
  err += assert('GDL', 'gd*', 1, /fold)
  err += assert('GDL', '*dl', 0)
  err += assert('GDL', '*dl', 1, /fold)
  err += assert('GDL', 'g?l', 0)
  err += assert('GDL', 'g?l', 1, /fold)
  err += assert('GDL', 'g?', 0)
  err += assert('GDL', 'g?', 0, /fold)
  err += assert('GDLGDL', 'GD?G*L', 1)
  err += assert('GDLGDL', 'G?G*L', 0)
  err += assert('GDLGDL', '*DL*', 1)
  err += assert('GDLGDL', '\*DL\*', 0)
  err += assert('A*DL*A', 'A\*DL\*A', 1)
  err += assert('A?DL?A', 'A\?DL\?A', 1)
  err += assert('*DL*', '\*DL\*', 1)
  err += assert('?DL?', '\?DL\?', 1)
  err += assert('GDLGDL', '?DL?', 0)
  err += assert('GDLGDL', 'G[ABCD]LGDL', 1)
  err += assert('GDLGDL', 'G[!ABCD]LGDL', 0)
  err += assert('GDLGDL', 'G[A-D]LGDL', 1)
  err += ~array_equal(strmatch(['gdl', 'GDL'], 'gdl'), [1,0])
  if err ne 0 then exit, status=1
end
