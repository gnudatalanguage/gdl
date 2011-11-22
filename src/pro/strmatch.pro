; part of GDL - GNU Data Language (GPL)
; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>

; search & replace helper for strmatch()
function strmatch_strreplace, str, a, b
  tmp = strsplit(str, '\\\' + a, /extract, /regex)
  for i = 0, n_elements(tmp) - 1 do tmp[i] = strjoin(strsplit(tmp[i], '\' + a, /extract, /regex), b)
  tmp = $
    (strmid(str,0,2) eq ('\' + a) ? ('\' + a) : '') + $
    (strmid(str,0,1) eq a ? b : '') + $
    strjoin(tmp, '\' + a) + $
    (strmid(str,0,1,/rev) eq a ? b : '') + $
    (strmid(str,0,2,/rev) eq ('\' + a) ? ('\' + a) : '')
  return, tmp
end

; strmatch() implementation using stregex()
function strmatch, mstr, sstr, fold_case=fold_case
  on_error, 2
  if (size(sstr))[0] ne 0 then message, 'second argument must be a scalar string'

  ; * -> .* (but not \*)
  tmp = strmatch_strreplace(sstr, '*', '.*')
  ; ? -> . (but not \?)
  tmp = strmatch_strreplace(tmp, '?', '.')
  ; [!...] -> [^...] (but not \[!...)
  tmp = strmatch_strreplace(tmp, '[!', '[^')
  tmp = '^' + tmp + '$'

  return, stregex(mstr, tmp, /boolean, fold_case=fold_case)
end
