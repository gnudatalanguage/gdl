; part of GDL - GNU Data Language (GPL)
; by Sylwester Arabas <slayoo@igf.fuw.edu.pl>

; search & replace helper for strmatch()
function strmatch_strreplace, str, a, b
  pos = strpos(str, a)
  if pos eq -1 then return, str
  ret = strmid(str,0,pos)
  la = strlen(a)
  last = pos 
  for i = pos, strlen(str) - 1 do begin
    if strmid(str,i,la) eq a && (i eq 0 || strmid(str,i-1,1) ne '\') then begin
      ret += strmid(str,last,i-last) + b 
      last = i + la
    endif
  endfor
  ret += strmid(str,last)
  return, ret
end

; quick & dirty strmatch() implementation using stregex()
function strmatch, mstr, sstr, fold_case=fold_case
  on_error, 2
  if (size(sstr))[0] ne 0 then message, 'second argument must be a scalar string'

  tmp = sstr
  ; . -> \. (but not \.)
  tmp = strmatch_strreplace(tmp, '.', '\.')
  ; ( -> \( (but not \()
  tmp = strmatch_strreplace(tmp, '(', '\(')
  ; ) -> \) (but not \))
  tmp = strmatch_strreplace(tmp, ')', '\)')
  ; + -> \+ (but not \+)
  tmp = strmatch_strreplace(tmp, '+', '\+')
  ; { -> \{ (but not \{)
  tmp = strmatch_strreplace(tmp, '{', '\{')
  ; } -> \} (but not \})
  tmp = strmatch_strreplace(tmp, '}', '\}')
  ; | -> \| (but not \|)
  tmp = strmatch_strreplace(tmp, '|', '\|')
  ; ^ -> \^ (but not \^)
  tmp = strmatch_strreplace(tmp, '^', '\^')
  ; $ -> \$ (but not \$)
  tmp = strmatch_strreplace(tmp, '$', '\$')
  ; * -> .* (but not \*)
  tmp = strmatch_strreplace(tmp, '*', '.*')
  ; ? -> . (but not \?)
  tmp = strmatch_strreplace(tmp, '?', '.')
  ; [!...] -> [^...] (but not \[!...)
  tmp = strmatch_strreplace(tmp, '[!', '[^')
  ; the leading a trailing markers
  tmp = '^' + tmp + '$'
  return, stregex(mstr, tmp, /boolean, fold_case=fold_case)
end
